//
//  LanguageManager.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 8. 13..
//
//

#include "LanguageManager.h"
#include "GameManager.h"

LanguageManager::LanguageManager(){
    
}
LanguageManager* LanguageManager::m_mySingleton = NULL;
LanguageManager* LanguageManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = new LanguageManager();
        m_mySingleton->retain();
    }
    return m_mySingleton;
}

const char* LanguageManager::getLocalizedFont(){
    
    cocos2d::LanguageType type = getLanguageType();
    std::string str;
    if (type == LanguageType::KOREAN || type == LanguageType::ENGLISH) {
        return "BMDOHYEON.ttf";
    }else{
//        return "visitor1.ttf";
//        return "arial";
        return "Helveticas";
    }
}
cocos2d::Label* LanguageManager::getLocalizedLabel(const char* text, Color4B color, int size){
    if (LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN) {
        Label* lbl = Label::createWithTTF(text, getLocalizedFont(), size);
        lbl->setTextColor(color);
        return lbl;
    }else if (LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN) {
        Label* lbl = Label::createWithTTF(text, getLocalizedFont(), size*0.9f);
        lbl->setTextColor(color);
        return lbl;
    }else{
        
//                Label* lbl = Label::createWithTTF("label", "bitdust1.ttf", 80);
        Label* lbl = Label::createWithSystemFont(text, getLocalizedFont(), size);
        lbl->setTextColor(color);
//        lbl->enableShadow();
        return lbl;
    }
}
cocos2d::Label* LanguageManager::getLocalizedLabel(){
    return getLocalizedLabel("", Color4B::WHITE);
}
cocos2d::LanguageType LanguageManager::getLanguageType(){
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//    return LanguageType::KOREAN; // test
//#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    int languageType = UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_LANGUAGE, -1);
    cocos2d::LanguageType type = cocos2d::LanguageType::ENGLISH;
    if (languageType > -1) {
        type = (LanguageType)languageType;
    }else{
        type = Application::getInstance()->getCurrentLanguage();
        if(type == LanguageType::KOREAN){
            type = LanguageType::KOREAN;
        }else if(type == LanguageType::SPANISH){
            type = LanguageType::SPANISH;
        }else if(type == LanguageType::RUSSIAN){
            type = LanguageType::RUSSIAN;
        }else if(type == LanguageType::TURKISH){
            type = LanguageType::TURKISH;
        }else if(type == LanguageType::GERMAN){
            type = LanguageType::GERMAN;
        }else if(type == LanguageType::FRENCH){
            type = LanguageType::FRENCH;
        }else if(type == LanguageType::JAPANESE){
            type = LanguageType::JAPANESE;
        }else{
            type = LanguageType::ENGLISH;
        }
    }
    return type;
}
std::string LanguageManager::getText(std::string textId){
    if(languageTable.find(textId) == languageTable.end()){
        return textId;
    }
    ValueMap row = languageTable[textId].asValueMap();
    cocos2d::LanguageType type = getLanguageType();
    std::string str;
    if (type == LanguageType::KOREAN) {
        str = row.at("korean").asString();
    }
    else if (type == LanguageType::RUSSIAN) {
        str = row.at("russian").asString();
    }
    else if (type == LanguageType::SPANISH) {
        str = row.at("spanish").asString();
    }
    else if (type == LanguageType::FRENCH) {
        str = row.at("french").asString();
    }else if (type == LanguageType::GERMAN) {
        str = row.at("german").asString();
    }else if (type == LanguageType::TURKISH) {
        str = row.at("turkish").asString();
    }else if (type == LanguageType::JAPANESE) {
        str = row.at("japanese").asString();
    }else if (type == LanguageType::PORTUGUESE) {
        str = row.at("pt").asString();
    }else{
        str = row.at("english").asString();
    }
//    str = row.at("english").asString();
    int index = (int)str.find("\\n");
    if (index >= 0) {
        str.replace(index, 2, "\n");
    }
    while((index = (int)str.find("`")) >= 0){
        if (index >= 0) {
            str.replace(index, 1, ",");
        }
    }
//    log("getText: %s", str.c_str());
    return str;
}
void LanguageManager::loadLanguageSheet(){
    std::string fileName = "Cartoon Craft Language Sheet - language.csv";
    std::string csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    
    // change , to ` between " and " that is automatically created when , exists in the value
    int counter = 0;
    size_t quoteIndex = 0;
    size_t lastQuoteIndex = 0;
    bool isFirstQuote = true;
    while(counter < csvStr.length()){
        quoteIndex = csvStr.find("\"", quoteIndex, 1);
        if (quoteIndex == std::string::npos) {
            break;
        }
        if(!isFirstQuote){
            std::string subStr = csvStr.substr(lastQuoteIndex, quoteIndex - lastQuoteIndex);
            std::replace( subStr.begin(), subStr.end(), ',', '`');
            csvStr.replace(lastQuoteIndex, quoteIndex - lastQuoteIndex, subStr);
        }
        lastQuoteIndex = quoteIndex;
        isFirstQuote = !isFirstQuote;
        csvStr.replace(quoteIndex, 1, "");
    }
    
    ValueVector rows = GameManager::getInstance()->split(csvStr, "\r\n");
    ValueVector keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    for (int i = 1; i < (int)rows.size(); i ++) {
        ValueMap userParams;
        std::string strRow = rows.at(i).asString();
        
        ValueVector params = GameManager::getInstance()->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j ++) {
            std::string value =  params.at(j).asString();
            std::replace( value.begin(), value.end(), '`', ','); // change ` to ,
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
            if(j == 1){
                log("%s", value.c_str());
            }
        }
        std::string rowKey = params.at(0).asString();
        languageTable[rowKey] = userParams;
    }
    log("end language");
}

void LanguageManager::setLocalizedString(Label* lbl, std::string str){
    std::string text = getText(str.c_str());
    if (text.compare(lbl->getString()) != 0) {
        lbl->setString(text);
    }
}
void LanguageManager::setLocalizedString(Text* lbl, std::string str){
    std::string text = getText(str.c_str());
    setLocalizedStringNotKey(lbl, text);
}
void LanguageManager::setLocalizedStringNotKey(Text* lbl, std::string text){
//    if (text.compare(lbl->getString()) != 0) {
        lbl->setString(text);
        lbl->setFontName(getLocalizedFont());
        lbl->setFontSize(lbl->getFontSize());
        if ((lbl->getOutlineSize() > 0 || lbl->getChildren().size() > 0) &&
            getLanguageType() != LanguageType::KOREAN &&
            getLanguageType() != LanguageType::ENGLISH) {
            lbl->removeAllChildren();
            int outlineSize= lbl->getOutlineSize()*0.5f;
            Text* lblClone = (Text*)lbl->clone();
            lbl->addChild(lblClone, -1);
            lblClone->setTag(3);
            for (int i = 0; i < 3; i++) {
                lblClone = (Text*)lblClone->clone();
                lbl->addChild(lblClone, -1);
                lblClone->setTag(i);
            }
            Vec2 centerPos = Vec2(lblClone->getContentSize().width*lblClone->getAnchorPoint().x, lblClone->getContentSize().height*lblClone->getAnchorPoint().y);
            for (int i = 0; i < 4; i++) {
                lblClone = (Text*)lbl->getChildByTag(i);
                if( i == 0){
                    lblClone->setPosition(centerPos + Vec2(-outlineSize, 0));
                }else if( i == 1){
                    lblClone->setPosition(centerPos + Vec2(outlineSize, 0));
                }else if( i == 2){
                    lblClone->setPosition(centerPos + Vec2(0, outlineSize));
                }else if( i == 3){
                    lblClone->setPosition(centerPos + Vec2(0, -outlineSize));
                }
                lblClone->setColor(Color3B::BLACK);
            }
            lbl->setLocalZOrder(lbl->getLocalZOrder() + 1);
        }
//    }
}
