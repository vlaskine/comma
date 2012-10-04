// This file is part of comma library
//
// Copyright (c) Matthew Herrmann 2007
// Copyright (c) Vsevolod Vlaskine 2010-2011
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

#ifndef COMMA_PACKED_BYTE_HEADER_H_
#define COMMA_PACKED_BYTE_HEADER_H_

#if defined(__linux__) || defined(__APPLE__) || defined(__QNXNTO__)
#include <arpa/inet.h>
#elif defined(WIN32)
#if defined(WINCE)
#include <Winsock.h>
#else
#include <Winsock2.h>
#endif
#endif

#include <boost/static_assert.hpp>
#include <comma/packed/field.h>

namespace comma { namespace packed {

/// packed byte
struct byte : public packed::field< byte, unsigned char, sizeof( unsigned char ) >
{
    enum { size = sizeof( unsigned char ) };

    BOOST_STATIC_ASSERT( size == 1 );

    typedef unsigned char type;

    typedef packed::field< byte, unsigned char, sizeof( unsigned char ) > base_type;

    static type default_value() { return 0; }

    static void pack( char* storage, type value ) { *storage = static_cast< char >( value ); }

    static type unpack( const char* storage ) { return static_cast< unsigned char >( *storage ); }

    const byte& operator=( const byte& rhs ) { return base_type::operator=( rhs ); }

    const byte& operator=( type rhs ) { return base_type::operator=( rhs ); }
};

/// packed fixed-value byte
template < unsigned char C >
struct const_byte : public packed::field< const_byte< C >, unsigned char, sizeof( unsigned char ) >
{
    enum { size = sizeof( unsigned char ) };

    BOOST_STATIC_ASSERT( size == 1 );

    typedef unsigned char type;

    typedef packed::field< const_byte, unsigned char, sizeof( unsigned char ) > base_type;

    const_byte() { base_type::operator=( C ); }

    static type default_value() { return C; }

    static void pack( char* storage, type value ) { *storage = static_cast< char >( value ); }

    static type unpack( const char* storage ) { return static_cast< unsigned char >( *storage ); }

    //const const_byte& operator=( const const_byte& rhs ) { return base_type::operator=( rhs ); }

    //const const_byte& operator=( type rhs ) { return base_type::operator=( rhs ); }
};

} } // namespace comma { namespace packed {

#endif // #ifndef COMMA_PACKED_BYTE_HEADER_H_
