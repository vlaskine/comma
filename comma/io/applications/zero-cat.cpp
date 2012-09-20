#include <zmq.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/array.hpp>
#include <comma/io/publisher.h>
#include <comma/application/signal_flag.h>

int main(int argc, char* argv[])
{
    try
    {
    unsigned int size;
    std::size_t hwm;
    std::string server;
    boost::program_options::options_description description( "options" );
    description.add_options()
        ( "help,h", "display help message" )
        ( "publish", "use bind and publish instead of connect and subscribe" )
        ( "connect", "use connect instead of bind" )
        ( "bind", "use bind instead of connect" )
        ( "endl", "output end of line after each packet" )
        ( "size,s", boost::program_options::value< unsigned int >( &size )->default_value( 1024 ), "packet size in bytes, in publish mode" )
        ( "buffer,b", boost::program_options::value< std::size_t >( &hwm )->default_value( 1024 ), "set buffer size in packets ( high water mark in zmq )" )
        ( "server", boost::program_options::value< std::string >( &server ), "in subscribe mode, republish the data on a socket, eg tcp:1234" );

    boost::program_options::variables_map vm;
    boost::program_options::store( boost::program_options::parse_command_line( argc, argv, description), vm );
    boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options( description ).allow_unregistered().run();
    boost::program_options::notify( vm );

    if ( vm.count("help") )
    {
        std::cerr << "forward stdin to zeromq publisher or subscribe from zeromq to stdout" << std::endl;
        std::cerr << "usage: zero-cat <options> [endpoints]" << std::endl;
        std::cerr << "publisher example: yes hello | zero-cat --publish --size 6 ipc:///tmp/socket tcp://*:5555 -" << std::endl;
        std::cerr << "subscriber example: zero-cat ipc:///tmp/socket tcp://*:5555 " << std::endl;
        std::cerr << description << "\n";
        return 1;
    }

    bool endl = ( vm.count("endl") );

    std::vector< std::string > endpoints = boost::program_options::collect_unrecognized( parsed.options, boost::program_options::include_positional );
    if( endpoints.empty() )
    {
        std::cerr << "please provide at least one endpoint" << std::endl;
    }

    comma::signal_flag shutdown_flag;
    
    zmq::context_t context( 1 );
    int mode = ZMQ_SUB;
    if ( vm.count("publish") )
    {
        mode = ZMQ_PUB;
    }
    zmq::socket_t socket( context, mode );
    socket.setsockopt( ZMQ_HWM, &hwm, sizeof( hwm ) );
    if ( vm.count("publish") )
    {
        bool stdout = false;
        for( unsigned int i = 0; i < endpoints.size(); i++ )
        {
            if( endpoints[i] == "-" )
            {
                std::cerr << " stdout " << std::endl;
                stdout = true;
            }
            else if( vm.count("connect") )
            {
                socket.connect( endpoints[i].c_str() );
            }
            else
            {
                socket.bind( endpoints[i].c_str() );
            }
        }
        std::string buffer;
        buffer.resize( size );
        
        while( !shutdown_flag.is_set() && std::cin.good() && !std::cin.eof() && !std::cin.bad() )
        {
            std::cin.read( &buffer[0], buffer.size() );
            unsigned int read = std::cin.gcount();
            if( read == size )
            {
                zmq::message_t message( buffer.size() );
                ::memcpy( (void *) message.data (), &buffer[0], buffer.size() );
                socket.send( message );
                if( stdout )
                {
                    std::cout.write( &buffer[0], buffer.size() );
                    std::cout.flush();
                }
            }
        }
    }
    else
    {
        for( unsigned int i = 0; i < endpoints.size(); i++ )
        {
            if( vm.count("bind") )
            {
                socket.bind( endpoints[i].c_str() );
            }
            else
            {
                socket.connect( endpoints[i].c_str() );
            }
        }
        socket.setsockopt( ZMQ_SUBSCRIBE, "", 0 );
        if( vm.count( "server" ) == 0 )
        {
            while( std::cout.good() && !std::cout.eof() && !std::cout.bad() )
            {
                zmq::message_t message;
                socket.recv( &message );
                std::cout.write( ( const char* )message.data(), message.size() );
                if( endl )
                {
                    std::cout << std::endl;
                }
                else
                {
                    std::cout.flush();
                }
            }
        }
        else
        {
            comma::io::publisher publisher( server, comma::io::mode::binary, true, false );
            while( !shutdown_flag.is_set() )
            {
                zmq::message_t message;
                socket.recv( &message );
                publisher.write( ( const char* )message.data(), message.size() );
                if( endl )
                {
                    publisher << '\n'/* std::endl*/;
                }
            }
        }
    }
    
    }
    catch ( zmq::error_t& e )
    {
        std::cerr << argv[0] << " : zeromq error: " << e.what() << std::endl;
    }
    catch ( std::exception& e )
    {
        std::cerr << argv[0] << " : Exception: " << e.what() << std::endl;
    }

    return 0;
}

