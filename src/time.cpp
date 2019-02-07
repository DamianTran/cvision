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

#include "cvision/time.hpp"
#include "EZC/toolkit/string.hpp"

#include <cmath>
#include <ctime>
#include <iomanip>

using namespace EZC;

namespace cvis{

std::string day_of_week(const TimePoint& time){
    int dist = time.total_days() - EPOCH_TIME.total_days(),
        mod = dist >= 0 ? dist % 7 : 6 - (-dist % 7);
    switch(mod){
        case 0:{
            return "Sunday";
        }
        case 1:{
            return "Monday";
        }
        case 2:{
            return "Tuesday";
        }
        case 3:{
            return "Wednesday";
        }
        case 4:{
            return "Thursday";
        }
        case 5:{
            return "Friday";
        }
        default:{
            return "Saturday";
        }
    }
}

int days_between_months(const int& start_month,
                               const int& end_month,
                               const bool& leap_year){
    int output = 0;
    if(end_month < 1) return output;

    for(size_t i = start_month > 0 ? start_month : 1; i <= end_month; ++i){
        output += days_in_month(i, leap_year);
    }
    return output;
}

std::string monthStr(const unsigned int& monthInt){
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

int monthInt(const std::string& month){

    return EZC::getMatchingIndex(month, calendar_months, CMP_STR_DEFAULT | CMP_STR_SW) + 1;

}

int day_int(const std::string& day){
    if(cmpString(day, "Sunday")) return 0;
    else if(cmpString(day, "Monday")) return 1;
    else if(cmpString(day, "Tuesday")) return 2;
    else if(cmpString(day, "Wednesday")) return 3;
    else if(cmpString(day, "Thursday")) return 4;
    else if(cmpString(day, "Friday")) return 5;
    else return 6;
}

void advance_day(TimePoint& time, const std::string& day_str){
    int cDay = day_int(day_of_week(time)),
        oDay = day_int(day_str);
    if(oDay > cDay){
        time.setDay(time.day() + oDay - cDay);
    }
    else{
        time.setDay(time.day() + 7 - (cDay - oDay));
    }
    time.distribute();
}

std::string num_to_time(unsigned int hour,
                        unsigned int minute,
                        unsigned int second){

    std::stringstream ss, output;
    std::string tmp;

    while((second != UINT_MAX) && (second >= 60)){
        ++minute;
        second -= 60;
    }

    while(minute >= 60){
        ++hour;
        minute -= 60;
    }

    output << hour << ':';

    ss << minute;
    tmp = ss.str();
    while(tmp.size() < 2) tmp.insert(tmp.begin(), '0');

    output << tmp;

    if((second) && (second != UINT_MAX)){

        ss.str("");
        ss << second;
        tmp = ss.str();
        while(tmp.size() < 2) tmp.insert(tmp.begin(), '0');

        output << ':' << tmp;
    }

    return output.str();

}

TimePoint::TimePoint():
    timeInfo(6,0){
        getTimeNow();
}

TimePoint::TimePoint(const int* timeInfo):
    timeInfo(6,0){
    for(size_t i = 0; i < 6; ++i){
        this->timeInfo[i] = timeInfo[i];
    }
}

TimePoint::TimePoint(const std::vector<int>& timeInfo){
    if(timeInfo.size() > 6) this->timeInfo.assign(timeInfo.begin(), timeInfo.begin() + 6);
    else{
        this->timeInfo = timeInfo;
        while(this->timeInfo.size() < 6){
            this->timeInfo.push_back(0);
        }
    }
}

TimePoint::TimePoint(const std::string& text):
    timeInfo(6, 0){

    StringVector parseBuffer;
    splitString(text, parseBuffer, DELIM_STANDARD);
    int* idx_ptr = nullptr;
    int time_val = UINT_MAX;

    for(size_t i = 0; i < parseBuffer.size(); ++i){
        try{
            time_val = std::stoi(parseBuffer[i]);
        }catch(...){
            try{
                idx_ptr = &getIndex(parseBuffer[i]);
            }catch(...){ }
        }

        if((time_val != INT_MAX) && (idx_ptr != nullptr)){
            *idx_ptr = time_val;
            idx_ptr = nullptr;
            time_val = INT_MAX;
        }
    }

}

TimePoint::TimePoint(const int& seconds):
    timeInfo(6, 0){
        setSecond(seconds);

        if(seconds > 0){
            while(timeInfo[5] >= 60){
                timeInfo[5] -= 60;
                ++timeInfo[4];
            }

            while(timeInfo[4] >= 60){
                timeInfo[4] -= 60;
                ++timeInfo[3];
            }

            while(timeInfo[3] >= 24){
                timeInfo[3] -= 24;
                ++timeInfo[2];
            }

            while(timeInfo[2] >= 365){
                timeInfo[2] -= 365;
                ++timeInfo[0];
            }
        }
        else if(seconds < 0){
            while(timeInfo[5] <= 60){
                timeInfo[5] += 60;
                --timeInfo[4];
            }

            while(timeInfo[4] <= 60){
                timeInfo[4] += 60;
                --timeInfo[3];
            }

            while(timeInfo[3] <= 24){
                timeInfo[3] += 24;
                --timeInfo[2];
            }

            while(timeInfo[2] <= 365){
                timeInfo[2] += 365;
                --timeInfo[0];
            }
        }
}

TimePoint::TimePoint(const int& years,
          const int& months,
          const int& days,
          const int& hours,
          const int& minutes,
          const int& seconds):
              timeInfo({years, months, days, hours, minutes, seconds}){ }

std::ostream& operator<<(std::ostream& output, const TimePoint& T){
    output << T.year() << '/' << T.month() << '/' << T.day() << '\t' << num_to_time(T.hour(),T.minute(),T.second());
    return output;
}

TimePoint abs(const TimePoint& other){

    return TimePoint(std::abs(other.year()),
                     std::abs(other.month()),
                     std::abs(other.day()),
                     std::abs(other.hour()),
                     std::abs(other.minute()),
                     std::abs(other.second()));

}

int TimePoint::total_months() const{ return 12*timeInfo[0] + timeInfo[1] - 1; }
int TimePoint::total_days() const{
    if(timeInfo[1]) return floor(365.25f * timeInfo[0] + days_between_months(1, timeInfo[1]-1) + timeInfo[2]);
    else return floor(365.25f * timeInfo[0] + days_between_months(1, timeInfo[1]-1) + timeInfo[2]); }
int TimePoint::total_hours() const{ return 24 * (total_days()) + timeInfo[3]; }
int TimePoint::total_minutes() const{ return total_hours()*60 + timeInfo[4]; }
int TimePoint::total_seconds() const{ return total_minutes()*60 + timeInfo[5]; }

std::string TimePoint::getDate() const{
    return monthStr(month()) + " " + std::to_string(day()) + ", " + std::to_string(year());
}
std::string TimePoint::getTime(bool _24hr, bool seconds) const{
    std::stringstream oss, buf;
    std::string tmp;

    if(_24hr){
        oss << hour() << ':';

        buf << minute();
        tmp = buf.str();
        while(tmp.size() < 2) tmp.insert(tmp.begin(), '0');

        oss << tmp;
        if(seconds){
            oss << ':';

            buf.str("");
            buf << second();
            tmp = buf.str();
            while(tmp.size() < 2) tmp.insert(tmp.begin(), '0');

            oss << tmp;
        }
    }
    else{
        int adj_hour = hour();
        std::string suffix = "AM";
        if(adj_hour > 12){
            adj_hour -= 12;
            suffix = "PM";
        }
        else if(!adj_hour){
            adj_hour = 12;
        }
        else if(adj_hour == 12){
            suffix = "PM";
        }

        oss << adj_hour << ':';

        buf << minute();
        while(buf.str().size() < 2) buf << '0';

        oss << buf.str();
        if(seconds){
            oss << ':';

            buf.str("");
            buf << second();
            while(buf.str().size() < 2) buf << '0';

            oss << buf.str();
        }

        oss << ' ' << suffix;
    }

    return oss.str();
}

std::string TimePoint::getDurationStr() const{

    std::stringstream ss;

    int length = total_seconds();

    if(length > 2678400){

        ss << total_months() << " months";

    }
    else if(length > 604800){

        ss << total_days()/7 << " weeks";

    }
    else if(length > 86400){

        ss << total_days() << " days";

    }
    else if(length > 3600){

        ss << total_hours() << " hours";

    }
    else if(length == 1800){

        ss << "half an hour";

    }
    else if(length > 60){

        ss << total_minutes() << " minutes";

    }
    else{

        ss << total_seconds() << " seconds";

    }

    return ss.str();
}

void TimePoint::getTimeNow(){
    timeInfo.clear();
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "20%y %m %d %H %M %S");

    EZC::StringVector timeStr; EZC::splitString(oss.str(), timeStr, " ");

    for(auto str : timeStr){
        try{
            timeInfo.push_back(std::stoi(str));
        }catch(...)
        {
            throw std::runtime_error("Failed to acquire time point (Now)");
        }
    }
}

void TimePoint::distribute(){

    while(timeInfo[5] >= 60){
        timeInfo[5] -= 60;
        ++timeInfo[4];
    }
    while(timeInfo[5] < 0){
        timeInfo[5] = 60 - timeInfo[5];
        --timeInfo[4];
    }

    while(timeInfo[4] >= 60){
        timeInfo[4] -= 60;
        ++timeInfo[3];
    }
    while(timeInfo[4] < 0){
        timeInfo[4] = 60 - timeInfo[4];
        --timeInfo[3];
    }

    while(timeInfo[3] >= 24){
        timeInfo[3] -= 24;
        ++timeInfo[2];
    }
    while(timeInfo[3] < 0){
        timeInfo[3] = 24 - timeInfo[3];
        --timeInfo[2];
    }

    while(timeInfo[1] > 12){

        timeInfo[1] -= 12;
        ++timeInfo[0];

    }
    while(timeInfo[1] <= 0){

        --timeInfo[0];
        timeInfo[1] += 12;

    }

    while((timeInfo[1] <= 12) &&
          (timeInfo[2] > days_in_month(timeInfo[1], false))){
        timeInfo[2] -= days_in_month(timeInfo[1], false);
        if(timeInfo[1] == 12){
            timeInfo[1] = 1;
            ++timeInfo[0];
        }
        else ++timeInfo[1];
    }
    while(timeInfo[2] <= 0){
        if(!timeInfo[1] || (timeInfo[1] == 1)){
            timeInfo[1] = 12;
            if(!timeInfo[2]) timeInfo[2] = days_in_month(timeInfo[1], false) + timeInfo[2];
            else timeInfo[2] = days_in_month(timeInfo[1], false) + timeInfo[2] + 1;
        }
        else{
            --timeInfo[1];
            if(!timeInfo[2]) timeInfo[2] = days_in_month(timeInfo[1], false) + timeInfo[2];
            else timeInfo[2] = days_in_month(timeInfo[1], false) + timeInfo[2] + 1;
        }
    }

    if((timeInfo[1] > 12) || (timeInfo[2] > days_in_month(timeInfo[1])) ||
       (timeInfo[3] > 24) || (timeInfo[4] > 60) || (timeInfo[5] > 60)) distribute();

    return;

}

void TimePoint::setFromEpoch(const int& timeCount){

    *this = EPOCH_TIME;
    *this += TimePoint(0,0,0,0,0,timeCount);
    distribute();

}

int& TimePoint::getIndex(const std::string& text){
    if(cmpString(text, "year"))     return timeInfo[0];
    if(cmpString(text, "month"))    return timeInfo[1];
    if(cmpString(text, "day"))      return timeInfo[2];
    if(cmpString(text, "hour"))     return timeInfo[3];
    if(cmpString(text, "minute"))   return timeInfo[4];
    if(cmpString(text, "second"))   return timeInfo[5];

    throw std::invalid_argument("TimePoint::getIndex(): Text does not correspond to a time denominator");
}

bool TimePoint::operator==(const TimePoint& other) const{
    return((second() == other.second()) &&
           (minute() == other.minute()) &&
           (hour() == other.hour()) &&
           (day() == other.day()) &&
           (month() == other.month()) &&
           (year() == other.year()));
}

bool TimePoint::operator!=(const TimePoint& other) const{
    return !(*this == other);
}

bool TimePoint::operator<(const TimePoint& other) const{

    if(year() < other.year()) return true;
    else if(year() == other.year()){

        if(month() < other.month()) return true;
        else if(month() == other.month()){

            if(day() < other.day()) return true;
            else if(day() == other.day()){

                if(hour() < other.hour()) return true;
                else if(hour() == other.hour()){

                    if(minute() < other.minute()) return true;
                    else if(minute() == other.minute()){

                        if(second() < other.second()) return true;

                    }

                }

            }

        }

    }

    return false;

}

bool TimePoint::operator<=(const TimePoint& other) const{
    return !(*this > other);
}

bool TimePoint::operator>(const TimePoint& other) const{

    if(year() > other.year()) return true;
    else if(year() == other.year()){

        if(month() > other.month()) return true;
        else if(month() == other.month()){

            if(day() > other.day()) return true;
            else if(day() == other.day()){

                if(hour() > other.hour()) return true;
                else if(hour() == other.hour()){

                    if(minute() > other.minute()) return true;
                    else if(minute() == other.minute()){

                        if(second() > other.second()) return true;

                    }

                }

            }

        }

    }

    return false;
}

bool TimePoint::operator>=(const TimePoint& other) const{
    return !(*this < other);
}

TimePoint TimePoint::operator+(const TimePoint& other) const{

    TimePoint output = *this;

    output.setYear(output.year() + other.year());
    output.setMonth(output.month() + other.month());
    output.setDay(output.day() + other.day());
    output.setHour(output.hour() + other.hour());
    output.setMinute(output.minute() + other.minute());
    output.setSecond(output.second() + other.second());

    return output;

}

TimePoint TimePoint::operator-(const TimePoint& other) const{

    TimePoint output = *this;

    output.setYear(output.year() - other.year());
    output.setMonth(output.month() - other.month());
    output.setDay(output.day() - other.day());
    output.setHour(output.hour() - other.hour());
    output.setMinute(output.minute() - other.minute());
    output.setSecond(output.second() - other.second());

    return output;

}

TimePoint TimePoint::operator-() const{
    TimePoint output = *this;
    output.setYear(-output.year());
    output.setMonth(-output.month());
    output.setDay(-output.day());
    output.setHour(-output.hour());
    output.setMinute(-output.minute());
    output.setSecond(-output.second());

    return output;
}

void TimePoint::operator+=(const TimePoint& other){
    setYear(year() + other.year());
    setMonth(month() + other.month());
    setDay(day() + other.day());
    setHour(hour() + other.hour());
    setMinute(minute() + other.minute());
    setSecond(second() + other.second());
}

void TimePoint::operator-=(const TimePoint& other){
    setYear(year() - other.year());
    setMonth(month() - other.month());
    setDay(day() - other.day());
    setHour(hour() - other.hour());
    setMinute(minute() - other.minute());
    setSecond(second() - other.second());
}

void fread(TimePoint& timePoint, FILE* inFILE){
    int new_info[6];
    fread(new_info, sizeof(int), 6, inFILE);
    timePoint.timeInfo.assign(new_info, new_info + 6);
}

void fwrite(const TimePoint& timePoint, FILE* outFILE){
    fwrite(timePoint.timeInfo.data(), sizeof(int), 6, outFILE);
}

}
