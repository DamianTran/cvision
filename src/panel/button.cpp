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

#include "cvision/panel.hpp"
#include "cvision/cvis_buttonIO.hpp"

namespace cvis{

CVButtonPanel::CVButtonPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         outerPadding(12.0f),
                         innerPadding(1.0f){

}

void CVButtonPanel::organizeGrid(){
    for(size_t y = 0, x; y < gridSize.y; ++y){
        for(x = 0; x < gridSize.x; ++x){
            if(buttonGrid[y][x] != nullptr){
                buttonGrid[y][x]->setSize(sf::Vector2f((bounds.width - outerPadding*2 - innerPadding * (gridSize.x - 1))/gridSize.x,
                                                       (bounds.height - outerPadding*2 - innerPadding * (gridSize.y - 1))/gridSize.y));
                buttonGrid[y][x]->setPosition(getGridPosition(sf::Vector2i(x, y)) + sf::Vector2f(buttonGrid[y][x]->getSize().x/2, 0.0f));
            }
        }
    }
}

bool CVButtonPanel::checkInteraction() const{
    return !interactTag.empty();
}

void CVButtonPanel::setGrid(const unsigned int& x, const unsigned int& y){
    setGrid(sf::Vector2i(x, y));
}

void CVButtonPanel::setGrid(const sf::Vector2i& size){
    gridSize = size;

    buttonGrid.resize(size.y);
    for(auto& row : buttonGrid){
        row.resize(size.x, nullptr);
    }

    organizeGrid();
}

void CVButtonPanel::setOuterPadding(const float& value){
    outerPadding = value;
    organizeGrid();
}

void CVButtonPanel::setInnerPadding(const float& value){
    innerPadding = value;
    organizeGrid();
}

sf::Vector2f CVButtonPanel::getGridPosition(const sf::Vector2i& position) const{
    return sf::Vector2f(bounds.left + outerPadding + (bounds.width - 2*outerPadding - innerPadding*(gridSize.x-1))*position.x/gridSize.x
                        + innerPadding*(gridSize.x - 1),
                        bounds.top + outerPadding + (bounds.height - 2*outerPadding + innerPadding*(gridSize.y-1))*position.y/gridSize.y
                        + innerPadding*(gridSize.y - 1));
}

void CVButtonPanel::addButton(const sf::Vector2i& coord,
                              const std::string& tag,
                              const std::string& icon,
                              const std::string& text,
                              const unsigned int& numStates,
                              const unsigned int& initialState){
    if(coord.x >= gridSize.x) setGrid(sf::Vector2i(coord.x + 1, coord.y));
    if(coord.y >= gridSize.y) setGrid(sf::Vector2i(coord.x, coord.y + 1));



    CVButton* newButton = new CVButton(View, getGridPosition(coord),
                                                (bounds.width - outerPadding*2 - innerPadding * (gridSize.x - 1))/gridSize.x,
                                                (bounds.height - outerPadding*2 - innerPadding * (gridSize.y - 1))/gridSize.y,
                                                icon, numStates, initialState, true, nullptr);
    newButton->setTag(tag);
    newButton->setOrigin(sf::Vector2f(0.0f,0.0f));
    buttonGrid[coord.y][coord.x] = newButton;

    addPanelElement(newButton, tag);

    organizeGrid();
}

std::string CVButtonPanel::getInteraction(){
    std::string output = interactTag;
    interactTag.clear();
    return output;
}

bool CVButtonPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(event.LMBreleased &&
       bounds.contains(event.LMBpressPosition) &&
       event.LMBreleaseFrames == 1){
        for(auto& row : buttonGrid){
            for(auto& button : row){
                if(button != nullptr){
                    if(button->getTrigger()){
                        interactTag = button->tag();
                    }
                }
            }
        }
    }

    return true;
}

bool CVButtonPanel::draw(sf::RenderTarget* target){
    if(!CVBasicViewPanel::draw(target)) return false;

    return true;
}

}
