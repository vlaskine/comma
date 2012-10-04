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
#include <string>
#include <boost/optional.hpp>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/types.h>
#include <comma/csv/format.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "prepend input with timestamp" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat a.csv | csv-time-stamp [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << "    --binary,-b=<format>: binary format" << std::endl;
    std::cerr << "    --size=<size>: binary input of size" << std::endl;
    std::cerr << "    --delimiter,-d <delimiter>: ascii only; default ','" << std::endl;
    std::cerr << "    --local: if present, local time; default: utc" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        bool local = options.exists( "--local" );
        
        bool binary = options.exists( "--binary,-b,--size" );
        std::size_t size = options.value( "--size", 0 );
        char delimiter = options.value( "--delimiter,-d", ',' );

        #ifdef WIN32
        if( binary )
        {
            _setmode( _fileno( stdin ), _O_BINARY );
            _setmode( _fileno( stdout ), _O_BINARY );
        }
        #endif

        comma::signal_flag is_shutdown;
        if( binary )
        {
            boost::array< char, 65536 > buf;
            char* begin = &buf[0];
            const char* end = begin + ( buf.size() / size ) * size;
            char* cur = begin;
            unsigned int offset = 0;
            while( !is_shutdown && std::cin.good() && !std::cin.eof() )
            {
                if( offset >= size )
                {
                    boost::posix_time::ptime now = local ? boost::posix_time::microsec_clock::local_time() : boost::posix_time::microsec_clock::universal_time();
                    static const unsigned int time_size = comma::csv::format::traits< boost::posix_time::ptime, comma::csv::format::time >::size;
                    static char timestamp[ time_size ];
                    comma::csv::format::traits< boost::posix_time::ptime, comma::csv::format::time >::to_bin( now, timestamp );
                    for( ; offset >= size; cur += size, offset -= size )
                    {
                        std::cout.write( ( char* )( &timestamp ), time_size );
                        std::cout.write( cur, size );
                    }
                    std::cout.flush();
                    if( cur == end ) { cur = begin; }
                }
                int r = ::read( 0, cur + offset, end - cur - offset );
                if( r <= 0 ) { break; }
                offset += r;
            }            
        }
        else
        {
            while( !is_shutdown && std::cin.good() && !std::cin.eof() )
            {
                std::string line;
                std::getline( std::cin, line );
                if( line.empty() ) { continue; }
                boost::posix_time::ptime now = local ? boost::posix_time::microsec_clock::local_time() : boost::posix_time::microsec_clock::universal_time();
                std::cout << boost::posix_time::to_iso_string( now ) << delimiter << line << std::endl;
            }
        }
        if( is_shutdown ) { std::cerr << "csv-time-stamp: interrupted by signal" << std::endl; }
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-time-stamp: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-time-stamp: unknown exception" << std::endl; }
    usage();
}
