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

#include "cvision/panel/switch.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include <hyper/toolkit/string.hpp>

using namespace hyperC;
using namespace std;

namespace cvis
{

CVSwitchPanel::CVSwitchPanel(CVView* parentView, string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         viewIndex(0),
                         bCanPan(false),
                         bAutoPan(false),
                         bCenterOnNew(true),
                         bClosablePanels(true),
                         bLoop(true),
                         bTimed(false),
                         panRate(60.0f),
                         elementPadding(3.0f),
                         autoSwitchLatency(10.0f),
                         timeLastSwitch(TIME_NOW){
    setExpand(false);
}

string CVSwitchPanel::getFocusTag(){

    if(numPanels()){

        return viewPanelTags[viewIndex];

    }

    return string();

}

void CVSwitchPanel::switch_next()
{
    if(viewIndex + 1 < numPanels())
    {
        setCenter(viewIndex + 1);
    }
    else if(bLoop)
    {
        setCenter(0);
    }
}

void CVSwitchPanel::switch_previous()
{
    if(viewIndex > 0)
    {
        setCenter(viewIndex - 1);
    }
    else if(bLoop && numPanels())
    {
        setCenter(numPanels() - 1);
    }
}

void CVSwitchPanel::setElementPadding(const float& newPadding){
    elementPadding = newPadding;
}

bool CVSwitchPanel::checkFocus(const string& tag){
    if(viewIndex >= viewPanelTags.size()) return false;
    return viewPanelTags[viewIndex] == tag;
}

CVElement* CVSwitchPanel::getActiveElement(){
    if(numPanels() < 1) return nullptr;
    else return viewPanelElements[viewIndex];
}

void CVSwitchPanel::addPanelElement(CVElement* newElement,
                                    const string& tag,
                                    const unsigned int& index){

    if(numPanels()){
        if(!index)
        {
            newElement->setPosition(viewPanelElements.front()->getPosition().x - newElement->getBounds().width - elementPadding,
                          viewPanelElements.front()->getPosition().y + viewPanelElements.front()->getSize().y/2 - newElement->getSize().y/2);
        }
        else if(index < numPanels()){
            newElement->setPosition(viewPanelElements[index]->getPosition().x,
                          viewPanelElements[index]->getPosition().y + viewPanelElements[index]->getSize().y/2 - newElement->getSize().y/2);
            for(size_t i = index; i < numPanels(); ++i){
                viewPanelElements[i]->move(newElement->getBounds().width + elementPadding, 0.0f);
            }
        }
        else{
            newElement->setPosition(viewPanelElements.back()->getPosition().x + viewPanelElements.back()->getSize().x + elementPadding,
                                    getPosition().y + getSize().y/2 - newElement->getSize().y/2);
        }
    }
    else{
        newElement->setPosition(getPosition().x + getSize().x/2 - newElement->getBounds().width/2,
                                getPosition().y + getSize().y/2 - newElement->getBounds().height/2);
    }

    CVViewPanel::addPanelElement(newElement, tag, index);

    if(bClosablePanels)
    {
        newElement->setClosable(true);
    }

    if(bCenterOnNew)
    {
        if(index < numPanels())
        {
            setCenter(index);
        }
        else
        {
            setCenter(numPanels()-1);
        }
    }

}

void CVSwitchPanel::removePanelElement(const unsigned int& index){
    sf::FloatRect panelBounds = viewPanelElements[index]->getBounds();
    CVViewPanel::removePanelElement(index);

    for(size_t i = index; i < viewPanelElements.size(); ++i){
        viewPanelElements[i]->move(sf::Vector2f(-panelBounds.width, 0.0f));
    }

    if(bExpand){
        updateBounds();
    }

    if(viewIndex > (int)index) setCenter(viewIndex - 1);
    else setCenter(viewIndex);

}

void CVSwitchPanel::enablePanning(){
    bCanPan = true;
}

void CVSwitchPanel::disablePanning(){
    bCanPan = false;
}

void CVSwitchPanel::setPanRate(const float& newRate){
    panRate = newRate;
}

void CVSwitchPanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
}

void CVSwitchPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

bool CVSwitchPanel::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVViewPanel::update(event, mousePos)) return false;

    for(int i = viewPanelElements.size() - 1; (i >= 0) && !viewPanelElements.empty(); --i){
        if(viewPanelElements[i]->shouldDelete())
        {
            removePanelElement(i);
        }
        else{
            if(bOutOfBoundsUpdate || View->contains(*viewPanelElements[i])){
                viewPanelElements[i]->update(event, mousePos);
            }
        }
    }

    if(bCanPan){

        if(event.viewHasFocus &&
           (event.keyLog.size() > 0) && shiftPressed()){
            for(auto& key : event.keyLog){
                if(key == getKey(CV_CTRL_FWD)){
                    setCenter(viewIndex + 1);
                }
                else if(key == getKey(CV_CTRL_REV)){
                    setCenter(viewIndex - 1);
                }
            }
        }

        if(bounds.contains(mousePos) &&
           (event.mouseWheelDelta.x != 0.0f)){ // Horizontal pan
            float moveDist = panRate*event.mouseWheelDelta.x;
            if(getPosition().x + moveDist < View->getWidth()/2){
                if(getPosition().x + getSize().x + moveDist > View->getWidth()/2){
                    for(auto& element : viewPanelElements)
                    {
                        element->move(sf::Vector2f(moveDist, 0.0f));
                    }
                }
                else setPosition(View->getWidth()/2 - getSize().x, getPosition().y);
            }
            else setPosition(View->getWidth()/2, getPosition().y);

            for(size_t i = 0; i < viewPanelElements.size(); ++i){
                if(viewPanelElements[i]->getBounds().contains(View->getSize()/2)){
                    viewIndex = i;
                    break;
                }
            }

            bAutoPan = false;
        }
    }

    if(bAutoPan && (viewIndex != UINT_MAX) && numPanels()){

        sf::Vector2f moveDist = panRate/500*(sf::Vector2f(getBounds().left + getSize().x/2,
                                       getBounds().top + getSize().y/2) -
             sf::Vector2f(viewPanelElements[viewIndex]->getPosition().x + viewPanelElements[viewIndex]->getBounds().width/2,
                          viewPanelElements[viewIndex]->getPosition().y + viewPanelElements[viewIndex]->getBounds().height/2));

        for(auto& element : viewPanelElements)
        {
            element->move(moveDist);
        }

    }

    if(bTimed)
    {
        if(chrono::duration<float>(TIME_NOW - timeLastSwitch).count() > autoSwitchLatency)
        {
            switch_next();
            timeLastSwitch = TIME_NOW;
        }
    }

    // Handle events

    string triggerInfo = take_trigger("SwitchTo");
    if(!triggerInfo.empty())
    {
        if(isNumeric(triggerInfo))
        {
            try
            {
                switch_to(stoi(triggerInfo));
            }
            catch(...)
            {
                switch_to(triggerInfo);
            }
        }
        else
        {
            switch_to(triggerInfo);
        }
    }
    else
    {
        triggerInfo = take_trigger("SwitchNext");

        if(triggerInfo == "TRUE")
        {
            switch_next();
        }
        else
        {

            triggerInfo = take_trigger("SwitchPrevious");

            if(triggerInfo == "TRUE")
            {
                switch_previous();
            }

        }
    }

    return true;
}

void CVSwitchPanel::setCenter(const int& index){
    if(index >= (int)numPanels()){
        viewIndex = 0;
    }
    else if(index < 0){
        viewIndex = numPanels() - 1;
    }
    else{
        viewIndex = index;
    }

    if(viewIndex < numPanels())
    {
        sendTrigger(viewPanelElements[viewIndex], "focus");
    }

    bAutoPan = true;
}

void CVSwitchPanel::setCenter(const string& tag){
    for(size_t i = 0; i < viewPanelTags.size(); ++i){
        if(viewPanelTags[i] == tag) setCenter(i);
    }
}

}
