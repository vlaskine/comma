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

#include <sstream>
#include <boost/thread/thread.hpp>
#include <comma/String/String.h>
#include "./Multiplay.h"

namespace comma {


/*!
    @brief Constructor
    @param configs csv options
*/
Multiplay::Multiplay( const std::vector< SourceConfig >& configs
                    , double speed
                    , bool quiet
                    , const boost::posix_time::time_duration& precision
                    , boost::posix_time::ptime from
                    , boost::posix_time::ptime to
                    , bool flush )
    : m_configs( configs )
    , is_treams( configs.size() )
    , m_inputStreams( configs.size() )
    , m_publishers( configs.size() )
    , m_play( speed, quiet, precision )
    , m_timestamps( configs.size() )
    , m_started( false )
    , m_from( from )
    , m_to( to )
    , ascii_( configs.size() )
    , binary_( configs.size() )
{
    for( unsigned int i = 0; i < configs.size(); i++ )
    {
        // todo: quick and dirty for now: blocking streams for named pipes
        is_treams[i].reset( new Io::IStream( configs[i].options.filename, m_configs[i].options.binary() ? Io::Mode::binary : Io::Mode::ascii, Io::Mode::blocking ) );
        if( !( *is_treams[i] )() ) { COMMA_THROW( comma::exception, "named pipe " << configs[i].options.filename << " is closed (todo: support closed named pipes)" ); }
        m_inputStreams[i].reset( new csv::input_stream< time >( *( *is_treams[i] )(), m_configs[i].options ) );
        unsigned int j;
        for( j = 0; j < i && configs[j].outputFileName != configs[i].outputFileName; ++j ); // quick and dirty: unique publishers
        if( j == i ) { m_publishers[i].reset( new Io::Publisher( configs[i].outputFileName, m_configs[i].options.binary() ? Io::Mode::binary : Io::Mode::ascii, true, flush ) ); }
        else { m_publishers[i] = m_publishers[j]; }
        boost::posix_time::time_duration d;
        if( configs[i].offset.total_microseconds() != 0 )
        {
            if( m_configs[i].options.binary() )
            {
                binary_[i].reset( new csv::binary< time >( m_configs[i].options.fields ) );
                buf_fer.resize( m_configs[i].options.format().size() );
            }
            else
            {
                ascii_[i].reset( new csv::ascii< time >( m_configs[i].options.fields ) );
            }
        }
    }
}

void Multiplay::close()
{
    for( unsigned int i = 0U; i < m_configs.size(); i++ )
    {
        is_treams[i]->close();
        m_publishers[i]->close();
    }
}

namespace impl {
    
static std::string endl()
{
    std::ostringstream oss;
    oss << std::endl;
    return oss.str();
}

} // namespace impl {

bool Multiplay::ready() // quick and dirty; should not it be in Io::Publisher?
{
    if( m_started ) { return true; }
    for( unsigned int i = 0; i < m_configs.size(); ++i )
    {
        m_publishers[i]->accept();
        if( m_publishers[i]->size() < m_configs[i].minNumberOfClients )
        {
            boost::this_thread::sleep( boost::posix_time::millisec( 200 ) );
            return false;
        }
    }
    m_started = true;
    return true;
}
    
/*!
    @brief try to read from all files and write the oldest
    @return true if at least one file could be read
*/
bool Multiplay::read()
{
    if( !ready() ) { return true; }
    bool end = true;
    for( unsigned int i = 0U; i < m_configs.size(); ++i )
    {
        if( !m_timestamps[i].is_not_a_date_time() ) { end = false; continue; }
        const time* time = m_inputStreams[i]->read();
        if( time == NULL ) { continue; }
        boost::posix_time::ptime t = time->time;
        if( m_configs[i].offset.total_microseconds() != 0 )
        {
            t += m_configs[i].offset;
        }
        end = false;
        if( ( ( !m_from.is_not_a_date_time() ) && ( t < m_from ) ) || ( ( !m_to.is_not_a_date_time() ) && ( t > m_to ) ) )
        {            
            i--;
            continue;
        }
        m_timestamps[i] = t;
    }
    if( end ) { return false; }
    boost::posix_time::ptime oldest;
    std::size_t index = 0;
    for( unsigned int i = 0; i < m_timestamps.size(); ++i )
    {
        if( !oldest.is_not_a_date_time() && ( m_timestamps[i].is_not_a_date_time() || m_timestamps[i] >= oldest ) ) { continue; }
        oldest = m_timestamps[i];
        index = i;
    }
    if( ( ( !m_from.is_not_a_date_time() ) && ( oldest < m_from ) ) || ( ( !m_to.is_not_a_date_time() ) && ( oldest > m_to ) ) )
    {
        return true;
    }
    m_play.wait( oldest );
    if( m_configs[index].options.binary() )
    {
        if( binary_[index] )
        {
            ::memcpy( &buf_fer[0], m_inputStreams[index]->binary().last(), buf_fer.size() );
            binary_[index]->put( time( oldest ), &buf_fer[0] );
            m_publishers[index]->write( &buf_fer[0], buf_fer.size() );
        }
        else
        {
            m_publishers[index]->write( m_inputStreams[index]->binary().last(), m_configs[index].options.format().size() );
        }
    }
    else
    {
        static std::string endl = impl::endl(); // quick and dirty, since publisher is not std::stream
        if( ascii_[index] )
        {
            std::vector< std::string > last = m_inputStreams[index]->ascii().last();
            ascii_[index]->put( time( oldest ), last );
            ( *m_publishers[index] ) << comma::join( last, m_configs[index].options.delimiter ) << endl;
        }
        else
        {
            ( *m_publishers[index] ) << comma::join( m_inputStreams[index]->ascii().last(), m_configs[index].options.delimiter ) << endl;
        }
    }
    m_timestamps[index] = boost::posix_time::not_a_date_time;
    return true;
}

} // namespace comma {

