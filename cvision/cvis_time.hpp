#pragma once

#ifndef CVIS_TIME
#define CVIS_TIME

#include "algorithm.hpp"

namespace cvis{

inline std::string monthStr(const unsigned int& monthInt){
    switch(monthInt){
        case 1:{ return "January"; }
        case 2:{ return "February"; }
        case 3:{ return "March"; }
        case 4:{ return "April"; }
        case 5:{ return "May"; }
        case 6:{ return "June"; }
        case 7:{ return "July"; }
        case 8:{ return "August"; }
        case 9:{ return "September"; }
        case 10:{ return "October"; }
        case 11:{ return "November"; }
        case 12:{ return "December"; }
        default: return std::string();
    }
}

struct TimePoint{
    std::vector<int> timeInfo;

    inline const int& year() const{ return timeInfo[0]; }
    inline const int& month() const{ return timeInfo[1]; }
    inline const int& day() const{ return timeInfo[2]; }
    inline const int& hour() const{ return timeInfo[3]; }
    inline const int& minute() const{ return timeInfo[4]; }
    inline const int& second() const{ return timeInfo[5]; }

    inline const int* data() const{ return timeInfo.data(); }

    inline int& operator[](const unsigned int& index){ return timeInfo[index]; }
    friend std::ostream& operator<<(std::ostream& output, TimePoint& T){
        output << T.year() << '/' << T.month() << '/' << T.day() << ' ' << T.hour() << ':' << T.minute() << ':' << T.second();
        return output;
    }

    inline std::string getDate() const{
        return monthStr(month()) + " " + std::to_string(day()) + ", " + std::to_string(year());
    }
    inline std::string getTime(bool _24hr = false, bool seconds = false) const{
        std::ostringstream oss;
        if(_24hr){
             oss << hour() << ':' << minute();
             if(seconds) oss << ':' << second();
        }
        else{
            if(hour() > 12){
                oss << hour()-12 << ':' << minute();
                if(seconds) oss << ':' << second();
                oss << " PM";
            }
            else{
                oss << hour() << ':' << minute();
                if(seconds) oss << ':' << second();
                oss << " AM";
            }
        }

        return oss.str();
    }

    void getTimeNow(){
        timeInfo.clear();
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "20%y %m %d %H %M %S");

        AIALG::StringVector timeStr; AIALG::splitString(oss.str(), timeStr, " ");
        for(auto str : timeStr){
            timeInfo.push_back(std::stoi(str));
        }
    }

    TimePoint(){
        getTimeNow();
    }

    TimePoint(const int* timeInfo):
        timeInfo(6,0){
        for(size_t i = 0; i < 6; ++i){
            this->timeInfo[i] = timeInfo[i];
        }
    }

    TimePoint(const std::vector<int>& timeInfo){
        if(timeInfo.size() > 6) this->timeInfo.assign(timeInfo.begin(), timeInfo.begin() + 5);
        else{
            this->timeInfo = timeInfo;
            while(this->timeInfo.size() < 6){
                this->timeInfo.push_back(0);
            }
        }
    }

    ~TimePoint() = default;
};

inline std::string current_time_str(){
    TimePoint t;
    t.getTimeNow();
    return t.getTime();
}

struct TimeLog{

    std::vector<TimePoint> timeInfo;

    inline unsigned int size() const{ return timeInfo.size(); }
    inline void erase(const unsigned int& index){ timeInfo.erase(timeInfo.begin() + index); }
    inline void emplace_back(const std::vector<int>& timePoint){ timeInfo.emplace_back(timePoint); }
    inline void emplace_back(const int* timePoint){ timeInfo.emplace_back(timePoint); }
    inline void clear(){ timeInfo.clear(); }

    inline TimePoint& operator[](const unsigned int& index){ return timeInfo[index]; }
    inline const TimePoint& front() const{ return timeInfo.front(); }
    inline const TimePoint& back() const{ return timeInfo.back(); }

    inline void addTimePoint(const unsigned int& index){ timeInfo.insert(timeInfo.begin() + index, TimePoint()); }
    inline void removeTimePoint(const unsigned int& index){ timeInfo.erase(timeInfo.begin() + index); }
    inline void addTimePoint(){ timeInfo.emplace_back(); }
    inline void removeTimePoint(){ timeInfo.pop_back(); }

    ~TimeLog() = default;

};

}

#endif // CVIS_TIME
