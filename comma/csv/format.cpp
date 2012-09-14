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

#include <sstream>
#include <string.h>
#include <time.h>
#include <cmath>
#include <sstream>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include <comma/base/exception.h>
#include <comma/base/types.h>
#include <comma/string/string.h>
#include <comma/csv/format.h>
#include "./impl/epoch.h"

namespace comma { namespace csv {

format::format( const std::string& f )
    : string_( f )
    , size_( 0 )
    , count_( 0 )
{
    std::string format = comma::strip( f, " \t\r\n" );
    if( format == "" ) { return; }
    format = comma::strip( format, "%" );
    std::vector< std::string > v = comma::split( format, ",%" );
    std::size_t offset = 0;
    for( unsigned int i = 0; i < v.size(); ++i )
    {
        std::string s;
        for( ; s.length() < v[i].length() && v[i][ s.length() ] >= '0' && v[i][ s.length() ] <= '9'; s += v[i][ s.length() ] );
        if( s.length() >= v[i].length() ) { COMMA_THROW( comma::exception, "expected format, got '" << v[i] << "' in " << format ); }
        std::size_t arraySize = s.empty() ? 1 : boost::lexical_cast< std::size_t >( s );
        std::string type = v[i].substr( s.length() );
        types_enum t;
        unsigned int size;
        if( type == "b" ) { t = format::int8; size = 1; }
        else if( type == "ub" ) { t = format::uint8; size = 1; }
        else if( type == "w" ) { t = format::int16; size = 2; }
        else if( type == "uw" ) { t = format::uint16; size = 2; }
        else if( type == "i" ) { t = format::int32; size = 4; }
        else if( type == "ui" ) { t = format::uint32; size = 4; }
        else if( type == "l" ) { t = format::int64; size = 8; }
        else if( type == "ul" ) { t = format::uint64; size = 8; }
        else if( type == "c" ) { t = format::char_t; size = 1; }
        else if( type == "t" ) { t = format::time; size = sizeof( comma::int64 ); }
        else if( type == "lt" ) { t = format::long_time; size = sizeof( comma::int32 ) + sizeof( comma::int64 ); }
        else if( type == "f" ) { t = format::float_t; size = sizeof( float ); }
        else if( type == "d" ) { t = format::double_t; size = sizeof( double ); }
        else if( type[0] == 's' && type.length() == 1 ) { COMMA_THROW( comma::exception, "got variable size string in [" << format << "]: not implemented, use fixed size string instead, e.g. \"s[8]\"" ); }
        else if( type[0] == 's' && type.length() > 3 && type[1] == '[' && *type.rbegin() == ']' )
        {
            t = format::fixed_string;
            size = boost::lexical_cast< std::size_t >( type.substr( 2, type.length() - 3 ) );
        }
        else { COMMA_THROW( comma::exception, "expected format, got '" << type << "' in " << format ); }
        elements_.push_back( element( offset, arraySize, size, t ) );
        count_ += arraySize;
        size *= arraySize;
        offset += size;
        size_ += size;
    }
}
 
const std::string& format::string() const { return string_; }

std::string format::to_format( types_enum type, unsigned int size )
{
    std::ostringstream oss;
    oss << to_format( type );
    if( type == format::fixed_string ) { oss << "[" << size << "]"; }
    return oss.str();
}

std::string format::to_format( format::types_enum type )
{
    switch( type )
    {
        case format::int8: return "b";
        case format::uint8: return "ub";
        case format::int16: return "w";
        case format::uint16: return "uw";
        case format::int32: return "i";
        case format::uint32: return "ui";
        case format::int64: return "l";
        case format::uint64: return "ul";
        case format::char_t: return "c";
        case format::float_t: return "f";
        case format::double_t: return "d";
        case format::time: return "t";
        case format::long_time: return "lt";
        case format::fixed_string: return "s";
    }
    COMMA_THROW( comma::exception, "expected type, got " << type );
}

std::size_t format::size() const { return size_; }

std::size_t format::count() const { return count_; }

static boost::array< unsigned int, 12 > Sizesimpl()
{
    boost::array< unsigned int, 12 > sizes;
    sizes[ format::char_t ] = sizeof( char );
    sizes[ format::int8 ] = sizeof( char );
    sizes[ format::uint8 ] = sizeof( unsigned char );
    sizes[ format::int16 ] = sizeof( int16 );
    sizes[ format::uint16 ] = sizeof( uint16 );
    sizes[ format::int32 ] = sizeof( int32 );
    sizes[ format::uint32 ] = sizeof( uint32 );
    sizes[ format::int64 ] = sizeof( int64 );
    sizes[ format::uint64 ] = sizeof( uint64 );
    sizes[ format::float_t ] = sizeof( float );
    sizes[ format::double_t ] = sizeof( double );
    sizes[ format::time ] = sizeof( int64 );
    sizes[ format::long_time ] = sizeof( int64 ) + sizeof( int32 );
    sizes[ format::fixed_string ] = 0; // will it blast somewhere?
    return sizes;
}

const format& format::operator+=( const std::string& rhs ) // quick and dirty
{
    if( rhs == "" ) { return *this; }
    const std::string& lhs = string();
    return operator=( format( lhs == "" ? rhs : lhs + "," + rhs ) );
}

const format& format::operator+=( const format& rhs )
{
    return operator+=( rhs.string() );
}

std::string format::usage()
{
    std::ostringstream oss;
    oss << "        <format> : e.g. \"%t%d%d%d%ul\" or \"t,d,d,d,ul\" (same)" << std::endl
        << "            b  : byte (8-bit int)" << std::endl
        << "            ub : unsigned byte (unsigned 8-bit int)" << std::endl
        << "            w  : 16-bit int (not implemented)" << std::endl
        << "            uw : unsigned 16-bit int" << std::endl
        << "            i  : 32-bit int (not implemented)" << std::endl
        << "            ui : unsigned 32-bit int" << std::endl
        << "            l  : 64-bit int (not implemented)" << std::endl
        << "            ul : unsigned 64-bit int" << std::endl
        << "            c  : char" << std::endl
        << "            f  : float" << std::endl
        << "            d  : double" << std::endl
        << "            s  : variable size string (not implemented)" << std::endl
        << "            s[<length>]  : fixed size string, e.g. \"s[4]\"" << std::endl
        << "            t  : time (64-bit signed int, number of microseconds since epoch)" << std::endl
        << "            lt  : time (64+32 bit, seconds since epoch and nanoseconds)" << std::endl;
    return oss.str();
}

std::size_t format::sizeOf( types_enum type ) // todo: returns 0 for fixed size string, which is lame
{
    static boost::array< unsigned int, 12 > sizes = Sizesimpl();
    return sizes[ static_cast< std::size_t >( type ) ];
}

namespace impl {

template < typename T >
static std::size_t csv_to_bin( char* buf, const std::string& s )
{
    // T t = boost::lexical_cast< T >( s );
    //::memcpy( buf, &t, sizeof( T ) );
    *reinterpret_cast< T* >( buf ) = boost::lexical_cast< T >( s );
    return sizeof( T );
}

template < typename T >
static void withPrecision( std::ostringstream& oss, T t, const boost::optional< unsigned int >& ) { oss << t; }

static void withPrecision( std::ostringstream& oss, float t, const boost::optional< unsigned int >& precision )
{
    oss.precision( precision ? *precision : 6 );
    oss << t;
}

static void withPrecision( std::ostringstream& oss, double t, const boost::optional< unsigned int >& precision )
{
    oss.precision( precision ? *precision : 16 );
    oss << t;
}

template < typename T >
static std::size_t bin_to_csv( std::ostringstream& oss, const char* buf, const boost::optional< unsigned int >& precision )
{
    //T t;
    //::memcpy( &t, buf, sizeof( T ) );
    //withPrecision( oss, t, precision );
    withPrecision( oss, *reinterpret_cast< const T* >( buf ), precision );
    return sizeof( T );
}

static std::size_t csv_to_bin( char* buf, const std::string& s, format::types_enum type, std::size_t size )
{
    try
    {
        switch( type ) // todo: tear down csv_to_bin, use format::traits
        {
            case format::int8:
            {
                int i = boost::lexical_cast< int >( s );
                if( i < -127 || i > 128 ) { COMMA_THROW( comma::exception, "expected byte, got " << i ); }
                *buf = static_cast< char >( i );
                return sizeof( char );
            }
            case format::uint8:
            {
                unsigned int i = boost::lexical_cast< unsigned int >( s );
                if( i > 255 ) { COMMA_THROW( comma::exception, "expected unsigned byte, got " << i ); }
                //unsigned char c = static_cast< unsigned char >( i );
                //::memcpy( buf, &c, 1 );
                *buf = static_cast< unsigned char >( i );
                return sizeof( unsigned char );
            }
            case format::int16: return csv_to_bin< comma::int16 >( buf, s );
            case format::uint16: return csv_to_bin< comma::uint16 >( buf, s );
            case format::int32: return csv_to_bin< comma::int32 >( buf, s );
            case format::uint32: return csv_to_bin< comma::uint32 >( buf, s );
            case format::int64: return csv_to_bin< comma::int64 >( buf, s );
            case format::uint64: return csv_to_bin< comma::uint64 >( buf, s );
            case format::char_t: return csv_to_bin< char >( buf, s );
            case format::float_t: return csv_to_bin< float >( buf, s );
            case format::double_t: return csv_to_bin< double >( buf, s );
            case format::time: // TODO: quick and dirty: use serialization traits
                format::traits< boost::posix_time::ptime, format::time >::to_bin( boost::posix_time::from_iso_string( s ), buf );
                return format::traits< boost::posix_time::ptime, format::time >::size;
            case format::long_time: // TODO: quick and dirty: use serialization traits
                format::traits< boost::posix_time::ptime, format::long_time >::to_bin( boost::posix_time::from_iso_string( s ), buf );
                return format::traits< boost::posix_time::ptime, format::long_time >::size;
            case format::fixed_string:
                if( s.length() > size ) { COMMA_THROW( comma::exception, "expected string not longer than " << size << "; got \"" << s << "\"" ); }
                ::memset( buf, 0, size );
                if( !s.empty() ) { ::memcpy( buf, &s[0], s.length() ); }
                return size;
            default: COMMA_THROW( comma::exception, "todo: not implemented" );
        }
    }
    catch( std::exception& ex )
    {
        COMMA_THROW( comma::exception, "for [" << s << "]: " << ex.what() );
    }
    catch( ... )
    {
        throw;
    }
}

static std::size_t bin_to_csv( std::ostringstream& oss, const char* buf, format::types_enum type, std::size_t size, const boost::optional< unsigned int >& precision )
{
    switch( type ) // todo: tear down bin_to_csv, use format::traits
    {
        case format::int8:
            oss << static_cast< int >( *buf );
            return sizeof( char );
        case format::uint8:
            oss << static_cast< unsigned int >( static_cast< unsigned char >( *buf ) );
            return sizeof( unsigned char );
        case format::int16: return bin_to_csv< comma::int16 >( oss, buf, precision );
        case format::uint16: return bin_to_csv< comma::uint16 >( oss, buf, precision );
        case format::int32: return bin_to_csv< comma::int32 >( oss, buf, precision );
        case format::uint32: return bin_to_csv< comma::uint32 >( oss, buf, precision );
        case format::int64: return bin_to_csv< comma::int64 >( oss, buf, precision );
        case format::uint64: return bin_to_csv< comma::uint64 >( oss, buf, precision );
        case format::char_t: return bin_to_csv< char >( oss, buf, precision );
        case format::float_t: return bin_to_csv< float >( oss, buf, precision );
        case format::double_t: return bin_to_csv< double >( oss, buf, precision );
        case format::time:
            oss << boost::posix_time::to_iso_string( format::traits< boost::posix_time::ptime, format::time >::from_bin( buf, sizeof( comma::uint64 ) ) );
            return format::traits< boost::posix_time::ptime, format::time >::size;
        case format::long_time:
            oss << boost::posix_time::to_iso_string( format::traits< boost::posix_time::ptime, format::long_time >::from_bin( buf, sizeof( comma::uint64 ) + sizeof( comma::uint32 ) ) );
            return format::traits< boost::posix_time::ptime, format::long_time >::size;
        case format::fixed_string:
            oss << ( buf[ size - 1 ] == 0 ? std::string( buf ) : std::string( buf, size ) );
            return size;
        default : COMMA_THROW( comma::exception, "Todo: not implemented" );
    }
}

} // namespace impl {

void format::csv_to_bin( std::ostream& os, const std::string& csv, char delimiter ) const
{
    const std::vector< std::string >& v = comma::split( csv, delimiter );
    csv_to_bin( os, v );
}
    
void format::csv_to_bin( std::ostream& os, const std::vector< std::string >& v ) const
{
    if( v.size() != count_ ) { COMMA_THROW( comma::exception, "expected csv string with " << count_ << " elements, got [" << comma::join( v, ',' ) << "]" ); }
    std::vector< char > buf( size_ ); //char buf[ size_ ]; // stupid Windows
    char* p = &buf[0];
    unsigned int offsetIndex = 0u;
    unsigned int count = 0u;
    for( unsigned int i = 0; i < v.size(); ++i, ++count )
    {
        if( count >= elements_[ offsetIndex ].count ) { count = 0; ++offsetIndex; }
        p += impl::csv_to_bin( p, v[i], elements_[ offsetIndex ].type, elements_[ offsetIndex ].size );
    }
    os.write( &buf[0], size_ );
}

std::string format::csv_to_bin( const std::string& csv, char delimiter ) const
{
    std::ostringstream oss( std::ios::out | std::ios::binary );
    csv_to_bin( oss, csv, delimiter );
    return oss.str();
}

std::string format::csv_to_bin( const std::vector< std::string >& csv ) const
{
    std::ostringstream oss( std::ios::out | std::ios::binary );
    csv_to_bin( oss, csv );
    return oss.str();
}

std::string format::bin_to_csv( const std::string& bin, char delimiter, const boost::optional< unsigned int >& precision ) const
{
    if( bin.length() != size_ ) { COMMA_THROW( comma::exception, "expected binary string of size " << size_ << ", got " << bin.length() << " bytes" ); }
    return bin_to_csv( bin.c_str(), delimiter, precision );
}

std::string format::bin_to_csv( const char* buf, char delimiter, const boost::optional< unsigned int >& precision ) const
{
    std::ostringstream oss;
    const char* p = buf;
    unsigned int offsetIndex = 0u; // index in elements_
    unsigned int count = 0u;
    for( unsigned int i = 0u; i < count_; ++i, ++count )
    {
        if( i > 0 ) { oss << delimiter; }
        if( count >= elements_[ offsetIndex ].count ) { count = 0; ++offsetIndex; }
        p += impl::bin_to_csv( oss, p, elements_[ offsetIndex ].type, elements_[ offsetIndex ].size, precision );
    }
    return oss.str();
}

const std::vector< format::element >& format::elements() const { return elements_; }

std::pair< unsigned int, unsigned int > format::index( std::size_t ind ) const
{
    unsigned int count = 0;
    for( unsigned int i = 0; i < elements_.size(); count += elements_[i].count, ++i )
    {
        if( ind < count + elements_[i].count ) { return std::make_pair( i, ind - count ); }
    }
    COMMA_THROW( comma::exception, "expected index less than " << count << "; got " << ind );
}

format::element format::offset( std::size_t ind ) const
{
    std::pair< unsigned int, unsigned int > i = index( ind );
    return element( elements_[ i.first ].offset + elements_[ i.first ].size * i.second
                  , 1
                  , elements_[ i.first ].size
                  , elements_[ i.first ].type );
}

boost::posix_time::ptime format::traits< boost::posix_time::ptime, format::long_time >::from_bin( const char* buf, std::size_t size )
{
    //comma::int64 seconds; // todo: due to bug in boost, will be casted down to int32, but for the dates we use seconds will never overflow, thus, leave it like this now
    //comma::int32 nanoseconds;
    //::memcpy( &seconds, buf, sizeof( comma::int64 ) );
    //::memcpy( &nanoseconds, buf + sizeof( comma::int64 ), sizeof( comma::int32 ) );
	(void) size;
    comma::int64 seconds = *reinterpret_cast< const comma::int64* >( buf );
    comma::int32 nanoseconds = *reinterpret_cast< const comma::int32* >( buf + sizeof( comma::int64 ) );
    return boost::posix_time::ptime( csv::impl::epoch, boost::posix_time::seconds( static_cast< long >( seconds ) ) + boost::posix_time::microseconds( nanoseconds / 1000 ) );
}

void format::traits< boost::posix_time::ptime, format::long_time >::to_bin( const boost::posix_time::ptime& t, char* buf, std::size_t size )
{
	(void)size;
    static const boost::posix_time::ptime base( csv::impl::epoch );
    const boost::posix_time::time_duration duration = t - base;
    comma::int64 seconds = duration.total_seconds();
    comma::int32 nanoseconds = static_cast< comma::int32 >( ( duration - boost::posix_time::seconds( static_cast< long >( seconds ) ) ).total_microseconds() % 1000000 ) * 1000;
    //::memcpy( buf, &seconds, sizeof( comma::int64 ) );
    //::memcpy( buf + sizeof( comma::int64 ), &nanoseconds, sizeof( comma::int32 ) );
    *reinterpret_cast< comma::int64* >( buf ) = seconds;
    *reinterpret_cast< comma::int32* >( buf + sizeof( comma::int64 ) ) = nanoseconds;
}

boost::posix_time::ptime format::traits< boost::posix_time::ptime, format::time >::from_bin( const char* buf, std::size_t size )
{
    //comma::int64 microseconds;
    //::memcpy( &microseconds, buf, sizeof( comma::int64 ) );
	(void)size;
    comma::int64 microseconds = *reinterpret_cast< const comma::int64* >( buf );
    long seconds = static_cast< long >( microseconds / 1000000 ); // todo: due to bug in boost, will be casted down to int32, but for the dates we use seconds will never overflow, thus, leave it like this now
    microseconds -= static_cast< comma::int64 >( seconds ) * 1000000;
    return boost::posix_time::ptime( csv::impl::epoch, boost::posix_time::seconds( seconds ) + boost::posix_time::microseconds( static_cast< long >( microseconds ) ) );
}

void format::traits< boost::posix_time::ptime, format::time >::to_bin( const boost::posix_time::ptime& t, char* buf, std::size_t size )
{
	(void)size;
    static const boost::posix_time::ptime base( csv::impl::epoch );
    const boost::posix_time::time_duration duration = t - base;
    long seconds = duration.total_seconds(); // boost uses long, which is a bug for 32-bit
    comma::int64 microseconds = static_cast< comma::int64 >( seconds ) * 1000000l;
    microseconds += ( duration - boost::posix_time::seconds( seconds ) ).total_microseconds();
    *reinterpret_cast< comma::int64* >( buf ) = microseconds; // ::memcpy( buf, &microseconds, sizeof( comma::int64 ) );
    
}

std::string format::traits< std::string, format::fixed_string >::from_bin( const char* buf, std::size_t size )
{
    return buf[ size - 1 ] == 0 ? std::string( buf ) : std::string( buf, size );
}

void format::traits< std::string, format::fixed_string >::to_bin( const std::string& t, char* buf, std::size_t size )
{
    if( t.length() > size ) { COMMA_THROW( comma::exception, "expected string no longer than " << size << ", got '" << t << "' of " << t.length() ); }
    ::memcpy( buf, &t[0], t.length() );
    if( t.length() < size ) { ::memset( buf + t.length(), 0, size - t.length() ); }
}

} } // namespace comma { namespace csv {
