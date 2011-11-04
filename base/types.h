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

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#elif defined(WIN32)

typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef __int16 int_16;
typedef __int32 int_32;
typedef __int64 int_64;

// Windows, you know...
BOOST_STATIC_ASSERT( sizeof( uint16 ) == 2 );
BOOST_STATIC_ASSERT( sizeof( uint32 ) == 4 );
BOOST_STATIC_ASSERT( sizeof( uint64 ) == 8 );
BOOST_STATIC_ASSERT( sizeof( int16 ) == 2 );
BOOST_STATIC_ASSERT( sizeof( int32 ) == 4 );
BOOST_STATIC_ASSERT( sizeof( int64 ) == 8 );

#endif

} // namespace comma {

#endif /*COMMA_BASE_TYPES_H_*/
