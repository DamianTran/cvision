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

#include "EZC/toolkit/charsurf.hpp"
#include "EZC/toolkit/string.hpp"

namespace EZC
{

bool ptr_at_string(const char* ptr, const char* query,
                   bool case_insensitive)
{
    if(!(*ptr) || !(*query)) return false;
    while((*ptr) && (*query))
    {
        if(*ptr != *query)
        {
            if(!case_insensitive || !case_cmp(*ptr, *query))
            {
                return false;
            }
        }

        ++ptr;
        ++query;
    }
    return !(*query);
}

std::string read_to(const char** ptr, const char* query)
{

    std::string output;

    if(!(*ptr) || !query)
    {
        return output;
    }

    const char* begin = *ptr;

    while(**ptr)
    {
        if(ptr_at_string(*ptr, query))
        {
            output.assign(begin, *ptr);
            return output;
        }
        ++(*ptr);
    }

    return output;

}

bool replace_all(std::string& str,
                        const std::string& sequence,
                        const std::string& replacement)
{

    bool status = false;
    for(size_t i = 0; i < str.size(); ++i)
    {
        if(ptr_at_string(&str[i], sequence.c_str()))
        {
            str.replace(str.begin() + i, str.begin() + i + sequence.size(),
                        replacement);
            status = true;
        }
    }

    return status;
}

void remove_all(std::string& str,
                const std::string& sequence)
{
    for(size_t i = 0; i < str.size(); ++i)
    {
        if(ptr_at_string(&str[i], sequence.c_str()))
        {
            str.erase(str.begin() + i, str.begin() + i + sequence.size());
        }
    }

}

unsigned int num_matches(const char* query, const char* background,
                         bool case_insensitive)
{

    unsigned int output = 0;

    if(!(*background) || !(*query))
    {
        return output;
    }

    while(*background)
    {
        if(ptr_at_string(background, query, case_insensitive))
        {
            ++output;
        }
        ++background;
    }

    return output;

}

bool shiftTextIndex(unsigned int& index, const std::string& text,
                           const unsigned int& distance, const bool forward)
{

    size_t i, L = text.size();
    if(forward)
    {
        if(index < L-1)
        {
            for(i = 0; (i < distance) && (index < L); ++i)
            {
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index < L))  // Shift to end of word if not already so
                {
                    ++index;
                }
                while(isCharType(text[index], " ,;._+\\/?:\"") && (index < L))  // Shift past punctuation
                {
                    ++index;
                }
            }
        }
    }
    else
    {
        if(index > 0)
        {
            for(i = 0; (i < distance) && (index > 0); ++i)
            {
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index > 0))  // Shift to beginning of word if not already so
                {
                    --index;
                }
                while(isCharType(text[index], " ,;._+\\/?:\"") && (index > 0))  // Shift past punctuation
                {
                    --index;
                }
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index > 0))  // Shift to beginning of next index word
                {
                    --index;
                }
            }
        }
    }

    return i == distance;
}

bool read_word(const char** ptr, std::string& output)
{
    if(!(**ptr)) return false;

    const char* begin = *ptr;

    while(**ptr && !isTextDelim(**ptr))
    {
        ++(*ptr);
    }

    if(*ptr != begin)
    {

        output.assign(begin, *ptr);

        while(**ptr && isTextDelim(**ptr))
        {
            ++(*ptr);
        }

        return true;

    }

    return false;
}

std::string get_word(const char* ptr)
{
    std::string output;
    const char* begin = ptr;

    while(*ptr && !isTextDelim(*ptr))
    {
        ++ptr;
    }

    output.assign(begin, ptr);

    return output;
}

}
