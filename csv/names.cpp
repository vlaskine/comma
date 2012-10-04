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
#include <comma/csv/names.h>
#include <comma/string/split.h>

namespace comma { namespace csv {

bool fields_exist( const std::vector< std::string >& fields, const std::vector< std::string >& subset )
{
    for( unsigned int i = 0; i < subset.size(); ++i )
    {
        bool found = false;
        for( unsigned int j = 0; !found && j < fields.size(); found = fields[j] == subset[i], ++j );
        if( !found ) { return false; }
    }
    return true;
}

bool fields_exist( const std::string& fields, const std::string& subset, char delimiter )
{
    return fields_exist( comma::split( fields, delimiter ), comma::split( subset, delimiter ) );
}

} } // namespace comma { namespace csv {
