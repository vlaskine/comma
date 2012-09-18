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

#ifndef COMMA_IO_IMPL_PUBLISHER_H_
#define COMMA_IO_IMPL_PUBLISHER_H_

#include <set>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <comma/io/file_descriptor.h>
#include <comma/io/select.h>
#include <comma/io/stream.h>

namespace comma { namespace io { namespace impl {

struct acceptor
{
    virtual ~acceptor() {}
    virtual io::ostream* accept() = 0;
    virtual void notify_closed() {} // quick and dirty
    virtual void close() {}
};
    
class publisher
{
    public:
        publisher( const std::string& name, io::mode::value mode, bool blocking = false, bool flush = true );

        unsigned int write( const char* buf, std::size_t size );

        template < typename T >
        impl::publisher& operator<<( const T& lhs ) // quick and dirty, inefficient, but then ascii is meant to be slow...
        {
            accept();
            select_.check();
            unsigned int count = 0;
            for( streams::iterator i = streams_.begin(); i != streams_.end(); )
            {
                streams::iterator it = i++;
                if( !blocking_ && !select_.write().ready( **it ) ) { continue; }
                ( ***it ) << lhs;
                if( ( **it )->good() ) { ( **it )->flush(); ++count; }
                else { remove( it ); }
            }
            return *this;
        }

        void close();

        std::size_t size() const;

        void accept();

    private:
        bool blocking_;
        bool m_flush;
        boost::scoped_ptr< acceptor > acceptor_;
        typedef std::set< boost::shared_ptr< io::ostream > > streams;
        streams streams_;
        io::select select_;
         
        void remove( streams::iterator it );
};

} } } // namespace comma { namespace io { namespace impl {

#endif // #ifndef COMMA_IO_IMPL_PUBLISHER_H_
