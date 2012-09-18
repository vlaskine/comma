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

#ifndef COMMA_IO_FILE_DESCRIPTOR_HEADER
#define COMMA_IO_FILE_DESCRIPTOR_HEADER

#if defined(WIN32)
#if defined(WINCE)
#include <Winsock.h>
#else
#include <Winsock2.h>
#endif
#endif

namespace comma { namespace io {

#ifdef WIN32
    typedef ::SOCKET file_descriptor;
    static const ::SOCKET invalid_file_descriptor = INVALID_SOCKET;
    static const ::SOCKET stdin_fd = INVALID_SOCKET;
    static const ::SOCKET stdout_fd = INVALID_SOCKET;
#else
    typedef int file_descriptor; // according to POSIX standard
    static const file_descriptor invalid_file_descriptor = -1;
    static const file_descriptor  stdin_fd = 0;
    static const file_descriptor  stdout_fd = 1;
#endif    

class has_file_descriptor
{
    public:
        /// constructor
        has_file_descriptor( file_descriptor fd );

        /// return true, if file descriptor is valid
        bool valid() const;

        /// return file descriptor, throw, if invalid
        file_descriptor descriptor() const;

    protected:
        /// invalidate (i.e. when file gets closed)
        void invalidate();

    private:
        file_descriptor fd_;
};

} } // namespace comma { namespace io {

#endif // COMMA_IO_FILE_DESCRIPTOR_HEADER
