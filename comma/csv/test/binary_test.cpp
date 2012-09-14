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
#include <comma/csv/binary.h>
#include <comma/csv/format.h>
#include <comma/string/string.h>

namespace comma { namespace csv { namespace binary_test {

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
    //std::string s;
    boost::posix_time::ptime t;
    binary_test::nested nested;
    simple_struct() : a( 0 ), b( 0 ), c( 0 ) {}
};

struct test_struct
{
    int a;
    boost::optional< int > z;
    boost::optional< binary_test::nested > nested;
};

struct large_struct
{
    large_struct( bool bo, int aa, int bb, int cc, unsigned int s, double al, double be, double ga, double de, const std::string& string1, const std::string& string2, unsigned int i ):
        boule( bo ), a( aa ), b( bb ), c( cc ), size( s ), alpha( al ), beta( be ), gamma( ga ), delta( de ), string1( string1 ), string2( string2 ), id( i ) {}
    large_struct() :
        boule( false ), a( 0 ), b( 0 ), c( 0 ), size( 0 ), alpha( 0 ), beta( 0 ), gamma( 0 ), delta( 0 ), id( 0 ) {}
    bool boule;
    int a;
    int b;
    int c;
    unsigned int size;
    double alpha;
    double beta;
    double gamma;
    double delta;
    std::string string1;
    std::string string2;
    unsigned int id;
};

struct containers
{
    boost::array< int, 4 > array;
};

} } } // namespace comma { namespace csv { namespace binary_test {

namespace comma { namespace visiting {

template <> struct traits< comma::csv::binary_test::nested >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::binary_test::nested& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }
    
    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::binary_test::nested& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }    
};

template <> struct traits< comma::csv::binary_test::simple_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::binary_test::simple_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        //v.apply( "s", p.s );
        v.apply( "t", p.t );
        v.apply( "nested", p.nested );
    }
    
    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::binary_test::simple_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        //v.apply( "s", p.s );
        v.apply( "t", p.t );
        v.apply( "nested", p.nested );
    }    
};

template <> struct traits< comma::csv::binary_test::test_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::binary_test::test_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "z", p.z );
        v.apply( "nested", p.nested );
    }
    
    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::binary_test::test_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "z", p.z );
        v.apply( "nested", p.nested );
    }    
};

template <> struct traits< comma::csv::binary_test::large_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::binary_test::large_struct& p, Visitor& v )
    {
        v.apply( "boule", p.boule );
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        v.apply( "size", p.size );
        v.apply( "alpha", p.alpha );
        v.apply( "beta", p.beta );
        v.apply( "gamma", p.gamma );
        v.apply( "delta", p.delta );
        v.apply( "string1", p.string1 );
        v.apply( "string2", p.string2 );
        v.apply( "id", p.id );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::binary_test::large_struct& p, Visitor& v )
    {
        v.apply( "boule", p.boule );
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        v.apply( "size", p.size );
        v.apply( "alpha", p.alpha );
        v.apply( "beta", p.beta );
        v.apply( "gamma", p.gamma );
        v.apply( "delta", p.delta );
        v.apply( "string1", p.string1 );
        v.apply( "string2", p.string2 );
        v.apply( "id", p.id );
    }
};

template <> struct traits< comma::csv::binary_test::containers >
{
    template < typename Key, class Visitor > static void visit( const Key&, const comma::csv::binary_test::containers& p, Visitor& v )
    {
        v.apply( "array", p.array );
    }

    template < typename Key, class Visitor > static void visit( const Key&, comma::csv::binary_test::containers& p, Visitor& v )
    {
        v.apply( "array", p.array );
    }
};

} } // namespace comma { namespace visiting {

TEST( csv, binary_get )
{
    {
        comma::csv::binary_test::simple_struct t;
        t.a = 1;
        t.b = 2;
        t.c = 'c';
        t.t = boost::posix_time::from_iso_string( "20110304T111111.1234" );
        t.nested.x = 5;
        t.nested.y = 6;
        char buf[1024];
        comma::csv::binary< comma::csv::binary_test::simple_struct > b("%i%d%b%t%2i");
        b.put( t, buf );
        {
            comma::csv::binary_test::simple_struct s;
            EXPECT_EQ( comma::join( comma::csv::names( s ), ',' ), "a,b,c,t,nested/x,nested/y" );
            comma::csv::binary< comma::csv::binary_test::simple_struct > binary("%i%d%b%t%2i");
            binary.get( s, buf );
            EXPECT_EQ( s.a, 1 );
            EXPECT_EQ( s.b, 2 );
            EXPECT_EQ( s.c, 'c' );
            EXPECT_EQ( s.t, boost::posix_time::from_iso_string( "20110304T111111.1234" ) );
            EXPECT_EQ( s.nested.x, 5 );
            EXPECT_EQ( s.nested.y, 6 );
        }
        {
            comma::csv::binary_test::simple_struct s;
            comma::csv::binary< comma::csv::binary_test::simple_struct > binary( "%i%d%c%t%2i", ",,,," );
            EXPECT_EQ( comma::join( comma::csv::names< comma::csv::binary_test::simple_struct >( ",,,," ), ',' ), ",,,," );
            binary.get( s, buf );
            EXPECT_EQ( s.a, 0 );
            EXPECT_EQ( s.b, 0 );
            EXPECT_EQ( s.c, 0 );
            EXPECT_EQ( s.t, boost::posix_time::not_a_date_time );
            EXPECT_EQ( s.nested.x, 0 );
            EXPECT_EQ( s.nested.y, 0 );
        }
        {
            comma::csv::binary_test::simple_struct s;
            comma::csv::binary< comma::csv::binary_test::simple_struct > binary( "%i%d%c%t%2i", ",,,t,," );
            binary.get( s, buf );
            EXPECT_EQ( s.a, 0 );
            EXPECT_EQ( s.b, 0 );
            EXPECT_EQ( s.c, 0 );
            EXPECT_EQ( s.t, boost::posix_time::from_iso_string( "20110304T111111.1234" ) );
            EXPECT_EQ( s.nested.x, 0 );
            EXPECT_EQ( s.nested.y, 0 );
        }
    }
    {
        comma::csv::binary_test::large_struct s( true, 1, 2, 3, 100, 1.1, 2.2, 3.3, 4.4, "hello", "world", 123 );
        comma::csv::binary< comma::csv::binary_test::large_struct > binary( "%b%3i%ui%4d%2s[8]%ui" );
        comma::csv::binary_test::large_struct t;
        char buf[1024];
        binary.put( s, buf );
        binary.get( t, buf );
        EXPECT_EQ( t.boule, s.boule );
        EXPECT_EQ( t.a, s.a );
        EXPECT_EQ( t.b, s.b );
        EXPECT_EQ( t.c, s.c );
        EXPECT_EQ( t.size, s.size );
        EXPECT_EQ( t.alpha, s.alpha );
        EXPECT_EQ( t.beta, s.beta );
        EXPECT_EQ( t.gamma, s.gamma );
        EXPECT_EQ( t.delta, s.delta );
        EXPECT_EQ( t.string1, s.string1 );
        EXPECT_EQ( t.string2, s.string2 );
        EXPECT_EQ( t.id, s.id );
    }    
    // todo: more testing
}

template < typename T > struct test_cast
{
    T value;
    test_cast( const T& t ) : value( t ) {}
    test_cast() : value() {}
};

namespace comma { namespace visiting {

template < typename T > struct traits< test_cast< T > >
{
    template < typename Key, class Visitor > static void visit( const Key&, const test_cast< T >& p, Visitor& v ) { v.apply( "value", p.value ); }
    template < typename Key, class Visitor > static void visit( const Key&, test_cast< T >& p, Visitor& v ) { v.apply( "value", p.value ); }
};

} } // namespace comma { namespace visiting {

template < typename T > struct test_traits
{
    template < typename S > static void expect_equal( T v, S w ) { EXPECT_EQ( v, w ); }
};

template <> struct test_traits< float >
{
    template < typename S > static void expect_equal( float v, S w ) { EXPECT_NEAR( v, w, 0.0001 ); }
};

template <> struct test_traits< double >
{
    template < typename S > static void expect_equal( double v, S w ) { EXPECT_NEAR( v, w, 0.0001 ); }
};
    
template < typename T, typename S >
static void test_binary_cast( const char* format, T t, S s, T ti )
{
    {
        comma::csv::binary< test_cast< T > > bt( format );
        comma::csv::binary< test_cast< S > > bs( format );
        {
            char buf[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }; // just to trash it
            test_cast< T > vt( t );
            test_cast< S > vs;
            bt.put( vt, buf );
            bs.get( vs, buf );
            test_traits< T >::expect_equal( vs.value, s );
        }
        {
            char buf[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }; // just to trash it
            test_cast< S > vs( s );
            test_cast< T > vt;
            bs.put( vs, buf );
            bt.get( vt, buf );
            test_traits< T >::expect_equal( vt.value, ti );
        }
    }
}

template < typename T >
static void test_binary_cast_throw( const char* format )
{
    comma::csv::binary< test_cast< T > > bt( format );
    char buf[32];
    test_cast< T > vt;
    TEST_THROW( bt.put( vt, buf ) );
    TEST_THROW( bt.get( vt, buf ) );
}

TEST( csv, binary_binary_cast )
{
    test_binary_cast< char, char >( "%c", 111, 111, 111 );
    test_binary_cast< char, unsigned char >( "%c", 111, 111, 111 );
    test_binary_cast< char, comma::int16 >( "%c", 111, 111, 111 );
    test_binary_cast< char, comma::uint16 >( "%c", 111, 111, 111 );
    test_binary_cast< char, comma::int32 >( "%c", 111, 111, 111 );
    test_binary_cast< char, comma::uint32 >( "%c", 111, 111, 111 );
    test_binary_cast< char, float >( "%c", 111, 111, 111 );
    test_binary_cast< char, double >( "%c", 111, 111, 111 );
    
    test_binary_cast< comma::uint32, char >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, unsigned char >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, comma::int16 >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, comma::uint16 >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, comma::int32 >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, comma::uint32 >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, float >( "%ui", 111, 111, 111 );
    test_binary_cast< comma::uint32, double >( "%ui", 111, 111, 111 );
    
    test_binary_cast< float, char >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, unsigned char >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, comma::int16 >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, comma::uint16 >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, comma::int32 >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, comma::uint32 >( "%f", 111.1f, 111, 111 );
    test_binary_cast< float, float >( "%f", 111.1f, 111.1f, 111.1f );
    //test_binary_cast< float, double >( "%f", 111.1, 111.1, 111.1 ); // todo

    test_binary_cast< double, char >( "%d", 111.1, 111, 111 );
    test_binary_cast< double, unsigned char >( "%d", 111.1, 111, 111 );
    test_binary_cast< double, comma::int16 >( "%d", 111.1, 111, 111 );
    test_binary_cast< double, comma::uint16 >( "%d", 111.1, 111, 111 );
    test_binary_cast< double, comma::int32 >( "%d", 111.1, 111, 111 );
    test_binary_cast< double, comma::uint32 >( "%d", 111.1, 111, 111 );
    //test_binary_cast< double, float >( "%d", 111.1, 111.1, 111.1 ); // todo
    test_binary_cast< double, double >( "%d", 111.1, 111.1, 111.1 );

    test_binary_cast< std::string, std::string >( "%s[8]", "hello", "hello", "hello" );
    boost::posix_time::ptime t = boost::posix_time::from_iso_string( "20110123T123456" );
    test_binary_cast< boost::posix_time::ptime, boost::posix_time::ptime >( "%t", t, t, t );
}

TEST( csv, binary_put )
{
    // todo
}

TEST( csv, binary_optional_element )
{
    // todo
//    {
//        comma::csv::binary_test::test_struct s;
//        EXPECT_EQ( comma::join( comma::csv::names( s ), ',' ), "a,z,nested/x,nested/y" );
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii;
//        binary.get( s, "1,2,3,4" );
//        EXPECT_TRUE( s.z );
//        EXPECT_TRUE( s.nested );
//        EXPECT_EQ( s.a, 1 );
//        EXPECT_EQ( *s.z, 2 );
//        EXPECT_EQ( s.nested->x, 3 );
//        EXPECT_EQ( s.nested->y, 4 );
//    }
//    {
//        comma::csv::binary_test::test_struct s;
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii( ",,z" );
//        binary.get( s, "1,2,3" );
//        EXPECT_TRUE( s.z );
//        EXPECT_TRUE( !s.nested );
//        EXPECT_EQ( *s.z, 3 );
//    }
//    {
//        comma::csv::binary_test::test_struct s;
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii( "nested,z" );
//        binary.get( s, "1,2,3" );
//        EXPECT_TRUE( s.z );
//        EXPECT_TRUE( s.nested );
//        EXPECT_EQ( *s.z, 3 );
//        EXPECT_EQ( s.nested->x, 1 );
//        EXPECT_EQ( s.nested->y, 2 );
//    }    
//    {
//        comma::csv::binary_test::test_struct s;
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii( ",nested" );
//        binary.get( s, "1,2,3" );
//        EXPECT_TRUE( !s.z );
//        EXPECT_TRUE( s.nested );
//        EXPECT_EQ( s.nested->x, 2 );
//        EXPECT_EQ( s.nested->y, 3 );
//    }    
//    {
//        comma::csv::binary_test::test_struct s;
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii( "nested,,a" );
//        binary.get( s, "1,2,3,4" );
//        EXPECT_TRUE( !s.z );
//        EXPECT_TRUE( s.nested );
//        EXPECT_EQ( s.nested->x, 1 );
//        EXPECT_EQ( s.nested->y, 2 );
//        EXPECT_EQ( s.a, 4 );
//    }
//    {
//        comma::csv::binary_test::test_struct s;
//        comma::csv::binary< comma::csv::binary_test::test_struct > ascii( ",x,y,a", ',', false );
//        binary.get( s, "1,2,3,4" );
//        EXPECT_TRUE( !s.z );
//        EXPECT_TRUE( s.nested );
//        EXPECT_EQ( s.nested->x, 2 );
//        EXPECT_EQ( s.nested->y, 3 );
//        EXPECT_EQ( s.a, 4 );
//    }    
//    // todo: more testing
}

TEST( csv, binary_containers )
{
    {
        comma::csv::binary_test::containers c;
        for( unsigned int i = 0; i < c.array.size(); ++i ) { c.array[i] = i; }
        comma::csv::binary< comma::csv::binary_test::containers > binary;
        {
            char buf[ 4 * 4 ];
            binary.put( c, buf );
            comma::csv::binary_test::containers d;
            binary.get( d, buf );
            EXPECT_EQ( d.array[0], 0 );
            EXPECT_EQ( d.array[1], 1 );
            EXPECT_EQ( d.array[2], 2 );
            EXPECT_EQ( d.array[3], 3 );
        }
    }
    // todo: more tests
}
