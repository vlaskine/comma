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

#ifndef COMMA_IO_SELECT_HEADER
#define COMMA_IO_SELECT_HEADER

#if defined(WINCE)
#include <Winsock.h>
#elif defined(WIN32)
#include <Winsock2.h>
#else
#include "sys/select.h"
#endif

#include <cassert>
#include <set>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_set.hpp>
#include <comma/base/exception.h>
#include "comma/io/file_descriptor.h"

namespace comma { namespace io {

/// select() wrapper; currently implemented in a quick way: it
/// works as expected in POSIX systems, but in Windows it works
/// only on sockets, not on files (because select() in Winsock
/// works only for sockets.
///
/// @todo implement POSIX select() behaviour for Windows
///
/// @todo clean up: add WSAStartup for Windows (move WSAStartup to a proper
///       places in utilities, as currently it is sitting in Bytestreams;
///       now, it still works, because one can use select() in Windows
///       only on sockets, therefore, WSAStartup will be called from our
///       socket library; but the latter solution indirectly relies on
///       something extrinsic to select and therefore is not good.
class select
{
    public:
        /// blocking wait, if OK, returns what select() returned, otherwise throws
        std::size_t wait();

        /// wait with timeout, if OK, returns what select() returned, otherwise throws
        std::size_t wait( unsigned int timeout_seconds, unsigned int timeout_nanoseconds = 0 );

        /// wait with timeout, if OK, returns what select() returned, otherwise throws
        std::size_t wait( boost::posix_time::time_duration timeout );

        /// same as wait( 0 )
        std::size_t check();

        /// descriptor pool for select to monitor
        class descriptors
        {
            public:
                descriptors();
                void add( file_descriptor fd );
                void remove( file_descriptor fd );
                bool ready( file_descriptor fd ) const;
                const std::set< file_descriptor >& operator()() const { return descriptors_; } //const boost::unordered_set< file_descriptor >& operator()() const { return descriptors_; }
                template < typename T > void add( const T& t ) { add( t.fd() ); }
                template < typename T > void remove( const T& t ) { remove( t.fd() ); }
                template < typename T > bool ready( const T& t ) const { return ready( t.fd() ); }

            private:
                friend class select;
                fd_set* reset_fds_();
                std::set< file_descriptor > descriptors_; //boost::unordered_set< file_descriptor > descriptors_;
                fd_set fd_set_;
        };

        /// return read descriptors
        descriptors& read() { return read_descriptors_; }
        const descriptors& read() const { return read_descriptors_; }

        /// return write descriptors
        descriptors& write() { return write_descriptors_; }
        const descriptors& write() const { return write_descriptors_; }

        /// return except descriptors
        descriptors& except() { return except_descriptors_; }
        const descriptors& except() const { return except_descriptors_; }
        
    private:
        descriptors read_descriptors_;
        descriptors write_descriptors_;
        descriptors except_descriptors_;
};

inline void select::descriptors::add( file_descriptor fd )
{
    if( fd != invalid_file_descriptor )
    {
        descriptors_.insert( fd );
    }
    else
    {
        COMMA_THROW( comma::exception, "invalid file descriptor" );
    }
}

inline void select::descriptors::remove( file_descriptor fd )
{
    if( fd == invalid_file_descriptor ) { COMMA_THROW( comma::exception, "invalid file descriptor" ); }
    descriptors_.erase( fd );
}

inline bool select::descriptors::ready( file_descriptor fd ) const
{
    if( fd == invalid_file_descriptor ) { COMMA_THROW( comma::exception, "invalid file descriptor" ); }
    assert( descriptors_.find( fd ) != descriptors_.end() );
    return FD_ISSET( fd, const_cast< fd_set* >( &fd_set_ ) ) != 0;
}


} } // namespace comma { namespace io {

#endif // COMMA_IO_SELECT_HEADER
