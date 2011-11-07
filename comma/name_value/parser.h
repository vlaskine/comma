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

#ifndef COMMA_APPLICATION_NAME_VALUE_PARSER_H
#define COMMA_APPLICATION_NAME_VALUE_PARSER_H

#include <map>

#include <comma/base/exception.h>
#include <comma/visiting/apply.h>
#include <comma/name_value/map.h>
#include <comma/name_value/impl/options.h>
#include <comma/name_value/impl/from_name_value.h>
#include <comma/name_value/impl/to_name_value.h>

namespace comma
{
namespace name_value
{
    
/// parser for semicolon-separated name-value string
class parser
{
public:
    /// constructor
    /// @param delimiter delimiter between named values
    /// @param value_delimiter delimiter between name and value
    /// @param full_path_as_name use full path as name
    parser( char delimiter = ';', char value_delimiter = '=', bool full_path_as_name = true );
    
    /// constructor
    /// @param delimiter delimiter between named values
    /// @param value_delimiter delimiter between name and value
    /// @param full_path_as_name use full path as name
    /// @param fields default names for unnamed value
    parser( const std::string& fields, char delimiter = ';', char value_delimiter = '=', bool full_path_as_name = true );

    /// get struct from string
    template < typename S >
    S get( const std::string& line, const S& default_s = S() ) const;
    
    /// put struct into string
    template < typename S >
    std::string put( const S& s ) const;
    
    /// put struct into string
    /// @todo implement
    template < typename S >
    void put( std::string& line, const S& s ) const;

private:
    impl::options m_options;
};


inline parser::parser( char delimiter, char value_delimiter, bool full_path_as_name ):
    m_options( delimiter, value_delimiter, full_path_as_name )
{
}

inline parser::parser( const std::string& fields, char delimiter, char value_delimiter, bool full_path_as_name ):
    m_options( fields, delimiter, value_delimiter, full_path_as_name )
{
}

template < typename S >
inline S parser::get( const std::string& line, const S& default_s ) const
{
    map::map_type m = map( line, m_options ).get();
    name_value::impl::from_name_value from_name_value( m, m_options.m_full_path_as_name );
    S s = default_s;
    visiting::apply( from_name_value ).to( s );
    return s;
}

template < typename S >
inline std::string parser::put( const S& s ) const
{
    name_value::impl::to_name_value toname_value( m_options.m_value_delimiter, m_options.m_full_path_as_name );
    visiting::apply( toname_value ).to( s );
    return join( toname_value.strings(), m_options.m_delimiter );
}

} }

#endif // COMMA_APPLICATION_NAME_VALUE_PARSER_H
