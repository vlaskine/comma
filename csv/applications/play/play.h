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

#ifndef COMMA_CSV_APPLICATIONS_PLAY_H
#define COMMA_CSV_APPLICATIONS_PLAY_H

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace comma { namespace csv { namespace impl {

/// play back timestamped data in a real time manner
class play
{
public:
    play( double speed = 1.0, bool quiet = false, const boost::posix_time::time_duration& precision = boost::posix_time::milliseconds(1) );
    play( const boost::posix_time::ptime& first, double speed = 1.0, bool quiet = false, const boost::posix_time::time_duration& precision = boost::posix_time::milliseconds(1) );

    void wait( const boost::posix_time::ptime& time );

    void wait( const std::string& isoTime );

private:
    boost::posix_time::ptime m_systemFirst; /// system time at first timestamp
    boost::optional< boost::posix_time::time_duration > m_offset; /// offset between timestamps and system time
    boost::posix_time::ptime m_first; /// first timestamp
    boost::posix_time::ptime m_last; /// last timestamp received
    const double m_speed;
    const boost::posix_time::time_duration m_precision;
    bool m_lag;
    unsigned int m_lagCounter;
    bool m_quiet;
};

} } } // namespace comma { namespace csv { namespace impl {

#endif // COMMA_CSV_APPLICATIONS_PLAY_H
