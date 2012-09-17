#include <deque>
#include <queue>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/types.h>
#include <comma/csv/Stream.h>
#include <comma/visiting/traits.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "mark data blocks by putting size value in size/rsize (reverse size) columns" << std::endl;
    std::cerr << std::endl;
    std::cerr << "policies (also see examples)" << std::endl;
    std::cerr << "    by block: if 'block' field present, set size/rsize of an entry by its position in the block" << std::endl;
    std::cerr << "    by size: if --size=<value> option present, chop into blocks of size <value>" << std::endl;
    std::cerr << "    by life span: if --life=<seconds> and 't' column option present, mark blocks" << std::endl;
    std::cerr << "                  according to the sliding window of the last <seconds>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat something.csv csv-make-blocks \"something_else.csv[,options]\" [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "fields" << std::endl;
    std::cerr << "    block: block number" << std::endl;
    std::cerr << "    t: timestamp" << std::endl;
    std::cerr << "    size: column to output remaining block size for a given block" << std::endl;
    std::cerr << "    rsize: reverse size, column to output already accumulated block size for a given block" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    note: if needed, add size and/or rsize column, using csv-stitch" << std::endl;
    std::cerr << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << "    --verbose,-v: more output to stderr" << std::endl;
    std::cerr << "    --life=<seconds>: mark blocks by sliding window of the last <seconds>" << std::endl;
    std::cerr << "    --size=<value>: chop into blocks of size <value>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "csv options" << std::endl;
    std::cerr << comma::csv::options::usage() << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples" << std::endl;
    std::cerr << "    echo 0,0,0 > test.csv" << std::endl;
    std::cerr << "    echo 0,0,0 >> test.csv" << std::endl;
    std::cerr << "    echo 1,0,0 >> test.csv" << std::endl;
    std::cerr << "    echo 1,0,0 >> test.csv" << std::endl;
    std::cerr << "    echo 1,0,0 >> test.csv" << std::endl;
    std::cerr << "    cat test.csv | csv-make-blocks --fields=block,size,rsize" << std::endl;
    std::cerr << "    0,2,1" << std::endl;
    std::cerr << "    0,1,2" << std::endl;
    std::cerr << "    1,3,1" << std::endl;
    std::cerr << "    1,2,2" << std::endl;
    std::cerr << "    1,1,3" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    todo: more examples" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

struct Input
{
    boost::posix_time::ptime timestamp;
    comma::uint32 block;
    comma::uint32 size;
    comma::uint32 rsize;
    
    Input(): block( 0 ), size( 0 ), rsize( 0 ) {}
};

namespace comma { namespace visiting {

template <> struct traits< Input >
{
    template < typename K, typename V > static void visit( const K&, const Input& p, V& v )
    { 
        v.apply( "t", p.timestamp );
        v.apply( "block", p.block );
        v.apply( "size", p.size );
        v.apply( "rsize", p.rsize );
    }
    
    template < typename K, typename V > static void visit( const K&, Input& p, V& v )
    { 
        v.apply( "t", p.timestamp );
        v.apply( "block", p.block );
        v.apply( "size", p.size );
        v.apply( "rsize", p.rsize );
    }
};

} } // namespace comma { namespace visiting {

static bool verbose;

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h" ) ) { usage(); }
        verbose = options.exists( "--verbose,-v" );
        comma::csv::options csv( options );
        options.assertMutuallyExclusive( "--life,--size" );
        boost::optional< boost::posix_time::time_duration > life;
        boost::optional< std::size_t > chunk = options.optional< std::size_t >( "--size" );
        if( options.exists( "--life" ) ) { life = boost::posix_time::microseconds( static_cast< int >( options.value< double >( "--life" ) * 1000000 ) ); }
        if( life && !csv.has_field( "t" ) ) { std::cerr << "csv-make-blocks: for --life, please specify 't' field" << std::endl; return 1; }
        if( life && csv.has_field( "size" ) ) { std::cerr << "csv-make-blocks: for --life, only rsize implemented; forward size: todo" << std::endl; return 1; }
        if( !csv.has_field( "size" ) && !csv.has_field( "rsize" ) ) { std::cerr << "csv-make-blocks: please specify field 'size' and/or 'rsize'" << std::endl; return 1; }
        bool has_size = csv.has_field( "size" );
        comma::csv::input_stream< Input > istream( std::cin, csv );
        comma::csv::output_stream< Input > ostream( std::cout, csv );
        Input q;
        typedef std::deque< std::pair< Input, std::string > > Points;
        Points points;
        std::size_t block = 0;
        comma::signal_flag is_shutdown;
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            const Input* p = istream.read();
            // todo: if !chunk && !life && !p, output the last block
            if( !p ) { break; }
            q = *p;
            if( life )
            {
                static std::queue< boost::posix_time::ptime > timestamps;
                if( p->timestamp == boost::posix_time::not_a_date_time ) { std::cerr << "csv-make-blocks: expected timestamp, got not-a-date-time" << std::endl; return 1; }
                if( !timestamps.empty() && p->timestamp - timestamps.front() >= life ) { ++block; }
                for( ; !timestamps.empty() && p->timestamp - timestamps.front() >= life; timestamps.pop() );
                timestamps.push( p->timestamp );
                q.block = block;
                q.rsize = timestamps.size();
            }
            else if( chunk )
            {
                static std::size_t size = 0;
                q.rsize = *chunk - size;
                ++size;
                q.size = size;
                q.block = block;
                if( size == *chunk ) { size = 0; ++block; }
            }
            else
            {
                static std::string buffer;
                static std::size_t rsize = 0;
                if( q.block == block )
                {
                    ++rsize;
                    q.rsize = rsize;
                    if( has_size )
                    {
                        if( csv.binary() ) { buffer.resize( csv.format().size() ); ::memcpy( &buffer[0], istream.binary().last(), csv.format().size() ); }
                        else { buffer = comma::join( istream.ascii().last(), csv.delimiter ); }
                        points.push_back( std::make_pair( q, buffer ) );
                    }
                }
                else
                {
                    if( has_size ) // quick and dirty
                    {
                        std::size_t size = points.size();
                        for( Points::iterator it = points.begin(); it != points.end(); ++it, --size )
                        {
                            it->first.size = size;
                            if( csv.binary() ) { ostream.binary().write( it->first, &it->second[0] ); }
                            else { ostream.ascii().write( it->first, it->second ); }
                        }
                        points.clear();
                    }
                    rsize = 1;
                    q.rsize = 1;
                    block = q.block;
                    if( has_size ) // quick and dirty, use boost::optional instead
                    {
                        if( csv.binary() ) { buffer.resize( csv.format().size() ); ::memcpy( &buffer[0], istream.binary().last(), csv.format().size() ); }
                        else { buffer = comma::join( istream.ascii().last(), csv.delimiter ); }
                        points.push_back( std::make_pair( q, buffer ) );
                    }
                }
            }
            if( !life && !chunk && has_size ) { continue; } // quick and dirty
            if( csv.binary() ) { ostream.binary().write( q, istream.binary().last() ); }
            else { ostream.ascii().write( q, istream.ascii().last() ); }
        }
        std::size_t size = points.size();
        for( Points::iterator it = points.begin(); it != points.end(); ++it, --size )
        {
            it->first.size = size;
            if( csv.binary() ) { ostream.binary().write( it->first, &it->second[0] ); }
            else { ostream.ascii().write( it->first, it->second ); }
        }
        return 0;
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-make-blocks: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-make-blocks: unknown exception" << std::endl;
    }
    return 1;
}
