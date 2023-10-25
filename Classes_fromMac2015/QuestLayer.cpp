//
//  QuestLayer.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 24/09/2019.
//

#include "QuestLayer.h"

#include "TopBar.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
#include "ShopLayer.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
bool QuestLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    Node* layer = CSLoader::createNodeWithVisibleSize("QuestLayer.csb");
    this->addChild(layer, 4);
    layer->setName("questLayer");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn;
    btn = (Button*)layer->getChildByName("btnPvp");
//    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroPvpClick, this));
    
    TopBar* topBar = TopBar::create();
    layer->addChild(topBar, POPUP_ZORDER);
    
    btn = (Button*)layer->getChildByName("btnClose");
//    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onCloseHeroPage, this));
    topBar->setPosition(btn->getPosition() + Vec2(-98, 3));
    
    btn = (Button*)layer->getChildByName("btnLimited");
//    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onLimitedPacakgeForHeroClick, this));
    spine::SkeletonAnimation* spChar;
    Text* lbl = (Text*)btn->getChildByName("lbl");
    
    // init done
    return true;
}
