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

#include <sstream>
#include <comma/base/last_error.h>

namespace comma {

/// constructor
exception::exception( const char *message, const char *filename, unsigned long line_number, const char *function_name ) :
    std::runtime_error( message ),
    m_message( message ),
    m_filename( filename ),
    m_line_number( line_number ),
    m_function_name( function_name )
{
    generate_formatted_string();
}

/// constructor
exception::exception( const std::string& message, const char *filename, unsigned long line_number, const char *function_name ) :
    std::runtime_error( message.c_str() ),
    m_message( message ),
    m_filename( filename ),
    m_line_number( line_number ),
    m_function_name( function_name )
{
    generate_formatted_string();
}


/// what
const char* exception::what(void) const throw()
{
    const char * string = "exception::what() m_formatted_message.c_str() threw exception";
    try
    {
      string = m_formatted_message.c_str();
    }
    catch( ... )
    {}
    return string;
}

/// error message string
const char* exception::error() const
{
    return m_message.c_str();
}


/// file name
const char* exception::file() const
{
    return m_filename.c_str();
}


/// line number
unsigned long exception::line() const
{
    return m_line_number;
}

/// function name
const char* exception::function() const
{
    return m_function_name.c_str();
}




/// generate formatted string
void exception::generate_formatted_string(void)
{
    static const std::string separator = "---------------------------";

    std::ostringstream formatted_string;
    formatted_string
        << error() << std::endl
        << separator << std::endl
        << "FILE: "     << m_filename << std::endl
        << "LINE: "     << m_line_number << std::endl
        << "FUNCTION: " << m_function_name << std::endl
        << separator << std::endl;

    m_formatted_message = formatted_string.str();
}

}
