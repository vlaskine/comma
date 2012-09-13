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

#ifndef COMMA_SERIALIZATION_TOSTRING_HEADER_GUARD_
#define COMMA_SERIALIZATION_TOSTRING_HEADER_GUARD_

#include <sstream>
#include <vector>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits.hpp>
#include <comma/visiting/apply.h>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>

namespace comma { namespace serialization {

/// to-string visitor: take an arbitrary structure and output it as a string
class to_string : public boost::noncopyable
{
    public:
        /// constructor
        to_string( bool indented = true );
        
        /// traverse
        template < typename K, typename T >
        void apply( const K& name, const T& value );
        
        /// traverse vector
        template < typename K, typename T >
        void apply_next( const K& name, const std::vector< T >& value );
        
        /// traverse boost::array
        template < typename K, typename T, std::size_t S >
        void apply_next( const K& name, const boost::array< T, S >& value );
        
        /// traverse
        template < typename K, typename T >
        void apply_next( const K& name, const T& value );
        
        /// output string
        template < typename K >
        void apply_final( const K& name, const std::string& value );
        
        /// output a non-string type
        template < typename K, typename T >
        void apply_final( const K& name, const T& value );
        
        /// return string
        std::string operator()() const;

        /// convenience function
        template < typename T >
        static std::string apply( const T& t, bool indented = true ) { to_string v( indented ); visiting::apply( v, t ); return v();  }
        
    private:
        std::ostringstream m_stream;
        unsigned int m_indent;
        bool m_indented;
        void indent( unsigned int i = 1 );
        void endl();
        template < typename K, typename T >
        void apply_to_container( const K& name, const T& value );
        bool nameEmpty( const char* name ) { return *name == 0; }
        bool nameEmpty( unsigned int ) { return false; }
};

inline to_string::to_string( bool indented )
    : m_indent( 0 )
    , m_indented( indented )
{
}

template < typename K, typename T >
inline void to_string::apply( const K& name, const T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                     && !boost::is_same< T, std::string >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void to_string::apply_next( const K& name, const T& value )
{
    if( !nameEmpty( name ) )
    {
        indent();
        m_stream << name << "=";
        endl();
    }
    indent( 0 );
    m_stream << "{";
    endl();
    m_indent += 4;
    comma::visiting::visit( name, value, *this );
    m_indent -= 4;
    indent();
    m_stream << "}";
    endl(); // if( !nameEmpty( name ) ) { endl(); }
}

template < typename K >
inline void to_string::apply_final( const K& name, const std::string& value ) { indent(); m_stream << name << "=\"" << value << "\""; endl(); }

template < typename K, typename T >
inline void to_string::apply_final( const K& name, const T& value ) { indent(); m_stream << name << "=" << value; endl(); }

template < typename K, typename T >
inline void to_string::apply_next( const K& name, const std::vector< T >& value ) { apply_to_container( name, value ); }

template < typename K, typename T, std::size_t S >
inline void to_string::apply_next( const K& name, const boost::array< T, S >& value ) { apply_to_container( name, value ); }

template < typename K, typename T >
inline void to_string::apply_to_container( const K& name, const T& value )
{
    indent();
    if( *name != 0 ) { m_stream << name << "="; }
    if( value.empty() )
    {
        m_stream << "[]";
    }
    else
    {
        endl();
        indent( 0 );
        m_stream << "[";
        endl();
        m_indent += 4;
        comma::visiting::visit( name, value, *this );
        m_indent -= 4;
        indent();
        m_stream << "]";
    }
    endl();
}

inline void to_string::endl() { if( m_indented ) { m_stream << std::endl; } }

inline void to_string::indent( unsigned int i ) { m_stream << std::string( m_indented ? m_indent : i, ' ' ); }

inline std::string to_string::operator()() const { return m_stream.str(); }

} } // namespace comma { namespace serialization {

#endif // #ifndef COMMA_SERIALIZATION_TOSTRING_HEADER_GUARD_
