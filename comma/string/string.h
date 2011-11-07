// This file is part of comma, a generic and flexible library
//
// Copyright (C) 2011 Vsevolod Vlaskine and Cedric Wohlleber
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

#ifndef COMMA_STRING_STRING_H_
#define COMMA_STRING_STRING_H_

#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <comma/string/split.h>

namespace comma {

/// strip given characters from the beginning and end
std::string strip( const std::string& s, const char* characters = " \t\r\n" );

/// strip given character from the beginning and end
std::string strip( const std::string& s, char character );

/// join array elements into a string with given delimiter
template < typename A >
std::string join( const A& a, std::size_t size, char delimiter );

/// join array elements into a string with given delimiter
template < typename A >
inline std::string join( const A& a, char delimiter ) { return join( a, a.size(), delimiter ); }

template < typename A >
inline std::string join( const A& a, std::size_t size, char delimiter )
{
    if( size == 0 ) { return ""; }
    std::ostringstream oss;
    oss << a[0];
    for( std::size_t i = 1; i < size; ++i ) { oss << delimiter << a[i]; }
    return oss.str();
}

template < typename It >
inline std::string join( It begin, It end, char delimiter )
{
    if( begin == end ) { return ""; }
    std::ostringstream oss;
    oss << *begin;
    for( It i = begin + 1; i != end; ++i ) { oss << delimiter << *i; }
    return oss.str();
}

} // namespace comma {

#endif // COMMA_STRING_STRING_H_
