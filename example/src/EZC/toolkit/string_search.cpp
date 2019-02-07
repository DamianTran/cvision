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
