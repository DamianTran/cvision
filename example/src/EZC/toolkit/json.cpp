/** ////////////////////////////////////////////////////////////////

    *** EZ-C++ - A simplified C++ experience ***

        Yet (another) open source library for C++

        Original Copyright (C) Damian Tran 2019

        By aiFive Technologies, Inc. for developers

    Copying and redistribution of this code is freely permissible.
    Inclusion of the above notice is preferred but not required.

    This software is provided AS IS without any expressed or implied
    warranties.  By using this code, and any modifications and
    variants arising thereof, you are assuming all liabilities and
    risks that may be thus associated.

////////////////////////////////////////////////////////////////  **/

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
