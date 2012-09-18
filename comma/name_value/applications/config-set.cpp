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
    std::cerr << "usage: cat data.xml | config-set <path=value> [<options>]" << std::endl;
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
        std::vector< std::string > unnamed = options.unnamed( "--linewise,-l", "--from,--equal-sign,-e,--delimiter,-d" );
        if( unnamed.empty() ) { std::cerr << std::endl << "config-set: xpath missing" << std::endl; usage(); }
        std::vector< boost::property_tree::ptree::path_type > paths( unnamed.size() );
        std::vector< std::string > values( unnamed.size() );
        std::vector< boost::property_tree::ptree > trees( unnamed.size() );
        std::deque< bool > is_tree( unnamed.size() );
        for( std::size_t i = 0; i < unnamed.size(); ++i )
        {
            std::vector< std::string > v = Ark::split( unnamed[i], '=' );
            if( v.size() < 2 ) { ARK_THROW_STREAM( Ark::Exception, "expected path=value, got " << unnamed[i] ); }
            values[i] = Ark::join( v.begin() + 1, v.end(), '=' );
            std::istringstream iss( values[i] );
            input( iss, trees[i] );
            is_tree[i] = trees[i].find( values[i] ) == trees[i].not_found();
            Ark::XPath xpath( v[0], '/' );
            paths[i] = xpath.toString( '.' );
        }
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
                static const boost::property_tree::ptree::path_type empty;
                for( std::size_t i = 0; i < paths.size(); ++i )
                {
                    if( is_tree[i] ) { ptree.put_child( paths[i], trees[i] ); }
                    else { ptree.put( paths[i], values[i] ); }
                }
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
            for( std::size_t i = 0; i < paths.size(); ++i )
            {
                if( is_tree[i] ) { ptree.put_child( paths[i], trees[i] ); }
                else { ptree.put( paths[i], values[i] ); }
            }            
            output( std::cout, ptree );
        }
    }
    catch( std::exception& ex )
    {
        std::cerr << std::endl << "config-set: " << ex.what() << std::endl << std::cerr << std::endl;
    }
    catch( ... )
    {
        std::cerr << std::endl << "config-set: unknown exception" << std::endl << std::endl;
    }
    return 1;
}
