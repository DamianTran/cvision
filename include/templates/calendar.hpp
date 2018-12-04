#pragma once

#ifndef CVIS_CALENDAR
#define CVIS_CALENDAR

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_time.hpp"
#include "cvision/widgets.hpp"
#include "cvision/panel.hpp"

namespace cvis{

class CVCalendarRecord{
protected:

    bool bRepeat;

    std::string info,
                location;

    std::vector<std::string> repeatTags;

    TimePoint time,
            end_time,
            frequency;

    time_scale scale;
    sf::Color color_code;

public:

    const std::string& tag() const;
    void setTag(const std::string& newTag);

    void setLocation(const std::string& newLocation);
    const std::string& getLocation() const;

    void setRepeatTags(const std::vector<std::string>& newTags);
    const std::vector<std::string>& getRepeatTags() const;

    const bool& is_repeating() const;
    void set_repeat(const TimePoint& interval);
    void clear_repeat();
    const TimePoint& repeat_frequency() const;

    const TimePoint& start() const;
    const TimePoint& end() const;

    void setStart(const TimePoint& newStart);
    void setEnd(const TimePoint& newEnd);

    const time_scale& getScale() const;
    void setScale(const time_scale& newScale);

    const sf::Color& color() const;
    void setColor(const sf::Color& newColor);

    void save(FILE* outFILE) const;
    void load(FILE* inFILE);

    bool check_overlap(const TimePoint& time) const;
    bool check_overlap(const CVCalendarRecord& other) const;

    TimePoint duration() const;

    bool shift(const CVCalendarRecord& other, const TimePoint& padding = MINUTES(15),
               const bool& forward = true);
    void shift(const TimePoint& distance);

    float diff_years() const;
    float diff_months() const;
    float diff_days() const;
    float diff_hours() const;
    float diff_minutes() const;
    float diff_seconds() const;

    float median_years() const;
    float median_months() const;
    float median_days() const;
    float median_hours() const;
    float median_minutes() const;
    float median_seconds() const;

    CVCalendarRecord();
    CVCalendarRecord(const std::string& newInfo,
           const TimePoint& start,
           const TimePoint& end,
           const time_scale& scale,
           const sf::Color& color = sf::Color(0,17,150));
};

class CVSchedule: public std::vector<CVCalendarRecord>{
protected:

    std::map<std::string, sf::Color> color_legend;

public:

    void setLegend(const std::string& name, const sf::Color& color);
    sf::Color getColor(const std::string& name) const;

    void setLocation(const std::string& scheduleTag, const std::string& newLocation);
    std::string getLocation(const std::string& scheduleTag) const;
    std::string getLocationItem(const std::string& scheduleTag) const;
    std::string getTimeTag(const TimePoint& beginTime) const;

    bool checkOverlap(const CVCalendarRecord& record) const;
    bool checkOverlap(const CVSchedule& other) const;

    std::vector<CVCalendarRecord*> getOverlap(const CVCalendarRecord& record);
    std::vector<CVCalendarRecord*> getOverlap(const CVSchedule& other);

    bool checkMatch(const std::string& scheduleTag) const;
    bool checkLocation(const std::string& locationTag) const;

    bool save(const std::string& path) const;
    bool load(const std::string& path);

    CVSchedule();

};

std::vector<CVCalendarRecord*> check_availability(CVSchedule& records,
                        const TimePoint& begin_time,
                        const TimePoint& end_time);

bool addRecord(CVSchedule& records,
               const std::string& info,
               const TimePoint& time,
               const TimePoint& end_time,
               const time_scale& scale = time_scale::any,
               const sf::Color& color_code = sf::Color(0,17,150));

bool changeRecord(CVSchedule& records,
                  const std::string& info,
                  const TimePoint& time,
                  const TimePoint& end_time); // Change an entry by name
bool changeRecord(CVSchedule& records,
                  const std::string& info,
                  const TimePoint& new_start_time); // Change the start time of a record
bool changeRecords(CVSchedule& records,
                   const std::string& info,
                   const TimePoint& time_change);    // Change groups of entries by common expression, shifted by time frame

bool changeRecord(CVSchedule& records,
                  const TimePoint& old_time,    // Change a record by time span
                  const TimePoint& new_time,
                  const TimePoint& new_end_time,
                  const time_scale& scale = time_scale::any);

bool changeRecord(CVSchedule& records,
                  const TimePoint& old_time,
                  const TimePoint& new_time,
                  const time_scale& scale = time_scale::any);   // Change a record from old time to new time

bool removeRecord(CVSchedule& records,
                  const std::string& info);

bool removeRecord(CVSchedule& records,
                  const TimePoint& start_time);

class CVCalendarPanel: public CVBasicViewPanel{
protected:

    // Current view time

    time_scale scale;

    size_t year;
    unsigned int day,
                month,
                hour,
                minute,
                second;

    unsigned int headerColorIndex,
                sideBarColorIndex,
                innerLineColorIndex,
                cellBorderColorIndex,
                cellFillColorIndex;

    uint8_t scheduleBlockAlpha;

    float tableOutlineThickness;

    std::vector<float> margins;

    CVSchedule records;

    std::map<std::string, sf::Color> color_legend;

    CVTextBox* headerBox;
    std::vector<CVTextBox*> headerBars,
                            rowBars,
                            plotSpace,
                            scheduleBlocks;

    sf::FloatRect scheduleBounds;

    CVScrollBar scrollBarX,
                scrollBarY;

    inline sf::Color& headerColor(){ return colorTheme[headerColorIndex]; }
    inline sf::Color& sideBarColor(){ return colorTheme[sideBarColorIndex]; }
    inline sf::Color& innerLineColor(){ return colorTheme[innerLineColorIndex]; }
    inline sf::Color& cellBorderColor(){ return colorTheme[cellBorderColorIndex]; }
    inline sf::Color& cellFillColor(){ return colorTheme[cellFillColorIndex]; }

    void updateRecords();

public:

    inline const float& left_margin() const{ return margins[0]; }
    inline const float& top_margin() const{ return margins[1]; }
    inline const float& right_margin() const{ return margins[2]; }
    inline const float& bottom_margin() const{ return margins[3]; }

    void setTimeScale(const time_scale& newScale);

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void move(const sf::Vector2f& distance);
    void move(const float& x, const float& y);

    void setPosition(const sf::Vector2f& newPos);
    void setPosition(const float& x, const float& y);

    bool removeRecord(const std::string& info);
    bool removeRecord(const TimePoint& time);

    void save(const std::string& path) const;
    bool load(const std::string& path);

    void load(const CVSchedule& records);

    void clear();

    bool addRecord(const std::string& info,
                   const TimePoint& time,
                   const TimePoint& end_time,
                   const time_scale& scale,
                   const std::string& legend_category);

    bool changeRecord(const std::string& info,
                      const TimePoint& time,
                      const TimePoint& end_time); // Change an entry by name
    bool changeRecords(const std::string& info,
                       const TimePoint& time_change);    // Change groups of entries by common expression, shifted by time frame

    bool changeRecord(const TimePoint& old_time,    // Change a record by time span
                      const TimePoint& new_time,
                      const TimePoint& new_end_time,
                      const time_scale& scale);

    CVCalendarPanel(CVView* parentView, const std::string& panelTag = "",
                    const sf::Color& bkgColor = sf::Color::White,
                    const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                    const bool& bFitWindow = true,
                    const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_CALENDAR
