#include "cvision/panel.hpp"
#include "algorithm.hpp"

using namespace AIALG;

namespace cvis{

bool CVBasicViewPanel::draw(sf::RenderTarget* target){

    CV_DRAW_CLIP_BEGIN

    if(!CVTextBox::draw(target)) return false;

    for(auto& panel : viewPanelElements){
        if(bOutOfBoundsDraw || getBounds().intersects(panel->getBounds())) panel->draw(target);
    }

    CV_DRAW_CLIP_END

    return true;
}

bool CVBasicViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos){ // Disperse update function
    if(!CVViewPanel::update(event, mousePos)) return false;
    for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
        if(bOutOfBoundsUpdate || View->contains(*panel)){
            panel->update(event, mousePos);
        }
    }
    return true;
}

CVSwitchPanel::CVSwitchPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         viewIndex(0),
                         bCanPan(false),
                         bAutoPan(false),
                         panRate(60.0f),
                         elementPadding(3.0f){
    setExpand(true);
}

void CVSwitchPanel::setElementPadding(const float& newPadding){
    elementPadding = newPadding;
}

bool CVSwitchPanel::checkFocus(const std::string& tag){
    if(viewIndex >= viewPanelTags.size()) return false;
    return viewPanelTags[viewIndex] == tag;
}

CVElement* CVSwitchPanel::getActiveElement(){
    if(numPanels() < 1) return nullptr;
    else return viewPanelElements[viewIndex];
}

void CVSwitchPanel::addPanelElement(CVElement* newElement, std::string tag, const unsigned int& index){

    if(numPanels() > 0){
        if(index < numPanels()){
            newElement->setPosition(viewPanelElements[index]->getPosition());
            for(size_t i = index; i < numPanels(); ++i){
                viewPanelElements[i]->move(newElement->getBounds().width + elementPadding, 0.0f);
            }
        }
        else{
            newElement->setPosition(getPosition().x + bounds.width + elementPadding,
                          getPosition().y);
        }
    }
    else{
        setPosition(View->getWidth()/2 - newElement->getBounds().width/2, getPosition().y);
        newElement->setPosition(getPosition());
    }

    CVViewPanel::addPanelElement(newElement, tag, index);

    closeButtons.emplace_back(View, sf::Vector2f(newElement->getPosition().x + newElement->getBounds().width - 20.0f,
                                                   newElement->getPosition().y + 20.0f),
                                15.0f,15.0f,"gen_x",1,0,true,nullptr);
    closeButtons.back().setSpriteColor(sf::Color(160,160,160,160));

    setCenter(numPanels()-1);
}

void CVSwitchPanel::removePanelElement(const unsigned int& index){
    sf::FloatRect panelBounds = viewPanelElements[index]->getBounds();
    CVViewPanel::removePanelElement(index);
    closeButtons.erase(closeButtons.begin() + index);

    for(size_t i = index; i < viewPanelElements.size(); ++i){
        viewPanelElements[i]->move(sf::Vector2f(-panelBounds.width, 0.0f));
    }
    for(size_t i = index; i < closeButtons.size(); ++i){
        closeButtons[i].move(-panelBounds.width, 0.0f);
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
    for(auto& button : closeButtons){
        button.move(distance);
    }
}

void CVSwitchPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

bool CVSwitchPanel::draw(sf::RenderTarget* target){
    if(!CVBasicViewPanel::draw(target)) return false;

    for(auto& button : closeButtons){
        button.draw(target);
    }

    return true;
}

bool CVSwitchPanel::update(CVEvent& event, const sf::Vector2f& mousePos){

    for(size_t i = 0; i < closeButtons.size();){
        closeButtons[i].update(event, mousePos);
        if(closeButtons[i].getTrigger()){
            removePanelElement(i);
        }
        else ++i;
    }

    if(!CVBasicViewPanel::update(event, mousePos)) return false;

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

    if(bCanPan){
        if(bounds.contains(mousePos) &&
           (event.mouseWheelDelta.x != 0.0f)){ // Horizontal pan
            float moveDist = panRate*event.mouseWheelDelta.x;
            if(getPosition().x + moveDist < View->getWidth()/2){
                if(getPosition().x + getSize().x + moveDist > View->getWidth()/2){
                    move(sf::Vector2f(moveDist, 0.0f));
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

    if(bAutoPan && (viewIndex != UINT_MAX)){
        if(numPanels() == 0){
            setPosition(View->getWidth()/2, getPosition().y);
        }
        else{
            move(panRate/500*(sf::Vector2f(View->getBounds().left + View->getBounds().width/2,
                                           View->getBounds().top + View->getHeight()/2) -
                 sf::Vector2f(viewPanelElements[viewIndex]->getPosition().x + viewPanelElements[viewIndex]->getBounds().width/2,
                              viewPanelElements[viewIndex]->getPosition().y + viewPanelElements[viewIndex]->getBounds().height/2)));
        }
    }

    for(auto& button : closeButtons){
        button.setFocus(hasFocus());
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
    bAutoPan = true;
}

void CVSwitchPanel::setCenter(const std::string& tag){
    for(size_t i = 0; i < viewPanelTags.size(); ++i){
        if(viewPanelTags[i] == tag) setCenter(i);
    }
}

CVNodePanel::CVNodePanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         center(nullptr),
                         nodeIndex(0),
                         maxTraceLength(1000),
                         nodeAlpha(255),
                         bAutoPan(false),
                         bNodeOpened(false),
                         bNodeInteracted(false),
                         bShowTagOnHover(false),
                         panRate(25.0f),
                         nodeDistance((size.x + size.y)/2),
                         elasticCoef(5.0f),
                         pathThickness(2.0f),
                         anchorPoint(position){
    bTransduceFade = true; // Allow chaining of node panel fade effects
    bTransduceFocus = true;
    setDraggableStatus(true);
}

void CVNodePanel::setNodeAlpha(const uint8_t& newAlpha){
    nodeAlpha = newAlpha;
}

std::string CVNodePanel::lastInteracted() const{
    if(interactionTrace.size() > 0){
        return interactionTrace.back();
    }
    return std::string();
}

bool CVNodePanel::captureInteraction(std::string* output){
    if(interactionTrace.size() > 0){
        if(output != nullptr) *output = interactionTrace.front();
        interactionTrace.erase(interactionTrace.begin());
        return true;
    }
    return false;
}

void CVNodePanel::logInteraction(const std::string& tag){
    interactionTrace.push_back(tag);
    if(interactionTrace.size() > maxTraceLength) interactionTrace.pop_back();
    bNodeInteracted = true;
}

void CVNodePanel::setHoverTag(const bool& status){
    bShowTagOnHover = status;

    if(bShowTagOnHover){
        for(size_t i = 0; i < numPanels(); ++i){
            if(i >= hoverTags.size()){
                hoverTags.emplace_back(viewPanelElements[i]->tag(), *mainApp()->fonts[textInfo.font], textInfo.fontSize);
            }
        }
    }
    else{
        hoverTags.clear();
    }
}

bool CVNodePanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVViewPanel::update(event, mousePos)) return false;

    if(bAutoPan && (nodeIndex != UINT_MAX)){
        if(numPanels() == 0){
            setPosition(anchorPoint);
        }
        else{
            move(panRate/250*(anchorPoint -
                              sf::Vector2f(viewPanelElements[nodeIndex]->getPosition().x,
                                           viewPanelElements[nodeIndex]->getPosition().y)));
        }
    }

    if(event.LMBreleased && (event.LMBreleaseFrames == 2)){
        if(center->getBounds().contains(event.LMBreleasePosition) &&
       event.captureMouse() && event.captureFocus()){
            setFocus(true);
            if(event.LMBreleased && (event.LMBholdTime < 0.15f)){
                bNodeOpened = !bNodeOpened;
            }
       }
    }
    else if(event.LMBhold && bounds.contains(event.lastFrameMousePosition) &&
            event.captureMouse()){
                setFocus(true);
    }

    if(hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) bNodeOpened = false;

    if(event.LMBhold ||
       (hasFocus() && ((event.LMBreleaseTime < 2.0f) || (event.timeLastKey < 2.0f)))){
        if(bNodeOpened){
            sf::Vector2f onscreen_arc = internal_arc(anchorPoint, View->getBounds(), nodeDistance);
//            std::cout << nodeDistance << ' ' << onscreen_arc.x << ' ' << onscreen_arc.y << "       \r";
            for(size_t i = 0; i < numPanels(); ++i){
                if(viewPanelElements[i] == center) continue;
                float angle = -(i*2*PI/(numPanels()-1));
                if(!View->getBounds().contains(radial_position(anchorPoint, nodeDistance, angle)))
                        angle = -(onscreen_arc.x + i*(onscreen_arc.y-onscreen_arc.x)/(numPanels()));
                viewPanelElements[i]
                    ->move(elasticCoef*event.lastFrameTime*(radial_position(anchorPoint, nodeDistance, // Move toward radial position from the anchorpoint at distance of
                            angle) // nodeDistance proportional to the elastic coefficient, at a fractional angle of the available arc
                           - viewPanelElements[i]->getPosition()));

                nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), anchorPoint), pathThickness));
                nodePaths[i].setPosition(anchorPoint);
                nodePaths[i].setRotation(get_angle(anchorPoint, viewPanelElements[i]->getPosition())*180/PI);
                nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());

                if(viewPanelElements[i]->isType<CVNodePanel>()){    // Ensure other nodes unfold if path is selected
                    if(pathSelected[i]){ // Path selected vector only detects CVNodePanel class types
                        ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = true;
                    }
                }
            }

        }
        else{
            for(size_t i = 0; i < numPanels(); ++i){
                if(viewPanelElements[i] == center) continue;
                viewPanelElements[i] // Shrink toward center
                    ->move(elasticCoef*event.lastFrameTime*(anchorPoint - viewPanelElements[i]->getPosition()));

                if(viewPanelElements[i]->isType<CVNodePanel>()){
                    if(((CVNodePanel*)viewPanelElements[i])->bNodeOpened){
                        ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = false;
                    }
                }

                viewPanelElements[i]->setFade(0, -5);
                nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), anchorPoint), pathThickness));
                nodePaths[i].setPosition(anchorPoint);
                nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());
            }
        }
    }

    for(size_t i = 0; i < numPanels(); ++i){
        if(i > 0){
            if(bNodeOpened){
                viewPanelElements[i]->setFade(nodeAlpha, 5);
            }
            else viewPanelElements[i]->setFade(0, -5);
        }

        if(viewPanelElements[i]->hasFocus()) setFocus(true);

        if(bShowTagOnHover){
            sf::Color tmp;
            if(bNodeOpened && !(viewPanelElements[i]->isType<CVNodePanel>() && ((CVNodePanel*)viewPanelElements[i])->isOpen())){
                sf::Color col;
                if((viewPanelElements[i]->getBounds().contains(mousePos) || altPressed())){
                    tmp = hoverTags[i].getFillColor(); // Transfer the element color hue but not alpha
                    col = viewPanelElements[i]->baseFillColor();
                    uint8_t alpha = tmp.a;
                    tmp = col;
                    if(alpha + 5 < nodeAlpha) tmp.a = alpha + 5;
                    else tmp.a = nodeAlpha;
                    hoverTags[i].setFillColor(tmp);
                }
                else{
                    tmp = hoverTags[i].getFillColor();
                    col = viewPanelElements[i]->baseFillColor();
                    uint8_t alpha = tmp.a;
                    tmp = col;
                    if(alpha >= 2) tmp.a = alpha - 2;
                    else tmp.a = 0;
                    hoverTags[i].setFillColor(tmp);
                }

                hoverTags[i].setPosition(sf::Vector2f(viewPanelElements[i]->getBounds().left + viewPanelElements[i]->getBounds().width/2
                                                               - hoverTags[i].getGlobalBounds().width/2,
                                                              viewPanelElements[i]->getBounds().top + viewPanelElements[i]->getBounds().height +
                                                                hoverTags[i].getGlobalBounds().height + 12.0f));
            }
            else{
                tmp = hoverTags[i].getFillColor();
                tmp.a = 0;
                hoverTags[i].setFillColor(tmp);
            }
        }
    }

    bool interactionCaptured = false;
    int index = numPanels()-1;

    for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
        if(View->contains(*panel)){
            panel->update(event, mousePos);
        }

        if(bNodeOpened && !interactionCaptured && event.LMBreleased &&
           (event.LMBreleaseFrames == 1) &&
           panel->getBounds().contains(mousePos)){
               if(panel->isType<CVNodePanel>()){
                      if(((CVNodePanel*)panel)->isOpen()){
                            std::string interaction;
                            if(((CVNodePanel*)panel)->captureInteraction(&interaction)){
                                logInteraction(interaction);
                                interactionCaptured = true;
                            }
                            if(((CVNodePanel*)panel)
                                ->center
                                    ->getBounds().contains(mousePos)) pathSelected[index] = false;
                      }
                      else{
                            pathSelected[index] = true;
//                            setCenter(*panel);
                      }
                }
                else{
                    logInteraction(viewPanelTags[index]);
                    interactionCaptured = true;
                }
        }
        --index;

    }

    return true;
}

void CVNodePanel::setNode(const int& index){
    if(index >= (int)numPanels()){
        nodeIndex = 0;
    }
    else if(index < 0){
        nodeIndex = numPanels() - 1;
    }
    else{
        nodeIndex = index;
    }
    bAutoPan = true;
}

void CVNodePanel::setNodeRadius(const float& newRadiusPx){
    nodeDistance = newRadiusPx;
}

bool CVNodePanel::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;
    for(auto& line : nodePaths){
        target->draw(line);
    }
    for(auto& item : viewPanelElements){
        item->draw(target);
    }
    for(auto& text : hoverTags){
        target->draw(text);
    }

    return true;
}

void CVNodePanel::setCenter(CVElement& element){
    center = &element;
    anchorPoint = element.getPosition() - element.getOrigin();
}

void CVNodePanel::addPanelElement(CVElement* newElement, std::string newTag){
    CVViewPanel::addPanelElement(newElement, newTag);
    nodePaths.emplace_back();
    nodePaths.back().setSize(sf::Vector2f(0.0f, pathThickness));
    nodePaths.back().setOrigin(sf::Vector2f(0.0f, pathThickness/2));
    if(center != nullptr)
        nodePaths.back().setFillColor(center->baseFillColor());
    else nodePaths.back().setFillColor(sf::Color::Transparent);

    if(bShowTagOnHover){
        hoverTags.emplace_back(newTag, *mainApp()->fonts[textInfo.font], textInfo.fontSize);
    }

    pathSelected.push_back(false);
}

void CVNodePanel::removePanelElement(const unsigned int& index){
    CVViewPanel::removePanelElement(index);
    nodePaths.erase(nodePaths.begin() + index);
    if(nodeIndex > (int)index) setNode(index-1);
    else setNode(index);

    pathSelected.erase(pathSelected.begin() + index);
}

void CVNodePanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
    anchorPoint += distance;
}

void CVNodePanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

CVListPanel::CVListPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         listPadding(12.0f),
                         scrollBarPadding(24.0f),
                         outerPadding(30.0f),
                         innerPadding(12.0f),
                         listItemHeight(40.0f*View->getViewScale()),
                         listItemColor(colorTheme.front()),
                         scrollBarY(parentView,
                                    sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                   bounds.top + scrollBarPadding),
                                    sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                                        bounds.top + bounds.height - scrollBarPadding),
                      size.y, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
                      1000.0f, CV_OBJ_ANIM_FADE_IN){
    scrollBarY.setPanelSize(size.y - 2*outerPadding);
    scrollBarY.setPosition(sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                                        bounds.top + bounds.height - scrollBarPadding));

    colorTheme.emplace_back(backgroundColor);
    darken(colorTheme.back(), 50);
    colorTheme.back().a += 100;
    listItemColor = std::ref(colorTheme.back());
    highlightColor = getListItemColor();
    brighten(highlightColor, 50);
}

void CVListPanel::toggleSelection(CVElement* element){
    for(size_t i = 0; i < selected.size(); ++i){
        if(selected[i] == element){
            selected.erase(selected.begin() + i);
            element->highlight(false);
            return;
        }
    }
    selected.push_back(element);
    element->highlight(true);
}

void CVListPanel::toggleSelection(const std::string& tag){
    for(auto& item : viewPanelElements){
        if(item->tag() == tag){
            toggleSelection(item);
            return;
        }
    }
}

void CVListPanel::setSelection(CVElement* element, const bool& state){
    if(state){
        if(anyEqual(element, selected)) return;
        selected.push_back(element);
        element->highlight(true);
    }
    else{
        for(size_t i = 0; i < selected.size(); ++i){
            if(selected[i] == element){
                selected.erase(selected.begin() + i);
                element->highlight(false);
                return;
            }
        }
    }
}

void CVListPanel::setSelection(const std::string& tag, const bool& state){
    for(auto& item : viewPanelElements){
        if(item->tag() == tag){
            setSelection(item);
            return;
        }
    }
}

StringVector CVListPanel::getSelection() const{
    StringVector output;
    for(auto& element : selected){
        output.emplace_back(element->tag());
    }
    return output;
}

void CVListPanel::clearSelection(){
    for(auto& item : selected){
        item->highlight(false);
    }
    selected.clear();
}

bool CVListPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
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


    float listHeight = 0.0f;
    if(numPanels() > 0) listHeight = viewPanelElements.back()->getBounds().top +
       viewPanelElements.back()->getBounds().height -
       viewPanelElements.front()->getBounds().top;

    scrollBarY.setScrollMax(listHeight);
    scrollBarY.update(event, mousePos);

    if(listHeight > bounds.height){
        float moveDist = 2*outerPadding + bounds.top -
                            viewPanelElements.front()->getBounds().top
                            - scrollBarY.getScrollOffset();
        for(auto& item : viewPanelElements){
            item->move(sf::Vector2f(0.0f, moveDist));
        }
    }
    else if(numPanels() > 0){
        float moveDist = bounds.top + outerPadding - viewPanelElements.front()->getBounds().top;
        for(auto& item : viewPanelElements){
            item->move(sf::Vector2f(0.0f, moveDist));
        }
    }

    if(bounds.contains(event.LMBpressPosition) && (event.LMBreleaseFrames == 1)){
        if(hasFocus()){
            for(auto& item : boost::adaptors::reverse(viewPanelElements)){
                if(item->getBounds().contains(mousePos) && event.captureMouse()){
                    clearSelection();
                    setSelection(item, true);
                }
            }
        }
        else setFocus(false);
    }

    return true;
}

bool CVListPanel::draw(sf::RenderTarget* target){
    if(!CVBasicViewPanel::draw(target)) return false;
    scrollBarY.draw(target);

    return true;
}

void CVListPanel::addTextEntry(const std::string& newText, const unsigned int& index){
    CVTextBox* newItem = new CVTextBox(View, sf::Vector2f(bounds.left + innerPadding,
                                                          bounds.top + innerPadding),
                                       bounds.width - 2*outerPadding,
                                       listItemHeight, getListItemColor(),
                                            sf::Color::Transparent, 0.0f);
    newItem->setRounding(8.0f*View->getViewScale(), 10);
    newItem->setTextWrap(true);
    newItem->setExpand(true);

    newItem->addTextEntry(textEntry(newText, textInfo.font, textInfo.fontSize, textInfo.alignment, textInfo.textColor),
                          sf::Vector2f(newItem->getBounds().left + innerPadding,
                                       newItem->getBounds().top + innerPadding));

    newItem->setMask(appTexture("gradient_linear"));
    newItem->setMaskColor(sf::Color(255,255,255,150));

    addPanelElement(newItem, newText, index);
    listTags.emplace_back(newText);
}

void CVListPanel::addTextEntries(const StringVector& newText){
    for(auto& text : newText){
        addTextEntry(text);
    }
}

void CVListPanel::setTextEntries(const StringVector& newText){
    size_t i = 0;
    for(; (i < newText.size() && i < listTags.size()); ++i){
        if(newText[i] != listTags[i]){
            listTags.insert(listTags.begin() + i, newText[i]);
            addTextEntry(newText[i], i);
            ++i;
        }
    }
    while(i < newText.size()){
        addTextEntry(newText[i]);
        ++i;
    }
    for(i = 0; i < listTags.size(); ++i){
        if(!anyEqual(listTags[i], newText)){
            removeTextEntry(i);
        }
    }
}

void CVListPanel::removeTextEntry(const unsigned int& index){
    if(index < numPanels()){
        for(size_t i = index; i < numPanels(); ++i){
            viewPanelElements[i]->move(0.0f, -viewPanelElements[index]->getBounds().height - listPadding);
        }
        removePanelElement(index);
    }
    else{
        removePanelElement(UINT_MAX);
    }
}

void CVListPanel::setListPadding(const float& newPadding){
    for(size_t i = 1; i < viewPanelElements.size(); ++i){
        viewPanelElements[i]->move(sf::Vector2f(newPadding - listPadding, 0.0f));
    }
    listPadding = newPadding;
}

void CVListPanel::setOuterPadding(const float& newPadding){
    sf::Vector2f moveDist(0.0f,0.0f);
    for(auto& item : viewPanelElements){
        moveDist.x = 0.0f;
        moveDist.y = 0.0f;

        if(item->getBounds().left < bounds.left + newPadding)
            moveDist.x = bounds.left + newPadding - item->getBounds().left;
        if(item->getBounds().left + item->getBounds().width > bounds.left + bounds.width - newPadding)
            moveDist.x = bounds.left + bounds.width + newPadding - item->getBounds().left - item->getBounds().width;
        if(item->getBounds().top < bounds.top + newPadding)
            moveDist.y = bounds.top + newPadding - item->getBounds().top;
        if(item->getBounds().top + item->getBounds().height > bounds.top + bounds.height - newPadding)
            moveDist.y = bounds.top + bounds.height + newPadding - item->getBounds().left - item->getBounds().height;

        item->move(moveDist);
    }

    scrollBarY.move(sf::Vector2f(outerPadding - newPadding, 0.0f));
    outerPadding = newPadding;
}

void CVListPanel::setScrollBarPadding(const float& newPadding){
    scrollBarPadding = newPadding;
    scrollBarY.setAnchorPoints(sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                   bounds.top + scrollBarPadding),
                                    sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                                        bounds.top + bounds.height - scrollBarPadding));
}

void CVListPanel::setInnerPadding(const float& newPadding){
    innerPadding = newPadding;
    for(auto& item : viewPanelElements){
        if(item->isType<CVTextBox>()){
            ((CVTextBox*)item)->setTextPadding(innerPadding);
        }
    }
}

void CVListPanel::setDragAndDrop(const bool& status){
    for(auto& item : viewPanelElements){
        item->setDragAndDrop(status);
    }
}

void CVListPanel::addPanelElement(CVElement* newElement, std::string newTag, const unsigned int& index){
    if(numPanels() == 0){
        newElement->setPosition(bounds.left + outerPadding,
                               bounds.top + outerPadding);
    }
    else{
        if(index < numPanels()){
            newElement->setPosition(viewPanelElements[index]->getPosition());
            for(size_t i = index; i < numPanels(); ++i){
                viewPanelElements[index]->move(0.0f, newElement->getBounds().height + listPadding);
            }
        }
        else{
            newElement->setPosition(bounds.left + outerPadding,
                                   viewPanelElements.back()->getBounds().top +
                                   viewPanelElements.back()->getBounds().height + listPadding);
        }
    }

    if(canHighlight){
        newElement->setHighlightableStatus(canHighlight);
        newElement->setHighlightColor(highlightColor);
    }

    newElement->setDragAndDrop(true);

    CVViewPanel::addPanelElement(newElement, newTag, index);
}

void CVListPanel::removePanelElement(const unsigned int& index){
    if(index > numPanels()){
        float shiftY = viewPanelElements.back()->getBounds().height;
        remove(viewPanelElements.back(), selected);
        CVViewPanel::removePanelElement(index);
    }
    else{
        float shiftY = viewPanelElements[index]->getBounds().height;
        remove(viewPanelElements[index], selected);
        CVViewPanel::removePanelElement(index);
        for(size_t i = index; i < numPanels(); ++i){
            viewPanelElements[i]->move(0.0f,-shiftY);
        }
        scrollBarY.scroll(-shiftY);
    }
}

void CVListPanel::move(const sf::Vector2f& distance){
    CVViewPanel::move(distance);
    scrollBarY.move(distance);
}

void CVListPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

CVAssemblyPanel::CVAssemblyPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
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
                      1000.0f, CV_OBJ_ANIM_FADE_IN){

    scrollBarY.setPanelSize(size.y);
    scrollBarY.setPosition(sf::Vector2f(bounds.left + bounds.width - scrollBarPadding/2,
                                                        bounds.top + scrollBarPadding));


}

CVAssemblyPanel::assembly::assembly(const std::vector<CVElement*>& members,
                                    CVAssemblyPanel& host):
    CVBox(host.View, host.getPosition(), 0.0f,0.0f,host.highlightColor, sf::Color::Transparent, 0.0f),
    host(&host),
    itemPadding(12.0f*viewScale()),
    outerPadding(18.0f*viewScale()),
    label(host.View, getPosition(), host.itemSize.x*2, host.itemSize.y/4,
          textEntry("Name this category...", mainApp()->getDefaultFont(), 18*viewScale(), ALIGN_LEFT_MIDLINE, sf::Color::Black),
          host.highlightColor, host.highlightColor, 1.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL),
    acceptButton(host.View, getPosition(), label.getBounds().height*0.6f, label.getBounds().height*0.6f, "gen_check", 1, 0, true),
    cancelButton(host.View, getPosition(), label.getBounds().height*0.6f, label.getBounds().height*0.6f, "gen_x", 1, 0, true),
    members(members){

        sf::Color tmp = baseFillColor();
        tmp.a = 50;
        setFillColor(tmp);

        tmp = host.highlightColor;
        tmp.a = 30;
        label.setFillColor(tmp);

        darken(tmp, 20);
        tmp.a = 80;

        label.setOutlineColor(tmp);

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
        std::swap(members[firstIndex], members[secondIndex]);
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
                switch(members[i]->getCurrentState()){
                    case 1:{
                        assembly_info.back() << "Descending";
                        break;
                    }
                    default:{
                        assembly_info.back() << "Ascending";
                        break;
                    }
                }
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

bool CVAssemblyPanel::assembly::getAssemblyInfo(std::ostream& output){
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

bool CVAssemblyPanel::getAssemblyInfo(std::ostream& output){
    for(auto& asmb : groups){
        if(asmb->getAssemblyInfo(output)) return true;
    }
    return false;
}

void CVAssemblyPanel::addAssemblyItem(const sf::Vector2f& position, const std::string& label, const unsigned int& state){

    std::string inputData = label;
    if(inputData.size() > maxLabelLength - 5){
        std::string tmp;
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

    newBox->setDraggableStatus(true);
    newBox->setPosition(position - newBox->getSize()/2);

    newBox->setNumStates(2);
    newBox->setToggleFlip(false, true);

    newBox->addSprite(appTexture("arrow_right_half"),
                      sf::Vector2f(itemSize.x/4, itemSize.y/2),
                      sf::Vector2f(itemSize.x/3, itemSize.y*0.8f),
                      sf::Color(15, 80, 255, 100));

    newBox->setMask(appTexture("gradient_linear"));
    newBox->setMaskColor(sf::Color(255,255,255,160));

    newBox->setResponseEvent(CV_EVENT_RMB);

    newBox->setState(state);

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
        std::string inputData;
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
                        std::stringstream cpyData;
                        for(size_t i = 0; i < selected.size(); ++i){
                            switch(selected[i]->getCurrentState()){
                                case 1:{
                                    cpyData << "Descending";
                                    break;
                                }
                                default:{
                                    cpyData << "Ascending";
                                    break;
                                }
                            }
                            cpyData << '\t' << selected[i]->tag();
                            if(i < selected.size() - 1) cpyData << '\n';
                        }
                        copyToClipboard(cpyData.str());
                    }
                    break;
                }
                case 'v':{
                    if(ctrlPressed()){
                        std::string data = getClipboardText();
                        std::stringstream ss(data);
                        unsigned int state = 0;
                        if(!data.empty()){
                            while(std::getline(ss, data)){
                                for(size_t i = 0; i < data.size();){
                                    if(data[i] == '\t'){
                                        std::string stateStr;
                                        stateStr.assign(data.begin(), data.begin() + i);

                                        if(cmpString(stateStr, "Descending")) state = 1;
                                        else if(cmpString(stateStr, "Ascending")) state = 0;
                                        else break; // Not suitable data format

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

void CVAssemblyPanel::newAssembly(const std::vector<CVElement*>& members){
    groups.emplace_back(new assembly(members, *this));
    groups.back()->setDraggableStatus(true);
}

void CVAssemblyPanel::setTracks(const bool& status){
    bUseTracks = true;
}

void CVAssemblyPanel::setPanelItemSize(const sf::Vector2f& newSize){
    if((newSize.x == itemSize.x) && (newSize.y == itemSize.y)) return;
    std::vector<CVElement*> remaining = viewPanelElements;
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

        std::string newLabel = item->tag();
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
