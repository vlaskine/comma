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

#ifndef COMMA_CSV_BINARY_HEADER_GUARD_
#define COMMA_CSV_BINARY_HEADER_GUARD_

#include <boost/optional.hpp>
#include <comma/csv/names.h>
#include <comma/csv/options.h>
#include <comma/csv/impl/binary_visitor.h>
#include <comma/csv/impl/from_binary.h>
#include <comma/csv/impl/to_binary.h>
#include <comma/string/string.h>

namespace comma { namespace csv {

template < typename S >
class binary
{
    public:
        /// constructor
        binary( const std::string& f = "", const std::string& column_names = "", bool full_path_as_name = true, const S& sample = S() );
        
        /// constructor from options
        binary( const options& o, const S& sample = S() );
        
        /// get value (returns reference pointing to the parameter) 
        const S& get( S& s, const char* buf ) const;
        
        /// put value at the right place in the vector
        char* put( const S& s, char* buf ) const;
        
        /// return format
        const csv::format& format() const { return format_; }
        
    private:
        const csv::format format_;
        boost::optional< impl::binary_visitor > binary_;
};

template < typename S >
inline binary< S >::binary( const std::string& f, const std::string& column_names, bool full_path_as_name, const S& sample )
    : format_( f == "" ? csv::format::value( sample ) : f )
{
    if( format_.size() == sizeof( S ) && format_.string() == csv::format::value( sample ) && join( csv::names( column_names, full_path_as_name, sample ), ',' ) == join( csv::names( full_path_as_name ), ',' ) ) { return; }
    binary_ = impl::binary_visitor( format_, join( csv::names( column_names, full_path_as_name, sample ), ',' ), full_path_as_name );
    visiting::apply( *binary_, sample );
}

template < typename S >
inline binary< S >::binary( const options& o, const S& sample )
    : format_( o.format().string() == "" ? csv::format::value( sample ) : o.format().string() )
{
    if( format_.size() == sizeof( S ) && format_.string() == csv::format::value( sample ) && join( csv::names( o.fields, o.full_xpath, sample ), ',' ) == join( csv::names( o.full_xpath ), ',' ) ) { return; }
    binary_ = impl::binary_visitor( format_, join( csv::names( o.fields, o.full_xpath, sample ), ',' ), o.full_xpath );
    visiting::apply( *binary_, sample );
}

template < typename S >
inline const S& binary< S >::get( S& s, const char* buf ) const
{
    if( binary_ )
    {
        impl::frobinary_ f( binary_->offsets(), binary_->optional(), buf );
        visiting::apply( f, s );
    }
    else // quick and dirty for better performance
    {
        ::memcpy( reinterpret_cast< char* >( &s ), buf, sizeof( S ) );
    }
    return s;
}

template < typename S >
inline char* binary< S >::put( const S& s, char* buf ) const
{
    if( binary_ )
    {
        impl::to_binary f( binary_->offsets(), buf );
        visiting::apply( f, s );
    }
    else // quick and dirty for better performance
    {
        ::memcpy( buf, reinterpret_cast< const char* >( &s ), sizeof( S ) );
    }
    return buf;
}

} } // namespace comma { namespace csv {

#endif // #ifndef COMMA_CSV_BINARY_HEADER_GUARD_
