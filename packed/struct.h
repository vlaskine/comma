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

#ifndef COMMA_PACKED_STRUCT_HPP_
#define COMMA_PACKED_STRUCT_HPP_

#include <cstring>
#include <boost/static_assert.hpp>

namespace comma { namespace packed {

/// packed structure
template < class Derived, size_t S >
class packed_struct
{
    public:
        enum { size = S };

        packed_struct() throw() { BOOST_STATIC_ASSERT( sizeof( Derived ) == size ); }

        const char* data() const throw() { return reinterpret_cast< const char* >( this ); }

        char* data() throw() { return reinterpret_cast< char* >( this ); }

        bool operator==( const packed_struct& rhs ) const { return ::memcmp( this, &rhs, size ) == 0; }

        bool operator!=( const packed_struct& rhs ) const { return !operator==( rhs ); }
};

} } // namespace comma { namespace packed {

#endif // #ifndef COMMA_PACKED_STRUCT_HPP_
