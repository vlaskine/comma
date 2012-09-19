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

#ifndef COMMA_BASE_EXCEPTION_H
#define COMMA_BASE_EXCEPTION_H

#include <stdexcept>
#include <sstream>
#include <string>

namespace comma {

#ifndef STRING_HELPER
#define STRING_HELPER(exp) #exp
#endif //STRING_HELPER

#ifndef STRINGIZE
#define STRINGIZE(exp) STRING_HELPER(exp)
#endif //STRINGIZE

#ifndef COMMA_THROW

#if defined( WIN32 )
    #define COMMA_THROW_IMPL_( exception, message )      \
    throw exception( message, __FILE__, __LINE__, __FUNCSIG__ );
#elif defined( __GNUC__ )
    #define COMMA_THROW_IMPL_( exception, message )      \
    throw exception( message, __FILE__, __LINE__, __PRETTY_FUNCTION__ );
#else
    #define COMMA_THROW_IMPL_( exception, message )      \
    throw exception( message, __FILE__, __LINE__, __FUNCTION__ );
#endif

#define COMMA_THROW( exception, strmessage ) { std::ostringstream CommaThrowStr##__LINE__; CommaThrowStr##__LINE__ << strmessage;  COMMA_THROW_IMPL_( exception, CommaThrowStr##__LINE__.str() ); }

#define COMMA_THROW_STREAM( exception, strmessage ) COMMA_THROW( exception, strmessage )

#endif // COMMA_THROW

#ifndef COMMA_RETHROW

#define COMMA_RETHROW() throw;

#endif // COMMA_RETHROW

class exception : public std::runtime_error
{
    public:

        /// constructor
        exception( const char *message, const char *filename, unsigned long line_number, const char *function_name );

        /// constructor
        exception( const std::string& message, const char *filename, unsigned long line_number, const char *function_name );

        /// destructor
        virtual ~exception() throw() {}

        /// e.what is the complete formatted info
        const char*     what(void) const throw();

        /// just the error message
        const char*     error() const;

        /// filename
        const char*     file() const;

        /// line number
        unsigned long   line() const;

        /// function name
        const char*     function() const;

    protected:

        virtual void    formatted_string_();

        std::string     m_message;
        std::string     m_filename;
        unsigned long   m_line_number;
        std::string     m_function_name;
        std::string     m_formatted_message;
};

inline exception::exception( const char *message, const char *filename, unsigned long line_number, const char *function_name ) :
    std::runtime_error( message ),
    m_message( message ),
    m_filename( filename ),
    m_line_number( line_number ),
    m_function_name( function_name )
{
    formatted_string_();
}

inline exception::exception( const std::string& message, const char *filename, unsigned long line_number, const char *function_name ) :
    std::runtime_error( message.c_str() ),
    m_message( message ),
    m_filename( filename ),
    m_line_number( line_number ),
    m_function_name( function_name )
{
    formatted_string_();
}

inline const char* exception::what(void) const throw()
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

inline const char* exception::error() const
{
    return m_message.c_str();
}


inline const char* exception::file() const
{
    return m_filename.c_str();
}

inline unsigned long exception::line() const
{
    return m_line_number;
}

inline const char* exception::function() const
{
    return m_function_name.c_str();
}

inline void exception::formatted_string_()
{
    std::ostringstream oss;
    oss << error() << std::endl
        << "============================================" << std::endl
        << "file: "     << m_filename << std::endl
        << "line: "     << m_line_number << std::endl
        << "function: " << m_function_name << std::endl
        << "============================================" << std::endl;
    m_formatted_message = oss.str();
}

}  // namespace comma

#endif //COMMA_BASE_EXCEPTION_H

