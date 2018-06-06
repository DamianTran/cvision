#pragma once

#ifndef CVIS_DATA
#define CVIS_DATA

#include "cvision/cvis_base.hpp"

namespace cvis{

template<class T>
class CVDataViewerPanel: public CVViewPanel{
protected:

    T* dataSource;

    std::vector<std::vector<CVTextBox>> viewMatrix;
    sf::Vector2u dataPos;

    sf::Color highlightColor;

public:

    virtual std::string get(const unsigned int& x, const unsigned int& y) = 0;
    virtual unsigned int rowSize(const unsigned int& x) = 0;
    virtual unsigned int nrow() = 0;

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw();

    CVDataViewerPanel(view* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Color& highlightColor = sf::Color(230,230,230)):
        CVViewPanel(parentView, panelTag, backgroundColor),
        highlightColor(highlightColor){ }


};

}

#endif // CVIS_DATA
