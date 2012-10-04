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

#ifndef COMMA_BASE_LAST_ERROR_HEADER
#define COMMA_BASE_LAST_ERROR_HEADER

#include <string>
#include <comma/base/exception.h>

namespace comma {

/// get last error in OS-independent manner
///
/// @todo: add more exception types
/// @todo: do a trick to pass source file name and line number
struct last_error
{
    /// return last error numeric value
    static int value();

    /// return last error as string
    static std::string to_string();

    /// throw last error as a typed exception (use COMMA_THROW_LASTERROR)
    /// @note return bool, because otherwise you may need an extra return
    ///       in the function using to_exception() to get rid of a
    ///       compiler warning
    static void to_exception( const std::string& msg );

    /// generic last error exception
    struct exception;

    /// interrupted system call exception
    struct interrupted_system_call_exception;
};

struct last_error::exception : public comma::exception
{
    exception( const char*, const char *filename, unsigned long line_number, const char *function_name );
    exception( const std::string&, const char *filename, unsigned long line_number, const char *function_name );
};

struct last_error::interrupted_system_call_exception : public last_error::exception
{
    interrupted_system_call_exception( const char*, const char *filename, unsigned long line_number, const char *function_name );
    interrupted_system_call_exception( const std::string&, const char *filename, unsigned long line_number, const char *function_name );
};

} // namespace comma {

#endif // #ifndef COMMA_BASE_LAST_ERROR_HEADER
