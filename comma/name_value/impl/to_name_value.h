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

#ifndef COMMA_APPLICATION_TO_NAME_VALUE_H
#define COMMA_APPLICATION_TO_NAME_VALUE_H

#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <comma/string/string.h>
#include <comma/visiting/while.h>
#include <comma/visiting/visit.h>
#include <comma/xpath/xpath.h>

namespace comma { namespace name_value { namespace impl {

/// visitor getting a string from a struct 
class to_name_value
{
public:
    /// constructor
    /// @param delimiter delimiter between name and value
    /// @param full_path_as_name use full path as name
    to_name_value( char delimiter = '=', bool full_path_as_name = true ):
        m_delimiter(delimiter), m_full_path_as_name(full_path_as_name){};

    /// apply
    template < typename K, typename T > void apply( const K& name, const boost::optional< T >& value );
    
    /// apply
    template < typename K, typename T > void apply( const K& name, const boost::scoped_ptr< T >& value );
    
    /// apply
    template < typename K, typename T > void apply( const K& name, const boost::shared_ptr< T >& value );        
        
    /// apply
    template < typename K, typename T >
    void apply( const K& name, const T& value );

    /// apply to non-leaf elements
    template < typename K, typename T >
    void apply_next( const K& name, const T& value );

    /// apply to leaf elements
    template < typename K, typename T >
    void apply_final( const K& name, const T& value );

    /// return named values as strings
    const std::vector< std::string >& strings() const { return m_strings; }

private:
    template < typename T >
        std::string as_string( T v )
        {
            std::ostringstream oss;
            oss << v;
            return oss.str();
        }
        
    char m_delimiter;
    bool m_full_path_as_name;
    std::vector< std::string > m_strings;
    xpath m_xpath;
     
};

template < typename K, typename T >
inline void to_name_value::apply( const K& name, const boost::optional< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_name_value::apply( const K& name, const boost::scoped_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_name_value::apply( const K& name, const boost::shared_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_name_value::apply( const K& name, const T& value )
{
    m_xpath /= xpath::element( name );
    visiting::do_while<    !boost::is_fundamental< T >::value
                        && !boost::is_same< T, std::string >::value >::visit( name, value, *this );
    m_xpath = m_xpath.head();
}



template < typename K, typename T >
inline void to_name_value::apply_next( const K& name, const T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void to_name_value::apply_final( const K&, const T& value )
{
    std::string string;
    if( m_full_path_as_name )
    {
        string += m_xpath.to_string();
    }
    else
    {
        string += m_xpath.elements.back().to_string();
    }
    string += m_delimiter + as_string( value );
    m_strings.push_back( string );
}

} } } // namespace comma { namespace name_value { namespace impl {

#endif // COMMA_APPLICATION_TO_NAME_VALUE_H
