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

#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/csv/options.h>
#include <comma/csv/stream.h>
#include <comma/name_value/parser.h>
#include <comma/csv/applications/play/play.h>
#include <comma/csv/applications/play/multiplay.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "play back timestamped data from standard input in a real time manner" << std::endl;
    std::cerr << "to standard output or optionally into given files/pipes" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: csv-play [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "options" << std::endl;
    std::cerr << "    --speed: speed-up playback by a factor, default is 1 (inverse to --slowdown)" << std::endl;
    std::cerr << "    --slowdown,--slow: slow-down playback by a factor, default is 1 (inverse to --speed)" << std::endl;
    std::cerr << "    --quiet: don't print warnings when lagging behind" << std::endl;
    std::cerr << "    --fields <fields> : specify where timestamp is" << std::endl;
    std::cerr << "                        e.g., if timestamp is the 4th field: --fields=\",,,t\"" << std::endl;
    std::cerr << "                        default: the timestamp is the first field" << std::endl;
    std::cerr << "    --binary <format> : use binary format" << std::endl;
    std::cerr << "    --clients: minimum number of clients to connect to each stream" << std::endl;
    std::cerr << "               before playback starts; default 0" << std::endl;
    std::cerr << "               can be specified individually for each client, e.g." << std::endl;
    std::cerr << "               csv-play file1;pipe;clients=1 file2;tcp:1234;clients=3" << std::endl;
    std::cerr << "    --no-flush : if present, do not flush the output stream ( use on high bandwidth sources )" << std::endl;
    std::cerr << "    --from <timestamp> : play back data starting at <timestamp> ( iso format )" << std::endl;
    std::cerr << "    --to <timestamp> : play back data up to <timestamp> ( iso format )" << std::endl;
    std::cerr << comma::csv::format::usage();
    std::cerr << std::endl;
    std::cerr << "output" << std::endl;
    std::cerr << "    -: write to stdout (default)" << std::endl;
    std::cerr << "    offset=<offset>: add <offset> seconds to the timestamp of this source" << std::endl;
    std::cerr << "    <filename>: write to file or named pipe, e.g. csv-play \"points.csv;pipe\"" << std::endl;
    std::cerr << "    tcp:<port>: open tcp server socket on given port and write to the tcp clients" << std::endl;
    std::cerr << "    local:<name>: same as tcp, but use unix/linux domain sockets" << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples" << std::endl;
    std::cerr << "    output timestamped 3d points in real time manner to stdout (e.g. for visualisation)" << std::endl;
    std::cerr << "        cat points.csv | csv-play | view-points --fields=,x,y,z" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    play back several files and output to, say, named pipes:" << std::endl;
    std::cerr << "        mkfifo file1.pipe file2.pipe" << std::endl;
    std::cerr << "        csv-play \"file1.csv;pipe1\" \"file2.csv;pipe2\" &" << std::endl;
    std::cerr << "        view-points pipe1 pipe2 --fields=,x,y,z" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    same as above, but block, until all the pipes are connected:" << std::endl;
    std::cerr << "        csv-play \"file1.csv;pipe1\" \"file2.csv;pipe2\" --clients=1 &" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    output multiple inputs of the same format to stdout:" << std::endl;
    std::cerr << "        csv-play \"file1.csv;-\" \"file2.csv;-\" &" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

int main( int argc, char** argv )
{
    boost::scoped_ptr< comma::Multiplay > multiPlay;
    try
    {
        const boost::array< comma::signal_flag::signals, 2 > signals = { { comma::signal_flag::sigint, comma::signal_flag::sigterm } };
        comma::signal_flag shutdownFlag( signals );
        comma::command_line_options options( argc, argv );
        if( options.exists( "--help,-h" ) ) { usage(); }
        options.assert_mutually_exclusive( "--speed,--slow,--slowdown" );
        double speed = options.value( "--speed", 1.0 / options.value< double >( "--slow,--slowdown", 1.0 ) );
        unsigned int precision = options.value( "--precision", 10u );
        std::string from = options.value< std::string>( "--from", "" );
        std::string to = options.value< std::string>( "--to", "" );
        bool quiet =  options.exists( "--quiet" );
        bool flush =  !options.exists( "--no-flush" );
        std::vector< std::string > configstrings = options.unnamed("--quiet,--no-flush","--slow,--slowdown,--speed,--precision,--binary,--fields,--clients,--from,--to");
        if( configstrings.empty() ) { configstrings.push_back( "-;-" ); }
        comma::csv::options csvoptions( argc, argv );
        comma::name_value::parser nameValue("filename,output", ';', '=', false );
        std::vector< comma::Multiplay::SourceConfig > sourceConfigs( configstrings.size() );
        comma::Multiplay::SourceConfig defaultConfig( "-", options.value( "--clients", 0 ), csvoptions );
        for( unsigned int i = 0U; i < configstrings.size(); ++i )
        {
            sourceConfigs[i] = nameValue.get< comma::Multiplay::SourceConfig >( configstrings[i], defaultConfig );
        }
        boost::posix_time::ptime fromtime;
        if( !from.empty() )
        {
            fromtime = boost::posix_time::from_iso_string( from );
        }
        boost::posix_time::ptime totime;
        if( !to.empty() )
        {
            totime = boost::posix_time::from_iso_string( to );
        }
        multiPlay.reset( new comma::Multiplay( sourceConfigs, 1.0 / speed, quiet, boost::posix_time::milliseconds(precision), fromtime, totime, flush ) );
        while( multiPlay->read() && !shutdownFlag && std::cout.good() && !std::cout.bad() &&!std::cout.eof() )
        {
            
        }
        multiPlay->close();
        if( shutdownFlag ) { std::cerr << "csv-play: interrupted by signal" << std::endl; return -1; }
        return 0;
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-play: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-play: unknown exception" << std::endl;
    }
    if( multiPlay ) { multiPlay->close(); }
    std::cerr << "reset multiplay" << std::endl;
    multiPlay.reset();
    std::cerr << "done" << std::endl;
    return -1;
}
