// This file is part of comma, a generic and flexible library 
// for robotics research.
//
// Copyright (C) 2011 The University of Sydney
//
// comma is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// comma is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License 
// for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with comma. If not, see <http://www.gnu.org/licenses/>.

#ifndef COMMA_CSV_MULTIPLAY_H
#define COMMA_CSV_MULTIPLAY_H

#include <vector>
#include <boost/thread/thread_time.hpp>
#include <comma/csv/options.h>
#include <comma/csv/stream.h>
#include <comma/io/publisher.h>
#include "./play."

namespace comma {

/// gets data from multiple input files, and output in a real time manner to output files,  using timestamps
class Multiplay
{
    public:
        struct time
        {
            time() {}
            time( boost::posix_time::ptime t ) : time( t ) {}
            boost::posix_time::ptime time;
        };

        struct SourceConfig
        {
            std::string outputFileName;
            std::size_t minNumberOfClients;
            csv::options options;
            boost::posix_time::time_duration offset;
            SourceConfig( const std::string& output, const csv::options& csv ) :
                outputFileName( output ), minNumberOfClients( 0 ), options( csv ) {}
            SourceConfig( const std::string& output, std::size_t n, const csv::options& csv ) :
                outputFileName( output ), minNumberOfClients( n ), options( csv ) {}
            SourceConfig() {};
        };

        Multiplay( const std::vector< SourceConfig >& configs
                , double speed = 1.0
                , bool quiet = false
                , const boost::posix_time::time_duration& precision = boost::posix_time::milliseconds( 1 )
                , boost::posix_time::ptime from = boost::posix_time::not_a_date_time
                , boost::posix_time::ptime to = boost::posix_time::not_a_date_time
                , bool flush = true
                 );

        void close();

        bool read();

    private:
        std::vector<SourceConfig> m_configs;
        std::vector< boost::shared_ptr< comma::Io::IStream > > is_treams;
        std::vector< boost::shared_ptr< csv::input_stream< time > > > m_inputStreams;
        std::vector< boost::shared_ptr< comma::Io::Publisher > > m_publishers;
        Timing::Play m_play;
        std::vector< boost::posix_time::ptime > m_timestamps;
        bool m_started;
        boost::posix_time::ptime m_from;
        boost::posix_time::ptime m_to;
        std::vector< boost::shared_ptr< csv::ascii< time > > > ascii_;
        std::vector< boost::shared_ptr< csv::binary< time > > > binary_;
        std::vector< char > buf_fer;
        bool ready();
};

} // namespace comma {

namespace comma { namespace visiting {

template <> struct traits< comma::Multiplay::time >
{
    template < typename Key, class Visitor >
    static void visit( Key, comma::Multiplay::time& t, Visitor& v )
    {
        v.apply( "t", t.time );
    }

    template < typename Key, class Visitor >
    static void visit( Key, const comma::Multiplay::time& t, Visitor& v )
    {
        v.apply( "t", t.time );
    }
};

template <> struct traits< comma::Multiplay::SourceConfig >
{
    template < typename Key, class Visitor >
    static void visit( Key, comma::Multiplay::SourceConfig& c, Visitor& v )
    {
        v.apply( "options", c.options );
        v.apply( "output", c.outputFileName );
        v.apply( "clients", c.minNumberOfClients );
        double duration = 0;
        v.apply( "offset", duration );
		c.offset = boost::posix_time::microseconds( static_cast< boost::int64_t >( duration * 1e6 ) );
    }

    template < typename Key, class Visitor >
    static void visit( Key, const comma::Multiplay::SourceConfig& c, Visitor& v )
    {
        v.apply( "options", c.options );
        v.apply( "output", c.outputFileName );
        v.apply( "clients", c.minNumberOfClients );
        double duration = c.offset.total_microseconds();
        duration /= 1e6;
        v.apply( "offset", duration );
    }
};

} } // namespace comma { namespace visiting {

#endif // COMMA_CSV_MULTIPLAY_H
