#include "cvision/cvis_time.hpp"
#include "algorithm.hpp"

using namespace AIALG;

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
}

std::string num_to_time(unsigned int hour,
                        unsigned int minute,
                        unsigned int second){

    std::stringstream h, m, s, output;

    while((second != UINT_MAX) && (second >= 60)){
        ++minute;
        second -= 60;
    }

    while(minute >= 60){
        ++hour;
        minute -= 60;
    }

    h << hour;

    m << minute;
    while(m.str().size() < 2) m << '0';

    s << second;
    while(s.str().size() < 2) s << '0';

    output << h.str() << ':' << m.str();

    if((second != 0) && (second != UINT_MAX)){
        output << ':' << second;
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

void TimePoint::getTimeNow(){
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


    while(timeInfo[2] > days_in_month(timeInfo[1], false)){
        timeInfo[2] -= days_in_month(timeInfo[1], false);
        if(timeInfo[1] == 12){
            timeInfo[1] = 1;
            ++timeInfo[0];
        }
        ++timeInfo[1];
    }
    while(timeInfo[2] < 0){
        if(timeInfo[1] == 1){
            timeInfo[1] = 12;
            timeInfo[2] = days_in_month(timeInfo[1], false) - timeInfo[2];
        }
        else{
            --timeInfo[1];
            timeInfo[2] = days_in_month(timeInfo[1], false) - timeInfo[2];
        }
    }
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
    return((year() < other.year()) ||
           (month() < other.month()) ||
           (day() < other.day()) ||
           (hour() < other.hour()) ||
           (minute() < other.minute()) ||
           (second() < other.second()));
}

bool TimePoint::operator<=(const TimePoint& other) const{
    return !(*this > other);
}

bool TimePoint::operator>(const TimePoint& other) const{
    return((year() > other.year()) ||
           (month() > other.month()) ||
           (day() > other.day()) ||
           (hour() > other.hour()) ||
           (minute() > other.minute()) ||
           (second() > other.second()));
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
