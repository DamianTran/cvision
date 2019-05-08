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

#ifndef CVIS_PLOT
#define CVIS_PLOT

#include "hyper/toolkit/string.hpp"

#include "cvision/type.hpp"
#include "cvision/textbox.hpp"
#include "cvision/viewpanel.hpp"

#include <cmath>

// CV Plot Macros ==============================

#define CV_PLOT_TYPE_NONE               0
#define CV_PLOT_TYPE_SCATTER            1
#define CV_PLOT_TYPE_CATEGORY           2
#define CV_PLOT_TYPE_BAR                3
#define CV_PLOT_TYPE_LINE               4
#define CV_PLOT_TYPE_STEP               5
#define CV_PLOT_TYPE_CUSTOM             254
#define CV_PLOT_TYPE_AUTO               255

#define CV_PLOT_ALIGN_NONE              0
#define CV_PLOT_ALIGN_HORIZONTAL        1
#define CV_PLOT_ALIGN_VERTICAL          2
#define CV_PLOT_ALIGN_DIAG_RIGHT        3
#define CV_PLOT_ALIGN_DIAG_LEFT         4

#define CV_LINE_TYPE_SOLID              0
#define CV_LINE_TYPE_BROKEN             1
#define CV_LINE_TYPE_DOTTED             2
#define CV_LINE_TYPE_SLANTED            3
#define CV_LINE_TYPE_CROSS              4

#define CV_LINE_CONN_NONE               0
#define CV_LINE_CONN_DIRECT             1
#define CV_LINE_CONN_STEP               2

#define CV_LINE_REGRESSION_NONE         0
#define CV_LINE_REGRESSION_PEARSON      1
#define CV_LINE_REGRESSION_SPEARMAN     2

#define CV_CLUSTER_NONE                 0
#define CV_CLUSTER_HIERARCHICAL         1

#define CV_HEAT_SCALE_ABS               0
#define CV_HEAT_SCALE_REL_ALL           1
#define CV_HEAT_SCALE_REL_ROW           2
#define CV_HEAT_SCALE_REL_COL           3

#define CV_PLOT_ID_NONE                 "none"
#define CV_PLOT_ID_SCATTER              "scatter"
#define CV_PLOT_ID_CATEGORY             "category"
#define CV_PLOT_ID_LINE                 "line"
#define CV_PLOT_ID_HEAT                 "heat"

#define CV_PLOT_UPDATE_NONE             0b0
#define CV_PLOT_UPDATE_DATA             0b1
#define CV_PLOT_UPDATE_POINTS           0b10
#define CV_PLOT_UPDATE_AXIS             0b100
#define CV_PLOT_UPDATE_DRAW             0b1000

#define CV_PLOT_UPDATE_ALL              0b11111111

#ifndef HYPER_ALGORITHM
#define DATA_TYPE_UNKNOWN               0
#define DATA_TYPE_NUMERIC               1
#define DATA_TYPE_VERBAL                2
#define DATA_TYPE_IMAGE                 3
#define DATA_TYPE_VIDEO                 4
#define DATA_TYPE_SOUND                 5
#endif

#ifndef CVIS_ALGORITHM
#define DIMX                            0u
#define DIMY                            1u
#define DIMZ                            2u

#define DIMENSION_X                     0u
#define DIMENSION_Y                     1u
#define DIMENSION_Z                     2u
#endif

namespace cvis{

class CVView;
class CVEvent;

class CVISION_API CVPlot: public CVTextBox{
protected:

    unsigned char updateState;

    bool bReqPlotUpdate,
        bReqAxisUpdate,
        bUseVertGrid,
        bUseHorizGrid,
        bAnimSprites,
        bShowSprites,
        bPlotBoundary;

    std::vector<bool> scaleAutoPadding;

    std::string plotTypeID;

    unsigned int dataPointNum,
                framesLastChange,
                labelFontSize;

    float animSpriteSpeed,
            spriteSize,
            fontScaling,
            spriteScaling,
            plotBoundaryThickness;

    std::vector<sf::RectangleShape> frontLines,
                                    backLines,
                                    gridLines;
    sf::RectangleShape plotBoundary;

    std::vector<sf::Text> plotText;

    std::vector<CVTypeBox> axisLabels;

    std::vector<CVTextBox> legends;
    std::vector<std::string> legendNames;

    std::vector<float> axisPos;

    sf::Rect<float> plotBounds;

    std::vector<sf::Vector2f> dimScales,
                            scalePadding;

    std::vector<std::string> xAxisLabels, yAxisLabels;

    std::vector<unsigned int> subset;

    ColorTheme plotColors;
    sf::Color plotHighlightColor,
            axisTextColor,
            axisLineColor;

    std::vector<std::vector<std::vector<float>>> data; // Numeric info for data entries for each dimension
    std::vector<std::vector<std::vector<std::string>>> tags; // Verbal info for labels for each dimension

    std::string title, fontName;

    const sf::Texture* plotSpriteTexture;
    const sf::Font* plotFont;

    struct CVISION_API axis{

        CVPlot* plot;
        TextEntry textInfo;
        std::string title;
        CVTypeBox* label;

        bool bTick,
            bAutoUpdateScale,
            bSpanLabels;

        float pos,
            thickness,
            textPadding,
            offset;

        sf::RectangleShape line;
        std::vector<sf::RectangleShape> lineTicks;

        unsigned int dimension;
        std::vector<std::string> labels;

        std::vector<float> axisIntervals;
        uint8_t dataType;
        sf::Color lineColor;

        std::vector<sf::Text> renderLabels;

        inline void setLabel(const std::string& newLabel){ title = newLabel; }
        inline void setFontSize(const unsigned int& newSize){
            textInfo.fontSize = newSize;
        }
        CVISION_API void setLabelColor(const sf::Color& newColor);
        CVISION_API void setLineColor(const sf::Color& newColor);

        CVISION_API void updateRenderLabels();
        CVISION_API void update();

        inline void removeLabel(const unsigned int& index){
            labels.erase(labels.begin() + index);
            renderLabels.erase(renderLabels.begin() + index);
            axisIntervals.erase(axisIntervals.begin() + index);
        }

        inline void setLabels(const std::vector<std::string>& labels){
            dataType = DATA_TYPE_VERBAL;
            this->labels = labels;
            updateRenderLabels();
        }

        CVISION_API float getLabelPosition(const std::string& label) const;

        CVISION_API void getNumericLabels(const unsigned int& numDecimals = 1);

        CVISION_API axis(const float& pos, const unsigned int& dimension, CVPlot* plot,
             const std::vector<std::string>& labels = std::vector<std::string>(),
             const uint8_t& dataType = DATA_TYPE_NUMERIC, const float& thickness = 3.0f,
             const float& offset = 0.0f,
             const sf::Color& lineColor= sf::Color::Black,
             const TextEntry& textInfo = TextEntry("", "Paratype", 16, ALIGN_LEFT, sf::Color::Black),
             const float& textPadding = 5.0f);

    };

    struct Dataset;

    struct CVISION_API dataPoint{

        Dataset* dataset;
        CVPlot* plot;

        bool selected,
            animated,
            showHoverDisplay,
            visible;

        unsigned int dataIndex;

        sf::Sprite sprite;
        std::string name;
        std::vector<std::string> labels;

        float animSpeed;

        std::vector<float> dimValues;

        sf::Vector2f offset;

        inline float& x(){ return dimValues[DIMX]; }
        inline float& y(){ return dimValues[DIMY]; }
        inline float& z(){ return dimValues[DIMZ]; }

        inline float& operator[](const unsigned int& index){ return dimValues[index]; }

        sf::Color spriteColor;

        inline bool valid() const{
            for(auto& coord : dimValues){
                if(!std::isnan(coord)) return true;
            }
            return false;
        }

        inline void setSize(const sf::Vector2f& newSize){
            sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
            sprite.setScale(
                newSize.x/texSize.x,
                newSize.y/texSize.y);
        }
        inline void setSize(const float& x, const float& y){
            setSize(sf::Vector2f(x, y));
        }

        inline sf::Vector2f getPosition() const{ return sprite.getPosition(); }

        inline void setBaseColor(const sf::Color& newColor){
            spriteColor = newColor;
            if(!selected) sprite.setColor(newColor);
        }

        inline std::string getDisplayString() const{
            if((plot->xAxis().dataType == DATA_TYPE_NUMERIC) &&
               (plot->yAxis().dataType == DATA_TYPE_NUMERIC))
                    if(labels.size() < 1)
                        return std::to_string(dataIndex) + ": [" + hyperC::strDigits(dimValues[DIMX], 3) + ',' + hyperC::strDigits(dimValues[DIMY], 3) + "]";
                    else return hyperC::concatenateString(labels, ",") + ": [" + hyperC::strDigits(dimValues[DIMX], 3) + ',' + hyperC::strDigits(dimValues[DIMY], 3) + "]";
            else if((plot->xAxis().dataType == DATA_TYPE_VERBAL) &&
                (plot->yAxis().dataType == DATA_TYPE_NUMERIC)){
                    return std::to_string(dataIndex) + ": [" + hyperC::strDigits(dimValues[DIMY], 3) + "]\nCategory: " + labels[DIMX];
            }
            else if((plot->xAxis().dataType == DATA_TYPE_NUMERIC) &&
                (plot->yAxis().dataType == DATA_TYPE_VERBAL)){
                    return std::to_string(dataIndex) + ": [" + hyperC::strDigits(dimValues[DIMX], 3) + "]\nCategory: " + labels[DIMY];
            }
            return std::string();
        }

        CVISION_API void setSelection(const bool& state);

        inline void setOffset(){
            switch(plot->xAxis().dataType){
                case DATA_TYPE_VERBAL:{
                    offset.x = getPosition().x - plot->xAxis().getLabelPosition(labels[DIMX]);
                    break;
                }
                default:{
                    offset.x = getPosition().x - plot->getPlotXPos(x());
                    break;
                }
            }

            switch(plot->yAxis().dataType){
                case DATA_TYPE_VERBAL:{
                    offset.y = getPosition().y - plot->yAxis().getLabelPosition(labels[DIMY]);
                    break;
                }
                default:{
                    offset.y = getPosition().y - plot->getPlotYPos(y());
                    break;
                }
            }
        }

        CVISION_API void update();

        CVISION_API dataPoint(const std::vector<float>& dimValues,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  const std::vector<std::string>& labels = std::vector<std::string>(),
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        CVISION_API dataPoint(const float& x, const float& y,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  const std::vector<std::string>& labels = std::vector<std::string>(),
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        CVISION_API dataPoint(const std::string& xLabel, const float& y,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        CVISION_API dataPoint(const float& x, const std::string& yLabel,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
    };

    std::vector<dataPoint*> selectedPoints;

    struct CVISION_API DataRequest{
        std::vector<std::vector<float>> data;
        std::vector<std::vector<std::string>> labels;
        std::vector<std::vector<unsigned int>> indices;

        std::string name;
        std::vector<std::string> pointTags;

        DataRequest(const std::vector<float>& xData, const std::vector<float>& yData,
                    const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                    const std::string& name = "", const std::vector<std::string>& pointTags = std::vector<std::string>()):
            data({xData, yData}),
            indices({indices}),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& matrix, const std::vector<unsigned int> indices,
                    const std::vector<std::vector<std::string>>& tags, const std::string& name = "",
                    const std::vector<std::string>& pointTags = std::vector<std::string>()):
                        data(matrix),
                        labels(tags),
                        indices({ indices }),
                        name(name),
                        pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& data,
                    const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                    const std::string& name = "", const std::vector<std::string>& pointTags = std::vector<std::string>()):
            data(data),
            indices({indices}),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const std::vector<std::string>& xLabels, const std::vector<std::vector<float>>& yData,
                    const std::vector<std::vector<unsigned int>>& indices, const std::string& name = "",
                    const std::vector<std::string>& pointTags = std::vector<std::string>()):
            data(yData),
            labels({xLabels}),
            indices(indices),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& xData, const std::vector<std::string>& yLabels,
                    const std::vector<std::vector<unsigned int>>& indices, const std::string& name = "",
                    const std::vector<std::string>& pointTags = std::vector<std::string>()):
            data(xData),
            labels({std::vector<std::string>(),yLabels}),
            indices(indices),
            name(name),
            pointTags(pointTags){ }

    };

    struct CVISION_API Dataset{

        CVPlot* plot;

        std::vector<uint8_t> dimensionDataTypes;

        std::vector<std::vector<float>> matrix;
        std::vector<std::vector<std::string>> labels;
        std::vector<std::vector<unsigned int>> indices;

        std::vector<unsigned int> subset;

        std::vector<dataPoint> points;
        std::string name;

        CVISION_API void rm_dimension(const unsigned int& dimension);

        ColorTheme colors;

        inline sf::Color& mainColor(){
            if(colors.size() < 1) throw std::out_of_range("No main color to access in plot dataset");
            return colors.front();
        }
        inline sf::Color& secondaryColor(){
            if(colors.size() < 2) throw std::out_of_range("No second color to access in plot dataset");
            return colors[1];
        }

        inline void setColor(const sf::Color& newColor){
            try{
                this->mainColor() = newColor;
            }catch(std::out_of_range){
                colors.emplace_back(newColor);
            }

            for(auto& point : points){
                point.setBaseColor(mainColor());
            }
        }

        inline void setDataSubset(const std::vector<unsigned int>& subsetIdx){
            subset = subsetIdx;
        }

        CVISION_API void update();

        CVISION_API std::vector<float> getSubsetData(const unsigned int& dimension) const;
        CVISION_API std::vector<dataPoint*> getSubsetPoints();
        CVISION_API std::vector<dataPoint*> getLabelledPoints(const std::string& tag);
        CVISION_API std::vector<float> getLabelledData(const std::string& tag, const unsigned int& dimension);

        inline unsigned int numDimensions() const{ return matrix.size(); }

        CVISION_API sf::Vector2f getScale(const unsigned int& dimension) const;

        inline sf::Vector2f getXScale() const{ return getScale(DIMENSION_X); }
        inline sf::Vector2f getYScale() const{ return getScale(DIMENSION_Y); }
        inline sf::Vector2f getZScale() const{ return getScale(DIMENSION_Z); }

        inline std::vector<float>& x(){ return matrix[DIMX]; }
        inline std::vector<float>& y(){ return matrix[DIMY]; }
        inline std::vector<float>& z(){ return matrix[DIMZ]; }

        CVISION_API size_t size() const;

        CVISION_API Dataset(const std::vector<std::vector<float>>& matrix,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const std::vector<std::string>& pointTags,
                CVPlot* plot);
        CVISION_API Dataset(const std::vector<float>& x, const std::vector<float>& y,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const std::vector<std::string>& pointTags,
                CVPlot* plot);
        CVISION_API Dataset(const std::vector<std::string>& xLabels, const std::vector<std::vector<float>>& y,
                const std::vector<std::vector<unsigned int>>& indices, const std::string& name, CVPlot* plot);
        CVISION_API Dataset(const std::vector<std::vector<float>>& x, const std::vector<std::string>& yLabels,
                const std::vector<std::vector<unsigned int>>& indices, const std::string& name, CVPlot* plot);

    };

    std::vector<Dataset> datasets;
    std::vector<axis> plotAxes;
    std::vector<DataRequest> dataAddRequests;

    inline CVTypeBox& titleBox(){
        if(axisLabels.empty()) throw std::out_of_range("Invalid call in CVPlot: No title box to index for plot");
        return axisLabels.front();
    }
    inline CVTypeBox& xLabelBox(){
        if(axisLabels.size() < 2) throw std::out_of_range("Invalid call in CVPlot: no x-axis label box to index for plot");
        return axisLabels[1];
    }
    inline CVTypeBox& yLabelBox(){
        if(axisLabels.size() < 3) throw std::out_of_range("Invalid call in CVPlot: no y-axis label box to index for plot");
        return axisLabels[2];
    }

    inline float areaScale() const{ return (plotBounds.width+plotBounds.height)/512 * viewScale(); }
    inline float heightScale() const{ return plotBounds.height/256 * viewScale(); }
    inline float widthScale() const{ return plotBounds.width/256 * viewScale(); }
    inline float densityScale() const{ return 2.0f/log10(numPoints()); }
    inline float getSpriteSize() const{
        return spriteSize*densityScale()*areaScale();
    }

    inline void updateScales(){
        unsigned int D = numDimensions();
        sf::Vector2f dataDims;
        for(size_t i = 0; i < D; ++i){
            if((plotAxes.size() > i) && !plotAxes[i].bAutoUpdateScale) continue;
            dimScales[i].x = NAN;
            dimScales[i].y = NAN;

            for(auto& set : datasets){
                if(set.dimensionDataTypes[i] != DATA_TYPE_NUMERIC){
                    continue;
                }

                dataDims = set.getScale(i);
                if(!std::isnan(dataDims.x)){
                    if(std::isnan(dimScales[i].x)) dimScales[i].x = dataDims.x;
                    else if(dataDims.x < dimScales[i].x) dimScales[i].x = dataDims.x;
                }
                if(!std::isnan(dataDims.y)){
                    if(std::isnan(dimScales[i].y)) dimScales[i].y = dataDims.y;
                    else if(dataDims.y > dimScales[i].y) dimScales[i].y = dataDims.y;
                }
            }
        }
    }

    CVISION_API void updateBounds();

public:

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    inline void callUpdate(const unsigned char& reqState = CV_PLOT_UPDATE_ALL){
        updateState |= reqState;
        this->framesLastChange = 0; }

    CVISION_API void get_numeric_x_axis();
    CVISION_API void get_numeric_y_axis();

    inline bool has_x_axis() const{ return axisLabels.size() > 0; }
    inline bool has_y_axis() const{ return axisLabels.size() > 1; }
    inline bool has_z_axis() const{ return axisLabels.size() > 2; }

    inline unsigned int numPoints() const{
        unsigned int output = 0;
        for(auto& set : datasets){
            output += set.points.size();
        }
        return output;
    }

    inline void setPlotHighlightColor(const sf::Color& newColor){
        plotHighlightColor = newColor;
    }

    inline void setScaleAuto(const std::vector<float>& states){
        for(size_t i = 0; i < states.size(); ++i){
            if(i >= scaleAutoPadding.size()) scaleAutoPadding.push_back(states[i]);
            else scaleAutoPadding[i] = states[i];
        }
    }

    inline const CVTypeBox& mainTitle() const{ return axisLabels.front(); }
    inline const CVTypeBox& xLabel() const{ return axisLabels[1]; }
    inline const CVTypeBox& yLabel() const{ return axisLabels[2]; }

    CVISION_API void get_verbal_x_axis();
    CVISION_API void get_verbal_y_axis();

    CVISION_API void removeDataOutliers();

    inline std::vector<unsigned int> getSelectedIndices() const{
        std::vector<unsigned int> output;
        for(auto& point : selectedPoints){
            output.push_back(point->dataIndex);
        }
        return output;
    }

    inline sf::Color& plotColor(const unsigned int& index = 0){
        return plotColors[index]; }

    inline const std::string& type() const{ return plotTypeID; }

    inline void addPlotColor(const sf::Color& newColor){ plotColors.emplace_back(newColor); }

    inline sf::Color getFillColor() const{ return colorTheme[0]; }
    inline const sf::Color& getBorderColor() const{ return colorTheme[1]; }

    inline const sf::Vector2f& getScalePadding(const unsigned int& dimension) const{
        return scalePadding[dimension];
    }

    inline void setScalePadding(const unsigned int& dimension, const sf::Vector2f& newPadding){
        while(dimension >= scalePadding.size()) scalePadding.emplace_back(0.0f,0.0f);
        scalePadding[dimension] = newPadding;
        for(size_t i = 0; i < scaleAutoPadding.size(); ++i){
            scaleAutoPadding[i] = false;
        }
    }
    inline void setScalePadding(const unsigned int& dimension, const float& min, const float& max){
        setScalePadding(dimension, sf::Vector2f(min, max));
    }

    inline void setScale(const unsigned int& dimension, const float& min, const float& max){
        while(dimension >= dimScales.size()) dimScales.emplace_back(0.0f, 0.0f);
        dimScales[dimension].x = min;
        dimScales[dimension].y = max;
    }

    CVISION_API bool setPlotSpriteTexture(const std::string& textureName);

    CVISION_API void addPlotText(const TextEntry& newText,
                                 const sf::Vector2f& position);

    inline void addAxis(const float& pos, const unsigned int& dimension,
                 const uint8_t& dataType, const float& thickness = 5.0f,
                 const float& offset = 0.0f,
                 const sf::Color& lineColor= sf::Color::Black,
                 const unsigned int& fontSize = 16,
                 const uint8_t& alignment = ALIGN_LEFT,
                 const sf::Color& fontColor = sf::Color::Black,
                 std::string fontName = "",
                 const float& textPadding = 5.0f){
        if(fontName.size() < 1) fontName = this->fontName;
        plotAxes.emplace_back(pos, dimension, this, std::vector<std::string>(), dataType, thickness, offset, lineColor,
                              TextEntry("", fontName, fontSize, alignment, fontColor),
                              textPadding);
    }

    inline unsigned int numDimensions() const{
        unsigned int output = 0;
        for(auto& set : datasets){
            if(set.numDimensions() > output) output = set.numDimensions();
        }
        return output;
    }
    inline unsigned int dimensionSize(const unsigned int dimension) const{ return data[dimension].size(); }
    inline unsigned int dimXSize() const{ return data[DIMX].size(); }
    inline unsigned int dimYSize() const{ return data[DIMY].size(); }
    inline unsigned int dimZSize() const{ return data[DIMZ].size(); }

    inline float getDataRange(const unsigned int& dimension){
        return dimScales[dimension].y - dimScales[dimension].x;
    }
    inline float getScaleRange(const unsigned int& dimension){
        return getDataRange(dimension) * (1.0f + scalePadding[dimension].x + scalePadding[dimension].y);
    }

    inline void setGridStatus(bool horiz, bool vert){
        bUseHorizGrid = horiz;
        bUseVertGrid = vert;
    }

    inline axis& xAxis() { return plotAxes[0]; }
    inline axis& yAxis() { return plotAxes[1]; }
    inline axis& zAxis() { return plotAxes[2]; }

    inline unsigned int numDataPoints() const{ return dataPointNum; }

    inline void useSubset(const std::vector<unsigned int>& subset){
        this->subset = subset;
        callUpdate();
    }
    inline void clearSubset(){
        subset.clear();
        callUpdate();
    }

    CVISION_API void addData(const std::vector<float>& xData, const std::vector<float>& yData,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const std::string& name = "", const std::vector<std::string>& pointTags = std::vector<std::string>());
    CVISION_API void addData(const std::vector<std::string>& xLabels, const std::vector<std::vector<float>>& yData,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = "", const std::vector<std::string>& pointTags = std::vector<std::string>());
    CVISION_API void addData(const std::vector<std::vector<float>>& xData, const std::vector<std::string>& yLabels,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = "");
    CVISION_API void addData(const std::vector<std::vector<float>>& matrix,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const std::vector<std::vector<std::string>>& labels =
                                std::vector<std::vector<std::string>>(),
                        const std::string& name = "");

    inline void addTag(const unsigned int dimension, const unsigned int index, const std::string& newTag){
        tags[dimension].insert(tags[dimension].begin() + index, std::vector<std::string>({ newTag }));
    }

    inline bool select_tags(const std::vector<std::string>& tags){
        if(datasets.size() < 1) return false;

        bool output = false;
        for(auto& set : datasets){
            for(auto& point : set.points){
                if(hyperC::cmpStringToList(tags, point.labels, CMP_STR_MATCH)){
                    point.setSelection(true);
                    output = true;
                }
                else point.setSelection(false);
            }
        }

        callUpdate(CV_PLOT_UPDATE_DRAW);

        return output;
    }

    CVISION_API bool select_indices(const std::vector<unsigned int>& idx);

    CVISION_API void removeData(const unsigned int dimension, const unsigned int index);
    CVISION_API void removeData(const unsigned int dimension,
                   const unsigned int begin,
                   const unsigned int end);

    CVISION_API void clearData();
    CVISION_API void clearData(const unsigned int dimension);

    CVISION_API friend std::ostream& operator<<(std::ostream& output, const CVPlot& plot);

    // Customization

    inline void setTitle(const std::string& main){
        if(axisLabels.size() > 0) titleBox().setTypeString(main); }
    inline void setXLabel(const std::string& xLab){
        if(axisLabels.size() > 1) xLabelBox().setTypeString(xLab); }
    inline void setYLabel(const std::string& yLab){
        if(axisLabels.size() > 2) yLabelBox().setTypeString(yLab); }

    inline void setPlotBorder(const bool& state = true, const float& thickness = 2.0f){
        bPlotBoundary = state;
        plotBoundary.setOutlineThickness(thickness);
    }
    inline void setPlotBorderColor(const sf::Color& newColor){
        plotBoundary.setOutlineColor(newColor);
    }
    inline void setSpriteSize(const float& newSize){
        spriteSize = newSize;
    }

    CVISION_API void setAxisLineColor(const sf::Color& newColor);
    CVISION_API void setAxisLabelColor(const sf::Color& newColor);
    CVISION_API void setAxisColor(const sf::Color& newColor);

    inline std::string getTitle(){
        if(axisLabels.size() > 0) return titleBox().getTypeString();
        else return std::string(); }
    inline std::string getXLabel(){
        if(axisLabels.size() > 1) return xLabelBox().getTypeString();
        else return std::string(); }
    inline std::string getYLabel(){
        if(axisLabels.size() > 2) return yLabelBox().getTypeString();
        else return std::string(); }

    inline void addLine(const uint8_t& orientation, const float& dimValue,
                        const float& thickness = 1.0f, const sf::Color& color = sf::Color::Black,
                        const uint8_t& type = CV_LINE_TYPE_SOLID, bool foreground = false);

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& distance);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    inline void setPlotColors(const ColorTheme& newTheme){ this->plotColors = newTheme; }

    CVISION_API sf::Vector2f getPlotPos(const sf::Vector2f& dataPoint); // Turn scale units to view draw coordinates
    CVISION_API sf::Vector2f getPlotPos(const float& x, const float& y);
    CVISION_API float getPlotYPos(const float& dataY);
    CVISION_API float getPlotXPos(const float& dataX);

    CVISION_API sf::Vector2f getScalePos(const sf::Vector2f& pos); // Turn view window coordinates to data scale coordinates

    CVISION_API float getScaleXPos(const float& pos);
    CVISION_API float getScaleYPos(const float& pos);

    CVISION_API CVPlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor = sf::Color::Transparent,
           const sf::Color& borderColor = sf::Color::Black,
           const sf::Color& plotColor = sf::Color(128,128,128),
           const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
           const std::string& fontName = "Paratype");

    virtual ~CVPlot(){ }

};

inline uint8_t parse_regression_type(const std::string& type){
    if(hyperC::cmpString(type, "pearson")){
        return CV_LINE_REGRESSION_PEARSON;
    }
    if(hyperC::cmpString(type, "spearman")){
        return CV_LINE_REGRESSION_SPEARMAN;
    }
    return CV_LINE_REGRESSION_NONE;
}

class CVISION_API CVScatterPlot: public CVPlot{
protected:

    std::string quadrantType;
    bool colorQuadrants;
    ColorTheme quadrantColors;

    uint8_t regressionType;

    struct CVISION_API RegLineInfo{

        CVScatterPlot* host;

        float coefficient,
                slope,
                intercept;

        CVISION_API void getLinearModel(const std::vector<float>& xData, const std::vector<float>& yData);

        RegLineInfo(const std::vector<float>& xData, const std::vector<float>& yData, CVScatterPlot* host):
            host(host){
            getLinearModel(xData, yData);
        }
    };

    std::vector<RegLineInfo> regression;

    std::vector<std::vector<dataPoint*>> quadrantPoints;

public:

    inline void setQuadrants(const std::string& sepType){
        quadrantType = sepType;
        callUpdate(CV_PLOT_UPDATE_DRAW); }
    inline void enableColorQuadrants(){
        if(!colorQuadrants){
            colorQuadrants = true;
            quadrantColors.clear();
            quadrantColors.emplace_back(178, 48, 46);
            quadrantColors.emplace_back(187, 195, 206);
            quadrantColors.emplace_back(49, 11, 79);
            quadrantColors.emplace_back(124, 198, 123);
        }
        callUpdate(CV_PLOT_UPDATE_DRAW);
    }
    inline void disableColorQuadrants(){
        if(colorQuadrants){
            quadrantColors.clear();
            colorQuadrants = false;
        }
        callUpdate(CV_PLOT_UPDATE_DRAW);
    }
    inline const bool& getQuadrantColorStatus() const{ return colorQuadrants; }

    inline void setRegressionLine(const uint8_t& state){
        regressionType = state;
        callUpdate(CV_PLOT_UPDATE_DRAW);
    }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVScatterPlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype",
                   const std::string& quadrantType = "");

};

class CVISION_API CVCategoryPlot: public CVPlot{
protected:

    bool showQ1,
        showMedian,
        showQ3,
        showSigBrackets,
        omitOutliers;

    std::vector<sf::Vector2u> sigPairs;
    std::vector<std::vector<float>> sigMatrix;
    std::vector<float> sigValues;

    unsigned int maxSigBrackets;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void setSignificanceBrackets(const bool& status){
        showSigBrackets = status;
        callUpdate();
    }

    void setCategoryMarkers(const bool& showQ1, const bool& showMedian, const bool& showQ3){
        this->showQ1 = showQ1;
        this->showMedian = showMedian;
        this->showQ3 = showQ3;
        callUpdate();
    }

    CVISION_API CVCategoryPlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype");

};

class CVISION_API CVLinePlot: public CVPlot{
protected:

    uint8_t lineType,
            connectionType;
    bool showSprites;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVLinePlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype",
                   const uint8_t& lineType = CV_LINE_TYPE_SOLID,
                   const uint8_t& connectionType = CV_LINE_CONN_DIRECT);


};

class CVISION_API CVHeatPlot: public CVPlot{
protected:

    uint8_t clusterType,
                scaleType;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API CVHeatPlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "basic_square",
                   const std::string& fontName = "Paratype",
                   const uint8_t& clusterType = CV_CLUSTER_HIERARCHICAL,
                   const uint8_t& scaleType = CV_HEAT_SCALE_REL_ROW);
};

class CVISION_API CVPlotPanel: public CVViewPanel{
protected:

    std::vector<sf::RectangleShape> highlightBlocks;
    sf::Color plotHighlightColor;
    std::vector<CVPlot*> selectedPlots;
    sf::Sprite logo;

    unsigned int logoAlpha;

public:

    CVISION_API std::vector<unsigned int> getAllSelectedIndices(); // Get all indices selected in plots
    CVISION_API std::vector<unsigned int> getCommonSelectedIndices(); // Get indices commonly selected in plots

    inline unsigned int numSelected() const{ return selectedPlots.size(); }
    CVISION_API bool select_tags(const std::vector<std::string>& tags);
    CVISION_API bool select_indices(const std::vector<unsigned int>& idx);

    inline const std::vector<CVPlot*>& getSelectedPlots() const{ return selectedPlots; }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API CVPlotPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Color& plotHighlightColor = sf::Color(160,207,255,128),
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                const bool& bFitWindow = true, const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

}

#endif // CVIS_PLOT
