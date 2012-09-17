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
#include <iostream>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/exception.h>
#include <comma/csv/format.h>
#include <comma/string/string.h>

using namespace comma;

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "Usage: cat blah.bin | csv-from-bin <format> --precision <precision> > blah.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "--precision: set precision (number of mantissa digits) for floating point types" << std::endl;
    std::cerr << csv::format::usage() << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    #ifdef WIN32
    _setmode( _fileno( stdin ), _O_BINARY ); /// @todo move to a library
    #endif
    try
    {
        signal_flag shutdownFlag;
        command_line_options options( ac, av );
        if( ac < 2 || options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        char delimiter = options.value( "--delimiter", ',' );
        boost::optional< unsigned int > precision;
        if( options.exists( "--precision" ) ) { precision = options.value< unsigned int >( "--precision" ); }
        comma::csv::format format( av[1] );
        std::vector< char > w( format.size() ); //char buf[ format.size() ]; // stupid windows
        char* buf = &w[0];
        while( std::cin.good() && !std::cin.eof() )
        {
            if( shutdownFlag ) { std::cerr << "csv-from-bin: interrupted by signal" << std::endl; return -1; }
            std::cin.read( buf, format.size() );
            if( std::cin.gcount() == 0 ) { break; }
            if( std::cin.gcount() < static_cast< int >( format.size() ) ) { COMMA_THROW( comma::exception, "expected " << format.size() << " bytes, got only " << std::cin.gcount() ); }
            std::cout << format.bin_to_csv( buf, delimiter, precision ) << std::endl;
        }
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-from-bin: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-from-bin: unknown exception" << std::endl; }
    usage();
}

