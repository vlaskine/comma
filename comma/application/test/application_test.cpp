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
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>

TEST( application, command_line_options )
{
    std::vector< std::string > argv;
    argv.push_back( "hello" );
    argv.push_back( "-a" );
    argv.push_back( "b" );
    argv.push_back( "-c" );
    argv.push_back( "-d" );
    argv.push_back( "--a=true" );
    argv.push_back( "--b=15" );
    argv.push_back( "--c=hello" );
    argv.push_back( "--c" );
    argv.push_back( "world" );
    argv.push_back( "--help" );
    argv.push_back( "-h" );
    argv.push_back( "-s=5" );
    argv.push_back( "-t" );
    argv.push_back( "6" );
    comma::command_line_options options( argv );
    EXPECT_TRUE( options.exists( "-a" ) );
    EXPECT_TRUE( options.exists( "-c" ) );
    EXPECT_TRUE( options.exists( "-d" ) );
    EXPECT_TRUE( options.exists( "--a" ) );
    EXPECT_TRUE( options.exists( "--b" ) );
    EXPECT_TRUE( options.exists( "--c" ) );
    EXPECT_TRUE( options.exists( "--help" ) );
    EXPECT_TRUE( options.exists( "-h" ) );
    EXPECT_TRUE( !options.exists( "b" ) );
    EXPECT_TRUE( !options.exists( "-b" ) );
    EXPECT_TRUE( !options.exists( "--d" ) );
    EXPECT_TRUE( options.values< int >( "-x" ).empty() );
    EXPECT_TRUE( options.values< int >( "-x,-y,-z" ).empty() );
    EXPECT_EQ( options.optional< int >( "-x" ), boost::optional< int >() );
    EXPECT_EQ( options.optional< int >( "-x,-y,-z" ), boost::optional< int >() );
    EXPECT_EQ( options.value< std::string >( "-a" ), "b" );
    EXPECT_EQ( options.value< bool >( "--a" ), true );
    EXPECT_EQ( options.value< int >( "--b" ), 15 );
    EXPECT_EQ( options.values< std::string >( "--c" )[0], "hello" );
    EXPECT_EQ( options.values< std::string >( "--c" )[1], "world" );
    EXPECT_TRUE( options.exists( "-s" ) );
    EXPECT_TRUE( options.exists( "-t" ) );
    EXPECT_EQ( options.values< std::string >( "-s" ).size(), 1u );
    EXPECT_EQ( options.values< std::string >( "-t" ).size(), 1u );
    // TODO: definitely more tests!
}

TEST( application, unnamed )
{
    {
        std::vector< std::string > argv;
        argv.push_back( "hello" );
        argv.push_back( "free0" );
        argv.push_back( "-a" );
        argv.push_back( "free1" );
        argv.push_back( "-b" );
        argv.push_back( "-c" );
        argv.push_back( "--x" );
        argv.push_back( " hello" );
        argv.push_back( "free2" );
        argv.push_back( "--y=world" );
        argv.push_back( "free3" );
        argv.push_back( "--z" );
        argv.push_back( "blah" );
        argv.push_back( "free4" );
        argv.push_back( "free5" );
        comma::command_line_options options( argv );
        std::vector< std::string > free = options.unnamed( "-a,-b,-c", "--x,--y,--z" );
        EXPECT_EQ( free.size(), 6u );
        EXPECT_EQ( free[0], "free0" );
        EXPECT_EQ( free[1], "free1" );
        EXPECT_EQ( free[2], "free2" );
        EXPECT_EQ( free[3], "free3" );
        EXPECT_EQ( free[4], "free4" );
        EXPECT_EQ( free[5], "free5" );
    }
    // TODO: definitely more tests!
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
