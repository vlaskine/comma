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

#ifndef COMMA_MATH_INTERVAL_H_
#define COMMA_MATH_INTERVAL_H_

#include <iostream>
#include <comma/base/exception.h>
#include <comma/math/compare.h>

namespace comma { namespace math {

/// [x, y) interval
template < typename T >
class interval
{
    public:
        /// copy constructor
        interval( const interval& rhs ) { this->operator=( rhs ); } 
        
        /// constructor
        interval( const T& min, const T& max ) : interval_( min, max ) { if( !math::less( min, max ) ) { COMMA_THROW( comma::exception, "invalid interval" ); } }
        
        /// return value
        const std::pair< T, T >& operator()() const { return interval_; }
        
        /// return left boundary (convenience method)
        T min() const { return interval_.first; }
        
        /// return right boundary (convenience method)
        T max() const { return interval_.second; }
                
        /// return interval size (convenience method)
        T size() const { return interval_.second - interval_.first; }
        
        /// return true, if variable belongs to the interval
        bool contains( const T& t ) const { return !math::less( t, interval_.first ) && math::less( t, interval_.second ); }
        
        /// return true, if variable belongs to the interval
        bool contains( const interval& rhs ) const { return !( math::less( rhs().first, interval_.first ) || math::less( interval_.second, rhs().second ) ); }
        
        /// equality
        bool operator==( const interval& rhs ) const { return math::equal( interval_.first, rhs().first ) && math::equal( interval_.second, rhs().second ); }
        
        /// unequality
        bool operator!=( const interval& rhs ) const { return !operator==( rhs ); }
        
    private:
        std::pair< T, T > interval_;
};

/// [x, y] interval
template < typename T >
class closed_interval
{
    public:
        /// copy constructor
        closed_interval( const closed_interval& rhs ) { this->operator=( rhs ); } 
        
        /// constructor
        closed_interval( const T& min, const T& max ) : interval_( min, max ) { if( math::less( max, min ) ) { COMMA_THROW( comma::exception, "invalid closed interval" ); } }
        
        /// return value
        const std::pair< T, T >& operator()() const { return interval_; }
        
        /// return left boundary (convenience method)
        T min() const { return interval_.first; }
        
        /// return right boundary (convenience method)
        T max() const { return interval_.second; }
                
        /// return interval size (convenience method)
        T size() const { return interval_.second - interval_.first; }
        
        /// return true, if variable belongs to the interval
        bool contains( const T& t ) const { return !math::less( t, interval_.first ) && !math::less( interval_.second, t ); }
        
        /// return true, if variable belongs to the interval
        bool contains( const closed_interval& rhs ) const { return !( math::less( rhs.first, interval_.first ) || math::less( interval_.second, rhs.second ) ); }
        
        /// operators
        /// @todo more operators
        bool operator==( const closed_interval& rhs ) const { return math::equal( interval_.first, rhs().first ) && math::equal( interval_.second, rhs().second ); }
        bool operator!=( const closed_interval& rhs ) const { return !operator==( rhs ); }
        
    private:
        std::pair< T, T > interval_;
};

/// project a value into the interval;
/// e.g. 200 degrees projected into [-180, 180) should be -160
template < typename T >
inline T mod( T t, const interval< T >& r );

/// print
template < typename T >
inline std::ostream& operator<<( std::ostream& os, const interval< T >& t ) { os << "[" << t().first << "," << t().second << ")"; return os; }

/// print
template < typename T >
inline std::ostream& operator<<( std::ostream& os, const closed_interval< T >& t ) { os << "[" << t().first << "," << t().second << "]"; return os; }

namespace impl {

inline float mod( float lhs, float rhs ) { return ::fmodf( lhs, rhs ); }
inline double mod( double lhs, double rhs ) { return ::fmod( lhs, rhs ); }
inline long double mod( long double lhs, long double rhs ) { return ::fmodl( lhs, rhs ); }
template < typename T > inline T mod( T lhs, T rhs ) { return lhs % rhs; }
    
} // namespace impl {

template < typename T >
inline T mod( T t, const interval< T >& r )
{
    T size = r.size();
    if( math::less( t, r().first ) )
    {
        return r().second - impl::mod( r().second - t, size );
    }
    return r().first + impl::mod( t - r().first, size );
}

} } // namespace comma { namespace math {

#endif // COMMA_MATH_INTERVAL_H_
