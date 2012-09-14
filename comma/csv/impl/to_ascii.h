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

#ifndef COMMA_CSV_IMPL_TOASCII_HEADER_GUARD_
#define COMMA_CSV_IMPL_TOASCII_HEADER_GUARD_

#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>

namespace comma { namespace csv { namespace impl {

/// visitor loading a struct from a csv file
/// see unit test for usage
class to_ascii
{
    public:
        /// constructor
        to_ascii( const std::vector< boost::optional< std::size_t > >& indices, std::vector< std::string >& line );

        /// apply
        template < typename K, typename T > void apply( const K& name, const boost::optional< T >& value );

        /// apply
        template < typename K, typename T > void apply( const K& name, const boost::scoped_ptr< T >& value );

        /// apply
        template < typename K, typename T > void apply( const K& name, const boost::shared_ptr< T >& value );

        /// apply
        template < typename K, typename T >
        void apply( const K& name, const T& value );

        /// apply to non-leaf elements
        template < typename K, typename T >
        void apply_next( const K& name, const T& value );

        /// apply to leaf elements
        template < typename K, typename T >
        void apply_final( const K& name, const T& value );

        /// set precision
        void precision( unsigned int p ) { precision_ = p; }

    private:
        const std::vector< boost::optional< std::size_t > >& indices_;
        std::vector< std::string >& row_;
        std::size_t index_;
        boost::optional< unsigned int > precision_;
        std::string as_string_( const boost::posix_time::ptime& v ) { return to_iso_string( v ); }
        std::string as_string_( const std::string& v ) { return std::string( "\"" ) + v + "\""; } // todo: escape/unescape
        // todo: better output semantics for char/unsigned char
        std::string as_string_( const char& v ) { std::ostringstream oss; oss << static_cast< int >( v ); return oss.str(); }
        std::string as_string_( const unsigned char& v ) { std::ostringstream oss; oss << static_cast< unsigned int >( v ); return oss.str(); }
        template < typename T >
        std::string as_string_( T v )
        {
            std::ostringstream oss;
            if( precision_ ) { oss.precision( *precision_ ); }
            oss << v;
            return oss.str();
        }
};

inline to_ascii::to_ascii( const std::vector< boost::optional< std::size_t > >& indices, std::vector< std::string >& line )
    : indices_( indices )
    , row_( line )
    , index_( 0 )
{
}

template < typename K, typename T >
inline void to_ascii::apply( const K& name, const boost::optional< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_ascii::apply( const K& name, const boost::scoped_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_ascii::apply( const K& name, const boost::shared_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_ascii::apply( const K& name, const T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                        && !boost::is_same< T, std::string >::value
                        && !boost::is_same< T, boost::posix_time::ptime >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void to_ascii::apply_next( const K& name, const T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void to_ascii::apply_final( const K&, const T& value )
{
    if( indices_[ index_ ] )
    {
        std::size_t i = *indices_[ index_ ];
        if( i >= row_.size() ) { COMMA_THROW( comma::exception, "got column index " << i << ", for " << row_.size() << " columns in row " << join( row_, ',' ) ); }
        row_[i] = as_string_( value );
    }
    ++index_;
}

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_TOASCII_HEADER_GUARD_
