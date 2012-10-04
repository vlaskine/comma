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

#ifdef WIN32
#include <io.h>
#endif

#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <comma/base/last_error.h>
#include <comma/io/file_descriptor.h>
#include "./publish.h"

namespace comma { namespace io { namespace applications {

publish::publish(const std::vector<std::string> filenames, unsigned int n, unsigned int c, unsigned int packet_size, bool discard, bool flush )
    : packet_(packet_size)
    , packet_offset_(0U)
    , packet_size_(packet_size)
    , packet_counter_(0U)
    , buffer_discarding_(false)
{
    if( c != 0 )
    {
        char_buffer_.reset( new comma::cyclic_buffer< std::vector<char> >( c ) );
    }
    if( n != 0 )
    {
        line_buffer_.reset( new comma::cyclic_buffer< std::string >( n ) );
    }

    // redirect SIGPIPE to SIG_IGN so that it does not kill the application when it tries to write on a closed pipe
#ifndef WIN32
    struct sigaction new_action, old_action;
    new_action.sa_handler = SIG_IGN;
    sigemptyset(&new_action.sa_mask);
    sigaction(SIGPIPE, NULL, &old_action);
    sigaction(SIGPIPE, &new_action, NULL);
    select_.read().add( stdin_fd );
#endif

    io::mode::value mode = packet_size_ == 0 ? io::mode::ascii : io::mode::binary;
    for( std::size_t i = 0; i < filenames.size(); ++i )
    {
        publishers_.push_back( boost::shared_ptr< io::publisher >( new io::publisher( filenames[i], mode, !discard, flush ) ) );
    }
}

publish::~publish()
{
    for( std::size_t i = 0; i < publishers_.size(); ++i ) { publishers_[i]->close(); }
}

void publish::read_line()
{
    if( line_buffer_ && !line_buffer_->empty() )
    {
        select_.wait();
    }
    if( !line_buffer_ || line_buffer_->empty() || select_.read().ready( stdin_fd ) )
    {
        std::string line;
        std::getline( std::cin, line );
        if( line.length() != 0 )
        {
            line += '\n';
            push(line);
        }
    }
    while( line_buffer_ && ( !line_buffer_->empty() ) && ( write( line_buffer_->front().data(), line_buffer_->front().size() ) ) ) // pop the buffer
    {
        line_buffer_->pop();
    }
}

bool publish::read_bytes()
{
    if( char_buffer_ && !char_buffer_->empty() )
    {
        select_.wait();
    }
    if( !char_buffer_ || char_buffer_->empty() || select_.read().ready( stdin_fd ) )
    {
        std::vector< char > w( packet_size_ );
        char* buf = &w[0];
#ifndef WIN32
        ::ssize_t gcount = ::read( 0, buf, packet_size_ );
#else
        int gcount = _read( 0, buf, packet_size_ );
#endif
        if( gcount == 0 ) { return false; } // i.e. end of file
        push( buf, gcount );
    }

    while( char_buffer_ &&  !char_buffer_->empty() && write( &char_buffer_->front()[0], packet_size_ ) ) // pop the buffer packet by packet
    {
        char_buffer_->pop();
    }
    return true;
}

std::size_t publish::write( const char* buffer, std::size_t size )
{
    std::size_t count = 0;
    for( std::size_t i = 0; i < publishers_.size(); ++i ) { count += publishers_[i]->write( buffer, size ); }
    return count;
}


void publish::push(const std::string& line)
{
    if( ( !line_buffer_ || ( line_buffer_->empty() ) ) && write( line.data(), line.size() ) )
    {
    }
    else if( line_buffer_ )
    {
        if(  line_buffer_->size() >= line_buffer_->capacity() )
        {
            line_buffer_->pop(); // throw oldest line away
            if( !buffer_discarding_ )
            {
                buffer_discarding_ = true;
                std::cerr << "io-publish: started discarding on packet " << packet_counter_ << std::endl;
                first_discarded_ = packet_counter_;
            }
        }
        line_buffer_->push(line);
    }
}

void publish::push(char* buffer, std::size_t size)
{
    // fill out the packet buffer
    char* p = buffer;
    bool fullPacket = ( packet_offset_ == 0U ) && ( size >= packet_size_ );
    char* packet = buffer;
    if( fullPacket )
    {
        // can copy whole packet, but no need yet, only if buffering
        p += packet_size_;
        packet_offset_ = packet_size_;
    }
    else
    {
        // packet_ partly filled or not enough data to fill it, fill as much as possible
        for (; (packet_offset_ < packet_size_) && (p < buffer + size); p++)
        {
            packet_[packet_offset_] = *p;
            packet_offset_++;
        }
        packet = &packet_[0];
    }

    if( packet_offset_ == packet_size_ )
    {
        // try to write packet buffer to the pipes
        if ( ( !char_buffer_ || ( char_buffer_->empty() ) ) &&  write( packet, packet_size_ ) )
        {
        }
        else if ( char_buffer_ )
        {
            if( char_buffer_->size() >= char_buffer_->capacity() )
            {
                char_buffer_->pop(); // throw oldest packet away
                if( !buffer_discarding_ )
                {
                    buffer_discarding_ = true;
                    std::cerr << "io-publish: started discarding on packet " << packet_counter_ << std::endl;
                    first_discarded_ = packet_counter_;
                }
            }
            if( fullPacket )
            {
                ::memcpy( &packet_[0], buffer, packet_size_ );
            }
            char_buffer_->push(packet_);
        }
        packet_offset_ = 0U;
    }
    // handle the remaining bytes
    if ( p < buffer + size )
    {
        push( p, size - static_cast<std::size_t>(p - buffer) );
    }
}

} } } /// namespace comma { namespace io { namespace applications {
