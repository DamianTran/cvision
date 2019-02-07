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

#pragma once

#ifndef CVIS_SKETCH
#define CVIS_SKETCH

#include "cvision/panel.hpp"
#include <SFML/Graphics.hpp>

// Base class for graphical draw tools

namespace cvis{

class CVISION_API CVBrush{
protected:

    float rounding,
            softening;

    sf::Color primaryColor,
                secondaryColor;

    sf::Vector2f position,
                dimensions,
                drawScale;

    const sf::Texture* icon;

    bool bVisible;

public:

    enum CVBrushType{
        none = -1,
        square = 0,
        rounded = 1,
        point = 2
    } type;

    enum class CVBrushEffect{
        none = 0,
        airbrush,
        distort,
        smudge,
        erase,
        blur,
        connective
    } effect;

    CVISION_API void drawToImage(sf::Image& image, const sf::Vector2i& coords);
    CVISION_API void drawToImage(sf::Image& image,
                     const std::vector<sf::Vector2i>& coords);
    CVISION_API void drawToImage(sf::Image& image,
                     const std::vector<std::vector<int>>& coords);

    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setIcon(const sf::Texture* icon);
    CVISION_API void clearIcon();

    CVISION_API void setPrimaryColor(const sf::Color& newColor);
    CVISION_API void setSecondaryColor(const sf::Color& newColor);

    CVISION_API const sf::Vector2f& getPosition() const;
    CVISION_API const sf::Vector2f& getDimensions() const;
    CVISION_API const sf::Vector2f& getDrawScale() const;

    CVISION_API void setPosition(const sf::Vector2f& position);
    CVISION_API void move(const sf::Vector2f& distance);

    CVISION_API void setVisible(const bool& status);
    inline const bool& isVisible() const{
        return bVisible;
    }

    CVISION_API void scale(const float& ratio);
    CVISION_API void scale(const sf::Vector2f& ratios);

    inline void setDrawScale(const sf::Vector2f& newScale){ drawScale = newScale; }
    inline void setDimensions(const sf::Vector2f& newDimensions){ dimensions = newDimensions; }
    inline void setRadius(const float& newRadius){
        dimensions.x = newRadius;
        dimensions.y = newRadius;
    }

    CVISION_API void setEffect(const CVBrushEffect& effect);
    CVISION_API void setType(const CVBrushType& type);

    CVISION_API CVBrush(const sf::Color& color = sf::Color::Black,
            const float& radius = 1.0f,
            const float& rounding = 0.0f,
            const float& softening = 0.0f,
            const CVBrushType& brushType = CVBrush::square,
            const CVBrushEffect& brushEffect = CVBrushEffect::none);
};

// Base class for graphical draw canvas managers

class CVISION_API CVSketchPanel : public CVBasicViewPanel{
protected:

    sf::Image drawCanvas;

    class CVISION_API CVSketchLayer : public sf::RectangleShape{
    protected:
        sf::Texture cachedTexture;
        std::string tag;
        CVSketchPanel* host;

        bool bVisible;

    public:

        inline const std::string& getTag() const{ return tag; }
        CVISION_API void loadFromImage(const sf::Image& img);
        CVISION_API void getTextureImage(sf::Image& output) const;

        CVISION_API sf::Vector2u getImageSize() const;

        inline const bool& isVisible() const{ return bVisible; }
        inline void setVisible(const bool& status = true){ bVisible = status; }

        CVISION_API CVSketchLayer(CVSketchPanel* host,
                      const sf::Vector2f& position,
                      const sf::Vector2f& size,
                      const sf::Color& background = sf::Color::White,
                      const std::string& tag = "");
    };

    std::vector<CVSketchLayer> layers;
    CVSketchLayer selectionLayer,
                highlightLayer;

    std::vector<CVBrush> brushes;
    sf::Text brushCoords;

    sf::Color fg_color,
            bg_color;

    std::vector<sf::Color> palette,
                            last_colors;

    unsigned int selected_layer,
                    selected_brush;

    std::vector<sf::Vector2i> selectedVertices;

    CVButtonPanel* toolset;

    bool bCanDraw,
        bBrushCoords;

public:

    CVISION_API void clear();

    CVISION_API bool canDraw() const;
    CVISION_API void lockDraw();
    CVISION_API void unlockDraw();

    CVISION_API void setDisplayCoords(const bool& status = true);

    CVISION_API sf::Vector2i viewToPixelCoords(const sf::Vector2f& position) const;

    CVISION_API void select_brush(const unsigned int& brush_index);
    CVISION_API void select_layer(const unsigned int& layer_index);
    CVISION_API void select_layer(const std::string& tag);

    CVISION_API void draw_to_layer(const unsigned int& layer_index,
                       sf::Vector2i coords);
    CVISION_API void draw_to_layer(const unsigned int& layer_index,
                       std::vector<std::vector<int>> coords);
    CVISION_API void draw_to_layer(const std::string& tag);
    CVISION_API void draw_to_layer(const std::string& tag,
                       std::vector<std::vector<int>> coords);

    template<class... Args>
    void add_brush(Args&&... args){
        brushes.emplace_back(std::forward<Args>(args)...);
    }

    template<class... Args>
    void add_layer(Args&&... args){
        brushes.emplace_back(std::forward<Args>(args)...);
    }

    CVISION_API void select_pixels(const sf::Rect<int>& boundaries);
    CVISION_API void select_pixels(const sf::VertexArray& vertices);
    CVISION_API void select_pixels(const std::vector<sf::Vector2i>& pixels);

    CVISION_API void highlight_pixels(const sf::Rect<int>& boundaries,
                          const sf::Color& color,
                          const bool& clear = false);
    CVISION_API void clear_highlight();

    CVISION_API void flattenToTexture(sf::Texture& out_tex);
    CVISION_API void flattenToImage(sf::Image& out_img);
    CVISION_API void flatten();

    CVISION_API void setForegroundColor(const sf::Color& newColor);
    CVISION_API void setBackgroundColor(const sf::Color& newColor);

    CVISION_API void save(const std::string& file);
    CVISION_API void loadLayer(const std::string& file,
                   const unsigned int& layerIdx = UINT_MAX);
    CVISION_API void load(const std::string& file);

    CVISION_API void setPosition(const sf::Vector2f& position);
    CVISION_API void setPosition(const float& x, const float& y);

    CVISION_API void move(const sf::Vector2f& distance);
    CVISION_API void move(const float& x, const float& y);

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVSketchPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_SKETCH
