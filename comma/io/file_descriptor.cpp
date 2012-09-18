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

#include <comma/base/exception.h>
#include "comma/io/file_descriptor.h"

namespace comma { namespace io {

has_file_descriptor::has_file_descriptor( file_descriptor fd )
    : fd_( fd )
{
    if( valid() ) { return; }
    COMMA_THROW( comma::exception, "expected file descriptor, got " << fd );
}

bool has_file_descriptor::valid() const
{
    return fd_ != invalid_file_descriptor; // TODO: quick and dirty; make sure it works for sockets in Win32
}

file_descriptor has_file_descriptor::descriptor() const
{
    //if( !valid() ) { COMMA_THROW( comma::exception, "file descriptor invalid" ); }
    return fd_;
}

void has_file_descriptor::invalidate()
{
    fd_ = (file_descriptor)invalid_file_descriptor;
}

} } // namespace comma { namespace io {

