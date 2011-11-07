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
#include <comma/x_path/x_path.h>
#include <comma/base/exception.h>

TEST( x_path, contruction )
{
    {
        comma::x_path x;
        EXPECT_EQ( x.to_string(), "" );
        comma::x_path y( "hello/world[1]/moon[2]/cloud[5]/bye" );
        EXPECT_EQ( y.to_string(), "hello/world[1]/moon[2]/cloud[5]/bye" );
        x = y;
        EXPECT_EQ( x.to_string(), y.to_string() );
        EXPECT_EQ( comma::x_path( y ).to_string(), "hello/world[1]/moon[2]/cloud[5]/bye" );
        EXPECT_THROW( comma::x_path( "hello[5" ), comma::exception );
        EXPECT_THROW( comma::x_path( "[5]" ), comma::exception );
        EXPECT_THROW( comma::x_path( "hello/[5]" ), comma::exception );
    }
    {
        EXPECT_EQ( comma::x_path(), comma::x_path( "" ) );
        EXPECT_EQ( comma::x_path( "/" ), comma::x_path( "///" ) );
        EXPECT_EQ( comma::x_path( "hello/" ), comma::x_path( "hello" ) );
    }
}

TEST( x_path, comparisons )
{
    EXPECT_TRUE( !( comma::x_path() < comma::x_path() ) );
    EXPECT_TRUE( !( comma::x_path() < comma::x_path( "/" ) ) );
    EXPECT_TRUE( !( comma::x_path( "/" ) < comma::x_path( "/" ) ) );
    EXPECT_TRUE( !( comma::x_path() < comma::x_path( "hello" ) ) );
    EXPECT_TRUE( !( comma::x_path( "hello" ) < comma::x_path( "world" ) ) );
    EXPECT_TRUE( !( comma::x_path( "world" ) < comma::x_path( "hello" ) ) );
    EXPECT_TRUE( !( comma::x_path( "hello" ) < comma::x_path( "hello[10]" ) ) );
    EXPECT_TRUE( !( comma::x_path( "hello[10]/world" ) < comma::x_path( "hello/world" ) ) );
    EXPECT_TRUE( !( comma::x_path( "hello/world[0]" ) < comma::x_path( "hello/world[1]" ) ) );
    EXPECT_TRUE( !( comma::x_path( "world/cloud[7]/hello[10]" ) < comma::x_path( "world/cloud[8]/hello" ) ) );
    EXPECT_TRUE( !( comma::x_path( "world/cloud[7]/hello" ) < comma::x_path( "world/cloud/hello" ) ) );
    
    EXPECT_TRUE( comma::x_path( "/" ) < comma::x_path() );
    EXPECT_TRUE( comma::x_path( "hello" ) < comma::x_path() );
    EXPECT_TRUE( comma::x_path( "hello[10]" ) < comma::x_path( "hello" ) );
    EXPECT_TRUE( comma::x_path( "/hello[10]" ) < comma::x_path( "/hello" ) );
    EXPECT_TRUE( comma::x_path( "hello[10]/x" ) < comma::x_path( "hello" ) );
    EXPECT_TRUE( comma::x_path( "world/cloud[7]/hello[10]" ) < comma::x_path( "world/cloud[7]/hello" ) );
}

TEST( x_path, concatenation )
{
    EXPECT_EQ( comma::x_path() / comma::x_path(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "hello" ) / comma::x_path(), comma::x_path( "hello" ) );
    EXPECT_EQ( comma::x_path() / comma::x_path( "hello" ), comma::x_path( "hello" ) );
    EXPECT_EQ( comma::x_path( "/" ) / comma::x_path(), comma::x_path( "/" ) );
    EXPECT_EQ( comma::x_path( "/" ) / comma::x_path( "/" ), comma::x_path( "/" ) );
    EXPECT_EQ( comma::x_path( "hello" ) / comma::x_path( "world" ), comma::x_path( "hello/world" ) );
    EXPECT_EQ( comma::x_path( "hello" ) / comma::x_path( "/world" ), comma::x_path( "hello/world" ) );
    EXPECT_EQ( comma::x_path( "hello[1]" ) / comma::x_path( "world[2]" ), comma::x_path( "hello[1]/world[2]" ) );
}

TEST( x_path, tail )
{
    EXPECT_EQ( comma::x_path().tail(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "/" ).tail(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "hello" ).tail(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "/hello" ).tail(), comma::x_path( "hello" ) );
    EXPECT_EQ( comma::x_path( "/hello[5]" ).tail(), comma::x_path( "hello[5]" ) );
    EXPECT_EQ( comma::x_path( "hello[5]/world" ).tail(), comma::x_path( "world" ) );
    EXPECT_EQ( comma::x_path( "/hello[5]/world" ).tail(), comma::x_path( "hello[5]/world" ) );
}

TEST( x_path, head )
{
    EXPECT_EQ( comma::x_path().head(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "/" ).head(), comma::x_path( "/" ) );
    EXPECT_EQ( comma::x_path( "hello" ).head(), comma::x_path() );
    EXPECT_EQ( comma::x_path( "/hello" ).head(), comma::x_path( "/" ) );
    EXPECT_EQ( comma::x_path( "hello/world" ).head(), comma::x_path( "hello" ) );
    EXPECT_EQ( comma::x_path( "/hello/world" ).head(), comma::x_path( "/hello" ) );
    EXPECT_EQ( comma::x_path( "hello[1]/world[2]" ).head(), comma::x_path( "hello[1]" ) );
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
