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

#include <gtest/gtest.h>
#include <comma/eigen/visiting.h>
// #include <comma/csv/format.h>
// #include <comma/csv/binary.h>

namespace comma {
namespace eigen {

TEST( Eigen, serialization )
{
    {
//         ::Eigen::Vector2d v( 1, 2 );
//         csv::format f( "%d%d" );
//         csv::binary< ::Eigen::Vector2d > binary;
//         std::string s;
//         s.resize( 2*sizeof(double) );
//         binary.put( v, &s[0] );
//         EXPECT_EQ( f.binTocsv( s ), "1,2" );
// 
//         ::Eigen::Vector2d u( 0, 0 );
//         std::string bin = f.csvToBin( "-1,-2" );
//         binary.get( u, &bin[0] );
//         EXPECT_TRUE( u.isApprox(::Eigen::Vector2d(-1,-2) ) );
    }

    {
//         ::Eigen::Vector3d v( 1, 2, 3 );
//         csv::format f( "%d%d%d" );
//         csv::binary< ::Eigen::Vector3d > binary;
//         std::string s;
//         s.resize( 3*sizeof(double) );
//         binary.put( v, &s[0] );
//         EXPECT_EQ( f.binTocsv( s ), "1,2,3" );
// 
//         ::Eigen::Vector3d u;
//         std::string bin = f.csvToBin( "-1,-2,-3" );
//         binary.get( u, &bin[0] );
//         EXPECT_TRUE( u.isApprox(::Eigen::Vector3d(-1,-2,-3) ) );
    }
}

} }

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
