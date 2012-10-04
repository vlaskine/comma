#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include <string.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <boost/crc.hpp>
#include <boost/optional.hpp>
#include <comma/application/command_line_options.h>
#include <comma/application/signal_flag.h>
#include <comma/base/types.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "wrap/check crc on fixed-width input (ascii or binary)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: csv-crc <commands> [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<command>" << std::endl;
    std::cerr << "    wrap: add crc" << std::endl;
    std::cerr << "    check: check crs; exit, if check fails" << std::endl;
    std::cerr << "    recover: recover with given parameters (see below)" << std::endl;
    std::cerr << std::endl;
    std::cerr << "data options" << std::endl;
    std::cerr << "    --delimiter,-d=<delimiter>: ascii csv delimiter" << std::endl;
    std::cerr << "    --size=<size>: binary data size; if absent, expect ascii csv" << std::endl;
    std::cerr << "                   for wrap: payload size" << std::endl;
    std::cerr << "                   for check/recover size including crc" << std::endl;
    std::cerr << std::endl;
    std::cerr << "crc options" << std::endl;
    std::cerr << "    --crc=<which>:" << std::endl;
    std::cerr << "        16: 16-bit, generator 0x8805" << std::endl;
    std::cerr << "        ccitt: 16-bit, generator 0x1021" << std::endl;
    std::cerr << "        xmodem: 16-bit, generator 0x8408" << std::endl;
    std::cerr << "        32: 32-bit, generator 0x04C11DB7" << std::endl;
    //std::cerr << "        checksum16: simple 16-bit checksum (todo)" << std::endl;
    //std::cerr << "        checksum32: simple 32-bit checksum (todo)" << std::endl;
    std::cerr << "        default: ccitt" << std::endl;
    std::cerr << "        note: see full parameter list e.g. here: http://www.boost.org/doc/libs/1_49_0/libs/crc/crc.html" << std::endl;
    std::cerr << "    --big-endian,--net-byte-order: if binary, crc is big endian" << std::endl;
    std::cerr << "    --verbose,-v: more output" << std::endl;
    std::cerr << std::endl;
    std::cerr << "recovery options" << std::endl;
    std::cerr << "    --give-up-after=<n>: if check fails, give up after <n> bytes" << std::endl;
    std::cerr << "                         if absent and no --recover-after: default 0" << std::endl;
    std::cerr << "                         if absent and --recover-after: default infinity (don't give up)" << std::endl;
    std::cerr << "    --recover-after=<n>: if check fails and then new valid crc found" << std::endl;
    std::cerr << "                         make sure that at least <n> subsequent lines (ascii) or" << std::endl;
    std::cerr << "                         packets (binary) are valid, before output; default 0, i.e. recover on the next valid" << std::endl;
    std::cerr << "    --discard-on-recovery,--discard: discard those <n> packets accumulated during the recovery check" << std::endl;
    std::cerr << std::endl;
    exit( 1 );
}

static bool verbose;
static boost::optional< unsigned int > give_up_after;
static unsigned int recover_after;
static bool discard_on_recovery;
static unsigned int size;
static bool wrap = false;
static bool recover = false;
static bool binary;
static bool big_endian;
static char delimiter;

template < typename T >
struct traits {};

template <>
struct traits< comma::uint16 >
{
    static comma::uint16 hton( comma::uint16 v ) { return htons( v ); }
    static comma::uint16 ntoh( comma::uint16 v ) { return ntohs( v ); }
};

template <>
struct traits< comma::uint32 >
{
    static comma::uint32 hton( comma::uint32 v ) { return htonl( v ); }
    static comma::uint32 ntoh( comma::uint32 v ) { return ntohl( v ); }
};

template < typename Crc >
static typename Crc::value_type crc_( const char* buf, std::size_t size )
{
    return std::for_each( buf, buf + size, Crc() )();
}

template < typename Crc >
static bool run_()
{
    comma::signal_flag is_shutdown;
    if( binary )
    {
        #ifdef WIN32
            _setmode( _fileno( stdin ), _O_BINARY );
            _setmode( _fileno( stdout ), _O_BINARY );
        #endif
        std::vector< char > buffer( 65536 < size ? size : ( 65536 - 65536 % size ) );
        char* begin = &buffer[0];
        char* end = &buffer[ buffer.size() ];
        char* p = begin;
        std::size_t offset = 0;
        bool recovered = true;
        std::size_t recovered_count = 0;
        std::size_t recovered_byte_count = 0;
        std::vector< char > recovery_buffer( recover_after * size );
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            if( offset >= size )
            {
                if( wrap )
                {
                    typename Crc::value_type crc = crc_< Crc >( p, size );
                    if( big_endian ) { crc = traits< typename Crc::value_type >::hton( crc ); }
                    std::cout.write( p, size );
                    std::cout.write( reinterpret_cast< const char* >( &crc ), sizeof( typename Crc::value_type ) );
                    std::cout.flush();
                }
                else if( recover )
                {
                    static const std::size_t payload_size = size - sizeof( typename Crc::value_type );
                    typename Crc::value_type crc = crc_< Crc >( p, payload_size );
                    typename Crc::value_type expected = *( reinterpret_cast< typename Crc::value_type* >( p + payload_size ) );
                    if( big_endian ) { expected = traits< typename Crc::value_type >::hton( expected ); }
                    if( crc == expected )
                    {
                        if( !recovered )
                        {
                            if( recovered_count == recover_after )
                            {
                                std::cerr << "csv-crc: recovered after " << recovered_byte_count << " byte(s)" << std::endl;
                                if( !discard_on_recovery ) { std::cout.write( &recovery_buffer[0], recovery_buffer.size() ); }
                                recovered = true;
                                recovered_count = 0;
                                recovered_byte_count = 0;
                            }
                            else
                            {
                                ::memcpy( &recovery_buffer[ recovered_count * size ], p, size );
                                ++recovered_count;
                            }
                        }
                        std::cout.write( p, size );
                        std::cout.flush();
                    }
                    else // quick and dirty: lots of code duplication, but just to make it working
                    {
                        if( recovered ) { std::cerr << "csv-crc: crc check failed" << ( !give_up_after || give_up_after > 0 ? "; recovering..." : "" ) << std::endl; }
                        recovered = false;
                        ++recovered_byte_count;
                    }
                }
                unsigned int step = recovered ? size : 1;
                p += step;
                offset -= step;
                if( end - p < int( size ) )
                {
                    ::memcpy( begin, p, offset ); // todo: quick and dirty, check if works in case of overlapping
                    p = begin;
                }
                continue;
            }
            int r = ::read( 0, p, end - p );
            if( r <= 0 ) { break; }
            offset += r;
        }
        if( offset > 0 && offset < size ) { std::cerr << "csv-crc: expected at least " << size << " byte(s), got only " << offset << std::endl; return 1; }
    }
    else
    {
        std::string line;
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        {
            std::getline( std::cin, line );
            if( line.empty() ) { continue; }
            if( wrap )
            {
                std::cout << line << delimiter << crc_< Crc >( &line[0], line.size() ) << std::endl;
            }
            else
            {
                std::vector< std::string > v = comma::split( line, delimiter );
                bool ok = true;
                typename Crc::value_type expected;
                try { expected = boost::lexical_cast< typename Crc::value_type >( v.back() ); }
                catch( ... ) { ok = false; }
                if( ok && v.size() > 1 && crc_< Crc >( &line[0], line.size() - v.back().size() - 1 ) == expected )
                {
                    std::cout << line << std::endl;
                }
                else
                {
                    std::cerr << "csv-crc: check failed (recovery is not implemented for ascii mode, todo)" << std::endl;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( ac < 2 || options.exists( "--help,-h" ) ) { usage(); }
        if( wrap && recover ) { std::cerr << "csv-crc: if 'wrap', then no 'check' or 'recover'" << std::endl; return 1; }
        verbose = options.exists( "--verbose,-v" );
        give_up_after = options.optional< unsigned int >( "--give-up-after" );
        recover_after = options.value( "--recover-after", 0 );
        discard_on_recovery = options.exists( "--discard-on-recovery,--discard" );
        binary = options.exists( "--size" );
        size = options.value< unsigned int >( "--size", 0 );
        big_endian = options.exists( "--big-endian,--net-byte-order" );
        delimiter = options.value< char >( "--delimiter,-d", ',' );
        std::vector< std::string > commands = options.unnamed( "--discard-on-recovery,--discard,--verbose,-v,--big-endian,--net-byte-order", "--size,--delimiter,-d,--crc,--give-up-after,--recover-after" );
        if( commands.empty() ) { std::cerr << "csv-crc: specify a command" << std::endl; return 1; }
        for( std::size_t i = 0; i < commands.size(); ++i )
        {
            if( commands[i] == "wrap" ) { wrap = true; }
            else if( commands[i] == "check" ) { recover = true; give_up_after = 0; }
            else if( commands[i] == "recover" ) { recover = true; }
            else { std::cerr << "csv-crc: expected command, got '" << commands[i] << "'" << std::endl; return 1; }
        }
        std::string crc = options.value< std::string >( "--crc", "ccitt" );
        if( crc == "16" ) { return run_< boost::crc_16_type >(); }
        else if( crc == "32" ) { return run_< boost::crc_32_type >(); }
        else if( crc == "ccitt" ) { return run_< boost::crc_ccitt_type >(); }
        else if( crc == "xmodem" ) { return run_< boost::crc_xmodem_type >(); }
        std::cerr << "csv-crc: expected crc type, got \"" << crc << "\"" << std::endl;
        return 1;
    }
    catch( std::exception& ex )
    {
        std::cerr << "csv-crc: " << ex.what() << std::endl;
    }
    catch( ... )
    {
        std::cerr << "csv-crc: unknown exception" << std::endl;
    }
    return 1;
}