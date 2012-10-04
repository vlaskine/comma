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

#ifndef COMMA_APPLICATION_SIGNALFLAG_H_
#define COMMA_APPLICATION_SIGNALFLAG_H_

#include <csignal>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <comma/base/exception.h>

namespace comma {

/// a simplistic singleton signal handler (e.g. for a shutdown flag)
struct signal_flag : public boost::noncopyable
{
    public:
        /// signals
        #ifdef WIN32
            enum signals { sigint = SIGINT, sigterm = SIGBREAK };
        #else
            enum signals { sigint = SIGINT, sigterm = SIGTERM, sigpipe = SIGPIPE /* etc */ };
        #endif
        
        /// constructor
        signal_flag()
        {
            #ifdef WIN32
            boost::array< signals, 2 > signals = { { sigint, sigterm } };
            #else
            boost::array< signals, 3 > signals = { { sigint, sigterm, sigpipe } };
            #endif 
            init_( signals );
        }

        /// constructor
        template < typename T >
        signal_flag( const T& signals ) { init_( signals ); }

        /// return true, if set
        bool is_set() const { return is_set_; }

        /// for those who does not like to type
        operator bool() const { return is_set_; }

        /// reset to false
        void reset() { is_set_ = false; }

    private:
        static bool is_set_;
        static void handle( int ) { is_set_ = true; }
        template < typename T >
        static bool init_( const T& signals ) { static bool r = register_once_< T >( signals ); return r; }
        template < typename T >
        static bool register_once_( const T& signals )
        {
            #ifndef WIN32
                struct sigaction sa;
                sa.sa_handler = handle;
                sigemptyset( &sa.sa_mask );
                sa.sa_flags = 0;
            #endif
            for( unsigned int i = 0; i < T::static_size; ++i )
            {
                #ifdef WIN32
                    // quick and dirty, use ::signal (but it won't handle Ctrl+C properly - fix it, when we know how)
                    // todo: restore from svn
                    if( ::signal( signals[i], handle_signal_ ) == SIG_ERR ) { COMMA_THROW( comma::exception, "failed to set handler for signal " << signals[i] ); }
                #else
                    if( ::sigaction( signals[i], &sa, NULL ) != 0 ) { COMMA_THROW( comma::exception, "failed to set handler for signal " << signals[i] ); }
                #endif
            }
            return true;
        }

    #ifdef WIN32
        static void handle_signal_( int ) { is_set_ = true; }
    #endif
};

} // namespace comma {

#endif // COMMA_APPLICATION_SIGNALFLAG_H_
