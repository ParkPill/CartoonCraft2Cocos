//
//  PPLabel.cpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 20/03/2018.
//

#include "PPLabel.h"
#include "LanguageManager.h"
#include "GameManager.h"
bool PPLabel::init()
{
    if ( !Node::init())
    {
        return false;
    }
    spriteBatch = SpriteBatchNode::create("CartoonCraftNumber.png");
    spriteBatch->getTexture()->setAntiAliasTexParameters();
    this->addChild(spriteBatch);
    return true;
}
PPLabel* PPLabel::create(std::string txt, int size, Color3B color, bool isSystemLabel){
    PPLabel* lbl = PPLabel::create();
    lbl->setFontColor(color, false);
    lbl->setFontSize(size, false);
    lbl->isImageLabel = !isSystemLabel;
    lbl->setString(txt);
    return lbl;
}
PPLabel* PPLabel::create(std::string txt, int size, Color3B color, bool isSystemLabel, bool isBordered, TextHAlignment align, bool shadow){
    PPLabel* lbl = PPLabel::create();
    lbl->setFontColor(color, false);
    lbl->setFontSize(size, false);
    lbl->setBorderEnabled(isBordered, false);
    lbl->setTextHAlignment(align, false);
    lbl->setShadowEnabled(shadow, 8, false);
    lbl->isImageLabel = !isSystemLabel;
    lbl->setString(txt);
    return lbl;
}
void PPLabel::setWidth(float width, bool refresh){
    textWidth = width;
    if(refresh){
        setString(text);
    }
    if(lblNormal != nullptr){
        lblNormal->setWidth(width);
    }
}
void PPLabel::setFontColor(Color3B color, bool refresh){
    if(fontColor != color){
        fontColor = color;
        if(refresh){
            setString(text);
        }
    }
}
void PPLabel::setFontSize(int size, bool refresh){
    fontSize = size;
    if(refresh){
        setString(text);
    }
}
void PPLabel::setShadowEnabled(bool enable, float distance, bool refresh){
    isShadowEnabled = enable;
    if(refresh){
        setString(text);
    }
}
void PPLabel::setBorderEnabled(bool enable, bool refresh){
    isBorderEnabled = enable;
    if(refresh){
        setString(text);
    }
}
void PPLabel::setTextHAlignment(TextHAlignment align, bool refresh){
    hAlign = align;
    if(lblNormal != nullptr){
        lblNormal->setAlignment(align);
    }
    if(refresh){
        setString(text);
    }
}
void PPLabel::setAnchorPoint(const Vec2& anchor) {
    Node::setAnchorPoint(anchor);
    if(lblNormal != nullptr){
        lblNormal->setAnchorPoint(anchor);
    }
}
void PPLabel::setString(std::string txt){
    text = txt;
    
    if (LM->getLanguageType() != LanguageType::ENGLISH && !isImageLabel) {
        if(lblNormal == nullptr){
            if(LM->getLanguageType() == LanguageType::KOREAN){
                lblNormal = Label::createWithTTF(text, LM->getLocalizedFont(), fontSize*1.2f);
            }else{
                lblNormal = Label::createWithSystemFont(text, LM->getLocalizedFont(), fontSize);
            }
            
            lblNormal->setTextColor(Color4B(fontColor, 255));
            if(isShadowEnabled || isBorderEnabled){
                lblNormal->enableShadow(Color4B(48, 53, 45, 255), Size(4, -4));
            }
            
            this->addChild(lblNormal);
        }
        lblNormal->setString(txt);
        lblNormal->setTextColor(Color4B(fontColor));
        if(hAlign == TextHAlignment::LEFT){
            lblNormal->setAnchorPoint(Point(0, lblNormal->getAnchorPoint().y));
        }else if(hAlign == TextHAlignment::CENTER){
            lblNormal->setAnchorPoint(Point(0.5, lblNormal->getAnchorPoint().y));
        }else if(hAlign == TextHAlignment::RIGHT){
            lblNormal->setAnchorPoint(Point(1, lblNormal->getAnchorPoint().y));
        }
        
        return;
    }
    
    sprites.clear();
    Vector<Sprite*> shadowSprites;
    spriteBatch->removeAllChildren();
    char buf[10];
    if(text.length() == 0){
        return;
    }
    int spaceTag = 7;
    for(int i = 0; i < text.length(); i++){
        char ch = text.substr(i, 1).c_str()[0];
//        log("char: %c", ch);
        std::string chStr = text.substr(i, 1);
        if (ch >= 'A' && ch <= 'Z') {
            ch += 32;
            std::stringstream ss;
            std::string target;
            char mychar=ch;
            ss << mychar;
            ss >> target;
            chStr = target;
        }else{
            
        }
        
        if(ch == '/'){
            chStr = "slash";
        }else if(ch == ':'){
            chStr = "colon";
        }else if(ch == '.'){
            chStr = "dot";
        }else if(ch == ' '){
            chStr = "space";
        }else if(ch == '\xe2'){
            if (text.substr(i, 3).compare("\xe2\x80\x99") == 0) {
                i += 2;
                chStr = "'";
            }else{
                chStr = "'";
            }
        }else if(ch == '\x80'){
            chStr = "'";
        }else if(ch == '\x99' || ch == '\x98'){
            chStr = "'";
        }
        if (isBorderEnabled && ch != ' '){
            sprintf(buf, "%s%s.png", chStr.c_str(), chStr.c_str());
        }else{
            sprintf(buf, "%s.png", chStr.c_str());
        }
        
        Sprite* sptCh = Sprite::createWithSpriteFrameName(buf);
        sptCh->setColor(fontColor);
        sprites.pushBack(sptCh);
        sptCh->getTexture()->setAntiAliasTexParameters();
        if(ch == ' '){
            sptCh->setTag(spaceTag);
        }
        if (isShadowEnabled) {
            if (chStr.size() > 0) {
                sprintf(buf, "%s.png", chStr.c_str());
            }else{
                sprintf(buf, "%c.png", ch);
            }
            sptCh = Sprite::createWithSpriteFrameName(buf);
            sptCh->setColor(Color3B(48, 53, 45));
            shadowSprites.pushBack(sptCh);
        }
    }
    int startX = 0;
    float height = sprites.at(0)->getContentSize().height;
    float rate = fontSize/height;
    float gap = 1;
    int index = 0;
    int lineFeed = height*1.1f*rate;
    float y = 0;
    int lastSpace = -1;
    int lastLineStartIndex = 0;
    for (auto sprite : sprites){
        //        if (index == 0) {
        //            index++;
        //            startX = sprite->getContentSize().width*rate;
        //            first = sprite;
        //            continue;
        //        }
        if (sprite->getTag() == spaceTag) {
            lastSpace = index;
        }
        sprite->setScale(rate);
        sprite->setPosition(Point(startX + sprite->getContentSize().width*rate/2, y));
        startX += sprite->getContentSize().width*rate + gap;
        spriteBatch->addChild(sprite);
        
        index++;
        if (textWidth > 0 && startX >= textWidth) {
            int previousLineStartIndex = lastLineStartIndex;
            int previousLineStartX = startX;
            startX = 0;
            lastLineStartIndex = index;
            y -= lineFeed;
            if (lastSpace > 0) {
                for(int i = lastSpace + 1; i < index; i++){
                    Sprite* spt = sprites.at(i);
                    spt->setPosition(Point(startX + spt->getContentSize().width*rate/2, y));
                    startX += spt->getContentSize().width*rate + gap;
                    previousLineStartX -= spt->getContentSize().width*rate + gap;
                    lastLineStartIndex--;
                }
            }
            if(textWidth > 0){
                if (hAlign == TextHAlignment::CENTER) {
                    for(int i = previousLineStartIndex; i < lastLineStartIndex; i++){
                        Sprite* spt = sprites.at(i);
                        spt->setPositionX(spt->getPositionX() + (textWidth - previousLineStartX)/2);
                    }
                }else if (hAlign == TextHAlignment::RIGHT) {
                    for(int i = previousLineStartIndex; i < lastLineStartIndex; i++){
                        Sprite* spt = sprites.at(i);
                        spt->setPositionX(spt->getPositionX() + (textWidth));
                    }
                }
            }
            
        }
    }
    totalWidth = startX;
    if (textWidth > 0) {
        totalWidth = textWidth;
    }
    totalHeight = -y + lineFeed;
    if (hAlign == TextHAlignment::CENTER) {
        for(auto unit:sprites){
            unit->setPositionX(unit->getPositionX() - totalWidth/2);
        }
//        for(auto unit:shadowSprites){
//            unit->setPositionX(unit->getPositionX() - totalWidth/2);
//        }
        if (textWidth > 0) {
            // handle last line when multiline
            for(int i = lastLineStartIndex; i < index; i++){
                Sprite* spt = sprites.at(i);
                spt->setPositionX(spt->getPositionX() + (textWidth - startX)/2);
            }
        }
        
//        }
    }else if (hAlign == TextHAlignment::RIGHT) {
        for(auto unit:sprites){
            unit->setPositionX(unit->getPositionX() - totalWidth);
        }
        if (textWidth > 0) {
            // handle last line when multiline
            for(int i = lastLineStartIndex; i < index; i++){
                Sprite* spt = sprites.at(i);
                spt->setPositionX(spt->getPositionX() + (startX));
            }
        }
    }
    
    if (shadowSprites.size() > 0) {
        for (int i = 0; i < sprites.size(); i++) {
            spriteBatch->addChild(shadowSprites.at(i), -1);
            shadowSprites.at(i)->setPosition(sprites.at(i)->getPosition() + Point(shadowDistance, -shadowDistance));
            shadowSprites.at(i)->setScale(sprites.at(i)->getScale());
        }
    }
    shadowSprites.clear();
}
Size PPLabel::getSize(){
    if(lblNormal != nullptr){
        return lblNormal->getBoundingBox().size;
    }
    return Size(totalWidth, totalHeight);
}
//void PPLabel::setOpacity(GLubyte opacity){
//    for(auto unit: getChildren()){
////        unit->setOpacity(opacity);
//    }
//}
//Action* PPLabel::runAction(Action* action) {
//    for(auto unit: getChildren()){
////        unit->runAction(action);
//    }
//}

//void PPLabel::setColor(const Color3B& color) {
//    if(getColor() != color){
//        for(auto unit:sprites){
//            unit->setColor(color);
//        }
//        this->Node::setColor(color);
//    }
//}

void PPLabel::setWordColor(std::string str, Color3B color){
    if(!isImageLabel){
        return;
    }
    int start = (int)text.find(str);
//    log("%s, start: %d, length: %d", text.c_str(), start, (int)str.length());
    if(start >= 0){
        for(int i = start; i < start + str.length(); i++){
            sprites.at(i)->setColor(color);
        }
    }
}
