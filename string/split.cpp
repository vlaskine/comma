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

#include <comma/string/split.h>

namespace comma {

static bool is_separator( char c, const char* separators )
{
    for( const char* s = separators; *s; ++s ) { if( c == *s ) { return true; } }
    return false;
}

std::vector< std::string > split( const std::string& s, char separator )
{
    char separators[] = { separator, 0 };
    return split( s, separators );
}

std::vector< std::string > split( const std::string& s, const char* separators )
{
    std::vector< std::string > v;
    const char* begin( &s[0] );
    const char* end( begin + s.length() );
    v.push_back( std::string() );
    for( const char* p = begin; p < end; ++p )
    {
        if( is_separator( *p, separators ) )
        {
            v.push_back( std::string() );
        }
        else
        {
            v.back() += *p;
        }
    }
    return v;
}

} // namespace comma {
