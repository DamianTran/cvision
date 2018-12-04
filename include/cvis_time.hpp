//
// CVision: a multi-platform graphics interface libary for C++
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
// Redistribution of CVision is permitted under accordance with
// the GNU general public licence (GPL) version 3.0 and the
// terms and conditions specified in CVlicence.txt

#pragma once

#ifndef CVIS_TIME
#define CVIS_TIME

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

int days_between_months(const int& start_month,
                           const int& end_month,
                           const bool& leap_year = false);

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

std::string num_to_time(unsigned int hour,
                        unsigned int minute = 0,
                        unsigned int second = 0);

class TimePoint{
protected:
    std::vector<int> timeInfo;
    std::string annotation;

public:

    inline const int& year() const{ return timeInfo[0]; }
    inline const int& month() const{ return timeInfo[1]; }
    inline const int& day() const{ return timeInfo[2]; }
    inline const int& hour() const{ return timeInfo[3]; }
    inline const int& minute() const{ return timeInfo[4]; }
    inline const int& second() const{ return timeInfo[5]; }

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

    inline int& operator[](const unsigned int& index){ return timeInfo[index]; }
    friend std::ostream& operator<<(std::ostream& output, const TimePoint& T);

    void distribute();

    // Operator overloads

    bool operator==(const TimePoint& other) const;
    bool operator!=(const TimePoint& other) const;

    bool operator<(const TimePoint& other) const;
    bool operator<=(const TimePoint& other) const;
    bool operator>(const TimePoint& other) const;
    bool operator>=(const TimePoint& other) const;

    TimePoint operator+(const TimePoint& other) const;
    TimePoint operator-(const TimePoint& other) const;
    TimePoint operator-() const;

    void operator+=(const TimePoint& other);
    void operator-=(const TimePoint& other);

    std::string getDate() const;
    std::string getTime(bool _24hr = false, bool seconds = false) const;

    void getTimeNow();

    friend void fread(TimePoint& timePoint, FILE* inFILE);
    friend void fwrite(const TimePoint& timePoint, FILE* outFILE);

    int& getIndex(const std::string& text);

    TimePoint();
    TimePoint(const int* timeInfo);
    TimePoint(const std::vector<int>& timeInfo);
    TimePoint(const int& seconds);
    TimePoint(const int& years,
              const int& months,
              const int& days,
              const int& hours,
              const int& minutes,
              const int& seconds);
    TimePoint(const std::string& text);

    inline void setTag(const std::string& tag){
        annotation = tag;
    }
    inline const std::string& getTag() const{ return annotation; }

    ~TimePoint() = default;
};

std::string day_of_week(const TimePoint& time);
int day_int(const std::string& day);
void advance_day(TimePoint& time, const std::string& day);

inline std::string current_time_str(){
    TimePoint t;
    t.getTimeNow();
    return t.getTime();
}

struct TimeLog : public std::vector<TimePoint>{

    inline void addTimePoint(const unsigned int& index){ insert(begin() + index, TimePoint()); }
    inline void removeTimePoint(const unsigned int& index){ erase(begin() + index); }
    inline void addTimePoint(){ emplace_back(); }
    inline void removeTimePoint(){ pop_back(); }

};


}

#endif // CVIS_TIME
