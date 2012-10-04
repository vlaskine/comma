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

#include <comma/string/string.h>

namespace comma {

static bool is_in( char c, const char* characters )
{
    for( const char* s = characters; *s; ++s ) { if( c == *s ) { return true; } }
    return false;
}

std::string strip( const std::string& s, char character )
{
    char characters[] = { character, 0 };
    return strip( s, characters );
}

std::string strip( const std::string& s, const char* characters )
{
    if( s.empty() ) { return s; }
    std::size_t begin = 0;
    while( begin < s.length() && is_in( s.at( begin ), characters ) ) { ++begin; }
    std::size_t end = s.length() - 1;
    while( end > begin && is_in( s.at( end ), characters ) ) { --end; }
    return s.substr( begin, end + 1 - begin );
}

} // namespace comma {
