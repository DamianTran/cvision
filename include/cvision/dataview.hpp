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

#ifndef CVIS_DATA
#define CVIS_DATA

#include "cvision/table.hpp"
#include "EZC/algorithm.hpp"
#include "cvision/coordmap.hpp"
#include "cvision/view.hpp"

class _2Dstream;

namespace cvis{

template<typename T> size_t nrow(const std::vector<std::vector<T>>& M){
    return M.size();
}

template<typename T> size_t ncol(const std::vector<std::vector<T>>& M){
    return EZC::maxSize(M);
}

template<typename T> size_t rowSize(const std::vector<std::vector<T>>& M, const unsigned int& index){
    return M[index].size();
}

class CVISION_API CVDataViewerPanel: public CVTablePanel{
protected:

    _2Dstream* dataSource, // Requires a double-indexable item with nrow() and ncol() overload
       * incomingDataSource;

    sf::FloatRect totalBounds,
                exclusionBounds;

    sf::RectangleShape cornerBlock;

    sf::Vector2f tableOffset;
    sf::Vector2i coordOffset,
                coordAnchor,
                dataSize;

    std::vector<sf::Vector2i> selectedCoords;

    coord_map modifications;

    std::vector<unsigned int> selectedRows,
                selectedColumns;

    sf::RectangleShape cordon;

    bool bDisconnect;

    CVISION_API bool updateCells();

    CVISION_API void applyModifications();

    // Required virtual function overloads

    CVISION_API virtual void getDataSize();
    CVISION_API virtual std::string getData(const int& x, const int& y) const;
    CVISION_API virtual size_t rowSize(const int& y) const;

public:

    inline const unsigned int& view_x() const{ return view_coords[0]; }
    inline const unsigned int& view_y() const{ return view_coords[1]; }
    inline const unsigned int& view_z() const{ return view_coords[2]; }

    inline const unsigned int& view_dim(const unsigned int& dimension) const{ return view_coords[dimension]; }

    inline void set_pos(const std::vector<unsigned int>& coords){ view_coords = coords; }
    inline const std::vector<unsigned int>& get_coords() const{ return view_coords; }

    inline void set_x(const unsigned int& newX){ view_coords[0] = newX; }
    inline void set_y(const unsigned int& newY){ view_coords[1] = newY; }
    inline void set_z(const unsigned int& newZ){ view_coords[2] = newZ; }

    inline void changeFocus(_2Dstream* newFocus){

        incomingDataSource = newFocus;

    }

    CVISION_API void setup();

    const _2Dstream* getDataSource() const
    {
        return dataSource;
    }

    inline void move(const sf::Vector2f& distance) override{

        CVTablePanel::move(distance);
        exclusionBounds.left += distance.x;
        exclusionBounds.top += distance.y;
        cornerBlock.move(distance);

    }

    inline void setHighlightColor(const sf::Color& newColor){

        CVTablePanel::setHighlightColor(newColor);
        setCellHighlightColor(newColor);

        cordon.setFillColor(cellHighlightColor);
        cordon.setOutlineThickness(2.0f);

        sf::Color cordonOutlineColor(cellHighlightColor);
        cordonOutlineColor -= 10;
        cordonOutlineColor.a = cordonOutlineColor.a + 30;

        cordon.setOutlineColor(cordonOutlineColor);

    }

    inline void setPosition(const sf::Vector2f& newPosition) override{

        move(newPosition - getPosition());

    }

    inline void set_dim(const unsigned int& dimension,
                        const unsigned int& value){ view_coords[dimension] = value; }

    inline void disconnect(){

        bDisconnect = true;

    }

    inline void clear(){

        CVTablePanel::clear();
        coordOffset.x = 0;
        coordOffset.y = 0;
        tableOffset.x = 0.0f;
        tableOffset.y = 0.0f;
        scrollBarX.setScrollOffset(0.0f);
        scrollBarY.setScrollOffset(0.0f);
        modifications.clear();

    }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    inline bool draw(sf::RenderTarget* target){
        if(!CVTextBox::draw(target)) return false;

        CV_DRAW_CLIP_BEGIN

        for(auto& row : cells){

            for(auto& cell : row){

                cell->draw(target);

            }

        }

        target->draw(cordon);

        for(auto& bar : headerBars){

            bar->draw(target);

        }

        for(auto& bar : rowBars){

            bar->draw(target);

        }

        target->draw(cornerBlock);

        scrollBarX.draw(target);
        scrollBarY.draw(target);

        CV_DRAW_CLIP_END

        return true;
    }

    CVISION_API bool copySelected();
    CVISION_API bool pasteFromClipboard();

    virtual bool saveToFile(const std::string& file) const;

    CVDataViewerPanel(CVView* parentView, _2Dstream* dataSource = nullptr, std::string panelTag = "",
                      const sf::Color& backgroundColor = sf::Color::Transparent,
                      const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                      const bool& bFitWindow = true,
                      const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f)):
        CVTablePanel(parentView, panelTag, backgroundColor, size, bFitWindow, position),
        dataSource(dataSource),
        incomingDataSource(nullptr),
        totalBounds(bounds),
        exclusionBounds(bounds),
        tableOffset(0.0f,0.0f),
        coordOffset(0,0),
        bDisconnect(false)
        {

            setHighlightColor(cellHighlightColor);

            setup();

        }


};

}

#endif // CVIS_DATA
