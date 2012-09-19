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

#ifndef COMMA_IO_APPLICATIONS_PUBLISH_H_
#define COMMA_IO_APPLICATIONS_PUBLISH_H_

#include <map>
#include <boost/shared_ptr.hpp>
#include <comma/containers/cyclic_buffer.h>
#include <comma/io/publisher.h>

namespace comma { namespace io { namespace applications {

class publish
{
public:
    
    publish(const std::vector<std::string> file_names, unsigned int n = 10u, unsigned int c = 10u, unsigned int packet_size = 0, bool discard = true, bool flush = true );
    ~publish();
    void read_line();
    bool read_bytes();

private:
    std::size_t write(const char* buffer, std::size_t size);
    void push(const std::string& line);
    void push(char* buffer, std::size_t size);
    void pop();
    std::vector< boost::shared_ptr< io::publisher > > publishers_;
    io::select select_;

    boost::scoped_ptr< comma::cyclic_buffer< std::string > > line_buffer_;
    boost::scoped_ptr< comma::cyclic_buffer< std::vector<char> > > char_buffer_;
    std::vector<char> packet_; // for reassembly of packets
    unsigned int packet_offset_; // for reassembly of packets
    unsigned int packet_size_;
    unsigned long int packet_counter_;
    unsigned long int first_discarded_;
    bool buffer_discarding_;
    bool discard_;
};

} } } /// namespace comma { namespace io { namespace applications {

#endif // COMMA_IO_APPLICATIONS_PUBLISH_H_
