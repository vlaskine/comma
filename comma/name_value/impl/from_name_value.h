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

#ifndef COMMA_APPLICATION_FROM_NAME_VALUE_H
#define COMMA_APPLICATION_FROM_NAME_VALUE_H

#include <deque>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <comma/base/types.h>
#include <comma/string/string.h>
#include <comma/visiting/while.h>
#include <comma/visiting/visit.h>
#include <comma/x_path/x_path.h>

namespace comma { namespace name_value { namespace impl {

/// visitor getting a struct from named values
class from_name_value
{
public:
    /// map type
    typedef std::multimap< std::string, std::string > map_type;
    
    /// constructor
    /// @param values values to read from
    /// @param full_path_as_name use full path as name
    from_name_value( const map_type& values, bool full_path_as_name = true ):
        m_values( values ), m_full_path_as_name(full_path_as_name){};

    /// apply
    template < typename K, typename T > void apply( const K& name, boost::optional< T >& value );
    
    /// apply
    template < typename K, typename T > void apply( const K& name, boost::scoped_ptr< T >& value );
    
    /// apply
    template < typename K, typename T > void apply( const K& name, boost::shared_ptr< T >& value );
        
    /// apply
    template < typename K, typename T > void apply( const K& name, T& value );

    /// apply to non-leaf elements
    template < typename K, typename T > void apply_next( const K& name, T& value );

    /// apply to leaf elements
    template < typename K, typename T > void apply_final( const K& name, T& value );

private:
    const map_type& m_values;
    bool m_full_path_as_name;
    x_path m_xpath;
    std::deque< bool > m_empty;
    static void lexical_cast( bool& v, const std::string& s ) { v = s == "" || boost::lexical_cast< bool >( s ); }
    static void lexical_cast( boost::posix_time::ptime& v, const std::string& s ) { v = boost::posix_time::from_iso_string( s ); }
    static void lexical_cast( boost::posix_time::time_duration& v, const std::string& s )
    {
        std::vector< std::string > t = comma::split( s, '.' );
        if( t.size() > 2 ) { COMMA_THROW_STREAM( comma::exception, "expected duration in seconds, got " << s ); }
        comma::int_64 seconds = boost::lexical_cast< comma::int_64 >( t[0] );
        if( t[1].length() > 6 ) { t[1] = t[1].substr( 0, 6 ); }
        else { t[1] += std::string( 6 - t[1].length(), '0' ); }
        comma::int_32 microseconds = boost::lexical_cast< comma::int_32 >( t[1] );
        if( seconds < 0 ) { microseconds = -microseconds; }
        v = boost::posix_time::seconds( seconds ) + boost::posix_time::microseconds( microseconds );
    }
    template < typename T > static void lexical_cast( T& v, const std::string& s ) { v = boost::lexical_cast< T >( s ); }
};

template < typename K, typename T >
inline void from_name_value::apply( const K& name, boost::optional< T >& value )
{
    if( value ) { apply( name, *value ); return; }
    T t;
    m_empty.push_back( true );
    apply( name, t );
    if( !m_empty.back() ) { value = t; }
    m_empty.pop_back();
}

template < typename K, typename T >
inline void from_name_value::apply( const K& name, boost::scoped_ptr< T >& value )
{
    if( value ) { apply( name, *value ); return; }
    T t;
    m_empty.push_back( true );
    apply( name, t );
    if( !m_empty.back() ) { value.reset( new T( t ) ); }
    m_empty.pop_back();
}

template < typename K, typename T >
inline void from_name_value::apply( const K& name, boost::shared_ptr< T >& value )
{
    if( value ) { apply( name, *value ); return; }
    T t;
    m_empty.push_back( true );
    apply( name, t );
    if( !m_empty.back() ) { value.reset( new T( t ) ); }
    m_empty.pop_back();
}

template < typename K, typename T >
inline void from_name_value::apply( const K& name, T& value )
{
    m_xpath /= x_path::element( name );
    visiting::while<    !boost::is_fundamental< T >::value
                     && !boost::is_same< T, boost::posix_time::ptime >::value
                     && !boost::is_same< T, boost::posix_time::time_duration >::value
                     && !boost::is_same< T, std::string >::value >::visit( name, value, *this );
    m_xpath = m_xpath.head();
}

template < typename K, typename T >
inline void from_name_value::apply_next( const K& name, T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void from_name_value::apply_final( const K& key, T& value )
{
    map_type::const_iterator iter = m_values.find( m_full_path_as_name ? m_xpath.to_string() : m_xpath.elements.back().to_string() );
    if( iter == m_values.end() ) { return; }
    lexical_cast( value, iter->second );
    for( std::size_t i = 0; i < m_empty.size(); ++i ) { m_empty[i] = false; }
}

} } } // namespace comma { namespace name_value { namespace impl {

#endif // COMMA_APPLICATION_FROM_NAME_VALUE_H
