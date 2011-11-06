// This file is part of comma, a generic and flexible library
//
// Copyright (C) 2011 Vsevolod Vlaskine and Cedric Wohlleber
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

#if defined(WIN32)
#include "Windows.h"
#include <sstream>
#else
#include "errno.h"
#include "string.h"
#endif

#include <comma/base/exception.h>
#include "comma/base/last_error.h"

namespace comma {

int last_error::value()
{
    #if defined(WIN32)
    return get_last_error();
    #else
    return errno;
    #endif
}

std::string last_error::to_string()
{
    #if defined(WIN32)
    std::ostringstream oss;
    oss << "Windows error #" << value();
    return oss.str();
    #else
    return ::strerror( errno );
    #endif
}

void last_error::to_exception( const std::string& msg )
{
    #ifdef WIN32
    switch( value() )
    {
        // TODO: add more exceptions
        case 0: break;
        case WSAEINTR: COMMA_THROW( last_error::interrupted_system_call_exception, msg );
        default: COMMA_THROW( last_error::exception, msg );
    }
    #else
    switch( value() )
    {
        // TODO: add more exceptions
        case 0: break;
        case EINTR: COMMA_THROW( last_error::interrupted_system_call_exception, msg );
        default: COMMA_THROW( last_error::exception, msg );
    };
    #endif
}

last_error::exception::exception( const char* msg, const char *filename, unsigned long line_number, const char *function_name )
    : comma::exception( std::string( msg ) + ": " + last_error::to_string(), filename, line_number, function_name )
{
}

last_error::exception::exception( const std::string& msg, const char *filename, unsigned long line_number, const char *function_name )
    : comma::exception( msg + ": " + last_error::to_string(), filename, line_number, function_name )
{
}

last_error::interrupted_system_call_exception::interrupted_system_call_exception( const char* msg, const char *filename, unsigned long line_number, const char *function_name )
    : last_error::exception( msg, filename, line_number, function_name )
{
}

last_error::interrupted_system_call_exception::interrupted_system_call_exception( const std::string& msg, const char *filename, unsigned long line_number, const char *function_name )
    : last_error::exception( msg, filename, line_number, function_name )
{
}

} // namespace comma {
    