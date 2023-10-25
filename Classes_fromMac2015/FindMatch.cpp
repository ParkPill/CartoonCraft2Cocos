//
//  FindMatch.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 09/04/2019.
//

#include "FindMatch.h"

#include "Title.h"
#include "BattleHud.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "GameSharing.h"
#include "BuggyServerManager.h"
bool FindMatch::init()
{
    if ( !Layer::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    Node* layer = CSLoader::createNode("FindMatch.csb");
    this->addChild(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnCancel");
    btn->addClickEventListener(CC_CALLBACK_0(FindMatch::onCancelClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "cancel");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    lbl->setString(strmake("%s...", LM->getText("find match").c_str()));
    
    lbl = (Text*)layer->getChildByName("lblTip");
    LM->setLocalizedString(lbl, strmake("tip%d", rand()%1));
    
    GM->raidMatchState = NETWORK_HANDLE_STATE_REQUESTED;
    BSM->findMatch(UDGetInt(KEY_TROPHY, 0));
    
    listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(FindMatch::onKeyReleased, this);
    
    this->schedule(schedule_selector(FindMatch::update), 0.1f);
    // init done
    return true;
}

void FindMatch::update(float dt){
    minTime -= dt;
    if(minTime > 0){
        return;
    }
    if(GM->raidMatchState == NETWORK_HANDLE_STATE_ARRIVED){
        GM->raidMatchState = NETWORK_HANDLE_STATE_HANDLED;
        removeListener();
        this->unschedule(schedule_selector(FindMatch::update));
        GM->nextScene = STAGE_RAID;
        auto scene = HelloWorld::scene(GM->nextScene, false); // test
//        Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
        Director::getInstance()->replaceScene(scene);
    }else if(GM->raidMatchState == NETWORK_HANDLE_STATE_ERROR){
        GM->raidMatchState = NETWORK_HANDLE_STATE_HANDLED;
        showErrorMessage();
    }
}
void FindMatch::showErrorMessage(){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, 4);
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn = (Button*)layer->getChildByName("btnNo");
    btn->setVisible(false);
    Button* btnYES = (Button*)layer->getChildByName("btnYes");
    btnYES->addClickEventListener(CC_CALLBACK_0(FindMatch::onCancelClick, this));
    
    Text* lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "failed to find match");
    lbl = (Text*)btnYES->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
}
void FindMatch::removeListener(){
    _eventDispatcher->removeAllEventListeners();
}
void FindMatch::onCancelClick(){
    removeListener();
    this->unschedule(schedule_selector(FindMatch::update));
    GM->nextScene = STAGE_LOBBY;
    auto scene = HelloWorld::scene(STAGE_LOBBY, false);
    Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
    
    if(GM->isAdsUser() && rand()%2 == 0){
        GameSharing::showInterstitial();
    }
}
void FindMatch::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        onCancelClick();
    }
}
