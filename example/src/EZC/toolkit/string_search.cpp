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

#include "EZC/toolkit/string_search.hpp"
#include "EZC/toolkit/charsurf.hpp"

namespace EZC
{

bool getClusterMap(const char* str,
                   ez_cluster_map& output,
                   const int& offset)
{

    std::string word;
    const char* last_ptr = str;

    size_t i = 0;

    while(read_word(&str, word))
    {
        output[word.front()].emplace_back(last_ptr + offset);
        last_ptr = str;

        ++i;
    }

    return !output.empty();

}

void getClusterCodex(const char* str,
                            std::string& output,
                            const int& offset)
{

    char c = *(str + offset);

    int i = 0;

    if(output.empty())
    {
        output.push_back(c);
    }
    else
    {
        for(i = 0; i < output.size(); ++i)
        {
            if(c < output[i])
            {
                output.insert(output.begin() + i, c);
                goto next;
            }
            else if(c == output[i]) goto start;
        }

        output.push_back(c);

        start:;
    }

    while(*str)
    {
        while(*str && !isTextDelim(*str))
        {
            ++str;
        }

        while(*str && isTextDelim(*str))
        {
            ++str;
        }

        if(*str)
        {

            c = *(str + offset);

            for(i = 0; i < output.size(); ++i)
            {
                if(c < output[i])
                {
                    output.insert(output.begin() + i, c);
                    goto next;
                }
                else if(c == output[i]) goto next;
            }

            output.push_back(c);

            next:;

        }
    }

}

void getTreeCodex(const char* str,
                  tree_search_codex& output,
                  bool case_insensitive)
{

    std::string word;

    while(read_word(&str, word))
    {
        for(size_t i = 0; i < word.size(); ++i)
        {
            if(i >= output.size())
            {
                output.resize(i + 1);
            }

            if(output[i].empty())
            {
                output[i].push_back(word[i]);
            }
            else
            {
                for(size_t j = 0; j < output[i].size(); ++j)
                {
                    if(word[i] < output[i][j])
                    {
                        output[i].insert(output[i].begin() + j, word[i]);
                        goto inserted;
                    }
                    else if(word[i] == output[i][j])
                    {
                        goto inserted;
                    }
                }

                output[i].push_back(word[i]);

                inserted:;

                if(case_insensitive && !i && isLetter(word[i]))
                {

                    char c;
                    if(isUpperCase(word[i]))
                    {
                        c = word[i] + 32;
                    }
                    else if(isLowerCase(word[i]))
                    {
                        c = word[i] - 32;
                    }

                    for(size_t j = 0; j < output[i].size(); ++j)
                    {
                        if(c < output[i][j])
                        {
                            output[i].insert(output[i].begin() + j, c);
                            goto case_inserted;
                        }
                        else if(c == output[i][j])
                        {
                            goto case_inserted;
                        }
                    }

                    output[i].push_back(c);

                    case_inserted:;
                }
            }
        }
    }

}

std::string getAcronym(const char* text,
                       const char* acronym)
{
    int pos = 0, surf_pos;
    const char* surf, *ac_surf, *match = NULL;
    std::string output;

    if(!(*text) || !(*acronym)) return output;

    while(*text)
    {
        if(pos && ptr_at_string(text, acronym) &&
           (*(text - 1) == '(') && (*(text + strlen(acronym)) == ')'))
        {

            ac_surf = acronym + strlen(acronym) - 1;
            surf = text - 1;
            surf_pos = pos - 1;

            while(*surf && isTextDelim(*surf))
            {
                --surf;
                --surf_pos;
            }

            match = surf + 1;

            while((surf_pos >= 0) && !isSentenceDelim(*surf))
            {

                if(!surf_pos || at_word_begin(surf))
                {
                    if((*surf == *ac_surf) || case_cmp(*surf, *ac_surf))
                    {

                        if(ac_surf == acronym)
                        {
                            output.assign(surf, match);
                            return output;
                        }
                        else
                        {
                            --ac_surf;
                        }
                    }
                }

                --surf;
                --surf_pos;
            }
        }

        ++text;
        ++pos;
    }

    return output;
}

bool sentence_context(const char* text,
                      const char* focus,
                      const char* context,
                      bool case_insensitive)
{

    bool bFocus = false,
        bContext = false;

    while(*text)
    {

        if(isSentenceDelim(*text))
        {
            bFocus = false;
            bContext = false;
        }

        if(!bFocus && ptr_at_string(text, focus, case_insensitive))
        {
            bFocus = true;
            if(bContext) return true;
            text += strlen(focus);
        }
        else if(!bContext && ptr_at_string(text, context, case_insensitive))
        {
            bContext = true;
            if(bFocus) return true;
            text += strlen(context);
        }
        else ++text;
    }

    return false;
}

bool sentence_context(const char* text,
                      const std::vector<std::string>& terms,
                      bool case_insensitive,
                      bool ordered)
{

    size_t i = 0;

    if(ordered)
    {

        while(*text)
        {

            if(ptr_at_string(text, terms[i].c_str(), case_insensitive))
            {
                ++i;

                if(i == terms.size())
                {
                    return true;
                }
            }

            ++text;
        }

    }
    else
    {

        std::vector<bool> matches(terms.size(), false);

        while(*text)
        {

            for(i = 0; i < terms.size(); ++i)
            {
                if(!matches[i] && ptr_at_string(text, terms[i].c_str(), case_insensitive))
                {
                    matches[i] = true;

                    for(auto flag : matches)
                    {
                        if(!flag)
                        {
                            goto next;
                        }
                    }

                    return true;

                    next:;

                    text += strlen(terms[i].c_str());
                }
            }

            ++text;

        }

    }

    return false;

}

}
