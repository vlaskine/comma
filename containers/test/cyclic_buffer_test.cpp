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

#include <gtest/gtest.h>
#include <comma/containers/cyclic_buffer.h>

namespace comma {

TEST( cyclic_buffer, basics )
{    
    cyclic_buffer< int > b( 5 );
    EXPECT_TRUE( b.empty() );
    EXPECT_EQ( b.size(), 0u );
    b.push( 4 );
    b.front() = 5;
    EXPECT_TRUE( !b.empty() );
    EXPECT_EQ( b.size(), 1u );
    EXPECT_EQ( b.front(), 5 );
    EXPECT_EQ( b.back(), 5 );
    b.push( 6 );
    EXPECT_TRUE( !b.empty() );
    EXPECT_EQ( b.size(), 2u );
    EXPECT_EQ( b.front(), 5 );
    EXPECT_EQ( b.back(), 6 );
    b.pop();
    EXPECT_TRUE( !b.empty() );
    EXPECT_EQ( b.size(), 1u );
    EXPECT_EQ( b.front(), 6 );
    EXPECT_EQ( b.back(), 6 );
    b.pop();
    EXPECT_TRUE( b.empty() );
    EXPECT_EQ( b.size(), 0u );
    b.pop();
    EXPECT_TRUE( b.empty() );
    EXPECT_EQ( b.size(), 0u );
}

TEST( cyclic_buffer, push_pop )
{
    cyclic_buffer< unsigned int > b( 5 );
    for( unsigned int i = 0; i < 5u; ++i )
    {
        EXPECT_EQ( b.size(), i );
        b.push( i );
        EXPECT_EQ( b.size(), i + 1 );
        EXPECT_EQ( b.back(), i );
    }
    try { b.push( 0 ); EXPECT_TRUE( false ); } catch ( ... ) {}
    EXPECT_EQ( b.size(), 5u );
    for( unsigned int i = 0; i < 5u; ++i )
    {
        EXPECT_EQ( b.size(), 5 - i );
        EXPECT_EQ( b.front(), i );
        b.pop();
        EXPECT_EQ( b.size(), 4 - i );
    }
    EXPECT_TRUE( b.empty() );
    EXPECT_EQ( b.size(), 0u );    
    
    for( unsigned int i = 0; i < 5u; ++i )
    {
        b.push( i );
    }    
    b.pop(4);
    EXPECT_EQ( b.size(), 1u );    
}

TEST( cyclic_buffer, fixed_cyclic_buffer )
{
    fixed_cyclic_buffer< unsigned int, 3 > b;
    EXPECT_EQ( b.size(), 3u );
    b[0] = 0;
    b[1] = 1;
    b[2] = 2;
    EXPECT_EQ( b[0], 0u );
    EXPECT_EQ( b[1], 1u );
    EXPECT_EQ( b[2], 2u );
    b >> 1;
    EXPECT_EQ( b[0], 1u );
    EXPECT_EQ( b[1], 2u );
    EXPECT_EQ( b[2], 0u );
    b >> 2;
    EXPECT_EQ( b[0], 0u );
    EXPECT_EQ( b[1], 1u );
    EXPECT_EQ( b[2], 2u );
    b << 1;
    EXPECT_EQ( b[0], 2u );
    EXPECT_EQ( b[1], 0u );
    EXPECT_EQ( b[2], 1u );    
    b << 3;
    EXPECT_EQ( b[0], 2u );
    EXPECT_EQ( b[1], 0u );
    EXPECT_EQ( b[2], 1u );    
}

} // namespace comma {

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
