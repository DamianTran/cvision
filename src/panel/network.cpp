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

#include <EZC/algorithm.hpp>
#include <EZC/toolkit/string.hpp>

using namespace EZC;
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
    fUISizePaddingScale(1.2f),
    fadeLayers(CV_LAYER_ALL)
{
    align_text(alignment);
    displayText.setFillColor(textFillColor);
}

CVNetworkNode::~CVNetworkNode()
{

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
        if((node.getElement() == link.getOrigin().getElement()) &&
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
        if((node.getElement() == link.getNode().getElement()) &&
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

    element->setSize(element->getSize() * newScale/fScale);
    element->setSpriteScale(newScale);
    align_text(textAlignment);
    fScale = newScale;

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
        edge.update();
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

        attached_UI->update(event, mousePos);
        attached_UI->setPosition(getPosition());

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

void CVNetworkNode::drawEdges(sf::RenderTarget* target)
{
    if(target && isVisible())
    {
        for(auto& edge : connected_to)
        {
            edge.draw(target);
        }
    }
}

void CVNetworkNode::attach_UI(CVElement* newUI,
                              const uint8_t& alpha,
                              const unsigned char& fadeLayers)
{
    newUI->setSize(getSize() * fUISizePaddingScale);
    newUI->setPosition(getPosition() - newUI->getSize()/2);
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
                                  const float& range_threshold) noexcept
{
    for(auto& edge : connected_to)
    {
        edge.apply_tether(distance, elastic_coefficient, range_threshold);
    }
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
                                  origin(&origin),
                                  node(&outNode),
                                  type(type),
                                  weight(weight),
                                  lineWeightScale(1.0f),
                                  lineWidth(2.0f)
{

    line.setFillColor(edgeColor);

}

void CVNetworkEdge::setLineWidth(const float& newWidth) noexcept
{
    lineWidth = newWidth;
}

void CVNetworkEdge::apply_tether(const float& distance,
                                 const float& elastic_coefficient,
                                 const float& range_threshold)
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

    if(!getOrigin().bStatic) getOrigin().move(components(adjust * getNode().getWeight() / totalWeight, theta1));
    if(!getNode().bStatic) getNode().move(components(adjust * getOrigin().getWeight() / totalWeight, theta2));

}

void CVNetworkEdge::draw(sf::RenderTarget* target)
{
    if(target && origin->isVisible() && node->isVisible())
    {
        target->draw(line);
    }
}

void CVNetworkEdge::update()
{

    sf::Vector2f originPos = getBoundCenter(origin->getBounds());
    sf::Vector2f destinationPos = getBoundCenter(node->getBounds());

    line.setSize(sf::Vector2f(getDistance(originPos, destinationPos),
                              lineWidth * lineWeightScale));
    line.setOrigin(sf::Vector2f(0.0f, lineWidth * lineWeightScale/2));
    line.setPosition(originPos);
    line.setRotation(get_angle(originPos, destinationPos)*180/PI);

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
                                 defaultNodePosition(size/2),
                                 defaultNodeSize(sf::Vector2f(96.0f * viewScale(),
                                                              96.0f * viewScale())),
                                 panOffset(0.0f, 0.0f),
                                 panVelocity(0.0f, 0.0f),
                                 zoomAnchor(position + size/2),
                                 fontWeightScale(1.0f),
                                 defaultNodeOutlineThickness(2.0f),
                                 defaultNodeRounding(NAN),
                                 fNodePushStrength(0.5f),
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
                                 selectionColor(sf::Color::Yellow),
                                 defaultNodeTextAlignment(ALIGN_CENTER_MIDLINE),
                                 bUniqueNodesOnly(false),
                                 bSelection(false),
                                 bCanPan(true),
                                 bCanZoom(true),
                                 bCanCordonSelect(true)
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

}

bool CVNetworkPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    for(auto& node : nodes)
    {
        node.updateInterface(event, mousePos);
    }

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    for(auto& node : nodes)
    {
        node.update(event, mousePos);
    }

    // Handle selection

    if(!bSelection)
    {
        if(event.LMBhold &&
           bounds.contains(event.LMBpressPosition) &&
           event.focusFree())
        {

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

            for(auto& node : nodes)
            {
                if(contains(node.getBounds(), selectionCordon.getGlobalBounds()))
                {
                    select(node);
                }
            }

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

    if(bCanPan && event.focusFree())
    {

        sf::Vector2f panAcceleration(0.0f, 0.0f);

        if((mousePos.x > bounds.left - bounds.width * fPanInstigateOuterThreshold) &&
           (mousePos.x < bounds.left + bounds.width * fPanInstigateInnerThreshold))
        {
            panAcceleration.x = -1.0f + (mousePos.x - (bounds.left - bounds.width * fPanInstigateOuterThreshold)) /
                                (bounds.width * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else if((mousePos.x > bounds.left + bounds.width * (1.0f - fPanInstigateInnerThreshold)) &&
                (mousePos.x < bounds.left + bounds.width * (1.0f + fPanInstigateOuterThreshold)))
        {
            panAcceleration.x = 1.0f - (bounds.left + bounds.width * ( 1.0f + fPanInstigateOuterThreshold) - mousePos.x) /
                                (bounds.width * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else
        {
            panVelocity.x *= fPanAttenutationRate;
        }

        if((mousePos.y > bounds.top - bounds.height * fPanInstigateOuterThreshold) &&
           (mousePos.y < bounds.top + bounds.height * fPanInstigateInnerThreshold))
        {
            panAcceleration.y = -1.0f + (mousePos.y - (bounds.top - bounds.height * fPanInstigateOuterThreshold)) /
                                (bounds.height * (fPanInstigateInnerThreshold + fPanInstigateOuterThreshold));
        }
        else if((mousePos.y > bounds.top + bounds.height * (1.0f - fPanInstigateInnerThreshold)) &&
                (mousePos.y < bounds.top + bounds.height * (1.0f + fPanInstigateOuterThreshold)))
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

        for(auto& node : nodes)
        {
            if(!event.isCaptured(*node.getElement()))
            {
                node.move(-panVelocity/event.avgFrameRate());
            }
        }

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

        if(event.mouseWheelDelta.y)
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

            for(auto& node : nodes)
            {
                fMouseDistance = scalar(node.getPosition() - zoomAnchor);
                dAngle = get_angle(zoomAnchor, node.getPosition());
                node.setPosition(radial_position(zoomAnchor,
                                                 fMouseDistance * fZoomLevel/fLastZoomLevel,
                                                 dAngle));
                node.setScale(fZoomLevel);
            }

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

                    stringstream copyData;
                    for(size_t i = 0, j, S, L = selected.size(); i < L; ++i)
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

                    if(!copyData.str().empty())
                    {
                        copyToClipboard(copyData.str());
                    }
                }
            }
            else if(key == 'a')
            {
                select_all();
            }
        }

    }

    // Handle drop data

    // Handle physics

    if(fNodePushStrength || fTetherElasticCoefficient)
    {
        updatePhysics(event, mousePos);
    }

    // Handle layout requests

    return true;

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

    for(size_t i = 0; i < nodes.size();)
    {

        if(nodes[i].getTag() == tag)
        {

            nodes.erase(nodes.begin() + i);

        }
        else ++i;

    }

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

    for(size_t i = 0; i < nodes.size(); ++i)
    {

        if(&nodes[i] == &node)
        {
            nodes.erase(nodes.begin() + i);
            break;
        }

    }

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

    for(auto& node : nodes)
    {
        if(event.isCaptured(*node.getElement()))
        {
            node.bStatic = true;
        }
        else
        {
            node.bStatic = false;
        }

        node.setWeight(1.0f + (fNodeEdgeWeightScale * node.numEdges()));
    }

    for(size_t i = 0, j, L = nodes.size(); i + 1 < L; ++i){

        for(j = i + 1; j < L; ++j){

            shape1Bounds = nodes[i].getBounds();
            shape2Bounds = nodes[j].getBounds();

            if(shape1Bounds.intersects(shape2Bounds))
            {

                sizeRatio = (shape1Bounds.width*shape1Bounds.height)/
                (shape1Bounds.width*shape1Bounds.height + shape2Bounds.height*shape2Bounds.width);

                r1 = shape1Bounds.width < shape1Bounds.height ? shape1Bounds.width/2 : shape1Bounds.height/2;
                r2 = shape2Bounds.width < shape2Bounds.height ? shape2Bounds.width/2 : shape2Bounds.height/2;

                max_dist = r1 + r2;

                shape1ctr = getBoundCenter(shape1Bounds);
                shape2ctr = getBoundCenter(shape2Bounds);

                lin_dist = getDistance(shape1ctr, shape2ctr);

                if(lin_dist < 1e-6f)
                {
                    lin_dist = max_dist / 100;
                    angle = rand(0.0f, 360.0f)/180*PI;
                }
                else
                {
                    angle = get_angle(shape1ctr, shape2ctr);
                }

                pushDist = max_dist - lin_dist;
                if(pushDist <= 0.0f) continue;

                pushDist *= fNodePushStrength/2;

                if(nodes[i].isVisible() && !nodes[j].bStatic)
                {
                    moveDist = sf::Vector2f(components(pushDist * sizeRatio, angle));
                    nodes[j].move(moveDist);
                }

                if(nodes[j].isVisible() && !nodes[i].bStatic)
                {
                    moveDist = sf::Vector2f(components(pushDist * (1.0f - sizeRatio), angle));
                    nodes[i].move(-moveDist);
                }

            }
        }

        nextLoop:;

    }

    for(auto& node : nodes)
    {
        node.apply_tethers(fTetherBaseDistance * fZoomLevel * pow(1.0f + fTetherEdgeDistanceModifier, node.numEdges()),
                           fTetherElasticCoefficient * pow(1.0f + fTetherEdgeElasticModifier, node.numEdges()),
                           fTetherRangeThreshold * pow(1.0f + fTetherEdgeDistanceModifier, node.numEdges()*2));
    }
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

    // Draw edges before nodes
    for(auto& node : nodes)
    {
        node.drawEdges(target);
    }

    // Second pass for nodes
    for(auto& node : nodes)
    {
        node.draw(target);
    }

    for(auto& text : displayText)
    {
        target->draw(text);
    }

    for(auto& node : nodes)
    {
        node.drawInterface(target);
    }

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

void CVNetworkPanel::addPanelElement(CVElement* element,
                                    const string& newTag,
                                    const string& newType,
                                    const float& weight,
                                    const unsigned int& label_orientation)
{

    CVBasicViewPanel::addPanelElement(element, newTag);

    sf::Color nodeTextColor;
    try
    {
        nodeTextColor = getTextLegendColor(newType);
    }catch(...)
    {
        nodeTextColor = getDefaultNodeTextColor();
    }

    element->setSize(element->getSize() * fZoomLevel);
    element->setSpriteScale(fZoomLevel);

    nodes.emplace_back(element, newType, weight, appFont(textInfo.font), textInfo.fontSize, label_orientation, nodeTextColor);
    element->setHighlightColor(selectionColor);

    updateNodeConnections(nodes.back());
}

void CVNetworkPanel::removePanelElement(CVElement* element)
{
    for(size_t i = 0; i < nodes.size(); ++i)
    {
        if(nodes[i].getElement() == element)
        {
            CVBasicViewPanel::removePanelElement(nodes[i].getElement());
            nodes.erase(nodes.begin() + i);
        }
    }
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
            }
        }
    }

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
            }
        }
    }

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
            }
        }
    }

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
            }
        }
    }

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

sf::Vector2f CVNetworkPanel::convert_to_local(const sf::Vector2f& screen_coords) const noexcept
{

    return (screen_coords - panOffset) * fZoomLevel;

}

sf::Vector2f CVNetworkPanel::convert_to_screen(const sf::Vector2f& local_coords) const noexcept
{

    return local_coords/fZoomLevel - panOffset;

}

}
