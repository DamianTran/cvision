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

#include "EZC/network/webio.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sstream>

#include <curl/curl.h>

struct text_stream
{
    size_t size;
    char* data;

    text_stream():
        size(0),
        data(new char[1])
    {
        data[1] = 0;
    }

    ~text_stream()
    {
        delete[] data;
    }
};

size_t net_write_data(void* buf, size_t size, size_t nmemb, void* userp)
{

    size_t tSIZE = size*nmemb;

    if(!tSIZE)
    {
        return 0;
    }

    text_stream* stream = (text_stream*)userp;

    char* realloc_stream = new char[stream->size + tSIZE + 1];
    memcpy(realloc_stream, stream->data, stream->size);
    delete[] stream->data;
    stream->data = realloc_stream;

    if(!stream->data)
    {
        std::cout << "Error in net write: out of memory\n";
        return 0;
    }

    memcpy(&stream->data[stream->size], buf, tSIZE);

    stream->size += tSIZE;
    stream->data[stream->size] = 0;

    return tSIZE;
}

bool getNetworkData(std::ostream* output,
                    const std::string& url)
{

    CURL* curl = curl_easy_init();
    CURLcode res;

    text_stream data;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &net_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
        std::cout << "Failed to obtain network data: " << curl_easy_strerror(res) << '\n';
        return false;
    }

    *output << data.data;

    curl_easy_cleanup(curl);

    return true;

}

bool downloadFile(const std::string& URL,
                  const std::string& filename)
{

    std::stringstream ss;

    if(getNetworkData(&ss, URL))
    {
        FILE* outFILE = fopen(filename.c_str(), "wb");

        if(!outFILE)
        {
            std::cout << "Error: failed to write to hard drive\n";
            return false;
        }

        fwrite(ss.str().c_str(), sizeof(char), ss.str().size(), outFILE);
        fclose(outFILE);

        return true;
    }

    std::cout << "Error: download failed\n";

    return false;

}
