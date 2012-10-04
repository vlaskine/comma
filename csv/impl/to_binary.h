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

#ifndef COMMA_CSV_IMPL_TOBINARY_HEADER_GUARD_
#define COMMA_CSV_IMPL_TOBINARY_HEADER_GUARD_

#include <string.h>
#include <vector>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <comma/base/types.h>
#include <comma/csv/format.h>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>
#include "./static_cast.h"

namespace comma { namespace csv { namespace impl {

/// visitor loading a struct from a csv file
/// see unit test for usage
class to_binary
{
    public:
        /// constructor
        to_binary( const std::vector< boost::optional< format::element > >& offsets, char* buf );
        
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
        
    private:
        const std::vector< boost::optional< format::element > >& offsets_;
        char* buf_;
        std::size_t index_;
//         static void copy( char* buf, const boost::posix_time::ptime& v, std::size_t )
//         {
//             static const boost::posix_time::ptime base( impl::epoch );
//             const boost::posix_time::time_duration posix = v - base;
//             int64 seconds = posix.total_seconds();
//             int32 nanoseconds = static_cast< int32 >( posix.total_microseconds() % 1000000 ) * 1000;
//             ::memcpy( buf, &seconds, sizeof( int64 ) );
//             ::memcpy( buf + sizeof( int64 ), &nanoseconds, sizeof( int32 ) );
//         }
//         static void copy( char* buf, const std::string& v, std::size_t size ) // currently only for fixed size string
//         {
//             if( v.length() > size ) { COMMA_THROW( comma::exception, "expected string not longer than " << size << ", got " << v.length() << " bytes (\"" << v << "\")" ); }
//             ::memset( buf, 0, size );
//             if( v.empty() ) { return; }
//             ::memcpy( buf, &v[0], v.length() );
//         }
//         template < typename T >
//         static void copy( char* buf, T v, std::size_t size ) { ::memcpy( buf, &v, size ); }
};

inline to_binary::to_binary( const std::vector< boost::optional< format::element > >& offsets, char* buf )
    : offsets_( offsets )
    , buf_( buf )
    , index_( 0 )
{
}

template < typename K, typename T >
inline void to_binary::apply( const K& name, const boost::optional< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_binary::apply( const K& name, const boost::scoped_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_binary::apply( const K& name, const boost::shared_ptr< T >& value )
{
    if( value ) { apply( name, *value ); }
}

template < typename K, typename T >
inline void to_binary::apply( const K& name, const T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                     && !boost::is_same< T, std::string >::value
                     && !boost::is_same< T, boost::posix_time::ptime >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void to_binary::apply_next( const K& name, const T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void to_binary::apply_final( const K&, const T& value )
{
    //if( offsets_[ index_ ] ) { copy( buf_ + offsets_[ index_ ]->offset, value, offsets_[ index_ ]->size ); }
    if( offsets_[ index_ ] )
    {
        char* buf = buf_ + offsets_[ index_ ]->offset;
        std::size_t size = offsets_[ index_ ]->size;
        format::types_enum type = offsets_[ index_ ]->type;
        if( type == format::traits< T >::type ) // quick path
        {
            format::traits< T >::to_bin( value, buf, size ); //copy( buf, value, size );
        }
        else
        {
            switch( type )
            {
                case format::int8: format::traits< char >::to_bin( static_cast_impl< char >::value( value ), buf ); break;
                case format::uint8: format::traits< unsigned char >::to_bin( static_cast_impl< unsigned char >::value( value ), buf ); break;
                case format::int16: format::traits< comma::int16 >::to_bin( static_cast_impl< comma::int16 >::value( value ), buf ); break;
                case format::uint16: format::traits< comma::uint16 >::to_bin( static_cast_impl< comma::uint16 >::value( value ), buf ); break;
                case format::int32: format::traits< comma::int32 >::to_bin( static_cast_impl< comma::int32 >::value( value ), buf ); break;
                case format::uint32: format::traits< comma::int32 >::to_bin( static_cast_impl< comma::uint32 >::value( value ), buf ); break;
                case format::int64: format::traits< comma::int64 >::to_bin( static_cast_impl< comma::int64 >::value( value ), buf ); break;
                case format::uint64: format::traits< comma::uint64 >::to_bin( static_cast_impl< comma::uint64 >::value( value ), buf ); break;
                case format::char_t: format::traits< char >::to_bin( static_cast_impl< char >::value( value ), buf ); break;
                case format::float_t: format::traits< float >::to_bin( static_cast_impl< float >::value( value ), buf ); break;
                case format::double_t: format::traits< double >::to_bin( static_cast_impl< double >::value( value ), buf ); break;
                case format::time: format::traits< boost::posix_time::ptime, format::time >::to_bin( static_cast_impl< boost::posix_time::ptime >::value( value ), buf ); break;
                case format::long_time: format::traits< boost::posix_time::ptime, format::long_time >::to_bin( static_cast_impl< boost::posix_time::ptime >::value( value ), buf ); break;
                case format::fixed_string: format::traits< std::string >::to_bin( static_cast_impl< std::string >::value( value ), buf, size ); break;
            };
        }
    }
    ++index_;
}

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_TOBINARY_HEADER_GUARD_
