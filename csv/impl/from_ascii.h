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

#ifndef COMMA_CSV_IMPL_FROMASCII_HEADER_GUARD_
#define COMMA_CSV_IMPL_FROMASCII_HEADER_GUARD_

#include <deque>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <comma/base/exception.h>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>

namespace comma { namespace csv { namespace impl {

/// visitor loading a struct from a csv file
/// see unit test for usage
class from_ascii_
{
    public:
        /// constructor
        from_ascii_( const std::vector< boost::optional< std::size_t > >& indices
                  , const std::deque< bool >& optional
                  , const std::vector< std::string >& line );
        
        /// apply
        template < typename K, typename T > void apply( const K& name, boost::optional< T >& value );
        
        /// apply
        template < typename K, typename T > void apply( const K& name, boost::scoped_ptr< T >& value );
        
        /// apply
        template < typename K, typename T > void apply( const K& name, boost::shared_ptr< T >& value );
        
        /// apply
        template < typename K, typename T > void apply( const K& name, T& value );
        
        /// apply to non-leaf elements
        template < typename K, typename T > void apply_next( const K& name, T& value );
        
        /// apply to leaf elements
        template < typename K, typename T > void apply_final( const K& name, T& value );
        
    private:
        const std::vector< boost::optional< std::size_t > >& indices_;
        const std::deque< bool >& optional_;
        const std::vector< std::string >& row_;
        std::size_t index_;
        std::size_t optional_index;
        static void lexical_cast_( char& v, const std::string& s ) { v = s.at( 0 ) == '\'' && s.at( 2 ) == '\'' && s.length() == 3 ? s.at( 1 ) : static_cast< char >( boost::lexical_cast< int >( s ) ); }
        static void lexical_cast_( unsigned char& v, const std::string& s ) { v = s.at( 0 ) == '\'' && s.at( 2 ) == '\'' && s.length() == 3 ? s.at( 1 ) : static_cast< unsigned char >( boost::lexical_cast< unsigned int >( s ) ); }
        static void lexical_cast_( boost::posix_time::ptime& v, const std::string& s ) { v = boost::posix_time::from_iso_string( s ); }
        static void lexical_cast_( std::string& v, const std::string& s ) { v = comma::strip( s, "\"" ); }
        static void lexical_cast_( bool& v, const std::string& s ) { v = static_cast< bool >( boost::lexical_cast< unsigned int >( s ) ); }
        template < typename T >
        static void lexical_cast_( T& v, const std::string& s ) { v = boost::lexical_cast< T >( s ); }        
};

inline from_ascii_::from_ascii_( const std::vector< boost::optional< std::size_t > >& indices
                           , const std::deque< bool >& optional
                           , const std::vector< std::string >& line )
    : indices_( indices )
    , optional_( optional )
    , row_( line )
    , index_( 0 )
    , optional_index( 0 )
{
}

template < typename K, typename T >
inline void from_ascii_::apply( const K& name, boost::optional< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}

template < typename K, typename T >
inline void from_ascii_::apply( const K& name, boost::scoped_ptr< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}

template < typename K, typename T >
inline void from_ascii_::apply( const K& name, boost::shared_ptr< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}

template < typename K, typename T >
inline void from_ascii_::apply( const K& name, T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                        && !boost::is_same< T, std::string >::value
                        && !boost::is_same< T, boost::posix_time::ptime >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void from_ascii_::apply_next( const K& name, T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void from_ascii_::apply_final( const K& key, T& value )
{ 
	(void)key;
    if( indices_[ index_ ] )
    {
        std::size_t i = *indices_[ index_ ];
        if( i >= row_.size() ) { COMMA_THROW( comma::exception, "got column index " << i << ", for " << row_.size() << " column(s) in line: \"" << join( row_, ',' ) << "\"" ); }
        const std::string& s = row_[i];
        if( !s.empty() ) { lexical_cast_( value, s ); }
    }
    ++index_;
}

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_FROMASCII_HEADER_GUARD_
