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

#include "cvision/panel/toggle.hpp"

#include <boost/range/adaptor/reversed.hpp>

using namespace std;
using namespace boost;

namespace cvis
{

CVTogglePanel::CVTogglePanel(CVView* parentView, const string& panelTag, const sf::Color& backgroundColor,
                     const sf::Vector2f& size, const bool& bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         active_index(0),
                         bUpdateNonFocused(false)
{



}

bool CVTogglePanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVTextBox::update(event, mousePos))
    {
        return false;
    }

    if(active_index < numPanels())
    {
        viewPanelElements[active_index]->update(event, mousePos);
    }

    if(bUpdateNonFocused)
    {
        for(int i = int(numPanels()) - 1; i >= 0; --i)
        {
            if(size_t(i) == active_index)
            {
                continue;
            }
            viewPanelElements[i]->update(event, mousePos);
        }
    }

    if(trigger_waiting())
    {
        string trigger = take_trigger("SwitchTo");
        if(!trigger.empty())
        {
            switch_to(trigger);
        }
    }

    return false;
}

void CVTogglePanel::switch_to(const size_t& index)
{
    for(size_t i = 0; i < numPanels(); ++i)
    {
        if(i == index)
        {
            viewPanelElements[i]->setVisible(true);
        }
        else
        {
            viewPanelElements[i]->setVisible(false);
        }
    }

    active_index = index;
}

void CVTogglePanel::switch_to(const string& tag)
{

    for(size_t i = 0; i < numPanels(); ++i)
    {
        if(viewPanelElements[i]->tag() == tag)
        {
            viewPanelElements[i]->setVisible(true);
            active_index = i;
        }
        else
        {
            viewPanelElements[i]->setVisible(false);
        }
    }

}

void CVTogglePanel::addPanelElement(CVElement* element,
                                    const string& tag,
                                    const unsigned int& index)
{
    CVViewPanel::addPanelElement(element, tag, index);

    if(active_index >= index)
    {
        ++active_index;
    }

    if(numPanels() > 1)
    {
        if(index < numPanels())
        {
            viewPanelElements[index]->setVisible(false);
        }
        else
        {
            viewPanelElements.back()->setVisible(false);
        }
    }
}


}
