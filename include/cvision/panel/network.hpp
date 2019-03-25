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
                              const float& weight = 1.0f,
                              const sf::Font* font = nullptr,
                              const unsigned int& baseCharacterSize = 14u,
                              const unsigned int& alignment = ALIGN_CENTER_MIDLINE,
                              const sf::Color& textColor = sf::Color::Black);

    CVISION_API void connect_out(CVNetworkNode& other, const std::string& type = "none");         // Outbound connection
    CVISION_API void connect_in(CVNetworkNode& other, const std::string& type = "none");          // Inbound connection
    CVISION_API void connect_with(CVNetworkNode& other, const std::string& type = "none");        // Bidirectional connection

    CVISION_API void remove_connections(CVNetworkNode& other);
    CVISION_API void disconnect();

    CVISION_API CVNetworkEdge& getConnection(CVElement* other);
    CVISION_API CVNetworkEdge& getConnection(const std::string& tag);

    inline CVElement * const getElement() noexcept{ return element; }
    inline const CVElement * const getElement() const noexcept{ return element; }
    inline const std::string& getTag() noexcept{ return element->tag(); }

    inline void setWeight(const float& newWeight) noexcept{ weight = newWeight; }
    inline const float& getWeight() const noexcept{ return weight; }

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

    CVISION_API void addPanelElement(CVElement* newElement,
                         const std::string& newTag = "",
                         const unsigned int& index = UINT_MAX) = delete;

    CVISION_API void addPanelElement(CVElement* newElement,
                                     const std::string& newTag = "",
                                     const float& weight = 1.0f,
                                     const unsigned int& label_orientation = ALIGN_CENTER_MIDLINE);

    CVISION_API void removePanelElement(CVElement* element);

    // Add/remove nodes

    CVISION_API CVButton* addNode(const std::string& tag,
                                  const float& weight = 1.0f,
                                  const sf::Vector2f& position = sf::Vector2f(NAN, NAN),
                                  const sf::Vector2f& size = sf::Vector2f(NAN, NAN));

    CVISION_API CVButton* addNode(const std::string& tag,
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size,
                                  const sf::Color& fillColor,
                                  const sf::Color& outlineColor = sf::Color::Black,
                                  const float& outlineThickness = 1.0f,
                                  const float& rounding = NAN);

    CVISION_API CVButton* addImageNode(const std::string& tag,
                                       const std::string& texture,
                                       const float& weight = 1.0f,
                                       const sf::Vector2f& position = sf::Vector2f(NAN, NAN),
                                       const sf::Vector2f& size = sf::Vector2f(NAN, NAN));

    CVISION_API CVButton* addImageNode(const std::string& tag,
                                       const std::string& texture,
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

    CVISION_API void connectByTag(const std::string& source,
                                  const std::string& target,
                                  const bool& bidirectional = true);

    CVISION_API void connectByTags(const std::string& source,
                                   const std::vector<std::string>& targets,
                                   const bool& bidirectional = true);

    CVISION_API void connectByTags(const std::vector<std::string>& sources,
                                   const std::string& target,
                                   const bool& bidirectional = true);

    CVISION_API void connectByTags(const std::vector<std::string>& sources,
                                   const std::vector<std::string>& targets,
                                   const bool& bidirectional = true);

    // Layouts

    CVISION_API void setLayout(const CVNetworkLayout& newLayout);

    // Physics

    inline void setNodeRepulsion(const float& newForce) noexcept{ physicsPushStrength = newForce; }
    inline void setEdgeAttraction(const float& newForce) noexcept{ physicsPullStrength = newForce; }

    // Display

    inline void setFontWeightScale(const float& newScale) noexcept{ fontWeightScale = newScale; }
    CVISION_API void setNodeFillColor(const sf::Color& newColor) noexcept;
    inline void setSelectionColor(const sf::Color& newColor) noexcept{ selectionColor = newColor; }

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
    sf::Color defaultNodeFillColor;
    sf::Color defaultNodeOutlineColor;

    sf::RectangleShape selectionCordon;

    unsigned int defaultNodeTextAlignment;

    void updatePhysics(CVEvent& event);

private:

    EZC::reference_vector<CVNetworkNode> selected;

};

}

#endif // CVIS_NETWORK_PANEL
