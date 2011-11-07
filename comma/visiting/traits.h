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

#ifndef COMMA_VISITING_TRAITS_HEADER_GUARD_
#define COMMA_VISITING_TRAITS_HEADER_GUARD_

#ifndef WIN32
#include <stdlib.h>
#endif
#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/array.hpp>

namespace comma { namespace visiting {

/// visiting traits, need to be specialized for visited classes;
/// see visiting and serialization unit tests for examples
template < typename T >
struct traits
{
    /// visit arbitrary type
    template < typename K, typename V > static void visit( const K& key, T& t, V& v );
};

/// std::pair visiting traits; todo: better semantics?
template < typename T, typename S >
struct traits< std::pair< T, S > >
{
    /// visit
    template < typename K, typename V > static void visit( const K& key, std::pair< T, S >& t, V& v )
    {
        v.apply( "first", t.first );
        v.apply( "second", t.second );
    }
    
    /// visit const
    template < typename K, typename V > static void visit( const K&, const std::pair< T, S >& t, V& v )
    {
        v.apply( "first", t.first );
        v.apply( "second", t.second );
    }    
};

namespace Impl {

template < typename K, typename V, typename Visitor >
inline void visit_non_associative_container( const K&, const V& c, Visitor& v )
{
	std::size_t index = 0;
    for( typename V::const_iterator it = c.begin(); it != c.end(); ++it, ++index )
    {
        v.apply( index, *it );
    }
}

template < typename K, typename V, typename Visitor >
inline void visit_non_associative_container( const K&, V& c, Visitor& v )
{
    std::size_t index = 0;
    for( typename V::iterator it = c.begin(); it != c.end(); ++it, ++index )
    {
        v.apply( index, *it );
    }
}

template < typename K, typename M, typename Visitor >
inline void visit_associative_container( const K&, M& c, Visitor& v )
{
    for( typename M::iterator it = c.begin(); it != c.end(); ++it )
    {
        v.apply( it->first.c_str(), it->second ); // quick and dirty: get rid of c_str()
    }
}

template < typename K, typename M, typename Visitor >
inline void visit_associative_container( const K&, const M& c, Visitor& v )
{
    for( typename M::const_iterator it = c.begin(); it != c.end(); ++it )
    {
        v.apply( it->first.c_str(), it->second ); // quick and dirty: get rid of c_str()
    }
}

} // namespace Impl {

/// vector visiting traits
template < typename T, typename A >
struct traits< std::vector< T, A > >
{
    /// visit
    template < typename K, typename V > static void visit( const K& key, std::vector< T, A >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }
    
    /// visit const
    template < typename K, typename V > static void visit( const K& key, const std::vector< T, A >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }    
};

/// set visiting traits
template < typename T >
struct traits< std::set< T > >
{
    /// visit
    template < typename K, typename V > static void visit( const K& key, std::set< T >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }
    
    /// visit const
    template < typename K, typename V > static void visit( const K& key, const std::set< T >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }    
};

/// boost array visiting traits
template < typename T, std::size_t S >
struct traits< boost::array< T, S > >
{
    /// visit
    template < typename K, typename V > static void visit( const K& key, boost::array< T, S >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }
    
    /// visit const
    template < typename K, typename V > static void visit( const K& key, const boost::array< T, S >& t, V& v )
    {
        Impl::visit_non_associative_container( key, t, v );
    }    
};

/// set visiting traits
template < typename T, typename S >
struct traits< std::map< T, S > >
{
    /// visit
    template < typename K, typename V > static void visit( const K& key, std::map< T, S >& t, V& v )
    {
        Impl::visit_associative_container( key, t, v );
    }
    
    /// visit const
    template < typename K, typename V > static void visit( const K& key, const std::map< T, S >& t, V& v )
    {
        Impl::visit_associative_container( key, t, v );
    }    
};

/// @todo add more types as needed

} } // namespace comma { namespace visiting {

#endif // COMMA_VISITING_TRAITS_HEADER_GUARD_
