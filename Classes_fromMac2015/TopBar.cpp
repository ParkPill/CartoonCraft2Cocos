//
//  TopBar.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#include "TopBar.h"
#include "ShopLayer.h"
#include "PageBase.h"
bool TopBar::init()
{
    if ( !Layer::init())
    {
        return false;
    }
    
    size = Director::getInstance()->getWinSize();
    
    ndTopBar = CSLoader::createNode("TopBar.csb");
    this->addChild(ndTopBar, 4);
    ndTopBar->setName("topBar");
    
    Button* btn = (Button*)ndTopBar->getChildByName("btnGold");
    btn->addClickEventListener(CC_CALLBACK_0(TopBar::onGoldClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnTree");
    btn->addClickEventListener(CC_CALLBACK_0(TopBar::onTreeClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnGem");
    btn->addClickEventListener(CC_CALLBACK_0(TopBar::onGemClick, this));
    lblGemInHud = (Text*)ndTopBar->getChildByName("lblGem");
    lblGemInHud->setString(Value(GM->getGem()).asString());
    lastGemUpdated = GM->getGem();
    lblGoldInHud = (Text*)ndTopBar->getChildByName("lblGold");
    lblGoldInHud->setString(Value(GM->getCoin()).asString());
    lastGoldUpdated = GM->getCoin() - 1;
    lblTreeInHud = (Text*)ndTopBar->getChildByName("lblTree");
    lblTreeInHud->setString(Value(GM->getTree()).asString());
    lastTreeUpdated = GM->getTree() - 1;
    
    this->schedule(schedule_selector(TopBar::update), 0.1f);
    log("init done");
    return true;
}
Node* TopBar::getShopLayer(){
    ShopLayer* shopLayer = nullptr;
    if (getParent()->getName().compare("shopLayer") == 0) { // if it belongs to shop layer
        shopLayer = (ShopLayer*)getParent()->getParent();
    }else{
        shopLayer = ShopLayer::create();
        PageBase* layer = (PageBase*)getParent()->getParent();
        layer->addPopup(shopLayer);
        if (GM->titleLayer != nullptr) {
            shopLayer->hideTab(0);
        }
    }
    return shopLayer;
}
void TopBar::onGoldClick(){
    if (GM->market == MARKET_SMARTPASS) {
        return;
    }
    ShopLayer* shopLayer = (ShopLayer*)getShopLayer();
    shopLayer->onGoldClickFromTopBar();
}
void TopBar::onTreeClick(){
    if (GM->market == MARKET_SMARTPASS) {
        return;
    }
    ShopLayer* shopLayer = (ShopLayer*)getShopLayer();
    shopLayer->onTreeClickFromTopBar();
}
void TopBar::onGemClick(){
    if (GM->market == MARKET_SMARTPASS) {
        return;
    }
    ShopLayer* shopLayer = (ShopLayer*)getShopLayer();
    shopLayer->onGemClickFromTopBar();
}

void TopBar::update(float dt){
    if(lblGemInHud && GM->getGem() != lastGemUpdated){
        lastGemUpdated = GM->makeNumberCloseTo(lastGemUpdated, GM->getGem());
        lblGemInHud->setString(Value(lastGemUpdated).asString());
    }
    if(lblGoldInHud && GM->getCoin() != lastGoldUpdated){
        lastGoldUpdated = GM->makeNumberCloseTo(lastGoldUpdated, GM->getCoin());
        lblGoldInHud->setString(Value(lastGoldUpdated).asString());
        float rate = GM->getCoin()*1.0f/GM->getMaxGold();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgGoldGuage")->setContentSize(cocos2d::Size(443.56f*rate, 128.90f));
    }
    if(lblTreeInHud && GM->getTree() != lastTreeUpdated){
        lastTreeUpdated = GM->makeNumberCloseTo(lastTreeUpdated, GM->getTree());
        lblTreeInHud->setString(Value(lastTreeUpdated).asString());
        float rate = GM->getTree()*1.0f/GM->getMaxTree();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgTreeGuage")->setContentSize(cocos2d::Size(443.56f*rate, 128.90f));
    }
}
void TopBar::addGem(Ref* ref){
    Node* holder = (Node*)ref;
    GM->addGem(holder->getTag());
}
void TopBar::addGold(Ref* ref){
    Node* holder = (Node*)ref;
    GM->addCoin(holder->getTag());
}
void TopBar::addLumber(Ref* ref){
    Node* holder = (Node*)ref;
    GM->addTree(holder->getTag());
}
