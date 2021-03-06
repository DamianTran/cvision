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

#include "cvision/textbox.hpp"
#include "cvision/view.hpp"
#include "cvision/event.hpp"
#include "cvision/app.hpp"

#include "hyper/toolkit/string.hpp"

using namespace hyperC;
using namespace std;

namespace cvis
{

CVTextBox::CVTextBox(CVView* View,
                     const sf::Vector2f& position,
                     const float& width,
                     const float& height,
                     const sf::Color& fillColor,
                     const sf::Color& borderColor,
                     const float& borderWidth):
    CVBox(View,
          position,
          width,
          height,
          fillColor,
          borderColor,
          borderWidth),
    textInfo("", getDefaultFont(), 12),
    textPadding(0.0f),
    textFont(nullptr),
    bWrapText(false)
{
}

CVTextBox::CVTextBox(CVView* View,
                     const sf::Vector2f& position,
                     const float& width,
                     const float& height,
                     const TextEntry& textInfo,
                     const sf::Color& fillColor,
                     const sf::Color& borderColor,
                     const float& borderWidth):
    CVBox(View,
          position,
          width,
          height,
          fillColor,
          borderColor,
          borderWidth),
    textInfo(textInfo),
    textPadding((width+height)/50),
    textFont(appFont(textInfo.font)),
    bWrapText(false)
{

    addTextEntry(textInfo, position);

    alignText();

}

bool CVTextBox::fadeComplete() const noexcept
{
    if(!CVBox::fadeComplete()) return false;

    if(fadeLayers & CV_LAYER_TEXT)
    {
        for(auto& text : displayText)
        {
            if(text.getFillColor().a != targetAlpha)
            {
                return false;
            }
        }
    }

    return true;
}

void CVTextBox::alignText()
{
    sf::FloatRect textBounds;
    sf::Vector2i textPos(getPosition());
    unsigned int numText = displayText.size() + 1,
                 i = 1;

    for(auto& text : displayText)
    {

        textBounds = text.getGlobalBounds();

        switch(textInfo.alignment)
        {
        case ALIGN_CENTER:
        {
            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
            textPos.y = bounds.top + (i - 1)*bounds.height/numText + textPadding;
            break;
        }
        case ALIGN_CENTER_MIDLINE:
        {
            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;

            if(getLineCount(text) > 1)
            {
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text)/4;
            }
            else
            {
                textPos.y = bounds.top + i*bounds.height/numText - getTextCenterOffsetY(text);
            }

            break;
        }
        case ALIGN_CENTER_BOTTOM:
        {
            textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;

            if(getLineCount(text) > 1)
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                textBounds.height -
                                getTextCenterOffsetY(text)/4;
                }
                else textPos.y = bounds.top + bounds.height - textPadding - textBounds.height - getTextCenterOffsetY(text)/4;
            }
            else
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                getTextCenterOffsetY(text);
                }
                else textPos.y = bounds.top + bounds.height - textPadding - getTextCenterOffsetY(text);
            }

            break;
        }
        case ALIGN_RIGHT:
        {
            textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;
            textPos.y = bounds.top + i*bounds.height/numText + textPadding;
            break;
        }
        case ALIGN_RIGHT_MIDLINE:
        {
            textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;

            if(getLineCount(text) > 1)
            {
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text)/4;
            }
            else
            {
                textPos.y = bounds.top + i*bounds.height/numText - getTextCenterOffsetY(text);
            }

            break;
        }
        case ALIGN_RIGHT_BOTTOM:
        {
            textPos.x = bounds.left + bounds.width - textPadding - textBounds.width;

            if(getLineCount(text) > 1)
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                textBounds.height -
                                getTextCenterOffsetY(text)/4;
                }
                else textPos.y = bounds.top + bounds.height - textPadding - textBounds.height - getTextCenterOffsetY(text)/4;
            }
            else
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                getTextCenterOffsetY(text);
                }
                else textPos.y = bounds.top + bounds.height - textPadding - getTextCenterOffsetY(text);
            }

            break;
        }
        case ALIGN_LEFT_MIDLINE:
        {
            textPos.x = bounds.left + textPadding;

            if(getLineCount(text) > 1)
            {
                textPos.y = bounds.top + i*bounds.height/numText - textBounds.height/2 - getTextCenterOffsetY(text)/4;
            }
            else
            {
                textPos.y = bounds.top + i*bounds.height/numText - getTextCenterOffsetY(text);
            }

            break;
        }
        case ALIGN_LEFT_BOTTOM:
        {
            textPos.x = bounds.left + textPadding;

            if(getLineCount(text) > 1)
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                textBounds.height -
                                getTextCenterOffsetY(text)/4;
                }
                else textPos.y = bounds.top + bounds.height - textPadding - textBounds.height - getTextCenterOffsetY(text)/4;
            }
            else
            {
                if(i > 1)
                {
                    textPos.y = displayText[i-1].getGlobalBounds().top +
                                displayText[i-1].getGlobalBounds().height +
                                textPadding -
                                getTextCenterOffsetY(text);
                }
                else textPos.y = bounds.top + bounds.height - textPadding - getTextCenterOffsetY(text);
            }

            break;
        }
        default:  // Left
        {
            textPos.x = bounds.left + textPadding;
            textPos.y = bounds.top + textPadding + ((i - 1)*bounds.height)/numText;
            break;
        }
        }

        text.setPosition(textPos.x, textPos.y);
        ++i;
    }
}

void CVTextBox::wrapText()
{
    sf::FloatRect textBounds;
    for(auto& text : displayText)
    {
        textBounds = text.getGlobalBounds();

        if(textBounds.width > bounds.width - 2*textPadding)
        {
            wstring displayTextString = text.getString();
            for(size_t i = 0; i < displayTextString.size(); ++i)  // Add new line characters to fit text horizontally
            {

                if(text.findCharacterPos(i).x - bounds.left > bounds.width - 2*textPadding)
                {
                    for(int j = i; j > 0; --j)
                    {
                        if(displayTextString[j-1] == ' ')
                        {
                            displayTextString[j-1] = '\n';
                            break;
                        }
                        else if((displayTextString[j] != '\n') && isCharType(displayTextString[j-1], ",\\/:._-"))
                        {
                            displayTextString.insert(displayTextString.begin() + j, '\n');
                            break;
                        }
                        else if(abs(text.findCharacterPos(i).x -
                                    text.findCharacterPos(j).x) > bounds.width - 2*textPadding)
                        {
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

    if(bExpand)
    {
        updateBounds();
    }
    alignText();
}

void CVTextBox::setString(const string& newString)
{
    if(!displayText.empty())
    {
        displayText.front().setString(UTF8_to_UTF16(newString));
        if(bWrapText) wrapText();
    }
    alignText();
}

void CVTextBox::setString(const wstring& newString)
{
    if(!displayText.empty())
    {
        displayText.front().setString(newString);
        if(bWrapText) wrapText();
    }
    alignText();
}

void CVTextBox::fitText(const bool& fitX,
                        const bool& fitY,
                        const sf::Vector2f& padding)
{
    if(displayText.empty())
    {
        setSize(sf::Vector2f(0,0));
    }
    else if(fitX || fitY)
    {
        sf::FloatRect textBounds = displayText.front().getGlobalBounds();
        expandBounds(textBounds, textPadding);

        sf::Vector2f newPosition = getPosition();
        sf::Vector2f newSize = getSize();

        if(fitX)
        {
            newPosition.x = textBounds.left - padding.x;
            newSize.x = textBounds.width + 2*padding.x;
        }
        if(fitY)
        {
            newPosition.y = textBounds.top - padding.y;
            newSize.y = textBounds.height + 2*padding.y;
        }

        CVBox::setPosition(newPosition);
        CVBox::setSize(newSize);
    }

    alignText();
}

void CVTextBox::setTextSize(const unsigned int& newSize)
{
    for(auto& text : displayText)
    {
        text.setCharacterSize(newSize);
    }
    textInfo.fontSize = newSize;
    alignText();
}

void CVTextBox::setTextColor(const sf::Color& newColor)
{
    for(auto& text : displayText)
    {
        text.setFillColor(newColor);
    }
    textInfo.textColor = newColor;
}

void CVTextBox::setTextAlignment(const uint8_t& newAlignment)
{
    textInfo.alignment = newAlignment;
    alignText();
}

void CVTextBox::setTextWrap(const bool& state)
{
    bWrapText = state;
    if(bWrapText) wrapText();
}

void CVTextBox::setTextPadding(const float& newPadding)
{
    textPadding = newPadding;
    if(bWrapText) wrapText();
    else alignText();
}

void CVTextBox::setExpand(const bool& state)
{
    bExpand = state;
    if(bExpand) updateBounds();
}

void CVTextBox::updateBounds()
{
    CVBox::updateBounds();
    if(!bSpriteOnly)
    {
        sf::FloatRect textBounds = get_element_bounds(displayText);
        expandBounds(textBounds, textPadding);

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

void CVTextBox::sendData(CVEvent& event) const
{
    string cpyData = textInfo.text;
    event.sendData(cpyData.c_str(), cpyData.size(), CV_EVENT_LMB);
}

string CVTextBox::getText() const
{
    wstring newLine;

    if(displayText.empty()) return string();

    wstringstream output;

    for(size_t i = 0, j; i < displayText.size(); ++i)
    {
        newLine = displayText[i].getString();
        if(bWrapText)
        {
            for(j = 0; j < newLine.size();)
            {
                if(newLine[j] == '\n') newLine.erase(newLine.begin() + j);
                else ++j;
            }
        }
        output << newLine;
        if(i < displayText.size() - 1) output << '\n';
    }
    return UTF16_to_UTF8(output.str());
}

const sf::Color& CVTextBox::getTextColor() const
{
    return textInfo.textColor;
}

void CVTextBox::addTextEntry(const TextEntry& newText,
                             const float& padding,
                             const bool& regular)
{

    sf::Vector2f newTextPosition;
    sf::Rect<float> lastTextBounds;
    if(!displayText.empty())
    {
        if(regular) lastTextBounds = averageTextSize(displayText);
        else
        {
            lastTextBounds = displayText.back().getLocalBounds();
        }
        newTextPosition = displayText.back().getPosition();

        textInfo.text += '\n' + newText.text;
    }
    else
    {
        newTextPosition = getPosition();
        textInfo = newText;
    }

    if(appFont(newText.font))
    {
        displayText.emplace_back(
            UTF8_to_UTF16(newText.text),
            *appFont(newText.font),
            newText.fontSize
        );
    }
    else
    {
        displayText.emplace_back();
        displayText.back().setString(UTF8_to_UTF16(newText.text));
        displayText.back().setCharacterSize(newText.fontSize);
    }

    displayText.back().setFillColor(newText.textColor);
    colorTheme.emplace_back(newText.textColor);
    displayText.back().setPosition(round(newTextPosition));

    sf::Rect<float> newTextBounds = displayText.back().getLocalBounds();
    newTextPosition = getTextHorizontalAlignment(displayText.back(), bounds, newText.alignment);

    switch(textAlignment)
    {
    case TEXT_ALIGN_VERTICAL:
    {
        displayText.back().setPosition(std::round(newTextPosition.x),
                                       std::round(newTextPosition.y + lastTextBounds.height + padding));
        break;
    }
    case TEXT_ALIGN_HORIZONTAL:
    {
        displayText.back().setPosition(std::round(newTextPosition.x + lastTextBounds.width + padding),
                                       std::round(newTextPosition.y));
        break;
    }
    default:
        break;
    }

    if(bWrapText)
    {
        cvis::wrapText(displayText.back(), bounds, textPadding);
    }
    if(bExpand) updateBounds();
}

void CVTextBox::addTextEntry(const TextEntry& newText, const sf::Vector2f& position)
{

    if(displayText.empty())
    {
        textInfo = newText;
    }
    else
    {
        textInfo.text += '\n' + newText.text;
    }

    if(appFont(newText.font))
    {
        displayText.emplace_back(UTF8_to_UTF16(newText.text),
                                 *appFont(newText.font),
                                 newText.fontSize);
    }
    else
    {
        displayText.emplace_back();
        displayText.back().setString(UTF8_to_UTF16(newText.text));
        displayText.back().setCharacterSize(newText.fontSize);
    }

    displayText.back().setFillColor(newText.textColor);
    displayText.back().setStyle(newText.textStyle);

    colorTheme.emplace_back(newText.textColor);

    switch(newText.alignment)
    {
    case ALIGN_LEFT_MIDLINE:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x),
                                       std::round(position.y - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2));
        break;
    }
    case ALIGN_LEFT_BOTTOM:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x),
                                       std::round(position.y - textBounds.height));
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x - textBounds.width/2),
                                       std::round(position.y - textBounds.height/2));
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x - textBounds.width/2),
                                       std::round(position.y - textBounds.height));
        break;
    }
    case ALIGN_CENTER:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x - textBounds.width/2),
                                       std::round(position.y));
        break;
    }
    case ALIGN_RIGHT:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(std::round(position.x - textBounds.width),
                                       std::round(position.y));
        break;
    }
    case ALIGN_VERTICAL:
    {
        displayText.back().setRotation(-90);
        sf::FloatRect textBounds = displayText.back().getGlobalBounds();
        displayText.back().setPosition(std::round(position.x + textBounds.width),
                                       std::round(position.y + textBounds.height));
        break;
    }
    default:
    {
        displayText.back().setPosition(std::round(position.x),
                                       std::round(position.y));
        break;
    }
    }

    if(bWrapText)
    {
        cvis::wrapText(displayText.back(), bounds, textPadding);
    }
    if(bExpand) updateBounds();

}

void CVTextBox::setFont(const string& font)
{
    if(appFont(font))
    {
        textInfo.font = font;
        textFont = appFont(font);

        for(auto& text : displayText)
        {
            text.setFont(*textFont);
        }

         if(bWrapText)
        {
            wrapText();
        }
        else
        {
            alignText();
        }
    }
}

void CVTextBox::setFont(const sf::Font& font)
{
    textFont = &font;

    for(auto& text : displayText)
    {
        text.setFont(font);
    }

    if(bWrapText)
    {
        wrapText();
    }
    else
    {
        alignText();
    }
}

void CVTextBox::setFontSize(const unsigned int& newSize)
{

    textInfo.fontSize = newSize;
    for(auto& text : displayText)
    {
        text.setCharacterSize(newSize);
    }

}

void CVTextBox::setSize(const sf::Vector2f& newSize)
{
    CVBox::setSize(newSize);
    if(bWrapText) wrapText();
    alignText();
}

void CVTextBox::setPosition(const sf::Vector2f& position)
{
    CVTextBox::move(position - getPosition());
}

void CVTextBox::move(const sf::Vector2f& offset)
{
    if(bStatic) return;

    sf::Vector2i initPos = iDrawPos;

    CVBox::move(offset);

    sf::Vector2f roundedOffset(iDrawPos - initPos);

    for(auto& text : displayText)
    {
        text.move(roundedOffset);
    }
}

bool CVTextBox::draw(sf::RenderTarget* target)
{
    if(!CVShape::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    if(bDropShadow)
    {
        target->draw(dropShadow);
    }

    if(!bSpriteOnly)
    {
        for(auto& item : panel)
        {
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& sprite : spriteList)
    {
        target->draw(sprite);
    }

    for(auto& text : displayText)
    {
        target->draw(text);
    }

    if(is_closable())
    {
        closeButton->draw(target);
    }

    if(!active)
    {
        target->draw(inactiveMask);
    }

    CV_DRAW_CLIP_END

    if(bHasShadow)
    {
        if(View->viewPort && View->viewPort->isOpen())
        {
            View->viewPort->draw(shadow);
        }
        else
        {
            target->draw(shadow);
        }
    }

    return true;
}

bool CVTextBox::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBox::update(event, mousePos)) return false;

    if(bFade && (fadeLayers & CV_LAYER_TEXT))
    {
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*60.0f/View->getFrameRate());
        for(auto& text : displayText)
        {
            tmp = text.getFillColor();
            if(tmp.a != targetAlpha)
            {
                if(targetAlpha > tmp.a)
                {
                    if(tmp.a + adjusted_fr < targetAlpha)
                    {
                        tmp.a += adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
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

    return true;
}

sf::FloatRect CVTextBox::getTextBounds() const noexcept
{
    if(displayText.empty())
    {
        return sf::FloatRect(0, 0, 0, 0);
    }
    else if(displayText.size() == 1)
    {
        return displayText.front().getGlobalBounds();
    }
    else
    {
        sf::FloatRect output = displayText.front().getGlobalBounds();

        for(size_t i = 1; i < displayText.size(); ++i)
        {
            expandBounds(output, displayText[i].getGlobalBounds());
        }

        return output;
    }
}

void CVTextBox::setText(const unsigned int& textIndex, const char* newText)
{
    if(displayText.size() > textIndex)
    {
        string newTextStr(newText);
        displayText[textIndex].setString(UTF8_to_UTF16(newText));
        if(bWrapText) wrapText();
        else alignText();
    }
}

void CVTextBox::setText(const unsigned int& textIndex, const string& newText)
{
    if(displayText.size() > textIndex)
    {
        displayText[textIndex].setString(UTF8_to_UTF16(newText));
        if(bWrapText) wrapText();
        else alignText();
    }
}

void CVTextBox::setText(const unsigned int& textIndex, const wchar_t* newText)
{
    if(displayText.size() > textIndex)
    {
        displayText[textIndex].setString(newText);
        if(bWrapText) wrapText();
        else alignText();
    }
}

void CVTextBox::setText(const unsigned int& textIndex, const sf::String& newText)
{
    if(displayText.size() > textIndex)
    {
        displayText[textIndex].setString(newText);
        if(bWrapText) wrapText();
        else alignText();
    }
}

CVText::CVText(CVView* View,
               const TextEntry& textInfo,
               const sf::Vector2f& position,
               const sf::Vector2f& size,
               const bool& bWrap):
                   CVTextBox(View,
                             position,
                             size.x,
                             size.y,
                             textInfo,
                             sf::Color::Transparent,
                             sf::Color::Transparent,
                             0.0f)
{
    if(bWrap) setTextWrap(true);
    alignText();
}

}
