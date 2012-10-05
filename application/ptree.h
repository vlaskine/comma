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

#ifndef COMMA_APPLICATION_PTREE_H_
#define COMMA_APPLICATION_PTREE_H_

#include <fstream>

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <comma/name_value/ptree.h>
#include <comma/visiting/apply.h>
#include <comma/visiting/traits.h>

namespace comma {

/// get config from info file, using ptree
/// if the file does not exist, it gets created using the default config
/// if the section @param name does not exist, it gets created using the default
/// @param filename the file name to read from
/// @param name the name of the section in the config file containing the config
/// @param defaultConfig the default config
template < typename C >
C config_from_info( const std::string& filename, const std::string& name = "", const C& default_config = C() )
{
    boost::property_tree::ptree tree;

    C config = default_config;

    std::ifstream file;
    file.open(filename.c_str());
    if (!file.is_open())
    {
        to_ptree v( tree, name );
        visiting::apply( v, config );
        boost::property_tree::info_parser::write_info(filename, tree);
    }
    else
    {
        boost::property_tree::info_parser::read_info(filename, tree);
        boost::property_tree::ptree::assoc_iterator it = tree.find(name);
        if( it == tree.not_found() )
        {
            // section not found, put default
            to_ptree v( tree, name );
            visiting::apply( v, config );
            boost::property_tree::info_parser::write_info(filename, tree);
        }
        else
        {
            from_ptree v( tree, name );
            visiting::apply( v, config );
        }
    }

    return config;
}

/// get config from ini file, using ptree
/// if the file does not exist, it gets created using the default config
/// if the section @param name does not exist, it gets created using the default
/// @param filename the file name to read from
/// @param name the name of the section in the config file containing the config
/// @param defaultConfig the default config
template < typename C >
C config_from_ini( const std::string& filename, const std::string& name = "", const C& defaultConfig = C() )
{
    boost::property_tree::ptree tree;

    C config = defaultConfig;

    std::ifstream file;
    file.open( filename.c_str() );
    if ( !file.is_open() )
    {
        to_ptree v( tree, name );
        visiting::apply( v, config );
        boost::property_tree::ini_parser::write_ini( filename, tree );
    }
    else
    {
        boost::property_tree::ini_parser::read_ini( filename, tree );
        boost::property_tree::ptree::assoc_iterator it = tree.find( name );
        if( it == tree.not_found() && !name.empty() )
        {
            // section not found, put default
            to_ptree v( tree, name );
            visiting::apply( v, config );
            boost::property_tree::ini_parser::write_ini(filename, tree);
        }
        else
        {
            from_ptree v( tree, name );
            visiting::apply( v, config );
        }
    }

    return config;
}

}

#endif // COMMA_APPLICATION_PTREE_H_
