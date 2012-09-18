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

#include <comma/io/publisher.h>

namespace comma { namespace io {

publisher::publisher( const std::string& name, comma::io::mode::value mode, bool blocking, bool flush ) : pimpl_( new impl::publisher( name, mode, blocking, flush ) ) {}

publisher::~publisher() { delete pimpl_; }

std::size_t publisher::write( const char* buf, std::size_t size ) { return pimpl_->write( buf, size ); }

void publisher::accept() { pimpl_->accept(); }

void publisher::close() { pimpl_->close(); }

std::size_t publisher::size() const { return pimpl_->size(); }

} } // namespace comma { namespace io {
