// This file is part of comma, a generic and flexible library 
// for robotics research.
//
// Copyright (C) 2011 The University of Sydney
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

#ifndef COMMA_PACKED_BITS_HEADER_H_
#define COMMA_PACKED_BITS_HEADER_H_

#include <string.h>
#include <boost/static_assert.hpp>
#include <comma/packed/field.h>

namespace comma { namespace packed {

/// packed bit-field structure
/// @note had to use memcpy(), since reinterpret_cast gives a compilation warning
template < typename B, unsigned char Default = 0 >
struct bits : public packed::field< bits< B, Default >, B, sizeof( unsigned char ) >
{
    enum { size = sizeof( unsigned char ) };

    BOOST_STATIC_ASSERT( size == 1 );
    
    BOOST_STATIC_ASSERT( sizeof( B ) == 1 );

    typedef B type;

    typedef packed::field< bits< B, Default >, B, sizeof( unsigned char ) > base_type;

    static type default_value() { static const unsigned char d = Default; type t; ::memcpy( &t, &d, 1 ); return t; }

    static void pack( char* storage, type value ) { ::memcpy( storage, &value, 1 ); }

    static type unpack( const char* storage ) { type t; ::memcpy( &t, storage, 1 ); return t; }

    const bits& operator=( const bits& rhs ) { return base_type::operator=( rhs ); }

    const bits& operator=( type rhs ) { return base_type::operator=( rhs ); }
    
    const bits& operator=( unsigned char rhs ) { type t; ::memcpy( &t, &rhs, 1 ); return base_type::operator=( t ); }
    
    type& fields() { return *( reinterpret_cast< type* >( this ) ); }
    
    const type& fields() const { return *( reinterpret_cast< const type* >( this ) ); }
};

} } // namespace comma { namespace packed {

#endif // COMMA_PACKED_BITS_HEADER_H_
