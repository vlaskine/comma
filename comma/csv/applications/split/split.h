#ifndef COMMA_CSV_SPLIT_H
#define COMMA_CSV_SPLIT_H

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <comma/Base/Types.h>
#include <comma/csv/ascii.h>
#include <comma/csv/binary.h>
#include <comma/visiting/traits.h>

namespace comma { namespace csv { namespace Applications {

struct Input
{
    boost::posix_time::ptime timestamp;
    unsigned int block;
    unsigned int id;
}; 

} } } // namespace comma { namespace csv { namespace Applications {

namespace comma { namespace visiting {

template <> struct traits< comma::csv::Applications::Input >
{
    template < typename K, typename V > static void visit( const K&, const comma::csv::Applications::Input& p, V& v )
    { 
        v.apply( "t", p.timestamp );
        v.apply( "block", p.block );
        v.apply( "id", p.id );
    }
    
    template < typename K, typename V > static void visit( const K&, comma::csv::Applications::Input& p, V& v )
    { 
        v.apply( "t", p.timestamp );
        v.apply( "block", p.block );
        v.apply( "id", p.id );
    }
};

} } // namespace comma { namespace visiting {    

namespace comma { namespace csv { namespace Applications {
    
/// split data to files by time
/// files are named by timestamp, cut down to seconds
class Split
{
    public:
        Split( boost::optional< boost::posix_time::time_duration > period
             , const std::string& suffix
             , const comma::csv::Options& csv );
        void write( const char* data, unsigned int size );
        void write( const std::string& line );

    private:
        std::ofstream& ofstream_by_time_();
        std::ofstream& ofstream_by_block_();
        std::ofstream& ofstream_by_id_();
        void update_( const char* data, unsigned int size );
        void update_( const std::string& line );
        
        boost::function< std::ofstream&() > ofstream_;
        boost::scoped_ptr< comma::csv::ascii< Input > > ascii_;
        boost::scoped_ptr< comma::csv::binary< Input > > binary_;
        boost::optional< boost::posix_time::time_duration > period_;
        std::string suffix_;
        Input current_;
        boost::optional< Input > last_;
        std::ios_base::openmode mode_;
        std::ofstream file_;
        typedef boost::unordered_map< comma::uint32, boost::shared_ptr< std::ofstream > > Files;
        typedef boost::unordered_set< comma::uint32 > Ids;
        Files files_;
        Ids seen_ids_;
};

} } } // namespace comma { namespace csv { namespace Applications {
    
#endif // COMMA_CSV_SPLIT_H
