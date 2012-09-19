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
#include <comma/math/interval.h>

namespace comma { namespace math {

TEST( interval, basics )
{
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( 1 ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( 2 ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( 3 ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( 4 ) );
    EXPECT_TRUE( !interval< int >( 1, 5 ).contains( 0 ) );
    EXPECT_TRUE( !interval< int >( 1, 5 ).contains( 5 ) );
    EXPECT_TRUE( interval< int >( -1, 2 ).contains( -1 ) );
    EXPECT_TRUE( interval< int >( -1, 2 ).contains( 0 ) );
    EXPECT_TRUE( interval< int >( -1, 2 ).contains( 1 ) );
    EXPECT_TRUE( !interval< int >( -1, 2 ).contains( -2 ) );
    EXPECT_TRUE( !interval< int >( -1, 2 ).contains( 2 ) );
    EXPECT_TRUE( interval< int >( -1, 0 ).contains( -1 ) );
    EXPECT_TRUE( !interval< int >( -1, 0 ).contains( -2 ) );
    EXPECT_TRUE( !interval< int >( -1, 0 ).contains( 0 ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( interval< int >( 1, 5 ) ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( interval< int >( 1, 4 ) ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( interval< int >( 2, 5 ) ) );
    EXPECT_TRUE( interval< int >( 1, 5 ).contains( interval< int >( 2, 4 ) ) );
    EXPECT_TRUE( !interval< int >( 1, 5 ).contains( interval< int >( 0, 5 ) ) );
    EXPECT_TRUE( !interval< int >( 1, 5 ).contains( interval< int >( 1, 6 ) ) );
    EXPECT_EQ( interval< int >( 1, 5 ), interval< int >( 1, 5 ) );
    EXPECT_TRUE( interval< int >( 1, 5 ) != interval< int >( 1, 6 ) );
}

template < typename T >
static void test_interval_mod()
{
    EXPECT_EQ( mod( T( -182 ), interval< T >( -180, 180 ) ), T( 178 ) );
    EXPECT_EQ( mod( T( -181 ), interval< T >( -180, 180 ) ), T( 179 ) );
    EXPECT_EQ( mod( T( -180 ), interval< T >( -180, 180 ) ), T( -180 ) );
    EXPECT_EQ( mod( T( -179 ), interval< T >( -180, 180 ) ), T( -179 ) );
    EXPECT_EQ( mod( T( -1 ), interval< T >( -180, 180 ) ), T( -1 ) );
    EXPECT_EQ( mod( T( 0 ), interval< T >( -180, 180 ) ), T( 0 ) );
    EXPECT_EQ( mod( T( 1 ), interval< T >( -180, 180 ) ), T( 1 ) );
    EXPECT_EQ( mod( T( 179 ), interval< T >( -180, 180 ) ), T( 179 ) );
    EXPECT_EQ( mod( T( 180 ), interval< T >( -180, 180 ) ), T( -180 ) );
    EXPECT_EQ( mod( T( 181 ), interval< T >( -180, 180 ) ), T( -179 ) );
    EXPECT_EQ( mod( T( -361 ), interval< T >( 0, 360 ) ), T( 359 ) );
    EXPECT_EQ( mod( T( -1 ), interval< T >( 0, 360 ) ), T( 359 ) );
    EXPECT_EQ( mod( T( 0 ), interval< T >( 0, 360 ) ), T( 0 ) );
    EXPECT_EQ( mod( T( 1 ), interval< T >( 0, 360 ) ), T( 1 ) );
    EXPECT_EQ( mod( T( 359 ), interval< T >( 0, 360 ) ), T( 359 ) );
    EXPECT_EQ( mod( T( 360 ), interval< T >( 0, 360 ) ), T( 0 ) );
    EXPECT_EQ( mod( T( 361 ), interval< T >( 0, 360 ) ), T( 1 ) );
}

} } // namespace comma { namespace math {

TEST( interval, mod )
{
    comma::math::test_interval_mod< int >();
    comma::math::test_interval_mod< double >();
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

