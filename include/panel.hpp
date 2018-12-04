/* =========================================================================
 ===========================================================================
 ===========================================================================

		     AiDA - The Artificial Intelligence Desktop Assistant

	         Copyright (C) 2018 Damian Tran.  All rights reserved.

                          - See AiDA-licence.txt -

 ===========================================================================
 ===========================================================================
 ===========================================================================

 AiDA incorporates technologies held under the following copyrights:

 OpenSSL - Copyright (c) 1998-2018 The OpenSSL Project

 The Boost C++ Library:

          Copyright Joe Coder 2004 - 2006.
 Distributed under the Boost Software License, Version 1.0.
      (See https://www.boost.org/LICENSE_1_0.txt)

 CVision - Copyright (c) 2017 - 2018 Damian Tran (see CVlicence.txt)
                                                                            */

#pragma once

#ifndef CVIS_PANEL
#define CVIS_PANEL

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_buttonIO.hpp"
#include "cvision/widgets.hpp"
#include "cvision/cvis_typeIO.hpp"

#define CV_CTRL_FWD                 "forward"
#define CV_CTRL_REV                 "back"

#define CV_ASSEMBLE_NONE            0b0
#define CV_ASSEMBLE_HORIZONTAL      0b1
#define CV_ASSEMBLE_VERTICAL        0b10

namespace cvis{

class CVBasicViewPanel: public CVViewPanel{
public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    CVBasicViewPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f)):
        CVViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position){
        }
};

class CVSwitchPanel: public CVBasicViewPanel{ // Allows for switching of view focus between panel elements
protected:
    int viewIndex;
    bool bCanPan,
        bAutoPan;
    float panRate,
            elementPadding;

    std::vector<CVButton> closeButtons;

public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    CVElement* getActiveElement();

    void setCenter(const int& index);
    void setCenter(const std::string& tag);
    void setPanRate(const float& newRate);
    bool checkFocus(const std::string& tag); // Check if the view is focused on a tagged panel

    void setElementPadding(const float& newPadding);

    void addPanelElement(CVElement* newElement, std::string newTag = "", const unsigned int& index = UINT_MAX);
    void removePanelElement(const unsigned int& index);

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void enablePanning(); // Allow the user to pan the panel around
    void disablePanning();

    CVSwitchPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

class CVNodePanel : public CVBasicViewPanel{ // Panel items are distributed radially around source
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

    AIALG::StringVector interactionTrace;

    void logInteraction(const std::string& tag);

public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    void setHoverTag(const bool& status);

    void setNodeAlpha(const uint8_t& newAlpha);
    inline const bool& isOpen() const{ return bNodeOpened; }

    void setNode(const int& node);
    void setNodeRadius(const float& newRadiusPx);

    inline sf::Vector2f getPosition() const{
        if(center != nullptr) return center->getPosition();
        return anchorPoint; }

    std::string lastInteracted() const;
    bool captureInteraction(std::string* output = nullptr);

    void setCenter(CVElement& element); // Set the element to be centered
    void setCenterAnchor(const sf::Vector2f& position); // Set the anchor point to center the node map

    void addPanelElement(CVElement* newElement, std::string newTag = "");
    void removePanelElement(const unsigned int& index);

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVNodePanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

class CVListPanel : public CVBasicViewPanel{
protected:

    float listPadding,
            scrollBarPadding,
            outerPadding,
            innerPadding,
            listItemHeight;

    CVScrollBar scrollBarY;

    bool bSelect;

    std::reference_wrapper<sf::Color> listItemColor;
    AIALG::StringVector listTags;

public:

    std::vector<CVElement*> selected;

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    inline const float& getListPadding() const{ return listPadding; }
    inline const float& getScrollBarPadding() const{ return scrollBarPadding; }
    inline const float& getOuterPadding() const{ return outerPadding; }
    inline const float& getInnerPadding() const{ return innerPadding; }

    void setListPadding(const float& newPadding);
    void setScrollBarPadding(const float& newPadding);
    void setOuterPadding(const float& newPadding);
    void setInnerPadding(const float& newPadding);

    void setDragAndDrop(const bool& state);

    const sf::Color& getListItemColor() const{ return listItemColor.get(); }
    inline void setListItemColor(const sf::Color& newColor){ listItemColor.get() = newColor; }

    void addTextEntry(const std::string& newText, const unsigned int& index = UINT_MAX);
    void addTextEntries(const AIALG::StringVector& newText);
    void setTextEntries(const AIALG::StringVector& newText);
    void removeTextEntry(const unsigned int& index);
    inline bool textEntryExists(const std::string& text) const{ return AIALG::anyEqual(text, listTags); }

    void setElementPadding(const float& newPadding);

    void setSelectionStatus(const bool& status);
    inline const bool& canSelect() const{ return bSelect; }
    void toggleSelection(CVElement* element);
    void toggleSelection(const std::string& tag);
    void setSelection(CVElement* element, const bool& state = true);
    void setSelection(const std::string& tag, const bool& state = true);
    AIALG::StringVector getSelection() const;

    void clearSelection();

    void addPanelElement(CVElement*, std::string newTag = "", const unsigned int& index = UINT_MAX);
    void removePanelElement(const unsigned int& index);

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVListPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

class CVMapPanel: public CVBasicViewPanel{
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

class CVAssemblyPanel : public CVBasicViewPanel{
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

    void getAssemblyTags(AIALG::StringMatrix& output);

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

class CVButtonPanel: public CVBasicViewPanel{
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
