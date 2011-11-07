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

#ifndef COMMA_APPLICATION_NAME_VALUE_OPTIONS_H
#define COMMA_APPLICATION_NAME_VALUE_OPTIONS_H

#include <comma/string/string.h>
#include <comma/x_path/x_path.h>

namespace comma
{
namespace name_value
{
namespace impl
{

/// name_value options
struct options
{
    options( char delimiter = ';', char value_delimiter = '=', bool full_path_as_name = true );
    options( const std::string& fields, char delimiter = ';', char value_delimiter = '=', bool full_path_as_name = true );
    
    char m_delimiter;
    char m_value_delimiter;
    bool m_full_path_as_name;
    std::vector< std::string > m_names; /// names for unnamed values
};


inline options::options( char delimiter, char value_delimiter, bool full_path_as_name ):
    m_delimiter( delimiter ),
    m_value_delimiter( value_delimiter ),
    m_full_path_as_name( full_path_as_name )
{
}

inline options::options( const std::string& fields, char delimiter, char value_delimiter, bool full_path_as_name ):
    m_delimiter( delimiter ),
    m_value_delimiter( value_delimiter ),
    m_full_path_as_name( full_path_as_name ),
    m_names( split( fields, ',' ) )
{
    if( fields.empty() ) { COMMA_THROW( comma::exception, "expected fields, got empty string" ); }
    if( full_path_as_name ) { return; }
    for( std::size_t i = 0; i < m_names.size(); ++i )
    {
        if( m_names[i] != "" ) { m_names[i] = x_path( m_names[i] ).elements.back().to_string(); }
    }
}

} } }

#endif // COMMA_APPLICATION_NAME_VALUE_OPTIONS_H
