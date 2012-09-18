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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/types.h>
#endif

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <comma/base/exception.h>
#include <comma/string/string.h>
#include "./publisher.h"

namespace comma { namespace io { namespace impl {

class file_acceptor : public acceptor
{
    public:
        file_acceptor( const std::string& name, io::mode::value mode )
            : name_( name )
            , mode_( mode )
            , close_d( true )
            , fd_( io::invalid_file_descriptor )
        {
        }

        ~file_acceptor() 
        {
#ifndef WIN32
            ::close( fd_ );
#else
            _close( fd_ );
#endif
        }

        io::ostream* accept()
        {
            if( !close_d ) { return NULL; }
#ifndef WIN32
            fd_ = ::open( name_.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); // quick and dirty
#else
            fd_ = _open( name_.c_str(), O_WRONLY | _O_CREAT, _S_IWRITE );
#endif
            if( fd_ == io::invalid_file_descriptor ) { return NULL; }
            close_d = false;
            return new io::ostream( name_, mode_, io::mode::non_blocking ); // quick and dirty
        }

        void notify_closed() { close_d = true; ::close( fd_ ); }

    private:
        const std::string name_;
        const io::mode::value mode_;
        bool close_d;
        io::file_descriptor fd_; // todo: make io::ostream non-throwing on construction
};

struct Tcp {};
template < typename S > struct socket_traits {};

template <> struct socket_traits< Tcp >
{
    typedef boost::asio::ip::tcp::endpoint endpoint_type;
    typedef boost::asio::ip::tcp::acceptor acceptor;
    typedef boost::asio::ip::tcp::iostream iostream;
    typedef unsigned short name_type;
    static endpoint_type endpoint( unsigned short port ) { return endpoint_type( boost::asio::ip::tcp::v4(), port ); }
};

#ifndef WIN32
struct local {};
template <> struct socket_traits< local >
{
    typedef boost::asio::local::stream_protocol::endpoint endpoint_type;
    typedef boost::asio::local::stream_protocol::acceptor acceptor;
    typedef boost::asio::local::stream_protocol::iostream iostream;
    typedef std::string name_type;
    static endpoint_type endpoint( const std::string& name ) { return endpoint_type( name ); }
};
#endif

template < typename S >
class socket_acceptor : public acceptor
{
    public:
        socket_acceptor( const typename socket_traits< S >::name_type& name, io::mode::value mode )
            : mode_( mode )
            , acceptor_( m_service, socket_traits< S >::endpoint( name ) )
        {
#ifndef WIN32
            select_.read().add( acceptor_.native() );
#else
            SOCKET socket = acceptor_.native();
            select_.read().add( socket );
#endif
        }

        io::ostream* accept()
        {
            select_.check();
#ifndef WIN32
            if( !select_.read().ready( acceptor_.native() ) ) { return NULL; }
#else
            SOCKET socket = acceptor_.native();
            if( !select_.read().ready( socket ) ) { return NULL; }
#endif
            typename socket_traits< S >::iostream* stream = new typename socket_traits< S >::iostream;
            acceptor_.accept( *( stream->rdbuf() ) );
            return new io::ostream( stream, stream->rdbuf()->native(), mode_, boost::bind( &socket_traits< S >::iostream::close, stream ) );
        }

        void close() { acceptor_.close(); }

    private:
        io::mode::value mode_;
        io::select select_;
        boost::asio::io_service m_service;
        typename socket_traits< S >::acceptor acceptor_;
};

class zero_acceptor_ : public acceptor
{
    public:
        zero_acceptor_( const std::string& name, io::mode::value mode ):
            stream_( new io::ostream( name, mode ) ),
            accepted_( false )
        {
        }

        io::ostream* accept()
        {
            if( !accepted_ )
            {
                accepted_ = true;
                return stream_;
            }
            else
            {
                return NULL;
            }
        }

        void close() { stream_->close(); }

    private:
        io::ostream* stream_;
        bool accepted_;
};

publisher::publisher( const std::string& name, io::mode::value mode, bool blocking, bool flush )
    : blocking_( blocking ),
      m_flush( flush )
{
    std::vector< std::string > v = comma::split( name, ':' );
    if( v[0] == "tcp" )
    {
        if( v.size() != 2 ) { COMMA_THROW( comma::exception, "expected tcp server endpoint, got " << name ); }
        acceptor_.reset( new socket_acceptor< Tcp >( boost::lexical_cast< unsigned short >( v[1] ), mode ) );
    }
    else if( v[0] == "udp" )
    {
        COMMA_THROW( comma::exception, "udp: todo" );
    }
    else if( v[0] == "local" )
    {
#ifndef WIN32
        if( v.size() != 2 ) { COMMA_THROW( comma::exception, "expected local socket, got " << name ); }
        acceptor_.reset( new socket_acceptor< local >( v[1], mode ) );
#endif
    }
    else if( v[0].substr( 0, 4 ) == "zero" )
    {
        acceptor_.reset( new zero_acceptor_( name, mode ) );
    }
    else
    {
        if( name == "-" )
        {
            streams_.insert( boost::shared_ptr< io::ostream >( new io::ostream( name, mode ) ) );
#ifndef WIN32
            select_.write().add( 1 );
#endif
        }
        else
        {
            acceptor_.reset( new file_acceptor( name, mode ) );
        }
    }
}

unsigned int publisher::write( const char* buf, std::size_t size )
{
    accept();
    if( !blocking_ )
    {
        select_.check(); // todo: if slow, put all the files in one select
    }
    unsigned int count = 0;
    for( streams::iterator it = streams_.begin(); it != streams_.end(); ++it )
    {
        if( !blocking_ && !select_.write().ready( **it ) ) { continue; }
        ( **it )->write( buf, size );
        if( ( **it )->good() )
        {
            if( m_flush )
            {
                ( **it )->flush();
            }
            ++count;            
        }
       else { remove( it ); }
    }
    return count;
}

void publisher::close()
{
    if( acceptor_ ) { acceptor_->close(); }
    while( streams_.begin() != streams_.end() ) {

        remove( streams_.begin() );
    }
}

void publisher::accept()
{
    if( !acceptor_ ) { return; }
    while( true ) // while( streams_.size() < maxSize ?
    {
        io::ostream* s = acceptor_->accept();
        if( s == NULL ) { return; }
        streams_.insert( boost::shared_ptr< io::ostream >( s ) );
        select_.write().add( *s );
    }
}

void publisher::remove( streams::iterator it )
{
    select_.write().remove( **it );
    ( *it )->close();
    streams_.erase( it );
    if( acceptor_ ) { acceptor_->notify_closed(); }
}

std::size_t publisher::size() const { return streams_.size(); }

} } } // namespace comma { namespace io { namespace impl {
