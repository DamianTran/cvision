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

#ifndef CVIS_NODE
#define CVIS_NODE

#include "cvision/lib.hpp"
#include "cvision/panel.hpp"

namespace cvis
{

class CVISION_API CVNodePanel : public CVBasicViewPanel{    // Panel items are distributed radially around source
public:

    CVISION_API CVNodePanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setHoverTag(const bool& status);

    CVISION_API void setNodeAlpha(const uint8_t& newAlpha);
    inline const bool& isOpen() const{ return bNodeOpened; }

    CVISION_API void setNode(const int& node);
    CVISION_API void setNodeRadius(const float& newRadiusPx);

    inline sf::Vector2f getPosition() const{
        if(center != nullptr) return center->getPosition();
        return anchorPoint; }

    CVISION_API std::string lastInteracted() const;
    CVISION_API bool captureInteraction(std::string* output = nullptr);

    CVISION_API void setCenter(CVElement& element); // Set the element to be centered
    CVISION_API void setCenterAnchor(const sf::Vector2f& position); // Set the anchor point to center the node map

    CVISION_API void addPanelElement(CVElement* newElement, std::string newTag = "");
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void getConnectedNodes(std::vector<CVElement*>& output) noexcept;

protected:

    CVElement*      center;

    int             nodeIndex;

    unsigned int    maxTraceLength;

    uint8_t         nodeAlpha;

    bool            bAutoPan;
    bool            bNodeOpened;
    bool            bNodeInteracted;
    bool            bShowTagOnHover;
    bool            bIsChild;

    float           panRate;
    float           nodeDistance;       // Distance where potential is zero
    float           elasticCoef;        // Proportional to force tugging nodes together
    float           pathThickness;
    float           fRepulsionCoef;      // Force pushing nodes apart

    sf::Vector2f    anchorPoint;

    std::vector<sf::RectangleShape>     nodePaths;
    std::vector<CVNodePanel*>           branches;
    std::vector<sf::Text>               hoverTags;

    std::vector<bool>                   pathSelected;     // Record of selected paths

    std::vector<std::string>            interactionTrace;

    void logInteraction(const std::string& tag);

    void updatePhysics(cvis::CVEvent& event, const sf::Vector2f& mousePos);
    void updateBounds() override;

};

}

#endif // CVIS_NODE
