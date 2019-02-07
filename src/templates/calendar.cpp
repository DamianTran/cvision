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

#include "cvision/templates/calendar.hpp"
#include "cvision/time.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include "EZC/algorithm.hpp"
#include "EZC/toolkit/string.hpp"

namespace cvis{

using namespace EZC;

CVCalendarPanel::CVCalendarPanel(CVView* parentView, const std::string& panelTag,
                                 const sf::Color& bkgColor, const sf::Vector2f& size,
                                 const bool& bFitWindow, const sf::Vector2f& position):
                                     CVBasicViewPanel(parentView, panelTag, bkgColor,
                                                      size, bFitWindow, position),
                                        scale(time_scale::week),
                                        scheduleBlockAlpha(160),
                                        b24hour(false),
                                        tableOutlineThickness(1.0f),
                                        margins({ 64.0f, 64.0f, 0.0f, 0.0f }),
                                        headerBox(nullptr),
                                        backHeaderButton(nullptr),
                                        fwdHeaderButton(nullptr),
                                        scheduleBounds(left_margin(),top_margin(),
                                                       bounds.width - left_margin(),
                                                       bounds.height - top_margin()),
                                        scrollBarX(parentView,
                                        sf::Vector2f(bounds.left + left_margin() + 12.0f,
                                       bounds.top + bounds.height - 5.0f),
                                        sf::Vector2f(bounds.left + bounds.width - 12.0f,
                                                            bounds.top + bounds.height - 5.0f),
                                                   size.x, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
                                        1000.0f, CV_OBJ_ANIM_FADE_IN),
                                        scrollBarY(parentView,
                                        sf::Vector2f(bounds.left + bounds.width - 5.0f,
                                       bounds.top + 12.0f + top_margin()),
                                        sf::Vector2f(bounds.left + bounds.width - 5.0f,
                                                            bounds.top + bounds.height - 12.0f),
                                                   size.y, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
                                        1000.0f, CV_OBJ_ANIM_FADE_IN){

    viewing_time.getTimeNow();
    current_time.getTimeNow();

    textInfo.alignment = ALIGN_LEFT;
    textInfo.fontSize = 16*View->getViewScale();
    textInfo.font = mainApp()->getDefaultFont();

    scrollBarX.setDirection(CV_WHEEL_DIR_HORIZONTAL);

    scrollBarX.setPanelSize(size.x - left_margin() - right_margin());
    scrollBarY.setPanelSize(size.y - top_margin() - bottom_margin());

    headerColorIndex = colorTheme.size();
    colorTheme.emplace_back(sf::Color(128,128,128,180));
    sideBarColorIndex = colorTheme.size();
    colorTheme.emplace_back(sf::Color(160,160,160,180));
    innerLineColorIndex = colorTheme.size();
    colorTheme.emplace_back(sf::Color(200,200,200));
    cellBorderColorIndex = colorTheme.size();
    colorTheme.emplace_back(sf::Color(150,150,150));
    cellFillColorIndex = colorTheme.size();
    colorTheme.emplace_back(sf::Color(255,255,255));

    setTimeScale(time_scale::week);
    centerOnTime(current_time);

    setDrawClipping(true);
}

CVCalendarRecord::CVCalendarRecord():
    scale(time_scale::any),
    color_code(0,0,0,0){ }

CVCalendarRecord::CVCalendarRecord(const std::string& newInfo,
                                const TimePoint& time,
                                const TimePoint& end_time,
                                const time_scale& scale,
                                const sf::Color& color):
                                    bRepeat(false),
                                    info(newInfo),
                                    time(time),
                                    end_time(end_time),
                                    scale(scale),
                                    color_code(color){ }

const std::string& CVCalendarRecord::tag() const{ return info; }
void CVCalendarRecord::setTag(const std::string& newTag){ info = newTag; }

void CVCalendarRecord::setLocation(const std::string& newLocation){ location = newLocation; }
const std::string& CVCalendarRecord::getLocation() const{ return location; }

void CVCalendarRecord::setRepeatTags(const StringVector& newTags){
    repeatTags = newTags;
}

const StringVector& CVCalendarRecord::getRepeatTags() const{
    return repeatTags;
}

const bool& CVCalendarRecord::is_repeating() const{
    return bRepeat;
}

void CVCalendarRecord::set_repeat(const TimePoint& interval){
    frequency = interval;
    if(interval.total_seconds() > 0)
        bRepeat = true;
}

void CVCalendarRecord::clear_repeat(){
    bRepeat = false;
}

const TimePoint& CVCalendarRecord::repeat_frequency() const{
    return frequency;
}

const TimePoint& CVCalendarRecord::start() const{
    return time;
}

TimePoint CVCalendarRecord::getDuration() const{

    return end() - start();

}

std::string CVCalendarRecord::getDurationStr() const{

    return getDuration().getDurationStr();

}

const TimePoint& CVCalendarRecord::end() const{
    return end_time;
}

void CVCalendarRecord::setStart(const TimePoint& newStart){
    time = newStart;
}
void CVCalendarRecord::setEnd(const TimePoint& newEnd){
    end_time = newEnd;
}

const time_scale& CVCalendarRecord::getScale() const{
    return scale;
}

void CVCalendarRecord::setScale(const time_scale& newScale){
    scale = newScale;
}

void CVCalendarRecord::addReminder(const TimePoint& time_advance){

    reminders.emplace_back(time_advance);

}

bool CVCalendarRecord::flagReminder(const TimePoint& time){

    bool bShouldFlag = false;
    for(size_t i = 0; i < reminders.size();){

        if(time > start() - reminders[i]){
            if(time < start()){
                bShouldFlag = true;
            }
            reminders.erase(reminders.begin() + i);
        }
        else ++i;

    }

    return bShouldFlag;

}

const sf::Color& CVCalendarRecord::color() const{
    return color_code;
}
void CVCalendarRecord::setColor(const sf::Color& newColor){
    color_code = newColor;
}

void CVCalendarRecord::save(FILE* outFILE) const{
    fwrite(&bRepeat, 1, sizeof(bool), outFILE);
    writeString(info, outFILE);
    writeString(location, outFILE);
    writeString(notes, outFILE);
    writeVector(repeatTags, outFILE);

    size_t L = reminders.size();
    fwrite(&L, sizeof(L), 1, outFILE);
    for(size_t i = 0; i < L; ++i){
        fwrite(reminders[i], outFILE);
    }

    fwrite(time, outFILE);
    fwrite(end_time, outFILE);
    fwrite(frequency, outFILE);
    fwrite(&scale, 1, sizeof(time_scale), outFILE);
    fwrite(&color_code, sizeof(sf::Color), 1, outFILE);
}

void CVCalendarRecord::load(FILE* inFILE){
    fread(&bRepeat, 1, sizeof(bool), inFILE);
    readString(info, inFILE);
    readString(location, inFILE);
    readString(notes, inFILE);
    readVector(repeatTags, inFILE);

    size_t L;
    fread(&L, sizeof(L), 1, inFILE);
    reminders.resize(L);
    for(size_t i = 0; i < L; ++i){
        fread(reminders[i], inFILE);
    }

    fread(time, inFILE);
    fread(end_time, inFILE);
    fread(frequency, inFILE);
    fread(&scale, 1, sizeof(time_scale), inFILE);
    fread(&color_code, sizeof(sf::Color), 1, inFILE);
}

bool CVCalendarRecord::check_overlap(const TimePoint& test_time) const{
    std::vector<CVCalendarRecord> rep_time(1, *this);

    bool bAltFlag = false,
        bAltToggle = true;

    int s = 0;

    if(is_repeating()){ // Extrapolate to detect future overlaps

        bAltFlag = cmpStringToList("alt", getRepeatTags());

        TimePoint end_t = start();

        if(start() <= test_time){
            end_t.setDay(days_in_month(month));
            end_t.setHour(23);
            end_t.setMinute(59);
            end_t.setSecond(59);
            while(rep_time.back().start() <= (end_t - rep_time.front().repeat_frequency())){
                rep_time.emplace_back(rep_time.back());
                rep_time.back().shift(rep_time.front().repeat_frequency());
            }
        }
        else{
            end_t.setDay(0);
            end_t.setHour(0);
            end_t.setMinute(0);
            end_t.setSecond(0);

            while(rep_time.back().start() >= end_t + rep_time.front().repeat_frequency()){
                rep_time.emplace_back(rep_time.back());
                rep_time.back().shift(-rep_time.front().repeat_frequency());
            }
        }

    }

    for(auto& record : rep_time){

        if(record.start().month() != test_time.month()) continue;

        if(bAltFlag){
            if(bAltToggle){
                bAltToggle = false;
            }
            else{
                bAltToggle = true;
                continue;
            }
        }

        if(cmpStringToList(record.getRepeatTags(), calendar_days)){
            if(!cmpStringToList(day_of_week(record.start()), record.getRepeatTags())) continue;
        }

        if(cmpStringToList(record.getRepeatTags(), calendar_months)){
            if(!cmpStringToList(monthStr(record.start().month()), record.getRepeatTags())) continue;
        }

        if((test_time >= record.start()) && (test_time <= record.end())) return true;
    }
    return false;
}

bool CVCalendarRecord::check_overlap(const CVCalendarRecord& other) const{
    std::vector<CVCalendarRecord> rep_time(1, *this);

    bool bAltFlag = false,
        bAltToggle = true;

    if(is_repeating()){ // Extrapolate to detect future overlaps

        bAltFlag = cmpStringToList("alt", getRepeatTags());

        TimePoint end_t = start();

        if(start() <= other.start()){
            end_t.setDay(days_in_month(month));
            end_t.setHour(23);
            end_t.setMinute(59);
            end_t.setSecond(59);
            while(rep_time.back().start() <= (end_t - rep_time.front().repeat_frequency())){
                rep_time.emplace_back(rep_time.back());
                rep_time.back().shift(rep_time.front().repeat_frequency());
            }
        }
        else{
            end_t.setDay(0);
            end_t.setHour(0);
            end_t.setMinute(0);
            end_t.setSecond(0);

            while(rep_time.back().start() >= end_t + rep_time.front().repeat_frequency()){
                rep_time.emplace_back(rep_time.back());
                rep_time.back().shift(-rep_time.front().repeat_frequency());
            }
        }

    }

    for(auto& record : rep_time){

        if(record.start().month() != other.start().month()) continue;

        if(bAltFlag){
            if(bAltToggle){
                bAltToggle = false;
            }
            else{
                bAltToggle = true;
                continue;
            }
        }

        if(cmpStringToList(record.getRepeatTags(), calendar_days)){
            if(!cmpStringToList(day_of_week(record.start()), record.getRepeatTags())) continue;
        }

        if(cmpStringToList(record.getRepeatTags(), calendar_months)){
            if(!cmpStringToList(monthStr(record.start().month()), record.getRepeatTags())) continue;
        }

        if(other.check_overlap(record.time) || other.check_overlap(record.end_time)) return true;
    }
    return false;
}

TimePoint CVCalendarRecord::duration() const{
    return end_time - time;
}

bool CVCalendarRecord::shift(const CVCalendarRecord& other, const TimePoint& padding,
                             const bool& forward){

    if(check_overlap(other)){

        TimePoint T = duration();
        if(forward){
            time = other.end() + padding;
            end_time = time + T;
        }
        else{
            end_time = other.start() - padding;
            time = end_time - T;
        }

        return true;
    }

    return false;
}

void CVCalendarRecord::shift(const TimePoint& distance){
    time += distance;
    end_time += distance;
    time.distribute();
    end_time.distribute();
}

float CVCalendarRecord::diff_years() const{
    return float(end_time.year() - time.year());
}

float CVCalendarRecord::diff_months() const{
    return float(end_time.total_months() - time.total_months());
}

float CVCalendarRecord::diff_days() const{
    return float(end_time.total_days() - time.total_days());
}

float CVCalendarRecord::diff_hours() const{
    return float(end_time.total_hours() - time.total_hours());
}

float CVCalendarRecord::diff_minutes() const{
    return float(end_time.total_minutes() - time.total_minutes());
}

float CVCalendarRecord::diff_seconds() const{
    return float(end_time.total_seconds() - time.total_seconds());
}

float CVCalendarRecord::median_years() const{
    return float(end_time.year() + time.year())/2;
}

float CVCalendarRecord::median_months() const{
    return float(end_time.total_months() + time.total_months())/2;
}

float CVCalendarRecord::median_days() const{
    return float(end_time.total_days() + time.total_days())/2;
}

float CVCalendarRecord::median_hours() const{
    return float(end_time.total_hours() + time.total_hours())/2;
}

float CVCalendarRecord::median_minutes() const{
    return float(end_time.total_minutes() + time.total_minutes())/2;
}

float CVCalendarRecord::median_seconds() const{
    return float(end_time.total_seconds() + time.total_seconds())/2;
}

std::ostream& operator<<(std::ostream& output, const CVCalendarRecord& input){

    output << input.start() << '\t' << input.end();
    if(!input.reminders.empty()){

        output << "\nReminders:";
        for(auto& rem : input.reminders){

            output << "\n\t" << rem;

        }

    }
    return output;

}

CVSchedule::CVSchedule():
    std::vector<CVCalendarRecord>(){
        color_legend["Business"]        =           sf::Color(0,0,160);
        color_legend["School"]          =           sf::Color(0,160,0);
        color_legend["Social"]          =           sf::Color(68,0,150);
        color_legend["Appointment"]     =           sf::Color::Yellow;
        color_legend["Work"]            =           sf::Color(170,170,170);
        color_legend["Reserved"]        =           sf::Color::Red;
        color_legend["Household"]       =           sf::Color(0,255,255);
        color_legend["Miscellaneous"]   =           sf::Color(128,128,128);
}

void CVSchedule::setLegend(const std::string& name, const sf::Color& color){
    color_legend[name] = color;
}

sf::Color CVSchedule::getColor(const std::string& name) const{
    auto end_itr = color_legend.end();
    for(auto itr = color_legend.begin(); itr != end_itr; ++itr){
        if(cmpString(itr->first, name)) return itr->second;
    }
    return sf::Color(128,128,128);
}

void CVSchedule::setLocation(const std::string& scheduleTag, const std::string& newLocation){
    for(auto& item : *this){
        if(cmpString(item.tag(), scheduleTag)){
            item.setLocation(newLocation);
        }
    }
}

std::string CVSchedule::getLocation(const std::string& scheduleTag) const{
    for(auto& item : *this){
        if(cmpString(item.tag(), scheduleTag)){
            return item.getLocation();
        }
    }
    return std::string();
}

std::string CVSchedule::getLocationItem(const std::string& locationTag) const{
    for(auto& item : *this){
        if(cmpString(item.getLocation(), locationTag)){
            return item.tag();
        }
    }
    return std::string();
}

std::string CVSchedule::getTimeTag(const TimePoint& time) const{
    for(auto& item : *this){
        if(item.check_overlap(time)) return item.tag();
    }
    return std::string();
}

bool CVSchedule::checkMatch(const std::string& scheduleTag) const{
    for(auto& item : *this){
        if(cmpString(item.tag(), scheduleTag)){
            return true;
        }
    }
    return false;
}

bool CVSchedule::checkLocation(const std::string& locationTag) const{
    for(auto& item : *this){
        if(cmpString(item.getLocation(), locationTag)){
            return true;
        }
    }
    return false;
}

bool CVSchedule::checkOverlap(const CVCalendarRecord& record) const{
    for(auto& item : *this){
        if(item.check_overlap(record)){
            return true;
        }
    }
    return false;
}

bool CVSchedule::checkOverlap(const CVSchedule& other) const{
    for(auto& item : other){
        if(checkOverlap(item)) return true;
    }
    return false;
}

std::vector<CVCalendarRecord*> CVSchedule::getOverlap(const CVCalendarRecord& record){
    std::vector<CVCalendarRecord*> output;

    for(auto& item : *this){
        if(item.check_overlap(record)){
            output.push_back(&item);
        }
    }

    return output;
}

std::vector<CVCalendarRecord*> CVSchedule::getOverlap(const CVSchedule& other){
    std::vector<CVCalendarRecord*> output, newOutput;

    for(auto& item : other){
        newOutput = getOverlap(item);
        if(!newOutput.empty()) output.insert(output.end(), newOutput.begin(), newOutput.end());
    }

    return output;
}

bool CVSchedule::save(const std::string& path) const{

    FILE* outFILE = fopen(path.c_str(), "wb");

    size_t L = size();
    fwrite(&L, 1, sizeof(size_t), outFILE);
    for(auto& item : *this){
        item.save(outFILE);
    }

    fclose(outFILE);

    return !access(path.c_str(), F_OK);

}

bool CVSchedule::load(const std::string& path){

    if(access(path.c_str(), F_OK)) return false;

    FILE* inFILE = fopen(path.c_str(), "rb");

    size_t L;
    fread(&L, 1, sizeof(L), inFILE);
    reserve(L);
    for(size_t i = 0; i < L; ++i){
        emplace_back();
        back().load(inFILE);
    }

    fclose(inFILE);

    return true;
}

std::vector<CVCalendarRecord*> check_availability(CVSchedule& records,
                        const TimePoint& begin_time,
                        const TimePoint& end_time){

    std::vector<CVCalendarRecord*> output;

    for(auto& item : records){
        if(item.check_overlap(begin_time) || item.check_overlap(end_time)){
           output.push_back(&item);
        }
    }

    return output;
}

bool addRecord(CVSchedule& records,
               const std::string& info,
                const TimePoint& time,
                const TimePoint& end_time,
                const time_scale& scale,
                const sf::Color& color){

    records.emplace_back(info, time, end_time, scale, color);
    return true;
}

bool changeRecord(CVSchedule& records,
                  const std::string& info,
                   const TimePoint& new_time,
                   const TimePoint& new_end_time){

    for(auto& item : records){
        if(cmpString(item.tag(), info)){
            item.setStart(new_time);
            item.setEnd(new_end_time);
            return true;
        }
    }

    return false;

}

bool changeRecord(CVSchedule& records,
                  const std::string& info,
                   const TimePoint& new_start_time){

    for(auto& item : records){
        if(cmpString(item.tag(), info)){

            TimePoint duration = item.end() - item.start();

            item.setStart(new_start_time);
            item.setEnd(new_start_time + duration);
            return true;
        }
    }

    return false;

}

bool changeRecords(CVSchedule& records,
                   const std::string& info,
                    const TimePoint& shift_time){

    bool bFound = false;

    for(auto& item : records){
        if(cmpString(item.tag(), info)){
            item.setEnd(item.start() + shift_time);
            bFound = true;
        }
    }

    return bFound;

}

bool changeRecord(CVSchedule& records,
                  const TimePoint& old_time,
                   const TimePoint& new_time,
                   const TimePoint& new_end_time,
                   const time_scale& scale){

    for(auto& item : records){
        switch(scale){
            case time_scale::year:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month())){
                        item.setStart(new_time);
                        item.setEnd(new_end_time);

                        return true;
                }
                break;
            }
            case time_scale::month:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month()) &&
                   (item.start().day() == old_time.day())){
                        item.setStart(new_time);
                        item.setEnd(new_end_time);

                        return true;
                }
                break;
            }
            default:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month()) &&
                   (item.start().day() == old_time.day()) &&
                   (item.start().hour() == old_time.hour()) &&
                   (item.start().minute() == old_time.minute())){
                        item.setStart(new_time);
                        item.setEnd(new_end_time);

                        return true;
                }
                break;
            }
        }
    }

    return false;
}

bool changeRecord(CVSchedule& records,
                  const TimePoint& old_time,
                  const TimePoint& new_time,
                  const time_scale& scale){

    for(auto& item : records){
        switch(scale){
            case time_scale::year:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month())){

                       TimePoint duration = item.end() - item.start();

                        item.setStart(new_time);
                        item.setEnd(new_time + duration);

                        return true;
                }
                break;
            }
            case time_scale::month:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month()) &&
                   (item.start().day() == old_time.day())){

                       TimePoint duration = item.end() - item.start();

                        item.setStart(new_time);
                        item.setEnd(new_time + duration);

                        return true;
                }
                break;
            }
            default:{
                if((item.start().year() == old_time.year()) &&
                   (item.start().month() == old_time.month()) &&
                   (item.start().day() == old_time.day()) &&
                   (item.start().hour() == old_time.hour()) &&
                   (item.start().minute() == old_time.minute())){

                       TimePoint duration = item.end() - item.start();

                        item.setStart(new_time);
                        item.setEnd(new_time + duration);

                        return true;
                }
                break;
            }
        }
    }

    return false;
}

bool removeRecord(CVSchedule& records,
                  const std::string& info){

    StringVector tags;

    for(auto& item : records){
        tags.emplace_back(item.tag());
    }

    unsigned int matchIndex = getMatchingIndex(info, tags);
    if(matchIndex == UINT_MAX) return false;

    records.erase(records.begin() + matchIndex);

    return true;
}

bool removeRecord(CVSchedule& records,
                  const TimePoint& time){

    unsigned int matchIndex = 0;

    for(auto& item : records){
        if(item.start() == time){
            records.erase(records.begin() + matchIndex);
            return true;
        }
        ++matchIndex;
    }

    return false;
}

void CVCalendarPanel::clear(){
    CVViewPanel::clear();

    headerBox = nullptr;
    fwdHeaderButton = nullptr;
    backHeaderButton = nullptr;

    headerBars.clear();
    rowBars.clear();
    plotSpace.clear();
    scheduleBlocks.clear();
}

void CVCalendarPanel::setTimeScale(const time_scale& newScale){

    scale = newScale;
    updateDisplay();

}

void CVCalendarPanel::setTime(const TimePoint& newTime){

    viewing_time = newTime;
    viewing_time.distribute();
    updateDisplay();

}

void CVCalendarPanel::moveTime(const TimePoint& duration){

    viewing_time += duration;
    viewing_time.distribute();
    updateDisplay();

}

void CVCalendarPanel::set24hour(const bool& status){

    b24hour = status;
    updateDisplay();

}

bool CVCalendarPanel::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    for(auto& item : plotSpace){
        item->draw(target);
    }
    for(auto& item : scheduleBlocks){
        item->draw(target);
    }
    for(auto& item : headerBars){
        item->draw(target);
    }
    for(auto& item : rowBars){
        item->draw(target);
    }

    headerBox->draw(target);
    backHeaderButton->draw(target);
    fwdHeaderButton->draw(target);

    scrollBarX.draw(target);
    scrollBarY.draw(target);

    CV_DRAW_CLIP_END

    return true;
}

bool CVCalendarPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(fwdHeaderButton->getTrigger()){

        moveTime(MONTHS(1));

    }
    if(backHeaderButton->getTrigger()){

        moveTime(MONTHS(-1));

    }

    sf::FloatRect itemBounds(left_margin(), top_margin(), 0.0f,0.0f);
    if(!plotSpace.empty()){
        sf::FloatRect otherBounds;
        itemBounds = plotSpace.front()->getBounds();
        otherBounds = plotSpace.back()->getBounds();
        if(otherBounds.left + otherBounds.width > itemBounds.left + itemBounds.width){
            itemBounds.width = otherBounds.left + otherBounds.width - itemBounds.left;
        }
        if(otherBounds.top + otherBounds.height > itemBounds.top + itemBounds.height){
            itemBounds.height = otherBounds.top + otherBounds.height - itemBounds.top;
        }
    }

    scheduleBounds = itemBounds;

    scrollBarY.setScrollMax(itemBounds.height);
    scrollBarX.setScrollMax(itemBounds.width);

    if(bounds.contains(mousePos) && event.focusFree()){
        setFocus(true);
        scrollBarY.setScrollable(true);
        scrollBarX.setScrollable(true);
    }
    else{
        setFocus(false);
        scrollBarY.setScrollable(false);
        scrollBarX.setScrollable(false);
    }

    if(bounds.contains(mousePos)) event.captureFocus();

    scrollBarX.update(event, mousePos);
    scrollBarY.update(event, mousePos);

    if(hasFocus()){
        sf::Vector2f moveDist(0.0f, 0.0f);
        if(itemBounds.height > bounds.height - top_margin() - bottom_margin()){
            moveDist.y = top_margin() + bounds.top -
                                       itemBounds.top -
                                       scrollBarY.getScrollOffset();
        }
        else{
            moveDist.y = bounds.top + top_margin() - itemBounds.top;
        }

        if(itemBounds.width > bounds.width - left_margin() - right_margin()){
            moveDist.x = left_margin() + bounds.left -
                                       itemBounds.left -
                                       scrollBarX.getScrollOffset();
        }
        else{
            moveDist.x = bounds.left + left_margin() - itemBounds.left;
        }

        for(auto& cell : plotSpace){
            cell->move(moveDist);
        }
        for(size_t i = 0; i < (int)rowBars.size() - 1; ++i){ // Skip corner bar
            rowBars[i]->move(0.0f, moveDist.y);
        }
        for(auto& cell : headerBars){
            cell->move(moveDist.x, 0.0f);
        }
        for(auto& cell : scheduleBlocks){
            cell->move(moveDist);
        }

        for(auto& element : rowBars){

            if((mousePos.y < element->getGlobalBounds().top + element->getGlobalBounds().height) &&
               (mousePos.y > element->getGlobalBounds().top)){

                element->highlight(true);

            }
            else element->highlight(false);

        }

        for(auto& element : headerBars){

            if((mousePos.x < element->getGlobalBounds().left + element->getGlobalBounds().width) &&
               (mousePos.x > element->getGlobalBounds().left)){

                element->highlight(true);

            }
            else element->highlight(false);

        }
    }

    return true;
}

void CVCalendarPanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
    scrollBarX.move(distance);
    scrollBarY.move(distance);
    scheduleBounds.left += distance.x;
    scheduleBounds.top += distance.y;
}

void CVCalendarPanel::move(const float& x, const float& y){
    move(sf::Vector2f(x, y));
}

void CVCalendarPanel::setPosition(const sf::Vector2f& newPosition){
    move(newPosition - getPosition());
}

void CVCalendarPanel::setPosition(const float& x, const float& y){
    setPosition(sf::Vector2f(x, y));
}

void CVCalendarPanel::centerOnTime(const TimePoint& time){

    switch(scale){
        default:{

            if(time.month() != viewing_time.month()) setTime(time);

            break;

        }

    }

    scrollBarX.setScrollOffset((bounds.width - left_margin())/7 * (float)time.day() - (bounds.width - left_margin())/2);
    scrollBarY.setScrollOffset((bounds.height - top_margin())*(((float)time.hour() +
                                                    float(time.minute())/60)/24) * 3 - (bounds.height - top_margin())/2);

}

bool CVCalendarPanel::addRecord(const std::string& info,
                                const TimePoint& time,
                                const TimePoint& end_time,
                                const time_scale& scale,
                                const std::string& legend_category){

    if(cvis::addRecord(records, info, time, end_time, scale, records.getColor(legend_category))){
        updateRecords();
        return true;
    }

    return false;
}

bool CVCalendarPanel::changeRecord(const std::string& info,
                                   const TimePoint& new_time,
                                   const TimePoint& new_end_time){

    if(cvis::changeRecord(records, info, new_time, new_end_time)){
        updateRecords();
        return true;
    }

    return false;

}

bool CVCalendarPanel::changeRecords(const std::string& info,
                                    const TimePoint& shift_time){

    if(cvis::changeRecords(records, info, shift_time)){
        updateRecords();
        return true;
    }

    return false;

}

bool CVCalendarPanel::changeRecord(const TimePoint& old_time,
                                   const TimePoint& new_time,
                                   const TimePoint& new_end_time,
                                   const time_scale& scale){

    if(cvis::changeRecord(records, old_time, new_time, new_end_time, scale)){
        updateRecords();
        return true;
    }

    return false;

}

bool CVCalendarPanel::removeRecord(const std::string& info){

    if(cvis::removeRecord(records, info)){
        updateRecords();
        return true;
    }

    return false;
}

bool CVCalendarPanel::removeRecord(const TimePoint& time){

    if(cvis::removeRecord(records, time)){
        updateRecords();
        return true;
    }

    return false;
}

void CVCalendarPanel::updateRecords(){

    for(auto& item : scheduleBlocks){
        removePanelElement(item);
    }
    scheduleBlocks.clear();

    sf::FloatRect newBounds;

    bool bAltFlag = false,
         bAltToggle = true;
    unsigned int altCount = 0;

    for(auto& item : records){

        bAltFlag = cmpStringToList("alt", item.getRepeatTags());

        switch(scale){
            default:{ // Weekly

                std::vector<CVCalendarRecord> rep_time(1, item);
                if(item.is_repeating()){

                    TimePoint end_t = item.start();

                    end_t.setYear(viewing_time.year());
                    end_t.setMonth(viewing_time.month());
                    end_t.setDay(days_in_month(viewing_time.month()));
                    end_t.setHour(23);
                    end_t.setMinute(59);
                    end_t.setSecond(59);
                    while(rep_time.back().start() <= (end_t - rep_time.front().repeat_frequency())){
                        rep_time.emplace_back(rep_time.back());
                        rep_time.back().shift(rep_time.front().repeat_frequency());

                    }

                }

                for(auto& record : rep_time){

                    if(record.start().month() != viewing_time.month()) continue;

                    if(bAltFlag){
                        if(bAltToggle){
                            bAltToggle = false;
                        }
                        else{
                            bAltToggle = true;
                            continue;
                        }
                    }

                    if(cmpStringToList(record.getRepeatTags(), calendar_days)){
                        if(!cmpStringToList(day_of_week(record.start()), record.getRepeatTags())) continue;
                    }

                    if(cmpStringToList(record.getRepeatTags(), calendar_months)){
                        if(!cmpStringToList(monthStr(record.start().month()), record.getRepeatTags())) continue;
                    }


                    TimePoint tmp_begin = record.start(),
                                tmp_end = record.start();

                    tmp_end.setHour(23);
                    tmp_end.setMinute(59);
                    tmp_end.setSecond(59);

                    do{

                        if(tmp_end > record.end()) tmp_end = record.end();

                        newBounds.left = scheduleBounds.left +
                                          (bounds.width - left_margin())*(tmp_begin.day()-1)/7;
                        newBounds.width = (bounds.width - left_margin())/7 - 2*tableOutlineThickness;
                        newBounds.top = scheduleBounds.top + (bounds.height - top_margin())*(((float)tmp_begin.hour() +
                                                                                                float(tmp_begin.minute())/60)/24) * 3;
                        newBounds.height = (bounds.height - top_margin())
                                * (((float)tmp_end.hour() + float(tmp_end.minute())/60 -
                                    (float)tmp_begin.hour() - float(tmp_begin.minute())/60)/24) * 3;

                        unsigned int brightness = 0;
                        brightness = (record.color().r + record.color().g + record.color().b)/3;
                        sf::Color textColor;
                        if(brightness > 80) textColor = sf::Color::Black;
                        else textColor = sf::Color::White;
                        textColor.a = scheduleBlockAlpha;

                        sf::Color blockColor = record.color();
                        blockColor.a = scheduleBlockAlpha;

                        scheduleBlocks.emplace_back(new CVTextBox(View, sf::Vector2f(newBounds.left, newBounds.top),
                                                                  newBounds.width, newBounds.height,
                                                                        textEntry(record.start().getTime(b24hour) +
                                                                                  " - " + record.end().getTime(b24hour) +
                                                                                              "\n" + record.tag(),
                                                                                  textInfo.font, textInfo.fontSize,
                                                                                ALIGN_LEFT, textColor),
                                                                        blockColor, cellBorderColor(), tableOutlineThickness));
                        scheduleBlocks.back()->setTextWrap(true);
                        scheduleBlocks.back()->setHighlightableStatus(true);

                        std::vector<bool> rounded_edges(4, false);
                        if(tmp_begin.day() == record.start().day()){

                            rounded_edges[0] = true;
                            rounded_edges[1] = true;

                        }
                        if(tmp_end.day() == record.end().day()){

                            rounded_edges[2] = true;
                            rounded_edges[3] = true;

                        }

                        scheduleBlocks.back()->setRounding(12.0f, 12, rounded_edges);
                        addPanelElement(scheduleBlocks.back());

                        tmp_begin.setHour(0);
                        tmp_begin.setMinute(0);
                        tmp_begin.setSecond(0);

                        ++tmp_begin.day();
                        ++tmp_end.day();

                    }while(tmp_end.day() <= record.end().day());

                }

                break;

            }
        }
    }
}

void CVCalendarPanel::updateDisplay(){

    clear();

    switch(scale){
        default:{   // Weekly

            unsigned int D = days_in_month(viewing_time.month());
            sf::Color expiredColor = cellFillColor();
            if(expiredColor.r + expiredColor.g + expiredColor.b > 384) darken(expiredColor, 20);
            else brighten(expiredColor, 20);

            TimePoint calendar_day(viewing_time.year(), viewing_time.month(), 0, 0, 0, 0);

            headerBox = new CVTextBox(View, sf::Vector2f(getPosition().x, getPosition().y), bounds.width, top_margin()/2,
                                                            textEntry(monthStr(viewing_time.month()) + " " + std::to_string(viewing_time.year()), textInfo.font, textInfo.fontSize*1.5f,
                                                                ALIGN_CENTER, textInfo.textColor),
                                                            headerColor(), cellBorderColor(), tableOutlineThickness);
            headerBox->setMask(appTexture("gradient_linear"));
            headerBox->setMaskColor(sf::Color(250,250,250,160));

            float buttonSize = headerBox->getGlobalBounds().height*5/8;

            backHeaderButton = new CVButton(View, sf::Vector2f(getPosition().x + headerBox->getGlobalBounds().width/3,
                                                               getPosition().y + headerBox->getGlobalBounds().height/2),
                                            buttonSize, buttonSize,
                                            "chevron_arrow", 1, 0, true);
            backHeaderButton->rotate(180.0f);
            fwdHeaderButton = new CVButton(View, sf::Vector2f(getPosition().x + headerBox->getGlobalBounds().width*2/3,
                                                              getPosition().y + headerBox->getGlobalBounds().height/2),
                                            buttonSize, buttonSize,
                                            "chevron_arrow", 1, 0, true);

            fwdHeaderButton->setSpriteColor(sf::Color(15, 171, 225, 200));
            backHeaderButton->setSpriteColor(sf::Color(15, 171, 225, 200));

            addPanelElement(headerBox, "header");
            addPanelElement(backHeaderButton, "backButton");
            addPanelElement(fwdHeaderButton, "frontButton");

            for(size_t i = 0; i < D; ++i){

                calendar_day.setDay(i + 1);
                std::stringstream headerStr;
                headerStr << i+1 << " - " << day_of_week(calendar_day);

                if(current_time - calendar_day > DAYS(1)){

                    plotSpace.emplace_back(new CVTextBox(View, sf::Vector2f(left_margin() + getPosition().x +
                                                                            (bounds.width - left_margin())*i/7,
                                                                             top_margin() + getPosition().y), (bounds.width - left_margin())/7,
                                                         3*(bounds.height - top_margin()),
                                                            expiredColor, cellBorderColor(), tableOutlineThickness));
                    addPanelElement(plotSpace.back());

                }
                else{
                    plotSpace.emplace_back(new CVTextBox(View, sf::Vector2f(left_margin() + getPosition().x +
                                                                            (bounds.width - left_margin())*i/7,
                                                                             top_margin() + getPosition().y), (bounds.width - left_margin())/7,
                                                         3*(bounds.height - top_margin()),
                                                            cellFillColor(), cellBorderColor(), tableOutlineThickness));
                    addPanelElement(plotSpace.back());

                    if((current_time.day() == calendar_day.day()) &&
                       (abs(current_time - calendar_day) < DAYS(1))){
                        plotSpace.emplace_back(new CVTextBox(View, sf::Vector2f(left_margin() + getPosition().x +
                                                                                (bounds.width - left_margin())*i/7,
                                                                             top_margin() + getPosition().y), (bounds.width - left_margin())/7,
                                                         3*(bounds.height - top_margin())*(((float)current_time.hour() + float(current_time.minute())/60)/24),
                                                            expiredColor, cellBorderColor(), tableOutlineThickness));
                        addPanelElement(plotSpace.back());
                    }
                }

                headerBars.emplace_back(new CVTextBox(View, sf::Vector2f(left_margin() + getPosition().x + (bounds.width - left_margin())*i/7,
                                                                         getPosition().y + top_margin()/2), (bounds.width - left_margin())/7, top_margin()/2,
                                                      textEntry(headerStr.str(), textInfo.font, textInfo.fontSize*1.3f,
                                                                textInfo.alignment, textInfo.textColor),
                                                        headerColor(), cellBorderColor(), tableOutlineThickness));
                headerBars.back()->setMask(appTexture("gradient_linear"));
                headerBars.back()->setMaskColor(sf::Color(200,200,200,150));
                addPanelElement(headerBars.back());

            }

            scheduleBounds.left = plotSpace.front()->getGlobalBounds().left;
            scheduleBounds.top = plotSpace.front()->getGlobalBounds().top;
            scheduleBounds.width = plotSpace.back()->getGlobalBounds().left +
                                    plotSpace.back()->getGlobalBounds().width -
                                    scheduleBounds.left;
            scheduleBounds.height = plotSpace.back()->getGlobalBounds().top +
                                    plotSpace.back()->getGlobalBounds().height -
                                    scheduleBounds.top;

            TimePoint sideBarTime;
            sideBarTime.setMinute(0);
            sideBarTime.setSecond(0);

            for(size_t i = 0; i < 24; ++i){
                sideBarTime.setHour(i);
                rowBars.emplace_back(new CVTextBox(View, sf::Vector2f(getPosition().x,
                                                                      tableOutlineThickness + top_margin() +
                                                                      getPosition().y +
                                                                      (bounds.height - top_margin())*i/8),
                                                   left_margin(), (bounds.height - top_margin())/8,
                                                   textEntry(sideBarTime.getTime(b24hour), textInfo.font, textInfo.fontSize*1.25f,
                                                                textInfo.alignment, textInfo.textColor),
                                                   sideBarColor(), cellBorderColor(), tableOutlineThickness));
                rowBars.back()->setMask(appTexture("gradient_linear"));
                rowBars.back()->setMaskColor(sf::Color(250,250,250,60));
                addPanelElement(rowBars.back());
            }

            rowBars.emplace_back(new CVTextBox(View, sf::Vector2f(getPosition().x, getPosition().y + tableOutlineThickness),
                                               left_margin(), top_margin(),
                                               sideBarColor(), cellBorderColor(), tableOutlineThickness));
            rowBars.back()->setMask(appTexture("gradient_linear"));
            rowBars.back()->setMaskColor(sf::Color(250,250,250,60));
            addPanelElement(rowBars.back());

            break;
        }
    }

    sf::FloatRect itemBounds(left_margin(), top_margin(), 0.0f,0.0f);
    if(!plotSpace.empty()){
        sf::FloatRect otherBounds;
        itemBounds = plotSpace.front()->getBounds();
        otherBounds = plotSpace.back()->getBounds();
        if(otherBounds.left + otherBounds.width > itemBounds.left + itemBounds.width){
            itemBounds.width = otherBounds.left + otherBounds.width - itemBounds.left;
        }
        if(otherBounds.top + otherBounds.height > itemBounds.top + itemBounds.height){
            itemBounds.height = otherBounds.top + otherBounds.height - itemBounds.top;
        }
    }

    scheduleBounds = itemBounds;

    scrollBarY.setScrollMax(itemBounds.height);
    scrollBarX.setScrollMax(itemBounds.width);

    updateRecords();

}

void CVCalendarPanel::save(const std::string& path) const{
    records.save(path);
}

bool CVCalendarPanel::load(const std::string& path){
    if(records.load(path)){
        updateRecords();
        return true;
    }
    return false;
}

void CVCalendarPanel::load(const CVSchedule& records){
    this->records = records;
    updateRecords();
}

}
