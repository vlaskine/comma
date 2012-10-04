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

#include <gtest/gtest.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/filesystem/operations.hpp>
#include <comma/io/select.h>
#include <comma/io/stream.h>
#include <boost/concept_check.hpp>

#include <cstdio>
#include <fstream>

TEST( io, file_stream )
{
    {
        boost::filesystem::remove( "./blah" );
        boost::filesystem::remove( "./testfile" );
        comma::io::ostream ostream( "./testfile" );
        comma::io::istream istream( "./testfile" );
        std::string line;
        *ostream << "hello, world" << std::endl;
        ostream->flush();
        std::getline( *istream(), line );
        EXPECT_EQ( line, "hello, world" );
        ostream.close();
        istream.close();
        boost::filesystem::remove( "./testfile" );
    }
    // todo: more testing?
    system( "mkfifo blah" );
    comma::io::ostream os( "./blah" );
    while( true )
    {
        if( os() == NULL ) { std::cerr << "---> still NULL" << std::endl; }
        else
        {
            *os << "blah" << std::endl;
            std::cerr << "---> " << ( os->good() ? "good" : "bad" ) << std::endl;
        }
#ifndef WIN32
        sleep( 1 );
#else
        Sleep( 1000 );
#endif
    }
    system( "rm ./blah" );
}

TEST( io, std_stream )
{
    comma::io::istream istream( "-" );
    comma::io::ostream ostream( "-" );
    istream.close();
    ostream.close();
    // todo: more testing
}

TEST( io, tcp_stream )
{
//     boost::asio::io_service service;
//     boost::asio::ip::tcp::resolver resolver( service );
//     //boost::asio::ip::tcp::resolver::query query( "localhost" );
//     boost::asio::ip::tcp::resolver::query query( "localhost", "80" );
//     boost::asio::ip::tcp::resolver::iterator it = resolver.resolve( query );
//     std::cerr << "---> address=" << it->endpoint().address() << std::endl;
//     std::cerr << "---> port=" << it->endpoint().port() << std::endl;
//     //boost::asio::ip::tcp::endpoint endpoint( it, 12345 );
//     boost::asio::ip::tcp::socket socket( service );
//     //socket.connect( endpoint );
//     std::cerr << "---> socket.is_open()=" << socket.is_open() << std::endl;

//     // asio tcp server sample code
//     boost::asio::io_service service;
//     boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::tcp::v4(), 12345 );
//     boost::asio::ip::tcp::acceptor acceptor( service, endpoint );
//     boost::asio::ip::tcp::iostream stream;
//     std::cerr << "testTcpstream(): accepting..." << std::endl;
//     acceptor.accept( *stream.rdbuf() );
//     std::cerr << "testTcpstream(): accepted" << std::endl;
//     std::string line;
//     
//     int fd = stream.rdbuf()->native();
//     std::cerr << "testTcpstream(): fd = " << fd << std::endl;
//     comma::io::select select;
//     select.read().add( fd );
//     while( !stream.eof() )
//     {
//         std::cerr << "testTcpstream(): selecting..." << std::endl;
//         select.wait();
//         std::cerr << "testTcpstream(): select returned" << std::endl;
//         if( !select.read().ready( fd ) ) { break; }
//         std::getline( stream, line );
//         std::cerr << "testTcpstream(): just read \"" << line << "\"" << std::endl;
//     }
}

TEST( io, local_stream )
{
    #ifndef WIN32
    {
        boost::filesystem::remove( "./test.localsocket" );
        boost::asio::local::stream_protocol::endpoint endpoint( "test.localsocket" );
        EXPECT_TRUE( !boost::asio::local::stream_protocol::iostream( endpoint ) );
        boost::asio::io_service service;
        boost::asio::local::stream_protocol::acceptor acceptor( service, endpoint );
        EXPECT_TRUE( boost::asio::local::stream_protocol::iostream( endpoint ) );
        comma::io::istream istream( "./test.localsocket" );
        comma::io::ostream ostream( "./test.localsocket" );
        istream.close();
        ostream.close();
        acceptor.close();
        EXPECT_TRUE( !boost::asio::local::stream_protocol::iostream( endpoint ) );
        EXPECT_TRUE( !boost::filesystem::is_regular_file( "./test.localsocket" ) );
        boost::filesystem::remove( "./test.localsocket" );
    }
    {
        boost::filesystem::remove( "./test.file" );
        comma::io::ostream ostream( "./test.file" );
        ostream.close();
        boost::asio::io_service service;
        boost::asio::local::stream_protocol::endpoint endpoint( "test.file" );
        try { boost::asio::local::stream_protocol::acceptor acceptor( service, endpoint ); EXPECT_TRUE( false ); } catch( ... ) {}
        boost::filesystem::remove( "./test.file" );
    }
    #endif
}

int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}