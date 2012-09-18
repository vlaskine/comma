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
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <boost/noncopyable.hpp>
#include <comma/visiting/apply.h>
#include <comma/visiting/visit.h>

namespace comma { namespace visiting { namespace test {

/// visitor that outputs a struct in a text format
class o_stream_visitor : public boost::noncopyable
{
    public:
        /// constructor
        o_stream_visitor( std::ostream& s ) : m_stream( s ) {}
    
        /// apply to the "leaf" types
        template < typename Key >
        void apply( const Key& name, const bool& value ) { m_stream << "bool:" << name << "=" << ( value ? "true" : "false" ) << " "; }
        template < typename Key >
        void apply( const Key& name, const int& value ) { m_stream << "int:" << name << "=" << value << " "; }
        template < typename Key >
        void apply( const Key& name, const float& value ) { m_stream << "float:" << name << "=" << value << " "; }
        template < typename Key >
        void apply( const Key& name, const double& value ) { m_stream << "double:" << name << "=" << value << " "; }
        template < typename Key >
        void apply( const Key& name, const std::string& value ) { m_stream << "string:" << name << "=\"" << value << "\" "; }
    
        /// apply to a structure: traverse it depth-first
        template < typename K, typename T >
        void apply( const K& name, const T& value )
        {
            if( !empty( name ) ) { m_stream << "object:" << name << "={ "; }
            else { m_stream << "{ "; }
            comma::visiting::visit( name, value, *this );
            m_stream << "}" << ( empty( name ) ? "" : " " );
        }
    
    private:
        std::ostream& m_stream;
        bool empty( const char* name ) { return *name == 0; }
        bool empty( std::size_t ) { return false; }
};

/// visitor that multiplies all the structure members by a number;
/// for simplicity of demo, assumes that the structure has only int "leaf" fields 
class multiply : public boost::noncopyable
{
    public:
        /// constructor
        multiply( int v ) : m_value( v ) {}
    
        /// multiply an int "leaf" field 
        void apply( const char*, int& value ) { value *= m_value; }
    
        /// traverse depth first
        template < typename T >
        void apply( const char* name, T& value ) { comma::visiting::visit( name, value, *this ); }
    
    private:
        int m_value;
};

struct old_plain
{
    struct nested
    {
        int a;
        double b;
        std::string c;
    };
    float blah;
    int hello;
    nested world;
};

} } } // namespace comma { namespace visiting { namespace test {

namespace comma { namespace visiting {

/// traits specialization for old_plain
template <>
struct traits< test::old_plain >
{
    template < typename Key, typename visitor >
    static void visit( const Key&, const test::old_plain& p, visitor& v )
    {
        v.apply( "blah", p.blah );
        v.apply( "hello", p.hello );
        v.apply( "world", p.world );
    }
};

/// traits specialization for old_plain::nested
template <>
struct traits< test::old_plain::nested >
{
    template < typename Key, typename visitor >
    static void visit( const Key&, const test::old_plain::nested& p, visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
    }
};

} } // namespace comma { namespace visiting {

namespace comma { namespace visiting { namespace test {

TEST( visiting, nestedConst )
{
    {
        old_plain p;
        p.blah = 0;
        p.hello = 1;
        p.world.a = 2;
        p.world.b = 3;
        p.world.c = "HELLO";
        std::ostringstream oss;
        o_stream_visitor v( oss );
        visiting::apply( v, p );
        EXPECT_EQ( oss.str(), "{ float:blah=0 int:hello=1 object:world={ int:a=2 double:b=3 string:c=\"HELLO\" } }" );
    }
}

struct containers
{
    std::pair< std::string, int > pair;
    std::vector< std::string > vector;
    std::set< int > set;
    std::map< std::string, double > map;
};

} } } // namespace comma { namespace visiting { namespace test {

namespace comma { namespace visiting {

template <>
struct traits< test::containers >
{
    template < typename Key, typename visitor >
    static void visit( const Key&, const test::containers& p, visitor& v )
    {
        v.apply( "pair", p.pair );
        v.apply( "vector", p.vector );
        v.apply( "set", p.set );
        v.apply( "map", p.map );
    }
    
    template < typename Key, typename visitor >
    static void visit( const Key&, test::containers& p, visitor& v )
    {
        v.apply( "pair", p.pair );
        v.apply( "vector", p.vector );
        v.apply( "set", p.set );
        v.apply( "map", p.map );
    }
};

} } // namespace comma { namespace visiting {

namespace comma { namespace visiting { namespace test {

TEST( visiting, containter )
{
    containers p;
    p.pair = std::make_pair( "blah", 111 );
    p.vector.push_back( "first" );
    p.vector.push_back( "second" );
    p.set.insert( 111 );
    p.set.insert( 222 );
    p.map.insert( std::make_pair( "jupiter", 888 ) );
    p.map.insert( std::make_pair( "saturn", 999 ) );        
    {
        std::ostringstream oss;
        o_stream_visitor v( oss );
        visiting::apply( v, p );
        EXPECT_EQ( oss.str(), "{ object:pair={ string:first=\"blah\" int:second=111 } object:vector={ string:0=\"first\" string:1=\"second\" } object:set={ int:0=111 int:1=222 } object:map={ double:jupiter=888 double:saturn=999 } }" );
        std::cerr << oss.str() << std::endl;
    }
}

} } } /// namespace comma { namespace visiting { namespace test {

namespace comma { namespace visiting { namespace test {

struct struct_with_containers
{
    std::size_t size;
    std::vector< double > vector;
    boost::array< std::size_t, 5 > array;
};

} } } // namespace comma { namespace visiting { namespace test {

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
