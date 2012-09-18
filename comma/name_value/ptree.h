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

#ifndef COMMA_NAME_VALUE_PTREE_H_
#define COMMA_NAME_VALUE_PTREE_H_

#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <comma/base/exception.h>
#include <comma/string/string.h>
#include <comma/xpath/xpath.h>
#include <comma/visiting/visit.h>
#include <comma/visiting/while.h>

namespace comma {

struct property_tree // quick and dirty
{
    /// convert boost parameter tree into name=value-style string
    static std::string to_name_value_string( const boost::property_tree::ptree& ptree, bool indented = true, char equalSign = '=', char delimiter = ',' );
    
    /// convert boost parameter tree into path=value-style string (equal sign and delimiter have to be escaped)
    static std::string to_path_value_string( const boost::property_tree::ptree& ptree, char equalSign = '=', char delimiter = ',' );

    /// convert name=value-style string into boost parameter tree
    static boost::property_tree::ptree from_name_value_string( const std::string& s, char equalSign = '=', char delimiter = ',' );
    
    /// read as path-value from string
    static boost::property_tree::ptree from_path_value_string( const std::string& s, char equalSign = '=', char delimiter = ',' );

    /// write as name-value to output stream
    static void to_name_value( std::ostream& os, const boost::property_tree::ptree& ptree, bool indented = true, char equalSign = '=', char delimiter = ',' );
    
    /// write as path-value to output stream
    static void to_path_value( std::ostream& os, const boost::property_tree::ptree& ptree, char equalSign = '=', char delimiter = ',' );
    
    /// read as name-value from input stream
    /// @todo currently only line-based input supported
    static void from_name_value( std::istream& is, boost::property_tree::ptree& ptree, char equalSign = '=', char delimiter = ',' );
};
    
class from_ptree
{
    public:
        /// constructor
        /// @param ptree: property tree for the structure to fill
        /// @param root: path to the root of the subtree to visit
        /// @param branch: path to the subtree to visit (i.e. other branches will be pruned)
        from_ptree( const boost::property_tree::ptree& ptree ) : ptree_( ptree ), permissive_( false ) {}
        from_ptree( const boost::property_tree::ptree& ptree, bool permissive ) : ptree_( ptree ), permissive_( permissive ) {}
        from_ptree( const boost::property_tree::ptree& ptree, const char* root, bool permissive = false ) : ptree_( ptree ), path_( root ), permissive_( permissive ) {}
        from_ptree( const boost::property_tree::ptree& ptree, const xpath& root, bool permissive = false ) : ptree_( ptree ), path_( root ), permissive_( permissive ) {}
        from_ptree( const boost::property_tree::ptree& ptree, const xpath& root, const xpath& branch, bool permissive = false ) : ptree_( ptree ), path_( root ), branch_( branch ), permissive_( permissive ) {}

        /// apply on boost optional
        template < typename K, typename T >
        void apply_next( const K& name, boost::optional< T >& value )
        {
            boost::optional< std::string > s = ptree_.get_optional< std::string >( path_.to_string( '.' ) );
            if( !s ) { return; }
            if( !value ) { value = T(); }
            visiting::do_while<    !boost::is_fundamental< T >::value
                                && !boost::is_same< T, std::string >::value >::visit( name, *value, *this );
        }

        /// apply to vector
        template < typename K, typename T, typename A >
        void apply( const K& key, std::vector< T, A >& value )
        {
            if( !( path_ <= branch_ ) ) { return; } // visit, only if on the branch
            append_( key );
            boost::optional< const boost::property_tree::ptree& > t = ptree_.get_child_optional( path_.to_string( '.' ) );
            if( !permissive_ || t )
            {
                if( !t ) { COMMA_THROW( comma::exception, "path " << path_.to_string( '.' ) << " not found" ); }
                value.resize( t->size() );
                for( std::size_t i = 0; i < value.size(); ++i ) // quick and dirty
                {
                    append_( boost::lexical_cast< std::string >( i ).c_str() );
                    visiting::do_while<    !boost::is_fundamental< T >::value
                                        && !boost::is_same< T, std::string >::value >::visit( i, value[i], *this );
                    trim_();
                }
            }
            trim_( key );
        }
        
        /// apply to map
        template < typename K, typename L, typename T, typename A >
        void apply( const K& key, std::map< L, T, A >& value )
        {
            if( !( path_ <= branch_ ) ) { return; } // visit, only if on the branch
            append_( key );
            boost::optional< const boost::property_tree::ptree& > t = ptree_.get_child_optional( path_.to_string( '.' ) );
            if( !permissive_ || t )
            {
                if( !t ) { COMMA_THROW( comma::exception, "path " << path_.to_string( '.' ) << " not found" ); }
                for( boost::property_tree::ptree::const_assoc_iterator j = t->ordered_begin(); j != t->not_found(); ++j )
                {
                    append_( j->first.c_str() );
                    visiting::do_while<    !boost::is_fundamental< T >::value
                                        && !boost::is_same< T, std::string >::value >::visit( j->first.c_str(), value[ boost::lexical_cast< L >( j->first ) ], *this );
                    trim_( j->first.c_str() );
                }
            }
            trim_( key );
        }        
        
        /// apply
        template < typename K, typename T >
        void apply( const K& key, T& value )
        {
            if( !( path_ <= branch_ ) ) { return; } // visit, only if on the branch
            append_( key );
            visiting::do_while<    !boost::is_fundamental< T >::value
                                && !boost::is_same< T, std::string >::value >::visit( key, value, *this );
            trim_( key );
        }
        
        /// apply to non-leaf elements
        template < typename K, typename T >
        void apply_next( const K& name, T& value ) { comma::visiting::visit( name, value, *this ); }
                
        /// apply to leaf elements
        template < typename K, typename T >
        void apply_final( const K&, T& value )
        {
            value = permissive_ ? ptree_.get( path_.to_string( '.' ), value )
                                : ptree_.get< T >( path_.to_string( '.' ) );
        }
    
    private:
        const boost::property_tree::ptree& ptree_;
        xpath path_;
        xpath branch_;
        bool permissive_;
        // quick and dirty
        //const xpath& append_( std::size_t index ) { path_.elements.back().index = index; return path_; }
        const xpath& append_( std::size_t index ) { path_ /= xpath::element( boost::lexical_cast< std::string >( index ) ); return path_; }
        const xpath& append_( const char* name ) { path_ /= xpath::element( name ); return path_; }
        //const xpath& trim_( std::size_t size ) { (void) size; path_.elements.back().index = boost::optional< std::size_t >(); return path_; }
        const xpath& trim_( std::size_t size ) { ( void )( size ); trim_(); return path_; }
        const xpath& trim_( const char* name ) { if( *name ) { path_ = path_.head(); } return path_; }
        void trim_() { path_ = path_.head(); }
};

class to_ptree
{
    public:
        /// constructor
        /// @param ptree: property tree for the structure to fill
        /// @param root: path to the root of the subtree to visit
        /// @param branch: path to the subtree to visit (i.e. other branches will be pruned)
        to_ptree( boost::property_tree::ptree& ptree, const xpath& root = xpath(), const xpath& branch = xpath() ) : ptree_( ptree ), path_( root ), branch_( branch ) {}
    
        /// constructor
        to_ptree( boost::property_tree::ptree& ptree, const char* root ) : ptree_( ptree ), path_( root ) {}

        /// apply_next on boost optional
        template < typename K, typename T >
        void apply_next( const K& name, const boost::optional< T >& value )
        {
            if( value )
            {
                visiting::do_while<    !boost::is_fundamental< T >::value
                                    && !boost::is_same< T, std::string >::value >::visit( name, *value, *this );
            }
        }

        /// apply
        template < typename K, typename T, typename A >
        void apply( const K& name, const std::vector< T, A >& value )
        {
            if( !( path_ <= branch_ ) ) { return; } // visit, only if on the branch
            append_( name );
            for( unsigned int i = 0; i < value.size(); ++i )
            { 
                append_( boost::lexical_cast< std::string >( i ).c_str() );
                visiting::do_while<    !boost::is_fundamental< T >::value
                                    && !boost::is_same< T, std::string >::value >::visit( i, value[i], *this );
                trim_();
            }
            trim_( name );
        }

        /// apply
        template < typename K, typename T >
        void apply( const K& name, const T& value )
        {
            if( !( path_ <= branch_ ) ) { return; } // visit, only if on the branch
            std::string s = boost::lexical_cast< std::string >( name );
            append_( s.c_str() );
            visiting::do_while<    !boost::is_fundamental< T >::value
                                && !boost::is_same< T, std::string >::value >::visit( name, value, *this );
            trim_( s.c_str() );
        }
        
        /// apply to non-leaf elements
        template < typename K, typename T >
        void apply_next( const K& name, const T& value )
        { 
            comma::visiting::visit( name, value, *this );
        }
        
        /// apply to leaf elements
        template < typename K, typename T >
        void apply_final( const K&, const T& value ) { ptree_.put( path_.to_string( '.' ), value ); }
    
    private:
        boost::property_tree::ptree& ptree_;
        xpath path_;
        xpath branch_;
        // quick and dirty
        //const xpath& append_( std::size_t index ) { path_.elements.back().index = index; return path_; }
        const xpath& append_( std::size_t index ) { path_ /= xpath::element( boost::lexical_cast< std::string >( index ) ); return path_; }
        const xpath& append_( const char* name ) { path_ /= xpath::element( name ); return path_; }
        //const xpath& trim_( std::size_t size ) { (void) size; path_.elements.back().index = boost::optional< std::size_t >(); return path_; }
        const xpath& trim_( std::size_t size ) { ( void )( size ); trim_(); return path_; }
        const xpath& trim_( const char* name ) { if( *name ) { path_ = path_.head(); } return path_; }
        void trim_() { path_ = path_.head(); }
};

namespace Impl {

inline static void ptree_to_name_value_string_impl( std::ostream& os, boost::property_tree::ptree::const_iterator i, bool is_begin, bool indented, unsigned int indent, char equalSign, char delimiter )
{
    if( !is_begin && !( indented && ( delimiter == ' ' || delimiter == '\t' ) ) ) { os << delimiter; }
    if( indented ) { os << std::endl << std::string( indent, ' ' ); }
    os << i->first << equalSign;    
    if( i->second.begin() == i->second.end() )
    {
        //std::string value = i->second.get_value< std::string >();
        //bool quoted =    value.empty() // real quick and dirty
        //              || value.find_first_of( '\\' ) != std::string::npos
        //              || value.find_first_of( '"' ) != std::string::npos;
        //if( quoted ) { os << '"'; }
        //os << value;
        //if( quoted ) { os << '"'; }
        os << '"' << i->second.get_value< std::string >() << '"';
    }
    else
    {
        if( indented ) { os << std::endl << std::string( indent, ' ' ); }
        os << "{";
        for( boost::property_tree::ptree::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
        {
            ptree_to_name_value_string_impl( os, j, j == i->second.begin(), indented, indent + 4, equalSign, delimiter );
        }
        if( indented ) { os << std::endl << std::string( indent, ' ' ); }
        os << '}';
    }
}

inline static void ptree_to_path_value_string_impl( std::ostream& os, boost::property_tree::ptree::const_iterator i, bool is_begin, xpath& path, char equalSign, char delimiter )
{
    if( i->second.begin() == i->second.end() )
    {
        if( !is_begin ) { os << delimiter; }
        is_begin = false;
        if( !path.elements.empty() ) { os << path.to_string() << "/"; }
        os << i->first << equalSign;
        os << '"' << i->second.get_value< std::string >() << '"';
    }
    else
    {
        path /= i->first;
        for( boost::property_tree::ptree::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
        {
            ptree_to_path_value_string_impl( os, j, is_begin, path, equalSign, delimiter );
            is_begin = false;
        }
        path = path.head();
    }
}

} // namespace Impl {

inline void property_tree::to_name_value( std::ostream& os, const boost::property_tree::ptree& ptree, bool indented, char equalSign, char delimiter )
{
    for( boost::property_tree::ptree::const_iterator i = ptree.begin(); i != ptree.end(); ++i )
    {
        Impl::ptree_to_name_value_string_impl( os, i, i == ptree.begin(), indented, 0, equalSign, delimiter );
    }
    if( indented ) { os << std::endl; } // quick and dirty
}

inline void property_tree::to_path_value( std::ostream& os, const boost::property_tree::ptree& ptree, char equalSign, char delimiter )
{
    for( boost::property_tree::ptree::const_iterator i = ptree.begin(); i != ptree.end(); ++i )
    {
        xpath path;
        Impl::ptree_to_path_value_string_impl( os, i, i == ptree.begin(), path, equalSign, delimiter );
    }
}

inline void property_tree::from_name_value( std::istream& is, boost::property_tree::ptree& ptree, char equalSign, char delimiter )
{
    // quick and dirty, worry about performance, once needed
    std::ostringstream oss;
    while( is.good() && !is.eof() )
    {
        std::string line;
        std::getline( is, line );
        if( !line.empty() && line.at( 0 ) != '#' ) { oss << line; } // quick and dirty: allow comment lines
    }
    ptree = from_name_value_string( oss.str(), equalSign, delimiter );
}
    
inline std::string property_tree::to_name_value_string( const boost::property_tree::ptree& ptree, bool indented, char equalSign, char delimiter )
{
    std::ostringstream oss;
    to_name_value( oss, ptree, indented, equalSign, delimiter );
    return oss.str();
}

inline std::string property_tree::to_path_value_string( const boost::property_tree::ptree& ptree, char equalSign, char delimiter )
{
    std::ostringstream oss;
    to_path_value( oss, ptree, equalSign, delimiter );
    return oss.str();
}

inline boost::property_tree::ptree property_tree::from_name_value_string( const std::string& s, char equalSign, char delimiter )
{
    boost::property_tree::ptree ptree;
    bool escaped = false;
    bool quoted = false;
    std::ostringstream oss;
    for( std::size_t i = 0; i < s.length(); ++i )
    {
        char c = s[i];
        bool space = false;
        if( escaped )
        {
            escaped = false;
        }
        else
        {
            switch( c )
            {
                case '\\':
                    escaped = true;
                    break;
                case '"':
                    quoted = !quoted;
                    break;
                case '{':
                case '}':
                    space = !quoted;
                default:
                    if( quoted ) { break; }
                    if( c == equalSign ) { c = ' '; }
                    else if( c == delimiter ) { c = ' '; }
                    break;
            }
        }
        if( space ) { oss << ' '; }
        oss << c;
        if( space ) { oss << ' '; }
    }
    std::istringstream iss( oss.str() );
    boost::property_tree::read_info( iss, ptree );
    return ptree;
}

inline boost::property_tree::ptree property_tree::from_path_value_string( const std::string& s, char equalSign, char delimiter )
{
    boost::property_tree::ptree ptree;
    std::vector< std::string > v = comma::split( s, delimiter );
    for( std::size_t i = 0; i < v.size(); ++i )
    {
        if( v[i].empty() ) { continue; }
        std::vector< std::string > pair = comma::split( v[i], equalSign );
        if( pair.size() != 2 ) { COMMA_THROW( comma::exception, "expected '" << delimiter << "'-separated xpath" << equalSign << "value pairs; got \"" << v[i] << "\"" ); }
        ptree.put( boost::property_tree::ptree::path_type( comma::strip( pair[0], '"' ), '/' ), comma::strip( pair[1], '"' ) );
    }
    return ptree;
}

} // namespace comma {

#endif /*COMMA_NAME_VALUE_PTREE_H_*/
