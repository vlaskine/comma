#include <zmq.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/array.hpp>

int main(int argc, char* argv[])
{
    try
    {
    unsigned int size;
    std::size_t hwm;
    boost::program_options::options_description description( "options" );
    description.add_options()
        ( "help,h", "display help message" )
        ( "publish", "use bind and publish instead of connect and subscribe" )
        ( "connect", "use connect instead of bind" )
        ( "bind", "use bind instead of connect" )
        ( "endl", "output end of line after each packet" )
        ( "latest", "in subscribe mode: flush the queue and get the latest packet" )
        ( "size,s", boost::program_options::value< unsigned int >( &size )->default_value( 1024 ), "packet size in bytes, in publish mode" )
        ( "buffer,b", boost::program_options::value< std::size_t >( &hwm )->default_value( 1024 ), "set buffer size in packets ( high water mark in zmq )" );

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
        while( std::cin.good() && !std::cin.eof() && !std::cin.bad() )
        {
            std::cin.read( &buffer[0], buffer.size() );
            unsigned int read = std::cin.gcount();
            if( read > 0 )
            {
                if( read < size )
                {
                    std::cerr << " warning, could not read a full packet " << std::endl; // TODO reassembly ?
                }
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
        while( 1 )
        {
            if( vm.count("latest") )
            {
                boost::array< zmq::message_t, 2 > message; // double buffer
                unsigned int index = 0;
                // read until queue empty
                bool read = true;
                bool write = false;
                while( read )
                {
                    read = socket.recv( &message[index], ZMQ_NOBLOCK );
                    if( read && message[index].size() != 0 )
                    {
                        write = true;
                    }
                    index++;
                    index %= 2;
                    ::usleep( 1000 );
                }
                if( write )
                {
                    std::cout.write( ( const char* )message[index].data(), message[index].size() );
                }
            }
            else
            {
                zmq::message_t message;
                socket.recv( &message );
                std::cout.write( ( const char* )message.data(), message.size() );
            }
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

    }
    catch ( zmq::error_t& e )
    {
        std::cerr << argv[0] << " : zeromq error: " << e.what() << std::endl;
    }
    catch ( std::exception& e )
    {
        std::cerr << argv[0] << " : exception: " << e.what() << std::endl;
    }

    return 0;
}

