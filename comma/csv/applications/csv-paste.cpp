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
#include <sstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <comma/application/command_line_options.h>
#include <comma/base/exception.h>
#include <comma/csv/format.h>
#include <comma/Io/Stream.h>
#include <comma/name_value/parser.h>
#include <comma/string/string.h>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "join several csv sources and output to stdout, e.g.:" << std::endl;
    std::cerr << "    1,2,3 + 4,5 + 6 -> 1,2,3,4,5,6" << std::endl;
    std::cerr << std::endl;
    std::cerr << "in the simplest case, the lines in all the given csv sources" << std::endl;
    std::cerr << "are expected to be exactly in the same order;" << std::endl;
    std::cerr << std::endl;
    std::cerr << "something like:" << std::endl;
    std::cerr << "    csv-paste \"file1.csv\" \"file2.csv\" \"value=<value>\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: csv-paste [<options>] <file> <file>..." << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples:" << std::endl;
    std::cerr << "    csv-paste \"file1.bin;binary=%d%d\" \"file2.bin;size=5\" value=1,2;binary=ui,d" << std::endl;
    std::cerr << "    csv-paste \"file1.csv\" \"file2.csv\" value=1,2,3" << std::endl;
    std::cerr << std::endl;
    std::cerr << "options:" << std::endl;
    std::cerr << "    --delimiter,-d <delimiter> : default ','" << std::endl;
    std::cerr << "    <file> : <filename>[;size=<size>|binary=<format>]: file name or \"-\" for stdin; specify size or format, if binary" << std::endl;
    std::cerr << "    <value> : <csv values>[;binary=<format>]; specify size or format, if binary" << std::endl;
    std::cerr << comma::csv::format::usage() << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

class Source
{
    public:
        Source( const std::string& properties = "" ) : properties_( properties )
        {
            comma::NameValue::Map map( properties, ';', '=' );
            format_ = comma::csv::format( map.value< std::string >( "binary", "" ) );
            unsigned int size = map.value< unsigned int >( "size", format_.size() );
            binary_ = size > 0;
            value_ = std::string( size, 0 );
        }
        virtual ~Source() {}
        virtual const std::string* read() = 0;
        virtual const char* read( char* buf ) = 0;
        bool binary() const { return binary_; }
        const std::string& properties() const { return properties_; }
        std::size_t size() const { return value_.size(); }
        
    protected:
        std::string value_;
        bool binary_;
        comma::csv::format format_;
        std::string properties_;
};

class Stream : public Source
{
    public:
        Stream( const std::string& properties )
            : Source( properties )
			, stream_( comma::split( properties, ';' )[0], binary() ? comma::Io::Mode::binary : comma::Io::Mode::ascii )
        {
        }
        
        const std::string* read()
        {
            while( stream_->good() && !stream_->eof() )
            {
                std::getline( *stream_, value_ );
                if( !value_.empty() && *value_.rbegin() == '\r' ) { value_ = value_.substr( 0, value_.length() - 1 ); } // windows... sigh...
                if( !value_.empty() ) { return &value_; }
            }
            return NULL;
        }

        const char* read( char* buf )
        {
            stream_->read( buf, value_.size() );
            return stream_->gcount() == int( value_.size() ) ? buf : NULL;
        }
        
    private:
        comma::Io::IStream stream_;
};

struct Value : public Source
{
    Value( const std::string& properties ) : Source( properties )
    {
        comma::NameValue::Map map( properties, ';', '=' );
        std::string value = map.value< std::string >( "value" );
        char delimiter = map.value( "delimiter", ',' );
        value_ = binary_ ? format_.csv_to_bin( value, delimiter ) : value;
    }
    const std::string* read() { return &value_; }
    const char* read( char* buf ) { ::memcpy( buf, &value_[0], value_.size() ); return buf; } // quick and dirty
};

int main( int ac, char** av )
{
    bool show_usage = true;
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h" ) ) { usage(); }
        char delimiter = options.value( "--delimiter,-d", ',' );
        std::vector< std::string > unnamed = options.unnamed( "", "--delimiter,-d" );
        boost::ptr_vector< Source > sources;
        Source* source;
        for( unsigned int i = 0; i < unnamed.size(); ++i )
        {
            if( unnamed[i].substr( 0, 6 ) == "value=" ) { source = new Value( unnamed[i] ); }
            else { source = new Stream( unnamed[i] ); }
            if( i > 0 && sources.back().binary() != source->binary() ) { std::cerr << "csv-paste: one input is ascii, the other binary: " << sources.back().properties() << " vs " << source->properties() << std::endl; usage(); }
            sources.push_back( source );
        }
        if( sources.empty() ) { usage(); }
        show_usage = false;
        if( sources.back().binary() )
        {
            #ifdef WIN32
                _setmode( _fileno( stdin ), _O_BINARY );
                _setmode( _fileno( stdout ), _O_BINARY );
            #endif
            std::size_t size = 0;
            for( unsigned int i = 0; i < sources.size(); ++i ) { size += sources[i].size(); }
            std::vector< char > buffer( size );
            while( true )
            {
                char* p = &buffer[0];
                for( unsigned int i = 0; i < sources.size(); p += sources[i].size(), ++i )
                {
                    if( sources[i].read( p ) == NULL )
                    {
                        if( i == 0 ) { return 0; }
                        std::cerr << "csv-paste: unexpected end of file in " << unnamed[i] << std::endl; return 1;
                    }
                }
                std::cout.write( &buffer[0], buffer.size() );
                std::cout.flush();
            }
        }
        else
        {
            while( true )
            {
                std::ostringstream oss;
                for( unsigned int i = 0; i < sources.size(); ++i )
                {
                    const std::string* s = sources[i].read();
                    if( s == NULL )
                    {
                        if( i == 0 ) { return 0; }
                        std::cerr << "csv-paste: unexpected end of file in " << unnamed[i] << std::endl; return 1;
                    }
                    if( i > 0 ) { oss << delimiter; }
                    oss << *s;
                }
                std::cout << oss.str() << std::endl;
            }
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-paste: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-paste: unknown exception" << std::endl;
    }
    if( show_usage ) { usage(); }
}


// int main( int ac, char** av )
// {
//     bool show_usage = true;
//     try
//     {
//         comma::command_line_options options( ac, av );
//         if( options.exists( "--help,-h" ) ) { usage(); }
//         char delimiter = options.value( "--delimiter,-d", ',' );
//         std::vector< std::string > unnamed = options.unnamed( "", "--delimiter,-d" );
//         boost::ptr_vector< std::istream > files;
//         std::vector< std::pair< std::istream*, std::size_t > > sources;
//         bool binary = false;
//         for( unsigned int i = 0; i < unnamed.size(); ++i )
//         {
//             std::string filename = unnamed[i];
//             std::size_t size = 0;
//             std::vector< std::string > v = comma::split( unnamed[i], ';' );
//             filename = v[0];
//             for( std::size_t j = 1; j < v.size(); ++j )
//             {
//                 std::vector< std::string > w = comma::split( v[j], '=' );
//                 if( w.size() != 2 ) { COMMA_THROW( comma::exception, "expected filename and options, got \"" << unnamed[i] << "\"" ); }
//                 if( w[0] == "binary" )
//                 {
//                     if( i == 0 ) { binary = true; }
//                     else if( !binary ) { COMMA_THROW( comma::exception, unnamed[0] << " is ascii, but " << filename << " is binary" ); }
//                     size = comma::csv::format( w[1] ).size();
//                 }
//                 else if( w[0] == "size" )
//                 {
//                     if( i == 0 ) { binary = true; }
//                     else if( !binary ) { COMMA_THROW( comma::exception, unnamed[0] << " is ascii, but " << filename << " is binary" ); }
//                     size = boost::lexical_cast< std::size_t >( w[1] );
//                 }
//             }
//             if( binary && size == 0 ) { COMMA_THROW( comma::exception, "in binary mode, please specify size or format for \"" << filename << "\"" ); }
//             if( filename == "-" )
//             {
//                 sources.push_back( std::make_pair( &std::cin, size ) );
//             }
//             else
//             {
//                 files.push_back( new std::ifstream( filename.c_str() ) );
//                 if( !files.back().good() || files.back().eof() ) { COMMA_THROW( comma::exception, "failed to open " << unnamed[i] ); }
//                 sources.push_back( std::make_pair( &files.back(), size ) );
//             }
//         }
//         if( sources.empty() ) { usage(); }
//         #ifdef WIN32
//         if( binary ) { _setmode( _fileno( stdin ), _O_BINARY ); }
//         #endif
//         show_usage = false;
//         if( binary )
//         {
//             std::size_t size = 0;
//             for( unsigned int i = 0; i < sources.size(); ++i ) { size += sources[i].second; }
//             while( true )
//             {
//                 for( unsigned int i = 0; i < sources.size(); ++i )
//                 {
//                     std::string s( sources[i].second, 0 );
//                     char* buf = &s[0];
//                     sources[i].first->read( buf, sources[i].second );
//                     int count = sources[i].first->gcount();
//                     if( count != 0 && (unsigned int)count != sources[i].second ) { COMMA_THROW( comma::exception, unnamed[i] << ": expected " << sources[i].second << " bytes, got " << count ); }
//                     if( !sources[i].first->good() || sources[i].first->eof() )
//                     {
//                         bool ok = true;
//                         for( unsigned int j = 0; j < sources.size() && ok; ++j )
//                         {
//                             if( j > i ) { sources[j].first->peek(); }
//                             ok = !sources[j].first->good() || sources[j].first->eof();
//                         }
//                         if( ok ) { return 0; }
//                         else { COMMA_THROW( comma::exception, unnamed[i] << ": unexpected end of file" ); }
//                     }
//                     std::cout << s;
//                 }
//             }
//         }
//         else
//         {
//             while( true )
//             {
//                 bool first = true;
//                 for( unsigned int i = 0; i < sources.size(); ++i )
//                 {
//                     std::string s;
//                     std::getline( *sources[i].first, s );
//                     if( !sources[i].first->good() || sources[i].first->eof() )
//                     {
//                         bool ok = true;
//                         for( unsigned int j = 0; j < sources.size() && ok; ++j )
//                         {
//                             if( j > i ) { sources[j].first->peek(); }
//                             ok = !sources[j].first->good() || sources[j].first->eof();
//                         }
//                         if( ok ) { return 0; }
//                         else { COMMA_THROW( comma::exception, unnamed[i] << ": unexpected end of file" ); }
//                     }
//                     if( !s.empty() && *s.rbegin() == '\r' ) { s = s.substr( 0, s.length() - 1 ); } // windows... sigh...
//                     if( s.empty() ) { continue; }
//                     if( !first ) { std::cout << delimiter; } else { first = false; }
//                     std::cout << s;
//                 }
//                 std::cout << std::endl;
//             }
//         }
//     }
//     catch( std::exception& ex )
//     {
//         std::cerr << "csv-paste: " << ex.what() << std::endl;
//     }
//     catch( ... )
//     {
//         std::cerr << "csv-paste: unknown exception" << std::endl;
//     }
//     if( show_usage ) { usage(); }
// }
