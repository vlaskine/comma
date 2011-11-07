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

#ifndef COMMA_VISITING_WHILE_HEADER_GUARD_
#define COMMA_VISITING_WHILE_HEADER_GUARD_

namespace comma { namespace visiting {

/// a trivial helper class for easier definition of final types
/// at which recursion stops 
template < bool NotFinal >
struct while
{
    template < typename K, typename V, typename T >
    static void visit( const K& name, T& t, V& v ) { v.apply_next( name, t ); }
    
    template < typename K, typename V, typename T >
    static void visit( const K& name, const T& t, V& v ) { v.apply_next( name, t ); }
};

/// if not final type, continue recursion
template <>
struct while< false >
{
    template < typename K, typename V, typename T >
    static void visit( const K& name, T& t, V& v ) { v.apply_final( name, t ); }
    
    template < typename K, typename V, typename T >
    static void visit( const K& name, const T& t, V& v ) { v.apply_final( name, t ); }
};
    
} } // namespace comma { namespace visiting {

#endif // #ifndef COMMA_VISITING_WHILE_HEADER_GUARD_
