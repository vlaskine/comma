// This file is part of comma
//
// Copyright (c) Matthew Herrmann 2007
// Copyright (c) Vsevolod Vlaskine 2010-2011
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

#ifndef COMMA_PACKED_STRING_H_
#define COMMA_PACKED_STRING_H_

#include <string>
#include <comma/base/exception.h>
#include <comma/packed/field.h>

namespace comma { namespace packed {

/// packed fixed-length string
template < size_t S, char Padding = ' ' >
class string : public packed::Field< string< S, Padding >, std::string, S >
{
    public:
        enum { size = S };

        typedef std::string Type;

        typedef packed::Field< string< S, Padding >, std::string, S > base_type;

        static const std::string& default_value()
        {
            static const std::string s( S, Padding );
            return s;
        }

        static void pack( char* storage, const std::string& value )
        {
            if( value.length() != size ) { COMMA_THROW_STREAM( comma::Exception, "expected " << size << " bytes, got " << value.length() << " (\"" << value << "\")" ); }
            ::memcpy( storage, value.c_str(), size );
        }

        static std::string unpack( const char* storage )
        {
            return std::string( storage, size );
        }

        const string& operator=( const std::string& rhs ) { return base_type::operator=( rhs ); }

        const string& operator=( const char* rhs ) { return base_type::operator=( std::string( rhs, size ) ); }
};

} } // namespace comma { namespace packed {

#endif // #ifndef COMMA_PACKED_STRING_H_
