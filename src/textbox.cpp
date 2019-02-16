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

#include "EZC/toolkit/string.hpp"

using namespace EZC;

namespace cvis
{

CVTextBox::CVTextBox(CVView* View,
                     sf::Vector2f position, float width, float height,
                     sf::Color fillColor,
                     sf::Color borderColor,
                     float borderWidth):
    CVBox(View, position, width, height,
          fillColor, borderColor, borderWidth),

    textInfo("", getDefaultFont(), 12),
    textPadding(0.0f),
    textFont(nullptr),
    bWrapText(false)
{
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
    bWrapText(false)
{

    addTextEntry(textInfo, position);

    alignText();

}

void CVTextBox::alignText()
{
    sf::FloatRect textBounds;
    sf::Vector2f textPos = getPosition();
    unsigned int numText = displayText.size() + 1,
                 i = 1;

    for(auto& text : displayText)
    {;

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

        text.setPosition(textPos);
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
            std::string displayTextString = text.getString();
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
                        else if(isCharType(displayTextString[j-1], ",\\/:._-"))
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

    if(bExpand) updateBounds();
    alignText();
}

void CVTextBox::setString(const std::string& newString)
{
    if(displayText.size() > 0)
    {
        displayText.front().setString(newString);
        if(bWrapText) wrapText();
    }
    alignText();
}

void CVTextBox::fitText()
{
    if(displayText.empty())
    {
        setSize(sf::Vector2f(0,0));
    }
    else
    {
        sf::FloatRect textBounds = displayText.front().getGlobalBounds();
        expandBounds(textBounds, textPadding);
        CVBox::setPosition(sf::Vector2f(textBounds.left, textBounds.top));
        CVBox::setSize(sf::Vector2f(textBounds.width, textBounds.height));
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
    std::string cpyData = textInfo.text;
    event.sendData(cpyData.c_str(), cpyData.size(), CV_EVENT_LMB);
}

std::string CVTextBox::getText() const
{
    std::string newLine;

    if(displayText.empty()) return newLine;

    std::stringstream output;

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
    return output.str();
}

const sf::Color& CVTextBox::getTextColor() const
{
    return textInfo.textColor;
}

void CVTextBox::addTextEntry(textEntry newText, float padding, bool regular)
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
            newText.text,
            *appFont(newText.font),
            newText.fontSize
        );
    }
    else
    {
        displayText.emplace_back();
        displayText.back().setString(newText.text);
        displayText.back().setCharacterSize(newText.fontSize);
    }

    displayText.back().setFillColor(newText.textColor);
    colorTheme.emplace_back(newText.textColor);
    displayText.back().setPosition(newTextPosition);

    sf::Rect<float> newTextBounds = displayText.back().getLocalBounds();
    newTextPosition = getTextHorizontalAlignment(displayText.back(), bounds, newText.alignment);

    switch(textAlignment)
    {
    case TEXT_ALIGN_VERTICAL:
    {
        displayText.back().setPosition(newTextPosition.x,
                                       newTextPosition.y + lastTextBounds.height + padding);
        break;
    }
    case TEXT_ALIGN_HORIZONTAL:
    {
        displayText.back().setPosition(newTextPosition.x + lastTextBounds.width + padding,
                                       newTextPosition.y);
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

void CVTextBox::addTextEntry(const textEntry& newText, const sf::Vector2f& position)
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
        displayText.emplace_back(newText.text,
                                 *mainApp()->fonts[newText.font],
                                 newText.fontSize);
    }
    else
    {
        displayText.emplace_back();
        displayText.back().setString(newText.text);
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
        displayText.back().setPosition(position.x, position.y - textBounds.height/2 - getTextCenterOffsetY(displayText.back())/2);
        break;
    }
    case ALIGN_LEFT_BOTTOM:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(position.x, position.y - textBounds.height);
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(position.x - textBounds.width/2, position.y - textBounds.height/2);
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(position.x - textBounds.width/2, position.y - textBounds.height);
        break;
    }
    case ALIGN_CENTER:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(position.x - textBounds.width/2,
                                       position.y);
        break;
    }
    case ALIGN_RIGHT:
    {
        sf::FloatRect textBounds = displayText.back().getLocalBounds();
        displayText.back().setPosition(position.x - textBounds.width, position.y);
        break;
    }
    case ALIGN_VERTICAL:
    {
        displayText.back().setRotation(-90);
        sf::FloatRect textBounds = displayText.back().getGlobalBounds();
        displayText.back().setPosition(position.x + textBounds.width, position.y + textBounds.height);
        break;
    }
    default:
    {
        displayText.back().setPosition(position.x, position.y);
        break;
    }
    }

    if(bWrapText)
    {
        cvis::wrapText(displayText.back(), bounds, textPadding);
    }
    if(bExpand) updateBounds();

}

void CVTextBox::setFont(const std::string& font)
{
    if(appFont(font))
    {
        textInfo.font = font;
        textFont = appFont(font);

        for(auto& text : displayText)
        {
            text.setFont(*textFont);
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
}

void CVTextBox::setPosition(const sf::Vector2f& position)
{
    CVTextBox::move(position - getPosition());
}

void CVTextBox::move(const sf::Vector2f& offset)
{
    if(bStatic) return;
    CVBox::move(offset);
    for(auto& text : displayText)
    {
        text.move(offset);
    }
}

bool CVTextBox::draw(sf::RenderTarget* target)
{
    if(!CVShape::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

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

    if(bFade)
    {
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());
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

    return true;
}

void CVTextBox::setText(const unsigned int textIndex, std::string newText)
{
    if(displayText.size() > textIndex)
    {
        displayText[textIndex].setString(newText);
        if(bWrapText) wrapText();
        else alignText();
    }
}

CVText::CVText(CVView* View,
               const textEntry& textInfo,
               const sf::Vector2f& position,
               const sf::Vector2f& size,
               const bool& bWrap):
                   CVTextBox(View, position, size.x, size.y, textInfo,
                             sf::Color::Transparent, sf::Color::Transparent,
                             0.0f)
{
    if(bWrap) setTextWrap(true);
    alignText();
    bNoInteract = true; // Can be changed if flags are set
}

}
