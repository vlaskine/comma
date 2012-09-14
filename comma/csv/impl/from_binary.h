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

#ifndef COMMA_CSV_IMPL_FROMBINARY_HEADER_GUARD_
#define COMMA_CSV_IMPL_FROMBINARY_HEADER_GUARD_

#include <string.h>
#include <deque>
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
class frobinary_
{
    public:
        /// constructor
        frobinary_( const std::vector< boost::optional< format::element > >& offsets
                  , const std::deque< bool >& optional
                  , const char* buf );
        
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
        const std::vector< boost::optional< format::element > >& offsets_;
        const std::deque< bool >& optional_;
        std::size_t optional_index;
        const char* buf_;
        std::size_t index_;
//         static void copy( boost::posix_time::ptime& v, const char* buf, std::size_t )
//         {
//             int64 seconds;
//             int32 nanoseconds;
//             ::memcpy( &seconds, buf, sizeof( int64 ) );
//             ::memcpy( &nanoseconds, buf + sizeof( int64 ), sizeof( int32 ) );
//             v = boost::posix_time::ptime(Timing::epoch, boost::posix_time::seconds( static_cast< long >( seconds ) ) + boost::posix_time::microseconds(nanoseconds/1000));
//         }
//         static void copy( std::string& v, const char* buf, std::size_t size ) // currently only for fixed size string
//         {
//             v = buf[ size - 1 ] == 0 ? std::string( buf ) : std::string( buf, size );
//         }
//         template < typename T >
//         static void copy( T& v, const char* buf, std::size_t size ) { ::memcpy( &v, buf, size ); }
};

inline frobinary_::frobinary_( const std::vector< boost::optional< format::element > >& offsets
                               , const std::deque< bool >& optional
                               , const char* buf )
    : offsets_( offsets )
    , optional_( optional )
    , optional_index( 0 )
    , buf_( buf )
    , index_( 0 )
{
}

template < typename K, typename T >
inline void frobinary_::apply( const K& name, boost::optional< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}

template < typename K, typename T >
inline void frobinary_::apply( const K& name, boost::scoped_ptr< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}

template < typename K, typename T >
inline void frobinary_::apply( const K& name, boost::shared_ptr< T >& value ) // todo: watch performance
{
    if( !value && optional_[optional_index++] ) { value = T(); }
    if( value ) { this->apply( name, *value ); }
    else { ++index_; }
}
                             
template < typename K, typename T >
inline void frobinary_::apply( const K& name, T& value )
{
    visiting::do_while<    !boost::is_fundamental< T >::value
                        && !boost::is_same< T, std::string >::value
                        && !boost::is_same< T, boost::posix_time::ptime >::value >::visit( name, value, *this );
}

template < typename K, typename T >
inline void frobinary_::apply_next( const K& name, T& value ) { comma::visiting::visit( name, value, *this ); }

template < typename K, typename T >
inline void frobinary_::apply_final( const K&, T& value )
{
    //if( offsets_[ index_ ] ) { copy( value, buf_ + offsets_[ index_ ]->offset, offsets_[ index_ ]->size ); }
    if( offsets_[ index_ ] )
    {
        const char* buf = buf_ + offsets_[ index_ ]->offset;
        std::size_t size = offsets_[ index_ ]->size;
        format::types_enum type = offsets_[ index_ ]->type;
        if( type == format::traits< T >::type ) // quick path
        {
            value = format::traits< T >::from_bin( buf, size ); // copy( value, buf, size );
        }
        else
        {
            switch( type )
            {
                case format::int8: value = static_cast_impl< T >::value( format::traits< char >::from_bin( buf ) ); break;
                case format::uint8: value = static_cast_impl< T >::value( format::traits< unsigned char >::from_bin( buf ) ); break;
                case format::int16: value = static_cast_impl< T >::value( format::traits< comma::int16 >::from_bin( buf ) ); break;
                case format::uint16: value = static_cast_impl< T >::value( format::traits< comma::uint16 >::from_bin( buf ) ); break;
                case format::int32: value = static_cast_impl< T >::value( format::traits< comma::int32 >::from_bin( buf ) ); break;
                case format::uint32: value = static_cast_impl< T >::value( format::traits< comma::uint32 >::from_bin( buf ) ); break;
                case format::int64: value = static_cast_impl< T >::value( format::traits< comma::int64 >::from_bin( buf ) ); break;
                case format::uint64: value = static_cast_impl< T >::value( format::traits< comma::uint64 >::from_bin( buf ) ); break;
                case format::char_t: value = static_cast_impl< T >::value( format::traits< char >::from_bin( buf ) ); break;
                case format::float_t: value = static_cast_impl< T >::value( format::traits< float >::from_bin( buf ) ); break;
                case format::double_t: value = static_cast_impl< T >::value( format::traits< double >::from_bin( buf ) ); break;
                case format::time: value = static_cast_impl< T >::value( format::traits< boost::posix_time::ptime, format::time >::from_bin( buf ) ); break;
                case format::long_time: value = static_cast_impl< T >::value( format::traits< boost::posix_time::ptime, format::long_time >::from_bin( buf ) ); break;
                case format::fixed_string: value = static_cast_impl< T >::value( format::traits< std::string >::from_bin( buf, size ) ); break;
            };
        }
    }
    ++index_;
}

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_FROMBINARY_HEADER_GUARD_
