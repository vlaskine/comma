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
#else
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/last_error.h>
#include <comma/string/string.h>
#include "./publish.h"

//#include <google/profiler.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "read from standard input and write to given outputs (files, sockets, named pipes):" << std::endl;
    std::cerr << std::endl;
    std::cerr << "- the data is only written to the outputs that are ready for writing" << std::endl;
    std::cerr << "- client can connect and disconnect at any time" << std::endl;
    std::cerr << "- only full packets are written" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: io-publish [<options>] <outputs>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << "    --number,-n: ascii line-based input; buffer up to n lines, default is 0" << std::endl;
    std::cerr << "    --size,-s: binary input; packet size" << std::endl;
    std::cerr << "    --buffer,-b: buffer size, default is 0" << std::endl;
    std::cerr << "    --multiplier,-m: multiplier for packet size, default is 1. The actual packet size will be m*s" << std::endl;
    std::cerr << "    --no-discard: if present, do blocking write to every open pipe" << std::endl;
    std::cerr << "    --no-flush: if present, do not flush the output stream ( use on high bandwidth sources )" << std::endl;
    std::cerr << "<outputs>" << std::endl;
    std::cerr << "    tcp:<port>: e.g. tcp:1234" << std::endl;
    std::cerr << "    udp:<port>: e.g. udp:1234 (todo)" << std::endl;
    std::cerr << "    local:<name>: linux/unix local server socket e.g. local:./tmp/my_socket" << std::endl;
    std::cerr << "    <named pipe name>: named pipe, which will be re-opened, if client reconnects" << std::endl;
    std::cerr << "    <filename>: a regular file" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int ac, char** av )
{
    try
    {
        const boost::array< comma::signal_flag::signals, 2 > signals = { { comma::signal_flag::sigint, comma::signal_flag::sigterm } };
        comma::signal_flag is_shutdown( signals );
        comma::command_line_options options( ac, av );
        if( options.exists( "--help" ) || options.exists( "-h" ) ) { usage(); }
        std::vector< std::string > names = options.unnamed( "--no-discard,--verbose,-v,--no-flush", "-n,--number,-m,--multiplier,-b,--buffer,-s,--size" );
        unsigned int n = options.value( "-n,--number", 0 );
        unsigned int packet_size = options.value( "-s,--size", 0 ) * options.value( "-m,--multiplier", 1 );
        unsigned int buffer_size = options.value( "-b,--buffer", 0 );
        bool discard = !options.exists( "--no-discard" );
        bool flush = !options.exists( "--no-flush" );
        bool binary = packet_size != 0;
        if( names.empty() ) { std::cerr << "io-publish: please specify at least one file ('-' for stdout)" << std::endl; usage(); }
        if( binary )
        {
            //ProfilerStart( "io-publish.prof" ); {
            comma::io::applications::publish publish( names, n, buffer_size, packet_size, discard );
            while( !is_shutdown && publish.read_bytes() );
            //ProfilerStop(); }
        }
        else
        {
            comma::io::applications::publish publish( names, n, 1, 0, discard, flush );
            while( !is_shutdown && std::cin.good() && !std::cin.eof() ) { publish.read_line(); }
        }
        if( is_shutdown ) { std::cerr << "io-publish: interrupted by signal" << std::endl; }
        return 0;
    }
    catch( std::exception& ex )
    {
        #ifndef WIN32
        if( comma::last_error::value() == EINTR || comma::last_error::value() == EBADF ) { return 0; }
        #endif
        std::cerr << "io-publish: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        #ifndef WIN32
        if( comma::last_error::value() == EINTR || comma::last_error::value() == EBADF ) { return 0; }
        #endif
        std::cerr << "io-publish: unknown exception" << std::endl;
    }
    return 1;
}
