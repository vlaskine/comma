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

#ifndef COMMA_CSV_IMPL_STATICCAST_HEADER_GUARD_
#define COMMA_CSV_IMPL_STATICCAST_HEADER_GUARD_

#include <string.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace comma { namespace csv { namespace impl {

template < typename T > struct static_cast_impl
{
    static const T value( const std::string& s ) { COMMA_THROW( comma::exception, "cannot cast string " << s << " to given type" ); }
    static const T value( const boost::posix_time::ptime& s ) { COMMA_THROW( comma::exception, "cannot cast time " << boost::posix_time::to_iso_string( s ) << " to given type" ); }
    static const T& value( const T& t ) { return t; }
    template < typename S > static T value( const S& s ) { return static_cast< T >( s ); }
};

template <> struct static_cast_impl< std::string >
{
    static const std::string& value( const std::string& t ) { return t; }
    template < typename S > static std::string value( const S& s ) { COMMA_THROW( comma::exception, "cannot cast " << s << " to string" ); }
};

template <> struct static_cast_impl< boost::posix_time::ptime >
{
    static const boost::posix_time::ptime& value( const boost::posix_time::ptime& t ) { return t; }
    template < typename S > static boost::posix_time::ptime value( const S& s ) { COMMA_THROW( comma::exception, "cannot cast " << s << " to time" ); }
};

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_STATICCAST_HEADER_GUARD_
