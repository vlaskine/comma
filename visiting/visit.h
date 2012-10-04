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

#ifndef COMMA_VISITING_VISIT_HEADER_GUARD_
#define COMMA_VISITING_VISIT_HEADER_GUARD_

#include <comma/visiting/traits.h>

namespace comma { namespace visiting {
    
/// visit
template < typename K, typename T, typename V >
inline void visit( const K& key, T& t, V& v ) { traits< T >::visit( key, t, v ); }

/// visit const
template < typename K, typename T, typename V >
inline void visit( const K& key, const T& t, V& v ) { traits< T >::visit( key, t, v ); }
    
} } // namespace comma { namespace visiting {

#endif // COMMA_VISITING_VISIT_HEADER_GUARD_
