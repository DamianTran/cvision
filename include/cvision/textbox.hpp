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

#pragma once

#ifndef CVIS_TEXT_BOX
#define CVIS_TEXT_BOX

#include <string>

#include <SFML/Graphics.hpp>

#include "cvision/box.hpp"

#define TEXT_ALIGN_VERTICAL             0
#define TEXT_ALIGN_HORIZONTAL           1

#define ALIGN_LEFT                      0
#define ALIGN_LEFT_MIDLINE              1
#define ALIGN_LEFT_BOTTOM               2
#define ALIGN_CENTER                    3
#define ALIGN_CENTER_MIDLINE            4
#define ALIGN_CENTER_BOTTOM             5
#define ALIGN_RIGHT                     6
#define ALIGN_RIGHT_MIDLINE             8
#define ALIGN_RIGHT_BOTTOM              9
#define ALIGN_VERTICAL                  10
#define ALIGN_VERTICAL_INVERTED         11
#define ALIGN_30                        12
#define ALIGN_45                        13
#define ALIGN_60                        14
#define ALIGN_HORIZONTAL                15

namespace cvis
{

class CVEvent;
class CVView;

class CVISION_API TextEntry
{
public:

    TextEntry() { }
    TextEntry(std::string text,
              std::string font,
              unsigned int size = 12,
              unsigned int alignment = ALIGN_CENTER,
              sf::Color textColor = sf::Color::Black,
              sf::Text::Style textStyle = sf::Text::Style::Regular):
        text(text),
        font(font),
        fontSize(size),
        alignment(alignment),
        textColor(textColor),
        textStyle(textStyle) { }

    std::string text;
    std::string font;

    unsigned int fontSize;
    unsigned int alignment;

    sf::Color textColor;
    sf::Text::Style textStyle;

    inline void operator=(const std::string& newText)
    {
        text = newText;
    }
    inline void operator=(const unsigned int& newAlignment)
    {
        alignment = newAlignment;
    }
    inline void operator=(const sf::Color& newColor)
    {
        textColor = newColor;
    }

};

class CVISION_API CVTextBox : public CVBox
{
public:

    CVISION_API CVTextBox(CVView* View,
              const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f),
              const float& width = 0.0f,
              const float& height = 0.0f,
              const TextEntry& textInfo = TextEntry("", ""),
              const sf::Color& fillColor = sf::Color::Transparent,
              const sf::Color& borderColor = sf::Color::Transparent,
              const float& borderWidth = 0.0f);
    CVISION_API CVTextBox(CVView* View,
              const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f),
              const float& width = 0.0f,
              const float& height = 0.0f,
              const sf::Color& fillColor = sf::Color::Transparent,
              const sf::Color& borderColor = sf::Color::Transparent,
              const float& borderWidth = 0.0f);

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API sf::FloatRect getTextBounds() const noexcept;

    CVISION_API void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setString(const std::string& newString);
    CVISION_API void setString(const std::wstring& newString);

    CVISION_API void fitText(const bool& fitX = true,
                             const bool& fitY = true); // Fit the background panel to the text

    inline void setTextTemplate(const TextEntry& textInfo)
    {
        this->textInfo = textInfo;
    }

    CVISION_API void setText(const unsigned int& textIndex, const char* newText);
    CVISION_API void setText(const unsigned int& textIndex, const std::string& newText);

    CVISION_API void setText(const unsigned int& textIndex, const wchar_t* newText);
    inline void setText(const unsigned int& textIndex, const std::wstring& newText)
    {
        setText(textIndex, newText.c_str());
    }

    CVISION_API void setText(const unsigned int& textIndex, const sf::String& newText);
    CVISION_API void setTextSize(const unsigned int& newSize);

    CVISION_API void addTextEntry(const TextEntry& newText, float padding = 12.0f, bool regular = true);
    CVISION_API void addTextEntry(const TextEntry& newText, const sf::Vector2f& position);

    inline const TextEntry& getTextInfo()
    {
        return textInfo;
    }
    CVISION_API void setTextColor(const sf::Color& color);
    inline void setTextColor(const int& r, const int& g, const int& b, const int& a = 255)
    {
        setTextColor(sf::Color(r, g, b, a));
    }

    CVISION_API void setTextAlignment(const uint8_t& newAlignment);

    CVISION_API void setFont(const std::string& newFont);
    CVISION_API void setFont(const sf::Font& newFont);
    CVISION_API void setFontSize(const unsigned int& newSize);

    CVISION_API void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y)
    {
        setSize(sf::Vector2f(x, y));
    }

    CVISION_API void setTextWrap(const bool& state);
    CVISION_API void setTextPadding(const float& newPadding);
    CVISION_API void setExpand(const bool& state);
    CVISION_API void updateBounds();

    CVISION_API bool fadeComplete() const noexcept;

    CVISION_API void sendData(CVEvent& event) const;

    CVISION_API std::string getText() const;
    CVISION_API const sf::Color& getTextColor() const;

    inline const unsigned int& getTextSize() const noexcept{ return textInfo.fontSize; }

    inline void removeTextEntry(const unsigned int textIndex)
    {
        displayText.erase(displayText.begin() + textIndex);
    }

    inline const sf::Color& baseTextColor() const
    {
        return colorTheme[2];
    }

protected:

    friend class CVView;

    uint8_t textAlignment;
    TextEntry textInfo;

    std::vector<sf::Text> displayText;

    float textPadding;

    const sf::Font* textFont;

    bool bWrapText;

    CVISION_API virtual void alignText();
    CVISION_API void wrapText();

};

/*
    Plain text object inherits from CVTextBox in order to
    maintain box-like traits (wrapping, boundaries, etc)
*/

class CVISION_API CVText : public CVTextBox
{
public:

    CVISION_API CVText(CVView* View,
           const TextEntry& textInfo,
           const sf::Vector2f& location = sf::Vector2f(0.0f,0.0f),
           const sf::Vector2f& size = sf::Vector2f(0.0f,0.0f),
           const bool& bWrap = false);

};

}

#endif // CVIS_TEXT_BOX
