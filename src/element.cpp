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

#include "cvision/element.hpp"
#include "cvision/algorithm.hpp"
#include "cvision/button.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include <hyper/toolkit/string.hpp>

using namespace hyperC;
using namespace std;

namespace cvis
{

#define CVELEMENT_INIT \
        mouseHovering(false),\
        clickHeld(false),\
        highlighted(false),\
        bHasFocus(false),\
        bFollowMouseX(false),\
        bFollowMouseY(false),\
        bFade(false),\
        bMove(false),\
        bSpriteOnly(false),\
        bExpand(false),\
        bStop(true),\
        bBounce(false),\
        bForceOnScreen(false),\
        bHasShadow(false),\
        bDragAndDrop(false),\
        bClipBounds(false),\
        bDelete(false),\
        bTriggered(false),\
        bDropShadow(false),\
        bStatic(false),\
        bNoInteract(false),\
        closeButton(nullptr),\
        highlightColor(sf::Color::Transparent),\
        origin(0.0f,0.0f),\
        destination(NAN, NAN),\
        velocity(0.0f,0.0f),\
        acceleration(0.0f,0.0f),\
        fMoveAngle(0.0f),\
        fElasticity(1.0f),\
        fFriction(0.0f),\
        fSpriteScale(1.0f),\
        stateNum(0),\
        numStates(1),\
        state(0b0),\
        targetAlpha(255),\
        fadeLayers(CV_LAYER_ALL),\
        fadeRate(0),\
        fTruePos(0.0f,0.0f),\
        iDrawPos(0,0),\
        fTrueSize(0.0f,0.0f),\
        iDrawSize(0,0),

CVElement::CVElement():
    visible(false),
    canHighlight(false),
    canClick(false),
    canDrag(false),
    active(true),
    CVELEMENT_INIT
    View(nullptr),
    drawTarget(nullptr) { }

CVElement::CVElement(CVView* View,
                     bool canHighlight,
                     bool canClick,
                     bool canDrag,
                     bool active):
    visible(true),
    canHighlight(canHighlight),
    canClick(canClick),
    canDrag(canDrag),
    active(active),
    CVELEMENT_INIT
    View(View),
    drawTarget(View->viewPort) {

    }

CVElement::~CVElement()
{
    View->releaseMouseCapture(*this);
    if(is_closable())
    {
        delete(closeButton);
        closeButton = nullptr;
    }
}

void CVElement::setClosable(const bool& status,
                            CVElement* button)
{

    if(status && !is_closable())
    {
        if(button)
        {
            closeButton = button;
        }
        else
        {
            float buttonSize = View->getWidth() < View->getHeight() ? View->getWidth()*0.02 : View->getHeight() * 0.02;
            closeButton = new CVButton(View, sf::Vector2f(getPosition().x + getBounds().width - buttonSize * 1.5,
                                                       getPosition().y + buttonSize * 1.5),
                                    buttonSize, buttonSize,"gen_x",1,0,true,nullptr);
            ((CVButton*)closeButton)->setSpriteColor(sf::Color(160,160,160,160));
        }
    }
    else if(!status && is_closable())
    {
        delete(closeButton);
        closeButton = nullptr;
    }

}

bool CVElement::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!View->viewPort || !View->viewPort->isOpen())
    {
        return false;
    }

    if(bMove && !bStatic)
    {
        if(!isnan(destination.x) && !isnan(destination.y))
        {
            if(getPosition() == destination)
            {
                velocity.x = 0.0f;
                velocity.y = 0.0f;
                acceleration.x = 0.0f;
                acceleration.y = 0.0f;
            }
            else if(getDistance(getPosition(), destination) - scalar(velocity)*View->getEventTrace().lastFrameTime < 0.0f)
            {
                velocity.x = 0.0f;
                velocity.y = 0.0f;
                acceleration.x = 0.0f;
                acceleration.y = 0.0f;
                setPosition(destination);
                destination.x = NAN;
                destination.y = NAN;
            }
            else push(get_angle(getPosition(), destination), scalar(velocity), fFriction);
        }

        if(bStop)
        {
            if(acceleration.x != 0.0f)
            {
                if((velocity.x != 0.0f) && ((abs(velocity.x) - abs(acceleration.x*View->getEventTrace().lastFrameTime)) < 0.0f))
                {
                    velocity.x = 0.0f;
                    acceleration.x = 0.0f;
                }
            }
            if(acceleration.y != 0.0f)
            {
                if((velocity.y != 0.0f) && ((abs(velocity.y) - abs(acceleration.y*View->getEventTrace().lastFrameTime)) < 0.0f))
                {
                    velocity.y = 0.0f;
                    acceleration.y = 0.0f;
                }
            }
        }

        if(bBounce)  // Bounce inward
        {
            if((bounds.left <= View->getBounds().left) && (velocity.x < 0.0f))
            {
                velocity.x = -velocity.x*fElasticity; // Horizontal bounce
                velocity.y *= fElasticity;
            }
            else if((bounds.left + bounds.width >= View->getBounds().left + View->getWidth()) && (velocity.x > 0.0f))
            {
                velocity.x = -velocity.x*fElasticity;
                velocity.y *= fElasticity;
            }


            if((bounds.top <= View->getBounds().top) && (velocity.y < 0.0f))
            {
                velocity.y = -velocity.y*fElasticity; // Vertical bounce
                velocity.x *= fElasticity;
            }
            else if((bounds.top + bounds.height >= View->getBounds().top + View->getBounds().height) && (velocity.y > 0.0f))
            {
                velocity.y = -velocity.y*fElasticity;
                velocity.x *= fElasticity;
            }
        }

        if(fFriction != 0.0f)  // Apply frictional force
        {
            if(velocity.x > 0.0f)
            {
                velocity.x -= abs(fFriction*cos(fMoveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.x < 0.0f) velocity.x = 0.0f;
            }
            else
            {
                velocity.x += abs(fFriction*cos(fMoveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.x > 0.0f) velocity.x = 0.0f;
            }

            if(velocity.y > 0.0f)
            {
                velocity.y -= abs(fFriction*sin(fMoveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.y < 0.0f) velocity.y = 0.0f;
            }
            else
            {
                velocity.y += abs(fFriction*sin(fMoveAngle)*View->getEventTrace().lastFrameTime);
                if(velocity.y > 0.0f) velocity.y = 0.0f;
            }
        }

        velocity += acceleration*View->getEventTrace().lastFrameTime;
        if((velocity.x == 0.0f) && (velocity.y == 0.0f)) bMove = false;
        else move(velocity*View->getEventTrace().lastFrameTime);
    }

    if(bNoInteract || !visible) return false;

    if(bFollowMouseX)
    {
        if(!isnan(event.currentMousePosition.x)) setPosition(event.currentMousePosition.x, getPosition().y);
    }

    if(bFollowMouseY)
    {
        if(!isnan(event.currentMousePosition.y))
        {
            setPosition(getPosition().x, event.currentMousePosition.y);
        }
    }

    if(bSpriteOnly)
    {
        updateBounds();
    }

    if(highlightable() && active)
    {
        if(highlighted || (bounds.contains(mousePos) && event.focusFree()))
        {
            setFocus(true);
            if(highlightColor != sf::Color::Transparent)
            {
                for(auto& spr : spriteList)
                {
                    spr.setColor(highlightColor);
                }
            }
            else
            {
                sf::Color spriteHighlightColor = baseSpriteColor();
                brighten(spriteHighlightColor, 45);
                for(auto& spr : spriteList)
                {
                    spr.setColor(spriteHighlightColor);
                }
            }
        }
        else
        {
            setFocus(false);
            for(auto& spr : spriteList)
            {
                spr.setColor(baseSpriteColor());
            }
        }
    }

    if(!bStatic && canDrag &&
        bounds.contains(event.LMBpressPosition) &&
        event.focusFree())
    {
        setFocus(true);
        if(event.LMBhold && event.captureMouse() &&
                (event.LMBholdTime < 0.3f) &&
                !event.isCaptured(*this))
        {
            event.mouse_capture(*this);
        }
    }

    if(bFade)
    {

        int adjusted_fr = ceil((float)fadeRate*60.0f/View->getFrameRate());

        for(auto& color : colorTheme)
        {
            if(color.a == targetAlpha) continue;
            if(targetAlpha > color.a)
            {
                if((int)color.a + adjusted_fr < (int)targetAlpha)
                {
                    color.a += adjusted_fr;
                }
                else color.a = targetAlpha;
            }
            else
            {
                if((int)color.a - adjusted_fr > (int)targetAlpha)
                {
                    color.a -= adjusted_fr;
                }
                else
                {
                    color.a = targetAlpha;
                }
            }
        }

        if((highlightColor != sf::Color::Transparent) &&
           (fadeLayers & CV_LAYER_HIGHLIGHT))
        {
            if(highlightColor.a != targetAlpha)
            {
                if(targetAlpha > highlightColor.a)
                {
                    if((int)highlightColor.a + adjusted_fr < (int)targetAlpha)
                    {
                        highlightColor.a += adjusted_fr;
                    }
                    else highlightColor.a = targetAlpha;
                }
                else
                {
                    if((int)highlightColor.a - adjusted_fr > (int)targetAlpha)
                    {
                        highlightColor.a -= adjusted_fr;
                    }
                    else
                    {
                        highlightColor.a = targetAlpha;
                    }
                }
            }
        }

        if(fadeLayers & CV_LAYER_SPRITES)
        {
            sf::Color tmp;
            for(auto& sprite : spriteList)
            {
                tmp = sprite.getColor();
                if(tmp.a == targetAlpha) continue;

                if(targetAlpha > tmp.a)
                {
                    if((int)tmp.a + adjusted_fr < (int)targetAlpha)
                    {
                        tmp.a += adjusted_fr;
                    }
                    else tmp.a = targetAlpha;
                }
                else
                {
                    if((int)tmp.a - adjusted_fr > (int)targetAlpha)
                    {
                        tmp.a -= adjusted_fr;
                    }
                    else
                    {
                        tmp.a = targetAlpha;
                    }
                }

                sprite.setColor(tmp);
            }

            if(targetAlpha > spriteColor.a)
            {
                if((int)spriteColor.a + adjusted_fr < (int)targetAlpha)
                {
                    spriteColor.a += adjusted_fr;
                }
                else spriteColor.a = targetAlpha;
            }
            else if(targetAlpha < spriteColor.a)
            {
                if((int)spriteColor.a - adjusted_fr > (int)targetAlpha)
                {
                    spriteColor.a -= adjusted_fr;
                }
                else spriteColor.a = targetAlpha;
            }
        }
    }

    if(bForceOnScreen)
    {
        if(bounds.left < View->getBounds().left)
        {
            move(sf::Vector2f(View->getBounds().left-bounds.left, 0.0f));
        }
        if(bounds.top < View->getBounds().top)
        {
            move(sf::Vector2f(0.0f, View->getBounds().top-bounds.top));
        }
        if(bounds.left + bounds.width > View->getBounds().left + View->getWidth())
        {
            move(sf::Vector2f(View->getBounds().left + View->getWidth() - bounds.left - bounds.width, 0.0f));
        }
        if(bounds.top + bounds.height > View->getBounds().top + View->getHeight())
        {
            move(sf::Vector2f(0.0f, View->getBounds().top + View->getHeight() - bounds.top - bounds.height));
        }
    }

    if(bDragAndDrop && active)
    {
        if(event.LMBhold && (event.LMBholdTime > 0.15f) &&
                bounds.contains(event.LMBpressPosition))
        {
            createShadow(180);
            View->shadow.move(mousePos - event.LMBpressPosition);
            event.mouse_capture(shadow);
            setFocus(true);
        }
        else if(bHasShadow && event.LMBreleased)
        {
            removeShadow();
            sendData(event);
            setFocus(false);
        }
    }

    if(event.focusFree() &&
            ((event.LMBhold && (event.LMBholdFrames == 1) && bounds.contains(event.LMBpressPosition)) ||
             ((event.RMBhold && (event.RMBholdFrames == 1) && bounds.contains(event.RMBpressPosition)))) &&
            !tag().empty())
    {

        View->mainApp->pushEvent(tag());

    }

    if(bClipBounds)
    {
        drawMask.setOrigin(origin);
        drawMask.setPosition(getPosition());
    }

    if(event.LMBhold && (event.LMBholdTime < 0.3f) && !bounds.contains(event.LMBpressPosition))
    {
        setFocus(false);
    }

    if(is_closable())
    {
        closeButton->setFocus(hasFocus());
        closeButton->update(event, mousePos);
        if(closeButton->getTrigger())
        {
            bDelete = true;
        }
    }

    return true;
}

bool CVElement::fadeComplete() const noexcept
{
    for(auto& sprite : spriteList)
    {
        if(sprite.getColor().a != targetAlpha)
        {
            return false;
        }
    }

    return true;
}

string CVElement::getDefaultFont() const
{
    if(mainApp())
    {
        return mainApp()->getDefaultFont();
    }
    return "";
}

const sf::Font* CVElement::appFont(const string& font) const
{
    if(mainApp())
    {
        const sf::Font* output = mainApp()->fonts[font];

        if(!output)
        {
            try
            {
                return mainApp()->fonts[mainApp()->font_panel.at(font)];
            }catch(...)
            {
                return nullptr;
            }
        }

        return output;
    }
    return nullptr;
}

const sf::Texture* CVElement::appTexture(const string& tag) const
{
    if(mainApp()) return mainApp()->bitmaps.taggedTexture(tag);
    return nullptr;
}

const sf::Image* CVElement::appImage(const string& tag) const
{
    if(mainApp()) return mainApp()->bitmaps.taggedImage(tag);
    return nullptr;
}

const sf::Color& CVElement::appColor(const string& tag) const
{
    if(mainApp())
    {
        try
        {
            return mainApp()->colors.at(tag);
        }catch(...)
        {
            throw invalid_argument("CVElement: no color to map to tag \"" + tag + "\"");
        }
    }
    throw invalid_argument("CVElement: No app available to derive color from tag");
}

void CVElement::setViewCursor(const sf::Texture* texture,
                              const sf::Vector2f& size,
                              const sf::Color& color,
                              const sf::Vector2f& origin)
{
    View->setCursor(texture, size, color, origin);
}

void CVElement::setViewCursor(const string& texture,
                              const sf::Vector2f& size,
                              const sf::Color& color,
                              const sf::Vector2f& origin)
{
    const sf::Texture* cursorTexture = mainApp()->bitmaps.taggedTexture(texture);
    if(cursorTexture)
    {
        setViewCursor(texture, size, color, origin);
    }
}

float CVElement::viewScale() const
{
    return View->getViewScale();
}

void CVElement::setSpriteScale(const float& newScale)
{
    if(fSpriteScale != newScale)
    {
        for(auto& spr : spriteList)
        {
            sf::Vector2f scale = spr.getScale();
            scale *= newScale/fSpriteScale;
            spr.setScale(scale);
        }
        fSpriteScale = newScale;
    }
}

void CVElement::setDropShadow(const bool& state,
                              const int& radius,
                              const uint8_t& alpha,
                              const sf::Vector2f& offset,
                              const sf::Vector2f& scale)
{

    if(!bDropShadow && state)
    {
        getTexture(dropShadowTexture);

        sf::Image tmp = dropShadowTexture.copyToImage();

        modulate(tmp, sf::Color::Black);
        expand_canvas(tmp, sf::Vector2i(radius*2, radius*2), sf::Color::Red);
//        gaussianBlur(tmp, radius);

        dropShadowTexture.loadFromImage(tmp);

        dropShadow.setTexture(dropShadowTexture);
        dropShadow.setOrigin(sf::Vector2f(-offset.x, -offset.y));
        dropShadow.setPosition(getPosition());
        dropShadow.setScale(getSize() / dropShadowTexture.getSize() * scale);
        dropShadow.setColor(sf::Color(255,255,255,alpha));

    }

    bDropShadow = state;
}

string CVElement::take_trigger(const string& tag)
{
    string output;

    for(size_t i = 0; i < incoming_triggers.size(); ++i)
    {
        if(incoming_triggers[i].find(tag) == 0)
        {
            output = incoming_triggers[i];
            output.erase(output.begin(),
                         output.begin() + tag.size());

            if(!output.empty())
            {
                output.erase(output.begin());
            }

            incoming_triggers.erase(incoming_triggers.begin() + i);

            if(output.empty())
            {
                output = "TRUE";
            }

            return output;
        }
    }

    return output;
}

void CVElement::sendTriggers()
{

    for(auto& info : trigger_targets)
    {

        if(!info.target) // If the target is not present initially, re-check and cache the pointer if found
        {
            info.target = getElementById(info.tag);
            if(!info.target)
            {
                cout << "Warning: Could not send trigger \"" << info.signal << "\" (Invalid target)\n";
                continue;
            }
        }

        if(info.state == UINT_MAX)
        {
            info.target->receive_trigger(info.signal);
        }
        else if(stateNum == info.state)
        {
            info.target->receive_trigger(info.signal);
        }
    }

}

void CVElement::sendTrigger(CVElement* target, const string& signal)
{
    if(target)
    {
        target->receive_trigger(signal);
    }
    else
    {
        cout << "Warning (CVision): invalid trigger target" << '\n';
    }
}

void CVElement::setDrawClipping(const bool& status)
{
    bClipBounds = status;
    if(bClipBounds)
    {
        drawMask.setTexture(clipTexture);
        drawMask.setOrigin(origin);
        drawMask.setPosition(getPosition());
    }
}

void CVElement::highlight(const bool& state)
{
    highlighted = state;
    if(highlighted)
    {
        for(auto& spr : spriteList)
        {
            spr.setColor(highlightColor);
        }
    }
    else
    {
        for(auto& spr : spriteList)
        {
            spr.setColor(baseFillColor());
        }
    }
}

void CVElement::setState(const uint8_t& newState)
{
    state = newState;
}

void CVElement::setFade(const uint8_t& alpha, const int& rate, const unsigned char& flags)
{
    bFade = true;
    targetAlpha = alpha;
    fadeRate = abs(rate);
    fadeLayers = flags;
}

bool CVElement::draw(sf::RenderTarget* target)
{
    if(target == nullptr) return false;
    if(!visible) return false;

    for(auto& sprite : spriteList)
    {
        target->draw(sprite);
    }

    if(bDropShadow)
    {
        target->draw(dropShadow);
    }

    return true;
}

void CVElement::getTexture(sf::Texture& outTex,
                           const sf::Color& canvas_color)
{

    View->captureLock.lock();

    View->textureBuffer.setActive(true);

    View->textureBuffer.create(bounds.width, bounds.height);
    View->textureBuffer.setView(sf::View(bounds));
    View->textureBuffer.clear(canvas_color);

    draw(&View->textureBuffer);

    View->textureBuffer.display();
    outTex = View->textureBuffer.getTexture();

    View->mainApp->setContextActive();

    View->captureLock.unlock();

}

bool CVElement::saveImage(const string& save_file)
{

    sf::Color canvas_color = sf::Color::White;
    if(save_file.find(".png") == save_file.size() - 4)
    {
        canvas_color = sf::Color::Transparent;
    }

    sf::Context context;
    sf::Texture newTexture;

    getTexture(newTexture, canvas_color);

    View->captureLock.lock();

    context.setActive(true);

    bool bStatus = newTexture.copyToImage().saveToFile(save_file);

    context.setActive(false);

    View->captureLock.unlock();

    return bStatus;

}

void CVElement::createShadow(const uint8_t& alpha, const float& drawScale)
{
    View->setShadow(*this, alpha, drawScale);
    bHasShadow = true;
}

void CVElement::removeShadow()
{
    bHasShadow = false;
    View->clearShadow();
}

void CVElement::updateBounds()
{
    if(spriteList.size() > 0)
    {
        bounds = spriteList.front().getGlobalBounds();
        sf::FloatRect panelBounds;
        for(size_t i = 0; i < spriteList.size(); ++i)
        {
            panelBounds = spriteList[i].getGlobalBounds();
            if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
            if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
            if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
                bounds.width = panelBounds.left + panelBounds.width - bounds.left;
            if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
                bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        }
    }
    else
    {
        bounds.width = 0.0f;
        bounds.height = 0.0f;
    }
}

void CVElement::move(const sf::Vector2f& distance)
{

    if(bStatic) return;

    fTruePos += distance;
    iDrawPos.x = std::round(fTruePos.x);
    iDrawPos.y = std::round(fTruePos.y);

    bounds.left = fTruePos.x;
    bounds.top = fTruePos.y;

}

void CVElement::setExpand(const bool& state)
{
    bExpand = state;
    updateBounds();
}

sf::Vector2f CVElement::getPosition() const
{
    return fTruePos + origin;
}

void CVElement::setSize(const sf::Vector2f& newSize)
{
    fTrueSize = newSize;
    iDrawSize.x = std::round(newSize.x);
    iDrawSize.y = std::round(newSize.y);

    bounds.width = newSize.x;
    bounds.height = newSize.y;
}

void CVElement::setElasticity(const float& newElasticity)
{
    if(newElasticity > 1.0f) fElasticity = 1.0f;
    else if(newElasticity < 0.0f) fElasticity = 0.0f;
    else fElasticity = newElasticity;
}

float CVElement::speed()
{
    return scalar(velocity);
}

void CVElement::stop()
{
    bMove = false;
    destination.x = NAN;
    destination.y = NAN;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    acceleration.x = 0.0f;
    acceleration.y = 0.0f;
}

void CVElement::move_to(const sf::Vector2f& position,
                        const float& velocity,
                        const float& drag)
{
    if(bStatic) return;

    bNoInteract = false;
    bMove = true;
    destination = position;
    fMoveAngle = get_angle(getPosition(), position);
    this->velocity = components(velocity, fMoveAngle);
    fFriction = abs(drag);
}

void CVElement::anim_move(const sf::Vector2f& distance,
                          const float& velocity,
                          const float& drag)
{
    if(bStatic) return;

    if((!distance.x && !distance.y) ||
       !velocity)
    {
        return;
    }

    bNoInteract = false;

    if(!bMove)
    {
        destination = getPosition() + distance;
        bMove = true;
    }
    else
    {
        destination = getPosition() + distance;
    }

    fMoveAngle = get_angle(getPosition(), destination);
    this->velocity = components(velocity, fMoveAngle);
    fFriction = abs(drag);
}

void CVElement::push(const float& angle,
                     const float& velocity,
                     const float& drag)
{
    if(bStatic) return;

    bNoInteract = false;
    bMove = true;
    this->velocity += components(velocity, angle);
    fMoveAngle = atan2(this->velocity.y, this->velocity.x);
    fFriction = abs(drag);
}

void CVElement::scale_velocity(const float& proportion)
{
    velocity = components(scalar(velocity) * proportion, fMoveAngle);
    fFriction *= proportion;
}

const CVApp* CVElement::mainApp() const
{
    return View->mainApp;
}

CVElement* CVElement::getElementById(const string& tag)
{
    return View->getElementById(tag);
}

void CVElement::addSprite(const sf::Texture* texture,
                          const sf::Vector2f& position,
                          const sf::Vector2f& size,
                          const sf::Color& fillColor,
                          const sf::IntRect& subRect)
{

    if(!texture ||
       ((isnan(size.x) && isnan(size.y)))) return;

    if(spriteList.empty())
    {
        spriteColor = fillColor;
    }

    spriteList.emplace_back();
    spriteList.back().setTexture(*texture, true);
    sf::Vector2u texSize = texture->getSize();

    if(isnan(size.x))
    {
        spriteList.back().setScale(
            size.y/texSize.y,
            size.y/texSize.y);
    }
    else if(isnan(size.y))
    {
        spriteList.back().setScale(
            size.x/texSize.x,
            size.x/texSize.x);
    }
    else{
        spriteList.back().setScale(
            size.x/texSize.x,
            size.y/texSize.y);
    }


    if(subRect.width && subRect.height)
    {
        spriteList.back().setTextureRect(subRect);
        spriteList.back().setOrigin(std::round(subRect.width/2), std::round(subRect.height/2));
    }
    else
    {
        spriteList.back().setOrigin(std::round(texSize.x/2), std::round(texSize.y/2));
    }

    spriteList.back().setPosition(std::round(bounds.left + position.x),
                                  std::round(bounds.top + position.y));
    spriteList.back().setColor(fillColor);
}

void CVElement::removeSprites(const string& tag)
{
    size_t L = spriteList.size();
    for(size_t i = 0; i < L;)
    {
        if(mainApp()->bitmaps.getTextureName(spriteList[i].getTexture()) == tag)
        {
            spriteList.erase(spriteList.begin() + i);
            --L;
        }
        else ++i;
    }
}

bool CVElement::has_sprite(const string& tag) const
{
    for(auto& sprite : spriteList)
    {
        if(mainApp()->bitmaps.getTextureName(sprite.getTexture()) == tag) return true;
    }
    return false;
}

sf::Sprite& CVElement::lastSprite()
{
    if(spriteList.empty()) throw invalid_argument("No sprites in sprite list to index");
    return spriteList.back();
}

sf::Sprite& CVElement::firstSprite()
{
    if(spriteList.empty()) throw invalid_argument("No sprites in sprite list to index");
    return spriteList.front();
}

sf::Sprite& CVElement::getSprite(const unsigned int& index)
{
    if(spriteList.empty()) throw invalid_argument("No sprites in sprite list to index");
    if(index > spriteList.size()) throw out_of_range("Index out of range of sprite list");
    return spriteList[index];
}

void CVElement::setSpriteSize(const size_t& index,
                              const sf::Vector2f& newSize,
                              const bool& maintainRatio)
{
    if(index > spriteList.size())
    {
        throw std::out_of_range("CVElement::setSpriteSize(): index out of range of sprite array");
    }

    sf::Sprite& sprite = spriteList[index];

    sf::Vector2f currentScale = sprite.getScale();
    sf::Vector2f currentSize(sprite.getGlobalBounds().width,
                             sprite.getGlobalBounds().height);
    sf::Vector2f fullSize = currentSize / currentScale;
    sf::Vector2f originRatio = sprite.getOrigin() / fullSize;

    sprite.setOrigin(newSize * originRatio);

    if(maintainRatio)
    {
        if(newSize.x < newSize.y)
        {
            float newRatio = newSize.x/fullSize.x;
            sprite.setScale(sf::Vector2f(newRatio, newRatio));
        }
        else
        {
            float newRatio = newSize.y/fullSize.y;
            sprite.setScale(sf::Vector2f(newRatio, newRatio));
        }
    }
    else
    {
        sprite.setScale(newSize/fullSize);
    }
}

}
