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

class CVISION_API CVMapPanel: public CVBasicViewPanel{
protected:

    sf::Rect<int> coord_bounds; // Virtual coordinate bounds
    sf::FloatRect outer_bounds,
                    init_bounds; // Outer boundary where interactions become effective

    sf::Vector2i click_coords,
                release_coords; // Interactive tracking

    bool select_rows, // Allow selection of rows or columns from boundary
        row_selected,
        select_cols,
        col_selected,
        displayInfo,    // Show extra info at bottom
        coords_captured;    // Check if coords have been read since last update

    float outerSelectDist, // Distance from outer border where row-wise selection is instigated
            zoomLevel,
            zoomMax, zoomMin;
    unsigned int textEntryIndex,
                    extraInfoIndex;

    sf::RectangleShape highlight,
                        selection;
    rounded_rectangle base;

    class zone : public sf::RectangleShape{
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

public:

    inline const sf::Rect<int>& getCoordBounds() const{ return coord_bounds; }

    void setZoneColor(const std::string& tag, const sf::Color& newColor);
    void setZoneColor(const unsigned int& index, const sf::Color& newColor);

    unsigned int numZones() const;
    bool zoneExists(const std::string& tag) const;

    void addZone(const std::string& name, const sf::Color& newColor,
                 const sf::Rect<int>& bounds);
    void removeZone(const std::string& name);
    void removeZone(const unsigned int& index);

    void updateZone(const std::string& name, const sf::Vector2i& newCoords);
    void updateZone(const std::string& name, const sf::Rect<int>& newCoords);
    void updateZone(const unsigned int& index, const sf::Vector2i& newCoords);
    void updateZone(const unsigned int& index, const sf::Rect<int>& newCoords);

    void setZoneImage(const std::string& name, const sf::Texture& image);
    void setZoneImage(const unsigned int& index, const sf::Texture& image);

    void setRowSelect(const bool& status = true);
    void setColumnSelect(const bool& status = true);
    void showExtraInfo(const bool& status = true);

    void setCoords(const sf::Rect<int>& coords,
                   const bool& morph = true);
    void setOuterPadding(const float& padding);
    void setZoom(float newZoom, sf::Vector2f mousePos = sf::Vector2f(NAN, NAN));

    sf::Rect<int> selected_bounds() const;
    inline const sf::Vector2i select_begin() const{ return click_coords; }
    inline const sf::Vector2i select_end() const{ return release_coords; }
    bool sectionSelected() const;
    bool pointSelected() const;

    std::string getTitle() const;
    void setTitle(const std::string& newTitle);

    std::string getInfo() const;

    bool captureCoords(sf::Rect<int>& output); // Attempt to read new coords

    void move(const sf::Vector2f& distance);
    void move(const float& x, const float& y);

    void shiftMap(const sf::Vector2f& distance);
    void setCenter(const sf::Vector2i& coords);

    void setPosition(const sf::Vector2f& position);
    void setPosition(const float& x, const float& y);

    void setBaseColor(const sf::Color& color);
    sf::Color baseColor() const;

    sf::FloatRect basePanelBounds() const;

    void clear();

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    sf::Vector2i globalToCoords(const sf::Vector2f& position) const;
    sf::Vector2f coordsToGlobal(const sf::Vector2i& coords) const;

    CVMapPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

}

#endif // CVIS_MAP_PANEL
