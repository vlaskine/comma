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

#include <set>
#include <boost/optional.hpp>
#include <comma/application/command_line_options.h>
#include <comma/base/exception.h>
#include <comma/string/split.cpp>

namespace comma {

command_line_options::command_line_options( int argc, char ** argv )
{
    argv_.resize( argc );
    for( int i = 0; i < argc; ++i ) { argv_[i] = argv[i]; }
    fill_map_( argv_ );
}

command_line_options::command_line_options( const std::vector< std::string >& argv )
    : argv_( argv )
{
    fill_map_( argv_ );
}

command_line_options::command_line_options( const command_line_options& rhs ) { operator=( rhs ); }

const std::vector< std::string >& command_line_options::argv() const { return argv_; }

bool command_line_options::exists( const std::string& name ) const
{
    std::vector< std::string > names = comma::split( name, ',' );
    for( std::size_t i = 0; i < names.size(); ++i )
    {
        if( map_.find( names[i] ) != map_.end() ) { return true; }
    }
    return false;
}

std::vector< std::string > command_line_options::unnamed( const std::string& valueless_options, const std::string& options_with_values ) const
{
    std::vector< std::string > v = split( valueless_options, ',' );
    std::set< std::string > valueless;
    for( unsigned int i = 0; i < v.size(); ++i ) { valueless.insert( v[i] ); }
    std::vector< std::string > valued = split( options_with_values, ',' );
    std::vector< std::string > w;
    for( unsigned int i = 1; i < argv_.size(); ++i )
    {
        bool isValueless( valueless.find( argv_[i] ) != valueless.end() );
        bool isValued = false;
        bool has_equal_sign = false;
        for( unsigned int j = 0; !isValued && j < valued.size(); ++j )
        {
            has_equal_sign = argv_[i].find( valued[j] + "=" ) == 0;
            isValued = argv_[i] == valued[j] || has_equal_sign;
        }
        if( isValueless && isValued ) { COMMA_THROW( comma::exception, "option " << argv_[i] << " is among options with (" << options_with_values << ") and without (" << valueless_options << ") values at the same time" ); }
        if( isValueless ) { continue; }
        if( isValued ) { if( !has_equal_sign ) { ++i; } continue; }
        w.push_back( argv_[i] );
    }
    return w;
}

std::vector< std::string > command_line_options::names() const
{
    return names_;
}

void command_line_options::fill_map_( const std::vector< std::string >& v )
{
    for( std::size_t i = 1; i < v.size(); ++i )
    {
        if( v[i].at( 0 ) != '-' || v[i].length() < 2 ) { continue; }
        std::string name;
        boost::optional< std::string > value;
        std::size_t equal = v[i].find_first_of( '=' );
        if( equal == std::string::npos )
        {
            name = v[i];
            if( ( i + 1 ) < v.size() ) { value = v[ i + 1 ]; }
        }
        else
        {
            name = v[i].substr( 0, equal );
            if( ( equal + 1 ) < v[i].length() ) { value = v[i].substr( equal + 1 ); }
        }
//         if( v[i].at( 1 ) == '-' )
//         {
//             if( v[i].length() < 3 ) { continue; }
//             if( equal == std::string::npos )
//             {
//                 name = v[i];
//                 // check if the option has a value, i.e. not starting with '-' unless it is a negative number
//                 if( ( i + 1 < v.size() ) && ( ( v[i+1][0] != '-' ) || isdigit( v[i+1][1] ) ) ) { value = v[ i + 1 ]; }
//             }
//             else
//             {
//                 name = v[i].substr( 0, equal );
//                 value = v[i].substr( equal + 1 );
//             }
//         }
//         else
//         {
//             name = v[i];
//             if( i + 1 < v.size() ) { value = v[ i + 1 ]; }
//         }        
        std::vector< std::string >& values = map_[name];
        if( value ) { values.push_back( *value ); }
        if( name.size() != 0u )
        {
            names_.push_back( name );
        }
    }
}

void command_line_options::assert_mutually_exclusive( const std::string& names ) const
{
    std::vector< std::string > v = comma::split( names, ',' );
    std::size_t count = 0;
    for( std::size_t i = 0; i < v.size(); ++i )
    {
        count += exists( v[i] );
        if( count > 1 ) { COMMA_THROW( comma::exception, "options " << names << " are mutually exclusive" ); }
    }
    
}

} // namespace comma {
