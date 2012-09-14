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
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#endif

#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <comma/Application/SignalFlag.h>
#include <comma/Base/Exception.h>
#include <comma/csv/format.h>
#include <comma/csv/Options.h>
#include <comma/String/String.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "column-wise calculation, optionally by id and block" << std::endl;
    std::cerr << std::endl;
    std::cerr << "usage: cat data.csv | csv-extents <what> [<options>] > extents.csv" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<what>: comma-separated list of operations" << std::endl;
    std::cerr << "        results will be output in the same order" << std::endl;
    std::cerr << "        optionally followed by block,id (both as ui, if binary)" << std::endl;
    std::cerr << "    min: minimum" << std::endl;
    std::cerr << "    max: maximum" << std::endl;
    std::cerr << "    mean: mean value" << std::endl;
    std::cerr << "    centre: ( min + max ) / 2" << std::endl;
    std::cerr << "    diameter: max - min" << std::endl;
    std::cerr << "    radius: size / 2" << std::endl;
    std::cerr << "    var: variance" << std::endl;
    std::cerr << "    stddev: standard deviation" << std::endl;
    std::cerr << "    size: number of values" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<options>" << std::endl;
    std::cerr << "    --delimiter,-d <delimiter> : default ','" << std::endl;
    std::cerr << "    --fields,-f: field names for which the extents should be computed, default: all fields" << std::endl;
    std::cerr << "                 if 'block' field present, calculate block-wise" << std::endl;
    std::cerr << "                 if 'id' field present, calculate by id" << std::endl;
    std::cerr << "                 if 'block' and 'id' fields present, calculate by id in each block" << std::endl;
    std::cerr << "                 block and id fields will be appended to the output" << std::endl;
    std::cerr << "    --format: in ascii mode: format hint string containing the types of the csv data, default: double or time" << std::endl;
    std::cerr << "    --binary,-b: in binary mode: format string of the csv data types" << std::endl;
    std::cerr << comma::csv::format::usage() << std::endl;
    std::cerr << std::endl;
    std::cerr << "examples" << std::endl;
    std::cerr << "    todo" << std::endl;
    std::cerr << std::endl;
    exit( -1 );
}

class Values
{
    public:
        Values( const comma::csv::Options& csv, const comma::csv::format& input_format )
            : csv_( csv )
            , input_format_( input_format )
            , block_( 0 )
            , id_( 0 )
        {
            init_indices_();
            init_format_();
        }
        
        Values( const comma::csv::Options& csv, const std::string& hint )
            : csv_( csv )
            , block_( 0 )
            , id_( 0 )
        {
            init_indices_();
            std::vector< std::string > v = comma::split( hint, csv.delimiter );
            for( unsigned int i = 0; i < v.size(); ++i )
            {
                if( ( block_index_ && *block_index_ == i ) || ( id_index_ && *id_index_ == i ) ) { input_format_ += "ui"; continue; }
                try { boost::posix_time::frois_o_string( v[i] ); input_format_ += "t"; }
                catch( ... ) { input_format_ += "d"; }
            }
            std::cerr << "csv-calc: guessed format: " << input_format_.string() << std::endl;
            init_format_();
        }
        
        void set( const char* buf )
        {
            for( unsigned int i = 0; i < indices_.size(); ++i )
            {
                ::memcpy( &buffer_[0] + elements_[i].offset, buf + input_elements_[i].offset, elements_[i].size );
            }
            if( block_index_ ) { block_ = block_from_bin_( buf + block_element_.offset ); }
            if( id_index_ ) { id_ = id_from_bin_( buf + id_element_.offset ); }
        }
        
        void set( const std::string& line ) // quick and dirty, probably very slow
        {
            std::vector< std::string > v = comma::split( line, csv_.delimiter );
            std::vector< std::string > w( indices_.size() );
            for( unsigned int i = 0; i < indices_.size(); ++i ) { w[i] = v[indices_[i]]; }
            const std::string& s = format_.csv_to_bin( w );
            ::memcpy( &buffer_[0], &s[0], buffer_.size() );
            if( block_index_ ) { block_ = boost::lexical_cast< unsigned int >( v[ *block_index_ ] ); }
            if( id_index_ ) { id_ = boost::lexical_cast< unsigned int >( v[ *id_index_ ] ); }
        }
        
        const comma::csv::format& format() const { return format_; }
        unsigned int block() const { return block_; }
        unsigned int id() const { return id_; }
        const char* buffer() const { return &buffer_[0]; }
        
    private:
        comma::csv::Options csv_;
        comma::csv::format input_format_;
        comma::csv::format format_;
        std::vector< unsigned int > indices_;
        std::vector< comma::csv::format::element > input_elements_;
        std::vector< comma::csv::format::element > elements_;
        std::vector< char > buffer_;
        boost::optional< unsigned int > block_index_;
        boost::optional< unsigned int > id_index_;
        comma::csv::format::element block_element_;
        comma::csv::format::element id_element_;
        unsigned int block_;
        unsigned int id_;
        boost::function< comma::uint32( const char* ) > block_from_bin_;
        boost::function< comma::uint32( const char* ) > id_from_bin_;
        template < typename T > static comma::uint32 from_bin_( const char* buf ) { return comma::csv::format::traits< T >::from_bin( buf ); }
        
        void init_indices_()
        {
            std::vector< std::string > v = comma::split( csv_.fields, ',' );
            for( unsigned int i = 0; i < v.size(); ++i )
            {
                if( v[i] == "block" ) { block_index_ = i; }
                else if( v[i] == "id" ) { id_index_ = i; }
                else if( v[i] != "" ) { indices_.push_back( i ); }
            }
        }
        void init_format_()
        {
            if( indices_.empty() )
            {
                for( unsigned int i = 0; i < input_format_.count(); ++i )
                {
                    if( block_index_ && *block_index_ == i ) { continue; }
                    if( id_index_ && *id_index_ == i ) { continue; }
                    indices_.push_back( i );
                }
            }
            for( unsigned int i = 0; i < indices_.size(); ++i )
            {
                format_ += comma::csv::format::to_format( input_format_.offset( indices_[i] ).type );
            }
            for( unsigned int i = 0; i < indices_.size(); ++i )
            {
                elements_.push_back( format_.offset( i ) );
                input_elements_.push_back( input_format_.offset( indices_[i] ) );
            }
            buffer_.resize( format_.size() );
            if( block_index_ )
            {
                block_element_ = input_format_.offset( *block_index_ );
                switch( block_element_.type )
                {
                    case comma::csv::format::char_t: block_from_bin_ = boost::bind( &Values::from_bin_< char >, _1 ); break;
                    case comma::csv::format::int8: block_from_bin_ = boost::bind( &Values::from_bin_< char >, _1 ); break;
                    case comma::csv::format::uint8: block_from_bin_ = boost::bind( &Values::from_bin_< unsigned char >, _1 ); break;
                    case comma::csv::format::int16: block_from_bin_ = boost::bind( &Values::from_bin_< comma::int16 >, _1 ); break;
                    case comma::csv::format::uint16: block_from_bin_ = boost::bind( &Values::from_bin_< comma::uint16 >, _1 ); break;
                    case comma::csv::format::int32: block_from_bin_ = boost::bind( &Values::from_bin_< comma::int32 >, _1 ); break;
                    case comma::csv::format::uint32: block_from_bin_ = boost::bind( &Values::from_bin_< comma::uint32 >, _1 ); break;
                    case comma::csv::format::int64: block_from_bin_ = boost::bind( &Values::from_bin_< comma::int64 >, _1 ); break;
                    case comma::csv::format::uint64: block_from_bin_ = boost::bind( &Values::from_bin_< comma::uint64 >, _1 ); break;
                    default: COMMA_THROW( comma::exception, "expected integer for block id, got format " << input_format_.string() );
                }
            }
            if( id_index_ )
            {
                id_element_ = input_format_.offset( *id_index_ );
                switch( id_element_.type )
                {
                    case comma::csv::format::char_t: id_from_bin_ = boost::bind( &Values::from_bin_< char >, _1 ); break;
                    case comma::csv::format::int8: id_from_bin_ = boost::bind( &Values::from_bin_< char >, _1 ); break;
                    case comma::csv::format::uint8: id_from_bin_ = boost::bind( &Values::from_bin_< unsigned char >, _1 ); break;
                    case comma::csv::format::int16: id_from_bin_ = boost::bind( &Values::from_bin_< comma::int16 >, _1 ); break;
                    case comma::csv::format::uint16: id_from_bin_ = boost::bind( &Values::from_bin_< comma::uint16 >, _1 ); break;
                    case comma::csv::format::int32: id_from_bin_ = boost::bind( &Values::from_bin_< comma::int32 >, _1 ); break;
                    case comma::csv::format::uint32: id_from_bin_ = boost::bind( &Values::from_bin_< comma::uint32 >, _1 ); break;
                    case comma::csv::format::int64: id_from_bin_ = boost::bind( &Values::from_bin_< comma::int64 >, _1 ); break;
                    case comma::csv::format::uint64: id_from_bin_ = boost::bind( &Values::from_bin_< comma::uint64 >, _1 ); break;
                    default: COMMA_THROW( comma::exception, "expected integer for block id, got format " << input_format_.string() );
                }
            }
        }
};

class asciiInput
{
    public:
        asciiInput( const comma::csv::Options& csv, const boost::optional< comma::csv::format >& format ) : csv_( csv )
        {
            if( format ) { values_.reset( new Values( csv, *format ) ); }
        }
        
        const Values* read()
        {
            std::string line;
            std::getline( std::cin, line );
            if( line == "" ) { return NULL; }
            if( !values_ ) { values_.reset( new Values( csv_, line ) ); }
            values_->set( line );
            return values_.get();
        }
        
    private:
        comma::csv::Options csv_;
        boost::scoped_ptr< Values > values_;
};

class binaryInput
{
    public:
        binaryInput( const comma::csv::Options& csv )
            : csv_( csv )
            , values_( csv, csv.format() )
            , buffer_( csv.format().size() > 65536 ? csv.format().size() : 65536 / csv.format().size() * csv.format().size() )
            , cur_( &buffer_[0] )
            , end_( &buffer_[0] + buffer_.size() )
            , offset_( 0 )
        {
        }
        
        const Values* read()
        {
            while( true )
            {
                if( offset_ >= csv_.format().size() )
                {
                    values_.set( cur_ );
                    cur_ += csv_.format().size();
                    offset_ -= csv_.format().size();
                    if( cur_ == end_ ) { cur_ = &buffer_[0]; offset_ = 0; }
                    return &values_;
                }
                int count = ::read( 0, cur_ + offset_, end_ - cur_ - offset_ );
                if( count <= 0 ) { return NULL; }
                offset_ += count;
            }
        }
        
    private:
        comma::csv::Options csv_;
        Values values_;
        std::vector< char > buffer_;
        char* cur_;
        const char* end_;
        unsigned int offset_;
};

namespace Operations
{
    struct Base
    {
        virtual ~Base() {}
        virtual void push( const char* ) = 0;
        virtual void calculate( char* ) = 0;
        virtual Base* clone() const = 0;
    };

    template < typename T, comma::csv::format::types_enum F > class Centre;
    template < typename T, comma::csv::format::types_enum F > class Radius;
    template < typename T, comma::csv::format::types_enum F > class Diameter;

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Min : public Base
    {
        public:
            void push( const char* buf )
            {
                const T& t = comma::csv::format::traits< T, F >::from_bin( buf );
                if( !min_ || t < *min_ ) { min_ = t; }
            }
            void calculate( char* buf ) { if( min_ ) { comma::csv::format::traits< T, F >::to_bin( *min_, buf ); } }
            Base* clone() const { return new Min< T, F >( *this ); }
        private:
            friend class Centre< T, F >;
            friend class Diameter< T, F >;
            friend class Radius< T, F >;
            boost::optional< T > min_;
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Max : public Base
    {
        public:
            void push( const char* buf )
            {
                T t = comma::csv::format::traits< T, F >::from_bin( buf );
                if( !max_ || t > *max_ ) { max_ = t; }
            }
            void calculate( char* buf ) { if( max_ ) { comma::csv::format::traits< T, F >::to_bin( *max_, buf ); } }
            Base* clone() const { return new Max< T, F >( *this ); }
        private:
            friend class Centre< T, F >;
            friend class Diameter< T, F >;
            friend class Radius< T, F >;
            boost::optional< T > max_;
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Sum : public Base
    {
        public:
            void push( const char* buf )
            {
                T t = comma::csv::format::traits< T, F >::from_bin( buf );
                sum_ = sum_ ? *sum_ + t : t; 
            }
            void calculate( char* buf ) { if( sum_ ) { comma::csv::format::traits< T, F >::to_bin( *sum_, buf ); } }
            Base* clone() const { return new Sum< T, F >( *this ); }
        private:
            boost::optional< T > sum_;
    };
    
    template < comma::csv::format::types_enum F >
    class Sum< boost::posix_time::ptime, F > : public Base
    {
        void push( const char* ) { COMMA_THROW( comma::exception, "sum not defined for time" ); }
        void calculate( char* ) { COMMA_THROW( comma::exception, "sum not defined for time" ); }
        Base* clone() const { COMMA_THROW( comma::exception, "sum not defined for time" ); }
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Centre : public Base
    {
        public:
            void push( const char* buf ) { min_.push( buf ); max_.push( buf ); }
            void calculate( char* buf ) { if( min_.min_ ) { comma::csv::format::traits< T, F >::to_bin( *min_.min_ + ( *max_.max_ - *min_.min_ ) / 2, buf ); } }
            Base* clone() const { return new Centre< T, F >( *this ); }
        private:
            Min< T, F > min_;
            Max< T, F > max_;
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Mean : public Base
    {
        public:
            Mean() : count_( 0 ) {}
            void push( const char* buf )
            {
                T t = comma::csv::format::traits< T, F >::from_bin( buf );
                ++count_;
                mean_ = mean_ ? *mean_ + ( t - *mean_ ) / count_ : t ;
            }
            void calculate( char* buf ) { if( count_ > 0 ) { comma::csv::format::traits< T, F >::to_bin( *mean_, buf ); } }
            Base* clone() const { return new Mean< T, F >( *this ); }
        private:
            boost::optional< T > mean_;
            std::size_t count_;
    };
    
    template < typename T, comma::csv::format::types_enum F > class Stddev;
    
    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Variance : public Base // todo: generalise for kth moment
    {
        public:
            Variance() : count_( 0 ) {}
            void push( const char* buf )
            {
                T t = comma::csv::format::traits< T, F >::from_bin( buf );
                ++count_;
                mean_ = mean_ ? *mean_ + ( t - *mean_ ) / count_ : t;
                squares_ = squares_ ? *squares_ + ( t * t - *squares_ ) / count_ : t * t;
            }
            void calculate( char* buf ) { if( count_ > 0 ) { comma::csv::format::traits< T, F >::to_bin( *squares_ - *mean_ * *mean_, buf ); } }
            Base* clone() const { return new Variance< T, F >( *this ); }
        private:
            friend class Stddev< T, F >;
            boost::optional< T > mean_;
            boost::optional< T > squares_;
            std::size_t count_;
    };
    
    template < comma::csv::format::types_enum F >
    class Variance< boost::posix_time::ptime, F > : public Base
    {
        void push( const char* ) { COMMA_THROW( comma::exception, "variance not implemented for time, todo" ); }
        void calculate( char* ) { COMMA_THROW( comma::exception, "variance not implemented for time, todo" ); }
        Base* clone() const { COMMA_THROW( comma::exception, "variance not implemented for time, todo" ); }
    };
    
    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Stddev : public Base
    {
        public:
            void push( const char* buf ) { variance_.push( buf ); }
            void calculate( char* buf ) { if( variance_.count_ > 0 ) { comma::csv::format::traits< T, F >::to_bin( static_cast< T >( std::sqrt( static_cast< long double >( *variance_.squares_ - *variance_.mean_ * *variance_.mean_ ) ) ), buf ); } }
            Base* clone() const { return new Stddev< T, F >( *this ); }
        private:
            Variance< T, F > variance_;
    };
    
    template < comma::csv::format::types_enum F >
    class Stddev< boost::posix_time::ptime, F > : public Base
    {
        void push( const char* ) { COMMA_THROW( comma::exception, "standard deviation not implemented for time, todo" ); }
        void calculate( char* ) { COMMA_THROW( comma::exception, "standard deviation not implemented for time, todo" ); }
        Base* clone() const { COMMA_THROW( comma::exception, "standard deviation not implemented for time, todo" ); }
    };
    
    template < typename T > struct Diff
    { 
        typedef T Type;
        static Type subtract( T lhs, T rhs ) { return lhs - rhs; }
    };

    template <> struct Diff< boost::posix_time::ptime >
    {
        typedef double Type;
        static double subtract( boost::posix_time::ptime lhs, boost::posix_time::ptime rhs ) { return double( ( lhs - rhs ).total_microseconds() ) / 1e6; }
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Diameter : public Base
    {
        public:
            void push( const char* buf ) { min_.push( buf ); max_.push( buf ); }
            void calculate( char* buf ) { if( min_.min_ ) { comma::csv::format::traits< typename Diff< T >::Type >::to_bin( Diff< T >::subtract( *max_.max_, *min_.min_ ), buf ); } }
            Base* clone() const { return new Diameter< T, F >( *this ); }
        private:
            Min< T, F > min_;
            Max< T, F > max_;
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Radius : public Base
    {
        public:
            void push( const char* buf ) { min_.push( buf ); max_.push( buf ); }
            void calculate( char* buf ) { if( min_.min_ ) { comma::csv::format::traits< typename Diff< T >::Type >::to_bin( Diff< T >::subtract( *max_.max_, *min_.min_ ) / 2, buf ); } }
            Base* clone() const { return new Radius< T, F >( *this ); }
        private:
            Min< T, F > min_;
            Max< T, F > max_;
    };

    template < typename T, comma::csv::format::types_enum F = comma::csv::format::type_to_enum< T >::value >
    class Size : public Base
    {
        public:
            Size() : count_( 0 ) {}
            void push( const char* ) { ++count_; }
            void calculate( char* buf ) { comma::csv::format::traits< comma::uint32 >::to_bin( count_, buf ); }
            Base* clone() const { return new Size< T, F >( *this ); }
        private:
            std::size_t count_;
    };
    
    struct Enum { enum Values { min, max, centre, mean, sum, size, radius, diameter, variance, stddev }; };
    
    static Enum::Values from_name( const std::string& name )
    {
        if( name == "min" ) { return Enum::min; }
        else if( name == "max" ) { return Enum::max; }
        else if( name == "centre" ) { return Enum::centre; }
        else if( name == "mean" ) { return Enum::mean; }
        else if( name == "sum" ) { return Enum::sum; }
        else if( name == "radius" ) { return Enum::radius; }
        else if( name == "diameter" ) { return Enum::diameter; }
        else if( name == "var" ) { return Enum::variance; }
        else if( name == "stddev" ) { return Enum::stddev; }
        else if( name == "size" ) { return Enum::size; }
        else { COMMA_THROW( comma::exception, "expected operation name, got " << name ); }
    }
    
    template < Enum::Values E > struct traits {};
    template <> struct traits< Enum::min > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Min< T, F > Type; }; };
    template <> struct traits< Enum::max > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Max< T, F > Type; }; };
    template <> struct traits< Enum::centre > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Centre< T, F > Type; }; };
    template <> struct traits< Enum::mean > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Mean< T, F > Type; }; };
    template <> struct traits< Enum::sum > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Sum< T, F > Type; }; };
    template <> struct traits< Enum::size > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Size< T, F > Type; }; };
    template <> struct traits< Enum::radius > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Radius< T, F > Type; }; };
    template <> struct traits< Enum::diameter > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Diameter< T, F > Type; }; };
    template <> struct traits< Enum::variance > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Variance< T, F > Type; }; };
    template <> struct traits< Enum::stddev > { template < typename T, comma::csv::format::types_enum F > struct FromEnum { typedef Stddev< T, F > Type; }; };
} // namespace Operations

class OperationBase
{
    public:
        virtual ~OperationBase() {}
        virtual void push( const char* buf ) = 0;
        virtual void calculate() = 0;
        virtual OperationBase* clone() const = 0;
        const comma::csv::format& output_format() const { return output_format_; }
        const char* buffer() const { return &buffer_[0]; }
        
    protected:
        boost::ptr_vector< Operations::Base > operations_;
        comma::csv::format input_format_;
        std::vector< comma::csv::format::element > input_elements_;
        comma::csv::format output_format_;
        std::vector< comma::csv::format::element > output_elements_;
        std::vector< char > buffer_;
        
        OperationBase* deep_copy_to_( OperationBase* lhs ) const
        {
            lhs->input_format_ = input_format_;
            lhs->input_elements_ = input_elements_;
            lhs->output_format_ = output_format_;
            lhs->output_elements_ = output_elements_;
            lhs->buffer_ = buffer_;
            for( std::size_t i = 0; i < operations_.size(); ++i ) { lhs->operations_.push_back( operations_[i].clone() ); }
            return lhs;
        }
};

template < Operations::Enum::Values E >
struct Operation : public OperationBase
{
    Operation() {}
    Operation( const comma::csv::format& format )
    {
        input_format_ = format;
        input_elements_.reserve( input_format_.count() );
        for( std::size_t i = 0; i < input_format_.count(); ++i ) { input_elements_.push_back( input_format_.offset( i ) ); }
        operations_.reserve( input_elements_.size() );
        for( std::size_t i = 0; i < input_elements_.size(); ++i )
        {
            comma::csv::format::types_enum output_type = input_elements_[i].type;
            switch( E ) // quick and dirty, implement in operations::traits, just no time
            {
                case Operations::Enum::radius:
                case Operations::Enum::diameter:
                    if( input_elements_[i].type == comma::csv::format::time || input_elements_[i].type == comma::csv::format::long_time ) { output_type = comma::csv::format::double_t; }
                    break;
                case Operations::Enum::size:
                    output_type = comma::csv::format::uint32;
                    break;
                default:
                    break;
            }
            switch( input_elements_[i].type )
            {
                case comma::csv::format::char_t: operations_.push_back( new typename Operations::traits< E >::template FromEnum< char, comma::csv::format::char_t >::Type ); break;
                case comma::csv::format::int8: operations_.push_back( new typename Operations::traits< E >::template FromEnum< char, comma::csv::format::int8 >::Type ); break;
                case comma::csv::format::uint8: operations_.push_back( new typename Operations::traits< E >::template FromEnum< unsigned char, comma::csv::format::uint8 >::Type ); break;
                case comma::csv::format::int16: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::int16, comma::csv::format::int16 >::Type ); break;
                case comma::csv::format::uint16: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::uint16, comma::csv::format::uint16 >::Type ); break;
                case comma::csv::format::int32: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::int32, comma::csv::format::int32 >::Type ); break;
                case comma::csv::format::uint32: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::uint32, comma::csv::format::uint32 >::Type ); break;
                case comma::csv::format::int64: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::int64, comma::csv::format::int64 >::Type ); break;
                case comma::csv::format::uint64: operations_.push_back( new typename Operations::traits< E >::template FromEnum< comma::uint64, comma::csv::format::uint64 >::Type ); break;
                case comma::csv::format::float_t: operations_.push_back( new typename Operations::traits< E >::template FromEnum< float, comma::csv::format::float_t >::Type ); break;
                case comma::csv::format::double_t: operations_.push_back( new typename Operations::traits< E >::template FromEnum< double, comma::csv::format::double_t >::Type ); break;
                case comma::csv::format::time: operations_.push_back( new typename Operations::traits< E >::template FromEnum< boost::posix_time::ptime, comma::csv::format::time >::Type ); break;
                case comma::csv::format::long_time: operations_.push_back( new typename Operations::traits< E >::template FromEnum< boost::posix_time::ptime, comma::csv::format::long_time >::Type ); break;
                default: COMMA_THROW( comma::exception, "operations for " << i << "th element in " << format.string() << " not defined" );
            }
            output_format_ += comma::csv::format::to_format( output_type );
        }
        for( std::size_t i = 0; i < input_elements_.size(); ++i ) { output_elements_.push_back( output_format_.offset( i ) ); }
        buffer_.resize( output_format_.size() );
    }
    
    void push( const char* buf )
    {
        for( std::size_t i = 0; i < operations_.size(); ++i ) { operations_[i].push( buf + input_elements_[i].offset ); }
    }
    
    void calculate()
    {
        for( std::size_t i = 0; i < operations_.size(); ++i ) { operations_[i].calculate( &buffer_[0] + output_elements_[i].offset ); }
    }
    
    OperationBase* clone() const { Operation< E >* op = new Operation< E >; return deep_copy_to_( op ); }
};

typedef boost::unordered_map< comma::uint32, boost::ptr_vector< OperationBase >* > OperationsMap;

static void init_operations( boost::ptr_vector< OperationBase >& operations
                           , const std::vector< Operations::Enum::Values >& operation_ids
                           , const comma::csv::format& format )
{
    static boost::ptr_vector< OperationBase > sample;
    if( sample.empty() )
    {
        sample.reserve( operation_ids.size() );
        for( std::size_t i = 0; i < operation_ids.size(); ++i )
        { 
            switch( operation_ids[i] )
            {
                case Operations::Enum::min: sample.push_back( new Operation< Operations::Enum::min >( format ) ); break;
                case Operations::Enum::max: sample.push_back( new Operation< Operations::Enum::max >( format ) ); break;
                case Operations::Enum::centre: sample.push_back( new Operation< Operations::Enum::centre >( format ) ); break;
                case Operations::Enum::mean: sample.push_back( new Operation< Operations::Enum::mean >( format ) ); break;
                case Operations::Enum::radius: sample.push_back( new Operation< Operations::Enum::radius >( format ) ); break;
                case Operations::Enum::diameter: sample.push_back( new Operation< Operations::Enum::diameter >( format ) ); break;
                case Operations::Enum::variance: sample.push_back( new Operation< Operations::Enum::variance >( format ) ); break;
                case Operations::Enum::stddev: sample.push_back( new Operation< Operations::Enum::stddev >( format ) ); break;
                case Operations::Enum::sum: sample.push_back( new Operation< Operations::Enum::sum >( format ) ); break;
                case Operations::Enum::size: sample.push_back( new Operation< Operations::Enum::size >( format ) ); break;
            }
        }
    }
    operations.clear();
    for( std::size_t i = 0; i < sample.size(); ++i ) { operations.push_back( sample[i].clone() ); }
}

static void calculate_and_output( const comma::csv::Options& csv, OperationsMap& operations, boost::optional< comma::uint32 > block, bool has_block, bool has_id )
{
    for( OperationsMap::iterator it = operations.begin(); it != operations.end(); ++it )
    {
        for( std::size_t i = 0; i < it->second->size(); ++i )
        {
            ( *it->second )[i].calculate();
            if( csv.binary() ) { std::cout.write( ( *it->second )[i].buffer(), ( *it->second )[i].output_format().size() ); }
            else { if( i > 0 ) { std::cout << csv.delimiter; } std::cout << ( *it->second )[i].output_format().bin_to_csv( ( *it->second )[i].buffer(), csv.delimiter, 12 ); }
        }
        if( csv.binary() )
        {
            if( has_id )  { std::cout.write( reinterpret_cast< const char* >( &it->first ), sizeof( comma::uint32 ) ); } // quick and dirty
            if( has_block ) { std::cout.write( reinterpret_cast< const char* >( &( *block ) ), sizeof( comma::uint32 ) ); } // quick and dirty        
            std::cout.flush();
        }
        else
        {
            if( has_id ) { std::cout << csv.delimiter << it->first; }
            if( has_block ) { std::cout << csv.delimiter << *block; }
            std::cout << std::endl;
        }
    }
    for( OperationsMap::iterator it = operations.begin(); it != operations.end(); ++it ) { delete it->second; } // quick and dirty
    operations.clear();
}

int main( int ac, char** av )
{
    try
    {
        comma::command_line_options options( ac, av );
        if( options.exists( "--help,-h" ) ) { usage(); }
        std::vector< std::string > unnamed = options.unnamed( "", "--binary,-b,--delimiter,-d,--format,--fields,-f" );
        comma::csv::Options csv( options );
        #ifdef WIN32
        if( csv.binary() ) { _setmode( _fileno( stdin ), _O_BINARY ); _setmode( _fileno( stdout ), _O_BINARY ); }
        #endif
        if( unnamed.empty() ) { std::cerr << "csv-calc: please specify operations" << std::endl; exit( 1 ); }
        std::vector< std::string > v = comma::split( unnamed[0], ',' );
        std::vector< Operations::Enum::Values > operation_ids( v.size() );
        for( std::size_t i = 0; i < v.size(); ++i ) { operation_ids[i] = Operations::from_name( v[i] ); }
        boost::optional< comma::csv::format > format;
        if( csv.binary() ) { format = csv.format(); }
        else if( options.exists( "--format" ) ) { format = comma::csv::format( options.value< std::string >( "--format" ) ); }
        boost::scoped_ptr< asciiInput > ascii;
        boost::scoped_ptr< binaryInput > binary;
        if( csv.binary() ) { binary.reset( new binaryInput( csv ) ); }
        else { ascii.reset( new asciiInput( csv, format ) ); }
        OperationsMap operations;
        boost::optional< comma::uint32 > block;
        bool has_block = csv.has_field( "block" );
        bool has_id = csv.has_field( "id" );
        comma::SignalFlag is_shutdown;
        while( !is_shutdown && std::cin.good() && !std::cin.eof() )
        { 
            const Values* v = csv.binary() ? binary->read() : ascii->read();
            if( v == NULL ) { break; }
            if( has_block )
            {
                if( block && *block != v->block() ) { calculate_and_output( csv, operations, block, has_block, has_id ); }
                block = v->block();
            }
            OperationsMap::iterator it = operations.find( v->id() );
            if( it == operations.end() )
            {
                it = operations.insert( std::make_pair( v->id(), new boost::ptr_vector< OperationBase > ) ).first;
                init_operations( *it->second, operation_ids, v->format() );
            }
            for( std::size_t i = 0; i < it->second->size(); ++i ) { ( *it->second )[i].push( v->buffer() ); }
        }
        calculate_and_output( csv, operations, block, has_block, has_id );
        return 0;
    }
    catch( std::exception& ex ) { std::cerr << "csv-calc: " << ex.what() << std::endl; }
    catch( ... ) { std::cerr << "csv-calc: unknown exception" << std::endl; }
}
