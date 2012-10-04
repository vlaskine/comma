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

#include <sys/stat.h> 
#ifndef WIN32
#include <signal.h>
#else
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#endif

#include <fcntl.h>
#include <fstream>
#include <vector>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <comma/base/exception.h>
#include <comma/io/file_descriptor.h>
#include <comma/io/select.h>
#include <comma/io/stream.h>
#include <comma/string/string.h>

#ifdef USE_ZEROMQ
#include <comma/io/zeromq/stream.h>
#endif

namespace comma { namespace io {

namespace impl {

template < typename S >
struct traits {};

template <>
struct traits < std::istream >
{
    typedef std::ifstream file_stream;
    static bool is_standard( const std::istream* is ) { return is == &std::cin; }
    static std::istream* standard( comma::io::mode::value mode )
    {
        #ifdef WIN32
        ( void )( mode );
        //if( mode == comma::io::mode::binary ) { _setmode( _fileno( stdin ), _O_BINARY ); }
        #endif
        return &std::cin;
    }
    static comma::io::file_descriptor standard_fd() 
    { 
        #ifndef WIN32
        return 0;
        #else
        return io::invalid_file_descriptor;
        #endif
    }
    #ifdef WIN32
    static io::file_descriptor open( const std::string name ) { return io::invalid_file_descriptor; }
    #else
    static io::file_descriptor open( const std::string name ) { return ::open( name.c_str(), O_RDONLY | O_NONBLOCK ); }
    #endif
};

template <>
struct traits < std::ostream >
{
    typedef std::ofstream file_stream;
    static bool is_standard( const std::ostream* is ) { return is == &std::cout || is == &std::cerr; }
    static std::ostream* standard( comma::io::mode::value mode )
    {
        #ifdef WIN32
        ( void )( mode );
        //if( mode == comma::io::mode::binary ) { _setmode( _fileno( stdout ), _O_BINARY ); }
        #endif
        return &std::cout;
    }
    static comma::io::file_descriptor standard_fd() 
    {
        #ifndef WIN32
        return 1;
        #else
        return io::invalid_file_descriptor;
        #endif
    }
    #ifdef WIN32
        #ifdef O_LARGEFILE
            static io::file_descriptor open( const std::string name ) { return _open( name.c_str(), O_WRONLY | O_CREAT | O_LARGEFILE, _S_IWRITE ); }
        #else
            static io::file_descriptor open( const std::string name ) { return _open( name.c_str(), O_WRONLY | O_CREAT, _S_IWRITE ); }
        #endif
    #else    
        #ifdef O_LARGEFILE
            static io::file_descriptor open( const std::string name ) { return ::open( name.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | O_LARGEFILE ); }
        #else
            static io::file_descriptor open( const std::string name ) { return ::open( name.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); }
        #endif
    #endif
};

template <>
struct traits < std::iostream >
{
    typedef std::iostream file_stream; // quick and dirty, does not matter for now
    static bool is_standard( const std::iostream* ) { return false; }
    static std::iostream* standard( comma::io::mode::value mode ) { (void) mode; return NULL; }
    static comma::io::file_descriptor standard_fd() { return comma::io::invalid_file_descriptor; }
    #ifdef WIN32
        static io::file_descriptor open( const std::string name ) { return io::invalid_file_descriptor; }
    #else
        #ifdef O_LARGEFILE
            static io::file_descriptor open( const std::string name ) { return ::open( name.c_str(), O_RDWR | O_NONBLOCK | O_LARGEFILE ); }
        #else
            static io::file_descriptor open( const std::string name ) { return ::open( name.c_str(), O_RDWR | O_NONBLOCK ); }
        #endif
    #endif
};

template < typename S > void close_file_stream( typename traits< S >::file_stream* s, int fd )
{
    if( s ) { s->close(); }
    if( fd != io::invalid_file_descriptor ) { ::close( fd ); }
}

} // namespace impl

template < typename S >
stream< S >::~stream()
{
    if( stream_ == NULL || impl::traits< S >::is_standard( stream_ ) ) { return; }
    delete stream_;
    stream_ = NULL;
    close_ = NULL;
}

template < typename S >
void stream< S >::close() { close_d = true; if( close_ ) { close_(); } }

template < typename S >
S* stream< S >::operator()() { return this->operator->(); }

template < typename S >
S& stream< S >::operator*() { return *this->operator->(); }

template < typename S >
S* stream< S >::operator->()
{
#ifndef WIN32
    if( stream_ == NULL ) // quick and dirty: if fstream, cannot open on construction, as pipe might block
    {
        if( !boost::filesystem::is_regular_file( name_ ) && !blocking_ ) // quick and dirty
        {
            io::select select;
            select.read().add( fd_ ); // todo: express via traits
            select.write().add( fd_ ); // todo: express via traits
            select.check(); //if( !select.check() ) { return NULL; }
            if( !select.read().ready( fd_ ) && !select.write().ready( fd_ ) ) { return NULL; }
        }
        typename impl::traits< S >::file_stream* s = new typename impl::traits< S >::file_stream( name_.c_str(), static_cast< std::ios::openmode >( mode_ ) );
        if( s->bad() ) { COMMA_THROW( comma::exception, "failed to open " << name_ ); }
        stream_ = s;
        close_ = boost::bind( &impl::close_file_stream< S >, s, fd_ );
    }
#endif // #ifndef WIN32
    return stream_;
}

template < typename S >
comma::io::file_descriptor stream< S >::fd() const { return fd_; }

template < typename S >
const std::string& stream< S >::name() const { return name_; }

template < typename S >
stream< S >::stream( const std::string& name, mode::value m, mode::blocking_value blocking )
    : name_( name )
    , mode_( m )
    , stream_( NULL )
    , fd_( comma::io::invalid_file_descriptor )
    , close_d( false )
    , blocking_( blocking )
{
    std::vector< std::string > v = comma::split( name, ':' );
    if( v[0] == "tcp" )
    {
        if( v.size() != 3 ) { COMMA_THROW( comma::exception, "expected tcp:<address>:<port>, got \"" << name << "\"" ); }
        boost::asio::io_service service;
        boost::asio::ip::tcp::resolver resolver( service );
        boost::asio::ip::tcp::resolver::query query( v[1], v[2] );
        boost::asio::ip::tcp::resolver::iterator it = resolver.resolve( query );        
        boost::asio::ip::tcp::iostream* s = new boost::asio::ip::tcp::iostream( it->endpoint() );
        if( !*s ) { delete s; COMMA_THROW( comma::exception, "failed to connect to " << name << ( blocking_ ? " (todo: implement blocking mode)" : "" ) ); }
        close_ = boost::bind( &boost::asio::ip::tcp::iostream::close, s );
        // todo: make unidirectional
        fd_ = s->rdbuf()->native();
        stream_ = s;
    }
    else if( v[0] == "udp" )
    {
        COMMA_THROW( comma::exception, "todo" );
    }
    else if( v[0] == "serial" )
    {
        COMMA_THROW( comma::exception, "todo" );
    }
#ifndef WIN32
    else if( v[0] == "local" )
    {
        boost::asio::local::stream_protocol::iostream* ls = new boost::asio::local::stream_protocol::iostream( boost::asio::local::stream_protocol::endpoint( v[1] ) );
        if( !( *ls ) ) { COMMA_THROW( comma::exception, "failed to open " << name_ << ( blocking_ ? " (todo: implement blocking)" : "" ) ); }
        close_ = boost::bind( &boost::asio::local::stream_protocol::iostream::close, ls );
        // todo: make unidirectional
        fd_ = ls->rdbuf()->native();
        stream_ = ls;
    }
#endif
#ifdef USE_ZEROMQ
    else if( v[0].substr( 0, 4 ) == "zero" )
    {
        std::string transport = v[0].substr( 5, v[0].size() );
        std::string endpoint;
        if( transport == "local" )
        {
            endpoint = "ipc://" + v[1];
        }
        else if( transport == "tcp" )
        {
            if( v.size() != 3 ) { COMMA_THROW( comma::exception, "expected zero-tcp:<address>:<port>, got \"" << name << "\"" ); }
            endpoint = "tcp://" + v[1] + ":" + v[2];
        }
        assert( !endpoint.empty() );
        stream_ = zeromq::stream< S >::create( endpoint, fd_ );
    }
#endif
    else if( name == "-" )
    {
        stream_ = impl::traits< S >::standard( m );
        fd_ = impl::traits< S >::standard_fd();
    }
    else
    {
#ifdef WIN32
        typename impl::traits< S >::file_stream* s = 
            m == comma::io::mode::binary ? new typename impl::traits< S >::file_stream( name.c_str(), std::ios::binary )
                                       : new typename impl::traits< S >::file_stream( name.c_str() );
        if( s->bad() ) { COMMA_THROW( comma::exception, "failed to open " << name_ ); }
        stream_ = s;
        close_ = boost::bind( &impl::close_file_stream< S >, s, fd_ );
        fd_ = invalid_file_descriptor; // as select does not work on regular files on windows
#else // #ifdef WIN32
        // this is a very quick and dirty fix, since STL omits file descriptors
        // as an implementation detail, but does not provide any explicit concept
        // for sensing change on a file stream (e.g. calling select() on it)
        //
        // select std::ifstream would not work anyway, because select on a file
        // always returns immediately; however if one would like to use select()
        // on named pipes, which otherwise look like files, he wilio::ostream::model need a file
        // descriptor
        //
        // extracting file descriptor for files is implemented here:
        // http://www.ginac.de/~kreckel/fileno/
        // however, the author laments that it is a hack, etc...
        //
        // a cleaner solution, though, seems to be:
        //
        // - implement select taking selectable objects without the notion
        //   of file descriptor (which is almost done in io::select, just
        //   need to add traits there); then select on std::ifstream simply
        //   will make select returning immediately, without registering fd
        //
        // - implement proper classes for named pipes deriving from
        //   std::istream/std::fstream and make them selectable
        //
        //   currently, we simply go for a dirty trick below, which we
        //   have been using successfully in a few applications
        fd_ = impl::traits< S >::open( name );
        if( fd_ == io::invalid_file_descriptor ) { COMMA_THROW( comma::exception, "failed to open " << name ); }
        std::size_t flags = ::fcntl( fd_, F_GETFL, 0 );
        flags = flags & ( ~O_NONBLOCK );
        ::fcntl( fd_, F_SETFL, flags );
        #endif // #ifdef WIN32
    }
#ifdef WIN32
    //if( m == comma::io::mode::binary ) { _setmode( fd_, _O_BINARY ); }
#endif
}

template class stream< std::istream >;
template class stream< std::ostream >;
//template class stream< std::iostream >;

istream::istream( const std::string& name, mode::value mode, mode::blocking_value blocking ) : stream< std::istream >( name, mode, blocking ) {}
istream::istream( std::istream* s, io::file_descriptor fd, mode::value mode, boost::function< void() > close ) : stream< std::istream >( s, fd, mode, mode::non_blocking, close ) {}
istream::istream( std::istream* s, io::file_descriptor fd, mode::value mode, mode::blocking_value blocking, boost::function< void() > close ) : stream< std::istream >( s, fd, mode, blocking, close ) {}
ostream::ostream( const std::string& name, mode::value mode, mode::blocking_value blocking ) : stream< std::ostream >( name, mode, blocking ) {}
ostream::ostream( std::ostream* s, io::file_descriptor fd, mode::value mode, boost::function< void() > close ) : stream< std::ostream >( s, fd, mode, mode::non_blocking, close ) {}
ostream::ostream( std::ostream* s, io::file_descriptor fd, mode::value mode, mode::blocking_value blocking, boost::function< void() > close ) : stream< std::ostream >( s, fd, mode, blocking, close ) {}
//iostream::iostream( const std::string& name, mode::value mode ) : stream< std::iostream >( name, mode ) {}

} } // namespace comma { namespace io {
