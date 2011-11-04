// This file is part of comma library
//
// Copyright (c) Matthew Herrmann 2007
// Copyright (c) Vsevolod Vlaskine 2010-2011
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

#ifndef WIN32
#include <stdlib.h>
#endif
#include <math.h>
#include <iostream>
#include <gtest/gtest.h>
#include <comma/packed/packed.h>

struct test_packed_struct_t : public comma::packed::packed_struct< test_packed_struct_t, 16 >
{
    comma::packed::string< 4 > hello;
    comma::packed::string< 5 > world;
    comma::packed::net_uint16 int16;
    comma::packed::net_uint32 int32;
    comma::packed::byte byte;
};

TEST( packed_struct, test_packed_struct )
{
    test_packed_struct_t s;
    EXPECT_EQ( s.hello == comma::packed::string< 4 >::default_value(), true );
    EXPECT_EQ( s.world == comma::packed::string< 5 >::default_value(), true );
    EXPECT_EQ( s.hello(), comma::packed::string< 4 >::default_value() );
    EXPECT_EQ( s.world(), comma::packed::string< 5 >::default_value() );
    EXPECT_EQ( s.world != std::string( "blah" ), true );
    s.world = "WORLD";
    EXPECT_EQ( s.world == std::string( "WORLD" ), true );
    EXPECT_EQ( s.int16 == 0, true );
    EXPECT_EQ( s.int32 == 0, true );
    s.int16 = 1;
    s.int32 = 2;
    s.byte = 3;
    EXPECT_EQ( s.int16 == 1, true );
    EXPECT_EQ( s.int32 == 2, true );
    EXPECT_EQ( s.byte == 3, true );
    EXPECT_EQ( s.int16(), 1 );
    EXPECT_EQ( s.int32(), 2 );
    EXPECT_EQ( s.byte(), 3 );
}

template < typename T >
void test_packed_int( int value )
{
    T t;
    EXPECT_EQ( t == 0, true );
    t = value;
    EXPECT_EQ( t == value, true );
    EXPECT_EQ( t(), value );
}

TEST( test_packed_struct_test, test_little_endian )
{
    test_packed_int< comma::packed::uint16 >( 1234 );
    test_packed_int< comma::packed::uint24 >( 1234 );
    test_packed_int< comma::packed::uint32 >( 1234 );
    test_packed_int< comma::packed::int16 >( 1234 );
    test_packed_int< comma::packed::int24 >( 1234 );
    test_packed_int< comma::packed::int32 >( 1234 );
    test_packed_int< comma::packed::int16 >( -1234 );
    test_packed_int< comma::packed::int24 >( -1234 );
    test_packed_int< comma::packed::int32 >( -1234 );
}

TEST( test_packed_struct_test, test_big_endian )
{
    test_packed_int< comma::packed::net_uint16 >( 1234 );
    test_packed_int< comma::packed::net_uint32 >( 1234 );
}

static void test_int24_byte_order( int value, char byte0, char byte1, char byte2 )
{
    comma::packed::int24 a;
    a = value;
    EXPECT_EQ( ( 0xff & a.data()[0] ), ( 0xff & byte0 ) );
    EXPECT_EQ( ( 0xff & a.data()[1] ), ( 0xff & byte1 ) );
    EXPECT_EQ( ( 0xff & a.data()[2] ), ( 0xff & byte2 ) );
}

TEST( test_packed_struct_test, test_int24_byte_order )
{
    test_int24_byte_order( 0, 0x00, 0x00, 0x00 );
    test_int24_byte_order( 1, 0x01, 0x00, 0x00 );
    test_int24_byte_order( 2, 0x02, 0x00, 0x00 );
    test_int24_byte_order( 32767, 0xff, 0x7f, 0x00 );
    test_int24_byte_order( 8388607, 0xff, 0xff, 0x7f );
    test_int24_byte_order( -1, 0xff, 0xff, 0xff );
    test_int24_byte_order( -2, 0xfe, 0xff, 0xff );
    test_int24_byte_order( -32767, 0x01, 0x80, 0xff );
    test_int24_byte_order( -32768, 0x00, 0x80, 0xff );
    test_int24_byte_order( -8388607, 0x01, 0x00, 0x80 );
    test_int24_byte_order( -8388608, 0x00, 0x00, 0x80 );
}

int main( int argc, char *argv[] )
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
