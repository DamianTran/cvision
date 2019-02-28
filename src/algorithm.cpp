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

#include "cvision/algorithm.hpp"
#include "EZC/toolkit/string.hpp"

#ifndef CVIS_TEXT_BOX
#define ALIGN_LEFT                      0
#define ALIGN_LEFT_MIDLINE              1
#define ALIGN_LEFT_BOTTOM               2
#define ALIGN_CENTER                    3
#define ALIGN_CENTER_MIDLINE            4
#define ALIGN_CENTER_BOTTOM             5
#define ALIGN_RIGHT                     6
#define ALIGN_RIGHT_MIDLINE             8
#define ALIGN_RIGHT_BOTTOM              9
#define ALIGN_VERTICAL                  10
#define ALIGN_VERTICAL_INVERTED         11
#define ALIGN_30                        12
#define ALIGN_45                        13
#define ALIGN_60                        14
#define ALIGN_HORIZONTAL                15
#endif // CVIS_TEXT_BOX

using namespace EZC;

namespace cvis{

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
                    else if(isCharType(displayTextString[j-1], ",\\/:._-") &&
                            (displayTextString[j] != '\n')){
                        displayTextString.insert(displayTextString.begin() + j, '\n');
                        break;
                    }
                    else if((abs(textItem.findCharacterPos(i).x -
                            textItem.findCharacterPos(j).x) > bounds.width - 2*padding) &&
                            (displayTextString[i-1] != '\n')){
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
    if(SetClipboardData(CF_TEXT, hg) == NULL){
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

    sf::FloatRect shape1Bounds, shape2Bounds;
    sf::Vector2f moveDist(0.0f, 0.0f),
                oriDist(0.0f, 0.0f),
                maxDist(0.0f, 0.0f);
    float sizeRatio;

    for(size_t i = 0, j, L = sprites.size(); i + 1 < L; ++i){
        for(j = i + 1; j < L; ++j){

            shape1Bounds = sprites[i].getGlobalBounds();
            shape2Bounds = sprites[j].getGlobalBounds();

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

                sprites[j].move(moveDist.x*sizeRatio, moveDist.y*sizeRatio);
                sprites[i].move(-moveDist.x*(1.0f-sizeRatio), -moveDist.y*(1.0f-sizeRatio));

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

    for(size_t i = 0, j, L = shapes.size(); i + 1 < L; ++i){
        for(j = i + 1; j < L; ++j){

            shape1Bounds = shapes[i]->getGlobalBounds();
            shape2Bounds = shapes[j]->getGlobalBounds();

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

                shapes[j]->move(moveDist.x*sizeRatio, moveDist.y*sizeRatio);
                shapes[i]->move(-moveDist.x*(1.0f-sizeRatio), -moveDist.y*(1.0f-sizeRatio));

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

    for(size_t i = 0, j, L = shapes.size(); i + 1 < L; ++i){
        for(j = i + 1; j < L; ++j){

            shape1Bounds = shapes[i]->getGlobalBounds();
            shape2Bounds = shapes[j]->getGlobalBounds();

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

                shapes[j]->move(moveDist.x*sizeRatio, moveDist.y*sizeRatio);
                shapes[i]->move(-moveDist.x*(1.0f-sizeRatio), -moveDist.y*(1.0f-sizeRatio));

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

void modulate(sf::Image& image, const sf::Color& newColor)
{

    sf::Color tmp;
    for(size_t x = 0, y, xL = image.getSize().x, yL = image.getSize().y; x < xL; ++x)
    {
        for(y = 0; y < yL; ++y)
        {
            tmp = image.getPixel(x, y);
            tmp.r = newColor.r;
            tmp.g = newColor.g;
            tmp.b = newColor.b;
            image.setPixel(x, y, tmp);
        }
    }

}

void expand_canvas(sf::Image& image, const sf::Vector2i& distance, const sf::Color& background)
{

    sf::Image tmp;
    tmp.create(image.getSize().x + distance.x,
               image.getSize().y + distance.y);

    tmp.copy(image, distance.x/2, distance.y/2, sf::IntRect(0,0,0,0), true);

    image = tmp;

}

void gaussianBlur(sf::Image& image, const int& radius)
{

    sf::Color tmp;

    int x;
    int y;
    int xs;
    int xmin;
    int xmax;
    int ys;
    int ymin;
    int ymax;

    int xL = image.getSize().x;
    int yL = image.getSize().y;
    int rs = ceil(radius * 2.57); // Radius where integral drops to ~0

    double wsum;
    double weight;
    double dsq;

    double _2rsq = 2 * radius * radius;
    double PI2rsq = PI * _2rsq;

    double r;
    double g;
    double b;
    double a;

    for(x = 0; x < xL; ++x)
    {
        for(y = 0; y < yL; ++y)
        {
            r = 0;
            g = 0;
            b = 0;
            a = 0;

            wsum = 0.0;

            xmin = x - rs < 0 ? 0 : x - rs;
            xmax = x + rs < xL ? x + rs : xL;
            ymin = y - rs < 0 ? 0 : y - rs;
            ymax = y + rs < yL ? y + rs : yL;

            for(xs = xmin; xs < xmax; ++xs)
            {
                for(ys = ymin; ys < ymax; ++ys)
                {
                    tmp = image.getPixel(x, y);

                    dsq = (xs - x)*(xs - x) + (ys - y)*(ys - y);
                    weight = exp(-dsq/_2rsq)/PI2rsq;

                    r += weight * tmp.r;
                    g += weight * tmp.g;
                    b += weight * tmp.b;
                    a += weight * tmp.a;

                    wsum += weight;

                }
            }

            image.setPixel(x, y, sf::Color(round(r/wsum), round(g/wsum), round(b/wsum), round(a/wsum)));
        }
    }

}

}
