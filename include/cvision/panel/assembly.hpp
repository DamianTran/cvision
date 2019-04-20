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

#pragma once

#ifndef CVIS_ASSEMBLY_PANEL
#define CVIS_ASSEMBLY_PANEL

#include "cvision/panel.hpp"
#include "cvision/widgets.hpp"
#include "cvision/type.hpp"
#include "cvision/button.hpp"

namespace cvis
{

class CVISION_API CVAssemblyPanel : public CVBasicViewPanel{
protected:

    float scrollBarPadding,
            snapDistance;

    CVScrollBar scrollBarY;

    unsigned char assemblyType; // Direction of assembly

    unsigned int maxLabelLength;

    bool bUseTracks,        // Assemble on regularly-spaced linear tracks
        bUseLabels;         // Include editable labels on the assembly groups

    sf::Vector2f itemSize;

    std::vector<CVElement*> selected;

    std::vector<std::string> assemblyStateTags;     // The state information associated with new assembly items
    uint8_t                  defaultAssemblyState;  // The state to set new assembly items to

    class assembly : public CVBox{
    protected:

        CVAssemblyPanel*    host;

        float               itemPadding;
        float               outerPadding;

        CVTypeBox           label;

        CVButton            acceptButton;
        CVButton            cancelButton;

        std::vector<std::stringstream> assembly_info;

    public:

        std::vector<CVElement*> members;

        bool getAssemblyInfo(std::ostream& output);

        inline size_t size() const{ return members.size(); }
        inline bool empty() const{ return members.empty(); }

        bool update(CVEvent& event, const sf::Vector2f& position);
        bool draw(sf::RenderTarget* target);

        void addMember(CVElement* element, const unsigned int& index = UINT_MAX);
        void addMemberBefore(CVElement* newMember, CVElement* otherMember);
        void addMemberAfter(CVElement* newMember, CVElement* otherMember);

        void setItemPadding(const float& newPadding);
        void setOuterPadding(const float& newPadding);

        inline const float& getItemPadding() const{ return itemPadding; }
        inline const float& getOuterPadding() const{ return outerPadding; }

        void swapMembers(CVElement* firstMember, CVElement* otherMember);

        void removeMember(CVElement* element);
        void shiftMember(CVElement* element, const unsigned int& index);

        assembly(const std::vector<CVElement*>& members, CVAssemblyPanel& host);
    };

    std::vector<assembly*> groups;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setTracks(const bool& status);

    CVISION_API bool getAssemblyInfo(std::ostream& output);
    CVISION_API void addAssemblyItem(const sf::Vector2f& position, const std::string& label = "", const unsigned int& state = 0);

    CVISION_API void setAssemblyTags(const std::vector<std::string>& newTags);
    CVISION_API void setInitialAssemblyState(const uint8_t& initialState);

    CVISION_API void newAssembly(const std::vector<CVElement*>& members = std::vector<CVElement*>());
    CVISION_API void detachMember(CVElement* member);       // Detach from all assemblies
    CVISION_API assembly* getAssembly(CVElement* member);   // Get the assembly this member is a part of if applicable
    CVISION_API void removeAssembly(assembly* asmb);

    inline void setAssemblyType(const unsigned char& newType){ assemblyType = newType; }

    CVISION_API void setPanelItemSize(const sf::Vector2f& newSize);
    CVISION_API void setMaxLabelLength(const unsigned int& newLength);

    CVISION_API void getAssemblyTags(std::vector<std::vector<std::string>>& output);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }
    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API CVAssemblyPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
    CVISION_API ~CVAssemblyPanel();

};

}

#endif // CVIS_ASSEMBLY_PANEL
