#pragma once

#ifndef CVIS_PLOT
#define CVIS_PLOT

#include "algorithm.hpp"

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_typeIO.hpp"

// CV Plot Macros ==============================

#define CV_PLOT_TYPE_NONE               0_BIT
#define CV_PLOT_TYPE_SCATTER            1_BIT
#define CV_PLOT_TYPE_CATEGORY           2_BIT
#define CV_PLOT_TYPE_BAR                3_BIT
#define CV_PLOT_TYPE_LINE               4_BIT
#define CV_PLOT_TYPE_STEP               5_BIT
#define CV_PLOT_TYPE_CUSTOM             254_BIT
#define CV_PLOT_TYPE_AUTO               255_BIT

#define CV_PLOT_ALIGN_NONE              0_BIT
#define CV_PLOT_ALIGN_HORIZONTAL        1_BIT
#define CV_PLOT_ALIGN_VERTICAL          2_BIT
#define CV_PLOT_ALIGN_DIAG_RIGHT        3_BIT
#define CV_PLOT_ALIGN_DIAG_LEFT         4_BIT

#define CV_LINE_TYPE_SOLID              0_BIT
#define CV_LINE_TYPE_BROKEN             1_BIT
#define CV_LINE_TYPE_DOTTED             2_BIT
#define CV_LINE_TYPE_SLANTED            3_BIT
#define CV_LINE_TYPE_CROSS              4_BIT

#define CV_LINE_CONN_NONE               0_BIT
#define CV_LINE_CONN_DIRECT             1_BIT
#define CV_LINE_CONN_STEP               2_BIT

#define CV_LINE_REGRESSION_NONE         0_BIT
#define CV_LINE_REGRESSION_PEARSON      1_BIT
#define CV_LINE_REGRESSION_SPEARMAN     2_BIT

#define CV_PLOT_ID_NONE                 "none"
#define CV_PLOT_ID_SCATTER              "scatter"
#define CV_PLOT_ID_CATEGORY             "category"
#define CV_PLOT_ID_LINE                 "line"

#define CV_PLOT_UPDATE_NONE             0b0
#define CV_PLOT_UPDATE_DATA             0b1
#define CV_PLOT_UPDATE_POINTS           0b10
#define CV_PLOT_UPDATE_AXIS             0b100
#define CV_PLOT_UPDATE_DRAW             0b1000

#define CV_PLOT_UPDATE_ALL              0b11111111

#define CV_PLOT_AXIS_FONT_SIZE          14U

namespace cvis{

class CVPlot: public CVTextBox{
protected:

    BYTE updateState;

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
                framesLastChange;

    float animSpriteSpeed,
            spriteSize,
            fontScaling,
            spriteScaling,
            plotBoundaryThickness;

    std::vector<sf::RectangleShape> frontLines;
    std::vector<sf::RectangleShape> backLines;
    std::vector<sf::RectangleShape> gridLines;
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
    sf::Color highlightColor;

    std::vector<std::vector<std::vector<float>>> data; // Numeric info for data entries for each dimension
    std::vector<std::vector<std::vector<std::string>>> tags; // Verbal info for labels for each dimension

    std::string title, fontName;

    const sf::Texture* plotSpriteTexture;
    const sf::Font* plotFont;

    struct axis{

        CVPlot* plot;
        textEntry textInfo;
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
        UINT8 dataType;
        sf::Color lineColor;

        std::vector<sf::Text> renderLabels;

        inline void setLabel(const std::string& newLabel){ title = newLabel; }
        inline void setFontSize(const unsigned int& newSize){
            textInfo.fontSize = newSize;
        }

        inline void updateRenderLabels();
        inline void update();

        inline void removeLabel(const unsigned int& index){
            labels.erase(labels.begin() + index);
            renderLabels.erase(renderLabels.begin() + index);
            axisIntervals.erase(axisIntervals.begin() + index);
        }

        inline void setLabels(const AIALG::StringVector& labels){
            dataType = DATA_TYPE_VERBAL;
            this->labels = labels;
            updateRenderLabels();
        }

        inline float getLabelPosition(const std::string& label) const{
            unsigned int index = 0;
            for(auto& L : labels){
                if(L == label){
                    switch(dimension){
                        case DIMX:{
                            return renderLabels[index].getPosition().x;
                        }
                        case DIMY:{
                            return renderLabels[index].getPosition().y;
                        }
                        default: break;
                    }
                }
                ++index;
            }

            return NAN;
        }

        void getNumericLabels(const unsigned int& numDecimals = 1);

        axis(const float& pos, const unsigned int& dimension, CVPlot* plot,
             const AIALG::StringVector& labels = AIALG::StringVector(),
             const UINT8& dataType = DATA_TYPE_NUMERIC, const float& thickness = 3.0f,
             const float& offset = 0.0f,
             const sf::Color& lineColor= sf::Color::Black,
             const textEntry& textInfo = textEntry("", "Paratype", 16, ALIGN_LEFT, sf::Color::Black),
             const float& textPadding = 5.0f);

    };

    struct Dataset;

    struct dataPoint{

        Dataset* dataset;
        CVPlot* plot;

        bool selected,
            animated,
            showHoverDisplay,
            visible;

        unsigned int dataIndex;

        sf::Sprite sprite;
        std::string name;
        AIALG::StringVector labels;

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
                if(!isnan(coord)) return true;
            }
            return false;
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
                        return std::to_string(dataIndex) + ": [" + AIALG::strDigits(dimValues[DIMX], 3) + ',' + AIALG::strDigits(dimValues[DIMY], 3) + "]";
                    else return AIALG::concatenateString(labels, ",") + ": [" + AIALG::strDigits(dimValues[DIMX], 3) + ',' + AIALG::strDigits(dimValues[DIMY], 3) + "]";
            else if((plot->xAxis().dataType == DATA_TYPE_VERBAL) &&
                (plot->yAxis().dataType == DATA_TYPE_NUMERIC)){
                    return std::to_string(dataIndex) + ": [" + AIALG::strDigits(dimValues[DIMY], 3) + "]\nCategory: " + labels[DIMX];
            }
            else if((plot->xAxis().dataType == DATA_TYPE_NUMERIC) &&
                (plot->yAxis().dataType == DATA_TYPE_VERBAL)){
                    return std::to_string(dataIndex) + ": [" + AIALG::strDigits(dimValues[DIMX], 3) + "]\nCategory: " + labels[DIMY];
            }
            return std::string();
        }

        inline void setSelection(const bool& state){
            if(state){
                if(!selected){
                    sprite.setColor(plot->highlightColor);
                    selected = true;
                    plot->selectedPoints.push_back(this);
                }
            }
            else{
                if(selected){
                    sprite.setColor(spriteColor);
                    selected = false;
                    AIALG::remove(this, plot->selectedPoints);
                }
            }
        }

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

        void update();

        dataPoint(const std::vector<float>& dimValues,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  const AIALG::StringVector& labels = AIALG::StringVector(),
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        dataPoint(const float& x, const float& y,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  const AIALG::StringVector& labels = AIALG::StringVector(),
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        dataPoint(const std::string& xLabel, const float& y,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
        dataPoint(const float& x, const std::string& yLabel,
                  Dataset* dataset, const unsigned int& index = UINT_MAX,
                  bool hover = true,
                  const sf::Vector2f& animSource = sf::Vector2f(NAN, NAN));
    };

    std::vector<dataPoint*> selectedPoints;

    struct DataRequest{
        std::vector<std::vector<float>> data;
        std::vector<std::vector<std::string>> labels;
        std::vector<std::vector<unsigned int>> indices;

        std::string name;
        AIALG::StringVector pointTags;

        DataRequest(const std::vector<float>& xData, const std::vector<float>& yData,
                    const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                    const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data({xData, yData}),
            indices({indices}),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& matrix, const std::vector<unsigned int> indices,
                    const std::vector<std::vector<std::string>>& tags, const std::string& name = "",
                    const AIALG::StringVector& pointTags = AIALG::StringVector()):
                        data(matrix),
                        labels(tags),
                        indices({ indices }),
                        name(name),
                        pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& data,
                    const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                    const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data(data),
            indices({indices}),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const AIALG::StringVector& xLabels, const std::vector<std::vector<float>>& yData,
                    const std::vector<std::vector<unsigned int>>& indices, const std::string& name = "",
                    const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data(yData),
            labels({xLabels}),
            indices(indices),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const std::vector<std::vector<float>>& xData, const AIALG::StringVector& yLabels,
                    const std::vector<std::vector<unsigned int>>& indices, const std::string& name = "",
                    const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data(xData),
            labels({AIALG::StringVector(),yLabels}),
            indices(indices),
            name(name),
            pointTags(pointTags){ }

    };

    struct Dataset{

        CVPlot* plot;

        std::vector<UINT8> dimensionDataTypes;

        std::vector<std::vector<float>> matrix;
        std::vector<std::vector<std::string>> labels;
        std::vector<std::vector<unsigned int>> indices;

        std::vector<unsigned int> subset;

        std::vector<dataPoint> points;
        std::string name;

        sf::Color color;

        inline void setColor(const sf::Color& newColor){
            this->color = newColor;
            for(auto& point : points){
                point.setBaseColor(newColor);
            }
        }

        inline void setDataSubset(const std::vector<unsigned int>& subsetIdx){
            subset = subsetIdx;
        }

        inline void update(){
            size_t L = size();

            std::vector<unsigned int> allIdx;
            for(auto& idx : indices){
                AIALG::append(allIdx, idx);
            }

            for(size_t i = 0; i < L; ++i){
                for(auto& point : points){
                    if(point.dataIndex == allIdx[i]){
                        point.dimValues = AIALG::getCrossSection(i, matrix);
                    }
                }
            }

            for(size_t i = 0; i < L; ++i){
                if(subset.size() > 0){
                    if(!AIALG::anyEqual(points[i].dataIndex, subset)) points[i].visible = false;
                }
                if(plot->subset.size() > 0){
                    if(!AIALG::anyEqual(points[i].dataIndex, plot->subset)) points[i].visible = false;
                }
            }

            plot->updateState |= CV_PLOT_UPDATE_ALL; // Update of data requires update of all else

        }

        inline std::vector<float> getSubsetData(const unsigned int& dimension) const{
            if((plot->subset.size() == 0) && (subset.size() == 0)) return matrix[dimension];

            std::vector<float> output;
            for(auto point : points){
                if(this->subset.size() == 0){
                    if(plot->subset.size() == 0) return matrix[dimension];
                    else if(AIALG::anyEqual(point.dataIndex, plot->subset)){
                        output.push_back(point[dimension]);
                    }
                }
                else{
                    if(plot->subset.size() == 0) return matrix[dimension];
                    else if(AIALG::anyEqual(point.dataIndex, plot->subset) && AIALG::anyEqual(point.dataIndex, this->subset)){
                        output.push_back(point[dimension]);
                    }
                }
            }

            return output;
        }

        inline std::vector<dataPoint*> getSubsetPoints(){
            std::vector<dataPoint*> output;
            if((plot->subset.size() == 0) && (subset.size() == 0)){
                output.reserve(this->size());
                for(auto& point : points){
                    output.push_back(&point);
                }
                return output;
            }

            output.reserve(plot->subset.size());

            for(auto& point : points){
                if(this->subset.size() < 1){
                    if(AIALG::anyEqual(point.dataIndex, plot->subset)){
                        output.push_back(&point);
                    }
                }
                else{
                    if(AIALG::anyEqual(point.dataIndex, plot->subset) && AIALG::anyEqual(point.dataIndex, this->subset)){
                        output.push_back(&point);
                    }
                }
            }

            return output;
        }

        inline std::vector<dataPoint*> getLabelledPoints(const std::string& tag){
            std::vector<dataPoint*> output;
            for(auto& point : points){
                if(AIALG::anyEqual(tag, point.labels)){
                    if((plot->subset.size() > 0) && !AIALG::anyEqual(point.dataIndex, plot->subset)) continue;
                    output.push_back(&point);
                }
            }
            return output;
        }

        inline std::vector<float> getLabelledData(const std::string& tag, const unsigned int& dimension){
            std::vector<float> output;
            for(auto& point : points){
                if(AIALG::anyEqual(tag, point.labels)){
                    if((plot->subset.size() > 0) && !AIALG::anyEqual(point.dataIndex, plot->subset)) continue;
                    output.push_back(point[dimension]);
                }
            }
            return output;
        }

        inline unsigned int numDimensions() const{ return matrix.size(); }

        inline sf::Vector2f getScale(const unsigned int& dimension) const{
            if(dimension >= matrix.size()) return sf::Vector2f(NAN, NAN);
            if(dimensionDataTypes[dimension] != DATA_TYPE_NUMERIC) return sf::Vector2f(NAN, NAN);

            if((plot->subset.size() > 0) || (subset.size() > 0)){
                std::vector<float> subsetData = getSubsetData(dimension);
                return sf::Vector2f(AIALG::min(subsetData), AIALG::max(subsetData));
            }

            return sf::Vector2f(AIALG::min(matrix[dimension]), AIALG::max(matrix[dimension])); }

        inline sf::Vector2f getXScale() const{ return getScale(DIMENSION_X); }
        inline sf::Vector2f getYScale() const{ return getScale(DIMENSION_Y); }
        inline sf::Vector2f getZScale() const{ return getScale(DIMENSION_Z); }

        inline std::vector<float>& x(){ return matrix[DIMX]; }
        inline std::vector<float>& y(){ return matrix[DIMY]; }
        inline std::vector<float>& z(){ return matrix[DIMZ]; }

        inline size_t size() const{ return AIALG::minSize(matrix); }

        Dataset(const std::vector<std::vector<float>>& matrix,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const AIALG::StringVector& pointTags,
                CVPlot* plot);
        Dataset(const std::vector<float>& x, const std::vector<float>& y,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const AIALG::StringVector& pointTags,
                CVPlot* plot);
        Dataset(const AIALG::StringVector& xLabels, const std::vector<std::vector<float>>& y,
                const std::vector<std::vector<unsigned int>>& indices, const std::string& name, CVPlot* plot);
        Dataset(const std::vector<std::vector<float>>& x, const AIALG::StringVector& yLabels,
                const std::vector<std::vector<unsigned int>>& indices, const std::string& name, CVPlot* plot);

    };

    std::vector<Dataset> datasets;
    std::vector<axis> plotAxes;
    std::vector<DataRequest> dataAddRequests;

    inline CVTypeBox& titleBox(){ return axisLabels.front(); }
    inline CVTypeBox& xLabelBox(){ return axisLabels[1]; }
    inline CVTypeBox& yLabelBox(){ return axisLabels[2]; }

    inline float areaScale() const{ return (plotBounds.width+plotBounds.height)/512; }
    inline float heightScale() const{ return plotBounds.height/256; }
    inline float widthScale() const{ return plotBounds.width/256; }
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
                if(!isnan(dataDims.x)){
                    if(isnan(dimScales[i].x)) dimScales[i].x = dataDims.x;
                    else if(dataDims.x < dimScales[i].x) dimScales[i].x = dataDims.x;
                }
                if(!isnan(dataDims.y)){
                    if(isnan(dimScales[i].y)) dimScales[i].y = dataDims.y;
                    else if(dataDims.y > dimScales[i].y) dimScales[i].y = dataDims.y;
                }
            }
        }
    }

    void updateBounds();

public:

    bool draw();
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    inline void callUpdate(const BYTE& reqState = CV_PLOT_UPDATE_ALL){
        updateState |= reqState;
        this->framesLastChange = 0; }

    void get_numeric_x_axis();
    void get_numeric_y_axis();

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

    inline void setHighlightColor(const sf::Color& newColor){
        highlightColor = newColor;
    }

    inline void setScaleAuto(const std::vector<float>& states){
        for(size_t i = 0; i < states.size(); ++i){
            if(i >= scaleAutoPadding.size()) scaleAutoPadding.emplace_back(states[i]);
            else scaleAutoPadding[i] = states[i];
        }
    }

    inline const CVTypeBox& mainTitle() const{ return axisLabels.front(); }
    inline const CVTypeBox& xLabel() const{ return axisLabels[1]; }
    inline const CVTypeBox& yLabel() const{ return axisLabels[2]; }

    void get_verbal_x_axis();
    void get_verbal_y_axis();

    void removeDataOutliers();

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

    inline const sf::Color& getFillColor() const{ return colorTheme[0]; }
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

    inline bool setPlotSpriteTexture(const std::string& textureName){
        const sf::Texture* txtCheck = View->mainApp->bitmaps.taggedTexture(textureName);
        if(txtCheck == nullptr) return false;
        else plotSpriteTexture = txtCheck;
        return true;
    }

    void addAxis(const float& pos, const unsigned int& dimension,
                 const UINT8& dataType, const float& thickness = 5.0f,
                 const float& offset = 0.0f,
                 const sf::Color& lineColor= sf::Color::Black,
                 const unsigned int& fontSize = 16,
                 const UINT8& alignment = ALIGN_LEFT,
                 const sf::Color& fontColor = sf::Color::Black,
                 std::string fontName = "",
                 const float& textPadding = 5.0f){
        if(fontName.size() < 1) fontName = this->fontName;
        plotAxes.emplace_back(pos, dimension, this, AIALG::StringVector(), dataType, thickness, offset, lineColor,
                              textEntry("", fontName, fontSize, alignment, fontColor),
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

    inline void addData(const std::vector<float>& xData, const std::vector<float>& yData,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector()){
        dataAddRequests.emplace_back(xData, yData, indices, name, pointTags);
    }
    inline void addData(const AIALG::StringVector& xLabels, const std::vector<std::vector<float>>& yData,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector()){
        dataAddRequests.emplace_back(xLabels, yData, indices, name, pointTags);
    }
    inline void addData(const std::vector<std::vector<float>>& xData, const AIALG::StringVector& yLabels,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = ""){
        dataAddRequests.emplace_back(xData, yLabels, indices, name);
    }
    inline void addData(const std::vector<std::vector<float>>& matrix,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const std::vector<std::vector<std::string>>& labels =
                                std::vector<std::vector<std::string>>(),
                        const std::string& name = ""){
        dataAddRequests.emplace_back(matrix, indices, labels, name);
    }

    inline void addTag(const unsigned int dimension, const unsigned int index, const std::string& newTag){
        tags[dimension].insert(tags[dimension].begin() + index, std::vector<std::string>({ newTag }));
    }

    inline bool select_tags(const AIALG::StringVector& tags){
        if(datasets.size() < 1) return false;

        bool output = false;
        for(auto& set : datasets){
            for(auto& point : set.points){
                if(AIALG::cmpStringToList(tags, point.labels, CMP_STR_MATCH)){
                    point.setSelection(true);
                    output = true;
                }
                else point.setSelection(false);
            }
        }

        callUpdate(CV_PLOT_UPDATE_DRAW);

        return output;
    }

    inline bool select_indices(const std::vector<unsigned int>& idx){
        if(datasets.size() < 1) return false;

        bool output = false;
        for(auto& set : datasets){
            for(auto& point : set.points){
                if(AIALG::anyEqual(point.dataIndex, idx)){
                    point.setSelection(true);
                    output = true;
                }
                else point.setSelection(false);
            }
        }

        callUpdate(CV_PLOT_UPDATE_DRAW);

        return output;
    }

    inline void removeData(const unsigned int dimension, const unsigned int index){
        data[dimension].erase(data[dimension].begin() + index);
        tags[dimension].erase(tags[dimension].begin() + index);
        bReqPlotUpdate = true;
        bReqAxisUpdate = true;
    }
    inline void removeData(const unsigned int dimension,
                           const unsigned int begin,
                           const unsigned int end){
        data[dimension].erase(data[dimension].begin() + begin, data[dimension].end() + end);
        tags[dimension].erase(tags[dimension].begin() + begin, tags[dimension].end() + end);
        bReqPlotUpdate = true;
        bReqAxisUpdate = true;
    }

    inline void clearData(){
        data.clear();
        tags.clear();
        dimScales.clear();
    }
    inline void clearData(const unsigned int dimension){
        data[dimension].clear();
        tags[dimension].clear();
        dimScales.erase(dimScales.begin() + dimension);
    }

    friend std::ostream& operator<<(std::ostream& output, const CVPlot& plot){
        unsigned int D = plot.numDimensions();
        for(size_t i = 0; i < plot.datasets.size(); ++i){
            if((plot.axisLabels.size() > 0) && (plot.axisLabels.front().getTypeString().size() > 0)){
                output << plot.axisLabels.front().getTypeString() << '\n';
            }
            output << "Set\t" << i << "\n";
            output << "Index\t";
            for(size_t j = 0; j < plot.numDimensions(); ++j){
                switch(j){
                    case 0:{
                        if(plot.axisLabels.size() > 1) output << plot.axisLabels[1].getTypeString() << '\t';
                        else output << "x\t";
                        break;
                    }
                    case 1:{
                        if(plot.axisLabels.size() > 2) output << plot.axisLabels[2].getTypeString() << '\t';
                        else output << "y\t";
                        break;
                    }
                    case 2:{
                        if(plot.axisLabels.size() > 3) output << plot.axisLabels[3].getTypeString() << '\t';
                        else output << "z\t";
                        break;
                    }
                    default:{
                        output << "Dimension " << j << '\t';
                        break;
                    }
                }
            }
            output << '\n';

            for(auto& point : plot.datasets[i].points){
                if(point.visible){
                    output << point.dataIndex << '\t';
                    for(size_t j = 0; j < D; ++j){
                        output << point.dimValues[j];
                        if(j < D-1) output << '\t';
                    }
                    output << '\n';
                }
            }

            if(i < plot.datasets.size() - 1) output << '\n';
        }

        return output;
    }

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

    void move(const sf::Vector2f& distance);
    void setPosition(const sf::Vector2f& distance);

    inline void setPlotColors(const ColorTheme& newTheme){ this->plotColors = newTheme; }

    inline sf::Vector2f getPlotPos(const sf::Vector2f& dataPoint){ // Turn scale units to view draw coordinates

        if((scalePadding.size() < 2) || (dimScales.size() < 2)) return sf::Vector2f(NAN, NAN);

        float scaleMinX = dimScales[DIMX].x,
                scaleMaxX = dimScales[DIMX].y,
                scaleMinY = dimScales[DIMY].x,
                scaleMaxY = dimScales[DIMY].y,
                dataRangeX = scaleMaxX - scaleMinX,
                dataRangeY = scaleMaxY - scaleMinY;

        scaleMinX -= dataRangeX*scalePadding[DIMX].x;
        scaleMinY -= dataRangeY*scalePadding[DIMY].x;

        dataRangeX *= (1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y);
        dataRangeY *= (1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y);

        sf::Vector2f output;
        if(dataRangeX == 0.0f){
            output.x = plotBounds.left + plotBounds.width/2;
        }
        else output.x = plotBounds.left +
                            plotBounds.width*(dataPoint.x - scaleMinX)/
                            (dataRangeX);

        if(dataRangeY == 0.0f){
            output.y = plotBounds.top + plotBounds.height/2;
        }
        else output.y = plotBounds.top + plotBounds.height*(1.0f-
                            (dataPoint.y - scaleMinY)/dataRangeY);

        return output;

    }
    inline sf::Vector2f getPlotPos(const float& x, const float& y){
        return getPlotPos(sf::Vector2f(x, y));
    }
    inline float getPlotYPos(const float& dataY){
        float scaleRange = dimScales[DIMY].y - dimScales[DIMY].x;
        if(scaleRange == 0.0f) return plotBounds.top + plotBounds.height/2;

        return plotBounds.top + plotBounds.height*(1.0f-
                     (dataY - dimScales[DIMY].x + scaleRange*scalePadding[DIMY].x)/
                     (scaleRange*(1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y)));
    }
    inline float getPlotXPos(const float& dataX){
        float scaleRange = dimScales[DIMX].y - dimScales[DIMX].x;
        if(scaleRange == 0.0f) return plotBounds.left + plotBounds.width/2;

        return plotBounds.left + plotBounds.width*
                     (dataX - dimScales[DIMX].x + scaleRange*scalePadding[DIMX].x)/
                     (scaleRange*(1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y));
    }

    inline sf::Vector2f getScalePos(const sf::Vector2f& pos){ // Turn view window coordinates to data scale coordinates
        float scaleMinX = dimScales[DIMX].x,
                scaleMaxX = dimScales[DIMX].y,
                scaleMinY = dimScales[DIMY].x,
                scaleMaxY = dimScales[DIMY].y,
                dataRangeX = scaleMaxX - scaleMinX,
                dataRangeY = scaleMaxY - scaleMinY;

        scaleMinX -= dataRangeX*scalePadding[DIMX].x;
        scaleMinY -= dataRangeY*scalePadding[DIMY].x;

        dataRangeX *= (1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y);
        dataRangeY *= (1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y);

        sf::Vector2f output;

        if(dataRangeX == 0.0f){
            output.x = scaleMinX;
        }
        else output.x = (pos.x - plotBounds.left)/plotBounds.width*dataRangeX + scaleMinX;

        if(dataRangeY == 0.0f){
            output.y = scaleMinY;
        }
        else output.y = (plotBounds.top + plotBounds.height - pos.y)/plotBounds.height*dataRangeY + scaleMinY;

        return output;

    }

    inline float getScaleXPos(const float& pos){
        float scaleRange = dimScales[DIMX].y - dimScales[DIMX].x;
        if(scaleRange == 0.0f) return dimScales[DIMX].x;

        return (pos - plotBounds.left)/
        plotBounds.width*scaleRange*(1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y) +
        dimScales[DIMX].x - scaleRange*scalePadding[DIMX].x;
    }

    inline float getScaleYPos(const float& pos){
        float scaleRange = dimScales[DIMY].y - dimScales[DIMY].x;
        if(scaleRange == 0.0f) return dimScales[DIMY].x;

        return (plotBounds.top + plotBounds.height - pos)/
        plotBounds.height*scaleRange*(1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y) +
        dimScales[DIMY].x - scaleRange*scalePadding[DIMY].x;
    }

    CVPlot(view* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor = sf::Color::Transparent,
           const sf::Color& borderColor = sf::Color::Black,
           const sf::Color& plotColor = sf::Color(128,128,128),
           const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
           const std::string& fontName = "Paratype");

    virtual ~CVPlot(){ }

};

inline UINT8 parse_regression_type(const std::string& type){
    if(AIALG::cmpString(type, "pearson")){
        return CV_LINE_REGRESSION_PEARSON;
    }
    if(AIALG::cmpString(type, "spearman")){
        return CV_LINE_REGRESSION_SPEARMAN;
    }
    return CV_LINE_REGRESSION_NONE;
}

class CVScatterPlot: public CVPlot{
protected:

    std::string quadrantType;
    bool colorQuadrants;
    ColorTheme quadrantColors;

    UINT8 regressionType;

    struct RegLineInfo{

        CVScatterPlot* host;

        float coefficient,
                slope,
                intercept;

        void getLinearModel(const std::vector<float>& xData, const std::vector<float>& yData){
            switch(host->regressionType){
                case CV_LINE_REGRESSION_SPEARMAN:{
                    coefficient = AIALG::spearman_coefficient(xData, yData);
                    break;
                }
                default:{ // Pearson
                    coefficient = AIALG::pearson_coefficient(xData, yData);
                    break;
                }
            }

            AIALG::least_squares_fit(xData, yData, slope, intercept);
        }

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

    inline void setRegressionLine(const UINT8& state){
        regressionType = state;
        callUpdate(CV_PLOT_UPDATE_DRAW);
    }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVScatterPlot(view* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype",
                   const std::string& quadrantType = "");

};

class CVCategoryPlot: public CVPlot{
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

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

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

    CVCategoryPlot(view* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype");

};

class CVLinePlot: public CVPlot{
protected:

    UINT8 lineType,
            connectionType;
    bool showSprites;

public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVLinePlot(view* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype",
                   const UINT8& lineType = CV_LINE_TYPE_SOLID,
                   const UINT8& connectionType = CV_LINE_CONN_DIRECT);


};

class CVPlotPanel: public CVViewPanel{
protected:

    std::vector<sf::RectangleShape> highlightBlocks;
    sf::Color highlightColor;
    std::vector<CVPlot*> selectedPlots;
    sf::Sprite logo;

    unsigned int logoAlpha;

public:

    std::vector<unsigned int> getAllSelectedIndices(); // Get all indices selected in plots
    std::vector<unsigned int> getCommonSelectedIndices(); // Get indices commonly selected in plots

    inline unsigned int numSelected() const{ return selectedPlots.size(); }
    bool select_tags(const std::vector<std::string>& tags);
    bool select_indices(const std::vector<unsigned int>& idx);

    inline const std::vector<CVPlot*>& getSelectedPlots() const{ return selectedPlots; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw();

    CVPlotPanel(view* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Color& highlightColor = sf::Color(160,207,255,128)):
        CVViewPanel(parentView, panelTag, backgroundColor),
        highlightColor(highlightColor),
        logo(*parentView->mainApp->bitmaps.taggedTexture("CVPlot_logo")),
        logoAlpha(80){
            sf::FloatRect logoBounds = logo.getGlobalBounds();
            logo.setOrigin(logoBounds.width/2, logoBounds.height/2);
            logo.setPosition(bounds.left + bounds.width/2, bounds.top + bounds.height/2);
            logo.setScale((bounds.width + bounds.height)/(2.0f*(logoBounds.width + logoBounds.height)),
                          (bounds.width + bounds.height)/(2.0f*(logoBounds.width + logoBounds.height)));
            logo.setColor(sf::Color(255,255,255,0));
        }
};

}

#endif // CVIS_PLOT
