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

#include <string.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/types.h>
#include <comma/csv/stream.h>
#include <comma/io/stream.h>
#include <comma/name_value/parser.h>
#include <comma/string/string.h>
#include <comma/visiting/traits.h>

static void usage( bool long_help = false )
{
    std::cerr << std::endl;
    std::cerr << "join two csv files or streams by one or several keys (integer only for now)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat something.csv csv-join \"something_else.csv[,options]\" [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "    fields:" << std::endl;
    std::cerr << "        block: block number" << std::endl;
    std::cerr << "        any other field names: keys" << std::endl;
    std::cerr << std::endl;
    std::cerr << "options:" << std::endl;
    //std::cerr << "    --long-help: more help" << std::endl;
    std::cerr << "    --first-matching: output only the first matching record (a bit of hack for now, but we needed it)" << std::endl;
    std::cerr << "    --verbose,-v: more output to stderr" << std::endl;
    std::cerr << comma::csv::options::usage() << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples:" << std::endl;
    std::cerr << "    todo" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

struct input
{
    enum { size = 16 }; // quick and dirty: up to 16 keys
    boost::array< comma::uint64, size > keys;
    comma::uint32 block;
 
    static unsigned int keys_size;
    
    struct Hash;
    
    input() : block( 0 ) {}
    
    bool operator==( const input& rhs ) const
    {
        for( std::size_t i = 0; i < keys_size; ++i ) { if( keys[i] != rhs.keys[i] ) { return false; } }
        return true;
    }
    
    bool operator<( const input& rhs ) const
    {
        for( std::size_t i = 0; i < keys_size; ++i ) { if( keys[i] < rhs.keys[i] ) { return true; } }
        return false;
    }
};

unsigned int input::keys_size = 0;

namespace comma { namespace visiting {

template <> struct traits< input >
{
    template < typename K, typename V > static void visit( const K&, const input& p, V& v )
    { 
        v.apply( "keys", p.keys );
        v.apply( "block", p.block );
    }
    template < typename K, typename V > static void visit( const K&, input& p, V& v )
    { 
        v.apply( "keys", p.keys );
        v.apply( "block", p.block );
    }
};

} } // namespace comma { namespace visiting {

static bool verbose;
static comma::signal_flag is_shutdown;
static boost::scoped_ptr< comma::io::istream > filter_transport;
static boost::scoped_ptr< comma::csv::input_stream< input > > stdin_stream;
static boost::scoped_ptr< comma::csv::input_stream< input > > filter_stream;
static comma::csv::options stdin_csv;
static comma::csv::options filter_csv;
static bool first_matching;

struct input::Hash : public std::unary_function< input, std::size_t >
{
    std::size_t operator()( input const& p ) const
    {
        std::size_t seed = 0;
        for( std::size_t i = 0; i < input::keys_size; ++i ) { boost::hash_combine( seed, p.keys[i] ); }
        return seed;
    }
};

//typedef boost::unordered_map< input, std::deque< std::string >, input::Hash > FilterMap;
typedef boost::unordered_map< input, std::vector< std::string >, input::Hash > FilterMap;
FilterMap filter_map;
static comma::uint32 block;

void read_filter_block_()
{
    static const input* last = filter_stream->read();
    block = last->block;
    filter_map.clear();
    comma::uint64 count = 0;
    while( last->block == block && !is_shutdown && ( *filter_transport )->good() && !( *filter_transport )->eof() )
    {
        if( filter_stream->is_binary() )
        {
            FilterMap::mapped_type& d = filter_map[ *last ];
            d.push_back( std::string() );
            d.back().resize( filter_csv.format().size() );
            ::memcpy( &d.back()[0], filter_stream->binary().last(), filter_csv.format().size() );
        }
        else
        {
            filter_map[ *last ].push_back( comma::join( filter_stream->ascii().last(), stdin_csv.delimiter ) );
        }
        
        if( verbose ) { ++count; if( count % 10000 == 0 ) { std::cerr << "csv-join: reading block " << block << "; loaded " << count << " point[s]; hash map size: " << filter_map.size() << std::endl; } }
        last = filter_stream->read();
        if( !last ) { break; }
    }
    if( verbose ) { std::cerr << "csv-join: read block " << block << " of " << count << " point[s]; hash map size: " << filter_map.size() << std::endl; }
}

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h,--long-help" ) ) { usage( options.exists( "--long-help" ) ); }
        verbose = options.exists( "--verbose,-v" );
        first_matching = options.exists( "--first-matching" );
        stdin_csv = comma::csv::options( options );
        std::vector< std::string > unnamed = options.unnamed( "--verbose,-v,--first-matching", "--binary,-b,--delimiter,-d,--fields,-f" );
        if( unnamed.empty() ) { std::cerr << "csv-join: please specify the second source" << std::endl; return 1; }
        if( unnamed.size() > 1 ) { std::cerr << "csv-join: expected one file or stream to join, got " << comma::join( unnamed, ' ' ) << std::endl; return 1; }
        comma::name_value::parser parser( "filename", ';', '=', false );
        filter_csv = parser.get< comma::csv::options >( unnamed[0] );
        if( stdin_csv.binary() != filter_csv.binary() ) { std::cerr << "csv-join: expected both streams ascii or both streams binary" << std::endl; return 1; }
        std::vector< std::string > v = comma::split( stdin_csv.fields, ',' );
        std::vector< std::string > w = comma::split( filter_csv.fields, ',' );
        for( std::size_t i = 0; i < v.size(); ++i ) // quick and dirty, wasteful, but who cares
        { 
            if( v[i].empty() || v[i] == "block" ) { continue; }
            for( std::size_t k = 0; k < w.size(); ++k )
            {
                if( v[i] != w[k] ) { continue; }
                v[i] = "keys[" + boost::lexical_cast< std::string >( input::keys_size ) + "]";
                w[k] = "keys[" + boost::lexical_cast< std::string >( input::keys_size ) + "]";
                ++input::keys_size;
            }
        }
        if( input::keys_size == 0 ) { std::cerr << "csv-join: please specify at least one common key" << std::endl; return 1; }
        stdin_csv.fields = comma::join( v, ',' );
        filter_csv.fields = comma::join( w, ',' );
        stdin_stream.reset( new comma::csv::input_stream< input >( std::cin, stdin_csv ) );
        filter_transport.reset( new comma::io::istream( filter_csv.filename, filter_csv.binary() ? comma::io::mode::binary : comma::io::mode::ascii ) );
        filter_stream.reset( new comma::csv::input_stream< input >( **filter_transport, filter_csv ) );
        std::size_t discarded = 0;
        read_filter_block_();
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            const input* p = stdin_stream->read();
            if( !p ) { break; }
            if( block != p->block ) { read_filter_block_(); }
            if( filter_map.empty() ) { break; }
            FilterMap::const_iterator it = filter_map.find( *p );
            if( it == filter_map.end() || it->second.empty() ) { ++discarded; continue; }
            if( stdin_stream->is_binary() )
            {
                for( std::size_t i = 0; i < ( first_matching ? 1 : it->second.size() ); ++i )
                {
                    std::cout.write( stdin_stream->binary().last(), stdin_csv.format().size() );
                    std::cout.write( &( it->second[i][0] ), filter_csv.format().size() );
                    std::cout.flush();
                }
                std::cout.flush();
            }
            else
            {
                for( std::size_t i = 0; i < ( first_matching ? 1 : it->second.size() ); ++i )
                {
                    std::cout << comma::join( stdin_stream->ascii().last(), stdin_csv.delimiter ) << stdin_csv.delimiter << it->second[i] << std::endl;
                }
            }
            if( first_matching ) { filter_map.erase( it->first ); } // quick and dirty for now
        }
        if( verbose ) { std::cerr << "csv-join: discarded " << discarded << " entrie[s] with no matches" << std::endl; }
        return 0;
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-join: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-join: unknown exception" << std::endl;
    }
    return 1;
}
