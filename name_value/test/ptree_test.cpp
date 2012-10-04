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
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <comma/base/types.h>
#include <comma/name_value/ptree.h>
#include <comma/visiting/apply.h>

namespace comma { namespace test {

struct nested_type
{
    nested_type() : moon( 0 ) {}
    int moon;
    boost::optional< std::string > value;
};

struct test_type
{
    int hello;
    boost::optional< std::string > world;
    boost::optional< nested_type > nested;
    test_type() : hello( 0 ) {  }
};

struct vector_type
{
    std::vector< std::string > v;
};

struct nested_vector
{
    std::string hello;
    vector_type v;
};

} } // namespace comma { namespace test {

namespace comma { namespace visiting {

template <>
struct traits< comma::test::nested_type >
{
    template < typename Key, typename Visitor >
    static void visit( const Key&, comma::test::nested_type& t, Visitor& v )
    {
        v.apply( "moon", t.moon );
        v.apply( "value", t.value );
    }

    template < typename Key, typename Visitor >
    static void visit( const Key&, const comma::test::nested_type& t, Visitor& v )
    {
        v.apply( "moon", t.moon );
        v.apply( "value", t.value );
    }
};

template <>
struct traits< comma::test::test_type >
{
    template < typename Key, typename Visitor >
    static void visit( const Key&, comma::test::test_type& t, Visitor& v )
    {
        v.apply( "hello", t.hello );
        v.apply( "world", t.world );
        v.apply( "nested", t.nested );
    }

    template < typename Key, typename Visitor >
    static void visit( const Key&, const comma::test::test_type& t, Visitor& v )
    {
        v.apply( "hello", t.hello );
        v.apply( "world", t.world );
        v.apply( "nested", t.nested );
    }
};

template <>
struct traits< comma::test::vector_type >
{
    template < typename Key, typename Visitor >
    static void visit( const Key&, comma::test::vector_type& t, Visitor& v )
    {
        v.apply( "v", t.v );
    }

    template < typename Key, typename Visitor >
    static void visit( const Key&, const comma::test::vector_type& t, Visitor& v )
    {
        v.apply( "v", t.v );
    }
};

template <>
struct traits< comma::test::nested_vector >
{
    template < typename Key, typename Visitor >
    static void visit( const Key&, comma::test::nested_vector& t, Visitor& v )
    {
        v.apply( "hello", t.hello );
        v.apply( "v", t.v );
    }

    template < typename Key, typename Visitor >
    static void visit( const Key&, const comma::test::nested_vector& t, Visitor& v )
    {
        v.apply( "hello", t.hello );
        v.apply( "v", t.v );
    }
};

} } // namespace comma { namespace visiting {

namespace comma { namespace test {

TEST( ptree, basics )
{
    {
        test_type t;
        t.world = "value";
        boost::property_tree::ptree tree;
        to_ptree to( tree, "test_type" );
        visiting::apply( to, t );
        
        test_type t1;
        from_ptree from( tree, "test_type" );
        visiting::apply( from, t1 );
        EXPECT_EQ( t.hello, t1.hello );
        EXPECT_EQ( t.world, t1.world );
        EXPECT_EQ( *t1.world, "value" );
        EXPECT_TRUE( !t.nested );
        EXPECT_TRUE( !t1.nested );
        
        boost::property_tree::ptree tree1;
        to_ptree to1( tree1, "test_type" );
        from_ptree from1( tree1, "test_type" );
        t.world.reset();
        t.nested = nested_type();
        t1.world.reset();
        visiting::apply( to1, t );
        visiting::apply( from1, t1 );
        EXPECT_EQ( t.hello, t1.hello );
        EXPECT_TRUE( !t.world );
        EXPECT_TRUE( !t1.world );
        EXPECT_EQ( t.nested->moon, t1.nested->moon );
    }
    {
        boost::property_tree::ptree ptree;
        to_ptree to_ptree( ptree, "vector" );
        std::vector< std::string > v;
        v.push_back( "hello" );
        v.push_back( "world" );
        visiting::apply( to_ptree, v );
        from_ptree from_ptree( ptree, "vector" );
        visiting::apply( from_ptree, v );
        EXPECT_EQ( v.size(), 2u );
        EXPECT_EQ( v[0], "hello" );
        EXPECT_EQ( v[1], "world" );
//         property_tree::to_name_value( std::cerr, ptree );
        v.clear();
        visiting::apply( from_ptree, v );
        EXPECT_EQ( v.size(), 2u );
        EXPECT_EQ( v[0], "hello" );
        EXPECT_EQ( v[1], "world" );
    }
    {
        boost::property_tree::ptree ptree;
        to_ptree to_ptree( ptree, "vector" );
        std::vector< std::string > v;
        v.push_back( "hello" );
        v.push_back( "world" );
        vector_type vec;
        vec.v = v;
        nested_vector nested;
        nested.v = vec;
        visiting::apply( to_ptree, nested );
        from_ptree from_ptree( ptree, "vector" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
        nested.v.v.clear();
        //property_tree::to_name_value( std::cerr, ptree );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
    }
    {
        boost::property_tree::ptree ptree;
        to_ptree t( ptree, "map" );
        std::map< unsigned int, std::string > m;
        m.insert( std::make_pair( 1, "hello" ) );
        m.insert( std::make_pair( 3, "world" ) );
        visiting::apply( t, m );
        from_ptree from_ptree( ptree, "map" );
        visiting::apply( from_ptree, m );
        EXPECT_EQ( m.size(), 2u );
        EXPECT_EQ( m[1], "hello" );
        EXPECT_EQ( m[3], "world" );
        m.clear();
        visiting::apply( from_ptree, m );
        EXPECT_EQ( m.size(), 2u );
        EXPECT_EQ( m[1], "hello" );
        EXPECT_EQ( m[3], "world" );
        //property_tree::to_name_value( std::cerr, ptree );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "a={ b={ 1=hello 3=world } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_path_value( std::cerr, ptree, '=', '\n' );
        from_ptree from_ptree( ptree, "a/b" );
        std::map< unsigned int, std::string > m;
        visiting::apply( from_ptree, m );
        EXPECT_EQ( m.size(), 2u );
        EXPECT_EQ( m[1], "hello" );
        EXPECT_EQ( m[3], "world" );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "a={ 4={ moon=1 value=hello } 6={ moon=2 value=world } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_path_value( std::cerr, ptree, '=', '\n' );
        from_ptree from_ptree( ptree, "a" );
        std::map< unsigned int, nested_type > m;
        visiting::apply( from_ptree, m );
        EXPECT_EQ( m.size(), 2u );
        EXPECT_EQ( m[4].moon, 1 );
        EXPECT_EQ( *m[4].value, "hello" );
        EXPECT_EQ( m[6].moon, 2 );
        EXPECT_EQ( *m[6].value, "world" );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "a={ 4={ hello=4 nested={ moon=1 value=hello } } 6={ hello=6 nested={ moon=2 value=world } } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_path_value( std::cerr, ptree, '=', '\n' );
        from_ptree from_ptree( ptree, "a" );
        std::map< unsigned int, test_type > m;
        visiting::apply( from_ptree, m );
        EXPECT_EQ( m.size(), 2u );
        EXPECT_EQ( m[4].hello, 4 );
        EXPECT_EQ( m[4].nested->moon, 1 );
        EXPECT_EQ( *m[4].nested->value, "hello" );
        EXPECT_EQ( m[6].hello, 6 );
        EXPECT_EQ( m[6].nested->moon, 2 );
        EXPECT_EQ( *m[6].nested->value, "world" );
    }
}

TEST( ptree, permissive_visiting )
{
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree );
        nested_vector nested;
        ptree.put< std::string >( "hello", "blah" );
        ptree.put< std::string >( "v.v.0", "hello" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 1u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.hello, "blah" );
        nested.v.v.clear();
        ptree.put< std::string >( "v.v.1", "world" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
        EXPECT_EQ( nested.hello, "blah" );
    }
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree, true );
        nested_vector nested;
        ptree.put< std::string >( "hello", "blah" );
        ptree.put< std::string >( "v.v.0", "hello" );
        ptree.put< std::string >( "v.v.1", "world" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
        EXPECT_EQ( nested.hello, "blah" );
    }        
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree, true );
        nested_vector nested;
        ptree.put< std::string >( "v.v.0", "hello" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 1u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.hello, "" );
    }
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree );
        nested_vector nested;
        try { visiting::apply( from_ptree, nested ); EXPECT_TRUE( false ); } catch ( ... ) {}
    }
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree );
        nested_vector nested;
        ptree.put< std::string >( "hello", "blah" );
        try { visiting::apply( from_ptree, nested ); EXPECT_TRUE( false ); } catch ( ... ) {}
    }
    {
        boost::property_tree::ptree ptree;
        from_ptree from_ptree( ptree );
        nested_vector nested;
        ptree.put< std::string >( "hello", "blah" );
        ptree.put< std::string >( "v.v.0", "hello" );
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 1u );
        EXPECT_EQ( nested.v.v[0], "hello" );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "root={ hello=blah v={ v={} } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree f( ptree, "root" );
        nested_vector nested;
        visiting::apply( f, nested );
        EXPECT_EQ( nested.hello, "blah" );
        EXPECT_TRUE( nested.v.v.empty() );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "root={ hello=blah v={} }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree f( ptree, "root" );
        nested_vector nested;
        try { visiting::apply( f, nested ); EXPECT_TRUE( false ); } catch( ... ) {}
        from_ptree from_ptree_permissive( ptree, "root", true );
        try { visiting::apply( from_ptree_permissive, nested ); } catch( ... ) { EXPECT_TRUE( false ); }
    }
}

TEST( ptree, array )
{
    {
        boost::property_tree::ptree ptree;
        std::string s = "0=hello 1=world";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree f( ptree );
        boost::array< std::string, 2 > array;
        visiting::apply( f, array );
        EXPECT_EQ( array[0], "hello" );
        EXPECT_EQ( array[1], "world" );
    }        
    {
        boost::property_tree::ptree ptree;
        std::string s = "0=hello 1=world";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree from_ptree( ptree );
        boost::array< std::string, 3 > array;
        try { visiting::apply( from_ptree, array ); EXPECT_TRUE( false ); } catch( ... ) {}
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "root={ 0=hello 1=world }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree from_ptree( ptree, "root" );
        boost::array< std::string, 2 > array;
        visiting::apply( from_ptree, array );
        EXPECT_EQ( array[0], "hello" );
        EXPECT_EQ( array[1], "world" );
    }
}

TEST( ptree, name_value_string )
{
    {
        std::string s = "x { a { b 1 c 2 } } y 3";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, ' ' );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false ), "x={a={b=\"1\",c=\"2\"}},y=\"3\"" );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false, ' ', ' ' ), "x {a {b \"1\" c \"2\"}} y \"3\"" );
        //std::cerr <<  property_tree::to_name_value_string( ptree, true ) << std::endl;        
        //std::cerr <<  property_tree::to_name_value_string( ptree, true, ' ', ' 'test_ptree_permissive ) << std::endl;
        // todo: more testing
    }
    {
        std::string s = "x = 1, y = 2";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, '=', ',' );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false, ' ', ' ' ), "x \"1\" y \"2\"" );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false ), "x=\"1\",y=\"2\"" );
    }
    {
        std::string s = "x = { a = { b = 1, c = 2 } }, y = 3";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, '=', ',' );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false, ' ', ' ' ), "x {a {b \"1\" c \"2\"}} y \"3\"" );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false ), "x={a={b=\"1\",c=\"2\"}},y=\"3\"" );
    }
    {
        std::string s = "x={ a=\"\", b=2, c=\"hi\" }";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, '=', ',' );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false, ' ', ' ' ), "x {a \"\" b \"2\" c \"hi\"}" );
        EXPECT_EQ( property_tree::to_name_value_string( ptree, false ), "x={a=\"\",b=\"2\",c=\"hi\"}" );
    }
    boost::property_tree::ptree p;
    boost::property_tree::ptree::path_type path = "a";
    std::string s = "b 1 c 2";
    std::istringstream iss( s );
    boost::property_tree::ptree q;
    boost::property_tree::read_info( iss, q );
    {
        std::ostringstream oss;
        boost::property_tree::write_info( oss, q );
        EXPECT_EQ( oss.str(), "b 1\nc 2\n" );
    }
    {
        boost::property_tree::ptree::value_type v( "blah", q );
        p.push_back( v );
        std::ostringstream oss;
        boost::property_tree::write_info( oss, p );
        EXPECT_EQ( oss.str(), "blah\n{\n    b 1\n    c 2\n}\n" );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "root={ v={ v={ 0=hello 1=world } } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_name_value( std::cerr, ptree );
        from_ptree from_ptree_permissive( ptree, "root", true );
        nested_vector nested;
        visiting::apply( from_ptree_permissive, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
    }
    {
        boost::property_tree::ptree ptree;
        std::string s = "hello=blah v={ v={ 0=hello 1=world } }";
        std::istringstream iss( s );
        property_tree::from_name_value( iss, ptree );
        //property_tree::to_path_value( std::cerr, ptree, '=', '\n' );
        from_ptree from_ptree( ptree );
        nested_vector nested;
        visiting::apply( from_ptree, nested );
        EXPECT_EQ( nested.v.v.size(), 2u );
        EXPECT_EQ( nested.v.v[0], "hello" );
        EXPECT_EQ( nested.v.v[1], "world" );
        EXPECT_EQ( nested.hello, "blah" );
    }
}

TEST( ptree, path_value_string )
{
    {
        std::string s = "x = 1, y = 2";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, '=', ',' );
        EXPECT_EQ( property_tree::to_path_value_string( ptree ), "x=\"1\",y=\"2\"" );
    }
    {
        std::string s = "x = { a = { b = 1, c = 2 } }, y = 3";
        boost::property_tree::ptree ptree = property_tree::from_name_value_string( s, '=', ',' );
        EXPECT_EQ( property_tree::to_path_value_string( ptree ), "x/a/b=\"1\",x/a/c=\"2\",y=\"3\"" );
    }
    // todo: more tests
}

} } // namespace comma { namespace test {
