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

#pragma once

#ifndef KYC_USER
#define KYC_USER

#include <vector>
#include <string>

namespace KYC
{

enum class Gender
{
    unknown = 0,
    male = 1,
    female,
    other
};

class client
{
public:

    client(const std::string& first,
           const std::string& last,
           const std::string& username,
           const std::string& pass,
           const std::string& title,
           const std::string& email,
           const Gender& gender);

    client() = default;

    friend void fread(client& load_info, FILE* inFILE);
    friend void fwrite(const client& save_info, FILE* outFILE);

    const std::string& first_name() const;
    const std::string& last_name() const;
    const std::string& user_name() const;
    const std::string& password() const;
    const std::string& user_title() const;
    const std::string& email_address() const;

    const Gender& user_gender() const;

    void set_first_name(const std::string& newValue);
    void set_last_name(const std::string& newValue);
    void set_user_name(const std::string& newValue);
    void set_password(const std::string& newValue);
    void set_user_title(const std::string& newValue);
    void set_email_address(const std::string& newValue);

    void set_gender(const Gender& newGender);

protected:

    std::string first;
    std::string last;
    std::string username;
    std::string pass;
    std::string title;
    std::string email;

    Gender gender;



};

class client_base : public std::vector<client>
{
public:

    client_base() = default;
    client_base(const std::string& filename);

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;

    bool checkLogin(const std::string& username,
                    const std::string& password) const;

    bool check_first_name(const client& newClient) const;
    bool check_last_name(const client& newClient) const;
    bool check_user_name(const client& newClient) const;
    bool check_email(const client& newClient) const;

    client getUser(const std::string& userName);

};

bool is_email(const std::string& str);
float check_pass_strength(const std::string& str);

}

#endif // KYC_USER
