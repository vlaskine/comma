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

#ifndef COMMA_IO_PUBLISHER_H_
#define COMMA_IO_PUBLISHER_H_

#include <stdlib.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <comma/io/stream.h>
#include <comma/io/impl/publisher.h>

namespace comma { namespace io {

/// a simple publisher that opens and writes to using services (e.g. tcp, udp, etc)
class publisher : public boost::noncopyable
{
    public:
        /// constructor
        /// @param name ::= tcp:<port> | udp:<port> | <filename>
        ///     if tcp:<port>, create tcp server
        ///     @todo if udp:<port>, broadcast on udp
        ///     if <filename> is a regular file, just write to it
        ///     if <filename> is named pipe, keep reopening it, if closed
        ///     @todo if <filename> is Linux domain socket, create Linux domain socket server
        /// @param mode ascii or binary, a hint for Windows
        /// @param blocking if true, blocking write to a client, otherwise discard, if client not ready
        publisher( const std::string& name, io::mode::value mode, bool blocking = false, bool flush = true );

        /// destructor
        ~publisher();

        /// publish to all existing connections (blocking), return number of client with successful write
        std::size_t write( const char* buf, std::size_t size );

        /// publish to all existing connections (blocking)
        /// @note data integrity is the user's responsibility
        ///       i.e. if someone writes:
        ///           publisher p( "tcp:localhost:1234" );
        ///           p << 1 << "," << 2 << std::endl;
        ///       and a client connects after "1" already
        ///       has been output, this client will receive
        ///       ",2", which most likely was not intended
        template < typename T >
        publisher& operator<<( const T& rhs ) { pimpl_->operator<<( rhs ); return *this; }

        /// close
        void close();

        /// return current number of connected clients
        std::size_t size() const;

        /// accept waiting clients, non-blocking
        void accept();
        
    private:
        impl::publisher* pimpl_;
};

} } // namespace comma { namespace io {

#endif // #ifndef COMMA_IO_PUBLISHER_H_
