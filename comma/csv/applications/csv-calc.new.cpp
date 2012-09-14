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
#include <comma/Application/command_line_options.h>
#include <comma/Application/SignalFlag.h>
#include <comma/Base/Types.h>
#include <comma/csv/Stream.h>
#include <comma/Io/Stream.h>
#include <comma/NameValue/Parser.h>
#include <comma/String/String.h>
#include <comma/visiting/traits.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "column-wise calculation, optionally by id and block" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat data.csv | csv-extents <what> [<options>] > extents.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<what>: comma-separated list of operations" << std::endl;
    std::cerr << "        results will be output in the same order" << std::endl;
    std::cerr << "        optionally followed by block,id (both as ui, if binary)" << std::endl;
    std::cerr << "    min: minimum" << std::endl;
    std::cerr << "    max: maximum" << std::endl;
    std::cerr << "    mean: mean value" << std::endl;
    std::cerr << "    centre: ( min + max ) / 2" << std::endl;
    std::cerr << "    diameter: max - min" << std::endl;
    std::cerr << "    radius: size / 2" << std::endl;
    std::cerr << "    var: variance" << std::endl;
    std::cerr << "    stddev: standard deviation" << std::endl;
    std::cerr << "    size: number of values" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << "    --delimiter,-d <delimiter> : default ','" << std::endl;
    std::cerr << "    --fields,-f: field names for which the extents should be computed, default: all fields" << std::endl;
    std::cerr << "                 if 'block' field present, calculate block-wise" << std::endl;
    std::cerr << "                 if 'id' field present, calculate by id" << std::endl;
    std::cerr << "                 if 'block' and 'id' fields present, calculate by id in each block" << std::endl;
    std::cerr << "                 block and id fields will be appended to the output" << std::endl;
    std::cerr << "    --format: in ascii mode: format hint string containing the types of the csv data, default: double or time" << std::endl;
    std::cerr << "    --binary,-b: in binary mode: format string of the csv data types" << std::endl;
    std::cerr << comma::csv::format::usage() << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples" << std::endl;
    std::cerr << "    todo" << std::endl;
    std::cerr << std::endl;
    exit( 1 );
}

struct Input
{
    std::vector< comma::uint64 > keys;
    std::vector< double > doubles;
    std::vector< boost::posix_time::ptime > times;
    comma::uint32 block;
 
    static unsigned int keys_size;
    static unsigned int doubles_size;
    static unsigned int times_size;
    
    struct Hash;
    
    Input() : block( 0 ), keys( keys_size ), doubles( doubles_size ), times( times_size ) {}
    
    bool operator==( const Input& rhs ) const
    {
        for( std::size_t i = 0; i < keys.size(); ++i ) { if( keys[i] != rhs.keys[i] ) { return false; } }
        return true;
    }
};

unsigned int Input::keys_size = 0;
unsigned int Input::doubles_size = 0;
unsigned int Input::times_size = 0;

namespace comma { namespace visiting {

template <> struct traits< Input >
{
    template < typename K, typename V > static void visit( const K&, const Input& p, V& v )
    { 
        v.apply( "keys", p.keys );
        v.apply( "doubles", p.doubles );
        v.apply( "times", p.times );
        v.apply( "block", p.block );
    }
    template < typename K, typename V > static void visit( const K&, Input& p, V& v )
    { 
        v.apply( "keys", p.keys );
        v.apply( "doubles", p.doubles );
        v.apply( "times", p.times );
        v.apply( "block", p.block );
    }
};

} } // namespace comma { namespace visiting {

static bool verbose;
static comma::SignalFlag is_shutdown;
static boost::scoped_ptr< comma::csv::input_stream< Input > > stdin_stream;
static comma::csv::Options stdin_csv;

struct Input::Hash : public std::unary_function< Input, std::size_t >
{
    std::size_t operator()( Input const& p ) const
    {
        std::size_t seed = 0;
        for( std::size_t i = 0; i < Input::keys_size; ++i ) { boost::hash_combine( seed, p.keys[i] ); }
        return seed;
    }
};

typedef boost::unordered_map< Input, std::vector< std::string >, Input::Hash > Map;
static Map map;
static comma::uint32 block;
static std::string input_fields;
static std::string input_format_string;
static std::string output_fields;
static std::string output_format_string;

static void set_fields_( const std::string& fields, const std::string& sample, char delimiter )
{
    std::vector< std::string > v = comma::split( fields, ',' );
    std::vector< std::string > w = comma::split( sample, delimiter );
    for( unsigned int i = 0; i < v.size(); ++i )
    {
        if( v[i] == "block" ) { continue; }
        bool is_id = v[i].length() > 2 && v[i].substr( 0, 2 ) == "id";
        if( is_id )
        {
            try { boost::lexical_cast<>( v[i].substr( 2, v[i].length() - 2 ) ); }
            catch ( ... ) { is_id = false; }
        }
        if( is_id )
        {
            v[i] = "keys[" + boost::lexical_cast< std::string >( Input::keys_size ) + "]";
            ++Input::keys_size;
        }
        else if( v[i] == "id" )
        {
            v[i] = "keys[0]";
            ++Input::keys_size;
        }
        //else if(        
}

std::string set_fields_ascii( const std::string& fields, const std::string& sample, char delimiter = ',' )
{
    std::vector< std::string > v = comma::split( sample, delimiter );
    std::vector< std::string > w = comma::split( fields, ',' );
    for( unsigned int i = 0; i < v.size(); ++i )
    {
        if( w[i] == "block" ) { continue; }
        if( w[i] == "id" )
        {
            w[i] = "keys[0]";
            ++Input::keys_size;
        }
        if( w[i].substr( 0, 2 ) == "id" )
        { 
            w[i] = "keys[" + boost::lexical_cast< std::string >( Input::keys_size ) + "]";
            ++Input::keys_size;
        }
        if( ( block_index_ && *block_index_ == i ) || ( id_index_ && *id_index_ == i ) ) { input_format_ += "ui"; continue; }
        try { boost::posix_time::frois_o_string( v[i] ); input_format_ += "t"; }
        catch( ... ) { input_format_ += "d"; }
    }
    std::cerr << "csv-calc: guessed format: " << input_format_.string() << std::endl;
}

void read_block_()
{
    static const Input* last = filter_stream->read();
    block = last->block;
    map.clear();
    while( last->block == block && !is_shutdown && ( *filter_transport )->good() && !( *filter_transport )->eof() )
    {
        std::string value;
        if( filter_stream->isbinary() )
        {
            value.resize( filter_stream->binary().size() );
            ::memcpy( &value[0], filter_stream->binary().last(), filter_stream->binary().size() );
        }
        else
        {
            value = comma::join( filter_stream->ascii().last(), stdin_csv.delimiter );
        }
        map[ *last ].push_back( value );
        last = filter_stream->read();
        if( !last ) { break; }
    }
}

std::vector< std::string > matching_( const Input* p )
{
    static std::vector< std::string > empty;
    FilterMap::const_iterator it = map.find( *p );
    return it == map.end() ? empty : it->second;
}

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h,--long-help" ) ) { usage( options.exists( "--long-help" ) ); }
        verbose = options.exists( "--verbose,-v" );
        stdin_csv = comma::csv::Options( options );
        std::vector< std::string > unnamed = options.unnamed( "--verbose,-v", "--binary,-b,--delimiter,-d,--fields,-f" );
        if( unnamed.empty() ) { std::cerr << "csv-calc: please specify the second source" << std::endl; return 1; }
        if( unnamed.size() > 1 ) { std::cerr << "csv-calc: expected one file or stream to join, got " << comma::join( unnamed, ' ' ) << std::endl; return 1; }
        comma::NameValue::Parser parser( "filename", ';', '=', false );
        comma::csv::Options filter_csv = parser.get< comma::csv::Options >( unnamed[0] );
        if( stdin_csv.binary() != filter_csv.binary() ) { std::cerr << "csv-calc: expected both streams ascii or both streams binary" << std::endl; return 1; }
        std::vector< std::string > v = comma::split( stdin_csv.fields, ',' );
        std::vector< std::string > w = comma::split( filter_csv.fields, ',' );
        for( std::size_t i = 0; i < v.size(); ++i ) // quick and dirty, wasteful, but who cares
        { 
            if( v[i].empty() || v[i] == "block" ) { continue; }
            for( std::size_t k = 0; k < w.size(); ++k )
            {
                if( v[i] != w[k] ) { continue; }
                v[i] = "keys[" + boost::lexical_cast< std::string >( i ) + "]";
                w[k] = "keys[" + boost::lexical_cast< std::string >( k ) + "]";
                ++Input::keys_size;
            }
        }
        if( Input::keys_size == 0 ) { std::cerr << "csv-calc: please specify at least one common key" << std::endl; return 1; }
        stdin_csv.fields = comma::join( v, ',' );
        filter_csv.fields = comma::join( w, ',' );
        stdin_stream.reset( new comma::csv::input_stream< Input >( std::cin, stdin_csv ) );
        filter_transport.reset( new comma::Io::IStream( filter_csv.filename, filter_csv.binary() ? comma::Io::Mode::binary : comma::Io::Mode::ascii ) );
        filter_stream.reset( new comma::csv::input_stream< Input >( **filter_transport, filter_csv ) );
        std::size_t discarded = 0;
        read_block_();
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            const Input* p = stdin_stream->read();
            if( !p ) { break; }
            if( block != p->block ) { read_block_(); }
            if( map.empty() ) { break; }
            const std::vector< std::string > matching = matching_( p );
            if( matching.empty() ) { ++discarded; }
            if( stdin_stream->isbinary() )
            {
                for( std::size_t i = 0; i < matching.size(); ++i )
                {
                    std::cout.write( stdin_stream->binary().last(), stdin_csv.format().size() );
                    std::cout.write( &matching[i][0], filter_csv.format().size() );
                }
                std::cout.flush();
            }
            else
            {
                for( std::size_t i = 0; i < matching.size(); ++i )
                {
                    std::cout << comma::join( stdin_stream->ascii().last(), stdin_csv.delimiter ) << stdin_csv.delimiter << matching[i] << std::endl;
                }
            }
        }
        if( verbose ) { std::cerr << "csv-calc: discarded " << discarded << " entries with no matches" << std::endl; }
        return 0;
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-calc: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-calc: unknown exception" << std::endl;
    }
    return 1;
}
