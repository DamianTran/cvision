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

#ifndef CVIS_MAP_PANEL
#define CVIS_MAP_PANEL

#include "cvision/panel.hpp"

namespace cvis
{

class CVISION_API CVMapPanel: public CVBasicViewPanel
{
public:

    CVISION_API CVMapPanel(CVView* parentView,
               const std::string& panelTag = "",
               const sf::Color& backgroundColor = sf::Color::Transparent,
               const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
               const bool& bFitToWindow = true,
               const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    inline const sf::Rect<int>& getCoordBounds() const
    {
        return coord_bounds;
    }

    CVISION_API void setZoneColor(const std::string& tag, const sf::Color& newColor);
    CVISION_API void setZoneColor(const unsigned int& index, const sf::Color& newColor);

    CVISION_API unsigned int numZones() const;
    CVISION_API bool zoneExists(const std::string& tag) const;

    CVISION_API void addZone(const std::string& name, const sf::Color& newColor,
                 const sf::Rect<int>& bounds);
    CVISION_API void removeZone(const std::string& name);
    CVISION_API void removeZone(const unsigned int& index);

    CVISION_API void updateZone(const std::string& name, const sf::Vector2i& newCoords);
    CVISION_API void updateZone(const std::string& name, const sf::Rect<int>& newCoords);
    CVISION_API void updateZone(const unsigned int& index, const sf::Vector2i& newCoords);
    CVISION_API void updateZone(const unsigned int& index, const sf::Rect<int>& newCoords);

    CVISION_API void setZoneImage(const std::string& name, const sf::Texture& image);
    CVISION_API void setZoneImage(const unsigned int& index, const sf::Texture& image);

    CVISION_API void setRowSelect(const bool& status = true);
    CVISION_API void setColumnSelect(const bool& status = true);
    CVISION_API void showExtraInfo(const bool& status = true);

    CVISION_API void setCoords(const sf::Rect<int>& coords,
                   const bool& morph = true);
    CVISION_API void setOuterPadding(const float& padding);
    CVISION_API void setZoom(float newZoom, sf::Vector2f mousePos = sf::Vector2f(NAN, NAN));
    CVISION_API void resetZoom();

    CVISION_API sf::Rect<int> selected_bounds() const;
    inline const sf::Vector2i select_begin() const
    {
        return click_coords;
    }
    inline const sf::Vector2i select_end() const
    {
        return release_coords;
    }
    CVISION_API bool sectionSelected() const;
    CVISION_API bool pointSelected() const;

    CVISION_API std::string getTitle() const;
    CVISION_API void setTitle(const std::string& newTitle);

    CVISION_API std::string getInfo() const;

    CVISION_API bool captureCoords(sf::Rect<int>& output); // Attempt to read new coords

    CVISION_API void move(const sf::Vector2f& distance);
    CVISION_API void move(const float& x, const float& y);

    CVISION_API void shiftMap(const sf::Vector2f& distance);
    CVISION_API void resetShift();
    CVISION_API void setCenter(const sf::Vector2i& coords);

    CVISION_API void setPosition(const sf::Vector2f& position);
    CVISION_API void setPosition(const float& x, const float& y);

    CVISION_API void setBaseColor(const sf::Color& color);
    CVISION_API sf::Color baseColor() const;

    CVISION_API sf::FloatRect basePanelBounds() const;

    CVISION_API void clear();

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API sf::Vector2i globalToCoords(const sf::Vector2f& position) const;
    CVISION_API sf::Vector2f coordsToGlobal(const sf::Vector2i& coords) const;

protected:

    sf::Rect<int>       coord_bounds;     // Virtual coordinate bounds

    sf::FloatRect       outer_bounds;
    sf::FloatRect       init_bounds;      // Outer boundary where interactions become effective

    sf::Vector2i        click_coords;
    sf::Vector2i        release_coords;   // Interactive tracking

    sf::Vector2f        map_shift;        // Current offset of the map

    bool                select_rows;      // Allow selection of rows or columns from boundary
    bool                row_selected;
    bool                select_cols;
    bool                col_selected;
    bool                displayInfo;      // Show extra info at bottom
    bool                coords_captured;  // Check if coords have been read since last update

    float               outerSelectDist;  // Distance from outer border where row-wise selection is instigated
    float               zoomLevel;
    float               zoomMax;
    float               zoomMin;

    unsigned int        TextEntryIndex;
    unsigned int        extraInfoIndex;

    sf::RectangleShape  highlight;
    sf::RectangleShape  selection;

    rounded_rectangle   base;

    class zone : public sf::RectangleShape
    {
    public:

        CVMapPanel* parent;

        sf::Rect<int> bounds;
        std::string name;

        void updateCoords(const sf::Rect<int>& newCoords);
        void updateCoords(const sf::Vector2i& newCoords);
        void updateCoords();

        zone(CVMapPanel* parent);
        zone(CVMapPanel* parent, const std::string& name,
             const sf::Rect<int>& bounds, const sf::Color& color);
    };

    std::vector<zone> zones; // Zones making up this map

    sf::Text& extraInfoText();
    sf::Text& titleText();

};

}

#endif // CVIS_MAP_PANEL
