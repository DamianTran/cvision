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

#ifndef CVIS_NETWORK_PANEL
#define CVIS_NETWORK_PANEL

#include <unordered_map>
#include <unordered_set>

#include "cvision/panel.hpp"

#include <hyper/toolkit/reference_vector.hpp>
#include <hyper/toolkit/static_vector.hpp>
#include <hyper/toolkit/thread.hpp>

namespace cvis
{

class CVNetworkEdge;
class CVNetworkPanel;
class CVButton;
class CVTypeBox;
class CVNetworkGroup;

/** Wrapper class for element embedded within network panel.
    All items added to CVNetworkPanel are implicitly wrapped
    in a network node class if they are not already wrapped.
    It's easiest to wrap an item before adding it to the network
    panel, but wrapping after is possible. */

class CVISION_API CVNetworkNode
{
public:

    CVISION_API CVNetworkNode(CVElement* element,
                              const std::string& newType = "",
                              const float& weight = 1.0f,
                              const sf::Font* font = nullptr,
                              const unsigned int& baseCharacterSize = 14u,
                              const unsigned int& alignment = ALIGN_CENTER_MIDLINE,
                              const sf::Color& textColor = sf::Color::Black);

    CVISION_API ~CVNetworkNode();

    CVISION_API void connect_out(CVNetworkNode& other,
                                 const float& weight = 1.0f,
                                 const sf::Color& edgeColor = sf::Color::Black,
                                 const std::string& type = "");         // Outbound connection
    CVISION_API void connect_in(CVNetworkNode& other,
                                const float& weight = 1.0f,
                                const sf::Color& edgeColor = sf::Color::Black,
                                const std::string& type = "");          // Inbound connection
    CVISION_API void connect_with(CVNetworkNode& other,
                                  const float& weight = 1.0f,
                                  const sf::Color& edgeColor = sf::Color::Black,
                                  const std::string& type = "");        // Bidirectional connection

    CVISION_API void remove_connections(CVNetworkNode& other);
    CVISION_API void disconnect();

    inline void pin() noexcept{ bPinned = true; }
    inline void unpin() noexcept{ bPinned = false; }
    inline const bool& isPinned() const noexcept{ return bPinned; }

    CVISION_API CVNetworkEdge& getConnection(CVElement* other);
    CVISION_API CVNetworkEdge& getConnection(CVNetworkNode& other);
    CVISION_API CVNetworkEdge& getConnection(const std::string& tag);

    CVISION_API CVNetworkEdge& getOutConnection(const size_t& index);
    CVISION_API CVNetworkEdge& getInConnection(const size_t& index);

    CVISION_API bool hasConnectionTo(const CVNetworkNode& node);

    inline size_t numOutEdges() const noexcept{ return connected_to.size(); }
    inline size_t numInEdges() const noexcept{ return connected_from.size(); }
    inline size_t numEdges() const noexcept{ return numInEdges() + numOutEdges(); }

    inline bool hasConnections() const noexcept{ return numEdges() > 0; }

    inline CVElement * const getElement() noexcept{ return element; }
    inline const CVElement * const getElement() const noexcept{ return element; }
    inline const std::string& getTag() const noexcept{ return element->tag(); }
    inline const std::string& getType() const noexcept{ return type; }
    inline const float& getWeight() const noexcept{ return weight; }

    CVISION_API void setWeight(const float& newWeight,
                               const bool& rescale = true) noexcept;
    inline void setType(const std::string& newType) noexcept{ type = newType; }
    CVISION_API void setTag(const std::string& newTag) noexcept;

    inline void setTextDisplayOffset(const sf::Vector2f& newOffset) noexcept{ textDisplayOffset = newOffset; }

    inline void setDisplayTextOnHover(const bool& state = true) noexcept{ bDisplayTextOnHover = state; }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void push(const float& angle, const float& velocity, const float& drag)
    {
        getElement()->push(angle, velocity, drag);
    }
    CVISION_API void setPosition(const sf::Vector2f& newPosition);

    inline const sf::FloatRect& getBounds() const noexcept{ return getElement()->getBounds(); }
    inline sf::Vector2f getSize() noexcept{ return element->getSize(); }
    inline sf::Vector2f getPosition() const noexcept{ return getElement()->getPosition(); }
    inline const bool& isVisible() const{ return getElement()->isVisible(); }
    inline void setVisible(const bool& state){ getElement()->setVisible(state); }

    CVISION_API void setScale(const float& newScale);
    CVISION_API void setViewScale(const float& newScale);
    CVISION_API void setSprite(const sf::Texture* newTexture);

    inline void setTextPadding(const float& padding) noexcept{ textPadding = padding; }
    inline void setTextFadeRate(const uint8_t& rate) noexcept{ textFadeRate = rate; }

    inline void setHighlightColor(const sf::Color& newColor) noexcept{ element->setHighlightColor(newColor); }

    CVISION_API void setSelected(const bool& status = true);
    inline const bool& isSelected() const noexcept{ return bSelected; }

    CVISION_API void update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API void updateInterface(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API void draw(sf::RenderTarget* target);
    CVISION_API void drawEdges(sf::RenderTarget* target,
                               const unsigned int& LOD);
    CVISION_API void drawInterface(sf::RenderTarget* target);

    CVISION_API void attach_UI(CVElement* newUI,
                               const uint8_t& alpha = 255,
                               const unsigned char& fadeLayers = CV_LAYER_ALL);
    CVISION_API void remove_UI(const bool& fadeout = true);
    inline void setRemoveUIOnDeselect(const bool& state = true){ bUIremoveOnDeselect = state; }
    inline void setUISizeScale(const float& newScale){ fUISizePaddingScale = newScale; }

    inline bool hasUI() const noexcept{ return attached_UI != nullptr; }

    CVISION_API void apply_tethers(const float& distance,
                                   const float& elastic_coefficient,
                                   const float& range_threshold,
                                   const float& friction) noexcept;

    CVISION_API void addGroup(CVNetworkGroup& group) noexcept;
    CVISION_API void removeGroup(CVNetworkGroup& group) noexcept;

    inline size_t numGroups() const noexcept{ return groups.size(); }

    CVISION_API bool checkGroup(CVNetworkGroup& group) const noexcept;
    inline bool isGrouped() const noexcept{ return !groups.empty(); }

    CVISION_API bool checkGroups(const CVNetworkNode& other) const noexcept;

protected:

    friend class CVNetworkPanel;
    friend class CVNetworkEdge;

    CVISION_API void align_text(const unsigned int& alignment);

    /** Attached UI that fades in on selection and out on deselection */

    CVElement*      attached_UI;

    uint8_t         UI_alpha;

    bool            bUIFadeout;
    bool            bUIremove;
    bool            bUIremoveOnDeselect;
    bool            bStatic;        // Ignore physics
    bool            bPinned;

    float           fUISizePaddingScale;

    unsigned char   fadeLayers;

    /** @brief Groups that this node is a member of. */
    std::vector<CVNetworkGroup*> groups;

private:

    CVElement* element;

    sf::Text displayText;

    std::string type;

    std::vector<CVNetworkEdge> connected_to;
    std::vector<CVNetworkEdge> connected_from;

    float weight;
    float textPadding;
    float fScale;
    float fViewScale;
    float fScaleTransform;

    sf::Vector2f lastElementPosition;
    sf::Vector2f textDisplayOffset;

    bool bDisplayTextOnHover;
    bool bSelected;

    uint8_t targetTextAlpha;
    uint8_t textFadeRate;

    unsigned int textAlignment;

};

/** Wrapper class for CVNetworkPanel edges */

class CVISION_API CVNetworkEdge
{
public:

    CVISION_API CVNetworkEdge(CVNetworkNode& origin,
                              CVNetworkNode& outNode,
                              const std::string& type = "",
                              const float& weight = 1.0f,
                              const sf::Color& edgeColor = sf::Color::Black);

    inline const std::string& getType() const noexcept{ return type; }

    inline CVNetworkNode& getNode(){ return *node; }
    inline CVNetworkNode& getOrigin(){ return *origin; }
    inline CVElement * const getElement() noexcept{ return node->getElement(); }
    inline const std::string& getTag() noexcept{ return node->getTag(); }

    inline void setWeight(const float& newWeight) noexcept{ weight = newWeight; }
    inline const float& getWeight() const noexcept{ return weight; }

    CVISION_API void setLineWidth(const float& newWidth) noexcept;
    inline const float& getLineWidth() const noexcept{ return lineWidth; }
    inline void setViewScale(const float& newViewScale) noexcept{ fViewScale = newViewScale; }
    inline void setLineColor(const sf::Color& newColor) noexcept
    {
        line.setFillColor(newColor);
    }
    inline void setLineWeightScale(const float& newScale) noexcept{ lineWeightScale = newScale; }

    /** Apply one frame of tether push/pull to distance (0 = no push) */
    CVISION_API void apply_tether(const float& distance,
                                  const float& elastic_coefficient,
                                  const float& range_threshold,
                                  const float& friction);

    CVISION_API void draw(sf::RenderTarget* target);
    CVISION_API void update(CVEvent& event, const sf::Vector2f& mousePos);

protected:

    CVNetworkNode* origin;
    CVNetworkNode* node;

    std::string type;

    float weight;
    float lineWeightScale;
    float lineWidth;
    float fViewScale;

private:

    sf::RectangleShape line;

    sf::Text displayText;
    sf::FloatRect displayBounds;

    bool bTextVisible;

};

class CVISION_API CVNetworkGroup : public CVBasicViewPanel
{
public:

    enum class Mode
    {
        Boxed,
        Centered
    };

    CVNetworkGroup(CVView * View,
                   const hyperC::reference_vector<CVNetworkNode>& nodes,
                   const Mode& mode,
                   const TextEntry& textInfo,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& outlineColor = sf::Color::Black,
                   const float& outlineThickness = 2.0f,
                   const float& rounding = 0.0f);

    ~CVNetworkGroup();

    CV_UPDATE_OVERRIDE;

    CVISION_API void setSize(const sf::Vector2f& newSize) override;

    CVISION_API void addNode(CVNetworkNode& node);
    CVISION_API void removeNode(CVNetworkNode& node);
    CVISION_API bool checkNode(CVNetworkNode& node) const noexcept;

    inline bool empty() const noexcept{ return nodes.empty(); }
    inline void clear() noexcept{ nodes.clear(); }
    inline size_t size() const noexcept{ return nodes.size(); }

    enum class AnnotationDirection
    {
        TopLeft = 0,
        TopCenter,
        TopRight,
        Right,
        BottomRight,
        BottomCenter,
        BottomLeft,
        Left,
        Center
    };

    CVISION_API void setGroupMode(const CVNetworkGroup::Mode& newMode) noexcept;

    inline const std::vector<std::string>& getKeyWords() const noexcept{ return key_words; }

    /** @brief Obtain the letter-wise alignment score between all key words of this and
      * another group.
      *
      * Assesses the maximum soft match of each key word in this group to the other,
      * and returns the average.
      */
    CVISION_API float checkKeyWords(const CVNetworkGroup& other) const noexcept;
    CVISION_API float checkKeyWords(const std::vector<std::string>& key_words) const noexcept;

    friend void merge(hyperC::reference_vector<CVNetworkGroup>& groups,
                      CVNetworkGroup& output);

protected:

    friend class CVNetworkPanel;

    Mode groupMode;

    CVTypeBox * annotation;

    AnnotationDirection annotation_dir;

    hyperC::reference_vector<CVNetworkNode> nodes;

    std::vector<std::string> sub_groups;
    std::vector<std::string> key_words;

    CVISION_API void updateAnnotationDir(const sf::Vector2f& relative_center) noexcept;

};

enum class CVNetworkLayout
{
    None = 0,
    Force,
    Organic
};

template<typename T>
struct prohibited_function_type : std::false_type
{

};

/** Mindmap-type physics-enabled panel */

class CVISION_API CVNetworkPanel : public CVBasicViewPanel
{
public:

    CVISION_API CVNetworkPanel(CVView* View,
                   const std::string& tag = "",
                   const sf::Vector2f& position = sf::Vector2f(0.0f, 0.0f),
                   const sf::Vector2f& size = sf::Vector2f(0.0f, 0.0f),
                   const sf::Color& backgroundColor = sf::Color::White,
                   const sf::Color& outlineColor = sf::Color::Transparent,
                   const TextEntry& textInfo = TextEntry("", ""),
                   const float& outlineThickness = 0.0f);

    ~CVNetworkPanel();

    CVISION_API CV_UPDATE_OVERRIDE;
    CVISION_API CV_DRAW_OVERRIDE;

    CVISION_API void getTexture(sf::Texture& output,
                                const sf::Color& canvas_color = sf::Color::Transparent) override;

    // Manual addition/addition of special elements to network

    /** This method is deprecated in the network panel to avoid confusion */

    template<typename T = bool>
    void addPanelElement(CVElement* newElement,
                         const std::string& newTag = "",
                         const unsigned int& index = UINT_MAX)
    {
        static_assert(prohibited_function_type<T>::value, "Default addPanelElement() is prohibited for CVNetworkPanel");
    }

    /** Add a new custom element.  The element will be wrapped in a CVNetworkNode class and
        added to the registry of nodes for this panel. */

    CVISION_API void addPanelElement(CVElement* newElement,
                                     const std::string& newTag = "",
                                     const std::string& newType = "",
                                     const float& weight = 1.0f,
                                     const unsigned int& label_orientation = ALIGN_CENTER_MIDLINE);

    CVISION_API void removePanelElement(CVElement* element) override;

    // Add/remove nodes

    /** Add a shape node with templated color settings */
    CVISION_API CVButton* addNode(const std::string& tag,
                                  const std::string& type = "",
                                  const float& weight = 1.0f,
                                  const sf::Vector2f& position = sf::Vector2f(NAN, NAN),
                                  const sf::Vector2f& size = sf::Vector2f(NAN, NAN));

    /** Add a shape node with specific color settings */
    CVISION_API CVButton* addNode(const std::string& tag,
                                  const std::string& type,
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size,
                                  const sf::Color& fillColor,
                                  const sf::Color& outlineColor = sf::Color::Black,
                                  const float& outlineThickness = 1.0f,
                                  const float& rounding = NAN);

    /** Add a sprite node with templated color settings */
    CVISION_API CVButton* addImageNode(const std::string& tag,
                                       const std::string& texture,
                                       const std::string& type = "",
                                       const float& weight = 1.0f,
                                       const sf::Vector2f& position = sf::Vector2f(NAN, NAN),
                                       const sf::Vector2f& size = sf::Vector2f(NAN, NAN));

    /** Add a sprite node with specific color settings */
    CVISION_API CVButton* addImageNode(const std::string& tag,
                                       const std::string& texture,
                                       const std::string& type,
                                       const float& weight,
                                       const sf::Vector2f& position,
                                       const sf::Vector2f& size,
                                       const sf::Color& spriteColor,
                                       const sf::Color& fillColor = sf::Color::Transparent,
                                       const sf::Color& outlineColor = sf::Color::Transparent,
                                       const float& outlineThickness = 1.0f,
                                       const float& rounding = NAN);

    CVISION_API virtual void removeNodesByTag(const std::string& tag);
    CVISION_API void removeNodesByTags(const std::vector<std::string>& tags);
    CVISION_API void removeNode(CVNetworkNode& node);
    CVISION_API virtual void removeSelected();

    CVISION_API bool nodeExists(const std::string& node) const noexcept;

    // Connection

    /** Connect nodes with source tag to nodes with target tag */
    CVISION_API void connectByTag(const std::string& source,
                                  const std::string& target,
                                  const float& weight = 1.0f,
                                  const std::string& type = "",
                                  const bool& bidirectional = true);

    /** Connect nodes with source tag to nodes with target tags */
    CVISION_API void connectByTags(const std::string& source,
                                   const std::vector<std::string>& targets,
                                   const float& weight = 1.0f,
                                   const std::string& type = "",
                                   const bool& bidirectional = true);

    /** Connect nodes with source tags to nodes with target tag */
    CVISION_API void connectByTags(const std::vector<std::string>& sources,
                                   const std::string& target,
                                   const float& weight = 1.0f,
                                   const std::string& type = "",
                                   const bool& bidirectional = true);

    /** Cross-connect nodes with source tags to nodes with target tags */
    CVISION_API void connectByTags(const std::vector<std::string>& sources,
                                   const std::vector<std::string>& targets,
                                   const float& weight = 1.0f,
                                   const std::string& type = "",
                                   const bool& bidirectional = true);

    // Selection

    CVISION_API void select(CVElement* element);
    CVISION_API void deselect(CVElement* element);
    CVISION_API void toggle_select(CVElement* element);

    CVISION_API void select_all();
    CVISION_API void select_none();

    CVISION_API void select_neighbors(CVNetworkNode& node);
    CVISION_API void select_neighbors(const std::string& tag);

    CVISION_API bool node_has_neighbors(const CVNetworkNode& node);
    CVISION_API bool node_has_neighbors(const std::string& tag);

    CVISION_API void setCordonState(const bool& state) noexcept;

    CVISION_API void selectGroup(CVNetworkGroup& group) noexcept;
    CVISION_API void selectGroup(const std::string& group_name) noexcept;

    // Selection Manipulation

    CVISION_API void pinSelected() noexcept;
    CVISION_API void unpinSelected() noexcept;
    CVISION_API void pinAll() noexcept;
    CVISION_API void unpinAll() noexcept;

    // Grouping

    /** @brief The method for visualizing groups
      *
      * Boxed: surround groups by a bounding box and title
      * them at the corners
      * Centered: Invisible bounding box, titled at the center */

    CVISION_API void setGroupMode(const CVNetworkGroup::Mode& newGroupMode) noexcept;
    inline const CVNetworkGroup::Mode& getGroupMode() const noexcept{ return groupMode; }

    /** @brief Adds nodes with tags to a new or existing group.
      *
      * Groups are visualized as annotated box overlays that partition
      * the network into separate regions.  Groups add another layer of
      * physics on top of the edge attraction and node repulsion in order
      * to help separate the network in meaningful ways.
      */
    CVISION_API void addToGroup(const std::vector<std::string>& tags,
                                const std::string& group_tag);
    CVISION_API void addToGroup(const std::string& tag,
                                const std::string& group_tag);
    CVISION_API void addSelectedToGroup(const std::string& group_tag);

    CVISION_API CVNetworkGroup * createGroup(const std::string& group_tag);

    CVISION_API void removeSelectedFromGroups();
    CVISION_API void removeGroup(const std::string& group_tag);

    CVISION_API void clearGroups();
    CVISION_API void trimGroupSizes(const int& lower_limit = 0,
                                    const int& upper_limit = INT_MAX);

    CVISION_API void mergeGroups(hyperC::reference_vector<CVNetworkGroup>& groups,
                                 const std::string& newGroupName = "");

    /** @brief Performs a keyword-based merge on all present groups.
      *
      * The Smith-Waterman algorithm, as well as direct string matching,
      * are performed to attempt to consolidate groups with similar key words.
      *
      * @param threshold    The sum of key word scores that must be acheived
      * to perform a match.
      */
    CVISION_API void mergeGroupsByKeywords(const float& threshold = 0.6f) noexcept;

    inline bool groupExists(const std::string& name) const noexcept{ return groups.find(name) != groups.end(); }

    // Layouts

    CVISION_API void setLayout(const CVNetworkLayout& newLayout);

    // Motion

    inline void setZoom(const float& newZoom) noexcept;
    inline void zoom(const float& magnitude) noexcept;

    inline void setPanInnerThreshold(const float& newThreshold) noexcept{ fPanInstigateInnerThreshold = newThreshold; }
    inline void setPanOuterThreshold(const float& newThreshold) noexcept{ fPanInstigateOuterThreshold = newThreshold; }

    // Physics

    inline void setNodePushStrength(const float& newCoef) noexcept{ fNodePushStrength = newCoef; }
    inline void setNodeFriction(const float& newFriction) noexcept{ fNodeFriction = newFriction; }

    inline const float& getNodeFriction() const noexcept{ return fNodeFriction; }

    inline void setTetherCoefficient(const float& newCoef) noexcept{ fTetherElasticCoefficient = newCoef; }
    inline void setTetherDistance(const float& newDistance) noexcept{ fTetherBaseDistance = newDistance; }
    inline void setTetherRangeThreshold(const float& percentage) noexcept{ fTetherRangeThreshold = percentage; }
    inline void setTetherEdgeDistanceModifier(const float& factorPerEdge) noexcept{ fTetherEdgeDistanceModifier = factorPerEdge; }
    inline void setTetherEdgeStrengthModifier(const float& factorPerEdge) noexcept{ fTetherEdgeElasticModifier = factorPerEdge; }

    // Display

    inline void setFontWeightScale(const float& newScale) noexcept{ fontWeightScale = newScale; }
    CVISION_API void setNodeFillColor(const sf::Color& newColor) noexcept;
    inline void setSelectionColor(const sf::Color& newColor) noexcept{ selectionColor = newColor; }

    inline void setDefaultNodeSize(const sf::Vector2f& newSize) noexcept{ defaultNodeSize = newSize; }
    inline void setDefaultNodePosition(const sf::Vector2f& newPosition) noexcept{ defaultNodePosition = newPosition; }
    inline void setDefaultEdgeColor(const sf::Color& newColor) noexcept{ edgeLegend["__DEFAULT__"] = newColor; }
    inline void setDefaultNodeFillColor(const sf::Color& newColor) noexcept{ nodeLegend["__DEFAULT__"] = newColor; }
    inline void setDefaultNodeOutlineColor(const sf::Color& newColor) noexcept{ outlineLegend["__DEFAULT__"] = newColor; }
    inline void setDefaultNodeTextColor(const sf::Color& newColor) noexcept{ textColorLegend["__DEFAULT__"] = newColor; }

    inline void setDefaultNodeTextAlignment(const unsigned int& newAlignment) noexcept{ defaultNodeTextAlignment = newAlignment; }

    inline void setLegendNodeColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ nodeLegend[type] = newColor; }
    inline void setLegendEdgeColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ edgeLegend[type] = newColor; }
    inline void setLegendOutlineColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ outlineLegend[type] = newColor; }
    inline void setLegendTextColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ textColorLegend[type] = newColor; }

    inline void setUniqueNodesOnly(const bool& status = true) noexcept{ bUniqueNodesOnly = status; }

    inline const unsigned int& getLOD() const noexcept{ return uLOD; }
    inline void setLOD(const unsigned int& newValue) noexcept{ uLOD = newValue; }

    // Access

    inline const sf::Vector2f& getDefaultNodeSize() const noexcept{ return defaultNodeSize; }
    inline const sf::Color& getDefaultNodeFillColor() const{ return nodeLegend.at("__DEFAULT__"); }
    inline const sf::Color& getDefaultNodeOutlineColor() const{ return outlineLegend.at("__DEFAULT__"); }
    inline const sf::Color& getDefaultEdgeColor() const{ return edgeLegend.at("__DEFAULT__"); }
    inline const sf::Color& getDefaultNodeTextColor() const{ return textColorLegend.at("__DEFAULT__"); }

    CVISION_API const sf::Color& getNodeColor(const std::string& type) const;
    CVISION_API const sf::Color& getEdgeColor(const std::string& type) const;
    CVISION_API const sf::Color& getOutlineLegendColor(const std::string& type) const;
    CVISION_API const sf::Color& getTextLegendColor(const std::string& type) const;

    CVISION_API std::vector<std::string> getNodeTags() const noexcept;

    // Options

    inline void setPan(const bool& state) noexcept{ bCanPan = state; }
    inline void setZoomable(const bool& state) noexcept{ bCanZoom = state; }
    inline void setNodesScaleWithWeights(const bool& state){ bScaleNodesWithWeight = state; }

    // Misc

    inline bool isSelected(CVElement* element)
    {
        for(auto& ref : selected)
        {
            if(ref.getElement() == element)
            {
                return true;
            }
        }

        return false;
    }

    inline bool anySelected() const noexcept{ return !selected.empty(); }
    inline size_t numSelected() const noexcept{ return selected.size(); }

    inline const hyperC::reference_vector<CVNetworkNode>& getSelected() noexcept{ return selected; }

    sf::Vector2f convert_to_local(const sf::Vector2f& screen_coords) const noexcept;
    sf::Vector2f convert_to_screen(const sf::Vector2f& local_coords) const noexcept;
    inline const float& getZoomLevel() const noexcept{ return fZoomLevel; }

protected:

    hyperC::TransferrableMutex  modLock;

    CVNetworkLayout layout;
    CVNetworkGroup::Mode groupMode;

    hyperC::static_vector<CVNetworkNode> nodes;
    std::unordered_map<std::string, CVNetworkGroup*> groups;

    sf::Vector2f defaultNodePosition;
    sf::Vector2f defaultNodeSize;
    sf::Vector2f panOffset;
    sf::Vector2f panVelocity;
    sf::Vector2f zoomAnchor;
    sf::Vector2f networkCenter;

    float fontWeightScale;
    float defaultNodeOutlineThickness;
    float defaultNodeRounding;
    float fNodePushStrength;
    float fNodeFriction;
    float fNodeEdgeWeightScale;
    float fTetherElasticCoefficient;
    float fTetherEdgeElasticModifier;
    float fTetherEdgeDistanceModifier;
    float fTetherBaseDistance;
    float fTetherRangeThreshold;
    float fPanInstigateInnerThreshold;
    float fPanInstigateOuterThreshold;
    float fPanRateScale;
    float fPanAttenutationRate;
    float fMaxPanSpeed;
    float fZoomLevel;
    float fLastZoomLevel;
    float fMaxZoomLevel;
    float fMinZoomLevel;
    float fZoomRateScale;
    float fZoomAttenutationRate;
    float fCurrentZoomRate;
    float fGroupPushDistance;
    float fGroupPushStrength;
    float fGroupCenterGravity;
    float fGroupDisplayRatio;
    float fNetworkCenterGravity;

    sf::Color selectionColor;

    std::unordered_map<std::string, sf::Color> edgeLegend;
    std::unordered_map<std::string, sf::Color> nodeLegend;
    std::unordered_map<std::string, sf::Color> outlineLegend;
    std::unordered_map<std::string, sf::Color> textColorLegend;

    sf::RectangleShape selectionCordon;

    unsigned int defaultNodeTextAlignment;
    unsigned int uFramesLastPhysicsUpdate;
    unsigned int uLOD;
    unsigned int uMinGroupDisplaySize;

    bool bUniqueNodesOnly;
    bool bSelection;
    bool bCanZoom;
    bool bCanPan;
    bool bCanCordonSelect;
    bool bScaleNodesWithWeight;

    /** @brief Main network panel physics update function

      * Update frequency is based on LOD.

      * >=3.0 = every frame
      * 2.0 = every other frame
      * 1.0 = every 2nd frame
      * 0.0 = every 3rd frame

      * Can be overridden for custom inherited classes */

    CVISION_API virtual void updatePhysics(CVEvent& event, const sf::Vector2f& mousePos);

    /** @brief Update the level of detail to maintain a nominal frame rate
        By default, adjusts the LOD according to the average frame rate
        Maximum LOD (3.0) at >48 fps */

    CVISION_API virtual void updateLOD(CVEvent& event);

    /** @brief Override to change how new nodes are connected to the existing network */
    virtual void updateNodeConnections(CVNetworkNode& node){ }

    hyperC::reference_vector<CVNetworkNode> selected;

    CVISION_API void select(CVNetworkNode& node);
    CVISION_API void deselect(CVNetworkNode& node);
    CVISION_API void toggle_select(CVNetworkNode& node);

};

}

#endif // CVIS_NETWORK_PANEL
