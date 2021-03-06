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

#include "cvision/word.hpp"
#include "cvision/algorithm.hpp"
#include "cvision/event.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include "hyper/toolkit/string.hpp"

using namespace hyperC;
using namespace std;

namespace cvis
{

CVTypePanel::CVTypePanel(CVView* parentView, const TextEntry& textInfo,
                         const string& panelTag,
                         const sf::Color& color,
                         const sf::Vector2f& size,
                         const vector<float>& margins,
                         const sf::Vector2f& position):
    CVViewPanel(parentView, panelTag, color, size, (isnan(size.x) || isnan(size.y)), position),
    margins(margins),
    scrollBarPadding(right_margin()/2),
    paragraphSpacing(12.0f * viewScale()),
    highlightColors(
{
    sf::Color(255,0,0,128), sf::Color(255,165,0,128), sf::Color(255,255,0,128)
}),
scrollBarY(parentView,
           sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                        bounds.top + top_margin() + scrollBarPadding),
           sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                        bounds.top + bounds.height - bottom_margin() - scrollBarPadding),
           size.y - top_margin() - bottom_margin(), 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
           1000.0f, CV_OBJ_ANIM_FADE_IN),
marginLines(4),
bShowMarginLines(true),
bCanEdit(true)
{

    this->textInfo = textInfo;
    addTextElement(sf::Vector2f(0.0f,0.0f), textInfo);
    scrollBarY.setPanelSize(bounds.height - top_margin() - bottom_margin());
    scrollBarY.setPosition(sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                        bounds.top + top_margin() + scrollBarPadding));
    scrollBarY.setRounding(2.5f);

    left_margin_line().setSize(sf::Vector2f(2.0f, bounds.height));
    left_margin_line().setOrigin(sf::Vector2f(1.0f, 0.0f));
    left_margin_line().setPosition(bounds.left + left_margin(), bounds.top);
    left_margin_line().setFillColor(sf::Color(200,200,200));

    top_margin_line().setSize(sf::Vector2f(bounds.width, 2.0f));
    top_margin_line().setOrigin(sf::Vector2f(0.0f, 1.0f));
    top_margin_line().setPosition(bounds.left, bounds.top + top_margin());
    top_margin_line().setFillColor(sf::Color(200,200,200));

    right_margin_line().setSize(sf::Vector2f(2.0f, bounds.height));
    right_margin_line().setOrigin(sf::Vector2f(1.0f, 0.0f));
    right_margin_line().setPosition(bounds.left + bounds.width - right_margin(), bounds.top);
    right_margin_line().setFillColor(sf::Color(200,200,200));

    bottom_margin_line().setSize(sf::Vector2f(bounds.width, 2.0f));
    bottom_margin_line().setOrigin(sf::Vector2f(0.0f, 1.0f));
    bottom_margin_line().setPosition(bounds.left, bounds.top + bounds.height - bottom_margin());
    bottom_margin_line().setFillColor(sf::Color(200,200,200));
}

CVTypePanel::~CVTypePanel()
{
    for(auto& text : typeElements)
    {
        delete(text);
    }
}

void CVTypePanel::clear()
{
    CVViewPanel::clear();
    for(auto& text : typeElements)
    {
        delete(text);
    }
    typeElements.clear();
}

void CVTypePanel::set_left_margin(const float& newMargin)
{
    margins[0] = newMargin;
    left_margin_line().setPosition(bounds.left + left_margin(), bounds.top);
}

void CVTypePanel::set_top_margin(const float& newMargin)
{
    margins[1] = newMargin;
    top_margin_line().setPosition(bounds.left, bounds.top + top_margin());
    scrollBarY.setAnchorPoints(sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + top_margin() + scrollBarPadding),
                               sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + bounds.height - bottom_margin() - scrollBarPadding));
    scrollBarY.setPanelSize(bounds.height - top_margin() - bottom_margin());
}

void CVTypePanel::set_right_margin(const float& newMargin)
{
    margins[2] = newMargin;
    right_margin_line().setPosition(bounds.left + bounds.width - right_margin(), bounds.top);
}

void CVTypePanel::set_bottom_margin(const float& newMargin)
{
    margins[3] = newMargin;
    top_margin_line().setPosition(bounds.left, bounds.top + top_margin());
    scrollBarY.setAnchorPoints(sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + top_margin() + scrollBarPadding),
                               sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + bounds.height - bottom_margin() - scrollBarPadding));
    scrollBarY.setPanelSize(bounds.height - top_margin() - bottom_margin());
}

void CVTypePanel::show_margins()
{
    bShowMarginLines = true;
}

void CVTypePanel::hide_margins()
{
    bShowMarginLines = false;
}

bool CVTypePanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVViewPanel::update(event, mousePos)) return false;

    sf::FloatRect itemBounds = get_element_bounds(typeElements);
    scrollBarY.setScrollMax(itemBounds.height);

    if(bounds.contains(mousePos) && event.focusFree())
    {
        setFocus(true);
        scrollBarY.setScrollable(true);
    }
    else
    {
        setFocus(false);
        scrollBarY.setScrollable(false);
    }

    scrollBarY.update(event, mousePos);
    for(auto& text : typeElements)
    {
        text->update(event, mousePos);
    }

    for(auto& item : viewPanelElements)
    {
        item->update(event, mousePos);
    }

    if(bounds.contains(mousePos)) event.captureFocus();

    if(hasFocus() && event.keyPressed)
    {
        for(auto& text : typeElements)
        {
            if(text->hasFocus())
            {
                if(text->getCursorPos().y + text->getCursorSize().y > bounds.top + bounds.height - top_margin() - bottom_margin())
                {
                    scrollBarY.scroll(-(bounds.top + bounds.height - bottom_margin() - itemBounds.top - itemBounds.height));
                }
            }
        }
    }

    if(itemBounds.height > bounds.height - top_margin() - bottom_margin())
    {
        float moveDist = top_margin() + bounds.top -
                         itemBounds.top -
                         scrollBarY.getScrollOffset();
        for(auto& text : typeElements)
        {
            text->move(sf::Vector2f(0.0f, moveDist));
        }
        for(auto& element : viewPanelElements)
        {
            element->move(sf::Vector2f(0.0f, moveDist));
        }
        top_margin_line().move(sf::Vector2f(0.0f,moveDist));
        bottom_margin_line().setPosition(sf::Vector2f(bounds.left, itemBounds.top + itemBounds.height));
    }
    else
    {
        float minY = NAN;
        unsigned int minIndex = 0, i = 0;
        for(auto& text : typeElements)
        {
            if(isnan(minY) || text->getPosition().y < minY)
            {
                minY = text->getPosition().y;
                minIndex = i;
            }
            ++i;
        }
        float moveDist = bounds.top + top_margin() - typeElements[minIndex]->getPosition().y;
        for(auto& text : typeElements)
        {
            text->move(sf::Vector2f(0.0f, moveDist));
        }
        for(auto& element : viewPanelElements)
        {
            element->move(sf::Vector2f(0.0f, moveDist));
        }

        top_margin_line().setPosition(sf::Vector2f(bounds.left, bounds.top + top_margin()));
        bottom_margin_line().setPosition(sf::Vector2f(bounds.left, bounds.top + bounds.height - bottom_margin()));
    }

    // Draw highlights if waiting queue

    if(event.viewHasFocus && (event.keyLog.size() > 0))
    {
        highlight.clear();
    }

    while(waitingHighlightText.size() > 0)
    {

        unsigned int matchIndex = UINT_MAX, S, i, j, k = 0;
        string strBuff;

        for(auto& text : typeElements)
        {

            strBuff = text->getDisplayString();
            S = strBuff.size();
            matchIndex = findString(strBuff, waitingHighlightText.front(), CMP_STR_CASE_INSENSITIVE | CMP_STR_SIZE_INSENSITIVE);

            while(matchIndex != UINT_MAX)
            {

                i = matchIndex;
                j = i;

                highlight.emplace_back(sf::Vector2f(0.0f, 20.0f*textInfo.fontSize/12));
                highlight.back().setFillColor(highlightColors.front());
                highlight.back().setOrigin(0.0f, getTextCenterOffsetY(text->getTextElement()));
                highlight.back().setPosition(text->getTextElement().findCharacterPos(matchIndex));

                while((i < S) && (j < S) && (i < waitingHighlightText.front().size()))
                {

                    ++j;

                    if(strBuff[j] == '\n')
                    {

                        highlight.back().setSize(sf::Vector2f(text->getTextElement().findCharacterPos(k + j).x -
                                                              text->getTextElement().findCharacterPos(k + i).x,
                                                              highlight.back().getSize().y));
                        i = j+1;

                        highlight.emplace_back();

                        highlight.back().setFillColor(highlightColors.front());
                        highlight.back().setOrigin(0.0f, getTextCenterOffsetY(text->getTextElement()));
                        highlight.back().setPosition(text->getTextElement().findCharacterPos(k + i));

                    }

                }

                highlight.back().setSize(sf::Vector2f(text->getTextElement().findCharacterPos(k + j).x -
                                                      text->getTextElement().findCharacterPos(k + i).x,
                                                      highlight.back().getSize().y));

                strBuff.erase(strBuff.begin(), strBuff.begin() + j);

                S -= j;
                matchIndex = findString(strBuff, waitingHighlightText.front(), CMP_STR_CASE_INSENSITIVE | CMP_STR_SIZE_INSENSITIVE);
                k += j;

            }

        }

        waitingHighlightText.erase(waitingHighlightText.begin());

    }

    return true;
}

bool CVTypePanel::draw(sf::RenderTarget* target)
{
    if(!CVTextBox::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    if(bShowMarginLines)
    {
        for(auto& line : marginLines)
        {
            target->draw(line);
        }
    }
    for(auto& item : highlight)
    {
        target->draw(item);
    }
    for(auto& text : typeElements)
    {
        text->draw(target);
    }
    for(auto& item : viewPanelElements)
    {
        item->draw(target);
    }
    scrollBarY.draw(target);

    CV_DRAW_CLIP_END

    return true;
}

void CVTypePanel::addTextElement(const sf::Vector2f& position, TextEntry textInfo)
{

    if(textInfo.font.empty())
    {
        textInfo.font = this->textInfo.font;
        textInfo.fontSize = 13;
        textInfo.alignment = ALIGN_LEFT;
        textInfo.textColor = sf::Color(40,40,40);
        textInfo.textStyle = sf::Text::Regular;
        if(textInfo.font.empty())
        {
            textInfo.font = mainApp()->getDefaultFont();
        }
    }

    typeElements.emplace_back(new CVTypeBox(View, sf::Vector2f(position.x + bounds.left + left_margin(),
                                            position.y + bounds.top + top_margin()),
                                            bounds.width - right_margin() - left_margin(), 20.0f*textInfo.fontSize/12,
                                            textInfo, sf::Color::Transparent, textInfo.textColor, 0.0f, CV_CURSOR_ANIM_FADE,
                                            CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));

    if(!bCanEdit)
    {
        typeElements.back()->disable_edit();
    }

    typeElements.back()->setExpand(true);

}

void CVTypePanel::getText(StringVector& output) const
{
    for(auto& text : typeElements)
    {
        output.emplace_back(text->getTypeString());
    }
}

string CVTypePanel::getText() const
{
    stringstream ss;
    for(size_t i = 0, L = typeElements.size(); i < L; ++i)
    {
        ss << typeElements[i]->getTypeString();
        if(i < L - 1)
        {
            ss << '\n';
        }
    }
    return ss.str();
}

void CVTypePanel::setText(const StringVector& newText)
{
    for(size_t i = 0; (i < typeElements.size()) && (i < newText.size()); ++i)
    {
        typeElements[i]->setTypeString(newText[i]);
        switch(typeElements[i]->getTextInfo().alignment)
        {
        case ALIGN_LEFT_BOTTOM:
        {
            typeElements[i]->move(bounds.left + left_margin() - typeElements[i]->getBounds().left, 0.0f);
            break;
        }
        case ALIGN_LEFT_MIDLINE:
        {
            typeElements[i]->move(bounds.left + left_margin() - typeElements[i]->getBounds().left, 0.0f);
            break;
        }
        default:  // Left top
        {
            typeElements[i]->move(bounds.left + left_margin() - typeElements[i]->getBounds().left, 0.0f);
            break;
        }
        }
    }
    scrollBarY.setScrollOffset(0.0f);
}

void CVTypePanel::setText(const unsigned int& index, const std::string& newText)
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested text edit for index out of range of available elements");
    }

    typeElements[index]->setTypeString(newText);
}

void CVTypePanel::clearText()
{
    if(!typeElements.empty())
    {
        while(typeElements.size() > 1)
        {
            delete(typeElements.back());
            typeElements.pop_back();
        }
        typeElements.front()->clearTypeString();
    }
}

void CVTypePanel::setTextSize(const unsigned int& newSize)
{
    for(auto& text : typeElements)
    {
        text->setFontSize(newSize);
    }
    textInfo.fontSize = newSize;
}

void CVTypePanel::setTextColor(const sf::Color& newColor)
{
    for(auto& text : typeElements)
    {
        text->setTextColor(newColor);
    }
    textInfo.textColor = newColor;
}

void CVTypePanel::enable_edit()
{
    for(auto& text : typeElements)
    {
        text->enable_edit();
    }
    bCanEdit = true;
}

void CVTypePanel::enable_edit(const unsigned int& index)
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested edit access to index out of range of available elements");
    }

    typeElements[index]->enable_edit();
}

void CVTypePanel::disable_edit()
{
    for(auto& text : typeElements)
    {
        text->disable_edit();
    }
    bCanEdit = false;
}

void CVTypePanel::disable_edit(const unsigned int& index)
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested edit access to index out of range of available elements");
    }

    typeElements[index]->disable_edit();
}

sf::Vector2f CVTypePanel::getTextElementPosition(const unsigned int& index) const
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested position for index out of range of available elements");
    }

    return typeElements[index]->getPosition();
}

sf::FloatRect CVTypePanel::getTextElementBounds(const unsigned int& index) const
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested position for index out of range of available elements");
    }

    return typeElements[index]->getBounds();
}

void CVTypePanel::setTextElementPadding(const unsigned int& index, const float& newPadding)
{
    if(index > numTextElements())
    {
        throw std::out_of_range("CVTypePanel: requested text padding change for index out of range of available elements");
    }

    return typeElements[index]->setTextPadding(newPadding);
}

bool CVTypePanel::setHighlight(const StringVector& query)
{
    append(waitingHighlightText, query);
    string strBuff;
    for(auto& text : typeElements)
    {
        strBuff = text->getTypeString();
        for(auto& term : query)
        {
            if(cmpString(term, strBuff, CMP_STR_CASE_INSENSITIVE | CMP_STR_SIZE_INSENSITIVE))
            {
                return true;
            }
        }
    }

    return false;
}

void CVTypePanel::setHighlightableStatus(const bool& status)
{
    CVViewPanel::setHighlightableStatus(status);
    for(auto& text : typeElements)
    {
        text->setHighlightableStatus(status);
    }
}

void CVTypePanel::move(const sf::Vector2f& distance)
{
    CVViewPanel::move(distance);
    for(auto& line : marginLines)
    {
        line.move(distance);
    }
    for(auto& item : highlight)
    {
        item.move(distance);
    }
    for(auto& text : typeElements)
    {
        text->move(distance);
    }
    scrollBarY.move(distance);
}

void CVTypePanel::setPosition(const sf::Vector2f& position)
{
    sf::Vector2f distance = position - getPosition();
    move(distance);
}

void CVTypePanel::setSize(const sf::Vector2f& newSize)
{
    CVViewPanel::setSize(newSize);

    left_margin_line().setSize(sf::Vector2f(left_margin_line().getSize().x,
                                            newSize.y));
    left_margin_line().setPosition(bounds.left + left_margin(), bounds.top);
    right_margin_line().setSize(sf::Vector2f(right_margin_line().getSize().x,
                                newSize.y));
    right_margin_line().setPosition(bounds.left + bounds.width - right_margin(), bounds.top);
    top_margin_line().setSize(sf::Vector2f(newSize.x,
                                           top_margin_line().getSize().y));
    top_margin_line().setPosition(bounds.left, bounds.top + top_margin());
    bottom_margin_line().setSize(sf::Vector2f(newSize.x,
                                 bottom_margin_line().getSize().y));
    bottom_margin_line().setPosition(bounds.left, bounds.top + bounds.height - bottom_margin());

    scrollBarY.setAnchorPoints(sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + top_margin() + scrollBarPadding),
                               sf::Vector2f(bounds.left + bounds.width - right_margin() + scrollBarPadding,
                                            bounds.top + bounds.height - bottom_margin() - scrollBarPadding));
    scrollBarY.setPanelSize(bounds.height - top_margin() - bottom_margin());
}

void CVTypePanel::arrange_media()
{

    if(spriteList.empty() || !numTextElements())
    {
        return;
    }

    sf::FloatRect textBounds;
    sf::FloatRect spriteBounds;
    sf::FloatRect elementBounds;
    sf::FloatRect otherBounds;

    sf::Vector2f textCenter;

    for(auto& text : typeElements)
    {

        textBounds = text->getBounds();
        expandBounds(textBounds, textPadding);
        textCenter = getBoundCenter(textBounds);

        for(auto& sprite : spriteList)
        {

            if(sprite.getColor().a < 50) // Anything less than this is a watermark
            {
                continue;
            }

            spriteBounds = sprite.getGlobalBounds();

            if(textBounds.intersects(spriteBounds))
            {

                if(spriteBounds.left > textCenter.x)
                {
                    float moveDist = text->getSize().y;

                    text->setSize(sf::Vector2f(spriteBounds.left - textPadding - textBounds.left,
                                               text->getSize().y));

                    moveDist = text->getSize().y - moveDist;

                    for(auto& o_text : typeElements)
                    {
                        if(o_text == text) continue;

                        if(o_text->getPosition().y >= text->getPosition().y)
                        {
                            o_text->move(sf::Vector2f(0.0f, moveDist));
                        }

                    }
                }

            }

        }

        textBounds = text->getBounds();
        expandBounds(textBounds, textPadding);
        textCenter = getBoundCenter(textBounds);

        for(auto& element : viewPanelElements)
        {
            elementBounds = element->getGlobalBounds();

            if(textBounds.intersects(elementBounds))
            {

                if(elementBounds.left > textCenter.x)
                {
                    float moveDist = text->getSize().y;

                    text->setSize(sf::Vector2f(elementBounds.left - textPadding - textBounds.left,
                                               text->getSize().y));

                    moveDist = text->getSize().y - moveDist;

                    for(auto& o_text : typeElements)
                    {
                        if(o_text == text) continue;

                        if(o_text->getPosition().y >= text->getPosition().y)
                        {
                            o_text->move(sf::Vector2f(0.0f, moveDist));
                        }

                    }
                }

            }
        }

    }

}

void CVTypePanel::arrange_media(const sf::FloatRect& boundary)
{

    if(spriteList.empty() || !numTextElements())
    {
        return;
    }

    sf::FloatRect textBounds;
    sf::FloatRect otherBounds;

    sf::Vector2f textCenter;

    for(auto& text : typeElements)
    {

        textBounds = text->getBounds();
        expandBounds(textBounds, paragraphSpacing);
        textCenter = getBoundCenter(textBounds);

        if(textBounds.intersects(boundary))
        {

            if(boundary.left > textCenter.x)
            {

                text->setSize(sf::Vector2f(boundary.left - paragraphSpacing - textBounds.left,
                                           text->getSize().y));

            }

        }

    }

    for(auto& text : typeElements)
    {

        textBounds = text->getBounds();

        for(auto& o_text : typeElements)
        {
            if(o_text == text) continue;

            otherBounds = o_text->getBounds();

            if(textBounds.intersects(otherBounds) &&
                    (o_text->getPosition().y >= text->getPosition().y))
            {
                o_text->move(sf::Vector2f(0.0f,
                                          textBounds.top +
                                          textBounds.height +
                                          otherBounds.top));
            }

        }
    }

}

}

