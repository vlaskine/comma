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

#ifndef COMMA_MATH_CYCLIC_H_
#define COMMA_MATH_CYCLIC_H_

#include <iostream>
#include <comma/base/exception.h>
#include <comma/math/compare.h>
#include <comma/math/interval.h>

namespace comma { namespace math {

/// cyclic value for discrete types
template < typename T >
class cyclic
{
    public:
        /// constructor
        cyclic( const math::interval< T >& r ) : interval_( r ), value_( r().first ) {}
    
        /// constructor
        cyclic( const T& first, const T& second ) : interval_( first, second ), value_( first ) {}
    
        /// constructor
        cyclic( const math::interval< T >& r, const T& t ) : interval_( r ), value_( math::mod( t, interval_ ) ) {}
    
        /// copy constructor
        cyclic( const cyclic& rhs ) : interval_( rhs.interval() ) { operator=( rhs ); }
    
        /// return value
        const T& operator()() const { return value_; }
    
        /// return interval
        const math::interval< T >& interval() const { return interval_; }
    
        /// operators
        const cyclic& operator=( const cyclic& rhs ) { interval_ = rhs.interval_; value_ = rhs.value_; return *this; }
        const cyclic& operator=( const T& t ) { value_ = math::mod( t, interval_ ); return *this; }
        bool operator==( const cyclic& rhs ) const { return equal( interval_, rhs.interval_ ) && equal( value_, rhs.value_ ); }
        bool operator!=( const cyclic& rhs ) const { return !operator==( rhs ); }
        bool between( const cyclic& lower, const cyclic& upper ) const;
        const cyclic& operator++() { operator+=( 1 ); return *this; }
        cyclic operator++( int ) { T v = value_; operator++(); return v; }
        const cyclic& operator--() { operator-=( 1 ); return *this; }
        cyclic operator--( int ) { T v = value_; operator--(); return v; }
        const cyclic& operator+=( const cyclic& rhs ) { compatible( rhs, "+=" ); return operator+=( rhs() - interval_().first ); }
        const cyclic& operator-=( const cyclic& rhs ) { compatible( rhs, "-=" ); return operator-=( rhs() - interval_().first ); }
        const cyclic& operator+=( const T& t )
        {
            value_ = math::mod( value_ + t, interval_ );
            return *this;
        }
        const cyclic& operator-=( const T& t )
        {
            T s = math::mod( t, interval_ );
            value_ = ( math::less( value_, s ) ? interval_().second : interval_().first ) + value_ - s;
            return *this;
        }
        cyclic operator+( const cyclic& rhs ) const { cyclic v( *this ); v += rhs; return v; }
        cyclic operator+( const T& rhs ) const { cyclic v( *this ); v += rhs; return v; }
        cyclic operator-( const cyclic& rhs ) const { cyclic v( *this ); v -= rhs; return v; }
        cyclic operator-( const T& rhs ) const { cyclic v( *this ); v -= rhs; return v; }
    
    private:
        math::interval< T > interval_;
        T value_;
        void compatible( const cyclic& rhs, const char* op )
        {
            if( interval_ != rhs.interval() )
            {
                COMMA_THROW( comma::exception, "operation \"" << op << "\": incompatible cyclic variables" );
            }
        }
};

/// print
template < typename T >
std::ostream& operator<<( std::ostream& os, const cyclic< T >& t ) { os << t() << " in " << t.interval(); return os; }

template < typename T >
bool cyclic< T >::between( const cyclic< T >& lower, const cyclic< T >& upper ) const
{
    if( less( upper.value_, lower.value_ ) )
    {
        return !less( value_, lower.value_ ) || less( value_, upper.value_ );
    }
    else
    {
        return !less( value_, lower.value_ ) && less( value_, upper.value_ );
    }
}

} } // namespace comma { namespace math {

#endif // COMMA_MATH_CYCLIC_H_
