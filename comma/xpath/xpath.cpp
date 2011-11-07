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

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <comma/base/exception.h>
#include <comma/string/split.h>
#include "./x_path.h"

#include <iostream>

namespace comma {

x_path::element::element() {}

x_path::element::element( const x_path::element& rhs ) { operator=( rhs ); }

x_path::element::element( const std::string& n )
{
    std::vector< std::string > v = split( n, '[' );
    switch( v.size() )
    {
        case 1:
            name = v[0];
            break;
        case 2:
            if( v[0] == "" ) { COMMA_THROW_STREAM( comma::exception, "got non-empty index " << v[1] << " in empty element" ); }
            if( v[1].length() < 2 || v[1][ v[1].size() - 1 ] != ']' ) { COMMA_THROW_STREAM( comma::exception, "invalid index in element \"" << n << "\"" ); }
            name = v[0];
            index = boost::lexical_cast< std::size_t >( v[1].substr( 0, v[1].size() - 1 ) );
            break;
        default:
            COMMA_THROW_STREAM( comma::exception, "invalid element \"" << n << "\"" );
    }    
}

x_path::element::element( const std::string& n, std::size_t idx )
    : name( n )
    , index( idx )
{
    if( index && n == "" ) { COMMA_THROW_STREAM( comma::exception, "got non-empty index in empty element" ); }
}

x_path::element::element( const std::string& n, boost::optional< std::size_t > idx )
    : name( n )
    , index( idx )
{
    if( index && name == "" ) { COMMA_THROW_STREAM( comma::exception, "got non-empty index in empty element" ); }
}

bool x_path::element::operator==( const x_path::element& rhs ) const
{
    return name == rhs.name && index == rhs.index;
}
            
bool x_path::element::operator!=( const x_path::element& rhs ) const { return !operator==( rhs ); }

bool x_path::element::operator<( const x_path::element& rhs ) const
{
    return name == rhs.name && index && !rhs.index;
}

bool x_path::element::operator<=( const x_path::element& rhs ) const
{
    return operator<( rhs ) || operator==( rhs );
}

std::string x_path::element::to_string() const
{
    if( !index ) { return name; }
    return name + "[" + boost::lexical_cast< std::string >( *index ) + "]";
}

x_path::x_path() {}

x_path::x_path( const x_path& rhs ) { operator=( rhs ); }

static void init( x_path& x, const std::string& str, char delimiter )
{
    if( str == "" ) { return; }
    std::vector< std::string > v = comma::split( str, delimiter );
    for( std::size_t i = 0; i < v.size(); ++i )
    {
        if( i > 0 && v[i] == "" ) { continue; }
        x.elements.push_back( x_path::element( v[i] ) );
    }	
}

x_path::x_path( const char* str, char delimiter ) { init( *this, str, delimiter ); }

x_path::x_path( const std::string& str, char delimiter ) { init( *this, str, delimiter ); }

x_path::x_path( const x_path::element& rhs, char delimiter ) { init( *this, "", delimiter ); operator/=( rhs ); }

const x_path& x_path::operator/=( const x_path& rhs )
{
    if( !rhs.elements.empty() )
    {
        for( std::size_t i = 0; i < rhs.elements.size(); ++i )
        {
            if( rhs.elements[i] != element( "" ) ) { elements.push_back( rhs.elements[i] ); }
        }
    }
    return *this;
}

const x_path& x_path::operator/=( const x_path::element& rhs )
{
    if( rhs != x_path::element( "" ) ) { elements.push_back( rhs ); }
    return *this;
}

const x_path& x_path::operator/=( const std::string& rhs ) { return operator/=( x_path( rhs ) ); }

x_path x_path::tail() const
{
    x_path x;
    if( elements.size() > 1 )
    {
        x.elements.resize( elements.size() - 1 );
        for( std::size_t i = 1; i < elements.size(); ++i ) { x.elements[ i - 1 ] = elements[i]; }
    }
    return x;
}

x_path x_path::operator/( const x_path& rhs ) const { x_path x( *this ); x /= rhs; return x; }

x_path x_path::operator/( const x_path::element& rhs ) const { x_path x( *this ); x /= rhs; return x; }

x_path x_path::operator/( const std::string& rhs ) const { x_path x( *this ); x /= rhs; return x; }

x_path x_path::head() const
{
    if( *this == x_path( "/" ) ) { return *this; }
    x_path x;
    if( elements.size() > 1 )
    {
        x.elements.resize( elements.size() - 1 );
        for( std::size_t i = 0; i < x.elements.size(); ++i ) { x.elements[i] = elements[i]; }
    }
    return x;
}

std::string x_path::to_string( char delimiter ) const
{
    if( elements.empty() ) { return ""; }
    std::ostringstream oss;
    oss << elements[0].to_string();
    for( std::size_t i = 1; i < elements.size(); ++i ) { oss << delimiter << elements[i].to_string(); }
    return oss.str();
}

bool x_path::operator==( const x_path& rhs ) const
{
    if( elements.size() != rhs.elements.size() ) { return false; }
    for( std::size_t i = 0; i < elements.size(); ++i )
    {
        if( elements[i] != rhs.elements[i] ) { return false; }
    }
    return true;
} 

bool x_path::operator!=( const x_path& rhs ) const { return !operator==( rhs ); }

bool x_path::operator<( const x_path& rhs ) const
{
    if( elements.size() < rhs.elements.size() ) { return false; }
    if( elements.empty() ) { return false; }
    if( rhs.elements.empty() ) { return true; }
    std::size_t i = 0;
    for( ; i < rhs.elements.size() - 1; ++i )
    {
        if( elements[i] != rhs.elements[i] ) { return false; }
    }
    return   elements.size() == rhs.elements.size()
           ? elements.back() < rhs.elements.back()
           : elements[i] <= rhs.elements.back();
}

bool x_path::operator<=( const x_path& rhs ) const { return operator<( rhs ) || operator==( rhs ); }

std::ostream& operator<<( std::ostream& os, const x_path& x ) { return os << x.to_string(); }

std::ostream& operator<<( std::ostream& os, const x_path::element& e ) { return os << e.to_string(); }

} // namespace comma {
