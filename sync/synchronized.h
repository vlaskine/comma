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

#ifndef COMMA_SYNC_SYNCHRONIZED_HEADER_GUARD_
#define COMMA_SYNC_SYNCHRONIZED_HEADER_GUARD_

#include <boost/scoped_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace comma {

/// wrapper synchronizing access to the whole class, rather
/// than to some of its methods, as otherwise we would need
/// to write a wrapper exposing all the class's methods
/// protected by a mutex; owns the class;
///
/// see unit test for examples
template < typename T >
class synchronized
{
    public:
        /// constructors
        synchronized() : t_( new T ) {}
        synchronized( T* t ) : t_( t ) {}
        template < typename A1 > synchronized( A1 a1 ) : t_( new T( a1 ) ) {}
        template < typename A1, typename A2 > synchronized( A1 a1, A2 a2 ) : t_( new T( a1, a2 ) ) {}
        template < typename A1, typename A2, typename A3 > synchronized( A1 a1, A2 a2, A3 a3 ) : t_( new T( a1, a2, a3 ) ) {}
        template < typename A1, typename A2, typename A3, typename A4 > synchronized( A1 a1, A2 a2, A3 a3, A4 a4 ) : t_( new T( a1, a2, a3, a4 ) ) {}
        // etc as needed

        /// lock
        void lock() const { mutex_.lock(); }

        /// unlock
        void unlock() const { mutex_.unlock(); }

        /// accessor class
        class scoped_transaction
        {
            public:
                /// constructor, locks mutex
                scoped_transaction( synchronized& s ) : synchronized_( s ) { synchronized_.lock(); }

                /// destructor, unlocks mutex
                ~scoped_transaction() { synchronized_.unlock(); }

                /// access operators
                T& operator*() { return *synchronized_.t_; }
                const T& operator*() const { return *synchronized_.t_; }
                T* operator->() { return synchronized_.t_.get(); }
                const T* operator->() const { return synchronized_.t_.get(); }

            private:
                synchronized& synchronized_;

        };

        /// accessor class
        class const_scoped_transaction
        {
            public:
                /// constructor, locks mutex
                const_scoped_transaction( const synchronized& s ) : synchronized_( s ) { synchronized_.lock(); }

                /// destructor, unlocks mutex
                ~const_scoped_transaction() { synchronized_.unlock(); }

                /// access operators
                const T& operator*() const { return *synchronized_.t_; }
                const T* operator->() const { return synchronized_.t_.get(); }

            private:
                const synchronized& synchronized_;

        };

    private:
        friend class scoped_transaction;
        friend class const_scoped_transaction;
        boost::scoped_ptr< T > t_;
        mutable boost::recursive_mutex mutex_;
};

} // namespace comma {

#endif // COMMA_SYNC_SYNCHRONIZED_HEADER_GUARD_
