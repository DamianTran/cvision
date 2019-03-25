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

#include "cvision/dataview.hpp"
#include "cvision/coordmap.hpp"
#include "cvision/algorithm.hpp"

#include "EZC/algorithm.hpp"
#include "EZC/toolkit/string.hpp"

#include "AIDA/dataStream.hpp"

using namespace EZC;

namespace cvis
{

bool CVDataViewerPanel::updateCells()
{

    sf::FloatRect tl = cells.front().front()->getBounds(),
                  br = cells.back().back()->getBounds();

    int line_coord,
        extend_coord,
        maxWidth;

    bool bDisrupt = false;

    // Fill left side

    while(tl.left > bounds.left - tl.width)
    {

        if(!coordOffset.x) break;

        tableOffset.x -= (cells.front().front()->getBounds().width);
        --coordOffset.x;

        insertCol(0);

        for(size_t y = 0; y < cells.size(); ++y)
        {
            if((y + coordOffset.y < dataSize.y) &&
                    (coordOffset.x < rowSize(y + coordOffset.y)))
            {
                addCell(getData(coordOffset.x, y + coordOffset.y), 0, y);

                try{

                    cells[y][0]->setTypeString(modifications.at(sf::Vector2i(coordOffset.x, y + coordOffset.y)));

                }catch(...){ }

            }
        }

        tl = cells.front().front()->getBounds();

        headerBars.insert(headerBars.begin(), new CVTextBox(View, sf::Vector2f(headerBars.front()->getPosition().x - tl.width, bounds.top),
                          tl.width, top_margin(),
                          TextEntry(std::to_string(coordOffset.x + 1), textInfo.font,
                                    15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                    headerThemeColor - 50),
                          headerThemeColor, headerThemeColor - 40, 1.0f));
        headerBars.front()->setMask(appTexture("gradient_linear"));
        headerBars.front()->setMaskColor(sf::Color(250,250,250,160));
        addPanelElement(headerBars.front());

        bDisrupt = true;

    }

    // Fill right side

    while(br.left - br.width < bounds.left + bounds.width)
    {

        if(coordOffset.x + EZC::maxSize(cells) >= dataSize.x) break;

        extend_coord = EZC::maxSize(cells);

        for(size_t y = 0; y < cells.size(); ++y)
        {

            if(y + coordOffset.y < dataSize.y)
            {
                if(extend_coord + coordOffset.x >= rowSize(y + coordOffset.y))
                {

                    addCell("", extend_coord, y);

                }
                else
                {

                    addCell(getData(extend_coord + coordOffset.x,
                                    y + coordOffset.y), extend_coord, y);

                }
            }

            try{

                cells[y][extend_coord]->setTypeString(modifications.at(sf::Vector2i(extend_coord + coordOffset.x,
                                                                         y + coordOffset.y)));

            }catch(...){ }
        }

        if(cells.back().back()->getBounds().left == br.left) break;
        br = cells.back().back()->getBounds();

        headerBars.emplace_back(new CVTextBox(View, sf::Vector2f(headerBars.back()->getPosition().x + headerBars.back()->getSize().x, bounds.top),
                                              br.width, top_margin(),
                                              TextEntry(std::to_string(cells.front().size() + coordOffset.x), textInfo.font,
                                                      15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                                      headerThemeColor - 50),
                                              headerThemeColor, headerThemeColor - 40, 1.0f));
        headerBars.back()->setMask(appTexture("gradient_linear"));
        headerBars.back()->setMaskColor(sf::Color(250,250,250,160));
        addPanelElement(headerBars.back());

        bDisrupt = true;
    }

    // Fill bottom

    while(br.top - br.height < bounds.top + bounds.height)
    {

        if(cells.size() + coordOffset.y >= dataSize.y) break;

        extend_coord = cells.size();
        line_coord = coordOffset.y + cells.size();
        maxWidth = maxSize(cells);

        for(size_t x = 0; x < maxWidth; ++x)
        {

            if(coordOffset.x + x < rowSize(line_coord))
            {

                addCell(getData(coordOffset.x + x, line_coord), x, extend_coord);

            }
            else addCell("", x, extend_coord);

            try{

                cells[extend_coord][x]->setTypeString(modifications.at(sf::Vector2i(x + coordOffset.x,
                                                                         line_coord)));

            }catch(...){ }

        }

        if(cells.back().back()->getBounds().top == br.top) break;

        br = cells.back().back()->getBounds();

        rowBars.emplace_back(new CVTextBox(View, sf::Vector2f(bounds.left, br.top),
                                           left_margin(), br.height,
                                           TextEntry(std::to_string(line_coord), textInfo.font,
                                                   15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                                   headerThemeColor - 70),
                                           headerThemeColor, headerThemeColor - 40, 1.0f));
        rowBars.back()->setMask(appTexture("gradient_linear"));
        rowBars.back()->setMaskColor(sf::Color(250,250,250,160));
        addPanelElement(rowBars.back());

        bDisrupt = true;
    }

    // Fill top

    while(tl.top > bounds.top - tl.height)
    {

        if(!coordOffset.y) break;

        tableOffset.y -= (cells.front().front()->getBounds().height +
                          (tableOffset.y - cellSize.y * coordOffset.y)/coordOffset.y);
        --coordOffset.y;

        if(!coordOffset.y)
        {
            tableOffset.y = 0.0f;
            sizeOffset.y = 0.0f;
        }

        insertRow(0);
        extend_coord = EZC::maxSize(cells);

        for(size_t x = 0; x < extend_coord; ++x)
        {
            if(x + coordOffset.x < rowSize(coordOffset.y))
            {

                addCell(getData(x + coordOffset.x, coordOffset.y), x, 0);

            }

            try{

                cells.front()[x]->setTypeString(modifications.at(sf::Vector2i(x + coordOffset.x,
                                                                         coordOffset.y)));

            }catch(...){ }
        }

        tl = cells.front().front()->getBounds();

        rowBars.insert(rowBars.begin(), new CVTextBox(View, sf::Vector2f(bounds.left, tl.top),
                       left_margin(), tl.height,
                       TextEntry(std::to_string(coordOffset.y + 1), textInfo.font,
                                 15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                 headerThemeColor - 70),
                       headerThemeColor, headerThemeColor - 40, 1.0f));
        rowBars.front()->setMask(appTexture("gradient_linear"));
        rowBars.front()->setMaskColor(sf::Color(250,250,250,160));
        addPanelElement(rowBars.front());

        bDisrupt = true;

    }

    for(size_t x = 0; x < cells.front().size();)
    {

        if((cells.front()[x]->getPosition().x + cells.front()[x]->getSize().x < exclusionBounds.left) ||
                (cells.front()[x]->getPosition().x > exclusionBounds.left + exclusionBounds.width))
        {

            if(cells.front()[x]->getGlobalBounds().left < bounds.left)
            {
                tableOffset.x += cells.front()[x]->getGlobalBounds().width;
                ++coordOffset.x;
            }

            for(auto& row : cells)
            {

                removePanelElement(row[x]);
                row.erase(row.begin() + x);

            }

            removePanelElement(headerBars[x]);
            headerBars.erase(headerBars.begin() + x);

            bDisrupt = true;

        }
        else ++x;

    }

    for(size_t y = 0; y < cells.size();)
    {

        if((cells[y].front()->getPosition().y + cells[y].front()->getSize().y < exclusionBounds.top) ||
                (cells[y].front()->getPosition().y > exclusionBounds.top + exclusionBounds.height))
        {

            if(cells[y].front()->getGlobalBounds().top < bounds.top)
            {
                tableOffset.y += cells[y].front()->getGlobalBounds().height;
                ++coordOffset.y;
            }
            else
            {
                sizeOffset.y -= (cells[y].front()->getGlobalBounds().height - cellSize.y);
            }

            for(auto& cell : cells[y])
            {

                removePanelElement(cell);

            }

            cells.erase(cells.begin() + y);

            removePanelElement(rowBars[y]);
            rowBars.erase(rowBars.begin() + y);

            bDisrupt = true;

        }
        else ++y;

    }

    updateExpand();

    scrollBarY.setScrollMax(dataSize.y * cellSize.y + sizeOffset.y);
    scrollBarX.setScrollMax(dataSize.x * cellSize.x + sizeOffset.x);

    return bDisrupt;

}

void CVDataViewerPanel::applyModifications()
{

    if(!cells.empty()){
        for(size_t y = 0, x; y < cells.size(); ++y)
        {
            for(x = 0; x < cells[y].size(); ++x)
            {
                for(auto& pair : modifications)
                {
                    if((pair.first.x == x + coordOffset.x) &&
                       (pair.first.y == y + coordOffset.y))
                    {
                        cells[y][x]->setTypeString(pair.second);
                    }
                }
            }
        }
    }

}

void CVDataViewerPanel::getDataSize()
{

    if(!dataSource)
    {
        dataSize.x = 0;
        dataSize.y = 0;
    }
    else
    {
        dataSize.x = dataSource->ncol();
        dataSize.y = dataSource->nrow();
    }

}

std::string CVDataViewerPanel::getData(const int& x, const int& y) const
{
    if(dataSource)
    {
        return dataSource->getString(x, y);
    }

    return std::string();
}

size_t CVDataViewerPanel::rowSize(const int& y) const
{
    return dataSource->rowSize(y);
}

void CVDataViewerPanel::setup()
{

    if(!dataSource) return;

    clear();

    exclusionBounds = bounds;

    getDataSize();

    scrollBarY.setScrollMax(dataSize.y*cellSize.y);
    scrollBarX.setScrollMax(dataSize.x*cellSize.x);

    exclusionBounds.left -= 8*cellSize.x;
    exclusionBounds.width += 16*cellSize.x;
    exclusionBounds.top -= 8*cellSize.y;
    exclusionBounds.height += 16*cellSize.x;

    cornerBlock.setSize(sf::Vector2f(left_margin(), top_margin()));
    cornerBlock.setPosition(sf::Vector2f(bounds.left, bounds.top));
    cornerBlock.setFillColor(setAlpha(headerThemeColor - 10, 255));
    cornerBlock.setOutlineColor(setAlpha(headerThemeColor - 40, 255));
    cornerBlock.setOutlineThickness(cellOutlineWidth);

    for(size_t x = 0; x < dataSize.x; ++x)
    {
        for(size_t y = 0; y < dataSize.y; ++y)
        {

            if(x < rowSize(y))
            {
                addCell(getData(x, y), x, y);
            }
            else
            {
                addCell("", x, y);
            }

            if(!x)
            {

                rowBars.emplace_back(new CVTextBox(View, sf::Vector2f(bounds.left, cells[y][x]->getPosition().y),
                                                   left_margin(), cells[y][x]->getSize().y,
                                                   TextEntry(std::to_string(y + 1), textInfo.font,
                                                           15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                                           headerThemeColor - 50),
                                                   headerThemeColor, headerThemeColor - 40, 1.0f));
                rowBars.back()->setMask(appTexture("gradient_linear"));
                rowBars.back()->setMaskColor(sf::Color(250,250,250,160));
                addPanelElement(rowBars.back());

            }

            if(cells[y][x]->getBounds().top >
                    bounds.top + bounds.height +
                    3*cells.front()[x]->getBounds().height) break;

        }

        headerBars.emplace_back(new CVTextBox(View, sf::Vector2f(cells.front()[x]->getPosition().x, bounds.top),
                                              cells.front()[x]->getSize().x, top_margin(),
                                              TextEntry(std::to_string(x + 1), textInfo.font,
                                                      15*View->getViewScale(), ALIGN_CENTER_MIDLINE,
                                                      headerThemeColor - 50),
                                              headerThemeColor, headerThemeColor - 40, 1.0f));
        headerBars.back()->setMask(appTexture("gradient_linear"));
        headerBars.back()->setMaskColor(sf::Color(250,250,250,160));
        addPanelElement(headerBars.back());

        if(cells.front()[x]->getBounds().left >
                bounds.left + bounds.width +
                3*cells.front()[x]->getBounds().width) break;

    }

}

bool CVDataViewerPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(bNoInteract || !View->viewPort) return false;

    std::cout << "Updating...\n";

    if(bDisconnect)
    {

        clear();
        dataSource = nullptr;

    }
    if(incomingDataSource)
    {

        dataSource = incomingDataSource;
        incomingDataSource = nullptr;

        setup();

    }

    std::cout << "Check...\n";

    // Capture keys

    if(hasFocus() && event.keyPressed)
    {
        for(auto& key : event.keyLog)
        {
            switch(key)
            {
            case 'v':
                {
                    if(ctrlPressed())
                    {
                        pasteFromClipboard();
                    }

                    event.clearKeys();

                    break;
                }
            case 'c':
                {
                    if(ctrlPressed())
                    {
                        copySelected();
                    }

                    event.clearKeys();

                    break;
                }
            default:
                {
                    break;
                }
            }
        }
    }

    if(!CVBasicViewPanel::update(event, mousePos) || !dataSource) return false;

    if(!cells.empty())
    {

        if(updateCells())
        {

            for(size_t y = 0, x; y < cells.size(); ++y)
            {

                for(x = 0; x < cells[y].size(); ++x)
                {

                    for(auto& coord : selectedCoords)
                    {

                        if((coord.x == x + coordOffset.x) &&
                                (coord.y == y + coordOffset.y))
                        {

                            cells[y][x]->setFillColor(cellHighlightColor);

                            rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));
                            headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                            goto next1;

                        }

                    }

                    if((!selectedColumns.empty() && !selectedRows.empty()))
                    {

                        if(anyEqual(x + coordOffset.x, selectedColumns) &&
                                anyEqual(y + coordOffset.y, selectedRows))
                        {

                            cells[y][x]->setFillColor(cellHighlightColor);

                            rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));
                            headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                        }

                    }
                    else if(anyEqual(x + coordOffset.x, selectedColumns))
                    {

                        cells[y][x]->setFillColor(cellHighlightColor);
                        headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                    }
                    else if(anyEqual(y + coordOffset.y, selectedRows))
                    {

                        cells[y][x]->setFillColor(cellHighlightColor);
                        rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                    }
                    else
                    {

                        cells[y][x]->setFillColor(cellFillColor);

                    }

                    next1:;

                }

            }

        }

    }

    totalBounds.top = 0.0f;
    totalBounds.left = 0.0f;
    totalBounds.width = dataSize.x*cellSize.x;
    totalBounds.height = dataSize.y*cellSize.y;

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

    std::cout << "Check...\n";

    if(bounds.contains(mousePos)) event.captureFocus();

    scrollBarX.update(event, mousePos);
    scrollBarY.update(event, mousePos);

    if(!cells.empty())
    {

        sf::FloatRect tl = cells.front().front()->getBounds(),
                      br = cells.back().back()->getBounds();

        sf::Vector2f moveDist(0.0f, 0.0f);
        if(totalBounds.height > bounds.height - top_margin() - bottom_margin())
        {
            moveDist.y = top_margin() + bounds.top -
                         tl.top + tableOffset.y -
                         (scrollBarY.getScrollOffset());
        }
        else
        {
            moveDist.y = bounds.top + top_margin() - tl.top + tableOffset.y;
        }

        if(totalBounds.width > bounds.width - left_margin() - right_margin())
        {
            moveDist.x = left_margin() + bounds.left -
                         tl.left + tableOffset.x -
                         (scrollBarX.getScrollOffset());
        }
        else
        {
            moveDist.x = bounds.left + left_margin() - tl.left + tableOffset.x;
        }

        for(auto& row : cells)
        {
            for(auto& cell : row)
            {
                cell->move(moveDist);
            }
        }

        for(auto& bar : headerBars)
        {
            bar->move(moveDist.x, 0.0f);
        }

        for(auto& bar : rowBars)
        {
            bar->move(0.0f, moveDist.y);
        }

        // Handle selection

        if(hasFocus())
        {
            if(event.LMBhold &&
            bounds.contains(event.LMBpressPosition) &&
            (event.LMBholdFrames > 5) &&
            (mousePos != event.LMBpressPosition))
            {

                if((event.LMBpressPosition.x > bounds.left + left_margin()) &&
                        (event.LMBpressPosition.y > bounds.top + top_margin()))
                {

                    sf::FloatRect cordonBounds;

                    for(auto& row : cells)
                    {

                        for(auto& cell : row)
                        {

                            if(cell->getBounds().contains(mousePos) || cell->getBounds().contains(event.LMBpressPosition))
                            {

                                if(!cordonBounds.width && !cordonBounds.height)
                                {

                                    cordonBounds = cell->getBounds();

                                }
                                else
                                {

                                    expandBounds(cordonBounds, cell->getBounds());

                                }

                            }

                        }

                    }

                    cordon.setPosition(sf::Vector2f(cordonBounds.left, cordonBounds.top));
                    cordon.setSize(sf::Vector2f(cordonBounds.width, cordonBounds.height));

                }

            }
            else if(event.LMBreleased && (event.LMBreleaseFrames == 1))
            {

                if(!ctrlPressed())
                {
                    selectedCoords.clear();
                }

                for(auto& bar : rowBars)
                {

                    bar->setFillColor(headerThemeColor);

                }

                for(auto& bar : headerBars)
                {

                    bar->setFillColor(headerThemeColor);

                }

                if(event.LMBpressPosition.y < bounds.top + top_margin())
                {

                    for(size_t x = 0; x < headerBars.size(); ++x)
                    {

                        if(headerBars[x]->getBounds().contains(event.LMBpressPosition) ||
                                headerBars[x]->getBounds().contains(mousePos) ||
                                ((headerBars[x]->getPosition().x >
                                  (event.LMBpressPosition.x < mousePos.x ?
                                   event.LMBpressPosition.x : mousePos.x)) &&
                                 (headerBars[x]->getPosition().x + headerBars[x]->getSize().x <
                                  (event.LMBpressPosition.x > mousePos.x ?
                                   event.LMBpressPosition.x : mousePos.x))))
                        {

                            if(shiftPressed() && !selectedColumns.empty())
                            {

                                selectedColumns.clear();
                                selectedColumns.emplace_back(coordAnchor.x);

                                while(x + coordOffset.x > selectedColumns.back())
                                {

                                    selectedColumns.emplace_back(selectedColumns.back() + 1);

                                }
                                while(x + coordOffset.x < selectedColumns.back())
                                {

                                    selectedColumns.emplace_back(selectedColumns.back() - 1);

                                }

                            }
                            else
                            {

                                selectedColumns.clear();
                                selectedColumns.emplace_back(x + coordOffset.x);
                                coordAnchor.x = x + coordOffset.x;

                            }

                        }

                    }

                }
                else if(!ctrlPressed())
                {

                    selectedColumns.clear();

                }

                if(event.LMBpressPosition.x < bounds.left + left_margin())
                {

                    for(size_t y = 0; y < rowBars.size(); ++y)
                    {

                        if(rowBars[y]->getBounds().contains(event.LMBpressPosition) ||
                                rowBars[y]->getBounds().contains(mousePos) ||
                                ((rowBars[y]->getPosition().y >
                                  (event.LMBpressPosition.y < mousePos.y ?
                                   event.LMBpressPosition.y : mousePos.y)) &&
                                 (rowBars[y]->getPosition().y + rowBars[y]->getSize().y <
                                  (event.LMBpressPosition.y > mousePos.y ?
                                   event.LMBpressPosition.y : mousePos.y))))
                        {

                            if(shiftPressed() && !selectedRows.empty())
                            {

                                if(!ctrlPressed()) selectedRows.clear();
                                selectedRows.emplace_back(coordAnchor.y);

                                while(y + coordOffset.y > selectedRows.back())
                                {

                                    selectedRows.emplace_back(selectedRows.back() + 1);

                                }
                                while(y + coordOffset.y < selectedRows.back())
                                {

                                    selectedRows.emplace_back(selectedRows.back() - 1);

                                }

                            }
                            else
                            {

                                if(!ctrlPressed()) selectedRows.clear();
                                selectedRows.emplace_back(y + coordOffset.y);
                                coordAnchor.y = y + coordOffset.y;


                            }

                        }

                    }

                }
                else if(!ctrlPressed())
                {

                    selectedRows.clear();

                }

                for(size_t y = 0, x; y < cells.size(); ++y)
                {

                    for(x = 0; x < cells[y].size(); ++x)
                    {

                        if((!cordon.getSize().x && !cordon.getSize().y && cells[y][x]->getBounds().contains(mousePos)) ||
                                contains(cells[y][x]->getBounds(), cordon.getGlobalBounds()))
                        {

                            if(shiftPressed())
                            {

                                int beginX = coordAnchor.x < coordOffset.x + x ? coordAnchor.x : coordOffset.x + x,
                                    endX = coordAnchor.x > coordOffset.x + x ? coordAnchor.x : coordOffset.x + x,
                                    beginY = coordAnchor.y < coordOffset.y + y ? coordAnchor.y : coordOffset.y + y,
                                    endY = coordAnchor.y > coordOffset.y + y ? coordAnchor.y : coordOffset.y + y;

                                for(int i = beginY, j; i <= endY; ++i)
                                {
                                    for(j = beginX; j <= endX; ++j)
                                    {

                                        selectedCoords.emplace_back(j, i);

                                    }
                                }

                            }
                            else
                            {

                                selectedCoords.emplace_back(coordOffset.x + x,
                                                            coordOffset.y + y);
                                coordAnchor = selectedCoords.back();

                            }

                        }

                    }
                }

                for(size_t y = 0, x; y < cells.size(); ++y)
                {

                    for(x = 0; x < cells[y].size(); ++x)
                    {

                        for(auto& coord : selectedCoords)
                        {

                            if((coord.x == x + coordOffset.x) &&
                                    (coord.y == y + coordOffset.y))
                            {

                                cells[y][x]->setFillColor(cellHighlightColor);

                                rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));
                                headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                                goto next2;

                            }

                        }

                        if((!selectedColumns.empty() && !selectedRows.empty()))
                        {

                            if(anyEqual(x + coordOffset.x, selectedColumns) &&
                                    anyEqual(y + coordOffset.y, selectedRows))
                            {

                                cells[y][x]->setFillColor(cellHighlightColor);

                                rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));
                                headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                            }

                        }
                        else if(anyEqual(x + coordOffset.x, selectedColumns))
                        {

                            cells[y][x]->setFillColor(cellHighlightColor);
                            headerBars[x]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                        }
                        else if(anyEqual(y + coordOffset.y, selectedRows))
                        {

                            cells[y][x]->setFillColor(cellHighlightColor);
                            rowBars[y]->setFillColor(setAlpha(headerThemeColor * cellHighlightColor, headerThemeColor.a));

                        }
                        else
                        {

                            cells[y][x]->setFillColor(cellFillColor);

                        }

                        next2:;

                    }

                }

                cordon.setSize(sf::Vector2f(0.0f,0.0f));

            }

            if(event.keyPressed)
            {
                for(size_t y = 0, x; y < cells.size(); ++y)
                {
                    if(y + coordOffset.y < dataSize.y)
                    {
                        for(x = 0; x < cells[y].size(); ++x)
                        {
                            if(((x + coordOffset.x < rowSize(y + coordOffset.y)) &&
                                    (cells[y][x]->getTypeString() != getData(x + coordOffset.x, y + coordOffset.y))) ||
                               ((x + coordOffset.x > rowSize(y + coordOffset.y)) &&
                                !cells[y][x]->getTypeString().empty()))
                            {

                                modifications[sf::Vector2i(x + coordOffset.x, y + coordOffset.y)] = cells[y][x]->getTypeString();

                            }
                            else
                            {
                                modifications.erase(sf::Vector2i(x + coordOffset.x, y + coordOffset.y));
                            }

                        }

                    }
                    else
                    {
                        for(x = 0; x < cells[y].size(); ++x)
                        {
                            if(!cells[y][x]->getTypeString().empty())
                            {
                                modifications[sf::Vector2i(x + coordOffset.x, y + coordOffset.y)] = cells[y][x]->getTypeString();
                            }
                            else
                            {
                                modifications.erase(sf::Vector2i(x + coordOffset.x, y + coordOffset.y));
                            }
                        }
                    }

                }
            }
        }
    }

    std::cout << "Done...\n";

    return true;

}

bool CVDataViewerPanel::copySelected()
{

    if(selectedCoords.empty() && selectedRows.empty() && selectedColumns.empty()) return false;

    std::stringstream output;

    sf::Vector2i minCoords,
                maxCoords;

    if(!selectedCoords.empty())
    {
        minCoords = selectedCoords.front();
        maxCoords = selectedCoords.front();

        for(auto& coord : selectedCoords)
        {
            if(coord.x < minCoords.x)
            {
                minCoords.x = coord.x;
            }

            if(coord.y < minCoords.y)
            {
                minCoords.y = coord.y;
            }

            if(coord.x > maxCoords.x)
            {
                maxCoords.x = coord.x;
            }

            if(coord.y > maxCoords.y)
            {
                maxCoords.y = coord.y;
            }
        }

    }

    if(!selectedRows.empty())
    {

        if(selectedCoords.empty())
        {
            minCoords.y = selectedRows.front();
            maxCoords.y = selectedRows.front();
        }

        minCoords.x = 0;

        for(auto& index : selectedRows)
        {
            if(index < minCoords.y)
            {
                minCoords.y = index;
            }

            if(index > maxCoords.y)
            {
                maxCoords.y = index;
            }

            if(rowSize(index) > maxCoords.x)
            {
                maxCoords.x = rowSize(index);
            }
        }
    }
    if(!selectedColumns.empty())
    {

        if(selectedCoords.empty())
        {
            minCoords.x = selectedRows.front();
            maxCoords.x = selectedRows.front();
        }

        minCoords.y = 0;
        maxCoords.y = dataSize.y;

        for(auto& index : selectedColumns)
        {
            if(index < minCoords.x)
            {
                minCoords.x = index;
            }

            if(index > maxCoords.x)
            {
                maxCoords.x = index;
            }
        }
    }

    for(size_t y = minCoords.y, x; y <= maxCoords.y; ++y)
    {
        for(x = minCoords.x; x <= maxCoords.x; ++x)
        {

            if(!selectedCoords.empty())
            {
                for(auto& coord : selectedCoords)
                {
                    if((coord.x == x) &&
                       (coord.y == y))
                    {

                        for(auto& pair : modifications)
                        {
                            if((pair.first.x == x) &&
                               (pair.first.y == y))
                            {
                                output << pair.second;
                                goto next;
                            }
                        }

                        if((y < dataSize.y) &&
                           (x < rowSize(y)))
                        {
                            output << getData(x, y);
                            goto next;
                        }

                    }
                }
            }

            if(!selectedRows.empty() && !selectedColumns.empty())
            {

                if((anyEqual(x, selectedColumns) &&
                   anyEqual(y, selectedRows)) &&
                   ((y < dataSize.y) &&
                   (x < rowSize(y))))
                {

                    for(auto& pair : modifications)
                    {
                        if((pair.first.x == x) &&
                           (pair.first.y == y))
                        {
                            output << pair.second;
                            goto next;
                        }
                    }

                    output << getData(x, y);
                }

            }
            else if(!selectedRows.empty())
            {
                if(anyEqual(y, selectedRows) &&
                   ((y < dataSize.y) &&
                   (x < rowSize(y))))
                {

                    for(auto& pair : modifications)
                    {
                        if((pair.first.x == x) &&
                           (pair.first.y == y))
                        {
                            output << pair.second;
                            goto next;
                        }
                    }

                    output << getData(x, y);
                }

            }
            else if(!selectedColumns.empty())
            {

                if(anyEqual(x, selectedColumns) &&
                   ((y < dataSize.y) &&
                   (x < rowSize(y))))
                {

                    for(auto& pair : modifications)
                    {
                        if((pair.first.x == x) &&
                           (pair.first.y == y))
                        {
                            output << pair.second;
                            goto next;
                        }
                    }

                    output << getData(x, y);
                }
            }

            next:;

            if(x < maxCoords.x)
            {
                output << '\t';
            }

        }

        if(y < maxCoords.y)
        {
            output << '\n';
        }
    }

    copyToClipboard(output.str());

    return true;

}

bool CVDataViewerPanel::pasteFromClipboard()
{

    if(selectedCoords.empty())
    {
        return false;
    }

    sf::Vector2i minCoords = selectedCoords.front();

    for(auto& coord : selectedCoords)
    {
        if(coord.x < minCoords.x)
        {
            minCoords.x = coord.x;
        }

        if(coord.y < minCoords.y)
        {
            minCoords.y = coord.y;
        }
    }

    std::stringstream clipStream;
    clipStream << getClipboardText();

    if(clipStream.str().empty()) return false;

    StringMatrix import;

    std::string newLine, newElement;
    std::stringstream lineStream;

    while(std::getline(clipStream, newLine))
    {

        lineStream.str(newLine);

        import.emplace_back();

        splitString(lineStream.str(), import.back(), "\t");

    }

    for(size_t y = 0, x; y < import.size(); ++y)
    {
        for(x = 0; x < import[y].size(); ++x)
        {

            modifications[sf::Vector2i(x + minCoords.x,
                                       y + minCoords.y)] = import[y][x];
        }
    }

    applyModifications();

    return true;

}

bool CVDataViewerPanel::saveToFile(const std::string& file) const
{

    std::ofstream outFILE(file, std::ios::out | std::ios::binary);

    if(!outFILE)
    {
        std::cout << "Error opening file \"" << file << "\"\n";
        return false;
    }

    size_t num_rows = dataSize.y;

    char delim;

    if(file.find(".csv") < file.size())
    {
        delim = ';';
    }
    else
    {
        delim = '\t';
    }

    for(size_t y = 0, x, xL; y < dataSize.y; ++y)
    {

        xL = rowSize(y);

        for(x = 0; x < dataSize.x; ++x)
        {
            for(auto& pair : modifications)
            {
                if((pair.first.x == x) &&
                   (pair.first.y == y))
                {
                    outFILE << pair.second;
                    goto next;
                }
            }

            if((y < num_rows) && (x < xL))
            {
                outFILE << dataSource->getString(x, y);
            }

            next:;

            if(x < dataSize.x - 1)
            {
                outFILE << delim;
            }
        }

        if(y < dataSize.y - 1)
        {
            outFILE << '\n';
        }
    }

    return true;

}

}
