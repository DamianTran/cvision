/**================================================================

>	Keystone ID Source Code
>	Copyright (C) 2019 Keystone ID, Incorporated

>	DESCRIPTION:

>	Keystone ID is a web-searching service for enhanced
>	due diligence on individuals with web, political, or
>	government presence.  Keystone ID makes use of several
>	database APIs including Twitter, Facebook, and the
>	News API to help clients verify the credentials of, or 
>	discover affiliations to, target individuals.

>	LEGAL:

>	The source code herein is PROPRIETARY and its
>	use, modification, and distribution are to be
>	performed solely with the permission of the
>	owning Company ("Keystone ID, Incorporated").
>	Use, modification, and/or distribution of the
>	enclosed material is ILLEGAL and parties involved
>	in such acts are liable for damages incurred as
>	a result of such acts to the owning Company.

================================================================**/

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
