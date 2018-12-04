#pragma once

#ifndef CVIS_SKETCH
#define CVIS_SKETCH

#include "cvision/panel.hpp"
#include <SFML/Graphics.hpp>

// Base class for graphical draw tools

namespace cvis{

class CVBrush{
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

    void drawToImage(sf::Image& image, const sf::Vector2i& coords);
    void drawToImage(sf::Image& image,
                     const std::vector<sf::Vector2i>& coords);
    void drawToImage(sf::Image& image,
                     const std::vector<std::vector<int>>& coords);

    bool draw(sf::RenderTarget* target);

    void setIcon(const sf::Texture* icon);
    void clearIcon();

    void setPrimaryColor(const sf::Color& newColor);
    void setSecondaryColor(const sf::Color& newColor);

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getDimensions() const;
    const sf::Vector2f& getDrawScale() const;

    void setPosition(const sf::Vector2f& position);
    void move(const sf::Vector2f& distance);

    void setVisible(const bool& status);
    inline const bool& isVisible() const{
        return bVisible;
    }

    void scale(const float& ratio);
    void scale(const sf::Vector2f& ratios);

    inline void setDrawScale(const sf::Vector2f& newScale){ drawScale = newScale; }
    inline void setDimensions(const sf::Vector2f& newDimensions){ dimensions = newDimensions; }
    inline void setRadius(const float& newRadius){
        dimensions.x = newRadius;
        dimensions.y = newRadius;
    }

    void setEffect(const CVBrushEffect& effect);
    void setType(const CVBrushType& type);

    CVBrush(const sf::Color& color = sf::Color::Black,
            const float& radius = 1.0f,
            const float& rounding = 0.0f,
            const float& softening = 0.0f,
            const CVBrushType& brushType = CVBrush::square,
            const CVBrushEffect& brushEffect = CVBrushEffect::none);
};

// Base class for graphical draw canvas managers

class CVSketchPanel : public CVBasicViewPanel{
protected:

    sf::Image drawCanvas;

    class CVSketchLayer : public sf::RectangleShape{
    protected:
        sf::Texture cachedTexture;
        std::string tag;
        CVSketchPanel* host;

        bool bVisible;

    public:

        inline const std::string& getTag() const{ return tag; }
        void loadFromImage(const sf::Image& img);
        void getTextureImage(sf::Image& output) const;

        sf::Vector2u getImageSize() const;

        inline const bool& isVisible() const{ return bVisible; }
        inline void setVisible(const bool& status = true){ bVisible = status; }

        CVSketchLayer(CVSketchPanel* host,
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

    void clear();

    bool canDraw() const;
    void lockDraw();
    void unlockDraw();

    void setDisplayCoords(const bool& status = true);

    sf::Vector2i viewToPixelCoords(const sf::Vector2f& position) const;

    void select_brush(const unsigned int& brush_index);
    void select_layer(const unsigned int& layer_index);
    void select_layer(const std::string& tag);

    void draw_to_layer(const unsigned int& layer_index,
                       sf::Vector2i coords);
    void draw_to_layer(const unsigned int& layer_index,
                       std::vector<std::vector<int>> coords);
    void draw_to_layer(const std::string& tag);
    void draw_to_layer(const std::string& tag,
                       std::vector<std::vector<int>> coords);

    template<class... Args>
    void add_brush(Args&&... args){
        brushes.emplace_back(std::forward<Args>(args)...);
    }

    template<class... Args>
    void add_layer(Args&&... args){
        brushes.emplace_back(std::forward<Args>(args)...);
    }

    void select_pixels(const sf::Rect<int>& boundaries);
    void select_pixels(const sf::VertexArray& vertices);
    void select_pixels(const std::vector<sf::Vector2i>& pixels);

    void highlight_pixels(const sf::Rect<int>& boundaries,
                          const sf::Color& color,
                          const bool& clear = false);
    void clear_highlight();

    void flattenToTexture(sf::Texture& out_tex);
    void flattenToImage(sf::Image& out_img);
    void flatten();

    void setForegroundColor(const sf::Color& newColor);
    void setBackgroundColor(const sf::Color& newColor);

    void save(const std::string& file);
    void loadLayer(const std::string& file,
                   const unsigned int& layerIdx = UINT_MAX);
    void load(const std::string& file);

    void setPosition(const sf::Vector2f& position);
    void setPosition(const float& x, const float& y);

    void move(const sf::Vector2f& distance);
    void move(const float& x, const float& y);

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVSketchPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_SKETCH
