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
#include <sstream>
#include <vector>
#include <boost/array.hpp>
//#include <google/profiler.h>
#include <comma/base/types.h>
#include <comma/csv/stream.h>

namespace comma { namespace csv { namespace test {

struct test_struct
{
    comma::uint32 x;
    comma::uint32 y;
    test_struct() : x( 0 ), y( 0 ) {}
    test_struct( comma::uint32 x, comma::uint32 y ) : x( x ), y( y ) {}
};

} } } // namespace comma { namespace csv { namespace test {

namespace comma { namespace visiting {

template <> struct traits< comma::csv::test::test_struct >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::test::test_struct& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }
    
    template < typename Key, class Visitor >
    static void visit( const Key&, comma::csv::test::test_struct& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }    
};

} } // namespace comma { namespace visiting {

namespace comma { namespace csv { namespace test {

TEST( csv, stream )
{
//	std::cerr << "ProfileStream(): start" << std::endl;
//	const std::size_t size = 100000;
//	std::string istring( size * 4 * 2, 0 );
//	for( unsigned int i = 0; i < size; ++i ) // no need, really
//	{
//		::memcpy( &istring[i*4*2], &i, 4 );
//		::memcpy( &istring[4 + i*4*2], &i, 4 );
//	}
//	std::istringstream iss( istring );
//	std::ostringstream oss;
//	comma::csv::Options csv;
//	csv.format( "%ui%ui" );
//	EXPECT_TRUE( csv.binary() );
//	comma::csv::input_stream< test_struct > istream( iss, csv );
//	comma::csv::output_stream< test_struct > ostream( oss, csv );
//	ProfilerStart( "csv_stream.prof" );	{
//	for( unsigned int i = 0; i < size; ++i )
//	{
//		const test_struct* s = istream.read();
//		test_struct t( s->x + 1, s->y + 1 );
//		//ostream.write( t, istream.last() );
//		ostream.binary().write( t, istream.binary().last() );
//	}
//	ProfilerStop(); }
//	std::cerr << "ProfileStream(): stop" << std::endl;
}

} } } // namespace comma { namespace csv { namespace test {

