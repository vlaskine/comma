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

#ifndef COMMA_PACKED_FIELD_H_
#define COMMA_PACKED_FIELD_H_

#include <string.h>
#include <boost/static_assert.hpp>

namespace comma { namespace packed {

/// packed field base class
template < typename Derived, typename T, size_t S >
class field
{
    public:
        enum { size = S };

        BOOST_STATIC_ASSERT( size > 0 );

        typedef T type;

        field()
        {
            BOOST_STATIC_ASSERT( sizeof( field ) == size );
            Derived::pack( storage_, Derived::default_value() );
        }

        field( const type& t )
        {
            BOOST_STATIC_ASSERT( sizeof( field ) == size );
            Derived::pack( storage_, t );
        }

        field( const field& rhs )
        {
            BOOST_STATIC_ASSERT( sizeof( field ) == size );
            operator=( rhs );
        }

        type operator()() const { return Derived::unpack( storage_ ); }

        const char* data() const throw() { return storage_; }

        char* data() throw() { return storage_; }

        void pack( char* storage, const type& t );

        type unpack( const char* storage );

        const Derived& operator=( const Derived& rhs ) { ::memcpy( storage_, rhs.storage_, size ); return reinterpret_cast< const Derived& >( *this ); }

        const Derived& operator=( const type& rhs ) { Derived::pack( storage_, rhs ); return reinterpret_cast< const Derived& >( *this ); }

        bool operator==( const Derived& rhs ) { return ::memcmp( storage_, rhs.storage_, size ) == 0; }

        bool operator==( const type& rhs ) { return Derived::unpack( storage_ ) == rhs; }

        bool operator!=( const Derived& rhs ) { return !operator==( rhs ); }

        bool operator!=( const type& rhs ) { return !operator==( rhs ); }

    private:
        char storage_[ size ];
};

} } // namespace comma { namespace packed {

#endif // COMMA_PACKED_FIELD_H_
