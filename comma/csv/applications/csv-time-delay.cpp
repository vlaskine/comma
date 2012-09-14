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

#include <iostream>
#include <string>
#include <comma/Application/command_line_options.h>
#include <comma/Application/SignalFlag.h>
#include <comma/Base/Types.h>
#include <comma/csv/Stream.h>
#include <comma/NameValue/Parser.h>
#include <comma/String/String.h>
#include <comma/visiting/traits.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "subtract given value from timestamp" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat a.csv | csv-time-delay <seconds> [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << "    --binary,-b <format>: binary format" << std::endl;
    std::cerr << "    --delimiter,-d <delimiter>: ascii only; default ','" << std::endl;
    std::cerr << "    --fields,-f <fields>: input fields; default: t" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

struct Point
{
    boost::posix_time::ptime timestamp;
    Point() {}
    Point( const boost::posix_time::ptime& timestamp ) : timestamp( timestamp ) {}
};

namespace comma { namespace visiting {

template <> struct traits< Point >
{
    template < typename K, typename V > static void visit( const K&, const Point& p, V& v ) { v.apply( "t", p.timestamp ); }
    template < typename K, typename V > static void visit( const K&, Point& p, V& v ) { v.apply( "t", p.timestamp ); }
};
    
} } // namespace comma { namespace visiting {

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help" ) || options.exists( "-h" ) || ac == 1 ) { usage(); }        
        double d = boost::lexical_cast< double >( options.unnamed( "", "--binary,-b,--delimiter,-d,--fields,-f" )[0] );
        int sign = d < 0 ? -1 : 1;
        int seconds = int( std::floor( std::abs( d ) ) );
        int microseconds = int( ( std::abs( d ) - seconds ) * 1000000 );
        seconds *= sign;
        microseconds *= sign;
        boost::posix_time::time_duration delay = boost::posix_time::seconds( seconds ) + boost::posix_time::microseconds( microseconds );
        comma::csv::Options csv( options );
        comma::csv::input_stream< Point > istream( std::cin, csv );
        comma::csv::output_stream< Point > ostream( std::cout, csv );
        comma::SignalFlag is_shutdown;
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            const Point* p = istream.read();
            if( !p ) { break; }
            Point q = *p;
            q.timestamp = p->timestamp + delay;
            if( csv.binary() ) { ostream.write( q, istream.binary().last() ); }
            else { ostream.write( q, istream.ascii().last() ); }
        }
        if( is_shutdown ) { std::cerr << "csv-time-delay: interrupted by signal" << std::endl; }
        return 0;     
    }
    catch( std::exception& ex ) { std::cerr << "csv-time-delay: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-time-delay: unknown exception" << std::endl; }
    usage();
}
