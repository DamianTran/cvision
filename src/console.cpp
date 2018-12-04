#include "cvision/console.hpp"

using namespace AIALG;

namespace cvis{

CVConsoleLog::CVConsoleLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType,
              const std::string& logFile, CVTypeBox* usrEntryBox):
                  CVTextLog(View, position, width, height, textInfo, fillColor, borderColor, borderWidth,
                            animType, logFile, usrEntryBox){

    setTextPanelPadding(3.0f);
    initialMessageCount = 150;
    messageLoadInc = 50;

    disableWidthTextWrap();

    setIncomingMsgPanelFillColor(sf::Color::Transparent);
    setMsgPanelFillColor(sf::Color::Transparent);
    setUserTextAlign(ALIGN_LEFT);
    setHighlightPanelFillColor(sf::Color::Transparent);
}

}
