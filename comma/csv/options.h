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

#ifndef COMMA_CSV_OPTIONS_H_
#define COMMA_CSV_OPTIONS_H_

#include <sstream>
#include <boost/program_options.hpp>
#include <comma/application/command_line_options.h>
#include <comma/csv/format.h>
#include <comma/string/string.h>
#include <comma/visiting/traits.h>

namespace comma { namespace csv {

/// a helper class to extract csv-related command line options 
class options
{
    public:
        /// constructor
        options();
        
        /// constructor
        options( int argc, char** argv, const std::string& defaultFields = "" );
        
        /// constructor
        options( const comma::command_line_options& options, const std::string& defaultFields = "" );
        
        /// return usage to incorporate into application usage
        static std::string usage();
        
        /// filename (optional)
        std::string filename;
        
        /// if true, expect full xpaths as field names;
        /// e.g. "point/scalar" rather than "scalar"  
        bool full_xpath;
        
        /// field (column) names
        std::string fields;
        
        /// csv delimiter
        char delimiter;
        
        /// precision
        unsigned int precision;

        /// return format
        const csv::format& format() const { return *format_; }

        /// set format
        void format( const std::string& s ) { format_ = csv::format( s ); }

        /// set format
        void format( const csv::format& f ) { format_ = f; }

        /// true, if --binary specified
        bool binary() const { return format_; }
        
        /// return true, if fields have given field (convenience function, slow)
        bool has_field( const std::string& field ) const;

    private:
        boost::optional< csv::format > format_;
};

namespace impl {

inline static void init( comma::csv::options& csvoptions, const comma::command_line_options& options, const std::string& defaultFields )
{
    csvoptions.full_xpath = options.exists( "--full-xpath" );
    csvoptions.fields = options.value( "--fields", defaultFields );
    if( options.exists( "--binary" ) )
    {
        boost::optional< std::string > format = options.optional< std::string >( "--binary" );
        if( format )
        {
            csvoptions.format( options.value< std::string >( "--binary" ) );
        }
    }
    csvoptions.precision = options.value< unsigned int >( "--precision", 6 );
    csvoptions.delimiter = options.exists( "--delimiter" ) ? options.value( "--delimiter", ',' ) : options.value( "-d", ',' );
}

} // namespace impl {

inline options::options() : full_xpath( false ), delimiter( ',' ), precision( 6 ) {}
    
inline options::options( int argc, char** argv, const std::string& defaultFields )
{
    impl::init( *this, comma::command_line_options( argc, argv ), defaultFields );
}

inline options::options( const comma::command_line_options& options, const std::string& defaultFields )
{
    impl::init( *this, options, defaultFields );
}

inline std::string options::usage()
{
    std::ostringstream oss;
    oss << "    --binary,-b <format> : use binary format" << std::endl;
    oss << "    --delimiter,-d <delimiter> : default: ','" << std::endl;
    oss << "    --fields,-f <names> : field names, e.g. t,,x,y,z" << std::endl;
    oss << "    --full-xpath : expect full xpaths as field names" << std::endl;
    oss << "    --precision <precision> : floating point precision; default: 6" << std::endl;
    oss << format::usage();
    return oss.str();
}

inline bool options::has_field( const std::string& field ) const
{
    std::vector< std::string > v = split( fields, ',' );
    return std::find( v.begin(), v.end(), field ) != v.end();
}

} } // namespace comma { namespace csv {

namespace comma { namespace visiting {

/// visiting traits
template <> struct traits< comma::csv::options >
{
    /// const visiting
    template < typename Key, class Visitor >
    static void visit( const Key&, const comma::csv::options& p, Visitor& v )
    {
        v.apply( "filename", p.filename );
        v.apply( "delimiter", p.delimiter );
        v.apply( "fields", p.fields );
        v.apply( "full-xpath", p.full_xpath );
        v.apply( "precision", p.precision );
        if( p.binary() ) { v.apply( "binary", p.format().string() ); }
    }

    /// visiting
    template < typename Key, class Visitor >
    static void visit( Key, comma::csv::options& p, Visitor& v )
    {
    	v.apply( "filename", p.filename );
        v.apply( "delimiter", p.delimiter );
        v.apply( "fields", p.fields );
        v.apply( "full-xpath", p.full_xpath );
        v.apply( "precision", p.precision );
        std::string s;
        v.apply( "binary", s );
        if( s != "" ) { p.format( s ); }
    }
};

} } // namespace comma { namespace visiting {

#endif /*COMMA_CSV_OPTIONS_H_*/
