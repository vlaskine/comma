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
#include <comma/Application/command_line_options.h>
#include <comma/csv/format.h>

using namespace comma;

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "a convenience utility: output to stdout size of given binary format" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage: csv-size <format> [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "options" << std::endl;
    std::cerr << "    --count,-c: output number of fields, rather than size" << std::endl;
    std::cerr << std::endl;
    std::cerr << "e.g.: csv-size t,d,d,d,ui will output 40" << std::endl;
    std::cerr << "      csv-size t will output 8" << std::endl;
    std::cerr << "      csv-size 2d will output 16" << std::endl;
    std::cerr << "      csv-size 2d --count will output 2" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    try
    {
        command_line_options options( ac, av );
        if( ac < 2 || options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        comma::csv::format format( options.unnamed( "--count,-c", "" )[0] );
        std::cout << ( options.exists( "--count,-c" ) ? format.count() : format.size() ) << std::endl;
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-size: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-size: unknown exception" << std::endl; }
    usage();
}
