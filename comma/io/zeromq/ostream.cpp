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


#include <comma/io/ZeroMQ/ostream.h>

namespace comma {
namespace io {
namespace ZeroMQ {
    
ostream::ostream( const std::string& endpoint ):
    m_context( new zmq::context_t( 1 ) ),
    m_socket( new zmq::socket_t( *m_context, ZMQ_PUB ) )
{
    m_socket->bind( endpoint.c_str() );
}


std::streamsize ostream::write( const char* s, std::streamsize n )
{
    zmq::message_t message( n );
    ::memcpy( (void *) message.data (), s, n );
    if( m_socket->send( message ) )
    {
        return n;
    }
    else
    {
        return -1; // eof
    }
}


} } }

