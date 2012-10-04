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

#ifndef COMMA_CSV_IMPL_TONAMES_H_
#define COMMA_CSV_IMPL_TONAMES_H_

#include <sstream>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <comma/visiting/traits.h>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>
#include <comma/xpath/xpath.h>

namespace comma { namespace csv { namespace impl {

/// visitor converting a structure into comma-separated xpaths
/// @todo implement handling vectors, arrays, etc
class to_names
{
    public:
        /// constructor
        to_names( bool full_path_as_name = true );
        
        /// constructor
        to_names( const xpath& root, bool full_path_as_name );
        
        /// traverse
        template < typename K, typename T >
        void apply( const K& name, const boost::optional< T >& value );
        
        /// traverse
        template < typename K, typename T >
        void apply( const K& name, const boost::scoped_ptr< T >& value );
        
        /// traverse
        template < typename K, typename T >
        void apply( const K& name, const boost::shared_ptr< T >& value );
        
        /// traverse
        template < typename K, typename T >
        void apply( const K& name, const T& value );
        
        /// traverse
        template < typename K, typename T >
        void apply_next( const K& name, const T& value );
        
        /// output a non-string type
        template < typename K, typename T >
        void apply_final( const K& name, const T& value );
        
        /// return string
        const std::vector< std::string >& operator()() const;
        
    private:
        bool full_path_as_name_;
        xpath xpath_;
        xpath root_;
        boost::optional< std::size_t > index_;
        std::vector< std::string > names_;
        const xpath& append( std::size_t index ) { xpath_.elements.back().index = index; return xpath_; }
        const xpath& append( const char* name ) { xpath_ /= xpath::element( name ); return xpath_; }
        const xpath& trim( std::size_t ) { xpath_.elements.back().index = boost::optional< std::size_t >(); return xpath_; }
        const xpath& trim( const char* ) { xpath_ = xpath_.head(); return xpath_; }
};

inline to_names::to_names( bool full_path_as_name ) : full_path_as_name_( full_path_as_name ) {}

inline to_names::to_names( const xpath& root, bool full_path_as_name ) : full_path_as_name_( full_path_as_name ), root_( root ) {}

template < typename K, typename T >
inline void to_names::apply( const K& name, const boost::optional< T >& value )
{
    if( value ) { apply( name, *value ); } else { T v; apply( name, v ); }
}

template < typename K, typename T >
inline void to_names::apply( const K& name, const boost::scoped_ptr< T >& value )
{
    if( value ) { apply( name, *value ); } else { T v; apply( name, v ); }
}

template < typename K, typename T >
inline void to_names::apply( const K& name, const boost::shared_ptr< T >& value )
{
    if( value ) { apply( name, *value ); } else { T v; apply( name, v ); }
}

template < typename K, typename T >
inline void to_names::apply( const K& name, const T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                     && !boost::is_same< T, std::string >::value
                     && !boost::is_same< T, boost::posix_time::ptime >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void to_names::apply_next( const K& name, const T& value )
{
    append( name );
    comma::visiting::visit( name, value, *this );
    trim( name );
}

template < typename K, typename T >
inline void to_names::apply_final( const K& name, const T& )
{
    append( name );
    if( xpath_ <= root_ ) { names_.push_back( full_path_as_name_ ? xpath_.to_string() : xpath_.elements.back().to_string() ); }
    trim( name );
}

inline const std::vector< std::string >& to_names::operator()() const { return names_; }

} } } // namespace comma { namespace csv { namespace impl {

#endif // COMMA_CSV_IMPL_TONAMES_H_
