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

#ifndef COMMA_EIGEN_CONTAINERS_H
#define COMMA_EIGEN_CONTAINERS_H

#define EIGEN_USE_NEW_STDVECTOR
#include <Eigen/Core>
#include <Eigen/StdVector>
#include <map>

namespace comma {
namespace eigen {

/// typedef for vector of eigen matrix
template < typename T >
struct vector
{
    typedef std::vector< T, Eigen::aligned_allocator< T > > type;
};

/// typedef for map of eigen matrix
template < typename K, typename T, typename Less = std::less<K> >
struct map
{
    typedef std::map< K, T, Less, Eigen::aligned_allocator< T > > type;
};
    
} }

#endif // COMMA_EIGEN_CONTAINERS_H

