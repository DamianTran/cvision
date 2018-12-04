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

#include "cvision/cvis_algorithm.hpp"

using namespace AIALG;

namespace cvis{

void setImageColor(sf::Image& img, const sf::Color& newColor){
    size_t x = 0;
    sf::Vector2u imgSize = img.getSize();
    for(size_t y = 0; y < imgSize.y; ++y){
        for(x = 0; x < imgSize.x; ++x){
            img.setPixel(x, y, newColor);
        }
    }
}

sf::Color textToColor(const std::string& input){
    sf::Color output(0,0,0,255);
    StringVector parseBuff;
    splitString(input, parseBuff, DELIM_STANDARD);
    std::vector<unsigned int> values;

    if(parseBuff.size() > 1){
        for(auto& str : parseBuff){
            try{
                int newVal = std::stoi(str);
                if(newVal < 0) newVal = 0;
                else if(newVal > 255) newVal = 255;
                values.push_back(newVal);
            }catch(...){ }
        }
        if(values.size() > 0) output.r = values.front();
        if(values.size() > 1) output.b = values[1];
        if(values.size() > 2) output.g = values[2];
        if(values.size() > 3) output.a = values[3];
    }

    if(values.size() < 1){
        if(cmpString(input, "red")) output = sf::Color::Red;
        else if(cmpString(input, "sky")) output = sf::Color(135,206,235);
        else if(cmpString(input, "navy")) output = sf::Color(0,0,128);
        else if(cmpString(input, "blue")) output = sf::Color(0,0,255);
        else if(cmpString(input, "lime")) output = sf::Color(0,255,0);
        else if(cmpString(input, "olive")) output = sf::Color(128,128,0);
        else if(cmpString(input, "pine")) output = sf::Color(0,100,0);
        else if(cmpString(input, "green")) output = sf::Color(0,128,0);
        else if(cmpString(input, "magenta")) output = sf::Color::Magenta;
        else if(cmpString(input, "purple")) output = sf::Color(128,0,128);
        else if(cmpString(input, "indigo")) output = sf::Color(75,0,130);
        else if(cmpString(input, "violet")) output = sf::Color(238,130,238);
        else if(cmpString(input, "yellow")) output = sf::Color::Yellow;
        else if(cmpString(input, "peach")) output = sf::Color(255,218,185);
        else if(cmpString(input, "white")) output = sf::Color::White;
        else if(cmpString(input, "turquoise")) output = sf::Color(64,224,208);
        else if(cmpStringToList(input, { "aqua", "cyan" })) output = sf::Color(0,183,255);
        else if(cmpStringToList(input, {"ocean", "sea"})) output = sf::Color(0, 119, 190);
        else if(cmpString(input, "orange")) output = sf::Color(255,165,0);
        else if(cmpString(input, "gold")) output = sf::Color(255,215,0);
        else if(cmpString(input, "tomato")) output = sf::Color(255,99,71);
        else if(cmpString(input, "coral")) output = sf::Color(255,127,80);
        else if(cmpString(input, "pink")) output = sf::Color(255,192,203);
        else if(cmpString(input, "maroon")) output = sf::Color(128,0,0);
        else if(cmpString(input, "silver")) output = sf::Color(192,192,192);
        else if(cmpString(input, "slate")) output = sf::Color(112,128,144);
        else if(cmpString(input, "grey")) output = sf::Color(128,128,128);

        if(cmpString(input, "dark")) darken(output, 60);
        else if(cmpStringToList(input, { "strong", "bold", "dim" })) darken(output, 30);
        else if(cmpString(input, "pale")) brighten(output, 30);
        else if(cmpStringToList(input, {"light", "bright"})) brighten(output, 60);

        if(cmpString(input, "translucent")) output.a = 128;
        else if(cmpString(input, "transparent")) output.a = 255;
    }

    return output;

}

ColorTheme::ColorTheme(const std::vector<sf::Color>& colors):
    std::vector<sf::Color>(colors),
    initStates(colors){ }

const sf::Color& ColorTheme::getInitial(const unsigned int& index) const{
    return initStates[index];
}

const sf::Color& ColorTheme::getInitial(const sf::Color& color) const{
    for(size_t i = 0; i < size(); ++i){
        if((*this)[i] == color) return initStates[i];
    }
    throw std::invalid_argument("No color matching reference in color theme");
}

void ColorTheme::push_back(const sf::Color& color){
    std::vector<sf::Color>::push_back(color);
    initStates.push_back(color);
}

void ColorTheme::operator=(const std::vector<sf::Color>& colors){
    std::vector<sf::Color>::operator=(colors);
    initStates = colors;
}

void ColorTheme::erase(std::vector<sf::Color>::iterator it){
    unsigned int index = 0;
    for(std::vector<sf::Color>::iterator bit = begin();
        bit != end(); ++bit, ++index){
        if(bit == it){
            initStates.erase(initStates.begin() + index);
            break;
        }
    }
    std::vector<sf::Color>::erase(it);
}
void ColorTheme::insert(std::vector<sf::Color>::iterator it, const sf::Color& value){
    unsigned int index = 0;
    for(std::vector<sf::Color>::iterator bit = begin();
        bit != end(); ++bit, ++index){
        if(bit == it){
            initStates.insert(initStates.begin() + index, value);
            break;
        }
    }
    std::vector<sf::Color>::insert(it, value);
}

void alignText(std::vector<sf::Text>& textItems, std::vector<sf::Vector2f> coords){
    std::vector<sf::Vector2f>::iterator coordsIT = coords.begin();
    for(std::vector<sf::Text>::iterator it = textItems.begin();
    it != textItems.end(); ++it){
        it->setPosition(*coordsIT);
        if(coordsIT != coords.end()) ++coordsIT;
    }
}
void alignShapes(std::vector<sf::Shape*>& textItems, std::vector<sf::Vector2f> coords){
    std::vector<sf::Vector2f>::iterator coordsIT = coords.begin();
    for(std::vector<sf::Shape*>::iterator it = textItems.begin();
    it != textItems.end(); ++it){
        (*it)->setPosition(*coordsIT);
        if(coordsIT != coords.end()) ++coordsIT;
    }
}

void sigFigs(std::string& numeric){
    if(numeric.find('.') >= numeric.size()) return;
    while(numeric.back() == '0') numeric.pop_back();
    while(numeric.back() == '.') numeric.pop_back();
}

std::string sigFigs(const float& number){
    std::string output = std::to_string(number);
    sigFigs(output);
    return output;
}
void sleep(float time){
    std::chrono::high_resolution_clock::time_point t0 = TIME_NOW,
                                                    t1 = t0;
    while(std::chrono::duration_cast<std::chrono::duration<float>>(t1-t0).count() < time){
        t1 = TIME_NOW;
    }
}

void wrapText(sf::Text& textItem, const sf::FloatRect& bounds, const float& padding){
    sf::FloatRect textBounds = textItem.getGlobalBounds();

    if(textBounds.width > bounds.width - 2*padding){
        std::string displayTextString = textItem.getString();

        for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
            if(textItem.findCharacterPos(i).x - bounds.left > bounds.width - 2*padding){
                for(int j = i; j > 0; --j){
                    if(displayTextString[j-1] == ' '){
                        displayTextString[j-1] = '\n';
                        break;
                    }
                    else if(isCharType(displayTextString[j-1], ",\\/:._-")){
                        displayTextString.insert(displayTextString.begin() + j, '\n');
                        break;
                    }
                    else if(abs(textItem.findCharacterPos(i).x -
                            textItem.findCharacterPos(j).x) > bounds.width - 2*padding){
                        displayTextString.insert(displayTextString.begin() + i - 1, '\n');
                        ++i;
                        break;
                    }
                }

                textItem.setString(displayTextString);
                textBounds = textItem.getGlobalBounds();
                if(textBounds.width < bounds.width - 2*padding) break;
            }
        }
    }
}

sf::Vector2f getTextHorizontalAlignment(sf::Text& text, sf::Rect<float>& bounds, uint8_t alignment,
                         float borderPadding){
    sf::Vector2f cPosition(bounds.left, text.getPosition().y);
    sf::Rect<float> textBounds = text.getLocalBounds();
    switch(alignment){
        case ALIGN_LEFT:{
            cPosition.x += borderPadding;
            break;
        }
        case ALIGN_CENTER:{
            cPosition.x += (bounds.width/2 - textBounds.width/2);
            break;
        }
        case ALIGN_RIGHT:{
            cPosition.x += (bounds.width - textBounds.width - borderPadding);
            break;
        }
        default:{
            cPosition.x += (bounds.width/2 - textBounds.width/2);
            break;
        }
    }
    return cPosition;
}

sf::Rect<float> averageTextSize(std::vector<sf::Text>& textItems){
    sf::Rect<float> output = textItems.begin()->getLocalBounds();
    size_t L = 1;
    for(std::vector<sf::Text>::iterator it = textItems.begin();
        it != textItems.end(); ++it){
            if(it == textItems.begin()){
                continue;
            }
            output += it->getLocalBounds();
            ++L;
        }
    if(L > 1) output /= L;
    return output;
}

bool copyToClipboard(const std::string& cpyString){
    #if defined WIN32 || defined _WIN32
    if(!OpenClipboard(0)){
        std::cout << ">> Unable to open clipboard\n";
        return false;
    }
    if(!EmptyClipboard()){
        std::cout << ">> Unable to empty clipboard\n";
        return false;
    }
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, cpyString.size()+1);
    if(!hg){
        std::cout << ">> Unable to allocate memory in Windows environment\n";
        CloseClipboard();
        return false;
    }
    memcpy(GlobalLock(hg), cpyString.c_str(), cpyString.size()+1);
    GlobalUnlock(hg);
    if( ::SetClipboardData(CF_TEXT, hg) == NULL){
        std::cout << ">> Unable to set clipboard data\n";
        return false;
    }
    CloseClipboard();
    GlobalFree(hg);
    return true;
    #elif defined __APPLE__
    system((std::string("echo '") + cpyString + "' | pbcopy").c_str());
    return true;
    #elif defined __linux__
    std::cout << ">> Clipboard support for Linux OS is not yet implemented\n";
    return false;
    #endif

    std::cout << ">> Could not detect operating system environment for clipboard copy\n";
    return false;
}

std::string getClipboardText(){

    #if defined WIN32 || defined _WIN32
    if(!OpenClipboard(0)){
        std::cout << ">> Unable to open clipboard\n";
        return std::string();
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if(hData == nullptr){
        std::cout << ">> Unable to find clipboard data\n";
        return std::string();
    }
    char* clipText = (char*)(GlobalLock(hData));
    if(clipText == nullptr){

        std::cout << ">> Unable to access clipboard data\n";

        GlobalUnlock(hData);

        return std::string();
    }
    std::string output(clipText);
    GlobalUnlock(hData);
    CloseClipboard();
    return output;
    #elif defined __APPLE__
    std::string output;
    system("pbpaste > memory/OSXclip.tmp");
    //if(access("memory/OSXclip.tmp", F_OK)){
        FILE* inFILE = fopen("memory/OSXclip.tmp", "rb");
        fseek(inFILE, 0, SEEK_END);
        size_t dataSIZE = ftell(inFILE);
        fseek(inFILE, 0, SEEK_SET);

        if(dataSIZE > 1){
            char clipData[dataSIZE];
            clipData[dataSIZE-1] = '\0';
            fread(clipData, dataSIZE-1, sizeof(char), inFILE);

            output = clipData;
        }

        remove("memory/OSXclip.tmp");
    //}
    return output;
    #elif defined __linux__
    return std::string();
    #else
    std::cout << ">> Could not detect operating system environment for clipboard access\n";
    return std::string();
    #endif

}

void physicsSpread(std::vector<sf::Sprite>& sprites,
                   const float& strength,
                   const BYTE& direction){

    bool moveDir;
    sf::FloatRect sp1Bounds, sp2Bounds;

    if(direction & CV_DIRECTION_X){
        for(auto& sp1 : sprites){
            for(auto& sp2 : sprites){

                if(&sp1 == &sp2) continue;

                sp1Bounds = sp1.getGlobalBounds(),
                sp2Bounds = sp2.getGlobalBounds();

                if(sp1Bounds.intersects(sp2Bounds)){

                    moveDir = false;
                    if((sp2Bounds.left + sp2Bounds.width/2) >
                       (sp1Bounds.left + sp1Bounds.width/2)) moveDir = true;
                    else if((sp2Bounds.left + sp2Bounds.width/2) ==
                       (sp1Bounds.left + sp1Bounds.width/2)) moveDir = (rand(0.0f,10.0f) >= 5.0f);

                    if(moveDir){
                        sp2.move(sp2Bounds.width*strength, 0.0f);
                    }
                    else{
                        sp2.move(-sp2Bounds.width*strength, 0.0f);
                    }

                }
            }
        }
    }

}

void physicsSpread(std::vector<sf::Shape*>& shapes,
                   const float& strength,
                   const BYTE& direction){

    sf::FloatRect shape1Bounds, shape2Bounds;
    sf::Vector2f moveDist(0.0f, 0.0f),
                oriDist(0.0f, 0.0f),
                maxDist(0.0f, 0.0f);
    float sizeRatio;

    for(auto& shape1 : shapes){
        for(auto& shape2 : shapes){
            if(shape1 == shape2) continue;

            shape1Bounds = shape1->getGlobalBounds();
            shape2Bounds = shape2->getGlobalBounds();

            if(shape1Bounds.intersects(shape2Bounds)){

                sizeRatio = (shape1Bounds.width*shape1Bounds.height)/
                (shape1Bounds.width*shape1Bounds.height + shape2Bounds.height*shape2Bounds.width);

                if(direction & CV_DIRECTION_X){
                    oriDist.x = shape1Bounds.left + shape1Bounds.width/2 - shape2Bounds.left - shape2Bounds.width/2;
                    maxDist.x = shape1Bounds.width/2 + shape2Bounds.width/2;
                    if((shape2Bounds.left + shape2Bounds.width/2) >
                       (shape1Bounds.left + shape1Bounds.width/2)) moveDist.x = (maxDist.x - oriDist.x)*strength/2;
                    else if((shape2Bounds.left + shape2Bounds.width/2) <
                       (shape1Bounds.left + shape1Bounds.width/2)) moveDist.x = (oriDist.x - maxDist.x)*strength/2;
                    else{
                        if(rand(0.0f,10.0f) >= 5.0f) moveDist.x = (maxDist.x - oriDist.x)*strength;
                        else moveDist.x = (oriDist.x - maxDist.x)*strength;
                   }
                }

                if(direction & CV_DIRECTION_Y){
                    oriDist.y = shape1Bounds.top + shape1Bounds.height/2 - shape2Bounds.top - shape2Bounds.height/2;
                    maxDist.y = shape1Bounds.height/2 + shape2Bounds.height/2;
                    if((shape2Bounds.top + shape2Bounds.height/2) >
                       (shape1Bounds.top + shape1Bounds.height/2)) moveDist.y = (maxDist.y - oriDist.y)*strength/2;
                    else if((shape2Bounds.top + shape2Bounds.height/2) <
                       (shape1Bounds.top + shape1Bounds.height/2)) moveDist.y = (oriDist.y - maxDist.y)*strength/2;
                    else{
                        if(rand(0.0f,10.0f) >= 5.0f) moveDist.y = (maxDist.y - oriDist.y)*strength/2;
                        else moveDist.y = (oriDist.y - maxDist.y)*strength/2;
                   }
                }

                shape2->move(moveDist.x*sizeRatio, moveDist.y*sizeRatio);
                shape1->move(-moveDist.x*(1.0f-sizeRatio), -moveDist.y*(1.0f-sizeRatio));

            }
        }
    }
}

void physicsSpread(std::vector<sf::Sprite*>& shapes,
                   const float& strength,
                   const BYTE& direction){

    sf::FloatRect shape1Bounds, shape2Bounds;
    sf::Vector2f moveDist(0.0f, 0.0f),
                oriDist(0.0f, 0.0f),
                maxDist(0.0f, 0.0f);
    float sizeRatio;

    for(auto& shape1 : shapes){
        for(auto& shape2 : shapes){
            if(shape1 == shape2) continue;

            shape1Bounds = shape1->getGlobalBounds();
            shape2Bounds = shape2->getGlobalBounds();

            if(shape1Bounds.intersects(shape2Bounds)){

                sizeRatio = (shape1Bounds.width*shape1Bounds.height)/
                (shape1Bounds.width*shape1Bounds.height + shape2Bounds.height*shape2Bounds.width);

                if(direction & CV_DIRECTION_X){
                    oriDist.x = shape1Bounds.left + shape1Bounds.width/2 - shape2Bounds.left - shape2Bounds.width/2;
                    maxDist.x = shape1Bounds.width/2 + shape2Bounds.width/2;
                    if((shape2Bounds.left + shape2Bounds.width/2) >
                       (shape1Bounds.left + shape1Bounds.width/2)) moveDist.x = (maxDist.x - oriDist.x)*strength/2;
                    else if((shape2Bounds.left + shape2Bounds.width/2) <
                       (shape1Bounds.left + shape1Bounds.width/2)) moveDist.x = (oriDist.x - maxDist.x)*strength/2;
                    else{
                        if(rand(0.0f,10.0f) >= 5.0f) moveDist.x = (maxDist.x - oriDist.x)*strength;
                        else moveDist.x = (oriDist.x - maxDist.x)*strength;
                   }
                }

                if(direction & CV_DIRECTION_Y){
                    oriDist.y = shape1Bounds.top + shape1Bounds.height/2 - shape2Bounds.top - shape2Bounds.height/2;
                    maxDist.y = shape1Bounds.height/2 + shape2Bounds.height/2;
                    if((shape2Bounds.top + shape2Bounds.height/2) >
                       (shape1Bounds.top + shape1Bounds.height/2)) moveDist.y = (maxDist.y - oriDist.y)*strength/2;
                    else if((shape2Bounds.top + shape2Bounds.height/2) <
                       (shape1Bounds.top + shape1Bounds.height/2)) moveDist.y = (oriDist.y - maxDist.y)*strength/2;
                    else{
                        if(rand(0.0f,10.0f) >= 5.0f) moveDist.y = (maxDist.y - oriDist.y)*strength/2;
                        else moveDist.y = (oriDist.y - maxDist.y)*strength/2;
                   }
                }

                shape2->move(moveDist.x*sizeRatio, moveDist.y*sizeRatio);
                shape1->move(-moveDist.x*(1.0f-sizeRatio), -moveDist.y*(1.0f-sizeRatio));

            }
        }
    }
}

double get_angle(const sf::Vector2f& origin, const sf::Vector2f& destination){
    if(origin == destination) return 0.0;
    double dist = sqrt(pow(double(destination.x - origin.x),2) + pow(double(destination.y - origin.y), 2));
    double vDist = destination.y - origin.y;
    if(destination.y >= origin.y){
        if(destination.x >= origin.x) return asin(vDist/dist); // Q1
        else return PI - asin(vDist/dist); // Q2
    }
    else{
        if(destination.x >= origin.x) return 2*PI + asin(vDist/dist); // Q4
        else return PI - asin(vDist/dist); // Q3
    }
}

sf::Vector2f internal_arc(const sf::Vector2f& origin,
                          const sf::FloatRect& bounds,
                          const float& distance,
                          const float& resolution){

    if((origin.x - distance > bounds.left) &&
       (origin.y - distance > bounds.top) &&
       (origin.x + distance < bounds.left + bounds.width) &&
       (origin.y + distance < bounds.top + bounds.height)) return sf::Vector2f(0.0f,2*PI);
       // None of the surfaces are contacting the boundary

    sf::Vector2f output(NAN, NAN);
    float angle = 0.0f;
    sf::Vector2f spoint;
    bool bMeasure = false;

    while(isnan(output.y)){ // Sample in a circle
        spoint = radial_position(origin, distance, -angle);
        if(!bMeasure){
            if(!bounds.contains(spoint)){
                bMeasure = true;
            }
        }
        else if(isnan(output.x)){
            if(bounds.contains(spoint)){
                output.x = angle;
            }
        }
        else if(!bounds.contains(spoint)){
            output.y = angle - resolution;
        }
        angle += resolution;
        if((angle >= 2*PI) && (isnan(output.x))) return sf::Vector2f(0.0f, 2*PI); // Unknown arc
    }

    return output;
}

}
