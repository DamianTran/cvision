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

#include "EZC/toolkit/docx.hpp"
#include "EZC/toolkit/tree.hpp"
#include "EZC/toolkit/html.hpp"
#include "EZC/toolkit/charsurf.hpp"
#include "EZC/toolkit/string.hpp"

#include "AIDA/AIDA_filesystem.hpp"
#include "EZC/algorithm.hpp"

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/foreach.hpp"

#include "zlib.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>
#include <sstream>

#include <zip.h>

using namespace boost::filesystem;
namespace pt = boost::property_tree;
using namespace EZC;

bool getDocumentBody(std::string& output, const std::string& filename)
{

    if(access(filename.c_str(), F_OK)) return false;

    size_t initSIZE = output.size();

    std::string ext_type = path(filename).extension().string();
    if(ext_type == ".docx")
    {

        int err, len, fd, pos = 0, readSize;

        struct zip_file* zf;
        struct zip_stat sb;
        zip_t* doc_archive = zip_open(filename.c_str(), 0, &err);
        if(doc_archive == nullptr) return false;

        std::stringstream ss;
        char buf[ZIP_READ_BUF_SIZE*2];

        size_t N = zip_get_num_entries(doc_archive, 0);
        for(size_t i = 0; i < N; ++i)
        {
            if(zip_stat_index(doc_archive, i, 0, &sb) == 0)
            {
                path sub_file(sb.name);
                if(sub_file.filename().string() == "document.xml")  // Find the document body
                {
                    len = strlen(sb.name);
                    zf = zip_fopen_index(doc_archive, i, 0);

#if defined __WIN32 || defined _WIN32 || defined WIN32
                    fd = open(sub_file.string().c_str(), O_RDWR | O_TRUNC | O_CREAT | O_BINARY, 0644);
#else
                    fd = open(sub_file.string().c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);
#endif

                    while(pos < sb.size)
                    {
                        readSize = pos + ZIP_READ_BUF_SIZE < sb.size ?
                                   ZIP_READ_BUF_SIZE : sb.size - pos;
                        if((len = zip_fread(zf, buf, readSize)) < 0)
                        {
                            break;
                        }

                        write(fd, buf, len);
                        buf[len] = '\0';
                        ss << buf;

                        pos += readSize;
                    }

                    close(fd);
                    zip_fclose(zf);
                    zip_close(doc_archive);

                    break;
                }
            }
        }

        if(ss.str().empty()) return false;

        pt::ptree tree;

        try
        {
            pt::read_xml(ss, tree);
        }
        catch(...)
        {
            throw std::invalid_argument("Input file not a word document or corrupted");
        }

        ss.str("");
        std::function<void(const std::pair<std::string, pt::ptree>&)> recurse_branch;
        recurse_branch = [&](const std::pair<std::string, pt::ptree>& node)
        {

            if(node.first == "w:t")
            {
//                if(!ss.str().empty() &&
//                   !isSpecial(node.second.get("", "").front()) &&
//                   !isSpecial(ss.str().back())) ss << ' ';
                ss << node.second.get("", "");
            }
            else if(node.first == "w:p")
            {
                if(!ss.str().empty()) ss << '\n';
            }
            for(auto& sub_node : node.second)
            {
                if(!node.second.empty())
                {
                    recurse_branch(sub_node);
                }
            }
        };

        for(auto& node : tree)
        {
            recurse_branch(node);
        }

        output = ss.str();

        // Post-process

        for(size_t i = 0; i < output.size();)
        {

            if((output[i] == -30) && (output[i + 1] == -128))
            {
                if(output[i + 2] == -100)
                {
                    output.erase(output.begin() + i, output.begin() + i + 2);
                    output[i] = '\"';
                }
                else if(output[i + 2] == -99)
                {
                    output.erase(output.begin() + i, output.begin() + i + 2);
                    output[i] = '\"';
                }
                else if(output[i + 2] == -103)
                {
                    output.erase(output.begin() + i, output.begin() + i + 2);
                    output[i] = '\'';
                }
                else if(output[i + 2] == -90)
                {
                    output[i] = output[i + 1] = output[i + 2] = '.';
                }
                else ++i;
            }
            else if((output[i] == -62) && (output[i + 1] == -96))
            {
                output.erase(output.begin() + i);
                output[i] = ' ';
            }
            else if(!isTypeChar(output[i]))
            {

                output.replace(output.begin() + i, output.begin() + i + 1,
                               std::to_string((int)output[i]));

            }
            else ++i;

        }

    }
    else if(ext_type == ".doc")
    {

        size_t L, i = 0;
        MSW_HEADER WHDR;

        FILE* inFILE = fopen(filename.c_str(), "rb");

        fseek(inFILE, 0, SEEK_END);
        L = ftell(inFILE);
        fseek(inFILE, 0, SEEK_SET);

        char * text = new char[L + 1];
        text[L] = '\0';

        fread(text, L, sizeof(char), inFILE);

        fclose(inFILE);

        // Slide to find reading frame

        for(; i < L; ++i)
        {

            if(!WHDR.valid())
            {
                WHDR.read(&text[i]);
            }
            else
            {
                break;
            }

        }

        output.assign(text + i + WHDR.size(),
                      text + i + WHDR.size() + WHDR.cbMac());
        delete[] (text);

        unsigned int beginIdx = 0, endIdx = 0;
        bool bHyperLink = false;

        for(i = 0; i < output.size();)
        {

            if((output[i] == '\r') || (output[i] == -105))
            {
                output[i] = '\n';
                ++i;
            }
            else if(output[i] == '\0')
            {
                output.erase(output.begin() + i);
            }
            else if(output[i] == 21)
            {
                if(output[i + 1] == 7)
                {
                    output[i] = '\t';
                    output[i + 1] = '\t';
                    i += 2;
                }
                else
                {
                    output[i] = '\n';
                    ++i;
                }
            }
            else if((output[i] == 7) && (output[i + 1] == 7))
            {
                output.erase(output.begin() + i, output.begin() + i + 1);
                output[i] = '\n';
                ++i;
            }
            else if((output[i] == -109) || (output[i] == -108))
            {
                output[i] = '\"';
                ++i;
            }
            else if(output[i] == -110)
            {
                output[i] = '\'';
                ++i;
            }
            else if((output[i] == -96) || (output[i] == 11))
            {
                output[i] = '\t';
                ++i;
            }
            else if(ptr_at_string(&output[i], "HYPERLINK") &&
                    ((output[i - 2] == 19) || (output[i-1] == 19)))
            {

                bHyperLink = false;
                beginIdx = i-2;
                endIdx = i;

                while(endIdx < output.size() - 1)
                {
                    if((int)output[endIdx + 1] == 20)
                    {
                        bHyperLink = true;
                        break;
                    }
                    ++endIdx;
                }

                if(bHyperLink)
                {
                    output.erase(output.begin() + beginIdx, output.begin() + endIdx + 2);
                    i = beginIdx;
                }
                else
                {
                    ++i;
                }
            }
            else if(output[i] == 3)
            {
                output.erase(output.begin() + i, output.end());
            }
            else
            {
                ++i;
            }

        }

    }
    else if(ext_type == ".pdf")
    {

        unsigned long L, S = output.size(), fileSIZE, outSIZE;
        int err;

        cMap encodeMap; // For cMap-encoded PDFs

        FILE* inFILE = fopen(filename.c_str(), "rb");

        fseek(inFILE, 0, SEEK_END);
        fileSIZE = L = ftell(inFILE);
        fseek(inFILE, 0, SEEK_SET);

        unsigned char* bin_data = new unsigned char[L];

        fread(bin_data, L, sizeof(unsigned char), inFILE);

        fclose(inFILE);

        std::string str_begin = "stream",
                    str_end = "endstream";
        unsigned int matchIndex = 0,
                     beginIdx = UINT_MAX,
                     endIdx = UINT_MAX, j, k;
        bool bBracket = false;

        for(size_t i = 0; i < fileSIZE; ++i)
        {
            if(beginIdx == UINT_MAX)
            {
                if(bin_data[i] == str_begin[matchIndex])
                {
                    ++matchIndex;
                }
                else matchIndex = 0;

                if(matchIndex == str_begin.size())
                {
                    beginIdx = i+1;
                    matchIndex = 0;
                }
            }
            else if(beginIdx != UINT_MAX)
            {
                if(bin_data[i] == str_end[matchIndex])
                {
                    ++matchIndex;
                }
                else matchIndex = 0;

                if(matchIndex == str_end.size())  // Read compressed stream data
                {

                    while(isCharType(bin_data[beginIdx], "\n\r"))
                    {
                        ++beginIdx;
                    }
                    endIdx = i - str_end.size();
                    while(isCharType(bin_data[endIdx - 1], "\n\r"))
                    {
                        --endIdx;
                    }

                    L = endIdx - beginIdx;
                    unsigned char* stream_data = new unsigned char[L];
                    memcpy(stream_data, bin_data + beginIdx, L);

                    outSIZE = L*10;

                    unsigned char* read_buffer = new unsigned char[outSIZE + 1];
                    read_buffer[outSIZE] = '\0';

                    if(err = uncompress(read_buffer, &outSIZE, stream_data, L))
                    {
                        beginIdx = UINT_MAX;
                        matchIndex = 0;

                        delete[] (stream_data);
                        delete[] (read_buffer);

                        continue;
                    }

                    read_buffer[outSIZE] = '\0';

                    if(!getCMap(encodeMap, (const char*)read_buffer))
                    {

                        output.reserve(output.size() + outSIZE/2);

                        bBracket = false;

                        for(j = 0; j < outSIZE; ++j)
                        {
                            if(!bBracket)
                            {
                                if(read_buffer[j] == '\0') break;
                                if((read_buffer[j] == '(') &&
                                        (!j || !(read_buffer[j - 1] == '\\')))
                                {
                                    bBracket = true;
                                    beginIdx = j + 1;
                                }
                            }
                            else if(bBracket)
                            {
                                if((read_buffer[j] == ')') &&
                                        (!j || !(read_buffer[j - 1] == '\\')))
                                {
                                    bBracket = false;

                                    // Add spacing if applicable
                                    if(read_buffer[beginIdx-2] == '[')
                                    {
                                        output += ' ';
                                    }
                                    else if((beginIdx > 1) && isNumber(read_buffer[beginIdx - 2]))
                                    {
                                        unsigned int p = beginIdx - 2;
                                        char* end = (char*)read_buffer + beginIdx - 1;
                                        while((p > 0) &&
                                                ((isNumber(read_buffer[p]) ||
                                                  (read_buffer[p] == '.') ||
                                                  read_buffer[p] == '-')))
                                        {
                                            --p;
                                        }

                                        try
                                        {
                                            float spacing = strtof((char*)read_buffer + p + 1, &end);
                                            if(abs(spacing) > 30.0f) output += ' ';
                                        }
                                        catch(...) { }
                                    }

                                    bool utf16 = false;
                                    unsigned int init_k = beginIdx;

                                    for(k = beginIdx; k < j; ++k)
                                    {
                                        if(read_buffer[k] == '\0')
                                        {
                                            continue;
                                        }
                                        else if((k < beginIdx - 1) && (read_buffer[k] == '\\') &&
                                                ((read_buffer[k + 1] == '(') || (read_buffer[k + 1] == ')'))) continue;
                                        else
                                        {
                                            output += char(read_buffer[k]);
                                            ++S;
                                        }
                                    }
                                }
                            }
                        }

                    }

                    delete[] (stream_data);
                    delete[] (read_buffer);

                    beginIdx = UINT_MAX;
                    matchIndex = 0;
                }
            }
        }

        // Decrypt if necessary

        if(!encodeMap.empty())
        {
            auto it_end = encodeMap.end();
            for(size_t i = 0; i < output.size(); ++i)
            {

                for(auto it = encodeMap.begin(); it != it_end; ++it)
                {
                    wchar_t cast = output[i];
                    if(it->first == cast)
                    {
                        cast = it->second;
                    }
//                    output.erase(output.begin() + i);
                    output[i] = cast;
                }

            }

        }

        delete[] (bin_data);

    }
    else if(anyEqual(ext_type, StringVector({ ".htm", ".html" })))
    {

        StringVector html_text;
        HTML_tree html_tree;
        html_tree.read_file(filename);

        html_tree.getTreeText(html_text);

        output = concatenateString(html_text, " ");

    }
    else if(ext_type == ".txt")
    {
        size_t L;
        FILE* inFILE = fopen(filename.c_str(), "rb");
        fseek(inFILE, 0, SEEK_END);
        L = ftell(inFILE);
        fseek(inFILE, 0, SEEK_SET);

        char * text = new char[L + 1];
        text[L] = '\0';

        fread(text, L, sizeof(char), inFILE);

        fclose(inFILE);

        output.assign(text, L + 1);

        delete[] (text);

    }

    return output.size() > initSIZE;

}

bool checkCMap(const char* str)
{
    return checkString("/CIDInit", str);
}

bool getCMap(cMap& output, const char* str)
{

    if(!checkCMap(str)) return false;

    size_t beginCodex = findString("beginbfrange", str),
           endCodex = findString("endbfrange", str),
           initSize = output.size(),
           beginIdx = UINT_MAX;

    if((beginCodex == UINT_MAX) || (endCodex == UINT_MAX)) return false;

    bool bBracket = false, bBeginLine = true;
    std::string encodeBuf;
    wchar_t mapC;

    for(size_t i = beginCodex + 12; i < endCodex; ++i)
    {
        if(!bBracket)
        {
            if(str[i] == '<')
            {
                bBracket = true;
                beginIdx = i + 1;
            }
            else if(str[i] == '\n')
            {
                bBeginLine = true;
            }
        }
        else
        {
            if(str[i] == '>')
            {
                bBracket = false;

                if(encodeBuf.size() == 4)
                {
                    wchar_t c = wchar_t(hexToInt(encodeBuf.c_str()));
                    if(bBeginLine) mapC = c;
                    else
                    {
                        output[mapC] = c;
                    }
                }

                encodeBuf.clear();
                bBeginLine = false;
            }
            else encodeBuf += str[i];
        }
    }

    return output.size() > initSize;

}
