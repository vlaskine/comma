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
#include <boost/array.hpp>
#include <gtest/gtest.h>
#include <comma/packed/packed.h>

struct packed_t : public comma::packed::packed_struct< packed_t, 16 >
{
    comma::packed::string< 4 > hello;
    comma::packed::string< 5 > world;
    comma::packed::net_uint16 int16;
    comma::packed::net_uint32 int32;
    comma::packed::byte byte;
};

// static void packed_test()
// {
//     packed_t simple;
//     TEST_CHECK( simple.hello == packed::string< 4 >::default_value() );
//     TEST_CHECK( simple.world == packed::string< 5 >::default_value() );
//     TEST_CHECK( simple.world != std::string( "blah" ) );
//     simple.world = "WORLD";
//     TEST_CHECK( simple.world == std::string( "WORLD" ) );
//     TEST_CHECK( simple.int16 == 0 );
//     TEST_CHECK( simple.int32 == 0 );
//     simple.int16 = 5;
//     simple.int32 = 25;
//     simple.byte = 10;
//     TEST_CHECK( simple.int16 == 5 );
//     TEST_CHECK( simple.int32 == 25 );
//     TEST_CHECK( simple.byte == 10 );
//     TEST_CHECK( simple.int16() == 5 );
//     TEST_CHECK( simple.int32() == 25 );
//     TEST_CHECK( simple.byte() == 10 );
// }
// 
// static void testNetuint()
// {
//     {
//         packed::Netuint16 a;
//         TEST_CHECK( a == 0 );
//         a = 9876;
//         TEST_CHECK( a == 9876 );
//         TEST_CHECK( a() == 9876 );
//     }
//     {
//         packed::Netuint32 a;
//         TEST_CHECK( a == 0 );
//         a = 9876;
//         TEST_CHECK( a == 9876 );
//         TEST_CHECK( a() == 9876 );
//     }
// }
// 
// static void testInt24( int value, char byte0, char byte1, char byte2 )
// {
//     packed::Int24 a;
//     a = value;
//     TEST_CHECK_EQUAL( ( 0xff & a.data()[0] ), ( 0xff & byte0 ) );
//     TEST_CHECK_EQUAL( ( 0xff & a.data()[1] ), ( 0xff & byte1 ) );
//     TEST_CHECK_EQUAL( ( 0xff & a.data()[2] ), ( 0xff & byte2 ) );
// }
// 
// static void testInt24()
// {
//     testInt24( 0, 0x00, 0x00, 0x00 );
//     testInt24( 1, 0x01, 0x00, 0x00 );
//     testInt24( 2, 0x02, 0x00, 0x00 );
//     testInt24( 32767, 0xff, 0x7f, 0x00 );
//     testInt24( 8388607, 0xff, 0xff, 0x7f );
//     testInt24( -1, 0xff, 0xff, 0xff );
//     testInt24( -2, 0xfe, 0xff, 0xff );
//     testInt24( -32767, 0x01, 0x80, 0xff );
//     testInt24( -32768, 0x00, 0x80, 0xff );
//     testInt24( -8388607, 0x01, 0x00, 0x80 );
//     testInt24( -8388608, 0x00, 0x00, 0x80 );
// }

template < typename T >
void test_little_endian( int value )
{
    T t;
    EXPECT_EQ( t == 0, true );
    t = value;
    EXPECT_EQ( t == value, true );
    EXPECT_EQ( t(), value );
}

TEST( packed_test, test_little_endian )
{
    test_little_endian< comma::packed::uint16 >( 1234 );
}
//     {
//         packed::uint16 a;
//         TEST_CHECK( a == 0 );
//         a = 9876;
//         TEST_CHECK( a == 9876 );
//         TEST_CHECK_EQUAL( a(), 9876 );
//     }
//     {
//         packed::uint24 a;
//         TEST_CHECK( a == 0 );
//         a = 9876;
//         TEST_CHECK( a == 9876 );
//         TEST_CHECK_EQUAL( a(), 9876u );
//     }
//     {
//         packed::uint32 a;
//         TEST_CHECK( a == 0 );
//         a = 9876;
//         TEST_CHECK( a == 9876 );
//         TEST_CHECK_EQUAL( a(), 9876u );
//     }
//     {
//         packed::Int16 a;
//         TEST_CHECK( a == 0 );
//         a = 1234;
//         TEST_CHECK( a == 1234 );
//         TEST_CHECK_EQUAL( a(), 1234 );
//         a = -1234;
//         TEST_CHECK( a == -1234 );
//         TEST_CHECK_EQUAL( a(), -1234 );
//     }
//     {
//         packed::Int24 a;
//         TEST_CHECK( a == 0 );
//         a = 1234;
//         TEST_CHECK( a == 1234 );
//         TEST_CHECK_EQUAL( a(), 1234 );
//         a = -1234;
//         TEST_CHECK( a == -1234 );
//         TEST_CHECK_EQUAL( a(), -1234 );
//     }
//     {
//         packed::Int32 a;
//         TEST_CHECK( a == 0 );
//         a = 1234;
//         TEST_CHECK( a == 1234 );
//         TEST_CHECK_EQUAL( a(), 1234 );
//         a = -1234;
//         TEST_CHECK( a == -1234 );
//         TEST_CHECK_EQUAL( a(), -1234 );
//     }

int main( int argc, char *argv[] )
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
