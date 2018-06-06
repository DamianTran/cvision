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

#include <mingw.thread.h>
#include <mingw.mutex.h>
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

#define DELIM_NONE                      ""
#define DELIM_BASIC                     " ,:;\t\n"
#define DELIM_STANDARD                  " ,.?\\\"/\t\n!:;&=#"
#define DELIM_CODE                      "\"\t\n.;:&$+=#<>{}[]"
#define DELIM_ALL                       " ,.?\\\"\'/+-=\t\n*~!_:;&#()[]{}<>"

#define CMP_STR_DEFAULT 0b11111111

#define PI                          3.14159265358979323846264338327950288419716939937510
#define MIN_PRECISION               1e-16f
#define SQRT2                       1.4142135623730950f

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

template<typename T> std::ostream& operator<<(std::ostream& output, const std::vector<T>& V){
    size_t L = V.size();
    output << '[';
    for(size_t i = 0; i < L; ++i){
        output << V[i];
        if((L > 1) & (i < L - 1)) output << ',';
    }
    std::cout << ']';
    return output;
}

// Input/Output

template<typename T> void vwrite(const std::vector<T>& V, FILE* outFile){
    size_t vSIZE = V.size();
    fwrite(&vSIZE, sizeof(unsigned int), 1, outFile);
    fwrite(V.data(), vSIZE, sizeof(T), outFile);
}

inline void fwrite(const std::string& s, FILE* outFILE){
    size_t sSIZE = s.size()+1;
    fwrite(&sSIZE, 1, sizeof(size_t), outFILE);
    fwrite(s.c_str(), sSIZE, sizeof(char), outFILE);
}

inline void vwrite(const StringVector& V, FILE* outFILE){
    size_t vSIZE = V.size();
    fwrite(&vSIZE, 1, sizeof(unsigned int), outFILE);
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
    delete(newChar);
}

template<typename T> void vread(std::vector<T>& output, FILE* inFile){
    size_t vSIZE;
    fread(&vSIZE, sizeof(unsigned int), 1, inFile);
    output.clear();
    if(vSIZE < 1) return;

    output.reserve(vSIZE);
    T* tmp = new T[vSIZE];
    fread(tmp, vSIZE, sizeof(T), inFile);
    output.assign(tmp, tmp + vSIZE);
    delete(tmp);
}

inline void vread(StringVector& output, FILE* inFILE){
    size_t vSIZE;
    output.clear();
    fread(&vSIZE, 1, sizeof(unsigned int), inFILE);

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
inline void writeString(const std::string& s, FILE* output){
    writeData(s.size(), output);
    fwrite(s.c_str(), 1, s.size(), output);
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
    delete(inputData);
}

inline void readString(std::string& output, FILE* inFILE){
    size_t inSIZE; readData(inSIZE, inFILE);
    char* inputData = new char[inSIZE+1];
    inputData[inSIZE] = '\0';
    fread(inputData, 1, inSIZE, inFILE);
    output = inputData;
    delete(inputData);
}

inline void readVector(std::vector<std::string>& output, FILE* inFILE){
    size_t inSIZE; readData(inSIZE, inFILE);
    output.reserve(output.size() + inSIZE);
    for(size_t i = 0; i < inSIZE; ++i){
        output.emplace_back();
        readString(output.back(), inFILE);
    }
}

inline void writeVector(const std::vector<std::string>& V, FILE* output){
    writeData(V.size(), output);
    const c_itr<std::string> itEND = V.end();
    for(c_itr<std::string> it = V.begin(); it != itEND; ++it){
        writeString(*it, output);
    }
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
        if(dataClass = getTypeID<char>()){
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

    inline void write(FILE* outFILE){
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
    template<typename T> constexpr T getValue() const{
        if(dataSize < 1) return T();
        return *((T*)data);
    }
    template<typename T> constexpr void putValue(T& item){
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

class GenDataStruct{
protected:

    StringVector tags;
    std::vector<GenDataVar> data;

public:

    template<typename T> void addEntry(const std::string& tag, const T& newData){
        tags.push_back(tag);
        data.emplace_back();
        data.back() = newData;
    }
    inline bool entryExists(const std::string& tag){
        for(auto& t : tags){
            if(t == tag) return true;
        }
        return false;
    }

    inline void setEntries(const StringVector& entries){
        tags = entries;
        data.resize(entries.size());
    }

    inline bool save(const std::string& filename){
        if(!access(filename.c_str(), X_OK)){
            if(access(filename.c_str(), W_OK)) return false;
        }

        FILE* outFILE = fopen(filename.c_str(), "wb");
        write(outFILE);
        fclose(outFILE);

        return true;
    }

    inline void write(FILE* outFILE){
        size_t L = data.size();
        fwrite(&L, 1, sizeof(unsigned int), outFILE);
        for(size_t i = 0; i < L; ++i){
            fwrite(tags[i], outFILE);
            data[i].write(outFILE);
        }
    }

    bool load(const std::string& filename){
        if(access(filename.c_str(), X_OK | R_OK)) return false;

        FILE* inFILE = fopen(filename.c_str(), "rb");
        read(inFILE);
        fclose(inFILE);

        return true;
    }

    inline void read(FILE* inFILE){
        size_t L, matchIndex, j = 0;
        fread(&L, 1, sizeof(unsigned int), inFILE);
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
struct vector2{
    T x, y;
    friend std::ostream& operator<<(std::ostream& output, const vector2<T>& other){
        output << other.x << '\t' << other.y;
        return output;
    }

    inline bool operator==(const vector2<T>& other){
        return ((this->x == other.x) && (this->y == other->y));
    }
    inline bool operator!=(const vector2<T>& other){
        return ((this->x != other.x) || (this->y != other->y));
    }

    inline T& in() { return x; }
    inline T& out() { return y; }
    vector2(){ }
    vector2(T x, T y):x(x), y(y){ }
    vector2(std::vector<T>& xy): x(xy[0]), y(xy[1]){ }
};

template<typename T> void fwrite(const vector2<T>& coords, FILE* outFILE){
    fwrite(&coords.x, 1, sizeof(T), outFILE);
    fwrite(&coords.y, 1, sizeof(T), outFILE);
}

template<typename T> void fread(const vector2<T>& coords, FILE* inFILE){
    fread(&coords.x, 1, sizeof(T), inFILE);
    fread(&coords.y, 1, sizeof(T), inFILE);
}

template<class T>
struct VectorPair{
    std::vector<T> x, y;
    inline unsigned int size() const{ return x.size() < y.size() ? x.size() : y.size(); }
    inline void push_back(const vector2<T>& newPair){ x.push_back(newPair.x); y.push_back(newPair.y); }
    inline void push_back(const T& newX, const T& newY){ x.push_back(newX); y.push_back(newY); }
    inline void emplace_back(const vector2<T>& newPair){ x.emplace_back(newPair.x); y.emplace_back(newPair.y); }
    inline void emplace_back(const T& newX, const T& newY){ x.emplace_back(newX); y.emplace_back(newY); }
    inline void erase(const unsigned int index){ x.erase(x.begin() + index); y.erase(y.begin() + index); }
    inline vector2<T> operator[](const unsigned int index) const{ return vector2<T>(x[index], y[index]); }
    inline vector2<T> back() const{ return vector2<T>(x.back(), y.back()); }
    inline vector2<T> front() const{ return vector2<T>(x.front(), y.front()); }
    inline void clear(){ x.clear(); y.clear(); }
    inline void insert(const unsigned int& index, const T& newX, const T& newY){
        x.insert(x.begin() + index, newX);
        y.insert(y.begin() + index, newY);
    }

    inline void remove(const vector2<T>& ref){
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
    return ((T) std::rand()*(f-i) / (RAND_MAX)+i);
}

inline bool randSwitch(const float& threshold = 0.5f){
    return rand(0.0f,1.0f) > threshold;
}

template<class T1, class T2>
class VariableMonitor{
protected:
    BYTE monitorState; // State of monitoring of response
    std::thread* monitorThread;
    float updateLatency;

    bool monitoring;

    vMatrix<float>* recordOutput;

    T1* variable;
    const T2* response;

    T1 lastVariableValue,
        variableIncrement,
        lastVariableIncrement,
        rangeMin, rangeMax;

    T2 lastResponseValue,
        lastResponseRate;

public:

    inline void resetVariableStates(){
        if(variable != nullptr){
            lastVariableValue = variable;
            variableIncrement = *variable*0.001f;
        }
        else{
            lastVariableValue = 0.0f;
            variableIncrement = 0.0f;
        }

        if(response != nullptr){
            lastResponseValue = *response;
        }
        else{
            lastResponseValue = 0.0f;
        }

        lastResponseRate = 0.0f;
    }

    inline void setMonitorState(const BYTE& newState){ monitorState = newState; }
    inline void setMonitor(T1& variable, const T2& response, const BYTE& monitorState){
        this->variable = &variable;
        this->response = &response;
        this->monitorState = monitorState;
        resetVariableStates();
    }

    inline void setMonitorVariable(T1& newVariable){
        this->variable = &newVariable;
        resetVariableStates();
    }
    inline void setMonitorVariableRange(const T1& newMin, const T1& newMax);
    inline void setResponseVariable(T2& newResponse){
        this->response = &newResponse;
        resetVariableStates();
    }

    inline void setVariableIncrement(const T1& newIncrement){ variableIncrement = newIncrement; }

    void update(){

        float responseRate = *response - lastResponseValue;

        if(responseRate == 0.0f){ // No change in response
            lastResponseRate = 0.0f;
            return;
        }

        if(monitorState & MONITOR_STATE_MAX){
            if(*variable > lastVariableValue){
                lastVariableValue = *variable;
                if(responseRate > lastResponseRate){
                    if(variableIncrement > lastVariableIncrement){ // Update increment by 1% if different than last
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }

                    *variable += variableIncrement;
                }
                else if(responseRate < lastResponseRate){
                    if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    *variable -= variableIncrement;
                }
            }
            else if(*variable < lastVariableValue){
                lastVariableValue = *variable;
                if(responseRate < lastResponseRate){
                    if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    *variable += variableIncrement;
                }
                else if(responseRate > lastResponseRate){
                    if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    *variable -= variableIncrement;
                }
            }
            else if(monitorState & MONITOR_STATE_ACTIVE){ // Randomly peturb to prevent stagnancy
                lastVariableValue = *variable;
                if(rand(0.0f, 1.0f) > 0.5f){
                    *variable += variableIncrement;
                }
                else{
                    *variable -= variableIncrement;
                }
            }
        }
        if(monitorState & MONITOR_STATE_MIN){
            if(*variable < lastVariableValue){
                lastVariableValue = *variable;
                if(responseRate > lastResponseRate){
                    if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    if(*variable + variableIncrement < rangeMax)
                        *variable += variableIncrement;
                }
                else if(responseRate < lastResponseRate){
                    if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    if(*variable - variableIncrement > rangeMin)
                        *variable -= variableIncrement;
                }
            }
            else if(*variable > lastVariableValue){
                lastVariableValue = *variable;
                if(responseRate < lastResponseRate){
                    if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    if(*variable + variableIncrement < rangeMax)
                        *variable += variableIncrement;
                }
                else if(responseRate > lastResponseRate){
                    if(variableIncrement < lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement += variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(variableIncrement > lastVariableIncrement){
                        lastVariableIncrement = variableIncrement;
                        variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                    }
                    else if(monitorState & MONITOR_STATE_ACTIVE){
                        lastVariableIncrement = variableIncrement;
                        if(rand(0.0f, 1.0f) > 0.5f){
                            variableIncrement += variableIncrement*abs(responseRate)/(*response);
                        }
                        else{
                            variableIncrement -= variableIncrement*abs(responseRate)/(*response);
                        }
                    }
                    if(*variable - variableIncrement > rangeMin)
                        *variable -= variableIncrement;
                }
            }
            else if(monitorState & MONITOR_STATE_ACTIVE){
                lastVariableValue = *variable;
                if(rand(0.0f, 1.0f) > 0.5f){
                    if(*variable + variableIncrement < rangeMax){
                        *variable += variableIncrement;
                    }
                }
                else{
                    if(*variable - variableIncrement > rangeMin)
                        *variable -= variableIncrement;
                }
            }
        }

        lastResponseValue = *response;
        lastResponseRate = responseRate;

    }

    bool beginMonitor(){
        if((monitorThread == nullptr)
           && (variable != nullptr)
            && (response != nullptr)){
                monitoring = true;
                monitorThread = new std::thread([&](){
                    std::chrono::duration<float> latency(updateLatency);
                    while(monitoring){
                        update();
                        std::this_thread::sleep_for(latency);
                    }
                });
                return true;
        }
        else{
            if(monitorState & MONITOR_STATE_VERBOSE)
                std::cout << ">> Could not begin variable monitoring\n";
            monitoring = false;
            return false;
        }
    }

    bool stopMonitor(){
        if(monitorThread != nullptr){
            monitorThread->join();
            if(monitorState & MONITOR_STATE_VERBOSE)
                std::cout << ">> Variable monitor stopped\n";
            monitoring = false;
            return true;
        }
        else{
            if(monitorState & MONITOR_STATE_VERBOSE)
                std::cout << ">> Variable monitor is not active\n";
            monitoring = false;
            return false;
        }
    }

    VariableMonitor():
        monitorState(MONITOR_STATE_NONE),
        monitorThread(nullptr),
        updateLatency(1.0f),
        monitoring(false),
        variable(nullptr),
        response(nullptr),
        lastVariableValue(0),
        variableIncrement(0),
        lastVariableIncrement(0),
        lastResponseValue(0),
        lastResponseRate(0),
        rangeMin(0),
        rangeMax(0)
        { }
    VariableMonitor(T1& variable, const T2& response,
                    const T1& rangeMin = -INFINITY, const T1& rangeMax = INFINITY,
                    const BYTE& monitorState = MONITOR_STATE_NONE,
                        const float& updateLatency = 1.0f):
        monitorState(monitorState),
        monitorThread(nullptr),
        updateLatency(updateLatency),
        monitoring(false),
        variable(&variable),
        response(&response),
        lastVariableValue(variable),
        variableIncrement(abs(0.001f*variable)),
        lastVariableIncrement(this->variableIncrement),
        lastResponseValue(response),
        lastResponseRate(0),
        rangeMin(-INFINITY),
        rangeMax(INFINITY){
            beginMonitor();
        }

    ~VariableMonitor(){ // Force stop
        monitoring = false;
        if(monitorThread != nullptr) monitorThread->join();
    }

};

// Functions

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

template<typename T> void printColumn(const std::vector<T>& V, const unsigned int indent = 0){
    for(auto it : V){
        size_t i = 0;
        while(i < indent){
            std::cout << '\t';
            ++i;
        }
        std::cout << it << '\n';
    }
}

struct targetList{
    std::vector<std::vector<unsigned int>> targetMatrix;
    std::vector<std::string> tags;

    friend std::ostream& operator<<(std::ostream& output, targetList& item){
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

template<typename T> size_t totalSize(const std::vector<std::vector<T>>& matrix){
    size_t totalSIZE = 0;
    if(matrix.size() < 1) return totalSIZE;
    const typename std::vector<std::vector<T>>::const_iterator itEND = matrix.end();
    for(typename std::vector<std::vector<T>>::const_iterator it = matrix.begin(); it != itEND; ++it){
        totalSIZE += it->size();
    }
    return totalSIZE;
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

template<typename T, typename N> bool anyEqual(const T& item, const std::vector<N> &V){
    for(auto x : V){
        if(x == item) return true;
    }
    return false;
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
    unsigned int index = 0;
    for(auto it : V){
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

// Vector selection

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
    output.clear(); output.reserve(length);
    if(repeat){
        for(size_t i = 0; i < length; ++i){
            output.push_back(rand<T>(ri, rf));
        }
        return;
    }

    for(size_t i = 0; i < length; ++i){
        T randValue = rand<T>(ri, rf);
        while(anyEqual(randValue, output)){
            randValue = rand<T>(ri, rf);
        }
        output.push_back(randValue);
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
    std::vector<unsigned int> randIndex = vrand(size_t(0), L-1, length, false);
    output.clear(); output.reserve(L);

    for(size_t i = 0; i < length; ++i){
        output.push_back(V[randIndex[i]]);
    }
    return output;
}

template<typename T> void randomize(std::vector<T>& V){
    size_t L = V.size();
    std::vector<unsigned int> newIndex; vrand(newIndex, 0U, L-1, L, false);
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

template<typename T> std::ostream vstream(std::ostream& output, std::vector<T> &V){
    for(size_t i = 0; i < V.size(); ++i){
        output << V[i];
        if((V.size() > 1) && (i < V.size() - 1)){
            output << ',';
        }
    }
    output << ']';
    return output;
}

template<typename T> unsigned int maxSizeIndex(std::vector<std::vector<T>> matrix){
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
    DataSummary(std::vector<T> data){
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
    T output(0);
    const c_itr<T> itEND = V.end();
    for(c_itr<T> it = V.begin(); it != itEND; ++it){
        if(isnan(*it) || isinf(*it)) continue;
        output += *it;
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
    T output = 0;
    unsigned int N = 0;
    typename std::vector<T>::const_iterator itEND = V.end();
    for(typename std::vector<T>::const_iterator it = V.begin(); it != itEND; ++it){
        if(isnan(*it) || isinf(*it)) continue;
        output += pow(*it - vMean, 2);
        ++N;
    }
    return output/N;
}

template<typename T> T stdev(const std::vector<std::vector<T>>& V){
    T vMean = average(V);
    T output = 0;
    unsigned int tSIZE = 0;
    for(auto& row : V){
        for(auto& col : row){
            if(isnan(col)) continue;
            output += pow(col - vMean, 2);
            ++tSIZE;
        }
    }
    return output/tSIZE;
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

// Coordinate functions

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
    if(L < 1) return NAN;

    std::vector<float> sample, rem;
    unsigned int N = 0;

    for(size_t i = 0; i < L; ++i){
        if(isnan(data[i])) continue;
        else if(!anyEqual(i, outcomeIndex)) rem.push_back(data[i]);
        else{
            sample.push_back(data[i]);
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

inline float student_t_test(const std::vector<float>& P1,
                             const std::vector<float>& P2){
    size_t S1 = P1.size(), S2 = P2.size(), V = S1 + S2 - 2;
    if((S1 < 3) || (S2 < 3)) return NAN;

    float SD1 = stdev(P1), SD2 = stdev(P2);
    float Sp = sqrt(((S1-1)*pow(SD1, 2) + (S2-1)*pow(SD2, 2))/(V));
    float t_stat = absolute((average(P1) - average(P2))/(Sp*sqrt(1.0f/S1 + 1.0f/S2)));

    boost::math::students_t dist(V);
    return boost::math::cdf(boost::math::complement(dist, t_stat))*2;
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

inline float fisher_exact_P(const int& O, int OT, int N, int T){

    unsigned int total = T;

    T -= N;
    OT -= O;
    N -= O;
    T -= OT;

    return (factorial((long double)(O+OT))*factorial((long double)(N+T)) *
            factorial((long double)(O+N))*factorial((long double)(OT+T))) /
                (factorial((long double)(total))*factorial((long double)(O)) *
                 factorial((long double)(OT))*factorial((long double)(N))*factorial((long double)(T)));

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

inline constexpr bool isSpecial(const char& c){
    return(((c < 'A') && (c > 'Z')) &&
           ((c < 'a') && (c > 'z')) &&
           ((c < '0') && (c > '9')) &&
           (c != '-') && (c != '+') &&
           (c != '/'));
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
    if(numeric.find('.') >= numeric.size()) return numeric;
    std::string output = numeric;
    while(output.back() == '0') output.pop_back();
    while(output.back() == '.') output.pop_back();
    return output;
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

inline void to_uppercase(std::string& s){
    size_t L = s.size();
    for(size_t i = 0; i < L; ++i){
        if(isLowerCase(s[i])) s[i] -= 32;
    }
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

inline bool cmpShortString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    size_t s1SIZE = focus.size(), s2SIZE = other.size();
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
                if(index > 0) if(!isCharType(other[index-1], " ,.;!_/\\+=|<>")) bOutput = false;
                if(index + s1SIZE < s2SIZE) if(!isCharType(other[index+s1SIZE], " ,.;!_/\\+=|<>")) bOutput = false;
            }
            else bOutput = false;
        }
    }
    else{
        unsigned int index = focus.find(other);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(index < s1SIZE){
                if(index > 0) if(!isCharType(focus[index-1], " ,.;!_/\\+=|<>")) bOutput = false;
                if(index + s2SIZE < s1SIZE) if(!isCharType(focus[index+s2SIZE], " ,.;!_/\\+=|<>")) bOutput = false;
            }
            else bOutput = false;
        }
    }

    return bOutput;
}

inline bool cmpString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    if(focus == other)
        return true; // Attempt short-circuit for direct matches
    else if(!params) return false;

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
    }
    else if(focus.find(other) < s1SIZE) return true;

    return false;
}

inline bool cmpStringToList(const std::string& focus, const std::vector<std::string>& list, const BYTE& params = CMP_STR_DEFAULT){
    for (auto str : list){
        if(cmpString(focus, str, params)) return true;
    }
    return false;
}

inline bool cmpStringToList(const StringVector& query, const StringVector& other, const BYTE& params = CMP_STR_DEFAULT){
    for(auto str: query){
        if(cmpStringToList(str, other, params)) return true;
    }
    return false;
}

inline bool cmpStringIncludeList(const std::string& focus, const std::vector<std::string>& list, const BYTE& params = CMP_STR_DEFAULT){
    for (auto str : list){
        if(!cmpString(focus, str, params)) return false;
    }
    return true;
}

inline unsigned int findShortString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    size_t s1SIZE = focus.size(), s2SIZE = other.size();
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
    unsigned int index;

    if(s2SIZE >= s1SIZE){
        index = other.find(focus);
        if(params & CMP_STR_SMALL_DISCRETE){
            if(index < s2SIZE){
                if(index > 0) if(!isCharType(other[index-1], " ,.;!_/\\+=")) bOutput = false;
                if(index + s1SIZE < s2SIZE) if(!isCharType(other[index+s1SIZE], " ,.;!_/\\+=")) bOutput = false;
            }
            else bOutput = false;
        }
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
    }

    if(bOutput) return index;
    else return UINT_MAX;
}

inline unsigned int findString(std::string focus, std::string other, const BYTE& params = CMP_STR_DEFAULT){

    if(focus == other) return 0; // Attempt short-circuit for direct matches

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
                        processString(output.back());
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

}



#endif // BIOALGORITHM_HPP
