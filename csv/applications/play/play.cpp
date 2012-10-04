// This file is part of Ark, a generic and flexible library 
// for robotics research.
//
// Copyright (C) 2011 The University of Sydney
//
// Ark is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Ark is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License 
// for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Ark. If not, see <http://www.gnu.org/licenses/>.

#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include "./play.h"


namespace comma { namespace csv { namespace impl {
    
/// constructor    
play::play( double speed, bool quiet, const boost::posix_time::time_duration& precision ):
    m_speed( speed ),
    m_precision( precision ),
    m_lag( false ),
    m_lagCounter( 0U ),
    m_quiet( quiet )
{
}

/// constructor
/// @param first first timestamp
/// @param speed slow-down factor: 1.0 = real time, 2.0 = twice as slow etc...
/// @param quiet if true, do not output warnings if we can not keep up with the desired playback speed
/// @param precision expected precision from the sleep function
play::play( const boost::posix_time::ptime& first, double speed, bool quiet, const boost::posix_time::time_duration& precision ):

    m_systemFirst( boost::get_system_time() ),
    m_offset( m_systemFirst - first ),
    m_first( first ),
    m_last( first ),
    m_speed( speed ),
    m_precision( precision ),
    m_lag( false ),
    m_lagCounter( 0U ),
    m_quiet( quiet )
{
    
}


/// wait until a timestamp
/// @param time timestamp as ptime
void play::wait( const boost::posix_time::ptime& time )
{

    if ( !m_offset )
    {
        boost::posix_time::ptime systemTime = boost::get_system_time();
        m_offset = systemTime - time;
        m_systemFirst = systemTime;
        m_first = time;
        m_last = time;
    }
    else
    {        
        if ( time > m_last )
        {
            boost::posix_time::ptime systemTime = boost::get_system_time();
            const boost::posix_time::ptime target = m_systemFirst + boost::posix_time::milliseconds( static_cast<long>(( time - m_first ).total_milliseconds() * m_speed ) );
            const boost::posix_time::time_duration lag = systemTime - target;
            if ( !m_quiet && ( lag > m_precision ) ) // no need to be alarmed for a lag less than the expected accuracy
            {
                if( !m_lag )
                {
                    m_lag = true;
                    std::cerr << "csv-play: warning, lagging behind " << lag << std::endl;
                }
                m_lagCounter++;
            }
            else
            {
                if( !m_quiet && m_lag )
                {
                    m_lag = false;
                    std::cerr << "csv-play: recovered after " << m_lagCounter << " packets " << std::endl;
                    m_lagCounter = 0U;
                }
                if ( lag < -m_precision ) // no need to sleep less than the expected accuracy
                {
                    boost::this_thread::sleep( target );
                }
            }
            m_last = time;
        }
        else
        {
            // timestamp same or earlier than last time, nothing to do
        }
    }
}

/// wait until a timestamp
/// @param isoTime timestamp in iso format
void play::wait( const std::string& isoTime )
{
    wait( boost::posix_time::from_iso_string( isoTime ) );
}

} } } // namespace comma { namespace csv { namespace impl {
    