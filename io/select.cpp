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

#ifndef WIN32
#include <cerrno>
#endif

#include <string>
#include <comma/base/exception.h>
#include <comma/base/last_error.h>
#include <comma/io/select.h>

namespace comma { namespace io {
    
static std::size_t select_impl_( int nfds, fd_set* fdr, fd_set* fdw, fd_set* fde, struct timeval* t )
{
    if( fdr == NULL && fdw == NULL && fde == NULL ) { return 0; } // good semantics?
    int r = ::select( nfds, fdr, fdw, fde, t );
    if( r >= 0 ) { return r; }
    int error = last_error::value();
#ifndef WIN32
    if( error != EINTR ) // do no throw if select is interrupted by signal
#endif
    {
        last_error::to_exception( "select() failed" );
    }
    return 0;
}

static int nfds( file_descriptor a, file_descriptor b, file_descriptor c ) // quick and dirty
{
    if( a < b ) { a = b; }
    if( a < c ) { a = c; }
    return ++a;
}

std::size_t select::wait()
{
    return select_impl_(   nfds( read_descriptors_.descriptors_.empty() ? 0 : *read_descriptors_.descriptors_.rbegin()
                             , write_descriptors_.descriptors_.empty() ? 0 : *write_descriptors_.descriptors_.rbegin()
                             , except_descriptors_.descriptors_.empty() ? 0 : *except_descriptors_.descriptors_.rbegin() )
                       , read_descriptors_.reset_fds_()
                       , write_descriptors_.reset_fds_()
                       , except_descriptors_.reset_fds_()
                       , NULL );
}

std::size_t select::wait( unsigned int timeout_seconds, unsigned int timeout_nanoseconds )
{
    struct timeval t;
    t.tv_sec = static_cast< int >( timeout_seconds );
    t.tv_usec = static_cast< int >( timeout_nanoseconds / 1000 );
    return select_impl_(   nfds( read_descriptors_.descriptors_.empty() ? 0 : *read_descriptors_.descriptors_.rbegin()
                             , write_descriptors_.descriptors_.empty() ? 0 : *write_descriptors_.descriptors_.rbegin()
                             , except_descriptors_.descriptors_.empty() ? 0 : *except_descriptors_.descriptors_.rbegin() )
                       , read_descriptors_.reset_fds_()
                       , write_descriptors_.reset_fds_()
                       , except_descriptors_.reset_fds_()
                       , &t );
}

std::size_t select::wait( boost::posix_time::time_duration timeout )
{
    unsigned int sec = timeout.total_seconds();
	unsigned int nanosec = ( static_cast< unsigned int >( timeout.total_microseconds() ) - sec * 1000000 ) * 1000;
//     std::cerr << "select wait " << sec << " , " << nanosec << std::endl;
    return wait( sec, nanosec );
}

std::size_t select::check() { return wait( 0 ); }

select::descriptors::descriptors()
{
    reset_fds_();
}

fd_set* select::descriptors::reset_fds_()
{
    FD_ZERO( &fd_set_ );
    if( descriptors_.empty() ) { return NULL; }
    for( std::set< file_descriptor >::const_iterator it = descriptors_.begin(); it != descriptors_.end(); ++it )
    {
        #ifdef WIN32
        #pragma warning( disable : 4127 )
        #endif
        FD_SET( *it, &fd_set_ );
        #ifdef WIN32
        #pragma warning( default : 4127 )
        #endif
    }
    return &fd_set_;
}

} } // namespace comma { namespace io {
