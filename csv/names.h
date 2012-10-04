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

#ifndef COMMA_CSV_NAMES_H_
#define COMMA_CSV_NAMES_H_

#include <comma/csv/impl/to_names.h>
#include <comma/string/string.h>
#include <comma/visiting/apply.h>
#include <comma/xpath/xpath.h>

namespace comma { namespace csv {

/// the most generic way: return default column names for
/// a given sample and given subtree in comma-separated xpaths
template < typename S >
std::vector< std::string > names( const std::string& paths, bool useFullxpath = true, const S& sample = S() );

/// return default column names for a given sample and given subtree in comma-separated xpaths
template < typename S >
std::vector< std::string > names( const char* paths, bool useFullxpath = true, const S& sample = S() ) { return names( std::string( paths ), useFullxpath, sample ); }

/// return default column names for a given sample
template < typename S >
std::vector< std::string > names( bool useFullxpath, const S& sample = S() ) { return names( "", useFullxpath, sample ); }

/// return default column names for a given sample, use full xpath
template < typename S >
std::vector< std::string > names( const S& sample = S() ) { return names( true, sample ); }

/// return true, if all the fields from subset present in fields
/// @todo make a generic subset application
bool fields_exist( const std::vector< std::string >& fields, const std::vector< std::string >& subset );
bool fields_exist( const std::string& fields, const std::string& subset, char delimiter = ',' );

template < typename S >
inline std::vector< std::string > names( const std::string& paths, bool useFullxpath, const S& sample )
{
    std::vector< std::string > p = split( paths, ',' );
    std::vector< std::string > r;
    for( std::size_t i = 0; i < p.size(); ++i )
    {
        if( p.size() > 1 && p[i] == "" )
        {
            r.push_back( "" );
        }
        else
        {
            impl::to_names v( p[i], useFullxpath );
            visiting::apply( v, sample );
            if( v().empty() ) { r.push_back( p[i] ); } // unknown name, don't replace
            else { r.insert( r.end(), v().begin(), v().end() ); }
        }
    }
    return r;
}

} } // namespace comma { namespace csv {

#endif // COMMA_CSV_NAMES_H_
