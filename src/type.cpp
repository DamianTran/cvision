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

#include <cvision/type.hpp>
#include <cvision/event.hpp>
#include <cvision/view.hpp>

#include <cvision/widgets.hpp>

#include "hyper/toolkit/string.hpp"

using namespace hyperC;
using namespace std;

namespace cvis
{

CVTextLogMsg::CVTextLogMsg(CVView* View, const TextEntry& newEntry):
    CVTextBox(View, sf::Vector2f(0.0f,0.0f), 2.0f, 2.0f, newEntry,
              sf::Color::Transparent, sf::Color::Transparent, 0.0f),
                  bUser(false),
                  bMsg(true),
                  textLogIndex(0)
{

}

CVTypeBox::CVTypeBox(CVView* View, const sf::Vector2f& position, const float width, const float height,
                     const TextEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                     const float borderWidth, const uint8_t& animType, const uint8_t& textFitType,
                     const uint8_t& expandType, const float& expandRate):
    CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
    bkgString(UTF8_to_UTF16(textInfo.text)),
    timeLastBlink(0.0f),
    blinkFreq(1.0f),
    expandRate(expandRate),
    bBlink(false),
    bTypeStringChanged(false),
    bAcceptSuggestions(true),
    bCanEdit(true),
    bModified(false),
    bEncode(false),
    expandType(expandType),
    initBounds(bounds),
    animType(animType),
    textFitType(textFitType),
    cursorPos(0),
    typeFrameStart(0),
    typeFrameEnd(0),
    logTextIndex(0),
    selectionStart(UINT_MAX),
    selectionLineStart(0),
    selectionLineEnd(0),
    bEnterLine(false),
    bClearOnEnterLine(true),
    bUserTyped(false),
    textLog(nullptr),
    jumpTarget(nullptr)
{

    this->textInfo = textInfo;
    addTextEntry(textInfo);
    setTextTemplate(textInfo);

    sf::FloatRect textBounds;

    textBounds = displayText.back().getGlobalBounds();

    xPadding = bounds.height < bounds.width ? bounds.height/2 : bounds.width/2;

    displayText.back().setPosition(bounds.left + xPadding, bounds.top + xPadding);

    float cursorOrigin = 0.0f;

    switch(textInfo.alignment)
    {
    case ALIGN_RIGHT:
    {
        displayText.back().move(bounds.width - xPadding - textBounds.width - borderWidth, 0.0f);
        break;
    }
    case ALIGN_RIGHT_MIDLINE:
    {
        displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2 - xPadding,
                                       bounds.top +
                                      bounds.height/2 -
                                      getTextCenterOffsetY(displayText.back()));
        break;
    }
    case ALIGN_CENTER:
    {
        displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2, displayText.back().getPosition().y);
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                       bounds.top +
                                      bounds.height/2 -
                                      getTextCenterOffsetY(displayText.back()));
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                       displayText.back().getPosition().y);
        break;
    }
    case ALIGN_VERTICAL_INVERTED:
    {

        panel.front().setOrigin(bounds.width/2, bounds.height/2);
        panel.front().setRotation(-90);
        panel.front().setPosition(position);
        bounds = panel.front().getGlobalBounds();
        setOrigin(sf::Vector2f(bounds.width, bounds.height/2));

        displayText.back().setOrigin(textBounds.width/2, textBounds.height/2);
        displayText.back().setPosition(panel.front().getPosition());
        displayText.back().setRotation(-90);

        break;
    }
    case ALIGN_LEFT_MIDLINE:
    {
        displayText.back().setPosition(bounds.left + xPadding,
                                      bounds.top +
                                      bounds.height/2 -
                                      getTextCenterOffsetY(displayText.back()));
        break;
    }
    default:  // Left-aligned
    {
        break;
    }
    }

    cursorOrigin = cursor.getSize().y/2;
    textBounds = displayText.back().getGlobalBounds();
    cursor.setSize(sf::Vector2f(2.0f,
                                displayText.back().getFont()->getGlyph(65, textInfo.fontSize, false, 0).bounds.height*2));

    sf::Vector2f cursorInitPos(displayText.back().getPosition());

    if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
    {
        cursor.setOrigin(0.0f, cursorOrigin);
        cursor.setRotation(-90);

        cursorInitPos.x = displayText.back().getPosition().x +
                            displayText.back().getGlobalBounds().width -
                            getTextCenterOffsetY(displayText.back())/2;
    }
    else if(textInfo.alignment == ALIGN_VERTICAL)
    {
        cursor.setOrigin(0.0f, cursorOrigin);
        cursor.setRotation(90);

        cursorInitPos.x = displayText.back().getPosition().x +
                            displayText.back().getGlobalBounds().width -
                            getTextCenterOffsetY(displayText.back())/2;
    }
    else
    {
        cursor.setOrigin(0.0f, cursorOrigin);

        cursorInitPos.y = displayText.back().getPosition().y +
                            displayText.back().getGlobalBounds().height -
                            getTextCenterOffsetY(displayText.back())/2;
    }

    cursor.setPosition(cursorInitPos);

    if((borderColor != fillColor) && (borderColor != sf::Color::Transparent)) cursor.setFillColor(borderColor);
    else cursor.setFillColor(textInfo.textColor);

}

CVTypeBox::~CVTypeBox()
{

    View->stopAnim(&cursor);

}

void CVTypeBox::setBackgroundString(const std::string& newString)
{
    bkgString = UTF8_to_UTF16(newString);
}

void CVTypeBox::setTypeString(const string& newString)
{
   setTypeString(UTF8_to_UTF16(newString));
}

void CVTypeBox::setTypeString(wstring newString)
{

    if(textFitType == CV_TEXT_FIT_WRAP)
    {

        if(!bEncode)
        {
            displayText.back().setString(newString);
        }
        else
        {
            displayText.back().setString(string(newString.size(), '*'));
        }

        cvis::wrapText(displayText.back(), bounds, textPadding);
        alignText();

        updateBounds();

    }
    else if(textFitType == CV_TEXT_FIT_LATERAL)
    {
        for(size_t i = 0; i < newString.size();)
        {
            if(newString[i] == '\n')
            {
                bool nextChar = true;
                while((i < newString.size() - 1) && (newString[i+1] == '\n'))
                {
                    newString.erase(newString[i+1]);
                    nextChar = false;
                }
                newString[i] = ' ';
                if(nextChar) ++i;
            }
            else ++i;
        }
    }
    else
    {
        displayText.back().setString(newString);
    }


    this->typeString = newString;
    selectionStart = UINT_MAX;
    typeFrameStart = 0;
    typeFrameEnd = this->typeString.size();
    bTypeStringChanged = true;
    bUserTyped = true;

    if(!this->typeString.empty())
    {
        cursorPos = this->typeString.size()-1;
        cursor.setPosition(displayText.front().findCharacterPos(cursorPos));
    }
    else
    {
        cursorPos = 0;
        cursor.setPosition(bounds.left + textPadding,
                           bounds.top + bounds.height/2);
    }

}

std::string CVTypeBox::getTypeString() const
{
    return UTF16_to_UTF8(typeString);
}

void CVTypeBox::make_suggestion(const string& text)
{
    suggested = text;
}

void CVTypeBox::clear_suggestion()
{
    suggested.clear();
}

const bool& CVTypeBox::can_suggest() const
{
    return bAcceptSuggestions;
}

string CVTypeBox::string_at_cursor() const
{
    string output;
    if(typeString.empty() || (cursorPos > typeString.size())) return output;

    int i = cursorPos - 1;
    while((i >= 0) && !isCharType(typeString[i], DELIM_BASIC))
    {
        output.insert(output.begin(), typeString[i]);
        --i;
    }
    i = cursorPos;
    while((i < typeString.size()) && !isCharType(typeString[i], DELIM_BASIC))
    {
        output += typeString[i];
        ++i;
    }
    return output;
}

bool CVTypeBox::cursor_at_word_end() const
{
    if(typeString.empty() ||
            (cursorPos > typeString.size())
            || (cursorPos == 0)) return false;
    if(!isCharType(typeString[cursorPos-1], DELIM_BASIC))
    {
        if((cursorPos == typeString.size()) ||
                isCharType(typeString[cursorPos], DELIM_BASIC)) return true;
    }
    return false;
}

void CVTypeBox::setColor(const sf::Color& newColor)
{

    panel.front().setFillColor(newColor);
    for(auto& spr : spriteList)
    {
        spr.setColor(newColor);
    }

}

void CVTypeBox::enterString()
{
    enteredString = UTF16_to_UTF8(typeString);
    selectionStart = UINT_MAX;

    if(bClearOnEnterLine)
    {
        typeString.clear();
        bTypeStringChanged = true;
        bUserTyped = true;
        cursorPos = 0;
    }
}

void CVTypeBox::setFillColor(const sf::Color& newColor)
{
    panel.front().setFillColor(newColor);
}

bool CVTypeBox::cursor_at_word_begin() const
{
    if(typeString.empty() ||
            (cursorPos > typeString.size())) return false;
    if(!isCharType(typeString[cursorPos], DELIM_BASIC))
    {
        if((cursorPos == 0) || isCharType(typeString[cursorPos - 1], DELIM_BASIC)) return true;
    }
    return false;
}

unsigned int CVTypeBox::cursor_string_position() const
{
    int i = cursorPos;
    while((i > 0) && !isCharType(typeString[i-1], DELIM_BASIC))
    {
        --i;
    }
    return cursorPos - i;
}

bool CVTypeBox::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVElement::update(event, mousePos)) return false;

    bool slideCursor = true;
    bool cursorChanged = false;
    bool mouseCaptured = false;

    unsigned int rangeBegin = 0;
    unsigned int rangeEnd = displayText.back().getString().getSize();

    if(!View->cursor_overriden() && event.focusFree() && bounds.contains(mousePos))
    {

        if(cmpStringToList(getTypeString(), { "http", "www.", ".com", ".net", ".org" }))
        {
            event.setCursor(sf::Cursor::Hand);

            if(event.LMBreleased &&
               (event.LMBreleaseFrames == 1) &&
               (event.LMBholdTime < 0.15f) &&
               bounds.contains(event.LMBpressPosition))
            {
                vector<string> parse;

                splitString(getTypeString(), parse, " ");

                for(auto& term : parse)
                {

                    if(cmpStringToList(term, { "http", "www.", ".com", ".net", ".org" }))
                    {
                        stringstream ss;
                        ss << "open " << term;
                        system(ss.str().c_str());
                    }

                }
            }

        }
        else
        {
            event.setCursor(sf::Cursor::Text);
        }
    }

    if(highlightable())
    {
        if((event.focusFree() && bounds.contains(event.lastFrameMousePosition)) || highlighted)
        {
            sf::Color tmp;
            for(auto& item : panel)
            {
                tmp = highlightColor;
                tmp.a = baseFillColor().a;
                item.setFillColor(tmp);
            }
        }
        else
        {
            for(auto& item : panel)
            {
                item.setFillColor(baseFillColor());
            }
        }
    }

    if(bFade)
    {
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());

        if(!bNoFill && !bSpriteOnly)
        {

            if(fadeLayers & CV_LAYER_FILL)
            {

                tmp = panel.front().getFillColor();
                if(targetAlpha > tmp.a)
                {
                    if(tmp.a + adjusted_fr < targetAlpha)
                    {
                        tmp.a += adjusted_fr;
                    }
                    else tmp.a = targetAlpha;
                }
                else
                {
                    if(tmp.a - adjusted_fr > targetAlpha)
                    {
                        tmp.a -= adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
                    }
                }
                panel.front().setFillColor(tmp);

            }

            if(fadeLayers & CV_LAYER_OUTLINE)
            {

                tmp = panel.front().getOutlineColor();
                if(targetAlpha > tmp.a)
                {
                    if(tmp.a + adjusted_fr < targetAlpha)
                    {
                        tmp.a += adjusted_fr;
                    }
                    else tmp.a = targetAlpha;
                }
                else
                {
                    if(tmp.a - adjusted_fr > targetAlpha)
                    {
                        tmp.a -= adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
                    }
                }
                panel.front().setOutlineColor(tmp);

            }

        }

        if(fadeLayers & CV_LAYER_TEXT)
        {

            for(auto& text : displayText)
            {
                tmp = text.getFillColor();
                if(targetAlpha > tmp.a)
                {
                    if(tmp.a + adjusted_fr < targetAlpha)
                    {
                        tmp.a += adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
                        bFade = false;
                    }
                }
                else
                {
                    if(tmp.a - adjusted_fr > targetAlpha)
                    {
                        tmp.a -= adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
                    }
                }
                text.setFillColor(tmp);
            }

        }
    }

    if(event.LMBhold && bounds.contains(event.LMBpressPosition))  // Handle click and drag
    {
        if((bounds.contains(mousePos) || displayText.back().getGlobalBounds().contains(mousePos)) && event.captureFocus() && event.captureMouse())
        {
            if(!hasFocus())
            {
                timeLastBlink = 0.0f;
                bBlink = false;
            }
            mouseCaptured = true;
            setFocus(true);
            size_t i = 0, S = displayText.back().getString().getSize();
            float charDist = 0.0f;
            bool cursorFound = false,
                 selectionFound = false;

            if(!typeString.empty() && ((event.LMBholdTime < 0.2f) || (mousePos != event.lastFrameMousePosition)))
            {
                if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                {
                    if(textFitType != CV_TEXT_FIT_LATERAL)
                    {
                        for(size_t j = 0; i < S; i += 100)
                        {
                            if(!selectionFound)
                            {
                                if(!cursorFound)
                                {
                                    if(event.LMBpressPosition.x < displayText.back().findCharacterPos(i).x)
                                    {
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        selectionFound = true;
                                    }
                                }
                                else
                                {
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(event.LMBpressPosition.x < displayText.back().findCharacterPos(j).x)
                                    {
                                        rangeEnd = j;
                                        selectionFound = true;
                                    }
                                }
                            }
                            if(!cursorFound)
                            {
                                if(!selectionFound)
                                {
                                    if(mousePos.x < displayText.back().findCharacterPos(i).x)
                                    {
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        cursorFound = true;
                                    }
                                }
                                else
                                {
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(mousePos.x < displayText.back().findCharacterPos(j).x)
                                    {
                                        rangeEnd = j;
                                        cursorFound = true;
                                    }
                                }
                            }

                            if(selectionFound && cursorFound) break;
                        }

                        if(!selectionFound || !cursorFound) rangeBegin = rangeEnd > 200 ? rangeEnd - 200 : 0;

                        cursorFound = false;
                        selectionFound = false;
                    }

                    for(i = rangeBegin; i < rangeEnd; ++i)
                    {
                        if(i == typeString.size() - 1)
                        {
                            charDist = displayText.back().getPosition().y -
                                       displayText.back().findCharacterPos(i).y;
                        }
                        else charDist = displayText.back().findCharacterPos(i).y - displayText.back().findCharacterPos(i+1).y;

                        if((event.LMBholdTime < 0.2f) && !selectionFound)
                        {
                            if(event.LMBpressPosition.y > displayText.back().findCharacterPos(i).y - charDist/2)
                            {
                                selectionStart = i + typeFrameStart;
                                selectionFound = true;
                            }
                            else if(i == typeString.size() - 1)
                            {
                                selectionStart = i + typeFrameStart + 1;
                            }
                        }
                        if(!cursorFound)
                        {
                            if(mousePos.y > displayText.back().findCharacterPos(i).y - charDist/2)
                            {
                                cursorPos = i + typeFrameStart;
                                cursorFound = true;
                            }
                            else if(i == typeString.size() - 1)
                            {
                                cursorPos = i + typeFrameStart + 1;
                            }
                        }

                        if(cursorFound && (selectionStart != UINT_MAX)) break;
                    }
                }
                else
                {
                    if(textFitType != CV_TEXT_FIT_LATERAL)
                    {
                        for(size_t j = 0; i < S; i += 100)
                        {
                            if(!selectionFound)
                            {
                                if(!cursorFound)
                                {
                                    if(event.LMBpressPosition.y < displayText.back().findCharacterPos(i).y)
                                    {
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        selectionFound = true;
                                    }
                                }
                                else
                                {
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(event.LMBpressPosition.y < displayText.back().findCharacterPos(j).y)
                                    {
                                        rangeEnd = j;
                                        selectionFound = true;
                                    }
                                }
                            }
                            if(!cursorFound)
                            {
                                if(!selectionFound)
                                {
                                    if(mousePos.y < displayText.back().findCharacterPos(i).y)
                                    {
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        cursorFound = true;
                                    }
                                }
                                else
                                {
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(mousePos.y < displayText.back().findCharacterPos(j).y)
                                    {
                                        rangeEnd = j;
                                        cursorFound = true;
                                    }
                                }
                            }

                            if(selectionFound && cursorFound) break;
                        }

                        if(!selectionFound || !cursorFound) rangeBegin = rangeEnd > 200 ? rangeEnd - 200 : 0;

                        cursorFound = false;
                        selectionFound = false;
                    }

                    for(i = rangeBegin; i < rangeEnd; ++i)
                    {
                        if(i == typeString.size() - 1)
                        {
                            charDist = displayText.back().getGlobalBounds().width -
                                       displayText.back().findCharacterPos(i).x
                                       + displayText.back().getPosition().x;
                        }
                        else charDist = displayText.back().findCharacterPos(i+1).x - displayText.back().findCharacterPos(i).x;

                        if((event.LMBholdTime < 0.2f) && !selectionFound)
                        {
                            if((event.LMBpressPosition.x < displayText.back().findCharacterPos(i).x + charDist/2)
                                    && ((textFitType != CV_TEXT_FIT_WRAP) ||
                                        (event.LMBpressPosition.y < displayText.back().findCharacterPos(i).y +
                                         2*getTextCenterOffsetY(displayText.back()))))
                            {
                                selectionStart = i + typeFrameStart;
                                selectionFound = true;
                            }
                            else if(i == typeString.size() - 1)
                            {
                                selectionStart = i + typeFrameStart + 1;
                            }
                        }

                        if(!cursorFound)
                        {
                            if((mousePos.x < displayText.back().findCharacterPos(i).x + charDist/2)
                                    && ((textFitType != CV_TEXT_FIT_WRAP) ||
                                        (mousePos.y < displayText.back().findCharacterPos(i).y +
                                         2*getTextCenterOffsetY(displayText.back()))))
                            {
                                cursorPos = i + typeFrameStart;
                                cursorFound = true;
                            }
                            else if(i == typeString.size() - 1)
                            {
                                cursorPos = i + typeFrameStart + 1;
                            }
                        }

                        if(cursorFound && (selectionStart != UINT_MAX)) break;
                    }
                }

                slideCursor = false;
                cursorChanged = true;
            }
            else if(typeString.empty())
            {
                cursorPos = 0;
                typeFrameEnd = 0;
                typeFrameStart = 0;
            }
        }
        else if(bounds.contains(event.LMBpressPosition))  // Dragging off the boundaries
        {
            if(textFitType == CV_TEXT_FIT_LATERAL)
            {
                if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                {
                    if(mousePos.y < bounds.top)
                    {
                        if(cursorPos < typeFrameEnd) ++cursorPos; // Shift right
                        else if(typeFrameEnd < typeString.size())
                        {
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                    else
                    {
                        if(cursorPos > typeFrameStart) --cursorPos; // Shift left
                        else if(typeFrameStart > 0)
                        {
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                }
                else if(textInfo.alignment == ALIGN_VERTICAL)
                {
                    if(mousePos.y < bounds.top)
                    {
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart > 0)
                        {
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else
                    {
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size())
                        {
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
                else
                {
                    if(mousePos.x < bounds.left)
                    {
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart)
                        {
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else
                    {
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size())
                        {
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
            }
        }
        else
        {
            setFocus(false);
            selectionStart = UINT_MAX;
        }
    }
    else if(event.LMBhold || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        setFocus(false);
        selectionStart = UINT_MAX;
    }

    if(hasFocus())
    {
        if(bAcceptSuggestions &&
                (selectionStart == UINT_MAX) &&
                bCanEdit && cursor_at_word_end() && (suggested.size() > 0))  // Handle auto-fill
        {
            if(cursorPos > typeString.size()) cursorPos = typeString.size();
            unsigned int indexBegin = findString(suggested, string_at_cursor(),
                                                 CMP_STR_CASE_INSENSITIVE | CMP_STR_SIZE_INSENSITIVE),
                                      L = string_at_cursor().size();
            if((indexBegin != UINT_MAX) &&
                    (indexBegin + L < suggested.size()))
            {
                typeString.insert(typeString.begin() + cursorPos,
                                  suggested.begin() + indexBegin + L,
                                  suggested.end());
                selectionStart = cursorPos + suggested.size() - indexBegin - L;
                typeFrameEnd += suggested.size() - indexBegin - L;
                if(typeFrameStart > 0) typeFrameStart += suggested.size() - indexBegin - L;
                bAcceptSuggestions = false;
            }

            cursorChanged = true;
            bTypeStringChanged = true;
            bUserTyped = true;
        }

        for(auto& key : event.keyLog)
        {
            if(key == CV_KEY_BACKSPACE)
            {
                if(bCanEdit && (!typeString.empty()))  // Backspace
                {
                    if(selectionStart != UINT_MAX)
                    {
                        if(selectionStart < cursorPos)
                        {
                            if(cursorPos > typeString.size()) cursorPos = typeString.size();
                            typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                            cursorPos = selectionStart;
                        }
                        else if(selectionStart > cursorPos)
                        {
                            if(selectionStart > typeString.size()) selectionStart = typeString.size();
                            typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                        }
                        selectionStart = UINT_MAX;
                    }
                    else if(cursorPos > 0)
                    {
                        --cursorPos;
                        --typeFrameEnd;
                        typeString.erase(typeString.begin() + cursorPos);
                    }
                    clear_suggestion();
                    bAcceptSuggestions = false;
                }
            }
            else if(key == CV_KEY_DELETE)
            {
                if(bCanEdit && !typeString.empty())  // Delete
                {
                    if(selectionStart != UINT_MAX)
                    {
                        if(selectionStart < cursorPos)
                        {
                            if(cursorPos > typeString.size()) cursorPos = typeString.size();
                            typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                            cursorPos = selectionStart;
                        }
                        else if(selectionStart > cursorPos)
                        {
                            if(selectionStart > typeString.size()) selectionStart = typeString.size();
                            typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                        }
                        selectionStart = UINT_MAX;
                    }
                    else if(cursorPos < typeString.size())
                    {
                        if(typeFrameStart == 0) --typeFrameEnd;
                        else
                        {
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                        typeString.erase(typeString.begin() + cursorPos);
                    }
                    clear_suggestion();
                    bAcceptSuggestions = false;
                }
            }
            else if(key == CV_KEY_LEFT)
            {

                if((selectionStart != UINT_MAX) && (suggested.size() > 0))
                {
                    if(selectionStart > typeString.size()) selectionStart = typeString.size();
                    typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                }

                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart == UINT_MAX) selectionStart = cursorPos;

                if(cursorPos > typeFrameStart) --cursorPos;
                else if((textFitType == CV_TEXT_FIT_LATERAL) && (typeFrameStart > 0))
                {
                    --cursorPos;
                    --typeFrameStart;
                    --typeFrameEnd;
                }

                bAcceptSuggestions = false;
            }
            else if(key == CV_KEY_RIGHT)
            {

                if(!suggested.empty())
                {
                    cursorPos = selectionStart;
                }

                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart == UINT_MAX) selectionStart = cursorPos;

                if(cursorPos < typeFrameEnd) ++cursorPos;
                else if((textFitType == CV_TEXT_FIT_LATERAL) && (typeFrameEnd < typeString.size()))
                {
                    ++cursorPos;
                    ++typeFrameEnd;
                    ++typeFrameStart;
                }
                bAcceptSuggestions = false;
            }
            else
            {
                if(ctrlPressed())
                {
                    if(key == 'c')
                    {
                        wstring tmp;
                        if(selectionStart != UINT_MAX)
                        {
                            if(selectionStart < cursorPos) tmp.assign(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                            else if(selectionStart > cursorPos) tmp.assign(typeString.begin() + cursorPos, typeString.begin() + selectionStart);

                            if(selectionStart != cursorPos) copyToClipboard(UTF16_to_UTF8(tmp));
                        }
                    }
                    else if(bCanEdit && (key == 'v'))
                    {
                        string data = getClipboardText();
                        wstring tmp = UTF8_to_UTF16(data);

                        if(!tmp.empty())
                        {
                            if(textFitType != CV_TEXT_FIT_WRAP)
                            {
                                for(size_t i = 0; i < tmp.size();)
                                {
                                    if(tmp[i] == '\n')
                                    {
                                        if(((i > 0) && (tmp[i-1] == ' ')) ||
                                                ((i < tmp.size()-1) && (tmp[i+1] == ' '))) tmp.erase(tmp.begin() + i);
                                        else
                                        {
                                            tmp[i] = ' ';
                                            ++i;
                                        }
                                    }
                                    else ++i;
                                }
                            }

                            if(selectionStart != UINT_MAX)
                            {
                                if(selectionStart > typeString.size()) selectionStart = typeString.size();
                                if(cursorPos > typeString.size()) cursorPos = typeString.size();

                                if(selectionStart < cursorPos)
                                {
                                    typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                                    cursorPos = selectionStart;
                                }
                                else if(selectionStart > cursorPos)
                                {
                                    typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                                }
                                selectionStart = UINT_MAX;
                            }

                            typeString.insert(cursorPos, tmp);
                            cursorPos += tmp.size();
                            typeFrameEnd += tmp.size();

                            if(cursorPos > typeString.size()) cursorPos = typeString.size();
                            if(typeFrameEnd > typeString.size()) typeFrameEnd = typeString.size();
                        }
                    }
                    else if(key == 'a')
                    {
                        selectionStart = 0;
                        cursorPos = typeString.size();
                    }
                }
                else
                {
                    if(key == CV_KEY_RETURN)  // Return key
                    {
                        if(bCanEdit && !suggested.empty())
                        {
                            typeString.erase(typeString.begin() + cursorPos, typeString.end());
                            clear_suggestion();
                        }

                        if(bEnterLine)
                        {
                            if(((textFitType == CV_TEXT_FIT_WRAP) ||
                                    (textFitType == CV_TEXT_FIT_NONE)) && shiftPressed())
                            {
                                goto insertKey;
                            }
                            else
                            {
                                enterString();
                                logTextIndex = 0;

                                if(textLog)
                                {
                                    sendEnteredString();
                                }

                                goto skipInsertKey;
                            }
                        }
                        else if(textFitType == CV_TEXT_FIT_LATERAL)
                        {
                            sendTriggers();
                            goto skipInsertKey;
                        }
                    }
                    else if(key == CV_KEY_TAB)  // Tab key
                    {
                        if(jumpTarget)
                        {

                            setFocus(false);
                            event.keyLog.clear();
                            suggested.clear();

                            jumpTarget->setFocus(true);

                            return true;
                        }

                    }

                    insertKey:;

                    if(bCanEdit &&
                       (key != CV_KEY_UP) &&
                       (key != CV_KEY_DOWN))
                    {

                        bAcceptSuggestions = true;

                        if(cursorPos > typeString.size()) cursorPos = typeString.size();
                        if((selectionStart != UINT_MAX) && (selectionStart > typeString.size())) selectionStart = typeString.size();

                        if(selectionStart != UINT_MAX)
                        {
                            if(selectionStart < cursorPos)
                            {
                                typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                                typeFrameEnd -= (cursorPos - selectionStart);
                                cursorPos = selectionStart;
                            }
                            else if(selectionStart > cursorPos)
                            {
                                typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                                typeFrameEnd -= (selectionStart - cursorPos);
                            }
                            selectionStart = UINT_MAX;
                        }

                        typeString.insert(typeString.begin() + cursorPos, wchar_t(key));

                        ++cursorPos;
                        ++typeFrameEnd;

                    }

                    skipInsertKey:;
                }
            }

            if(has_text_target() && (textFitType == CV_TEXT_FIT_LATERAL))
            {
                if(key == CV_KEY_UP)
                {
                    ++logTextIndex;
                    getTextLogString(logTextIndex-1);
                }
                else if(key == CV_KEY_DOWN)
                {
                    if(logTextIndex > 0)
                    {
                        --logTextIndex;
                        getTextLogString(logTextIndex-1);
                    }
                }
            }
            else if(textFitType == CV_TEXT_FIT_WRAP)
            {
                if((key == CV_KEY_UP) && (cursorPos > 0))
                {
                    size_t i = cursorPos - 1;
                    wstring strBuff = displayText.back().getString();
                    while((i > 0) && (strBuff[i] != '\n')) --i;
                    while((i > 0) &&
                            (displayText.back().findCharacterPos(i).x >
                             displayText.back().findCharacterPos(cursorPos).x)) --i;
                    cursorPos = i;
                }
                else if(key == CV_KEY_DOWN)
                {
                    size_t i = cursorPos;
                    wstring strBuff = displayText.back().getString();
                    unsigned int L = strBuff.size();
                    while((i < L) && (strBuff[i] != '\n')) ++i;
                    ++i;
                    if(strBuff[i] != '\n')
                    {
                        while((i < L) && (strBuff[i] != '\n') &&
                                (displayText.back().findCharacterPos(i).x <
                                 displayText.back().findCharacterPos(cursorPos).x)) ++i;
                    }
                    cursorPos = i;
                }
            }
        }

        if(!event.keyLog.empty())
        {
            cursorChanged = true;
            bTypeStringChanged = true;
            bUserTyped = true;
            event.keyLog.clear(); // Capture all key strokes
            suggested.clear();
        }

        if(cursorChanged)
        {
            bBlink = false;
            timeLastBlink = 0.0f;
        }

        // Animate

        switch(animType)
        {
        case CV_CURSOR_ANIM_FADE:
        {
            if(timeLastBlink >= blinkFreq)
            {
                bBlink = !bBlink;
                timeLastBlink = 0.0f;
            }
            else
            {
                sf::Color newFill = cursor.getFillColor();
                if(bBlink)
                {
                    newFill.a = 255_BIT*(timeLastBlink/blinkFreq);
                }
                else
                {
                    newFill.a = 255_BIT*(1.0f-timeLastBlink/blinkFreq);
                }
                cursor.setFillColor(newFill);
            }
            break;
        }
        default:  // Blink
        {
            if(timeLastBlink >= blinkFreq)
            {
                bBlink = !bBlink;
                timeLastBlink = 0.0f;
            }
            else
            {
                sf::Color newFill = cursor.getFillColor();
                if(bBlink)
                {
                    newFill.a = 0_BIT;
                }
                else
                {
                    newFill.a = 255_BIT;
                }
                cursor.setFillColor(newFill);
            }
            break;
        }
        }

        timeLastBlink += event.lastFrameTime;
    }

    if(bTypeStringChanged)
    {

        bModified = true;

        if(!bEncode)
        {
            displayText.back().setString(typeString);
        }
        else
        {
            displayText.back().setString(string(typeString.size(), '*'));
        }

        if(textFitType == CV_TEXT_FIT_LATERAL)  // Fit laterally only
        {
            wstring typeFrameStr;

            if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding)
            {
                if(typeFrameStart < typeFrameEnd)
                {
                    if(!bEncode)
                    {
                        typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                            typeString.begin() + typeFrameEnd); // Assign frames and check again
                    }
                    else
                    {
                        typeFrameStr.assign(typeFrameEnd - typeFrameStart, '*');
                    }
                }
                displayText.back().setString(typeFrameStr);

                if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding)
                {
                    if(cursorPos >= typeFrameStr.size() + typeFrameStart)
                    {
                        ++typeFrameStart;
                    }
                    else
                    {
                        --typeFrameEnd;
                    }
                }
            }
            else
            {
                typeFrameStart = 0;
                typeFrameEnd = typeString.size();
            }

            if(typeFrameStart < typeFrameEnd)
            {
                if(!bEncode)
                {
                    typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                        typeString.begin() + typeFrameEnd);
                }
                else
                {
                    typeFrameStr.assign(typeFrameEnd - typeFrameStart, '*');
                }
            }
            displayText.back().setString(typeFrameStr);
        }
        else if(textFitType == CV_TEXT_FIT_WRAP)
        {

            for(auto& text : displayText)
            {

                cvis::wrapText(text, bounds, textPadding);
                alignText();

            }

        }

        displayText.back().setFillColor(colorTheme.back());
    }
    else if((typeString.size() < 1) && !hasFocus() && (bkgString.size() > 0))
    {
        displayText.back().setString(this->bkgString);
        sf::Color bkgColor = colorTheme.back();
        bkgColor.a = bkgColor.a > 100 ? bkgColor.a - 100 : 0;
        displayText.back().setFillColor(bkgColor);
    }
    else if((typeString.size() < 1) && hasFocus())
    {
        displayText.back().setString("");
    }

    switch(textInfo.alignment)  // Shift position to match typing
    {
    case ALIGN_VERTICAL_INVERTED:
    {
        displayText.back().setOrigin(displayText.back().getLocalBounds().width/2,
                                     displayText.back().getLocalBounds().height/2);
        displayText.back().setPosition(panel.front().getPosition());
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    case ALIGN_CENTER:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    default:  // Unknown text alignment
    {
        break;
    }
    }

    // Handle text expansion
    if(expandType)
    {

        sf::FloatRect textBounds = displayText.back().getGlobalBounds();
        expandBounds(textBounds, textPadding);

        float expandDist = 0.0f;
        if(expandType & CV_TEXT_EXPAND_BOTTOM)
        {

            if(expandRate) expandDist = (textBounds.top + textBounds.height - bounds.top - bounds.height)*expandRate;
            else expandDist = (textBounds.top + textBounds.height - bounds.top - bounds.height);

            if(expandDist > 0.0f)
            {
                if(bounds.height + expandDist < textBounds.top + textBounds.height)
                    bounds.height += expandDist;
                else bounds.height = textBounds.top + textBounds.height - bounds.top;

                alignText();
            }
            else if(expandDist < 0.0f)
            {
                if(bounds.top + bounds.height + expandDist > initBounds.top + initBounds.height)
                {
                    bounds.height += expandDist;
                }
                else bounds.height = initBounds.height;

                alignText();
            }
        }
        if(expandType & CV_TEXT_EXPAND_TOP)
        {
//            if(textBounds.top < bounds.top){
//                move(sf::Vector2f(0.0f,-expandRate));
//                bounds.height += expandRate;
//            }
//            else if((textBounds.top < bounds.top - expandRate) &&
//                    (bounds.top < initBounds.top)){
//                move(sf::Vector2f(0.0f,expandRate));
//                bounds.height -= expandRate;
//            }
        }
        if(expandType & CV_TEXT_EXPAND_RIGHT)
        {
//            if(textBounds.left + textBounds.width > bounds.left + bounds.width){
//                bounds.width += expandRate;
//            }
//            else if((textBounds.left + textBounds.width < bounds.left + bounds.width - expandRate) &&
//                    (bounds.left + bounds.width > initBounds.left + initBounds.width)){
//                bounds.width -= expandRate;
//            }
        }
        if(expandType & CV_TEXT_EXPAND_LEFT)
        {
//            if(textBounds.left < bounds.left){
//                move(sf::Vector2f(-expandRate, 0.0f));
//                bounds.width += expandRate;
//            }
//            else if((textBounds.left < bounds.left - expandRate) &&
//                    (bounds.left < initBounds.left)){
//                move(sf::Vector2f(expandRate, 0.0f));
//                bounds.width -= expandRate;
//            }
        }
        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
    }

    // Move the cursor
    if(hasFocus())
    {
        sf::Vector2f movePos(0.0f,0.0f);

        if((textInfo.alignment == ALIGN_VERTICAL_INVERTED) || (textInfo.alignment == ALIGN_VERTICAL))
        {
            movePos.y = displayText.back().findCharacterPos(cursorPos - typeFrameStart).y + 1.0f;
            movePos.x = displayText.back().findCharacterPos(cursorPos - typeFrameStart).x;
            View->anim_to(&cursor, movePos, pow(getDistance(cursor.getPosition(), movePos), 2), CV_OBJ_ANIM_SLIDE);
        }
        else
        {
            movePos.x = displayText.back().findCharacterPos(cursorPos - typeFrameStart).x - 1.0f;
            movePos.y = displayText.back().findCharacterPos(cursorPos - typeFrameStart).y;
            View->anim_to(&cursor, movePos, pow(getDistance(cursor.getPosition(), movePos), 2), CV_OBJ_ANIM_SLIDE);
        }
    }

    if(hasFocus() && (selectionStart != UINT_MAX) && (selectionStart != cursorPos))
    {

        if(lineFrames.empty())
        {
            lineFrames.emplace_back(2, 0);
        }

        sf::Color textHighlightColor;
        if(cursorChanged || bTypeStringChanged)
        {
            lineFrames.clear();
            lineFrames.emplace_back(2);
            selectionLineStart = 0;
            selectionLineEnd = 0;

            textHighlightColor = displayText.front().getFillColor();
            textHighlightColor.a = displayText.front().getFillColor().a/4;
            wstring stringBuff = displayText.back().getString();

            if(selectionStart < cursorPos)
            {
                for(size_t i = rangeBegin, line = 0; i <= cursorPos; ++i)
                {
                    if(selectionStart == i)
                    {
                        selectionLineStart = line;
                        lineFrames.back()[0] = i;
                    }
                    else if(cursorPos == i)
                    {
                        selectionLineEnd = line;
                        lineFrames.back()[1] = i;
                    }

                    if(stringBuff[i] == '\n')
                    {
                        ++line;
                        lineFrames.back()[1] = i;
                        lineFrames.emplace_back(2);
                        lineFrames.back()[0] = i+1;
                    }
                }
                lineFrames.back()[1] = cursorPos;
            }
            else
            {
                for(size_t i = rangeBegin, line = 0; i <= selectionStart; ++i)
                {
                    if(cursorPos == i)
                    {
                        selectionLineStart = line;
                        lineFrames.back()[0] = i;
                    }
                    if(selectionStart == i)
                    {
                        selectionLineEnd = line;
                        lineFrames.back()[1] = i;
                    }

                    if(stringBuff[i] == '\n')
                    {
                        ++line;
                        lineFrames.back()[1] = i;
                        lineFrames.emplace_back(2);
                        lineFrames.back()[0] = i+1;
                    }
                }
                lineFrames.back()[1] = selectionStart;
            }

            if(panel.size() > 1)
            {
                panel.erase(panel.begin() + 1, panel.end());
            }

        }

        float selectionWidth = 0.0f, lineSelectBegin = 0.0f, lineSelectEnd = 0.0f;

        for(size_t i = selectionLineStart, j = 1; (i <= selectionLineEnd) && (i < lineFrames.size()); ++i, ++j)
        {
            if(cursorChanged || bTypeStringChanged)
            {
                panel.emplace_back();
                panel.back().setFillColor(textHighlightColor);
                panel.back().setOrigin(cursor.getOrigin());
            }

            if(textInfo.alignment == ALIGN_VERTICAL)
            {
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][0]).y;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][1]).y;
            }
            else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
            {
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][1]).y;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][0]).y;
            }
            else
            {
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][0]).x;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][1]).x;
            }

            if(j < panel.size())
            {
                panel[j].setPosition(displayText.back().findCharacterPos(lineFrames[i][0]));
            }

            if(i == selectionLineEnd)
            {
                if(cursorPos > selectionStart)
                {
                    if(textInfo.alignment == ALIGN_VERTICAL)
                    {
                        lineSelectEnd = cursor.getPosition().y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                    {
                        lineSelectEnd = cursor.getPosition().y;
                    }
                    else
                    {
                        lineSelectEnd = cursor.getPosition().x;
                    }
                }
                else
                {
                    if(textInfo.alignment == ALIGN_VERTICAL)
                    {
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                    {
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else
                    {
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).x;
                    }
                }
            }
            if(i == selectionLineStart)
            {
                if(cursorPos < selectionStart)
                {
                    if(textInfo.alignment == ALIGN_VERTICAL)
                    {
                        lineSelectBegin = cursor.getPosition().y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                    {
                        lineSelectBegin = cursor.getPosition().y;
                    }
                    else
                    {
                        lineSelectBegin = cursor.getPosition().x;
                    }

                    if(j < panel.size())
                    {
                        panel[j].setPosition(cursor.getPosition());
                    }
                }
                else
                {
                    if(textInfo.alignment == ALIGN_VERTICAL)
                    {
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                    {
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else
                    {
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).x;
                    }

                    if(j < panel.size())
                    {
                        panel[j].setPosition(sf::Vector2f(displayText.back().findCharacterPos(selectionStart).x,
                                                          cursor.getPosition().y));
                    }
                }
            }

            selectionWidth = lineSelectEnd - lineSelectBegin;

            if(j < panel.size())
            {
                if(textInfo.alignment == ALIGN_VERTICAL)
                {
                    panel[j].setSize(sf::Vector2f(cursor.getGlobalBounds().width, selectionWidth));
                }
                else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED)
                {
                    panel[j].setSize(sf::Vector2f(cursor.getGlobalBounds().width, selectionWidth));
                }
                else
                {
                    panel[j].setSize(sf::Vector2f(selectionWidth, cursor.getGlobalBounds().height));
                }
            }
        }
    }
    else if((panel.size() > 1) && (!hasFocus() || cursorChanged || bTypeStringChanged))
    {
        panel.erase(panel.begin() + 1, panel.end());
    }

    bTypeStringChanged = false;
    return true;
}

void CVTypeBox::setSize(const sf::Vector2f& newSize)
{

    CVBox::setSize(newSize);

    if(textFitType == CV_TEXT_FIT_WRAP)
    {
        wrapText();
    }

    alignText();

    initBounds = bounds;

}

void CVTypeBox::setSize(const float& x, const float& y)
{

    setSize(sf::Vector2f(x, y));

}

void CVTypeBox::setAlignment(const uint8_t& alignment)
{
    textInfo.alignment = alignment;
    switch(alignment)  // Shift position to match typing
    {
    case ALIGN_VERTICAL_INVERTED:
    {
        displayText.back().setOrigin(displayText.back().getLocalBounds().width/2,
                                     displayText.back().getLocalBounds().height/2);
        displayText.back().setPosition(panel.front().getPosition());
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    case ALIGN_CENTER:
    {
        float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                           displayText.back().getGlobalBounds().width/2;
        displayText.back().move(centerDist, 0.0f);
        break;
    }
    default:  // Unknown text alignment
    {
        break;
    }
    }
}

bool CVTypeBox::draw(sf::RenderTarget* target)
{
    if(!CVTextBox::draw(target)) return false;
    if(hasFocus())
    {

    CV_DRAW_CLIP_BEGIN

    target->draw(cursor);

    CV_DRAW_CLIP_END

    }
    return true;
}

CVTextLog::CVTextLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
                     const TextEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                     const float& borderWidth, const uint8_t& animType, const string& logFile,
                     CVTypeBox* userEntrySource):
    CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
    usrEntryBox(userEntrySource),
    logFile(logFile),
    selectedLogIndex(0),
    maxMessages(1000),
    initialMessageCount(100),
    messageLoadInc(25),
    timeWaiting(0.0f),
    msgPopupSpeed(250.0f),
    msgScrollSpeed(2600.0f),
    maxPanelWidthPct(0.7f),
    msgPanelPaddingPx(12.0f*View->getViewScale()),
    templateTheme(textInfo),
    bClear(false),
    bMsgReady(true),
    bCanScroll(true),
    animType(animType),
    userTextAlign(ALIGN_RIGHT),
    otherTextAlign(ALIGN_LEFT),
    selectedEntry(nullptr)
{

    colorTheme.emplace_back(borderColor);                   // Message panel fill
    brighten(colorTheme.back(), 20_BIT);
    colorTheme.emplace_back(borderColor);                   // Message panel border
    colorTheme.emplace_back(getMsgPanelFillColor());        // Incoming message panel fill
    brighten(colorTheme.back(), 60_BIT);
    colorTheme.emplace_back(getMsgPanelFillColor());        // Highlight panel color
    darken(colorTheme.back(), 50_BIT);
    colorTheme.emplace_back(textInfo.textColor);            // Incoming message text color

    sf::Color scrollBarColor = getMsgPanelFillColor() - sf::Color(0, 0, 0, 255);

    scroll_bar = new CVScrollBar(View,
                                 sf::Vector2f(bounds.left + bounds.width * 0.97f,
                                              bounds.top + bounds.height * 0.975f),
                                 sf::Vector2f(bounds.left + bounds.width * 0.97f,
                                              bounds.top + bounds.height * 0.025f),
                                 height * 0.925f, 8.0f,
                                 scrollBarColor,
                                 sf::Color::Transparent,
                                 0.0f,
                                 msgScrollSpeed);

    scroll_bar->reverseY();
    setDrawClipping(true);

    this->textInfo = templateTheme;
}

CVTextLog::~CVTextLog()
{
    for(auto& panel : msgPanels)
    {
        delete(panel);
    }
    msgPanels.clear();
    delete(scroll_bar);
}

void CVTextLog::move(const sf::Vector2f& distance)
{
    CVTextBox::move(distance);
    for(auto& item : msgPanels)
    {
        item->move(distance);
    }
    scroll_bar->move(distance);
}

void CVTextLog::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVTextLog::setSize(const sf::Vector2f& newSize)
{

    float offsetY = newSize.y - getSize().y;
    float offsetX = newSize.x - getSize().x;

    CVBox::setSize(newSize);

    for(size_t i = 0; i < msgPanels.size(); ++i)
    {
        if(textLogUserEntered[i])
        {
            msgPanels[i]->move(offsetX, offsetY);
        }
        else
        {
            msgPanels[i]->move(0.0f, offsetY);
        }
    }

    scroll_bar->setAnchorPoints(sf::Vector2f(bounds.left + bounds.width * 0.97f,
                                             bounds.top + bounds.height * 0.025f),
                                sf::Vector2f(bounds.left + bounds.width * 0.97f,
                                             bounds.top + bounds.height * 0.975f));
    scroll_bar->setPanelSize(bounds.height * 0.95f);
}

float CVTextLog::scrollOffsetY() const noexcept
{
    return scroll_bar->getScrollOffset();
}

void CVTextLog::shiftMsgs(const float& dist)
{
    // Negative is up (SFML coords)
    scroll_bar->scroll(dist);
}

bool CVTextLog::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!visible || bNoInteract) return false;

    scroll_bar->update(event, mousePos);

    if(bClear)
    {
        scroll_bar->setScrollOffset(0.0f);
        textLog.clear();
        times.clear();

        for(auto& panel : msgPanels)
        {
            delete(panel);
        }
        msgPanels.clear();

        textLogUserEntered.clear();
        save();
        bClear = false;
    }

    for(auto& panel : msgPanels)
    {
        panel->update(event, mousePos);
    }

    if(bMsgReady && (!waitingText.empty()))
    {

        printLock.lock();

        if(timeWaiting < updateAddDelay.front())
        {

            timeWaiting += event.lastFrameTime;

        }
        else
        {

            updateAddDelay.erase(updateAddDelay.begin());
            timeWaiting = 0.0f;

            times.addTimePoint();

            textLog.push_back(waitingText.front());
            textLogUserEntered.push_back(textLogUserWaiting.front());

            msgPanels.emplace_back(new CVTextLogMsg(View, templateTheme));

            msgPanels.back()->setSize(sf::Vector2f(maxPanelWidthPct*bounds.width, 2.0f));
            msgPanels.back()->setOutlineColor(getMsgPanelBorderColor());
            msgPanels.back()->setTextPadding(msgPanelPaddingPx);
            msgPanels.back()->setRounding(10.0f*View->getViewScale());
            msgPanels.back()->setTextWrap(true);
            msgPanels.back()->setText(0, waitingText.front());
            msgPanels.back()->fitText();
            msgPanels.back()->textLogIndex = textLog.size() - 1;

            float moveBump = 0.0f;

            if(textLogUserEntered.size() > 1)
            {
                if(textLogUserEntered.back() ^ textLogUserEntered[textLogUserEntered.size()-2])
                {
                    moveBump = msgPanelPaddingPx;
                }
            }

            if(textLogUserWaiting.front())
            {

                switch(userTextAlign)
                {
                case ALIGN_LEFT:
                {
                    msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                               bounds.top + bounds.height * 0.95f +
                                                               msgPanelPaddingPx/2 + scrollOffsetY()));
                    break;
                }
                default:  // Right
                {
                    msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - msgPanels.back()->getSize().x,
                                                               bounds.top + bounds.height * 0.95f +
                                                               msgPanelPaddingPx/2 + scrollOffsetY()));
                    break;
                }
                }

                msgPanels.back()->setFillColor(getMsgPanelFillColor());
                msgPanels.back()->setTextColor(templateTheme.textColor);
                msgPanels.back()->bUser = true;

            }
            else
            {

                switch(otherTextAlign)
                {
                case ALIGN_RIGHT:
                {
                    msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - msgPanels.back()->getSize().x,
                                                               bounds.top + bounds.height * 0.95f +
                                                               msgPanelPaddingPx/2 + scrollOffsetY()));
                    break;
                }
                default:  // Left
                {
                    msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                               bounds.top + bounds.height * 0.95f +
                                                               msgPanelPaddingPx/2 + scrollOffsetY()));
                    break;
                }
                }

                msgPanels.back()->setFillColor(getIncomingMsgPanelFillColor());
                msgPanels.back()->setTextColor(getOtherTextColor());
                msgPanels.back()->bUser = false;

            }

            waitingText.erase(waitingText.begin());
            textLogUserWaiting.erase(textLogUserWaiting.begin());

            save();

        }

        printLock.unlock();

    }
    else
    {
        timeWaiting = 0.0f;
    }

    // Process scrolling

    if((bounds.contains(mousePos) || (event.LMBhold && bounds.contains(event.LMBpressPosition)))
            && event.captureFocus())
    {
        setFocus(true);
        scroll_bar->setScrollable(true);
    }
    else
    {
        if(event.LMBhold)
        {
            setFocus(false);
        }
        scroll_bar->setScrollable(false);
    }

    // Process clicking

    if(hasFocus() &&
       (event.LMBreleaseFrames == 1) &&
       bounds.contains(event.LMBreleasePosition))
    {

        if(usrEntryBox && event.captureMouse())
        {

            for(auto& item : msgPanels)
            {

                if(item->getGlobalBounds().contains(event.LMBpressPosition))
                {
                    item->setFillColor(getHighlightPanelFillColor());
                    if(item->bUser)
                    {
                        usrEntryBox->setTypeString(textLog[item->textLogIndex]);
                    }
                    selectedLogIndex = item->textLogIndex;
                    selectedEntry = item;
                }
                else
                {
                    selectedEntry = nullptr;
                    if(item->bUser)
                    {
                        item->setFillColor(getMsgPanelFillColor());
                    }
                    else
                    {
                        item->setFillColor(getIncomingMsgPanelFillColor());
                    }
                }
            }

        }

    }

    // Process scrolling

    if(msgPanels.size() > 0)
    {

        float boundHeight = msgPanels.back()->getPosition().y +
                            msgPanels.back()->getSize().y -
                            msgPanels.front()->getPosition().y;

        scroll_bar->setScrollMax(boundHeight);

        if(boundHeight > bounds.height)
        {

            float moveDist = bounds.top + bounds.height * 0.95f -
                             (msgPanels.back()->getPosition().y +
                             msgPanels.back()->getSize().y) +
                             scroll_bar->getScrollOffset();

            for(auto& panel : msgPanels)
            {
                panel->anim_move(sf::Vector2f(0.0f, moveDist), msgScrollSpeed * abs(moveDist)/600.0f + 700.0f);
            }

        }
        else
        {

            float moveDist = bounds.top + bounds.height * 0.95f -
                             (msgPanels.back()->getPosition().y +
                             msgPanels.back()->getSize().y);

            for(auto& panel : msgPanels)
            {
                panel->anim_move(sf::Vector2f(0.0f, moveDist), msgScrollSpeed * abs(moveDist)/600.0f + 700.0f);
            }

        }

    }

    // Fade elements at the edge of the top scroll boundary

    sf::Color tmp;
    float gradient = 0.0f;
    for(auto& item : msgPanels)
    {
        if(item->getGlobalBounds().height > bounds.height)
        {
            gradient = (item->getPosition().y + item->getGlobalBounds().height - bounds.top)/bounds.height*4;
        }
        else
        {
            gradient = (item->getPosition().y + item->getGlobalBounds().height/2 - bounds.top)/item->getGlobalBounds().height;
        }

        if(gradient < 0.0f) gradient = 0.0f;
        if(gradient > 1.0f) gradient = 1.0f;

        tmp = item->getFillColor();
        tmp.a = getMsgPanelFillColor().a * gradient;
        item->setFillColor(tmp);

        tmp = item->getTextColor();

        if(item->bUser)
        {
            tmp.a = templateTheme.textColor.a * gradient;
        }
        else
        {
            tmp.a = getOtherTextColor().a * gradient;
        }

        item->setTextColor(tmp);
    }

    if(hasFocus())
    {
        for(auto& key : event.keyLog)
        {
            switch(key)
            {
            case char(10):
            {
                if(usrEntryBox && usrEntryBox->canEnterLine())
                {
                    usrEntryBox->enterString();
                    usrEntryBox->sendEnteredString();
                }
                break;
            }
            case 'c':
            {
                if(ctrlPressed())
                {
                    copyToClipboard(textLog[selectedLogIndex]);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    return true;

}

void CVTextLog::addLogEntry(const string& text, bool userEntry, float updateDelay)
{
    printLock.lock();
    waitingText.push_back(text);
    textLogUserWaiting.push_back(userEntry);
    updateAddDelay.push_back(updateDelay);
    if(userEntry) lastUserEntered = text;
    else lastOtherEntered = text;
    printLock.unlock();
}

void CVTextLog::addToLastEntry(const string& text, bool userEntry)
{
    if(!waitingText.empty() && (textLogUserWaiting.back() == userEntry))
    {
        printLock.lock();
        waitingText.back().append(text);
        printLock.unlock();
    }
    else if(!textLogUserEntered.empty() && (textLogUserEntered.back() == userEntry))
    {
        printLock.lock();
        textLog.back().append(text);
        float shiftDist = msgPanels.back()->getGlobalBounds().height;
        msgPanels.back()->setSize(sf::Vector2f(bounds.width * maxPanelWidthPct,
                                               msgPanels.back()->getSize().y));
        msgPanels.back()->setString(textLog.back());
        shiftDist -= msgPanels.back()->getGlobalBounds().height;

        if(shiftDist)
        {
            for(size_t i = 0; i + 1 < msgPanels.size(); ++i)
            {
                msgPanels[i]->anim_move(sf::Vector2f(0.0f, shiftDist), msgPopupSpeed);
            }
        }

        printLock.unlock();
    }
    else
    {
        addLogEntry(text, userEntry, 0.0f);
    }
}

void CVTextLog::editLastEntry(const string& text, bool userEntry)
{
    if(!waitingText.empty() && (textLogUserWaiting.back() == userEntry))
    {
        printLock.lock();
        waitingText.back() = text;
        printLock.unlock();
    }
    else if(!textLogUserEntered.empty() && (textLogUserEntered.back() == userEntry))
    {
        printLock.lock();
        textLog.back() = text;
        float shiftDist = msgPanels.back()->getGlobalBounds().height;
        msgPanels.back()->setSize(sf::Vector2f(bounds.width * maxPanelWidthPct,
                                               msgPanels.back()->getSize().y));
        msgPanels.back()->setString(text);
        shiftDist -= msgPanels.back()->getGlobalBounds().height;

        if(shiftDist)
        {
            for(size_t i = 0; i + 1 < msgPanels.size(); ++i)
            {
                msgPanels[i]->anim_move(sf::Vector2f(0.0f, shiftDist), msgPopupSpeed);
            }
        }

        printLock.unlock();
    }
    else
    {
        addLogEntry(text, userEntry, 0.0f);
    }
}

bool CVTextLog::save()
{

    if(logFile.size() < 1) return false;

    if(!access(logFile.c_str(), F_OK))
    {
        if(access(logFile.c_str(), W_OK)) return false;
    }

    FILE* outFILE = fopen(logFile.c_str(), "wb");

    size_t L = textLog.size(), sSIZE;
    fwrite(&L, 1, sizeof(size_t), outFILE);
    bool userEntered;

    for(size_t i = 0; i < L; ++i)
    {
        sSIZE = textLog[i].size();
        fwrite(&sSIZE, 1, sizeof(size_t), outFILE);
        fwrite(textLog[i].c_str(), sizeof(char), textLog[i].size(), outFILE);
        fwrite(times[i].data(), sizeof(int), 6, outFILE);

        userEntered = textLogUserEntered[i];
        fwrite(&userEntered, 1, sizeof(bool), outFILE);
    }

    fclose(outFILE);

    return true;
}

bool CVTextLog::load()
{

    if(logFile.empty()) return false;

    if(access(logFile.c_str(), F_OK))
    {
        cout << "Error loading text log from " << logFile << '\n';
        cout << "Error: " << strerror(errno) << '\n';
        return false;
    }

    for(auto& panel : msgPanels)
    {
        delete(panel);
    }

    msgPanels.clear();
    scroll_bar->setScrollOffset(0.0f);

    FILE* inFILE = fopen(logFile.c_str(), "rb");

    size_t L, cSIZE;
    fread(&L, 1, sizeof(size_t), inFILE);
    int newTime [] = {0,0,0,0,0,0};
    bool userEntered;

    textLog.clear();
    textLog.reserve(L);
    textLogUserEntered.clear();
    textLogUserEntered.reserve(L);
    times.clear();
    times.reserve(L);

    for(size_t i = 0; i < L; ++i)
    {
        fread(&cSIZE, 1, sizeof(size_t), inFILE);
        char* newStr = new char[cSIZE+1];
        newStr[cSIZE] = '\0';
        fread(newStr, cSIZE, sizeof(char), inFILE);
        textLog.emplace_back();
        textLog.back().assign(newStr, newStr + cSIZE);

        fread(newTime, 6, sizeof(int), inFILE);
        times.emplace_back(newTime);

        fread(&userEntered, 1, sizeof(bool), inFILE);
        textLogUserEntered.push_back(userEntered);
    }

    fclose(inFILE);

    int date = 0, newDate = 0;
    unsigned int loadBegin = L <= initialMessageCount ? 0 : L - initialMessageCount; // Load an initial count, then require user scroll

    for(size_t N = loadBegin; N < L; ++N)
    {

        newDate = times[N].year() * times[N].month() * times[N].day();

        if(date != newDate)  // Show the date when days change
        {

            string dateString;
            TimePoint time_now;

            if(newDate == time_now.year() * time_now.month() * time_now.day()) dateString = "Today";
            else dateString = times[N].getDate();

            sf::Color dateColor = getMsgPanelFillColor();
            dateColor.a = 60;

            msgPanels.emplace_back(new CVTextLogMsg(View, templateTheme));

            msgPanels.back()->setOutlineColor(getMsgPanelBorderColor());
            msgPanels.back()->setTextPadding(msgPanelPaddingPx);
            msgPanels.back()->setRounding(10.0f*View->getViewScale());
            msgPanels.back()->setText(0, dateString);
            msgPanels.back()->setFillColor(dateColor);
            msgPanels.back()->fitText();
            msgPanels.back()->time = times[N];
            msgPanels.back()->bMsg = false;

            msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width/2 - msgPanels.back()->getSize().x/2,
                                                       bounds.top + bounds.height - 2*msgPanelPaddingPx - msgPanels.back()->getSize().y));

            if(msgPanels.size() > 1)
            {
                float moveDist = msgPanels.back()->getSize().y + 3.5*msgPanelPaddingPx;

                for(size_t i = 1; i < msgPanels.size(); ++i)
                {
                    msgPanels[i - 1]->move(sf::Vector2f(0.0f, -moveDist));
                }
            }

            date = newDate;
        }

        msgPanels.emplace_back(new CVTextLogMsg(View, templateTheme));

        msgPanels.back()->setSize(sf::Vector2f(maxPanelWidthPct*bounds.width, 2.0f));
        msgPanels.back()->setOutlineColor(getMsgPanelBorderColor());
        msgPanels.back()->setRounding(10.0f*View->getViewScale());
        msgPanels.back()->setTextPadding(msgPanelPaddingPx);
        msgPanels.back()->setTextWrap(true);
        msgPanels.back()->setText(0, textLog[N]);
        msgPanels.back()->fitText();
        msgPanels.back()->time = times[N];
        msgPanels.back()->textLogIndex = N;

        float moveBump = 0.0f;
        float shiftDist = msgPanels.back()->getSize().y + msgPanelPaddingPx/2;

        if(N > 0)
        {
            if(textLogUserEntered[N-1] ^ textLogUserEntered[N])
            {
                moveBump = msgPanelPaddingPx;
            }
        }

        for(size_t i = 1; i < msgPanels.size(); ++i)
        {
            msgPanels[i - 1]->move(sf::Vector2f(0.0f, -(shiftDist + moveBump)));
        }

        if(textLogUserEntered[N])
        {

            switch(userTextAlign)
            {
            case ALIGN_LEFT:
            {
                msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                           bounds.top + bounds.height));
                break;
            }
            default:  // Right
            {
                msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - msgPanels.back()->getSize().x,
                                                           bounds.top + bounds.height));
                break;
            }
            }

            msgPanels.back()->setFillColor(getMsgPanelFillColor());
            msgPanels.back()->setTextColor(templateTheme.textColor);
            msgPanels.back()->bUser = true;

        }
        else
        {

            switch(otherTextAlign)
            {
            case ALIGN_RIGHT:
            {
                msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - msgPanels.back()->getSize().x,
                                               bounds.top + bounds.height));
                break;
            }
            default:  // Left
            {
                msgPanels.back()->setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                               bounds.top + bounds.height));
                break;
            }
            }

            msgPanels.back()->setFillColor(getIncomingMsgPanelFillColor());
            msgPanels.back()->setTextColor(getOtherTextColor());
            msgPanels.back()->bUser = false;

        }

        msgPanels.back()->move(sf::Vector2f(0.0f,-shiftDist));

    }

    float scrollMax = 0.0f;

    if(!msgPanels.empty())
    {
        scrollMax = msgPanels.back()->getPosition().y +
                    msgPanels.back()->getSize().y -
                    msgPanels.front()->getPosition().y;
    }

    scroll_bar->setScrollMax(scrollMax);

    return true;

}

string CVTextLog::getReadableLog() const
{
    stringstream output;

    for(size_t i = 0; i < textLog.size(); ++i)
    {

        output << times[i].getDate() << ' ' << times[i].getTime();

        if(textLogUserEntered[i])
        {
            output << " >> ";
        }
        else
        {
            output << " << ";
        }

        output << textLog[i] << '\n';

    }

    return output.str();
}

string CVTextLog::getUserLogText(const unsigned int& index, bool lastEntered) const
{
    unsigned int j = 0;
    if(lastEntered)
    {
        for(size_t i = textLog.size(); i > 0; --i)
        {
            if(textLogUserEntered[i-1])
            {
                if(j == index) return textLog[i-1];
                else ++j;
            }
        }

    }
    else
    {
        for(size_t i = 0; i < textLog.size(); ++i)
        {
            if(textLogUserEntered[i])
            {
                if(j == index) return textLog[i];
                else ++j;
            }
        }
    }
    return string();
}

string CVTextLog::getOtherLogText(const unsigned int& index, bool lastEntered) const
{
    unsigned int j = 0;
    if(lastEntered)
    {
        for(size_t i = textLog.size(); i > 0; --i)
        {
            if(!textLogUserEntered[i-1])
            {
                if(j == index) return textLog[i-1];
                else ++j;
            }
        }

    }
    else
    {
        for(size_t i = 0; i < textLog.size(); ++i)
        {
            if(!textLogUserEntered[i])
            {
                if(j == index) return textLog[i];
                else ++j;
            }
        }
    }
    return string();
}

void CVTextLog::replaceLastMessage(const string& search,
                                   const string& substitute,
                                   bool user)
{

    for(int i = textLog.size() - 1; i >= 0; --i)
    {

        if(textLogUserEntered[i] == user)
        {

            for(int j = displayText.size() - 1; j >= 0; --j)
            {

                if(cmpString(displayText[j].getString(), textLog[i],
                             CMP_STR_SW |
                             CMP_STR_CASE_INSENSITIVE |
                             CMP_STR_SIZE_INSENSITIVE,
                             0.7f))
                {

                    replace(textLog[i], search, substitute);
                    displayText[j].setString(textLog[i]);

                    return;

                }

            }

        }

    }

}

bool CVTextLog::draw(sf::RenderTarget* target)
{
    if(View->viewPort == nullptr) return false;
    if(!visible) return false;

    CV_DRAW_CLIP_BEGIN

    for(auto& item : panel)
    {
        target->draw(item);
    }
    for(auto& sprite : spriteList)
    {
        target->draw(sprite);
    }
    for(auto& panel : msgPanels)
    {
        if(bounds.intersects(panel->getGlobalBounds()))
        {
            panel->draw(target);
        }
    }
    for(auto& text : displayText)
    {
        target->draw(text);
    }
    for(auto& item : mask)
    {
        target->draw(item);
    }

    scroll_bar->draw(target);

    CV_DRAW_CLIP_END

    return true;
}

}
