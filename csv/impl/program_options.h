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

#ifndef COMMA_CSV_PROGRAM_OPTIONS_H_
#define COMMA_CSV_PROGRAM_OPTIONS_H_

#include <string>
#include <boost/program_options.hpp>
#include <comma/csv/options.h>

namespace comma { namespace csv {

struct program_options
{
    /// return option description
    static boost::program_options::options_description description( const char* default_fields = "" );
    
    /// return csv options filled from command line parameters
    static csv::options get( const boost::program_options::variables_map& vm, const csv::options& default_csv = csv::options() );
};

inline boost::program_options::options_description program_options::description( const char* default_fields )
{
    boost::program_options::options_description d;
    d.add_options()
        ( "fields", boost::program_options::value< std::string >()->default_value( default_fields ), "csv fields" )
        ( "binary,b", boost::program_options::value< std::string >(), "csv binary format" )
        ( "delimiter,d", boost::program_options::value< char >()->default_value( ',' ), "csv delimiter" )
        ( "full-xpath", "expect full xpaths as field names" )
        ( "precision", boost::program_options::value< unsigned int >()->default_value( 6 ), "floating point precision" );
    return d;
}

inline csv::options program_options::get( const boost::program_options::variables_map& vm, const csv::options& default_csv )
{
    csv::options csv = default_csv;
    if( vm.count("fields") ) { csv.fields = vm[ "fields" ].as< std::string >(); }
    if( vm.count("delimiter") ) { csv.delimiter = vm[ "delimiter" ].as< char >(); }
    if( vm.count("precision") ) { csv.precision = vm[ "precision" ].as< unsigned int >(); }
    if( vm.count("binary") ) { csv.format( vm[ "binary" ].as< std::string >() ); }
    if( vm.count( "full-xpath" ) ) { csv.full_xpath = true; }
    return csv;
}

} } // namespace comma { namespace csv {

#endif /*COMMA_CSV_PROGRAM_OPTIONS_H_*/
