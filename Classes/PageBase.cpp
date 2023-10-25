//
//  PageBase.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#include "PageBase.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "LanguageManager.h"
bool PageBase::init()
{
    if ( !Layer::init())
    {
        return false;
    }
    
    size = Director::getInstance()->getWinSize();
    this->schedule(schedule_selector(PageBase::update), 0.1f);
    log("init done");
    return true;
}
void PageBase::update(float dt){
    if(hideIndicatorRequested){
        hideIndicatorRequested = false;
        hideIndicator();
    }
}
Node* PageBase::getPopup(){
    if(popupArray.size() > 0){
        return popupArray.at(popupArray.size() - 1);
    }
    return nullptr;
}

void PageBase::closePopup(){
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        popup->removeFromParent();
    }
    if(popupArray.size() == 0 && WORLD && !WORLD->isVisible()){
        WORLD->setVisible(true);
    }
}

bool PageBase::setAsPopup(Node* node){
    popupArray.pushBack(node);
    node->setLocalZOrder(200 + (int)popupArray.size());
    return true;
}

void PageBase::addPopup(Node* node){
    this->addChild(node, POPUP_ZORDER);
    setAsPopup(node);
}
void PageBase::showInstanceMessage(std::string msg, int offset){
    Label* lbl = LM->getLocalizedLabel(msg.c_str(), Color4B(255, 255, 255, 255), 50);//Label::createWithSystemFont(msg, "Thonburi", 50);
    this->addChild(lbl, 3000);
    lbl->setName("lblInstanceMessage");
    lbl->enableShadow();
    lbl->enableOutline(Color4B(28, 28, 28, 255), 3);
    lbl->setTag(77);
    lbl->setPosition(Vec2(size.width/2, -100 + offset));
    lbl->setAlignment(TextHAlignment::CENTER);
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(EaseInOut::create(MoveBy::create(dur, Vec2(0, distanceToMove)), 3), DelayTime::create(2), EaseInOut::create(MoveBy::create(dur, Vec2(0, -distanceToMove)), 3), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, lbl)), nullptr));
    
    Sprite* spt = Sprite::create("whiteBigCircle.png");
    spt->setColor(Color3B::BLACK);
    this->addChild(spt, 1001);
    spt->setPosition(lbl->getPosition());
    spt->setScale(10, 1);
    spt->setOpacity(150);
    spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(dur, Vec2(0, distanceToMove)), 3), DelayTime::create(2), EaseInOut::create(MoveBy::create(dur, Vec2(0, -distanceToMove)), 3), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, spt)), nullptr));
}
void PageBase::showIndicator(){
    Button* btn = Button::create("uiBoxSmall.png");
    this->addChild(btn, 1000);
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    btn->setPosition(size/2);
    btn->setName("btnIndicator");
    
    Label* lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
    this->addChild(lbl, 1001);
    lbl->setPosition(size/2);
    lbl->setName("lblIndicator");
}
void PageBase::hideIndicator(){
    int counter = 0;
    while(this->getChildByName("btnIndicator") != nullptr){
        this->removeChildByName("btnIndicator");
        counter++;
        if(counter > 10){
            break;
        }
    }
    counter = 0;
    while(this->getChildByName("lblIndicator") != nullptr){
        this->removeChildByName("lblIndicator");
        counter++;
        if(counter > 10){
            break;
        }
    }
}
void PageBase::closeThis(){
    PageBase* parentPage = (PageBase*)getParent();
    if (parentPage && parentPage != nullptr) {
        parentPage->closePopup();
    }else{
        removeFromParent();
    }
}
