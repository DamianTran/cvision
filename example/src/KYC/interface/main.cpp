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

#include <string>
#include <vector>
#include <map>

#include <ctime>

#include "EZC/toolkit/console.hpp"

#include <boost/filesystem.hpp>

#include "KYC/interface/app.hpp"

using namespace EZC;
using namespace KYC;

namespace fs = boost::filesystem;

int main(int argc, char** argv)
{

    fs::path exec_dir(argv[0]);

    arg_map args;
    get_args(argc, argv, args);

    srand(time(NULL));

    KYCApp app;

    return 0;

}
