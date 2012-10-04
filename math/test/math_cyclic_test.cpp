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
#include <comma/math/cyclic.h>

namespace comma { namespace math {

TEST( cyclic, comparison )
{
    interval< int > i( 5, 10 );
    EXPECT_EQ( cyclic< int >( i ), cyclic< int >( i ) );
    EXPECT_EQ( cyclic< int >( i, 7 ), cyclic< int >( i, 7 ) );
    EXPECT_TRUE( cyclic< int >( i ) != cyclic< int >( interval< int >( 6, 10 ) ) );
    EXPECT_TRUE( cyclic< int >( i, 7 ) != cyclic< int >( interval< int >( 6, 10 ), 7 ) );
    EXPECT_TRUE( cyclic< int >( i, 7 ).between( cyclic< int >( i, 7 ), cyclic< int >( i, 8 ) ) );
    EXPECT_TRUE( cyclic< int >( i, 7 ).between( cyclic< int >( i, 6 ), cyclic< int >( i, 8 ) ) );
    EXPECT_TRUE( cyclic< int >( i, 7 ).between( cyclic< int >( i, 6 ), cyclic< int >( i, 5 ) ) );
    EXPECT_TRUE( cyclic< int >( i, 7 ).between( cyclic< int >( i, 9 ), cyclic< int >( i, 8 ) ) );
    EXPECT_TRUE( !cyclic< int >( i, 7 ).between( cyclic< int >( i, 7 ), cyclic< int >( i, 7 ) ) );
    EXPECT_TRUE( !cyclic< int >( i, 7 ).between( cyclic< int >( i, 9 ), cyclic< int >( i, 7 ) ) );
    EXPECT_TRUE( !cyclic< int >( i, 7 ).between( cyclic< int >( i, 8 ), cyclic< int >( i, 6 ) ) );
    EXPECT_TRUE( !cyclic< int >( i, 7 ).between( cyclic< int >( i, 6 ), cyclic< int >( i, 7 ) ) );
}


TEST( cyclic, increment_decrement )
{
    cyclic< int > a( interval< int >( 5, 10 ) );
    EXPECT_EQ( a(), 5 );
    EXPECT_EQ( ( ++a )(), 6 );
    EXPECT_EQ( ( ++a )(), 7 );
    EXPECT_EQ( ( ++a )(), 8 );
    EXPECT_EQ( ( ++a )(), 9 );
    EXPECT_EQ( ( ++a )(), 5 );
    EXPECT_EQ( ( ++a )(), 6 );
    EXPECT_EQ( ( --a )(), 5 );
    EXPECT_EQ( ( --a )(), 9 );
    EXPECT_EQ( ( --a )(), 8 );
    EXPECT_EQ( ( --a )(), 7 );
    EXPECT_EQ( ( --a )(), 6 );
    EXPECT_EQ( ( --a )(), 5 );
}

TEST( cyclic, addition_subtraction )
{
    {
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) )(), 5 );
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) + 2 )(), 7 );
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) + 7 )(), 7 );
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) - 0 )(), 5 );
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) - 2 )(), 8 );
        EXPECT_EQ( ( cyclic< int >( 5, 10 ) - 7 )(), 8 );
        EXPECT_EQ( ( ( cyclic< int >( 5, 10 ) + 3 ) - 2 )(), 6 );
    }    
    {
        cyclic< int > a( interval< int >( 5, 10 ) );
        cyclic< int > b( interval< int >( 5, 10 ) );
        EXPECT_EQ( a + b, a );
        EXPECT_EQ( b + a, a );
        EXPECT_EQ( b + b + b + b, b );
        EXPECT_EQ( a - b, a );
        EXPECT_EQ( b - b - b - b, b );
        a = 7;
        EXPECT_EQ( a + b, a );
        EXPECT_EQ( b + a, a );
        EXPECT_EQ( a - b, a );
        EXPECT_EQ( a - 20, a );
    }
    {
        cyclic< int > a( 0, 4 );
        cyclic< int > b( 0, 4 );
        EXPECT_EQ( ( a - b )(), 0 );
        EXPECT_EQ( ( ( a + 1 ) - b )(), 1 );
        EXPECT_EQ( ( ( a + 2 ) - b )(), 2 );
        EXPECT_EQ( ( ( a + 3 ) - b )(), 3 );
        EXPECT_EQ( ( ( a + 4 ) - b )(), 0 );
        EXPECT_EQ( ( a - ( b + 1 ) )(), 3 );
        EXPECT_EQ( ( a - ( b + 2 ) )(), 2 );
        EXPECT_EQ( ( a - ( b + 3 ) )(), 1 );
        EXPECT_EQ( ( a - ( b + 4 ) )(), 0 );
        EXPECT_EQ( ( ( a + 1 ) - ( b + 1 ) )(), 0 );
        EXPECT_EQ( ( ( a + 1 ) - ( b + 2 ) )(), 3 );
        EXPECT_EQ( ( ( a + 1 ) - ( b + 3 ) )(), 2 );
        EXPECT_EQ( ( ( a + 1 ) - ( b + 4 ) )(), 1 );
    }
    {
        cyclic< int > a( 0, 5 );
        cyclic< int > b( 0, 5 );
        EXPECT_EQ( ( b - 1 )(), 4 );
        EXPECT_EQ( ( b - ( a + 1 ) )(), 4 );
        EXPECT_EQ( ( b - ( ++a ) )(), 4 );
        EXPECT_EQ( ( b - cyclic< int >( interval< int >( 0, 5 ), 1 ) )(), 4 );
    }
    {
        cyclic< unsigned int > a( 0, 5 );
        cyclic< unsigned int > b( 0, 5 );
        EXPECT_EQ( ( b - 1 )(), 4u );
        EXPECT_EQ( ( b - ( a + 1 ) )(), 4u );
        EXPECT_EQ( ( b - ( ++a ) )(), 4u );
    }
}

} } // namespace comma { namespace math {
