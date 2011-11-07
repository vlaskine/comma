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

#ifndef COMMA_XPATH_XPATH_HEADER_GUARD_
#define COMMA_XPATH_XPATH_HEADER_GUARD_

#ifndef WIN32
#include <float.h>
#include <stdlib.h>
#endif
#include <iostream>
#include <string>
#include <vector>
#include <boost/optional.hpp>

namespace comma {

/// xpath, like "hello/world[5]/moon"
class x_path
{
    public:
        /// xpath element
        struct element
        {
            /// element name 
            std::string name;
            
            /// optionally, element index
            boost::optional< std::size_t > index;
            
            /// constructor
            element( const std::string& name );
            
            /// constructor
            element( const std::string& name, std::size_t index );
            
            /// constructor
            element( const std::string& name, boost::optional< std::size_t > index );
            
            /// default constructor
            element();
            
            /// copy constructor
            element( const element& rhs );
            
            /// comparison
            bool operator==( const element& rhs ) const;
            
            /// comparison
            bool operator!=( const element& rhs ) const;
            
            /// comparison
            bool operator<( const element& rhs ) const;
            
            /// comparison
            bool operator<=( const element& rhs ) const;
            
            /// to string
            std::string to_string() const;
        };
        
        /// constructor
        x_path( const std::string& str, char delimiter = '/' );
        
        /// constructor
        x_path( const char* str, char delimiter = '/' );

        /// constructor
        x_path( const x_path::element& rhs, char delimiter = '/' );
        
        /// default constructor
        x_path();
        
        /// copy constructor
        x_path( const x_path& rhs );
        
        /// append
        const x_path& operator/=( const x_path& );
        const x_path& operator/=( const x_path::element& );
        const x_path& operator/=( const std::string& );
        x_path operator/( const x_path& ) const;
        x_path operator/( const x_path::element& ) const;
        x_path operator/( const std::string& ) const;
        
        /// comparisons
        bool operator==( const x_path& rhs ) const;
        
        /// comparison
        bool operator!=( const x_path& rhs ) const;
        
        /// return true, if it's a subpath of rhs, e.g hello/world < hello
        bool operator<( const x_path& rhs ) const;
        
        /// comparison
        bool operator<=( const x_path& rhs ) const;
        
        /// return xpath without the first element; if empty, return empty xpath
        x_path tail() const;
        
        /// return xpath without the last element; if empty, return empty xpath
        x_path head() const;
        
        /// xpath elements
        std::vector< element > elements;
        
        /// return as string
        std::string to_string( char delimiter = '/' ) const;
};

/// output
std::ostream& operator<<( std::ostream& os, const x_path& x );

/// output
std::ostream& operator<<( std::ostream& os, const x_path::element& e );

} // namespace comma {

#endif // #ifndef COMMA_XPATH_XPATH_HEADER_GUARD_
