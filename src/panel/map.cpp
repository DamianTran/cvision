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

#include "cvision/panel.hpp"
#include "cvision/view.hpp"

#include "EZC/algorithm.hpp"

using namespace EZC;

namespace cvis{

CVMapPanel::zone::zone(CVMapPanel* parent):
    parent(parent){ }
CVMapPanel::zone::zone(CVMapPanel* parent, const std::string& name, const sf::Rect<int>& bounds,
                       const sf::Color& color):
    sf::RectangleShape(sf::Vector2f((float)bounds.width/parent->getCoordBounds().width * parent->getBounds().width,
                     (float)bounds.height/parent->getCoordBounds().height * parent->getBounds().height)),
    parent(parent),
    bounds(bounds),
    name(name){

        setPosition(sf::Vector2f(parent->basePanelBounds().left +
                                 (float)bounds.left/parent->getCoordBounds().width*parent->basePanelBounds().width,
                        parent->basePanelBounds().top +
                                 (float)bounds.top/parent->getCoordBounds().height*parent->basePanelBounds().height));
        setFillColor(color);

}

void CVMapPanel::zone::updateCoords(const sf::Rect<int>& newCoords){
    bounds = newCoords;
    sf::FloatRect baseBounds = parent->basePanelBounds();
    setSize(sf::Vector2f((float)newCoords.width/parent->getCoordBounds().width * parent->basePanelBounds().width,
                     (float)newCoords.height/parent->getCoordBounds().height * parent->basePanelBounds().height));
    setPosition(sf::Vector2f(parent->basePanelBounds().left +
                             (float)bounds.left/parent->getCoordBounds().width*parent->basePanelBounds().width,
                        parent->basePanelBounds().top +
                             (float)bounds.top/parent->getCoordBounds().height*parent->basePanelBounds().height));
}

void CVMapPanel::zone::updateCoords(const sf::Vector2i& newCoords){
    bounds.left = newCoords.x;
    bounds.top = newCoords.y;
    updateCoords(bounds);
}

void CVMapPanel::zone::updateCoords(){
    updateCoords(bounds);
}

sf::FloatRect CVMapPanel::basePanelBounds() const{
    return base.getGlobalBounds();
}

CVMapPanel::CVMapPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         outer_bounds(bounds),
                         init_bounds(bounds),
                         click_coords(INT_MAX, INT_MAX),
                         release_coords(INT_MAX, INT_MAX),
                         select_rows(false),
                         row_selected(false),
                         select_cols(false),
                         col_selected(false),
                         displayInfo(false),
                         coords_captured(true),
                         outerSelectDist(24.0f),
                         zoomLevel(1.0f),
                         zoomMax(100.0f),
                         zoomMin(0.1f),
                         base(panel.front()){

    bClipBounds = true; // Clip boundaries to minimap by default
    setOutOfBoundsDraw(); // Enable text rendering outside the boundaries

    highlight.setFillColor(sf::Color(255,255,255,120));
    selection.setFillColor(sf::Color(17,80,150,160));
    base.setFillColor(sf::Color(100,100,100,160));

    expandBounds(outer_bounds, outerSelectDist);

    textEntryIndex = displayText.size();
    addTextEntry(textInfo, sf::Vector2f(bounds.left, bounds.top - outerSelectDist - 6.0f));
    extraInfoIndex = displayText.size();
    addTextEntry(textInfo, sf::Vector2f(bounds.left, bounds.top + bounds.height + outerSelectDist));

};

void CVMapPanel::setTitle(const std::string& newTitle){
    displayText[textEntryIndex].setString(newTitle);
}

std::string CVMapPanel::getTitle() const{
    return std::string(displayText[textEntryIndex].getString());
}

void CVMapPanel::setCoords(const sf::Rect<int>& newCoords, const bool& morph){
    if(newCoords == coord_bounds) return;
    coord_bounds = newCoords;
    if(morph){

        sf::Vector2f moveDist(0.0f,0.0f), init_size = base.getSize();
        if(newCoords.width > newCoords.height){
            base.setSize(sf::Vector2f(init_bounds.width, init_bounds.height * (float)newCoords.height/newCoords.width));
        }
        else if(newCoords.height > newCoords.width){
            base.setSize(sf::Vector2f(init_bounds.width * (float)newCoords.width/newCoords.height, init_bounds.height));
        }

        moveDist = (init_size - base.getSize())/2;

        base.move(moveDist);

        outer_bounds = base.getGlobalBounds();
        expandBounds(outer_bounds, outerSelectDist);

        for(auto& zone : zones){
            zone.move(moveDist);
        }
        selection.move(moveDist);
        highlight.move(moveDist);

        setZoom(1.0);

    }
}

void CVMapPanel::clear(){
    zones.clear();
}

void CVMapPanel::setRowSelect(const bool& status){
    select_rows = status;
}

void CVMapPanel::setColumnSelect(const bool& status){
    select_cols = status;
}

void CVMapPanel::showExtraInfo(const bool& status){
    displayInfo = status;
}

void CVMapPanel::setZoneColor(const std::string& tag, const sf::Color& newColor){
    for(auto& zone : zones){
        if(zone.name == tag) zone.setFillColor(newColor);
    }
}
void CVMapPanel::setZoneColor(const unsigned int& index, const sf::Color& newColor){
    zones[index].setFillColor(newColor);
}

void CVMapPanel::setBaseColor(const sf::Color& newColor){
    base.setFillColor(newColor);
}

sf::Color CVMapPanel::baseColor() const{
    return base.getFillColor();
}

unsigned int CVMapPanel::numZones() const{
    return zones.size();
}

bool CVMapPanel::zoneExists(const std::string& tag) const{
    for(auto& zone : zones){
        if(zone.name == tag) return true;
    }
    return false;
}

void CVMapPanel::addZone(const std::string& name, const sf::Color& newColor,
             const sf::Rect<int>& bounds){
    for(auto& zone : zones){
        if(zone.name == name) return;
    }
    zones.emplace_back(this, name, bounds, newColor);
}
void CVMapPanel::removeZone(const std::string& name){
    for(size_t i = 0; i < zones.size(); ++i){
        if(zones[i].name == name) zones.erase(zones.begin() + i);
    }
}
void CVMapPanel::removeZone(const unsigned int& index){
    zones.erase(zones.begin() + index);
}

void CVMapPanel::updateZone(const std::string& name, const sf::Vector2i& newCoords){
    for(auto& zone : zones){
        if(zone.name == name){
            zone.updateCoords(newCoords);
        }
    }
}
void CVMapPanel::updateZone(const unsigned int& index, const sf::Vector2i& newCoords){
    zones[index].updateCoords(newCoords);
}

void CVMapPanel::updateZone(const std::string& name, const sf::Rect<int>& newCoords){
    for(auto& zone : zones){
        if(zone.name == name){
            zone.updateCoords(newCoords);
        }
    }
}
void CVMapPanel::updateZone(const unsigned int& index, const sf::Rect<int>& newCoords){
    zones[index].updateCoords(newCoords);
}

void CVMapPanel::setZoneImage(const std::string& tag, const sf::Texture& img){
    for(auto& zone : zones){
        if(zone.name == tag){
            zone.setTexture(&img);
            return;
        }
    }
}

void CVMapPanel::setZoneImage(const unsigned int& index, const sf::Texture& img){
    zones[index].setTexture(&img);
}

sf::Text& CVMapPanel::extraInfoText(){
    return displayText[extraInfoIndex];
}

sf::Text& CVMapPanel::titleText(){
    return displayText[textEntryIndex];
}

sf::Vector2i CVMapPanel::globalToCoords(const sf::Vector2f& position) const{
    return sf::Vector2i((position.x - base.getGlobalBounds().left)/base.getGlobalBounds().width * coord_bounds.width,
                        (position.y - base.getGlobalBounds().top)/base.getGlobalBounds().height * coord_bounds.height);
}

sf::Vector2f CVMapPanel::coordsToGlobal(const sf::Vector2i& coords) const{
    return sf::Vector2f((float)coords.x/coord_bounds.width * base.getGlobalBounds().width + base.getGlobalBounds().left,
                        (float)coords.y/coord_bounds.height * base.getGlobalBounds().height + base.getGlobalBounds().top);
}

void CVMapPanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);

    init_bounds.left += distance.x;
    init_bounds.top += distance.y;
    outer_bounds.left += distance.x;
    outer_bounds.top += distance.y;

    for(auto& zone : zones){
        zone.move(distance);
    }
    selection.move(distance);
    highlight.move(distance);
    base.move(distance);
}

void CVMapPanel::move(const float& x, const float& y){
    move(sf::Vector2f(x, y));
}

void CVMapPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVMapPanel::setPosition(const float& x, const float& y){
    setPosition(sf::Vector2f(x, y));
}

bool CVMapPanel::draw(sf::RenderTarget* target){
    if(!CVBox::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    target->draw(base);

    for(auto& zone : zones){
        target->draw(zone);
    }

    target->draw(selection);
    target->draw(highlight);

    for(auto& item : viewPanelElements){
        item->draw(target);
    }

    CV_DRAW_CLIP_END

    for(auto& item : displayText){
        target->draw(item);
    }

    return true;
}

void CVMapPanel::setZoom(float newZoom, sf::Vector2f mousePos){

    if(newZoom < zoomMin) newZoom = 0.01f;
    else if(newZoom > zoomMax) newZoom = 100.0f;

    sf::Vector2f old_size = base.getSize(),
                moveDist;

    base.setSize(old_size * newZoom/zoomLevel);
    base.move((old_size - base.getSize())/2);

    old_size = selection.getSize();
    selection.setSize(selection.getSize() * newZoom/zoomLevel);
    selection.move((old_size - selection.getSize())/2);

    old_size = highlight.getSize();
    highlight.setSize(highlight.getSize() * newZoom/zoomLevel);
    highlight.move((old_size - selection.getSize())/2);

    if(isnan(mousePos.x) || isnan(mousePos.y)){
        mousePos.x = bounds.left + bounds.width/2;
        mousePos.y = bounds.top + bounds.height/2;
    }

    base.setPosition(radial_position(mousePos,
                                     getDistance(mousePos, base.getPosition() + base.getSize()/2)*newZoom/zoomLevel,
                                     get_angle(mousePos, base.getPosition() + base.getSize()/2)) - base.getSize()/2);
    selection.setPosition(radial_position(mousePos,
                                     getDistance(mousePos, selection.getPosition() + selection.getSize()/2)*newZoom/zoomLevel,
                                     get_angle(mousePos, selection.getPosition() + selection.getSize()/2)) - selection.getSize()/2);
    highlight.setPosition(radial_position(mousePos,
                                     getDistance(mousePos, highlight.getPosition())*newZoom/zoomLevel,
                                     get_angle(mousePos, highlight.getPosition() + highlight.getSize()/2)) - highlight.getSize()/2);

    base.move(moveDist);
    outer_bounds = base.getGlobalBounds();
    expandBounds(outer_bounds, outerSelectDist);

    for(auto& zone : zones){
        zone.updateCoords();
    }

    selection.move(moveDist);
    highlight.move(moveDist);

    zoomLevel = newZoom;

}

sf::Rect<int> CVMapPanel::selected_bounds() const{
    return sf::Rect<int>(click_coords.x, click_coords.y,
                         release_coords.x - click_coords.x,
                         release_coords.y - click_coords.y);
}

bool CVMapPanel::sectionSelected() const{
    return (release_coords.x != INT_MAX) && (release_coords.y != INT_MAX) && pointSelected();
}

bool CVMapPanel::pointSelected() const{
    return (click_coords.x != INT_MAX) && (click_coords.y != INT_MAX);
}

void CVMapPanel::shiftMap(const sf::Vector2f& distance){
    base.move(distance);
    selection.move(distance);
    highlight.move(distance);
    for(auto& zone : zones){
        zone.move(distance);
    }
}

void CVMapPanel::setCenter(const sf::Vector2i& center){
    shiftMap(coordsToGlobal(center) - base.getPosition() + base.getSize()/2);
}

bool CVMapPanel::captureCoords(sf::Rect<int>& output){
    if(!coords_captured){
        output = selected_bounds();
        coords_captured = true;
        return true;
    }
    output = sf::Rect<int>(0,0,0,0);
    return false;
}

bool CVMapPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(event.focusFree()){

        if(bounds.contains(mousePos) && event.mouseWheelDelta.y && event.captureFocus()){
            setZoom(zoomLevel + event.mouseWheelDelta.y*0.3f, mousePos);
        }

        if(select_rows){
            if((mousePos.x < bounds.left) && (mousePos.x >= bounds.left - outerSelectDist) &&
                   (mousePos.y > bounds.top) && (mousePos.y < bounds.top + bounds.height)){
                highlight.setPosition(sf::Vector2f(bounds.left, globalToCoords(mousePos).y));
                highlight.setSize(sf::Vector2f(bounds.width, bounds.height/coord_bounds.height));
            }
            else if((mousePos.x > bounds.left + bounds.width) && (mousePos.x <= bounds.left + bounds.width + outerSelectDist) &&
                   (mousePos.y > bounds.top) && (mousePos.y < bounds.top + bounds.height)){
                highlight.setPosition(sf::Vector2f(bounds.left, globalToCoords(mousePos).y));
                highlight.setSize(sf::Vector2f(bounds.width, bounds.height/coord_bounds.height));
            }
        }
        if(select_cols){
            if((mousePos.y < bounds.top) && (mousePos.y >= bounds.top - outerSelectDist) &&
                   (mousePos.x > bounds.left) && (mousePos.x < bounds.left + bounds.width)){
                highlight.setPosition(sf::Vector2f(bounds.top, globalToCoords(mousePos).y));
                highlight.setSize(sf::Vector2f(bounds.width, bounds.height/coord_bounds.height));
            }
            else if((mousePos.y > bounds.top + bounds.height) && (mousePos.y <= bounds.top + bounds.height + outerSelectDist) &&
                   (mousePos.x > bounds.left) && (mousePos.x < bounds.left + bounds.width)){
                highlight.setPosition(sf::Vector2f(bounds.top, globalToCoords(mousePos).y));
                highlight.setSize(sf::Vector2f(bounds.width/coord_bounds.width, bounds.height));
            }
        }

        if(bounds.contains(event.LMBpressPosition)){
            if(event.LMBhold && event.captureFocus() && event.captureMouse()){
                if(event.LMBholdFrames == 1){
                    click_coords = globalToCoords(mousePos);
                    if(click_coords.x < coord_bounds.left) click_coords.x = coord_bounds.left;
                    if(click_coords.y < coord_bounds.top) click_coords.y = coord_bounds.top;
                    selection.setPosition(event.LMBpressPosition);
                }

                release_coords = globalToCoords(mousePos);
                if(release_coords.x > coord_bounds.left + coord_bounds.width)
                    release_coords.x = coord_bounds.left + coord_bounds.width;
                if(release_coords.y < coord_bounds.top)
                    release_coords.y = coord_bounds.top + coord_bounds.height;


                selection.setSize(sf::Vector2f(mousePos.x - event.LMBpressPosition.x,
                                               mousePos.y - event.LMBpressPosition.y));

                if(displayInfo){
                    std::stringstream infoStr;
                    infoStr << "Selected from: [" << click_coords.x << ',' << click_coords.y
                            << "] to [" << release_coords.x << ',' << release_coords.y << "]\n"
                            << "Size: [" << release_coords.x - click_coords.x + 1 << " x "
                            << release_coords.y - click_coords.y + 1 << ']';
                    extraInfoText().setString(infoStr.str());
                }

            }
            else if(event.LMBreleased && (event.LMBreleaseFrames == 1)){
                release_coords = globalToCoords(mousePos);
                if(release_coords.x > coord_bounds.left + coord_bounds.width)
                    release_coords.x = coord_bounds.left + coord_bounds.width;
                if(release_coords.y < coord_bounds.top)
                    release_coords.y = coord_bounds.top + coord_bounds.height;

                if((release_coords == click_coords) ||
                   !selection.getGlobalBounds().intersects(base.getGlobalBounds())){
                    release_coords.x = INT_MAX;
                    release_coords.y = INT_MAX;
                    selection.setSize(sf::Vector2f(0.0f,0.0f));
                }
                else{
                    selection.setSize(sf::Vector2f(coordsToGlobal(release_coords).x - coordsToGlobal(click_coords).x,
                                                   coordsToGlobal(release_coords).y - coordsToGlobal(click_coords).y));
                    selection.setPosition(sf::Vector2f(coordsToGlobal(click_coords).x,
                                                       coordsToGlobal(click_coords).y));

                    coords_captured = false;
                }

                if(displayInfo){
                    std::stringstream infoStr;
                    if(sectionSelected()){
                        infoStr << "Selected from: [" << click_coords.x << ',' << click_coords.y
                                << "] to [" << release_coords.x << ',' << release_coords.y << "]\n"
                                << "Size: [" << release_coords.x - click_coords.x + 1 << " x "
                                << release_coords.y - click_coords.y + 1 << ']';
                    }
                    else if((click_coords.x != INT_MAX) && (click_coords.y != INT_MAX)){
                        infoStr << "Selected: [" << click_coords.x << ',' << click_coords.y << ']';
                    }

                    extraInfoText().setString(infoStr.str());
                }

            }
        }
        else if(event.LMBhold){
            selection.setSize(sf::Vector2f(0.0f,0.0f));

            click_coords.x = INT_MAX;
            click_coords.y = INT_MAX;
            release_coords.x = INT_MAX;
            release_coords.y = INT_MAX;

            extraInfoText().setString("");
        }

        if(event.RMBhold && bounds.contains(event.RMBpressPosition)){

            shiftMap(mousePos - event.lastFrameMousePosition);

        }
    }

    return true;
}

}
