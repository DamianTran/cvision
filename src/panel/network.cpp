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
                             const float& weight,
                             const sf::Font* font,
                             const unsigned int& baseCharacterSize,
                             const unsigned int& alignment,
                             const sf::Color& textFillColor):
    element(element),
    displayText(element->tag(), *font, baseCharacterSize),
    weight(weight),
    textPadding(24.0f),
    lastElementPosition(element->getPosition()),
    textDisplayOffset(0.0f, 0.0f),
    bDisplayTextOnHover(false),
    targetTextAlpha(textFillColor.a),
    textFadeRate(2)
{
    align_text(alignment);
    displayText.setFillColor(textFillColor);
}

void CVNetworkNode::connect_out(CVNetworkNode& node, const string& type)
{
    for(auto& link : connected_to)
    {
        if(&link.getNode() == &node)
        {
            return;
        }
    }

    connected_to.emplace_back(*this, node, type);

    for(auto& link : node.connected_from)
    {
        if(&link.getNode() == &node)
        {
            return;
        }
    }

    node.connected_from.emplace_back(*this, node, type);
}

void CVNetworkNode::connect_in(CVNetworkNode& node, const string& type)
{

    for(auto& link : connected_from)
    {
        if(&link.getNode() == &node)
        {
            return;
        }
    }

    connected_from.emplace_back(node, *this, type);

    for(auto& link : node.connected_to)
    {
        if(&link.getNode() == &node)
        {
            return;
        }
    }

    node.connected_to.emplace_back(node, *this, type);
}

void CVNetworkNode::connect_with(CVNetworkNode& node, const string& type)
{
    connect_out(node, type);
    connect_in(node, type);
}

void CVNetworkNode::remove_connections(CVNetworkNode& other)
{

    cout << "Removing...\n";

    for(size_t i = 0; i < connected_to.size();)
    {
        if(&connected_to[i].getNode() == &other)
        {
            connected_to.erase(connected_to.begin() + i);
        }
        else ++i;
    }

    cout << "Check...\n";

    for(size_t i = 0; i < connected_from.size();)
    {
        if(&connected_from[i].getOrigin() == &other)
        {
            cout << "Erasing...\n";
            connected_from.erase(connected_from.begin() + i);
            cout << "Success\n";
        }
        else ++i;
    }

    cout << "Check...\n";

    for(size_t i = 0; i < other.connected_to.size();)
    {
        if(&other.connected_to[i].getNode() == this)
        {
            other.connected_to.erase(other.connected_to.begin() + i);
        }
        else ++i;
    }

    cout << "Check...\n";

    for(size_t i = 0; i < other.connected_from.size();)
    {
        if(&other.connected_from[i].getOrigin() == this)
        {
            other.connected_from.erase(other.connected_from.begin() + i);
        }
        else ++i;
    }

    cout << "Done\n";

}

void CVNetworkNode::disconnect()
{
    while(!connected_to.empty())
    {
        connected_to.front().getNode().remove_connections(*this);
        connected_to.erase(connected_to.begin());
    }
    while(!connected_from.empty())
    {
        connected_from.front().getNode().remove_connections(*this);
        connected_from.erase(connected_from.begin());
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

void CVNetworkNode::update(const sf::Vector2f& mousePos)
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

void CVNetworkNode::draw(sf::RenderTarget* target)
{
    if(element->draw(target))
    {
        target->draw(displayText);
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

            displayText.setOrigin(sf::Vector2f(textBounds.width, textBounds.height - getTextCenterOffsetY(displayText)));
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

            displayText.setOrigin(sf::Vector2f(textBounds.width/2, textBounds.height - getTextCenterOffsetY(displayText)));
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

            displayText.setOrigin(sf::Vector2f(0.0f, textBounds.height - getTextCenterOffsetY(displayText)));
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

    throw invalid_argument("CVNetworkNode: requested connection does not exist");
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

    throw invalid_argument("CVNetworkNode: requested connection does not exist");
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
                                 fontWeightScale(1.0f),
                                 defaultNodeOutlineThickness(2.0f),
                                 defaultNodeRounding(NAN),
                                 physicsPushStrength(2.0f),
                                 physicsPullStrength(2.0f),
                                 tetherDistanceScale(4.0f),
                                 selectionColor(sf::Color::Yellow),
                                 defaultNodeFillColor(sf::Color::White),
                                 defaultNodeOutlineColor(sf::Color::Black),
                                 defaultNodeTextAlignment(ALIGN_CENTER_MIDLINE)
{
    this->textInfo = textInfo;
    setOutlineThickness(outlineThickness);
    setOutlineColor(outlineColor);

    selectionCordon.setOutlineThickness(1.0f);
    selectionCordon.setFillColor(sf::Color(128,128,128,60));
    selectionCordon.setOutlineColor(sf::Color(128,128,128,200));

    setDrawClipping(true);

}

bool CVNetworkPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    for(auto& node : nodes)
    {
        node.update(mousePos);
    }

//    if(event.LMBhold &&
//       bounds.contains(event.LMBpressPosition))
//    {
//        for(auto& node : nodes)
//        {
//            if(event.isCaptured(node.getElement()))
//            {
//                selectionCordon.setSize(sf::Vector2f(0.0f, 0.0f));
//                goto noCordon;
//            }
//        }
//
//        selectionCordon.setPosition(event.LMBpressPosition);
//        selectionCordon.setSize(mousePos - event.LMBpressPosition);
//
//        noCordon:;
//    }
//    else if(event.LMBreleased && event.LMBreleaseFrames == 1)
//    {
//        if((event.LMBholdTime > 0.2f) && selectionCordon.getSize().x)
//        {
//            for(auto& node : nodes)
//            {
//                if(contains(node.getBounds(), selectionCordon.getGlobalBounds()))
//                {
//                    node.setSelected(true);
//                }
//                else if(!ctrlPressed())
//                {
//                    node.setSelected(false);
//                }
//            }
//
//            selectionCordon.setSize(sf::Vector2f(0.0f, 0.0f));
//        }
//        else
//        {
//            for(auto& node : nodes)
//            {
//                if(node.getBounds().contains(mousePos))
//                {
//                    node.setSelected(true);
//                }
//                else if(!ctrlPressed())
//                {
//                    node.setSelected(false);
//                }
//            }
//
//            selectionCordon.setSize(sf::Vector2f(0.0f, 0.0f));
//        }
//    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
    {
//        for(size_t i = 0; i < nodes.size();)
//        {
//            if(nodes[i].isSelected())
//            {
//                removePanelElement(nodes[i].getElement());
//            }
//            else ++i;
//        }

        while(numPanels())
        {
            removePanelElement(nodes.front().getElement());
        }

    }

    // Handle drop data

    // Handle physics

    if(physicsPushStrength)
    {
        updatePhysics(event);
    }

    // Handle layout requests

    return true;

}

void CVNetworkPanel::updatePhysics(CVEvent& event)
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

    std::vector<CVNetworkNode*> mobile_nodes;

    for(auto& node : nodes)
    {
        if(node.isVisible() &&
           !event.isCaptured(node.getElement()))
        {
            mobile_nodes.emplace_back(&node);
        }
    }

    for(size_t i = 0, j, L = nodes.size(); i + 1 < L; ++i){

        for(j = i + 1; j < L; ++j){

            shape1Bounds = nodes[i].getBounds();
            shape2Bounds = nodes[j].getBounds();

            if(shape1Bounds.intersects(shape2Bounds))
            {

                sizeRatio = (shape1Bounds.width*shape1Bounds.height)/
                (shape1Bounds.width*shape1Bounds.height + shape2Bounds.height*shape2Bounds.width);

                max_dist = sqrt(pow(shape1Bounds.width + shape2Bounds.width, 2) +
                                pow(shape1Bounds.height + shape2Bounds.height, 2));

                shape1ctr = getBoundCenter(shape1Bounds);
                shape2ctr = getBoundCenter(shape2Bounds);

                lin_dist = getDistance(shape1ctr, shape2ctr);

                if(!lin_dist)
                {
                    lin_dist = max_dist / 100;
                    angle = rand(0.0, double(2*PI));
                }
                else
                {
                    angle = get_angle(shape1ctr, shape2ctr);
                }

                if(anyEqual(&nodes[j], mobile_nodes))
                {
                    moveDist = sf::Vector2f(components((1.0 - lin_dist/max_dist) * physicsPushStrength * sizeRatio, angle));
                    nodes[j].move(moveDist);
                }
                if(anyEqual(&nodes[i], mobile_nodes))
                {
                    moveDist = sf::Vector2f(components((1.0 - lin_dist/max_dist) * physicsPushStrength * (1.0 - sizeRatio), angle));
                    nodes[i].move(-moveDist);
                }

            }
        }

        nextLoop:;

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

    target->draw(selectionCordon);

    for(auto& item : mask)
    {
        target->draw(item);
    }

    CV_DRAW_CLIP_END

    return true;sf::Color baseColor;
    sf::Color selectionColor;
}

void CVNetworkPanel::addPanelElement(CVElement* element,
                                    const string& newTag,
                                    const float& weight,
                                    const unsigned int& label_orientation)
{

    CVBasicViewPanel::addPanelElement(element, newTag);
    nodes.emplace_back(element, weight, appFont(textInfo.font), textInfo.fontSize, label_orientation, textInfo.textColor);
    element->setHighlightColor(selectionColor);
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
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size)
{

    CVButton* newNode = nullptr;

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
                                           textInfo.textColor),
                                 "", defaultNodeFillColor, defaultNodeOutlineColor,
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
                                           textInfo.textColor),
                                 "", defaultNodeFillColor, defaultNodeOutlineColor,
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

    addPanelElement(newNode, tag, weight);

    return newNode;

}

CVButton* CVNetworkPanel::addNode(const string& tag,
                                  const float& weight,
                                  const sf::Vector2f& position,
                                  const sf::Vector2f& size,
                                  const sf::Color& fillColor,
                                  const sf::Color& outlineColor,
                                  const float& outlineThickness,
                                  const float& rounding)
{
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

    addPanelElement(newNode, tag, weight);

    return newNode;
}

CVButton* CVNetworkPanel::addImageNode(const string& tag,
                                       const string& texture,
                                       const float& weight,
                                       const sf::Vector2f& position,
                                       const sf::Vector2f& size)
{

    if(!appTexture(texture)) return nullptr;

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

    newNode->setSpriteColor(defaultNodeFillColor);
    newNode->setDraggableStatus(true);

    addPanelElement(newNode, tag, weight, ALIGN_CENTER_BOTTOM);

    return newNode;

}

CVButton* CVNetworkPanel::addImageNode(const string& tag,
                                       const string& texture,
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

    addPanelElement(newNode, tag, weight, ALIGN_CENTER_BOTTOM);

    return newNode;
}

void CVNetworkPanel::connectByTag(const std::string& source,
                                  const std::string& target,
                                  const bool& bidirectional)
{

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpString(nodes[i].getTag(), source, CMP_STR_CASE_INSENSITIVE) &&
               cmpString(nodes[j].getTag(), target, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_out(nodes[j]);
                }
            }
            else if(cmpString(nodes[i].getTag(), target, CMP_STR_CASE_INSENSITIVE) &&
                    cmpString(nodes[j].getTag(), source, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_in(nodes[j]);
                }
            }
        }
    }

}

void CVNetworkPanel::connectByTags(const string& source,
                                   const vector<string>& targets,
                                   const bool& bidirectional)
{

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if((nodes[i].getTag() == source) && anyEqual(nodes[j].getTag(), targets))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_out(nodes[j]);
                }
            }
            else if(anyEqual(nodes[i].getTag(), targets) && (nodes[j].getTag() == source))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_in(nodes[j]);
                }
            }
        }
    }

}

void CVNetworkPanel::connectByTags(const vector<string>& sources,
                                   const string& target,
                                   const bool& bidirectional)
{

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpStringToList(nodes[i].getTag(), sources, CMP_STR_CASE_INSENSITIVE) &&
               cmpString(nodes[j].getTag(), target, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_out(nodes[j]);
                }
            }
            else if(cmpString(nodes[i].getTag(), target, CMP_STR_CASE_INSENSITIVE),
                    cmpStringToList(nodes[j].getTag(), sources, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_in(nodes[j]);
                }
            }
        }
    }

}

void CVNetworkPanel::connectByTags(const vector<string>& sources,
                                   const vector<string>& targets,
                                   const bool& bidirectional)
{

    for(size_t i = 0, j, L = nodes.size(); i < L - 1; ++i)
    {
        for(j = i + 1; j < L; ++j)
        {
            if(cmpStringToList(nodes[i].getTag(), sources, CMP_STR_CASE_INSENSITIVE) &&
               cmpStringToList(nodes[j].getTag(), targets, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_out(nodes[j]);
                }
            }
            else if(cmpStringToList(nodes[i].getTag(), targets, CMP_STR_CASE_INSENSITIVE) &&
                    cmpStringToList(nodes[j].getTag(), sources, CMP_STR_CASE_INSENSITIVE))
            {
                if(bidirectional)
                {
                    nodes[i].connect_with(nodes[j]);
                }
                else
                {
                    nodes[i].connect_in(nodes[j]);
                }
            }
        }
    }

}

void CVNetworkPanel::setNodeFillColor(const sf::Color& newColor) noexcept
{
    defaultNodeFillColor = newColor;
}

}
