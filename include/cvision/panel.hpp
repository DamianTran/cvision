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

#ifndef CVIS_PANEL
#define CVIS_PANEL

#include "cvision/lib.hpp"

#include "cvision/button.hpp"
#include "cvision/widgets.hpp"
#include "cvision/type.hpp"
#include "cvision/viewpanel.hpp"

#include <chrono>

#define CV_CTRL_FWD                 "forward"
#define CV_CTRL_REV                 "back"

#define CV_ASSEMBLE_NONE            0b0
#define CV_ASSEMBLE_HORIZONTAL      0b1
#define CV_ASSEMBLE_VERTICAL        0b10

#define CV_PANEL_CONSTRUCTOR(class)     \
    class(cvis::CVView* parentView, const std::string& panelTag = "",                       \
          const sf::Color& backgroundColor = sf::Color::Transparent,                        \
          const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,      \
          const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f))

#define CV_DEFINE_PANEL_CONSTRUCT(class)    \
    class::class(CVView* parentView, const std::string& panelTag,               \
                 const sf::Color& backgroundColor, const sf::Vector2f& size,    \
                 bool bFitToWindow, const sf::Vector2f& position):              \
                    CVBasicViewPanel(parentView, panelTag, backgroundColor,     \
                                       size, bFitToWindow, position)

namespace cvis{

class CVISION_API CVBasicViewPanel: public CVViewPanel{
public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API CVBasicViewPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

class CVISION_API CVSwitchPanel: public CVBasicViewPanel{ // Allows for switching of view focus between panel elements
protected:
    int viewIndex;

    bool bCanPan,
        bAutoPan,
        bCenterOnNew,
        bClosablePanels,
        bLoop,
        bTimed;

    float panRate,
            elementPadding,
            autoSwitchLatency;

    std::chrono::high_resolution_clock::time_point timeLastSwitch;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVElement* getActiveElement();
    CVISION_API std::string getFocusTag();

    inline void enableAutoSwitch()
    {
        bTimed = true;
    }
    inline void disableAutoSwitch()
    {
        bTimed = false;
    }

    inline void setAutoSwitchTime(const float& newTime)
    {
        bTimed = newTime > 0.0f;
        autoSwitchLatency = newTime;
    }

    inline void setSwitchLoopable(const bool& status = true)
    {
        bLoop = status;
    }

    void setCenter(const int& index);
    inline void switch_to(const int& index)
    {
        setCenter(index);
    }

    CVISION_API void setCenter(const std::string& tag);
    void switch_to(const std::string& tag)
    {
        setCenter(tag);
    }

    CVISION_API void switch_next();
    CVISION_API void switch_previous();

    CVISION_API void setPanRate(const float& newRate);
    CVISION_API bool checkFocus(const std::string& tag); // Check if the view is focused on a tagged panel

    CVISION_API void setElementPadding(const float& newPadding);

    CVISION_API void addPanelElement(CVElement* newElement, std::string newTag = "", const unsigned int& index = UINT_MAX);
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void enablePanning(); // Allow the user to pan the panel around
    CVISION_API void disablePanning();

    CVISION_API void enableAutoPan();
    CVISION_API void disableAutoPan();

    inline void setClosablePanels(const bool& state = true)
    {
        bClosablePanels = state;
    }

    inline void setCenterOnNew(const bool& state = true)
    {
        bCenterOnNew = state;
    }

    CVISION_API CVSwitchPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

class CVISION_API CVNodePanel : public CVBasicViewPanel{ // Panel items are distributed radially around source
protected:
    CVElement* center;
    int nodeIndex;
    unsigned int maxTraceLength;
    uint8_t nodeAlpha;
    bool bAutoPan,
        bNodeOpened,
        bNodeInteracted,
        bShowTagOnHover;
    float panRate,
            nodeDistance, // Distance where potential is zero
            elasticCoef, // Proportional to force tugging nodes together
            pathThickness;

    sf::Vector2f anchorPoint;

    std::vector<sf::RectangleShape> nodePaths;
    std::vector<CVNodePanel*> branches;
    std::vector<sf::Text> hoverTags;

    std::vector<bool> pathSelected;     // Record of selected paths

    std::vector<std::string> interactionTrace;

    void logInteraction(const std::string& tag);

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setHoverTag(const bool& status);

    CVISION_API void setNodeAlpha(const uint8_t& newAlpha);
    inline const bool& isOpen() const{ return bNodeOpened; }

    CVISION_API void setNode(const int& node);
    CVISION_API void setNodeRadius(const float& newRadiusPx);

    inline sf::Vector2f getPosition() const{
        if(center != nullptr) return center->getPosition();
        return anchorPoint; }

    CVISION_API std::string lastInteracted() const;
    CVISION_API bool captureInteraction(std::string* output = nullptr);

    CVISION_API void setCenter(CVElement& element); // Set the element to be centered
    CVISION_API void setCenterAnchor(const sf::Vector2f& position); // Set the anchor point to center the node map

    CVISION_API void addPanelElement(CVElement* newElement, std::string newTag = "");
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API CVNodePanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

class CVISION_API CVListPanel : public CVBasicViewPanel{
protected:

    float listPadding,
            scrollBarPadding,
            outerPadding,
            innerPadding,
            listItemHeight;

    CVScrollBar scrollBarY;

    bool bSelect;

    std::reference_wrapper<sf::Color> listItemColor;
    std::vector<std::string> listTags;

public:

    std::vector<CVElement*> selected;

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    inline const float& getListPadding() const{ return listPadding; }
    inline const float& getScrollBarPadding() const{ return scrollBarPadding; }
    inline const float& getOuterPadding() const{ return outerPadding; }
    inline const float& getInnerPadding() const{ return innerPadding; }

    CVISION_API void setListPadding(const float& newPadding);
    CVISION_API void setScrollBarPadding(const float& newPadding);
    CVISION_API void setOuterPadding(const float& newPadding);
    CVISION_API void setInnerPadding(const float& newPadding);

    CVISION_API void setDragAndDrop(const bool& state);

    const sf::Color& getListItemColor() const{ return listItemColor.get(); }
    inline void setListItemColor(const sf::Color& newColor){ listItemColor.get() = newColor; }

    CVISION_API void addTextEntry(const std::string& newText, const unsigned int& index = UINT_MAX);
    CVISION_API void addTextEntries(const std::vector<std::string>& newText);
    CVISION_API void setTextEntries(const std::vector<std::string>& newText);
    CVISION_API void removeTextEntry(const unsigned int& index);
    CVISION_API bool textEntryExists(const std::string& text) const;

    CVISION_API void setElementPadding(const float& newPadding);

    CVISION_API void setSelectionStatus(const bool& status);
    inline const bool& canSelect() const{ return bSelect; }
    CVISION_API void toggleSelection(CVElement* element);
    CVISION_API void toggleSelection(const std::string& tag);
    CVISION_API void setSelection(CVElement* element, const bool& state = true);
    CVISION_API void setSelection(const std::string& tag, const bool& state = true);
    CVISION_API std::vector<std::string> getSelection() const;

    CVISION_API void clearSelection();

    CVISION_API void addPanelElement(CVElement*, std::string newTag = "", const unsigned int& index = UINT_MAX);
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API CVListPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

class CVISION_API CVMapPanel: public CVBasicViewPanel{
protected:

    sf::Rect<int> coord_bounds; // Virtual coordinate bounds
    sf::FloatRect outer_bounds,
                    init_bounds; // Outer boundary where interactions become effective

    sf::Vector2i click_coords,
                release_coords; // Interactive tracking

    bool select_rows, // Allow selection of rows or columns from boundary
        row_selected,
        select_cols,
        col_selected,
        displayInfo,    // Show extra info at bottom
        coords_captured;    // Check if coords have been read since last update

    float outerSelectDist, // Distance from outer border where row-wise selection is instigated
            zoomLevel,
            zoomMax, zoomMin;
    unsigned int textEntryIndex,
                    extraInfoIndex;

    sf::RectangleShape highlight,
                        selection;
    rounded_rectangle base;

    class zone : public sf::RectangleShape{
    public:

        CVMapPanel* parent;

        sf::Rect<int> bounds;
        std::string name;

        void updateCoords(const sf::Rect<int>& newCoords);
        void updateCoords(const sf::Vector2i& newCoords);
        void updateCoords();

        zone(CVMapPanel* parent);
        zone(CVMapPanel* parent, const std::string& name,
             const sf::Rect<int>& bounds, const sf::Color& color);
    };

    std::vector<zone> zones; // Zones making up this map

    sf::Text& extraInfoText();
    sf::Text& titleText();

public:

    inline const sf::Rect<int>& getCoordBounds() const{ return coord_bounds; }

    void setZoneColor(const std::string& tag, const sf::Color& newColor);
    void setZoneColor(const unsigned int& index, const sf::Color& newColor);

    unsigned int numZones() const;
    bool zoneExists(const std::string& tag) const;

    void addZone(const std::string& name, const sf::Color& newColor,
                 const sf::Rect<int>& bounds);
    void removeZone(const std::string& name);
    void removeZone(const unsigned int& index);

    void updateZone(const std::string& name, const sf::Vector2i& newCoords);
    void updateZone(const std::string& name, const sf::Rect<int>& newCoords);
    void updateZone(const unsigned int& index, const sf::Vector2i& newCoords);
    void updateZone(const unsigned int& index, const sf::Rect<int>& newCoords);

    void setZoneImage(const std::string& name, const sf::Texture& image);
    void setZoneImage(const unsigned int& index, const sf::Texture& image);

    void setRowSelect(const bool& status = true);
    void setColumnSelect(const bool& status = true);
    void showExtraInfo(const bool& status = true);

    void setCoords(const sf::Rect<int>& coords,
                   const bool& morph = true);
    void setOuterPadding(const float& padding);
    void setZoom(float newZoom, sf::Vector2f mousePos = sf::Vector2f(NAN, NAN));

    sf::Rect<int> selected_bounds() const;
    inline const sf::Vector2i select_begin() const{ return click_coords; }
    inline const sf::Vector2i select_end() const{ return release_coords; }
    bool sectionSelected() const;
    bool pointSelected() const;

    std::string getTitle() const;
    void setTitle(const std::string& newTitle);

    std::string getInfo() const;

    bool captureCoords(sf::Rect<int>& output); // Attempt to read new coords

    void move(const sf::Vector2f& distance);
    void move(const float& x, const float& y);

    void shiftMap(const sf::Vector2f& distance);
    void setCenter(const sf::Vector2i& coords);

    void setPosition(const sf::Vector2f& position);
    void setPosition(const float& x, const float& y);

    void setBaseColor(const sf::Color& color);
    sf::Color baseColor() const;

    sf::FloatRect basePanelBounds() const;

    void clear();

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    sf::Vector2i globalToCoords(const sf::Vector2f& position) const;
    sf::Vector2f coordsToGlobal(const sf::Vector2i& coords) const;

    CVMapPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

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

    class assembly : public CVBox{
    protected:

        CVAssemblyPanel* host;
        float itemPadding,
            outerPadding;

        CVTypeBox label;
        CVButton acceptButton,
                cancelButton;

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

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    void setTracks(const bool& status);

    bool getAssemblyInfo(std::ostream& output);
    void addAssemblyItem(const sf::Vector2f& position, const std::string& label = "", const unsigned int& state = 0);

    void newAssembly(const std::vector<CVElement*>& members = std::vector<CVElement*>());
    void detachMember(CVElement* member);       // Detach from all assemblies
    assembly* getAssembly(CVElement* member);   // Get the assembly this member is a part of if applicable
    void removeAssembly(assembly* asmb);

    inline void setAssemblyType(const unsigned char& newType){ assemblyType = newType; }

    void setPanelItemSize(const sf::Vector2f& newSize);
    void setMaxLabelLength(const unsigned int& newLength);

    void getAssemblyTags(std::vector<std::vector<std::string>>& output);

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }
    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVAssemblyPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
    ~CVAssemblyPanel();

};

class CVISION_API CVButtonPanel: public CVBasicViewPanel{
protected:

    std::string interactTag;
    sf::Vector2i gridSize;

    void organizeGrid();
    sf::Vector2f getGridPosition(const sf::Vector2i& coord) const;

    std::vector<std::vector<CVButton*>> buttonGrid;

    float outerPadding,
            innerPadding;

public:

    std::string getInteraction();
    bool checkInteraction() const;

    void setGrid(const sf::Vector2i& size);
    void setGrid(const unsigned int& x, const unsigned int& y);

    void setOuterPadding(const float& value);
    void setInnerPadding(const float& value);

    void addButton(const sf::Vector2i& coord,
                   const std::string& tag = "",
                   const std::string& icon = "",
                   const std::string& text = "",
                   const unsigned int& numStates = 2,
                   const unsigned int& initialState = 0);

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    CVButtonPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_PANEL
