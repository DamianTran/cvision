#pragma once

#ifndef CVIS_NETWORK_PANEL
#define CVIS_NETWORK_PANEL

#include "cvision/panel.hpp"

#include <EZC/toolkit/reference_vector.hpp>
#include <EZC/toolkit/static_vector.hpp>

namespace cvis
{

class CVNetworkEdge;
class CVNetworkPanel;
class CVButton;

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

    CVISION_API CVNetworkEdge& getConnection(CVElement* other);
    CVISION_API CVNetworkEdge& getConnection(const std::string& tag);

    inline CVElement * const getElement() noexcept{ return element; }
    inline const CVElement * const getElement() const noexcept{ return element; }
    inline const std::string& getTag() noexcept{ return element->tag(); }
    inline const std::string& getType() const noexcept{ return type; }
    inline const float& getWeight() const noexcept{ return weight; }

    inline void setWeight(const float& newWeight) noexcept{ weight = newWeight; }
    inline void setType(const std::string& newType) noexcept{ type = newType; }

    inline void setTextDisplayOffset(const sf::Vector2f& newOffset) noexcept{ textDisplayOffset = newOffset; }

    inline void setDisplayTextOnHover(const bool& state = true) noexcept{ bDisplayTextOnHover = state; }

    CVISION_API void move(const sf::Vector2f& distance);
    CVISION_API void setPosition(const sf::Vector2f& newPosition);

    inline const sf::FloatRect& getBounds() const{ return getElement()->getBounds(); }
    inline const bool& isVisible() const{ return getElement()->isVisible(); }

    inline void setTextPadding(const float& padding) noexcept{ textPadding = padding; }
    inline void setTextFadeRate(const uint8_t& rate) noexcept{ textFadeRate = rate; }

    inline void setHighlightColor(const sf::Color& newColor) noexcept{ element->setHighlightColor(newColor); }

    CVISION_API void setSelected(const bool& status = true);
    inline const bool& isSelected() const noexcept{ return bSelected; }

    CVISION_API void update(const sf::Vector2f& mousePos);
    CVISION_API void draw(sf::RenderTarget* target);
    CVISION_API void drawEdges(sf::RenderTarget* target);

protected:

    friend class CVNetworkPanel;

    CVISION_API void align_text(const unsigned int& alignment);

private:

    CVElement* element;

    sf::Text displayText;

    std::string type;

    std::vector<CVNetworkEdge> connected_to;
    std::vector<CVNetworkEdge> connected_from;

    float weight;
    float textPadding;

    sf::Vector2f lastElementPosition;
    sf::Vector2f textDisplayOffset;

    bool bDisplayTextOnHover;
    bool bSelected;

    uint8_t targetTextAlpha;
    uint8_t textFadeRate;

};

/** Wrapper class for CVNetworkPanel edges */

class CVISION_API CVNetworkEdge
{
public:

    CVISION_API CVNetworkEdge(CVNetworkNode& origin,
                              CVNetworkNode& outNode,
                              const std::string& type = "none",
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
    inline void setLineWeightScale(const float& newScale) noexcept{ lineWeightScale = newScale; }

    CVISION_API void draw(sf::RenderTarget* target);
    CVISION_API void update();

protected:

    CVNetworkNode* origin;
    CVNetworkNode* node;

    std::string type;

    float weight;
    float lineWeightScale;
    float lineWidth;

private:

    sf::RectangleShape line;

};

/** Mindmap-type physics-enabled panel */

enum class CVNetworkLayout
{
    None = 0,
    Force,
    Organic
};

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

    CVISION_API CV_UPDATE_OVERRIDE;
    CVISION_API CV_DRAW_OVERRIDE;

    // Manual addition/addition of special elements to network

    // Deprecate the original method to avoid confusion

    CVISION_API void addPanelElement(CVElement* newElement,
                                    const std::string& newTag = "",
                                    const unsigned int& index = UINT_MAX) = delete;

    // New method for manual addition

    CVISION_API void addPanelElement(CVElement* newElement,
                                     const std::string& newTag = "",
                                     const std::string& newType = "",
                                     const float& weight = 1.0f,
                                     const unsigned int& label_orientation = ALIGN_CENTER_MIDLINE);

    CVISION_API void removePanelElement(CVElement* element);

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

    CVISION_API void removeNodesByTag(const std::string& tag);
    CVISION_API void removeNodesByTags(const std::string& tags);

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

    // Layouts

    CVISION_API void setLayout(const CVNetworkLayout& newLayout);

    // Physics

    inline void setNodeRepulsion(const float& newForce) noexcept{ physicsPushStrength = newForce; }
    inline void setEdgeAttraction(const float& newForce) noexcept{ physicsPullStrength = newForce; }

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

    inline void setLegendNodeColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ nodeLegend[type] = newColor; }
    inline void setLegendEdgeColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ edgeLegend[type] = newColor; }
    inline void setLegendOutlineColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ outlineLegend[type] = newColor; }
    inline void setLegendTextColor(const std::string& type,
                                   const sf::Color& newColor) noexcept{ textColorLegend[type] = newColor; }

    inline void setUniqueNodesOnly(const bool& status = true) noexcept{ bUniqueNodesOnly = status; }

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

protected:

    CVNetworkLayout layout;

    EZC::static_vector<CVNetworkNode> nodes;

    sf::Vector2f defaultNodePosition;
    sf::Vector2f defaultNodeSize;

    float fontWeightScale;
    float defaultNodeOutlineThickness;
    float defaultNodeRounding;
    float physicsPushStrength;
    float physicsPullStrength;
    float tetherDistanceScale;

    sf::Color selectionColor;

    std::unordered_map<std::string, sf::Color> edgeLegend;
    std::unordered_map<std::string, sf::Color> nodeLegend;
    std::unordered_map<std::string, sf::Color> outlineLegend;
    std::unordered_map<std::string, sf::Color> textColorLegend;

    sf::RectangleShape selectionCordon;

    unsigned int defaultNodeTextAlignment;

    bool bUniqueNodesOnly;
    bool bSelection;

    void updatePhysics(CVEvent& event);

    EZC::reference_vector<CVNetworkNode> selected;

    CVISION_API void select(CVNetworkNode& node);
    CVISION_API void deselect(CVNetworkNode& node);
    CVISION_API void toggle_select(CVNetworkNode& node);

};

}

#endif // CVIS_NETWORK_PANEL
