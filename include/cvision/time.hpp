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
// warranties.  By using CVision, you ackowledge and agree
// to this disclaimer.  Use of CVision in Users's programs
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

#ifndef CVIS_TIME
#define CVIS_TIME

#include "cvision/lib.hpp"

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

#define YEARS(N)        TimePoint( N, 0, 0, 0, 0, 0 )
#define MONTHS(N)       TimePoint( 0, N, 0, 0, 0, 0 )
#define DAYS(N)         TimePoint( 0, 0, N, 0, 0, 0 )
#define HOURS(N)        TimePoint( 0, 0, 0, N, 0, 0 )
#define MINUTES(N)      TimePoint( 0, 0, 0, 0, N, 0 )
#define SECONDS(N)      TimePoint( 0, 0, 0, 0, 0, N )

#define EPOCH_TIME      TimePoint(2018, 9, 16, 0, 0, 0)

#define TIME_NOW                        std::chrono::high_resolution_clock::now()
#define TIME_EPOCH                      std::chrono::duration_cast<std::chrono::seconds>(TIME_NOW.time_since_epoch()).count()

namespace cvis{

// Time scales used for calendar and other time-tracking apps

enum time_scale{
    any,
    millenium,
    century,
    decade,
    year,
    month,
    week,
    day,
    hour,
    minute,
    second
};

const static std::vector<std::string> calendar_days = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

const static std::vector<std::string> calendar_months = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

inline unsigned int days_in_month(const unsigned int& month, const bool& leap_year = false){
    switch(month){
        case 1:{
            return 31;
        }
        case 2:{
            if(leap_year) return 29;
            return 28;
        }
        case 3:{
            return 31;
        }
        case 4:{
            return 30;
        }
        case 5:{
            return 31;
        }
        case 6:{
            return 30;
        }
        case 7:{
            return 31;
        }
        case 8:{
            return 31;
        }
        case 9:{
            return 30;
        }
        case 10:{
            return 31;
        }
        case 11:{
            return 30;
        }
        case 12:{
            return 31;
        }
        default:{
            throw std::invalid_argument("In days_in_month: integer does not correspond to month");
        }
    }
}

CVISION_API int days_between_months(const int& start_month,
                           const int& end_month,
                           const bool& leap_year = false);
CVISION_API std::string monthStr(const unsigned int& monthInt);
CVISION_API int monthInt(const std::string& month);

CVISION_API std::string num_to_time(unsigned int hour,
                        unsigned int minute = 0,
                        unsigned int second = 0);

class CVISION_API TimePoint{
protected:
    std::vector<int> timeInfo;
    std::string annotation;

    friend class CVCalendarPanel;
    friend class CVCalendarRecord;

public:

    inline const int& year() const{ return timeInfo[0]; }
    inline const int& month() const{ return timeInfo[1]; }
    inline const int& day() const{ return timeInfo[2]; }
    inline const int& hour() const{ return timeInfo[3]; }
    inline const int& minute() const{ return timeInfo[4]; }
    inline const int& second() const{ return timeInfo[5]; }

    inline int& year(){ return timeInfo[0]; }
    inline int& month(){ return timeInfo[1]; }
    inline int& day(){ return timeInfo[2]; }
    inline int& hour(){ return timeInfo[3]; }
    inline int& minute(){ return timeInfo[4]; }
    inline int& second(){ return timeInfo[5]; }

    inline void setYear(const int& newValue){ timeInfo[0] = newValue; }
    inline void setMonth(const int& newValue){ timeInfo[1] = newValue; }
    inline void setDay(const int& newValue){ timeInfo[2] = newValue; }
    inline void setHour(const int& newValue){ timeInfo[3] = newValue; }
    inline void setMinute(const int& newValue){ timeInfo[4] = newValue; }
    inline void setSecond(const int& newValue){ timeInfo[5] = newValue; }

    int total_months() const;
    int total_days() const;
    int total_hours() const;
    int total_minutes() const;
    int total_seconds() const;

    inline const int* data() const{ return timeInfo.data(); }

    CVISION_API void setFromEpoch(const int& timeCount);    // Set using integer based on TIME_EPOCH reference

    inline int& operator[](const unsigned int& index){ return timeInfo[index]; }
    CVISION_API friend std::ostream& operator<<(std::ostream& output, const TimePoint& T);
    CVISION_API friend TimePoint abs(const TimePoint& other);

    // Operator overloads

    CVISION_API bool operator==(const TimePoint& other) const;
    CVISION_API bool operator!=(const TimePoint& other) const;

    CVISION_API bool operator<(const TimePoint& other) const;
    CVISION_API bool operator<=(const TimePoint& other) const;
    CVISION_API bool operator>(const TimePoint& other) const;
    CVISION_API bool operator>=(const TimePoint& other) const;

    CVISION_API TimePoint operator+(const TimePoint& other) const;
    CVISION_API TimePoint operator-(const TimePoint& other) const;
    CVISION_API TimePoint operator-() const;

    CVISION_API void operator+=(const TimePoint& other);
    CVISION_API void operator-=(const TimePoint& other);

    CVISION_API friend void advance_day(TimePoint& time, const std::string& day);

    CVISION_API std::string getDate() const;
    CVISION_API std::string getTime(bool _24hr = false, bool seconds = false) const;
    CVISION_API std::string getDurationStr() const;

    CVISION_API void getTimeNow();

    CVISION_API friend void fread(TimePoint& timePoint, FILE* inFILE);
    CVISION_API friend void fwrite(const TimePoint& timePoint, FILE* outFILE);

    CVISION_API int& getIndex(const std::string& text);

    CVISION_API void distribute(); // Normalize time categories to metric maxima

    CVISION_API TimePoint();
    CVISION_API TimePoint(const int* timeInfo);
    CVISION_API TimePoint(const std::vector<int>& timeInfo);
    CVISION_API TimePoint(const int& seconds);
    CVISION_API TimePoint(const int& years,
              const int& months,
              const int& days,
              const int& hours,
              const int& minutes,
              const int& seconds);
    CVISION_API TimePoint(const std::string& text);

    inline void setTag(const std::string& tag){
        annotation = tag;
    }
    inline const std::string& getTag() const{ return annotation; }

    ~TimePoint() = default;
};

CVISION_API std::string day_of_week(const TimePoint& time);
CVISION_API int day_int(const std::string& day);

inline std::string current_time_str(){
    TimePoint t;
    t.getTimeNow();
    return t.getTime();
}

struct CVISION_API TimeLog : public std::vector<TimePoint>{

    inline void addTimePoint(const unsigned int& index){ insert(begin() + index, TimePoint()); }
    inline void removeTimePoint(const unsigned int& index){ erase(begin() + index); }
    inline void addTimePoint(){ emplace_back(); }
    inline void removeTimePoint(){ pop_back(); }

};


}

#endif // CVIS_TIME
