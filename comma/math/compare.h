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

#ifndef COMMA_MATH_COMPARE_H_
#define COMMA_MATH_COMPARE_H_

#include <cmath>
#include <limits>

namespace comma { namespace math {

template < typename S > inline bool equal( float lhs, S rhs ) { return std::fabs( lhs - rhs ) < std::numeric_limits< float >::epsilon(); }
template < typename S > inline bool less( float lhs, S rhs ) { return static_cast< float >( rhs ) - lhs >= std::numeric_limits< float >::epsilon(); }

template < typename S > inline bool equal( double lhs, S rhs ) { return std::fabs( lhs - rhs ) < std::numeric_limits< double >::epsilon(); }
template < typename S > inline bool less( double lhs, S rhs ) { return static_cast< double >( rhs ) - lhs >= std::numeric_limits< double >::epsilon(); }

template < typename S > inline bool equal( long double lhs, S rhs ) { return std::fabs( lhs - rhs ) < std::numeric_limits< long double >::epsilon(); }
template < typename S > inline bool less( long double lhs, S rhs ) { return static_cast< long double >( rhs ) - lhs >= std::numeric_limits< long double >::epsilon(); }

template < typename T, typename S > inline bool equal( const T& lhs, const S& rhs ) { return lhs == rhs; }
template < typename T, typename S > inline bool less( const T& lhs, const S& rhs ) { return lhs < rhs; }

template < typename T, typename S, typename Diff > inline bool less( const T& lhs, const S& rhs, const Diff& epsilon ) { return less( lhs + epsilon, rhs ); }
template < typename T, typename S, typename Diff > inline bool equal( const T& lhs, const S& rhs, const Diff& epsilon ) { return !less( lhs, rhs, epsilon ) && !less( static_cast< T >( rhs ), lhs, epsilon ); }

} } // namespace comma { namespace math {

#endif // COMMA_MATH_COMPARE_H_
