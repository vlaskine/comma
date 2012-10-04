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

#ifndef COMMA_IO_STREAM_H_
#define COMMA_IO_STREAM_H_

#include <iostream>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <comma/io/file_descriptor.h>

namespace comma { namespace io {

struct mode
{
    enum value { ascii = 0, binary = std::ios::binary };
    enum blocking_value { non_blocking = false, blocking = true };
};
    
/// interface class
/// constructs standard stream from name and owns it:
///     filename: file stream
///     -: std::cin or std::cout
///     tcp:address:port: tcp client socket stream
///     @todo udp:address:port: udp socket stream
///     @todo linux socket name: linux socket client stream
///     @todo serial device name: serial stream
/// see unit test for usage
template < typename S >
class stream : boost::noncopyable
{
    public:
        /// close stream, if closable
        void close();

        /// return pointer to stream
        S* operator()();

        /// return reference to stream
        S& operator*();

        /// return pointer to stream
        S* operator->();

        /// return file descriptor (to use in select)
        comma::io::file_descriptor fd() const;

        /// return stream name
        const std::string& name() const;
        
    protected:
        stream( const std::string& name, mode::value mode, mode::blocking_value blocking );
        template < typename T >
        stream( T* s, io::file_descriptor fd, mode::value mode, mode::blocking_value blocking, boost::function< void() > close )
            : mode_( mode )
            , stream_( s )
            , close_( close )
            , fd_( fd )
            , close_d( false )
            , blocking_( blocking )
        {
        }
        ~stream();
        std::string name_;
        mode::value mode_;
        S* stream_;
        boost::function< void() > close_;
        comma::io::file_descriptor fd_;
        bool close_d;
        bool blocking_;
};
    
/// input stream owner
struct istream : public stream< std::istream >
{
    istream( const std::string& name, mode::value mode = mode::ascii, mode::blocking_value blocking = mode::blocking );
    istream( std::istream* s, io::file_descriptor fd, mode::value mode, boost::function< void() > close );
    istream( std::istream* s, io::file_descriptor fd, mode::value mode, mode::blocking_value blocking, boost::function< void() > close );
};

/// output stream owner
struct ostream : public stream< std::ostream >
{
    ostream( const std::string& name, mode::value mode = mode::ascii, mode::blocking_value blocking = mode::blocking );
    ostream( std::ostream* s, io::file_descriptor fd, mode::value mode, boost::function< void() > close );
    ostream( std::ostream* s, io::file_descriptor fd, mode::value mode, mode::blocking_value blocking, boost::function< void() > close );
};

/// input/output stream owner
struct iostream : public stream< std::iostream >
{
    iostream( const std::string& name, mode::value mode = mode::ascii, mode::blocking_value blocking = mode::blocking );
};

} } // namespace comma { namespace io {
    
#endif
