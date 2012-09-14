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
#include <comma/Application/command_line_options.h>
#include <comma/Application/SignalFlag.h>
#include <comma/csv/format.h>
#include <comma/String/String.h>

using namespace comma;

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "Usage: cat blah.csv | csv-to-bin <format> [--delimiter=<delimiter>] > blah.bin" << std::endl;
    std::cerr << std::endl;
    std::cerr << csv::format::usage() << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    #ifdef WIN32
    _setmode( _fileno( stdout ), _O_BINARY ); /// @todo move to a library
    #endif
    try
    {        
        SignalFlag shutdownFlag;
        command_line_options options( ac, av );
        if( ac < 2 || options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        char delimiter = options.value( "--delimiter", ',' );
        comma::csv::format format( av[1] );
        while( std::cin.good() && !std::cin.eof() )
        {
            if( shutdownFlag ) { std::cerr << "csv-to-bin: interrupted by signal" << std::endl; return -1; }
            std::string line;
            std::getline( std::cin, line );
            if( !line.empty() && *line.rbegin() == '\r' ) { line = line.substr( 0, line.length() - 1 ); } // windows... sigh...
            if( line.length() == 0 ) { continue; }
            format.csv_to_bin( std::cout, line, delimiter );
        }
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-to-bin: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-to-bin: unknown exception" << std::endl; }
    usage();
}
