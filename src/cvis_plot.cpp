//
// CVision: a multi-platform graphics interface libary for C++
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
// Redistribution of CVision is permitted under accordance with
// the GNU general public licence (GPL) version 3.0 and the
// terms and conditions specified in CVlicence.txt

#include "cvision/cvis_plot.hpp"

#include "algorithm.hpp"

namespace cvis{

using namespace AIALG;

CVPlot::CVPlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor, const sf::Color& borderColor,
           const sf::Color& plotColor, const float& borderWidth, const std::string& plotSprite,
           const std::string& fontName):
               CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
               updateState(CV_PLOT_UPDATE_NONE),
               bReqPlotUpdate(true),
               bReqAxisUpdate(true),
               bUseVertGrid(false),
               bUseHorizGrid(false),
               bAnimSprites(true),
               bShowSprites(true),
               bPlotBoundary(false),
               plotTypeID(CV_PLOT_ID_NONE),
               dataPointNum(0),
               framesLastChange(0),
               labelFontSize(16),
               animSpriteSpeed(400.0f),
               spriteSize(5.0f),
               spriteScaling(1.0f),
               plotBoundaryThickness(2.0f),
               plotColors({ plotColor }),
               plotHighlightColor(sf::Color::Red),
               axisTextColor(borderColor),
               axisLineColor(borderColor),
               fontName(fontName),
               plotBounds(bounds.left + bounds.width*0.05f, bounds.top + bounds.height*0.05f,
                            bounds.width*0.9f, bounds.height*0.9f),
               plotSpriteTexture(View->mainApp->bitmaps.taggedTexture(plotSprite)),
               plotFont(View->mainApp->fonts[fontName]){

        setClearFill();

        sf::Glyph refLetter = View->mainApp->fonts[mainApp()->getDefaultFont()]->getGlyph(65, 12, false, 0),
                testLetter = View->mainApp->fonts[fontName]->getGlyph(65, 12, false, 0);

        fontScaling = (refLetter.bounds.height*refLetter.bounds.width)/(testLetter.bounds.height*testLetter.bounds.width);

        plotBoundary.setSize(sf::Vector2f(plotBounds.width, plotBounds.height));
        plotBoundary.setOutlineThickness(plotBoundaryThickness);
        plotBoundary.setOutlineColor(borderColor);
        plotBoundary.setFillColor(sf::Color::Transparent);
        plotBoundary.setPosition(plotBounds.left, plotBounds.top);

        colorTheme.emplace_back(plotColor);

        axisLabels.emplace_back(View, sf::Vector2f(plotBounds.left + plotBounds.width/2,
                                                   plotBounds.top - plotBounds.height*0.25f),
                                plotBounds.width, 30.0f,
                                textEntry("Main Title", fontName, 22*fontScaling,
                                          ALIGN_CENTER_MIDLINE, borderColor, sf::Text::Style::Bold),
                                sf::Color::Transparent, sf::Color::Transparent, 0.0f, CV_OBJ_ANIM_SLIDE);
        axisLabels.emplace_back(View, sf::Vector2f(plotBounds.left + plotBounds.width/2,
                                                   plotBounds.top + plotBounds.height*1.2f),
                                plotBounds.width, 30.0f,
                                textEntry("x Label", fontName, 22*fontScaling,
                                          ALIGN_CENTER_MIDLINE, borderColor, sf::Text::Style::Bold),
                                sf::Color::Transparent, sf::Color::Transparent, 0.0f, CV_OBJ_ANIM_SLIDE);
        axisLabels.emplace_back(View, sf::Vector2f(plotBounds.left - plotBounds.width*0.3f,
                                                   plotBounds.top + plotBounds.height/2),
                                plotBounds.width, 30.0f, textEntry("y Label", fontName, 22*fontScaling,
                                                                   ALIGN_VERTICAL_INVERTED, borderColor),
                                sf::Color::Transparent, sf::Color::Transparent, 0.0f, CV_OBJ_ANIM_SLIDE);

}

CVPlot::Dataset::Dataset(const std::vector<std::vector<float>>& matrix, const std::vector<unsigned int>& indices,
                         const std::string& name, const StringVector& pointTags, CVPlot* plot):
    plot(plot),
    dimensionDataTypes(matrix.size(), DATA_TYPE_NUMERIC),
    matrix(matrix),
    indices({indices}),
    name(name),
    colors({plot->plotColor(plot->datasets.size())}){

    if(indices.size() < 1) this->indices.front() = vseq(0U, size() - 1);

    sf::Vector2f matrixScale;

    while(plot->dimScales.size() < matrix.size()) plot->dimScales.emplace_back(NAN, NAN);
    while(plot->scalePadding.size() < matrix.size()) plot->scalePadding.emplace_back(0.0f,0.0f);
    while(plot->scaleAutoPadding.size() < matrix.size()) plot->scaleAutoPadding.push_back(false);

    for(size_t i = 0; i < matrix.size(); ++i){
        matrixScale.x = min(matrix[i]);
        matrixScale.y = max(matrix[i]);

        if(isnan(plot->dimScales[i].x) || (matrixScale.x < plot->dimScales[i].x)) plot->dimScales[i].x = matrixScale.x;
        if(isnan(plot->dimScales[i].y) || (matrixScale.y > plot->dimScales[i].y)) plot->dimScales[i].y = matrixScale.y;
    }

    size_t L = size(), D = matrix.size();
    std::vector<float> dimValues(matrix.size(), NAN);

    for(size_t i = 0; i < L; ++i){
        for(size_t j = 0; j < D; ++j){
            dimValues[j] = matrix[j][i];
            if(isnan(dimValues[j])) goto nextPoint;
        }

        if(i < pointTags.size()){
            points.emplace_back(dimValues, this, this->indices.front()[i], StringVector({ pointTags[i] }));
        }
        else{
            points.emplace_back(dimValues, this, this->indices.front()[i]);
        }

        nextPoint:;
    }

    plot->callUpdate();
}

CVPlot::Dataset::Dataset(const std::vector<float>& x, const std::vector<float>& y,
                         const std::vector<unsigned int>& indices,
                         const std::string& name, const StringVector& pointTags,
                        CVPlot* plot):
    plot(plot),
    dimensionDataTypes(2, DATA_TYPE_NUMERIC),
    matrix({x, y}),
    indices({indices}),
    name(name),
    colors({plot->plotColor(plot->datasets.size())}){

    if(indices.size() < 1) this->indices.front() = vseq(0U, size()-1);
    while(plot->scaleAutoPadding.size() < matrix.size()) plot->scaleAutoPadding.push_back(false);

    sf::Vector2f matrixScale;
    matrixScale.x = min(x);
    matrixScale.y = max(x);

    while(plot->scalePadding.size() < 2) plot->scalePadding.emplace_back(0.0f, 0.0f);
    while(plot->dimScales.size() < 2) plot->dimScales.emplace_back(NAN,NAN);

    if(isnan(plot->dimScales[DIMX].x) || (matrixScale.x < plot->dimScales[DIMX].x)) plot->dimScales[DIMX].x = matrixScale.x;
    if(isnan(plot->dimScales[DIMX].y) || (matrixScale.y > plot->dimScales[DIMX].y)) plot->dimScales[DIMX].y = matrixScale.y;

    matrixScale.x = min(y);
    matrixScale.y = max(y);

    if(isnan(plot->dimScales[DIMY].x) || (matrixScale.x < plot->dimScales[DIMY].x)) plot->dimScales[DIMY].x = matrixScale.x;
    if(isnan(plot->dimScales[DIMY].y) || (matrixScale.y > plot->dimScales[DIMY].y)) plot->dimScales[DIMY].y = matrixScale.y;

    size_t L = size();
    for(size_t i = 0; i < L; ++i){
        if(isnan(x[i]) || isnan(y[i])) continue;

        if(i < pointTags.size()){
            points.emplace_back(x[i], y[i], this, this->indices.front()[i], StringVector({ pointTags[i] }));
        }
        else{
            points.emplace_back(x[i], y[i], this, this->indices.front()[i]);
        }
    }

    plot->callUpdate();

}

CVPlot::Dataset::Dataset(const StringVector& xLabels, const std::vector<std::vector<float>>& y,
                         const std::vector<std::vector<unsigned int>>& indices,
                         const std::string& name,
                        CVPlot* plot):
    plot(plot),
    dimensionDataTypes({DATA_TYPE_VERBAL, DATA_TYPE_NUMERIC}),
    matrix(2),
    labels({ xLabels }),
    indices(indices),
    name(name),
    colors({plot->plotColor(plot->datasets.size())}){

    while(plot->scalePadding.size() < 2) plot->scalePadding.emplace_back(0.0f, 0.0f);
    while(plot->dimScales.size() < 2) plot->dimScales.emplace_back(NAN,NAN);
    while(plot->scaleAutoPadding.size() < matrix.size()) plot->scaleAutoPadding.push_back(false);

    sf::Vector2f matrixScale;

    matrixScale.x = min(y);
    matrixScale.y = max(y);

    plot->dimScales[DIMX].x = 0.0f;
    plot->dimScales[DIMX].y = 1.0f;

    if(isnan(plot->dimScales[DIMY].x) || (matrixScale.x < plot->dimScales[DIMY].x)) plot->dimScales[DIMY].x = matrixScale.x;
    if(isnan(plot->dimScales[DIMY].y) || (matrixScale.y > plot->dimScales[DIMY].y)) plot->dimScales[DIMY].y = matrixScale.y;

    for(size_t i = 0; i < y.size(); ++i){
        append(matrix[DIMY], y[i]);
        for(size_t j = 0; j < y[i].size(); ++j){
            if(indices.size() > 0) points.emplace_back(xLabels[i], y[i][j], this, indices[i][j]);
            else points.emplace_back(xLabels[i], y[i][j], this, UINT_MAX);
        }
    }

    plot->callUpdate();

}

CVPlot::Dataset::Dataset(const std::vector<std::vector<float>>& x, const StringVector& yLabels,
                         const std::vector<std::vector<unsigned int>>& indices,
                         const std::string& name,
                        CVPlot* plot):
    plot(plot),
    dimensionDataTypes({DATA_TYPE_NUMERIC, DATA_TYPE_VERBAL}),
    matrix(1),
    labels({ yLabels }),
    indices(indices),
    name(name),
    colors({plot->plotColor(plot->datasets.size())}){

    while(plot->scalePadding.size() < 2) plot->scalePadding.emplace_back(0.0f, 0.0f);
    while(plot->dimScales.size() < 2) plot->dimScales.emplace_back(NAN,NAN);
    while(plot->scaleAutoPadding.size() < matrix.size()) plot->scaleAutoPadding.push_back(false);

    sf::Vector2f matrixScale;

    matrixScale.x = min(x);
    matrixScale.y = max(x);

    if(isnan(plot->dimScales[DIMX].x) || (matrixScale.x < plot->dimScales[DIMX].x)) plot->dimScales[DIMX].x = matrixScale.x;
    if(isnan(plot->dimScales[DIMX].y) || (matrixScale.y > plot->dimScales[DIMX].y)) plot->dimScales[DIMX].y = matrixScale.y;

    for(size_t i = 0; i < x.size(); ++i){
        append(matrix[DIMX], x[i]);
        for(size_t j = 0; j < x[i].size(); ++j){
            if(indices.size() > 0) points.emplace_back(x[i][j], yLabels[i], this, indices[i][j]);
            else points.emplace_back(x[i][j], yLabels[i], this, UINT_MAX);
        }
    }

    plot->callUpdate();

}

void CVPlot::Dataset::rm_dimension(const unsigned int& dimension){
    if(dimension < matrix.size()) matrix.erase(matrix.begin() + dimension);
    if(dimension < labels.size()) labels.erase(labels.begin() + dimension);
    if(dimension < indices.size()) indices.erase(indices.begin() + dimension);
    for(auto& point : points){
        if(dimension < point.dimValues.size()){
            point.dimValues.erase(point.dimValues.begin() + dimension);
        }
    }
}

CVPlot::axis::axis(const float& pos, const unsigned int& dimension, CVPlot* plot,
             const StringVector& labels,
             const UINT8& dataType, const float& thickness,
             const float& offset,
             const sf::Color& lineColor,
             const textEntry& textInfo,
             const float& textPadding):
                 plot(plot),
                 textInfo(textInfo),
                 label(nullptr),
                 bTick(true),
                 bAutoUpdateScale(true),
                 bSpanLabels(false),
                 pos(pos),
                 thickness(thickness),
                 textPadding(textPadding),
                 offset(offset),
                 dimension(dimension),
                 labels(labels),
                 dataType(dataType),
                 lineColor(lineColor){
    update();
    plot->callUpdate();
}

const static std::vector<float> AXIS_INT = { 1.0f, 3.0f, 5.0f, 8.0f };

void CVPlot::axis::getNumericLabels(const unsigned int& numDecimals){

    labels.clear();
    axisIntervals.clear();

    if(plot->dimScales.size() <= dimension) return;

    float dataRange = (plot->dimScales[dimension].y - plot->dimScales[dimension].x);
    if(isnan(dataRange)) return;

    dataRange *= (1.0f + plot->scalePadding[dimension].x + plot->scalePadding[dimension].y);

    float axisIncrement = 0.0f;

    if(dataRange == 0.0f){
        labels.push_back(strDigits(plot->dimScales[dimension].x, numDecimals));
        axisIntervals.push_back(plot->dimScales[dimension].x);
        updateRenderLabels();
        return;
    }
    else{
        float testRange = dataRange;
        if(dimension == DIMX) testRange /= 0.5f*pow(plot->widthScale(), (1.0f)/3);
        else if(dimension == DIMY) testRange /= 0.5f*pow(plot->heightScale(), (1.0f)/3);

        int power = floor(log10(testRange));
        float axisTest = closest(getFirstDigit(testRange), AXIS_INT);

        if(axisTest == 1.0f) axisIncrement = 0.2f*pow(10.0f, power);
        else if(axisTest == 3.0f) axisIncrement = 0.5f*pow(10.0f, power);
        else if(axisTest == 5.0f) axisIncrement = 1.0f*pow(10.0f, power);
        else if(axisTest == 8.0f) axisIncrement = 2.0f*pow(10.0f, power);
    }

    int multBegin, multEnd;

    if(dimension < plot->scalePadding.size()){
        multBegin = int(floor((plot->dimScales[dimension].x - (dataRange*plot->scalePadding[dimension].x))/axisIncrement)) - 1;
        multEnd = int(ceil((plot->dimScales[dimension].y + (dataRange*plot->scalePadding[dimension].y))/axisIncrement)) + 1;
    }
    else{
        multBegin = int(floor((plot->dimScales[dimension].x - dataRange)/axisIncrement)) - 1;
        multEnd = int(ceil((plot->dimScales[dimension].y + dataRange)/axisIncrement)) + 1;
    }

    if(dataRange <= 60.0f){
        for(int i = multBegin; i <= multEnd; ++i){
            if(axisIncrement < 1.0f){
                labels.push_back(strDigits(i*axisIncrement, numDecimals - (int)log10(axisIncrement)));
            }
            else{
                labels.push_back(strDigits(i*axisIncrement, numDecimals));
            }
            axisIntervals.push_back(i*axisIncrement);
        }
    }
    else{
        for(int i = multBegin; i <= multEnd; ++i){
            labels.push_back(sigFigs(i*axisIncrement));
            axisIntervals.push_back(i*axisIncrement);
        }
    }

}

void CVPlot::axis::updateRenderLabels(){

    switch(dataType){
        case DATA_TYPE_VERBAL:{
            labels.clear();
            for(auto& set : plot->datasets){
                for(auto& label : set.labels[dimension]){
                    if(!anyEqual(label, labels) &&
                       !set.getLabelledData(label, dimension).empty()) labels.push_back(label);
                }
            }
            break;
        }
        default: break;
    }

    renderLabels.resize(labels.size());
    lineTicks.clear();

    unsigned int index = 0;
    for(auto& text : renderLabels){
        text.setFont(*plot->View->mainApp->fonts[textInfo.font]);
        text.setString(labels[index]);
        text.setFillColor(textInfo.textColor);
        text.setCharacterSize(plot->labelFontSize*plot->fontScaling*pow(plot->areaScale(), (1.0f/3)));

        switch(dimension){
            case DIMX:{
                switch(dataType){
                    case DATA_TYPE_VERBAL:{
                        text.setOrigin(-6.0f,0.0f);
                        text.setRotation(45);
                        break;
                    }
                    default:{
                        text.setOrigin(text.getGlobalBounds().width/2,0.0f);
                        break;
                    }
                }
                break;
            }
            case DIMY:{
                text.setOrigin(text.getGlobalBounds().width + thickness,
                               getTextRelativeCenterY(text));
                break;
            }
            default: break;
        }
        ++index;
    }

    size_t labelSIZE = labels.size();
    sf::Vector2f axisCoord(0.0f, 0.0f);

    switch(dimension){
        case DIMENSION_X:{
            axisCoord.y = line.getPosition().y + thickness + textPadding;
            break;
        }
        case DIMENSION_Y:{
            axisCoord.x = line.getPosition().x - thickness - textPadding;
            break;
        }
        default: return;
    }

    for(size_t i = 0; i < labelSIZE;){

        if(bTick){
            switch(dimension){
                case DIMENSION_X:{
                    lineTicks.emplace_back(sf::Vector2f(thickness, textPadding));
                    break;
                }
                case DIMENSION_Y:{
                    lineTicks.emplace_back(sf::Vector2f(textPadding, thickness));
                    lineTicks.back().setOrigin(thickness, thickness/2);
                    break;
                }
                default: lineTicks.emplace_back();
            }

            lineTicks.back().setFillColor(line.getFillColor());
        }

        float numCast = 0.0f;
        switch(dataType){
            case DATA_TYPE_NUMERIC:{
                switch(dimension){
                    case DIMENSION_X:{
                        axisCoord.x = plot->getPlotXPos(axisIntervals[i]);

                        if((axisCoord.x < plot->plotBounds.left - thickness)
                           || (axisCoord.x > plot->plotBounds.left + plot->plotBounds.width + thickness)){
                            if((dimension > plot->scaleAutoPadding.size()) || !plot->scaleAutoPadding[dimension]){
                                removeLabel(i);
                                --labelSIZE;
                                continue;
                            }
                        }

                        if(bTick) lineTicks.back().setPosition(axisCoord.x, line.getPosition().y + thickness);

                        break;
                    }
                    case DIMENSION_Y:{
                        axisCoord.y = plot->getPlotYPos(axisIntervals[i]);

                        if(!plot->scaleAutoPadding[dimension] && ((axisCoord.y < plot->plotBounds.top - thickness)
                           || (axisCoord.y > plot->plotBounds.top + plot->plotBounds.height + thickness))){
                            if((dimension > plot->scaleAutoPadding.size()) || !plot->scaleAutoPadding[dimension]){
                                removeLabel(i);
                                --labelSIZE;
                                continue;
                            }
                        }

                        if(bTick) lineTicks.back().setPosition(line.getPosition().x - textPadding, axisCoord.y);

                        break;
                    }
                    default: break;
                }

                renderLabels[i].setPosition(axisCoord);
                ++i;

                break;
            }
            case DATA_TYPE_VERBAL:{
                switch(dimension){
                    case DIMENSION_X:{
                        axisCoord.x = plot->plotBounds.left +
                                    plot->plotBounds.width*(i+1)/(labelSIZE+1);
                        if(bTick) lineTicks.back().setPosition(axisCoord.x, line.getPosition().y + thickness);

                        if(plot->axisLabels.size() > 0){
                            float labelExtent = plot->xLabelBox().getPosition().y,
                                axisExtent = renderLabels[i].getPosition().y + renderLabels[i].getGlobalBounds().height;
                            if(axisExtent > labelExtent)
                                plot->xLabelBox().move(sf::Vector2f(0.0f, axisExtent + plot->plotBounds.height*0.02f - labelExtent));
                        }
                        break;
                    }
                    case DIMENSION_Y:{
                        axisCoord.y = plot->plotBounds.top + plot->plotBounds.height -
                                    plot->plotBounds.width*(i+1)/(labelSIZE+1);
                        if(bTick) lineTicks.back().setPosition(line.getPosition().x - textPadding, axisCoord.y);
                        break;
                    }
                    default: break;
                }

                renderLabels[i].setPosition(axisCoord);
                ++i;
                break;
            }
            default:{
                ++i; break;
            }
        }
    }
}

void CVPlot::axis::setLabelColor(const sf::Color& newColor){
    textInfo.textColor = newColor;
    for(auto& txt : renderLabels){
        txt.setFillColor(newColor);
    }
}

void CVPlot::axis::setLineColor(const sf::Color& newColor){
    lineColor = newColor;
    line.setFillColor(newColor);
    for(auto& tick : lineTicks){
        tick.setFillColor(newColor);
    }
}

void CVPlot::axis::update(){

    line.setFillColor(lineColor);

    switch(dataType){
        case DATA_TYPE_NUMERIC:{
            if((dimension < plot->scaleAutoPadding.size()) && plot->scaleAutoPadding[dimension]){
                plot->scalePadding[dimension].x = 0.0f;
                plot->scalePadding[dimension].y = 0.0f;

                getNumericLabels();

                plot->scalePadding[dimension].x = abs(plot->dimScales[dimension].x - min(axisIntervals))/plot->getDataRange(dimension);
                plot->scalePadding[dimension].y = abs(max(axisIntervals) - plot->dimScales[dimension].y)/plot->getDataRange(dimension);
            }
            else getNumericLabels();
            break;
        }
        default: break;
    }

    updateRenderLabels();

    switch(dimension){
        float anchorStart, anchorEnd;

        case DIMENSION_X:{
            if(bSpanLabels){
                anchorStart = getLabelPosition(labels.front());
                anchorEnd = getLabelPosition(labels.back()) + thickness;
            }else{
                anchorStart = plot->plotBounds.left;
                anchorEnd = plot->plotBounds.left + plot->plotBounds.width;
            }

            line.setSize(sf::Vector2f(anchorEnd - anchorStart, thickness));
            if(!isnan(pos)){
                line.setPosition(sf::Vector2f(anchorStart, plot->getPlotYPos(pos) + offset));
            }
            else line.setPosition(sf::Vector2f(anchorStart, plot->plotBounds.top + plot->plotBounds.height + offset));

            if(plot->axisLabels.size() > 1){
                label = &plot->xLabelBox();
                label->setFontSize(label->getTextInfo().fontSize*pow(plot->areaScale(), 1.0f/3));
                label->setPosition(plot->plotBounds.left + plot->plotBounds.width/2 - label->getBounds().width/2,
                               plot->plotBounds.top + plot->plotBounds.height + 1.1f*maxTextHeight(renderLabels) +
                               plot->xLabelBox().getBounds().height + offset);
            }
            else label = nullptr;

            break;
        }
        case DIMENSION_Y:{

            if(bSpanLabels){
                anchorStart = getLabelPosition(labels.back());
                anchorEnd = getLabelPosition(labels.front()) + thickness;
                line.setOrigin(thickness, thickness/2);
            }else{
                anchorStart = plot->plotBounds.top;
                anchorEnd = plot->plotBounds.top + plot->plotBounds.height + 3*thickness/2;
                line.setOrigin(thickness, thickness/2);
            }

            line.setSize(sf::Vector2f(thickness, anchorEnd - anchorStart));

            if(!isnan(pos)){
                line.setPosition(sf::Vector2f(plot->getPlotXPos(pos) - offset, anchorStart));
            }
            else line.setPosition(sf::Vector2f(plot->plotBounds.left - offset, anchorStart));

            if(plot->axisLabels.size() > 2){
                label = &plot->yLabelBox();
                label->setFontSize(label->getTextInfo().fontSize*pow(plot->areaScale(), 1.0f/3));
                label->setPosition(plot->plotBounds.left - offset -
                                   1.2f*maxTextWidth(renderLabels) - 1.5f*plot->yLabelBox().getBounds().width,
                                   plot->plotBounds.top + plot->plotBounds.height/2);
            }
            else label = nullptr;

            break;
        }
        default: break;
    }
}

CVPlot::dataPoint::dataPoint(const std::vector<float>& dimValues,
          Dataset* dataset, const unsigned int& dataIndex,
          const StringVector& labels,
          bool hover, const sf::Vector2f& animSource):
    dataset(dataset),
    plot(dataset->plot),
    selected(false),
    animated(true),
    visible(true),
    showHoverDisplay(hover),
    dataIndex(dataIndex),
    sprite(*plot->plotSpriteTexture),
    labels(labels),
    dimValues(dimValues),
    offset(0.0f,0.0f),
    spriteColor(dataset->mainColor()){

    for(auto& val : dimValues){
        if(isnan(val)){
            return;
        }
    }

    sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
    sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
    sprite.setScale(
        plot->getSpriteSize()/texSize.x,
        plot->getSpriteSize()/texSize.y);
    sprite.setColor(spriteColor);

    if(plot->bAnimSprites){
        if(isnan(animSource.x) || isnan(animSource.y)){
            sprite.setPosition(plot->plotBounds.left,
                                plot->plotBounds.top + plot->plotBounds.height);
        }
        else{
            sprite.setPosition(animSource.x, animSource.y);
        }
        sf::Vector2f plotPos = plot->getPlotPos(x(), y());

        sprite.setPosition(plotPos);

//        plot->View->anim_to(&sprite, plotPos, plot->animSpriteSpeed, false);
    }
    else{
        sprite.setPosition(plot->getPlotPos(x(), y()));
    }

    plot->callUpdate();

}

CVPlot::dataPoint::dataPoint(const float& x, const float& y,
          Dataset* dataset, const unsigned int& dataIndex,
          const StringVector& labels,
          bool hover, const sf::Vector2f& animSource):
    dataset(dataset),
    plot(dataset->plot),
    selected(false),
    animated(true),
    visible(true),
    showHoverDisplay(hover),
    dataIndex(dataIndex),
    sprite(*plot->plotSpriteTexture),
    labels(labels),
    dimValues({x, y, NAN}),
    offset(0.0f,0.0f),
    spriteColor(dataset->mainColor()){

    if(isnan(x) || isnan(y)){
        return;
    }

    sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
    sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
    sprite.setScale(
        plot->getSpriteSize()/texSize.x,
        plot->getSpriteSize()/texSize.y);
    sprite.setColor(spriteColor);

    if(plot->bAnimSprites){
        if(isnan(animSource.x) || isnan(animSource.y)){
            sprite.setPosition(plot->plotBounds.left, plot->plotBounds.top + plot->plotBounds.height);
        }
        else{
            sprite.setPosition(animSource.x, animSource.y);
        }
        sf::Vector2f plotPos = plot->getPlotPos(x, y);

        sprite.setPosition(plotPos);

//        plot->View->anim_to(&sprite, plotPos, plot->animSpriteSpeed, false);
    }
    else{
        sprite.setPosition(plot->getPlotPos(x, y));
    }

    plot->callUpdate();

}

CVPlot::dataPoint::dataPoint(const std::string& xLabel, const float& y,
          Dataset* dataset, const unsigned int& dataIndex,
          bool hover, const sf::Vector2f& animSource):
    dataset(dataset),
    plot(dataset->plot),
    selected(false),
    animated(true),
    visible(true),
    showHoverDisplay(hover),
    dataIndex(dataIndex),
    sprite(*plot->plotSpriteTexture),
    labels({xLabel}),
    animSpeed(400.0f),
    dimValues({NAN, y, NAN}),
    offset(0.0f,0.0f),
    spriteColor(dataset->mainColor()){

    if(isnan(y)){
        return;
    }

    sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
    sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
    sprite.setScale(
        plot->getSpriteSize()/texSize.x,
        plot->getSpriteSize()/texSize.y);

    sprite.setColor(spriteColor);

    if(plot->bAnimSprites){

        if(isnan(animSource.y) || isnan(animSource.x)){
            float xPos = plot->xAxis().getLabelPosition(xLabel);
            if(isnan(xPos)) xPos = plot->plotBounds.left + plot->plotBounds.width/2;
            sprite.setPosition(xPos, plot->plotBounds.top + plot->plotBounds.height);
        }
        else{
            sprite.setPosition(animSource.x, animSource.y);
        }

        float xPos = plot->xAxis().getLabelPosition(xLabel);
        if(isnan(xPos)) xPos = plot->plotBounds.left + plot->plotBounds.width/2;

        sf::Vector2f plotPos(xPos, plot->getPlotYPos(y));

        sprite.setPosition(plotPos);

//        plot->View->anim_to(&sprite, plotPos, plot->animSpriteSpeed, false);
    }
    else{
        float xPos = plot->xAxis().getLabelPosition(xLabel);
        if(isnan(xPos)) xPos = plot->plotBounds.left + plot->plotBounds.width/2;

        sprite.setPosition(sf::Vector2f(xPos, plot->getPlotYPos(y)));
    }

}

CVPlot::dataPoint::dataPoint(const float& x, const std::string& yLabel,
          Dataset* dataset, const unsigned int& dataIndex,
          bool hover, const sf::Vector2f& animSource):
    dataset(dataset),
    plot(dataset->plot),
    selected(false),
    animated(true),
    visible(true),
    showHoverDisplay(hover),
    dataIndex(dataIndex),
    sprite(*plot->plotSpriteTexture),
    labels({yLabel}),
    animSpeed(400.0f),
    dimValues({x, NAN, NAN}),
    offset(0.0f,0.0f),
    spriteColor(dataset->mainColor()){

    if(isnan(x)){
        return;
    }

    sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
    sprite.setOrigin(sprite.getGlobalBounds().width/2, sprite.getGlobalBounds().height/2);
    sprite.setScale(
        plot->getSpriteSize()/texSize.x,
        plot->getSpriteSize()/texSize.y);
    sprite.setColor(spriteColor);

    if(plot->bAnimSprites){
        if(isnan(animSource.y)){
            sprite.setPosition(plot->plotBounds.left,
                                plot->yAxis().getLabelPosition(yLabel));
        }
        else{
            sprite.setPosition(animSource.x, animSource.y);
        }
        sf::Vector2f plotPos(plot->getPlotXPos(x), plot->yAxis().getLabelPosition(yLabel));

        sprite.setPosition(plotPos);

//        plot->View->anim_to(&sprite, plotPos, plot->animSpriteSpeed, false);
    }
    else{
        sprite.setPosition(plot->getPlotXPos(x), plot->yAxis().getLabelPosition(yLabel));
    }

}

void CVPlot::dataPoint::update(){

    if(!valid()) return;

    sf::Vector2f pos;

    sf::Vector2f texSize(plot->plotSpriteTexture->getSize());
    sprite.setScale(
        plot->getSpriteSize()/texSize.x,
        plot->getSpriteSize()/texSize.y);

    switch(plot->xAxis().dataType){
        case DATA_TYPE_VERBAL:{
            pos.x = plot->xAxis().getLabelPosition(labels[DIMX]) + offset.x;
            break;
        }
        default:{
            pos.x = plot->getPlotXPos(x()) + offset.x;
            break;
        }
    }

    switch(plot->yAxis().dataType){
        case DATA_TYPE_VERBAL:{
            if(labels.size() > 1)
                pos.y = plot->yAxis().getLabelPosition(labels[DIMY]) + offset.y;
            break;
        }
        default:{
            pos.y = plot->getPlotYPos(y()) + offset.y;
            break;
        }
    }

    sprite.setPosition(pos);
}

sf::Vector2f CVPlot::getPlotPos(const sf::Vector2f& dataPoint){ // Turn scale units to view draw coordinates

    if((scalePadding.size() < 2) || (dimScales.size() < 2) ||
       isnan(dataPoint.x) || isnan(dataPoint.y)) return sf::Vector2f(NAN, NAN);

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
sf::Vector2f CVPlot::getPlotPos(const float& x, const float& y){
    return getPlotPos(sf::Vector2f(x, y));
}
float CVPlot::getPlotYPos(const float& dataY){
    if(isnan(dataY)) return NAN;
    float scaleRange = dimScales[DIMY].y - dimScales[DIMY].x;
    if(scaleRange == 0.0f) return plotBounds.top + plotBounds.height/2;

    return plotBounds.top + plotBounds.height*(1.0f-
                 (dataY - dimScales[DIMY].x + scaleRange*scalePadding[DIMY].x)/
                 (scaleRange*(1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y)));
}
float CVPlot::getPlotXPos(const float& dataX){
    if(isnan(dataX)) return NAN;
    float scaleRange = dimScales[DIMX].y - dimScales[DIMX].x;
    if(scaleRange == 0.0f) return plotBounds.left + plotBounds.width/2;

    return plotBounds.left + plotBounds.width*
                 (dataX - dimScales[DIMX].x + scaleRange*scalePadding[DIMX].x)/
                 (scaleRange*(1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y));
}

sf::Vector2f CVPlot::getScalePos(const sf::Vector2f& pos){ // Turn view window coordinates to data scale coordinates
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

float CVPlot::getScaleXPos(const float& pos){
    float scaleRange = dimScales[DIMX].y - dimScales[DIMX].x;
    if(scaleRange == 0.0f) return dimScales[DIMX].x;

    return (pos - plotBounds.left)/
    plotBounds.width*scaleRange*(1.0f + scalePadding[DIMX].x + scalePadding[DIMX].y) +
    dimScales[DIMX].x - scaleRange*scalePadding[DIMX].x;
}

float CVPlot::getScaleYPos(const float& pos){
    float scaleRange = dimScales[DIMY].y - dimScales[DIMY].x;
    if(scaleRange == 0.0f) return dimScales[DIMY].x;

    return (plotBounds.top + plotBounds.height - pos)/
    plotBounds.height*scaleRange*(1.0f + scalePadding[DIMY].x + scalePadding[DIMY].y) +
    dimScales[DIMY].x - scaleRange*scalePadding[DIMY].x;
}

void CVPlot::removeData(const unsigned int dimension, const unsigned int index){
    data[dimension].erase(data[dimension].begin() + index);
    tags[dimension].erase(tags[dimension].begin() + index);
    bReqPlotUpdate = true;
    bReqAxisUpdate = true;
}
void CVPlot::removeData(const unsigned int dimension,
                       const unsigned int begin,
                       const unsigned int end){
    data[dimension].erase(data[dimension].begin() + begin, data[dimension].end() + end);
    tags[dimension].erase(tags[dimension].begin() + begin, tags[dimension].end() + end);
    bReqPlotUpdate = true;
    bReqAxisUpdate = true;
}

void CVPlot::clearData(){
    data.clear();
    datasets.clear();
    tags.clear();
    dimScales.clear();
    selectedPoints.clear();
}
void CVPlot::clearData(const unsigned int dimension){
    for(auto& set : datasets){
        set.rm_dimension(dimension);
    }
    data[dimension].clear();
    tags[dimension].clear();
    dimScales.erase(dimScales.begin() + dimension);
}

bool CVPlot::draw(sf::RenderTarget* target){

    if(!CVBox::draw(target)) return false;

    for(auto& line : gridLines){ // Background grid
        target->draw(line);
    }

    for(auto& line : backLines){ // Quadrant lines, etc.
        target->draw(line);
    }

    if(bPlotBoundary) target->draw(plotBoundary);

    if(bShowSprites){
        for(auto& set : datasets){ // Plot points
            for(auto& point : set.points){
                if(point.visible && !anyEqual(&point, selectedPoints)){
                    target->draw(point.sprite);
                }
            }
        }
        for(auto& selected : selectedPoints){
            if(selected->visible) target->draw(selected->sprite);
        }
    }

    for(auto& line : frontLines){ // Regression lines
        target->draw(line);
    }

    for(auto& Axis : plotAxes){ // Draw Axes
        target->draw(Axis.line);
        for(auto& tick : Axis.lineTicks){
            target->draw(tick);
        }
        for(auto& text : Axis.renderLabels){
            target->draw(text);
        }
    }

    for(auto& box : legends){ // Draw legend labels
        box.draw(target);
    }

    for(auto& textbox : axisLabels){ // Draw axis labels
        textbox.draw(target);
    }

    for(auto& text : plotText){ // Draw persistent plot text
        target->draw(text);
    }

    for(auto& text : displayText){ // Draw temporary/pop-up text
        target->draw(text);
    }

    for(auto& item : mask){
        target->draw(item);
    }

    return true;
}

std::ostream& operator<<(std::ostream& output, const CVPlot& plot){
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

void CVPlot::setAxisLineColor(const sf::Color& newColor){
    for(auto& ax : plotAxes){
        ax.setLineColor(newColor);
    }
    axisLineColor = newColor;
}

void CVPlot::setAxisLabelColor(const sf::Color& newColor){

    try{
        xLabelBox().setTextColor(newColor);
    }catch(...){ }
    try{
        yLabelBox().setTextColor(newColor);
    }catch(...){ }

    for(auto& ax : plotAxes){
        ax.setLabelColor(newColor);
    }
    axisTextColor = newColor;
}

void CVPlot::setAxisColor(const sf::Color& newColor){
    setAxisLabelColor(newColor);
    setAxisLineColor(newColor);
}

void CVPlot::updateBounds(){

    sf::FloatRect textBounds;

    bounds = plotBounds;
    bounds.left -= plotBounds.width*0.1f;
    bounds.top -= plotBounds.height*0.1f;
    bounds.width += plotBounds.width*0.2f;
    bounds.height += plotBounds.height*0.2f;

    for(auto& Axis : plotAxes){
        for(auto& text : Axis.renderLabels){
            textBounds = text.getGlobalBounds();
            if(textBounds.left < bounds.left){
                bounds.width += (bounds.left - textBounds.left);
                bounds.left = textBounds.left;
            }
            if(textBounds.top < bounds.top){
                bounds.height += (bounds.top - textBounds.top);
                bounds.top = textBounds.top;
            }
            if(textBounds.left + textBounds.width > bounds.left + bounds.width)
                bounds.width = textBounds.left + textBounds.width - bounds.left;
            if(textBounds.top + textBounds.height > bounds.top + bounds.height)
                bounds.height = textBounds.top + textBounds.height - bounds.top;
        }
    }

    for(auto& label : axisLabels){
        textBounds = label.getBounds();
        if(textBounds.left < bounds.left){
            bounds.width += (bounds.left - textBounds.left);
            bounds.left = textBounds.left;
        }
        if(textBounds.top < bounds.top){
            bounds.height += (bounds.top - textBounds.top);
            bounds.top = textBounds.top;
        }
        if(textBounds.left + textBounds.width > bounds.left + bounds.width)
            bounds.width = textBounds.left + textBounds.width - bounds.left;
        if(textBounds.top + textBounds.height > bounds.top + bounds.height)
            bounds.height = textBounds.top + textBounds.height - bounds.top;
    }

    panel.front().setPosition(sf::Vector2f(bounds.left, bounds.top));
    panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));

}

void CVPlot::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVPlot::move(const sf::Vector2f& distance){
    CVTextBox::move(distance);

    plotBoundary.move(distance);
    for(auto& textBox : axisLabels){
        textBox.move(distance);
    }
    for(auto& line : gridLines){
        line.move(distance);
    }
    for(auto& line : backLines){
        line.move(distance);
    }
    for(auto& line : frontLines){
        line.move(distance);
    }
    for(auto& axis : plotAxes){
        axis.line.move(distance);
        for(auto& text : axis.renderLabels){
            text.move(distance);
        }
        for(auto& line : axis.lineTicks){
            line.move(distance);
        }
    }
    for(auto& set : datasets){
        for(auto& point : set.points){
            point.sprite.move(distance);
        }
    }
    for(auto& text : plotText){
        text.move(distance);
    }

    plotBounds.left += distance.x;
    plotBounds.top += distance.y;

}

void CVPlot::addData(const std::vector<float>& xData, const std::vector<float>& yData,
                    const std::vector<unsigned int>& indices,
                    const std::string& name, const AIALG::StringVector& pointTags){
    if(indices.empty()){
        dataAddRequests.emplace_back(xData, yData, vseq(size_t(0), xData.size() - 1), name, pointTags);
    }
    else{
        dataAddRequests.emplace_back(xData, yData, indices, name, pointTags);
    }
    callUpdate();
}
void CVPlot::addData(const AIALG::StringVector& xLabels, const AIALG::numMatrix& yData,
                    const std::vector<std::vector<unsigned int>>& indices,
                    const std::string& name, const AIALG::StringVector& pointTags){
    if(indices.empty()){
        std::vector<std::vector<unsigned int>> new_indices(yData.size());
        for(size_t i = 0; i < yData.size(); ++i){
            new_indices[i] = vseq(size_t(0), yData[i].size() - 1);
        }

        dataAddRequests.emplace_back(xLabels, yData, new_indices, name, pointTags);
    }
    else{
        dataAddRequests.emplace_back(xLabels, yData, indices, name, pointTags);
    }
    callUpdate();
}
void CVPlot::addData(const AIALG::numMatrix& xData, const AIALG::StringVector& yLabels,
                    const std::vector<std::vector<unsigned int>>& indices,
                    const std::string& name){
    if(indices.empty()){
        std::vector<std::vector<unsigned int>> new_indices(xData.size());
        for(size_t i = 0; i < xData.size(); ++i){
            new_indices[i] = vseq(size_t(0), xData[i].size() - 1);
        }

        dataAddRequests.emplace_back(xData, yLabels, new_indices, name);
    }
    else{
        dataAddRequests.emplace_back(xData, yLabels, indices, name);
    }
    callUpdate();
}
void CVPlot::addData(const AIALG::numMatrix& matrix,
                    const std::vector<unsigned int>& indices,
                    const AIALG::StringMatrix& labels,
                    const std::string& name){
    if(indices.empty()){
        dataAddRequests.emplace_back(matrix, vseq(size_t(0), minSize(matrix) - 1), labels, name);
    }
    else{
        dataAddRequests.emplace_back(matrix, indices, labels, name);
    }
    callUpdate();
}

bool CVPlot::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVTextBox::update(event, mousePos)) return false;

    ++framesLastChange;

    while(dataAddRequests.size() > 0){

        while(plotColors.size() < datasets.size() + 1){
            if(plotColors.empty()) plotColors.emplace_back(128,128,128);
            plotColors.emplace_back(plotColors.front());
        }

        if(dataAddRequests.front().labels.size() == 0){
                datasets.emplace_back(dataAddRequests.front().data, dataAddRequests.front().indices.front(),
                                        dataAddRequests.front().name, dataAddRequests.front().pointTags, this);
        }
        else if((axisLabels.size() > 0) && (xAxis().dataType == DATA_TYPE_VERBAL)) datasets.emplace_back(dataAddRequests.front().labels.front(),
                                                                                  dataAddRequests.front().data,
                                                                                  dataAddRequests.front().indices,
                                                                         dataAddRequests.front().name, this);
        else{
            datasets.emplace_back(dataAddRequests.front().data,
                                   dataAddRequests.front().indices.front(),
                                    dataAddRequests.front().name,
                                    dataAddRequests.front().labels.front(),
                                   this);
        }

        dataAddRequests.erase(dataAddRequests.begin());

    }

    for(auto& typeBox : axisLabels){
        typeBox.update(event, mousePos);
    }

    if(updateState & CV_PLOT_UPDATE_AXIS){
        if(axisLabels.size() > 0){
            titleBox().setFontSize(titleBox().getTextInfo().fontSize * pow(areaScale(), 1.0f/3));
            titleBox().setPosition(plotBounds.left + plotBounds.width/2 - titleBox().getBounds().width/2,
                                plotBounds.top - 1.5*titleBox().getBounds().height);
        }
    }

    for(auto& box : legends){
        box.update(event, mousePos);
    }

    displayText.clear();

    if(datasets.size() < 1) return false;

    if(framesLastChange < 5){

        if(updateState & CV_PLOT_UPDATE_DRAW){
            plotText.clear();
            gridLines.clear();
        }

        if(updateState & CV_PLOT_UPDATE_AXIS){
            for(auto& Axis : plotAxes){
                Axis.update();
            }
        }

        if(framesLastChange < 3){
            for(auto& set : datasets){
                if(updateState & CV_PLOT_UPDATE_DATA){
                    set.update();
                }
                if(updateState & CV_PLOT_UPDATE_POINTS){
                    for(auto& point : set.points){
                        point.update();
                        if(updateState & CV_PLOT_UPDATE_DATA){
                            if(!subset.empty()){
                                if(!anyEqual(point.dataIndex, subset)) point.visible = false;
                            }
                            if(!set.subset.empty()){
                                if(!anyEqual(point.dataIndex, set.subset)) point.visible = false;
                            }
                        }
                    }
                }
            }
            if(updateState & CV_PLOT_UPDATE_DATA){
                updateScales();
            }
        }else{
            updateState &= ~(CV_PLOT_UPDATE_DATA);
        }

        if(updateState & CV_PLOT_UPDATE_DRAW){
            if(has_y_axis() && bUseHorizGrid){
                sf::Color gridColor = getBorderColor();
                gridColor.a = 30;

                for(auto& val : yAxis().axisIntervals){
                    if(val == 0.0f) gridLines.emplace_back(sf::Vector2f(plotBounds.width, yAxis().thickness/2));
                    else gridLines.emplace_back(sf::Vector2f(plotBounds.width, yAxis().thickness/4));

                    gridLines.back().setOrigin(0.0f,gridLines.back().getSize().y/2);
                    gridLines.back().setPosition(plotBounds.left, getPlotYPos(val));
                    gridLines.back().setFillColor(gridColor);
                }
            }
            if(has_x_axis() && bUseVertGrid){
                sf::Color gridColor = getBorderColor();
                gridColor.a = 30;

                for(auto& val : xAxis().axisIntervals){
                    if(val == 0.0f) gridLines.emplace_back(sf::Vector2f(xAxis().thickness/2, plotBounds.height));
                    else gridLines.emplace_back(sf::Vector2f(xAxis().thickness/4, plotBounds.height));

                    gridLines.back().setOrigin(gridLines.back().getSize().x/2, 0.0f);
                    gridLines.back().setPosition(getPlotXPos(val), plotBounds.top);
                    gridLines.back().setFillColor(gridColor);
                }
            }
        }

        if(updateState & CV_PLOT_UPDATE_AXIS){
            updateBounds();
        }

    }
    else{
        updateState = CV_PLOT_UPDATE_NONE;
    }

    if(event.LMBhold && bounds.contains(event.lastFrameMousePosition) && event.viewHasFocus && event.captureMouse()){
        if(shiftPressed()){
            if((mousePos.x < bounds.left + bounds.width*0.95f) &&
               (mousePos.x > bounds.left + bounds.width*0.05f) &&
               (mousePos.y > bounds.top + bounds.height*0.05f) &&
               (mousePos.y < bounds.top + bounds.height*0.95f)){
                event.mouse_capture(*this);
            }
        }
        else if(plotBounds.contains(event.LMBpressPosition) && (event.LMBholdFrames > 5) && (mousePos != event.LMBpressPosition)){
            if(mask.size() < 1){
                mask.emplace_back();
                mask.back().setFillColor(sf::Color(178,207,255,78));
                mask.back().setOutlineThickness(1.0f);
                mask.back().setOutlineColor(sf::Color(160,207,255,128));
            }
            mask.back().setPosition(mousePos);

            displayText.emplace_back("[" + strDigits(getScaleXPos(mousePos.x), 3) + "," +
                                         strDigits(getScaleYPos(mousePos.y), 3) + "]",
                                          *plotFont, 13*fontScaling); // Read out current mouse position
            displayText.back().setPosition(sf::Vector2f(mousePos.x + 10.0f, mousePos.y -
                                                        displayText.back().getGlobalBounds().height/2));
            displayText.back().setFillColor(sf::Color(77,127,209,180));


            sf::Vector2f shiftPos(0.0f,0.0f);
            mask.back().setSize(sf::Vector2f(abs(mousePos.x - event.LMBpressPosition.x),
                                     abs(mousePos.y - event.LMBpressPosition.y)));


            if(mask.back().getSize().x * mask.back().getSize().y > 16){
                displayText.emplace_back("[" + strDigits(getScaleXPos(event.LMBpressPosition.x), 3) + "," +
                                         strDigits(getScaleYPos(event.LMBpressPosition.y), 3) + "]",
                                          *plotFont, 13*fontScaling); // Read out begin position
                displayText.back().setPosition(event.LMBpressPosition);

                if(mousePos.y < event.LMBpressPosition.y){
                    displayText.back().move(0.0f, 10.0f);
                }
                else{
                    displayText.back().move(0.0f, -displayText.back().getGlobalBounds().height - 10.0f);
                }

                if(mousePos.x > event.LMBpressPosition.x){
                    displayText.back().move(-10.0f - displayText.back().getGlobalBounds().width, 0.0f);
                }
                else{
                    displayText.back().move(10.0f, 0.0f);
                }

                displayText.back().setFillColor(sf::Color(77,127,209,180));
            }

            if(mousePos.x > event.LMBpressPosition.x) shiftPos.x = event.LMBpressPosition.x - mousePos.x;
            if(mousePos.y > event.LMBpressPosition.y) shiftPos.y = event.LMBpressPosition.y - mousePos.y;
            mask.back().move(shiftPos);
        }
        else if(event.LMBholdFrames == 1){
            bool pointSelected = false;
            for(auto& set : datasets){
                for(auto& point : set.points){
                    if(point.visible && point.sprite.getGlobalBounds().contains(mousePos)){
                        if(!pointSelected){
                            if(point.selected) point.setSelection(false);
                            else point.setSelection(true);
                            pointSelected = true;
                        }
                    }
                    else if(!ctrlPressed())
                        point.setSelection(false);
                }
            }

            callUpdate(CV_PLOT_UPDATE_DRAW);
        }
    }
    else{
        if(mask.size() > 0){
            sf::FloatRect selectionBounds = mask.back().getGlobalBounds(),
                        pointBounds;

            for(auto& set : datasets){
                for(auto& point : set.points){
                    pointBounds = point.sprite.getGlobalBounds();
                    if(point.visible && selectionBounds.contains(point.sprite.getPosition())){
                        point.setSelection(true);
                    }
                    else if(!ctrlPressed())
                        point.setSelection(false);
                }
            }
            mask.clear();

            callUpdate(CV_PLOT_UPDATE_DRAW);
        }

        if(plotBounds.contains(mousePos)){

            bool hoverSelected = false;

            for(auto& set : datasets){
                for(auto& point : set.points){
                    if(point.visible){
                        if(!hoverSelected && point.sprite.getGlobalBounds().contains(mousePos)){
                            displayText.emplace_back(point.getDisplayString(), *plotFont, 13*fontScaling);
                            displayText.back().setPosition(mousePos);
                            displayText.back().move(10.0f, -displayText.back().getGlobalBounds().height/2);
                            displayText.back().setFillColor(getBorderColor());
                            hoverSelected = true; // Prevent hover readout of more than one plot point at a time
                        }
                    }
                }
            }

        }
    }

    if(shiftPressed() && event.LMBhold && bounds.contains(event.lastFrameMousePosition) && !event.isCaptured(*this)){
        if(mousePos.x > bounds.left + bounds.width/2){
            plotBounds.width += (mousePos.x - event.lastFrameMousePosition.x);
            bounds.width += (mousePos.x - event.lastFrameMousePosition.x);
            panel.front().setSize(sf::Vector2f(panel.front().getSize().x + (mousePos.x - event.lastFrameMousePosition.x),
                                  panel.front().getSize().y));
            callUpdate(CV_PLOT_UPDATE_AXIS | CV_PLOT_UPDATE_POINTS | CV_PLOT_UPDATE_DRAW);
        }
        else if(mousePos.x < bounds.left + bounds.width/2){
            plotBounds.width -= (mousePos.x - event.lastFrameMousePosition.x);
            plotBounds.left += (mousePos.x - event.lastFrameMousePosition.x);
            bounds.width -= (mousePos.x - event.lastFrameMousePosition.x);
            bounds.left += (mousePos.x - event.lastFrameMousePosition.x);
            panel.front().setSize(sf::Vector2f(panel.front().getSize().x - (mousePos.x - event.lastFrameMousePosition.x),
                                  panel.front().getSize().y));
            callUpdate(CV_PLOT_UPDATE_AXIS | CV_PLOT_UPDATE_POINTS | CV_PLOT_UPDATE_DRAW);
        }

        if(mousePos.y > bounds.top + bounds.height/2){
            plotBounds.height += (mousePos.y - event.lastFrameMousePosition.y);
            bounds.height += (mousePos.y - event.lastFrameMousePosition.y);
            panel.front().setSize(sf::Vector2f(panel.front().getSize().x,
                                               panel.front().getSize().y + (mousePos.y - event.lastFrameMousePosition.y)));
            callUpdate(CV_PLOT_UPDATE_AXIS | CV_PLOT_UPDATE_POINTS | CV_PLOT_UPDATE_DRAW);
        }
        else if(mousePos.y < bounds.top + bounds.height/2){
            plotBounds.height -= (mousePos.y - event.lastFrameMousePosition.y);
            plotBounds.top += (mousePos.y - event.lastFrameMousePosition.y);
            bounds.height -= (mousePos.y - event.lastFrameMousePosition.y);
            bounds.top += (mousePos.y - event.lastFrameMousePosition.y);
            panel.front().setSize(sf::Vector2f(panel.front().getSize().x,
                                               panel.front().getSize().y - (mousePos.y - event.lastFrameMousePosition.y)));
            callUpdate(CV_PLOT_UPDATE_AXIS | CV_PLOT_UPDATE_POINTS | CV_PLOT_UPDATE_DRAW);
        }
    }

    if(updateState & CV_PLOT_UPDATE_DRAW){
        if((axisLabels.size() > 1) && (xAxis().dataType == DATA_TYPE_NUMERIC) && (yAxis().dataType == DATA_TYPE_NUMERIC)){
            if(selectedPoints.size() < 50){ // Prevent text cluttering
                for(auto& selected : selectedPoints){
                    if(selected->visible && (selected->labels.size() > 0)){
                        plotText.emplace_back(concatenateString(selected->labels, ","), *plotFont, 12*fontScaling);
                        plotText.back().setPosition(selected->getPosition() +
                                                       sf::Vector2f(12.0f, -(plotText.back().getGlobalBounds().height/2
                                                                             + getTextCenterOffsetY(plotText.back()))));
                        plotText.back().setFillColor(getBorderColor());
                    }
                }
            }
        }

        plotBoundary.setPosition(plotBounds.left, plotBounds.top);
        plotBoundary.setSize(sf::Vector2f(plotBounds.width, plotBounds.height));
    }

    return true;

}

CVScatterPlot::CVScatterPlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor, const sf::Color& borderColor, const sf::Color& plotColor,
           const float& borderWidth, const std::string& plotSprite,
           const std::string& fontName, const std::string& quadrantType):
               CVPlot(View, position, width, height, fillColor, borderColor, plotColor, borderWidth,
                      plotSprite, fontName),
                      quadrantType(quadrantType),
                      colorQuadrants(false),
                      regressionType(CV_LINE_REGRESSION_NONE){

    plotTypeID = CV_PLOT_ID_SCATTER;

    spriteSize = 10.0f;

    addAxis(NAN, DIMENSION_X, DATA_TYPE_NUMERIC, 5.0f, 12.0f, borderColor, labelFontSize*fontScaling);
    addAxis(NAN, DIMENSION_Y, DATA_TYPE_NUMERIC, 5.0f, 12.0f, borderColor, labelFontSize*fontScaling);

    xAxis().bSpanLabels = true;
    yAxis().bSpanLabels = true;

    setPlotBorder();
    sf::Color plotBorderColor = borderColor;
    brighten(plotBorderColor, 120);
    setPlotBorderColor(plotBorderColor);

    setScaleAuto({true, true});

}

bool CVScatterPlot::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVPlot::update(event, mousePos)) return false;

    if((framesLastChange < 5) && (datasets.size() > 0)){

        if(updateState & CV_PLOT_UPDATE_DRAW){

            frontLines.clear();

            if((quadrantType.size() != 0) && (quadrantType != "none")){

                quadrantPoints.resize(4);

                float xPos, yPos;
                std::vector<float> xData, yData;

                for(auto& set : datasets){
                    append(xData, set.getSubsetData(DIMX));
                    append(yData, set.getSubsetData(DIMY));
                }

                bool validType = false;

                if(quadrantType == "median"){
                    xPos = median(xData);
                    yPos = median(yData);
                    validType = true;
                }
                else if(quadrantType == "average"){
                    xPos = average(xData);
                    yPos = average(yData);
                    validType = true;
                }

                if(validType){

                    addLine(CV_PLOT_ALIGN_HORIZONTAL, yPos, plotAxes[DIMX].thickness/2, getBorderColor(),
                                CV_LINE_TYPE_BROKEN, true);
                    addLine(CV_PLOT_ALIGN_VERTICAL, xPos, plotAxes[DIMY].thickness/2, getBorderColor(),
                                CV_LINE_TYPE_BROKEN, true);

                    if(colorQuadrants){
                        for(auto& set : datasets){
                            for(auto& point : set.points){
                                if(!point.valid()) continue;
                                if((point.x() >= xPos) && (point.y() >= yPos)){
                                    point.setBaseColor(quadrantColors[0]); // Q1
                                    quadrantPoints[0].push_back(&point);
                                }
                                else if((point.x() < xPos) && (point.y() >= yPos)){
                                    point.setBaseColor(quadrantColors[1]); // Q2
                                    quadrantPoints[1].push_back(&point);
                                }
                                else if((point.x() < xPos) && (point.y() < yPos)){
                                    point.setBaseColor(quadrantColors[2]); // Q3
                                    quadrantPoints[2].push_back(&point);
                                }
                                else{
                                    point.setBaseColor(quadrantColors[3]); // Q4
                                    quadrantPoints[3].push_back(&point);
                                }
                            }
                        }
                    }
                    else{
                        for(auto& set : datasets){
                            for(auto& point : set.points){
                                point.setBaseColor(plotColor());
                            }
                        }
                    }

                }

            }
            else{
                for(auto& set : datasets){
                    for(auto& point : set.points){
                        point.setBaseColor(plotColor());
                    }
                }
            }

            if(regressionType){
                regression.clear();
                for(auto& set : datasets){
                    regression.emplace_back(set.getSubsetData(DIMX), set.getSubsetData(DIMY), this);

                    float regAngle = get_angle(getPlotPos(0.0f,0.0f), getPlotPos(1.0f, regression.back().slope))*180/PI;

                    sf::Vector2f endPos(plotBounds.left + plotBounds.width,
                                        getPlotYPos(regression.back().slope*getScaleXPos(plotBounds.left + plotBounds.width) +
                                                    regression.back().intercept)),
                                beginPos(plotBounds.left,
                                        getPlotYPos(regression.back().slope*getScaleXPos(plotBounds.left) +
                                                    regression.back().intercept));

                    if(endPos.y > plotBounds.top + plotBounds.height){
                        endPos.y = plotBounds.top + plotBounds.height;
                        endPos.x = getPlotXPos((getScaleYPos(plotBounds.top + plotBounds.height) - regression.back().intercept)/
                                               regression.back().slope);
                    }
                    else if(endPos.y < plotBounds.top){
                        endPos.y = plotBounds.top;
                        endPos.x = getPlotXPos((getScaleYPos(plotBounds.top) - regression.back().intercept)/
                                               regression.back().slope);
                    }

                    if(beginPos.y > plotBounds.top + plotBounds.height){
                        beginPos.y = plotBounds.top + plotBounds.height;
                        beginPos.x = getPlotXPos((getScaleYPos(plotBounds.top + plotBounds.height) - regression.back().intercept)/
                                               regression.back().slope);
                    }
                    else if(beginPos.y < plotBounds.top){
                        beginPos.y = plotBounds.top;
                        beginPos.x = getPlotXPos((getScaleYPos(plotBounds.top) - regression.back().intercept)/
                                               regression.back().slope);
                    }

                    frontLines.emplace_back(sf::Vector2f(getDistance(beginPos, endPos) + xAxis().thickness/2,
                                                        xAxis().thickness/2));

                    frontLines.back().setOrigin(xAxis().thickness/2, xAxis().thickness/4);
                    frontLines.back().setFillColor(sf::Color::Black);

                    frontLines.back().setPosition(beginPos);
                    frontLines.back().setRotation(regAngle);

                    plotText.emplace_back("R  = " + strDigits(regression.back().coefficient, 4), *plotFont, labelFontSize*fontScaling*pow(areaScale(), 1.0f/3));
                    plotText.back().setPosition(endPos.x + 10.0f, endPos.y -
                                                plotText.back().getGlobalBounds().height/2 -
                                                getTextCenterOffsetY(plotText.back())/2);
                    plotText.back().setFillColor(getBorderColor());

                    plotText.emplace_back("2", *plotFont, 8*fontScaling*pow(areaScale(), 1.0f/3));
                    plotText.back().setPosition(plotText[plotText.size()-2].findCharacterPos(1));
                    plotText.back().setFillColor(getBorderColor());

                }
            }

        }
    }

    return true;

}

CVCategoryPlot::CVCategoryPlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor, const sf::Color& borderColor, const sf::Color& plotColor,
           const float& borderWidth, const std::string& plotSprite,
           const std::string& fontName):
               CVPlot(View, position, width, height, fillColor, borderColor, plotColor, borderWidth,
                      plotSprite, fontName),
                      showQ1(false),
                      showMedian(false),
                      showQ3(false),
                      showSigBrackets(false),
                      omitOutliers(true),
                      maxSigBrackets(10){

    plotTypeID = CV_PLOT_ID_CATEGORY;

    spriteSize = 8.0f;

    addAxis(NAN, DIMENSION_X, DATA_TYPE_VERBAL, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);
    addAxis(NAN, DIMENSION_Y, DATA_TYPE_NUMERIC, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);

    setScalePadding(DIMENSION_X, 0.1f, 0.1f);
    setScalePadding(DIMENSION_Y, 0.1f, 0.2f);

}

bool CVCategoryPlot::update(CVEvent& event, const sf::Vector2f& position){

    if(!CVPlot::update(event, position)) return false;

    if(framesLastChange < 5){

        frontLines.clear();

        if(plotAxes.size() < 1) return false;

        for(auto& set : datasets){

            if(xAxis().dataType == DATA_TYPE_VERBAL){

                size_t L = xAxis().labels.size();

                if(updateState & CV_PLOT_UPDATE_DATA){
                    if(framesLastChange == 2){
                        std::vector<sf::Sprite*> spriteList;
                        for(auto& point : set.points){
                            spriteList.push_back(&point.sprite);
                        }
                        physicsSpread(spriteList, 1.0f/(L + 1), CV_DIRECTION_X);
                        for(auto& point : set.points){
                            point.setOffset();
                        }
                    }
                }

                if(updateState & CV_PLOT_UPDATE_DRAW){

                    if(showQ1 || showMedian || showQ3){
                        std::vector<float> labelData;
                        for(size_t i = 0; i < L; ++i){

                            float xPos = xAxis().getLabelPosition(xAxis().labels[i]),
                                    lineTop = 0.0f, lineBottom = 0.0f;
                            labelData = set.getLabelledData(xAxis().labels[i], DIMY);
                            if(labelData.empty()) continue;

                            DataSummary<float> dataSummary(labelData);

                            if(showQ3){
                                frontLines.emplace_back(sf::Vector2f(plotBounds.width/(4*(L+1)), xAxis().thickness/2)); // 3rd quartile marker
                                frontLines.back().setPosition(xPos - frontLines.back().getSize().x/2,
                                                              getPlotYPos(dataSummary.Q3));
                                frontLines.back().setFillColor(getBorderColor());
                                lineTop = frontLines.back().getPosition().y;
                                if(!showQ3 && !showMedian) lineBottom = NAN;
                            }

                            if(showMedian){
                                frontLines.emplace_back(sf::Vector2f(plotBounds.width/(2*(L+1)), xAxis().thickness/2)); // Median marker
                                frontLines.back().setPosition(xPos - frontLines.back().getSize().x/2,
                                                              getPlotYPos(dataSummary.median));
                                frontLines.back().setFillColor(getBorderColor());
                                if(!showQ3) lineTop = frontLines.back().getPosition().y;
                                if(!showQ1) lineBottom = frontLines.back().getPosition().y;
                            }

                            if(showQ1){
                                frontLines.emplace_back(sf::Vector2f(plotBounds.width/(4*(L+1)), xAxis().thickness/2)); // 1st quartile marker
                                frontLines.back().setPosition(xPos - frontLines.back().getSize().x/2,
                                                              getPlotYPos(dataSummary.Q1));
                                frontLines.back().setFillColor(getBorderColor());
                                if(!showQ3 && !showMedian) lineTop = NAN;
                                lineBottom = frontLines.back().getPosition().y;
                            }

                            if(frontLines.size() > 1){ // Connecting line
                                frontLines.emplace_back(sf::Vector2f(xAxis().thickness/2,
                                                        lineBottom - lineTop));
                                frontLines.back().setPosition(xPos - frontLines.back().getSize().x/2,
                                                              lineTop);
                                frontLines.back().setFillColor(getBorderColor());
                            }

                        }
                    }

                }


                if((updateState & CV_PLOT_UPDATE_DRAW) && showSigBrackets){

                    sigMatrix.clear();
                    sigMatrix.assign(L, std::vector<float>(L, NAN));
                    sigPairs.clear();
                    sigValues.clear();

                    size_t sigSIZE = 0;

                    for(size_t i = 0; i < L; ++i){
                        for(size_t j = 0; j < L; ++j){
                            if(i == j) continue;
                            sigMatrix[i][j] = student_t_test(set.getLabelledData(xAxis().labels[i], DIMY),
                                                             set.getLabelledData(xAxis().labels[j], DIMY));
                        }
                    }

                    for(size_t i = 0; i < L-1; ++i){
                        for(size_t j = i+1; j < L; ++j){
                            if(sigMatrix[i][j] < 0.05f){
                                unsigned int distIndex = 0;
                                while((distIndex < sigSIZE) &&
                                      ((j - i) > (sigPairs[distIndex].y - sigPairs[distIndex].x))){
                                    ++distIndex;
                                }
                                sigValues.insert(sigValues.begin() + distIndex, sigMatrix[i][j]);
                                sigPairs.insert(sigPairs.begin() + distIndex, sf::Vector2u(i, j));
                                ++sigSIZE;
                            }
                        }
                        if(sigSIZE > maxSigBrackets) break;
                    }

                    if((sigSIZE > 0) && (sigSIZE <= maxSigBrackets)){

                        float braceTop = plotBounds.top - plotBounds.height*0.05f*sigSIZE,
                                braceBottom = getPlotYPos(max(set.matrix[DIMY])) - plotBounds.height*0.1f,
                                braceInt = (braceBottom - braceTop)/sigSIZE;

                        for(size_t j = 0; j < sigSIZE; ++j){
                            float leftBraceX = plotBounds.left + plotBounds.width*(sigPairs[j].x+1)/(L+1),
                                    rightBraceX = plotBounds.left + plotBounds.width*(sigPairs[j].y+1)/(L+1),
                                    leftBraceY = getPlotYPos(max(set.getLabelledData(xAxis().labels[sigPairs[j].x], DIMY)))
                                     - 0.04f*plotBounds.height,
                                     rightBraceY = getPlotYPos(max(set.getLabelledData(xAxis().labels[sigPairs[j].y], DIMY)))
                                     - 0.04f*plotBounds.height;

                            frontLines.emplace_back(sf::Vector2f(plotBounds.width/(4*(L+1)), xAxis().thickness/2)); // Left bottom brace
                            frontLines.back().setPosition(leftBraceX - frontLines.back().getSize().x/2,
                                                          leftBraceY);
                            frontLines.back().setFillColor(getBorderColor());

                            frontLines.emplace_back(sf::Vector2f(plotBounds.width/(4*(L+1)), xAxis().thickness/2)); // Right bottom brace
                            frontLines.back().setPosition(rightBraceX - frontLines.back().getSize().x/2,
                                                          rightBraceY);
                            frontLines.back().setFillColor(getBorderColor());

                            frontLines.emplace_back(sf::Vector2f(xAxis().thickness/2, leftBraceY - braceBottom)); // Left bracket arm
                            frontLines.back().setPosition(leftBraceX, braceBottom);
                            frontLines.back().setFillColor(getBorderColor());

                            frontLines.emplace_back(sf::Vector2f(xAxis().thickness/2, rightBraceY - braceBottom)); // Right bracket arm
                            frontLines.back().setPosition(rightBraceX, braceBottom);
                            frontLines.back().setFillColor(getBorderColor());

                            frontLines.emplace_back(sf::Vector2f(rightBraceX - leftBraceX, xAxis().thickness/2)); // Top bracket edge
                            frontLines.back().setPosition(leftBraceX, braceBottom);
                            frontLines.back().setFillColor(getBorderColor());

                            // Significance stars

                            if(sigValues[j] < 0.001f){
                                plotText.emplace_back("***", *plotFont, 18*fontScaling);
                            }
                            else if(sigValues[j] < 0.01f){
                                plotText.emplace_back("**", *plotFont, 18*fontScaling);
                            }
                            else{
                                plotText.emplace_back("*", *plotFont, 18*fontScaling);
                            }

                            sf::FloatRect textBounds = plotText.back().getLocalBounds();
                            plotText.back().setOrigin(textBounds.width/2, textBounds.height/2 -
                                                      getTextCenterOffsetY(plotText.back())/2);
                            plotText.back().setFillColor(getBorderColor());
                            plotText.back().setPosition((rightBraceX + leftBraceX)/2,
                                                               braceBottom - 4.5f*textBounds.height);

                            braceBottom -= braceInt;

                        }

                    }

                }

//            if(yAxis().dataType == DATA_TYPE_VERBAL){
//                physicsSpread(spriteList, 1.0f/(yAxis().labels.size() + 1), CV_DIRECTION_Y);
//            }
            }

        }

        if(showSigBrackets && (sigValues.size() < maxSigBrackets)){
            sf::FloatRect sBounds;
            float height = titleBox().getBounds().height;

            for(auto& line : frontLines){
                sBounds = line.getGlobalBounds();
                sBounds.top -= height/2;
                while(sBounds.intersects(titleBox().getBounds())){
                    titleBox().move(sf::Vector2f(0.0f,-height));
                }
            }
        }

    }

    return true;

}

CVLinePlot::CVLinePlot(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
           const sf::Color& fillColor, const sf::Color& borderColor, const sf::Color& plotColor,
           const float& borderWidth, const std::string& plotSprite,
           const std::string& fontName, const UINT8& lineType, const UINT8& connectionType):
               CVPlot(View, position, width, height, fillColor, borderColor, plotColor, borderWidth,
                      plotSprite, fontName),
                      lineType(lineType),
                      connectionType(connectionType){

    plotTypeID = CV_PLOT_ID_LINE;

    spriteSize = 7.0f;

    addAxis(NAN, DIMENSION_X, DATA_TYPE_NUMERIC, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);
    addAxis(NAN, DIMENSION_Y, DATA_TYPE_NUMERIC, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);

    setScaleAuto({true, true});

}

bool CVLinePlot::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVPlot::update(event, mousePos)) return false;

    if(framesLastChange < 5){

        if(updateState & CV_PLOT_UPDATE_DRAW){

            backLines.clear();

            std::vector<float> xDataValues;
            std::vector<unsigned int> orderedI;
            unsigned int L;
            float dist, angle,
                lineWidth = getSpriteSize()/2;

            for(auto& set : datasets){

                xDataValues.clear();

                for(auto& point : set.points){
                    if(point.visible && !isnan(point.y())) xDataValues.push_back(point.x());
                    else xDataValues.push_back(NAN);
                }

                orderedI = orderedIndex(xDataValues);
                L = orderedI.size();

                backLines.reserve(L*2);

                for(size_t i = 1; i < L; ++i){
                    switch(connectionType){
                        case CV_LINE_CONN_STEP:{
                            sf::Vector2f cornerPos(set.points[orderedI[i-1]].sprite.getPosition().x,
                                                   set.points[orderedI[i]].sprite.getPosition().y);

                            float segW = cornerPos.y - set.points[orderedI[i-1]].sprite.getPosition().y,
                                segH = set.points[orderedI[i]].sprite.getPosition().x - cornerPos.x + lineWidth;

                            if(segW){
                                if(segW > 0.0f){
                                    backLines.emplace_back(sf::Vector2f(lineWidth, segW));
                                    backLines.back().setOrigin(lineWidth/2, lineWidth/2);
                                    backLines.back().setPosition(set.points[orderedI[i-1]].sprite.getPosition());
                                }
                                else{
                                    backLines.emplace_back(sf::Vector2f(lineWidth, -segW));
                                    backLines.back().setOrigin(lineWidth/2, lineWidth/2);
                                    backLines.back().setPosition(cornerPos);
                                }

                                if(set.points[orderedI[i-1]].selected && set.points[orderedI[i]].selected)
                                    backLines.back().setFillColor(plotHighlightColor);
                                else backLines.back().setFillColor(set.mainColor());
                            }
                            if(segH){
                                backLines.emplace_back(sf::Vector2f(segH, lineWidth));

                                backLines.back().setOrigin(lineWidth/2, lineWidth/2);
                                backLines.back().setPosition(cornerPos);


                                if(set.points[orderedI[i-1]].selected && set.points[orderedI[i]].selected)
                                    backLines.back().setFillColor(plotHighlightColor);
                                else backLines.back().setFillColor(set.mainColor());
                            }

                            break;
                        }
                        default:{
                            dist = getDistance(set.points[orderedI[i-1]].sprite.getPosition(), set.points[orderedI[i]].sprite.getPosition());
                            angle = get_angle(set.points[orderedI[i-1]].sprite.getPosition(), set.points[orderedI[i]].sprite.getPosition());

                            backLines.emplace_back(sf::RectangleShape(sf::Vector2f(dist, set.points[orderedI[i]].sprite.getGlobalBounds().width/2)));
                            backLines.back().setOrigin(0.0f,
                                                       set.points[orderedI[i]].sprite.getGlobalBounds().width/4);
                            backLines.back().setPosition(set.points[orderedI[i-1]].sprite.getPosition());
                            backLines.back().setRotation(angle*180/PI);
                            backLines.back().setFillColor(set.mainColor());
                            break;
                        }
                    }
                }
            }
        }
    }

    return true;

}

CVHeatPlot::CVHeatPlot(CVView* View, const sf::Vector2f& position, const float& width,
                       const float& height, const sf::Color& fillColor, const sf::Color& borderColor,
                       const sf::Color& plotColor, const float& borderWidth, const std::string& plotSprite,
                       const std::string& fontName, const UINT8& clusterType, const UINT8& scaleType):
                           CVPlot(View, position, width, height, fillColor, borderColor,
                                  plotColor, borderWidth, plotSprite, fontName),
                                  clusterType(clusterType),
                                  scaleType(scaleType){
    plotTypeID = CV_PLOT_ID_HEAT;

    addAxis(NAN, DIMENSION_X, DATA_TYPE_VERBAL, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);
    addAxis(NAN, DIMENSION_Y, DATA_TYPE_VERBAL, 5.0f, 0.0f, borderColor, labelFontSize*fontScaling);

    setScalePadding(DIMENSION_X, 0.0f, 0.0f);
    setScalePadding(DIMENSION_Y, 0.0f, 0.0f);

}

bool CVHeatPlot::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVPlot::update(event, mousePos)) return false;

    if(framesLastChange < 5){

        for(auto& set : datasets){
            unsigned int yCells = set.matrix.size(),
                        xCells = maxSize(set.matrix);
            float xLength = plotBounds.width/xCells,
                    yLength = plotBounds.height/yCells;
            for(auto& point : set.points){
                point.setSize(sf::Vector2f(xLength, yLength));

                switch(scaleType){
                    case CV_HEAT_SCALE_REL_COL:{
                        break;
                    }
                    case CV_HEAT_SCALE_REL_ROW:{
                        break;
                    }
                    case CV_HEAT_SCALE_REL_ALL:{
                        break;
                    }
                    default:{
                        break;
                    }
                }
            }
        }

    }

    return true;
}

void CVPlot::get_numeric_x_axis(){
    if(data.size() < 1) return;

    xAxisLabels.clear();
    float dataRange = dimScales[DIMX].y - dimScales[DIMX].x;
    float axisIncrement = 0.0f;

    if(dataRange == 0){
        xAxisLabels.push_back(std::to_string(data[DIMX][0].front()));
        sigFigs(xAxisLabels.back());
    }
    else{
        if(dataRange < 1e-2) axisIncrement = pow(10.0f, (int)floor(-log10(dataRange)-1));
        else if(dataRange <= 0.001f) axisIncrement = 0.0001f;
        else if(dataRange <= 0.02f) axisIncrement = 0.002f;
        else if(dataRange <= 0.05f) axisIncrement = 0.005f;
        else if(dataRange <= 0.1f) axisIncrement = 0.01f;
        else if(dataRange <= 0.2f) axisIncrement = 0.02f;
        else if(dataRange <= 0.5f) axisIncrement = 0.05f;
        else if(dataRange <= 1.0f) axisIncrement = 0.1f;
        else if(dataRange <= 2.0f) axisIncrement = 0.2f;
        else if(dataRange <= 5.0f) axisIncrement = 0.5f;
        else if(dataRange <= 10.0f) axisIncrement = 1.0f;
        else if(dataRange <= 20.0f) axisIncrement = 2.0f;
        else if(dataRange <= 50.0f) axisIncrement = 5.0f;
        else if(dataRange <= 100.0f) axisIncrement = 10.0f;
        else if(dataRange <= 200.0f) axisIncrement = 20.0f;
        else if(dataRange <= 500.0f) axisIncrement = 50.0f;
        else if(dataRange <= 1000.0f) axisIncrement = 100.0f;
        else if(dataRange <= 2000.0f) axisIncrement = 200.0f;
        else if(dataRange <= 5000.0f) axisIncrement = 500.0f;
        else if(dataRange <= 10000.0f) axisIncrement = 1000.0f;
        else if(dataRange <= 50000.0f) axisIncrement = 5000.0f;
        else{
            axisIncrement = pow(10.0f, (int)floor(log10(dataRange)));
        }
    }

    int multBegin = int(ceil(dimScales[0].x/axisIncrement)),
        multEnd = int(floor(dimScales[0].y/axisIncrement));

    for(int i = multBegin; i <= multEnd; ++i){
        xAxisLabels.push_back(std::to_string(i*axisIncrement));
        sigFigs(xAxisLabels.back());
    }
}

void CVPlot::get_numeric_y_axis(){
    if(data.size() < 1) return;

    yAxisLabels.clear();
    float dataRange = dimScales[DIMY].y - dimScales[DIMY].x;
    float axisIncrement = 0.0f;

    if(dataRange == 0){
        yAxisLabels.push_back(std::to_string(data[DIMY][0].front()));
        sigFigs(yAxisLabels.back());
    }
    else{
        if(dataRange < 1e-2) axisIncrement = pow(10.0f, (int)floor(-log10(dataRange)-1));
        else if(dataRange <= 0.001f) axisIncrement = 0.0001f;
        else if(dataRange <= 0.02f) axisIncrement = 0.002f;
        else if(dataRange <= 0.05f) axisIncrement = 0.005f;
        else if(dataRange <= 0.1f) axisIncrement = 0.01f;
        else if(dataRange <= 0.2f) axisIncrement = 0.02f;
        else if(dataRange <= 0.5f) axisIncrement = 0.05f;
        else if(dataRange <= 1.0f) axisIncrement = 0.1f;
        else if(dataRange <= 2.0f) axisIncrement = 0.2f;
        else if(dataRange <= 5.0f) axisIncrement = 0.5f;
        else if(dataRange <= 10.0f) axisIncrement = 1.0f;
        else if(dataRange <= 20.0f) axisIncrement = 2.0f;
        else if(dataRange <= 50.0f) axisIncrement = 5.0f;
        else if(dataRange <= 100.0f) axisIncrement = 10.0f;
        else if(dataRange <= 200.0f) axisIncrement = 20.0f;
        else if(dataRange <= 500.0f) axisIncrement = 50.0f;
        else if(dataRange <= 1000.0f) axisIncrement = 100.0f;
        else if(dataRange <= 2000.0f) axisIncrement = 200.0f;
        else if(dataRange <= 5000.0f) axisIncrement = 500.0f;
        else if(dataRange <= 10000.0f) axisIncrement = 1000.0f;
        else if(dataRange <= 50000.0f) axisIncrement = 5000.0f;
        else{
            axisIncrement = pow(10.0f, (int)floor(log10(dataRange)));
        }
    }

    int multBegin = int(ceil(dimScales[1].x/axisIncrement)),
        multEnd = int(floor(dimScales[1].y/axisIncrement));

    for(int i = multBegin; i <= multEnd; ++i){
        yAxisLabels.push_back(std::to_string(i*axisIncrement));
        sigFigs(yAxisLabels.back());
    }

}

void CVPlot::get_verbal_x_axis(){
    if(tags.size() < 1) return;

    xAxisLabels.clear();
    size_t L = tags[DIMX].size();

    for(size_t i = 0; i < L; ++i){
        xAxisLabels.push_back(tags[DIMX][i].front());
    }
}

void CVPlot::removeDataOutliers(){
    size_t nDim = numDimensions();
    for(size_t i = 0; i < nDim; ++i){
        size_t L = data[i].size();
        for(size_t j = 0; j < L; ++j){
            size_t S = data[i][j].size();
            if(S < 4) continue;
            DataSummary<float> dataSummary(data[i][j]);
            for(size_t k = 0; k < S;){
                if(data[i][j][k] > dataSummary.upperOutlierRange){
                    for(size_t a = 0; a < nDim; ++a){
                        if((data[a].size() <= j) || (data[a][j].size() <= k)) continue;
                        data[a][j].erase(data[a][j].begin() + k); // Erase all dimensions of this data point
                        --S;
                    }
                }
                else if(data[i][j][k] < dataSummary.lowerOutlierRange){
                    for(size_t a = 0; a < nDim; ++a){
                        if((data[a].size() <= j) || (data[a][j].size() <= k)) continue;
                        data[a][j].erase(data[a][j].begin() + k);
                        --S;
                    }
                }
                else ++k;
            }
        }
    }
}

void CVPlot::addLine(const uint8_t& orientation, const float& dimValue,
            const float& thickness, const sf::Color& color,
            const uint8_t& type, bool foreground){

    std::vector<sf::RectangleShape>* lineSpace = nullptr;
    if(foreground) lineSpace = &frontLines;
    else lineSpace = &backLines;

    switch(type){
        case CV_LINE_TYPE_BROKEN:{
            switch(orientation){
                case CV_PLOT_ALIGN_VERTICAL:{
                    float posY = plotBounds.top,
                            plotEnd = posY + plotBounds.height,
                            lineLength = 5.0f,
                            pos = getPlotXPos(dimValue);

                    while(posY < plotEnd){
                        if(posY + lineLength > plotEnd) lineLength = plotEnd - posY;
                        lineSpace->emplace_back(sf::Vector2f(thickness, lineLength));
                        lineSpace->back().setOrigin(thickness/2, 0.0f);
                        lineSpace->back().setPosition(pos, posY);
                        lineSpace->back().setFillColor(color);
                        posY += 2*lineLength;
                    }
                    break;
                }
                default:{ // Horizontal
                    float posX = plotBounds.left,
                            plotEnd = posX + plotBounds.width,
                            lineLength = 5.0f,
                            pos = getPlotYPos(dimValue);

                    while(posX < plotEnd){
                        if(posX + lineLength > plotEnd) lineLength = plotEnd - posX;
                        lineSpace->emplace_back(sf::Vector2f(lineLength, thickness));
                        lineSpace->back().setOrigin(0.0f, thickness/2);
                        lineSpace->back().setPosition(posX, pos);
                        lineSpace->back().setFillColor(color);
                        posX += 2*lineLength;
                    }
                    break;
                }
            }
            break;
        }
        default:{ // Solid
            switch(orientation){
                case CV_PLOT_ALIGN_VERTICAL:{
                    lineSpace->emplace_back(sf::Vector2f(thickness, plotBounds.height));
                    lineSpace->back().setOrigin(thickness/2, 0.0f);
                    float pos = getPlotXPos(dimValue);
                    lineSpace->back().setPosition(pos, plotBounds.top);
                    lineSpace->back().setFillColor(color);
                    break;
                }
                default:{ // Horizontal
                    lineSpace->emplace_back(sf::Vector2f(plotBounds.width, thickness));
                    lineSpace->back().setOrigin(0.0f, thickness/2);
                    float pos = getPlotYPos(dimValue);
                    lineSpace->back().setPosition(plotBounds.left, pos);
                    lineSpace->back().setFillColor(color);
                    break;
                }
            }
            break;
        }
    }
}

bool CVPlotPanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVViewPanel::update(event, mousePos)) return false;
    if(event.viewResized){
        logo.setPosition(bounds.left + bounds.width/2,
                         bounds.top + bounds.height/2);
    }

    if(viewPanelElements.size() < 1){
        if(logo.getColor().a < logoAlpha){
            sf::Color newColor = logo.getColor();
            ++newColor.a;
            logo.setColor(newColor);
        }
    }
    else{
        if(logo.getColor().a > 0){
            sf::Color newColor = logo.getColor();
            if(newColor.a > 2){
                newColor.a -= 2;
            }
            else newColor.a = 0;
            logo.setColor(newColor);
        }

        for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
            panel->update(event, mousePos);
        }
    }

    if(event.viewHasFocus && bounds.contains(mousePos) && event.captureFocus()){
        if(event.LMBhold){
            highlightBlocks.clear();
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)){
                for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
                    if(panel->getBounds().contains(event.lastFrameMousePosition)){
                        if(!anyEqual((CVPlot*)panel, selectedPlots)){
                            selectedPlots.push_back((CVPlot*)panel);
                        }
                        break;
                    }
                }

                for(auto& plot : selectedPlots){
                    if(event.isCaptured(*plot)){
                        for(auto& p : selectedPlots){
                            event.mouse_capture(*p);
                        }
                        break;
                    }
                }

            }
            else if(event.LMBholdFrames == 1){
                if(!ctrlPressed()){
                    selectedPlots.clear();
                }
                for(auto& panel : boost::adaptors::reverse(viewPanelElements)){
                    if(panel->getBounds().contains(mousePos)){
                        if(!anyEqual((CVPlot*)panel, selectedPlots)){
                            selectedPlots.push_back((CVPlot*)panel);
                        }
                        else remove((CVPlot*)panel, selectedPlots);
                        break;
                    }
                }
            }

            for(auto& plot : selectedPlots){
                highlightBlocks.emplace_back(sf::Vector2f(plot->getBounds().width, plot->getBounds().height));
                highlightBlocks.back().setFillColor(sf::Color::Transparent);
                highlightBlocks.back().setOutlineThickness(3.0f);
                highlightBlocks.back().setOutlineColor(plotHighlightColor);
                highlightBlocks.back().setPosition(plot->getPosition());
            }

        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
            highlightBlocks.clear();
            selectedPlots.clear();
        }

        if(event.keyLog.size() > 0){
            for(auto& key : event.keyLog){
                if(key == 127){
                    for(auto& selected : selectedPlots){
                        removePanelElement(selected);
                    }
                    selectedPlots.clear();
                    highlightBlocks.clear();
                }
            }
        }
    }

    return true;
}

bool CVPlotPanel::select_tags(const std::vector<std::string>& tags){

    bool output = false;

    if(numSelected() > 0){
        for(auto& plot : selectedPlots){
            output |= plot->select_tags(tags);
        }
    }
    else{
        for(auto& panel : viewPanelElements){
            output |= ((CVPlot*)panel)->select_tags(tags);
        }
    }

    return output;

}

bool CVPlotPanel::select_indices(const std::vector<unsigned int>& idx){
    bool output = false;
    if(numSelected() > 0){
        for(auto& plot : selectedPlots){
            output |= plot->select_indices(idx);
        }
    }
    else{
        for(auto& panel : viewPanelElements){
            output |= ((CVPlot*)panel)->select_indices(idx);
        }
    }

    return output;
}

bool CVPlotPanel::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;

    target->draw(logo);

    for(auto& block : highlightBlocks){
        target->draw(block);
    }

    for(auto& panel : viewPanelElements){
        panel->draw(target);
    }

    return true;

}

void CVPlotPanel::move(const sf::Vector2f& distance){
    CVViewPanel::move(distance);
    for(auto& block : highlightBlocks){
        block.move(distance);
    }
    logo.move(distance);
}

void CVPlotPanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

std::vector<unsigned int> CVPlotPanel::getAllSelectedIndices(){
    std::vector<unsigned int> output;
    for(auto& plot : selectedPlots){
        append(output, plot->getSelectedIndices());
    }
    rmDuplicates(output);
    return output;
}

std::vector<unsigned int> CVPlotPanel::getCommonSelectedIndices(){
    std::vector<unsigned int> output;
    for(auto& plot : selectedPlots){
        if(output.size() < 1) output = plot->getSelectedIndices();
        else output = shared(output, plot->getSelectedIndices());
    }
    return output;
}

}
