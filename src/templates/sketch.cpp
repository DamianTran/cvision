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
// warranties.  By using CVision, you ackowledge and agree
// to this disclaimer.  Use of CVision in Users's programs
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

#include "cvision/templates/sketch.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"


#include "EZC/algorithm.hpp"

using namespace EZC;

namespace cvis{

CVBrush::CVBrush(const sf::Color& color,
            const float& radius,
            const float& rounding,
            const float& softening,
            const CVBrushType& brushType,
            const CVBrushEffect& brushEffect):
                rounding(rounding),
                softening(softening),
                primaryColor(color),
                secondaryColor(sf::Color::White),
                position(0.0f,0.0f),
                dimensions(1.0f,1.0f),
                drawScale(1.0f, 1.0f),
                icon(nullptr),
                bVisible(false),
                effect(brushEffect),
                type(brushType){ }

void CVBrush::setIcon(const sf::Texture* newIcon){
    icon = newIcon;
}

void CVBrush::clearIcon(){
    icon = nullptr;
}

void CVBrush::drawToImage(sf::Image& image, const sf::Vector2i& coords){
    switch(type){
        case CVBrush::square:{
            for(int x = coords.x - dimensions.x; x < coords.x + dimensions.x; ++x){
                if(x >= image.getSize().x) continue;
                for(int y = coords.y - dimensions.y; y < coords.y + dimensions.y; ++y){
                    if(y >= image.getSize().y) continue;
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) image.setPixel(x, y, primaryColor);
                    else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) image.setPixel(x, y, secondaryColor);
                }
            }
            break;
        }
        case CVBrush::rounded:{
            for(int x = coords.x - dimensions.x; x < coords.x + dimensions.x; ++x){
                if(x >= image.getSize().x) continue;
                for(int y = coords.y - dimensions.y; y < coords.y + dimensions.y; ++y){
                    if(y >= image.getSize().y) continue;

                    if(getDistance(sf::Vector2f(coords), sf::Vector2f(x,y)) > dimensions.x){
                        continue;
                    }

                    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) image.setPixel(x, y, primaryColor);
                    else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) image.setPixel(x, y, secondaryColor);
                }
            }
            break;
        }
        case CVBrush::point:{
            switch(effect){
                case CVBrush::CVBrushEffect::connective:{
                    for(int x = 0; x < image.getSize().x; ++x){
                        for(int y = 0; y < image.getSize().y; ++y){
                            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) image.setPixel(x, y, primaryColor);
                            else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) image.setPixel(x, y, secondaryColor);
                        }
                    }
                    break;
                }
                default:{
                    break;
                }
            }
            break;
        }
        default:{
            break;
        }
    }
}

void CVBrush::drawToImage(sf::Image& image, const std::vector<sf::Vector2i>& coords){
    for(auto& coord : coords){
        drawToImage(image, coord);
    }
}

void CVBrush::drawToImage(sf::Image& image, const vMatrix<int>& coords){
    for(auto& coord : coords){
        drawToImage(image,
                    sf::Vector2i(coord[0],coord[1]));
    }
}

const sf::Vector2f& CVBrush::getPosition() const{
    return position;
}

const sf::Vector2f& CVBrush::getDimensions() const{
    return dimensions;
}

const sf::Vector2f& CVBrush::getDrawScale() const{
    return drawScale;
}

void CVBrush::setPosition(const sf::Vector2f& position){
    this->position = position;
}

void CVBrush::move(const sf::Vector2f& distance){
    this->position.x += distance.x;
    this->position.y += distance.y;
}

void CVBrush::setPrimaryColor(const sf::Color& newColor){
    primaryColor = newColor;
}

void CVBrush::setSecondaryColor(const sf::Color& newColor){
    secondaryColor = newColor;
}

void CVBrush::setVisible(const bool& status){
    bVisible = status;
}

void CVBrush::scale(const float& ratio){
    dimensions.x *= ratio;
    dimensions.y *= ratio;
}

void CVBrush::scale(const sf::Vector2f& ratios){
    dimensions.x *= ratios.x;
    dimensions.y *= ratios.y;
}

void CVBrush::setEffect(const CVBrushEffect& newEffect){
    effect = newEffect;
}

void CVBrush::setType(const CVBrushType& newType){
    type = newType;
}

bool CVBrush::draw(sf::RenderTarget* target){
    if(!bVisible || (target == nullptr)) return false;

    if(icon != nullptr){
        sf::Sprite cursor(*icon);
        cursor.setScale(dimensions*2/icon->getSize());
        cursor.setPosition(position);
        target->draw(cursor);
    }
    else{
        sf::Shape* cursor = nullptr;

        switch(type){
            case CVBrush::rounded:{
                cursor = new sf::CircleShape(dimensions.x);
                break;
            }
            default:{ // Square
                cursor = new sf::RectangleShape(sf::Vector2f(dimensions.x*2,
                                                             dimensions.y*2));
                break;
            }
        }

        cursor->setOutlineColor(sf::Color(60,60,60,200));
        cursor->setFillColor(sf::Color(255,255,255,60));
        cursor->setOutlineThickness(1.0);

        cursor->setOrigin(dimensions);
        cursor->setPosition(position);
        cursor->setScale(drawScale);

        target->draw(*cursor);

        delete(cursor);
    }

    return true;
}


CVSketchPanel::CVSketchPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         selectionLayer(this, size*0.1f, size*0.8f, sf::Color::Transparent, "Selection"),
                         highlightLayer(this, size*0.1f, size*0.8f, sf::Color::Transparent, "Highlight"),
                         brushes(18),
                         brushCoords("", *appFont(textInfo.font), textInfo.fontSize*1.2f*View->getViewScale()),
                         fg_color(sf::Color::Black),
                         bg_color(sf::Color::White),
                         selected_layer(0),
                         selected_brush(0),
                         toolset(nullptr),
                         bCanDraw(true),
                         bBrushCoords(true){

    setDrawClipping(true);

    brushCoords.setFillColor(sf::Color(17,70,180,200));
    layers.emplace_back(this, size*0.1f, size*0.8f, sf::Color::White, "Background");

    toolset = new CVButtonPanel(parentView, "toolset", sf::Color(30,30,30,120),
                                sf::Vector2f(size.x*0.08f, size.y*0.8f), false,
                                sf::Vector2f(size.x*0.01f, size.y*0.1f));

    toolset->setGrid(3,18);
    toolset->setOuterPadding(6.0f);

    toolset->addButton(sf::Vector2i(0,0), "pencil", "draw_logo");

    toolset->addButton(sf::Vector2i(1,0), "paint", "paint_icon");
    brushes[1].type = CVBrush::rounded;
    brushes[1].setRadius(10.0f);

    toolset->addButton(sf::Vector2i(2,0), "eraser", "eraser_icon");
    brushes[2].type = CVBrush::rounded;
    brushes[2].setPrimaryColor(sf::Color::White);
    brushes[2].setSecondaryColor(sf::Color::Transparent);
    brushes[2].setRadius(10.0f);

    toolset->addButton(sf::Vector2i(0,1), "bucket", "paintbucket_icon");
    brushes[3].setType(CVBrush::point);
    brushes[3].setPrimaryColor(sf::Color::White);
    brushes[3].setSecondaryColor(sf::Color::Transparent);
    brushes[3].setEffect(CVBrush::CVBrushEffect::connective);
    brushes[3].setIcon(appTexture("paintbucket_cursor"));
    brushes[3].setRadius(48.0f*View->getViewScale());

    toolset->addButton(sf::Vector2i(1,1), "rect_select", "draw_logo");
    toolset->addButton(sf::Vector2i(2,1), "ellipse_select", "draw_logo");
    toolset->addButton(sf::Vector2i(0,2), "crop", "draw_logo");
    toolset->addButton(sf::Vector2i(1,2), "pencil", "draw_logo");
    toolset->addButton(sf::Vector2i(2,2), "pencil", "draw_logo");

    addPanelElement(toolset, "toolset");

}

void CVSketchPanel::clear(){
    layers.clear();
}

bool CVSketchPanel::canDraw() const{
    return bCanDraw;
}

void CVSketchPanel::lockDraw(){
    bCanDraw = false;
}

void CVSketchPanel::unlockDraw(){
    bCanDraw = true;
}

void CVSketchPanel::setDisplayCoords(const bool& status){
    bBrushCoords = status;
}

sf::Vector2i CVSketchPanel::viewToPixelCoords(const sf::Vector2f& position) const{
    if(layers.empty()) return sf::Vector2i(0,0);
    return sf::Vector2i((position - layers.front().getPosition()) /
                            (layers.front().getSize()*layers.front().getScale()) * layers.front().getImageSize());
}

void CVSketchPanel::setForegroundColor(const sf::Color& newColor){
    fg_color = newColor;
}

void CVSketchPanel::setBackgroundColor(const sf::Color& newColor){
    bg_color = newColor;
}

void CVSketchPanel::select_layer(const unsigned int& layer){
    if(layer < layers.size()){
        selected_layer = layer;
    }
    selected_layer = layers.size();
}

void CVSketchPanel::select_layer(const std::string& tag){
    for(size_t i = 0; i < layers.size(); ++i){
        if(layers[i].getTag() == tag){
            selected_layer = i;
        }
    }
    selected_layer = layers.size();
}

void CVSketchPanel::draw_to_layer(const unsigned int& index,
                                  sf::Vector2i coords){
    if(index >= layers.size() ||
       !layers[index].isVisible() ||
       (selected_brush >= brushes.size())) return;

    drawCanvas.create(layers[selected_layer].getSize().x, layers[selected_layer].getSize().y);
    layers[selected_layer].getTextureImage(drawCanvas);
    brushes[selected_brush].drawToImage(drawCanvas, coords);
    layers[index].loadFromImage(drawCanvas);
}

void CVSketchPanel::draw_to_layer(const unsigned int& index,
                                  vMatrix<int> coords){
    if(index >= layers.size() ||
       !layers[index].isVisible() ||
       (selected_brush >= brushes.size()) ||
       coords.empty()) return;

    drawCanvas.create(layers[selected_layer].getSize().x, layers[selected_layer].getSize().y);
    layers[selected_layer].getTextureImage(drawCanvas);
    brushes[selected_brush].drawToImage(drawCanvas, coords);
    layers[index].loadFromImage(drawCanvas);
}

void CVSketchPanel::flattenToTexture(sf::Texture& out_tex){
    View->captureLock.lock();

    View->textureBuffer.create(layers.front().getImageSize().x,
                               layers.front().getImageSize().y);
    View->textureBuffer.setView(sf::View(sf::FloatRect(0.0f,0.0f,
                                                       layers.front().getImageSize().x,
                                                       layers.front().getImageSize().y)));
    View->textureBuffer.clear(sf::Color::Transparent);

    std::vector<CVSketchLayer> tmp_layers = layers;
    sf::Vector2f moveDist = layers.front().getPosition() * -1.0f;

    for(auto& layer : tmp_layers){
        layer.move(moveDist);
        layer.setScale(sf::Vector2f(1.0f,1.0f));
        View->textureBuffer.draw(layer);
    }

    View->textureBuffer.display();
    out_tex = View->textureBuffer.getTexture();

    View->captureLock.unlock();
}

void CVSketchPanel::flattenToImage(sf::Image& out_img){
    sf::Texture tmp;
    flattenToTexture(tmp);
    out_img = tmp.copyToImage();
}

void CVSketchPanel::highlight_pixels(const sf::Rect<int>& boundaries, const sf::Color& color,
                                     const bool& clear){
    if(layers.empty()) return;

    highlightLayer.getTextureImage(drawCanvas);
    if(clear) setImageColor(drawCanvas, sf::Color::Transparent);

    size_t xL = drawCanvas.getSize().x,
            yL = drawCanvas.getSize().y,
            x;
    sf::Color pixColor;

    for(size_t y = boundaries.top; y < boundaries.top + boundaries.height; ++y){
        if((y < 0) || (y >= yL)) continue;
        for(x = boundaries.left; x < boundaries.left + boundaries.width; ++x){
            if((x < 0) || (x >= xL)) continue;
            drawCanvas.setPixel(x, y, color);
        }
    }

    highlightLayer.loadFromImage(drawCanvas);
}

void CVSketchPanel::clear_highlight(){
    drawCanvas.create(layers.front().getSize().x,
                      layers.front().getSize().y,
                      sf::Color::Transparent);
    highlightLayer.loadFromImage(drawCanvas);
}

CVSketchPanel::CVSketchLayer::CVSketchLayer(CVSketchPanel* host, const sf::Vector2f& position,
                                            const sf::Vector2f& size,
                                            const sf::Color& background,
                                            const std::string& tag):
    sf::RectangleShape(size),
    tag(tag),
    host(host),
    bVisible(true){

        setPosition(position);
        host->drawCanvas.create((unsigned int)size.x, (unsigned int)size.y, background);
        cachedTexture.loadFromImage(host->drawCanvas);
        setTexture(&cachedTexture);

}

sf::Vector2u CVSketchPanel::CVSketchLayer::getImageSize() const{
    return cachedTexture.getSize();
}

void CVSketchPanel::CVSketchLayer::loadFromImage(const sf::Image& img){
    cachedTexture.loadFromImage(img);
}

void CVSketchPanel::CVSketchLayer::getTextureImage(sf::Image& output) const{
    output = cachedTexture.copyToImage();
}

void CVSketchPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVSketchPanel::setPosition(const float& x, const float& y){
    setPosition(sf::Vector2f(x,y));
}

void CVSketchPanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
    for(auto& layer : layers){
        layer.move(distance);
    }
    selectionLayer.move(distance);
    highlightLayer.move(distance);
}

void CVSketchPanel::move(const float& x, const float& y){
    move(sf::Vector2f(x, y));
}

bool CVSketchPanel::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    for(auto& layer : layers){
        target->draw(layer);
    }
    target->draw(selectionLayer);
    target->draw(highlightLayer);

    brushes[selected_brush].draw(target);
    target->draw(brushCoords);

    for(auto& panel : viewPanelElements){
        panel->draw(target);
    }

    CV_DRAW_CLIP_END

    return true;
}

bool CVSketchPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    // Check toolset for input

    if(toolset->checkInteraction()){
        std::string buttonPressed = toolset->getInteraction();
        if(buttonPressed == "pencil"){
            selected_brush = 0;
            brushes[selected_brush].setPrimaryColor(fg_color);
            brushes[selected_brush].setSecondaryColor(bg_color);
        }
        else if(buttonPressed == "paint"){
            selected_brush = 1;
            brushes[selected_brush].setPrimaryColor(fg_color);
            brushes[selected_brush].setSecondaryColor(bg_color);
        }
        else if(buttonPressed == "eraser"){
            selected_brush = 2;
        }
        else if(buttonPressed == "bucket"){
            selected_brush = 3;
            brushes[selected_brush].setPrimaryColor(fg_color);
            brushes[selected_brush].setSecondaryColor(bg_color);
        }
    }

    if(selected_brush < brushes.size()){
        brushes[selected_brush].setPosition(mousePos);
    }

    if(bounds.contains(mousePos) &&
       event.focusFree() && !layers.empty()){

        if(layers.front().getGlobalBounds().contains(mousePos)){
            brushes[selected_brush].setVisible(true);
            View->viewPort->setMouseCursorVisible(false);
        }
        else{
            brushes[selected_brush].setVisible(false);
            View->viewPort->setMouseCursorVisible(true);
        }

        if(((event.LMBhold && bounds.contains(event.LMBpressPosition)) ||
           (event.RMBhold && bounds.contains(event.RMBpressPosition))) &&
           event.captureMouse()){
            if(shiftPressed()){

                // Pan

                for(auto& layer : layers){
                    event.mouse_capture(layer);
                }
                event.mouse_capture(selectionLayer);
                event.mouse_capture(highlightLayer);

            }
            else if(bCanDraw){

                // Draw

                if(mousePos == event.lastFrameMousePosition){
                    draw_to_layer(selected_layer, sf::Vector2i((mousePos - layers[selected_layer].getPosition())/
                                                               layers[selected_layer].getScale()));
                }
                else{
                    vMatrix<int> coords;
                    sf::Vector2i start((event.lastFrameMousePosition - layers[selected_layer].getPosition()) /
                                       layers[selected_layer].getScale()),
                                    end((mousePos - layers[selected_layer].getPosition()) /
                                        layers[selected_layer].getScale());
                    _2Dpath(coords, std::vector<int>({ start.x, start.y }),
                                std::vector<int>({ end.x, end.y }));
                    draw_to_layer(selected_layer, coords);
                }
            }
        }

        // Scale

        if(event.mouseWheelDelta.y != 0.0f){
            float scale_inc = 1.0f + event.mouseWheelDelta.y*0.1f;
            if(ctrlPressed()){
                brushes[selected_brush].scale(scale_inc);
            }
            else{
                for(auto& layer : layers){
                    layer.setPosition(radial_position(mousePos,
                                                      getDistance(layer.getPosition(), mousePos) *
                                                      scale_inc, get_angle(mousePos, layer.getPosition())));
                    layer.setScale(layer.getScale()*scale_inc);
                }
                for(auto& brush : brushes){
                    brush.setDrawScale(brush.getDrawScale()*scale_inc);
                }

                // Permanent layers
                selectionLayer.setPosition(radial_position(mousePos,
                                                      getDistance(selectionLayer.getPosition(), mousePos) *
                                                      scale_inc, get_angle(mousePos, selectionLayer.getPosition())));
                selectionLayer.setScale(selectionLayer.getScale()*scale_inc);
                highlightLayer.setPosition(radial_position(mousePos,
                                                      getDistance(highlightLayer.getPosition(), mousePos) *
                                                      scale_inc, get_angle(mousePos, highlightLayer.getPosition())));
                highlightLayer.setScale(highlightLayer.getScale()*scale_inc);
            }
        }

        std::stringstream coordText;
        if(bBrushCoords && !layers.empty() &&
           layers.front().getGlobalBounds().contains(mousePos)){
            sf::Vector2i imgCoords = viewToPixelCoords(mousePos);
            coordText << "[" << imgCoords.x << ',' << imgCoords.y << "]";
        }
        brushCoords.setString(coordText.str());
        brushCoords.setPosition(brushes[selected_brush].getPosition() +
                                brushes[selected_brush].getDimensions() +
                                sf::Vector2f(12.0f, 12.0f)*brushes[selected_brush].getDrawScale());

    }
    else if(layers.empty() ||
            !layers.front().getGlobalBounds().contains(mousePos) ||
            event.mouseCaptured){
        brushes[selected_brush].setVisible(false);
        View->viewPort->setMouseCursorVisible(true);
    }

    return true;
}

}
