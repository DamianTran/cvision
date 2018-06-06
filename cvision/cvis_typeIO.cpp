#include "cvision/cvis_typeIO.hpp"

using namespace AIALG;

namespace cvis{

CVTypeBox::CVTypeBox(view* View, const sf::Vector2f& position, const float width, const float height,
      const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
      const float borderWidth, const uint8_t& animType, const uint8_t& textFitType):
          CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
          bkgString(textInfo.text),
          timeLastBlink(0.0f),
          blinkFreq(1.0f),
          bHasFocus(false),
          bBlink(false),
          bWrapExpandDown(true),
          animType(animType),
          textFitType(textFitType),
          cursorPos(0),
          typeFrameStart(0),
          typeFrameEnd(0),
          logTextIndex(0),
          selectionStart(UINT_MAX),
          enterLine(false),
          textLog(nullptr)
          {

    addTextEntry(textInfo);
    setTextTemplate(textInfo);

    sf::FloatRect textBounds;

    textBounds = displayText.back().getGlobalBounds();

    xPadding = (bounds.height - textBounds.height)/2;
    if(xPadding < 0.0f) xPadding = 0.0f;

    displayText.back().setPosition(bounds.left + xPadding, bounds.top + bounds.height/2 - textBounds.height);

    if(textInfo.text.find("\n") > textInfo.text.size()){
        if(textBounds.height > bounds.height) bounds.height = textBounds.height;
    }

    float cursorOrigin = 0.0f;

    switch(textInfo.alignment){
        case ALIGN_RIGHT:{
            displayText.back().move(bounds.width - xPadding - textBounds.width - borderWidth, 0.0f);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2, displayText.back().getPosition().y);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                           displayText.back().getPosition().y - getTextCenterOffsetY(displayText.back())/2);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER_TOP:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                           displayText.back().getPosition().y);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_VERTICAL_INVERTED:{

            panel.front().setOrigin(bounds.left/2, bounds.height/2);
            panel.front().setRotation(-90);
            panel.front().setPosition(position);

            bounds = panel.front().getGlobalBounds();

            displayText.back().setOrigin(textBounds.width/2, textBounds.height/2);
            displayText.back().setPosition(panel.front().getPosition());
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            displayText.back().setRotation(-90);

            break;
        }
        case ALIGN_LEFT_MIDLINE:{
            displayText.back().setPosition(displayText.back().getPosition().x,
                                           displayText.back().getPosition().y - textBounds.height/2);
            cursorOrigin = textBounds.height/2-getTextCenterOffsetY(displayText.back());
            break;
        }
        default:{ // Left-aligned
            break;
        }
    }

    textBounds = displayText.back().getGlobalBounds();
    cursor.setSize(sf::Vector2f(2.0f,
                displayText.back().getFont()->getGlyph(65, textInfo.fontSize, false, 0).bounds.height*1.2f +
                                xPadding/2));
    panel.emplace_back();

    if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
        cursor.setOrigin(0.0f, cursorOrigin);
        panel.back().setOrigin(cursor.getOrigin());
        cursor.setRotation(-90);
        panel.back().setSize(sf::Vector2f(cursor.getGlobalBounds().width, 0.0f));
    }
    else if(textInfo.alignment == ALIGN_VERTICAL){
        cursor.setOrigin(0.0f, cursorOrigin);
        panel.back().setOrigin(cursor.getOrigin());
        cursor.setRotation(90);
        panel.back().setSize(sf::Vector2f(cursor.getGlobalBounds().width, 0.0f));
    }
    else{
        cursor.setOrigin(0.0f, cursorOrigin);
        panel.back().setOrigin(cursor.getOrigin());
        panel.back().setSize(sf::Vector2f(0.0f, cursor.getGlobalBounds().height));
    }

    sf::Color textHighlightColor = borderColor;
    textHighlightColor.a = 60;
    panel.back().setFillColor(textHighlightColor);

    cursor.setPosition(sf::Vector2f(displayText.back().getPosition().x,
                                    displayText.back().getPosition().y));
    panel.back().setPosition(cursor.getPosition());

    if((borderColor != fillColor) && (borderColor != sf::Color::Transparent)) cursor.setFillColor(borderColor);
    else cursor.setFillColor(textInfo.textColor);

}

bool CVTypeBox::update(CVEvent& event, const sf::Vector2f& mousePos){

    bool slideCursor = true,
        cursorChanged = false;

    if(event.LMBhold){ // Handle click and drag
        if((bounds.contains(mousePos) || displayText.back().getGlobalBounds().contains(mousePos)) && event.captureMouse()){
            if(!bHasFocus){
                timeLastBlink = 0.0f;
                bBlink = false;
            }
            bHasFocus = true;
            size_t i = 0;
            float charDist = 0.0f;
            bool cursorFound = false,
                selectionFound = false;

            for(; i < displayText.back().getString().getSize(); ++i){
                if((textInfo.alignment == ALIGN_VERTICAL_INVERTED)){
                    if(i == typeString.size() - 1){
                        charDist = displayText.back().getPosition().y -
                                displayText.back().findCharacterPos(i).y;
                    }
                    else charDist = displayText.back().findCharacterPos(i).y - displayText.back().findCharacterPos(i+1).y;

                    if((event.LMBholdFrames < 3) && !selectionFound){
                        if(event.LMBpressPosition.y > displayText.back().findCharacterPos(i).y - charDist/2){
                            selectionStart = i + typeFrameStart;
                            selectionFound = true;
                        }
                        else if(i == typeString.size() - 1){
                            selectionStart = i + typeFrameStart + 1;
                        }
                    }
                    if(!cursorFound){
                        if(mousePos.y > displayText.back().findCharacterPos(i).y - charDist/2){
                            cursorPos = i + typeFrameStart;
                            cursorFound = true;
                        }
                        else if(i == typeString.size() - 1){
                            cursorPos = i + typeFrameStart + 1;
                        }
                    }
                }
                else{
                    if(i == typeString.size() - 1){
                        charDist = displayText.back().getGlobalBounds().width -
                        displayText.back().findCharacterPos(i).x
                        + displayText.back().getPosition().x;
                    }
                    else charDist = displayText.back().findCharacterPos(i+1).x - displayText.back().findCharacterPos(i).x;

                    if((event.LMBholdFrames < 3) && !selectionFound){
                        if(event.LMBpressPosition.x < displayText.back().findCharacterPos(i).x - charDist/2){
                            selectionStart = i + typeFrameStart - 1;
                            selectionFound = true;
                        }
                        else if(i == typeString.size() - 1){
                            selectionStart = i + typeFrameStart + 1;
                        }
                    }

                    if(!cursorFound){
                        if(mousePos.x < displayText.back().findCharacterPos(i).x + charDist/2){
                            cursorPos = i + typeFrameStart;
                            cursorFound = true;
                        }
                        else if(i == typeString.size() - 1){
                            cursorPos = i + typeFrameStart + 1;
                        }
                    }
                }
            }

            slideCursor = false;
            cursorChanged = true;
        }
        else if(bounds.contains(event.LMBpressPosition) && event.captureMouse()){ // Dragging off the boundaries
            if(textFitType == CV_TEXT_FIT_LATERAL){
                if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                    if(mousePos.y < bounds.top){
                        if(cursorPos < typeFrameEnd) ++cursorPos; // Shift right
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                    else{
                        if(cursorPos > typeFrameStart) --cursorPos; // Shift left
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                }
                else if(textInfo.alignment == ALIGN_VERTICAL){
                    if(mousePos.y < bounds.top){
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else{
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
                else{
                    if(mousePos.x < bounds.left){
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else{
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
            }
        }
        else{
            bHasFocus = false;
            selectionStart = UINT_MAX;
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        bHasFocus = false;
        selectionStart = UINT_MAX;
    }

    if(bHasFocus){
        if(event.keyLog.size() > 0) cursorChanged = true;
        for(auto& key : event.keyLog){
            if((key > 31) && (key < 127)){ // ASCII non-white space
                if(ctrlPressed()){
                    if(key == 'c'){
                        std::string tmp;
                        if(selectionStart < cursorPos) tmp.assign(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                        else if(selectionStart > cursorPos) tmp.assign(typeString.begin() + cursorPos, typeString.begin() + selectionStart);

                        copyToClipboard(tmp);
                    }
                    if(key == 'v'){
                        std::string tmp = getClipboardText();

                        if(tmp.size() > 0){
                            unsigned int cShift = tmp.size();

                            if(selectionStart != UINT_MAX){
                                if(selectionStart < cursorPos){
                                    cShift -= (cursorPos - selectionStart);
                                    typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                                }
                                else if(selectionStart > cursorPos){
                                    cShift -= (selectionStart - cursorPos);
                                    typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                                    cursorPos = selectionStart;
                                }
                                selectionStart = UINT_MAX;
                            }

                            if(cursorPos > typeString.size()) cursorPos = typeString.size();
                            for(auto& c : tmp){
                                if(c == '\n') c = ' ';
                            }

                            typeString.insert(cursorPos, tmp);
                            cursorPos += cShift;
                            typeFrameEnd += cShift;
                        }
                    }
                    if(key == 'a'){
                        selectionStart = 0;
                        cursorPos = typeFrameEnd;
                    }
                }
                else{

                    if(cursorPos > typeString.size()) cursorPos = typeString.size();
                    if((selectionStart != UINT_MAX) && (selectionStart > typeString.size())) selectionStart = typeString.size();

                    if(selectionStart != UINT_MAX){
                        if(selectionStart < cursorPos){
                            typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                            typeFrameEnd -= (cursorPos - selectionStart);
                            cursorPos = selectionStart;
                        }
                        else if(selectionStart > cursorPos){
                            typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                            typeFrameEnd -= (selectionStart - cursorPos);
                        }
                        selectionStart = UINT_MAX;
                    }

                    typeString.insert(typeString.begin() + cursorPos, key);
                    ++cursorPos;
                    ++typeFrameEnd;
                }
            }
            else if((key == 8) && (typeString.size() > 0)){ // Backspace
                if(selectionStart != UINT_MAX){
                    if(selectionStart < cursorPos){
                        typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                        cursorPos = selectionStart;
                    }
                    else if(selectionStart > cursorPos){
                        typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                    }
                    selectionStart = UINT_MAX;
                }
                else if(cursorPos > 0){
                    --cursorPos;
                    if(typeString.size() > typeFrameEnd - typeFrameStart){
                        --typeFrameEnd;
                    }
                    typeString.erase(typeString.begin() + cursorPos);
                }
            }
            else if(enterLine && (key == 10)){ // Return key
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)){
                        typeString += '\n';
                   }
                else{
                    enterString();
                    logTextIndex = 0;
                    if(textLog != nullptr) sendEnteredString();
                }
            }
            else if((key == 127) && (typeString.size() > 0)){ // Delete
                if(selectionStart != UINT_MAX){
                    if(selectionStart < cursorPos){
                        typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                        cursorPos = selectionStart;
                    }
                    else if(selectionStart > cursorPos){
                        typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                    }
                    selectionStart = UINT_MAX;
                }
                else if(cursorPos < typeString.size()){
                    --typeFrameEnd;
                    typeString.erase(typeString.begin() + cursorPos);
                }
            }
            else if(key == CV_KEY_LEFT){
                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart = UINT_MAX) selectionStart = cursorPos;

                if(cursorPos > typeFrameStart) --cursorPos;
                else if(typeFrameStart > 0){
                    --cursorPos;
                    --typeFrameStart;
                    --typeFrameEnd;
                }
            }
            else if(key == CV_KEY_RIGHT){
                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart = UINT_MAX) selectionStart = cursorPos;

                if(cursorPos < typeFrameEnd) ++cursorPos;
                else if(typeFrameEnd < typeString.size()){
                    ++cursorPos;
                    ++typeFrameEnd;
                    ++typeFrameStart;
                }
            }

            if(has_text_target() && (textFitType == CV_TEXT_FIT_LATERAL)){
                if(key == CV_KEY_UP){
                    ++logTextIndex;
                    getTextLogString(logTextIndex-1);
                }
                else if(key == CV_KEY_DOWN){
                    if(logTextIndex > 0){
                        --logTextIndex;
                        getTextLogString(logTextIndex-1);
                    }
                }
            }

        }
        event.keyLog.clear(); // Capture all key strokes

        if(cursorChanged){
            bBlink = false;
            timeLastBlink = 0.0f;
        }

        // Animate

        switch(animType){
            case CV_CURSOR_ANIM_FADE:{
                if(timeLastBlink >= blinkFreq){
                    bBlink = !bBlink;
                    timeLastBlink = 0.0f;
                }
                else{
                    sf::Color newFill = cursor.getFillColor();
                    if(bBlink){
                        newFill.a = 255_BIT*(timeLastBlink/blinkFreq);
                    }
                    else{
                        newFill.a = 255_BIT*(1.0f-timeLastBlink/blinkFreq);
                    }
                    cursor.setFillColor(newFill);
                }
                break;
            }
            default:{ // Blink
                if(timeLastBlink >= blinkFreq){
                    bBlink = !bBlink;
                    timeLastBlink = 0.0f;
                }
                else{
                    sf::Color newFill = cursor.getFillColor();
                    if(bBlink){
                        newFill.a = 0_BIT;
                    }
                    else{
                        newFill.a = 255_BIT;
                    }
                    cursor.setFillColor(newFill);
                }
                break;
            }
        }

        timeLastBlink += event.lastFrameTime;

    }

    if(bHasFocus || (typeString.size() > 0)){

        displayText.back().setString(typeString);

        if(textFitType == CV_TEXT_FIT_LATERAL){ // Fit laterally only
            std::string typeFrameStr;

            if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding){
                if(typeFrameStart < typeFrameEnd){
                    typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                    typeString.begin() + typeFrameEnd); // Assign frames and check again
                }
                displayText.back().setString(typeFrameStr);

                if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding){
                    if(cursorPos >= typeFrameStr.size() + typeFrameStart){
                        ++typeFrameStart;
                    }
                    else{
                        --typeFrameEnd;
                    }
                }
            }
            else{
                typeFrameStart = 0;
                typeFrameEnd = typeString.size();
            }

            if(typeFrameStart < typeFrameEnd){
                typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                typeString.begin() + typeFrameEnd);
            }
            displayText.back().setString(typeFrameStr);
        }

        displayText.back().setFillColor(colorTheme.back());
    }
    else if(bkgString.size() > 0){
        displayText.back().setString(this->bkgString);
        sf::Color bkgColor = colorTheme.back();
        bkgColor.a = bkgColor.a > 100 ? bkgColor.a - 100 : 0;
        displayText.back().setFillColor(bkgColor);
    }

    switch(textInfo.alignment){
        case ALIGN_VERTICAL_INVERTED:{
            displayText.back().setOrigin(displayText.back().getLocalBounds().width/2,
                                         displayText.back().getLocalBounds().height/2);
            displayText.back().setPosition(panel.front().getPosition());
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER_TOP:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        default:{
            break;
        }
    }

    if(cursorChanged && bHasFocus){
        sf::Vector2f movePos(0.0f,0.0f);

        if((textInfo.alignment == ALIGN_VERTICAL_INVERTED) || (textInfo.alignment == ALIGN_VERTICAL)){
            if(cursorPos - typeFrameStart < typeString.size()){
                movePos.y = displayText.back().findCharacterPos(cursorPos - typeFrameStart).y + 1.0f;
                movePos.x = displayText.back().getPosition().x;
                View->anim_to(&cursor, movePos, getDistance(cursor.getPosition(), movePos)*20, CV_OBJ_ANIM_SLIDE);
            }
            else{
                movePos.y = displayText.back().getGlobalBounds().top - 1.0f;
                movePos.x = displayText.back().getPosition().x;
                View->anim_to(&cursor, movePos, getDistance(cursor.getPosition(), movePos)*20, CV_OBJ_ANIM_SLIDE);
            }
        }
        else{
            if(cursorPos - typeFrameStart < typeString.size()){
                movePos.x = displayText.back().findCharacterPos(cursorPos - typeFrameStart).x - 1.0f;
                movePos.y = displayText.back().getPosition().y;
                View->anim_to(&cursor, movePos, getDistance(cursor.getPosition(), movePos)*20, CV_OBJ_ANIM_SLIDE);
            }
            else{
                movePos.x = displayText.back().getPosition().x + displayText.back().getGlobalBounds().width + 1.0f;
                movePos.y = displayText.back().getPosition().y;
                View->anim_to(&cursor, movePos, getDistance(cursor.getPosition(), movePos)*20, CV_OBJ_ANIM_SLIDE);
            }
        }
//        cursor.setPosition(movePos);
    }

    if((selectionStart != UINT_MAX) && (selectionStart != cursorPos)){
        float selectionWidth = 0.0f;

        if(selectionStart == typeFrameEnd){
            if(textInfo.alignment == ALIGN_VERTICAL){
                selectionWidth = displayText.back().getPosition().y + displayText.back().getGlobalBounds().height
                 - cursor.getPosition().y;
            }
            else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                selectionWidth = cursor.getPosition().y -
                displayText.back().getPosition().y;
            }
            else{
                selectionWidth = displayText.back().getPosition().x + displayText.back().getGlobalBounds().width
                 - cursor.getPosition().x;
            }
        }
        else{
            if(selectionStart < typeFrameStart){
                if(textInfo.alignment == ALIGN_VERTICAL){
                    selectionWidth = displayText.back().findCharacterPos(typeFrameStart).y - cursor.getPosition().y;
                }
                else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                    selectionWidth = cursor.getPosition().y - displayText.back().findCharacterPos(typeFrameStart).y;
                }
                else{
                    selectionWidth = displayText.back().findCharacterPos(typeFrameStart).x - cursor.getPosition().x;
                }
            }
            else{
                if(textInfo.alignment == ALIGN_VERTICAL){
                    selectionWidth = displayText.back().findCharacterPos(selectionStart - typeFrameStart).y - cursor.getPosition().y;
                }
                else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                    selectionWidth = cursor.getPosition().y - displayText.back().findCharacterPos(selectionStart - typeFrameStart).y;
                }
                else{
                    selectionWidth = displayText.back().findCharacterPos(selectionStart - typeFrameStart).x - cursor.getPosition().x;
                }
            }
        }

        if(textInfo.alignment == ALIGN_VERTICAL){
            panel.back().setSize(sf::Vector2f(panel.back().getSize().x, selectionWidth));
        }
        else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
            panel.back().setSize(sf::Vector2f(panel.back().getSize().x, -selectionWidth));
        }
        else{
            panel.back().setSize(sf::Vector2f(selectionWidth, panel.back().getSize().y));
        }

        panel.back().setPosition(cursor.getPosition());
    }
    else{
        panel.back().setSize(sf::Vector2f(0.0f, panel.back().getSize().y));
    }

    return true;
}

bool CVTypeBox::draw(){
    if(!CVTextBox::draw()) return false;
    if(bHasFocus){
        View->viewPort->draw(cursor);
    }
    return true;
}

CVTextLog::CVTextLog(view* View, const sf::Vector2f& position, const float& width, const float& height,
                     const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                     const float& borderWidth, const uint8_t& animType, const std::string& logFile,
                     CVTypeBox* userEntrySource):
                         CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
                         usrEntryBox(userEntrySource),
                         logFile(logFile),
                         selectedLogIndex(0),
                         timeWaiting(0.0f),
                         scrollOffsetY(0.0f),
                         msgPopupSpeed(600.0f),
                         msgScrollSpeed(3600.0f),
                         scrollBar(sf::Vector2f(width*0.02f, 0.0f)),
                         templateTheme(textInfo),
                         bClear(false){

    colorTheme.emplace_back(borderColor); // Message panel fill
    brighten(colorTheme.back(), 20_BIT);
    colorTheme.emplace_back(borderColor); // Message panel border
    colorTheme.emplace_back(getMsgPanelFillColor()); // Incoming message panel fill
    brighten(colorTheme.back(), 60_BIT);
    colorTheme.emplace_back(getMsgPanelFillColor()); // Highlight panel color
    darken(colorTheme.back(), 50_BIT);

    sf::Color scrollBarColor = getMsgPanelFillColor();
    scrollBarColor.a = 0_BIT;   // Invisible until scroll
    scrollBar.setFillColor(scrollBarColor);
    scrollBar.setPosition(sf::Vector2f(bounds.left + bounds.width*0.97f,
                                       bounds.top + bounds.height*0.025f));
    load();
}

bool CVTextLog::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(bClear){
        scrollOffsetY = 0.0f;
        textLog.clear();
        times.clear();
        msgPanels.clear();
        displayText.clear();
        textLogUserEntered.clear();
        save();
        bClear = false;
    }

    if(waitingText.size() > 0){

        while(waitingText.size() > 0){

            if(timeWaiting < updateAddDelay.front()){
                timeWaiting += event.lastFrameTime;
                break;
            }

            updateAddDelay.erase(updateAddDelay.begin());
            timeWaiting = 0.0f;

            times.addTimePoint();

            textLog.push_back(waitingText.front());
            textLogUserEntered.push_back(textLogUserWaiting.front());

            addTextEntry(textEntry(waitingText.front(),
                                   templateTheme.font,
                                   templateTheme.fontSize,
                                   templateTheme.alignment,
                                   templateTheme.textColor),
                          sf::Vector2f(bounds.left, bounds.top));

            sf::FloatRect textBounds = displayText.back().getGlobalBounds();

            if(textBounds.width > bounds.width*0.7f){
                std::string displayTextString = displayText.back().getString();

                for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
                    if(displayText.back().findCharacterPos(i).x - bounds.left > bounds.width*0.7f){
                        for(size_t j = i; j > 0; --j){
                            if(displayTextString[j-1] == ' '){
                                displayTextString[j-1] = '\n';
                                goto continueCheck;
                            }
                            else if((displayTextString[j-1] == ',') ||
                                    (displayTextString[j-1] == ',') ||
                                    (displayTextString[j-1] == '\\') ||
                                    (displayTextString[j-1] == '/') ||
                                    (displayTextString[j-1] == ':') ||
                                     (displayTextString[j-1] == '.') ||
                                    (displayTextString[j-1] == '_')){
                                displayTextString.insert(displayTextString.begin() + j-1, '\n');
                                goto continueCheck;
                            }
                        }
                        displayTextString.insert(displayTextString.begin() + i, '\n');

                        continueCheck:;
                        displayText.back().setString(displayTextString);
                        textBounds = displayText.back().getGlobalBounds();
                    }
                }
            }

            msgPanels.emplace_back(sf::Vector2f(textBounds.width + 24.0f, textBounds.height + 24.0f));
            msgPanels.back().setOutlineColor(getMsgPanelBorderColor());

            float moveBump = 0.0f, shiftDist = msgPanels.back().getSize().y + 6.0f + getTextCenterOffsetY(displayText.back())/2;
            if(textLogUserEntered.size() > 1){
                if((textLogUserEntered.back() & !textLogUserEntered[textLogUserEntered.size()-2]) ||
                   (!textLogUserEntered.back() & textLogUserEntered[textLogUserEntered.size()-2])){
                    moveBump = 12.0f;
                }
            }

            for(size_t i = 0; i < msgPanels.size() - 1; ++i){
                View->anim_move(&msgPanels[i],
                                sf::Vector2f(0.0f, -(shiftDist + moveBump)),
                                msgPopupSpeed);
                View->anim_move(&displayText[i],
                                sf::Vector2f(0.0f, -(shiftDist + moveBump)),
                                msgPopupSpeed);
            }

            if(textLogUserWaiting.front()){
                displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                            bounds.top + bounds.height + scrollOffsetY));
                msgPanels.back().setFillColor(getMsgPanelFillColor());
            }
            else{
                displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                            bounds.top + bounds.height + scrollOffsetY));
                msgPanels.back().setFillColor(getIncomingMsgPanelFillColor());
            }

            msgPanels.back().setPosition(displayText.back().getPosition().x - 12.0f,
                                                displayText.back().getPosition().y - 6.0f +
                                         getTextCenterOffsetY(displayText.back())/2);

            View->anim_move(&displayText.back(),
                              sf::Vector2f(0.0f,
                                           -(shiftDist + 12.0f)),
                              msgPopupSpeed);
            View->anim_move(&msgPanels.back(),
                          sf::Vector2f(0.0f,
                                       -(shiftDist + 12.0f)),
                          msgPopupSpeed);

            waitingText.erase(waitingText.begin());
            textLogUserWaiting.erase(textLogUserWaiting.begin());

            save();
        }
    }
    else{
        timeWaiting = 0.0f;
    }

    // Process scrolling

    float scrollRange = 0.0f;
    if(textLog.size() > 0){
        scrollRange = 24.0f + msgPanels.back().getPosition().y + msgPanels.back().getSize().y - msgPanels.front().getPosition().y
                    - bounds.height;

        if(scrollRange > 0.0f){
            scrollBar.setSize(sf::Vector2f(scrollBar.getSize().x,
                              0.95f*bounds.height*bounds.height/(scrollRange+bounds.height)));
            View->anim_to(&scrollBar, sf::Vector2f(bounds.left + bounds.width*0.97f,
                                  bounds.top + bounds.height*0.025f +
                                  ((bounds.height * 0.95f) - scrollBar.getSize().y)*
                                  (1.0f-(scrollOffsetY/(scrollRange)))), msgScrollSpeed*(bounds.height*0.95f-scrollBar.getSize().y)/scrollRange,
                          CV_OBJ_ANIM_SLIDE, true);
        }
    }

    if(event.viewHasFocus){

        bool fadeOut = true;
        float scrollDist = 0.0f, scrollSpeed = msgScrollSpeed;

        if(bounds.contains(mousePos) && (mousePos.x > bounds.left + bounds.width*0.9f)){

            if((scrollRange > 0.0f) && (event.LMBholdFrames > 5)){
                scrollSpeed *= 3;
                scrollDist = (scrollBar.getPosition().y + scrollBar.getSize().y/2 - mousePos.y)*event.lastFrameTime*20;
            }

            if(scrollBar.getFillColor().a < 255_BIT) View->anim_passive(&scrollBar, 1000.0f, CV_OBJ_ANIM_FADE_IN);
            fadeOut = false;
        }
        if(event.zoomDelta != 0.0f){
            scrollDist = 20.0f*event.zoomDelta;
        }

        if(scrollDist != 0.0f){

            if((scrollDist < 0.0f) && (scrollOffsetY > 0.0f)){
                if(scrollOffsetY + scrollDist < 0.0f) scrollDist = -scrollOffsetY;
                scrollOffsetY += scrollDist;
            }
            else if((scrollDist > 0.0f) && (scrollOffsetY < scrollRange)){
                if(scrollDist + scrollOffsetY > scrollRange) scrollDist = scrollRange - scrollOffsetY;
                scrollOffsetY += scrollDist;
            }
            else{
                scrollDist = 0.0f;
            }

            if(scrollBar.getFillColor().a < 255_BIT) View->anim_passive(&scrollBar, 1000.0f, CV_OBJ_ANIM_FADE_IN);

            if(scrollDist != 0.0f){
                for(size_t i = 0; i < msgPanels.size(); ++i){
                    View->anim_move(&msgPanels[i], sf::Vector2f(sf::Vector2f(0.0f, scrollDist)), msgScrollSpeed, CV_OBJ_ANIM_SLIDE);
                    View->anim_move(&displayText[i], sf::Vector2f(sf::Vector2f(0.0f, scrollDist)), msgScrollSpeed, CV_OBJ_ANIM_SLIDE);
                }
            }
            fadeOut = false;
        }

        if(fadeOut && (scrollBar.getFillColor().a > 0)){
            View->anim_passive(&scrollBar, 30.0f, CV_OBJ_ANIM_FADE_OUT);
        }
    }
    else{
        if(scrollBar.getFillColor().a > 0) View->anim_passive(&scrollBar, 30.0f, CV_OBJ_ANIM_FADE_OUT);
    }

    // Process clicking

    if(event.captureMouse() && (usrEntryBox != nullptr) && event.LMBhold && (event.LMBholdFrames < 5)){

        unsigned int index = 0, textIndex;
        std::string checkString;
        float xCtr;
        sf::FloatRect panelBounds;

        for(auto& item : msgPanels){
            panelBounds = item.getGlobalBounds();
            xCtr = item.getPosition().x + panelBounds.width/2;
            if(panelBounds.contains(mousePos)){
                if(xCtr > bounds.left + bounds.width/2 - 2.0f){
                    item.setFillColor(getHighlightPanelFillColor());
                    usrEntryBox->setTypeString(displayText[index].getString());
                    selectedLogIndex = index;
                }
                else if(xCtr < bounds.left + bounds.width/2 + 2.0f){
                    item.setFillColor(getHighlightPanelFillColor());
                    selectedLogIndex = index;
                }
            }
            else{
                if(xCtr < bounds.left + bounds.width/2 - 2.0f){
                    item.setFillColor(getIncomingMsgPanelFillColor());
                }
                else if(xCtr > bounds.left + bounds.width/2 + 2.0f){
                    item.setFillColor(getMsgPanelFillColor());
                }
            }
            ++index;
        }
    }

    for(auto key : event.keyLog){
        switch(key){
            case char(10):{
                if((usrEntryBox != nullptr) && usrEntryBox->canEnterLine()){
                    usrEntryBox->enterString();
                    usrEntryBox->sendEnteredString();
                }
                break;
            }
            case 'c':{
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)){
                    copyToClipboard(displayText[selectedLogIndex].getString());
                }
                break;
            }
            default: break;
        }
    }

    return true;

}

bool CVTextLog::save(){

    if(logFile.size() < 1) return false;

    if(!access(logFile.c_str(), X_OK)){
        if(access(logFile.c_str(), W_OK)) return false;
    }

    FILE* outFILE = fopen(logFile.c_str(), "wb");

    size_t L = textLog.size(), sSIZE;
    fwrite(&L, 1, sizeof(unsigned int), outFILE);
    bool userEntered;

    for(size_t i = 0; i < L; ++i){
        sSIZE = textLog[i].size();
        fwrite(&sSIZE, 1, sizeof(unsigned int), outFILE);
        fwrite(textLog[i].c_str(), sizeof(char), textLog[i].size(), outFILE);
        fwrite(times[i].data(), sizeof(unsigned int), 6, outFILE);

        userEntered = textLogUserEntered[i];
        fwrite(&userEntered, 1, sizeof(bool), outFILE);
    }

    fclose(outFILE);

    return true;
}

bool CVTextLog::load(){

    if(logFile.size() < 1) return false;

    if(access(logFile.c_str(), X_OK | R_OK)){
        return false;
    }

    msgPanels.clear();
    displayText.clear();

    FILE* inFILE = fopen(logFile.c_str(), "rb");

    size_t L, cSIZE;
    fread(&L, 1, sizeof(unsigned int), inFILE);
    int newTime [] = {0,0,0,0,0,0};
    bool userEntered;

    textLog.clear();                textLog.reserve(L);
    textLogUserEntered.clear();     textLogUserEntered.reserve(L);
    times.clear();                  times.timeInfo.reserve(L);

    for(size_t i = 0; i < L; ++i){
        fread(&cSIZE, 1, sizeof(unsigned int), inFILE);
        char* newStr = new char[cSIZE+1];
        newStr[cSIZE] = '\0';
        fread(newStr, cSIZE, sizeof(char), inFILE);
        textLog.emplace_back();
        textLog.back().assign(newStr, newStr + cSIZE);

        fread(newTime, 6, sizeof(unsigned int), inFILE);
        times.emplace_back(newTime);

        fread(&userEntered, 1, sizeof(bool), inFILE);
        textLogUserEntered.push_back(userEntered);
    }

    fclose(inFILE);

    int date = 0, newDate = 0;
    unsigned int loadLength = L < 20 ? L : 20; // Load the first 20, then require user scroll

    for(size_t N = 0; N < loadLength; ++N){

        newDate = times[N].year() * times[N].month() * times[N].day();

        if(date != newDate){ // Show the date when days change
            sf::FloatRect textBounds;

            std::string dateString;
            TimePoint time_now;

            if(newDate == time_now.year() * time_now.month() * time_now.day()) dateString = "Today";
            else dateString = times[N].getDate();

            addTextEntry(textEntry(dateString,
                                templateTheme.font,
                               templateTheme.fontSize,
                               ALIGN_CENTER_TOP,
                            getMsgPanelFillColor()),
                         sf::Vector2f(bounds.left + bounds.width/2, bounds.top + bounds.height - 48.0f));
            textBounds = displayText.back().getGlobalBounds();
            sf::Color dateColor = getMsgPanelFillColor();
            dateColor.a = 60;
            msgPanels.emplace_back(sf::Vector2f(textBounds.width + 48.0f,
                                           textBounds.height + 24.0f));
            msgPanels.back().setPosition(sf::Vector2f(displayText.back().getPosition().x - 24.0f,
                                                      displayText.back().getPosition().y - 6.0f
                                                      + getTextCenterOffsetY(displayText.back())/2));
            msgPanels.back().setFillColor(dateColor);

            for(size_t i = 0; i < msgPanels.size() - 1; ++i){
                msgPanels[i].move(sf::Vector2f(0.0f, -(msgPanels.back().getSize().y + 18.0f)));
                displayText[i].move(sf::Vector2f(0.0f, -(msgPanels.back().getSize().y + 18.0f)));
            }

            date = newDate;
        }

        addTextEntry(textEntry(textLog[N],
                               templateTheme.font,
                               templateTheme.fontSize,
                               templateTheme.alignment,
                               templateTheme.textColor),
                      sf::Vector2f(bounds.left, bounds.top));

        sf::FloatRect textBounds = displayText.back().getGlobalBounds();

        if(textBounds.width > bounds.width*0.7f){
            std::string displayTextString = displayText.back().getString();

            for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
                if(displayText.back().findCharacterPos(i).x - bounds.left > bounds.width*0.7f){
                    for(size_t j = i; j > 0; --j){
                        if(displayTextString[j-1] == ' '){
                            displayTextString[j-1] = '\n';
                            goto continueCheck;
                        }
                        else if((displayTextString[j-1] == ',') ||
                                (displayTextString[j-1] == ',') ||
                                (displayTextString[j-1] == '\\') ||
                                (displayTextString[j-1] == '/') ||
                                (displayTextString[j-1] == ':') ||
                                 (displayTextString[j-1] == '.') ||
                                (displayTextString[j-1] == '_')){
                            displayTextString.insert(displayTextString.begin() + j-1, '\n');
                            goto continueCheck;
                        }
                    }
                    displayTextString.insert(displayTextString.begin() + i, '\n');

                    continueCheck:;
                    displayText.back().setString(displayTextString);
                    textBounds = displayText.back().getGlobalBounds();
                }
            }
        }

        msgPanels.emplace_back(sf::Vector2f(textBounds.width + 24.0f, textBounds.height + 24.0f));
        msgPanels.back().setOutlineColor(getMsgPanelBorderColor());

        float moveBump = 0.0f, shiftDist = msgPanels.back().getSize().y + 6.0f + getTextCenterOffsetY(displayText.back())/2;
        if(N > 0){
            if((textLogUserEntered[N-1] & !textLogUserEntered[N]) ||
               (!textLogUserEntered[N-1] & textLogUserEntered[N])){
                moveBump = 12.0f;
            }
        }

        for(size_t i = 0; i < msgPanels.size() - 1; ++i){
            msgPanels[i].move(sf::Vector2f(0.0f, -(shiftDist + moveBump)));
            displayText[i].move(sf::Vector2f(0.0f, -(shiftDist + moveBump)));
        }

        if(textLogUserEntered[N]){
            displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                        bounds.top + bounds.height + scrollOffsetY));
            msgPanels.back().setFillColor(getMsgPanelFillColor());
        }
        else{
            displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                        bounds.top + bounds.height + scrollOffsetY));
            msgPanels.back().setFillColor(getIncomingMsgPanelFillColor());
        }

        msgPanels.back().setPosition(displayText.back().getPosition().x - 12.0f,
                                            displayText.back().getPosition().y - 6.0f +
                                     + getTextCenterOffsetY(displayText.back())/2);

        displayText.back().move(
                          sf::Vector2f(0.0f,
                                       -(shiftDist + 12.0f)));
        msgPanels.back().move(
                      sf::Vector2f(0.0f,
                                   -(shiftDist + 12.0f)));

    }

    if(textLog.size() > 0){
        float scrollRange = 24.0f + msgPanels.back().getPosition().y + msgPanels.back().getSize().y - msgPanels.front().getPosition().y
                    - bounds.height;

        if(scrollRange > 0.0f){
            scrollBar.setSize(sf::Vector2f(scrollBar.getSize().x,
                              0.95f*bounds.height*bounds.height/(scrollRange+bounds.height)));
            scrollBar.setPosition(sf::Vector2f(bounds.left + bounds.width*0.97f,
                                  bounds.top + bounds.height*0.975f - scrollBar.getSize().y));
        }
    }

    return true;

}

bool CVTextLog::draw(){
    if(View->viewPort == nullptr) return false;
    if(!visible) return false;

    for(auto& item : panel){
        View->viewPort->draw(item);
    }
    for(auto& sprite : spriteList){
        View->viewPort->draw(sprite);
    }
    for(auto& panel : msgPanels){
        View->viewPort->draw(panel);
    }
    for(auto& text : displayText){
        View->viewPort->draw(text);
    }
    for(auto& item : mask){
        View->viewPort->draw(item);
    }
    View->viewPort->draw(scrollBar);

    return true;
}

}
