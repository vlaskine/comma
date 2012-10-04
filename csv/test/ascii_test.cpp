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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <comma/csv/ascii.h>
#include <comma/string/string.h>

namespace comma { namespace csv { namespace ascii_test {

struct nested
{
    int x;
    int y;
    nested() : x( 0 ), y( 0 ) {}
};

struct simple_struct
{
    int a;
    double b;
    char c;
    std::string s;
    boost::posix_time::ptime t;
    ascii_test::nested nested;
    simple_struct() : a( 0 ), b( 0 ), c( 0 ) {}
};

struct test_struct
{
    int a;
    boost::optional< int > z;
    boost::optional< ascii_test::nested > nested;
};

struct containers
{
    boost::array< int, 4 > array;
};

} } } // namespace comma { namespace csv { namespace ascii_test {

namespace comma { namespace visiting {

template <> struct traits< comma::csv::ascii_test::nested >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::ascii_test::nested& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::ascii_test::nested& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }
};

template <> struct traits< comma::csv::ascii_test::simple_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::ascii_test::simple_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        v.apply( "s", p.s );
        v.apply( "t", p.t );
        v.apply( "nested", p.nested );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::ascii_test::simple_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        v.apply( "s", p.s );
        v.apply( "t", p.t );
        v.apply( "nested", p.nested );
    }
};

template <> struct traits< comma::csv::ascii_test::test_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::ascii_test::test_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "z", p.z );
        v.apply( "nested", p.nested );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::ascii_test::test_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "z", p.z );
        v.apply( "nested", p.nested );
    }
};

template <> struct traits< comma::csv::ascii_test::containers >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::ascii_test::containers& p, Visitor& v )
    {
        v.apply( "array", p.array );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::ascii_test::containers& p, Visitor& v )
    {
        v.apply( "array", p.array );
    }
};

} } // namespace comma { namespace visiting {

TEST( csv, ascii_get )
{
    {
        comma::csv::ascii_test::simple_struct s;
        EXPECT_EQ( comma::join( comma::csv::names( s ), ',' ), "a,b,c,s,t,nested/x,nested/y" );
        comma::csv::ascii< comma::csv::ascii_test::simple_struct > ascii;
        ascii.get( s, "1,2,'c',hello,20110304T111111.1234,5,6" );
        EXPECT_EQ( s.a, 1 );
        EXPECT_EQ( s.b, 2 );
        EXPECT_EQ( s.c, 'c' );
        EXPECT_EQ( s.s, "hello" );
        EXPECT_EQ( s.t, boost::posix_time::from_iso_string( "20110304T111111.1234" ) );
        EXPECT_EQ( s.nested.x, 5 );
        EXPECT_EQ( s.nested.y, 6 );
    }
    {
        comma::csv::ascii_test::simple_struct s;
        comma::csv::ascii< comma::csv::ascii_test::simple_struct > ascii( ",,,," );
        EXPECT_EQ( comma::join( comma::csv::names< comma::csv::ascii_test::simple_struct >( ",,,," ), ',' ), ",,,," );
        ascii.get( s, "1,2,'c',\"hello\",20110304T111111.1234,5,6" );
        EXPECT_EQ( s.a, 0 );
        EXPECT_EQ( s.b, 0 );
        EXPECT_EQ( s.c, 0 );
        EXPECT_EQ( s.s, "" );
        EXPECT_EQ( s.t, boost::posix_time::not_a_date_time );
        EXPECT_EQ( s.nested.x, 0 );
        EXPECT_EQ( s.nested.y, 0 );
    }
    {
        comma::csv::ascii_test::simple_struct s;
        comma::csv::ascii< comma::csv::ascii_test::simple_struct > ascii( ",,,,t,," );
        ascii.get( s, "1,2,'c',\"hello\",20110304T111111.1234,5,6" );
        EXPECT_EQ( s.a, 0 );
        EXPECT_EQ( s.b, 0 );
        EXPECT_EQ( s.c, 0 );
        EXPECT_EQ( s.s, "" );
        EXPECT_EQ( s.t, boost::posix_time::from_iso_string( "20110304T111111.1234" ) );
        EXPECT_EQ( s.nested.x, 0 );
        EXPECT_EQ( s.nested.y, 0 );
    }
    // todo: more testing
}

TEST( csv, ascii_put )
{
    // todo
}

TEST( csv, ascii_optional_element )
{
    {
        comma::csv::ascii_test::test_struct s;
        EXPECT_EQ( comma::join( comma::csv::names( s ), ',' ), "a,z,nested/x,nested/y" );
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii;
        ascii.get( s, "1,2,3,4" ); // TODO fails on windows
        EXPECT_TRUE( s.z );
        EXPECT_TRUE( s.nested );
        EXPECT_EQ( s.a, 1 );
        EXPECT_EQ( *s.z, 2 );
        EXPECT_EQ( s.nested->x, 3 );
        EXPECT_EQ( s.nested->y, 4 );
    }
    {
        comma::csv::ascii_test::test_struct s;
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii( ",,z" );
        ascii.get( s, "1,2,3" );
        EXPECT_TRUE( s.z );
        EXPECT_TRUE( !s.nested );
        EXPECT_EQ( *s.z, 3 );
    }
    {
        comma::csv::ascii_test::test_struct s;
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii( "nested,z" );
        ascii.get( s, "1,2,3" );
        EXPECT_TRUE( s.z );
        EXPECT_TRUE( s.nested );
        EXPECT_EQ( *s.z, 3 );
        EXPECT_EQ( s.nested->x, 1 );
        EXPECT_EQ( s.nested->y, 2 );
    }
    {
        comma::csv::ascii_test::test_struct s;
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii( ",nested" );
        ascii.get( s, "1,2,3" );
        EXPECT_TRUE( !s.z );
        EXPECT_TRUE( s.nested );
        EXPECT_EQ( s.nested->x, 2 );
        EXPECT_EQ( s.nested->y, 3 );
    }
    {
        comma::csv::ascii_test::test_struct s;
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii( "nested,,a" );
        ascii.get( s, "1,2,3,4" );
        EXPECT_TRUE( !s.z );
        EXPECT_TRUE( s.nested );
        EXPECT_EQ( s.nested->x, 1 );
        EXPECT_EQ( s.nested->y, 2 );
        EXPECT_EQ( s.a, 4 );
    }
    {
        comma::csv::ascii_test::test_struct s;
        comma::csv::ascii< comma::csv::ascii_test::test_struct > ascii( ",x,y,a", ',', false );
        ascii.get( s, "1,2,3,4" );
        EXPECT_TRUE( !s.z );
        EXPECT_TRUE( s.nested );
        EXPECT_EQ( s.nested->x, 2 );
        EXPECT_EQ( s.nested->y, 3 );
        EXPECT_EQ( s.a, 4 );
    }
    // todo: more testing
}

TEST( csv, ascii_containers )
{
    {
        comma::csv::ascii_test::containers c;
        for( unsigned int i = 0; i < c.array.size(); ++i ) { c.array[i] = i; }
        comma::csv::ascii< comma::csv::ascii_test::containers > ascii;
        {
            std::string s;
            ascii.put( c, s );
            EXPECT_EQ( s, "0,1,2,3" );
        }        
        {
            std::string s = "6,7,8,9";
            ascii.get( c, s );
            EXPECT_EQ( c.array[0], 6 );
            EXPECT_EQ( c.array[1], 7 );
            EXPECT_EQ( c.array[2], 8 );
            EXPECT_EQ( c.array[3], 9 );
        }
    }
    // todo: more tests
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
