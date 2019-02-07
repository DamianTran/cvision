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

#include "EZC/toolkit/sorted_vector.hpp"
#include "EZC/toolkit/string.hpp"

#include <boost/range/adaptors.hpp>

#include "EZC/algorithm.hpp"

using namespace EZC;

void SortedStringVector::update_map(){

    size_t idx = 0;
    char last_c;

    for(auto& str : *this){

        if(!str.empty()){

            if(str.front() != last_c){

                search_key[str.front()] = idx;
                last_c = str.front();

            }

        }

        ++idx;

    }

}

void SortedStringVector::get_map_idx(const char& c, size_t& start, size_t& end){

    for(auto& key_pair : boost::adaptors::reverse(search_key)){

        if(key_pair.first < c){
            start = key_pair.second;
            break;
        }

    }

    for(auto& key_pair : search_key){

        if(key_pair.first > c){

            end = key_pair.second;
            break;

        }

    }

}

unsigned int SortedStringVector::match(const std::string& search_query){

    if(search_query.empty()) return UINT_MAX;

    size_t start_idx = 0, end_idx = size();

    get_map_idx(search_query.front(), start_idx, end_idx);

    while(start_idx != end_idx){

        if((*this)[start_idx] == search_query) return start_idx;

        ++start_idx;

    }

    return UINT_MAX;

}

bool SortedStringVector::anyEqual(const std::string& search_query){

    if(search_query.empty()) return false;

    size_t start_idx = 0, end_idx = size();

    get_map_idx(search_query.front(), start_idx, end_idx);

    while(start_idx != end_idx){

        if((*this)[start_idx] == search_query) return true;

        ++start_idx;

    }

    return false;

}

std::string& SortedStringVector::operator[](const std::string& search_query){

    if(search_query.empty()) throw std::invalid_argument("Search query is empty");

    size_t start_idx = 0, end_idx = size();

    get_map_idx(search_query.front(), start_idx, end_idx);

    while(start_idx != end_idx){

        if(cmpString((*this)[start_idx], search_query, CMP_STR_CASE_INSENSITIVE)) return (*this)[start_idx];

        ++start_idx;

    }

    if(isUpperCase(search_query.front())){

        start_idx = 0;
        end_idx = size();

        get_map_idx(search_query.front() + 32, start_idx, end_idx);

        while(start_idx != end_idx){

            if(cmpString((*this)[start_idx], search_query, CMP_STR_CASE_INSENSITIVE)) return (*this)[start_idx];

            ++start_idx;

        }

    }
    else if(isLowerCase(search_query.front())){

        start_idx = 0;
        end_idx = size();

        get_map_idx(search_query.front(), start_idx, end_idx);

        while(start_idx != end_idx){

            if(cmpString((*this)[start_idx], search_query, CMP_STR_CASE_INSENSITIVE)) return (*this)[start_idx];

            ++start_idx;

        }

    }

    throw std::invalid_argument("Search query does not exist in sorted vector");

}
