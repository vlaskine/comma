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
#include <comma/xpath/xpath.h>
#include <comma/base/exception.h>

TEST( xpath, contruction )
{
    {
        comma::xpath x;
        EXPECT_EQ( x.to_string(), "" );
        comma::xpath y( "hello/world[1]/moon[2]/cloud[5]/bye" );
        EXPECT_EQ( y.to_string(), "hello/world[1]/moon[2]/cloud[5]/bye" );
        x = y;
        EXPECT_EQ( x.to_string(), y.to_string() );
        EXPECT_EQ( comma::xpath( y ).to_string(), "hello/world[1]/moon[2]/cloud[5]/bye" );
        EXPECT_THROW( comma::xpath( "hello[5" ), comma::exception );
        EXPECT_THROW( comma::xpath( "[5]" ), comma::exception );
        EXPECT_THROW( comma::xpath( "hello/[5]" ), comma::exception );
    }
    {
        EXPECT_EQ( comma::xpath(), comma::xpath( "" ) );
        EXPECT_EQ( comma::xpath( "/" ), comma::xpath( "///" ) );
        EXPECT_EQ( comma::xpath( "hello/" ), comma::xpath( "hello" ) );
    }
}

TEST( xpath, comparisons )
{
    EXPECT_TRUE( !( comma::xpath() < comma::xpath() ) );
    EXPECT_TRUE( !( comma::xpath() < comma::xpath( "/" ) ) );
    EXPECT_TRUE( !( comma::xpath( "/" ) < comma::xpath( "/" ) ) );
    EXPECT_TRUE( !( comma::xpath() < comma::xpath( "hello" ) ) );
    EXPECT_TRUE( !( comma::xpath( "hello" ) < comma::xpath( "world" ) ) );
    EXPECT_TRUE( !( comma::xpath( "world" ) < comma::xpath( "hello" ) ) );
    EXPECT_TRUE( !( comma::xpath( "hello" ) < comma::xpath( "hello[10]" ) ) );
    EXPECT_TRUE( !( comma::xpath( "hello[10]/world" ) < comma::xpath( "hello/world" ) ) );
    EXPECT_TRUE( !( comma::xpath( "hello/world[0]" ) < comma::xpath( "hello/world[1]" ) ) );
    EXPECT_TRUE( !( comma::xpath( "world/cloud[7]/hello[10]" ) < comma::xpath( "world/cloud[8]/hello" ) ) );
    EXPECT_TRUE( !( comma::xpath( "world/cloud[7]/hello" ) < comma::xpath( "world/cloud/hello" ) ) );
    
    EXPECT_TRUE( comma::xpath( "/" ) < comma::xpath() );
    EXPECT_TRUE( comma::xpath( "hello" ) < comma::xpath() );
    EXPECT_TRUE( comma::xpath( "hello[10]" ) < comma::xpath( "hello" ) );
    EXPECT_TRUE( comma::xpath( "/hello[10]" ) < comma::xpath( "/hello" ) );
    EXPECT_TRUE( comma::xpath( "hello[10]/x" ) < comma::xpath( "hello" ) );
    EXPECT_TRUE( comma::xpath( "world/cloud[7]/hello[10]" ) < comma::xpath( "world/cloud[7]/hello" ) );
}

TEST( xpath, concatenation )
{
    EXPECT_EQ( comma::xpath() / comma::xpath(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "hello" ) / comma::xpath(), comma::xpath( "hello" ) );
    EXPECT_EQ( comma::xpath() / comma::xpath( "hello" ), comma::xpath( "hello" ) );
    EXPECT_EQ( comma::xpath( "/" ) / comma::xpath(), comma::xpath( "/" ) );
    EXPECT_EQ( comma::xpath( "/" ) / comma::xpath( "/" ), comma::xpath( "/" ) );
    EXPECT_EQ( comma::xpath( "hello" ) / comma::xpath( "world" ), comma::xpath( "hello/world" ) );
    EXPECT_EQ( comma::xpath( "hello" ) / comma::xpath( "/world" ), comma::xpath( "hello/world" ) );
    EXPECT_EQ( comma::xpath( "hello[1]" ) / comma::xpath( "world[2]" ), comma::xpath( "hello[1]/world[2]" ) );
}

TEST( xpath, tail )
{
    EXPECT_EQ( comma::xpath().tail(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "/" ).tail(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "hello" ).tail(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "/hello" ).tail(), comma::xpath( "hello" ) );
    EXPECT_EQ( comma::xpath( "/hello[5]" ).tail(), comma::xpath( "hello[5]" ) );
    EXPECT_EQ( comma::xpath( "hello[5]/world" ).tail(), comma::xpath( "world" ) );
    EXPECT_EQ( comma::xpath( "/hello[5]/world" ).tail(), comma::xpath( "hello[5]/world" ) );
}

TEST( xpath, head )
{
    EXPECT_EQ( comma::xpath().head(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "/" ).head(), comma::xpath( "/" ) );
    EXPECT_EQ( comma::xpath( "hello" ).head(), comma::xpath() );
    EXPECT_EQ( comma::xpath( "/hello" ).head(), comma::xpath( "/" ) );
    EXPECT_EQ( comma::xpath( "hello/world" ).head(), comma::xpath( "hello" ) );
    EXPECT_EQ( comma::xpath( "/hello/world" ).head(), comma::xpath( "/hello" ) );
    EXPECT_EQ( comma::xpath( "hello[1]/world[2]" ).head(), comma::xpath( "hello[1]" ) );
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
