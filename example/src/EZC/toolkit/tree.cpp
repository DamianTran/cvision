/** /////////////////////////////////////////////////////////////
//
//  CVision: the flexible cascading-style GUI library for C++
//
// //////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2019 Damian Tran
//
// DESCRIPTION:
//
// CVision is a graphical user interface (GUI) library that
// attempts to simplify and speed up the process of desktop
// app design.  CVision incorporates a cascading structure
// scheme that resembles the following:
//
// App -> View -> Panel -> Element -> Primitives/Sprites
//
// The subsequent connection of each "leaf" of the hierarchy
// automatically ensures that the element will be updated,
// drawn to the renderer, and otherwise disposed of at
// the program's termination.
//
// LEGAL:
//
// Modification and redistribution of CVision is freely 
// permissible under any circumstances.  Attribution to the 
// Author ("Damian Tran") is appreciated but not necessary.
// 
// CVision is an open source library that is provided to you
// (the "User") AS IS, with no implied or explicit
// warranties.  By using CVision, you acknowledge and agree
// to this disclaimer.  Use of CVision in the Users's programs
// or as a part of a derivative library is performed at
// the User's OWN RISK.
//
// ACKNOWLEDGEMENTS:
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
/////////////////////////////////////////////////////////////  **/

#include "EZC/toolkit/tree.hpp"

#include "boost/property_tree/xml_parser.hpp"
#include "boost/filesystem.hpp"

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

std::ostream& print_tree(std::ostream& output, const pt::ptree& tree){
    std::function<void(const std::pair<std::string, pt::ptree>&, int)> recurse_print;

    recurse_print = [&](const std::pair<std::string, pt::ptree>& node,
                         int level = 0){
        for(size_t i = 0; i < level; ++i){
            output << "-";
        }
        output << node.first << '\n';
        for(auto& sub_node : node.second){
            if(!node.second.empty()){
                recurse_print(sub_node, level + 1);
            }
        }
    };

    for(auto& node : tree){
        recurse_print(node, 0);
    }

    return output;
}

std::vector<std::string> getData(const pt::ptree& tree,
                                       const std::string& property){

    std::vector<std::string> output;

    std::function<void(const std::pair<std::string, pt::ptree>&)> recurse_branch;

    recurse_branch = [&](const std::pair<std::string, pt::ptree>& node){

        if(node.first == property){
            output.push_back(node.second.get("", ""));
        }
        for(auto& sub_node : node.second){
            if(!node.second.empty()){
                recurse_branch(sub_node);
            }
        }
    };

    for(auto& node : tree){
        recurse_branch(node);
    }

    return output;
}
