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

//
// AIALG - Simplifying C++
//
// Inspired for AI programming
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// Redistribution of the AIALG header library is permitted
// under accordance with  the GNU general public licence (GPL)
// version 3.0 and the terms and conditions specified in CVlicence.txt

#pragma once

#ifndef BIOALGORITHM_HPP
#define BIOALGORITHM_HPP

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <cmath>
#include <typeinfo>
#include <unistd.h>
#include <sstream>
#include <cstdarg>
#include <exception>
#include <random>
#include <array>
#include <map>

#if defined WIN32 || defined _WIN32
#include <thread>
#include <mutex>
#else
#include <thread>
#include <mutex>
#endif

#define ALPHABET        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGITS          "0123456789"
#define PUNCTUATION     ".,;\"'()!&\\/-?"
#define CODE_CHAR       "<>[]{}"
#define OPERATOR_CHAR   "-+/\\*"
#define HASH_CHAR       "@#$%"
#define MISC_CHAR       "^`"

#include <boost/math/distributions/students_t.hpp>
#include <boost/predef.h>

inline constexpr std::uint8_t operator"" _U8(unsigned long long uint){
    return static_cast<std::uint8_t>(uint);
}
inline constexpr std::uint8_t operator"" _BIT(unsigned long long uint){
    return static_cast<std::uint8_t>(uint);
}
inline constexpr unsigned char operator"" _BYTE(unsigned long long uint){
    return static_cast<unsigned char>(uint);
}

#define OBJ_CLASS_VOID              0_U8
#define OBJ_CLASS_INT_8             1_U8
#define OBJ_CLASS_INT_16            2_U8
#define OBJ_CLASS_INT_32            3_U8
#define OBJ_CLASS_INT_64            4_U8
#define OBJ_CLASS_UINT_8            5_U8
#define OBJ_CLASS_UINT_16           6_U8
#define OBJ_CLASS_UINT_32           7_U8
#define OBJ_CLASS_UINT_64           8_U8
#define OBJ_CLASS_FLOAT             9_U8
#define OBJ_CLASS_DOUBLE            10_U8
#define OBJ_CLASS_CHAR              11_U8
#define OBJ_CLASS_UCHAR             12_U8
#define OBJ_CLASS_STRING            13_U8
#define OBJ_CLASS_WCHAR             14_U8

#define GET_VARIABLE_NAME(var)      (#var)

#define NULLBYTE                    0_BYTE
#define FULLBYTE                    255_BYTE

#define TIME_NOW                    std::chrono::high_resolution_clock::now()
#define REFRESH_4_FPS               1.0f/4U
#define REFRESH_6_FPS               1.0f/6U
#define REFRESH_12_FPS              1.0f/12U
#define REFRESH_24_FPS              1.0f/24U
#define REFRESH_30_FPS              1.0f/30U
#define REFRESH_60_FPS              1.0f/60U
#define REFRESH_100_FPS             1.0f/100U
#define REFRESH_120_FPS             1.0f/120U

#define MONITOR_STATE_NONE          NULLBYTE
#define MONITOR_STATE_MAX           0b1
#define MONITOR_STATE_MIN           0b10
#define MONITOR_STATE_ZERO          0b100
#define MONITOR_STATE_STOCHASTIC    0b10000
#define MONITOR_STATE_RECORD        0b100000
#define MONITOR_STATE_VERBOSE       0b1000000
#define MONITOR_STATE_ACTIVE        0b10000000

#define OPERATOR_EQUAL              0b1
#define OPERATOR_GREATER            0b10
#define OPERATOR_LESS               0b100
#define OPERATOR_INCREASING         0b1000
#define OPERATOR_DECREASING         0b10000
#define OPERATOR_EXTREME            0b100000

#define DATA_TYPE_UNKNOWN           0_U8
#define DATA_TYPE_NUMERIC           1_U8
#define DATA_TYPE_VERBAL            2_U8
#define DATA_TYPE_IMAGE             3_U8
#define DATA_TYPE_VIDEO             4_U8
#define DATA_TYPE_SOUND             5_U8

#define CMP_STR_MATCH 0b0
#define CMP_STR_CASE_INSENSITIVE 0b1
#define CMP_STR_SIZE_INSENSITIVE 0b10
#define CMP_STR_SMALL_DISCRETE   0b100
#define CMP_STR_SW               0b1000

#define CMP_STR_DEFAULT 0b111

#define SW_PTR_DIAG                     0
#define SW_PTR_LEFT                     1
#define SW_PTR_UP                       2
#define SW_PTR_NONE                     3

#define STATS_TWO_TAILED        0_BIT
#define STATS_UPPER_TAIL        1_BIT
#define STATS_LOWER_TAIL        2_BIT

const static std::string DELIM_NONE                    =  "";
const static std::string DELIM_BASIC                   =  " ,:;\t\n\r";
const static std::string DELIM_STANDARD                =  " ,.?\\\"/\t\n\r!:;&=#";
const static std::string DELIM_STANDARD_MISC           =  ",.?\\\"/\t\n\r!:;&=#";
const static std::string DELIM_CODE                    =  "\"\t\n\r.;:&$+=#<>{}[]";
const static std::string DELIM_ALL                     =  " ,.?\"'/+-=\t\n\r*~!_:;^\\&#()[]{}<>";

#define PI                          acos(-1.0L)
#define MIN_PRECISION               1e-16f
#define SQRT2                       1.4142135623730950

// Typedefs

namespace AIALG{

typedef unsigned char BYTE;

typedef uint8_t UINT8;
typedef uint8_t ByteNum;

typedef uint16_t UINT16;

template<class T> using vMatrix = std::vector<std::vector<T>>;
template<class T> using vMatrix3D = std::vector<std::vector<std::vector<T>>>;
template<class T> using itr = typename std::vector<T>::iterator;
template<class T> using c_itr = typename std::vector<T>::const_iterator;

typedef std::vector<std::string> StringVector;
typedef std::vector<std::vector<std::string>> StringMatrix;

typedef vMatrix<float> numVector;
typedef vMatrix<float> numMatrix;

template<typename T> std::ostream& operator<<(std::ostream& output, const std::vector<T>& V){
    size_t L = V.size();
    output << '[';
    for(size_t i = 0; i < L; ++i){
        output << V[i];
        if((L > 1) & (i < L - 1)) output << ',';
    }
    output << ']';
    return output;
}

template<typename T1, typename T2> std::ostream& operator<<(std::ostream& output, const std::map<T1, T2>& M){

    if(!M.empty()){
        auto endIT = M.end();
        for(auto IT = M.begin(); IT != endIT; ++IT){
            output << IT->first << ": " << IT->second << '\n';
        }
    }

    return output;
}

// Input/Output

template<typename T> void vwrite(const std::vector<T>& V, FILE* outFile){
    size_t vSIZE = V.size();
    fwrite(&vSIZE, sizeof(size_t), 1, outFile);
    fwrite(V.data(), vSIZE, sizeof(T), outFile);
}

inline void fwrite(const std::string& s, FILE* outFILE){
    size_t sSIZE = s.size()+1;
    fwrite(&sSIZE, 1, sizeof(size_t), outFILE);
    fwrite(s.c_str(), sSIZE, sizeof(char), outFILE);
}

inline void vwrite(const StringVector& V, FILE* outFILE){
    size_t vSIZE = V.size();
    fwrite(&vSIZE, 1, sizeof(size_t), outFILE);
    for(auto& str : V){
        fwrite(str, outFILE);
    }
}

inline void fread(std::string& s, FILE* inFILE){
    size_t sSIZE;
    fread(&sSIZE, 1, sizeof(size_t), inFILE);
    s.clear();

    char* newChar = new char[sSIZE];
    fread(newChar, sSIZE, sizeof(char), inFILE);
    s = newChar;
    delete[] newChar;
}

template<typename T> void vread(std::vector<T>& output, FILE* inFile){
    size_t vSIZE;
    fread(&vSIZE, sizeof(size_t), 1, inFile);
    output.clear();
    if(vSIZE < 1) return;

    output.reserve(vSIZE);
    T* tmp = new T[vSIZE];
    fread(tmp, vSIZE, sizeof(T), inFile);
    output.assign(tmp, tmp + vSIZE);
    delete[] tmp;
}

inline void vread(StringVector& output, FILE* inFILE){
    size_t vSIZE;
    output.clear();
    fread(&vSIZE, 1, sizeof(size_t), inFILE);

    for(size_t i = 0; i < vSIZE; ++i){
        output.emplace_back();
        fread(output.back(), inFILE);
    }
}

template<typename T> void writeData(const T& data, FILE* output){
    fwrite(&data, 1, sizeof(T), output);
}

template<typename T> void writeVector(const std::vector<T>& V, FILE* output){
    writeData(V.size(), output);
    fwrite(V.data(), V.size(), sizeof(T), output);
}

template<typename T> void writeMatrix(const std::vector<T>& M, FILE* output){
    size_t L = M.size();
    fwrite(&L, 1, sizeof(size_t), output);
    for(auto& V : M){
        writeVector(V, output);
    }
}

inline void writeString(const std::string& s, FILE* output){
    writeData(s.size(), output);
    fwrite(s.c_str(), 1, s.size(), output);
}

template<> inline void writeData<std::string>(const std::string& data, FILE* output){
    writeString(data, output);
}

template<typename T>
void writeData(const std::vector<T>& data, FILE* output){
    writeVector(data, output);
}

template<typename T> void readData(T& inputVar, FILE* inputFile){
    fread(&inputVar, 1, sizeof(T), inputFile);
}

template<typename T> void readVector(std::vector<T>& output, FILE* inputFile){
    size_t inSIZE; readData(inSIZE, inputFile);
    T* inputData = new T[inSIZE];
    fread(inputData, inSIZE, sizeof(T), inputFile);
    output.reserve(output.size() + inSIZE);
    for(size_t i = 0; i < inSIZE; ++i){
        output.push_back(inputData[i]);
    }
    delete[] inputData;
}



inline void readString(std::string& output, FILE* inFILE){
    output.clear();
    size_t inSIZE; readData(inSIZE, inFILE);
    char* inputData = new char[inSIZE+1];
    inputData[inSIZE] = '\0';
    fread(inputData, 1, inSIZE, inFILE);
    output = inputData;
    delete[] inputData;
}

template<> inline void readData<std::string>(std::string& inputVar, FILE* inputFile){
    readString(inputVar, inputFile);
}

template<typename T>
void readData(std::vector<T>& inputVar, FILE* inputFILE){
    readVector(inputVar, inputFILE);
}

inline void readVector(std::vector<std::string>& output, FILE* inFILE){
    output.clear();
    size_t inSIZE; readData(inSIZE, inFILE);
    output.reserve(output.size() + inSIZE);
    for(size_t i = 0; i < inSIZE; ++i){
        output.emplace_back();
        readString(output.back(), inFILE);
    }
}

template<typename T>
void readMatrix(std::vector<std::vector<T>>& M, FILE* inFILE){
    size_t L;
    fread(&L, 1, sizeof(size_t), inFILE);
    M.resize(L);
    for(auto& V : M){
        readVector(V, inFILE);
    }
}

inline void writeVector(const std::vector<std::string>& V, FILE* output){
    writeData(V.size(), output);
    const c_itr<std::string> itEND = V.end();
    for(c_itr<std::string> it = V.begin(); it != itEND; ++it){
        writeString(*it, output);
    }
}

template<typename key_t, typename value_t> void writeMap(const std::map<key_t, value_t>& M, FILE* outFILE){

    size_t L = M.size();
    fwrite(&L, sizeof(size_t), 1, outFILE);
    if(L == 0) return;

    auto endIT = M.end();
    for(auto IT = M.begin(); IT != endIT; ++IT){

        writeData(IT->first, outFILE);
        writeData(IT->second, outFILE);

    }
}

template<typename key_t, typename value_t> void readMap(std::map<key_t, value_t>& M, FILE* inFILE){

    size_t L;
    fread(&L, sizeof(size_t), 1, inFILE);

    if(L == 0) return;

    M.clear();

    key_t tmp_key;
    value_t tmp_value;

    for(size_t i = 0; i < L; ++i){
        readData(tmp_key, inFILE);
        readData(tmp_value, inFILE);

        M.emplace(tmp_key, tmp_value);
    }

}

// File manipulation

inline size_t file_size(FILE* file){
    fpos_t pos;
    fgetpos(file, &pos);

    fseek(file, 0, SEEK_END);
    size_t output = ftell(file);
    fsetpos(file, &pos);

    return output;
}

inline bool check_hash(FILE* file, const std::string& hash){
    if(hash.empty()) return true;

    // Checks the hash code and sets the stream pointer to the position following
    size_t fileSIZE = file_size(file);
    if(fileSIZE < (2*hash.size())){
        return false;
    }

    char newHash[hash.size() + 1];
    newHash[hash.size()] = '\0';
    fread(newHash, sizeof(char), hash.size(), file);
    if(std::string(newHash) != hash){
        fclose(file);
        return false;
    }

    fpos_t initPos, newPos;
    fgetpos(file, &initPos);

    fseek(file, 0, SEEK_END);
    fseek(file, hash.size(), SEEK_SET);

    fread(newHash, sizeof(char), hash.size(), file);
    if(std::string(newHash) != hash){
        fclose(file);
        return false;
    }

    fsetpos(file, &initPos);

    return true;
}

// Classes

template<class T> constexpr char getTypeID(const T& item){
    return typeid(item).name()[0];
}

template<class T> constexpr char getTypeID(){
    return typeid(T).name()[0];
}

struct GenDataVar{

    void* data;
    char dataClass;
    UINT16 dataSize;

    GenDataVar():
        dataClass(getTypeID<void>()),
        dataSize(0){ }
    GenDataVar(const uint32_t& val){
        uint32_t* newVal = new uint32_t(val);
        data = (void*)newVal;
        dataClass = getTypeID(val);
        dataSize = sizeof(val);
    }
    GenDataVar(const int32_t& val){
        int32_t* newVal = new int32_t(val);
        data = (void*)newVal;
        dataClass = getTypeID(val);
        dataSize = sizeof(val);
    }
    GenDataVar(const bool& val){
        bool* newVal = new bool(val);
        data = (void*)newVal;
        dataClass = getTypeID(val);
        dataSize = sizeof(val);
    }
    GenDataVar(const float& val){
        float* newVal = new float(val);
        data = (void*)newVal;
        dataClass = getTypeID(val);
        dataSize = sizeof(val);
    }
    GenDataVar(const std::string& val){
        dataSize = val.size() + 1;
        char* newStr = new char[dataSize];
        newStr[dataSize-1] = '\0';
        memcpy(newStr, val.c_str(), dataSize-1);
        dataClass = getTypeID<char>();
        data = (void*)newStr;
    }
    GenDataVar(const char* val){
        dataSize = 1;
        while(val[dataSize-1] != '\0'){
            ++dataSize;
        }
        char* newStr = new char[dataSize];
        memcpy(newStr, val, dataSize);
        dataClass = getTypeID<char>();
        data = (void*)newStr;
    }

    inline size_t size() const{ return (size_t)dataSize; }

    GenDataVar& operator=(const char* str){
        if(dataSize > 0) clear();
        dataSize = 1;
        while(str[dataSize-1] != '\0'){
            ++dataSize;
        }
        char* newStr = new char[dataSize];
        memcpy(newStr, str, dataSize);
        dataClass = getTypeID<char>();
        data = (void*)newStr;
        return *this;
    }

    GenDataVar& operator=(const std::string& other){
        return (*this = other.c_str());
    }

    template<typename T> GenDataVar& operator=(const T& other){
        if(dataClass != getTypeID(other)){
            T* newVal = new T(other);
            this->data = (void*)newVal;
            this->dataClass = getTypeID<T>();
        }
        else{
            *((T*)data) = other;
        }
        dataSize = sizeof(other);

        return *this;
    }

    GenDataVar& operator=(const GenDataVar& other){
        clear();
        dataClass = other.dataClass;
        dataSize = other.dataSize;
        if(dataClass == getTypeID<char>()){
            char* newStr = new char[dataSize];
            memcpy(newStr, other.data, dataSize);
            data = (void*)newStr;
        }
        else if(dataClass == getTypeID<uint32_t>()){
            uint32_t* newVal = new uint32_t(other.getValue<uint32_t>());
            data = (void*)newVal;
        }
        else if(dataClass == getTypeID<float>()){
            float* newVal = new float(other.getValue<float>());
            data = (void*)newVal;
        }
        else if(dataClass == getTypeID<bool>()){
            bool* newVal = new bool(other.getValue<bool>());
            data = (void*)newVal;
        }
        else if(dataClass == getTypeID<int32_t>()){
            int32_t* newVal = new int32_t(other.getValue<int32_t>());
            data = (void*)newVal;
        }
        else if(dataClass == getTypeID<uint8_t>()){
            uint8_t* newVal = new uint8_t(other.getValue<uint8_t>());
            data = (void*)newVal;
        }
        return *this;
    }

    inline void write(FILE* outFILE) const{
        fwrite(&dataClass, 1, sizeof(char), outFILE);
        fwrite(&dataSize, 1, sizeof(uint16_t), outFILE);
        if(dataSize > 0){
            fwrite(data, 1, dataSize, outFILE);
        }
    }

    inline void read(FILE* inFILE){
        if(dataSize > 0) clear();
        fread(&dataClass, 1, sizeof(char), inFILE);
        fread(&dataSize, 1, sizeof(uint16_t), inFILE);
        if(dataSize > 0){
            char* newData = new char[dataSize];
            fread(newData, 1, dataSize, inFILE);
            data = (void*)newData;
        }
    }

    friend std::ostream& operator<<(std::ostream& output, const GenDataVar& other){
        if(other.dataClass == getTypeID<uint32_t>()) output << (*(uint32_t*)other.data);
        else if(other.dataClass == getTypeID<float>()) output << (*(float*)other.data);
        else if(other.dataClass == getTypeID<bool>()){
            if(*(bool*)other.data) output << "TRUE";
            else output << "FALSE";
        }
        else if(other.dataClass == getTypeID<char>()) output << ((char*)other.data);
        else if(other.dataClass == getTypeID<int32_t>()) output << (*(int32_t*)other.data);
        else if(other.dataClass == getTypeID<double>()) output << (*(double*)other.data);
        else if(other.dataClass == getTypeID<int8_t>()) output << (*(int8_t*)other.data);
        else if(other.dataClass == getTypeID<int16_t>()) output << (*(int16_t*)other.data);
        else if(other.dataClass == getTypeID<int64_t>()) output << (*(int64_t*)other.data);
        else if(other.dataClass == getTypeID<uint8_t>()) output << (*(uint8_t*)other.data);
        else if(other.dataClass == getTypeID<uint16_t>()) output << (*(uint16_t*)other.data);
        else if(other.dataClass == getTypeID<uint64_t>()) output << (*(uint64_t*)other.data);
        else if(other.dataClass == getTypeID<wchar_t>()) output << ((wchar_t*)other.data);
        return output;
    }

    inline void clear(){
        if(dataClass == getTypeID<uint32_t>()) delete((uint32_t*)data);
        else if(dataClass == getTypeID<float>()) delete((float*)data);
        else if(dataClass == getTypeID<bool>()) delete((bool*)data);
        else if(dataClass == getTypeID<int32_t>()) delete((int32_t*)data);
        else if(dataClass == getTypeID<double>()) delete((double*)data);
        else if(dataClass == getTypeID<int8_t>()) delete((int8_t*)data);
        else if(dataClass == getTypeID<int16_t>()) delete((int16_t*)data);
        else if(dataClass == getTypeID<int64_t>()) delete((int64_t*)data);
        else if(dataClass == getTypeID<uint8_t>()) delete((uint8_t*)data);
        else if(dataClass == getTypeID<uint16_t>()) delete((uint16_t*)data);
        else if(dataClass == getTypeID<uint64_t>()) delete((uint64_t*)data);
        else if(dataClass == getTypeID<char>()) delete((char*)data);
        else if(dataClass == getTypeID<wchar_t>()) delete((wchar_t*)data);
        dataClass = getTypeID<void>();
        dataSize = 0;
    }

    inline std::string getString() const{ return std::string((char*)data); } // MUST use for strings
    template<typename T> inline T getValue() const{
        if(dataSize < 1) return T();
        return *((T*)data);
    }
    template<typename T> inline void putValue(T& item){
        item = getValue<T>();
    }

    GenDataVar(const GenDataVar& other):
        dataClass(other.dataClass),
        dataSize(other.dataSize){
            if(dataClass == getTypeID<uint32_t>()){
                uint32_t* newVal = new uint32_t(other.getValue<uint32_t>());
                data = (void*)newVal;
            }
            else if(dataClass == getTypeID<float>()){
                float* newVal = new float(other.getValue<float>());
                data = (void*)newVal;
            }
            else if(dataClass == getTypeID<bool>()){
                bool* newVal = new bool(other.getValue<bool>());
                data = (void*)newVal;
            }
            else if(dataClass == getTypeID<char>()){
                char* newStr = new char[dataSize];
                memcpy(newStr, other.data, dataSize);
                data = (void*)newStr;
            }
            else if(dataClass == getTypeID<int32_t>()){
                int32_t* newVal = new int32_t(other.getValue<int32_t>());
                data = (void*)newVal;
            }
            else if(dataClass == getTypeID<uint8_t>()){
                uint8_t* newVal = new uint8_t(other.getValue<uint8_t>());
                data = (void*)newVal;
            }
        }

    ~GenDataVar(){
        clear();
    }
};

std::string capitalized(std::string s);
std::string non_capitalized(std::string s);

template<typename T, typename N> bool anyEqual(const T& item, const std::vector<N> &V){
    for(auto& x : V){
        if(x == item) return true;
    }
    return false;
}

class GenDataStruct{
protected:

    StringVector tags;
    std::vector<GenDataVar> data;

public:

    template<typename T> void addEntry(const std::string& tag, const T& newData){
        if(anyEqual(tag, tags)){
            (*this)[tag] = newData;
        }
        else{
            tags.push_back(tag);
            data.emplace_back();
            data.back() = newData;
        }
    }
    inline bool entryExists(const std::string& tag) const{
        for(auto& t : tags){
            if(t == tag) return true;
        }
        return false;
    }

    inline void setEntries(const StringVector& entries){
        tags = entries;
        data.resize(entries.size());
    }

    inline bool save(const std::string& filename, const std::string& hash = "") const{
        if(!access(filename.c_str(), F_OK)){
            if(access(filename.c_str(), W_OK)) return false;
        }

        FILE* outFILE = fopen(filename.c_str(), "wb");

        if(!hash.empty()){
            fwrite(hash.c_str(), hash.size(), sizeof(char), outFILE);
        }

        write(outFILE);

        if(!hash.empty()){
            fwrite(hash.c_str(), hash.size(), sizeof(char), outFILE);
        }

        fclose(outFILE);

        return true;
    }

    inline void write(FILE* outFILE) const{
        size_t L = data.size();
        fwrite(&L, 1, sizeof(size_t), outFILE);
        for(size_t i = 0; i < L; ++i){
            fwrite(tags[i], outFILE);
            data[i].write(outFILE);
        }
    }

    bool load(const std::string& filename, const std::string& hash = ""){
        if(access(filename.c_str(), F_OK)){
            std::cout << "Load from " << filename << " failed\n";
            std::cout << "Error: " << std::strerror(errno) << '\n';
            return false;
        }

        FILE* inFILE = fopen(filename.c_str(), "rb");

        if(!check_hash(inFILE, hash)) return false;

        read(inFILE);

        fclose(inFILE);

        return true;
    }

    inline void read(FILE* inFILE){
        size_t L, matchIndex, j = 0;
        fread(&L, 1, sizeof(size_t), inFILE);
        for(size_t i = 0; i < L; ++i){
            std::string varName;
            fread(varName, inFILE);

            matchIndex = UINT_MAX;
            j = 0;

            for(auto& tag : tags){
                if(tag == varName) matchIndex = j;
                ++j;
            }

            if(matchIndex != UINT_MAX){
                data[matchIndex].read(inFILE);
            }
            else{
                tags.push_back(varName);
                data.emplace_back();
                data.back().read(inFILE);
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& output, const GenDataStruct& other){
        size_t L = other.size();
        for(size_t i = 0; i < L; ++i){
            output << std::left << '\t' << std::setw(25) << capitalized(other.tags[i]) << '\t' << other.data[i] << '\n';
        }
        return output;
    }

    inline GenDataStruct& operator=(const StringVector& variables){
        tags = variables;
        data.clear();
        data.resize(tags.size());
        return *this;
    }

    template<typename value_t> inline value_t getValue(const std::string& tag) const{
        unsigned int index = 0;
        for(auto& T : tags){
            if(T == tag){
                return data[index].getValue<value_t>();
            }
            ++index;
        }
        throw std::invalid_argument("Parameter \"" + tag + "\" does not exist");
    }
    inline std::string getString(const std::string& tag) const{
        unsigned int index = 0;
        for(auto& T : tags){
            if(T == tag){
                return data[index].getString();
            }
            ++index;
        }
        throw std::invalid_argument("Parameter \"" + tag + "\" does not exist");
    }

    inline GenDataVar& operator[](const unsigned int& index){ return data[index]; }
    inline GenDataVar& operator[](const std::string& tag){
        unsigned int index = 0;
        for(auto& T : tags){
            if(T == tag){
                return data[index];
            }
            ++index;
        }
        throw std::invalid_argument("INVALID TAG: tag does not exist in generic data struct");
    }

    inline std::string& getTag(const unsigned int& index){ return tags[index]; }
    inline const StringVector& params() const{ return tags; }

    inline size_t size() const{ return tags.size(); }
    inline void clear(){
        tags.clear();
        data.clear();
    }

    GenDataStruct(){ }
    GenDataStruct(const StringVector& variables):
        tags(variables),
        data(variables.size()){ }

    ~GenDataStruct() = default;

};

template<class T>
struct Vector2{
    T x, y;

    friend std::ostream& operator<<(std::ostream& output, const Vector2<T>& other){
        output << other.x << '\t' << other.y;
        return output;
    }

    template<typename T2>
    inline bool operator==(const Vector2<T2>& other){
        return ((this->x == other.x) && (this->y == other->y));
    }
    template<typename T2>
    inline bool operator!=(const Vector2<T2>& other){
        return !(this == other);
    }
    template<typename T2>
    inline bool operator<(const Vector2<T2>& other){
        return (x < other.x) && (y < other.y);
    }
    template<typename T2>
    inline bool operator>(const Vector2<T2>& other){
        return (x > other.x) && (y > other.y);
    }
    template<typename T2>
    inline bool operator<=(const Vector2<T2>& other){
        return !(*this > other);
    }
    template<typename T2>
    inline bool operator>=(const Vector2<T2>& other){
        return !(*this < other);
    }

    template<typename T2>
    Vector2<T> operator+(const Vector2<T2>& other) const{
        return Vector2(x + other.x, y + other.y);
    }
    template<typename T2>
    Vector2<T> operator-(const Vector2<T2>& other) const{
        return Vector2(x - other.x, y - other.y);
    }
    template<typename T2>
    Vector2<T> operator*(const Vector2<T2>& other) const{
        return Vector2(x * other.x, y * other.y);
    }
    template<typename T2>
    Vector2<T> operator/(const Vector2<T2>& other) const{
        return Vector2(x / other.x, y / other.y);
    }

    template<typename T2>
    Vector2<T> operator+(const T2& other) const{
        return Vector2(x + other, y + other);
    }
    template<typename T2>
    Vector2<T> operator-(const T2& other) const{
        return Vector2(x - other, y - other);
    }
    template<typename T2>
    Vector2<T> operator*(const T2& other) const{
        return Vector2(x * other, y * other);
    }
    template<typename T2>
    Vector2<T> operator/(const T2& other) const{
        return Vector2(x / other, y / other);
    }

    template<typename T2>
    void operator+=(const Vector2<T2>& other){
        x += other.x;
        y += other.y;
    }
    template<typename T2>
    void operator-=(const Vector2<T2>& other){
        x -= other.x;
        y -= other.y;
    }
    template<typename T2>
    void operator*=(const Vector2<T2>& other){
        x *= other.x;
        y *= other.y;
    }
    template<typename T2>
    void operator/=(const Vector2<T2>& other){
        x /= other.x;
        y /= other.y;
    }

    template<typename T2>
    void operator+=(const T2& other){
        x += other;
        y += other;
    }
    template<typename T2>
    void operator-=(const T2& other){
        x -= other;
        y -= other;
    }
    template<typename T2>
    void operator*=(const T2& other){
        x *= other;
        y *= other;
    }
    template<typename T2>
    void operator/=(const T2& other){
        x /= other;
        y /= other;
    }

    template <typename T2>
    Vector2<T>& operator=(const Vector2<T2>& other){
        x = other.x;
        y = other.y;
        return *this;
    }
    template <typename T2>
    Vector2<T>& operator=(const T2& other){
        x = other;
        y = other;
        return *this;
    }

    inline T& in() { return x; }
    inline T& out() { return y; }

    Vector2(){ }
    Vector2(T x, T y):x(x), y(y){ }
    Vector2(std::vector<T>& xy): x(xy[0]), y(xy[1]){ }
};

typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<float> Vector2f;

template<typename T> void fwrite(const Vector2<T>& coords, FILE* outFILE){
    fwrite(&coords.x, 1, sizeof(T), outFILE);
    fwrite(&coords.y, 1, sizeof(T), outFILE);
}

template<typename T> void fread(const Vector2<T>& coords, FILE* inFILE){
    fread(&coords.x, 1, sizeof(T), inFILE);
    fread(&coords.y, 1, sizeof(T), inFILE);
}

template<class T>
class VectorPair{
public:

    std::vector<T> x, y;

    inline bool empty() const{
        return x.empty() || y.empty();
    }

    inline unsigned int size() const{
        return x.size() < y.size() ? x.size() : y.size();
    }

    inline void push_back(const Vector2<T>& newPair){ x.push_back(newPair.x); y.push_back(newPair.y); }
    inline void push_back(const T& newX, const T& newY){ x.push_back(newX); y.push_back(newY); }

    inline void emplace_back(const Vector2<T>& newPair){ x.emplace_back(newPair.x); y.emplace_back(newPair.y); }
    inline void emplace_back(const T& newX, const T& newY){ x.emplace_back(newX); y.emplace_back(newY); }

    inline void erase(const unsigned int index){ x.erase(x.begin() + index); y.erase(y.begin() + index); }
    inline Vector2<T> operator[](const unsigned int index) const{ return Vector2<T>(x[index], y[index]); }
    inline Vector2<T> back() const{ return Vector2<T>(x.back(), y.back()); }
    inline Vector2<T> front() const{ return Vector2<T>(x.front(), y.front()); }

    inline void clear(){
        x.clear();
        y.clear();
    }

    inline void insert(const unsigned int& index, const T& newX, const T& newY){
        x.insert(x.begin() + index, newX);
        y.insert(y.begin() + index, newY);
    }

    inline void remove(const Vector2<T>& ref){
        size_t L = this->size();
        for(size_t i = 0; i < L;){
            if((x[i] == ref.x) && (y[i] == ref.y)){
                erase(i);
                --L;
            }
            else ++i;
        }
    }

    VectorPair(){}
    VectorPair(const std::vector<T>& x, const std::vector<T>& y): x(x), y(y){ }

};

typedef VectorPair<unsigned int> VectorPairU;

template<typename T> std::ostream& operator<<(std::ostream& output, const VectorPair<T>& vP){
    output << "x: " << vP.x << "\ny: " << vP.y;
    return output;
}

template<typename T> T rand(T i, T f){
//    srand(time(NULL));
    return (double(std::rand()) / (RAND_MAX) * (f - i) + i);
}

//template<typename T> T rand(T i, T f){
//    std::array<int, std::mt19937::state_size> seed_data;
//    std::random_device r;
//
//    std::generate(seed_data.begin(), seed_data.end(), std::ref(r));
//    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
//    std::mt19937 eng(seq);
//
//    std::uniform_real_distribution<> randU(i, f);
//
//    return randU(eng);
//}

inline bool draw(const float& chance){
    return rand(0.0f, 1.0f) < chance;
}

inline bool randSwitch(const float& threshold = 0.5f){
    return rand(0.0f,1.0f) < threshold;
}

// Functions

template<typename T>
T nround(const T& value){
    if(value < 0){
        return nearbyint(value);
    }
    return round(value);
}

inline void timed_sleep(float sleepTime){
    std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
}

std::string loadingIndicator(unsigned int barWidth, unsigned int progress);

template<typename T> void vprint(std::vector<T> &V){
    size_t L = V.size();
    std::cout << '[';
    for(size_t i = 0; i < L; ++i){
        std::cout << V[i];
        if((L > 1) & (i < L - 1)) std::cout << ',';
    }
    std::cout << ']';
}

template<typename T, typename out_type> out_type& printColumn(const std::vector<T>& V, const unsigned int indent = 0,
                                                         out_type& outstream = std::cout){
    for(auto& it : V){
        size_t i = 0;
        while(i < indent){
            outstream << '\t';
            ++i;
        }
        outstream << it << '\n';
    }
    return outstream;
}

struct targetList{
    std::vector<std::vector<unsigned int>> targetMatrix;
    std::vector<std::string> tags;

    template<typename T> friend T& operator<<(T& output, targetList& item){
        output << ">> Targets:\n";
        for(size_t i = 0; i < item.targetMatrix.size(); ++i){
            output << "\t* " << item.tags[i] << ": ";
            output << item.targetMatrix[i];
            output << '\n';
        }
        return output;
    }
};

class rotatingIndicator{
private:
    unsigned int orientation;
public:
    inline void rotateRight(){
        if(orientation < 7) ++orientation;
        else orientation = 0; }
    inline void rotateLeft(){
        if(orientation > 0) --orientation;
        else orientation = 7; }

    friend std::ostream& operator<<(std::ostream &output, rotatingIndicator &rotator){
        switch(rotator.orientation){
            case 0:{ output << '|'; break; }
            case 1:{ output << '/'; break; }
            case 2:{ output << '-'; break; }
            case 3:{ output << '\\'; break; }
            case 4:{ output << '|'; break; }
            case 5:{ output << '/'; break; }
            case 6:{ output << '-'; break; }
            case 7:{ output << '\\'; break; }
            default: { output << '|'; break; }
        }
        return output;
    }

    rotatingIndicator(): orientation(0){ }
    ~rotatingIndicator(){ }
};

class numberBox{
public:
    std::vector<std::vector<unsigned int>> indexBox;
    std::vector<unsigned int> indArray;
    bool repeat;

    void init(const unsigned int combo, const unsigned int depth, bool random);

    bool increment();
    void randIndex();

    std::vector<unsigned int> pull(const unsigned int index = 0);

    numberBox(){ }
    numberBox(const unsigned int combo, const unsigned int depth, bool random = false, bool repeat = false):
        indArray(combo, 0),
        repeat(repeat){
        init(combo, depth, random);
    }

    ~numberBox(){ }
};

// Basic operations

template<typename T> inline T absolute(const T& item){
    return (item >= 0) ? item : -item;
}

template<typename T> T min(const std::vector<T> &V){
    size_t L = V.size();
    if(L < 1) return T(0);
    T minV(V[0]);
    for(size_t i = 1; i < L; ++i){
        if(isnan(V[i]) || (V[i] == UINT_MAX)) continue;
        if(isnan(minV) || (minV == UINT_MAX)){
            minV = V[i];
            continue;
        }
        if(V[i] < minV) minV = V[i];
    }
    return minV;
}

template<typename T> unsigned int minIndex(const std::vector<T> &V){
    T minV(V[0]);
    size_t L = V.size();
    unsigned int minI = UINT_MAX;
    for(size_t i = 0; i < L; ++i){
        if(isnan(V[i])) continue;
        if(isnan(minV)){
            minV = V[i];
            minI = i;
            continue;
        }
        if(V[i] < minV){
            minV = V[i];
            minI = i;
        }
        else if(minI == UINT_MAX){
            minI = i;
        }
    }
    return minI;
}

template<typename T> T max(const std::vector<T> &V){
    size_t L = V.size();
    if(L < 1) return T(0);
    T maxV(V[0]);
    for(size_t i = 1; i < L; ++i){
        if(isnan(V[i]) || (V[i] == UINT_MAX)) continue;
        if(isnan(maxV) || (maxV == UINT_MAX)){
            maxV = V[i];
            continue;
        }
        if(V[i] > maxV) maxV = V[i];
    }
    return maxV;
}

template<typename T> T max(const std::vector<std::vector<T>>& M){
    T maxV = M.front().front();
    for(auto& V : M){
        for(auto& val : V){
            if(isnan(val) || (val == UINT_MAX)) continue;
            if(isnan(maxV) || (maxV == UINT_MAX)){
                maxV = val;
                continue;
            }
            if(val > maxV) maxV = val;
        }
    }

    return maxV;
}

// R-style indexing operators

template<typename T, typename index> std::vector<T> indexVector(std::vector<T>& V, const std::vector<index>& indexV){
    std::vector<T> output;
    size_t L = indexV.size(), VL = V.size();
    if((VL < 1) || (L < 1)) return output;
    for(size_t i = 0; i < L; ++i){
        if(indexV[i] < VL){
            output.push_back(V[indexV[i]]);
        }
    }
    return output;
}

template<typename T1, typename T2> unsigned int findIndex(std::vector<T1>& V, T2 query){
    size_t L = V.size();
    for(size_t i = 0; i < L; ++i){
        if(V[i] == query) return i;
    }
    return UINT_MAX;
}

template<typename T> unsigned int numDuplicated(std::vector<T> &V){
    size_t L = V.size(), count(0);
    for(size_t i = 0; i < L; ++i){
        for(size_t j = 0; j < L; ++j){
            if(i == j) continue;
            if(V[i] == V[j]) ++count;
        }
    }
    return count;
}

template<typename T> bool anyDuplicated(std::vector<T> &V){
    size_t L = V.size();
    for(size_t i = 0; i < L; ++i){
        for(size_t j = 0; j < L; ++j){
            if(i == j) continue;
            if(V[i] == V[j]) return true;
        }
    }
    return false;
}

template<typename T1, typename T2> std::vector<T1> vseq(T1 i, T2 f, const unsigned int length = 0){
    std::vector<T1> output;
    if(length == 0){
        output.reserve((size_t)(f-i+T1(1)));
        for(; i <= f; ++i){
            output.push_back(i);
        }
        return output;
    }

    T1 inc = f-1/length + T1(1);
    output.reserve(length);
    for(; i <= f; i += inc){
        output.push_back(i);
    }
    return output;
}

template<typename T> std::vector<T> collapse(const std::vector<std::vector<T>>& matrix){
    std::vector<T> output;
    for(auto& V : matrix){
        for(auto& item : V){
            output.push_back(item);
        }
    }
    return output;
}

template<typename T> size_t maxSize(const std::vector<std::vector<T>>& matrix){
    size_t L(matrix.size());
    if(L < 1) return 0;
    size_t maxSize(0);
    const typename std::vector<std::vector<T>>::const_iterator itEND = matrix.end();
    for(typename std::vector<std::vector<T>>::const_iterator it = matrix.begin(); it != itEND; ++it){
        if(it->size() > maxSize) maxSize = it->size();
    }
    return maxSize;
}

template<typename T> size_t minSize(const std::vector<std::vector<T>>& matrix){
    size_t L(matrix.size());
    if(L < 1) return 0;
    size_t minSize(matrix.front().size());
    const typename std::vector<std::vector<T>>::const_iterator itEND = matrix.end();
    for(typename std::vector<std::vector<T>>::const_iterator it = matrix.begin(); it != itEND; ++it){
        if(it->size() < minSize) minSize = it->size();
    }
    return minSize;
}

template<typename T> size_t validSize(const std::vector<std::vector<T>>& matrix){
    size_t output = 0;
    if(matrix.size() < 1) return output;
    for(auto& y : matrix){
        for(auto& x : y){
            if(!isnan(x) && !isinf(x)) ++output;
        }
    }
    return output;
}

template<typename T> size_t validSize(const std::vector<T>& V){
    size_t output = 0;
    for(auto& x : V){
        if(!isnan(x) && !isinf(x)) ++output;
    }

    return output;
}

template<typename T> std::vector<T> getCrossSection(const unsigned int& index, const std::vector<std::vector<T>>& matrix){
    std::vector<T> output;
    size_t L = matrix.size();
    output.reserve(L);
    for(size_t i = 0; i < L; ++i){
        output.push_back(matrix[i][index]);
    }
    return output;
}

template<typename T> unsigned int minSizeIndex(std::vector<std::vector<T>>& matrix){
    size_t L(matrix.size());
    if(L < 1) return 0;
    size_t minSize(matrix[0].size()), minIndex;
    for(size_t i = 1; i < L; ++i){
        if(matrix[i].size() < minSize){
            minSize = matrix[i].size();
            minIndex = i;
        }
    }
    return minIndex;
}

template<typename T1, typename T2> bool operator==(std::vector<T1>& V1, std::vector<T2>& V2){
    size_t L1 = V1.size(), L2 = V2.size();
    if(L1 != L2) return false;
    for(size_t i = 0; i < L1; ++i){
        if(V1[i] != V2[i]) return false;
    }
    return true;
}

template<typename T1, typename T2> bool operator!=(std::vector<T1>& V1, std::vector<T2>& V2){
    return !(V1 == V2);
}

template<typename T1, typename T2> unsigned int numEqual(const std::vector<T1>& V1, const std::vector<T2>& V2){
    unsigned int output = 0;
    for(auto& val : V1){
        if(anyEqual(val, V2)){
            ++output;
        }
    }
    return output;
}

template<typename T1, typename T2> unsigned int match(const T1& item, const std::vector<T2>& V){
    if(V.size() < 1) return UINT_MAX;
    unsigned int index = 0;
    for(auto& it : V){
        if(it == item) return index;
        ++index;
    }
    return UINT_MAX;
}

template<typename T1, typename T2> const T2& closest(const T1& item, const std::vector<T2>& V){
    std::vector<T2> dist(V.size(), 0);
    size_t L = V.size();

    for(size_t i = 0; i < L; ++i){
        dist[i] = absolute(V[i]-item);
    }

    return V[minIndex(dist)];
}

template<typename T1, typename T2> unsigned int closest_index(const T1& item, const std::vector<T2>& V){
    std::vector<T2> dist(V.size(), 0);
    size_t L = V.size();

    for(size_t i = 0; i < L; ++i){
        dist[i] = absolute(V[i]-item);
    }

    return minIndex(dist);
}

template<typename T1, typename T2> void remove(const T1& item, std::vector<T2>& V){
    size_t L = V.size();
    for(size_t i = 0; i < L;){
        if(V[i] == item){
            V.erase(V.begin() + i);
            --L;
        }
        else ++i;
    }
}

// Vector operations

template<typename T1, typename T2>
std::vector<T2>& coerce(const std::vector<T1>& V1, std::vector<T2>& V2){
    V2.resize(V1.size());
    for(size_t i = 0; i < V2.size(); ++i){
        V2[i] = V1[i];
    }
    return V2;
}

// Vector selection

template<typename T> T& last(const std::vector<T>& V){
    return V.back();
}

template<typename T> T& last(const std::vector<std::vector<T>>& M){
    return M.back().back();
}

template<typename T> T& first(const std::vector<T>& V){
    return V.front();
}

template <typename T> T& first(const std::vector<std::vector<T>>& M){
    return M.front().front();
}

template<typename T1, typename T2> bool vExclusive(std::vector<T1>& V, std::vector<T2>& other){
    size_t L1 = V.size(), L2 = other.size();
    if((L1 < 1) || (L2 < 1)) return true;
    for(size_t i = 0; i < L1; ++i){
        for(size_t j = 0; j < L2; ++j){
            if(V[i] == other[j]) return false;
        }
    }
    return true;
}

template<typename T1, typename T2> std::vector<T1> shared(const std::vector<T1>& V1, const std::vector<T2>& V2){
    size_t L1 = V1.size(), L2 = V2.size();
    std::vector<T1> output;
    for(size_t i = 0; i < L1; ++i){
        for(size_t j = 0; j < L2; ++j){
            if(V1[i] == V2[j]){
                output.push_back(V1[i]);
                break; // Add shared entry once only
            }
        }
    }
    return output;
}

template<typename T1, typename T2> std::vector<T1> without(const std::vector<T1>& V1, const std::vector<T2>& V2){
    std::vector<T1> output;
    for(auto& item : V1){
        if(!anyEqual(item, V2)){
            output.push_back(item);
        }
    }
    return output;
}

template<typename T> std::vector<T> at_index(const std::vector<T>& V, const std::vector<unsigned int>& index){
    size_t L = index.size();
    std::vector<T> output; output.reserve(L);
    for(size_t i = 0; i < L; ++i){
        output.push_back(V[index[i]]);
    }
    return output;
}

template<typename T> std::vector<T> not_index(const std::vector<T>& V, const std::vector<unsigned int>& index){
    size_t L = index.size();
    std::vector<T> output; output.reserve(L);
    for(size_t i = 0; i < L; ++i){
        if(!anyEqual(i, index)) output.push_back(V[i]);
    }
    return output;
}

template<typename T> void select(std::vector<T>& V, const std::vector<unsigned int>& index){
    unsigned int vIdx = 0;
    size_t L = V.size();
    for(size_t i = 0; i < L; ++vIdx){
        if(!anyEqual(vIdx, index)){
            V.erase(V.begin() + i);
            --L;
        }
        else ++i;
    }
}

template<typename T>
struct IndexPair{
    T data;
    unsigned int index;

    IndexPair():
        data(0),
        index(UINT_MAX){ }
    IndexPair(const unsigned int& index, const T& data):
        data(data),
        index(index){ }
};

template<typename T>
struct SubsetInfo{
    std::vector<unsigned int> indices;
    std::vector<T> data;

    inline const unsigned int& index(const unsigned int& i) const{ return indices[i]; }
    inline const T& item(const unsigned int& i) const{ return data[i]; }

    inline unsigned int size() const{ return indices.size(); }

    inline void emplace_back(const unsigned int& index, const T& newValue){
        indices.emplace_back(index);
        data.emplace_back(newValue);
    }
    inline void push_back(const unsigned int& index, const T& newValue){
        indices.push_back(index);
        data.push_back(newValue);
    }

    inline IndexPair<T> operator[](const unsigned int& i) const{
        return IndexPair<T>(indices[i], data[i]);
    }

    friend std::ostream& operator<<(std::ostream& output, const SubsetInfo<T>& other){
        unsigned int L = other.size();
        if(L < 1) return output;

        output << "[";
        for(size_t i = 0; i < L; ++i){
            output << other.indices[i] << ':' << other.data[i];
            if(i < L-1) output << ',';
        }
        output << "]";
        return output;
    }
};

template<typename T1, typename T2> SubsetInfo<T1> select_subset(const std::vector<T1>& V,
                                                                 const BYTE& operators,
                                                                 const T2& metric){
    SubsetInfo<T1> output;
    unsigned int index = 0;
    if(operators & OPERATOR_GREATER){
        if(operators & OPERATOR_EQUAL){
            for(auto& item : V){
                if(item >= metric){
                    output.emplace_back(index, item);
                }
                ++index;
            }
        }
        else{
            for(auto& item : V){
                if(item > metric){
                    output.emplace_back(index, item);
                }
                ++index;
            }
        }
    }
    else if(operators & OPERATOR_LESS){
        if(operators & OPERATOR_EQUAL){
            for(auto& item : V){
                if(item <= metric){
                    output.emplace_back(index, item);
                }
                ++index;
            }
        }
        else{
            for(auto& item : V){
                if(item < metric){
                    output.emplace_back(index, item);
                }
                ++index;
            }
        }
    }

    return output;
}

template<typename T1, typename T2> std::vector<unsigned int> select_subset_idx(const std::vector<T1>& V,
                                                                 const BYTE& operators,
                                                                 const T2& metric){
    std::vector<unsigned int> output;
    unsigned int index = 0;
    if(operators & OPERATOR_GREATER){
        if(operators & OPERATOR_EQUAL){
            for(auto& item : V){
                if(!isnan(item) && (item >= metric)){
                    output.emplace_back(index);
                }
                ++index;
            }
        }
        else{
            for(auto& item : V){
                if(!isnan(item) && (item > metric)){
                    output.emplace_back(index);
                }
                ++index;
            }
        }
    }
    else if(operators & OPERATOR_LESS){
        if(operators & OPERATOR_EQUAL){
            for(auto& item : V){
                if(!isnan(item) && (item <= metric)){
                    output.emplace_back(index);
                }
                ++index;
            }
        }
        else{
            for(auto& item : V){
                if(!isnan(item) && (item < metric)){
                    output.emplace_back(index);
                }
                ++index;
            }
        }
    }

    return output;
}

// Random vector

template<typename T> void vrand(std::vector<T>& output, T ri, T rf, const unsigned int& length, const bool& repeat){ // No copy
    output.clear(); output.resize(length);
    if(repeat){
        for(size_t i = 0; i < length; ++i){
            output.push_back(rand<T>(ri, rf));
        }
        return;
    }

    std::vector<T> possibilities = vseq(ri, rf);
    size_t i = 0, randIndex;

    while((i < length) && !possibilities.empty()){
        randIndex = rand(size_t(0), possibilities.size() - 1);
        output[i] = possibilities[randIndex];
        possibilities.erase(possibilities.begin() + randIndex);
        ++i;
    }
}

template<typename T> std::vector<T> vrand(T ri, T rf, const unsigned int& length, const bool& repeat){
    std::vector<T> output; output.reserve(length);
    if(repeat){
        for(size_t i = 0; i < length; ++i){
            output.push_back(rand<T>(ri, rf));
        }
        return output;
    }

    for(size_t i = 0; i < length; ++i){
        T randValue = rand<T>(ri, rf);
        while(anyEqual(randValue, output)){
            randValue = rand<T>(ri, rf);
        }
        output.push_back(randValue);
    }

    return output;
}

template<typename T> std::vector<T> pickRand(std::vector<T>& output, const std::vector<T>& V, const unsigned int length){
    size_t L = V.size();
    std::vector<size_t> randIndex = vrand(size_t(0), L-1, length, false);
    output.clear(); output.reserve(L);

    for(size_t i = 0; i < length; ++i){
        output.push_back(V[randIndex[i]]);
    }
    return output;
}

inline std::string rand_hash(const unsigned int& length, const std::string& allowed = ""){

    std::string output;

    if(allowed.empty()){
        std::string allowed = std::string(ALPHABET) + DIGITS + HASH_CHAR;
        while(output.size() < length){
            output += allowed[rand(size_t(0),allowed.size()-1)];
        }
    }
    else{
        while(output.size() < length){
            output += allowed[rand(size_t(0),allowed.size()-1)];
        }
    }

    return output;

}

template<typename T> void randomize(std::vector<T>& V){
    size_t L = V.size();
    std::vector<size_t> newIndex;
    vrand(newIndex, size_t(0), L-1, L, false);

    std::vector<T> output(L);
    for(size_t i = 0; i < L; ++i){
        output[i] = V[newIndex[i]];
    }
    V = output;
}

template<typename T> std::vector<T> pickRange(std::vector<T>& V, const unsigned int first, const unsigned int last){
    std::vector<T> output;
    if(first < last){
        output.reserve(last - first + 1);
        for(unsigned int i = first; i <= last; ++i){
            output.push_back(i);
        }
        return output;
    }
    else if(first > last){
        output.reserve(first - last + 1);
        for(unsigned int i = first+1; i > last; --i){
            output.push_back(i-1);
        }
        return output;
    }
    output.push_back(V[first]);
    return output;
}

template<typename T, typename N> unsigned int numEqual(T item, std::vector<N> &V){
    size_t L = V.size();
    unsigned int count(0);
    for(size_t i = 0; i < L; ++i){
        if(V[i] == item) ++count;
    }
    return count;
}

template<typename T1, typename T2> unsigned int numEqual(std::vector<T1>& V1, std::vector<T2>& V2){
    size_t L1 = V1.size(), L2 = V2.size();
    unsigned int count(0);
    for(size_t i = 0; i < L1; ++i){
        for(size_t j = 0; j < L2; ++j){
            if(V1[i] == V2[j]) ++count;
        }
    }
    return count;
}

template<typename T> void erase_all(std::vector<T>& V){
    V.erase(V.begin(), V.end());
}

template<typename T> std::string vstring(std::vector<T> &V){
    std::string output = "[";
    for(size_t i = 0; i < V.size(); ++i){
        output += std::to_string(V[i]);
        if((V.size() > 1) && (i < V.size() - 1)){
            output += ',';
        }
    }
    output += ']';
    return output;
}

template<typename T> std::ostream& vstream(std::ostream& output, std::vector<T> &V){
    for(size_t i = 0; i < V.size(); ++i){
        output << V[i];
        if((V.size() > 1) && (i < V.size() - 1)){
            output << ',';
        }
    }
    output << ']';
    return output;
}

template<typename T> unsigned int maxSizeIndex(const std::vector<std::vector<T>>& matrix){
    size_t L = matrix.size();
    unsigned int maxInd(0), maxL(0);
    for(size_t i = 0; i < L; ++i){
        if(matrix[i].size() > maxL){
            maxL = matrix[i].size();
            maxInd = i;
        }
    }
    return maxInd;
}

template<typename T> T average(const std::vector<T> &V){
    size_t L = V.size();
    if(L < 1) return T();
    if(L == 1) return V.front();

    T output(0);
    size_t N = 0;
    for(size_t i = 0; i < L; ++i){
        if(isnan(V[i])) continue;
        output += V[i];
        ++N;
    }
    return output/N;
}

template<typename T> T average(const std::vector<std::vector<T>>& V){
    size_t tSIZE = 0;
    if(V.size() < 1) return T();

    T output(0);
    for(auto& y : V){
        for(auto& x : y){
            if(isnan(x)) continue;
            output += x;
            ++tSIZE;
        }
    }

    return output/tSIZE;
}

template<typename T> void order(std::vector<T> &V, bool ascending = true){
    size_t L = V.size();
    if(L < 2) return;

    T tmp;
    if(ascending){
        for(size_t i = 0; i < L-1; ++i){
            for(size_t j = i+1; j < L; ++j){
                if(V[j] < V[i]){
                    tmp = V[i];
                    V[i] = V[j];
                    V[j] = tmp;
                }
            }
        }
    }
    else{
        for(size_t i = 0; i < L-1; ++i){
            for(size_t j = i+1; j < L; ++j){
                if(V[j] > V[i]){
                    tmp = V[i];
                    V[i] = V[j];
                    V[j] = tmp;
                }
            }
        }
    }
}

template<typename T> void bringToFront(const T& item, std::vector<T>& V){
    if(V.size() < 2) return;
    size_t matchIndex = match(item, V);
    if((matchIndex == UINT_MAX) || (matchIndex < 1)) return;

    V.insert(V.begin(), item);
    V.erase(V.begin() + matchIndex + 1);
}

template<typename T> void sendToBack(const T& item, std::vector<T>& V){
    if(V.size() < 2) return;
    size_t matchIndex = match(item, V);
    if(matchIndex >= V.size() - 1) return;

    V.insert(V.end(), item);
    V.erase(V.begin() + matchIndex);
}

template<typename T> void bringForward(const T& item, std::vector<T>& V){
    if(V.size() < 2) return;
    size_t matchIndex = match(item, V);
    if((matchIndex == UINT_MAX) || (matchIndex < 1)) return;

    T tmp;
    tmp = V[matchIndex - 1];
    V[matchIndex - 1] = V[matchIndex];
    V[matchIndex] = tmp;
}

template<typename T> void sendBackward(const T& item, std::vector<T>& V){
    if(V.size() < 2) return;
    size_t matchIndex = match(item, V);
    if(matchIndex >= V.size() - 1) return;

    T tmp;
    tmp = V[matchIndex + 1];
    V[matchIndex + 1] = V[matchIndex];
    V[matchIndex] = tmp;
}

template<typename T> T median(std::vector<T> V){
    size_t L = V.size();
    if(L < 1) return NAN;
    if(L == 1) return V.front();

    order(V);

    for(size_t i = 0; i < L;){
        if(isnan(V[i])){
            V.erase(V.begin() + i);
            --L;
        }
        else ++i;
    }

    if((L % 2) == 0) return (V[L/2-1]+V[L/2])/2;
    return V[L/2];
}

template<typename T> T median(const std::vector<std::vector<T>>& M){
    std::vector<T> V;
    for(auto& row : M){
        V.insert(V.end(), row.begin(), row.end());
    }
    return median(V);
}

template<typename T> T Q1(std::vector<T> V){
    order(V);
    size_t L = V.size();
    while((L > 2) && (V.size() > L/2)){
        V.pop_back();
    }
    return median(V);
}

template<typename T> T Q3(std::vector<T> V){
    order(V);
    size_t L = V.size();
    while((L > 2) && (V.size() > L/2)){
        V.erase(V.begin());
    }
    return median(V);
}

template<class T>
struct DataSummary{
    T lowerOutlierRange,
        Q1, median, Q3,
        upperOutlierRange;

    DataSummary(){ }
    DataSummary(std::vector<T> data):
        lowerOutlierRange(0),
        Q1(0),
        median(0),
        Q3(0),
        upperOutlierRange(0){

        order(data);
        size_t L = data.size();

        for(size_t i = 0; i < L;){
            if(isnan(data[i]) || isinf(data[i])){
                data.erase(data.begin() + i);
                --L;
            }
            ++i;
        }

        if(!L) return;
        if(L == 1){
            Q3 = median = Q1 = data.front();
            return;
        }
        else if(L < 4){
            Q1 = data.front();
            Q3 = data.back();
            if(L == 2) median = average(data);
            else median = data[1];
            return;
        }


        size_t initL = L;

        std::vector<T> Q3data = data;

        if((L % 2) == 0) this->median = (data[L/2-1]+data[L/2])/2;
        else this->median = data[L/2];

        while(data.size() > initL/2){
            data.pop_back();
        }

        L = data.size();

        if((L % 2) == 0) this->Q1 = (data[L/2-1]+data[L/2])/2;
        else this->Q1 = data[L/2];

        while(Q3data.size() > initL/2){
            Q3data.erase(Q3data.begin());
        }

        L = Q3data.size();

        if((L % 2) == 0) this->Q3 = (Q3data[L/2-1]+Q3data[L/2])/2;
        else this->Q3 = Q3data[L/2];

        this->lowerOutlierRange = this->median - 1.5*(this->Q3 - this->Q1);
        this->upperOutlierRange = this->median + 1.5*(this->Q3 - this->Q1);
    }

    void getSummary(const std::vector<T>& data){
        *this = summary(data);
    }

    friend std::ostream& operator<<(std::ostream& output, const DataSummary& other){
        output << "Data summary:\n";
        output << "\nLower outlier range:\t" << other.lowerOutlierRange
               << "\nLower quartile:\t" << other.Q1
               << "\nMedian:\t" << other.median
               << "\nUpper Quartile:\t" << other.Q3
               << "\nUpper Outlier Range:\t" << other.upperOutlierRange;
        return output;
    }

};

template<typename T> DataSummary<T> summary(std::vector<T> data){

    DataSummary<T> output;

    order(data);
    size_t L = data.size();
    for(size_t i = 0; i < L;){
        if(isnan(data[i]) || isinf(data[i])){
            data.erase(data.begin() + i);
            --L;
        }
        ++i;
    }

    size_t initL = L;
    std::vector<T> Q3data = data;

    if((L % 2) == 0) output.median = (data[L/2-1]+data[L/2])/2;
    else output.median = data[L/2];

    while(data.size() > initL/2){
        data.pop_back();
    }

    L = data.size();

    if((L % 2) == 0) output.Q1 = (data[L/2-1]+data[L/2])/2;
    else output.Q1 = data[L/2];

    while(Q3data.size() > initL/2){
        Q3data.erase(Q3data.begin());
    }

    L = Q3data.size();

    if((L % 2) == 0) output.Q3 = (Q3data[L/2-1]+Q3data[L/2])/2;
    else output.Q3 = Q3data[L/2];

    output.lowerOutlierRange = output.median - 1.5f*(output.Q3 - output.Q1);
    output.upperOutlierRange = output.median + 1.5f*(output.Q3 - output.Q1);

    return output;

}

template<typename T> T percentileValue(float percentile, std::vector<T> V, bool ascending = true){
    if(V.size() < 1) return T();
    order(V, ascending);
    unsigned int position = (V.size() - 1)*percentile;
    return V[position];
}

template<typename T> T sum(const std::vector<T> &V){
    if(V.size() < 1) return NAN;
    T output(NAN);
    for(auto& num : V){
        if(isnan(output)) output = num;
        else if(isnan(num)) continue;
        else output += num;
    }
    return output;
}

template<typename T> T sum(const vMatrix<T>& M){
    if(M.empty()) return NAN;
    T output(NAN);
    for(auto& row : M){
        for(auto& num : row){
            if(isnan(output)) output = num;
            else if(isnan(num)) continue;
            else output += num;
        }
    }
    return output;
}

template<typename T> T cumprod(const std::vector<T> &V){
    if(V.size() < 1) return T();
    T output(V.front());
    size_t L = V.size();
    for(size_t i = 1; i < L; ++i){
        output *= V[i];
    }
    return output;
}

template<typename T> unsigned int maxIndex(const std::vector<T> &V){
    T maxV(V[0]);
    size_t L = V.size();
    unsigned int maxI = UINT_MAX;
    for(size_t i = 0; i < L; ++i){
        if(isnan(V[i]) || isinf(V[i])) continue;
        if(isnan(maxV) || isinf(maxV)){
            maxV = V[i];
            maxI = i;
            continue;
        }
        if(V[i] > maxV){
            maxV = V[i];
            maxI = i;
        }
        else if(maxI == UINT_MAX){
            maxI = i;
        }
    }
    return maxI;
}

template<typename T> T mode(const std::vector<T>& V){
    typename std::vector<T>::const_iterator itEND = V.end();
    std::vector<T> counted;
    std::vector<unsigned int> counts;
    for(typename std::vector<T>::const_iterator it = V.begin(); it != itEND; ++it){
        if(!anyEqual(*it, counted)) counted.push_back(*it);
        counts.push_back(1);
        for(typename std::vector<T>::const_iterator subIT = V.begin(); subIT != itEND; ++subIT){
            if(it == subIT) continue;
            if(*it == *subIT) ++counts.back();
        }
    }
    return counted[maxIndex(counts)];
}

template<typename T> T minDist(const std::vector<T>& V){
    size_t L = V.size();
    if(L < 2) return T(0);
    T minD = abs(V[1] - V[0]);
    for(size_t i = 0; i < L; ++i){
        for(size_t j = i+1; j < L-1; ++j){
            if(abs(V[i] - V[j]) < minD) minD = abs(V[i] - V[j]);
        }
    }
    return minD;
}

template<typename T> T maxDist(const std::vector<T>& V){
    size_t L = V.size();
    if(L < 2) return T(0);
    T maxD = abs(V[1] - V[0]);
    for(size_t i = 0; i < L; ++i){
        for(size_t j = i+1; j < L-1; ++j){
            if(abs(V[i] - V[j]) > maxD) maxD = abs(V[i] - V[j]);
        }
    }
    return maxD;
}

template<typename T> T rankedMax(const unsigned int rank, const std::vector<T>& V){
    T highVal, minVal = min(V);
    unsigned int highIdx;
    std::vector<unsigned int> highIndices;
    size_t L = V.size();
    for(size_t i = 0; i <= rank; ++i){
        highVal = minVal;
        for(size_t j = 0; j < L; ++j){
            if(anyEqual(j, highIndices)) continue;
            if(V[j] > highVal){
                highVal = V[j];
                highIdx = j;
            }
        }
        highIndices.push_back(highIdx);
    }
    return V[highIndices.back()];
}

template<typename T> unsigned int rankedMaxIndex(const unsigned int rank, const std::vector<T>& V){
    T highVal, minVal = min(V);
    unsigned int highIdx;
    std::vector<unsigned int> highIndices;
    size_t L = V.size();
    for(size_t i = 0; i <= rank; ++i){
        highVal = minVal;
        for(size_t j = 0; j < L; ++j){
            if(anyEqual(j, highIndices)) continue;
            if(V[j] > highVal){
                highVal = V[j];
                highIdx = j;
            }
        }
        highIndices.push_back(highIdx);
    }
    return highIndices.back();
}

template<typename T> T rankedMin(const unsigned int rank, const std::vector<T>& V){
    T lowVal, maxVal = max(V);
    unsigned int lowIdx;
    std::vector<unsigned int> lowIndices;
    size_t L = V.size();
    for(size_t i = 0; i <= rank; ++i){
        lowVal = maxVal;
        for(size_t j = 0; j < L; ++j){
            if(anyEqual(j, lowIndices)) continue;
            if(V[j] < lowVal){
                lowVal = V[j];
                lowIdx = j;
            }
        }
        lowIndices.push_back(lowIdx);
    }
    return V[lowIndices.back()];
}

template<typename T> unsigned int rankedMinIndex(const unsigned int rank, const std::vector<T>& V){
    T lowVal, maxVal = max(V);
    unsigned int lowIdx;
    std::vector<unsigned int> lowIndices;
    size_t L = V.size();
    for(size_t i = 0; i <= rank; ++i){
        lowVal = maxVal;
        for(size_t j = 0; j < L; ++j){
            if(anyEqual(j, lowIndices)) continue;
            if(V[j] < lowVal){
                lowVal = V[j];
                lowIdx = j;
            }
        }
        lowIndices.push_back(lowIdx);
    }
    return lowIndices.back();
}

template<typename T> T min(const std::vector<std::vector<T>>& M){
    T minV = M.front().front();
    for(auto& V : M){
        for(auto& val : V){
            if(isnan(minV) || (val == UINT_MAX)){
                minV = val;
                continue;
            }

            if(isnan(val) || (minV == UINT_MAX)) continue;
            if(val < minV) minV = val;
        }
    }

    return minV;
}

template<typename T> T range(const std::vector<T>& V){
    typename std::vector<T>::const_iterator itEND = V.end();
    float vMax = V.front(), vMin = V.front();
    for(typename std::vector<T>::const_iterator it = V.begin(); it != itEND; ++it){
        if(*it > vMax) vMax = *it;
        else if(*it < vMin) vMin = *it;
    }
    return absolute(vMax - vMin);
}

template<typename T> T stdev(const std::vector<T>& V){
    T vMean = average(V);
    T output(0);
    unsigned int N = 0;
    for(const auto& num : V){
        if(isnan(num) || isinf(num)) continue;
        output += pow(num - vMean, 2);
        ++N;
    }
    return sqrt(output/N);
}

template<typename T> T stdev(const std::vector<std::vector<T>>& V){
    T vMean = average(V);
    T output(0);

    unsigned int tSIZE = 0;
    for(const auto& row : V){
        for(const auto& col : row){
            if(isnan(col) || isinf(col)) continue;
            output += pow(col - vMean, 2);
            ++tSIZE;
        }
    }

    return sqrt(output/tSIZE);
}

template<typename T> std::vector<unsigned int> orderedIndex(std::vector<T> V, bool ascending = true){
    size_t L = V.size(), MI = UINT_MAX;
    std::vector<unsigned int> output;

    if(ascending){
        for(size_t i = 0; i < L; ++i){
            MI = minIndex(V);
            if((MI == UINT_MAX) || isnan(V[MI])) continue;
            output.push_back(MI);
            V[MI] = NAN;
        }
    }
    else{
        for(size_t i = 0; i < L; ++i){
            MI = maxIndex(V);
            if((MI == UINT_MAX) || isnan(V[MI])) continue;
            output.push_back(MI);
            V[MI] = NAN;
        }
    }

    return output;
}

template<typename T> std::vector<T> unique(std::vector<T> V){
    size_t L = V.size();
    for(size_t i = 0; i < L; ++i){
        for(size_t j = 0; j < L;){
            if(j == i){ ++j; continue; }
            else if(V[j] == V[i]){
                V.erase(V.begin() + j);
                --L;
            }
            else ++j;
        }
    }
    return V;
}

template<typename T> unsigned int numUnique(const std::vector<T>& V){
    if(V.empty()) return 0;
    else if(V.size() == 1) return 1;

    unsigned int numDuplicated = 0;
    bool duplicated = false;
    std::vector<T> uniqueItems;
    typename std::vector<T>::const_iterator endIT = V.end();
    for(typename std::vector<T>::const_iterator it1 = V.begin(); it1 != endIT - 1; ++it1){
        if(anyEqual(*it1, uniqueItems)) continue;
        for(typename std::vector<T>::const_iterator it2 = it1 + 1; it2 != endIT; ++it2){
            if(*it1 == *it2){
                duplicated = true;
                ++numDuplicated;
            }
        }
        if(duplicated){
            uniqueItems.emplace_back(*it1);
            duplicated = false;
        }
    }
    return V.size() - numDuplicated;
}

template<typename T> std::vector<unsigned int> getRepeatCounts(const std::vector<T>& V){
    size_t L = V.size(), j;
    std::vector<unsigned int> output(L, 1);
    for(size_t i = 0; i < L; ++i){
        for(j = 0; j < L; ++j){
            if(i == j) continue;
            if(V[i] == V[j]) ++output[i];
        }
    }
    return output;
}

template<typename T> void rmDuplicates(std::vector<T> &V){
    size_t L = V.size();
    if(L < 2) return;

    for(size_t i = 0; i < L-1; ++i){
        for(size_t j = i+1; j < L;){
            if(V[j] == V[i]){
                V.erase(V.begin() + j);
                --L;
            }
            else ++j;
        }
    }
}

inline unsigned int numValid(const std::vector<float>& V){
    unsigned int output = 0;
    for(auto& val : V){
        if(!isnan(val) && !isinf(val)) ++output;
    }
    return output;
}

template<typename T> void append(std::vector<T>& source, const std::vector<T>& other){
    source.insert(source.end(), other.begin(), other.end());
}

template<typename T1, typename T2> unsigned int rank_in(const T1& item, std::vector<T2> V, bool ascending = false){
    if(V.size() < 1) return T1();
    rmDuplicates(V);
    typename std::vector<T2>::const_iterator itEND = V.end();
    unsigned int rank = 0;
    if(!ascending){
        for(typename std::vector<T2>::const_iterator it = V.begin(); it != itEND; ++it){
            if(*it > item) ++rank;
        }
    }else{
        for(typename std::vector<T2>::const_iterator it = V.begin(); it != itEND; ++it){
            if(*it < item) ++rank;
        }
    }
    return rank;
}

template<typename T1, typename T2> void rank_list(std::vector<T1>& outputRanks, const std::vector<T2>& V, bool ascending = false){
    if(V.size() < 1) return;
    std::vector<T2> uniqueV = V;
    outputRanks.reserve(outputRanks.size() + V.size());
    rmDuplicates(uniqueV);
    typename std::vector<T2>::const_iterator itEND = V.end(), uniqueEND = uniqueV.end();
    if(!ascending){
        for(typename std::vector<T2>::const_iterator it = V.begin(); it != itEND; ++it){
            unsigned int rank = 0;
            for(typename std::vector<T2>::const_iterator unique = uniqueV.begin(); unique != uniqueEND; ++unique){
                if(*unique > *it) ++rank;
            }
            outputRanks.push_back(rank);
        }
    }else{
        for(typename std::vector<T2>::const_iterator it = V.begin(); it != itEND; ++it){
            unsigned int rank = 0;
            for(typename std::vector<T2>::const_iterator unique = uniqueV.begin(); unique != uniqueEND; ++unique){
                if(*unique < *it) ++rank;
            }
            outputRanks.push_back(rank);
        }
    }
}

template<typename T, typename O> void pairSort(std::vector<T> &V, std::vector<O> &other, bool ascending = true){
    size_t L = V.size();
    if(ascending){
        for(size_t i = 0; i < L-1; ++i){
            for(size_t j = i+1; j < L; ++j){
                if(V[j] < V[i]){
                    T tmp(V[i]);
                    V[i] = V[j];
                    V[j] = tmp;
                    O otmp(other[i]);
                    other[i] = other[j];
                    other[j] = otmp;
                }
            }
        }
    }
    else{
        for(size_t i = 0; i < L-1; ++i){
            for(size_t j = i+1; j < L; ++j){
                if(V[j] > V[i]){
                    T tmp(V[i]);
                    V[i] = V[j];
                    V[j] = tmp;
                    O otmp(other[i]);
                    other[i] = other[j];
                    other[j] = otmp;
                }
            }
        }
    }
}

template<typename T> T factorial(T value){
    if(value == 0) return T(1);
    if(value >= T(1)) value *= factorial(value-T(1));
    return value;
}

template<typename T> void to_ptrV(std::vector<T> &V, std::vector<T*>& ptrV){
    size_t L = V.size();
    ptrV.reserve(L);
    for(size_t i = 0; i < L; ++i){
        ptrV.push_back(&V[i]);
    }
}

bool increment(std::vector<unsigned int>& V, std::vector<unsigned int>& index, unsigned int iMAX);
unsigned int strSum(std::string& s);

// Matrix functions

template<typename T> std::vector<T> collapse(std::vector<std::vector<T>>& V){
    std::vector<T> output = V[0];
    for(size_t i = 1; i < V.size(); ++i){
        output = shared(output, V[i]);
    }
    return output;
}

template<typename T>
size_t area(const std::vector<std::vector<T>>& M){
    size_t output = 0;
    for(auto& row : M){
        output += row.size();
    }
    return output;
}

template<typename T>
size_t volume(const std::vector<std::vector<std::vector<T>>>& tensor){
    size_t output = 0;
    for(auto& plane : tensor){
        output += area(plane);
    }
    return output;
}

// Coordinate functions

//template<typename T>
//bool _2Dpath(vMatrix<T>& output,
//             const std::vector<T>& begin,
//             const std::vector<T>& end){
//
//    output.clear();
//    if((begin.size() < 2) || (end.size() < 2)) return false;
//
//    float distX = end[0]-begin[0];
//    float distY = end[1]-begin[1];
//
//    // Determine straight line distance
//    float dist = sqrt(float(pow(distX, 2) + pow(distY, 2)));
//    float stepX = distX != 0.0 ? distX/distY : 0.0;
//    float stepY = distY != 0.0 ? distY/distX : 0.0;
//
//    std::vector<T> xCoords; xCoords.reserve(dist);
//    std::vector<T> yCoords; yCoords.reserve(dist);
//    xCoords.push_back(begin[0]); yCoords.push_back(begin[1]);
//
//    if(stepX != 0.0) for(float x = begin[0] + stepX; x <= distX; x += stepX){ xCoords.push_back(x); }
//    if(stepY != 0.0) for(float y = begin[1] + stepY; y <= distY; y += stepY){ yCoords.push_back(y); }
//    for(size_t i = 1; i < xCoords.size(); ++i){
//        if(xCoords[i] > xCoords[i-1]+1){
//            xCoords.insert(xCoords.begin() + i, xCoords[i-1]+1);
//        }
//    }
//    for(size_t i = 1; i < yCoords.size(); ++i){
//        if(yCoords[i] > yCoords[i-1]+1){
//            yCoords.insert(yCoords.begin() + i, yCoords[i-1]+1);
//        }
//    }
//
//    while(xCoords.back() < end[0]){
//        xCoords.push_back(xCoords.back() + 1);
//    }
//    while(yCoords.back() < end[1]){
//        yCoords.push_back(yCoords.back() + 1);
//    }
//
//    while(xCoords.size() < yCoords.size()){
//        xCoords.push_back(xCoords.back());
//    }
//    while(yCoords.size() < xCoords.size()){
//        yCoords.push_back(yCoords.back());
//    }
//
//    for(size_t i = 0; i < xCoords.size(); ++i){
//        output.push_back({xCoords[i], yCoords[i]});
//    }
//
//    return output.size() > 0;
//
//}

template<typename T>
bool _2Dpath(vMatrix<T>& output,
             const std::vector<T>& begin,
             const std::vector<T>& end){

    output.clear();
    if((begin.size() < 2) || (end.size() < 2)) return false;

    double distX = double(end[0])-double(begin[0]);
    double distY = double(end[1])-double(begin[1]);

    // Determine straight line distance

    double xPos = begin[0],
            yPos = begin[1],
            x = 0.0,
            y = 0.0,
            inc,
            xEnd = nround(end[0]),
            yEnd = nround(end[1]);

    // Determine angle of motion

    double dist = sqrt(pow(distX,2) + pow(distY, 2)),
            angle;
    if(distY >= 0){
        if(distX >= 0) angle = asin(distY/dist); // Q1
        else angle = PI - asin(distY/dist); // Q2
    }
    else{
        if(distX >= 0) angle = 2*PI + asin(distY/dist); // Q4
        else angle = PI - asin(distY/dist); // Q3
    }

    if(abs(distX) >= abs(distY)){
        do{
            output.emplace_back(std::vector<T>({T(xPos), T(yPos)}));

            if(distX >= 0){
                ++xPos;
            }
            else{
                --xPos;
            }

            ++x;

            inc = 1.0*tan(angle);
            if((inc > 0) ^ (distY > 0)){
                inc = -inc;
            }
            yPos += inc;

        }while(x <= abs(distX));
    }
    else{
        do{
            output.emplace_back(std::vector<T>({T(xPos), T(yPos)}));

            if(distY >= 0){
                ++yPos;
            }
            else{
                --yPos;
            }

            ++y;

            inc = 1.0/tan(angle);
            if((inc > 0) ^ (distX > 0)){
                inc = -inc;
            }
            xPos += inc;

        }while(y <= abs(distY));
    }

    return output.size() > 0;

}

template<typename T> float horizontal_angle(const T& originX, const T& originY,
                                 const T& destinationX, const T& destinationY){
    float dist = sqrt(pow(destinationX - originX,2) + pow(destinationY - originY, 2));
    float vDist = destinationY - originY;
    return asin(vDist/dist) * 180.0f/PI;
}

template<typename T> float vertical_angle(const T& originX, const T& originY,
                                 const T& destinationX, const T& destinationY){
    float dist = sqrt(float(pow(destinationX - originX, 2) + pow(destinationY - originY, 2)));
    float hDist = destinationX - originX;
    return acos(hDist/dist) * 180.0f/PI;
}

// Statistical algorithms

inline float pearson_coefficient(const std::vector<float>& xV, const std::vector<float>& yV){
    float sumX(0.0f), sumY(0.0f), sumXY(0.0f), sumXsq(0.0f), sumYsq(0.0f);
    unsigned int L = (xV.size() <= yV.size()) ? xV.size() : yV.size();
    for(size_t i = 0; i < L; ++i){
        if(isnan(xV[i]) || isnan(yV[i])) continue;
        sumX += xV[i]; sumY += yV[i]; sumXY += (xV[i]*yV[i]);
        sumXsq += pow(xV[i], 2); sumYsq += pow(yV[i], 2);
    }
    return (L*sumXY - (sumX*sumY))/(sqrt(L*sumXsq - pow(sumX, 2))*sqrt(L*sumYsq - pow(sumY, 2)));
}

inline float spearman_coefficient(const std::vector<float>& xV, const std::vector<float>& yV){
    unsigned int L = (xV.size() <= yV.size()) ? xV.size() : yV.size();
    if(L < 1) return NAN;

    std::vector<float> rankX, rankY;
    rank_list(rankX, xV); rank_list(rankY, yV);

    float sumX(0.0f), sumY(0.0f), sumXY(0.0f), sumXsq(0.0f), sumYsq(0.0f);
    for(size_t i = 0; i < L; ++i){
        if(isnan(rankX[i]) || isnan(rankY[i])) continue;
        sumX += rankX[i]; sumY += rankY[i]; sumXY += (rankX[i]*rankY[i]);
        sumXsq += pow(rankX[i], 2); sumYsq += pow(rankY[i], 2);
    }
    return (L*sumXY - (sumX*sumY))/(sqrt(L*sumXsq - pow(sumX, 2))*sqrt(L*sumYsq - pow(sumY, 2)));
}

inline float point_biserial_coefficient(const std::vector<float>& data,
                                        const std::vector<unsigned int>& outcomeIndex){
    unsigned int L = data.size();
    if(L < 1) return NAN;

    float posMean = 0.0f, negMean = 0.0f, allMean = 0.0f, MSE = 0.0f;
    unsigned int posSIZE = 0, negSIZE = 0, tSIZE = 0;
    for(size_t i = 0; i < L; ++i){
        if(isnan(data[i])) continue;

        allMean += data[i];
        ++tSIZE;
        if(anyEqual(i, outcomeIndex)){
            ++posSIZE; posMean += data[i];
        }
        else{
            ++negSIZE; negMean += data[i];
        }
    }
    if((posSIZE < 1) || (negSIZE < 1)) return NAN;

    posMean /= posSIZE;
    negMean /= negSIZE;
    allMean /= tSIZE;

    for(size_t i = 0; i < L; ++i){
        if(isnan(data[i])) continue;
        MSE += pow(data[i] - allMean, 2);
    }

    return ((posMean-negMean)/sqrt(MSE/tSIZE))*sqrt(posSIZE*negSIZE/pow(tSIZE, 2));
}

inline float signal_to_noise(const std::vector<float>& data,
                             const std::vector<unsigned int>& outcomeIndex){
    unsigned int L = data.size();
    if(numValid(data) < 2) return NAN;
    if(outcomeIndex.empty()) return NAN;

    std::vector<float> sample, rem;

    for(size_t i = 0; i < L; ++i){
        if(isnan(data[i])) continue;
        else if(!anyEqual(i, outcomeIndex)) rem.push_back(data[i]);
        else{
            sample.push_back(data[i]);
        }
    }

    if(sample.empty() || rem.empty()) return NAN;

    if(sample.size() == 1){

        if(rem.size() > 1){
            return (sample.front() - average(rem))/stdev(data);
        }
        else{
            return (sample.front() - rem.front())/stdev(data);
        }

    }

    return (average(sample) - average(rem))/(stdev(rem) + stdev(sample));
}

inline void least_squares_fit(const std::vector<float>& xV, const std::vector<float>& yV,
                              float& slopeOutput, float& interceptOutput){

    float sumX(0.0f), sumY(0.0f), sumXY(0.0f), sumXsq(0.0f), sumDev = 0.0f, avgX = (average(xV));
    unsigned int L = (xV.size() <= yV.size()) ? xV.size() : yV.size(), N = 0;

    for(size_t i = 0; i < L; ++i){
        if(isnan(xV[i]) || isnan(yV[i])) continue;

        sumX += xV[i];
        sumY += yV[i];
        sumXY += (xV[i]*yV[i]);
        sumXsq += pow(xV[i], 2);
        sumDev += pow((xV[i] - avgX), 2);
        ++N;
    }

    slopeOutput = (N*sumXY - sumX*sumY)/(N*sumDev);
    interceptOutput = (sumXsq*sumY - sumX*sumXY)/(N*sumDev);

}

inline bool isValid(const std::vector<float>& V){
    for(auto val : V){
        if(!isnan(val) && !isinf(val)) return true;
    }
    return false;
}

inline float chisq_p(float F, int df){
    if((df < 1) || F < 0.0f) return 0.0f;

    float X = F/2;
    if(df == 2){
        return exp(-X);
    }

    float K = (float(df)/2);

    float Sc = 1.0f/K * pow(X, K) * exp(-X);

    float sum = 1.0f, nom = 1.0f, denom = 1.0f, sK = K;

    for(unsigned int i = 0; i < 200; ++i){
        nom *= X;
        ++sK;
        denom *= sK;
        sum += (nom/denom);
    }

    float p = sum * Sc;
    if(isnan(p) || isinf(p)){
        return 1e-14;
    }

    return 1.0f - p / tgamma(K);
}

template<typename T> double student_t_test(const std::vector<T>& P1,
                             const std::vector<T>& P2,
                             const uint8_t& tail = STATS_TWO_TAILED){

    size_t S1 = validSize(P1), S2 = validSize(P2), V = S1 + S2 - 2;
    if((S1 < 3) || (S2 < 3)) return NAN;

    double SD1 = stdev(P1), SD2 = stdev(P2);
    double Sp = sqrt(((S1-1)*pow(SD1, 2) + (S2-1)*pow(SD2, 2))/(V));
    double t_stat = absolute((average(P1) - average(P2))/(Sp*sqrt(1.0f/S1 + 1.0f/S2)));

    if(isinf(t_stat) || isnan(t_stat)) return NAN;

    boost::math::students_t dist(V);

    double output;
    switch(tail){
        case STATS_UPPER_TAIL:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat));
            if(output < 0.0) return 1.0;
            break;
        }
        case STATS_LOWER_TAIL:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat));
            if(output > 0.0) return 1.0;
            break;
        }
        default:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat))*2;
            break;
        }
    }

    return output;
}

template<typename T> double population_mean_test(const std::vector<T>& sample,
                                                const std::vector<T>& population,
                                                const uint8_t& tail = STATS_TWO_TAILED,
                                                bool bonferroni_correct = true){

    size_t S1 = validSize(sample), S2 = validSize(population), V = S1 + S2 - 2;
    if((S1 < 2) || (S2 < 2)) return NAN;

    double t_stat = abs(average(sample));
    double Sp = sqrt(((S1-1)*pow(stdev(sample), 2) + (S2-1)*pow(stdev(population), 2))/(V)); // Pooled variation

    if((Sp == 0.0) || isinf(Sp) || isnan(Sp)) return NAN;

    boost::math::normal_distribution<double> dist(average(population), Sp);

    double output;
    switch(tail){
        case STATS_UPPER_TAIL:{
            if(t_stat < average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        case STATS_LOWER_TAIL:{
            if(t_stat > average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        default:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat))*2;
            break;
        }
    }

    if(bonferroni_correct){
        output *= V;
        if(output > 1.0) return 1.0;
    }

    return output;

}

template<typename T> double one_sample_test(const T& sample,
                                                const std::vector<T>& population,
                                                const uint8_t& tail = STATS_TWO_TAILED,
                                                bool bonferroni_correct = true){
    if(isnan(sample)) return NAN;
    size_t N = validSize(population);
    double t_stat = abs(sample),
            SD = stdev(population);

    if((SD == 0.0) || isinf(SD) || isnan(SD)) return NAN;
    boost::math::normal_distribution<double> dist(average(population), SD);

    double output;
    switch(tail){
        case STATS_UPPER_TAIL:{
            if(t_stat < average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        case STATS_LOWER_TAIL:{
            if(t_stat > average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        default:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat))*2;
            break;
        }
    }

    if(bonferroni_correct){
        output *= N;
        if(output > 1.0) return 1.0;
    }

    return output;
}

template<typename T> double one_sample_test(const T& sample,
                                                const std::vector<std::vector<T>>& population,
                                                const uint8_t& tail = STATS_TWO_TAILED,
                                                bool bonferroni_correct = true){
    if(isnan(sample)) return NAN;
    size_t N = validSize(population);
    double t_stat = abs(sample),
            SD = stdev(population);

    if((SD == 0.0) || isinf(SD) || isnan(SD)) return NAN;
    boost::math::normal_distribution<double> dist(average(population), SD);

    double output;
    switch(tail){
        case STATS_UPPER_TAIL:{
            if(t_stat < average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        case STATS_LOWER_TAIL:{
            if(t_stat > average(population)) output = 1.0;
            else output = boost::math::cdf(boost::math::complement(dist, t_stat));
            break;
        }
        default:{
            output = boost::math::cdf(boost::math::complement(dist, t_stat))*2;
            break;
        }
    }

    if(bonferroni_correct){
        output *= N;
        if(output > 1.0) return 1.0;
    }

    return output;
}

template<typename T> double one_sample_test(const std::vector<T>& sample,
                                            const std::vector<std::vector<T>>& population,
                                            const uint8_t& tail = STATS_TWO_TAILED,
                                            bool bonferroni_correct = true){
    return one_sample_test(average(sample), population, tail, bonferroni_correct);
}

inline float odds_ratio(const int& O, int OT,
                        int N, int T){

    // O = number of outcomes of interest in experimental group
    // OT = number of outcomes of interest in total
    // N = population size of interest
    // T = total population size

    T -= N;
    OT -= O;
    N -= O;
    T -= OT;

    return ((float)O/OT)/((float)N/T);
}

inline float odds_chisq_test(const int& O, int OT, int N, int T){

    float df = OT-1;
    if(df < 1) return NAN;

    float P = (float)N/T;

    float EO = (float)OT*P;
    float EOT = (float)OT*(1.0f-P);
    float ET = (float)T-(float)N-EOT;
    float EN = (float)N-EO;

    T -= N;
    OT -= O;
    N -= O;
    T -= OT;

    float alpha = pow(((float)O - EO), 2)/EO + pow(((float)OT - EOT), 2)/EOT +
            pow(((float)N - EN), 2)/EN + pow(((float)T - ET), 2)/ET;
    if(isnan(alpha) || isinf(alpha)) return 1.0f;

    boost::math::students_t dist(df);
    return boost::math::cdf(boost::math::complement(dist, alpha))*2;
}

template<typename T> double logFactorial(T N){
    double output = 0.0;
    for(; N > 0; --N){
        output += log10(double(N));
    }
    return output;
}

inline long double hypergeometric_probability(const int& a, const int& b, const int& c, const int& d){
    return pow(10, (logFactorial(a+b) +
            logFactorial(c+d) +
            logFactorial(a+c) +
            logFactorial(b+d) -
                logFactorial(a+b+c+d) -
                logFactorial(a) -
                 logFactorial(b) -
                 logFactorial(c) -
                 logFactorial(d)));
}

inline long double fisher_exact_P(const int& O, int OT, int N, int T,
                                  const bool& bonferroni_correct = false){

    size_t total = T;

    T -= N;
    OT -= O;
    N -= O;
    T -= OT;

    long double p_cutoff = hypergeometric_probability(O, OT, N, T),
            p_value = 0.0L;

    for(int i = 0; i <= total; ++i){
        if((O + OT - i >= 0) && (O + N - i >= 0) && (T - O + i >= 0)){
            long double p = hypergeometric_probability(i, O + OT - i, O + N - i, T - O + i);
            if(p <= p_cutoff) p_value += p;
        }
    }

    if(bonferroni_correct){
        p_value *= total;
        if(p_value > 1.0L) return 1.0L;
    }

    return p_value;

}


// String algorithms

inline unsigned int getStrSize(char* c){
    unsigned int output = 0;
    char* newC = c;
    while((*newC != '\t') && (*newC != '\n') && (*newC != '\0')){
        ++output;
        ++newC;
    }
    newC = c-1;
    while((*newC != '\t') && (*newC != '\n') && (*newC != '\0')){
        ++output;
        --newC;
    }
    return output;
}

inline bool isCharType(const char& c, const std::string& type){
    for(size_t i = 0; i < type.size(); ++i){
        if(type[i] == c) return true;
    }
    return false;
}

inline bool atWord(const unsigned int& index, const std::string& body, const std::string& query){
    unsigned int L = body.size(), S = query.size(), j = 0;
    for(size_t i = index; (i < L) && (j < S); ++i, ++j){
        if(body[i] != query[j]) return false;
    }
    return true;
}

inline constexpr bool isLetter(const char& c){
    return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
}

inline constexpr bool isUpperCase(const char& c){
    return ((c >= 'A') && (c <= 'Z'));
}

inline constexpr bool isLowerCase(const char& c){
    return ((c >= 'a') && (c <= 'z'));
}

inline constexpr bool isNumber(const char& c){
    return ((c >= '0') && (c <= '9'));
}

inline constexpr bool isDelimiter(const char& c){
    return ((c == ' ') || ( c == '\t') || (c == '\n') ||
            (c == ',') || (c == '-'));
}

inline bool check_letter(const char& c, const char& other_c){
    if(isLetter(c)){
        if(c == other_c) return true;
        if(isUpperCase(c) && isLowerCase(other_c)){
            if((c + 32) == other_c) return true;
        }
        else if(isLowerCase(c) && isUpperCase(other_c)){
            if((c - 32) == other_c) return true;
        }
    }

    return false;
}

inline constexpr bool isSpecial(const char& c){
    return(((c < 'A') || (c > 'Z')) &&
           ((c < 'a') || (c > 'z')) &&
           ((c < '0') || (c > '9')) &&
           (c != '-') && (c != '+') &&
           (c != '/'));
}

inline constexpr bool isTypeChar(const char& c){
    return (((c > 31) && (c < 127)) || (c == '\t') || (c == '\n'));
}

inline bool isNumeric(const char& c){
    return ((c > 39) && (c < 58));
}

inline bool isNumeric(char* c){
    char* dataPos = c;

    bool numericCheck = false;
    while((*dataPos == ' ') || (*dataPos == '-') || (*dataPos == '+')) ++dataPos;

    while((*dataPos != '\0') && (*dataPos != '\t') && (*dataPos != '\n')){
        if(!isNumber(*dataPos) && (*dataPos != '.')) return false;
        if(((*dataPos == '-') || (*dataPos == '+')) && ((*(dataPos-1) != 'E') || (*(dataPos-1) != 'e'))) return false;
        if(((*dataPos == 'E') || (*dataPos == 'e')) && (dataPos == c)) return false;

        numericCheck = true;
        ++dataPos;
    }
    return numericCheck;
}

inline bool isNumeric(const std::string& s){
    bool numericCheck = false;
    unsigned int startIndex = 0;
    while((startIndex < s.size()) &&
          ((s[startIndex] == ' ') ||
            (s[startIndex] == '-') || (s[startIndex] == '+'))){
        ++startIndex;
    }

    for(size_t i = startIndex; i < s.size(); ++i){
        if(!isNumber(s[i]) && (s[i] != '.')) return false;
        if(((s[i] == '-') || (s[i] == '+')) && ((s[i-1] != 'E') || (s[i-1] != 'e'))) return false;
        if(((s[i] == 'E') || (s[i] == 'e')) && (i == 0)) return false;

        numericCheck = true;
    }

    return numericCheck;
}

inline std::string sigFigs(const std::string& numeric){
    if((numeric.find('.') >= numeric.size()) ||
       (numeric.find('E') >= numeric.size())) return numeric;

    std::string output = numeric;
    while(output.back() == '0') output.pop_back();
    while(output.back() == '.') output.pop_back();

    return output;
}

inline std::string sigFigs(const float& value){
    std::stringstream str;
    str << value;
    return sigFigs(str.str());
}

template<typename T> std::string strDigits(const T& numeric, const unsigned int& numDecimals){ // Only works with classes with to_string support in std
    std::string numStr = std::to_string(numeric);
    if(!isNumeric(numStr)) return numStr;

    unsigned int index = numStr.find('.');
    if(index > numStr.size()){
        if(numDecimals < 1) return numStr;
        index = numStr.size();
        numStr += '.';
    }
    else{
        if(numDecimals == 0){
            numStr.resize(index);
            return numStr;
        }
        numStr.resize(index + numDecimals + 1);
    }

    size_t L = index + numDecimals + 1;
    while(numStr.size() < L){
        numStr += '0';
    }

    return numStr;

}

inline int hexToInt(const char* c){
    int L = strlen(c);
    if((L < 2) || (L % 2)) return -1;
    int output = 0, d1, d2;

    for(int i = L - 1, j = 0; i > 0; i -= 2, j += 2){
        if((c[i] > 'f') || (c[i - 1] > 'f')) return -1;

        if(isNumber(c[i])){
            d1 = int(c[i]) - 48;
        } else {
            d1 = int(c[i]) - 87;
        }

        if(isNumber(c[i - 1])){
            d2 = int(c[i - 1]) - 48;
        } else {
            d2 = int(c[i - 1]) - 87;
        }

        output += d1*pow(16, j) + d2*pow(16, j+1);

    }

    return output;
}

inline int hexToInt(const std::string& s){
    return hexToInt(s.c_str());
}

inline void operator+=(std::string& text, const float& numeric){
    text += sigFigs(std::to_string(numeric));
}

inline void operator+=(std::string& text, const unsigned int& numeric){
    text += sigFigs(std::to_string(numeric));
}

inline void operator+=(std::string& text, const int& numeric){
    text += sigFigs(std::to_string(numeric));
}

inline std::string operator+(const std::string& text, const float& numeric){
    return text + sigFigs(std::to_string(numeric));
}

inline std::string operator+(const std::string& text, const unsigned int& numeric){
    return text + sigFigs(std::to_string(numeric));
}

inline std::string operator+(const std::string& text, const int& numeric){
    return text + sigFigs(std::to_string(numeric));
}

template<typename T> std::string operator+(const std::string& text, const std::vector<T>& V){
    std::string output = text;
    output += '(';
    for(auto item : V){
        output += item;
        output += ',';
    }
    output.back() = ')';
    return output;
}

inline void to_lowercase(std::string& s){
    size_t L = s.size();
    for(size_t i = 0; i < L; ++i){
        if(isUpperCase(s[i])) s[i] += 32;
    }
}

inline std::string get_lowercase(const std::string& s){
    std::string output = s;
    to_lowercase(output);
    return output;
}

inline void to_uppercase(std::string& s){
    size_t L = s.size();
    for(size_t i = 0; i < L; ++i){
        if(isLowerCase(s[i])) s[i] -= 32;
    }
}

inline std::string get_uppercase(const std::string& s){
    std::string output = s;
    to_uppercase(output);
    return output;
}

inline void capitalize(std::string& s){
    if(isLowerCase(s.front())){
        s.front() -= 32;
    }
}

inline void uncapitalize(std::string& s){
    if(isUpperCase(s.front())){
        s.front() += 32;
    }
}

inline std::string capitalized(std::string s){
    capitalize(s);
    return s;
}

inline std::string non_capitalized(std::string s){
    uncapitalize(s);
    return s;
}

inline bool isWord(const std::string& s){
    for(auto& c : s){
        if(isCharType(c, DELIM_BASIC)){
            return false;
        }
    }
    return true;
}

inline bool sw_align(const std::string& query, const std::string& background,
                     const float& threshold = 0.6f,
                     unsigned int* output = nullptr){
    unsigned int xL = query.size()+1, yL = background.size()+1;
    vMatrix<int> scores(yL, std::vector<int>(xL, 0)),
                    dir(yL, std::vector<int>(xL, 3));

    std::vector<int> assessment(4, 0);

    for(size_t y = 1; y < yL; ++y){
        for(size_t x = 1; x < xL; ++x){
            if(query[x-1] == background[y-1]){
                assessment[0] = scores[y-1][x-1] + 5;
            }
            else{
                assessment[0] = scores[y-1][x-1] - 4;
            }

            if((x > 1) && (dir[y][x-1] == dir[y][x-2])) assessment[1] = scores[y][x-1] - 4; // Gap extension penalty
            else{
                assessment[1] = scores[y][x-1] - 12; // Gap opening penalty
            }

            if((y > 1) && (dir[y-1][x] == dir[y-2][x])){
                assessment[2] = scores[y-1][x] - 4;
            }
            else assessment[2] = scores[y-1][x] - 12;

            dir[y][x] = maxIndex(assessment);
            scores[y][x] = assessment[dir[y][x]];
        }
    }

    int maxScore = max(scores), thresScore = maxScore * threshold;
    if(output != nullptr) *output = maxScore;
    VectorPairU maxCoords;

    for(size_t y = 1; y < yL; ++y){
        for(size_t x = 1; x < xL; ++x){
            if(scores[y][x] > thresScore) maxCoords.emplace_back(x, y);
        }
    }

    std::vector<VectorPairU> coords;

    int stepX = 0, stepY = 0, identical;
    bool walk;
    for(size_t i = 0; i < maxCoords.size(); ++i){

        coords.emplace_back();

        stepX = maxCoords[i].x;
        stepY = maxCoords[i].y;
        identical = 0;

        walk = true;
        while(walk){
            switch(dir[stepY][stepX]){
                case SW_PTR_DIAG:{
                    if(scores[stepY-1][stepX-1] < scores[stepY][stepX]) ++identical;
                    --stepX;
                    --stepY;
                    coords.back().insert(0, stepX, stepY);
                    break;
                }
                case SW_PTR_LEFT:{
                    --stepX;
                    coords.back().insert(0, stepX, UINT_MAX);
                    break;
                }
                case SW_PTR_UP:{
                    --stepY;
                    coords.back().insert(0, UINT_MAX, stepY);
                    break;
                }
                default:{
                    walk = false;
                    break;
                }
            }
        }

    }

    return (float)maxScore > ((float)(background.size())+(float)(query.size()))/2*5*threshold;

}

inline bool cmpShortString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    size_t s1SIZE = focus.size(), s2SIZE = other.size();
    if((s1SIZE < 1) || (s2SIZE < 1)) return false;
    if(!(params & CMP_STR_SIZE_INSENSITIVE) && (s1SIZE != s2SIZE)) return false;

    if(params & CMP_STR_CASE_INSENSITIVE){
        for(size_t i = 0; i < s1SIZE; ++i){
            if((focus[i] < 'A') || (focus[i] > 'Z')) continue;
            focus[i] += 32; // all lowercase
        }
        for(size_t i = 0; i < s2SIZE; ++i){
            if((other[i] < 'A') || (other[i] > 'Z')) continue;
            other[i] += 32;
        }
    }

    bool bOutput = true;

    if(s2SIZE >= s1SIZE){
        unsigned int index = other.find(focus);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(index < s2SIZE){
                if(index > 0) if(!isCharType(other[index-1], DELIM_STANDARD)) bOutput = false;
                if(index + s1SIZE < s2SIZE) if(!isCharType(other[index+s1SIZE], DELIM_STANDARD)) bOutput = false;
            }
            else bOutput = false;
        }
        else bOutput = index < other.size();
    }
    else{
        unsigned int index = focus.find(other);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(index < s1SIZE){
                if(index > 0) if(!isCharType(focus[index-1], DELIM_STANDARD)) bOutput = false;
                if(index + s2SIZE < s1SIZE) if(!isCharType(focus[index+s2SIZE], DELIM_STANDARD)) bOutput = false;
            }
            else bOutput = false;
        }
        else bOutput = index < focus.size();
    }

    return bOutput;
}

inline bool cmpString(std::string focus, std::string other,
                      const BYTE& params = CMP_STR_DEFAULT,
                      const float& threshold = 0.6f){

    if(focus == other)
        return true; // Attempt short-circuit for direct matches
    else if(!params) return false;
    else if(focus.empty() || other.empty()) return false;

    size_t s1SIZE = focus.size(), s2SIZE = other.size();
    if(!(params & CMP_STR_SIZE_INSENSITIVE) && (s1SIZE != s2SIZE)) return false;

    if((s1SIZE < 4) || (s2SIZE < 4)){ // Attempt to prevent false positives from short strings
        return cmpShortString(focus, other, params);
    }

    if(params & CMP_STR_CASE_INSENSITIVE){
        for(size_t i = 0; i < s1SIZE; ++i){
            if((focus[i] < 'A') || (focus[i] > 'Z')) continue;
            focus[i] += 32; // all lowercase
        }
        for(size_t i = 0; i < s2SIZE; ++i){
            if((other[i] < 'A') || (other[i] > 'Z')) continue;
            other[i] += 32;
        }
    }

    if(s2SIZE >= s1SIZE){
        if(other.find(focus) < s2SIZE) return true;
        else if(params & CMP_STR_SW){
            return sw_align(focus, other, threshold);
        }
    }
    else{
        if(focus.find(other) < s1SIZE) return true;
        else if(params & CMP_STR_SW){
            return sw_align(focus, other, threshold);
        }
    }

    return false;
}

inline bool cmpStringToList(const std::string& focus, const std::vector<std::string>& list,
                            const BYTE& params = CMP_STR_DEFAULT, const float& threshold = 0.6f){
    for (auto& str : list){
        if(cmpString(focus, str, params, threshold)) return true;
    }
    return false;
}

inline bool cmpStringToList(const StringVector& query, const StringVector& other,
                            const BYTE& params = CMP_STR_DEFAULT, const float& threshold = 0.6f){
    for(auto& str: query){
        if(cmpStringToList(str, other, params)) return true;
    }
    return false;
}

inline bool cmpStringIncludeList(const std::string& focus, const std::vector<std::string>& list,
                                 const BYTE& params = CMP_STR_DEFAULT,
                                 const float& threshold = 0.6f){
    for (auto& str : list){
        if(!cmpString(focus, str, params)) return false;
    }
    return true;
}

inline unsigned int findShortString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    if(focus.empty() || other.empty()) return UINT_MAX;
    size_t s1SIZE = focus.size(), s2SIZE = other.size();
    if(!(params & CMP_STR_SIZE_INSENSITIVE) && (s1SIZE != s2SIZE)) return UINT_MAX;

    if(params & CMP_STR_CASE_INSENSITIVE){
        for(size_t i = 0; i < s1SIZE; ++i){
            if((focus[i] < 'A') || (focus[i] > 'Z')) continue;
            focus[i] += 32; // all lowercase
        }
        for(size_t i = 0; i < s2SIZE; ++i){
            if((other[i] < 'A') || (other[i] > 'Z')) continue;
            other[i] += 32;
        }
    }

    bool bOutput = true;
    unsigned int index;

    if(s2SIZE >= s1SIZE){
        index = other.find(focus);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(focus.size() == other.size()) bOutput = true;
            else if(index < s2SIZE){
                if(index > 0) if(!isCharType(other[index-1], " ,.;!_/\\+=")) bOutput = false;
                if(index + s1SIZE < s2SIZE) if(!isCharType(other[index+s1SIZE], " ,.;!_/\\+=")) bOutput = false;
            }
            else bOutput = false;
        }
        else bOutput = index < other.size();
    }
    else{
        index = focus.find(other);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(index < s1SIZE){
                if(index > 0) if(!isCharType(focus[index-1], " ,.;!_/\\+=")) bOutput = false;
                if(index + s2SIZE < s1SIZE) if(!isCharType(focus[index+s2SIZE], " ,.;!_/\\+=")) bOutput = false;
            }
            else bOutput = false;
        }
        else bOutput = index < focus.size();
    }

    if(bOutput) return index;
    else return UINT_MAX;
}

inline unsigned int findString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    if(focus == other) return 0; // Attempt short-circuit for direct matches

    if(focus.empty() || other.empty()) return UINT_MAX;
    size_t s1SIZE = focus.size(), s2SIZE = other.size();
    if(!(params & CMP_STR_SIZE_INSENSITIVE) && (s1SIZE != s2SIZE)) return UINT_MAX;

    if((s1SIZE < 4) || (s2SIZE < 4)){ // Attempt to prevent false positives from short strings
        return findShortString(focus, other, params);
    }

    if(params & CMP_STR_CASE_INSENSITIVE){
        for(size_t i = 0; i < s1SIZE; ++i){
            if((focus[i] < 'A') || (focus[i] > 'Z')) continue;
            focus[i] += 32; // all lowercase
        }
        for(size_t i = 0; i < s2SIZE; ++i){
            if((other[i] < 'A') || (other[i] > 'Z')) continue;
            other[i] += 32;
        }
    }

    unsigned int fIndex = focus.find(other);
    if(fIndex < s1SIZE) return fIndex;

    return UINT_MAX;
}

inline size_t findString(const char* query, const char* background){
    size_t S = strlen(query),
            L = strlen(background),
            beginIdx = UINT_MAX,
            matchIdx = 0;

    if(S > L) return UINT_MAX;

    for(size_t i = 0; i < L - S + 1; ++i){
        if(background[i] == query[matchIdx]){
            if(beginIdx == UINT_MAX){
                beginIdx = i;
            }
            ++matchIdx;
        }
        else{
            matchIdx = 0;
            beginIdx = UINT_MAX;
        }

        if(matchIdx == S){
            return beginIdx;
        }
    }

    return UINT_MAX;
}
inline bool checkString(const char* query, const char* background){
    size_t S = strlen(query),
            L = strlen(background),
            matchIdx = 0;

    if(S > L) return false;

    for(size_t i = 0; i < L - S + 1; ++i){
        if(background[i] == query[matchIdx]){
            ++matchIdx;
        }
        else{
            matchIdx = 0;
        }

        if(matchIdx == S){
            return true;
        }
    }

    return false;
}

inline bool ptr_at_string(const char* ptr, const char* query){
    if(!(*ptr) || !(*query)) return false;
    while((*ptr) && (*query)){
        if(*ptr != *query) return false;
        ++ptr;
        ++query;
    }
    return !(*query);
}

inline std::string getBestStringMatch(const std::string& tag,
                                      const std::vector<std::string>& list,
                                      const float& threshold = 0.6f){
    size_t tagSIZE = tag.size();
    if(tagSIZE < 1) return std::string();
    if(list.empty()) return std::string();
    if(list.size() == 1) return list.front();

    unsigned int maxScore = 0, maxIndex = UINT_MAX, cScore = 0, cIndex = 0;
    for(auto& term : list){
        if(sw_align(tag, term, threshold, &cScore)){
            if(cScore > maxScore){
                maxScore = cScore;
                maxIndex = cIndex;
            }
        }
        ++cIndex;
    }

    if(maxIndex == UINT_MAX) return std::string();
    return list[maxIndex];
}

inline unsigned int charMatchNum(const std::string& query, const std::string& other){ // Case insensitive match comparison
    size_t L1 = query.size(), L2 = other.size(), minL = L1 > L2 ? L2 : L1;
    unsigned int matchNum, bestMatchNum = 0;

    for(size_t i = 0; i < L1; ++i){
        for(size_t j = 0; j < L2; ++j){

            matchNum = 0;

            if(isLowerCase(query[i])){ // Find start match position
                while(((i < L1) && (j < L2)) && (query[i] != other[j])){
                    if(query[i] == (other[j] + 32)) break;
                    ++j;
                }
            }
            else if(isUpperCase(query[i])){
                while(((i < L1) && (j < L2)) && (query[i] != other[j])){
                    if(query[i] == (other[j] - 32)) break;
                    ++j;
                }
            }
            else{
                while(((i < L1) && (j < L2)) && (query[i] != other[j])){
                    ++j;
                }
            }

            if(j >= L2) break;

            unsigned int startPos = i;

            while((startPos < L1) && (j < L2) && (matchNum < minL)){
                if(query[startPos] == other[j]){
                    ++startPos; ++j; ++matchNum;
                }
                else if(isLowerCase(query[startPos])){
                    if(query[startPos] == (other[j] + 32)){
                        ++startPos; ++j; ++matchNum;
                    }
                    else{
                        ++startPos; ++j;
                    }
                }
                else if(isUpperCase(query[startPos])){
                    if(query[startPos] == (other[j] - 32)){
                        ++startPos; ++j; ++matchNum;
                    }
                    else{
                        ++startPos; ++j;
                    }
                }
                else{
                    ++startPos; ++j;
                }
            }

            if(matchNum > bestMatchNum) bestMatchNum = matchNum;
        }
    }

    return bestMatchNum;
}

inline unsigned int getBestStringMatchIndex(const std::string& tag,
                                      const std::vector<std::string>& list){
    size_t tagSIZE = tag.size();
    if(tagSIZE < 1) return UINT_MAX;
    if(list.size() < 1) return UINT_MAX;
    if(list.size() == 1) return 0U;

    unsigned int closeSizeIndex = 0, cIndex = 0, minSizeDiff = tagSIZE;
    for(auto& term : list){
        if(term.size() == tagSIZE) return cIndex;
        else{
            unsigned int matchNum = charMatchNum(tag, term);
            if(tagSIZE > matchNum){
                if((tagSIZE - matchNum) < minSizeDiff){
                    minSizeDiff = tagSIZE - matchNum;
                    closeSizeIndex = cIndex;
                }
            }
            else{
                if((matchNum - tagSIZE) < minSizeDiff){
                    minSizeDiff = matchNum - tagSIZE;
                    closeSizeIndex = cIndex;
                }
            }
        }
        ++cIndex;
    }
    return closeSizeIndex;
}

inline bool cmpString(char* focus, char* other, float threshold){
    size_t s1SIZE = 0, s2SIZE = 0, matchNum = 0;
    char* f1 = focus, *f2 = other;
    while(!((*f1 == '\t') || (*f1 == '\n') || (*f1 == '\0'))){
        ++f1; ++s1SIZE;
    }
    char* f1END = f1;
    while(!((*f2 == '\t') || (*f2 == '\n') || (*f2 == '\0'))){
        ++f2; ++s2SIZE;
    }
    char* f2END = f2;
    f1 = focus; f2 = other;

    if(s1SIZE != s2SIZE){

        if((s1SIZE < 3) || (s2SIZE < 3)) return false; // Attempt to prevent false positives from common words
        bool lenient = threshold > 0.0f;

        if(lenient){ // Allow adjustable leniency for interpretation
            if(s1SIZE < (float)s2SIZE*(1.0f-threshold)) return false;
            else if (s2SIZE < (float)s1SIZE*(1.0f-threshold)) return false;
        }

    }

    unsigned int i = 0;
    if(s1SIZE >= s2SIZE){
        while(f1 != f1END){
            if(*f2 == *f1){
                ++matchNum;
                if(matchNum == s2SIZE) return true;
                ++f2; ++f1;
            }
            else if(isUpperCase(*f2)){
                if(*f2 == *f1 - 32){
                    ++matchNum;
                    if(matchNum == s2SIZE) return true;
                    ++f2; ++f1;
                }
                else{
                    ++f1;
                    matchNum = 0;
                    if(i + s2SIZE - matchNum >= s1SIZE) return false;
                }
            }
            else if(isLowerCase(*f2)){
                if(*f2 == *f1 + 32){
                    ++matchNum;
                    if(matchNum == s2SIZE) return true;
                    ++f2; ++f1;
                }
                else{
                    ++f1;
                    matchNum = 0;
                    if(i + s2SIZE - matchNum >= s1SIZE) return false;
                }
            }
            else{
                ++f1;
                matchNum = 0;
                if(i + s2SIZE - matchNum >= s1SIZE) return false;
            }
            ++i;
        }
    }
    else{
        while(f2 != f2END){
            if(*f2 == *f1){
                ++matchNum;
                if(matchNum == s1SIZE) return true;
                ++f2; ++f1;
            }
            else if(isUpperCase(*f1)){
                if(*f1 == *f2 - 32){
                    ++matchNum;
                    if(matchNum == s1SIZE) return true;
                    ++f2; ++f1;
                }
                else{
                    ++f2;
                    matchNum = 0;
                    if(i + s1SIZE - matchNum >= s2SIZE) return false;
                }
            }
            else if(isLowerCase(*f1)){
                if(*f1 == *f2 + 32){
                    ++matchNum;
                    if(matchNum == s1SIZE) return true;
                    ++f2; ++f1;
                }
                else{
                    ++f2;
                    matchNum = 0;
                    if(i + s1SIZE - matchNum >= s2SIZE) return false;
                }
            }
            else{
                ++f2;
                matchNum = 0;
                if(i + s1SIZE - matchNum >= s2SIZE) return false;
            }
            ++i;
        }
    }

    return false;
}

inline std::string getMatchingTag(const std::string& tag, const std::vector<std::string>& list, const BYTE& params = CMP_STR_DEFAULT){
    std::vector<std::string> matches;
    for(auto& item : list){
        if(tag == item) return tag;
        else if(cmpString(tag, item, params)) matches.push_back(item);
    }
    if(matches.size() > 1){
        return getBestStringMatch(tag, matches);
    }
    else if(matches.size() == 1) return matches.front();
    else return std::string();
}

inline unsigned int getMatchingIndex(const std::string& focus, const std::vector<std::string>& list, const BYTE& params = CMP_STR_DEFAULT){
    unsigned int matchIndex = 0;
    std::vector<std::string> matches;
    std::vector<unsigned int> matchIndices;
    for(auto& item : list){
        if(focus == item) return matchIndex;
        if(cmpString(focus, item, params)){
            matches.push_back(item);
            matchIndices.push_back(matchIndex);
        }
        ++matchIndex;
    }
    if(matches.size() > 1) return matchIndices[getBestStringMatchIndex(focus, matches)];
    else if(matches.size() == 1) return matchIndices.front();
    else return UINT_MAX;
}

template<typename T> T* matching_item(const std::string& query,
                                      std::map<std::string, T>& M,
                                      const unsigned char& params = CMP_STR_DEFAULT){
    if(M.empty()) return nullptr;

    std::vector<std::string> tags; tags.reserve(M.size());

    auto endIT = M.end();
    for(auto IT = M.begin(); IT != endIT; ++IT){
        tags.emplace_back(IT->first);
    }

    size_t matchIndex = getMatchingIndex(query, tags, params);
    if(matchIndex != UINT_MAX){
        return &M[tags[matchIndex]];
    }

    return nullptr;
}

inline bool removeStrings(StringVector& prompt, const StringVector& strings){
    for(size_t i = 0; i < prompt.size();){
        if(cmpStringToList(prompt[i], strings)){
            prompt.erase(prompt.begin() + i);
        }
        else ++i;
    }
    return prompt.size() > 0;
}

inline std::string concatenateColumn(const StringVector& info){
    std::string output;
    for(size_t i = 0; i < info.size(); ++i){
        output += info[i];
        if(i < info.size()-1) output += '\n';
    }
    return output;
}

template<typename T> std::string concatenateString(const std::vector<T>& input, const std::string& delim, const std::string& terminator = ""){

    std::stringstream oss;
    size_t L = input.size();
    unsigned int i = 0;
    for(auto item : input){
        oss << item;
        if(terminator.size() > 0){
            if(L > 1){
                if(i == L-2) oss << (terminator);
                else if(i < L-1) oss << delim;
            }
        }
        else{
            if(i < L-1) oss << delim;
        }
        ++i;
    }
    return oss.str();
}

inline void concatenateString(const std::vector<std::string>& input, std::string& output,
                              const char delim = ' '){
    size_t L = input.size();
    unsigned int i = 0;
    for(auto str : input){
        output += str;
        if(i < L-1) output += delim;
        ++i;
    }
}

inline unsigned int nline(const std::string& str){
    unsigned int output = 1;
    for(size_t i = 0; i < str.size(); ++i){
        if((str[i] == '\n') && (i < str.size() - 1)) ++output;
    }
    return output;
}

inline unsigned int line_begin(const std::string& str, const unsigned int& index){

    if(index >= nline(str)) return UINT_MAX;

    if(index == 0) return 0;

    size_t i = 0, cLine = 0;

    while((i < str.size()) && (cLine < index)){
        if(str[i] == '\n'){
            ++cLine;
            if(cLine == index){
                if(i < str.size() - 1) return i + 1;
                else return UINT_MAX;
            }
        }
        ++i;
    }

    return UINT_MAX;
}

inline unsigned int line_end(const std::string& str, const unsigned int& index){

    if(index >= nline(str)) return UINT_MAX;

    size_t i = 0, cLine = 0;

    while((i < str.size()) && (cLine <= index)){
        if(str[i] == '\n'){
            if(cLine == index){
                return i;
            }
            ++cLine;
        }
        ++i;
    }

    return UINT_MAX;
}

inline unsigned int ncol(const std::string& str, const std::string& delim = "\t"){
    unsigned int output = 0, rowLength = 1;

    for(const auto& c : str){
        if(c == '\n'){
            if(rowLength > output){
                output = rowLength;
            }
            rowLength = 1;
        }
        else if(isCharType(c, delim)) ++rowLength;
    }

    return output;
}

inline unsigned int nrow(const std::string& str){ return nline(str); }

inline void concatenate_lateral(std::string& left, const std::string& right,
                                       const unsigned int& spacing = 1, const std::string& delim = "\t"){

    if(right.empty() || delim.empty()) return;
    if(left.empty()){
        left = right;
        return;
    }

    size_t L1 = nline(left), L2 = nline(right), L = L1 > L2 ? L1 : L2,
        x = ncol(left, delim), rowLength = 1, cLine = 0;

    for(size_t i = 0; cLine < L; ++i){
        if(i >= left.size()){

            if(cLine < L2){
                while(rowLength < x + spacing){
                    left.insert(left.end(), delim.front());
                    ++i;
                    ++rowLength;
                }
            }
            else{
                while(rowLength < x){
                    left.insert(left.end(), delim.front());
                    ++i;
                    ++rowLength;
                }
            }

            left.insert(left.end(),
                            right.begin() + line_begin(right, cLine),
                            right.begin() + line_end(right, cLine));

            i += line_end(right, cLine) - line_begin(right, cLine);

            left.insert(left.end(), '\n');

            ++i;
            ++cLine;
            rowLength = 1;
        }
        else if(left[i] == '\n'){
            if(cLine < L2){
                while(rowLength < x + spacing){
                    left.insert(left.begin() + i, delim.front());
                    ++i;
                    ++rowLength;
                }
            }
            else{
                while(rowLength < x){
                    left.insert(left.begin() + i, delim.front());
                    ++i;
                    ++rowLength;
                }
            }

            if(cLine < L2){

                left.insert(left.begin() + i,
                            right.begin() + line_begin(right, cLine),
                            right.begin() + line_end(right, cLine));

                i += line_end(right, cLine) - line_begin(right, cLine);

            }
            ++cLine;
            rowLength = 1;
        }
        else if(isCharType(left[i], delim)){
            ++rowLength;
        }
    }

}

inline void trimSpaces(std::string& tag){
    while((tag.size() > 0) && (tag.front() == ' ')) tag.erase(tag.begin());
    while((tag.size() > 0) && (tag.back() == ' ')) tag.pop_back();
}

inline void trim(std::string& str, const std::string& delim = DELIM_BASIC){
    while((str.size() > 0) && isCharType(str.back(), delim)) str.pop_back();
    while((str.size() > 0) && isCharType(str.front(), delim)) str.erase(str.begin());
}

inline void trim_all(std::string& str){
    while((str.size() > 0) && !isLetter(str.back()) && !isNumber(str.back())) str.pop_back();
    while((str.size() > 0) && !isLetter(str.front()) && !isNumber(str.front())) str.pop_back();
}

inline bool shiftTextIndex(unsigned int& index, const std::string& text,
                           const unsigned int& distance = 1, const bool forward = true){

    size_t i, L = text.size();
    if(forward){
        if(index < L-1){
            for(i = 0; (i < distance) && (index < L); ++i){
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index < L)){ // Shift to end of word if not already so
                    ++index;
                }
                while(isCharType(text[index], " ,;._+\\/?:\"") && (index < L)){ // Shift past punctuation
                    ++index;
                }
            }
        }
    }
    else{
        if(index > 0){
            for(i = 0; (i < distance) && (index > 0); ++i){
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index > 0)){ // Shift to beginning of word if not already so
                    --index;
                }
                while(isCharType(text[index], " ,;._+\\/?:\"") && (index > 0)){ // Shift past punctuation
                    --index;
                }
                while(!isCharType(text[index], " ,;._+\\/?:\"") && (index > 0)){ // Shift to beginning of next index word
                    --index;
                }
            }
        }
    }

    return i == distance;
}

inline void processString(std::string& s){
    if(s.size() < 1) return;
    while((s.front() == ' ') || (s.front() == '"')) s.erase(s.begin());
    if(s.size() < 1) return;
    while((s.back() == ' ') || (s.back() == '"')) s.pop_back();
}

inline void splitString(const std::string& input, std::vector<std::string>& output,
                        const std::string& delim = DELIM_BASIC){
    size_t L = input.size();
    unsigned int lastPos = 0;
    if(L < 3) return;
    const std::string::const_iterator delimEND = delim.end();

    for(size_t i = 0; i < L; ++i){
        for(std::string::const_iterator delimIT = delim.begin();
            delimIT != delimEND; ++delimIT){
                if(*delimIT == input[i]){
                        output.emplace_back();
                        output.back().assign(input, lastPos, i-lastPos);
                        trim(output.back(), delim);
                        if(output.back().size() < 1) output.pop_back();
                        lastPos = i+1;
                }
            }
    }
    if(lastPos < L){
        output.emplace_back();
        output.back().assign(input, lastPos, L-lastPos);
        processString(output.back());
    }
}

inline bool splitString(const std::string input, StringVector& output, const StringVector& delim,
                        const std::string& splitTrim = DELIM_BASIC, const BYTE& params = CMP_STR_DEFAULT){

    unsigned int foundIndex = UINT_MAX, endIndex = 0, lastIndex = 0;

    std::vector<unsigned int> foundIndices;

    for(auto& str : delim){
        foundIndex = findString(input, str, params);
        if(foundIndex != UINT_MAX) foundIndices.push_back(foundIndex);
    }

    rmDuplicates(foundIndices);
    order(foundIndices);

    for(auto& idx : foundIndices){
        if(idx != lastIndex){
            endIndex = idx-1;
            while(isCharType(input[endIndex], DELIM_STANDARD)) --endIndex;
            output.emplace_back();
            output.back().assign(input.begin() + lastIndex, input.begin() + endIndex+1);
            lastIndex = idx;
        }
        while((lastIndex < input.size()) && !isCharType(input[lastIndex], DELIM_STANDARD)) ++lastIndex;
        while((lastIndex < input.size()) && isCharType(input[lastIndex], DELIM_STANDARD)) ++lastIndex; // Shift to next word
    }

    if((foundIndices.size() > 0) && (lastIndex < input.size())){
        output.emplace_back();
        output.back().assign(input.begin() + lastIndex, input.end());
    }

    return foundIndices.size() > 0;

}

// Number manipulation

inline float getFirstDigit(const float& number){
    return floor(number/pow(10.0f, floor(log10(number))));
}

inline float placeDecimal(const float& number, int position = 0){
    return number/pow(10.0f, floor(log10(number)) - position);
}

// Time

inline float getDuration(const std::chrono::high_resolution_clock::time_point& time_point){
    return std::chrono::duration<float>(TIME_NOW - time_point).count();
}

}



#endif // BIOALGORITHM_HPP
