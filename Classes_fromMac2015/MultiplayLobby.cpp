//
//  MultiplayLobby.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 06/12/2019.
//

#include "MultiplayLobby.h"


#include "HeroPage.h"
#include "TopBar.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
#include "ShopLayer.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
bool MultiplayLobby::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    
    Node* layer = CSLoader::createNode("MultiplayLobby.csb");
    this->addChild(layer, 4);
    layer->setName("multiplayLobby");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn;
    btn = (Button*)layer->getChildByName("btnMultiplay");
    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onFindMatchClick, this));
    
    spine::SkeletonAnimation* spChar;
    
    update(0);
    this->schedule(schedule_selector(HeroPage::update));
    
    // init done
    return true;
}
void MultiplayLobby::onFindMatchClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    
}
void MultiplayLobby::update(float dt){
    
}
