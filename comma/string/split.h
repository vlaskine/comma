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

#ifndef COMMA_STRING_SPLIT_H_
#define COMMA_STRING_SPLIT_H_

#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>

namespace comma {

/// split string into tokens (a quick implementation); always contains at least one element
std::vector< std::string > split( const std::string& s, const char* separators = " " );

/// split string into tokens (a quick implementation); always contains at least one element
std::vector< std::string > split( const std::string& s, char separator );

} // namespace comma {

#endif // COMMA_STRING_SPLIT_H_
