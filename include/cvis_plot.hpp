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

#define CV_CLUSTER_NONE                 0_BIT
#define CV_CLUSTER_HIERARCHICAL         1_BIT

#define CV_HEAT_SCALE_ABS               0_BIT
#define CV_HEAT_SCALE_REL_ALL           1_BIT
#define CV_HEAT_SCALE_REL_ROW           2_BIT
#define CV_HEAT_SCALE_REL_COL           3_BIT

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

namespace cvis{

class CVPlot: public CVTextBox{
protected:

    AIALG::BYTE updateState;

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

    std::vector<AIALG::numMatrix> data; // Numeric info for data entries for each dimension
    std::vector<AIALG::StringMatrix> tags; // Verbal info for labels for each dimension

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
        AIALG::UINT8 dataType;
        sf::Color lineColor;

        std::vector<sf::Text> renderLabels;

        inline void setLabel(const std::string& newLabel){ title = newLabel; }
        inline void setFontSize(const unsigned int& newSize){
            textInfo.fontSize = newSize;
        }
        void setLabelColor(const sf::Color& newColor);
        void setLineColor(const sf::Color& newColor);

        void updateRenderLabels();
        void update();

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
             const AIALG::UINT8& dataType = DATA_TYPE_NUMERIC, const float& thickness = 3.0f,
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
                    sprite.setColor(plot->plotHighlightColor);
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
        AIALG::numMatrix data;
        AIALG::StringMatrix labels;
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

        DataRequest(const AIALG::numMatrix& matrix, const std::vector<unsigned int> indices,
                    const AIALG::StringMatrix& tags, const std::string& name = "",
                    const AIALG::StringVector& pointTags = AIALG::StringVector()):
                        data(matrix),
                        labels(tags),
                        indices({ indices }),
                        name(name),
                        pointTags(pointTags){ }

        DataRequest(const AIALG::numMatrix& data,
                    const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                    const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data(data),
            indices({indices}),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const AIALG::StringVector& xLabels, const AIALG::numMatrix& yData,
                    const std::vector<std::vector<unsigned int>>& indices, const std::string& name = "",
                    const AIALG::StringVector& pointTags = AIALG::StringVector()):
            data(yData),
            labels({xLabels}),
            indices(indices),
            name(name),
            pointTags(pointTags){ }

        DataRequest(const AIALG::numMatrix& xData, const AIALG::StringVector& yLabels,
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

        std::vector<AIALG::UINT8> dimensionDataTypes;

        AIALG::numMatrix matrix;
        AIALG::StringMatrix labels;
        std::vector<std::vector<unsigned int>> indices;

        std::vector<unsigned int> subset;

        std::vector<dataPoint> points;
        std::string name;

        void rm_dimension(const unsigned int& dimension);

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

        Dataset(const AIALG::numMatrix& matrix,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const AIALG::StringVector& pointTags,
                CVPlot* plot);
        Dataset(const std::vector<float>& x, const std::vector<float>& y,
                const std::vector<unsigned int>& indices,
                const std::string& name,
                const AIALG::StringVector& pointTags,
                CVPlot* plot);
        Dataset(const AIALG::StringVector& xLabels, const AIALG::numMatrix& y,
                const std::vector<std::vector<unsigned int>>& indices, const std::string& name, CVPlot* plot);
        Dataset(const AIALG::numMatrix& x, const AIALG::StringVector& yLabels,
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



    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    inline void callUpdate(const AIALG::BYTE& reqState = CV_PLOT_UPDATE_ALL){
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
                 const AIALG::UINT8& dataType, const float& thickness = 5.0f,
                 const float& offset = 0.0f,
                 const sf::Color& lineColor= sf::Color::Black,
                 const unsigned int& fontSize = 16,
                 const AIALG::UINT8& alignment = ALIGN_LEFT,
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

    void addData(const std::vector<float>& xData, const std::vector<float>& yData,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector());
    void addData(const AIALG::StringVector& xLabels, const AIALG::numMatrix& yData,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = "", const AIALG::StringVector& pointTags = AIALG::StringVector());
    void addData(const AIALG::numMatrix& xData, const AIALG::StringVector& yLabels,
                        const std::vector<std::vector<unsigned int>>& indices = std::vector<std::vector<unsigned int>>(),
                        const std::string& name = "");
    void addData(const AIALG::numMatrix& matrix,
                        const std::vector<unsigned int>& indices = std::vector<unsigned int>(),
                        const AIALG::StringMatrix& labels =
                                AIALG::StringMatrix(),
                        const std::string& name = "");

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

    void removeData(const unsigned int dimension, const unsigned int index);
    void removeData(const unsigned int dimension,
                   const unsigned int begin,
                   const unsigned int end);

    void clearData();
    void clearData(const unsigned int dimension);

    friend std::ostream& operator<<(std::ostream& output, const CVPlot& plot);

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

    void setAxisLineColor(const sf::Color& newColor);
    void setAxisLabelColor(const sf::Color& newColor);
    void setAxisColor(const sf::Color& newColor);

    inline std::string getTitle(){
        if(axisLabels.size() > 0) return titleBox().getTypeString();
        else return std::string(); }
    inline std::string getXLabel(){
        if(axisLabels.size() > 1) return xLabelBox().getTypeString();
        else return std::string(); }
    inline std::string getYLabel(){
        if(axisLabels.size() > 2) return yLabelBox().getTypeString();
        else return std::string(); }

    inline void addLine(const AIALG::UINT8& orientation, const float& dimValue,
                        const float& thickness = 1.0f, const sf::Color& color = sf::Color::Black,
                        const AIALG::UINT8& type = CV_LINE_TYPE_SOLID, bool foreground = false);

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setPosition(const sf::Vector2f& distance);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    inline void setPlotColors(const ColorTheme& newTheme){ this->plotColors = newTheme; }

    sf::Vector2f getPlotPos(const sf::Vector2f& dataPoint); // Turn scale units to view draw coordinates
    sf::Vector2f getPlotPos(const float& x, const float& y);
    float getPlotYPos(const float& dataY);
    float getPlotXPos(const float& dataX);

    sf::Vector2f getScalePos(const sf::Vector2f& pos); // Turn view window coordinates to data scale coordinates

    float getScaleXPos(const float& pos);
    float getScaleYPos(const float& pos);

    CVPlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor = sf::Color::Transparent,
           const sf::Color& borderColor = sf::Color::Black,
           const sf::Color& plotColor = sf::Color(128,128,128),
           const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
           const std::string& fontName = "Paratype");

    virtual ~CVPlot(){ }

};

inline AIALG::UINT8 parse_regression_type(const std::string& type){
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

    AIALG::UINT8 regressionType;

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

    inline void setRegressionLine(const AIALG::UINT8& state){
        regressionType = state;
        callUpdate(CV_PLOT_UPDATE_DRAW);
    }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVScatterPlot(CVView* View, const sf::Vector2f& position, const float& width,
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
    AIALG::numMatrix sigMatrix;
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

    CVCategoryPlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype");

};

class CVLinePlot: public CVPlot{
protected:

    AIALG::UINT8 lineType,
            connectionType;
    bool showSprites;

public:



    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVLinePlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "grey_circle",
                   const std::string& fontName = "Paratype",
                   const AIALG::UINT8& lineType = CV_LINE_TYPE_SOLID,
                   const AIALG::UINT8& connectionType = CV_LINE_CONN_DIRECT);


};

class CVHeatPlot: public CVPlot{
protected:

    AIALG::UINT8 clusterType,
                scaleType;

public:



    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVHeatPlot(CVView* View, const sf::Vector2f& position, const float& width,
                  const float& height,
                   const sf::Color& fillColor = sf::Color::Transparent,
                   const sf::Color& borderColor = sf::Color::Black,
                   const sf::Color& plotColor = sf::Color(60,60,60),
                   const float& borderWidth = 0.0f, const std::string& plotSprite = "basic_square",
                   const std::string& fontName = "Paratype",
                   const AIALG::UINT8& clusterType = CV_CLUSTER_HIERARCHICAL,
                   const AIALG::UINT8& scaleType = CV_HEAT_SCALE_REL_ROW);
};

class CVPlotPanel: public CVViewPanel{
protected:

    std::vector<sf::RectangleShape> highlightBlocks;
    sf::Color plotHighlightColor;
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
    bool draw(sf::RenderTarget* target);

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVPlotPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Color& plotHighlightColor = sf::Color(160,207,255,128),
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                const bool& bFitWindow = true, const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f)):
        CVViewPanel(parentView, panelTag, backgroundColor, size, bFitWindow, position),
        plotHighlightColor(plotHighlightColor),
        logo(*parentView->mainApp->bitmaps.taggedTexture("CVPlot_logo")),
        logoAlpha(80){

            sf::FloatRect logoBounds = logo.getGlobalBounds();
            logo.setOrigin(logoBounds.width/2, logoBounds.height/2);
            logo.setPosition(bounds.left + bounds.width/2, bounds.top + bounds.height/2);
            logo.setScale((bounds.width + bounds.height)/(3.0f*(logoBounds.width + logoBounds.height)),
                          (bounds.width + bounds.height)/(3.0f*(logoBounds.width + logoBounds.height)));
            logo.setColor(sf::Color(255,255,255,0));
        }
};

}

#endif // CVIS_PLOT
