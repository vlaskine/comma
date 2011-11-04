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

#ifndef COMMA_PACKED_BIG_ENDIAN_H_
#define COMMA_PACKED_BIG_ENDIAN_H_

#include <boost/static_assert.hpp>
#include <comma/base/exception.h>
#include <comma/base/types.h>
#include <comma/packed/field.h>

namespace comma { namespace packed {

namespace detail {

template < typename T > struct big_endian_traits {};

template <> struct big_endian_traits< comma::uint16 >
{
    static comma::uint16 hton( comma::uint16 v ) { return htons( v ); }
    static comma::uint16 ntoh( comma::uint16 v ) { return ntohs( v ); }
};

template <> struct big_endian_traits< comma::uint32 >
{
    static comma::uint32 hton( comma::uint32 v ) { return htonl( v ); }
    static comma::uint32 ntoh( comma::uint32 v ) { return ntohl( v ); }
};

template < typename T >
class big_endian_int : public packed::Field< big_endian_int< T >, T, sizeof( T ) >
{
    public:
        enum { size = sizeof( T ) };

        BOOST_STATIC_ASSERT( size == 2 || size == 4 );

        typedef T type;

        typedef packed::Field< big_endian_int< T >, T, size > base_type;

        static type default_value() { return 0; }

        static void pack( char* storage, type value )
        {
            type v( big_endian_traits< type >::hton( value ) );
            ::memcpy( storage, ( void* )&v, size );
        }

        static type unpack( const char* storage )
        {
            type value;
            ::memcpy( ( void* )&value, storage, size );
            return big_endian_traits< type >::ntoh( value );
        }

        const big_endian_int& operator=( const big_endian_int& rhs ) { return base_type::operator=( rhs ); }

        const big_endian_int& operator=( type rhs ) { return base_type::operator=( rhs ); }
};

} // namespace detail {

/// big endian 16-bit integer    
typedef detail::big_endian_int< comma::uint16 > big_endian_uint16;
/// alias for big endian 16-bit integer
typedef big_endian_uint16 net_uint16;
/// big endian 32-bit integer
typedef detail::big_endian_int< comma::uint32 > big_endian_uint32;
/// alias for big endian 32-bit integer
typedef big_endian_uint32 net_uint32;

} } // namespace comma { namespace packed {

#endif // #ifndef COMMA_PACKED_BIG_ENDIAN_H_
