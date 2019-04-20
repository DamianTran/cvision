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

#pragma once

#ifndef CVIS_CALENDAR
#define CVIS_CALENDAR

#include <EZC/toolkit/time.hpp>

#include "cvision/widgets.hpp"
#include "cvision/panel.hpp"

namespace cvis{

class CVISION_API CVCalendarRecord{
protected:

    bool bRepeat;

    std::string                     info;
    std::string                     location;
    std::string                     notes;

    std::vector<std::string>        repeatTags;

    EZC::TimePoint                  time;
    EZC::TimePoint                  end_time;
    EZC::TimePoint                  frequency;

    std::vector<EZC::TimePoint>      reminders;

    EZC::time_scale scale;
    sf::Color color_code;

public:

    CVISION_API const std::string& tag() const;
    CVISION_API void setTag(const std::string& newTag);

    CVISION_API void setLocation(const std::string& newLocation);
    CVISION_API const std::string& getLocation() const;

    CVISION_API void setRepeatTags(const std::vector<std::string>& newTags);
    CVISION_API const std::vector<std::string>& getRepeatTags() const;

    CVISION_API const bool& is_repeating() const;
    CVISION_API void set_repeat(const EZC::TimePoint& interval);
    CVISION_API void clear_repeat();
    CVISION_API const EZC::TimePoint& repeat_frequency() const;

    CVISION_API const EZC::TimePoint& start() const;
    CVISION_API const EZC::TimePoint& end() const;
    CVISION_API EZC::TimePoint getDuration() const;
    CVISION_API std::string getDurationStr() const; // Get a string of the duration of this event

    CVISION_API void setStart(const EZC::TimePoint& newStart);
    CVISION_API void setEnd(const EZC::TimePoint& newEnd);

    CVISION_API const EZC::time_scale& getScale() const;
    CVISION_API void setScale(const EZC::time_scale& newScale);

    CVISION_API void addReminder(const EZC::TimePoint& time_advance);    // Set a reminder time for this record
    CVISION_API bool flagReminder(const EZC::TimePoint& time);           // Check if it's time to remind and if so, turn off the flag

    CVISION_API const sf::Color& color() const;
    CVISION_API void setColor(const sf::Color& newColor);

    CVISION_API void save(FILE* outFILE) const;
    CVISION_API void load(FILE* inFILE);

    CVISION_API bool check_overlap(const EZC::TimePoint& time) const;
    CVISION_API bool check_overlap(const CVCalendarRecord& other) const;

    CVISION_API EZC::TimePoint duration() const;

    CVISION_API bool shift(const CVCalendarRecord& other, const EZC::TimePoint& padding = MINUTES(15),
               const bool& forward = true);
    CVISION_API void shift(const EZC::TimePoint& distance);

    CVISION_API float diff_years() const;
    CVISION_API float diff_months() const;
    CVISION_API float diff_days() const;
    CVISION_API float diff_hours() const;
    CVISION_API float diff_minutes() const;
    CVISION_API float diff_seconds() const;

    CVISION_API float median_years() const;
    CVISION_API float median_months() const;
    CVISION_API float median_days() const;
    CVISION_API float median_hours() const;
    CVISION_API float median_minutes() const;
    CVISION_API float median_seconds() const;

    CVISION_API friend std::ostream& operator<<(std::ostream& output, const CVCalendarRecord& input);

    CVISION_API CVCalendarRecord();
    CVISION_API CVCalendarRecord(const std::string& newInfo,
           const EZC::TimePoint& start,
           const EZC::TimePoint& end,
           const EZC::time_scale& scale,
           const sf::Color& color = sf::Color(0,17,150));
};

class CVISION_API CVSchedule: public std::vector<CVCalendarRecord>{
protected:

    std::map<std::string, sf::Color> color_legend;

public:

    CVISION_API void setLegend(const std::string& name, const sf::Color& color);
    CVISION_API sf::Color getColor(const std::string& name) const;

    CVISION_API void setLocation(const std::string& scheduleTag, const std::string& newLocation);
    CVISION_API std::string getLocation(const std::string& scheduleTag) const;
    CVISION_API std::string getLocationItem(const std::string& scheduleTag) const;
    CVISION_API std::string getTimeTag(const EZC::TimePoint& beginTime) const;

    CVISION_API bool checkOverlap(const CVCalendarRecord& record) const;
    CVISION_API bool checkOverlap(const CVSchedule& other) const;

    CVISION_API std::vector<CVCalendarRecord*> getOverlap(const CVCalendarRecord& record);
    CVISION_API std::vector<CVCalendarRecord*> getOverlap(const CVSchedule& other);

    CVISION_API bool checkMatch(const std::string& scheduleTag) const;
    CVISION_API bool checkLocation(const std::string& locationTag) const;

    CVISION_API bool save(const std::string& path) const;
    CVISION_API bool load(const std::string& path);

    CVISION_API CVSchedule();

};

CVISION_API std::vector<CVCalendarRecord*> check_availability(CVSchedule& records,
                        const EZC::TimePoint& begin_time,
                        const EZC::TimePoint& end_time);

CVISION_API bool addRecord(CVSchedule& records,
               const std::string& info,
               const EZC::TimePoint& time,
               const EZC::TimePoint& end_time,
               const EZC::time_scale& scale = EZC::time_scale::any,
               const sf::Color& color_code = sf::Color(0,17,150));

CVISION_API bool changeRecord(CVSchedule& records,
                  const std::string& info,
                  const EZC::TimePoint& time,
                  const EZC::TimePoint& end_time); // Change an entry by name
CVISION_API bool changeRecord(CVSchedule& records,
                  const std::string& info,
                  const EZC::TimePoint& new_start_time); // Change the start time of a record
CVISION_API bool changeRecords(CVSchedule& records,
                   const std::string& info,
                   const EZC::TimePoint& time_change);    // Change groups of entries by common expression, shifted by time frame

CVISION_API bool changeRecord(CVSchedule& records,
                  const EZC::TimePoint& old_time,    // Change a record by time span
                  const EZC::TimePoint& new_time,
                  const EZC::TimePoint& new_end_time,
                  const EZC::time_scale& scale = EZC::time_scale::any);

CVISION_API bool changeRecord(CVSchedule& records,
                  const EZC::TimePoint& old_time,
                  const EZC::TimePoint& new_time,
                  const EZC::time_scale& scale = EZC::time_scale::any);   // Change a record from old time to new time

CVISION_API bool removeRecord(CVSchedule& records,
                  const std::string& info);

CVISION_API bool removeRecord(CVSchedule& records,
                  const EZC::TimePoint& start_time);

class CVISION_API CVCalendarPanel: public CVBasicViewPanel{
protected:

    // Current view time

    EZC::time_scale scale;
    EZC::TimePoint viewing_time,
            current_time;

    unsigned int headerColorIndex,
                sideBarColorIndex,
                innerLineColorIndex,
                cellBorderColorIndex,
                cellFillColorIndex;

    uint8_t scheduleBlockAlpha;

    bool b24hour;

    float tableOutlineThickness;

    std::vector<float> margins;

    CVSchedule records;

    std::map<std::string, sf::Color> color_legend;

    CVTextBox* headerBox;
    std::vector<CVTextBox*> headerBars,
                            rowBars,
                            plotSpace,
                            scheduleBlocks;

    CVButton* backHeaderButton,
           * fwdHeaderButton;

    sf::FloatRect scheduleBounds;

    CVScrollBar scrollBarX,
                scrollBarY;

    inline sf::Color& headerColor(){ return colorTheme[headerColorIndex]; }
    inline sf::Color& sideBarColor(){ return colorTheme[sideBarColorIndex]; }
    inline sf::Color& innerLineColor(){ return colorTheme[innerLineColorIndex]; }
    inline sf::Color& cellBorderColor(){ return colorTheme[cellBorderColorIndex]; }
    inline sf::Color& cellFillColor(){ return colorTheme[cellFillColorIndex]; }

    CVISION_API void updateDisplay();
    CVISION_API void updateRecords();

public:

    inline const float& left_margin() const{ return margins[0]; }
    inline const float& top_margin() const{ return margins[1]; }
    inline const float& right_margin() const{ return margins[2]; }
    inline const float& bottom_margin() const{ return margins[3]; }

    CVISION_API void setTimeScale(const EZC::time_scale& newScale);

    CVISION_API void setTime(const EZC::TimePoint& newTime);
    CVISION_API void moveTime(const EZC::TimePoint& duration);

    CVISION_API void set24hour(const bool& status = true);

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API void move(const sf::Vector2f& distance);
    CVISION_API void move(const float& x, const float& y);

    CVISION_API void setPosition(const sf::Vector2f& newPos);
    CVISION_API void setPosition(const float& x, const float& y);

    CVISION_API void centerOnTime(const EZC::TimePoint& newTime);

    CVISION_API bool removeRecord(const std::string& info);
    CVISION_API bool removeRecord(const EZC::TimePoint& time);

    CVISION_API void save(const std::string& path) const;
    CVISION_API bool load(const std::string& path);

    CVISION_API void load(const CVSchedule& records);

    CVISION_API void clear();

    CVISION_API bool addRecord(const std::string& info,
                   const EZC::TimePoint& time,
                   const EZC::TimePoint& end_time,
                   const EZC::time_scale& scale,
                   const std::string& legend_category);

    CVISION_API bool changeRecord(const std::string& info,
                      const EZC::TimePoint& time,
                      const EZC::TimePoint& end_time); // Change an entry by name
    CVISION_API bool changeRecords(const std::string& info,
                       const EZC::TimePoint& time_change);    // Change groups of entries by common expression, shifted by time frame

    CVISION_API bool changeRecord(const EZC::TimePoint& old_time,    // Change a record by time span
                      const EZC::TimePoint& new_time,
                      const EZC::TimePoint& new_end_time,
                      const EZC::time_scale& scale);

    CVISION_API CVCalendarPanel(CVView* parentView, const std::string& panelTag = "",
                    const sf::Color& bkgColor = sf::Color::White,
                    const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                    const bool& bFitWindow = true,
                    const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_CALENDAR
