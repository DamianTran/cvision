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

#include "EZC/toolkit/console.hpp"

namespace EZC
{

void clear_console(char fill)
{

    #if defined WIN32 || defined _WIN32 || defined __WIN32

    COORD tl({0,0});
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(console, &bufferInfo);

    DWORD written, cells = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;

    FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    FillConsoleOutputAttribute(console, bufferInfo.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);

    #endif

}

void get_args(const int& argc, char* argv[], std::map<std::string, std::string>& output)
{
    std::string newArg;

    for(int i = 1; i < argc; ++i)
    {

        if(argv[i][0] == '-')
        {

            if(!newArg.empty() && output[newArg].empty())
            {
                output[newArg] = "TRUE";
                newArg.assign(argv[i] + 1);

                if(i == (argc - 1))
                {
                    output[newArg] = "TRUE";
                }
            }
            else if(i == (argc - 1))
            {
                newArg.assign(argv[i] + 1);
                output[newArg] = "TRUE";
            }
            else
            {
                newArg.assign(argv[i] + 1);
            }

        }
        else
        {
            if(!newArg.empty())
            {
                if(!output[newArg].empty())
                {
                    output[newArg] += ';';
                }
                output[newArg].insert(output[newArg].size(), argv[i]);
            }
        }

    }
}

bool check_arg(const std::string& arg, const arg_map& args)
{
    try
    {
        args.at(arg);
        return true;
    }catch(...)
    {
        return false;
    }
}

}
