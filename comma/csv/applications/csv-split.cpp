#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <comma/csv/ProgramOptions.h>
#include <comma/csv/Applications/Split/Split.h>

int main( int argc, char** argv )
{
    
    try
    {
        double period = 0;
        unsigned int size = 0;
        std::string extension;
        boost::program_options::options_description description( "options" );
        description.add_options()
            ( "help,h", "display help message" )
            ( "size,c", boost::program_options::value< unsigned int >( &size ), "packet size, only full packets will be written" )
            ( "period,t", boost::program_options::value< double >( &period ), "period in seconds after which a new file is created" )
            ( "suffix,s", boost::program_options::value< std::string >( &extension ), "filename extension; default will be csv or bin, depending whether it is ascii or binary" );
        description.add( comma::csv::ProgramOptions::description() );
        boost::program_options::variables_map vm;
        boost::program_options::store( boost::program_options::parse_command_line( argc, argv, description), vm );
        boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options( description ).allow_unregistered().run();
        boost::program_options::notify( vm );
        if ( vm.count( "help" ) || vm.count( "long-help" ) )
        {
            std::cerr << std::endl;
            std::cerr << "read from stdin by packet or by line and split the data into files, named by time" << std::endl;
            std::cerr << "usage: csv-split [options]" << std::endl;
            std::cerr << std::endl;
            std::cerr << description << std::endl;
            std::cerr << std::endl;
            std::cerr << "fields" << std::endl;
            std::cerr << "    t: if present, use timestamp from the packet; if absent, use system time" << std::endl;
            std::cerr << "    block: split on the block number change" << std::endl;
            std::cerr << "    id: split by id (same as block, except does not have to be contiguous by the price of worse performance)" << std::endl;
            std::cerr << std::endl;
            return 1;
        }
        comma::csv::Options csv = comma::csv::ProgramOptions::get( vm );
        if( csv.binary() ) { size = csv.format().size(); }
        boost::optional< boost::posix_time::time_duration > duration;
        if( period > 0 ) { duration = boost::posix_time::microseconds( period * 1e6 ); }
        std::string suffix;
        if( extension.empty() ) { suffix = csv.binary() || size > 0 ? ".bin" : ".csv"; }
        else { suffix += "."; suffix += extension; }
        comma::csv::Applications::Split split( duration, suffix, csv );
        if( size == 0 )
        {
            std::string line;
            while( std::cin.good() && !std::cin.eof() )
            {
                std::getline( std::cin, line );
                if( line.empty() ) { break; }
                split.write( line );
            }
        }
        else
        {
            std::vector< char > packet( size );
            while( std::cin.good() && !std::cin.eof() )
            {
                std::cin.read( &packet[0], size );
                if( std::cin.gcount() > 0 ) { split.write( &packet[0], size ); }
            }
        }
        return 0;
    }
    catch( std::exception& ex )
    {
        std::cerr << argv[0] << ": " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << argv[0] << ": unknown exception" << std::endl;
    }
    return 1;
}
