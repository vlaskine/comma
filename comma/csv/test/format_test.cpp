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
#include <limits>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <comma/csv/format.h>

TEST( csv, format )
{
    {
        try { comma::csv::format f( "%" ); EXPECT_TRUE( false ); } catch ( ... ) {}
        try { comma::csv::format f( "blah" ); EXPECT_TRUE( false ); } catch ( ... ) {}
    }
    {
        comma::csv::format f( "%uw" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234" ) ), "1234" );
    }
    {
        comma::csv::format f( "%w" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "-1234" ) ), "-1234" );
    }
    {
        comma::csv::format f( "%ui" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234" ) ), "1234" );
    }
    {
        comma::csv::format f( "%i" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "-1234" ) ), "-1234" );
    }
    {
        comma::csv::format f( "%ul" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234" ) ), "1234" );
    }
    {
        comma::csv::format f( "%l" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "-1234" ) ), "-1234" );
    }
    {
        comma::csv::format f( "%c" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "c" ) ), "c" );
    }
    {
        comma::csv::format f( "%f" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234.56" ) ), "1234.56" ); // floats have just 6-digit precision
    }
    {
        comma::csv::format f( "%d" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234.123456789012" ) ), "1234.123456789012" );
    }
    {
        comma::csv::format f( "%uw%ui%f" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234,5678,90.1234" ) ), "1234,5678,90.1234" );
    }
    {
        comma::csv::format f( "%t" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "20100621T182601" ) ), "20100621T182601" );
    }
    {
        comma::csv::format f( "%t" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "20100621T182601.0123" ) ), "20100621T182601.012300" );
    }
    {
        comma::csv::format f( "%t" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "20100621T182601.123" ) ), "20100621T182601.123000" );
    }
    {
        comma::csv::format f( "%3ui" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1,2,3" ) ), "1,2,3" );
    }
    {
        comma::csv::format f( "%ui%2w%3d" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "0,-1,-2,1.123,2.345,3.678" ) ), "0,-1,-2,1.123,2.345,3.678" );
    }
    {
        comma::csv::format f( "%ui%2w%3d" );
        EXPECT_EQ( f.index( 0 ).first, 0u );
        EXPECT_EQ( f.index( 0 ).second, 0u );
        EXPECT_EQ( f.index( 1 ).first, 1u );
        EXPECT_EQ( f.index( 1 ).second, 0u );
        EXPECT_EQ( f.index( 2 ).first, 1u );
        EXPECT_EQ( f.index( 2 ).second, 1u );
        EXPECT_EQ( f.index( 3 ).first, 2u );
        EXPECT_EQ( f.index( 3 ).second, 0u );
        EXPECT_EQ( f.index( 4 ).first, 2u );
        EXPECT_EQ( f.index( 4 ).second, 1u );
        EXPECT_EQ( f.index( 5 ).first, 2u );
        EXPECT_EQ( f.index( 5 ).second, 2u );
    }    
    {
        try { comma::csv::format f( "%s" ); EXPECT_TRUE( false ); } catch ( ... ) {}
    }
    {
        comma::csv::format f( "%s[4]%2s[8]%10ui" );
        EXPECT_EQ( f.elements()[0].count, 1u );
        EXPECT_EQ( f.elements()[0].offset, 0u );
        EXPECT_EQ( f.elements()[0].size, 4u );
        EXPECT_EQ( f.elements()[1].count, 2u );
        EXPECT_EQ( f.elements()[1].offset, 4u );
        EXPECT_EQ( f.elements()[1].size, 8u );
        EXPECT_EQ( f.elements()[2].count, 10u );
        EXPECT_EQ( f.elements()[2].offset, 20u );
        EXPECT_EQ( f.elements()[2].size, 4u );
    }
    {
        comma::csv::format f( "%ui%2s[4]%3d" );
        EXPECT_EQ( f.index( 0 ).first, 0u );
        EXPECT_EQ( f.index( 0 ).second, 0u );
        EXPECT_EQ( f.index( 1 ).first, 1u );
        EXPECT_EQ( f.index( 1 ).second, 0u );
        EXPECT_EQ( f.index( 2 ).first, 1u );
        EXPECT_EQ( f.index( 2 ).second, 1u );
        EXPECT_EQ( f.index( 3 ).first, 2u );
        EXPECT_EQ( f.index( 3 ).second, 0u );
        EXPECT_EQ( f.index( 4 ).first, 2u );
        EXPECT_EQ( f.index( 4 ).second, 1u );
        EXPECT_EQ( f.index( 5 ).first, 2u );
        EXPECT_EQ( f.index( 5 ).second, 2u );
    }    
}

TEST( csv, format_add )
{
    {
        comma::csv::format f;
        comma::csv::format d;
        EXPECT_EQ( ( f += d ).string(), "" );
    }
    {
        comma::csv::format f( "i,f" );
        comma::csv::format d;
        EXPECT_EQ( ( f += d ).string(), "i,f" );
    }
    {
        comma::csv::format f;
        comma::csv::format d( "ui,d" );
        EXPECT_EQ( ( f += d ).string(), "ui,d" );
    }
    {
        comma::csv::format f( "f" );
        comma::csv::format d( "d" );
        EXPECT_EQ( ( f += d ).string(), "f,d" );
    }
    {
        comma::csv::format f( "i,f" );
        comma::csv::format d( "ui,d" );
        EXPECT_EQ( ( f += d ).string(), "i,f,ui,d" );
    }
}

TEST( csv, format_floating_point )
{
    {
        comma::csv::format f( "%f" );
        EXPECT_EQ( f.bin_to_csv( f.csv_to_bin( "1234.56" ) ), "1234.56" ); // floats have just 6-digit precision
    }
// todo more tests
}

//TEST( csv, format_nan )
//{
//	double nan = std::numeric_limits< double >::quiet_NaN();
//	double d = nan;
//	EXPECT_TRUE( std::isnan( nan ) );
//	EXPECT_TRUE( std::isnan( d ) );
//	EXPECT_TRUE( !std::isnan( 5 ) );
//	EXPECT_TRUE( 5 != nan );
//	std::cerr << "double nan: ";
//	std::cerr << nan << ": ";
//	const unsigned char* p = reinterpret_cast< const unsigned char* >( &nan );
//	for( unsigned int i = 0; i < sizeof( double ); ++i, ++p )
//	{
//		std::cerr << static_cast< unsigned int >( *p ) << " ";
//	}
//	std::cerr << std::endl;
//}

struct nested_struct
{
    int x;
    int y;
};

struct simple_struct
{
    int a;
    double b;
    char c;
    std::string s;
    boost::posix_time::ptime t;
    nested_struct nested;
};

namespace comma { namespace visiting {

template <> struct traits< nested_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const nested_struct& p, Visitor& v )
    {
        v.apply( "x", p.x );
        v.apply( "y", p.y );
    }
};

template <> struct traits< simple_struct >
{
    template < typename Key, class Visitor > static void visit( const Key&, const simple_struct& p, Visitor& v )
    {
        v.apply( "a", p.a );
        v.apply( "b", p.b );
        v.apply( "c", p.c );
        v.apply( "s", p.s );
        v.apply( "t", p.t );
        v.apply( "nested", p.nested );
    }
};

} } // namespace comma { namespace visiting {

TEST( csv, format_with_fields )
{
    EXPECT_EQ( comma::csv::format::value< simple_struct >(), "i,d,b,s,t,i,i" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "a,b,c", true ), "i,d,b" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "a,s,t", true ), "i,s,t" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "nested/x,nested/y", true ), "i,i" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "nested", true ), "i,i" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "a,b,c", false ), "i,d,b" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "a,s,t", false ), "i,s,t" );
    EXPECT_EQ( comma::csv::format::value< simple_struct >( "x,y", false ), "i,i" );
}
