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

#include "cvision/widgets/dropdown.hpp"

#include "cvision/panel/list.hpp"
#include "cvision/button.hpp"
#include "cvision/type.hpp"
#include "cvision/event.hpp"

#include "cvision/algorithm.hpp"

#include <hyper/toolkit/string.hpp>

using namespace hyperC;
using namespace std;

namespace cvis
{

CVDropDownBox::CVDropDownBox(CVView * View,
                             const string& tag,
                             const vector<string>& selection,
                             const sf::Vector2f& position,
                             const sf::Vector2f& size,
                             const TextEntry& textInfo,
                             const sf::Color& fillColor,
                             const sf::Color& buttonFillColor,
                             const sf::Color& outlineColor,
                             const float& outlineThickness,
                             const float& rounding,
                             const float& maxDropDownHeight,
                             const string& dropDownIcon):
                                 CVBasicViewPanel(View, tag,
                                                  sf::Color::Transparent,
                                                  size, false,
                                                  position),
                                 bDropDownVisible(false),
                                 bTypeEntry(true),
                                 bAutoFill(true),
                                 bAutoScroll(true),
                                 fMaxDropDownHeight(maxDropDownHeight),
                                 fListItemSpacing(textInfo.fontSize * 1.2f),
                                 selection_list(selection),
                                 selected_index(0)
{

    setOutOfBoundsDraw(true);
    setOutOfBoundsUpdate(true);

    this->textInfo = textInfo;

    float fButtonWidth = size.y < size.x * 0.5 ? size.y : size.x * 0.5f;
    float fDialogWidth = size.x - fButtonWidth - outlineThickness;
    float fDialogHeight = size.y;

    vector<bool> dialogRoundingStates({ true, false, false, true });
    vector<bool> buttonRoundingStates({ false, true, true, false });
    vector<bool> menuRoundingStates({ false, false, true, true });

    dropDownDialog = new CVTypeBox(View,
                                   position,
                                   fDialogWidth,
                                   fDialogHeight,
                                   textInfo,
                                   fillColor,
                                   outlineColor,
                                   outlineThickness,
                                   CV_OBJ_ANIM_SLIDE,
                                   CV_TEXT_FIT_LATERAL);

    dropDownDialog->setRounding(rounding, 12, dialogRoundingStates);
    dropDownDialog->setClearOnKeyboardEntry(false);
    dropDownDialog->setHighlightableStatus(false);

    dropDownBtn = new CVButton(View,
                                sf::Vector2f(position.x + fDialogWidth + outlineThickness,
                                             position.y),
                                fButtonWidth,
                                fDialogHeight,
                                TextEntry("", textInfo.font, textInfo.fontSize,
                                          textInfo.alignment, textInfo.textColor,
                                          textInfo.textStyle),
                                dropDownIcon,
                                buttonFillColor,
                                outlineColor,
                                outlineThickness);

    dropDownBtn->setSpriteScale(0.7f);
    dropDownBtn->setHighlightColor(buttonFillColor + sf::Color(50, 50, 50, 0));

    sf::Color buttonSpriteColor = buttonFillColor;
    contrast_brightness(buttonSpriteColor, buttonFillColor, 80);

    dropDownBtn->setSpriteColor(buttonSpriteColor);
    dropDownBtn->setRounding(rounding, 12, buttonRoundingStates);
    dropDownBtn->setMask(appTexture("gradient_linear"), sf::Color(255, 255, 255, 120));

    dropDownMenu = new CVListPanel(View,
                                   tag + " Menu",
                                   fillColor,
                                   sf::Vector2f(fDialogWidth,
                                                fMaxDropDownHeight),
                                   false,
                                   sf::Vector2f(dropDownDialog->getPosition().x,
                                                dropDownDialog->getPosition().y +
                                                dropDownDialog->getSize().y/2));
    dropDownMenu->setVisible(false);
    dropDownMenu->setDrawClipping(true);
    dropDownMenu->setHighlightableStatus(false);
    dropDownMenu->setDragAndDrop(false);
    dropDownMenu->setOutlineColor(outlineColor);
    dropDownMenu->setOutlineThickness(outlineThickness);
    dropDownMenu->setListPadding(0.0f);
    dropDownMenu->setInnerPadding(0.0f);
    dropDownMenu->setOuterPadding(fDialogHeight/2 + fListItemSpacing/2);
    dropDownMenu->setRounding(rounding, 12, menuRoundingStates);

    for(auto& entry : selection_list)
    {
        addMenuItem(entry);
    }

    addPanelElement(dropDownMenu,       tag + " Menu");
    addPanelElement(dropDownDialog,     tag + " Dialog");
    addPanelElement(dropDownBtn,        tag + " Button");

}

void CVDropDownBox::addMenuItem(const std::string& newEntry)
{

    CVButton * newMenuItem = new CVButton(View,
                                          getPosition(),
                                          dropDownMenu->getSize().x,
                                          fListItemSpacing*1.5,
                                          TextEntry(newEntry, textInfo.font, textInfo.fontSize,
                                                    textInfo.alignment, textInfo.textColor),
                                          "",
                                          dropDownMenu->getFillColor(),
                                          sf::Color::Transparent,
                                          0.0f);
    newMenuItem->setTextPadding(12.0f);

    sf::Color highlightColor = dropDownMenu->getOutlineColor() + sf::Color(40, 40, 40, 0);
    contrast_brightness(highlightColor, textInfo.textColor, 80);

    newMenuItem->setHighlightColor(highlightColor);

    dropDownMenu->addPanelElement(newMenuItem, newEntry);
    selection_list.emplace_back(newEntry);

}

void CVDropDownBox::setList(const vector<string>& list_elements) noexcept
{
    dropDownMenu->clear();
    selection_list.clear();

    for(auto& element : list_elements)
    {
        addMenuItem(element);
    }
}

string CVDropDownBox::getSelected() const
{

    if(selected_index < selection_list.size())
    {
        return selection_list[selected_index];
    }
    else if(!selection_list.empty())
    {
        return selection_list.front();
    }
    else
    {
        return string();
    }

}

void CVDropDownBox::setSelected(const string& value)
{

    setSelected(getMatchingIndex(value, selection_list, CMP_STR_DEFAULT | CMP_STR_SW, 0.5f));

}

void CVDropDownBox::setSelected(const unsigned int& index)
{

    if(index < selection_list.size())
    {

        selected_index = index;

        dropDownDialog->setTypeString(selection_list[index]);

    }
    else
    {
        selected_index = 0;

        if(!selection_list.empty())
        {
            dropDownDialog->setTypeString(selection_list.front());
        }

    }

    sendTriggers();

}

void CVDropDownBox::openMenu()
{
    if(!bDropDownVisible)
    {
        bDropDownVisible = true;
        dropDownBtn->getSprite(0).rotate(180);
        dropDownMenu->setVisible(true);

        dropDownMenu->fitElements(false, true);

        if(dropDownMenu->getSize().y > fMaxDropDownHeight)
        {
            dropDownMenu->setPanelSize(sf::Vector2f(dropDownMenu->getSize().x,
                                                    fMaxDropDownHeight));
            dropDownMenu->setPosition(dropDownDialog->getPosition().x,
                                      dropDownDialog->getPosition().y +
                                      dropDownDialog->getSize().y/2);
        }

        if(selected_index < selection_list.size())
        {
            dropDownMenu->getElements()[selected_index]->highlight(true);
        }
    }
}

void CVDropDownBox::closeMenu()
{
    if(bDropDownVisible)
    {
        bDropDownVisible = false;
        dropDownBtn->getSprite(0).rotate(180);
        dropDownMenu->setVisible(false);

        for(auto& element : dropDownMenu->getElements())
        {
            element->highlight(false);
        }
    }
}

void CVDropDownBox::toggleMenu()
{

    if(bDropDownVisible)
    {
        closeMenu();
    }
    else
    {
        openMenu();
    }

}

bool CVDropDownBox::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    if(dropDownBtn->getTrigger())
    {
        toggleMenu();
    }

    if(bDropDownVisible &&
       event.LMBreleased &&
       (event.LMBreleaseFrames == 1))
    {

        if(dropDownMenu->getBounds().contains(mousePos))
        {
            for(size_t i = 0; i < dropDownMenu->numPanels(); ++i)
            {
                if(dropDownMenu->getElements()[i]->getBounds().contains(mousePos))
                {

                    setSelected(i);
                    closeMenu();

                    break;
                }
            }
        }
        else if(!bounds.contains(mousePos) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            closeMenu();
        }

    }

    if(bTypeEntry)
    {
        if(dropDownDialog->checkStringEntered())
        {
            string entered_string;
            dropDownDialog->getEnteredString(entered_string);

            setSelected(entered_string);

            closeMenu();
        }

        if(dropDownDialog->checkTypeStringChanged())
        {

            if(bAutoFill || bAutoScroll)
            {

                unsigned int bestMatchIndex = getMatchingIndex(dropDownDialog->getTypeString(),
                                                               selection_list,
                                                               CMP_STR_DEFAULT |
                                                               CMP_STR_SW,
                                                               0.5f);

                if(bestMatchIndex != UINT_MAX)
                {

                    if(bAutoFill)
                    {

                        dropDownDialog->make_suggestion(selection_list[bestMatchIndex]);

                    }
                    if(bAutoScroll && bDropDownVisible)
                    {

                        for(size_t i = 0; i < dropDownMenu->numPanels(); ++i)
                        {
                            if(i == bestMatchIndex)
                            {
                                dropDownMenu->getElements()[i]->highlight(true);
                            }
                            else
                            {
                                dropDownMenu->getElements()[i]->highlight(false);
                            }
                        }

                    }

                }
                else if(bDropDownVisible)
                {
                    for(auto& element : dropDownMenu->getElements())
                    {
                        element->highlight(false);
                    }
                }

            }

        }
    }

    return true;

}

}
