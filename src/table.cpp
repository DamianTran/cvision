#include "cvision/table.hpp"
#include "algorithm.hpp"

using namespace AIALG;

namespace cvis{

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
                                    cellFontSize(16*View->getViewScale()),
                                    headerFontSize(22*View->getViewScale()),
                                    cellOutlineWidth(1.0f),
                                    cellFillColor(bkgColor),
                                    cellOutlineColor(sf::Color(128,128,128)),
                                    margins({20.0f, 20.0f, 0.0f, 0.0f}),
                                    view_coords({0,0}){

    scrollBarX.setDirection(CV_WHEEL_DIR_HORIZONTAL);

    scrollBarX.setPanelSize(size.x - left_margin() - right_margin());
    scrollBarY.setPanelSize(size.y - top_margin() - bottom_margin());

    if(bkgColor == sf::Color::Transparent){
        cellHighlightColor = sf::Color(12,50,200,128);
    }
    else{
        cellHighlightColor = bkgColor;
        contrast(cellHighlightColor, bkgColor, 50);
    }

    textInfo.font = mainApp()->getDefaultFont();
    textInfo.alignment = ALIGN_LEFT;
    textInfo.fontSize = cellFontSize;
    textInfo.textColor = sf::Color::Black;

    setDrawClipping(true);

}

void CVTablePanel::setCellFillColor(const sf::Color& newColor){
    cellFillColor = newColor;
    for(auto& row : cells){
        for(auto& cell : row){
            cell->setFillColor(newColor);
        }
    }
}

void CVTablePanel::setCellOutlineColor(const sf::Color& newColor){
    cellOutlineColor = newColor;
    for(auto& row : cells){
        for(auto& cell : row){
            cell->setOutlineColor(newColor);
        }
    }
}

void CVTablePanel::setCellHighlightColor(const sf::Color& newColor){
    cellHighlightColor = newColor;
    for(auto& row : cells){
        for(auto& cell : row){
            cell->setHighlightColor(newColor);
        }
    }
}

void CVTablePanel::expandCols(const int& y){

    size_t x, nrow = cells.size();

    if(!cells.empty()) x = cells.back().size();
    else x = 0;

    for(size_t i = nrow; i < nrow + y; ++i){
        cells.emplace_back();
        if(cells.size() > 1){
            for(size_t j = 0; j < x; ++j){
                cells.back().emplace_back(new CVTextBox(View, sf::Vector2f(cells[nrow - 1][j]->getPosition().x + cellOutlineWidth,
                                                             cells[nrow - 1][j]->getPosition().y +
                                                             cells[nrow - 1][j]->getBounds().height),
                                          cells[nrow - 1][j]->getBounds().width - 2*cellOutlineWidth, cellSize.y,
                                          textInfo, cellFillColor, cellOutlineColor,
                                          cellOutlineWidth));
                addPanelElement(cells.back().back());
            }
        }
        else{
            sf::Vector2f newPos;
            for(size_t j = 0; j < x; ++j){
                if(j == 0) newPos.x = getPosition().x + left_margin() + cellOutlineWidth;
                else newPos.x = cells.back().back()->getPosition().x + cells.back().back()->getBounds().width + cellOutlineWidth;

                newPos.y = getPosition().y + top_margin();

                cells.back().emplace_back(new CVTextBox(View, newPos,
                                          cellSize.x, cellSize.y,
                                          textInfo, cellFillColor, cellOutlineColor,
                                          cellOutlineWidth));
                addPanelElement(cells.back().back());
            }
        }
    }
}

void CVTablePanel::expandRows(const int& x){

    if(cells.empty()){
        cells.emplace_back();
    }

    size_t y = cells.size(),
            ncol = minSize(cells);

    for(size_t i = ncol; i < ncol + x; ++i){
        for(size_t j = 0; j < y; ++j){
            if(i > 0){
                cells[j].emplace_back(new CVTextBox(View, sf::Vector2f(cells[j][i-1]->getPosition().x + cells[j][i-1]->getBounds().width,
                                                             cells[j][i-1]->getPosition().y + cellOutlineWidth),
                                          cellSize.x, cells[j][i-1]->getBounds().height - 2*cellOutlineWidth,
                                          textInfo, cellFillColor, cellOutlineColor,
                                          cellOutlineWidth));
            }
            else{
                cells[j].emplace_back(new CVTextBox(View, sf::Vector2f(getPosition().x + left_margin(),
                                                             getPosition().y + top_margin() + cellOutlineWidth + j*(cellSize.y)),
                                          cellSize.x, cellSize.y,
                                          textInfo, cellFillColor, cellOutlineColor,
                                          cellOutlineWidth));
            }

            addPanelElement(cells[j].back());
        }
    }
}

void CVTablePanel::removeRow(const unsigned int& y){
    if(y < cells.size()){

        for(size_t i = y + 1; i < cells.size(); ++i){
            for(size_t j = 0; j < cells[i].size(); ++j){
                cells[i][j]->move(0.0f, -cells[y].front()->getBounds().height);
            }
        }

        for(auto& cell : cells[y]){
            removePanelElement(cell);
        }

        cells.erase(cells.begin() + y);
    }
}

void CVTablePanel::removeCol(const unsigned int& x){
    for(auto& row : cells){
        if(x < row.size()){

            for(size_t i = x + 1; i < row.size(); ++i){
                row[i]->move(-row[x]->getBounds().width, 0.0f);
            }

            removePanelElement(row[x]);
            row.erase(row.begin() + x);
        }
    }
}

void CVTablePanel::insertRow(const unsigned int& y){

    if((y == 0) && cells.empty()){
        cells.emplace_back();
        return;
    }
    else if(y >= cells.size()) throw std::invalid_argument("CVTablePanel: Row out of range of cell matrix");

    size_t ncol = cells[y].size();

    cells.insert(cells.begin() + y, std::vector<CVTextBox*>());

    for(size_t i = 0; i < ncol; ++i){
        cells.back().emplace_back(new CVTextBox(View, sf::Vector2f(cells[y + 1][i]->getPosition().x + cellOutlineWidth,
                                                     cells[y + 1][i]->getPosition().y +
                                                     cells[y + 1][i]->getBounds().height),
                                  cells[y + 1][i]->getBounds().width - 2*cellOutlineWidth, cellSize.y,
                                  textInfo, cellFillColor, cellOutlineColor,
                                  cellOutlineWidth));
    }

    float moveDist = cells[y].front()->getBounds().height;

    if(y == 0){
        for(auto& cell : cells[y]){
            cell->move(0.0f, -moveDist);
        }
    }
    else{
        for(size_t i = y + 1; i < cells.size(); ++i){
            for(auto& cell : cells[i]){
                cell->move(0.0f, moveDist);
            }
        }
    }

}

void CVTablePanel::insertCol(const unsigned int& x){

    if((x == 0) && cells.empty()){
        cells.emplace_back();
    }
    else if(x >= maxSize(cells)) throw std::invalid_argument("CVTablePanel: Column out of range of cell matrix");

    for(size_t i = 0; i < cells.size(); ++i){
        cells[i].insert(cells[i].begin() + x, new CVTextBox(View, sf::Vector2f(cells[i][x]->getPosition().x,
                                                             cells[i][x]->getPosition().y + cellOutlineWidth),
                                          cellSize.x, cells[i][x]->getBounds().height - 2*cellOutlineWidth,
                                          textInfo, cellFillColor, cellOutlineColor,
                                          cellOutlineWidth));
        if(x == 0) cells[i].front()->move(-cells[i].front()->getBounds().width, 0.0f);
    }

    if(x != 0){
        float moveDist = cells.front()[x]->getBounds().width;
        for(auto& row : cells){
            for(size_t i = x + 1; i < row.size(); ++i){
                row[i]->move(moveDist, 0.0f);
            }
        }
    }

}

void CVTablePanel::addCell(const std::string& text,
                           const unsigned int& x, const unsigned int& y){

    if(x >= minSize(cells)){
        expandRows(x + 1 - minSize(cells));
    }
    if(y >= cells.size()){
        expandCols(y + 1 - cells.size());
    }

    cells[y][x]->setString(text);
}

void CVTablePanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
    scrollBarX.move(distance);
    scrollBarY.move(distance);
}

void CVTablePanel::move(const float& x, const float& y){
    move(sf::Vector2f(x, y));
}

void CVTablePanel::setPosition(const sf::Vector2f& newPosition){
    move(newPosition - getPosition());
}

void CVTablePanel::setPosition(const float& x, const float& y){
    setPosition(sf::Vector2f(x, y));
}

bool CVTablePanel::draw(sf::RenderTarget* target){
    if(!CVBasicViewPanel::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    scrollBarX.draw(target);
    scrollBarY.draw(target);

    CV_DRAW_CLIP_END

    return true;
}

bool CVTablePanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    sf::FloatRect itemBounds(left_margin(), top_margin(), 0.0f,0.0f);
    if(!cells.empty()){
        sf::FloatRect otherBounds;
        itemBounds = cells.front().front()->getBounds();
        otherBounds = cells[maxSizeIndex(cells)].back()->getBounds();
        if(otherBounds.left + otherBounds.width > itemBounds.left + itemBounds.width){
            itemBounds.width = otherBounds.left + otherBounds.width - itemBounds.left;
        }
        otherBounds = cells.back().front()->getBounds();
        if(otherBounds.top + otherBounds.height > itemBounds.top + itemBounds.height){
            itemBounds.height = otherBounds.top + otherBounds.height - itemBounds.top;
        }
    }

    scrollBarY.setScrollMax(itemBounds.height);
    scrollBarX.setScrollMax(itemBounds.width);

    if(bounds.contains(mousePos) && event.focusFree()){
        setFocus(true);
        scrollBarY.setScrollable(true);
        scrollBarX.setScrollable(true);
    }
    else{
        setFocus(false);
        scrollBarY.setScrollable(false);
        scrollBarX.setScrollable(false);
    }

    if(bounds.contains(mousePos)) event.captureFocus();

    scrollBarX.update(event, mousePos);
    scrollBarY.update(event, mousePos);

    if(hasFocus()){
        sf::Vector2f moveDist(0.0f, 0.0f);
        if(itemBounds.height > bounds.height - top_margin() - bottom_margin()){
            moveDist.y = top_margin() + bounds.top -
                                       itemBounds.top -
                                       scrollBarY.getScrollOffset();
        }
        else{
            moveDist.y = bounds.top + top_margin() - itemBounds.top;
        }

        if(itemBounds.width > bounds.width - left_margin() - right_margin()){
            moveDist.x = left_margin() + bounds.left -
                                       itemBounds.left -
                                       scrollBarX.getScrollOffset();
        }
        else{
            moveDist.x = bounds.left + left_margin() - itemBounds.left;
        }

        for(auto& row : cells){
            for(auto& cell : row){
                cell->move(moveDist);
            }
        }
    }

    return true;
}


}

