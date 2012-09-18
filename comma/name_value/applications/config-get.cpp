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
    std::cerr << "usage: cat data.xml | config-get <paths> [<options>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "<path>: x-path, e.g. \"command/type\"" << std::endl;
    std::cerr << std::endl;
    std::cerr << "data options" << std::endl;
    std::cerr << "    --from <what>" << std::endl;
    std::cerr << "      <what>" << std::endl;
    std::cerr << "        info: info data (see boost::property_tree)" << std::endl;
    std::cerr << "        ini: ini data" << std::endl;
    std::cerr << "        json: json data" << std::endl;
    std::cerr << "        name-value: name=value-style data; e.g. x={a=1,b=2},y=3" << std::endl;
    std::cerr << "        xml: xml data" << std::endl;
    std::cerr << "        default: name-value" << std::endl;
    std::cerr << std::endl;
    std::cerr << "name-value options:" << std::endl;
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

enum Types { ini, info, json, xml, nameValue };

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

template <> struct Traits< nameValue >
{
    // todo: handle indented input (quick and dirty: use exceptions)
    static void input( std::istream& is, boost::property_tree::ptree& ptree ) { Ark::PropertyTree::fromNameValue( is, ptree, equalSign, delimiter ); }
    static void output( std::ostream& os, boost::property_tree::ptree& ptree ) { Ark::PropertyTree::toNameValue( os, ptree, !linewise, equalSign, delimiter ); }
};

int main( int ac, char** av )
{
    try
    {
        Ark::CommandLineOptions options( ac, av );
        if( options.exists( "--help,-h" ) ) { usage(); }
        options.assertMutuallyExclusive( "--info,--ini,--json,--name-value,--xml" );
        std::vector< std::string > unnamed = options.unnamed( "--linewise,-l", "--from,--equal-sign,-e,--delimiter,-d" );
        if( unnamed.empty() ) { std::cerr << std::endl << "config-get: xpath missing" << std::endl; usage(); }
        std::vector< boost::property_tree::ptree::path_type > paths;
        for( std::size_t i = 0; i < unnamed.size(); ++i )
        {
            if( unnamed[i].find( '.' ) != std::string::npos ) { std::cerr << "config-get: got " << unnamed[i] << ", but paths containing '.' not supported (todo)" << std::endl; exit( 1 ); }
            Ark::XPath xpath( unnamed[i], '/' );
            paths.push_back( xpath.toString( '.' ) );
        }
        equalSign = options.value( "--equal-sign,-e", '=' );
        delimiter = options.value( "--delimiter,-d", ',' );
        linewise = options.exists( "--linewise,-l" );
        void ( * input )( std::istream& is, boost::property_tree::ptree& ptree );
        void ( * output )( std::ostream& is, boost::property_tree::ptree& ptree );
        std::string from = options.value< std::string >( "--from", "name-value" );
        if( from == "ini" ) { input = &Traits< ini >::input; output = &Traits< ini >::output; }
        else if( from == "info" ) { input = &Traits< info >::input; output = &Traits< info >::output; }
        else if( from == "json" ) { input = &Traits< json >::input; output = &Traits< json >::output; }
        else if( from == "xml" ) { input = &Traits< xml >::input; output = &Traits< xml >::output; }
        else { input = &Traits< nameValue >::input; output = &Traits< nameValue >::output; }
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
                static const boost::property_tree::ptree::path_type empty;
                for( std::size_t i = 0; i < paths.size(); ++i )
                {
                    boost::optional< boost::property_tree::ptree& > child = ptree.get_child_optional( paths[i] );
                    if( !child ) { continue; }
                    boost::optional< std::string > value = child->get_optional< std::string >( empty );
                    if( value && !value->empty() ) { oss << *value << std::endl; }
                    else { output( oss, *child ); }
                }
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
            static const boost::property_tree::ptree::path_type empty;
            for( std::size_t i = 0; i < paths.size(); ++i )
            {
                boost::optional< boost::property_tree::ptree& > child = ptree.get_child_optional( paths[i] );
                if( !child ) { continue; }
                boost::optional< std::string > value = child->get_optional< std::string >( empty );
                if( value && !value->empty() ) { std::cout << *value << std::endl; }
                else { output( std::cout, *child ); }
            }
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << std::endl << "config-get: " << ex.what() << std::endl << std::cerr << std::endl;
    }
    catch( ... )
    {
        std::cerr << std::endl << "config-get: unknown exception" << std::endl << std::endl;
    }
    return 1;
}
