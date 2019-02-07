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

#include "cvision/table.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include "EZC/algorithm.hpp"

using namespace EZC;

namespace cvis
{

CVTablePanel::CVTablePanel(CVView* parentView, const std::string& panelTag,
                           const sf::Color& bkgColor, const sf::Vector2f& size,
                           const bool& bFitWindow, const sf::Vector2f& position):
    CVBasicViewPanel(parentView, panelTag, bkgColor,
                     size, bFitWindow, position),
    scrollBarX(parentView,
               sf::Vector2f(bounds.left + 12.0f,
                            bounds.top + bounds.height - 5.0f),
               sf::Vector2f(bounds.left + bounds.width - 12.0f,
                            bounds.top + bounds.height - 5.0f),
               size.x, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
               1000.0f, CV_OBJ_ANIM_FADE_IN),
    scrollBarY(parentView,
               sf::Vector2f(bounds.left + bounds.width - 5.0f,
                            bounds.top + 12.0f),
               sf::Vector2f(bounds.left + bounds.width - 5.0f,
                            bounds.top + bounds.height - 12.0f),
               size.y, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
               1000.0f, CV_OBJ_ANIM_FADE_IN),
    outerPanelCacheDist(5),
    cellSize(128.0f, 36.0f),
    sizeOffset(0.0f,0.0f),
    cellFontSize(16*View->getViewScale()),
    headerFontSize(22*View->getViewScale()),
    cellOutlineWidth(1.0f),
    cellFillColor(bkgColor),
    cellOutlineColor(sf::Color(128,128,128)),
    headerThemeColor(160,160,160,255),
    margins(
    {
        20.0f, 20.0f, 20.0f, 20.0f
    }),
    view_coords({0,0}),
    bHighlightCells(false)
{

    scrollBarX.setDirection(CV_WHEEL_DIR_HORIZONTAL);

    scrollBarX.setPanelSize(size.x - left_margin() - right_margin());
    scrollBarY.setPanelSize(size.y - top_margin() - bottom_margin());

    if(bkgColor == sf::Color::Transparent)
    {
        cellHighlightColor = sf::Color(12,50,200,128);
    }
    else
    {
        cellHighlightColor = bkgColor;
        contrast(cellHighlightColor, bkgColor, 50);
    }

    textInfo.font = mainApp()->getDefaultFont();
    textInfo.alignment = ALIGN_LEFT_MIDLINE;
    textInfo.fontSize = cellFontSize;
    textInfo.textColor = sf::Color::Black;

    setOutOfBoundsUpdate(true);
    setOutOfBoundsDraw(true);

    setDrawClipping(true);

}

void CVTablePanel::clear()
{

    for(auto& row : cells)
    {

        for(auto& cell : row)
        {

            removePanelElement(cell);

        }

    }

    sizeOffset.x = 0.0f;
    sizeOffset.y = 0.0f;

    cells.clear();

}

void CVTablePanel::setCellFillColor(const sf::Color& newColor)
{
    cellFillColor = newColor;
    for(auto& row : cells)
    {
        for(auto& cell : row)
        {
            cell->setFillColor(newColor);
        }
    }
}

void CVTablePanel::setCellOutlineColor(const sf::Color& newColor)
{
    cellOutlineColor = newColor;
    for(auto& row : cells)
    {
        for(auto& cell : row)
        {
            cell->setOutlineColor(newColor);
        }
    }
}

void CVTablePanel::setCellHighlightColor(const sf::Color& newColor)
{
    cellHighlightColor = newColor;
    for(auto& row : cells)
    {
        for(auto& cell : row)
        {
            cell->setHighlightColor(newColor);
        }
    }
}

void CVTablePanel::setCellHighlightable(const bool& status)
{

    bHighlightCells = status;
    for(auto& row : cells)
    {

        for(auto& cell : row)
        {

            cell->setHighlightableStatus(status);

        }

    }

}

void CVTablePanel::expandCols(const int& y)
{

    size_t x, nrow = cells.size();

    if(!cells.empty()) x = maxSize(cells);
    else x = 0;

    for(size_t i = nrow; i < nrow + y; ++i)
    {
        cells.emplace_back();

        if(cells.size() > 1)
        {
            for(size_t j = 0; j < x; ++j)
            {
                cells.back().emplace_back(new CVTypeBox(View, sf::Vector2f(cells[nrow - 1][j]->getPosition().x,
                                                        cells[nrow - 1][j]->getPosition().y +
                                                        cells[nrow - 1][j]->getBounds().height),
                                                        cells[nrow - 1][j]->getBounds().width, cellSize.y,
                                                        textInfo, cellFillColor, cellOutlineColor,
                                                        cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                                                        CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
//                cells.back().back()->setHighlightColor(cellHighlightColor);
//                cells.back().back()->setHighlightableStatus(bHighlightCells);
                addPanelElement(cells.back().back());
            }
        }
        else
        {
            sf::Vector2f newPos;
            for(size_t j = 0; j < x; ++j)
            {
                if(j == 0) newPos.x = getPosition().x + left_margin() + cellOutlineWidth;
                else newPos.x = cells.back().back()->getPosition().x + cells.back().back()->getBounds().width + cellOutlineWidth;

                newPos.y = getPosition().y + top_margin();

                cells.back().emplace_back(new CVTypeBox(View, newPos,
                                                        cellSize.x, cellSize.y,
                                                        textInfo, cellFillColor, cellOutlineColor,
                                                        cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                                                        CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
//                cells.back().back()->setHighlightColor(cellHighlightColor);
//                cells.back().back()->setHighlightableStatus(bHighlightCells);
                addPanelElement(cells.back().back());
            }
        }
    }
}

void CVTablePanel::expandRows(const int& x)
{

    if(cells.empty())
    {
        cells.emplace_back();
    }

    size_t y = cells.size(),
           ncol = maxSize(cells),
           i, j = 0;

    for(; j < y; ++j)
    {

        for(i = cells[j].size(); i < ncol + x; ++i)
        {

            if(i)
            {
                cells[j].emplace_back(new CVTypeBox(View, sf::Vector2f(cells[j][i-1]->getPosition().x + cells[j][i-1]->getBounds().width,
                                                    cells[j][i-1]->getPosition().y),
                                                    cellSize.x, cells[j][i-1]->getBounds().height,
                                                    textInfo, cellFillColor, cellOutlineColor,
                                                    cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                                                    CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
            }
            else
            {
                cells[j].emplace_back(new CVTypeBox(View, sf::Vector2f(getPosition().x + left_margin(),
                                                    getPosition().y + top_margin() + j*(cellSize.y)),
                                                    cellSize.x, cellSize.y,
                                                    textInfo, cellFillColor, cellOutlineColor,
                                                    cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                                                    CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
            }

//            cells[j].back()->setHighlightColor(cellHighlightColor);
//            cells[j].back()->setHighlightableStatus(bHighlightCells);
            addPanelElement(cells[j].back());

        }
    }
}

void CVTablePanel::removeRow(const unsigned int& y)
{
    if(y < cells.size())
    {

        for(size_t i = y + 1; i < cells.size(); ++i)
        {
            for(size_t j = 0; j < cells[i].size(); ++j)
            {
                cells[i][j]->move(0.0f, -cells[y].front()->getBounds().height);
            }
        }

        for(auto& cell : cells[y])
        {
            removePanelElement(cell);
        }

        cells.erase(cells.begin() + y);
    }
}

void CVTablePanel::removeCol(const unsigned int& x)
{
    for(auto& row : cells)
    {
        if(x < row.size())
        {

            for(size_t i = x + 1; i < row.size(); ++i)
            {
                row[i]->move(-row[x]->getBounds().width, 0.0f);
            }

            removePanelElement(row[x]);
            row.erase(row.begin() + x);
        }
    }
}

void CVTablePanel::insertRow(const unsigned int& y)
{

    if(!y && cells.empty())
    {
        cells.emplace_back();
        return;
    }
    else if(y >= cells.size()) throw std::invalid_argument("CVTablePanel: Row out of range of cell matrix");

    size_t ncol = cells[y].size();

    cells.insert(cells.begin() + y, std::vector<CVTypeBox*>());

    float newCellHeight = cellSize.y;

    for(size_t i = 0; i < ncol; ++i)
    {
        cells[y].emplace_back(new CVTypeBox(View, sf::Vector2f(cells[y + 1][i]->getPosition().x,
                                            cells[y + 1][i]->getPosition().y),
                                            cells[y + 1][i]->getBounds().width, cellSize.y,
                                            textInfo, cellFillColor, cellOutlineColor,
                                            cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                                            CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
//        cells[y].back()->setHighlightColor(cellHighlightColor);
//        cells[y].back()->setHighlightableStatus(bHighlightCells);
        addPanelElement(cells[y].back());

        if(cells[y].back()->getGlobalBounds().height > newCellHeight)
        {
            newCellHeight = cells[y].back()->getGlobalBounds().height;
        }
    }

    if(newCellHeight != cellSize.y)
    {

        for(auto& cell : cells[y])
        {

            cell->setSize(cell->getSize().x, newCellHeight);

        }

    }

    float moveDist = cells[y].front()->getBounds().height + cellOutlineWidth;

    if(y)
    {
        if(ncol)
        {

            for(size_t i = y + 1; i < cells.size(); ++i)
            {
                for(auto& cell : cells[i])
                {
                    cell->move(0.0f, moveDist);
                }
            }

        }
    }
    else
    {

        for(auto& cell : cells.front())
        {

            cell->move(0.0f, -moveDist);

        }

    }

}

void CVTablePanel::updateExpand()
{

    if(cells.empty()) return;

    sf::Vector2f newCellSize;

    float moveDist;

    for(size_t y = 0; y < cells.size(); ++y)
    {

        newCellSize.y = cellSize.y;

        for(auto& cell : cells[y])
        {

            if(cell->getSize().y > newCellSize.y) newCellSize.y = cell->getSize().y;

        }

        if(newCellSize.y > cellSize.y)
        {

            for(auto& cell : cells[y])
            {

                cell->setSize(cell->getSize().x, newCellSize.y);

            }

            rowBars[y]->setSize(left_margin(), newCellSize.y);

            if(y < cells.size() - 1)
            {

                for(size_t x = 0; x < cells[y].size(); ++x)
                {

                    moveDist = cells[y][x]->getPosition().y +
                               cells[y][x]->getSize().y +
                               cellOutlineWidth -
                               cells[y+1][x]->getPosition().y;

                    for(size_t i = y + 1; i < cells.size(); ++i)
                    {

                        if(x < cells[i].size())
                        {

                            cells[i][x]->move(0.0f, moveDist);

                        }

                    }

                }

                for(size_t i = y + 1; i < cells.size(); ++i)
                {

                    rowBars[i]->move(0.0f, moveDist);

                }

                sizeOffset.y += moveDist;

            }

        }

    }

    int maxWidth = maxSize(cells);
    for(size_t x = 0; x < maxWidth; ++x)
    {

        newCellSize.x = cellSize.x;

        for(auto& row : cells)
        {

            if((x < row.size()) && (row[x]->getSize().x > newCellSize.x))
            {

                newCellSize.x = row[x]->getSize().x;

            }

        }

        if(newCellSize.x > cellSize.x)
        {

            for(auto& row : cells)
            {

                row[x]->setSize(newCellSize.x, row[x]->getSize().y);

            }

            int maxWidth = maxSize(cells);

            for(size_t i = x + 1; i < maxWidth; ++i)
            {

                for(auto& row : cells)
                {

                    row[i]->move(newCellSize.x - cellSize.x, 0.0f);

                }

            }

            sizeOffset.x += moveDist;

        }

    }

}

void CVTablePanel::insertCol(const unsigned int& x)
{

    if(!x && cells.empty())
    {
        cells.emplace_back();
    }
    else if(x >= maxSize(cells)) throw std::invalid_argument("CVTablePanel: Column out of range of cell matrix");

    for(size_t i = 0; i < cells.size(); ++i)
    {
        cells[i].insert(cells[i].begin() + x, new CVTypeBox(View, sf::Vector2f(cells[i][x]->getPosition().x,
                        cells[i][x]->getPosition().y),
                        cellSize.x, cells[i][x]->getBounds().height,
                        textInfo, cellFillColor, cellOutlineColor,
                        cellOutlineWidth, CV_OBJ_ANIM_SLIDE,
                        CV_TEXT_FIT_WRAP, CV_TEXT_EXPAND_BOTTOM));
        if(!x) cells[i].front()->move(-cells[i].front()->getBounds().width, 0.0f);

//        cells[i][x]->setHighlightColor(cellHighlightColor);
//        cells[i][x]->setHighlightableStatus(bHighlightCells);
        addPanelElement(cells[i][x]);
    }

    if(x)
    {
        float moveDist = cells.front()[x]->getBounds().width + cellOutlineWidth;
        for(auto& row : cells)
        {
            for(size_t i = x + 1; i < row.size(); ++i)
            {
                row[i]->move(moveDist, 0.0f);
            }
        }
    }

}

void CVTablePanel::addCell(const std::string& text,
                           const unsigned int& x, const unsigned int& y)
{

    while(x >= minSize(cells))
    {
        if(maxSize(cells) > x)
        {
            expandRows(0);
        }
        else expandRows(x + 1 - maxSize(cells));
    }
    while(y >= cells.size())
    {
        expandCols(y + 1 - cells.size());
    }

    cells[y][x]->setTypeString(text);

}

void CVTablePanel::move(const sf::Vector2f& distance)
{
    CVBasicViewPanel::move(distance);
    scrollBarX.move(distance);
    scrollBarY.move(distance);
}

void CVTablePanel::move(const float& x, const float& y)
{
    move(sf::Vector2f(x, y));
}

void CVTablePanel::setPosition(const sf::Vector2f& newPosition)
{
    move(newPosition - getPosition());
}

void CVTablePanel::setPosition(const float& x, const float& y)
{
    setPosition(sf::Vector2f(x, y));
}

bool CVTablePanel::draw(sf::RenderTarget* target)
{
    if(!CVBasicViewPanel::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    scrollBarX.draw(target);
    scrollBarY.draw(target);

    CV_DRAW_CLIP_END

    return true;
}

bool CVTablePanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    sf::FloatRect itemBounds(left_margin(), top_margin(), 0.0f,0.0f);
    if(!cells.empty())
    {
        sf::FloatRect otherBounds;
        itemBounds = cells.front().front()->getBounds();
        otherBounds = cells[maxSizeIndex(cells)].back()->getBounds();
        if(otherBounds.left + otherBounds.width > itemBounds.left + itemBounds.width)
        {
            itemBounds.width = otherBounds.left + otherBounds.width - itemBounds.left;
        }
        otherBounds = cells.back().front()->getBounds();
        if(otherBounds.top + otherBounds.height > itemBounds.top + itemBounds.height)
        {
            itemBounds.height = otherBounds.top + otherBounds.height - itemBounds.top;
        }
    }

    scrollBarY.setScrollMax(itemBounds.height);
    scrollBarX.setScrollMax(itemBounds.width);

    if(bounds.contains(mousePos) && event.focusFree())
    {
        setFocus(true);
        scrollBarY.setScrollable(true);
        scrollBarX.setScrollable(true);
    }
    else
    {
        setFocus(false);
        scrollBarY.setScrollable(false);
        scrollBarX.setScrollable(false);
    }

    if(bounds.contains(mousePos)) event.captureFocus();

    scrollBarX.update(event, mousePos);
    scrollBarY.update(event, mousePos);

    if(hasFocus())
    {
        sf::Vector2f moveDist(0.0f, 0.0f);
        if(itemBounds.height > bounds.height - top_margin() - bottom_margin())
        {
            moveDist.y = top_margin() + bounds.top -
                         itemBounds.top -
                         scrollBarY.getScrollOffset();
        }
        else
        {
            moveDist.y = bounds.top + top_margin() - itemBounds.top;
        }

        if(itemBounds.width > bounds.width - left_margin() - right_margin())
        {
            moveDist.x = left_margin() + bounds.left -
                         itemBounds.left -
                         scrollBarX.getScrollOffset();
        }
        else
        {
            moveDist.x = bounds.left + left_margin() - itemBounds.left;
        }

        for(auto& row : cells)
        {
            for(auto& cell : row)
            {
                cell->move(moveDist);
            }
        }
    }

    return true;
}


}

