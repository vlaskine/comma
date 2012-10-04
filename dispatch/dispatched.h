// This file is part of Ark, a generic and flexible library 
// for robotics research.
//
// Copyright (C) 2011 The University of Sydney
//
// Ark is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Ark is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License 
// for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Ark. If not, see <http://www.gnu.org/licenses/>.

#ifndef COMMA_DISPATCH_DISPATCHED_H_
#define COMMA_DISPATCH_DISPATCHED_H_

namespace comma { namespace dispatch {

/// base class for handler
struct handler
{ 
    /// virtual destructor
    virtual ~handler() {}
};

/// base class for handler of const values
struct handler_const
{
    /// virtual destructor 
    virtual ~handler_const() {}
};

/// base class for dispatched types
struct dispatched_base
{
    /// virtual destructor
    virtual ~dispatched_base() {}
    
    /// dispatch this to a handler
    virtual void dispatch_to( handler& d ) = 0;
    
    /// dispatch this to a handler of const
    virtual void dispatch_to( handler& d ) const = 0;
};

/// handler of a type
template < typename T >
struct handler_of : virtual public handler
{
    /// virtual destructor
    virtual ~handler_of() {}
    
    /// handle a dispatched value
    virtual void handle( T& t ) = 0;
};

/// handler of a const type
template < typename T >
struct handler_of_const : virtual public handler
{
    /// virtual destructor
    virtual ~handler_of_const() {}
    
    /// handle a dispatched value
    virtual void handle( const T& t ) = 0;
};

/// dispatched type
/// double dispatch implementation
/// 
///             An example of the problem:
///                 - we have a vector of base-class pointers to polymorphic objects
///                     of types A, B, C (dispatched types), etc. all derived from class base
///                 - every type should be handled differently, but we don't want to
///                     put the virtual methods for this handling in the base class
/// 
///             Solution (a double-dispatch pattern):
///                 - classes A, B, C, etc just derive from the common base (base),
///                     but base does not define any virtual methods for the derived
///                     classes
///                 - handler classes deriving from a common base Handler provide
///                     methods for handling various data types
///                 - double dispatching (using two dynamic casts) makes sure that
///                     the correct handlers are called
/// 
template < typename T >
struct dispatched : public dispatched_base
{
    /// dispatch this to a handler
    void dispatch_to( handler& d )
    {
        dynamic_cast< handler_of< T >& >( d ).handle( dynamic_cast< T& >( *this ) );
    }
    
    /// dispatch this to a handler of const
    void dispatch_to( handler& d ) const
    {
        dynamic_cast< handler_of_const< T >& >( d ).handle( dynamic_cast< const T& >( *this ) );
    }    
};

} } // namespace comma { namespace dispatch {

#endif /*COMMA_DISPATCH_DISPATCHED_H_*/
