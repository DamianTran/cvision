#include "cvision/cvis_base.hpp"

using namespace AIALG;

namespace cvis{

bool FontManager::addFont(std::string fileDir, std::string tag){
    fonts.push_back(sf::Font());
    if(!fonts.back().loadFromFile(fileDir)){
        fonts.pop_back();
        return false;
    }
    fontTags.push_back(tag);
    return true;
}

void APP::setViewGUI(std::string viewTag, std::string GUItag){
    setViewGUI(*taggedItem(viewTag, viewList, viewTags),
               *taggedItem(GUItag, GUIlist, GUITags));
}

void APP::mainUpdate(){
    for(size_t i = 0; i < viewList.size(); ++i){
        if(viewList[i]->closed()) closeView(i);
    }
}

void GUIpanel::draw(){
    const std::vector<visElement*>::const_iterator iterEND = panelElements.end();
    for(std::vector<visElement*>::iterator iter = panelElements.begin(); iter != iterEND; ++iter){
            if((*iter)->isVisible()) (*iter)->draw();
    }

    const std::vector<sf::Text>::const_iterator textEND = overlayText.end();
    for(std::vector<sf::Text>::const_iterator textIT = overlayText.begin();
        textIT != textEND; ++textIT){
            parentGUI->View->viewPort->draw(*textIT);
    }
}

inline void GUIpanel::clear(){
    overlayText.clear();
    const c_itr<visElement*> itEND = panelElements.end();
    for(itr<visElement*> it = panelElements.begin(); it != itEND; ++it){
        delete(*it);
    }
    panelElements.clear();
}

void GUIpanel::addText(const textEntry& newText, const sf::Vector2f& position){

    overlayText.emplace_back(
        newText.text,
        *parentGUI->View->mainApp->fonts[newText.font],
        newText.fontSize
    );

    overlayText.back().setFillColor(newText.textColor);

    switch(newText.alignment){
        case ALIGN_CENTER:{
            sf::FloatRect textBounds = overlayText.back().getLocalBounds();
            overlayText.back().setPosition(position.x - textBounds.width/2,
                                           position.y);
            break;
        }
        case ALIGN_RIGHT:{
            sf::FloatRect textBounds = overlayText.back().getLocalBounds();
            overlayText.back().setPosition(position.x - textBounds.width, position.y);
            break;
        }
        case ALIGN_VERTICAL:{
            overlayText.back().setRotation(-90);
            sf::FloatRect textBounds = overlayText.back().getGlobalBounds();
            overlayText.back().setPosition(position.x + textBounds.width, position.y + textBounds.height);
            break;
        }
        default:{
            overlayText.back().setPosition(position.x, position.y);
            break;
        }
    }

}

GUIpanel::~GUIpanel(){
    for(std::vector<visElement*>::iterator it = panelElements.begin();
        it != panelElements.end(); ++it){
            delete(*it);
        }
}

GUI::GUI(sf::RenderWindow* window, view* View):
        visible(true),
        mainApp(View->mainApp),
        View(View),
        viewPort(window){ }

void GUI::draw(){
    for(std::vector<GUIpanel>::iterator it = panelList.begin();
        it != panelList.end(); ++it){
            if(it->isVisible()) it->draw();
    }
}

inline void GUI::removePanel(std::string panelName){
    unsigned int matchIndex = taggedIndex(panelName, panelTags);
    if(matchIndex != UINT_MAX){
        panelList.erase(panelList.begin() + matchIndex);
        panelTags.erase(panelTags.begin() + matchIndex);
    }
}

void GUI::addPanelElement(std::string panelName, visElement* newElement){
    taggedItem(panelName, panelList, panelTags)->addElement(newElement);
}

GUIpanel* GUI::taggedPanel(const std::string& tag){
    return taggedItem(tag, panelList, panelTags);
}

view::view(unsigned int x, unsigned int y, std::string winName,
           uint32_t style, APP* mainApp,
           const sf::Vector2f& screenPosition, GUI* overlay):
    forceClose(false),
    bClosed(false),
    bElasticSelect(false),
    numPendingAnims(0),
    appThread(nullptr),
    frameRateLimit(mainApp->frameRate),
    frameRate(0.0f),
    width(x), height(y),
    resizeTarget(width, height),
    resizeSpeed(0,0),
    moveSpeed(0,0),
    viewState(VIEW_STATE_STARTUP),
    viewPort(nullptr),
    overlay(overlay),
    mainApp(mainApp),
    tag(winName)
        {

        const float frameTime = 0.5f/frameRateLimit;
        mainApp->viewThreads.push_back(std::thread([&, x, y, winName, style, frameTime](){

            sf::RenderWindow window(sf::VideoMode(x, y), winName, style);
            moveTarget = window.getPosition();
            viewPort = &window;
            sf::Vector2i intPos;
            if(!isnan(screenPosition.x) && !isnan(screenPosition.y)) window.setPosition(sf::Vector2i(screenPosition));

            std::chrono::high_resolution_clock::time_point t0 = TIME_NOW;
            const std::chrono::duration<float> frameUpdateLatency(frameTime);

            std::chrono::duration<float> duration;

            CVEvent viewEvent; // Set up event transducer

            viewEvent.viewBounds = sf::FloatRect(moveTarget.x, moveTarget.y, x, y);
            viewEvent.lastViewBounds = viewEvent.viewBounds;
            viewEvent.lastFrameMousePosition = sf::Vector2f(sf::Mouse::getPosition(window));

            while(!bClosed && window.isOpen()){

                if(forceClose){
                    window.close();
                    break;
                }

                intPos = sf::Mouse::getPosition(window);
                mousePos.x = intPos.x;
                mousePos.y = intPos.y;

                if(!bClosed) sync();
                if(!bClosed)
                    if(!update(viewEvent, mousePos)) break;

                window.clear();
                if(window.isOpen()) draw();
                window.display();

                // Framerate limit cycle

                duration = TIME_NOW - t0;

                if(duration < frameUpdateLatency){
                    std::this_thread::sleep_for(std::chrono::duration<float>(frameTime - duration.count()));
                }

                viewEvent.lastFrameTime = std::chrono::duration<float>(TIME_NOW - t0).count();
                t0 = TIME_NOW;
                frameRate = 1.0f/viewEvent.lastFrameTime;

            }

            if(window.isOpen()) window.close();
            viewState = VIEW_STATE_CLOSED;

        }));
        appThread = &mainApp->viewThreads.back(); // Establish reference to app thread for this view

        while(viewPort == nullptr){
            std::this_thread::sleep_for(std::chrono::duration<float>(frameTime)); // Wait for viewPort open to finish
        }
}

inline void view::setVisible(const unsigned int index, bool newVisibleState){
     viewPanels[index]->setVisible(newVisibleState);
}

inline void view::isolateVisible(const unsigned int index){ // Focus visibility on one panel in view
    size_t L = viewPanels.size();
    for(size_t i = 0; i < L; ++i){
        if(i == index) viewPanels[i]->setVisible(true);
        else viewPanels[i]->setVisible(false);
    }
}

CVViewPanel* view::getTaggedPanel(const std::string s){
    return *taggedItem(s, viewPanels, panelTags);
}
CVViewPanel* view::operator[](const std::string s){
    return *taggedItem(s, viewPanels, panelTags);
}

view* view::taggedSubView(std::string tag){
    return *taggedItem(tag, subViews, subViewTags);
}

void view::addSubView(view* newSubView, std::string tag){
    if(addUnique(newSubView, subViews)){
        if(tag.size() > 0) subViewTags.push_back(tag);
        else subViewTags.push_back("Subview " + std::to_string(subViews.size()));
    }
}

void view::addPanel(CVViewPanel* newPanel, std::string tag){
    if(addUnique(newPanel, viewPanels)){
        if(tag.size() > 0) panelTags.push_back(tag);
        else panelTags.push_back("Panel " + std::to_string(viewPanels.size()));
    }
}

void view::setVisiblePanel(const unsigned int i){
    unsigned int L = 0;
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(L != i) (*it)->setVisible(false);
            else (*it)->setVisible(true);
        }
}

void view::setVisiblePanel(const std::string tag){
    std::vector<std::string>::iterator strIT = panelTags.begin();
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(*strIT != tag) (*it)->setVisible(false);
            else (*it)->setVisible(true);
            ++strIT;
        }
}

void view::setVisiblePanel(CVViewPanel* panel){
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(*it != panel) (*it)->setVisible(false);
            else (*it)->setVisible(true);
        }
}

void view::setPosition(const sf::Vector2i& position){
    if(parentView != nullptr){ // Relative to parent if parent exists
        sf::Vector2i parentPosition(parentView->viewPort->getPosition());
        parentPosition.x += position.x;
        parentPosition.y += position.y;
        viewPort->setPosition(parentPosition);
    }
    else viewPort->setPosition(position);
}

inline void view::setScreenPosition(sf::Vector2i newPosition){
    viewPort->setPosition(newPosition);
}

void view::draw(){
    if(bClosed) return;
    if(viewPort == nullptr) return;
    if(!viewPort->isOpen()) return;
    switch(viewState){
        case VIEW_STATE_STARTUP:{
            break;
        }
        case VIEW_STATE_MAIN:{
            for(auto& item : viewPanels){
                if(item->isVisible()) item->draw();
            }
            if(overlay != nullptr){
                if(overlay->isVisible()) overlay->draw();
            }
            for(auto& item : popUpElements){
                if(item->isVisible()) item->draw();
            }
            break;
        }
        default: break;
    }
}

bool view::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(saveRequestFiles.size() > 0){
        sf::Texture screenshot;
        screenshot.create(getWidth(), getHeight());

        while(saveRequestFiles.size() > 0){
            screenshot.update(*viewPort);
            screenshot.copyToImage().saveToFile(saveRequestFiles.front());
            saveRequestFiles.erase(saveRequestFiles.begin());
        }
    }

    event.zoomDelta = 0.0f;
    event.keyLog.clear();
    event.viewResized = false;

    for(size_t i = 0; i < numPendingAnims;){
        if(pendingAnims[i].finished()){
            pendingAnims.erase(pendingAnims.begin() + i);
            --numPendingAnims;
        }
        else{
            pendingAnims[i].animFrame(event.lastFrameTime);
            ++i;
        }
    }

    if(!handleViewEvents(event)) return false;

    if(viewPort->getPosition() != moveTarget){
        sf::Vector2i viewPos(viewPort->getPosition());
        if(moveSpeed.x != 0){
            if(viewPos.x < moveTarget.x){
                if(viewPos.x + moveSpeed.x > moveTarget.x){
                   viewPos.x = moveTarget.x;
                   moveSpeed.x = 0;
                }
                else viewPos.x += moveSpeed.x;
            }
            if(viewPos.x > moveTarget.x){
                if(viewPos.x + moveSpeed.x < moveTarget.x){
                   viewPos.x = moveTarget.x;
                   moveSpeed.x = 0;
                }
                else viewPos.x += moveSpeed.x;
            }
        }

        if(moveSpeed.y != 0){
            if(viewPos.y < moveTarget.y){
                if(viewPos.y + moveSpeed.y > moveTarget.y){
                   viewPos.y = moveTarget.y;
                   moveSpeed.y = 0;
                }
                else viewPos.y += moveSpeed.y;
            }
            else if(viewPos.y > moveTarget.y){
                if(viewPos.y + moveSpeed.y < moveTarget.y){
                   viewPos.y = moveTarget.y;
                   moveSpeed.y = 0;
                }
                else viewPos.y += moveSpeed.y;
            }
        }
        viewPort->setPosition(viewPos);
    }

    if(viewPort->getSize() != resizeTarget){
        sf::Vector2u viewSize = viewPort->getSize();
        bool resizeFLAG = false;

        if(resizeSpeed.x != 0){
            if(viewSize.x < resizeTarget.x){
                if(viewSize.x + resizeSpeed.x > resizeTarget.x){
                   viewSize.x = resizeTarget.x;
                   resizeSpeed.x = 0;
                }
                else viewSize.x += resizeSpeed.x;
            }
            else if(viewSize.x > resizeTarget.x){
                if(viewSize.x + resizeSpeed.x < resizeTarget.x){
                   viewSize.x = resizeTarget.x;
                   resizeSpeed.x = 0;
                }
                else viewSize.x += resizeSpeed.x;
            }
            resizeFLAG = true;
        }

        if(resizeSpeed.y != 0){
            if(viewSize.y < resizeTarget.y){
                if(viewSize.y + resizeSpeed.y > resizeTarget.y){
                   viewSize.y = resizeTarget.y;
                   resizeSpeed.y = 0;
                }
                else viewSize.y += resizeSpeed.y;
            }
            else if(viewSize.y > resizeTarget.y){
                if(viewSize.y + resizeSpeed.y < resizeTarget.y){
                   viewSize.y = resizeTarget.y;
                   resizeSpeed.y = 0;
                }
                else viewSize.y += resizeSpeed.y;
            }
            resizeFLAG = true;
        }

        if(resizeFLAG) viewPort->setSize(viewSize);
    }

    if(event.LMBhold) ++event.LMBholdFrames;
    else{
        ++event.LMBreleaseFrames;
        event.releaseCapturedShapes();
    }

    if(event.RMBhold) ++event.RMBholdFrames;
    else ++event.RMBreleaseFrames;

    event.mouseCaptured = false;

    event.timeLastKey += event.lastFrameTime;

    if(bClosed || (viewPort == nullptr)){
        close();
        return false;
    }

    if(event.viewHasFocus){
        event.currentMousePosition = mousePos;
    }
    else{
        event.currentMousePosition = sf::Vector2f(NAN, NAN);
    }

    for(auto& element : boost::adaptors::reverse(popUpElements)){
        element->update(event, mousePos);
    }
    for(auto& panel : boost::adaptors::reverse(viewPanels)){
        panel->update(event, mousePos);
    }

    event.lastFrameMousePosition = mousePos;

    if(overlay != nullptr) overlay->update(event, mousePos);

    return true;
}

bool view::handleViewEvents(CVEvent& event){
    sf::Event SFevent;
    while(viewPort->pollEvent(SFevent)){
        switch(SFevent.type){
            case sf::Event::Closed:{
                viewPort->close();
                bClosed = true;
                return false;
                break;
            }
            case sf::Event::GainedFocus:{
                event.viewHasFocus = true;
                break;
            }
            case sf::Event::LostFocus:{
                event.viewHasFocus = false;
                break;
            }
            case sf::Event::Resized:{
//                event.lastViewBounds = event.viewBounds;
                viewPort->setView(sf::View(sf::FloatRect(0, 0, SFevent.size.width, SFevent.size.height)));
                event.viewBounds = sf::FloatRect(viewPort->getPosition().x, viewPort->getPosition().y,
                                                 viewPort->getSize().x, viewPort->getSize().y);
                event.lastViewBounds = event.viewBounds;
                event.viewResized = true;
                break;
            }
            case sf::Event::MouseButtonPressed:{
                switch(SFevent.mouseButton.button){
                    case sf::Mouse::Left:{
                        if(!event.LMBhold){
                            event.LMBhold = true;
                            event.LMBreleaseFrames = 0;
                            event.LMBpressPosition = mousePos;
                        }
                        break;
                    }
                    case sf::Mouse::Right:{
                        if(!event.RMBhold){
                            event.RMBhold = true;
                            event.RMBreleaseFrames = 0;
                            event.RMBpressPosition = mousePos;
                        }
                        break;
                    }
                    default:{
                        break;
                    }
                }
                break;
            }
            case sf::Event::MouseButtonReleased:{
                switch(SFevent.mouseButton.button){
                    case sf::Mouse::Left:{
                        event.LMBhold = false;
                        event.LMBholdFrames = 0;
                        break;
                    }
                    case sf::Mouse::Right:{
                        event.RMBhold = false;
                        event.RMBholdFrames = 0;
                        break;
                    }
                    default:{
                        break;
                    }
                }
                break;
            }
            default: break;
        }

        if(event.viewHasFocus){ // Only processed if view has focus
            switch(SFevent.type){
                case sf::Event::TextEntered:{
                    event.timeLastKey = 0.0f;
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)){
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) event.keyLog += 'a';
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)) event.keyLog += 'v';
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) event.keyLog += 'c';
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) event.keyLog += 'z';
                            else break;
                    }
                    else if(SFevent.text.unicode < 127){
                        event.keyLog += static_cast<char>(SFevent.text.unicode);
                    }
                    break;
                }
                case sf::Event::KeyPressed:{
                    event.timeLastKey = 0.0f;
                    switch(SFevent.key.code){
                        case sf::Keyboard::Delete:{
                            event.keyLog += static_cast<char>(127);
                            break;
                        }
                        case sf::Keyboard::Left:{
                            event.keyLog += static_cast<char>(CV_KEY_LEFT);
                            break;
                        }
                        case sf::Keyboard::Right:{
                            event.keyLog += static_cast<char>(CV_KEY_RIGHT);
                            break;
                        }
                        case sf::Keyboard::Up:{
                            event.keyLog += static_cast<char>(CV_KEY_UP);
                            break;
                        }
                        case sf::Keyboard::Down:{
                            event.keyLog += static_cast<char>(CV_KEY_DOWN);
                            break;
                        }
                        case sf::Keyboard::Return:{
                            event.keyLog += static_cast<char>(10);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                case sf::Event::MouseWheelScrolled:{
                    event.zoomDelta = SFevent.mouseWheelScroll.delta;
                    break;
                }
                default: break;
            }
        }
    }
    return true;
}

inline void view::sync(){
    while(waitingPopUpElements.size() > 0){
        popUpElements.push_back(waitingPopUpElements.front());
        popUpElements.back()->drawTarget = viewPort;
        waitingPopUpElements.erase(waitingPopUpElements.begin());
    }
}

void view::close(){

    if(!bClosed){
        bClosed = true;
    }

    forceClose = true; // Let app master handle destructor from main thread
}

view::~view(){
    for(auto& S : subViews){
        S->close();
        delete(S);
    }
    for(auto& popUp : popUpElements){
        delete(popUp);
    }
    for(auto& panel : viewPanels){
        delete(panel);
    }
}

visElement::visElement(view* View,
               bool canMove,
               bool canHighlight,
               bool canClick,
               bool canDrag,
               bool active):
                   visible(true),
                   canMove(canMove),
                   canHighlight(canHighlight),
                   canClick(canClick),
                   canDrag(canDrag),
                   active(active),
                   mouseHovering(false),
                   clickHeld(false),
                   highlighted(false),
                   origin(0.0f,0.0f),
                   state(STATE_NONE),
                   View(View),
                   drawTarget(View->viewPort){ }

APP* visElement::mainApp(){ return View->mainApp; }

void visElement::addSprite(const sf::Texture* texture, const sf::Vector2f& position, const sf::Vector2f& size){
    spriteList.emplace_back(*texture);
    sf::Vector2f texSize(texture->getSize());
    spriteList.back().setOrigin(texSize.x/2, texSize.y/2);
    spriteList.back().setScale(
        size.x/texSize.x,
        size.y/texSize.y);

    spriteList.back().setPosition(position.x,
                                  position.y);
}

void CVShape::setPosition(const sf::Vector2f& position){
    sf::Vector2f oldPosition = getPosition(),
                 offset(position.x - oldPosition.x, position.y - oldPosition.y);
    move(offset);
}

void CVShape::move(const sf::Vector2f& offset){
    for(auto& sprite : spriteList){
        sprite.move(offset);
    }

    bounds.left += offset.x;
    bounds.top += offset.y;

}

bool CVBox::draw(){
    if(drawTarget == nullptr) return false;
    if(!visible) return false;

    for(auto& item : panel){
        drawTarget->draw(item);
    }

    for(auto& sprite : spriteList){
        drawTarget->draw(sprite);
    }

    return true;
}

void CVBox::setPosition(const sf::Vector2f& position){
    sf::Vector2f oldPosition = getPosition(),
                 offset(position.x - oldPosition.x, position.y - oldPosition.y);
    move(offset + origin);
}

void CVBox::move(const sf::Vector2f& offset){
    CVShape::move(offset);
    for(auto& item : panel){
        item.move(offset);
    }
    for(auto& item : mask){
        item.move(offset);
    }
}

void CVBox::setSize(const sf::Vector2f& newSize){
    sf::Vector2f sizeScale = newSize/getSize();
    for(auto& item : panel){
        item.setSize(item.getSize()*sizeScale);
    }
    CVShape::setSize(newSize);
}

void CVBox::rotate(const float& angle, const sf::Vector2f& origin){
    sf::Vector2f objCenter,
                itemCenter;

    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + getOrigin();
    else objCenter = origin;

    float dist, oldAngle;
    sf::FloatRect itemBounds;

    for(auto& item : panel){
        oldAngle = get_angle(objCenter, itemCenter);
        item.rotate(angle);

        itemBounds = item.getGlobalBounds();
        itemCenter.x += itemBounds.width/2;
        itemCenter.y += itemBounds.height/2;

        dist = getDistance(objCenter, itemCenter);
        item.setPosition(radial_position(objCenter, dist, (oldAngle + angle)/PI));
    }
}

void CVBox::setRotation(const float& angle, const sf::Vector2f& origin){
    sf::Vector2f objCenter,
                itemCenter;

    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + getOrigin();
    else objCenter = origin;

    float dist;
    sf::FloatRect itemBounds;

    for(auto& item : panel){
        item.setRotation(angle);

        itemBounds = item.getGlobalBounds();
        itemCenter.x += itemBounds.width/2;
        itemCenter.y += itemBounds.height/2;

        dist = getDistance(objCenter, itemCenter);
        item.setPosition(radial_position(objCenter, dist, angle/PI));
    }

}

CVBox::CVBox(view* View,
            sf::Vector2f position, float width, float height,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
            CVShape(View){

    createdOrigin = position;

    colorTheme.emplace_back(fillColor);
    colorTheme.emplace_back(borderColor);

    bounds = sf::FloatRect(position.x, position.y, width, height);

    colorTheme = {fillColor, borderColor};

    // Main panel
    panel.emplace_back(sf::Vector2f(width, height));
    panel.back().setPosition(position);
    panel.back().setFillColor(fillColor);
    panel.back().setOutlineThickness(borderWidth);
    panel.back().setOutlineColor(borderColor);
}

bool CVBox::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(drawTarget == nullptr) return false;
    return true;
}

CVTextBox::CVTextBox(view* View,
            sf::Vector2f position, float width, float height,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
                CVBox(View, position, width, height,
              fillColor, borderColor, borderWidth){ }

CVTextBox::CVTextBox(view* View,
            sf::Vector2f position, float width, float height,
            textEntry textInfo,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
                CVBox(View, position, width, height,
              fillColor, borderColor, borderWidth){

    addTextEntry(textInfo, position);
    sf::Vector2f textPos = position;
    sf::FloatRect textBounds = displayText.back().getLocalBounds();
    float xPadding = (bounds.height - textBounds.height)/2;

    switch(textInfo.alignment){
        case ALIGN_CENTER:{
            panel.front().setOrigin(bounds.width/2, 0.0f);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
            textPos.y = bounds.top + bounds.height - textBounds.height;
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            panel.front().setOrigin(bounds.width/2, bounds.height/2);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
            textPos.y = bounds.top + bounds.height/2 - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2;
            break;
        }
        case ALIGN_CENTER_TOP:{
            panel.front().setOrigin(bounds.width/2, bounds.height);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
            break;
        }
        case ALIGN_RIGHT:{
            panel.front().setOrigin(bounds.width, 0.0f);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width - xPadding - textBounds.width;
            textPos.y = bounds.top + bounds.height - textBounds.height;
            break;
        }
        case ALIGN_RIGHT_MIDLINE:{
            panel.front().setOrigin(bounds.width/2, bounds.height/2);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width - xPadding - textBounds.width;
            textPos.y = bounds.top + bounds.height/2 - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2;
            break;
        }
        case ALIGN_RIGHT_TOP:{
            panel.front().setOrigin(bounds.width/2, bounds.height);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + bounds.width - xPadding - textBounds.width;
            break;
        }
        case ALIGN_LEFT_MIDLINE:{
            panel.front().setOrigin(0.0f, bounds.height/2);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + xPadding;
            textPos.y = bounds.top + bounds.height/2 - textBounds.height/2 - getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_LEFT_TOP:{
            panel.front().setOrigin(0.0f, bounds.height);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();

            textPos.x = bounds.left + xPadding;
            break;
        }
        default:{ // Left
            textPos.x = bounds.left + xPadding;
            textPos.y = bounds.top + bounds.height - textBounds.height;
            break;
        }
    }

    displayText.back().setPosition(textPos);
}

void CVTextBox::addTextEntry(textEntry newText, float padding, bool regular){

    sf::Vector2f newTextPosition;
    sf::Rect<float> lastTextBounds;
    if(displayText.size() > 0){
        if(regular) lastTextBounds = averageTextSize(displayText);
        else{
            lastTextBounds = displayText.back().getLocalBounds();
        }
        newTextPosition = displayText.back().getPosition();
    }
    else newTextPosition = getPosition();

    displayText.emplace_back(
        newText.text,
        *mainApp()->fonts[newText.font],
        newText.fontSize
    );

    displayText.back().setFillColor(newText.textColor);
    colorTheme.emplace_back(newText.textColor);
    displayText.back().setPosition(newTextPosition);

    sf::Rect<float> newTextBounds = displayText.back().getLocalBounds();
    newTextPosition = getTextHorizontalAlignment(displayText.back(), bounds, newText.alignment);

    switch(textAlignment){
        case TEXT_ALIGN_VERTICAL:{
            displayText.back().setPosition(newTextPosition.x,
                                           newTextPosition.y + lastTextBounds.height + padding);
            break;
        }
        case TEXT_ALIGN_HORIZONTAL:{
            displayText.back().setPosition(newTextPosition.x + lastTextBounds.width + padding,
                                           newTextPosition.y);
            break;
        }
        default: break;
    }
}

void CVTextBox::addTextEntry(const textEntry& newText, const sf::Vector2f& position){

    displayText.emplace_back(newText.text,
                             *mainApp()->fonts[newText.font],
                             newText.fontSize);
    displayText.back().setFillColor(newText.textColor);
    displayText.back().setStyle(newText.textStyle);

    colorTheme.emplace_back(newText.textColor);

    switch(newText.alignment){
        case ALIGN_LEFT_MIDLINE:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x, position.y - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2);
            break;
        }
        case ALIGN_LEFT_TOP:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x, position.y - textBounds.height);
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x - textBounds.width/2, position.y - textBounds.height/2);
            break;
        }
        case ALIGN_CENTER_TOP:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x - textBounds.width/2, position.y - textBounds.height);
            break;
        }
        case ALIGN_CENTER:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x - textBounds.width/2,
                                           position.y);
            break;
        }
        case ALIGN_RIGHT:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x - textBounds.width, position.y);
            break;
        }
        case ALIGN_VERTICAL:{
            displayText.back().setRotation(-90);
            sf::FloatRect textBounds = displayText.back().getGlobalBounds();
            displayText.back().setPosition(position.x + textBounds.width, position.y + textBounds.height);
            break;
        }
        default:{
            displayText.back().setPosition(position.x, position.y);
            break;
        }
    }

}

void CVTextBox::setPosition(const sf::Vector2f& position){
    CVTextBox::move(position - getPosition());
}

void CVTextBox::move(const sf::Vector2f& offset){
    CVBox::move(offset);
    for(auto& text : displayText){
        text.move(offset);
    }
}

bool CVTextBox::draw(){
    if(!CVBox::draw()) return false;

    for(auto& text : displayText){
        drawTarget->draw(text);
    }

    return true;
}

void CVTextBox::setText(const unsigned int textIndex, std::string newText){
    if(displayText.size() > textIndex) displayText[textIndex].setString(newText);
}

CVViewPanel::CVViewPanel(view* parentView, std::string panelTag, sf::Color backgroundColor,
                         const sf::Vector2f& size, bool fitToWindow):
    CVTextBox(parentView, sf::Vector2f(0,0), parentView->getWidth(), parentView->getHeight(), backgroundColor, backgroundColor, 0.0f),
    bFitWindow(fitToWindow){
        if(!isnan(size.x) && !isnan(size.y)) setSize(size);
}

void CVViewPanel::addPanelElement(visElement* newElement, std::string newTag){
    if(newTag.size() < 1) newTag = "Panel" + viewPanelElements.size();
    for(auto& panelElement : viewPanelElements){
        if(panelElement == newElement) return;
    }
    unsigned int index = 0;
    std::string dupTag;

    for(auto tag : viewPanelTags){
        if(tag == newTag){
            if(dupTag.size() > 0){
                index = 0;
            }
            dupTag = tag;
            ++index;
        }
    }
    if(index > 0){
        newTag = dupTag + std::to_string(index);
    }


    viewPanelElements.push_back(newElement);
    viewPanelElements.back()->viewPanel = this;
    viewPanelTags.push_back(newTag);
}

bool CVViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVTextBox::update(event, mousePos)) return false;
    if(bFitWindow && event.viewResized){
       setPosition(0.0f,0.0f);
       setSize(sf::Vector2f(event.viewBounds.width, event.viewBounds.height));
    }
    return true;
}

void CVViewPanel::removePanelElement(visElement* element){
    for(size_t i = 0; i < viewPanelElements.size(); ++i){
        if(viewPanelElements[i] == element){
            delete(viewPanelElements[i]);
            viewPanelElements.erase(viewPanelElements.begin() + i);
            viewPanelTags.erase(viewPanelTags.begin() + i);
            return;
        }
    }
}

void CVViewPanel::removePanelElement(const std::string& tag){
    for(size_t i = 0; i < viewPanelElements.size(); ++i){
        if(viewPanelTags[i] == tag){
            delete(viewPanelElements[i]);
            viewPanelElements.erase(viewPanelElements.begin() + i);
            viewPanelTags.erase(viewPanelTags.begin() + i);
            return;
        }
    }
}

visElement* CVViewPanel::getTaggedViewPanel(const std::string& tag){
    unsigned int index = 0;
    for(auto T : viewPanelTags){
        if(T == tag) return viewPanelElements[index];
        else ++index;
    }
    return nullptr;
}

bool CVBasicViewPanel::draw(){
    if(!CVTextBox::draw()) return false;

    for(auto& panel : viewPanelElements){
            panel->draw();
    }

    return true;
}

bool CVBasicViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos){ // Disperse update function
    if(!CVViewPanel::update(event, mousePos)) return false;
    for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
        panel->update(event, mousePos);
    }
    return true;
}

}
