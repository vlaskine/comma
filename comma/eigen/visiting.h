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

#ifndef COMMA_EIGEN_VISITING_H
#define COMMA_EIGEN_VISITING_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <comma/visiting/apply.h>
#include <comma/visiting/visit.h>

namespace comma { namespace visiting {

template < typename T > struct traits< Eigen::Matrix< T, 2, 1 > >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, Eigen::Matrix< T, 2, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
    }

    template < typename Key, class Visitor >
    static void visit( const Key&, const Eigen::Matrix< T, 2, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
    }
};

template < typename T > struct traits< Eigen::Matrix< T, 3, 1 > >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, Eigen::Matrix< T, 3, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
    }

    template < typename Key, class Visitor >
    static void visit( const Key&, const Eigen::Matrix< T, 3, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
    }
};

template < typename T > struct traits< Eigen::Matrix< T, 4, 1 > >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, Eigen::Matrix< T, 4, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
        v.apply( "w", p.w() );
    }

    template < typename Key, class Visitor >
    static void visit( const Key&, const Eigen::Matrix< T, 4, 1 >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
        v.apply( "w", p.w() );
    }
};


template < typename T > struct traits< Eigen::Quaternion< T > >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, Eigen::Quaternion< T >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
        v.apply( "w", p.w() );
    }

    template < typename Key, class Visitor >
    static void visit( const Key&, const Eigen::Quaternion< T >& p, Visitor& v )
    {
        v.apply( "x", p.x() );
        v.apply( "y", p.y() );
        v.apply( "z", p.z() );
        v.apply( "w", p.w() );
    }
};

template < typename T > struct traits< Eigen::Translation< T, 3 > >
{
    template < typename Key, class Visitor >
    static void visit( const Key&, Eigen::Translation< T, 3 >& p, Visitor& v )
    {
        v.apply( "x", p.vector().x() );
        v.apply( "y", p.vector().y() );
        v.apply( "z", p.vector().z() );
    }

    template < typename Key, class Visitor >
    static void visit( const Key&, const Eigen::Translation< T, 3 >& p, Visitor& v )
    {
        v.apply( "x", p.vector().x() );
        v.apply( "y", p.vector().y() );
        v.apply( "z", p.vector().z() );
    }
};


} }

#endif // COMMA_EIGEN_VISITING_H

