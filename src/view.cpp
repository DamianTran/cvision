#include "cvision/view.hpp"

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

        #ifdef __APPLE__
        // Construct the viewPort outside of the worker thread for OSX compatibility
        viewPort = new sf::RenderWindow(sf::VideoMode(x, y), winName, style);
        #endif

        mainApp->viewThreads.emplace_back([&, x, y, winName, style, screenPosition](){

            const float frameTime = 0.5f/frameRateLimit;

            #ifndef __APPLE__
            viewPort = new sf::RenderWindow(sf::VideoMode(x, y), winName, style);
            #endif

            moveTarget = viewPort->getPosition();
            sf::Vector2i intPos;
            if(!isnan(screenPosition.x) && !isnan(screenPosition.y)) viewPort->setPosition(sf::Vector2i(screenPosition));

            std::chrono::high_resolution_clock::time_point t0 = TIME_NOW;
            const std::chrono::duration<float> frameUpdateLatency(frameTime);

            std::chrono::duration<float> duration;

            eventTrace.viewBounds = sf::FloatRect(moveTarget.x, moveTarget.y, x, y);
            eventTrace.lastViewBounds = eventTrace.viewBounds;
            eventTrace.lastFrameMousePosition =
                sf::Vector2f(sf::Mouse::getPosition(*viewPort));

            while(!bClosed && viewPort->isOpen()){

                if(forceClose){
                    #ifndef __APPLE__
                    viewPort->close();
                    #endif
                    break;
                }

                intPos = sf::Mouse::getPosition(*viewPort);
                mousePos.x = intPos.x;
                mousePos.y = intPos.y;

                if(update(eventTrace, mousePos)){
                    drawLock.lock();
                    viewPort->clear();
                    draw();
                    viewPort->display();
                    drawLock.unlock();
                }

                // Framerate limit cycle

                duration = TIME_NOW - t0;

                if(duration < frameUpdateLatency){
                    std::this_thread::sleep_for(std::chrono::duration<float>(frameTime - duration.count()));
                }

                eventTrace.lastFrameTime = std::chrono::duration<float>(TIME_NOW - t0).count();
                t0 = TIME_NOW;
                frameRate = 1.0f/eventTrace.lastFrameTime;

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

        });
        appThread = &mainApp->viewThreads.back(); // Establish reference to app thread for this view
        #ifndef __APPLE__
        // Wait for the view to be constructed
        while(viewPort == nullptr){
            std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
        }
        #endif
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
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return;
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

    #ifdef __APPLE__
    // Get event handle processed by the main thread
    if(!mainApp->getViewEventHandle(this, event)) return false;
    #else
    if(!handleViewEvents(event)) return false;
    #endif

//    if(viewPort->getPosition() != moveTarget){
//        sf::Vector2i viewPos(viewPort->getPosition());
//        if(moveSpeed.x != 0){
//            if(viewPos.x < moveTarget.x){
//                if(viewPos.x + moveSpeed.x > moveTarget.x){
//                   viewPos.x = moveTarget.x;
//                   moveSpeed.x = 0;
//                }
//                else viewPos.x += moveSpeed.x;
//            }
//            if(viewPos.x > moveTarget.x){
//                if(viewPos.x + moveSpeed.x < moveTarget.x){
//                   viewPos.x = moveTarget.x;
//                   moveSpeed.x = 0;
//                }
//                else viewPos.x += moveSpeed.x;
//            }
//        }
//
//        if(moveSpeed.y != 0){
//            if(viewPos.y < moveTarget.y){
//                if(viewPos.y + moveSpeed.y > moveTarget.y){
//                   viewPos.y = moveTarget.y;
//                   moveSpeed.y = 0;
//                }
//                else viewPos.y += moveSpeed.y;
//            }
//            else if(viewPos.y > moveTarget.y){
//                if(viewPos.y + moveSpeed.y < moveTarget.y){
//                   viewPos.y = moveTarget.y;
//                   moveSpeed.y = 0;
//                }
//                else viewPos.y += moveSpeed.y;
//            }
//        }
//        viewPort->setPosition(viewPos);
//    }

//    if(viewPort->getSize() != resizeTarget){
//        sf::Vector2u viewSize = viewPort->getSize();
//        bool resizeFLAG = false;
//
//        if(resizeSpeed.x != 0){
//            if(viewSize.x < resizeTarget.x){
//                if(viewSize.x + resizeSpeed.x > resizeTarget.x){
//                   viewSize.x = resizeTarget.x;
//                   resizeSpeed.x = 0;
//                }
//                else viewSize.x += resizeSpeed.x;
//            }
//            else if(viewSize.x > resizeTarget.x){
//                if(viewSize.x + resizeSpeed.x < resizeTarget.x){
//                   viewSize.x = resizeTarget.x;
//                   resizeSpeed.x = 0;
//                }
//                else viewSize.x += resizeSpeed.x;
//            }
//            resizeFLAG = true;
//        }
//
//        if(resizeSpeed.y != 0){
//            if(viewSize.y < resizeTarget.y){
//                if(viewSize.y + resizeSpeed.y > resizeTarget.y){
//                   viewSize.y = resizeTarget.y;
//                   resizeSpeed.y = 0;
//                }
//                else viewSize.y += resizeSpeed.y;
//            }
//            else if(viewSize.y > resizeTarget.y){
//                if(viewSize.y + resizeSpeed.y < resizeTarget.y){
//                   viewSize.y = resizeTarget.y;
//                   resizeSpeed.y = 0;
//                }
//                else viewSize.y += resizeSpeed.y;
//            }
//            resizeFLAG = true;
//        }
//
//        if(resizeFLAG) viewPort->setSize(viewSize);
//    }

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

    // Open the mouse pointer to capture
    event.mouseCaptured = false;

    for(auto& element : boost::adaptors::reverse(popUpElements)){
        element->update(event, mousePos);
    }
    for(auto& panel : boost::adaptors::reverse(viewPanels)){
        panel->update(event, mousePos);
    }

    event.lastFrameMousePosition = mousePos;
    event.timeLastKey += event.lastFrameTime;

    if(overlay != nullptr) overlay->update(event, mousePos);

    return true;
}

bool view::handleViewEvents(CVEvent& event){
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
//                event.lastViewBounds = event.viewBounds;
//                viewPort->setView(sf::View(sf::FloatRect(0, 0, SFevent.size.width, SFevent.size.height)));
//                event.viewBounds = sf::FloatRect(viewPort->getPosition().x, viewPort->getPosition().y,
//                                                 viewPort->getSize().x, viewPort->getSize().y);
//                event.lastViewBounds = event.viewBounds;
//                event.viewResized = true;
                break;
            }
            case sf::Event::MouseButtonPressed:{
                switch(SFevent.mouseButton.button){
                    case sf::Mouse::Left:{
                        if(!event.LMBhold){
                            event.LMBreleased = false;
                            event.LMBhold = true;
                            event.LMBreleaseFrames = 0;
                            event.LMBpressPosition = mousePos;
                        }
                        break;
                    }
                    case sf::Mouse::Right:{
                        if(!event.RMBhold){
                            event.RMBreleased = false;
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
                        event.LMBreleased = true;
                        event.LMBholdFrames = 0;
                        break;
                    }
                    case sf::Mouse::Right:{
                        event.RMBhold = false;
                        event.RMBreleased = true;
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
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) event.keyLog += 'd';
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

    if(event.LMBhold) ++event.LMBholdFrames;
    else{
        ++event.LMBreleaseFrames;
    }

    if(event.RMBhold) ++event.RMBholdFrames;
    else ++event.RMBreleaseFrames;

    event.eventsProcessed = true;
    return true;
}

void view::close(){

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
