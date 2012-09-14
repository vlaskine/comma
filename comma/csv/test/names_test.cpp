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
#include <vector>
#include <boost/array.hpp>
#include <comma/csv/names.h>
#include <comma/visiting/apply.h>
#include <comma/string/string.h>

namespace comma { namespace csv { namespace test {

struct deep_nested
{
    int X;
    int Y;
};

struct nested
{
    int A;
    deep_nested B;
};

struct test_struct
{
    int C;
    nested D;
};

struct containers
{
    std::size_t size;
    std::vector< deep_nested > vector;
    boost::array< int, 4 > array;
};

struct struct_with_optional_element
{
    boost::optional< int > x;
    boost::optional< deep_nested > nested;
};

} } } // namespace comma { namespace csv { namespace test {

namespace comma { namespace visiting {

template <> struct traits< comma::csv::test::deep_nested >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::deep_nested& p, Visitor& v )
    {
        v.apply( "X", p.X );
        v.apply( "Y", p.Y );
    }
};

template <> struct traits< comma::csv::test::nested >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::nested& p, Visitor& v )
    {
        v.apply( "A", p.A );
        v.apply( "B", p.B );
    }
};

template <> struct traits< comma::csv::test::test_struct >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::test_struct& p, Visitor& v )
    {
        v.apply( "C", p.C );
        v.apply( "D", p.D );
    }
};

template <> struct traits< comma::csv::test::containers >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::containers& p, Visitor& v )
    {
        v.apply( "size", p.size );
        v.apply( "vector", p.vector );
        v.apply( "array", p.array );
    }
};

template <> struct traits< comma::csv::test::struct_with_optional_element >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::struct_with_optional_element& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "nested", p.nested );
    }
};

} } // namespace comma { namespace visiting {

namespace comma { namespace csv { namespace test {

TEST( csv, names_to_names )
{
    {
        impl::to_names v;
        test_struct s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "C,D/A,D/B/X,D/B/Y" );
    }
    {
        impl::to_names v( "D/B", false );
        test_struct s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "X,Y" );
    }
    {
        impl::to_names v( "D/B", true );
        test_struct s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "D/B/X,D/B/Y" );
    }
    {
        impl::to_names v( false );
        test_struct s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "C,A,X,Y" );
    }
    /// @todo definitely more testing
}

TEST( csv, names_containers ) // currently only vectors and arrays supported
{
    {
        impl::to_names v;
        containers s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "size,array[0],array[1],array[2],array[3]" );
    }
    {
        impl::to_names v( false );
        containers s;
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "size,array[0],array[1],array[2],array[3]" );
    }
    {
        impl::to_names v;
        containers s;
        s.vector.resize( 2 );
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "size,vector[0]/X,vector[0]/Y,vector[1]/X,vector[1]/Y,array[0],array[1],array[2],array[3]" );
    }
    {
        impl::to_names v( "vector[0]", false );
        containers s;
        s.vector.resize( 2 );
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "X,Y" );
    }
    {
        impl::to_names v( "vector", false );
        containers s;
        s.vector.resize( 2 );
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "X,Y,X,Y" );
    }
    {
        impl::to_names v( "vector", true );
        containers s;
        s.vector.resize( 2 );
        visiting::apply( v, s );
        EXPECT_EQ( join( v(), ',' ), "vector[0]/X,vector[0]/Y,vector[1]/X,vector[1]/Y" );
    }
    /// @todo definitely more testing
}

TEST( csv, names )
{
    EXPECT_EQ( join( names< test_struct >(), ',' ), "C,D/A,D/B/X,D/B/Y" );
    EXPECT_EQ( join( names< test_struct >( "" ), ',' ), "C,D/A,D/B/X,D/B/Y" );
    EXPECT_EQ( join( names< test_struct >( ",,," ), ',' ), ",,," );
    EXPECT_EQ( join( names< test_struct >( "D/B", false ), ',' ), "X,Y" );
    EXPECT_EQ( join( names< test_struct >( ",,,D/B,," ), ',' ), ",,,D/B/X,D/B/Y,," );
    EXPECT_EQ( join( names< test_struct >( "D" ), ',' ), "D/A,D/B/X,D/B/Y" );
    EXPECT_EQ( join( names< test_struct >( "D/B" ), ',' ), "D/B/X,D/B/Y" );
    EXPECT_EQ( join( names< test_struct >( "D/B,D/B" ), ',' ), "D/B/X,D/B/Y,D/B/X,D/B/Y" );
    EXPECT_EQ( join( names< test_struct >( "D/B/X" ), ',' ), "D/B/X" );
    EXPECT_EQ( join( names< test_struct >( false ), ',' ), "C,A,X,Y" );
    EXPECT_EQ( join( names< containers >(), ',' ), "size,array[0],array[1],array[2],array[3]" );
    EXPECT_EQ( join( names< containers >( false ), ',' ), "size,array[0],array[1],array[2],array[3]" );
    containers s;
    s.vector.resize( 2 );
    EXPECT_EQ( join( names( s ), ',' ), "size,vector[0]/X,vector[0]/Y,vector[1]/X,vector[1]/Y,array[0],array[1],array[2],array[3]" );
    EXPECT_EQ( join( names( "vector[0]", false, s ), ',' ), "X,Y" );
    EXPECT_EQ( join( names( "vector", false, s ), ',' ), "X,Y,X,Y" );
    EXPECT_EQ( join( names( "vector", true, s ), ',' ), "vector[0]/X,vector[0]/Y,vector[1]/X,vector[1]/Y" );
    EXPECT_EQ( join( names< test_struct >( "no-such-field" ), ',' ), "no-such-field" );
    EXPECT_EQ( join( names< test_struct >( ",,,,no-such-field,,," ), ',' ), ",,,,no-such-field,,," );
}

TEST( csv, names_optional_element )
{
    EXPECT_EQ( join( names< struct_with_optional_element >(), ',' ), "x,nested/X,nested/Y" );
    EXPECT_EQ( join( names< struct_with_optional_element >( "", false ), ',' ), "x,X,Y" );
    EXPECT_EQ( join( names< struct_with_optional_element >( "nested" ), ',' ), "nested/X,nested/Y" );
    EXPECT_EQ( join( names< struct_with_optional_element >( "nested", false ), ',' ), "X,Y" );
    // todo: more testing
}

} } } // namespace comma { namespace csv { namespace test {
