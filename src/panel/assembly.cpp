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

#include "cvision/panel/assembly.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include <EZC/algorithm.hpp>
#include <EZC/toolkit/string.hpp>

using namespace EZC;
using namespace std;

namespace cvis
{

CVAssemblyPanel::CVAssemblyPanel(CVView* parentView, string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         scrollBarPadding(12.0f),
                         snapDistance(10.0f),
                         assemblyType(CV_ASSEMBLE_HORIZONTAL | CV_ASSEMBLE_VERTICAL),
                         maxLabelLength(10),
                         bUseTracks(false),
                         bUseLabels(true),
                         itemSize(60.0f,80.0f),
                         scrollBarY(parentView,
                                    sf::Vector2f(bounds.left + bounds.width - scrollBarPadding/2,
                                                 bounds.top + 2*scrollBarPadding),
                                    sf::Vector2f(bounds.left + bounds.width - scrollBarPadding/2,
                                                 bounds.top + bounds.height - 2*scrollBarPadding),
                                    size.y, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
                                    1000.0f, CV_OBJ_ANIM_FADE_IN),
                         assemblyStateTags({ "Ascending", "Descending" }),
                         defaultAssemblyState(0){

    IDtag = "AssemblyPanel";

    scrollBarY.setPanelSize(size.y);
    scrollBarY.setPosition(sf::Vector2f(bounds.left + bounds.width - scrollBarPadding/2,
                                                        bounds.top + scrollBarPadding));


}

void CVAssemblyPanel::setAssemblyTags(const vector<string>& newTags)
{
    assemblyStateTags = newTags;
}

void CVAssemblyPanel::setInitialAssemblyState(const uint8_t& newState)
{
    defaultAssemblyState = newState;
}

CVAssemblyPanel::assembly::assembly(const vector<CVElement*>& members,
                                    CVAssemblyPanel& host):
    CVBox(host.View, host.getPosition(), 0.0f,0.0f,host.highlightColor, sf::Color::Transparent, 0.0f),
    host(&host),
    itemPadding(12.0f*viewScale()),
    outerPadding(18.0f*viewScale()),
    label(host.View, getPosition(), host.itemSize.x*2, host.itemSize.y/4,
          textEntry("Name this category...", host.textInfo.font, host.textInfo.fontSize, ALIGN_LEFT_MIDLINE, sf::Color::Black),
          host.highlightColor, host.highlightColor, 1.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL),
    acceptButton(host.View, getPosition(), label.getBounds().height*0.6f, label.getBounds().height*0.6f, "gen_check", 1, 0, true),
    cancelButton(host.View, getPosition(), label.getBounds().height*0.6f, label.getBounds().height*0.6f, "gen_x", 1, 0, true),
    members(members){

        setRounding(host.itemSize.y/16);

        sf::Color tmp = baseFillColor();
        tmp.a = 50;
        setFillColor(tmp);

        tmp = host.highlightColor;
        tmp.a = 30;
        label.setFillColor(tmp);

        darken(tmp, 20);
        tmp.a = 80;

        label.setOutlineColor(tmp);
        label.setRounding(host.itemSize.y/8);

        acceptButton.setSpriteColor(sf::Color(0,204,0,150));
        cancelButton.setSpriteColor(sf::Color(204,0,0,150));

        if(!members.empty()){
            bounds = members.front()->getBounds();
            for(size_t i = 1; i < members.size(); ++i){
                expandBounds(bounds, members[i]->getBounds());
            }

            expandBounds(bounds, outerPadding);
            applyBounds(bounds, panel.front());
        }

        if(host.bUseLabels){
            label.setPosition(bounds.left, bounds.top + bounds.height + itemPadding);
            acceptButton.setPosition(label.getBounds().left + label.getBounds().width + acceptButton.getBounds().width,
                                 label.getBounds().top + label.getBounds().height/2);
            cancelButton.setPosition(acceptButton.getPosition().x + acceptButton.getBounds().width*1.25f,
                                     acceptButton.getPosition().y);
        }
        else{
            cancelButton.setPosition(bounds.left + bounds.width - cancelButton.getBounds().width/2,
                                 bounds.top - itemPadding - cancelButton.getBounds().width/2);
            acceptButton.setPosition(cancelButton.getPosition().x - cancelButton.getBounds().width*1.25f,
                                 cancelButton.getPosition().y);
        }
}

void CVAssemblyPanel::assembly::addMember(CVElement* member, const unsigned int& index){
    if(members.empty()){
        bounds = member->getBounds();
        expandBounds(bounds, outerPadding);
        members.push_back(member);
        applyBounds(bounds, panel.front());
    }
    else if(!anyEqual(member, members)){
        if(index > members.size()) members.push_back(member);
        else members.insert(members.begin() + index, member);

        bounds = members.front()->getBounds();
        for(size_t i = 1; i < members.size(); ++i){
            expandBounds(bounds, members[i]->getBounds());
        }
        expandBounds(bounds, outerPadding);
        applyBounds(bounds, panel.front());
    }
    else{
        shiftMember(member, index);
    }
}

void CVAssemblyPanel::assembly::setOuterPadding(const float& newPadding){
    if(!members.empty()){
        expandBounds(bounds, newPadding - outerPadding);
        applyBounds(bounds, panel.front());
    }
    outerPadding = newPadding;
}

void CVAssemblyPanel::assembly::setItemPadding(const float& newPadding){
    if(!members.empty()){
        bounds = members.front()->getBounds();
        for(size_t i = 1; i < members.size(); ++i){
            if(host->assemblyType & CV_ASSEMBLE_HORIZONTAL){
                members[i]->move(newPadding - itemPadding, 0.0f);
            }
            if(host->assemblyType & CV_ASSEMBLE_VERTICAL){
                members[i]->move(0.0f, newPadding - itemPadding);
            }
            expandBounds(bounds, members[i]->getBounds());
        }

        applyBounds(bounds, panel.front());
    }
    itemPadding = newPadding;
}

void CVAssemblyPanel::assembly::addMemberBefore(CVElement* newMember, CVElement* otherMember){
    if(anyEqual(newMember, members)){ // Shift to before other member
        for(size_t i = 0; i < members.size(); ++i){
            if(members[i] == otherMember){
                shiftMember(newMember, i);
                return;
            }
        }
    }
    else{
        for(size_t i = 0; i < members.size(); ++i){
            if(members[i] == otherMember){
                addMember(newMember, i);
                return;
            }
        }
    }
}

void CVAssemblyPanel::assembly::addMemberAfter(CVElement* newMember, CVElement* otherMember){
    if(anyEqual(newMember, members)){ // Shift to after other member
        for(size_t i = 0; i < members.size(); ++i){
            if(members[i] == otherMember){
                shiftMember(newMember, i+1);
                return;
            }
        }
    }
    else{
        for(size_t i = 0; i < members.size(); ++i){
            if(members[i] == otherMember){
                addMember(newMember, i+1);
                return;
            }
        }
    }
}

void CVAssemblyPanel::assembly::swapMembers(CVElement* firstMember, CVElement* otherMember){
    unsigned int firstIndex = UINT_MAX,
                secondIndex = UINT_MAX;
    for(size_t i = 0; i < members.size(); ++i){
        if(members[i] == firstMember) firstIndex = i;
        else if(members[i] == otherMember) secondIndex = i;
    }

    if((firstIndex != UINT_MAX) && (secondIndex != UINT_MAX)){
        swap(members[firstIndex], members[secondIndex]);
    }
}

void CVAssemblyPanel::assembly::removeMember(CVElement* member){
    for(size_t i = 0; i < members.size(); ++i){
        if(members[i] == member){
            members.erase(members.begin() + i);

            if(!members.empty()){
                bounds = members.front()->getBounds();
                for(size_t i = 1; i < members.size(); ++i){
                    expandBounds(bounds, members[i]->getBounds());
                }
                expandBounds(bounds, outerPadding);
            }
            else{
                bounds = sf::FloatRect(0.0f,0.0f,0.0f,0.0f);
            }

            applyBounds(bounds, panel.front());

            return;
        }
    }
}

void CVAssemblyPanel::assembly::shiftMember(CVElement* member, const unsigned int& index){
    for(size_t i = 0; i < members.size(); ++i){
        if(members[i] == member){
            if(index == i) return;
            members.erase(members.begin() + i);
            members.insert(members.begin() + index, member);
            return;
        }
    }
}

bool CVAssemblyPanel::assembly::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBox::update(event, mousePos)) return false;

    if(event.isCaptured(*this)){ // Transduce capture
        for(auto& item : members){
            event.mouse_capture(*item);
        }
    }

    setFocus(host->hasFocus());

    if(hasFocus() && event.LMBhold){
        if(!members.empty()) bounds = members.front()->getBounds();
        for(size_t i = 1; i < members.size(); ++i){
            expandBounds(bounds, members[i]->getBounds());
        }
        expandBounds(bounds, host->snapDistance);
        applyBounds(bounds, panel.front());
    }

    cancelButton.update(event, mousePos);
    acceptButton.update(event, mousePos);

    if(acceptButton.getTrigger() || (host->bUseLabels && label.hasFocus() && event.keyIsPressed(CV_KEY_RETURN))){
        if(label.getTypeString().size() > 0){
            assembly_info.emplace_back();
            assembly_info.back() << label.getTypeString() << '\n';
            for(size_t i = 0; i < members.size(); ++i){
                assembly_info.back() << host->assemblyStateTags[members[i]->getCurrentState()];
                assembly_info.back() << '\t' << members[i]->tag();
                if(i < members.size() - 1) assembly_info.back() << '\n';
            }
        }
        else{

        }
    }
    if(cancelButton.getTrigger()){
        for(int i = members.size() - 1; i > 0; --i){
            members[i]->move(sf::Vector2f(i*host->snapDistance*2, 0.0f));
        }
        while(members.size() > 0){
            removeMember(members.front());
        }
    }

    if(host->bUseLabels){
        label.setPosition(bounds.left, bounds.top + bounds.height + itemPadding);
        acceptButton.setPosition(label.getBounds().left + label.getBounds().width + acceptButton.getBounds().width,
                                 label.getBounds().top + label.getBounds().height/2);
        cancelButton.setPosition(acceptButton.getPosition().x + acceptButton.getBounds().width*1.25f,
                                 acceptButton.getPosition().y);
        label.update(event, mousePos);
    }
    else{
        cancelButton.setPosition(bounds.left + bounds.width - cancelButton.getBounds().width/2,
                                 bounds.top - itemPadding - cancelButton.getBounds().width/2);
        acceptButton.setPosition(cancelButton.getPosition().x - cancelButton.getBounds().width*1.25f,
                                 cancelButton.getPosition().y);
    }

    return true;
}

bool CVAssemblyPanel::assembly::draw(sf::RenderTarget* target){
    if(!CVBox::draw(target)) return false;

    if(host->bUseLabels) label.draw(target);
    cancelButton.draw(target);
    acceptButton.draw(target);

    return true;
}

bool CVAssemblyPanel::assembly::getAssemblyInfo(ostream& output){
    if(!assembly_info.empty()){
        output << assembly_info.front().str();
        assembly_info.erase(assembly_info.begin());
        return true;
    }
    return false;
}

void CVAssemblyPanel::detachMember(CVElement* member){
    for(size_t i = 0; i < groups.size();){
        groups[i]->removeMember(member);
        if(groups[i]->members.size() < 2){
            delete(groups[i]);
            groups.erase(groups.begin() + i);
        }
        else ++i;
    }
}

CVAssemblyPanel::assembly* CVAssemblyPanel::getAssembly(CVElement* member){
    for(auto& asmb : groups){
        for(auto& element : asmb->members){
            if(element == member){
                return asmb;
            }
        }
    }

    return nullptr;
}

bool CVAssemblyPanel::draw(sf::RenderTarget* target){

    CV_DRAW_CLIP_BEGIN

    for(auto& item : panel){
        target->draw(item);
    }
    for(auto& spr : spriteList){
        target->draw(spr);
    }
    for(auto& asmb : groups){
        asmb->draw(target);
    }
    for(auto& item : viewPanelElements){
        item->draw(target);
    }
    for(auto& text : displayText){
        target->draw(text);
    }
    scrollBarY.draw(target);

    CV_DRAW_CLIP_END

    return true;
}

void CVAssemblyPanel::removeAssembly(assembly* asmb){
    for(size_t i = 0; i < groups.size(); ++i){
        if(groups[i] == asmb){
            delete(groups[i]);
            groups.erase(groups.begin() + i);
            return;
        }
    }
}

bool CVAssemblyPanel::getAssemblyInfo(ostream& output){
    for(auto& asmb : groups){
        if(asmb->getAssemblyInfo(output)) return true;
    }
    return false;
}

void CVAssemblyPanel::addAssemblyItem(const sf::Vector2f& position, const string& label, const unsigned int& state){

    string inputData = label;
    if(inputData.size() > maxLabelLength - 5){
        string tmp;
        tmp.assign(inputData.begin() + inputData.size() - (maxLabelLength/2 - 3), inputData.end());
        inputData.erase(inputData.begin() + maxLabelLength/2 - 2, inputData.end());
        inputData += " ... " + tmp;
    }

    CVButton* newBox = new CVButton(View, position, itemSize.x, itemSize.y,
                                      textEntry(inputData, mainApp()->getDefaultFont(), textInfo.fontSize, ALIGN_CENTER_MIDLINE, sf::Color(80,80,80)),
                                                "", sf::Color(120,120,120,120), sf::Color(160,160,160,50), 2.0f);

    newBox->setHighlightableStatus(true);
    newBox->setHighlightColor(sf::Color(15, 150, 255, 160));
    newBox->setTextWrap(true);
    newBox->setRounding(itemSize.y/16);
    newBox->setTag(label);

    newBox->setDraggableStatus(true);
    newBox->setPosition(position - newBox->getSize()/2);

    newBox->setNumStates(2);
    newBox->setToggleFlip(false, true);

    newBox->addSprite(appTexture("arrow_right_half"),
                      sf::Vector2f(itemSize.x/4, itemSize.y/2),
                      sf::Vector2f(itemSize.x/3, itemSize.y*0.8f));
    newBox->setSpriteColor(sf::Color(100,100,100,100));

    newBox->setMask(appTexture("gradient_linear"));
    newBox->setMaskColor(sf::Color(255,255,255,160));

    newBox->setResponseEvent(CV_EVENT_RMB);

    newBox->setState(defaultAssemblyState);

    for(size_t i = 0; i < numPanels(); ++i){
        if(viewPanelElements[i]->getBounds().intersects(newBox->getBounds())){
            newBox->move(viewPanelElements[i]->getBounds().left +
                         viewPanelElements[i]->getBounds().width -
                         newBox->getBounds().left, 0.0f);
            i = 0;
        }
    }

    addPanelElement(newBox, label);
}

bool CVAssemblyPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(bounds.contains(mousePos) && event.captureFocus()){
        setFocus(true);
        scrollBarY.setScrollable(true);
        event.releaseFocus();
    }
    else{
        setFocus(false);
        scrollBarY.setScrollable(false);
    }

    sf::FloatRect totalBounds = panel.front().getGlobalBounds(),
                    panelBounds;

    if(bounds.contains(event.LMBreleasePosition)){
        string inputData;
        while(event.getData(inputData)){
            addAssemblyItem(event.LMBreleasePosition, inputData);
        }
    }

    if(event.LMBhold && hasFocus() && !shiftPressed()){
        for(auto& item : selected){
            item->highlight(false);
        }
        selected.clear();
    }

    for(auto& item : viewPanelElements){
        panelBounds = item->getBounds();
        expandBounds(bounds, panelBounds);

        if(item->hasFocus()) setFocus(true);

        if(event.LMBhold && (event.LMBholdTime < 0.5f) && hasFocus()){
            if(panelBounds.contains(event.LMBpressPosition)){
                if(anyEqual(item, selected)){
                    remove(item, selected);
                    item->highlight(false);
                }
                else{
                    selected.push_back(item);
                    item->highlight(true);
                }
            }
        }
    }

    scrollBarY.setScrollMax(totalBounds.height);
    scrollBarY.setPanelSize(bounds.height);
    scrollBarY.update(event, mousePos);

//    if(totalBounds.height > bounds.height){
//        float moveDist = bounds.top - totalBounds.top
//                            - scrollBarY.getScrollOffset();
//        for(auto& item : viewPanelElements){
//            item->move(sf::Vector2f(0.0f, moveDist));
//        }
//    }
//    else if(numPanels() > 0){
//        float moveDist = bounds.top - totalBounds.top;
//        for(auto& item : viewPanelElements){
//            item->move(sf::Vector2f(0.0f, moveDist));
//        }
//    }

    for(size_t i = 0; i < groups.size();){
        groups[i]->update(event, mousePos);
        if(groups[i]->empty()){
            removeAssembly(groups[i]);
        }
        else ++i;
    }

    if(hasFocus()){
        for(auto& key : event.keyLog){
            switch(key){
                case 'c':{
                    if(ctrlPressed() && !selected.empty()){
                        stringstream cpyData;
                        for(size_t i = 0; i < selected.size(); ++i){
                            cpyData << assemblyStateTags[selected[i]->getCurrentState()]
                                    << '\t' << selected[i]->tag();
                            if(i < selected.size() - 1) cpyData << '\n';
                        }
                        copyToClipboard(cpyData.str());
                    }
                    break;
                }
                case 'v':{
                    if(ctrlPressed()){
                        string data = getClipboardText();
                        stringstream ss(data);
                        unsigned int state = 0;
                        if(!data.empty()){
                            while(getline(ss, data)){
                                for(size_t i = 0; i < data.size();){
                                    if(data[i] == '\t'){
                                        string stateStr;
                                        stateStr.assign(data.begin(), data.begin() + i);

                                        state = getMatchingIndex(stateStr, assemblyStateTags);
                                        if(state == UINT_MAX) break;

                                        data.erase(data.begin(), data.begin() + i + 1);
                                        i = 0;
                                        goto assembleCopied;
                                    }
                                    else ++i;
                                }

                                break;

                                assembleCopied:;
                                addAssemblyItem(getPosition() + getSize()/2, data, state);
                            }
                        }
                    }
                    break;
                }
                case CV_KEY_DELETE:{
                    for(auto& item : selected){
                        detachMember(item);
                        removePanelElement(item);
                    }
                    selected.clear();
                    break;
                }
                default:{
                    break;
                }
            }
        }

        event.clearKeys();
    }

    if(event.LMBhold && bounds.contains(mousePos)){
        for(auto& item : viewPanelElements){
            if(event.isCaptured(*item)){
                assembly* item_asmb = nullptr;
                item_asmb = getAssembly(item);

                if(item_asmb == nullptr){ // Does not already belong to an assembly - make a new one

                    for(auto& other : viewPanelElements){
                        if((other == item) || (other->tag() == item->tag())) continue;

                        if(assemblyType & CV_ASSEMBLE_HORIZONTAL){

                            assembly* asmb = nullptr;

                            sf::FloatRect captureBounds = other->getBounds();
                            captureBounds.left -= snapDistance;
                            captureBounds.width += 2*snapDistance;

                            if(item->getBounds().intersects(captureBounds)){
                                event.freeze_capture(*item);
                                if(item->getBounds().left + item->getBounds().width/2 <
                                    captureBounds.left + captureBounds.width/2){
                                        asmb = getAssembly(other);
                                        if(asmb != nullptr){
                                            asmb->addMemberBefore(item, other);
                                        }
                                        else{
                                            newAssembly({item, other});
                                        }
                                        item->setPosition(sf::Vector2f(other->getBounds().left - item->getBounds().width -
                                                                           item->getOutlineThickness() - other->getOutlineThickness(),
                                                                           (other->getBounds().top +
                                                                            (item->getBounds().height - other->getBounds().height)/2)));
                                }
                                else{
                                    asmb = getAssembly(other);
                                    if(asmb != nullptr){
                                        asmb->addMemberAfter(item, other);
                                    }
                                    else{
                                        newAssembly({other, item});
                                    }
                                    item->setPosition(sf::Vector2f(other->getBounds().left + other->getBounds().width +
                                                                       other->getOutlineThickness() + item->getOutlineThickness(),
                                                                           (other->getBounds().top +
                                                                            (item->getBounds().height - other->getBounds().height)/2)));
                                }
                            }
                        }
                        if(assemblyType & CV_ASSEMBLE_VERTICAL){

                            assembly* asmb = nullptr;

                            sf::FloatRect captureBounds = other->getBounds();
                            captureBounds.top -= snapDistance;
                            captureBounds.height += 2*snapDistance;

                            if(item->getBounds().intersects(captureBounds)){
                                event.freeze_capture(*item);
                                if(item->getBounds().top + item->getBounds().height/2 <
                                    captureBounds.top + captureBounds.height/2){
                                        asmb = getAssembly(other);
                                        if(asmb != nullptr){
                                            asmb->addMemberBefore(item, other);
                                        }
                                        else{
                                            newAssembly({item, other});
                                        }
                                        item->setPosition(sf::Vector2f(other->getBounds().left + ((item->getBounds().width - other->getBounds().width)/2),
                                                                           other->getBounds().top - item->getBounds().height -
                                                                           item->getOutlineThickness() - other->getOutlineThickness()));
                                }
                                else{
                                    asmb = getAssembly(other);
                                    if(asmb != nullptr){
                                        asmb->addMemberAfter(item, other);
                                    }
                                    else{
                                        newAssembly({other, item});
                                    }
                                    item->setPosition(sf::Vector2f(other->getBounds().left + ((item->getBounds().width - other->getBounds().width)/2),
                                                                           other->getBounds().top + other->getBounds().height +
                                                                           item->getOutlineThickness() + other->getOutlineThickness()));
                                }
                            }
                        }
                    }
                }
                else if(item->getBounds().contains(event.LMBpressPosition) && !item->getBounds().contains(mousePos)){
                    detachMember(item);
//                    item->move(components(snapDistance, get_angle(item->getPosition() + item->getSize()/2, mousePos)));
                }
                else if(!event.isCaptured(*item_asmb)) event.freeze_capture(*item);
            }
        }
    }

    return true;
}

void CVAssemblyPanel::newAssembly(const vector<CVElement*>& members){
    groups.emplace_back(new assembly(members, *this));
    groups.back()->setDraggableStatus(true);
}

void CVAssemblyPanel::setTracks(const bool& status){
    bUseTracks = true;
}

void CVAssemblyPanel::setPanelItemSize(const sf::Vector2f& newSize){
    if((newSize.x == itemSize.x) && (newSize.y == itemSize.y)) return;
    vector<CVElement*> remaining = viewPanelElements;
    for(auto& asmb : groups){
        sf::Vector2f sizeDiff;
        if(!asmb->empty()){
            sizeDiff = newSize - asmb->members.front()->getSize();
            asmb->members.front()->setSize(newSize);
            remove(asmb->members.front(), remaining);
        }
        for(size_t i = 1; i < asmb->members.size(); ++i){
            asmb->members[i]->move(sizeDiff);
            remove(asmb->members[i], remaining);
        }
    }

    for(auto& item : remaining){
        item->setSize(newSize);
    }

    itemSize = newSize;
}

void CVAssemblyPanel::setMaxLabelLength(const unsigned int& newLength){
    if(newLength == maxLabelLength) return;

    maxLabelLength = newLength;
    for(auto& item : viewPanelElements){
        if(dynamic_cast<CVTextBox*>(item) == nullptr) continue;

        string newLabel = item->tag();
        if(newLabel.size() > maxLabelLength){
            newLabel.erase(newLabel.begin() + maxLabelLength, newLabel.end());
            newLabel += "...";
        }

        ((CVTextBox*)item)->setString(newLabel);
    }
}

void CVAssemblyPanel::getAssemblyTags(StringMatrix& output){
    for(auto& asmb : groups){
        output.emplace_back();
        for(auto& item : asmb->members){
            output.back().emplace_back(item->tag());
        }
    }
}

void CVAssemblyPanel::move(const sf::Vector2f& distance){
    CVViewPanel::move(distance);
    scrollBarY.move(distance);
    for(auto& asmb : groups){
        asmb->move(distance);
    }
}
void CVAssemblyPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

CVAssemblyPanel::~CVAssemblyPanel(){
    for(auto& asmb : groups){
        delete(asmb);
    }
}

}
