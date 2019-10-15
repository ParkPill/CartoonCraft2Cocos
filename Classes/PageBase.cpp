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
    
    log("init done");
    return true;
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
    this->addChild(lbl, 300);
    lbl->setName("lblInstanceMessage");
    lbl->enableShadow();
    lbl->enableOutline(Color4B(28, 28, 28, 255), 3);
    lbl->setTag(77);
    lbl->setPosition(Point(size.width/2, -100 + offset));
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(EaseInOut::create(MoveBy::create(dur, Point(0, distanceToMove)), 3), DelayTime::create(2), EaseInOut::create(MoveBy::create(dur, Point(0, -distanceToMove)), 3), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, lbl)), nullptr));
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
    while(this->getChildByName("btnIndicator") != nullptr){
        this->removeChildByName("btnIndicator");
    }
    while(this->getChildByName("lblIndicator") != nullptr){
        this->removeChildByName("lblIndicator");
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
