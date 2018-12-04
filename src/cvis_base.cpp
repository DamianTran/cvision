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

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_algorithm.hpp"

#if defined _WIN32 || defined _WIN32 || defined WIN32
#include <windows.h>
#endif

using namespace AIALG;

namespace cvis{

const sf::Texture* ImageManager::taggedTexture(const std::string& s) const{
    if(s.size() < 1) return nullptr;
    unsigned int index = 0;
    for(auto& tag : textureTags){
        if(tag == s) return &textures[index];
        ++index;
    }
    return nullptr;
}
const sf::Image* ImageManager::taggedImage(const std::string& s) const{
    if(s.size() < 1) return nullptr;
    unsigned int index = 0;
    for(auto& tag : imageTags){
        if(tag == s) return &images[index];
        ++index;
    }
    return nullptr;
}

std::string ImageManager::getTextureName(const sf::Texture* texture) const{
    for(size_t i = 0; i < numTextures; ++i){
        if(&textures[i] == texture) return textureTags[i];
    }
    return std::string();
}

bool ImageManager::addImage(std::string fileName, std::string tag){
    images.emplace_back();
    if(!images.back().loadFromFile(fileName)){
        images.pop_back();
        return false;
    }
    imageTags.push_back(tag);
    ++numImages;
    return true;
}

bool ImageManager::addTexture(std::string fileName, std::string tag){
    textures.emplace_back();
    if(!textures.back().loadFromFile(fileName)){
        textures.pop_back();
        return false;
    }
    textures.back().setSmooth(true);
    textures.back().generateMipmap();
    textureTags.push_back(tag);
    ++numTextures;
    return true;
}

bool ImageManager::addTexture(const void* binaries, const size_t& size,
                       const std::string& tag){
    textures.emplace_back();
    if(!textures.back().loadFromMemory(binaries, size)){
        textures.pop_back();
        return false;
    }
    textures.back().setSmooth(true);
    textures.back().generateMipmap();
    textureTags.push_back(tag);
    ++numTextures;
    return true;
}

CVData::CVData(const CVData& other){
    infoType = other.infoType;
    releaseCond = other.releaseCond;
    releaseKey = other.releaseKey;
    length = other.length;

    data = new char[length+1];
    data[length] = '\0';
    memcpy(data, other.data, length);
}

CVData& CVData::operator=(const CVData& other){

    delete[] data;

    infoType = other.infoType;
    releaseCond = other.releaseCond;
    releaseKey = other.releaseKey;
    length = other.length;

    data = new char[length+1];
    data[length] = '\0';
    memcpy(data, other.data, length);

    return *this;

}

CVData::CVData(const void* data, const size_t& length, const uint8_t& infoType,
           const unsigned char& releaseCond, const sf::Keyboard::Key& releaseKey):
               infoType(infoType),
               releaseCond(releaseCond),
               releaseKey(releaseKey),
               length(length){
    this->data = new char[length+1];
    this->data[length] = '\0';
    memcpy(this->data, data, length);
}

CVData::~CVData(){
    delete[] data;
}

bool operator==(const CVCaptureRef& capref, const CVCaptureRef& other){
    return capref.item == other.item;
}

bool CVEvent::captureMouse(){
    if(viewHasFocus){
        if(!mouseCaptured){
            mouseCaptured = true;
            return true;
        }
        else return false;
    }
    return false;
}

bool CVEvent::captureFocus(){
    if(viewHasFocus){
        if(!focusCaptured){
            focusCaptured = true;
            return true;
        }
        else return false;
    }
    return false;
}

void CVEvent::releaseFocus(){
    focusCaptured = false;
}

bool CVEvent::checkCond(const unsigned char& cond, const sf::Keyboard::Key& keyCond){
    bool status = false;

    if(cond & CV_EVENT_LMB){
        status |= LMBhold;
    }
    if(cond & CV_EVENT_RMB){
        status |= RMBhold;
    }
    if(cond & CV_EVENT_KEY){
        status |= sf::Keyboard::isKeyPressed(keyCond);
    }

    if(!(cond & CV_EVENT_ON)) status = !status;

    return status;
}

void CVEvent::moveCapturedShapes(){
    for(auto& capref : mouseCapturedShapes){
        if(capref.type == CV_CLASS_CVIS){
            ((CVElement*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
        else if(capref.type == CV_CLASS_SPRITE){
            ((sf::Sprite*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
        else if(capref.type == CV_CLASS_SHAPE){
            ((sf::Shape*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
    }
}

bool CVEvent::leftClick(const float& duration) const{
    return LMBholdTime >= duration;
}

bool CVEvent::rightClick(const float& duration) const{
    return RMBholdTime >= duration;
}

bool CVEvent::leftClickReleased(const float& duration) const{
    return LMBholdTime >= duration;
}

bool CVEvent::rightClickReleased(const float& duration) const{
    return RMBholdTime >= duration;
}

bool CVEvent::focusFree(){
    return !focusCaptured;
}

void CVEvent::clearKeys(){
    keyLog.clear();
}

bool CVEvent::keyIsPressed(const char& key){
    for(auto& k : keyLog){
        if(k == key) return true;
    }
    return false;
}

float CVEvent::mouseVelocity() const{
    return getVelocity(mouseTraceBuffer[mouseTraceBuffer.size() - 2], currentMousePosition, avgFrameTime*2);
}

float CVEvent::mouseMoveAngle() const{
    return get_angle(mouseTraceBuffer[mouseTraceBuffer.size() - 2], currentMousePosition);
}

void CVEvent::releaseData(const unsigned char& cond){
    for(size_t i = 0; i < transferData.size();){
        if(transferData[i].releaseCond & cond){
            transferData.erase(transferData.begin() + i);
        }
        else ++i;
    }
}

void CVEvent::tossData(){
    for(size_t i = 0; i < transferData.size();){
        if(checkCond(transferData[i].releaseCond)){
            transferData.erase(transferData.begin() + i);
        }
        else ++i;
    }
}

void CVEvent::reset(){
    LMBhold = false;
    RMBhold = false;
    LMBreleased = false;
    RMBreleased = false;
    viewClosed = false;
    viewHasFocus = true;
    mouseCaptured = false;
    focusCaptured = false;
    viewResized = false;
    eventsProcessed = false;
    keyPressed = false;
    LMBholdFrames = 0;
    RMBholdFrames = 0;
    LMBreleaseFrames = 0;
    RMBreleaseFrames = 0;
    timeLastKey = 0.0f;
    lastFrameTime = 0.0f;
    zoomDelta = 0.0f;
    avgFrameTime = 0.0f;
    zoomState = CV_ZOOM_NONE;
    wheelDirection = CV_WHEEL_DIR_NONE;
    viewResizeScale = sf::Vector2f(1.0f,1.0f);
    LMBpressPosition = sf::Vector2f(0.0f,0.0f);
    RMBpressPosition = sf::Vector2f(0.0f,0.0f);
    LMBreleasePosition = sf::Vector2f(0.0f,0.0f);
    RMBreleasePosition = sf::Vector2f(0.0f,0.0f);
    lastFrameMousePosition = sf::Vector2f(0.0f,0.0f);
    currentMousePosition = sf::Vector2f(0.0f,0.0f);
    viewBounds = sf::FloatRect(0.0f,0.0f,0.0f,0.0f);
    lastViewBounds = viewBounds;
    keyLog.clear();
    mouseCapturedShapes.clear();
}

    #ifdef __APPLE__
void CVEvent::transfer(const CVEvent& other){
    LMBhold = other.LMBhold;
    RMBhold = other.RMBhold;
    LMBreleased = other.LMBreleased;
    RMBreleased = other.RMBreleased;
    viewClosed = other.viewClosed;
    viewHasFocus = other.viewHasFocus;
    viewResized = other.viewResized;
    LMBholdFrames = other.LMBholdFrames;
    RMBholdFrames = other.RMBholdFrames;
    LMBreleaseFrames = other.LMBreleaseFrames;
    RMBreleaseFrames = other.RMBreleaseFrames;
    LMBpressPosition = other.LMBpressPosition;
    RMBpressPosition = other.RMBpressPosition;
    LMBreleasePosition = other.LMBreleasePosition;
    RMBreleasePosition = other.RMBreleasePosition;
    LMBholdTime = other.LMBholdTime;
    RMBholdTime = other.RMBholdTime;
    LMBreleaseTime = other.LMBreleaseTime;
    RMBreleaseTime = other.RMBreleaseTime;
    zoomDelta = other.zoomDelta;
    zoomState = other.zoomState;
    viewResizeScale = other.viewResizeScale;
    viewBounds = other.viewBounds;
    lastViewBounds = other.lastViewBounds;
    keyLog = other.keyLog;
    transferData = other.transferData;
    mouseCapturedShapes = other.mouseCapturedShapes;
    mouseTraceBuffer = other.mouseTraceBuffer;
    wheelDirection = other.wheelDirection;
}
    #endif

CVEvent::CVEvent():
    LMBhold(false),
    RMBhold(false),
    LMBreleased(false),
    RMBreleased(false),
    viewClosed(false),
    viewHasFocus(true),
    mouseCaptured(false),
    focusCaptured(false),
    viewResized(false),
    eventsProcessed(false),
    keyPressed(false),
    closeSignal(false),
    LMBholdFrames(0),
    RMBholdFrames(0),
    LMBreleaseFrames(0),
    RMBreleaseFrames(0),
    maxMouseTrace(14),
    numFrameAvg(60),
    timeLastKey(0.0f),
    lastFrameTime(0.0f),
    zoomDelta(0.0f),
    avgFrameTime(0.0f),
    LMBholdTime(0.0f),
    RMBholdTime(0.0f),
    LMBreleaseTime(0.0f),
    RMBreleaseTime(0.0f),
    zoomState(CV_ZOOM_NONE),
    wheelDirection(CV_WHEEL_DIR_NONE),
    viewResizeScale(1.0f,1.0f),
    LMBpressPosition(0.0f,0.0f),
    RMBpressPosition(0.0f,0.0f),
    lastFrameMousePosition(0.0f,0.0f),
    currentMousePosition(0.0f,0.0f),
    LMBreleasePosition(0.0f,0.0f),
    RMBreleasePosition(0.0f,0.0f),
    mouseWheelDelta(0.0f,0.0f),
    lastFrameGlobalMousePosition(0.0f,0.0f),
    mouseTraceBuffer(maxMouseTrace, sf::Vector2f(0.0f, 0.0f)),
    viewBounds(0.0f,0.0f,0.0f,0.0f),
    lastViewBounds(viewBounds)
    { }

std::ostream& operator<<(std::ostream& output, const CVEvent& event){
    output << "LMB pressed: " << event.LMBhold <<
            "\nLMB hold frames: " << event.LMBholdFrames <<
            "\nLMB released: " << event.LMBreleased <<
            "\nLMB release frames: " << event.LMBreleaseFrames <<
            "\nRMB pressed: " << event.RMBhold <<
            "\nRMB hold frames: " << event.RMBholdFrames <<
            "\nRMB released: " << event.RMBreleased <<
            "\nRMB release frames: " << event.RMBreleaseFrames <<
            "\nView closed: " << event.viewClosed <<
            "\nView has focus: " << event.viewHasFocus <<
            "\nMouse captured: " << event.mouseCaptured <<
            "\nView resized: " << event.viewResized <<
            "\nEvents processed: " << event.eventsProcessed <<
            "\nTime last key: " << event.timeLastKey <<
            "\nLast frame time: " << event.lastFrameTime <<
            "\nZoom delta: " << event.mouseWheelDelta.y <<
            "\nZoom state: " << (int)event.zoomState <<
            "\nKey log: " << event.keyLog <<
            "\nView resize scale: " << event.viewResizeScale.x << ' ' << event.viewResizeScale.y <<
            "\nLMB press position: " << event.LMBpressPosition.x << ' ' << event.LMBpressPosition.y <<
            "\nRMB press position: " << event.RMBpressPosition.x << ' ' << event.RMBpressPosition.y <<
            "\nLast frame mouse pos: " << event.lastFrameMousePosition.x << ' ' << event.lastFrameMousePosition.y <<
            "\nCurrent frame mouse pos: " << event.currentMousePosition.x << ' ' << event.currentMousePosition.y;
    return output;
}

bool FontManager::addFont(std::string fileDir, std::string tag){
    fonts.push_back(sf::Font());
    if(!fonts.back().loadFromFile(fileDir)){
        fonts.pop_back();
        return false;
    }
    fontTags.push_back(tag);
    return true;
}

bool FontManager::addFont(const void* binaries, const size_t& size, const std::string& tag){
    fonts.emplace_back();
    if(!fonts.back().loadFromMemory(binaries, size)){
        fonts.pop_back();
        return false;
    }
    fontTags.push_back(tag);
    return true;
}

void CVApp::setViewGUI(std::string viewTag, std::string GUItag){
    setViewGUI(*taggedItem(viewTag, viewList, viewTags),
               *taggedItem(GUItag, GUIlist, GUITags));
}

#ifdef __APPLE__
void CVApp::handleViewEvents(){
    this->eventLock.lock();
    for(auto& view : viewList){
        if(!view->eventTrace.eventsProcessed){
            view->updateLock.lock();
            view->handleViewEvents(view->eventTrace);
            view->updateLock.unlock();
        }
    }
    this->eventLock.unlock();
}
#endif

void CVApp::mainUpdate(){
    for(size_t i = 0; i < viewList.size(); ++i){
        if(viewList[i]->closed()) closeView(i);
    }
}

void CVApp::closeView(const unsigned int viewIndex){
    viewList[viewIndex]->close();
    viewList[viewIndex]->appThread->join();

    for(size_t i = 0; i < viewThreads.size(); ++i){
        if(viewThreads[i] == viewList[viewIndex]->appThread){
            delete(viewThreads[i]);
            viewThreads.erase(viewThreads.begin() + i);
        }
    }

    delete(viewList[viewIndex]);
    viewList.erase(viewList.begin() + viewIndex);
    viewTags.erase(viewTags.begin() + viewIndex);
}

void CVApp::closeView(const std::string& tag){
    unsigned int index = 0;
    for(auto& word : viewTags){
        if(word == tag){
            closeView(index);
            return;
        }
        ++index;
    }
}

void CVApp::closeAll(){
    while(!viewList.empty()){
        viewList.back()->close();
        viewList.pop_back();
    }
}

CVApp::~CVApp(){
    closeAll();
    running = false;
    if(mainUpdateThread != nullptr) mainUpdateThread->join();
}

bool CVApp::addView(CVView* View, const std::string& viewTag){
    if(!AIALG::anyEqual(View, viewList)){
        viewList.push_back(View);
        if(AIALG::anyEqual(viewTag, viewTags)){
            unsigned int nameIndex = viewTags.size();
            do{
                viewTags.push_back(viewTag + "_" + std::to_string(nameIndex));
                ++nameIndex;
            }while(AIALG::anyEqual(viewTag, viewTags));
        }
        else viewTags.push_back(viewTag);
        return true;
    }
    else return false;
}

void GUIpanel::draw(sf::RenderTarget* target){
    const std::vector<CVElement*>::const_iterator iterEND = panelElements.end();
    for(std::vector<CVElement*>::iterator iter = panelElements.begin(); iter != iterEND; ++iter){
            if((*iter)->isVisible()) (*iter)->draw(target);
    }

    const std::vector<sf::Text>::const_iterator textEND = overlayText.end();
    for(std::vector<sf::Text>::const_iterator textIT = overlayText.begin();
        textIT != textEND; ++textIT){
            target->draw(*textIT);
    }
}

void GUIpanel::clear(){
    overlayText.clear();
    const c_itr<CVElement*> itEND = panelElements.end();
    for(itr<CVElement*> it = panelElements.begin(); it != itEND; ++it){
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
    for(std::vector<CVElement*>::iterator it = panelElements.begin();
        it != panelElements.end(); ++it){
            delete(*it);
        }
}

GUI::GUI(sf::RenderWindow* window, CVView* View):
        visible(true),
        mainApp(View->mainApp),
        View(View),
        viewPort(window){ }

void GUI::draw(sf::RenderTarget* target){
    for(std::vector<GUIpanel>::iterator it = panelList.begin();
        it != panelList.end(); ++it){
            if(it->isVisible()) it->draw(target);
    }
}

void GUI::removePanel(std::string panelName){
    unsigned int matchIndex = taggedIndex(panelName, panelTags);
    if(matchIndex != UINT_MAX){
        panelList.erase(panelList.begin() + matchIndex);
        panelTags.erase(panelTags.begin() + matchIndex);
    }
}

void GUI::addPanelElement(std::string panelName, CVElement* newElement){
    taggedItem(panelName, panelList, panelTags)->addElement(newElement);
}

GUIpanel* GUI::taggedPanel(const std::string& tag){
    return taggedItem(tag, panelList, panelTags);
}

CVView::CVView(unsigned int x, unsigned int y, std::string winName,
           uint32_t style, CVApp* mainApp,
           const sf::Vector2f& screenPosition,
           const sf::Color& backgroundColor):
    forceClose(false),
    bClosed(false),
    bElasticSelect(false),
    defaultViewScale(1920.0f*1080.0f),
    backgroundColor(backgroundColor),
    numPendingAnims(0),
    appThread(nullptr),
    frameRateLimit(mainApp->frameRate),
    frameRate(0.0f),
    width(x), height(y),
    titleBarHeight(0.0f),
    viewHeight(height),
    resizeTarget(width, height),
    resizeSpeed(0,0),
    velocity(0,0),
    viewState(VIEW_STATE_STARTUP),
    viewPort(nullptr),
    overlay(nullptr),
    mainApp(mainApp),
    tag(winName)
        {

        #ifdef __APPLE__
        // Construct the viewPort outside of the worker thread for OSX compatibility
        viewPort = new sf::RenderWindow();
        viewPort->setActive(false);
        viewPort->setVerticalSyncEnabled(true);
        #endif

        textureBuffer.setActive();

        mainApp->viewThreads.emplace_back(new std::thread([&, x, y, winName, style, screenPosition](){

            const float frameTime = 0.5f/frameRateLimit;

            #ifndef __APPLE__
            viewPort = new sf::RenderWindow();
            viewPort->setVerticalSyncEnabled(true);
            #endif

            moveTarget = viewPort->getPosition();
            sf::Vector2i intPos;

            std::chrono::high_resolution_clock::time_point t0 = TIME_NOW;
            const std::chrono::duration<float> frameUpdateLatency(frameTime);

            std::chrono::duration<float> duration;

            eventTrace.viewBounds = sf::FloatRect(moveTarget.x, moveTarget.y, x, y);
            eventTrace.lastViewBounds = eventTrace.viewBounds;
            eventTrace.lastFrameMousePosition =
                sf::Vector2f(sf::Mouse::getPosition(*viewPort));

            while(!bClosed){

                if(!viewPort->isOpen()){
                    if(getState() == VIEW_STATE_MAIN){
                        sf::ContextSettings newWinSettings;
                        newWinSettings.antialiasingLevel = 4;
                        viewPort->create(sf::VideoMode(x, y), winName, style, newWinSettings);
                        if(!isnan(screenPosition.x) && !isnan(screenPosition.y)) viewPort->setPosition(sf::Vector2i(screenPosition));
                        viewPort->setActive(true);
                    }
                    else{
                        goto nextFrame;
                    }
                }

                if(forceClose){
                    #ifndef __APPLE__
                    viewPort->close();
                    #endif
                    break;
                }

                intPos = sf::Mouse::getPosition(*viewPort);
                mousePos.x = intPos.x;
                mousePos.y = intPos.y;

                #ifdef __APPLE__
                while(!bClosed && viewPort->isOpen() && !eventTrace.eventsProcessed){
                    std::this_thread::sleep_for(std::chrono::duration<float>(frameTime/10));
                } // Wait for main thread to process events
                #endif // __APPLE__

                updateLock.lock();
                update(eventTrace, mousePos);
                updateLock.unlock();

                drawLock.lock();
                viewPort->clear(backgroundColor);
                draw(viewPort);
                viewPort->display();
                drawLock.unlock();

                // Framerate limit cycle

                nextFrame:;

                duration = TIME_NOW - t0;

                if(duration < frameUpdateLatency){
                    std::this_thread::sleep_for(std::chrono::duration<float>(frameTime - duration.count()));
                }

                eventTrace.lastFrameTime = std::chrono::duration<float>(TIME_NOW - t0).count();
                t0 = TIME_NOW;
                frameRate = 1.0f/eventTrace.avgFrameTime;

            }

            #ifdef __APPLE__
            // Wait for main thread to handle viewport close
            while(viewPort->isOpen()){
                std::this_thread::sleep_for(std::chrono::duration<float>(frameTime));
            }
            #else
            if(viewPort->isOpen()){
                viewPort->close();
            }
            #endif
            delete(viewPort);
            viewPort = nullptr;
            viewState = VIEW_STATE_CLOSED;

        }));
        appThread = mainApp->viewThreads.back(); // Establish reference to app thread for this view
        #ifndef __APPLE__
        // Wait for the view to be constructed
        while(viewPort == nullptr){
            std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
        }
        #endif
}

void CVView::setTopMargin(const float& newMargin){
    titleBarHeight = newMargin;
    viewHeight = height - titleBarHeight;
}

float CVView::getHeight() const{
    return viewHeight;
}

float CVView::renderWidth() const{ return width; }
float CVView::renderHeight() const{ return height; }

sf::Vector2f CVView::getSize(){
    return sf::Vector2f(width, viewHeight);
}

sf::FloatRect CVView::getBounds() const{
    return sf::FloatRect(0.0f,titleBarHeight,width,viewHeight);
}

void CVView::getTexture(sf::Texture& texture){
    captureLock.lock();

    textureBuffer.create(width, height);
    textureBuffer.setView(sf::View(getBounds()));
    textureBuffer.clear();

    draw(&textureBuffer);

    textureBuffer.display();
    texture = textureBuffer.getTexture();

    captureLock.unlock();
}

void CVView::setVisible(const unsigned int index, bool newVisibleState){
     viewPanels[index]->setVisible(newVisibleState);
}

void CVView::isolateVisible(const unsigned int index){ // Focus visibility on one panel in view
    size_t L = viewPanels.size();
    for(size_t i = 0; i < L; ++i){
        if(i == index) viewPanels[i]->setVisible(true);
        else viewPanels[i]->setVisible(false);
    }
}

bool CVView::contains(const CVElement& element){
    sf::FloatRect viewBounds(0.0f,0.0f,width,height);
    return viewBounds.intersects(element.getBounds());
}

CVViewPanel* CVView::getTaggedPanel(const std::string s){
    return *taggedItem(s, viewPanels, panelTags);
}
CVViewPanel* CVView::operator[](const std::string s){
    return *taggedItem(s, viewPanels, panelTags);
}

CVView* CVView::taggedSubView(std::string tag){
    return *taggedItem(tag, subViews, subViewTags);
}

const float CVView::getViewScale() const{
    return pow(sf::VideoMode::getDesktopMode().height*
            sf::VideoMode::getDesktopMode().width/
                defaultViewScale, 1.0f/3);
}
void CVView::setDefaultViewScale(const float& x, const float& y){
    defaultViewScale = x*y;
}

void CVView::addSubView(CVView* newSubView, std::string tag){
    if(addUnique(newSubView, subViews)){
        if(tag.size() > 0) subViewTags.push_back(tag);
        else subViewTags.push_back("Subview " + std::to_string(subViews.size()));
    }
}

void CVView::addPanel(CVViewPanel* newPanel, std::string tag, const unsigned int& index){
    if(addUnique(newPanel, viewPanels, index)){
        if(tag.size() > 0) panelTags.push_back(tag);
        else panelTags.push_back("Panel " + std::to_string(viewPanels.size()));
    }
}

void CVView::minimize(){
    #if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_MINIMIZE);
    #elif defined __APPLE

    #endif
}

void CVView::maximize(){
    #if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_MAXIMIZE);
    #elif defined __APPLE

    #endif
}

void CVView::restore(){
    #if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_RESTORE);
    #elif defined __APPLE

    #endif
}

void CVView::setVisiblePanel(const unsigned int i){
    unsigned int L = 0;
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(L != i) (*it)->setVisible(false);
            else (*it)->setVisible(true);
        }
}

void CVView::setVisiblePanel(const std::string tag){
    std::vector<std::string>::iterator strIT = panelTags.begin();
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(*strIT != tag) (*it)->setVisible(false);
            else (*it)->setVisible(true);
            ++strIT;
        }
}

void CVView::setVisiblePanel(CVViewPanel* panel){
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
        it != viewPanels.end(); ++it){
            if(*it != panel) (*it)->setVisible(false);
            else (*it)->setVisible(true);
        }
}

void CVView::setPosition(const sf::Vector2i& position){
    if(parentView != nullptr){ // Relative to parent if parent exists
        sf::Vector2i parentPosition(parentView->viewPort->getPosition());
        parentPosition.x += position.x;
        parentPosition.y += position.y;
        viewPort->setPosition(parentPosition);
    }
    else viewPort->setPosition(position);
}

void CVView::setScreenPosition(sf::Vector2i newPosition){
    viewPort->setPosition(newPosition);
}

void CVView::draw(sf::RenderTarget* target){
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return;
    switch(viewState){
        case VIEW_STATE_STARTUP:{
            break;
        }
        case VIEW_STATE_MAIN:{
            for(auto& item : viewPanels){
                if(item->isVisible()) item->draw(target);
            }
            if(overlay != nullptr){
                if(overlay->isVisible()) overlay->draw(target);
            }
            for(auto& item : popUpElements){
                if(item->isVisible()) item->draw(target);
            }
            break;
        }
        default: break;
    }
}

bool CVView::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return false;

    // Handle screenshots in the main update thread
    if(saveRequestFiles.size() > 0){
        sf::Texture screenshot;
        screenshot.create(getWidth(), getHeight());

        while(saveRequestFiles.size() > 0){
            screenshot.update(*viewPort);
            screenshot.copyToImage().saveToFile(saveRequestFiles.front());
            saveRequestFiles.erase(saveRequestFiles.begin());
        }
    }

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

    #ifndef __APPLE__
    if(!handleViewEvents(event)) return false;
    #else
    while(!event.eventsProcessed){
        if(bClosed || (viewPort == nullptr)){
            return false;
        }
        std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
    }
    #endif

    if(viewPort->getPosition() != moveTarget){
        sf::Vector2i viewPos(viewPort->getPosition());
        if(velocity.x != 0){
            if(viewPos.x < moveTarget.x){
                if(viewPos.x + velocity.x > moveTarget.x){
                   viewPos.x = moveTarget.x;
                   velocity.x = 0;
                }
                else viewPos.x += velocity.x;
            }
            if(viewPos.x > moveTarget.x){
                if(viewPos.x + velocity.x < moveTarget.x){
                   viewPos.x = moveTarget.x;
                   velocity.x = 0;
                }
                else viewPos.x += velocity.x;
            }
        }

        if(velocity.y != 0){
            if(viewPos.y < moveTarget.y){
                if(viewPos.y + velocity.y > moveTarget.y){
                   viewPos.y = moveTarget.y;
                   velocity.y = 0;
                }
                else viewPos.y += velocity.y;
            }
            else if(viewPos.y > moveTarget.y){
                if(viewPos.y + velocity.y < moveTarget.y){
                   viewPos.y = moveTarget.y;
                   velocity.y = 0;
                }
                else viewPos.y += velocity.y;
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

    #ifndef __APPLE__
    if(bClosed || (viewPort == nullptr)){
        close();
        return false;
    }
    #endif

    if(event.viewHasFocus){
        event.currentMousePosition = mousePos;
    }
    else{
        event.currentMousePosition = sf::Vector2f(NAN, NAN);
    }

    if(event.LMBreleased){
        event.releaseCapturedShapes();
    }

    event.mouseTraceBuffer.emplace_back(mousePos);
    event.mouseTraceBuffer.erase(event.mouseTraceBuffer.begin());

    // Open the mouse pointer to capture
    event.mouseCaptured = false;

    // Open the view focus to capture
    event.focusCaptured = false;

    for(auto& element : boost::adaptors::reverse(popUpElements)){
        element->update(event, mousePos);
    }
    for(auto& panel : boost::adaptors::reverse(viewPanels)){
        panel->update(event, mousePos);
    }

    if(event.closed()){ // Check for a close signal
        close();
        return false;
    }

    event.moveCapturedShapes();

    event.lastFrameMousePosition = mousePos;
    event.lastFrameGlobalMousePosition = sf::Vector2f(sf::Mouse::getPosition());
    event.timeLastKey += event.lastFrameTime;
    event.avgFrameTime *= event.numFrameAvg;
    event.avgFrameTime += event.lastFrameTime;
    event.avgFrameTime /= (event.numFrameAvg + 1);

    if(event.LMBhold){
        event.LMBholdTime += event.lastFrameTime;
    }
    else{
        event.LMBreleaseTime += event.lastFrameTime;
    }

    if(event.RMBhold){
        event.RMBholdTime += event.lastFrameTime;
    }
    else{
        event.RMBreleaseTime += event.lastFrameTime;
    }

    event.tossData(); // Toss data if conditions are met and not picked up

    if(overlay != nullptr) overlay->update(event, mousePos);

    event.eventsProcessed = false;

    return true;
}

bool CVView::handleViewEvents(CVEvent& event){
    event.mouseWheelDelta.x = 0.0f;
    event.mouseWheelDelta.y = 0.0f;
    event.keyLog.clear();
    event.viewResized = false;
    event.keyPressed = false;

    if((viewPort == nullptr) || (!viewPort->isOpen())) return false;

    sf::Event SFevent;
    event.viewHasFocus = viewPort->hasFocus();

    while(viewPort->pollEvent(SFevent)){
        switch(SFevent.type){
            case sf::Event::Closed:{
                bClosed = true;
                drawLock.lock();
                viewPort->close();
                drawLock.unlock();
                return false;
                break;
            }
            case sf::Event::Resized:{
                event.lastViewBounds = event.viewBounds;
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
                            event.LMBreleased = false;
                            event.LMBhold = true;
                            event.LMBholdFrames = 0;
                            event.LMBholdTime = 0.0f;
                            event.LMBpressPosition = mousePos;
                        }
                        break;
                    }
                    case sf::Mouse::Right:{
                        if(!event.RMBhold){
                            event.RMBreleased = false;
                            event.RMBhold = true;
                            event.RMBholdFrames = 0;
                            event.RMBholdTime = 0.0f;
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
                        event.LMBreleased = true;
                        event.LMBreleaseFrames = 0;
                        event.LMBreleaseTime = 0.0f;
                        event.LMBreleasePosition = mousePos;
                        break;
                    }
                    case sf::Mouse::Right:{
                        event.RMBhold = false;
                        event.RMBreleased = true;
                        event.RMBreleaseFrames = 0;
                        event.RMBreleaseTime = 0.0f;
                        event.RMBreleasePosition = mousePos;
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
                    event.keyPressed = true;
                    if(!ctrlPressed() && (SFevent.text.unicode < 127)){
                        event.keyLog += static_cast<char>(SFevent.text.unicode);
                    }
                    break;
                }
                case sf::Event::KeyPressed:{
                    event.keyPressed = true;
                    event.timeLastKey = 0.0f;

                    if(ctrlPressed()){
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) event.keyLog += 'a';
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) event.keyLog += 'd';
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)) event.keyLog += 'v';
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) event.keyLog += 'c';
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) event.keyLog += 'z';
                        break;
                    }

                    switch(SFevent.key.code){
                        case sf::Keyboard::Delete:{
                            event.keyLog += static_cast<char>(CV_KEY_DELETE);
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
                            event.keyLog += static_cast<char>(CV_KEY_RETURN);
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                case sf::Event::MouseWheelScrolled:{
                    if(SFevent.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel){
                        event.mouseWheelDelta.y = SFevent.mouseWheelScroll.delta;
                    }
                    if(SFevent.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel){
                        event.mouseWheelDelta.x = SFevent.mouseWheelScroll.delta;
                    }
                    break;
                }
                default: break;
            }
        }
    }

    if(event.LMBhold) ++event.LMBholdFrames;
    else{
        ++event.LMBreleaseFrames;
    }

    if(event.RMBhold) ++event.RMBholdFrames;
    else ++event.RMBreleaseFrames;

    event.eventsProcessed = true;
    return true;
}

void CVView::close(){

    if(!bClosed){
        bClosed = true;
        #ifdef __APPLE__
        // Require main thread to call close() on OSX
        drawLock.lock();
        viewPort->close();
        drawLock.unlock();
        #endif // __APPLE__
    }

    forceClose = true; // Let app master handle destructor from main thread
}

CVView::~CVView(){
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

#define CVELEMENT_INIT \
        mouseHovering(false),\
        clickHeld(false),\
        highlighted(false),\
        bHasFocus(false),\
        bFollowMouseX(false),\
        bFollowMouseY(false),\
        bFade(false),\
        bMove(false),\
        bSpriteOnly(false),\
        bExpand(false),\
        bStop(true),\
        bBounce(false),\
        bForceOnScreen(false),\
        bHasShadow(false),\
        bDragAndDrop(false),\
        bClipBounds(false),\
        highlightColor(255,255,255,255),\
        origin(0.0f,0.0f),\
        destination(NAN, NAN),\
        velocity(0.0f,0.0f),\
        acceleration(0.0f,0.0f),\
        moveAngle(0.0f),\
        elasticity(1.0f),\
        friction(0.0f),\
        stateNum(0),\
        numStates(1),\
        state(0b0),\
        targetAlpha(255),\
        fadeLayers(0b0),\
        fadeRate(0),

CVElement::CVElement():
        visible(false),
        canMove(false),
        canHighlight(false),
        canClick(false),
        canDrag(false),
        active(true),
        CVELEMENT_INIT
        View(nullptr),
        drawTarget(nullptr){ }

CVElement::CVElement(CVView* View,
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
                   CVELEMENT_INIT
                   View(View),
                   drawTarget(View->viewPort){ }

CVElement::~CVElement(){
    View->releaseMouseCapture(*this);
}

bool CVElement::update(CVEvent& event, const sf::Vector2f& mousePos){
    if((View->viewPort == nullptr) || !View->viewPort->isOpen() || !visible) return false;

    if(bFollowMouseX){
        if(!isnan(event.currentMousePosition.x)) setPosition(event.currentMousePosition.x, getPosition().y);
    }

    if(bFollowMouseY){
        if(!isnan(event.currentMousePosition.y)){
            setPosition(getPosition().x, event.currentMousePosition.y);
        }
    }

    if(bSpriteOnly){
        updateBounds();
    }

    if(highlightable()){
        if(highlighted || (bounds.contains(mousePos) && event.focusFree())){
            setFocus(true);
            for(auto& spr : spriteList){
                spr.setColor(highlightColor);
            }
        }
        else{
            setFocus(false);
            for(auto& spr : spriteList){
                spr.setColor(baseFillColor());
            }
        }
    }

    if(canDrag &&
       bounds.contains(event.LMBpressPosition) &&
       event.focusFree()){
        setFocus(true);
        if(event.LMBhold && event.captureMouse() &&
           (event.LMBholdTime < 0.3f) &&
           !event.isCaptured(*this)){
               event.mouse_capture(*this);
           }
    }

    if(bFade){
        bool status = false;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());

        for(auto& color : colorTheme){
            if(color.a == targetAlpha) continue;
            if(targetAlpha > color.a){
                if(color.a + adjusted_fr < targetAlpha){
                    color.a += adjusted_fr;
                }
                else color.a = targetAlpha;
            }
            else{
                if(color.a - adjusted_fr > targetAlpha){
                    color.a -= adjusted_fr;
                }
                else{
                    color.a = targetAlpha;
                }
            }
        }

        if(highlightColor.a != targetAlpha){
            if(targetAlpha > highlightColor.a){
                if(highlightColor.a + adjusted_fr < targetAlpha){
                    highlightColor.a += adjusted_fr;
                }
                else highlightColor.a = targetAlpha;
            }
            else{
                if(highlightColor.a - adjusted_fr > targetAlpha){
                    highlightColor.a -= adjusted_fr;
                }
                else{
                    highlightColor.a = targetAlpha;
                }
            }
        }

        if(fadeLayers & CV_LAYER_SPRITES){
            sf::Color tmp;
            for(auto& sprite : spriteList){
                tmp = sprite.getColor();
                if(tmp.a == targetAlpha) continue;
                if(targetAlpha > tmp.a){
                    if(tmp.a + adjusted_fr < targetAlpha){
                        tmp.a += adjusted_fr;
                    }
                    else tmp.a = targetAlpha;
                }
                else{
                    if(tmp.a - adjusted_fr > targetAlpha){
                        tmp.a -= adjusted_fr;
                    }
                    else{
                        tmp.a = targetAlpha;
                    }
                }
                sprite.setColor(tmp);
            }
        }
    }

    if(bMove){
        if(!isnan(destination.x) && !isnan(destination.y)){
            if(getPosition() == destination){
                velocity.x = 0.0f;
                velocity.y = 0.0f;
                acceleration.x = 0.0f;
                acceleration.y = 0.0f;
            }
            else if(getDistance(getPosition(), destination) - scalar(velocity)*View->getEventTrace().lastFrameTime < 0.0f){
                velocity.x = 0.0f;
                velocity.y = 0.0f;
                acceleration.x = 0.0f;
                acceleration.y = 0.0f;
                setPosition(destination);
                destination.x = NAN;
                destination.y = NAN;
            }
            else push(get_angle(getPosition(), destination), scalar(velocity), friction);
        }

        if(bStop){
            if(acceleration.x != 0.0f){
                if((velocity.x != 0.0f) && ((abs(velocity.x) - abs(acceleration.x*View->getEventTrace().lastFrameTime)) < 0.0f)){
                    velocity.x = 0.0f;
                    acceleration.x = 0.0f;
                }
            }
            if(acceleration.y != 0.0f){
                if((velocity.y != 0.0f) && ((abs(velocity.y) - abs(acceleration.y*View->getEventTrace().lastFrameTime)) < 0.0f)){
                    velocity.y = 0.0f;
                    acceleration.y = 0.0f;
                }
            }
        }

        if(bBounce){ // Bounce inward
            if((bounds.left <= View->getBounds().left) && (velocity.x < 0.0f)){
                velocity.x = -velocity.x*elasticity; // Horizontal bounce
                velocity.y *= elasticity;
            }
            else if((bounds.left + bounds.width >= View->getBounds().left + View->getWidth()) && (velocity.x > 0.0f)){
                velocity.x = -velocity.x*elasticity;
                velocity.y *= elasticity;
            }


            if((bounds.top <= View->getBounds().top) && (velocity.y < 0.0f)){
                velocity.y = -velocity.y*elasticity; // Vertical bounce
                velocity.x *= elasticity;
            }
            else if((bounds.top + bounds.height >= View->getBounds().top + View->getBounds().height) && (velocity.y > 0.0f)){
                velocity.y = -velocity.y*elasticity;
                velocity.x *= elasticity;
            }
        }

        if(friction != 0.0f){ // Apply frictional force
            if(velocity.x > 0.0f){
                velocity.x -= abs(friction*cos(moveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.x < 0.0f) velocity.x = 0.0f;
            }
            else{
                velocity.x += abs(friction*cos(moveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.x > 0.0f) velocity.x = 0.0f;
            }

            if(velocity.y > 0.0f){
                velocity.y -= abs(friction*sin(moveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.y < 0.0f) velocity.y = 0.0f;
            }
            else{
                velocity.y += abs(friction*sin(moveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.y > 0.0f) velocity.y = 0.0f;
            }
        }

        velocity += acceleration*View->getEventTrace().lastFrameTime;
        if((velocity.x == 0.0f) && (velocity.y == 0.0f)) bMove = false;
        else move(velocity*View->getEventTrace().lastFrameTime);
    }

    if(bForceOnScreen){
        if(bounds.left < View->getBounds().left){
            move(sf::Vector2f(View->getBounds().left-bounds.left, 0.0f));
        }
        if(bounds.top < View->getBounds().top){
            move(sf::Vector2f(0.0f, View->getBounds().top-bounds.top));
        }
        if(bounds.left + bounds.width > View->getBounds().left + View->getWidth()){
            move(sf::Vector2f(View->getBounds().left + View->getWidth() - bounds.left - bounds.width, 0.0f));
        }
        if(bounds.top + bounds.height > View->getBounds().top + View->getHeight()){
            move(sf::Vector2f(0.0f, View->getBounds().top + View->getHeight() - bounds.top - bounds.height));
        }
    }

    if(bDragAndDrop){
        if(!bHasShadow && event.LMBhold && (event.LMBholdTime > 0.3f) &&
           bounds.contains(event.LMBpressPosition) &&
           event.focusFree() && event.captureMouse()){
            createShadow(180);
            event.mouse_capture(shadow);
            setFocus(true);
        }
        else if(bHasShadow && event.LMBreleased){
            removeShadow();
            sendData(event);
            setFocus(false);
        }
    }

    if(bClipBounds){
        drawMask.setOrigin(origin);
        drawMask.setPosition(getPosition());
    }

    if(event.LMBhold && (event.LMBholdTime < 0.3f) && !bounds.contains(event.LMBpressPosition)){
        setFocus(false);
    }

    return true;
}

const sf::Texture* CVElement::appTexture(const std::string& tag) const{
    return mainApp()->bitmaps.taggedTexture(tag);
}

const sf::Image* CVElement::appImage(const std::string& tag) const{
    return mainApp()->bitmaps.taggedImage(tag);
}

float CVElement::viewScale() const{ return View->getViewScale(); }

void CVElement::setDrawClipping(const bool& status){
    bClipBounds = status;
    if(bClipBounds){
        drawMask.setTexture(clipTexture);
        drawMask.setOrigin(origin);
        drawMask.setPosition(getPosition());
    }
}

void CVElement::setFade(const uint8_t& alpha, const int& rate, const unsigned char& flags){
    bFade = true;
    targetAlpha = alpha;
    fadeRate = abs(rate);
    fadeLayers = flags;
}

bool CVElement::draw(sf::RenderTarget* target){
    if(target == nullptr) return false;
    if(!visible) return false;

    for(auto& sprite : spriteList){
        target->draw(sprite);
    }

    if(bHasShadow){
        target->draw(shadow);
    }

    return true;
}

void CVElement::getTexture(sf::Texture& outTex){

    View->captureLock.lock();

    View->textureBuffer.create(bounds.width, bounds.height);
    View->textureBuffer.setView(sf::View(bounds));
    View->textureBuffer.clear(sf::Color::Transparent);

    draw(&View->textureBuffer);

    View->textureBuffer.display();
    outTex = View->textureBuffer.getTexture();

    View->captureLock.unlock();

}

void CVElement::createShadow(const uint8_t& alpha, const sf::Vector2f& scale){

    bHasShadow = true;
    getTexture(shadowTexture);

    shadow.setTexture(shadowTexture);
    shadow.setOrigin(origin);
    shadow.setPosition(getPosition());
    shadow.setColor(sf::Color(255,255,255,alpha));

}

void CVElement::removeShadow(){
    bHasShadow = false;
}

void CVElement::updateBounds(){
    if(spriteList.size() > 0){
        bounds = spriteList.front().getGlobalBounds();
        sf::FloatRect panelBounds;
        for(size_t i = 0; i < spriteList.size(); ++i){
            panelBounds = spriteList[i].getGlobalBounds();
            if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
            if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
            if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
                bounds.width = panelBounds.left + panelBounds.width - bounds.left;
            if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
                bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        }
    }
    else{
        bounds.width = 0.0f;
        bounds.height = 0.0f;
    }
}

void CVElement::setExpand(const bool& state){
    bExpand = state;
    updateBounds();
}

void CVElement::setElasticity(const float& newElasticity){
    if(newElasticity > 1.0f) elasticity = 1.0f;
    else if(newElasticity < 0.0f) elasticity = 0.0f;
    else elasticity = newElasticity;
}

void CVElement::stop(){
    bMove = false;
    destination.x = NAN;
    destination.y = NAN;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    acceleration.x = 0.0f;
    acceleration.y = 0.0f;
}

void CVElement::move_to(const sf::Vector2f& position,
                        const float& velocity,
                        const float& drag){
    bMove = true;
    destination = position;
    moveAngle = get_angle(position, getPosition());
    this->velocity = components(velocity, moveAngle);
    friction = abs(drag);
}

void CVElement::push(const float& angle,
                     const float& velocity,
                     const float& drag){
    bMove = true;
    moveAngle = angle;
    this->velocity = components(velocity, moveAngle);
    friction = abs(drag);
}

const CVApp* CVElement::mainApp() const{ return View->mainApp; }

void CVElement::addSprite(const sf::Texture* texture,
                          const sf::Vector2f& position,
                          const sf::Vector2f& size,
                          const sf::Color& fillColor){

    if(texture == nullptr) return;

    spriteList.emplace_back(*texture);
    sf::Vector2f texSize(texture->getSize());
    spriteList.back().setOrigin(texSize.x/2, texSize.y/2);
    spriteList.back().setScale(
        size.x/texSize.x,
        size.y/texSize.y);

    spriteList.back().setPosition(bounds.left + position.x,
                                  bounds.top + position.y);
    spriteList.back().setColor(fillColor);
}

void CVElement::removeSprites(const std::string& tag){
    size_t L = spriteList.size();
    for(size_t i = 0; i < L;){
        if(mainApp()->bitmaps.getTextureName(spriteList[i].getTexture()) == tag){
            spriteList.erase(spriteList.begin() + i);
            --L;
        }
        else ++i;
    }
}

bool CVElement::has_sprite(const std::string& tag) const{
    for(auto& sprite : spriteList){
        if(mainApp()->bitmaps.getTextureName(sprite.getTexture()) == tag) return true;
    }
    return false;
}

sf::Sprite& CVElement::lastSprite(){
    if(spriteList.empty()) throw std::invalid_argument("No sprites in sprite list to index");
    return spriteList.back();
}

sf::Sprite& CVElement::firstSprite(){
    if(spriteList.empty()) throw std::invalid_argument("No sprites in sprite list to index");
    return spriteList.front();
}

sf::Sprite& CVElement::getSprite(const unsigned int& index){
    if(spriteList.empty()) throw std::invalid_argument("No sprites in sprite list to index");
    if(index > spriteList.size()) throw std::out_of_range("Index out of range of sprite list");
    return spriteList[index];
}

CVShape::CVShape(CVView* View):
        CVElement(View),
        bMasked(false){

}

void CVShape::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVShape::move(const sf::Vector2f& offset){
    for(auto& sprite : spriteList){
        sprite.move(offset);
    }

    bounds.left += offset.x;
    bounds.top += offset.y;

    shapeMask.move(offset);

}

void CVShape::setMask(const sf::Texture* texture,
                      const sf::Color& fillColor){
    shapeMask.setTexture(texture);
    shapeMask.setOrigin(origin);
    shapeMask.setSize(getSize());
    shapeMask.setRotation(getRotation());
    shapeMask.setPosition(getPosition());
    shapeMask.setFillColor(fillColor);
    bMasked = true;
}

void CVShape::hideMask(){
    bMasked = false;
}

void CVShape::showMask(){
    bMasked = true;
}

void CVShape::setSpriteColor(const sf::Color& newColor){
    CVElement::setColor(newColor);
}

void CVShape::setMaskAlpha(const uint8_t& newAlpha){
    sf::Color tmp = shapeMask.getFillColor();
    tmp.a = newAlpha;
    shapeMask.setFillColor(tmp);
}

bool CVShape::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVElement::update(event, mousePos)) return false;
    if(bMasked){
        shapeMask.setSize(getSize());
        shapeMask.setPosition(getPosition());

        sf::Color tmp = shapeMask.getFillColor();
        if(targetAlpha > tmp.a){
            if(tmp.a + fadeRate < targetAlpha){
                tmp.a += fadeRate;
            }
            else tmp.a = targetAlpha;
        }
        else{
            if(tmp.a - fadeRate > targetAlpha){
                tmp.a -= fadeRate;
            }
            else{
                tmp.a = targetAlpha;
            }
        }
        shapeMask.setFillColor(tmp);
    }
    return true;
}

bool CVShape::draw(sf::RenderTarget* target){
    if(target == nullptr) return false;
    if(!visible) return false;
    return true;
}

bool CVBox::draw(sf::RenderTarget* target){
    if(!CVShape::draw(target)) return false;

    if(!bSpriteOnly){
        for(auto& item : panel){
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& sprite : spriteList){
        target->draw(sprite);
    }

    return true;
}

void CVBox::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
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

void CVBox::rotate(const float& angle, const unsigned char& flags, const sf::Vector2f& origin){
    sf::Vector2f objCenter = origin, itemCenter;
    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + this->origin;

    float dist, oldAngle;

//    std::cout << "Object: " << objCenter.x << ' ' << objCenter.y << '\n';

    for(auto& item : panel){
        item.rotate(angle);
        itemCenter = item.getPosition();

//        std::cout << "Item: " << itemCenter.x << ' ' << itemCenter.y << '\n';

        dist = getDistance(objCenter, itemCenter);
        if(dist != 0.0f){
            oldAngle = get_angle(objCenter, itemCenter);
//            item.setPosition(radial_position(objCenter, dist, (oldAngle + angle)/PI));
        }
    }

    shapeMask.rotate(angle);

    for(auto& item : spriteList){
        item.rotate(angle);
        itemCenter = item.getPosition();

        dist = getDistance(objCenter, itemCenter);
        if(dist != 0.0f){
            oldAngle = get_angle(objCenter, itemCenter);
//            item.setPosition(radial_position(objCenter, dist, (oldAngle + angle)/PI));
        }
    }
}

void CVBox::setColor(const sf::Color& newColor){
    CVElement::setColor(newColor);
    for(auto& item : panel){
        item.setFillColor(newColor);
        item.setOutlineColor(newColor);
    }
    shapeMask.setFillColor(newColor);
}

void CVBox::setFillColor(const sf::Color& newColor){
    for(auto& item : panel){
        item.setFillColor(newColor);
    }
    shapeMask.setFillColor(newColor);
}

void CVBox::setOutlineColor(const sf::Color& newColor){
    for(auto& item : panel){
        item.setOutlineColor(newColor);
    }
}

void CVBox::setRounding(const float& radius,
                        const unsigned int& pointCount,
                        const std::vector<bool>& states){
    if(!panel.empty()){
        panel.front().setRounding(radius, pointCount, states);
    }
    shapeMask.setRounding(radius, pointCount, states);

}

const float& CVBox::getRoundingRadius() const{
    return panel.front().getRoundingRadius();
}

const unsigned int& CVBox::getRoundingPointCount() const{
    return panel.front().getArcPointCount();
}

void CVBox::setOutlineThickness(const float& newThickness){
    for(auto& item : panel){
        item.setOutlineThickness(newThickness);
    }
}

void CVBox::setRotation(const float& angle, const unsigned char& flags, const sf::Vector2f& origin){
    sf::Vector2f objCenter = origin, itemCenter;
    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + this->origin;

    float dist;

    if(flags & CV_LAYER_SHAPES){
        for(auto& item : panel){
            item.setRotation(angle);
            itemCenter = item.getPosition();

            dist = getDistance(objCenter, itemCenter);
            if(dist != 0.0f){
                item.setPosition(radial_position(objCenter, dist, angle/PI));
            }
        }
        shapeMask.setRotation(angle);
    }

    if(flags & CV_LAYER_SPRITES){
        for(auto& item : spriteList){
            item.setRotation(angle);
            itemCenter = item.getPosition();

            dist = getDistance(objCenter, itemCenter);
            if(dist != 0.0f){
                item.setPosition(radial_position(objCenter, dist, angle/PI));
            }
        }
    }

}

float CVBox::getRotation(const unsigned char& flags) const{
    if(flags & CV_LAYER_SHAPES){
        return panel.front().getRotation();
    }
    if((flags & CV_LAYER_SPRITES) && !spriteList.empty()){
        return spriteList.front().getRotation();
    }
    return 0.0f;
}

CVBox::CVBox(CVView* View,
            sf::Vector2f position, float width, float height,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
            CVShape(View),
            bNoFill(false){

    createdOrigin = position;

    colorTheme.emplace_back(fillColor);
    colorTheme.emplace_back(borderColor);

    highlightColor = fillColor;
    brighten(highlightColor, 50);

    bounds = sf::FloatRect(position.x, position.y, width, height);

    colorTheme = {fillColor, borderColor};

    // Main panel
    panel.emplace_back(sf::Vector2f(width, height));
    panel.back().setPosition(position);
    panel.back().setFillColor(fillColor);
    panel.back().setOutlineThickness(borderWidth);
    panel.back().setOutlineColor(borderColor);

    shapeMask.setSize(sf::Vector2f(width, height));
    shapeMask.setPosition(position);
    shapeMask.setFillColor(sf::Color(255,255,255,0));
}

void CVBox::setClearFill(const bool& state){
    bNoFill = state;
    if(bNoFill){
        panel.front().setFillColor(sf::Color::Transparent);
    }
    else{
        panel.front().setFillColor(baseFillColor());
    }
}

void CVBox::updateBounds(){
    CVElement::updateBounds();
    if(!bSpriteOnly){
        sf::FloatRect panelBounds = panel.front().getGlobalBounds();
        if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
        if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
        if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
            bounds.width = panelBounds.left + panelBounds.width - bounds.left;
        if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
            bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(sf::Vector2f(bounds.left, bounds.top));
    }
}

void CVBox::setExpand(const bool& state){
    bExpand = state;
    if(bExpand) updateBounds();
}

bool CVBox::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVShape::update(event, mousePos)) return false;
    if(bFade && !bNoFill && !bSpriteOnly){
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());

        for(auto& item : panel){
            tmp = item.getFillColor();
            if(targetAlpha > tmp.a){
                if(tmp.a + adjusted_fr < targetAlpha){
                    tmp.a += adjusted_fr;
                }
                else tmp.a = targetAlpha;
            }
            else{
                if(tmp.a - adjusted_fr > targetAlpha){
                    tmp.a -= adjusted_fr;
                }
                else{
                    tmp.a = targetAlpha;
                }
            }
            item.setFillColor(tmp);

            tmp = item.getOutlineColor();
            if(targetAlpha > tmp.a){
                if(tmp.a + adjusted_fr < targetAlpha){
                    tmp.a += adjusted_fr;
                }
                else tmp.a = targetAlpha;
            }
            else{
                if(tmp.a - adjusted_fr > targetAlpha){
                    tmp.a -= adjusted_fr;
                }
                else{
                    tmp.a = targetAlpha;
                }
            }
            item.setOutlineColor(tmp);
        }

    }
    if(canHighlight || highlighted){
        if((event.focusFree() && bounds.contains(event.lastFrameMousePosition)) || highlighted){
            sf::Color tmp;
            for(auto& item : panel){
                tmp = highlightColor;
                tmp.a = baseFillColor().a;
                item.setFillColor(tmp);
            }
        }
        else{
            for(auto& item : panel){
                item.setFillColor(baseFillColor());
            }
        }
    }

    return true;
}

CVTextBox::CVTextBox(CVView* View,
            sf::Vector2f position, float width, float height,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
                CVBox(View, position, width, height,
              fillColor, borderColor, borderWidth),
              textInfo("", mainApp()->getDefaultFont(), 12),
              textPadding(5.0f),
              textFont(nullptr),
              bWrapText(false){
}

CVTextBox::CVTextBox(CVView* View,
            sf::Vector2f position, float width, float height,
            textEntry textInfo,
            sf::Color fillColor,
            sf::Color borderColor,
            float borderWidth):
                CVBox(View, position, width, height,
              fillColor, borderColor, borderWidth),
              textInfo(textInfo),
              textPadding((width+height)/50),
              textFont(appFont(textInfo.font)),
              bWrapText(false){

    addTextEntry(textInfo, position);
    alignText();

}

void CVTextBox::alignText(){
    sf::FloatRect textBounds;
    sf::Vector2f textPos = getPosition();
    unsigned int numText = displayText.size()+1,
                i = 1;

    for(auto& text : displayText){
        textBounds = text.getGlobalBounds();

        switch(textInfo.alignment){
            case ALIGN_CENTER:{
                textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2;
                break;
            }
            case ALIGN_CENTER_MIDLINE:{
                textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text)/2;
                break;
            }
            case ALIGN_CENTER_BOTTOM:{
                textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
                if(i > 1) textPos.y = displayText[i-1].getGlobalBounds().top - textBounds.height;
                else textPos.y = bounds.top - textBounds.height;
                break;
            }
            case ALIGN_RIGHT:{
                textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2;
                break;
            }
            case ALIGN_RIGHT_MIDLINE:{
                textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text)/2;
                break;
            }
            case ALIGN_RIGHT_BOTTOM:{
                textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;
                if(i > 1) textPos.y = displayText[i-1].getGlobalBounds().top - textBounds.height;
                else textPos.y = bounds.top - textBounds.height;
                break;
            }
            case ALIGN_LEFT_MIDLINE:{
                textPos.x = bounds.left + textPadding;
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text);
                break;
            }
            case ALIGN_LEFT_BOTTOM:{
                textPos.x = bounds.left + textPadding;
                if(i > 1) textPos.y = displayText[i-1].getGlobalBounds().top - textBounds.height;
                else textPos.y = bounds.top - textBounds.height;
                break;
            }
            default:{ // Left
                textPos.x = bounds.left + textPadding;
                textPos.y = bounds.top + textPadding + ((i - 1)*bounds.height)/numText;
                break;
            }
        }

        text.setPosition(textPos);
        ++i;
    }
}

void CVTextBox::wrapText(){
    sf::FloatRect textBounds;
    for(auto& text : displayText){
        textBounds = text.getGlobalBounds();

        if(textBounds.width > bounds.width - 2*textPadding){
            std::string displayTextString = text.getString();
            for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
                if(text.findCharacterPos(i).x - bounds.left > bounds.width - 2*textPadding){
                    for(int j = i; j > 0; --j){
                        if(displayTextString[j-1] == ' '){
                            displayTextString[j-1] = '\n';
                            break;
                        }
                        else if(isCharType(displayTextString[j-1], ",\\/:._-")){
                            displayTextString.insert(displayTextString.begin() + j, '\n');
                            break;
                        }
                        else if(abs(text.findCharacterPos(i).x -
                                text.findCharacterPos(j).x) > bounds.width - 2*textPadding){
                            displayTextString.insert(displayTextString.begin() + i - 1, '\n');
                            ++i;
                            break;
                        }
                    }

                    text.setString(displayTextString);
                    textBounds = text.getGlobalBounds();
                    if(textBounds.width < bounds.width - 2*textPadding) break;
                }
            }
        }
    }

    if(bExpand) updateBounds();
    alignText();
}

void CVTextBox::setString(const std::string& newString){
    if(displayText.size() > 0){
        displayText.front().setString(newString);
        if(bWrapText) wrapText();
    }
}

void CVTextBox::setTextSize(const unsigned int& newSize){
    for(auto& text : displayText){
        text.setCharacterSize(newSize);
    }
    textInfo.fontSize = newSize;
}

void CVTextBox::setTextColor(const sf::Color& newColor){
    for(auto& text : displayText){
        text.setFillColor(newColor);
    }
    textInfo.textColor = newColor;
}

void CVTextBox::setTextAlignment(const uint8_t& newAlignment){
    textInfo.alignment = newAlignment;
    alignText();
}

void CVTextBox::setTextWrap(const bool& state){
    bWrapText = state;
    if(bWrapText) wrapText();
}

void CVTextBox::setTextPadding(const float& newPadding){
    textPadding = newPadding;
    if(bWrapText) wrapText();
    else alignText();
}

void CVTextBox::setExpand(const bool& state){
    bExpand = state;
    if(bExpand) updateBounds();
}

void CVTextBox::updateBounds(){
    CVBox::updateBounds();
    if(!bSpriteOnly){
        sf::FloatRect textBounds = get_element_bounds(displayText);

        if(textBounds.left < bounds.left) bounds.left = textBounds.left;
        if(textBounds.top < bounds.top) bounds.top = textBounds.top;
        if(textBounds.left + textBounds.width > bounds.left + bounds.width)
            bounds.width = textBounds.left + textBounds.width - bounds.left;
        if(textBounds.top + textBounds.height > bounds.top + bounds.height)
            bounds.height = textBounds.top + textBounds.height - bounds.top;

        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(sf::Vector2f(bounds.left, bounds.top) + panel.front().getOrigin());
    }
}

void CVTextBox::sendData(CVEvent& event) const{
    std::string cpyData = getText();
    event.sendData(cpyData.c_str(), cpyData.size(), CV_EVENT_LMB);
}

std::string CVTextBox::getText() const{
    std::stringstream output;
    for(size_t i = 0; i < displayText.size(); ++i){
        std::string newLine = displayText[i].getString();
        if(bWrapText){
            for(size_t j = 0; j < newLine.size();){
                if(newLine[j] == '\n') newLine.erase(newLine.begin() + j);
                else ++j;
            }
        }
        output << newLine;
        if(i < displayText.size() - 1) output << '\n';
    }
    return output.str();
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

    if(bWrapText){
        cvis::wrapText(displayText.back(), bounds, textPadding);
    }
    if(bExpand) updateBounds();
}

void CVTextBox::addTextEntry(const textEntry& newText, const sf::Vector2f& position){

    if(appFont(newText.font) != nullptr){
        displayText.emplace_back(newText.text,
                             *mainApp()->fonts[newText.font],
                             newText.fontSize);
    }
    else{
        displayText.emplace_back();
        displayText.back().setString(newText.text);
        displayText.back().setCharacterSize(newText.fontSize);
    }

    displayText.back().setFillColor(newText.textColor);
    displayText.back().setStyle(newText.textStyle);

    colorTheme.emplace_back(newText.textColor);

    switch(newText.alignment){
        case ALIGN_LEFT_MIDLINE:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x, position.y - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2);
            break;
        }
        case ALIGN_LEFT_BOTTOM:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x, position.y - textBounds.height);
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            sf::FloatRect textBounds = displayText.back().getLocalBounds();
            displayText.back().setPosition(position.x - textBounds.width/2, position.y - textBounds.height/2);
            break;
        }
        case ALIGN_CENTER_BOTTOM:{
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

    if(bWrapText){
        cvis::wrapText(displayText.back(), bounds, textPadding);
    }
    if(bExpand) updateBounds();

}

const sf::Font* CVTextBox::appFont(const std::string& font) const{
    return mainApp()->fonts[font];
}

void CVTextBox::setFont(const std::string& font){
    textInfo.font = font;
    textFont = appFont(font);

    for(auto& text : displayText){
        text.setFont(*textFont);
    }
}

void CVTextBox::setFont(const sf::Font& font){
    textFont = &font;

    for(auto& text : displayText){
        text.setFont(font);
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

bool CVTextBox::draw(sf::RenderTarget* target){
    if(!CVBox::draw(target)) return false;

    for(auto& text : displayText){
        target->draw(text);
    }

    if(bHasShadow){
        target->draw(shadow);
    }

    return true;
}

bool CVTextBox::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBox::update(event, mousePos)) return false;

    if(bFade){
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());
        for(auto& text : displayText){
            tmp = text.getFillColor();
            if(targetAlpha > tmp.a){
                if(tmp.a + adjusted_fr < targetAlpha){
                    tmp.a += adjusted_fr;
                }
                else{
                    tmp.a = targetAlpha;
                    bFade = false;
                }
            }
            else{
                if(tmp.a - adjusted_fr > targetAlpha){
                    tmp.a -= adjusted_fr;
                }
                else{
                    tmp.a = targetAlpha;
                }
            }
            text.setFillColor(tmp);
        }
    }

    return true;
}

void CVTextBox::setText(const unsigned int textIndex, std::string newText){
    if(displayText.size() > textIndex) displayText[textIndex].setString(newText);
}

CVViewPanel::CVViewPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                         const sf::Vector2f& size, bool fitToWindow, const sf::Vector2f& position):
    CVTextBox(parentView, position, parentView->getWidth(), parentView->getHeight(), backgroundColor, backgroundColor, 0.0f),
    bFitWindow(fitToWindow),
    bTransduceFade(false),
    bTransduceFocus(false),
    bDragAndDrop(false),
    bOutOfBoundsDraw(false),
    bOutOfBoundsUpdate(false){

        if(!isnan(size.x) && !isnan(size.y)) setSize(size);
        else{
            setSize(parentView->getSize());
        }
}

CVViewPanel::~CVViewPanel(){
    for(auto& item : viewPanelElements){
        delete(item);
    }
}

void CVViewPanel::updateBounds(){
    if(numPanels() > 0){
        bounds = viewPanelElements.front()->getBounds();
        sf::FloatRect panelBounds;
        for(size_t i = 1; i < viewPanelElements.size(); ++i){
            panelBounds = viewPanelElements[i]->getBounds();
            if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
            if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
            if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
                bounds.width = panelBounds.left + panelBounds.width - bounds.left;
            if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
                bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        }

        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(bounds.left, bounds.top);
    }
    else{
        bounds.width = 0.0f;
        bounds.height = 0.0f;
        panel.front().setSize(sf::Vector2f(0.0f,0.0f));
    }
}

void CVViewPanel::setExpand(const bool& state){
    bExpand = state;
    if(bExpand){
        updateBounds();
    }
}

void CVViewPanel::addPanelElement(CVElement* newElement, std::string newTag, const unsigned int& index){

    // Transfer flags

    newElement->viewPanel = this;
    newElement->setTag(newTag);

    if(index < numPanels()){
        viewPanelElements.insert(viewPanelElements.begin() + index, newElement);
        viewPanelTags.insert(viewPanelTags.begin() + index, newTag);
    }
    else{
        viewPanelTags.push_back(newTag);
        viewPanelElements.push_back(newElement);
    }

    if(bExpand){
        updateBounds(); // Update the panel boundaries to accommodate if applicable
    }

}

void CVViewPanel::clear(){
    for(auto& item : viewPanelElements){
        delete(item);
    }
    viewPanelElements.clear();
    viewPanelTags.clear();
}

bool CVViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVTextBox::update(event, mousePos)) return false;

    if(bExpand){
        updateBounds();
    }

    if(bFitWindow && event.viewResized){
       setPosition(0.0f,0.0f);
       setSize(sf::Vector2f(event.viewBounds.width, event.viewBounds.height));
    }

    if(bFade && bTransduceFade){
        for(auto& item : viewPanelElements){
            item->setFade(targetAlpha, fadeRate);
        }
    }

    if(bTransduceFocus){
        for(auto& item : viewPanelElements){
            item->setFocus(hasFocus());
        }
    }

    return true;
}

void CVViewPanel::removePanelElement(const unsigned int& index){
    if(index < numPanels()){
        delete(viewPanelElements[index]);
        viewPanelElements.erase(viewPanelElements.begin() + index);
        viewPanelTags.erase(viewPanelTags.begin() + index);
    }
    else{
        throw std::out_of_range("In CVViewPanel::removePanelElement: index out of range of element list");
    }

    if(bExpand){
        updateBounds();
    }
}

void CVViewPanel::removePanelElement(CVElement* element){
    for(size_t i = 0; i < viewPanelElements.size(); ++i){
        if(viewPanelElements[i] == element){
            removePanelElement(i);
            return;
        }
    }
}

void CVViewPanel::removePanelElement(const std::string& tag){
    for(size_t i = 0; i < viewPanelElements.size(); ++i){
        if(viewPanelTags[i] == tag){
            removePanelElement(i);
            return;
        }
    }
}

void CVViewPanel::move(const sf::Vector2f& distance){
    CVTextBox::move(distance);
    for(auto& panel : viewPanelElements){
        panel->move(distance);
    }
}

void CVViewPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVViewPanel::setSize(const sf::Vector2f& newSize){
    float scaleFactorX = getSize().x/newSize.x,
            scaleFactorY = getSize().y/newSize.y;
    for(auto& item : viewPanelElements){
        sf::Vector2f dist = item->getPosition() - getPosition(),
                    scaleDist = dist;
        scaleDist.x *= scaleFactorX;
        scaleDist.y *= scaleFactorY;
        item->move(-(scaleDist - dist));
//        item->setSize(item->getSize().x*scaleDist.x,
//                      item->getSize().y*scaleDist.y);
    }
    CVBox::setSize(newSize);
}

CVElement* CVViewPanel::getTaggedElement(const std::string& tag){
    unsigned int index = 0;
    for(auto& T : viewPanelTags){
        if(T == tag) return viewPanelElements[index];
        else ++index;
    }
    return nullptr;
}

bool CVViewPanel::elementExists(const std::string& tag){
    for(auto& T : viewPanelTags){
        if(T == tag) return true;
    }
    return false;
}

void CVViewPanel::setHighlightableStatus(const bool& status){
    CVElement::setHighlightableStatus(status);
    for(auto& item : viewPanelElements){
        item->setHighlightableStatus(status);
    }
}

void CVViewPanel::setFocus(const bool& state){
    CVElement::setFocus(state);
    if(bTransduceFocus && state){
        for(auto& item : viewPanelElements){
            item->setFocus(state);
        }
    }
}

void CVViewPanel::setHighlightColor(const sf::Color& color){
    highlightColor = color;
    for(auto& item : viewPanelElements){
        item->setHighlightColor(color);
    }
}

}
