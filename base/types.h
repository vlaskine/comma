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

#ifndef COMMA_BASE_TYPES_H_
#define COMMA_BASE_TYPES_H_

#if defined(__linux__) || defined(__APPLE__) || defined(__QNXNTO__)
#include <arpa/inet.h>
#elif defined(WIN32)
#if defined(WINCE)
#include <Winsock.h>
#else
#include <Winsock2.h>
#endif
#endif

#include <cmath>
#include <limits>
#include <boost/static_assert.hpp>

namespace comma {

#if defined(__linux__) || defined(__APPLE__) || defined(__QNXNTO__)

typedef uint16_t u_int_16;
typedef uint32_t u_int_32;
typedef uint64_t u_int_64;

typedef int16_t int_16;
typedef int32_t int_32;
typedef int64_t int_64;

#elif defined(WIN32)

typedef unsigned __int16 u_int_16;
typedef unsigned __int32 u_int_32;
typedef unsigned __int64 u_int_64;

typedef __int16 int_16;
typedef __int32 int_32;
typedef __int64 int_64;

// Windows, you know...
BOOST_STATIC_ASSERT( sizeof( u_int_16 ) == 2 );
BOOST_STATIC_ASSERT( sizeof( u_int_32 ) == 4 );
BOOST_STATIC_ASSERT( sizeof( u_int_64 ) == 8 );
BOOST_STATIC_ASSERT( sizeof( int_16 ) == 2 );
BOOST_STATIC_ASSERT( sizeof( int_32 ) == 4 );
BOOST_STATIC_ASSERT( sizeof( int_64 ) == 8 );

#endif

} // namespace comma {

#endif /*COMMA_BASE_TYPES_H_*/
