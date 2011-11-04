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

#ifndef COMMA_PACKED_LITTLEENDIAN_H_
#define COMMA_PACKED_LITTLEENDIAN_H_

#include <comma/base/types.h>
#include <comma/packed/field.h>

namespace comma { namespace packed {

namespace detail {

template < unsigned int Size, bool Signed > struct little_endian_traits { typedef void type; };
template <> struct little_endian_traits< 2, true > { typedef comma::Int16 type; };
template <> struct little_endian_traits< 2, false > { typedef comma::UInt16 type; };
template <> struct little_endian_traits< 3, true > { typedef comma::Int32 type; };
template <> struct little_endian_traits< 3, false > { typedef comma::UInt32 type; };
template <> struct little_endian_traits< 4, true > { typedef comma::Int32 type; };
template <> struct little_endian_traits< 4, false > { typedef comma::UInt32 type; };
    
template < unsigned int Size, bool Signed >
struct little_endian_int : public packed::field< little_endian_int< Size, Signed >, typename little_endian_traits< Size, Signed >::type, Size >
{
    static const unsigned int size = Size;

    typedef typename little_endian_traits< Size, Signed >::type type;
    
    BOOST_STATIC_ASSERT( size <= sizeof( type ) );

    typedef packed::field< little_endian_int< Size, Signed >, typename little_endian_traits< Size, Signed >::type, Size > base_type;

    static type default_value() { return 0; }

    static void pack( char* storage, type value )
    {
        for( unsigned int i = 0; i < size; ++i, value >>= 8 ) { storage[i] = value & 0xff; }
    }

    static type unpack( const char* storage )
    {
        type v = 0;
        int shift = 0;
        unsigned int i = 0;
        for( ; i < size; ++i, shift += 8 )
        {
            v += ( unsigned char )( storage[i] ) << shift;
        }
        if( storage[ size - 1 ] & 0x80 )
        {
            for( ; i < sizeof( type ); ++i, shift += 8 ) { v += 0xff << shift; }
        }
        return v;
    }

    const little_endian_int& operator=( const little_endian_int& rhs ) { return base_type::operator=( rhs ); }

    const little_endian_int& operator=( type rhs ) { return base_type::operator=( rhs ); }
};

} // namespace detail {

/// packed little endian 16-bit integer
typedef detail::little_endian_int< 2, true > int16;
/// packed little endian 24-bit integer (strangely, there are protocols using it)
typedef detail::little_endian_int< 3, true > int24;
/// packed little endian 32-bit integer
typedef detail::little_endian_int< 4, true > int32;
/// packed little endian 16-bit unsigned integer
typedef detail::little_endian_int< 2, false > uint16;
/// packed little endian 24-bit unsigned integer (strangely, there are protocols using it)
typedef detail::little_endian_int< 3, false > uint24;
/// packed little endian 32-bit unsigned integer
typedef detail::little_endian_int< 4, false > uint32;

} } // namespace comma { namespace packed {

#endif // #ifndef COMMA_PACKED_LITTLEENDIAN_H_
