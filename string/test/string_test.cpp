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

#include <comma/string/string.h>
#include <gtest/gtest.h>

namespace comma {

TEST( string, split )
{
    {
        std::vector< std::string > v( split( "" ) );
        EXPECT_TRUE( v.size() == 1 );
        EXPECT_TRUE( v[0] == "" );
    }
    {
        std::vector< std::string > v( split( ":::", ":" ) );
        EXPECT_TRUE( v.size() == 4 );
        for( unsigned int i = 0; i < 4; ++i ) { EXPECT_TRUE( v[i] == "" ); }
    }
    {
        std::vector< std::string > v( split( "hello:world::moon", ":" ) );
        EXPECT_TRUE( v.size() == 4 );
        EXPECT_TRUE( v[0] == "hello" );
        EXPECT_TRUE( v[1] == "world" );
        EXPECT_TRUE( v[2] == "" );
        EXPECT_TRUE( v[3] == "moon" );
    }
    {
        {
            std::vector< std::string > v( split( "hello:world:/moon", ":/" ) );
            EXPECT_TRUE( v.size() == 4 );
            EXPECT_TRUE( v[0] == "hello" );
            EXPECT_TRUE( v[1] == "world" );
            EXPECT_TRUE( v[2] == "" );
            EXPECT_TRUE( v[3] == "moon" );
        }
        {
            std::vector< std::string > v( split( "hello:world:/moon", "/:" ) );
            EXPECT_TRUE( v.size() == 4 );
            EXPECT_TRUE( v[0] == "hello" );
            EXPECT_TRUE( v[1] == "world" );
            EXPECT_TRUE( v[2] == "" );
            EXPECT_TRUE( v[3] == "moon" );
        }
    }
    {
        std::vector< std::string > v( split( ":,:", ":," ) );
        EXPECT_TRUE( v.size() == 4 );
        for( unsigned int i = 0; i < 4; ++i ) { EXPECT_TRUE( v[i] == "" ); }
    }
}

TEST( string, strip )
{
    EXPECT_EQ( strip( "", ";" ), "" );
    EXPECT_EQ( strip( ";", ";" ), "" );
    EXPECT_EQ( strip( ";;", ";" ), "" );
    EXPECT_EQ( strip( ";;;abc", ";" ), "abc" );
    EXPECT_EQ( strip( "abc;;;", ";" ), "abc" );
    EXPECT_EQ( strip( "a;bc;;;", ";" ), "a;bc" );
    EXPECT_EQ( strip( ";;;abc;;;", ";" ), "abc" );
    EXPECT_EQ( strip( ";,;abc;;,", ";," ), "abc" );
}

} // namespace comma {

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
