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

#include <stdlib.h>
#include <string.h>
#include <deque>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <comma/Application/command_line_options.h>
#include <comma/Application/SignalFlag.h>
#include <comma/Base/Exception.h>
#include <comma/csv/format.h>
#include <comma/String/String.h>

using namespace comma;

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "simplified, but similar as Linux cut utility, but for \"binary csv\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat blah.bin | csv-bin-cut <format> --fields=<fields>" << std::endl;
    std::cerr << "    <fields>: field numbers, starting from 1 (to keep" << std::endl;
    std::cerr << "              consistent with the standard cut utility)" << std::endl;
    std::cerr << std::endl;
    std::cerr << csv::format::usage() << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    #ifdef WIN32
    _setmode( _fileno( stdin ), _O_BINARY ); /// @todo move to a library
    _setmode( _fileno( stdout ), _O_BINARY ); /// @todo move to a library
    #endif
    try
    {        
        SignalFlag shutdownFlag;
        command_line_options options( ac, av );
        if( ac < 2 || options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        comma::csv::format format( av[1] );
        if( !options.exists( "--fields" ) ) { COMMA_THROW( comma::exception, "please specify --fields" ); }
        std::vector< std::string > v = comma::split( options.value< std::string >( "--fields" ), ',' );
        std::vector< comma::csv::format::element > offsets( v.size() );
        for( unsigned int i = 0; i < v.size(); ++i )
        {
            offsets[i] = format.offset( boost::lexical_cast< std::size_t >( v[i] ) - 1 );
        }        
        std::vector< char > w( format.size() ); // stupid windows
        char* buf = &w[0];
        while( std::cin.good() && !std::cin.eof() )
        {
            if( shutdownFlag ) { std::cerr << "csv-bin-cut: interrupted by signal" << std::endl; return -1; }
            // quick and dirty; if performance is an issue, you could read more than
            // one record every time, but absolutely don't make this read blocking!
            // see comma::csv::binary_input_stream::read() for reference - if you know
            // how to do it better, please tell everyone!
            std::cin.read( buf, format.size() );
            if( std::cin.gcount() == 0 ) { continue; }
            if( std::cin.gcount() < int( format.size() ) ) { COMMA_THROW( comma::exception, "expected " << format.size() << " bytes, got only " << std::cin.gcount() ); }
            for( unsigned int i = 0; i < offsets.size(); ++i )
            {
                std::cout.write( buf + offsets[i].offset, offsets[i].size );
            }
        }
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-bin-cut: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-bin-cut: unknown exception" << std::endl; }
    usage();
}        
        
