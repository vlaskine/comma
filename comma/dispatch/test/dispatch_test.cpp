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

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <comma/dispatch/dispatched.h>
#include <gtest/gtest.h>

namespace comma { namespace test {

struct alpha : public dispatch::dispatched< alpha > {};
struct beta : public dispatch::dispatched< beta > {};

class base
{
    public:
        base() : invoked_( false ) {}
        bool invoked() const { return invoked_; }
        const std::string& value() const { invoked_ = false; return value_; }
        void invoke( const std::string& s ) { invoked_ = true; value_ = s; }
    
    private:
        mutable bool invoked_;
        std::string value_;
};

struct multi_handler : public dispatch::handler_of< alpha >
             , public dispatch::handler_of< beta >
             , public dispatch::handler_of_const< beta >
             , public base
{
    void handle( alpha& ) { invoke( "d: got a" ); }
    void handle( beta& ) { invoke( "d: got b" ); }
    void handle( const beta& ) { invoke( "d: got const b" ); }
};

static void TestDispatch()
{
    alpha a;
    beta b;
    multi_handler d;
    dispatch::dispatched_base& aref( a );
    dispatch::dispatched_base& bref( b );
    const dispatch::dispatched_base& const_bref( b );
    dispatch::handler& dref( d );
    EXPECT_TRUE( !d.invoked() );
    aref.dispatch_to( dref );
    EXPECT_TRUE( d.invoked() );
    EXPECT_EQ( d.value(), "d: got a" );
    bref.dispatch_to( dref );
    EXPECT_TRUE( d.invoked() );
    EXPECT_EQ( d.value(), "d: got b" );
    const_bref.dispatch_to( dref );
    EXPECT_TRUE( d.invoked() );
    EXPECT_EQ( d.value(), "d: got const b" );
}

} } // namespace comma { namespace test {

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
