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

#include "KYC/network/user.hpp"

#include "EZC/algorithm.hpp"
#include "EZC/toolkit/string.hpp"

#include <stdio.h>

using namespace EZC;

namespace KYC
{

client::client(const std::string& first,
               const std::string& last,
               const std::string& username,
               const std::string& pass,
               const std::string& title,
               const std::string& email,
               const Gender& gender):
                   first(first),
                   last(last),
                   username(username),
                   pass(pass),
                   title(title),
                   email(email),
                   gender(gender)
{

}

const std::string& client::first_name() const
{
    return first;
}

const std::string& client::last_name() const
{
    return last;
}

const std::string& client::user_name() const
{
    return username;
}

const std::string& client::password() const
{
    return pass;
}

const std::string& client::user_title() const
{
    return title;
}

const std::string& client::email_address() const
{
    return email;
}

const Gender& client::user_gender() const
{
    return gender;
}

void client::set_first_name(const std::string& newValue)
{
    first = newValue;
}

void client::set_last_name(const std::string& newValue)
{
    last = newValue;
}

void client::set_user_name(const std::string& newValue)
{
    username = newValue;
}

void client::set_password(const std::string& newValue)
{
    pass = newValue;
}

void client::set_user_title(const std::string& newValue)
{
    title = newValue;
}

void client::set_email_address(const std::string& newValue)
{
    email = newValue;
}

void client::set_gender(const Gender& newGender)
{
    gender = newGender;
}

void fread(client& load_info, FILE* inFILE)
{
    readString(load_info.first, inFILE);
    readString(load_info.last, inFILE);
    readString(load_info.username, inFILE);
    readString(load_info.pass, inFILE);
    readString(load_info.title, inFILE);
    fread(&load_info.gender, 1, sizeof(load_info.gender), inFILE);
}

void fwrite(const client& save_info, FILE* outFILE)
{
    writeString(save_info.first, outFILE);
    writeString(save_info.last, outFILE);
    writeString(save_info.username, outFILE);
    writeString(save_info.pass, outFILE);
    writeString(save_info.title, outFILE);
    fwrite(&save_info.gender, 1, sizeof(save_info.gender), outFILE);
}

client_base::client_base(const std::string& filename)
{
    load(filename);
}

client client_base::getUser(const std::string& userName)
{

    for(auto& user : *this)
    {
        if(user.user_name() == userName)
        {
            return user;
        }
    }

    return client();

}

bool client_base::save(const std::string& filename) const
{

    FILE* outFILE;

    try
    {
        outFILE = fopen(filename.c_str(), "wb");

        if(outFILE)
        {
            size_t L = size();
            fwrite(&L, 1, sizeof(L), outFILE);
            for(auto& user : *this)
            {
                fwrite(user, outFILE);
            }

            return true;
        }
    }catch(...){ }

    return false;

}

bool client_base::load(const std::string& filename)
{
    FILE* inFILE;

    try
    {
        inFILE = fopen(filename.c_str(), "rb");
        if(inFILE)
        {
            size_t L;
            fread(&L, 1, sizeof(L), inFILE);
            for(size_t i = 0; i < L; ++i)
            {
                emplace_back();
                fread(back(), inFILE);
            }

            return true;
        }
    }catch(...){ }

    return false;
}

bool client_base::checkLogin(const std::string& username,
                             const std::string& password) const
{

    for(auto& user : *this)
    {
        if(user.user_name() == username)
        {
            if(user.password() == password)
            {
                return true;
            }
        }
    }

    return false;

}

bool client_base::check_last_name(const client& newClient) const
{

    for(auto& user : *this)
    {
        if(user.last_name() == newClient.last_name())
        {
            return true;
        }
    }

    return false;
}

bool client_base::check_user_name(const client& newClient) const
{

    for(auto& user : *this)
    {
        if(user.user_name() == newClient.user_name())
        {
            return true;
        }
    }

    return false;
}

bool client_base::check_first_name(const client& newClient) const
{

    for(auto& user : *this)
    {
        if(user.first_name() == newClient.first_name())
        {
            return true;
        }
    }

    return false;
}

bool client_base::check_email(const client& newClient) const
{

    for(auto& user : *this)
    {
        if(user.email_address() == newClient.email_address())
        {
            return true;
        }
    }

    return false;
}

bool is_email(const std::string& str)
{
    unsigned int syn_count = 0;
    for(auto& c : str)
    {
        if(!isLetter(c) && !isNumber(c) && !isCharType(c, "._@"))
        {
            return false;
        }
        if(c == '@')
        {
            ++syn_count;
        }
    }
    if(!syn_count || syn_count > 1)
    {
        return false;
    }

    unsigned int syn_split = str.find("@"),
                dom_index = 0;

    if(syn_split < 2)
    {
        return false;
    }

    syn_count = 0;

    for(size_t i = syn_split + 1; i < str.size(); ++i)
    {
        if(str[i] == '.')
        {
            ++syn_count;
            if(syn_count > 1)
            {
                return false;
            }
            dom_index = i;
        }
    }

    return (str.size() - dom_index + 1) > 1;

}

float check_pass_strength(const std::string& str)
{

    float output = 0.0f;

    if(cmpStringToList(str, {
                                "password",
                                "pass",
                                "1234"
                            }))
    {
        output -= 0.5f;
    }

    std::string unique_chars;
    unsigned int special_count  = 0,
                    num_count   = 0;

    bool        bLowerCase      = false,
                bUpperCase      = false;

    for(auto& c : str)
    {
        if(isSpecial(c))
        {
            ++special_count;
        }
        else if(!bUpperCase && isUpperCase(c))
        {
            bUpperCase = true;
        }
        else if(!bLowerCase && isLowerCase(c))
        {
            bLowerCase = true;
        }
        else if(isNumber(c))
        {
            ++num_count;
        }
        else
        {
            for(auto& oc : unique_chars)
            {
                if(oc == c)
                {
                    goto nonUnique;
                }
            }

            unique_chars.push_back(c);

            nonUnique:;
        }
    }

    output = 0.8f*(float)str.size()/unique_chars.size();
    if(num_count) output += 0.15f;
    if(bLowerCase && bUpperCase) output += 0.15f;
    if(special_count) output += 0.2f;

    if(output > 1.0f) output = 1.0f;
    else if(output < 0.0f) output = 0.0f;

    return output;

}

}
