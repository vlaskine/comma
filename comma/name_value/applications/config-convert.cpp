#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <Ark/Base/Exception.h>
#include <Ark/Application/CommandLineOptions.h>
#include <Ark/Application/SignalFlag.h>
#include <Ark/Serialization/PTree.h>
#include <Ark/XPath/XPath.h>

static void usage()
{
    std::cerr << std::endl;
    std::cerr << "take a stream of name-value style input on stdin," << std::endl;
    std::cerr << "output value at given path on stdout" << std::cerr;
    std::cerr << std::endl;
    std::cerr << "usage: cat data.xml | config-convert <from> [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<path>: x-path, e.g. \"command/type\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "data options" << std::endl;
    std::cerr << "    --from <format>: input format; default name-value" << std::endl;
    std::cerr << "    --to <format>: output format; default name-value" << std::endl;
    std::cerr << std::endl;
    std::cerr << "formats" << std::endl;
    std::cerr << "    info: info data (see boost::property_tree)" << std::endl;
    std::cerr << "    ini: ini data" << std::endl;
    std::cerr << "    json: json data" << std::endl;
    std::cerr << "    name-value: name=value-style data; e.g. x={a=1,b=2},y=3" << std::endl;
    std::cerr << "    path-value: path=value-style data; e.g. x/a=1,x/b=2,y=3" << std::endl;
    std::cerr << "    xml: xml data" << std::endl;
    std::cerr << std::endl;
    std::cerr << "name/path-value options:" << std::endl;
    std::cerr << "    --equal-sign,-e=<equal sign>: default '='" << std::endl;
    std::cerr << "    --delimiter,-d=<delimiter>: default ','" << std::endl;
    std::cerr << std::endl;
    std::cerr << "data flow options:" << std::endl;
    std::cerr << "    --linewise,-l: if present, treat each input line as a record" << std::endl;
    std::cerr << "                   if absent, treat all of the input as one record" << std::endl;
    std::cerr << std::endl;
    exit( 1 );
}

static char equalSign;
static char delimiter;
static bool linewise;

enum Types { ini, info, json, xml, name_value, path_value };

template < Types Type > struct Traits {};

template <> struct Traits< ini >
{
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { boost::property_tree::read_ini( is, ptree ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { boost::property_tree::write_ini( os, ptree ); }
};

template <> struct Traits< info >
{
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { boost::property_tree::read_info( is, ptree ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { boost::property_tree::write_info( os, ptree ); }
};

template <> struct Traits< json >
{
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { boost::property_tree::read_json( is, ptree ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { boost::property_tree::write_json( os, ptree ); }
};

template <> struct Traits< xml >
{
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { boost::property_tree::read_xml( is, ptree ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { boost::property_tree::write_xml( os, ptree ); }
};

template <> struct Traits< name_value >
{
    // todo: handle indented input (quick and dirty: use exceptions)
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { Ark::PropertyTree::fromNameValue( is, ptree, equalSign, delimiter ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { Ark::PropertyTree::toNameValue( os, ptree, !linewise, equalSign, delimiter ); }
};

template <> struct Traits< path_value > // quick and dirty
{
    static void input( std::istream& is, boost::property_tree::ptree& ptree )
    {
        std::string s;
        if( linewise )
        {
            std::getline( is, s );
        }
        else
        {
            while( is.good() && !is.eof() ) // quick and dirty: read to the end of file
            {
                std::string t;
                std::getline( is, t );
                std::string::size_type pos = t.find_first_not_of( ' ' );
                if( pos == std::string::npos || t[pos] == '#' ) { continue; }
                s += t + delimiter;
            }
        }
        ptree = Ark::PropertyTree::fromPathValueString( s, equalSign, delimiter );
    }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { Ark::PropertyTree::toPathValue( os, ptree, equalSign, delimiter ); }
};

int main( int ac, char** av )
{
    try
    {
        Ark::CommandLineOptions options( ac, av );
        if( options.exists( "--help,-h" ) ) { usage(); }
        std::string from = options.value< std::string >( "--from", "name-value" );
        std::string to = options.value< std::string >( "--to", "name-value" );
        equalSign = options.value( "--equal-sign,-e", '=' );
        linewise = options.exists( "--linewise,-l" );
        char default_delimiter = ( to == "path-value" || from == "path-value" ) && !linewise ? '\n' : ',';
        delimiter = options.value( "--delimiter,-d", default_delimiter );
        void ( * input )( std::istream& is, boost::property_tree::ptree& ptree );
        void ( * output )( std::ostream& is, boost::property_tree::ptree& ptree );
        if( from == "ini" ) { input = &Traits< ini >::input; }
        else if( from == "info" ) { input = &Traits< info >::input; }
        else if( from == "json" ) { input = &Traits< json >::input; }
        else if( from == "xml" ) { input = &Traits< xml >::input; }
        else if( from == "path-value" ) { input = &Traits< path_value >::input; }
        else { input = &Traits< name_value >::input; }
        if( to == "ini" ) { output = &Traits< ini >::output; }
        else if( to == "info" ) { output = &Traits< info >::output; }
        else if( to == "json" ) { output = &Traits< json >::output; }
        else if( to == "xml" ) { output = &Traits< xml >::output; }
        else if( to == "path-value" ) { output = &Traits< path_value >::output; }
        else { output = &Traits< name_value >::output; }
        if( linewise )
        {
            Ark::SignalFlag isShutdown;
            while( std::cout.good() )
            {
                std::string line;
                std::getline( std::cin, line );
                if( isShutdown || !std::cin.good() || std::cin.eof() ) { break; }
                std::istringstream iss( line );
                boost::property_tree::ptree ptree;
                input( iss, ptree );
                std::ostringstream oss;
                output( oss, ptree );
                std::string s = oss.str();
                if( s.empty() ) { continue; }
                bool escaped = false;
                bool quoted = false;
                for( std::size_t i = 0; i < s.size(); ++i ) // quick and dirty
                {
                    if( escaped ) { escaped = false; continue; }
                    switch( s[i] )
                    {
                        case '\r': if( !quoted ) { s[i] = ' '; } break;
                        case '\\': escaped = true; break;
                        case '"' : quoted = !quoted; break;
                        case '\n': if( !quoted ) { s[i] = ' '; } break;
                    }

                }
                std::cout << s << std::endl;
            }
        }
        else
        {
            boost::property_tree::ptree ptree;
            input( std::cin, ptree );
            output( std::cout, ptree );
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << std::endl << "config-convert: " << ex.what() << std::endl << std::cerr << std::endl;
    }
    catch( ... )
    {
        std::cerr << std::endl << "config-convert: unknown exception" << std::endl << std::endl;
    }
    return 1;
}
