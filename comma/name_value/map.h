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

#ifndef COMMA_APPLICATION_NAME_VALUE_MAP_H
#define COMMA_APPLICATION_NAME_VALUE_MAP_H

#include <comma/name_value/impl/options.h>
#include <boost/lexical_cast.hpp>

namespace comma
{
namespace name_value
{

/// constructs a map of name-value pair from an input string
/// TODO implement fullPathAsName ? 
class map
{
    public:
        /// constructor
        map( const std::string& line, char delimiter = ';', char value_delimiter = '=' );
        /// constructor
        map( const std::string& line, const std::string& fields, char delimiter = ';', char value_delimiter = '=' );
        /// constructor
        map( const std::string& line, const impl::options& options );

        /// return true, if field exists
        bool exists( const std::string& name ) const;

        /// return values for all the fields with a given name
        template < typename T >
        std::vector< T > values( const std::string& name ) const;

        /// return value, if field exists; otherwise return default
        template < typename T >
        T value( const std::string& name, const T& default_value ) const;

        /// return value, if field exists; otherwise throw
        template < typename T >
        T value( const std::string& name ) const;

        /// map type
        typedef std::multimap< std::string, std::string > map_type;

        /// return name-value map
        const map_type& get() const { return m_map; }

    private:
        void init( const comma::name_value::impl::options& options );
        const std::string m_line;
        map_type m_map;
};

inline map::map( const std::string& line, char delimiter, char value_delimiter ):
    m_line( line )
{
    init( impl::options( delimiter, value_delimiter ) );
}

inline map::map( const std::string& line, const std::string& fields, char delimiter, char value_delimiter ):
    m_line( line )
{
    init( impl::options( fields, delimiter, value_delimiter ) );
}

inline map::map(const std::string& line, const comma::name_value::impl::options& options):
    m_line( line )
{
    init( options );
}


inline void map::init( const comma::name_value::impl::options& options )
{
    std::vector< std::string > named_values = split( m_line, options.m_delimiter );
    for( std::size_t i = 0; i < options.m_names.size() && i < named_values.size(); ++i )
    {
        if( options.m_names[i].empty() ) { continue; }
        if( split( named_values[i], options.m_value_delimiter ).size() != 1U ) { COMMA_THROW_STREAM( comma::exception, "expected unnamed value for " << options.m_names[i] << ", got: " << named_values[i] ); }
        named_values[i] = options.m_names[i] + options.m_value_delimiter + named_values[i];
    }
    for( std::size_t i = 0; i < named_values.size(); ++i )
    {
        std::vector< std::string > pair = split( named_values[i], options.m_value_delimiter );
        switch( pair.size() )
        {
            case 1: m_map.insert( std::make_pair( pair[0], std::string() ) ); break; // quick and dirty
            case 2: m_map.insert( std::make_pair( pair[0], pair[1] ) ); break;
            default: { COMMA_THROW_STREAM( comma::exception, "expected name-value pair, got: " << join( pair, options.m_value_delimiter ) ); }
        }
    }
}

inline bool map::exists( const std::string& name ) const { return m_map.find( name ) != m_map.end(); }

namespace detail {

template < typename T >
inline T lexical_cast( const std::string& s ) { return boost::lexical_cast< T >( s ); }

template <>
inline bool lexical_cast< bool >( const std::string& s )
{
    if( s == "" || s == "true" ) { return true; }
    if( s == "false" ) { return false; }
    return boost::lexical_cast< bool >( s );
}

} // namespace detail {

template < typename T >
inline std::vector< T > map::values( const std::string& name ) const
{
    std::vector< T > v;
    for( typename map_type::const_iterator it = m_map.begin(); it != m_map.end(); ++it )
    {
        if( it->first == name ) { v.push_back( detail::lexical_cast< T >( it->second ) ); }
    }
    return v;
}

template < typename T >
inline T map::value( const std::string& name, const T& default_value ) const
{
    const std::vector< T >& v = values< T >( name );
    return v.empty() ? default_value : v[0];
}

template < typename T >
inline T map::value( const std::string& name ) const
{
    const std::vector< T >& v = values< T >( name );
    if( v.empty() ) { COMMA_THROW_STREAM( comma::exception, "'" << name << "' not found in \"" << m_line << "\"" ); }
    return v[0];
}

} }

#endif // COMMA_APPLICATION_NAME_VALUE_MAP_H
