#pragma once

#ifndef CVIS_DATA
#define CVIS_DATA

#include "cvision/table.hpp"
#include "algorithm.hpp"

namespace cvis{

template<typename T> size_t nrow(const std::vector<std::vector<T>>& M){
    return M.size();
}

template<typename T> size_t ncol(const std::vector<std::vector<T>>& M){
    return AIALG::maxSize(M);
}

template<typename T> size_t rowSize(const std::vector<std::vector<T>>& M, const unsigned int& index){
    return M[index].size();
}

template<class T>
class CVDataViewerPanel: public CVTablePanel{
protected:

    T* dataSource; // Requires a double-indexable item with nrow() and ncol() overload
    std::vector<std::vector<sf::Vector2u>> coords;
    sf::FloatRect totalBounds;

    sf::Vector2f frameAnchor;

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

    inline void set_dim(const unsigned int& dimension,
                        const unsigned int& value){ view_coords[dimension] = value; }

    inline void set_coords(const unsigned int& x, const unsigned int& y,
                           const sf::Vector2u& coords){

        if(this->coords.size() <= y){
            this->coords.resize(y + 1);
        }
        for(auto& row : this->coords){
            if(row.size() <= x) row.resize(x + 1, sf::Vector2u(UINT_MAX, UINT_MAX));
        }

        this->coords[y][x] = coords;
    }

    inline void removeCol(const unsigned int& x){
        CVTablePanel::removeCol(x);
        for(auto& row : coords){
            if(x < row.size()) row.erase(row.begin() + x);
        }
    }

    inline void removeRow(const unsigned int& y){
        CVTablePanel::removeRow(y);
        if(y < coords.size()) coords.erase(coords.begin() + y);
    }

    inline void insertRow(const unsigned int& y){
        CVTablePanel::insertRow(y);
        coords.insert(coords.begin() + y, std::vector<sf::Vector2u>(coords[y].size(), sf::Vector2u(UINT_MAX, UINT_MAX)));
    }

    inline void insertCol(const unsigned int& x){
        CVTablePanel::insertCol(x);
        for(auto& row : coords){
            row.insert(row.begin() + x, sf::Vector2u(UINT_MAX, UINT_MAX));
        }
    }

    bool update(CVEvent& event, const sf::Vector2f& mousePos){

        if(!CVBasicViewPanel::update(event, mousePos)) return false;

        sf::FloatRect tl = cells.front().front()->getBounds(),
                            br = cells.back().back()->getBounds();

        if(!cells.empty() &&
           ((event.mouseWheelDelta.y != 0.0f) ||
           (event.mouseWheelDelta.x != 0.0f))){

            size_t num_rows = nrow(*dataSource),
                    num_cols = ncol(*dataSource);

            sf::Vector2u newCoords;

            if(event.mouseWheelDelta.x != 0.0f){

                if(event.mouseWheelDelta.x > 0.0f){
                    if(br.left > bounds.left + bounds.width){

                        frameAnchor.x -= (cells.front().front()->getBounds().width);

                        removeCol(cells.back().size() - 1);
                        insertCol(0);

                        for(size_t y = 0; y < cells.size(); ++y){
                            if(y == 0) newCoords = coords[y].front();
                            else newCoords = coords[y][1];

                            if(newCoords.x == 0) break;

                            if(newCoords.y < num_rows){
                                set_coords(0, y, sf::Vector2u(newCoords.x - 1, newCoords.y));
                                addCell((*dataSource)[newCoords.y][newCoords.x - 1], 0, y);
                            }
                        }

                    }
                }
                else{
                    if(tl.left + tl.width < bounds.left){
                        removeCol(0);

                        for(size_t y = 0; y < cells.size(); ++y){
                            if(y == 0) newCoords = coords[y].back();
                            else newCoords = coords[y][coords[y].size() - 2];

                            if(newCoords.x + 1 >= num_cols) break;

                            if(newCoords.y < num_rows){
                                if(y == 0){
                                    set_coords(cells.front().size(), y, sf::Vector2u(newCoords.x + 1, newCoords.y));
                                    addCell((*dataSource)[newCoords.y][newCoords.x + 1], cells[y].size(), y);
                                }
                                else{
                                    set_coords(cells.front().size() - 1, y, sf::Vector2u(newCoords.x + 1, newCoords.y));
                                    addCell((*dataSource)[newCoords.y][newCoords.x + 1], cells[y].size() - 1, y);
                                }
                            }
                        }

                        if(cells.back().back()->getBounds().left +
                           cells.back().back()->getBounds().width > totalBounds.width){
                                totalBounds.height = cells.back().back()->getBounds().left +
                                                        cells.back().back()->getBounds().width;
                        }

                        frameAnchor.x += (cells.front().back()->getBounds().height);
                    }
                }
            }

            if(event.mouseWheelDelta.y != 0.0f){

                if(event.mouseWheelDelta.y < 0.0f){
                    if(tl.top + tl.height < bounds.top){

                        removeRow(0);

                        for(size_t x = 0; x < cells.back().size(); ++x){
                            if(x == 0) newCoords = coords.back()[x];
                            else newCoords = coords[coords.size() - 2][x];

                            if(newCoords.y + 1 >= num_rows) break;

                            if(newCoords.x < num_cols){
                                if(x == 0){
                                    set_coords(x, cells.size(), sf::Vector2u(newCoords.x, newCoords.y + 1));
                                    addCell((*dataSource)[newCoords.y + 1][newCoords.x], x, cells.size());
                                }
                                else{
                                    set_coords(x, cells.size() - 1, sf::Vector2u(newCoords.x, newCoords.y + 1));
                                    addCell((*dataSource)[newCoords.y + 1][newCoords.x], x, cells.size() - 1);
                                }
                            }
                        }

                        if(cells.back().back()->getBounds().top +
                           cells.back().back()->getBounds().height > totalBounds.height){
                                totalBounds.height = cells.back().back()->getBounds().top +
                                                        cells.back().back()->getBounds().height;
                        }

                        frameAnchor.y += (cells.back().front()->getBounds().height);
                    }
                }
                else{
                    if(br.top > bounds.top + bounds.height){

                        frameAnchor.y -= (cells.front().front()->getBounds().height);

                        removeRow(cells.size() - 1);
                        insertRow(0);

                        for(size_t x = 0; x < cells.back().size(); ++x){
                            if(x == 0) newCoords = coords.front()[x];
                            else newCoords = coords[1][x];

                            if(newCoords.y == 0) break;

                            if(newCoords.x < num_cols){
                                set_coords(x, 0, sf::Vector2u(newCoords.x, newCoords.y - 1));
                                addCell((*dataSource)[newCoords.y - 1][newCoords.x], x, 0);
                            }
                        }

                    }
                }
            }
        }

        scrollBarY.setScrollMax(totalBounds.height);
        scrollBarX.setScrollMax(totalBounds.width);

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
            if(totalBounds.height > bounds.height - top_margin() - bottom_margin()){
                moveDist.y = top_margin() + bounds.top -
                                           tl.top -
                                           (scrollBarY.getScrollOffset() + frameAnchor.y);
            }
            else{
                moveDist.y = bounds.top + top_margin() - tl.top;
            }

            if(totalBounds.width > bounds.width - left_margin() - right_margin()){
                moveDist.x = left_margin() + bounds.left -
                                           tl.left -
                                           (scrollBarX.getScrollOffset() + frameAnchor.x);
            }
            else{
                moveDist.x = bounds.left + left_margin() - tl.left;
            }

            for(auto& row : cells){
                for(auto& cell : row){
                    cell->move(moveDist);
                }
            }
        }

        return true;
    }

    bool draw(sf::RenderTarget* target){
        if(!CVTablePanel::draw(target)) return false;

        return true;
    }

    CVDataViewerPanel(CVView* parentView, T* dataSource = nullptr, std::string panelTag = "",
                      const sf::Color& backgroundColor = sf::Color::Transparent,
                      const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                      const bool& bFitWindow = true,
                      const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f)):
        CVTablePanel(parentView, panelTag, backgroundColor, size, bFitWindow, position),
        dataSource(dataSource),
        totalBounds(bounds),
        frameAnchor(0.0f,0.0f)
        {

            size_t num_rows = nrow(*dataSource),
                    num_cols = ncol(*dataSource);

            for(size_t x = 0; x < num_cols; ++x){
                for(size_t y = 0; y < num_rows; ++y){
                    if(x < rowSize(*dataSource, y)){
                        addCell((*dataSource)[y][x], x, y);
                        set_coords(x, y, sf::Vector2u(x, y));
                    }
                    else{
                        addCell("", x, y);
                        set_coords(x, y, sf::Vector2u(UINT_MAX, UINT_MAX));
                    }

                    if(cells[y][x]->getBounds().top > bounds.top + bounds.height) break;
                }
                if(cells.front()[x]->getBounds().left > bounds.left + bounds.width) break;
            }

        }


};

}

#endif // CVIS_DATA
