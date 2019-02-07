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
