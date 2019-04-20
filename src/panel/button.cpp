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

#include "cvision/panel/button.hpp"
#include "cvision/button.hpp"
#include "cvision/event.hpp"

using namespace std;

namespace cvis
{

CVButtonPanel::CVButtonPanel(CVView* parentView, string panelTag, sf::Color backgroundColor,
                             const sf::Vector2f& size, bool bFitToWindow,
                             const sf::Vector2f& position):
    CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
    outerPadding(12.0f),
    innerPadding(1.0f),
    fButtonRoundingRadius(0.0f),
    fButtonOutlineThickness(0.0f),
    buttonFillColorIndex(colorTheme.size()),
    buttonOutlineColorIndex(colorTheme.size() + 1),
    buttonRoundPoints(12),
    buttonRoundStates(4, true)
{

    colorTheme.resize(colorTheme.size() + 2, sf::Color::Transparent);

}

bool CVButtonPanel::buttonsShareGroup(const CVButton* btn1, const CVButton* btn2) const noexcept
{

    if(!btn1 || !btn2) return false;

    bool btn1Found;
    bool btn2Found;

    for(auto& pair : buttonGroups)
    {

        btn1Found = false;
        btn2Found = false;

        for(auto& btn : pair.second)
        {
            if(btn == btn1)
            {
                btn1Found = true;
            }
            if(btn == btn2)
            {
                btn2Found = true;
            }
        }

        if(btn1Found && btn2Found)
        {
            return true;
        }

    }

    return false;
}

bool CVButtonPanel::buttonBelongsToGroup(const sf::Vector2i& coords) const noexcept
{

    const CVButton * search_btn = getButton(coords);

    if(!search_btn) return false;

    for(auto& pair : buttonGroups)
    {

        for(auto& btn : pair.second)
        {
            if(btn == search_btn)
            {
                return true;
            }
        }

    }

    return false;
}

void CVButtonPanel::organizeGrid()
{
    for(size_t y = 0, x; y < gridSize.y; ++y)
    {
        for(x = 0; x < gridSize.x; ++x)
        {
            if(buttonGrid[y][x] != nullptr)
            {
                buttonGrid[y][x]->setSize(sf::Vector2f((bounds.width - outerPadding*2 - innerPadding * (gridSize.x - 1))/gridSize.x,
                                                       (bounds.height - outerPadding*2 - innerPadding * (gridSize.y - 1))/gridSize.y));
                buttonGrid[y][x]->setPosition(getGridPosition(sf::Vector2i(x, y)));
            }
        }
    }
}

bool CVButtonPanel::checkInteraction() const
{
    return !interactTag.empty();
}

void CVButtonPanel::setHighlight(const sf::Vector2i& coord)
{
    for(size_t y = 0, x; y < buttonGrid.size(); ++y)
    {
        for(x = 0; x < buttonGrid[y].size(); ++x)
        {
            if((y == coord.y) && (x == coord.x) && buttonGrid[y][x])
            {
                buttonGrid[y][x]->highlight(true);
            }
            else if(buttonGrid[y][x] && buttonsShareGroup(coord, sf::Vector2i(x, y)))
            {
                buttonGrid[y][x]->highlight(false);
            }
        }
    }
}

void CVButtonPanel::clearHighlight()
{
    for(size_t y = 0, x; y < buttonGrid.size(); ++y)
    {
        for(x = 0; x < buttonGrid[y].size(); ++x)
        {
            if(buttonGrid[y][x])
            {
                buttonGrid[y][x]->highlight(false);
            }
        }
    }
}

void CVButtonPanel::createButtonGroup(const string& newGroup) noexcept
{
    buttonGroups.emplace(newGroup, vector<CVButton*>());
}

void CVButtonPanel::removeButtonGroup(const string& groupName) noexcept
{
    buttonGroups.erase(groupName);
}

void CVButtonPanel::addButtonToGroup(const sf::Vector2i& coords,
                                     const string& group) noexcept
{
    buttonGroups[group].emplace_back(getButton(coords));
}

void CVButtonPanel::removeButtonFromGroup(const sf::Vector2i& coords,
                                          const string& group) noexcept
{
    try
    {
        vector<CVButton*>& group_v = buttonGroups.at(group);
        CVButton * rmButton = getButton(coords);
        for(size_t i = 0; i < group_v.size(); ++i)
        {
            if(group_v[i] == rmButton)
            {
                group_v.erase(group_v.begin() + i);
                break;
            }
        }
    }catch(...){ }
}

void CVButtonPanel::setGrid(const unsigned int& x, const unsigned int& y)
{
    setGrid(sf::Vector2i(x, y));
}

void CVButtonPanel::setGrid(const sf::Vector2i& size)
{
    gridSize = size;

    buttonGrid.resize(size.y);
    for(auto& row : buttonGrid)
    {
        row.resize(size.x, nullptr);
    }

    organizeGrid();
}

void CVButtonPanel::setOuterPadding(const float& value)
{
    outerPadding = value;
    organizeGrid();
}

void CVButtonPanel::setInnerPadding(const float& value)
{
    innerPadding = value;
    organizeGrid();
}

void CVButtonPanel::setSpriteColor(const sf::Color& newColor)
{
    CVBox::setSpriteColor(newColor);
    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {
            if(button) button->setSpriteColor(newColor);
        }
    }

}

void CVButtonPanel::setButtonFillColor(const sf::Color& newColor)
{
    buttonFillColor() = newColor;
    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {
            if(button) button->setFillColor(newColor);
        }
    }

}

void CVButtonPanel::setButtonOutlineColor(const sf::Color& newColor)
{
    buttonOutlineColor() = newColor;
    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {
            if(button) button->setOutlineColor(newColor);
        }
    }

}

void CVButtonPanel::setButtonSpriteScale(const float& newScale)
{
    fSpriteScale = newScale;
    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {
            if(button) button->setSpriteScale(newScale);
        }
    }

}

void CVButtonPanel::setButtonRounding(const float& rounding_radius,
                                      const size_t& point_count,
                                      const vector<bool>& corner_states)
{

    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {

            if(button) button->setRounding(rounding_radius, point_count, corner_states);

        }
    }

    fButtonRoundingRadius = rounding_radius;
    buttonRoundPoints = point_count;
    buttonRoundStates = corner_states;

}

void CVButtonPanel::setButtonOutlineThickness(const float& newThickness)
{
    fButtonOutlineThickness = newThickness;
    for(auto& row : buttonGrid)
    {
        for(auto& button : row)
        {
            if(button) button->setOutlineThickness(newThickness);
        }
    }

}

sf::Vector2f CVButtonPanel::getGridPosition(const sf::Vector2i& position) const
{
    return sf::Vector2f(bounds.left + outerPadding + (bounds.width - 2*outerPadding - innerPadding*(gridSize.x-1))*position.x/gridSize.x
                        + innerPadding*position.x,
                        bounds.top + outerPadding + (bounds.height - 2*outerPadding - innerPadding*(gridSize.y-1))*position.y/gridSize.y
                        + innerPadding*position.y);
}

void CVButtonPanel::addButton(const sf::Vector2i& coord,
                              const string& tag,
                              const string& icon,
                              const string& text,
                              const unsigned int& numStates,
                              const unsigned int& initialState)
{

    if(coord.x >= gridSize.x) setGrid(sf::Vector2i(coord.x + 1, coord.y));
    if(coord.y >= gridSize.y) setGrid(sf::Vector2i(coord.x, coord.y + 1));

    CVButton* newButton = new CVButton(View, getGridPosition(coord),
                                       (bounds.width - outerPadding*2 - innerPadding * (gridSize.x - 1))/gridSize.x,
                                       (bounds.height - outerPadding*2 - innerPadding * (gridSize.y - 1))/gridSize.y,
                                       TextEntry(text, textInfo.font, textInfo.fontSize, textInfo.alignment, textInfo.textColor),
                                       icon,
                                       buttonFillColor(),
                                       buttonOutlineColor(),
                                       fButtonOutlineThickness);

    newButton->setTag(tag);
    newButton->setSpriteScale(fSpriteScale);
    newButton->setSpriteColor(spriteColor);
    newButton->setNumStates(numStates);
    newButton->setState(initialState);

    newButton->setRounding(fButtonRoundingRadius, buttonRoundPoints, buttonRoundStates);

    buttonGrid[coord.y][coord.x] = newButton;

    addPanelElement(newButton, tag);

    organizeGrid();
}

CVButton * CVButtonPanel::getButton(const sf::Vector2i& coord)
{

    if(coord.y < buttonGrid.size())
    {
        if(coord.x < buttonGrid[coord.y].size())
        {
            return buttonGrid[coord.y][coord.x];
        }
    }

    return nullptr;

}

const CVButton * CVButtonPanel::getButton(const sf::Vector2i& coord) const
{

    if(coord.y < buttonGrid.size())
    {
        if(coord.x < buttonGrid[coord.y].size())
        {
            return buttonGrid[coord.y][coord.x];
        }
    }

    return nullptr;

}

string CVButtonPanel::getInteraction()
{
    string output = interactTag;
    interactTag.clear();
    return output;
}

string CVButtonPanel::getSelectedFromGroup(const string& group) const noexcept
{
    try
    {
        const vector<CVButton*>& group_v = buttonGroups.at(group);
        for(auto& btn : group_v)
        {
            if(btn->isHighlighted()) return btn->tag();
        }
    }catch(...){ }

    return string();
}

void CVButtonPanel::clearSelectedFromGroup(const string& group) noexcept
{
    try
    {
        const vector<CVButton*>& group_v = buttonGroups.at(group);
        for(auto& btn : group_v)
        {
            if(btn->isHighlighted()) btn->highlight(false);
        }
    }catch(...){ }
}

bool CVButtonPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(event.LMBreleased &&
            bounds.contains(event.LMBpressPosition) &&
            event.LMBreleaseFrames == 1)
    {
        for(size_t y = 0, x; y < buttonGrid.size(); ++y)
        {
            for(x = 0; x < buttonGrid[y].size(); ++x)
            {
                if(buttonGrid[y][x] != nullptr)
                {
                    if(buttonGrid[y][x]->getTrigger())
                    {

                        interactTag = buttonGrid[y][x]->tag();

                        if(buttonBelongsToGroup(sf::Vector2i(x, y)))
                        {
                            setHighlight(sf::Vector2i(x, y));
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool CVButtonPanel::draw(sf::RenderTarget* target)
{
    if(!CVBasicViewPanel::draw(target)) return false;

    return true;
}

}
