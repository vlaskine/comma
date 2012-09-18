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

#ifdef WIN32
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <iostream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/exception.h>
#include <comma/base/types.h>
#include <comma/io/file_descriptor.h>

using namespace comma;

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "Read input data and thin them down by the given percentage;" << std::endl;
    std::cerr << "buffer handling optimized for a high-output producer" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage: cat full.csv | csv-thin <rate> [<options>] > thinned.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "e.g. output 70% of data:  cat full.csv | csv-thin 0.7 > thinned.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    --size,-s <size>: if given, data is packets of fixed size" << std::endl;
    std::cerr << "                      otherwise data is line-based" << std::endl;
    std::cerr << "    --deterministic,-d: if given, input is downsampled by a factor of int(1 / <rate>)." << std::endl;
    std::cerr << "                     That is, if <rate> is 0.33, output every third packet." << std::endl;
    std::cerr << "                     Default is to output each packet with a probability of <rate>." << std::endl;
    std::cerr << std::endl;
    exit( 1 );
}

static double rate;
static bool deterministic;

bool ignore()
{
    static boost::mt19937 rng;
    static boost::uniform_real<> dist( 0, 1 );
    static boost::variate_generator< boost::mt19937&, boost::uniform_real<> > flip( rng, dist );
    static bool do_ignore = comma::less( rate, 1.0 );

    if(deterministic)
    {
        static int count = 0;
        bool result = count % int(1.0 / rate);
        count = (count + 1) % int(1.0 / rate);
        return result;
    }
    else
    {
        return do_ignore && flip() > rate;
    }
}

int main( int ac, char** av )
{
    try
    {
        signal_flag shutdownFlag;
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h" ) || ac == 1 ) { usage(); }
        bool binary = options.exists( "--size,-s" );
        deterministic = options.exists( "--deterministic,-d" );
        std::size_t size = options.value( "--size,-s", 0u );
        #ifdef WIN32
        if( binary ) { _setmode( _fileno( stdin ), _O_BINARY ); _setmode( _fileno( stdout ), _O_BINARY ); }
        #endif
        std::vector< std::string > v = options.unnamed( "", "-s,--size" );
        if( v.empty() ) { std::cerr << "csv-thin: please specify rate" << std::endl; usage(); }
        rate = boost::lexical_cast< double >( v[0] );
        if(comma::less( rate, 0 ) || comma::less( 1, rate ) ) { std::cerr << "csv-thin: expected rate between 0 and 1, got " << rate << std::endl; usage(); }

        if( binary ) // quick and dirty, improve performance by reading larger buffer
        {
            const unsigned int factor = 16384 / size; // arbitrary
            std::vector< char > buf( size * factor );
            #ifdef WIN32
            while( !shutdownFlag && std::cin.good() && !std::cin.eof() )
            {
                // it all does not seem to work: in_avail() always returns 0
                //std::streamsize available = std::cin.rdbuf()->in_avail();
                //if( available < 0 ) { continue; }
                //if( available > 0 ) { std::cerr << "available = " << available << std::endl; }
                //std::size_t e = available < int( size ) ? size : available - available % size;
                std::cin.read( &buf[0], size ); // quick and dirty
                if( std::cin.gcount() <= 0 ) { break; }
                if( std::cin.gcount() < int( size ) ) { std::cerr << "csv-thin: expected " << size << " bytes; got only " << std::cin.gcount() << std::endl; exit( 1 ); }
                { if( !ignore() ) { std::cout.write( &buf[0], size ); std::cout.flush(); } }
            }
            #else
            char* cur = &buf[0];
            unsigned int offset = 0;
            unsigned int capacity = buf.size();
            while( !shutdownFlag && std::cin.good() && !std::cin.eof() )
            {
                int count = ::read( comma::io::stdin_fd, cur + offset, capacity );
                if( count <= 0 )
                {
                    if( offset != 0 ) { std::cerr << "csv-thin: expected at least " << size << " bytes, got only " << offset << std::endl; return 1; }
                    break;                    
                }
                offset += count;
                capacity -= count;
                for( ; offset >= size; cur += size, offset -= size )
                {
                    if( !ignore() ) { std::cout.write( cur, size ); }
                }
                if( capacity == 0 ) { cur = &buf[0]; offset = 0; capacity = buf.size(); }
                std::cout.flush();
            }            
            #endif
        }
        else
        {
            std::string line;
            while( !shutdownFlag && std::cin.good() && !std::cin.eof() )
            {
                std::getline( std::cin, line );
                if( !ignore() ) { std::cout << line << std::endl; }
            }
        }
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-size: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-size: unknown exception" << std::endl; }
    usage();
}
