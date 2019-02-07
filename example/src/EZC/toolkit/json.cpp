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

#include "EZC/toolkit/json.hpp"
#include "EZC/toolkit/string.hpp"
#include "EZC/toolkit/filesystem.hpp"

#include <stdio.h>
#include <unistd.h>

using namespace EZC;

void get_JSON_name_type(const std::string& filename,
                        const std::string& var_name,
                        StringVector& output){

    if(access(filename.c_str(), F_OK)) return;

    FILE* inFILE = fopen(filename.c_str(), "rb");

    size_t fileSIZE = getFileSize(inFILE);
    char data[fileSIZE+1];
    fread(data, fileSIZE, sizeof(char), inFILE);

    unsigned int foundIndex = UINT_MAX, matchIndex = 0;

    for(size_t i = 0; i < fileSIZE; ++i){
        if(check_letter(data[i], var_name[matchIndex])){
            if(foundIndex == UINT_MAX){
                foundIndex = i;
            }

            ++matchIndex;

            if(matchIndex == var_name.size()){

                ++i;
                while((i < fileSIZE) && isCharType(data[i], " :\"")){
                    ++i;
                }
                foundIndex = i;
                while((i < fileSIZE) && !isCharType(data[i], "\"}")){
                    ++i;
                }

                output.emplace_back();
                output.back().assign(data + foundIndex, data + i);

                foundIndex = UINT_MAX;
                matchIndex = 0;
            }
        }
        else{
            matchIndex = 0;
            foundIndex = UINT_MAX;
        }
    }

    fclose(inFILE);

}
