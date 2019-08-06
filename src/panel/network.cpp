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

#include "cvision/panel/network.hpp"
#include "cvision/button.hpp"
#include "cvision/algorithm.hpp"
#include "cvision/view.hpp"
#include "cvision/type.hpp"
#include "cvision/app.hpp"

#include <hyper/algorithm.hpp>
#include <hyper/toolkit/string.hpp>

using namespace hyperC;
using namespace std;

namespace cvis
{

CVNetworkNode::CVNetworkNode(CVElement* element,
                             const string& newType,
                             const float& weight,
                             const sf::Font* font,
                             const unsigned int& baseCharacterSize,
                             const unsigned int& alignment,
                             const sf::Color& textFillColor):
    element(element),
    displayText(element->tag(), *font, baseCharacterSize),
    type(newType),
    weight(weight),
    textPadding(24.0f),
    fScale(1.0f),
    fViewScale(1.0f),
    fScaleTransform(1.0f),
    lastElementPosition(element->getPosition()),
    textDisplayOffset(0.0f, 0.0f),
    bDisplayTextOnHover(false),
    bSelected(false),
    targetTextAlpha(textFillColor.a),
    textFadeRate(2),
    textAlignment(alignment),
    attached_UI(nullptr),
    UI_alpha(255),
    bUIFadeout(true),
    bUIremove(false),
    bUIremoveOnDeselect(false),
    bStatic(false),
    bPinned(false),
    fUISizePaddingScale(1.2f),
    fadeLayers(CV_LAYER_ALL)
{
    align_text(alignment);
    displayText.setFillColor(textFillColor);
}

CVNetworkNode::~CVNetworkNode()
{

    for(auto& group : groups)
    {
        group->removeNode(*this);
    }

    disconnect();
    if(hasUI())
    {
        delete(attached_UI);
    }

}

void CVNetworkNode::connect_out(CVNetworkNode& node,
                                const float& weight,
                                const sf::Color& edgeColor,
                                const string& type)
{
    for(auto& link : connected_to)
    {
        if((node.getElement() == link.getNode().getElement()) &&
           (link.getType() == type))
        {
            link.setWeight(weight);
            goto skip_to;
        }
    }

    connected_to.emplace_back(*this, node, type, weight, edgeColor);

    skip_to:;

    for(auto& link : node.connected_from)
    {
        if((getElement() == link.getOrigin().getElement()) &&
           (link.getType() == type))
        {
            link.setWeight(weight);
            return;
        }
    }

    node.connected_from.emplace_back(*this, node, type, weight, edgeColor);
}

void CVNetworkNode::connect_in(CVNetworkNode& node,
                               const float& weight,
                               const sf::Color& edgeColor,
                               const string& type)
{

    for(auto& link : connected_from)
    {
        if((node.getElement() == link.getOrigin().getElement()) &&
           (link.getType() == type))
        {
            link.setWeight(weight);
            goto skip_from;
        }
    }

    connected_from.emplace_back(node, *this, type, weight, edgeColor);

    skip_from:;

    for(auto& link : node.connected_to)
    {
        if((getElement() == link.getNode().getElement()) &&
           (link.getType() == type))
        {
            link.setWeight(weight);
            return;
        }
    }

    node.connected_to.emplace_back(node, *this, type, weight, edgeColor);
}

void CVNetworkNode::connect_with(CVNetworkNode& node,
                                 const float& weight,
                                 const sf::Color& edgeColor,
                                 const string& type)
{
    connect_out(node, weight, edgeColor, type);
    connect_in(node, weight, edgeColor, type);
}

void CVNetworkNode::remove_connections(CVNetworkNode& other)
{

    size_t i;

    for(i = 0; i < connected_to.size();)
    {
        if(&connected_to[i].getNode() == &other)
        {
            connected_to.erase(connected_to.begin() + i);
        }
        else ++i;
    }

    for(i = 0; i < connected_from.size();)
    {
        if(&connected_from[i].getOrigin() == &other)
        {
            connected_from.erase(connected_from.begin() + i);
        }
        else ++i;
    }

    for(i = 0; i < other.connected_to.size();)
    {
        if(&other.connected_to[i].getNode() == this)
        {
            other.connected_to.erase(other.connected_to.begin() + i);
        }
        else ++i;
    }

    for(i = 0; i < other.connected_from.size();)
    {
        if(&other.connected_from[i].getOrigin() == this)
        {
            other.connected_from.erase(other.connected_from.begin() + i);
        }
        else ++i;
    }

}

void CVNetworkNode::disconnect()
{

    vector<CVNetworkNode*> disconnect_nodes;

    for(auto& edge : connected_to)
    {
        disconnect_nodes.emplace_back(&edge.getNode());
    }

    for(auto& edge : connected_from)
    {
        if(!anyEqual(&edge.getOrigin(), disconnect_nodes))
        {
            disconnect_nodes.emplace_back(&edge.getOrigin());
        }
    }

    for(auto& node : disconnect_nodes)
    {
        remove_connections(*node);
    }

}

void CVNetworkNode::setSelected(const bool& state)
{
    if(state ^ bSelected)
    {
        bSelected = state;
        element->highlight(bSelected);
    }
}

void CVNetworkNode::move(const sf::Vector2f& distance)
{
    element->move(distance);
    displayText.move(distance);
    lastElementPosition += distance;
}

void CVNetworkNode::setPosition(const sf::Vector2f& newPosition)
{
    move(newPosition - element->getPosition());
}

void CVNetworkNode::setScale(const float& newScale)
{

    float fNewTransform = newScale * fViewScale;

    element->setSize(element->getSize() * fNewTransform/fScaleTransform);
    element->setSpriteScale(fNewTransform);

    unsigned int newCharacterSize = 12 * fNewTransform;
    if(newCharacterSize < 12)
    {
        newCharacterSize = 12;
    }

    displayText.setCharacterSize(newCharacterSize);

    align_text(textAlignment);
    fScale = newScale;
    fScaleTransform = fNewTransform;

}

void CVNetworkNode::setViewScale(const float& newScale)
{

    float fNewTransform = newScale * fScale;

    element->setSize(element->getSize() * fNewTransform/fScaleTransform);
    element->setSpriteScale(fNewTransform);

    unsigned int newCharacterSize = 12 * fNewTransform;
    if(newCharacterSize < 12)
    {
        newCharacterSize = 12;
    }

    displayText.setCharacterSize(newCharacterSize);

    for(auto& edge : connected_to)
    {
        edge.setViewScale(newScale);
    }
    for(auto& edge : connected_from)
    {
        edge.setViewScale(newScale);
    }

    align_text(textAlignment);
    fViewScale = newScale;
    fScaleTransform = fNewTransform;
}

void CVNetworkNode::setSprite(const sf::Texture* newTexture)
{
    element->getSprite(0).setTexture(*newTexture, true);
}

void CVNetworkNode::setWeight(const float& newWeight,
                              const bool& rescale) noexcept
{

    weight = newWeight;

    if(rescale)
    {
        setScale(pow(newWeight, 1.0/3));
    }

}

void CVNetworkNode::setTag(const std::string& newTag) noexcept
{

    element->setTag(newTag);
    displayText.setString(newTag);

}

void CVNetworkNode::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(lastElementPosition != element->getPosition())
    {
        displayText.move(element->getPosition() - lastElementPosition);
        lastElementPosition = element->getPosition();
    }

    for(auto& edge : connected_to)
    {
        edge.update(event, mousePos);
    }

    if(bDisplayTextOnHover)
    {
        if(element->getBounds().contains(mousePos))
        {
            if(displayText.getFillColor().a < targetTextAlpha)
            {
                sf::Color tmp = displayText.getFillColor();
                tmp.a += textFadeRate;
                displayText.setFillColor(tmp);
            }
        }
        else if(displayText.getFillColor().a)
        {
            sf::Color tmp = displayText.getFillColor();
            tmp.a -= textFadeRate;
            displayText.setFillColor(tmp);
        }
    }

}

void CVNetworkNode::updateInterface(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(attached_UI)
    {
        if(bUIremoveOnDeselect && !isSelected())
        {
            remove_UI();
        }

        if(bUIremove)
        {
            attached_UI->setFade(0, 12, fadeLayers);
        }

        attached_UI->setPosition(sf::Vector2f(getBounds().left,
                                              getBounds().top));
        attached_UI->setSize(sf::Vector2f(getBounds().width,
                                          getBounds().height));
        attached_UI->update(event, mousePos);

        if(bUIremove && attached_UI->fadeComplete())
        {
            delete(attached_UI);
            attached_UI = nullptr;
            bUIremove = false;
        }
    }

}

void CVNetworkNode::draw(sf::RenderTarget* target)
{
    if(element->draw(target))
    {
        target->draw(displayText);
    }
}

void CVNetworkNode::drawInterface(sf::RenderTarget* target)
{
    if(hasUI())
    {
        attached_UI->draw(target);
    }
}

void CVNetworkNode::drawEdges(sf::RenderTarget* target,
                              const unsigned int& LOD)
{
    if(target)
    {
        for(size_t i = 0, L = connected_to.size(); i < L; ++i)
        {
            connected_to[i].draw(target);
        }
    }
}

void CVNetworkNode::attach_UI(CVElement* newUI,
                              const uint8_t& alpha,
                              const unsigned char& fadeLayers)
{
    newUI->setSize(sf::Vector2f(getBounds().width,
                                getBounds().height));
    newUI->setPosition(getBounds().left,
                       getBounds().top);
    newUI->setFade(alpha, 12, fadeLayers);

    this->fadeLayers = fadeLayers;
    attached_UI = newUI;
    UI_alpha = alpha;

}

void CVNetworkNode::remove_UI(const bool& fadeout)
{
    if(hasUI())
    {
        bUIremove = true;
    }
}

void CVNetworkNode::apply_tethers(const float& distance,
                                  const float& elastic_coefficient,
                                  const float& range_threshold,
                                  const float& friction) noexcept
{
    for(auto& edge : connected_to)
    {
        edge.apply_tether(distance, elastic_coefficient, range_threshold, friction);
    }
}

void CVNetworkNode::addGroup(CVNetworkGroup& group) noexcept
{

    if(!anyEqual(&group, groups))
    {
        groups.emplace_back(&group);
    }

}

void CVNetworkNode::removeGroup(CVNetworkGroup& group) noexcept
{

    for(size_t i = 0; i < groups.size(); ++i)
    {
        if(groups[i] == &group)
        {
            groups.erase(groups.begin() + i);
        }
    }

}

bool CVNetworkNode::checkGroup(CVNetworkGroup& group) const noexcept
{

    return anyEqual(&group, groups);

}

bool CVNetworkNode::checkGroups(const CVNetworkNode& node) const noexcept
{

    if(groups.empty() || node.groups.empty())
    {
        return false;
    }

    for(auto& group : node.groups)
    {
        if(checkGroup(*group))
        {
            return true;
        }
    }

    return false;
}

void CVNetworkNode::align_text(const unsigned int& alignment)
{

    sf::FloatRect textBounds = displayText.getGlobalBounds();
    sf::FloatRect elementBounds = element->getBounds();

    switch(alignment)
    {
    case ALIGN_LEFT:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width, textBounds.height));
            displayText.setPosition(sf::Vector2f(elementBounds.left - textPadding,
                                                 elementBounds.top - textPadding));

            break;
        }
    case ALIGN_LEFT_MIDLINE:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width, textBounds.height - getTextCenterOffsetY(displayText)/4));
            displayText.setPosition(sf::Vector2f(elementBounds.left - textPadding,
                                                 elementBounds.top + elementBounds.height/2));

            break;
        }
    case ALIGN_LEFT_BOTTOM:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width, 0.0f));
            displayText.setPosition(sf::Vector2f(elementBounds.left - textPadding,
                                                 elementBounds.top + elementBounds.height + textPadding));

            break;
        }
    case ALIGN_CENTER:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width/2, textBounds.height));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width/2,
                                                 elementBounds.top - textPadding));

            break;
        }
    case ALIGN_CENTER_MIDLINE:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width/2, textBounds.height - getTextCenterOffsetY(displayText)/4));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width/2,
                                                 elementBounds.top + elementBounds.height/2));

            break;
        }
    case ALIGN_CENTER_BOTTOM:
        {

            displayText.setOrigin(sf::Vector2f(textBounds.width/2, 0.0f));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width/2,
                                                 elementBounds.top + elementBounds.height + textPadding));

            break;
        }
    case ALIGN_RIGHT:
        {

            displayText.setOrigin(sf::Vector2f(0.0f, textBounds.height));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width + textPadding,
                                                 elementBounds.top - textPadding));

            break;
        }
    case ALIGN_RIGHT_MIDLINE:
        {

            displayText.setOrigin(sf::Vector2f(0.0f, textBounds.height - getTextCenterOffsetY(displayText)/4));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width + textPadding,
                                                 elementBounds.top + elementBounds.height/2));

            break;
        }
    case ALIGN_RIGHT_BOTTOM:
        {

            displayText.setOrigin(sf::Vector2f(0.0f, 0.0f));
            displayText.setPosition(sf::Vector2f(elementBounds.left + elementBounds.width + textPadding,
                                                 elementBounds.top + elementBounds.height + textPadding));

            break;
        }
    default:
        {
            break;
        }
    }

    textAlignment = alignment;

}

CVNetworkEdge& CVNetworkNode::getConnection(CVElement* element)
{
    for(auto& connection : connected_to)
    {
        if(connection.getElement() == element)
        {
            return connection;
        }
    }

    for(auto& connection : connected_from)
    {
        if(connection.getElement() == element)
        {
            return connection;
        }
    }

    throw invalid_argument("CVNetworkNode: requested element connection does not exist");
}

CVNetworkEdge& CVNetworkNode::getConnection(CVNetworkNode& other)
{
    for(auto& connection : connected_to)
    {
        if(&connection.getNode() == &other)
        {
            return connection;
        }
    }

    for(auto& connection : connected_from)
    {
        if(&connection.getNode() == &other)
        {
            return connection;
        }
    }

    throw invalid_argument("CVNetworkNode: requested node connection does not exist");
}

CVNetworkEdge& CVNetworkNode::getConnection(const string& tag)
{
    for(auto& connection : connected_to)
    {
        if(connection.getTag() == tag)
        {
            return connection;
        }
    }

    for(auto& connection : connected_from)
    {
        if(connection.getTag() == tag)
        {
            return connection;
        }
    }

    throw invalid_argument("CVNetworkNode: requested tag connection \"" + tag + "\" does not exist");
}

CVNetworkEdge& CVNetworkNode::getOutConnection(const size_t& index)
{
    if(index >= connected_to.size())
    {
        throw std::out_of_range("CVNetworkNode: requested index out of range of outbound connections");
    }
    return connected_to[index];
}

CVNetworkEdge& CVNetworkNode::getInConnection(const size_t& index)
{
    if(index >= connected_from.size())
    {
        throw std::out_of_range("CVNetworkNode: requested index out of range of inbound connections");
    }
    return connected_from[index];
}

bool CVNetworkNode::hasConnectionTo(const CVNetworkNode& other)
{
    for(auto& edge : connected_to)
    {
        if(&edge.getNode() == &other)
        {
            return true;
        }
    }

    for(auto& edge : connected_from)
    {
        if(&edge.getOrigin() == &other)
        {
            return true;
        }
    }

    return false;
}

CVNetworkEdge::CVNetworkEdge(CVNetworkNode& origin,
                             CVNetworkNode& outNode,
                             const string& type,
                             const float& weight,
                             const sf::Color& edgeColor):
                                  bTextVisible(false),
                                  origin(&origin),
                                  node(&outNode),
                                  type(type),
                                  weight(weight),
                                  lineWeightScale(1.0f),
                                  lineWidth(2.0f),
                                  fViewScale(1.0f)
{

    line.setFillColor(edgeColor);
    displayText.setString(type);

    displayText.setFont(*origin.getElement()->appFont("network_nodes"));
    displayText.setCharacterSize(14);

    sf::Color overlayTextColor = edgeColor;
    if(overlayTextColor.a > 180)
    {
        overlayTextColor.a = 180;
    }
    displayText.setFillColor(overlayTextColor);

}

void CVNetworkEdge::setLineWidth(const float& newWidth) noexcept
{
    lineWidth = newWidth;
}

void CVNetworkEdge::apply_tether(const float& distance,
                                 const float& elastic_coefficient,
                                 const float& range_threshold,
                                 const float& friction)
{

    float cDist = getDistance(getOrigin().getPosition(), getNode().getPosition());
    float adjust;

    if(cDist > distance * (1.0f + range_threshold))
    {
        adjust = (cDist - distance * (1.0f + range_threshold)) * elastic_coefficient;
    }
    else if(cDist < distance)
    {
        adjust = (cDist - distance) * elastic_coefficient;
    }
    else
    {
        return;
    }

    double theta1 = get_angle(getOrigin().getPosition(), getNode().getPosition());
    double theta2 = theta1 - PI;

    float totalWeight = getOrigin().getWeight() + getNode().getWeight();

    if(!getOrigin().bStatic) getOrigin().push(theta1, adjust * getNode().getWeight() * 5 / totalWeight, friction);
    if(!getNode().bStatic) getNode().push(theta2, adjust * getOrigin().getWeight() * 5 / totalWeight, friction);

}

void CVNetworkEdge::draw(sf::RenderTarget* target)
{
    if(target &&
       ((origin->isSelected() || node->isSelected()) ||
        (origin->isVisible()) || (node->isVisible())))
    {
        target->draw(line);
    }

    if(bTextVisible)
    {
        target->draw(displayText);
    }
}

void CVNetworkEdge::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    sf::Vector2f originPos = getBoundCenter(origin->getBounds());
    sf::Vector2f destinationPos = getBoundCenter(node->getBounds());

    line.setSize(sf::Vector2f(getDistance(originPos, destinationPos),
                              lineWidth * lineWeightScale * fViewScale));
    line.setOrigin(sf::Vector2f(0.0f, lineWidth * lineWeightScale/2));
    line.setPosition(originPos);
    line.setRotation(get_angle(originPos, destinationPos)*180/PI);

    displayBounds.width = displayBounds.height = line.getSize().x / 4;
    displayBounds.left = line.getGlobalBounds().left + line.getGlobalBounds().width/2 - line.getSize().x / 8;
    displayBounds.top = line.getGlobalBounds().top + line.getGlobalBounds().height/2 - line.getSize().x / 8;

    displayText.setPosition(sf::Vector2f(line.getGlobalBounds().left + line.getGlobalBounds().width/2 - displayText.getGlobalBounds().width / 2,
                                         line.getGlobalBounds().top + line.getGlobalBounds().height/2 - displayText.getGlobalBounds().height / 2));

    bTextVisible = displayBounds.contains(mousePos);

}

CVNetworkGroup::CVNetworkGroup(CVView * View,
                               const reference_vector<CVNetworkNode>& nodes,
                               const CVNetworkGroup::Mode& mode,
                               const TextEntry& textInfo,
                               const sf::Color& fillColor,
                               const sf::Color& outlineColor,
                               const float& outlineThickness,
                               const float& rounding):
                                   CVBasicViewPanel(View, textInfo.text, fillColor,
                                                    sf::Vector2f(0.0f, 0.0f),
                                                    false,
                                                    sf::Vector2f(0.0f, 0.0f)),
                                   groupMode(mode),
                                   annotation_dir(AnnotationDirection::TopLeft),
                                   nodes(nodes)
{

    this->textInfo = textInfo;

    setOutOfBoundsDraw(true);
    setOutOfBoundsUpdate(true);

    setRounding(rounding);
    setOutlineColor(outlineColor);
    setOutlineThickness(outlineThickness);

    annotation = new CVTypeBox(View,
                               sf::Vector2f(0.0f, 0.0f),
                               128.0f,
                               32.0f,
                               TextEntry("Enter a group name...",
                                         textInfo.font,
                                         textInfo.fontSize,
                                         textInfo.alignment,
                                         textInfo.textColor),
                               sf::Color::Transparent,
                               sf::Color::Transparent,
                               0.0f,
                               CV_OBJ_ANIM_SLIDE,
                               CV_TEXT_FIT_NONE);

    annotation->setTypeString(textInfo.text);
    annotation->setTextPadding(0.0f);

    addPanelElement(annotation, textInfo.text);

    if(!nodes.empty())
    {
        sf::FloatRect bounds = nodes.front().getBounds();

        for(size_t i = 1; i < nodes.size(); ++i)
        {
            expandBounds(bounds, nodes[i].getBounds());
        }

        setPosition(bounds.left, bounds.top);
        setSize(sf::Vector2f(bounds.width, bounds.height));
    }

    // Parse key words

    splitString(tag(), key_words, " \t\r\n,;\"-");

    setGroupMode(mode);

}

CVNetworkGroup::~CVNetworkGroup()
{

    for(auto& node : nodes)
    {
        node.removeGroup(*this);
    }

}

bool CVNetworkGroup::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    if(!nodes.empty())
    {

        sf::FloatRect bounds = nodes.front().getBounds();

        for(size_t i = 1; i < nodes.size(); ++i)
        {
            expandBounds(bounds, nodes[i].getBounds());
        }

        expandBounds(bounds, 6.0f + log(8.0f * nodes.size()));

        setPosition(bounds.left, bounds.top);
        setSize(sf::Vector2f(bounds.width, bounds.height));

    }

    return true;

}

void CVNetworkGroup::setSize(const sf::Vector2f& newSize)
{

    CVViewPanel::setSize(newSize);

    sf::Vector2f newPos;

    switch(annotation_dir)
    {
    case AnnotationDirection::TopRight:
        {
            annotation->setAlignment(ALIGN_RIGHT_MIDLINE);
            newPos.x = getPosition().x + getSize().x - annotation->getBounds().width - 4.0f;
            newPos.y = getPosition().y + 4.0f;
            break;
        }
    case AnnotationDirection::BottomRight:
        {
            annotation->setAlignment(ALIGN_RIGHT_MIDLINE);
            newPos.x = getPosition().x + getSize().x - annotation->getBounds().width - 4.0f;
            newPos.y = getPosition().y + getSize().y - annotation->getBounds().height - 4.0f;
            break;
        }
    case AnnotationDirection::BottomLeft:
        {
            newPos.x = getPosition().x + 4.0f;
            newPos.y = getPosition().y + getSize().y - annotation->getBounds().height - 4.0f;
            break;
        }
    case AnnotationDirection::Center:
        {
            annotation->setAlignment(ALIGN_CENTER_MIDLINE);
            newPos.x = getPosition().x + getSize().x/2 - annotation->getBounds().width/2;
            newPos.y = getPosition().y + getSize().y/2 - annotation->getBounds().height/2;
            break;
        }
    default: // Top-left
        {
            newPos.x = getPosition().x + 4.0f;
            newPos.y = getPosition().y + 4.0f;
            break;
        }
    }

    annotation->setPosition(newPos);

}

void CVNetworkGroup::addNode(CVNetworkNode& node)
{

    for(auto& n : nodes)
    {
        if(&n == &node)
        {
            return;
        }
    }

    node.addGroup(*this);
    nodes.emplace_back(node);

    unsigned int newTextSize;

    switch(groupMode)
    {
    case CVNetworkGroup::Mode::Centered:
        {

            newTextSize = 26 * sqrt((float)size()/10);

            if(newTextSize < 16)
            {
                newTextSize = 16;
            }
            else if(newTextSize > 72)
            {
                newTextSize = 72;
            }

            break;
        }
    default: // Boxed
        {

            newTextSize = 18 * sqrt((float)size()/10);

            if(newTextSize < 12)
            {
                newTextSize = 12;
            }
            else if(newTextSize > 28)
            {
                newTextSize = 28;
            }

            break;
        }
    }

    annotation->setTextSize(newTextSize);
    annotation->setSize(annotation->getTextBounds().width + 4.0f,
                        annotation->getTextBounds().height + 4.0f);

    if(groupMode == CVNetworkGroup::Mode::Centered)
    {
        annotation->setRounding(annotation->getTextBounds().height);
    }

}

void CVNetworkGroup::removeNode(CVNetworkNode& node)
{

    for(size_t i = 0; i < nodes.size(); ++i)
    {
        if(&nodes[i] == &node)
        {
            nodes.erase(nodes.begin() + i);
            return;
        }
    }

    unsigned int newTextSize;

    switch(groupMode)
    {
    case CVNetworkGroup::Mode::Centered:
        {

            newTextSize = 18 * sqrt((float)size()/10);

            if(newTextSize < 14)
            {
                newTextSize = 14;
            }
            else if(newTextSize > 52)
            {
                newTextSize = 52;
            }

            break;
        }
    default: // Boxed
        {

            newTextSize = 18 * sqrt((float)size()/10);

            if(newTextSize < 12)
            {
                newTextSize = 12;
            }
            else if(newTextSize > 28)
            {
                newTextSize = 28;
            }

            break;
        }
    }

    annotation->setTextSize(newTextSize);
    annotation->setSize(annotation->getTextBounds().width + 4.0f,
                        annotation->getTextBounds().height + 4.0f);

    if(groupMode == CVNetworkGroup::Mode::Centered)
    {
        annotation->setRounding(annotation->getTextBounds().height);
    }

}

bool CVNetworkGroup::checkNode(CVNetworkNode& node) const noexcept
{
    for(size_t i = 0; i < nodes.size(); ++i)
    {
        if(&node == &nodes[i])
        {
            return true;
        }
    }

    return false;
}

void CVNetworkGroup::setGroupMode(const CVNetworkGroup::Mode& newMode) noexcept
{

    groupMode = newMode;

    switch(groupMode)
    {
    case CVNetworkGroup::Mode::Centered:
        {
            annotation->setFillColor(sf::Color(255, 255, 255, 80));
            break;
        }
    default: // Boxed
        {
            annotation->setFillColor(sf::Color::Transparent);
            break;
        }
    }

}

float CVNetworkGroup::checkKeyWords(const CVNetworkGroup& other) const noexcept
{

    float output = 0.0f;

    for(auto& word : key_words)
    {
        for(auto& o_word : other.key_words)
        {
            if(cmpString(word, o_word, CMP_STR_CASE_INSENSITIVE))
            {
                ++output;
                break;
            }
        }
    }

    output /= key_words.size();

    return output;

}

float CVNetworkGroup::checkKeyWords(const vector<string>& o_keywords) const noexcept
{

    float output = 0.0f;

    for(auto& o_word : o_keywords)
    {
        for(auto& word : key_words)
        {
            if(cmpString(word, o_word, CMP_STR_CASE_INSENSITIVE))
            {
                ++output;
                break;
            }
        }
    }

    output /= o_keywords.size();

    return output;

}

void merge(reference_vector<CVNetworkGroup>& groups,
           CVNetworkGroup& output)
{

    if(groups.empty())
    {
        return;
    }
    else if(groups.size() == 1)
    {
        output.setTag(groups.front().tag());
        output.key_words = groups.front().key_words;
        output.sub_groups = groups.front().sub_groups;
        output.nodes = groups.front().nodes;

        return;
    }

    output.nodes.clear();
    output.sub_groups.clear();
    output.key_words.clear();

    for(auto& group : groups)
    {
        if(!group.empty())
        {
            for(auto& node : group.nodes)
            {
                output.addNode(node);
            }

            output.sub_groups.emplace_back(group.tag());

            for(auto& o_word : group.key_words)
            {
                if(!cmpStringToList(o_word, output.key_words, CMP_STR_CASE_INSENSITIVE))
                {
                    output.key_words.emplace_back(o_word);
                }
            }

        }
    }

}

void CVNetworkGroup::updateAnnotationDir(const sf::Vector2f& relative_center) noexcept
{

    switch(groupMode)
    {
    case CVNetworkGroup::Mode::Centered:
        {
            annotation_dir = AnnotationDirection::Center;
            break;
        }
    default: // Boxed
        {

            bool p_y = getBoundCenter(getBounds()).y > relative_center.y;
            bool p_x = getBoundCenter(getBounds()).x > relative_center.x;

            if(!p_y && !p_x)
            {
                annotation_dir = AnnotationDirection::TopLeft;
            }
            else if(!p_y && p_x)
            {
                annotation_dir = AnnotationDirection::TopRight;
            }
            else if(p_y && p_x)
            {
                annotation_dir = AnnotationDirection::BottomRight;
            }
            else
            {
                annotation_dir = AnnotationDirection::BottomLeft;
            }

            break;

        }
    }

}

CVNetworkPanel::CVNetworkPanel(CVView* View,
                               const string& tag,
                               const sf::Vector2f& position,
                               const sf::Vector2f& size,
                               const sf::Color& backgroundColor,
                               const sf::Color& outlineColor,
                               const TextEntry& textInfo,
                               const float& outlineThickness):
                                 CVBasicViewPanel(View, tag, backgroundColor, size,
                                                  false, position),
                                 layout(CVNetworkLayout::None),
                                 groupMode(CVNetworkGroup::Mode::Boxed),
                                 defaultNodePosition(size/2),
                                 defaultNodeSize(sf::Vector2f(96.0f * viewScale(),
                                                              96.0f * viewScale())),
                                 panOffset(0.0f, 0.0f),
                                 panVelocity(0.0f, 0.0f),
                                 zoomAnchor(position + size/2),
                                 fontWeightScale(1.0f),
                                 defaultNodeOutlineThickness(2.0f),
                                 defaultNodeRounding(NAN),
                                 fNodePushStrength(5.0f),
                                 fNodeFriction(200.0f),
                                 fNodeEdgeWeightScale(0.3f),
                                 fTetherElasticCoefficient(0.045f),
                                 fTetherEdgeDistanceModifier(0.0009f),
                                 fTetherEdgeElasticModifier(0.0001f),
                                 fTetherBaseDistance(300.0f * viewScale()),
                                 fTetherRangeThreshold(0.35f),
                                 fPanInstigateInnerThreshold(0.08),
                                 fPanInstigateOuterThreshold(0.05),
                                 fPanRateScale(20.0f),
                                 fPanAttenutationRate(0.91f),
                                 fMaxPanSpeed(512.0f * viewScale()),
                                 fZoomLevel(1.0f),
                                 fLastZoomLevel(1.0f),
                                 fMaxZoomLevel(25.0f),
                                 fMinZoomLevel(0.04f),
                                 fZoomRateScale(0.5f),
                                 fZoomAttenutationRate(0.85f),
                                 fCurrentZoomRate(0.0f),
                                 fGroupPushDistance(550.0f * viewScale()),
                                 fGroupPushStrength(0.1f),
                                 fGroupCenterGravity(0.001f),
                                 fGroupDisplayRatio(0.5f),
                                 fNetworkCenterGravity(0.0f),
                                 selectionColor(sf::Color::Yellow),
                                 defaultNodeTextAlignment(ALIGN_CENTER_MIDLINE),
                                 uFramesLastPhysicsUpdate(0),
                                 uLOD(3),
                                 uMinGroupDisplaySize(3),
                                 bUniqueNodesOnly(false),
                                 bSelection(false),
                                 bCanPan(true),
                                 bCanZoom(true),
                                 bCanCordonSelect(true),
                                 bScaleNodesWithWeight(true)
{

    this->textInfo = textInfo;
    setOutlineThickness(outlineThickness);
    setOutlineColor(outlineColor);

    selectionCordon.setOutlineThickness(1.0f);
    selectionCordon.setFillColor(sf::Color(128,128,128,60));
    selectionCordon.setOutlineColor(sf::Color(128,128,128,200));

    setDefaultNodeFillColor(sf::Color::White);
    setDefaultEdgeColor(sf::Color::Black);
    setDefaultNodeOutlineColor(sf::Color::Black);
    setDefaultNodeTextColor(textInfo.textColor);

    setDrawClipping(true);
    setOutOfBoundsUpdate(true);

}

CVNetworkPanel::~CVNetworkPanel()
{

    nodes.clear();

    for(auto& pair : groups)
    {
        delete(pair.second);
    }

}

bool CVNetworkPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    // Update node interface

    modLock.lock();

    for(auto& node : nodes)
    {
        node.updateInterface(event, mousePos);
    }

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    // Update nodes

    for(auto& node : nodes)
    {
        node.update(event, mousePos);
    }

    modLock.unlock();

    if(!groups.empty())
    {

        // Update group interface

        modLock.lock();

        uMinGroupDisplaySize = log(groups.size()/2);
        if(!uMinGroupDisplaySize) uMinGroupDisplaySize = 1;

        // Update network center

        networkCenter.x = 0.0f;
        networkCenter.y = 0.0f;
        for(auto& node : nodes)
        {
            networkCenter += node.getPosition();
        }
        networkCenter /= nodes.size();

        vector<CVNetworkGroup*> rmGroups;

        for(auto& pair : groups)
        {
            if(pair.second->empty())
            {
                rmGroups.emplace_back(pair.second);
            }
            else if(pair.second->size() < uMinGroupDisplaySize)
            {
                pair.second->setVisible(false);
            }
            else
            {
                pair.second->setVisible(true);
                pair.second->updateAnnotationDir(networkCenter);
                pair.second->update(event, mousePos);

                if(pair.second->annotation->hasFocus())
                {
                    selectGroup(*pair.second);
                }
            }
        }

        for(auto& group : rmGroups)
        {
            for(auto it = groups.begin(); it != groups.end(); ++it)
            {
                if(it->second == group)
                {
                    delete(it->second);
                    groups.erase(it);
                    break;
                }
            }
        }

        bool bStatus;

        do
        {

            bStatus = true;

            for(auto& pair : groups)
            {

                if(!pair.second->isVisible())
                {
                    continue;
                }

                for(auto& o_pair : groups)
                {
                    if((&pair == &o_pair) ||
                       (!o_pair.second->isVisible()))
                    {
                        continue;
                    }

                    if(pair.second->annotation->getBounds().intersects(o_pair.second->annotation->getBounds()))
                    {

                        if(pair.second->annotation_dir == CVNetworkGroup::AnnotationDirection::TopLeft)
                        {
                            pair.second->annotation->move(sf::Vector2f(0.0f, o_pair.second->annotation->getBounds().top -
                                                                             pair.second->annotation->getBounds().top -
                                                                             pair.second->annotation->getBounds().height - 1.0f));
                        }
                        else
                        {
                            pair.second->annotation->move(sf::Vector2f(0.0f, o_pair.second->annotation->getBounds().top +
                                                                             o_pair.second->annotation->getBounds().height + 1.0f -
                                                                             pair.second->annotation->getBounds().top));
                        }

                        bStatus = false;
                    }
                }
            }

        }while(!bStatus);

        modLock.unlock();

    }

    // Handle selection

    if(!bSelection)
    {
        if(event.LMBhold &&
           bounds.contains(event.LMBpressPosition) &&
           event.focusFree())
        {

            modLock.lock();

            for(auto& node : nodes)
            {
                if(node.getBounds().contains(event.lastFrameMousePosition))
                {
                    if(!node.isSelected())
                    {

                        if(!ctrlPressed())
                        {
                            select_none();
                        }

                        select(node);
                    }
                    else    // Transduce drag to other selected nodes
                    {

                        for(auto& node : selected)
                        {
                            if(!event.isCaptured(*node.getElement()))
                            {
                                event.mouse_capture(*node.getElement());
                            }
                        }

                    }

                    goto noCordon;
                }
            }

            if(event.captureMouse())
            {

                if(!ctrlPressed())
                {
                    select_none();
                }

                if(bCanCordonSelect)
                {

                    selectionCordon.setPosition(event.LMBpressPosition);
                    bSelection = true;

                }

            }

            noCordon:;

            modLock.unlock();

        }
    }
    else if(bSelection && event.LMBhold && bCanCordonSelect)
    {
        selectionCordon.setSize(mousePos - event.LMBpressPosition);
    }

    if(event.LMBreleased &&
       (event.LMBreleaseFrames == 1))
    {

        if(bounds.contains(event.LMBpressPosition) &&
           event.captureMouse())
        {
            bHasFocus = true;
        }
        else
        {
            bHasFocus = false;
        }

        if(bSelection && bCanCordonSelect)
        {

            modLock.lock();
            for(auto& node : nodes)
            {
                if(contains(node.getBounds(), selectionCordon.getGlobalBounds()))
                {
                    select(node);
                }
            }
            modLock.unlock();

            selectionCordon.setSize(sf::Vector2f(0.0f, 0.0f));

            bSelection = false;

        }
//        else if(ctrlPressed())
//        {
//
//            for(auto& node : nodes)
//            {
//                if(node.getBounds().contains(mousePos))
//                {
//                    toggle_select(node);
//                    break;
//                }
//            }
//
//        }
    }

    // Handle panning

    if(bCanPan &&
       event.focusFree() &&
       event.viewHasFocus)
    {

        sf::FloatRect panBounds = getBounds();

        float expand_x = bounds.width * fPanInstigateOuterThreshold;
        float expand_y = bounds.height * fPanInstigateOuterThreshold;

        panBounds.left -= expand_x;
        panBounds.top -= expand_y;
        panBounds.width += 2*expand_x;
        panBounds.height += 2*expand_y;

        bool bPanEligible = panBounds.contains(mousePos);

        sf::Vector2f panAcceleration(0.0f, 0.0f);

        if(bPanEligible &&
           ((mousePos.x > bounds.left - bounds.width * fPanInstigateOuterThreshold) &&
           (mousePos.x < bounds.left + bounds.width * fPanInstigateInnerThreshold)))
        {
            panAcceleration.x = -1.0f + (mousePos.x - (bounds.left - bounds.width * fPanInstigateOuterThreshold)) /
                                (bounds.width * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else if(bPanEligible &&
                ((mousePos.x > bounds.left + bounds.width * (1.0f - fPanInstigateInnerThreshold)) &&
                (mousePos.x < bounds.left + bounds.width * (1.0f + fPanInstigateOuterThreshold))))
        {
            panAcceleration.x = 1.0f - (bounds.left + bounds.width * ( 1.0f + fPanInstigateOuterThreshold) - mousePos.x) /
                                (bounds.width * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else
        {
            panVelocity.x *= fPanAttenutationRate;
        }

        if(bPanEligible &&
           ((mousePos.y > bounds.top - bounds.height * fPanInstigateOuterThreshold) &&
           (mousePos.y < bounds.top + bounds.height * fPanInstigateInnerThreshold)))
        {
            panAcceleration.y = -1.0f + (mousePos.y - (bounds.top - bounds.height * fPanInstigateOuterThreshold)) /
                                (bounds.height * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else if(bPanEligible &&
                ((mousePos.y > bounds.top + bounds.height * (1.0f - fPanInstigateInnerThreshold)) &&
                (mousePos.y < bounds.top + bounds.height * (1.0f + fPanInstigateOuterThreshold))))
        {
            panAcceleration.y = 1.0f - (bounds.top + bounds.height * (1.0f + fPanInstigateOuterThreshold) - mousePos.y) /
                                (bounds.height * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else
        {
            panVelocity.y *= fPanAttenutationRate;
        }

        panAcceleration *= fPanRateScale;

        panVelocity += panAcceleration;
        if(scalar(panVelocity) > fMaxPanSpeed)
        {
            panVelocity *= fMaxPanSpeed/scalar(panVelocity);
        }

        // Perform pan

        panOffset += panVelocity/event.avgFrameRate();

        modLock.lock();

        for(auto& node : nodes)
        {
            if(!event.isCaptured(*node.getElement()))
            {
                node.move(-panVelocity/event.avgFrameRate());
            }
        }

        modLock.unlock();

    }

    // Handle deletion

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
    {

        removeSelected();

    }

    // Handle zoom

    if(bCanZoom)
    {

        float fZoomAcceleration = 0.0f;

        if(event.mouseWheelDelta.y &&
           bounds.contains(mousePos) &&
           event.viewHasFocus)
        {
            zoomAnchor = mousePos;
            fZoomAcceleration += event.mouseWheelDelta.y * fZoomRateScale;
        }

        fCurrentZoomRate += fZoomAcceleration;

        // Perform zoom

        if((fCurrentZoomRate > 1e-5f) ||
           (fCurrentZoomRate < -1e-5f))
        {

            float zoomInc = fCurrentZoomRate/event.avgFrameRate() * fZoomLevel;
            float fLastZoomLevel = fZoomLevel;

            if(fZoomLevel + zoomInc > fMaxZoomLevel)
            {
                fZoomLevel = fMaxZoomLevel;
            }
            else if(fZoomLevel + zoomInc < fMinZoomLevel)
            {
                fZoomLevel = fMinZoomLevel;
            }
            else
            {
                fZoomLevel += zoomInc;
            }

            double dAngle = 0.0;
            float fMouseDistance = 0.0;
            float fZoomScale = fZoomLevel/fLastZoomLevel;

            modLock.lock();

            for(auto& node : nodes)
            {
                fMouseDistance = scalar(node.getPosition() - zoomAnchor);
                dAngle = get_angle(zoomAnchor, node.getPosition());
                node.setPosition(radial_position(zoomAnchor,
                                                 fMouseDistance * fZoomLevel/fLastZoomLevel,
                                                 dAngle));
                node.getElement()->scale_velocity(fZoomScale);
                node.setViewScale(fZoomLevel);
            }

            modLock.unlock();

            fCurrentZoomRate *= fZoomAttenutationRate;

            fLastZoomLevel = fZoomLevel;

        }
        else
        {
            fCurrentZoomRate = 0.0f;
        }

    }

    // Handle control keys

    if(ctrlPressed() && !event.keyLog.empty())
    {
        for(auto& key : event.keyLog)
        {
            if(key == 'c')
            {
                if(!selected.empty())
                {

                    modLock.lock();

                    stringstream copyData;
                    for(size_t i = 0, j, S, L = selected.size(); i < L; ++i)
                    {
                        if(!selected[i].connected_to.empty())
                        {
                            for(j = 0, S = selected[i].connected_to.size(); j < S; ++j)
                            {
                                copyData << selected[i].getTag() << '\t'
                                         << selected[i].connected_to[j].getType()
                                         << '\t' << selected[i].connected_to[j].getNode().getTag();
                                if(j < S - 1)
                                {
                                    copyData << '\n';
                                }
                            }

                            if(i < L - 1)
                            {
                                copyData << '\n';
                            }
                        }
                    }

                    if(!groups.empty())
                    {
                        copyData << "\n\n";
                        copyData << "Term\tMembers\tSize\n";
                        for(auto& pair : groups)
                        {
                            vector<string> group_nodes;
                            for(auto& node : selected)
                            {
                                if(pair.second->checkNode(node))
                                {
                                    group_nodes.emplace_back(node.getTag());
                                }
                            }

                            copyData << pair.first << '\t';
                            for(size_t i = 0; i < group_nodes.size(); ++i)
                            {
                                copyData << group_nodes[i];
                                if(i + 1 < group_nodes.size())
                                {
                                    copyData << ';';
                                }
                            }
                            copyData << '\t' << group_nodes.size();

                            copyData << '\n';
                        }
                    }

                    modLock.unlock();

                    if(!copyData.str().empty())
                    {
                        copyToClipboard(copyData.str());
                    }

                }
            }
            else if(key == 'a')
            {

                modLock.lock();
                select_all();
                modLock.unlock();
            }
        }

    }

    // Handle drop data

    // Update LOD

    updateLOD(event);

    // Handle physics

    if(fNodePushStrength || fTetherElasticCoefficient)
    {

        switch(getLOD())
        {
        case 2: // Every other frame
            {
                if(uFramesLastPhysicsUpdate >= 1)
                {
                    updatePhysics(event, mousePos);
                }
                else
                {
                    ++uFramesLastPhysicsUpdate;
                }

                break;
            }
        case 1: // Every 4th frame
            {
                if(uFramesLastPhysicsUpdate >= 3)
                {
                    updatePhysics(event, mousePos);
                }
                else
                {
                    ++uFramesLastPhysicsUpdate;
                }

                break;
            }
        case 0:
            {

                if(uFramesLastPhysicsUpdate >= 6)
                {
                    updatePhysics(event, mousePos);
                }
                else
                {
                    ++uFramesLastPhysicsUpdate;
                }

                break;
            }
        default: // Every frame
            {

                updatePhysics(event, mousePos);

                break;
            }
        }

    }

    return true;

}

void CVNetworkPanel::updateLOD(CVEvent& event)
{

    uLOD = (unsigned int)(event.avgFrameRate()/48 * 3);

}

void CVNetworkPanel::removeSelected()
{

    if(!selected.empty())
    {
        for(size_t i = 0; i < selected.size(); ++i)
        {
            removePanelElement(selected[i].getElement());
        }

        selected.clear();
    }

}

void CVNetworkPanel::removeNodesByTag(const std::string& tag)
{

    modLock.lock();

    for(size_t i = 0; i < nodes.size();)
    {

        if(nodes[i].getTag() == tag)
        {
            CVBasicViewPanel::removePanelElement(nodes[i].getElement());
            nodes.erase(nodes.begin() + i);
        }
        else ++i;

    }

    modLock.unlock();

}

void CVNetworkPanel::removeNodesByTags(const std::vector<std::string>& tags)
{

    for(auto& tag : tags)
    {

        removeNodesByTag(tag);

    }

}

void CVNetworkPanel::removeNode(CVNetworkNode& node)
{

    modLock.lock();

    for(size_t i = 0; i < nodes.size(); ++i)
    {

        if(&nodes[i] == &node)
        {
            nodes.erase(nodes.begin() + i);
            break;
        }

    }

    modLock.unlock();

}

void CVNetworkPanel::updatePhysics(CVEvent& event, const sf::Vector2f& mousePos)
{

    sf::FloatRect shape1Bounds;
    sf::FloatRect shape2Bounds;

    sf::Vector2f moveDist(0.0f, 0.0f);
    sf::Vector2f shape1ctr(0.0f, 0.0f);
    sf::Vector2f shape2ctr(0.0f, 0.0f);

    double sizeRatio;
    double lin_dist;
    double max_dist;
    double angle;
    double pushDist;
    double r1;
    double r2;

    modLock.lock();

    for(auto& node : nodes)
    {
        if(event.isCaptured(*node.getElement()) || node.isPinned())
        {
            node.bStatic = true;
        }
        else
        {
            node.bStatic = false;
        }

        if(bScaleNodesWithWeight)
        {
            node.setWeight(1.0f + (fNodeEdgeWeightScale * node.numEdges()));
        }

//        if(!node.bStatic && fNetworkCenterGravity)
//        {
//            node.push(get_angle(node.getPosition(), networkCenter),
//                      getDistance(networkCenter, node.getPosition()) * fNetworkCenterGravity,
//                      fNodeFriction);
//        }
    }

    for(auto& pair : groups)
    {

        if(!pair.second->isVisible())
        {
            continue;
        }

        sf::Vector2f gravity_ctr(0.0f, 0.0f);

        for(auto& node : pair.second->nodes)
        {
            gravity_ctr += node.getPosition();
        }

        gravity_ctr /= pair.second->nodes.size();

        for(auto& node : pair.second->nodes)
        {
            if(!node.bStatic)
            {
                node.push(get_angle(node.getPosition(), gravity_ctr),
                          getDistance(gravity_ctr, node.getPosition()) * fGroupCenterGravity / pow(node.numGroups(), 2),
                          fNodeFriction);
            }
        }
    }

    for(size_t i = 0, j, L = nodes.size(); i + 1 < L; ++i){

        for(j = i + 1; j < L; ++j){

            if(nodes[i].isGrouped() || nodes[j].isGrouped())
            {
                float fDistance = getDistance(nodes[i].getPosition(),
                                              nodes[j].getPosition());

                if((fDistance < fGroupPushDistance * fZoomLevel) &&
                   !nodes[i].checkGroups(nodes[j]))
                {

                    angle = get_angle(nodes[i].getPosition(),
                                      nodes[j].getPosition());
                    pushDist = (fGroupPushDistance * fZoomLevel - fDistance) * fGroupPushStrength/2;

                    if(!nodes[i].bStatic)
                    {
                        nodes[i].getElement()->push(angle - PI,
                                                    pushDist,
                                                    fNodeFriction);
                    }
                    if(!nodes[j].bStatic)
                    {
                        nodes[j].getElement()->push(angle,
                                                    pushDist,
                                                    fNodeFriction);
                    }

                }
            }

            shape1Bounds = nodes[i].getBounds();
            shape2Bounds = nodes[j].getBounds();

            if(shape1Bounds.intersects(shape2Bounds))
            {

                sizeRatio = nodes[i].getWeight() / (nodes[i].getWeight() + nodes[j].getWeight());

                r1 = shape1Bounds.width < shape1Bounds.height ? shape1Bounds.width/2 : shape1Bounds.height/2;
                r2 = shape2Bounds.width < shape2Bounds.height ? shape2Bounds.width/2 : shape2Bounds.height/2;

                max_dist = r1 + r2;

                shape1ctr = getBoundCenter(shape1Bounds);
                shape2ctr = getBoundCenter(shape2Bounds);

                lin_dist = getDistance(shape1ctr, shape2ctr);

                if(lin_dist < 1e-6f)
                {
                    angle = rand((long double)(0.0), 2.0*PI);
                }
                else
                {
                    angle = get_angle(shape1ctr, shape2ctr);
                }

                pushDist = max_dist - lin_dist;
                if(pushDist <= 0.0f) continue;

                pushDist *= fNodePushStrength;

                if(nodes[i].isVisible() && !nodes[j].bStatic)
                {
                    nodes[j].getElement()->push(angle, pushDist * sizeRatio, fNodeFriction);
                }

                if(nodes[j].isVisible() && !nodes[i].bStatic)
                {
                    nodes[i].getElement()->push(angle-PI, pushDist * (1.0f - sizeRatio), fNodeFriction);
                }

            }
        }

    }

    for(auto& node : nodes)
    {

        node.apply_tethers(fTetherBaseDistance * fZoomLevel * pow(1.0f + fTetherEdgeDistanceModifier, node.numEdges()),
                           fTetherElasticCoefficient * pow(1.0f + fTetherEdgeElasticModifier, node.numEdges()),
                           fTetherRangeThreshold * pow(1.0f + fTetherEdgeDistanceModifier, node.numEdges()*2),
                           fNodeFriction);

    }

    modLock.unlock();

    uFramesLastPhysicsUpdate = 0;

}

bool CVNetworkPanel::draw(sf::RenderTarget* target)
{

    if(!target || !visible)
    {
        return false;
    }

    CV_DRAW_CLIP_BEGIN

    for(auto& item : panel)
    {
        target->draw(item);
    }

    modLock.lock();

    if(groupMode != CVNetworkGroup::Mode::Centered)
    {
        // Draw group bounds under if boxed or other
        for(auto& pair : groups)
        {
            pair.second->draw(target);
        }
    }

    // Draw edges before nodes
    for(auto& node : nodes)
    {
        node.drawEdges(target, getLOD());
    }

    // Second pass for nodes
    for(auto& node : nodes)
    {
        node.draw(target);
    }

    if(groupMode == CVNetworkGroup::Mode::Centered)
    {
        // Draw group bounds over if centered mode
        for(auto& pair : groups)
        {
            pair.second->draw(target);
        }
    }

    for(auto& text : displayText)
    {
        target->draw(text);
    }

    for(auto& node : nodes)
    {
        node.drawInterface(target);
    }

    modLock.unlock();

    target->draw(selectionCordon);

    for(auto& item : mask)
    {
        target->draw(item);
    }

    if(is_closable())
    {
        closeButton->draw(target);
    }

    CV_DRAW_CLIP_END

    return true;
}

void CVNetworkPanel::getTexture(sf::Texture& output,
                                const sf::Color& canvas_color)
{

    if(nodes.empty())
    {
        return;
    }

    View->captureLock.lock();

    modLock.lock();

    sf::FloatRect networkBounds = nodes.front().getBounds();

    for(size_t i = 1; i < nodes.size(); ++i)
    {
        expandBounds(networkBounds, nodes[i].getBounds());
    }

    for(auto& pair : groups)
    {
        expandBounds(networkBounds, pair.second->getBounds());
        expandBounds(networkBounds, pair.second->annotation->getBounds());
    }

    expandBounds(networkBounds, 32.0f);

    View->textureBuffer.create(networkBounds.width, networkBounds.height);
    View->textureBuffer.setView(sf::View(networkBounds));
    View->textureBuffer.clear(canvas_color);

//    View->mainApp->setContextActive(true);
    View->drawLock.lock();

    if(groupMode != CVNetworkGroup::Mode::Centered)
    {
        for(auto& pair : groups)
        {
            pair.second->draw(&View->textureBuffer);
        }
    }

    for(auto& node : nodes)
    {
        node.drawEdges(&View->textureBuffer, getLOD());
    }

    for(auto& node : nodes)
    {
        node.draw(&View->textureBuffer);
    }

    if(groupMode == CVNetworkGroup::Mode::Centered)
    {
        for(auto& pair : groups)
        {
            pair.second->draw(&View->textureBuffer);
        }
    }

    for(auto& text : displayText)
    {
        View->textureBuffer.draw(text);
    }

    View->drawLock.unlock();
    modLock.unlock();

//    View->mainApp->setContextActive(false);

    View->textureBuffer.display();
    output = View->textureBuffer.getTexture();

    View->textureBuffer.setActive(false);

    View->captureLock.unlock();

}

void CVNetworkPanel::addPanelElement(CVElement* element,
                                    const string& newTag,
                                    const string& newType,
                                    const float& weight,
                                    const unsigned int& label_orientation)
{

    modLock.lock();

    CVBasicViewPanel::addPanelElement(element, newTag);

    sf::Color nodeTextColor;
    try
    {
        nodeTextColor = getTextLegendColor(newType);
    }catch(...)
    {
        nodeTextColor = getDefaultNodeTextColor();
    }

    element->setSize(element->getSize());

    nodes.emplace_back(element, newType, weight, appFont(textInfo.font), textInfo.fontSize, label_orientation, nodeTextColor);
    nodes.back().setViewScale(fZoomLevel);

    bool bIntersects;

    double angle = rand(0.0, double(2*PI));

    do
    {
        bIntersects = false;
        for(size_t i = 0; i + 1 < nodes.size(); ++i)
        {
            if(nodes[i].getBounds().intersects(nodes.back().getBounds()))
            {
                nodes.back().move(components(sqrt(nodes.back().getBounds().width * nodes.back().getBounds().height), angle));
                bIntersects = true;
            }
        }
    }while(bIntersects);

    element->setHighlightColor(selectionColor);

    updateNodeConnections(nodes.back());

    modLock.unlock();
}

void CVNetworkPanel::removePanelElement(CVElement* element)
{

    modLock.lock();

    for(size_t i = 0; i < nodes.size(); ++i)
    {
        if(nodes[i].getElement() == element)
        {
            CVBasicViewPanel::removePanelElement(nodes[i].getElement());
            nodes.erase(nodes.begin() + i);
        }
    }

    modLock.unlock();
}

CVButton* CVNetworkPanel::addNode(const string& tag,
                                  const string& type,
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size)
{

    CVButton* newNode = nullptr;

    if(bUniqueNodesOnly)
    {
        for(auto& node : nodes)
        {
            if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
            {
                return nullptr;
            }
        }
    }

    sf::Color newNodeFillColor;
    sf::Color newNodeOutlineColor;
    sf::Color newNodeTextColor;

    try
    {
        newNodeFillColor = nodeLegend.at(type);
    }
    catch(...){
        newNodeFillColor = getDefaultNodeFillColor();
    }

    try
    {
        newNodeOutlineColor = outlineLegend.at(type);
    }
    catch(...){
        newNodeOutlineColor = getDefaultNodeOutlineColor();
    }

    try
    {
        newNodeTextColor = textColorLegend.at(type);
    }
    catch(...){
        newNodeTextColor = getDefaultNodeTextColor();
    }

    if(isnan(position.x) || isnan(size.x))
    {
        sf::Vector2f newPosition;
        sf::Vector2f newSize;

        if(isnan(position.x) || isnan(position.y))
        {
            newPosition = getPosition() + defaultNodePosition;
        }
        else newPosition = position;

        if(isnan(size.x) || isnan(size.y))
        {
            newSize = defaultNodeSize;
        }

        newNode = new CVButton(View, newPosition, newSize.x, newSize.y,
                                 TextEntry("", textInfo.font,
                                           textInfo.fontSize * pow(weight, fontWeightScale),
                                           ALIGN_CENTER_MIDLINE,
                                           newNodeTextColor),
                                 "", newNodeFillColor, newNodeOutlineColor,
                                 defaultNodeOutlineThickness);

        newNode->setOrigin(newSize/2);
        newNode->move(-newSize/2);

        if(isnan(defaultNodeRounding))
        {
            newNode->setRounding(newSize.x > newSize.y ? newSize.x/2 : newSize.y/2);
        }
        else
        {
            newNode->setRounding(defaultNodeRounding);
        }
    }
    else
    {
        newNode = new CVButton(View, position, size.x, size.y,
                                 TextEntry("", textInfo.font,
                                           textInfo.fontSize * pow(weight, fontWeightScale),
                                           ALIGN_CENTER_MIDLINE,
                                           newNodeTextColor),
                                 "", newNodeFillColor, newNodeOutlineColor,
                                 defaultNodeOutlineThickness);

        newNode->setOrigin(size/2);
        newNode->move(-size/2);

        if(isnan(defaultNodeRounding))
        {
            newNode->setRounding(size.x > size.y ? size.x/2 : size.y/2);
        }
        else
        {
            newNode->setRounding(defaultNodeRounding);
        }
    }

    newNode->setDraggableStatus(true);

    addPanelElement(newNode, tag, type, weight, defaultNodeTextAlignment);

    return newNode;

}

CVButton* CVNetworkPanel::addNode(const string& tag,
                                  const string& type,
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size,
                                  const sf::Color& fillColor,
                                  const sf::Color& outlineColor,
                                  const float& outlineThickness,
                                  const float& rounding)
{
    CVButton* newNode = nullptr;

    if(bUniqueNodesOnly)
    {
        for(auto& node : nodes)
        {
            if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
            {
                return nullptr;
            }
        }
    }

    if(isnan(position.x) || isnan(size.x))
    {
        sf::Vector2f newPosition;
        sf::Vector2f newSize;

        if(isnan(position.x) || isnan(position.y))
        {
            newPosition = getPosition() + defaultNodePosition;
        }
        else
        {
            newPosition = position;
        }

        if(isnan(size.x) || isnan(size.y))
        {
            newSize = defaultNodeSize;
        }

        newNode = new CVButton(View, newPosition, newSize.x, newSize.y,
                                 TextEntry("", textInfo.font,
                                           textInfo.fontSize * pow(weight, fontWeightScale),
                                           ALIGN_CENTER_MIDLINE,
                                           textInfo.textColor),
                                 "", fillColor, outlineColor,
                                 outlineThickness);

        newNode->setOrigin(newSize/2);
        newNode->move(-newSize/2);

        if(isnan(rounding))
        {
            newNode->setRounding(newSize.x > newSize.y ? newSize.x/2 : newSize.y/2);
        }
        else
        {
            newNode->setRounding(rounding);
        }
    }
    else
    {
        newNode = new CVButton(View, position, size.x, size.y,
                                 TextEntry("", textInfo.font,
                                           textInfo.fontSize * pow(weight, fontWeightScale),
                                           ALIGN_CENTER_MIDLINE,
                                           textInfo.textColor),
                                 "", fillColor, outlineColor,
                                 outlineThickness);

        newNode->setOrigin(size/2);
        newNode->move(-size/2);

        if(isnan(rounding))
        {
            newNode->setRounding(size.x > size.y ? size.x/2 : size.y/2);
        }
        else
        {
            newNode->setRounding(rounding);
        }
    }

    newNode->setDraggableStatus(true);

    addPanelElement(newNode, tag, type, weight, defaultNodeTextAlignment);

    return newNode;
}

CVButton* CVNetworkPanel::addImageNode(const string& tag,
                                       const string& texture,
                                       const string& type,
                                       const float& weight,
                                       const sf::Vector2f& position,
                                       const sf::Vector2f& size)
{

    if(!appTexture(texture)) return nullptr;

    if(bUniqueNodesOnly)
    {
        for(auto& node : nodes)
        {
            if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
            {
                return nullptr;
            }
        }
    }

    sf::Color newNodeFillColor;

    try
    {
        newNodeFillColor = nodeLegend.at(type);
    }
    catch(...){
        newNodeFillColor = getDefaultNodeFillColor();
    }

    CVButton* newNode = nullptr;

    if(isnan(position.x) || isnan(size.x))
    {
        sf::Vector2f newPosition;
        sf::Vector2f newSize;

        if(isnan(position.x) || isnan(position.y))
        {
            newPosition = getPosition() + defaultNodePosition;
        }
        else
        {
            newPosition = position;
        }

        if(isnan(size.x) || isnan(size.y))
        {
            newSize = defaultNodeSize;

            sf::Vector2f textureSize(appTexture(texture)->getSize());

            if(textureSize.x > textureSize.y)
            {
                newSize.y *= textureSize.y/textureSize.x;
            }
            else if(textureSize.y > textureSize.x)
            {
                newSize.x *= textureSize.x/textureSize.y;
            }
        }

        newNode = new CVButton(View, newPosition, newSize.x, newSize.y,
                                 texture, 1);

    }
    else
    {
        newNode = new CVButton(View, position, size.x, size.y,
                                 texture, 1);
    }

    newNode->setSpriteColor(newNodeFillColor);
    newNode->setDraggableStatus(true);

    addPanelElement(newNode, tag, type, weight, defaultNodeTextAlignment);

    return newNode;

}

CVButton* CVNetworkPanel::addImageNode(const string& tag,
                                       const string& texture,
                                       const string& type,
                                       const float& weight,
                                       const sf::Vector2f& position,
                                       const sf::Vector2f& size,
                                       const sf::Color& spriteColor,
                                       const sf::Color& fillColor,
                                       const sf::Color& outlineColor,
                                       const float& outlineThickness,
                                       const float& rounding)
{

    if(!appTexture(texture)) return nullptr;

    if(bUniqueNodesOnly)
    {
        for(auto& node : nodes)
        {
            if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
            {
                return nullptr;
            }
        }
    }

    CVButton* newNode = nullptr;

    if(isnan(position.x) || isnan(size.x))
    {
        sf::Vector2f newPosition;
        sf::Vector2f newSize;

        if(isnan(position.x) || isnan(position.y))
        {
            newPosition = getPosition() + defaultNodePosition;
        }
        else
        {
            newPosition = position;
        }

        if(isnan(size.x) || isnan(size.y))
        {
            newSize = defaultNodeSize;

            sf::Vector2f textureSize(appTexture(texture)->getSize());

            if(textureSize.x > textureSize.y)
            {
                newSize.y *= textureSize.y/textureSize.x;
            }
            else if(textureSize.y > textureSize.x)
            {
                newSize.x *= textureSize.x/textureSize.y;
            }

        }

        newNode = new CVButton(View, newPosition, newSize.x, newSize.y, texture, 1);

        if(isnan(rounding))
        {
            newNode->setRounding(newSize.x > newSize.y ? newSize.x/2 : newSize.y/2);
        }
        else
        {
            newNode->setRounding(rounding);
        }
    }
    else
    {
        newNode = new CVButton(View, position, size.x, size.y, texture, 1);

        if(isnan(rounding))
        {
            newNode->setRounding(size.x > size.y ? size.x/2 : size.y/2);
        }
        else
        {
            newNode->setRounding(rounding);
        }
    }

    newNode->setFillColor(fillColor);
    newNode->setOutlineColor(outlineColor);
    newNode->setSpriteColor(spriteColor);
    newNode->setOutlineThickness(outlineThickness);
    newNode->setDraggableStatus(true);

    addPanelElement(newNode, tag, type, weight, defaultNodeTextAlignment);

    return newNode;
}

bool CVNetworkPanel::nodeExists(const std::string& str) const noexcept
{
    for(size_t i = 0; i < nodes.size(); ++i)
    {
        if(cmpString(str, nodes[i].getTag(), CMP_STR_CASE_INSENSITIVE))
        {
            return true;
        }
    }

    return false;
}

void CVNetworkPanel::connectByTag(const string& source,
                                  const string& target,
                                  const float& weight,
                                  const string& type,
                                  const bool& bidirectional)
{

    if(nodes.size() < 2) return;

    sf::Color edgeColor;

    try
    {
        edgeColor = getEdgeColor(type);
    }
    catch(...)
    {
        edgeColor = getDefaultEdgeColor();
    }

    modLock.lock();

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpString(nodes[i].getTag(), source, CMP_STR_CASE_INSENSITIVE) &&
               cmpString(nodes[j].getTag(), target, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_out(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
            else if(cmpString(nodes[i].getTag(), target, CMP_STR_CASE_INSENSITIVE) &&
                    cmpString(nodes[j].getTag(), source, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_in(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
        }
    }

    modLock.unlock();

}

void CVNetworkPanel::connectByTags(const string& source,
                                   const vector<string>& targets,
                                   const float& weight,
                                   const string& type,
                                   const bool& bidirectional)
{

    if(nodes.size() < 2) return;

    sf::Color edgeColor;

    try
    {
        edgeColor = getEdgeColor(type);
    }
    catch(...)
    {
        edgeColor = getDefaultEdgeColor();
    }

    modLock.lock();

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if((nodes[i].getTag() == source) && anyEqual(nodes[j].getTag(), targets))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_out(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
            else if(anyEqual(nodes[i].getTag(), targets) && (nodes[j].getTag() == source))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_in(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
        }
    }

    modLock.unlock();

}

void CVNetworkPanel::connectByTags(const vector<string>& sources,
                                   const string& target,
                                   const float& weight,
                                   const string& type,
                                   const bool& bidirectional)
{

    if(nodes.size() < 2) return;

    sf::Color edgeColor;

    try
    {
        edgeColor = getEdgeColor(type);
    }
    catch(...)
    {
        edgeColor = getDefaultEdgeColor();
    }

    modLock.lock();

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpStringToList(nodes[i].getTag(), sources, CMP_STR_CASE_INSENSITIVE) &&
               cmpString(nodes[j].getTag(), target, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_out(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
            else if(cmpString(nodes[i].getTag(), target, CMP_STR_CASE_INSENSITIVE),
                    cmpStringToList(nodes[j].getTag(), sources, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_in(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
        }
    }

    modLock.unlock();

}

void CVNetworkPanel::connectByTags(const vector<string>& sources,
                                   const vector<string>& targets,
                                   const float& weight,
                                   const string& type,
                                   const bool& bidirectional)
{

    if(nodes.size() < 2) return;

    sf::Color edgeColor;

    try
    {
        edgeColor = getEdgeColor(type);
    }
    catch(...)
    {
        edgeColor = getDefaultEdgeColor();
    }

    modLock.lock();

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpStringToList(nodes[i].getTag(), sources, CMP_STR_CASE_INSENSITIVE) &&
               cmpStringToList(nodes[j].getTag(), targets, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_out(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
            else if(cmpStringToList(nodes[i].getTag(), targets, CMP_STR_CASE_INSENSITIVE) &&
                    cmpStringToList(nodes[j].getTag(), sources, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j], weight, edgeColor, type);
                }
                else
                {
                    nodes[i].connect_in(nodes[j], weight, edgeColor, type);
                }

                if(bUniqueNodesOnly) break;
            }
        }
    }

    modLock.unlock();

}

void CVNetworkPanel::select(CVElement* element)
{
    for(auto& node : nodes)
    {
        if((node.getElement() == element) &&
           !node.isSelected())
        {
            node.setSelected(true);
            selected.emplace_back(node);
            return;
        }
    }
}

void CVNetworkPanel::deselect(CVElement* element)
{
    for(size_t i = 0; i < selected.size();)
    {
        if(selected[i].getElement() == element)
        {
            selected[i].setSelected(false);
            selected.erase(selected.begin() + i);
        }
        else ++i;
    }
}

void CVNetworkPanel::toggle_select(CVElement* element)
{
    for(auto& node : nodes)
    {
        if(node.getElement() == element)
        {
            if(node.isSelected())
            {
                deselect(node);
            }
            else
            {
                select(node);
            }

            return;
        }
    }
}

void CVNetworkPanel::select(CVNetworkNode& node)
{
    select(node.getElement());
}

void CVNetworkPanel::deselect(CVNetworkNode& node)
{
    deselect(node.getElement());
}

void CVNetworkPanel::toggle_select(CVNetworkNode& node)
{
    toggle_select(node.getElement());
}

void CVNetworkPanel::select_all()
{
    for(auto& node : nodes)
    {
        if(!node.isSelected())
        {
            node.setSelected(true);
            selected.emplace_back(node);
        }
    }
}

void CVNetworkPanel::select_none()
{
    for(auto& node : selected)
    {
        node.setSelected(false);
    }
    selected.clear();
}

void CVNetworkPanel::select_neighbors(CVNetworkNode& center)
{

    select(center);

    for(auto& node : nodes)
    {
        if(&node == &center)
        {
            continue;
        }

        if(node.hasConnectionTo(center))
        {
            select(node);
        }
    }

}

void CVNetworkPanel::select_neighbors(const std::string& tag)
{

    for(auto& node : nodes)
    {
        if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
        {
            select_neighbors(node);
        }
    }

}

bool CVNetworkPanel::node_has_neighbors(const CVNetworkNode& node)
{
    return node.hasConnections();
}

bool CVNetworkPanel::node_has_neighbors(const std::string& tag)
{
    for(auto& node : nodes)
    {
        if(cmpString(node.getTag(), tag, CMP_STR_CASE_INSENSITIVE))
        {
            return node.hasConnections();
        }
    }

    return false;
}

void CVNetworkPanel::setCordonState(const bool& state) noexcept
{

    if(bCanCordonSelect != state)
    {

        if(!state)
        {
            selectionCordon.setSize(sf::Vector2f(0.0f, 0.0f));
            bSelection = false;
        }

        bCanCordonSelect = state;

    }

}

void CVNetworkPanel::selectGroup(CVNetworkGroup& group) noexcept
{

    for(auto& node : group.nodes)
    {
        if(!isSelected(node.getElement()))
        {
            selected.emplace_back(node);
        }
    }

}

void CVNetworkPanel::selectGroup(const string& group_name) noexcept
{
    try
    {
        selectGroup(*groups.at(group_name));
    }catch(...){ }
}

void CVNetworkPanel::pinSelected() noexcept
{
    for(auto& node : selected)
    {
        node.pin();
    }
}

void CVNetworkPanel::unpinSelected() noexcept
{
    for(auto& node : selected)
    {
        node.unpin();
    }
}

void CVNetworkPanel::pinAll() noexcept
{
    for(auto& node : nodes)
    {
        node.pin();
    }
}

void CVNetworkPanel::unpinAll() noexcept
{
    for(auto& node : nodes)
    {
        node.unpin();
    }
}

void CVNetworkPanel::setGroupMode(const CVNetworkGroup::Mode& newGroupMode) noexcept
{

    if(newGroupMode != groupMode)
    {

        groupMode = newGroupMode;

        switch(groupMode)
        {
        case CVNetworkGroup::Mode::Centered:
            {
                for(auto& pair : groups)
                {
                    pair.second->setFillColor(sf::Color::Transparent);
                    pair.second->setOutlineColor(sf::Color::Transparent);
                }
                break;
            }
        default: // Boxed
            {
                for(auto& pair : groups)
                {

                    sf::Color newGroupFill(rand(0, 255),
                                           rand(0, 255),
                                           rand(0, 255),
                                           7);
                    sf::Color newGroupOutline = newGroupFill - sf::Color(60, 60, 60, 0) + sf::Color(0, 0, 0, 168);

                    pair.second->setFillColor(newGroupFill);
                    pair.second->setOutlineColor(newGroupOutline);

                }
                break;
            }
        }

    }

}

void CVNetworkPanel::addToGroup(const vector<string>& tags,
                                const string& group_tag)
{

    modLock.lock();

    CVNetworkGroup * group = nullptr;

    if(!groupExists(group_tag))
    {
        group = createGroup(group_tag);
        groups.emplace(group_tag, group);
    }

    try
    {
        if(!group)
        {
            group = groups.at(group_tag);
        }

        for(auto& tag : tags)
        {
            for(auto& node : nodes)
            {
                if(node.getTag() == tag)
                {
                    group->addNode(node);
                }
            }
        }
    }catch(std::out_of_range)
    {
        throw std::out_of_range("CVNetworkPanel::addToGroup(): requested group does not exist");
    }

    modLock.unlock();

}

void CVNetworkPanel::addToGroup(const string& tag,
                                const string& group_tag)
{

    modLock.lock();

    CVNetworkGroup * group = nullptr;

    if(!groupExists(group_tag))
    {
        group = createGroup(group_tag);
        groups.emplace(group_tag, group);
    }

    try
    {

        if(!group)
        {
            group = groups.at(group_tag);
        }

        for(auto& node : nodes)
        {
            if(node.getTag() == tag)
            {
                group->addNode(node);
            }
        }

    }catch(std::out_of_range)
    {
        throw std::out_of_range("CVNetworkPanel::addToGroup(): requested group does not exist");
    }

    modLock.unlock();

}

void CVNetworkPanel::addSelectedToGroup(const string& group_tag)
{

    modLock.lock();

    CVNetworkGroup * group = nullptr;

    if(!groupExists(group_tag))
    {
        group = createGroup(group_tag);
        groups.emplace(group_tag, group);
    }

    try
    {

        if(!group)
        {
            group = groups.at(group_tag);
        }

        for(auto& node : selected)
        {
            group->addNode(node);
        }

    }catch(std::out_of_range)
    {
        throw std::out_of_range("CVNetworkPanel::addSelectedToGroup(): requested group does not exist");
    }

    modLock.unlock();

}

CVNetworkGroup * CVNetworkPanel::createGroup(const string& group_tag)
{

    CVNetworkGroup * newGroup = nullptr;

    switch(groupMode)
    {
    case CVNetworkGroup::Mode::Centered:
        {
            sf::Color newGroupText(rand(0, 165),
                                   rand(0, 165),
                                   rand(0, 165),
                                   255);

            newGroup = new CVNetworkGroup(View,
                                          reference_vector<CVNetworkNode>(),
                                          groupMode,
                                          TextEntry(group_tag, textInfo.font, 18, ALIGN_LEFT, newGroupText),
                                          sf::Color::Transparent,
                                          sf::Color::Transparent);
            break;
        }
    default:
        {
            sf::Color newGroupFill(rand(0, 255),
                                   rand(0, 255),
                                   rand(0, 255),
                                   7);
            sf::Color newGroupOutline = newGroupFill - sf::Color(60, 60, 60, 0) + sf::Color(0, 0, 0, 168);
            sf::Color newGroupText = newGroupOutline - sf::Color(40, 40, 40, 0) + sf::Color(0, 0, 0, 70);

            newGroup = new CVNetworkGroup(View,
                                          reference_vector<CVNetworkNode>(),
                                          groupMode,
                                          TextEntry(group_tag, textInfo.font, 18, ALIGN_LEFT, newGroupText),
                                          newGroupFill,
                                          newGroupOutline);
            break;
        }
    }

    return newGroup;

}

void CVNetworkPanel::removeGroup(const string& group_tag)
{

    modLock.lock();

    try
    {
        delete(groups.at(group_tag));
        groups.erase(group_tag);
    }catch(...){ }

    modLock.unlock();

}

void CVNetworkPanel::clearGroups()
{

    modLock.lock();

    for(auto& pair : groups)
    {
        delete(pair.second);
    }

    groups.clear();

    modLock.unlock();

}

void CVNetworkPanel::trimGroupSizes(const int& lower_limit,
                                    const int& upper_limit)
{

    vector<string> rm_groups;

    modLock.lock();

    for(auto& pair : groups)
    {
        if((pair.second->size() < lower_limit) ||
           (pair.second->size() > upper_limit))
        {
            rm_groups.emplace_back(pair.first);
        }
    }

    for(auto& group : rm_groups)
    {
        delete(groups.at(group));
        groups.erase(group);
    }

    modLock.unlock();

}

void CVNetworkPanel::mergeGroups(reference_vector<CVNetworkGroup>& groups,
                                 const string& newGroupName)
{

    modLock.lock();

    CVNetworkGroup * newGroup = nullptr;

    if(newGroupName.empty())
    {

        int i = 1;
        string newName = "Group 1";

        while(groupExists(newName))
        {
            ++i;
            newName = "Group " + to_string(i);
        }

        newGroup = createGroup(newName);
        this->groups.emplace(newName, newGroup);

    }
    else
    {
        newGroup = createGroup(newGroupName);
        this->groups.emplace(newGroupName, newGroup);
    }

    merge(groups, *newGroup);

    modLock.unlock();

}

void CVNetworkPanel::mergeGroupsByKeywords(const float& threshold) noexcept
{

    reference_vector<CVNetworkGroup> group_refs;
    vector<reference_vector<CVNetworkGroup>> group_bundles;
    vector<vector<string>> key_words;

    modLock.lock();

    for(auto& pair : groups)
    {
        group_refs.emplace_back(*pair.second);
    }

    for(size_t i = 0, j, L = group_refs.size(); i + 1 < L; ++i)
    {

        group_bundles.emplace_back();
        group_bundles.back().emplace_back(group_refs[i]);
        key_words.emplace_back(group_refs[i].key_words);

        for(j = i + 1; j < L;)
        {

            if(group_refs[j].checkKeyWords(key_words[i]) > threshold)
            {
                key_words[i] = getOverlappingStrings(key_words[i], group_refs[j].key_words);
                group_bundles.back().emplace_back(group_refs[j]);
                group_refs.erase(group_refs.begin() + j);
                --L;
            }
            else
            {
                ++j;
            }
        }

    }

    modLock.unlock();

    vector<CVNetworkGroup*> rmGroups;

    map<string, CVNetworkGroup*> newGroups;

    for(size_t i = 0; i < group_bundles.size(); ++i)
    {

        if(group_bundles[i].size() < 2)
        {
            continue;
        }

        string newGroupName = concatenateString(key_words[i], " ");

        CVNetworkGroup * newGroup = createGroup(newGroupName);

        newGroups.emplace(newGroupName, newGroup);

        merge(group_bundles[i], *newGroup);

        for(auto& group : group_bundles[i])
        {
            rmGroups.emplace_back(&group);
        }

    }

    modLock.lock();

    for(auto& group : rmGroups)
    {
        for(auto it = groups.begin(); it != groups.end(); ++it)
        {
            if(it->second == group)
            {
                delete(it->second);
                groups.erase(it);
                break;
            }
        }
    }

    for(auto& pair : newGroups)
    {
        groups.emplace(pair.first, pair.second);
    }

    modLock.unlock();

}

void CVNetworkPanel::removeSelectedFromGroups()
{

    modLock.lock();

    for(auto& pair : groups)
    {
        for(auto& node : selected)
        {
            pair.second->removeNode(node);
        }
    }

    modLock.unlock();

}

void CVNetworkPanel::setNodeFillColor(const sf::Color& newColor) noexcept
{
    nodeLegend["__DEFAULT__"] = newColor;
}

const sf::Color& CVNetworkPanel::getNodeColor(const string& type) const
{
    try
    {
        return nodeLegend.at(type);
    }catch(...)
    {
        throw std::out_of_range("CVNetworkPanel: no matching node color in legend for requested type \"" + type + "\"");
    }
}

const sf::Color& CVNetworkPanel::getEdgeColor(const string& type) const
{
    try
    {
        return edgeLegend.at(type);
    }catch(...)
    {
        throw std::out_of_range("CVNetworkPanel: no matching edge color in legend for requested type \"" + type + "\"");
    }
}

const sf::Color& CVNetworkPanel::getOutlineLegendColor(const string& type) const
{
    try
    {
        return outlineLegend.at(type);
    }catch(...)
    {
        throw std::out_of_range("CVNetworkPanel: no matching outline color in legend for requested type \"" + type + "\"");
    }
}

const sf::Color& CVNetworkPanel::getTextLegendColor(const string& type) const
{
    try
    {
        return textColorLegend.at(type);
    }catch(...)
    {
        throw std::out_of_range("CVNetworkPanel: no matching text color in legend for requested type \"" + type + "\"");
    }
}

vector<string> CVNetworkPanel::getNodeTags() const noexcept
{

    vector<string> output;
    output.reserve(nodes.size());

    for(size_t i = 0; i < nodes.size(); ++i)
    {
        output.emplace_back(nodes[i].getTag());
    }

    return output;

}

sf::Vector2f CVNetworkPanel::convert_to_local(const sf::Vector2f& screen_coords) const noexcept
{

    return (screen_coords - panOffset) * fZoomLevel;

}

sf::Vector2f CVNetworkPanel::convert_to_screen(const sf::Vector2f& local_coords) const noexcept
{

    return local_coords/fZoomLevel - panOffset;

}

}
