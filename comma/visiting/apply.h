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

#ifndef COMMA_VISITING_APPLY_H_
#define COMMA_VISITING_APPLY_H_

namespace comma { namespace visiting {

namespace impl {

template < typename V >
class applier_
{
    public:
        applier_( V& v ) : m_visitor( v ) {}
        template < typename T > V& to( const T& t ) { m_visitor.apply( "", t ); return m_visitor; }
        template < typename T > V& to( T& t ) { m_visitor.apply( "", t ); return m_visitor; }
    private:
        V& m_visitor;
};
    
} // namespace impl {

/// apply visitor function    
template < typename Visitor, typename T >
inline void apply( Visitor& v, T& t ) { v.apply( "", t ); }

/// apply visitor function 
template < typename Visitor, typename T >
inline void apply( Visitor& v, const T& t ) { v.apply( "", t ); }

/// apply visitor function
template < typename Visitor >
inline impl::applier_< Visitor > apply( Visitor& v ) { return impl::applier_< Visitor >( v ); }

} } // namespace comma { namespace visiting {

#endif // COMMA_VISITING_APPLY_H_
