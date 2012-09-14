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

#ifndef COMMA_CSV_ASCII_HEADER_GUARD_
#define COMMA_CSV_ASCII_HEADER_GUARD_

#include <comma/csv/names.h>
#include <comma/csv/options.h>
#include <comma/csv/impl/ascii_visitor.h>
#include <comma/csv/impl/from_ascii.h>
#include <comma/csv/impl/to_ascii.h>
#include <comma/string/string.h>

namespace comma { namespace csv {

template < typename S >
class ascii
{
    public:
        /// constructor
        ascii( const std::string& column_names = "", char d = ',', bool full_path_as_name = true, const S& sample = S() );

        /// constructor from options
        ascii( const options& o, const S& sample = S() );

        /// get value (returns reference pointing to the parameter)
        const S& get( S& s, const std::vector< std::string >& v ) const;

        /// get value (convenience function)
        const S& get( S& s, const std::string& line ) const { return get( s, split( line, delimiter_ ) ); }

        /// get value (unfilled fields have the same value as in default constructor; convenience function)
        S get( const std::vector< std::string >& v ) const { S s; get( s, v ); return s; }

        /// get value (unfilled fields have the same value as in default constructor; convenience function)
        S get( const std::string& line ) const { S s; get( s, line ); return s; }

        /// put value at the right place in the vector
        const std::vector< std::string >& put( const S& s, std::vector< std::string >& v ) const;

        /// put value at the right place in the line (convenience function)
        const std::string& put( const S& s, std::string& line ) const;

        /// return delimiter
        char delimiter() const { return delimiter_; }

        /// set precision
        void precision( unsigned int p ) { precision_ = p; }

    private:
        char delimiter_;
        boost::optional< unsigned int > precision_;
        impl::asciiVisitor ascii_;
};

template < typename S >
inline ascii< S >::ascii( const std::string& column_names, char d, bool full_path_as_name, const S& sample )
    : delimiter_( d )
    , precision_( 12 )
    , ascii_( join( csv::names( column_names, full_path_as_name, sample ), ',' ), full_path_as_name )
{
    visiting::apply( ascii_, sample );
}

template < typename S >
inline ascii< S >::ascii( const options& o, const S& sample )
    : delimiter_( o.delimiter )
    , precision_( o.precision )
    , ascii_( join( csv::names( o.fields, o.full_xpath, sample ), ',' ), o.full_xpath )
{
    visiting::apply( ascii_, sample );
}

template < typename S >
inline const S& ascii< S >::get( S& s, const std::vector< std::string >& v ) const
{
    impl::from_ascii_ f( ascii_.indices(), ascii_.optional(), v );
    visiting::apply( f, s );
    return s;
}

template < typename S >
inline const std::vector< std::string >& ascii< S >::put( const S& s, std::vector< std::string >& v ) const
{
    if( v.empty() ) { v.resize( ascii_.size() ); }
    impl::to_ascii f( ascii_.indices(), v );
    if( precision_ ) { f.precision( *precision_ ); }
    visiting::apply( f, s );
    return v;
}

template < typename S >
inline const std::string& ascii< S >::put( const S& s, std::string& line ) const
{
    std::vector< std::string > v;
    if( !line.empty() ) { v = split( line, delimiter_ ); }
    line = join( put( s, v ), delimiter_ );
    return line;
}

} } // namespace comma { namespace csv {

#endif // #ifndef COMMA_CSV_ASCII_HEADER_GUARD_
