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
#include <comma/io/ZeroMQ/ostream.h>

namespace comma { namespace io {

namespace ZeroMQ {

template< typename S >
struct traits
{    
};

template<>
struct traits< std::istream >
{
    typedef comma::io::ZeroMQ::istream stream;
};

template<>
struct traits< std::ostream >
{
    typedef comma::io::ZeroMQ::ostream stream;
};

/// get ZeroMQ stream and file descriptor from endpoint
template< typename S >
struct stream
{
    static S* create( const std::string& endpoint, comma::io::file_descriptor& fd )
    {
        boost::iostreams::stream< typename traits< S >::stream >* stream = new boost::iostreams::stream< typename traits< S >::stream >( endpoint.c_str() );
        std::size_t size;
        bool haveFd = false;
        while( !haveFd ) // TODO ugly, have timeout instead.
        {
            try
            {
                (*stream)->socket().getsockopt( ZMQ_FD, &fd, &size );
                haveFd = true;
            }
            catch( std::exception& e ) { ::usleep( 10 ); }
        }
        assert( size == sizeof( fd ) );
        return stream;
    }
};

} } }