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

#ifndef COMMA_CSV_STREAM_H_
#define COMMA_CSV_STREAM_H_

#ifdef WIN32
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/noncopyable.hpp>
#include <comma/base/exception.h>
#include <comma/csv/ascii.h>
#include <comma/csv/binary.h>
#include <comma/csv/options.h>
#include <comma/string/string.h>

namespace comma { namespace csv {

/// ascii csv input stream 
template < typename S >
class ascii_input_stream : public boost::noncopyable
{
    public:
        /// constructor
        ascii_input_stream( std::istream& is, const std::string& column_names = "", char delimiter = ',', bool full_path_as_name = false, const S& sample = S() );
    
        /// constructor from csv options
        ascii_input_stream( std::istream& is, const options& o, const S& sample = S() );
    
        /// read; return NULL, if end of stream or alike
        const S* read();
    
        /// read with timeout; return NULL, if insufficient data (e.g. end of stream)
        /// @todo implement
        const S* read( const boost::posix_time::ptime& timeout );
    
        /// return the last line read
        const std::vector< std::string >& last() const { return line_; }
    
        /// a helper: return the engine
        const csv::ascii< S > ascii() const { return ascii_; }

        /// return fields
        const std::vector< std::string >& fields() const { return fields_; }

        /// return true, if read will not block
        bool ready() const { return false; }
    
    private:
        std::istream& is_;
        csv::ascii< S > ascii_;
        const S default_;
        S result_;
        std::vector< std::string > line_;
        std::vector< std::string > fields_;
};

/// ascii csv output stream 
template < typename S >
class ascii_output_stream : public boost::noncopyable
{
    public:
        /// constructor
        ascii_output_stream( std::ostream& os, const std::string& column_names = "", char delimiter = ',', bool full_path_as_name = false, const S& sample = S() );
    
        /// constructor from csv options
        ascii_output_stream( std::ostream& os, const options& o, const S& sample = S() );
    
        /// write
        void write( const S& s );
    
        /// substitute corresponding fields in the line and write
        void write( const S& s, const std::string& line );
    
        /// substitute corresponding fields and write
        void write( const S& s, const std::vector< std::string >& line );
    
        /// substitute corresponding fields and write
        void write( const S& s, std::vector< std::string >& line );
    
        /// flush
        /// @todo: to implement
        void flush() {}
    
        /// set precision
        void precision( unsigned int p ) { ascii_.precision( p ); }
        
        /// a helper: return the engine
        const csv::ascii< S > ascii() const { return ascii_; }

        /// return fields
        const std::vector< std::string >& fields() const { return fields_; }
    
    private:
        std::ostream& m_os;
        csv::ascii< S > ascii_;
        std::vector< std::string > fields_;
};

/// binary csv input stream 
template < typename S >
class binary_input_stream : public boost::noncopyable
{
    public:
        /// constructor
        binary_input_stream( std::istream& is, const std::string& format = "", const std::string& column_names = "", bool full_path_as_name = false, const S& sample = S() );
    
        /// constructor from options
        binary_input_stream( std::istream& is, const options& o, const S& sample = S() );
    
        /// read; return NULL, if insufficient data (e.g. end of stream)
        const S* read();
    
        /// read with timeout; return NULL, if insufficient data (e.g. end of stream)
        /// @todo implement
        const S* read( const boost::posix_time::ptime& timeout );
    
        /// return the last line read
        const char* last() const { return last_; }
    
        /// a helper: return the engine
        const csv::binary< S > binary() const { return binary_; }

        /// return size
        std::size_t size() const { return size_; }

        /// return fields
        const std::vector< std::string >& fields() const { return fields_; }

        /// return true, if read will not block
        bool ready() const;
    
    private:
        std::istream& is_;
        csv::binary< S > binary_;
        const S default_;
        S result_;
        const std::size_t size_;
        std::vector< char > buf_; // todo
        char* begin_;
        const char* end_;
        char* cur_;
        char* last_;
        std::size_t offset_;
        std::vector< std::string > fields_;
};

/// binary csv output stream 
template < typename S >
class binary_output_stream : public boost::noncopyable
{
    public:
        /// constructor
        binary_output_stream( std::ostream& os, const std::string& format = "", const std::string& column_names = "", bool full_path_as_name = false, const S& sample = S() );
    
        /// constructor from options
        binary_output_stream( std::ostream& os, const options& o, const S& sample = S() );
    
        /// destructor
        ~binary_output_stream() { flush(); }
    
        /// write
        void write( const S& s );
    
        /// substitute corresponding fields in the buffer and write
        void write( const S& s, const char* buf );
    
        /// flush
        void flush();
    
        /// a helper: return the engine
        const csv::binary< S > binary() const { return binary_; }

        /// return fields
        const std::vector< std::string >& fields() const { return fields_; }
    
    private:
        std::ostream& m_os;
        csv::binary< S > binary_;
        //const std::size_t size_;
        std::vector< char > buf_;
        //char* begin_;
        //const char* end_;
        //char* cur_;
        std::vector< std::string > fields_;
};

/// trivial generic csv input stream wrapper, less optimized, but more convenient 
template < typename S >
class input_stream : public boost::noncopyable
{
    public:
        /// construct from ascii stream
        input_stream( ascii_input_stream< S >* is ) : ascii_( is ) {}
    
        /// construct from binary stream
        input_stream( binary_input_stream< S >* is ) : binary_( is ) {}
    
        /// construct from csv option
        input_stream( std::istream& is, const csv::options& o, const S& sample = S() );
    
        /// read; return NULL, if insufficient data (e.g. end of stream)
        const S* read() { return ascii_ ? ascii_->read() : binary_->read(); }
    
        /// read with timeout; return NULL, if insufficient data (e.g. end of stream)
        const S* read( const boost::posix_time::ptime& timeout ) { return ascii_ ? ascii_->read( timeout ) : binary_->read( timeout ); }
    
        /// get last as string: an evil function, don't use it!
        //std::string last() const;
    
        const ascii_input_stream< S >& ascii() const { return *ascii_; }
        const binary_input_stream< S >& binary() const { return *binary_; }
        ascii_input_stream< S >& ascii() { return *ascii_; }
        binary_input_stream< S >& binary() { return *binary_; }
        bool is_binary() const { return binary_; }
        bool ready() const { return binary_ ? binary_->ready() : ascii_->ready(); }
    
    private:
        boost::scoped_ptr< ascii_input_stream< S > > ascii_;
        boost::scoped_ptr< binary_input_stream< S > > binary_;
};

/// trivial generic csv output stream wrapper, less optimized, but more convenient 
template < typename S >
class output_stream : public boost::noncopyable
{
    public:
        /// construct from ascii stream
        output_stream( ascii_output_stream< S >* os ) : ascii_( os ) {}
    
        /// construct from binary stream
        output_stream( binary_output_stream< S >* os ) : binary_( os ) {}
    
        /// construct from csv option
        output_stream( std::ostream& os, const csv::options& o, const S& sample = S() );
    
        /// write
        void write( const S& s ) { if( ascii_ ) { ascii_->write( s ); } else { binary_->write( s ); } }
    
        /// write, substituting corresponding fields in given line
        void write( const S& s, const char* line ) { if( ascii_ ) { ascii_->write( s, line ); } else { binary_->write( s, line ); } }
    
        /// write, substituting corresponding fields in given line
        void write( const S& s, const std::string& line ) { if( ascii_ ) { ascii_->write( s, line ); } else { binary_->write( s, &line[0] ); } }
    
        /// write, substituting corresponding fields in given line
        void write( const S& s, const std::vector< std::string >& line ) { ascii_->write( s, line ); }
    
        /// flush
        void flush() { if( ascii_ ) { ascii_->flush(); } else { binary_->flush(); } }
    
        const ascii_output_stream< S >& ascii() const { return *ascii_; }
        const binary_output_stream< S >& binary() const { return *binary_; }
        ascii_output_stream< S >& ascii() { return *ascii_; }
        binary_output_stream< S >& binary() { return *binary_; }
        bool is_binary() const { return binary_; }
    
    private:
        boost::scoped_ptr< ascii_output_stream< S > > ascii_;
        boost::scoped_ptr< binary_output_stream< S > > binary_;
};


template < typename S >
inline ascii_input_stream< S >::ascii_input_stream( std::istream& is, const std::string& column_names, char delimiter, bool full_path_as_name, const S& sample )
    : is_( is )
    , ascii_( column_names, delimiter, full_path_as_name, sample )
    , default_( sample )
    , result_( sample )
    , fields_( split( column_names, delimiter ) )
{
}

template < typename S >
inline ascii_input_stream< S >::ascii_input_stream(std::istream& is, const options& o, const S& sample )
    : is_( is )
    , ascii_( o.fields, o.delimiter, o.full_xpath, sample )
    , default_( sample )
    , result_( sample )
    , fields_( split( o.fields, o.delimiter ) )
{

}


template < typename S >
inline const S* ascii_input_stream< S >::read()
{
    while( is_.good() && !is_.eof() )
    {
        /// @todo implement reassembly
        std::string s;
        std::getline( is_, s );
        if( !s.empty() && *s.rbegin() == '\r' ) { s = s.substr( 0, s.length() - 1 ); } // windows... sigh...
        if( s.empty() ) { continue; }
        result_ = default_;
        line_ = split
( s, ascii_.delimiter() );
        ascii_.get( result_, line_ );
        return &result_;
    }
    return NULL;
}

template < typename S >
inline ascii_output_stream< S >::ascii_output_stream( std::ostream& os, const std::string& column_names, char delimiter, bool full_path_as_name, const S& sample )
    : m_os( os )
    , ascii_( column_names, delimiter, full_path_as_name, sample )
    , fields_( split
( column_names, delimiter ) )
{
}

template < typename S >
inline ascii_output_stream< S >::ascii_output_stream( std::ostream& os, const comma::csv::options& o, const S& sample )
    : m_os( os )
    , ascii_( o.fields, o.delimiter, o.full_xpath, sample )
    , fields_( split
( o.fields, o.delimiter ) )
{

}


template < typename S >
inline void ascii_output_stream< S >::write( const S& s )
{
    std::vector< std::string > v;
    write( s, v );
}

template < typename S >
inline void ascii_output_stream< S >::write( const S& s, const std::string& line )
{
    write( s, split
( line, ascii_.delimiter() ) );
}

template < typename S >
inline void ascii_output_stream< S >::write( const S& s, const std::vector< std::string >& line )
{
    std::vector< std::string > v( line );
    write( s, v );
}

template < typename S >
inline void ascii_output_stream< S >::write( const S& s, std::vector< std::string >& v )
{
    ascii_.put( s, v );
    if( v.empty() ) { return; } // never here, though
    m_os << v[0];
    for( std::size_t i = 1; i < v.size(); ++i ) { m_os << ascii_.delimiter() << v[i]; }
    m_os << std::endl;
}


template < typename S >
inline binary_input_stream< S >::binary_input_stream( std::istream& is, const std::string& format, const std::string& column_names, bool full_path_as_name, const S& sample )
    : is_( is )
    , binary_( format, column_names, full_path_as_name, sample )
    , default_( sample )
    , result_( sample )
    , size_( ( binary_.format().size() < 65536 ? 65536 / binary_.format().size() : 1 ) * binary_.format().size() ) // quick and dirty
    , buf_( size_ )
    , begin_( &buf_[0] )
    , end_( begin_ + size_ )
    , cur_( begin_ )
    , last_( begin_ )
    , offset_( 0 )
    , fields_( split
( column_names, ',' ) )
{
    #ifdef WIN32
    if( &is == &std::cin ) { _setmode( _fileno( stdin ), _O_BINARY ); }
    #endif
}

template < typename S >
inline binary_input_stream< S >::binary_input_stream( std::istream& is, const options& o, const S& sample )
    : is_( is )
    , binary_( o.format().string(), o.fields, o.full_xpath, sample )
    , default_( sample )
    , result_( sample )
    , size_( ( binary_.format().size() < 65536 ? 65536 / binary_.format().size() : 1 ) * binary_.format().size() ) // quick and dirty
    , buf_( size_ )
    , begin_( &buf_[0] )
    , end_( begin_ + size_ )
    , cur_( begin_ )
    , last_( begin_ )
    , offset_( 0 )
    , fields_( split( o.fields, ',' ) )
{
    
    #ifdef WIN32
    if( &is == &std::cin ) { _setmode( _fileno( stdin ), _O_BINARY ); }
    #endif
}

template < typename S >
inline bool binary_input_stream< S >::ready() const
{
    return offset_ >= binary_.format().size();
}

template < typename S >
inline const S* binary_input_stream< S >::read()
{ 
    while( true ) // reading a big chunk for better performance
    {
        if( ready() )
        {
            result_ = default_;
            binary_.get( result_, cur_ );
            last_ = cur_;
            cur_ += binary_.format().size();
            offset_ -= binary_.format().size();
            if( cur_ >= end_ ) { cur_ = begin_; offset_ = 0; }
            return &result_;
        }
        bool bad = is_.eof() || !is_.good() || is_.bad() || is_.fail();
        if( offset_ > 0 && bad ) { COMMA_THROW( comma::exception, "expected at least " << binary_.format().size() << " bytes; got " << offset_ ); }
        if( bad ) { return NULL; }
        std::size_t size = end_ - cur_ - offset_;
        // this is a painful part to read only the available bytes
        // if using sockets or pipes, make sure that data is there
        // e.g. using ::select(), comma::Io::Select(), or alike
        // on the internet they say there is no good way,
        // (google "readsome does not work"
        // STL streams seem to take you to a world of pain...)
        std::streamsize a = is_.rdbuf()->in_avail();
        if( a < 0 ) { continue; }
        std::size_t available = a;
        if( size > available )
        {
            size =   offset_ + available > binary_.format().size()
                   ? available
                   : binary_.format().size() - offset_;

            //if( binary_.format().size() < available )
            //{
            //    size = available;
            //}
            //else
            //{
            //    size = size % binary_.format().size();
            //    if( size == 0 ) { size = binary_.format().size(); }
            //}
        }
        is_.read( cur_ + offset_, size ); // blocks till full size bytes read
        std::streamsize count = is_.gcount();
        if( count < 0 ) { continue; }
        offset_ += count;
    }
}

template < typename S >
inline binary_output_stream< S >::binary_output_stream( std::ostream& os, const std::string& format, const std::string& column_names, bool full_path_as_name, const S& sample )
    : m_os( os )
    , binary_( format, column_names, full_path_as_name, sample )
    //, size_( binary_.format().size() * ( 4098 / binary_.format().size() ) ) // quick and dirty
    , buf_( binary_.format().size() ) //, buf_( size_ )
    //, begin_( &buf_[0] )
    //, end_( begin_ + size_ )
    //, cur_( begin_ )
    , fields_( split( column_names, ',' ) )
{
    #ifdef WIN32
    if( &os == &std::cout ) { _setmode( _fileno( stdout ), _O_BINARY ); }
    else if( &os == &std::cerr ) { _setmode( _fileno( stderr ), _O_BINARY ); }
    #endif
}

template < typename S >
inline binary_output_stream< S >::binary_output_stream( std::ostream& os, const options& o, const S& sample )
    : m_os( os )
    , binary_( o.format().string(), o.fields, o.full_xpath, sample )
//     , size_( binary_.format().size() ) //, size_( binary_.format().size() * ( 4098 / binary_.format().size() ) ) // quick and dirty
    , buf_( binary_.format().size() ) //, buf_( size_ )
//     , begin_( &buf_[0] )
//     , end_( begin_ + size_ )
//     , cur_( begin_ )
    , fields_( split( o.fields, ',' ) )
{
    #ifdef WIN32
    if( &os == &std::cout ) { _setmode( _fileno( stdout ), _O_BINARY ); }
    else if( &os == &std::cerr ) { _setmode( _fileno( stderr ), _O_BINARY ); }
    #endif
}

template < typename S >
inline void binary_output_stream< S >::flush()
{
    m_os.flush();
//     if( cur_ == begin_ ) { return; }
//     m_os.write( begin_, cur_ - begin_ );
//     m_os.flush();
//     cur_ = begin_;
}

template < typename S >
inline void binary_output_stream< S >::write( const S& s )
{
    binary_.put( s, &buf_[0] );
    m_os.write( &buf_[0], binary_.format().size() );
//     binary_.put( s, cur_ );
//     cur_ += binary_.format().size();
//     if( cur_ == end_ ) { flush(); }
}

template < typename S >
inline void binary_output_stream< S >::write( const S& s, const char* buf )
{
    ::memcpy( &buf_[0], buf, binary_.format().size() );
    write( s );    
//     ::memcpy( cur_, buf, binary_.format().size() );
//     write( s );
}

template < typename S >
inline input_stream< S >::input_stream( std::istream& is, const csv::options& o, const S& sample )
{
    if( o.binary() ) { binary_.reset( new binary_input_stream< S >( is, o, sample ) ); }
    else { ascii_.reset( new ascii_input_stream< S >( is, o, sample ) ); }
}

//template < typename S >
//std::string inline input_stream< S >::last() const
//{
//    // quick and dirty, otherwise string construction takes forever; profile!
//    //if( binary_ ) { return std::string( binary_->last(), binary_->size() ); }
//    //else { return comma::join( ascii_->last(), ascii_->ascii().delimiter() ); }
//    
//    if( binary_ )
//    {
//        std::string s( binary_->size(), 0 );
//        ::memcpy( &s[0], binary_->last(), binary_->size() );
//        return s;
//    }
//    else
//    {
//        return comma::join( ascii_->last(), ascii_->ascii().delimiter() );
//    }
//    
////    if( binary_ )
////    {
////        last_ = std::string( binary_->last(), binary_->size() );
////    }
////    else
////    {
////        last_ = comma::join( ascii_->last(), ascii_->ascii().delimiter() ); // improve performance, although ascii
////    }
////    return last_;
//}

template < typename S >
inline output_stream< S >::output_stream( std::ostream& os, const csv::options& o, const S& sample )
{
    if( o.binary() ) { binary_.reset( new binary_output_stream< S >( os, o, sample ) ); }
    else { ascii_.reset( new ascii_output_stream< S >( os, o, sample ) ); }
}

} } // namespace comma { namespace csv {

#endif /*COMMA_CSV_STREAM_H_*/
