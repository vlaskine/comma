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

#ifndef COMMA_IO_ZEROMQ_ISTREAM_H_
#define COMMA_IO_ZEROMQ_ISTREAM_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <zmq.hpp>

namespace comma {
namespace io {
namespace ZeroMQ {

/// istream wrapper for ZeroMQ    
class istream : public boost::iostreams::source
{
public:
    istream( const std::string& endpoint );
    
    std::streamsize read( char* s, std::streamsize n );

    zmq::socket_t& socket() { return *m_socket; }
    
private:
    boost::shared_ptr< zmq::context_t > m_context;
    boost::shared_ptr< zmq::socket_t > m_socket;
    std::vector< char > m_buffer;
    unsigned int m_index;
};

    
} } }

#endif // COMMA_IO_ZEROMQ_ISTREAM_H_
