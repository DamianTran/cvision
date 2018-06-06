#pragma once

#ifndef CVIS_ALGORITHM
#define CVIS_ALGORITHM

#include <windows.h>

#include "algorithm.hpp"
#include <SFML/Graphics.hpp>

// CVision macros

#define TIME_NOW                        std::chrono::high_resolution_clock::now()
#define TIME_EPOCH                      std::chrono::duration_cast<std::chrono::seconds>(TIME_NOW.time_since_epoch()).count()

// Alignment flags

#define ALIGN_LEFT                      0_BIT
#define ALIGN_LEFT_MIDLINE              1_BIT
#define ALIGN_LEFT_TOP                  2_BIT
#define ALIGN_CENTER                    3_BIT
#define ALIGN_CENTER_MIDLINE            4_BIT
#define ALIGN_CENTER_TOP                5_BIT
#define ALIGN_RIGHT                     6_BIT
#define ALIGN_RIGHT_MIDLINE             8_BIT
#define ALIGN_RIGHT_TOP                 9_BIT
#define ALIGN_VERTICAL                  10_BIT
#define ALIGN_VERTICAL_INVERTED         11_BIT
#define ALIGN_30                        12_BIT
#define ALIGN_45                        13_BIT
#define ALIGN_60                        14_BIT
#define ALIGN_HORIZONTAL                15_BIT

// Dimension space

#define DIMENSION_X                     0u
#define DIMENSION_Y                     1u
#define DIMENSION_Z                     2u

#define CV_DIRECTION_NONE               0b0
#define CV_DIRECTION_X                  0b1
#define CV_DIRECTION_Y                  0b10
#define CV_DIRECTION_Z                  0b100

#define DIMX                            0u
#define DIMY                            1u
#define DIMZ                            2u

// Functions

template<typename T> std::ostream& operator<<(std::ostream& output, const sf::Vector2<T>& V){
    output << '[' << V.x << ',' << V.y << ']';
    return output;
}

template<typename T> sf::Rect<T>& operator<<(std::ostream& output, const sf::Rect<T>& box){
    output << "[Left: " << box.left << ";Top: " << box.top << ";Width: " << box.width << ";Height: " << box.height << ']';
    return output;
}

namespace cvis{

// Colors

typedef std::vector<sf::Color> ColorTheme;

inline void brighten(sf::Shape* shape, uint8_t degree, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
        color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
        color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
        shape->setOutlineColor(color);
    }
};

inline void brighten(sf::Color& color, uint8_t degree){
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
}

inline void darken(sf::Shape* shape, uint8_t degree, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
    color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
    color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
    shape->setFillColor(color);
    if(border){
        color = shape->getOutlineColor();
        color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
        color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
        color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
        shape->setOutlineColor(color);
    }
};

inline void darken(sf::Color& color, uint8_t degree){
    color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
    color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
    color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
}

inline void contrast(sf::Color& color, const sf::Color& refColor, uint8_t degree){

    if(color.r >= refColor.r){
        if((uint16_t)color.r + degree <= 255) color.r += degree;
        else if(color.r >= degree) color.r = refColor.r - degree;
        else color.r = 255_BIT;
    }
    else{
        if(color.r >= degree) color.r -= degree;
        else if((uint16_t)color.r + degree <= 255) color.r = refColor.r + degree;
        else color.r = 0_BIT;
    }

    if(color.g >= refColor.g){
        if((uint16_t)color.g + degree <= 255) color.g += degree;
        else if(color.g >= degree) color.g = refColor.g - degree;
        else color.g = 255_BIT;
    }
    else{
        if(color.g >= degree) color.g -= degree;
        else if((uint16_t)color.g + degree <= 255) color.g = refColor.g + degree;
        else color.g = 0_BIT;
    }

    if(color.b >= refColor.b){
        if((uint16_t)color.b + degree <= 255) color.b += degree;
        else if(color.b >= degree) color.b = refColor.b - degree;
        else color.b = 255_BIT;
    }
    else{
        if(color.b >= degree) color.b -= degree;
        else if((uint16_t)color.b + degree <= 255) color.b = refColor.b + degree;
        else color.b = 0_BIT;
    }
}

inline void red_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    if(descrimination > 0){
        color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
        color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
        if(descrimination > 0){
            color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
            color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};
inline void blue_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
    if(descrimination > 0){
        color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
        color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
        if(descrimination > 0){
            color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
            color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};
inline void green_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    if(descrimination > 0){
        color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
        color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.g = (uint16_t)color.g + (uint16_t)degree <= 255 ? color.g + degree : 255;
        if(descrimination > 0){
            color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
            color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};

inline void colorSwatch(ColorTheme& colors, size_t numColors){
    colors.reserve(colors.size() + numColors);
    size_t L = numColors/2;
    uint8_t colGradient = numColors > uint8_t(8) ? uint8_t(255)/numColors : uint8_t(255)/uint8_t(8);
    for(size_t i = 0; i < L; ++i){
        colors.push_back(colors.front());
        brighten(colors.back(), colGradient);
    }

    L = numColors - L;
    for(size_t i = 0; i < L; ++i){
        colors.push_back(colors.front());
        darken(colors.back(), colGradient);
    }
}

sf::Color textToColor(const std::string& input);

void alignText(std::vector<sf::Text>& textItems, std::vector<sf::Vector2f> coords);
void alignShapes(std::vector<sf::Shape*>& textItems, std::vector<sf::Vector2f> coords);

template<typename T1, typename T2> bool addUnique(T1& item, std::vector<T2>& vectorItem){
    for(typename std::vector<T2>::iterator it = vectorItem.begin();
        it != vectorItem.end(); ++it){
            if(*it == item) return false;
        }
    vectorItem.push_back(item);
    return true;
}

template<typename T1, typename T2> void eraseItem(const T1& item, std::vector<T2>& vectorItem){
    for(typename std::vector<T2>::iterator it = vectorItem.begin();
        it != vectorItem.end();){
            if(*it == item) vectorItem.erase(it);
            else ++it;
        }
}

void sigFigs(std::string& numeric);
std::string sigFigs(const float& number);


inline sf::Vector2f getOffset(sf::Vector2f source, sf::Vector2f target){
    return sf::Vector2f(target.x - source.x, target.y - source.y);
}

inline float getElapsedTime(std::chrono::high_resolution_clock::time_point& t0,
                     std::chrono::high_resolution_clock::time_point t1){
    return std::chrono::duration_cast<std::chrono::duration<float>>(t1-t0).count();
}

inline float getDistance(const sf::Vector2f& source, const sf::Vector2f& target){
    return sqrt(pow(target.x - source.x, 2) + pow(target.y - source.y, 2));
}

sf::Vector2f getTextHorizontalAlignment(sf::Text& text, sf::Rect<float>& bounds,
                         uint8_t alignment, float borderPadding = 12.0f);

inline float getTextCenterOffsetY(const sf::Text& text){
    return text.getFont()->getLineSpacing(text.getCharacterSize())/2;
}
inline float getTextRelativeCenterY(const sf::Text& text){
    return text.getGlobalBounds().height/2 + getTextCenterOffsetY(text);
}

template<typename T> sf::Rect<T> operator+(const sf::Rect<T>& focus, const sf::Rect<T>& other){
    return sf::Rect<T>(focus.left,
                       focus.top,
                       focus.width + other.width,
                       focus.height + other.height);
}
template<typename T> void operator +=(sf::Rect<T>& focus, const sf::Rect<T>& other){
    focus.height += other.height;
    focus.width += other.width;
}
template<typename T1, typename T2> void operator/=(sf::Rect<T1>& focus, const T2 divisor){
    focus.height /= divisor;
    focus.width /= divisor;
}

template<typename T> sf::Rect<T> averageBounds(std::vector<sf::Rect<T>>& rectVector){
    sf::Rect<T> output;
    size_t L = 0;
    for(typename std::vector<sf::Rect<T>>::iterator it = rectVector.begin();
        it != rectVector.end(); ++it){
            output += *it;
            ++L;
        }
    if(L > 0) output /= L;
    return output;
}

sf::Rect<float> averageTextSize(std::vector<sf::Text>& textItems);

inline float totalTextHeight(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto text : textItems){
        output += text.getGlobalBounds().height;
    }
    return output;
}

inline float maxTextWidth(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto& text : textItems){
        if(text.getGlobalBounds().width > output) output = text.getGlobalBounds().width;
    }
    return output;
}

inline float maxTextHeight(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto& text : textItems){
        if(text.getGlobalBounds().height > output) output = text.getGlobalBounds().height;
    }
    return output;
}

// Keyboard

inline bool ctrlPressed(){
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}
inline bool shiftPressed(){
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
}
inline bool altPressed(){
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt));
}

// Clipboard

bool copyToClipboard(const std::string& cpyString);
std::string getClipboardText();

inline const unsigned int taggedIndex(std::string tag, std::vector<std::string>& tags){
    unsigned int index = 0;
    for(std::vector<std::string>::const_iterator it = tags.begin();
        it != tags.end(); ++it, ++index){
            if(*it == tag) return index;
        }
    return UINT_MAX;
}

template<typename T> T* taggedItem(std::string tag, std::vector<T>& items, std::vector<std::string>& tags){
    unsigned int L = 0;
    for(std::vector<std::string>::const_iterator strIT = tags.begin();
        strIT != tags.end(); ++strIT){
            if(*strIT == tag) return &items[L];
            else ++L;
        }
    return nullptr;
}

void sleep(float time);
float getElapsedTime(std::chrono::high_resolution_clock::time_point& t0,
                     std::chrono::high_resolution_clock::time_point t1);


// 2D space

void physicsSpread(std::vector<sf::Sprite>& sprites, const float& strength, const BYTE& direction = CV_DIRECTION_X);
void physicsSpread(std::vector<sf::Shape*>& shapes, const float& strength, const BYTE& direction = CV_DIRECTION_X);
void physicsSpread(std::vector<sf::Sprite*>& shapes, const float& strength, const BYTE& direction = CV_DIRECTION_X);

double get_angle(const sf::Vector2f& origin, const sf::Vector2f& destination); // Returns radians

inline sf::Vector2f radial_position(const sf::Vector2f& origin, const float& radius, const double& angle){ // Accepts degrees
    return origin + sf::Vector2f(radius*cos(-angle*PI/180), radius*sin(-angle*PI/180));
}

inline void rotate_about_origin(sf::Vector2f& position, const sf::Vector2f& origin, const double& angle){
    position = radial_position(origin, getDistance(origin, position), get_angle(origin, position)*180/PI + angle);
}

template<typename T> void rotate(sf::Rect<T>& rect, const sf::Vector2f& origin, const float& angle){ // Gets new bounds of virtual rotation

}

}

#endif // CVIS_ALGORITHM
