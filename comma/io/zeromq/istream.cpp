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


#include <comma/io/ZeroMQ/istream.h>
#include "istream.h"

namespace comma {
namespace io {
namespace ZeroMQ {

istream::istream( const std::string& endpoint ):
    m_context( new zmq::context_t( 1 ) ),
    m_socket( new zmq::socket_t( *m_context, ZMQ_SUB ) ),
    m_index( m_buffer.size() )
{
    m_socket->connect( endpoint.c_str() );
    m_socket->setsockopt( ZMQ_SUBSCRIBE, "", 0 );
}


std::streamsize istream::read( char* s, std::streamsize n )
{
    zmq::message_t message;
    if( m_socket->recv( &message ) )
    {
        if( m_index != m_buffer.size() )
        {
            unsigned int size = std::min( m_buffer.size() - m_index, static_cast< std::size_t >( n ) );
            ::memcpy( s, &m_buffer[ m_index ], size );
            m_index += size;
            return size;
        }
        else if ( message.size() < static_cast< unsigned int >( n ) )
        {            
            ::memcpy( s, ( const char* )message.data(), message.size() );
            return message.size();
        }
        else
        {
            ::memcpy( s, ( const char* )message.data(), n );
            m_buffer.resize( message.size() - n ); // TODO do not resize if smaller ( for performance )
            ::memcpy( &m_buffer[0], ( const char* )message.data(), m_buffer.size() );
            m_index = 0;
            return n;
        }
    }
    else
    {
        return -1; // eof
    }
}

} } }

