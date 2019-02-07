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

#include "EZC/toolkit/filesystem.hpp"
#include "AIDA/verbal/verbal_matrix.hpp"

#if __has_include("AIDA/dataStream.hpp")
#include "AIDA/dataStream.hpp"
#endif

#include <stdio.h>
#include <unistd.h>

#include "EZC/algorithm.hpp"

#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif

using namespace EZC;

bool getPathListStrings(const PathList& input, std::vector<std::string>& output)
{
    output.reserve(output.size() + input.size());
    const c_itr<boost::filesystem::path> itEND = input.end();
    for(c_itr<boost::filesystem::path> it = input.begin(); it != itEND; ++it)
    {
        output.push_back(it->string());
    }
    return output.size() > 0;
}

bool getPathListNames(const PathList& input, std::vector<std::string>& output)
{
    output.reserve(output.size() + input.size());
    const c_itr<boost::filesystem::path> itEND = input.end();
    for(c_itr<boost::filesystem::path> it = input.begin(); it != itEND; ++it)
    {
        output.push_back(it->stem().string());
    }
    return output.size() > 0;
}

void getFilesInDir(PathList& output,
                   const boost::filesystem::path& directory,
                   const std::string& extension)
{
    if(!DIR_EXISTS(directory)) return;

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
            itr != end_itr; ++itr)
    {
        if(boost::filesystem::is_directory(itr->status()))
        {
            getFilesInDir(output, itr->path(), extension);
        }
        else if((extension.size() > 0) &&
                (itr->path().extension().string().find(extension) <
                 itr->path().extension().string().size()))
            output.push_back(itr->path());
        else if(extension.size() < 1) output.push_back(itr->path());
    }
}

std::string getBestPathMatch(const std::string& query, const boost::filesystem::path& directory,
                             const float& threshold)
{

    if(!DIR_EXISTS(directory)) return std::string();
    if(DIR_EXISTS(query)) return query;

    StringVector candidates;
    DIR_ITERATOR end_itr;
    for(DIR_ITERATOR itr(directory); itr != end_itr; ++itr)
    {
        if(cmpString(itr->path().string(), query)) candidates.push_back(itr->path().string());
        if(boost::filesystem::is_directory(itr->status()))
        {
            std::string newCandidate = getBestPathMatch(query, itr->path(), threshold);
            if(!newCandidate.empty()) candidates.push_back(newCandidate);
        }
    }

    return getBestStringMatch(query, candidates, threshold);
}

unsigned int dirFileCount(const std::string& directory)
{
    if(!DIR_EXISTS(directory)) return 0;
    DIR_ITERATOR end_itr;
    unsigned int count = 0;
    for(DIR_ITERATOR itr(directory); itr != end_itr; ++itr)
    {
        ++count;
    }
    return count;
}

bool searchForDir(const std::string& query, const boost::filesystem::path& directory,
                  PathList& output)
{

    if(!boost::filesystem::exists(directory) ||
            !boost::filesystem::is_directory(directory)) return false;

    bool check = false;

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
            itr != end_itr; ++itr)
    {
        try
        {
            if(boost::filesystem::is_directory(itr->status()))
            {
                if(cmpString(itr->path().string(), query))
                {

                    bool dup = false;
                    for(auto& out : output)
                    {
                        if(cmpString(itr->path().string(), out.string()))
                        {
                            dup = true;
                            break;
                        }
                    }
                    if(!dup)
                    {
                        output.emplace_back(itr->path());
                        check = true;
                    }
                }
                check |= searchDir(query, itr->path(), output);
            }
        }
        catch(...)
        {
            continue;
        }
    }

    return check;
}

size_t numFilesInDir(const std::string& directory)
{

    if(!DIR_EXISTS(directory))
    {
        return 0;
    }

    size_t output = 0;

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
        itr != end_itr; ++itr)
    {
        ++output;
    }

    return output;
}

bool directory_empty(const std::string& directory)
{
    if(!DIR_EXISTS(directory))
    {
        return true;
    }

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
        itr != end_itr; ++itr)
    {
        return false;
    }

    return true;
}

bool searchForDir(const std::string& query, const PathList& directories,
                  PathList& output)
{

    bool check = false;

    for(auto& directory : directories)
    {

        if(!boost::filesystem::exists(directory) ||
                !boost::filesystem::is_directory(directory)) return false;

        boost::filesystem::directory_iterator end_itr;
        for(boost::filesystem::directory_iterator itr(directory);
                itr != end_itr; ++itr)
        {
            try
            {
                if(cmpString(itr->path().string(), query))
                {
                    output.emplace_back(itr->path());
                    check = true;
                }
                if(boost::filesystem::is_directory(itr->status()))
                {
                    check |= searchDir(query, itr->path(), output);
                }
            }
            catch(...)
            {
                continue;
            }
        }

    }

    return check;
}

bool searchDir(const std::string& query, const boost::filesystem::path& directory,
               PathList& output)
{

    bool check = false;

    if(!boost::filesystem::exists(directory) ||
            !boost::filesystem::is_directory(directory)) return false;

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
            itr != end_itr; ++itr)
    {
        try
        {
            if(cmpString(itr->path().string(), query))
            {
                output.emplace_back(itr->path());
                check = true;
            }
            if(boost::filesystem::is_directory(itr->status()))
            {
                check |= searchDir(query, itr->path(), output);
            }
        }
        catch(...)
        {
            continue;
        }
    }

    return check;
}

bool searchDir(const std::string& query, const PathList& directories,
               PathList& output)
{

    bool check = false;

    for(auto& directory : directories)
    {
        if(!boost::filesystem::exists(directory) ||
                !boost::filesystem::is_directory(directory)) return false;

        boost::filesystem::directory_iterator end_itr;
        for(boost::filesystem::directory_iterator itr(directory);
                itr != end_itr; ++itr)
        {
            try
            {
                if(cmpString(itr->path().string(), query))
                {
                    output.emplace_back(itr->path());
                    check = true;
                }
                if(boost::filesystem::is_directory(itr->status()))
                {
                    check |= searchDir(query, itr->path(), output);
                }
            }
            catch(...)
            {
                continue;
            }
        }

    }

    return check;
}

bool fileExists(const std::string& query, const std::string& directory)
{
    if(!DIR_EXISTS(directory)) return false;

    boost::filesystem::directory_iterator end_itr;
    for(boost::filesystem::directory_iterator itr(directory);
            itr != end_itr; ++itr)
    {
        if(itr->path().stem().string() == query) return true;
    }
    return false;
}

#ifdef DATASTREAM_HPP

bool assembleVM(const std::string& directory, const std::string& rootName,
                const bool& cleanUp, const bool& verbose)
{

    if(!DIR_EXISTS(directory)) return false;
    PathList files;
    getFilesInDir(files, directory, "");
    for(size_t i = 0; i < files.size();)
    {
        if(!cmpString(files[i].stem().string(), rootName)) files.erase(files.begin() + i);
        else if(files[i].stem().string() == rootName) files.erase(files.begin() + i);
        else ++i;
    }

    if(files.size() < 1) return false;

    if(verbose) std::cout << ">> Assembling fragments from \"" << directory << "\"\n"
                              << ">> Detected " << files.size() << " fragments with base \"" << rootName << "\"\n";

    VerbalMatrix queryBuffer;
    StringVector wordBuff, queries;
    unsigned int index = 0, j = 0, querySIZE = 0;

    if(verbose) std::cout << ">> Assembling word repository: 0%";
    for(auto& file : files)
    {
        _2Dstream fragment(file.string(), false);
        fragment.getRow(0).import(queryBuffer.words);
        queryBuffer.words.erase(queryBuffer.words.begin() + j);

        bool nextFlag;
        for(; j < queryBuffer.words.size();)
        {
            nextFlag = true;
            for(size_t i = 0; i < j; ++i)
            {
                if(queryBuffer.words[i] == queryBuffer.words[j])
                {
                    queryBuffer.words.erase(queryBuffer.words.begin() + j);
                    nextFlag = false;
                    break;
                }
            }
            if(nextFlag) ++j;
        }
        j = queryBuffer.words.size();
        ++index;
        querySIZE += fragment.nrow()-1;
        if(verbose) std::cout << "\r>> Assembling word repository: " << index*100/files.size() << "%    ";
    }

    if(verbose)
    {
        std::cout << "\n>> Word repository contains " << queryBuffer.words.size() << " words\n";
        std::cout << ">> Detected " << querySIZE << " queries\n";
    }

    index = 0;
    j = 0;

    std::ofstream outFILE(directory + '\\' + rootName + ".tsv", std::ios::out | std::ios::binary);
    outFILE << queryBuffer;
    std::vector<unsigned int> mapIndex;

    std::thread readout([&]()
    {
        std::chrono::duration<float> updateLatency(1.0f);
        while(index < files.size())
        {
            if(verbose) std::cout << "\r>> Assembling: " << files[index].string() << " (" << strDigits((float)j*100/querySIZE, 2) << "%)    ";
            std::this_thread::sleep_for(updateLatency);
        }
    });

    for(auto& file : files)
    {
        _2Dstream fragment(file.string(), false);

        fragment.getRow(0).import(wordBuff);
        wordBuff.erase(wordBuff.begin());
        mapIndex.reserve(wordBuff.size());
        for(auto& word : wordBuff)
        {
            mapIndex.push_back(match(word, queryBuffer.words));
        }

        unsigned int xL = fragment.ncol(),
                     yL = fragment.nrow();

        if((xL < 1) || (yL < 2)) continue;

        for(size_t y = 1; y < yL; ++y)
        {
            std::string query = fragment.getString(0, y);
            queryBuffer.addQuery(query);
            for(size_t x = 1; x < xL; ++x)
            {
                queryBuffer.frequencies[0][mapIndex[x-1]] += fragment.getFloat(x, y);
            }
            queryBuffer.streamQueryLine(outFILE, query);
            queryBuffer.removeQuery(query);
            ++j;
        }
        ++index;

        wordBuff.clear();
        mapIndex.clear();
    }

    if(cleanUp)
    {
        for(auto& file : files)
        {
            remove(file.string().c_str());
        }
        if(verbose) std::cout << ">> Removing fragment files...\n";
    }

    readout.join();

    outFILE.close();
    if(verbose) std::cout << ">> Assembly into \"" << directory << '\\' << rootName << "\" complete\n";

    return true;
}

#endif

boost::filesystem::path getBranchDirectory(const PathList& paths)
{

    boost::filesystem::path tmp, output;

    if(paths.empty()) return output;

    output = paths.front();
    if(paths.size() == 1) return output.parent_path();

    for(size_t i = 1; i < paths.size(); ++i)
    {

        while(!output.empty())
        {

            if(!boost::filesystem::is_directory(paths[i]))
            {
                tmp = paths[i].parent_path();
            }
            else
            {
                tmp = paths[i];
            }

            while(!tmp.empty())
            {
                if(output.stem().string() == tmp.stem().string())
                {
                    goto nextLoop;
                }
                else
                {
                    tmp = tmp.parent_path();
                }
            }

            if(tmp.empty())
            {
                output = output.parent_path();
            }
        }

        break;

        nextLoop:;

    }

    return output;

}

void activateFile(const std::string& filename)
{
#if defined WIN32 || defined _WIN32
    ShellExecute(NULL, NULL, filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    system(std::string("open ").append(filename).c_str());
#endif
}

size_t getFileSize(const std::string& filename)
{
    try{
        FILE* inFILE = fopen(filename.c_str(), "rb");

        if(!inFILE)
        {
            return 0;
        }

        return getFileSize(inFILE);
    }catch(...)
    {
        return 0;
    }
}

size_t getFileSize(FILE* inFILE)
{

    if(!inFILE)
    {
        return -1;
    }

    fpos_t initPos = ftell(inFILE);

    fseek(inFILE, 0, SEEK_END);
    size_t output = ftell(inFILE);
    fsetpos(inFILE, &initPos);

    return output;

}

size_t getFileData(const std::string& filename,
                void** output)
{
    try
    {
        FILE* inFILE = fopen(filename.c_str(), "rb");

        if(!inFILE)
        {
            return 0;
        }

        size_t L = getFileSize(inFILE);

        *output = new unsigned char[L];

        fread(*output, sizeof(unsigned char), L, inFILE);

        fclose(inFILE);

        return L;
    }catch(...)
    {
        *output = NULL;
    }

    return 0;
}



bool saveFileData(void* input,
                 const size_t& inputSIZE,
                 const std::string& filename)
{
    if(!input || !inputSIZE)
    {
        return false;
    }

    FILE* outFILE = fopen(filename.c_str(), "wb");

    if(!outFILE)
    {
        return false;
    }

    fwrite(input, sizeof(unsigned char), inputSIZE, outFILE);

    fclose(outFILE);

    return true;
}

unsigned int getMatchingIndex(const boost::filesystem::path& focus,
                              const std::vector<boost::filesystem::path>& list,
                              const BYTE& params)
{
    unsigned int matchIndex = 0;
    std::vector<std::string> matches;
    std::vector<unsigned int> matchIndices;
    for(auto& item : list)
    {
        if(focus == item) return matchIndex;
        if(cmpString(focus.string(), item.string(), params))
        {
            matches.push_back(item.string());
            matchIndices.push_back(matchIndex);
        }
        ++matchIndex;
    }
    if(matches.size() > 1) return matchIndices[getBestStringMatchIndex(focus.string(), matches)];
    else if(matches.size() == 1) return matchIndices.front();
    else return UINT_MAX;
}
