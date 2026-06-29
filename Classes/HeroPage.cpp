//
//  HeroPage.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 05/09/2019.
//

#include "HeroPage.h"
#include "TopBar.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
#include "GameManager.h"
#include "ShopLayer.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
bool HeroPage::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    GM->heroPage = this;
    Node* layer = CSLoader::createNode("HeroPage.csb");
    this->addChild(layer, 4);
    layer->setName("heroPage");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn;
    btn = (Button*)layer->getChildByName("btnPvp");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroPvpClick, this));
    btn->setScale(0.2f);
    btn->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1)));
    
    for (int i = 0; i < 12; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnEquip%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onEquipedHeroClick, this));
    }
    TopBar* topBar = TopBar::create();
    layer->addChild(topBar, POPUP_ZORDER);
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onCloseHeroPage, this));
    topBar->setPosition(btn->getPosition() + Vec2(-98, 3));
    
    btn = (Button*)layer->getChildByName("btnLimited");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onLimitedPacakgeForHeroClick, this));
    if (GM->market == MARKET_SMARTPASS) {
        btn->setVisible(false);
    }
    spine::SkeletonAnimation* spChar;
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "limited package");
    for(int i = 0; i < 3; i++){
        Node* img = btn->getChildByName(strmake("img%d", i));
        if(i == 0){
            spChar = GM->getHeroSpine(UNIT_HERO_KNIGHT);
        }else if(i == 1){
            spChar = GM->getHeroSpine(UNIT_HERO_SPEARMAN);
        }else if(i == 2){
            spChar = GM->getHeroSpine(UNIT_HERO_ASSASSIN);
        }
        btn->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        spChar->setPosition(img->getPosition() + Vec2(0, -30));
        img->removeFromParent();
        spChar->setScale(0.5f);
    }
    
    btn = (Button*)layer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onCloseHeroPage, this));
    btn = (Button*)layer->getChildByName("btnShop");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroShopClick, this));
    if (GM->market == MARKET_SMARTPASS) {
        btn->setVisible(false);
    }
    
    btn = (Button*)layer->getChildByName("btnInfo");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroDrawRateInfoClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "information");
    
    btn = (Button*)layer->getChildByName("btnGacha0");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onChestClick, this));
    lbl = (Text*)btn->getChildByName("lblDescription");
    LM->setLocalizedStringNotKey(lbl, strmake("%s, %s, %s", LM->getText("common hero").c_str(), LM->getText("good hero").c_str(), LM->getText("great hero").c_str()));
    btn = (Button*)layer->getChildByName("btnGacha1");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onChestClick, this));
    lbl = (Text*)btn->getChildByName("lblDescription");
    LM->setLocalizedStringNotKey(lbl, strmake("%s, %s, %s", LM->getText("great hero").c_str(), LM->getText("rare hero").c_str(), LM->getText("epic hero").c_str()));
    
    btn = (Button*)layer->getChildByName("btnArrangeRarity");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onSortByRarity, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "sort by rarity");
    
    btn = (Button*)layer->getChildByName("btnArrangeSlot");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onSortBySlot, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "sort by slot");
    
    btn = (Button*)layer->getChildByName("btnFuse");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onFusionClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "fusion");
    
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "deck");
    lbl = (Text*)layer->getChildByName("lblInventory");
    LM->setLocalizedString(lbl, "inventory");
    
    if(!UDGetBool(KEY_HERO_ALERT_NEVER_SHOW, false)){
        layer = CSLoader::createNodeWithVisibleSize("HeroAlert.csb");
        this->addChild(layer, 4);
        layer->setName("neverAlertHero");
        setAsPopup(layer);
        layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
        layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
        
        lbl = (Text*)layer->getChildByName("lblDescription");
        LM->setLocalizedString(lbl, "hero alert");
        ImageView* img = (ImageView*)layer->getChildByName("imgBackground");
        btn = (Button*)img->getChildByName("btnNever");
        btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onNeverShowHeroAlertClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "never show");
        
        btn = (Button*)img->getChildByName("btnOk");
        btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "ok");
    }
    updateHeroes();
    if (unitInfoListHeroInventory.size() == 0 && unitInfoListHeroDeck.size() == 0 && !UDGetBool(KEY_HERO_PAGE_TUTORIAL_DONE, false) && heroPageTutorialIndex == 0) {
        heroPageTutorialIndex = 0;
        showHeroPageTutorial(LM->getText("hero tutorial 0"));

        time_t now = BSM->getCurrentTimeT();
        std::string value = "";
        value += BSM->getStrFromTime(now - 1);
        UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value); // test

    }
    update(0);
    this->schedule(schedule_selector(HeroPage::update));
    

    
//    if (iapList.find(IAP_DETAIL_CREW_MANAGER_PACKAGE) == std::string::npos) {
//        value += BSM->getStrFromTime(now + 60*60*23 + 60*55);
//    }else{
//        value += BSM->getStrFromTime(now + 60*60*1);
//    }
//    time_t now = BSM->getCurrentTimeT();
//    std::string value = "";
//    value += BSM->getStrFromTime(now - 1);
//    UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value); // test
    
//    pvp6Reward = "13_100_1234"; // test
//    GM->setWoodKey(0);// test
//    GM->setGoldKey(0);// test
    // init done
    return true;
}
void HeroPage::update(float dt){
    time_t now = BSM->getCurrentTimeT();
    std::string strNextWoodChestGachaFreeTime = UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
    
    std::string iapList = UDGetStr(KEY_IAP_LIST, "");
    if (strNextWoodChestGachaFreeTime.size() <= 0) {
        std::string value = "";
        if (iapList.find(IAP_DETAIL_CREW_MANAGER_PACKAGE) == std::string::npos) {
            value += BSM->getStrFromTime(now + 60*60*23 + 60*55);
        }else{
            value += BSM->getStrFromTime(now + 60*60*1);
        }
        
        UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value);
        strNextWoodChestGachaFreeTime = UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
    }
    time_t nextWoodChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextWoodChestGachaFreeTime);
    bool isWoodChestGachaFreeReady = difftime(nextWoodChestGachaFreeTimeT, now) <= 0;
    
    std::string strNextGoldChestGachaFreeTime = UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
    if(strNextGoldChestGachaFreeTime.size() <= 0){
        std::string value = "";
        if (iapList.find(IAP_DETAIL_MASTER_CREW_PACKAGE) == std::string::npos){
            value += BSM->getStrFromTime(now + 60*60*24*7 - 60*30);
        }else{
            value += BSM->getStrFromTime(now + 60*60*12);
        }
        log("value: %s", value.c_str());
        UDSetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, value);
        strNextGoldChestGachaFreeTime = UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
    }
    time_t nextGoldChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextGoldChestGachaFreeTime);
    bool isGoldChestGachaFreeReady = difftime(nextGoldChestGachaFreeTimeT, now) <= 0;
    
    Node* layer = this->getChildByName("heroPage");
    
    if(layer && layer != nullptr){
        Button* btn = (Button*)layer->getChildByName("btnGacha0");
        btn->getChildByName("imgRedDot")->setVisible(isWoodChestGachaFreeReady);
        
        Text* lbl = (Text*)btn->getChildByName("lbl");
        lbl->setTag(60);
        ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
        if(GM->getWoodKey() > 0){
            lbl->setString(Value(GM->getWoodKey()).asString());
            img->loadTexture("keyWood.png");
            img->setContentSize(cocos2d::Size(31, 60));
        }else{
            if(isWoodChestGachaFreeReady){
                LM->setLocalizedString(lbl, "free");
                lbl->setTag(0);
            }else{
                lbl->setString("60");
            }
            
            img->loadTexture("gem.png");
            img->setContentSize(cocos2d::Size(97, 78));
        }
        GM->alignToCenter(img, lbl, 30, 422, 0);
        
        Text* lblTimeLeft = (Text*)btn->getChildByName("lblTimeLeft");
        lblTimeLeft->setVisible(!isWoodChestGachaFreeReady);
        LM->setLocalizedStringNotKey(lblTimeLeft, strmake("%s: %s", LM->getText("next free pick").c_str(), GM->getTimeLeftInStringHMS(difftime(nextWoodChestGachaFreeTimeT, now)).c_str()));
        
        btn = (Button*)layer->getChildByName("btnGacha1");
        btn->getChildByName("imgRedDot")->setVisible(isGoldChestGachaFreeReady);
        lbl = (Text*)btn->getChildByName("lbl");
        img = (ImageView*)btn->getChildByName("imgIcon");
        lbl->setTag(300);
        if(GM->getGoldKey() > 0){
            lbl->setString(Value(GM->getGoldKey()).asString());
            img->loadTexture("keyGold.png");
            img->setContentSize(cocos2d::Size(31, 60));
        }else{
            if(isGoldChestGachaFreeReady){
                LM->setLocalizedString(lbl, "free");
                lbl->setTag(0);
            }else{
                lbl->setString("300");
            }
            
            img->loadTexture("gem.png");
            img->setContentSize(cocos2d::Size(97, 78));
        }
        GM->alignToCenter(img, lbl, 30, 422, 0);
        lblTimeLeft = (Text*)btn->getChildByName("lblTimeLeft");
        lblTimeLeft->setVisible(!isGoldChestGachaFreeReady);
        LM->setLocalizedStringNotKey(lblTimeLeft, strmake("%s: %s", LM->getText("next free pick").c_str(), GM->getTimeLeftInStringHMS(difftime(nextGoldChestGachaFreeTimeT, now)).c_str()));
    }
    if (BSM->isServerFailed) {
        BSM->isServerFailed = false;
        showInstanceMessage(LM->getText("server fail try again"));
        hideIndicator();
    }
    if (pvpNetworkState == NETWORK_HANDLE_STATE_ARRIVED) {
        pvpNetworkState = NETWORK_HANDLE_STATE_COMPLETE;
        hideIndicator();
        if(isPvpInfoRequested){
            isPvpInfoRequested = false;
            showPvpInfo(pvpNumber);
        }else if(isPvpFindMatchRequested){
            long now = (long)BSM->getCurrentTimeT();
            if (pvpNumber == 6) {
                if (BSM->pvp6TicketCount == BSM->pvpTicketMax) {
                    BSM->pvp6TicketRefillStartTime = std::to_string(now);
                }
                BSM->pvp6TicketCount--;
                BSM->saveUserData(strmake("pvp6Ticket=%d&pvp6_refill_start_time=%s", BSM->pvp6TicketCount, BSM->pvp6TicketRefillStartTime.c_str()));
            }else if (pvpNumber == 12) {
                if (BSM->pvp12TicketCount == BSM->pvpTicketMax) {
                    BSM->pvp12TicketRefillStartTime = std::to_string(now);
                }
                BSM->pvp12TicketCount--;
                BSM->saveUserData(strmake("pvp12Ticket=%d&pvp12_refill_start_time=%s", BSM->pvp12TicketCount, BSM->pvp12TicketRefillStartTime.c_str()));
            }
            isPvpFindMatchRequested = false;
            this->unschedule(schedule_selector(HeroPage::update));
            GM->nextScene = STAGE_FIELD; // test
            if(WORLD){
                WORLD->removeUsedAssets();
            }
//            if(BHUD){
//                BHUD = nullptr;
//                removeUsedAssets();
//            }
//            if(TITLE){
//                TITLE->clearAssets();
//                GameManager::getInstance()->titleLayer = nullptr;
//               cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(TITLE->listener);
//            }
            clearVectors();
            auto scene = GameScene::scene(1, pvpNumber); // test
            Director::getInstance()->replaceScene(scene);
            WORLD->setPvpMode(pvpNumber);
            GM->resetAsset();
        }
    }
    
    layer = this->getChildByName("heroPvpDetail");
    if(layer && layer != nullptr){
        int timeLeftToMidnight = 86400 - (long)now%86400;
        Text* lbl = (Text*)layer->getChildByName("lblTimeLeft");
        LM->setLocalizedStringNotKey(lbl, strmake("%s: %s", LM->getText("time left").c_str(), GM->getTimeLeftInString(BSM->getTimeLeftToSunday()).c_str()));
        
        
        if(BSM->getTimeLeftToSunday() < 60*5){
            Button* btnPlay = (Button*)layer->getChildByName("btnPlay");
            btnPlay->setEnabled(false);
        }
        
        lbl = (Text*)layer->getChildByName("lblTicketTimeLeft");
        lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeftToMidnight).c_str()));
        
        lbl = (Text*)layer->getChildByName("lblTicket");
        lbl->setString(strmake("%d/%d", layer->getTag()==6?BSM->pvp6TicketCount:BSM->pvp12TicketCount, BSM->pvpTicketMax).c_str());
        
        Button* btn = (Button*)layer->getChildByName("btnVideo");
        std::string strWatchedTime = UDGetStr(layer->getTag()==6?KEY_LAST_VIDEO_FOR_PVP6_TICKET_WATCHED_TIME:KEY_LAST_VIDEO_FOR_PVP12_TICKET_WATCHED_TIME, "2019-10-30T15:46:00");
        
        time_t videoWatchedTime = BSM->getTimeTFromStr(strWatchedTime);
        bool isVideoReady = videoWatchedTime + 60*60 <= now;
        btn->setEnabled(isVideoReady);
        if (GM->market == MARKET_SMARTPASS) {
            btn->setVisible(false);
        }
        std::string value = "";
        value += BSM->getStrFromTime(now);
        lbl = (Text*)btn->getChildByName("lblTimeLeft");
        lbl->setVisible(!isVideoReady);
        if (!isVideoReady) {
            lbl->setString(GM->getTimeLeftInString(videoWatchedTime + 60*60 - now));
        }
        lbl = (Text*)btn->getChildByName("lbl");
        lbl->setVisible(isVideoReady);
        ImageView* img = (ImageView*)btn->getChildByName("imgTicket");
        img->setVisible(isVideoReady);
        
        if(isVideoDoneForPvpTicket){
            isVideoDoneForPvpTicket = false;
            
            if(layer->getTag() == 6){
                UDSetStr(KEY_LAST_VIDEO_FOR_PVP6_TICKET_WATCHED_TIME, value);
                BSM->pvp6TicketCount++;
                BSM->saveUserData(strmake("pvp6Ticket=%d", BSM->pvp6TicketCount));
            }else if(layer->getTag() == 12){
                UDSetStr(KEY_LAST_VIDEO_FOR_PVP12_TICKET_WATCHED_TIME, value);
                BSM->pvp12TicketCount++;
                BSM->saveUserData(strmake("pvp12Ticket=%d", BSM->pvp12TicketCount));
            }
        }
    }
    
    if (pvpNetworkState == NETWORK_HANDLE_STATE_ARRIVED) {
        if (isPvpFindMatchRequested) {
            isPvpFindMatchRequested = false;
            log("MAATCH!!!");
        }
    }
    
    if (pvp6Reward.length() > 1) {
        showPvpReward(pvp6Reward);
        pvp6Reward = "";
        UDSetInt(KEY_PVP6_TROPHY, 1000);
    }
    if (pvp12Reward.length() > 1) {
        showPvpReward(pvp12Reward);
        pvp12Reward = "";
        UDSetInt(KEY_PVP12_TROPHY, 1000);
    }
    layer = this->getChildByName("heroPvpDetail");
    if (layer && layer != nullptr) {
        // pvp 6
        long startTime = Value(BSM->pvp6TicketRefillStartTime).asDouble();
        int timePassed = now - startTime;
        int refillTime = 3600;
//        int refillTime = 60; // test
        
        if(BSM->pvpTicketMax > BSM->pvp6TicketCount){
            if (timePassed >= refillTime) {
                startTime += refillTime;
                timePassed -= refillTime;
                BSM->pvp6TicketCount++;
                if(BSM->pvp6TicketCount > BSM->pvpTicketMax){
                    BSM->pvp6TicketCount = BSM->pvpTicketMax;
                }
                BSM->pvp6TicketRefillStartTime = std::to_string(startTime);
            }
        }
        Text* lbl = (Text*)layer->getChildByName("lblTicketTimeLeft");
        if(layer->getTag() == 6){
            lbl->setString(GM->getTimeLeftInString(refillTime - timePassed));
            lbl->setVisible(BSM->pvp6TicketCount < BSM->pvpTicketMax);
        }
        
        // pvp 12
        startTime = Value(BSM->pvp12TicketRefillStartTime).asDouble();
        timePassed = now - startTime;
        if(BSM->pvpTicketMax > BSM->pvp12TicketCount){
            if (timePassed >= refillTime) {
                startTime += refillTime;
                timePassed -= refillTime;
                BSM->pvp12TicketCount++;
                if(BSM->pvp12TicketCount > BSM->pvpTicketMax){
                    BSM->pvp12TicketCount = BSM->pvpTicketMax;
                }
                BSM->pvp12TicketRefillStartTime = std::to_string(startTime);
            }
        }
        if(layer->getTag() == 12){
            lbl->setString(GM->getTimeLeftInString(refillTime - timePassed));
            lbl->setVisible(BSM->pvp12TicketCount < BSM->pvpTicketMax);
        }
    }
    
    if (isTimeReceivedForGacha) {
        isTimeReceivedForGacha = false;
        handleTimeReceivedForGacha();
    }
}
void HeroPage::handleTimeReceivedForGacha(){
    std::string value = "";
    std::string iapList = UDGetStr(KEY_IAP_LIST, "");
    time_t now = BSM->getCurrentTimeT();
    bool shouldInventorySave = false;
    if (requestedChestType == CHEST_WOOD) {
        std::string strNextWoodChestGachaFreeTime = UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
        time_t nextWoodChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextWoodChestGachaFreeTime);
        bool isWoodChestGachaFreeReady = difftime(nextWoodChestGachaFreeTimeT, now) <= 0;
        if(isWoodChestGachaFreeReady){
            if (iapList.find(IAP_DETAIL_CREW_MANAGER_PACKAGE) == std::string::npos) {
                value += BSM->getStrFromTime(now + 60*60*23 + 60*55);
            }else{
                value += BSM->getStrFromTime(now + 60*60*1);
            }
            
            UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value);
            log("value: %s", value.c_str());
            pickHero(0, 2);
            shouldInventorySave = true;
        }
    }else if(requestedChestType == CHEST_GOLD){
        std::string strNextGoldChestGachaFreeTime = UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
        time_t nextGoldChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextGoldChestGachaFreeTime);
        bool isGoldChestGachaFreeReady = difftime(nextGoldChestGachaFreeTimeT, now) <= 0;
        if (isGoldChestGachaFreeReady) {
            if (iapList.find(IAP_DETAIL_MASTER_CREW_PACKAGE) == std::string::npos){
                value += BSM->getStrFromTime(now + 60*60*24*7 - 60*30);
            }else{
                value += BSM->getStrFromTime(now + 60*60*12);
            }
            log("value: %s", value.c_str());
            UDSetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, value);
            
            pickHero(2, 4);
            shouldInventorySave = true;
        }
    }
    
    if(shouldInventorySave){
        std::vector<int> datas;
        updateHeroInventorySaveData();
        datas.push_back(DATA_TYPE_HERO_INVENTORY);
        BSM->saveUserData(datas);
        updateHeroes();
    }
}
void HeroPage::showPvpReward(std::string strReward){
    ValueVector list = GM->split(strReward, "_");
    Node* layer = CSLoader::createNodeWithVisibleSize("PvpReward.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setTag(-1);
    Node* ndPanel = layer->getChildByName("imgPanel");
    ndPanel->setScale(0.1f);
    ndPanel->runAction(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.05, 0.95),ScaleTo::create(0.05, 1), NULL));
    GM->addGlowBack(ndPanel);
    Button* btn = (Button*)ndPanel->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    Text* lbl = (Text*)ndPanel->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "result");
    lbl = (Text*)ndPanel->getChildByName("lblReward");
    LM->setLocalizedString(lbl, "reward");
    lbl = (Text*)ndPanel->getChildByName("lblTrophy");
    lbl->setString(Value(list.at(2)).asString());
    lbl = (Text*)ndPanel->getChildByName("lblRank");
    lbl->setString(strmake("No.%d / %d", Value(list.at(0)).asInt(), Value(list.at(1)).asInt()));
    int reward = 0;
    int goldKeyCount = 0;
    int silverKeyCount = 0;
    float percent = list.at(0).asFloat()*100/list.at(1).asFloat();
    if (percent < 1) {
        reward = 300;
        goldKeyCount = 3;
    }else if (percent < 5) {
        reward = 200;
        goldKeyCount = 2;
    }else if (percent < 20) {
        reward = 100;
        goldKeyCount = 1;
    }else if (percent < 50) {
        reward = 50;
        silverKeyCount = 3;
    }else{
        reward = 30;
        silverKeyCount = 2;
    }
    lbl = (Text*)ndPanel->getChildByName("lblGem");
    lbl->setString(strmake("%d", reward));
    lbl = (Text*)ndPanel->getChildByName("lblKey");
    if (goldKeyCount > 0) {
        ImageView* img = (ImageView*)ndPanel->getChildByName("imgKey");
        img->loadTexture("keyGold.png");
        lbl->setString(strmake("%d", goldKeyCount));
        GM->addGoldKey(goldKeyCount);
    }else if(silverKeyCount > 0){
        lbl->setString(strmake("%d", silverKeyCount));
        GM->addWoodKey(silverKeyCount);
    }
    GM->addGem(reward);
    BSM->saveUserData(strmake("gem=%d", GM->getGem()));
}
void HeroPage::onHeroPvpClick(){
    int heroInDeckCount = unitInfoListHeroDeck.size();
    if(heroInDeckCount < 12){
        showInstanceMessage(LM->getText("needs 12 heroes"));
        return;
    }
    checkChangesForHeroes();
    Node* layer = CSLoader::createNode("HeroPvpPopup.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("heroPvpPopup");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Text* lbl = (Text*)layer->getChildByName("imgTitle")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "heroes arena");
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    
    Node* ndMode = layer->getChildByName("ndMode");
    btn = (Button*)ndMode->getChildByName("btn0");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onPvpModeClick, this));
    UnitInfo* info;ImageView* img;Sprite* spt;
    for(int i = 0; i < unitInfoListHeroDeck.size(); i++){
        info = unitInfoListHeroDeck.at(i);
        img = (ImageView*)btn->getChildByName(strmake("img_%d", i));
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        btn->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition());
        
        spt = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
        btn->addChild(spt);
        spt->setPosition(img->getPosition() + Vec2(0, 80));
        Animation* animation = AnimationCache::getInstance()->getAnimation("cartoonyExplosion");
        Animate* animate = Animate::create(animation);
        spt->setScale(0.5f);
        if(i < 6){
            spt->runAction(RepeatForever::create(Sequence::create(animate, FadeOut::create(0), DelayTime::create(2 - 0.06f*9), FadeIn::create(0), NULL)));
            spChar->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0), DelayTime::create(1), FadeIn::create(0), DelayTime::create(1), NULL)));
        }else{
            spt->setOpacity(0);
            spt->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1), FadeIn::create(0),animate, FadeOut::create(0), DelayTime::create(1 - 0.06f*9), NULL)));
            spChar->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1), FadeOut::create(0), DelayTime::create(1), FadeIn::create(0), NULL)));
        }
        img->removeFromParent();
    }
    btn = (Button*)ndMode->getChildByName("btn1");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onPvpModeClick, this));
    for(int i = 0; i < unitInfoListHeroDeck.size(); i++){
        
        info = unitInfoListHeroDeck.at(i);
        img = (ImageView*)btn->getChildByName(strmake("img_%d", i));
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        btn->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        //        spChar->setScale(0.75f);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition() + Vec2(0, -45));
        spChar->setPosition(img->getPosition());
        img->removeFromParent();
    }
}
void HeroPage::onPlayPvpClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("heroPvpDetail");
    if(layer->getTag() == 6){
        if (BSM->pvp6TicketCount > 0) {
            showIndicator();
            BSM->findMatchForPvp6();
        }else{
            showInstanceMessage(LM->getText("not enough ticket"));
        }
    }else{
        if (BSM->pvp12TicketCount > 0) {
            showIndicator();
            BSM->findMatchForPvp12();
        }else{
            showInstanceMessage(LM->getText("not enough ticket"));
        }
    }
    isPvpFindMatchRequested = true;
}
void HeroPage::onPvpModeClick(Ref* ref){
    GM->pvpOpenDate = BSM->getDay();
    Button* btn = (Button*)ref;
    if(btn->getTag() == 0){
        pvpNumber = 6;
        if(BSM->shouldGetPvp6Info){
            showIndicator();
            pvpNetworkState = NETWORK_HANDLE_STATE_REQUESTED;
            isPvpInfoRequested = true;
            BSM->getPvp6Rank();
        }else{
            showPvpInfo(6);
        }
    }else{
        pvpNumber = 12;
        if(BSM->shouldGetPvp12Info){
            showIndicator();
            pvpNetworkState = NETWORK_HANDLE_STATE_REQUESTED;
            isPvpInfoRequested = true;
            BSM->getPvp12Rank();
        }else{
            showPvpInfo(12);
        }
    }
}
void HeroPage::showPvpInfo(int number){
    Node* layer = CSLoader::createNode("HeroPvpDetail.csb");
    
    layer->setTag(number);
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("heroPvpDetail");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn = (Button*)layer->getChildByName("btnPlay");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onPlayPvpClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "find match");
    btn = (Button*)layer->getChildByName("btnVideo");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onVideoForTicketClick, this));
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    ImageView* img = (ImageView*)layer->getChildByName("Image_14");
    img->setTouchEnabled(true);
    btn = (Button*)layer->getChildByName("imgBack");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    ImageView* imgTicket = (ImageView*)layer->getChildByName("imgTicket");
    imgTicket->loadTexture(strmake("ticketPvp%d.png", number));
    
    Text* lblTitle = (Text*)layer->getChildByName("btnTitle")->getChildByName("lbl");
    lblTitle->setString(strmake("%d vs %d", number, number));
    lbl = (Text*)layer->getChildByName("lblReward");
    LM->setLocalizedString(lbl, "reward");
    lbl = (Text*)layer->getChildByName("lblRank");
    LM->setLocalizedString(lbl, "top rank");
    UnitInfo* info;
    
    for (int i = 0; i < 12; i++) {
        img = (ImageView*)layer->getChildByName(strmake("img_%d", i));
        info = unitInfoListHeroDeck.at(i);
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        layer->addChild(spChar);
        spChar->setAnimation(0, "idle", true);
        spChar->setScaleX(-1);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition() + Vec2(0, -45));
        img->setColor(getRankColor(info->rank));
        img->removeFromParent();
    }
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    Button* btnTemp = (Button*)layer->getChildByName("btnTemp");
    sv->removeAllChildren();
    int gapY = 90;
    int scrollHeight = 0;
    if (number == 6) {
        scrollHeight = gapY*pvp6RankInfoName.size();
        if(scrollHeight < sv->getContentSize().height){
            scrollHeight = sv->getContentSize().height;
        }
        for (int i = 0; i < pvp6RankInfoName.size(); i++) {
            if (pvp6RankInfoName.at(i).length() < 1) {
                continue;
            }
            Button* btn = (Button*)btnTemp->clone();
            sv->addChild(btn);
            btn->setPosition(Vec2(3, scrollHeight - i*gapY - 50));
            lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString(strmake("%d. %s(%d)", i + 1, pvp6RankInfoName.at(i).c_str(), pvp6RankInfoTrohpy.at(i)));
            btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onRankerClick, this));
            btn->setTag(i);
        }
        lbl = (Text*)layer->getChildByName("btnMe")->getChildByName("lbl");
        lbl->setString(strmake("%d. %s(%d)", pvp6MyRank + 1, UDGetStr(KEY_NAME, "").c_str(), UDGetInt(KEY_PVP6_TROPHY, 1000)));
    }else if(number == 12){
        scrollHeight = gapY*pvp12RankInfoName.size();
        if(scrollHeight < sv->getContentSize().height){
            scrollHeight = sv->getContentSize().height;
        }
        for (int i = 0; i < pvp12RankInfoName.size(); i++) {
            if (pvp12RankInfoName.at(i).length() < 2) {
                continue;
            }
            Button* btn = (Button*)btnTemp->clone();
            sv->addChild(btn);
            btn->setPosition(Vec2(3, scrollHeight - i*gapY - 50));
            lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString(strmake("%d. %s(%d)", i + 1, pvp12RankInfoName.at(i).c_str(), pvp12RankInfoTrohpy.at(i)));
            btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onRankerClick, this));
            btn->setTag(i);
        }
        lbl = (Text*)layer->getChildByName("btnMe")->getChildByName("lbl");
        lbl->setString(strmake("%d. %s(%d)", pvp12MyRank + 1, UDGetStr(KEY_NAME, "").c_str(), UDGetInt(KEY_PVP12_TROPHY, 1000)));
    }
    sv->setInnerContainerSize(cocos2d::Size(sv->getContentSize().width, scrollHeight));
    btnTemp->setVisible(false);
    
    update(0);
}
void HeroPage::onVideoForTicketClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("heroPvpDetail");
    GM->showVideo(layer->getTag() == 6?VIDEO_PVP6_TICKET:VIDEO_PVP12_TICKET);
}
void HeroPage::onPvpTicketVideoDone(){
    isVideoDoneForPvpTicket = true;
}
void HeroPage::onRankerClick(Ref* ref){
    BTN_FROM_REF
    int rank = btn->getTag();
    Node* layer = CSLoader::createNode("PvpRankerInfo.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    Text* lbl = (Text*)layer->getChildByName("lblName");
    std::string name;std::string strData;
    int pvpNumber = this->getChildByName("heroPvpDetail")->getTag();
    if (pvpNumber == 6) {
        name = pvp6RankInfoName.at(rank);
        strData = pvp6RankInfoData.at(rank);
    }else if (pvpNumber == 12) {
        name = pvp12RankInfoName.at(rank);
        strData = pvp12RankInfoData.at(rank);
    }
    lbl->setString(strmake("%d. %s", rank + 1, name.c_str()));
    log("strData: %s", strData.c_str());
    ValueVector list = GM->split(strData, "_");
    ImageView* img;UnitInfo* info;
    int index = 0;
    for (int i = 0; i < list.size(); i++) {
        if (list.at(i).asString().length() < 2) {
            continue;
        }
        img = (ImageView*)layer->getChildByName(strmake("img_%d", index));
        
        info = GM->getUnitInfoFromString(list.at(i).asString());
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        layer->addChild(spChar);
        spChar->setName(strmake("hero%d", index));
        spChar->setAnimation(0, "idle", true);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition() + Vec2(0, -45));
        img->setColor(getRankColor(info->rank));
        lbl = (Text*)layer->getChildByName(strmake("lblLevel_%d", index));
        lbl->setString(strmake("LV.%d", info->level + 1));
        index++;
    }
}
void HeroPage::onNeverShowHeroAlertClick(){
    UDSetBool(KEY_HERO_ALERT_NEVER_SHOW, true);
    closePopup();
}
void HeroPage::onLimitedPacakgeForHeroClick(){
    onHeroShopClick();
}
void HeroPage::onHeroDrawRateInfoClick(){
    Node* layer = CSLoader::createNode("HeroDrawRate.csb");
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("heroDrawRate");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    float x = 0, y = 0;
    int columnCount = 4;
    int gapUnitX = sv->getContentSize().width/(columnCount*2);
    x = gapUnitX;
    float gapUnitY = gapUnitX*1.0f;
    y = gapUnitY*1.5f;
    
    int totalRate = 0;
    std::vector<int> list = getPickPossibleHeroes();
    for (int i = 0; i < list.size(); i++) {
        totalRate += getPickRate(list.at(i));
    }
    
    int heroCount = (int)list.size();
    float svHeight = (heroCount/columnCount + 1)*gapUnitY*2;
    if(svHeight > sv->getContentSize().height){
        sv->setInnerContainerSize(cocos2d::Size(sv->getContentSize().width, svHeight));
    }
    svHeight = sv->getInnerContainerSize().height;
    
    for (int j = 0; j < list.size(); j++) {
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(list.at(j));
        sv->addChild(spChar);
        spChar->setPosition(Vec2(x, svHeight - y));
        x += gapUnitX*2;
        if(x > sv->getContentSize().width){
            x = gapUnitX;
            y += gapUnitY*2;
        }
        Label* lbl = LM->getLocalizedLabel(LM->getText(GM->getUnitName(list.at(j))).c_str(), Color4B::WHITE, 40);
        lbl->enableOutline(Color4B::BLACK, 4);
        sv->addChild(lbl);
        lbl->setPosition(spChar->getPosition() + Vec2(0, -30));
        float percent = getPickRate(list.at(j))*100.0f/totalRate;
        Label* lblPercent = LM->getLocalizedLabel(strmake("(%.2f%%)", percent).c_str(), Color4B::GRAY, 30);
        lblPercent->enableOutline(Color4B::BLACK, 3);
        sv->addChild(lblPercent);
        lblPercent->setPosition(spChar->getPosition() + Vec2(0, -75));
        spChar->setAnimation(0, "idle", true);
        bool contains = false;
        for(auto unitInfo : unitInfoListHeroDeck){
            if(unitInfo->unitType == list.at(j)){
                contains = true;
            }
        }
        for(auto unitInfo : unitInfoListHeroInventory){
            if(unitInfo->unitType == list.at(j)){
                contains = true;
            }
        }
        
        if(!UDGetBool(strmake(KEY_COLLECTION_UNLOCKED_FORMAT, list.at(j)).c_str(), false) && !contains){
            spChar->setColor(Color3B::BLACK);
            lbl->setString("???");
        }
        
        if(list.at(j) >= UNIT_HERO_ENT){
            spChar->setColor(Color3B::BLACK);
            lbl->setString("Coming soon");
            lblPercent->setVisible(false);
        }
    }
    sv->getInnerContainer()->setPositionY(-svHeight+sv->getContentSize().height);
    
    Text* lbl = (Text*)layer->getChildByName("imgTitle")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "draw rate");
    list.clear();
}
void HeroPage::showHeroPageTutorial(std::string text){
    heroPageTutorialNode = Node::create();
    this->addChild(heroPageTutorialNode, 200);
    setAsPopup(heroPageTutorialNode);
    ImageView* img = ImageView::create("uiBox.png");
    heroPageTutorialNode->addChild(img, -1);
    img->setTouchEnabled(true);
    img->setScale(100, 100);
    img->setColor(Color3B::BLACK);
    img->setOpacity(50);
    img->setPosition(Vec2(size.width/2, size.height/2));
    img->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroPageTutorialBoxClick, this));
    
    img = ImageView::create("uiBox.png");
    heroPageTutorialNode->addChild(img);
    img->setScale9Enabled(true);
    img->setTouchEnabled(true);
    img->setContentSize(cocos2d::Size(2000, 500));
    img->setPosition(Vec2(size.width/2, 330));
    img->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroPageTutorialBoxClick, this));
    
    Label* lbl = LM->getLocalizedLabel(text.c_str(), Color4B::BLACK, 60);
    heroPageTutorialNode->addChild(lbl);
    lbl->setPosition(img->getPosition() + Vec2(200, 0));
    lbl->setDimensions(1500, 450);
    lbl->setName("lbl");
    lbl->setVerticalAlignment(TextVAlignment::CENTER);
    
    std::string strFile = "workerBigStand";
    spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
    spChar->setAnimation(0, "idle", true);
    spChar->setScale(-1.5f, 1.5f);
    Sprite* sptShadow = Sprite::create("shadow.png");
    spChar->addChild(sptShadow, -1);
    sptShadow->setOpacity(0);
    
    heroPageTutorialNode->addChild(spChar);
    spChar->setPosition(Vec2(size.width/2 - 750, 210));
}
void HeroPage::addGreenArrowToButton(Node* btn){
    Sprite* spt = Sprite::create("greenArrow.png");
    btn->addChild(spt);
    spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height));
    float dur = 0.5f;
    //    spt->setOpacity(0);
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Vec2(0, 40)), MoveBy::create(dur, Vec2(0, -40)), NULL)));
    //    spt->runAction(RepeatForever::create(Sequence::create(FadeIn::create(dur/3), DelayTime::create(dur/3), FadeOut::create(dur/3), NULL)));
    spt->setRotation(90);
//    spt->setGlobalZOrder(1);
    spt->setName("arrow");
}
void HeroPage::onHeroPageTutorialBoxClick(){
    heroPageTutorialIndex++;
    closePopup();
    if(heroPageTutorialIndex == 1){
        addGreenArrowToButton(this->getChildByName("heroPage")->getChildByName("btnGacha0"));
    }else if(heroPageTutorialIndex == 3){
        addGreenArrowToButton(this->getChildByName("heroPage")->getChildByName("svInventory")->getChildByTag(0));
    }else if (heroPageTutorialIndex == 5) {
        showHeroPageTutorial(LM->getText("hero tutorial 3"));
        UDSetBool(KEY_HERO_PAGE_TUTORIAL_DONE, true);
    }else if (heroPageTutorialIndex == 6) {
        int gemCount = 100;
        Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
        this->addChild(layer, 4);
        layer->setName("gemRewardMessageBox");
        setAsPopup(layer);
        layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
        layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
        ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
        GM->rollOpenScroll(imgBack);
        
        Button* btn = (Button*)layer->getChildByName("btnOk");
        btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
        Text* lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "ok");
        
        lbl = (Text*)layer->getChildByName("lblGemCount");
        lbl->setString(Value(gemCount).asString());
        GM->alignToCenter(layer->getChildByName("imgGem"), lbl, 50, size.width/2, 0);
        
        lbl = (Text*)layer->getChildByName("lblGold");
        Node* img = layer->getChildByName("imgGold");
        img->setVisible(false);
        lbl->setVisible(false);
        
        lbl = (Text*)layer->getChildByName("lblTree");
        img = layer->getChildByName("imgTree");
        img->setVisible(false);
        lbl->setVisible(false);
        
        lbl = (Text*)layer->getChildByName("lblGemCount");
        lbl->setString(Value(gemCount).asString());
        GM->alignToCenter(layer->getChildByName("imgGem"), lbl, 50, layer->getContentSize().width/2, 0);
        
        lbl = (Text*)layer->getChildByName("lblDescription");
        LM->setLocalizedString(lbl, "battle tutorial reward");
        
        GM->addGem(gemCount);
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_GEM);
        BSM->saveUserData(datas);
    }
}
void HeroPage::onEquipedHeroClick(Ref* ref){
    BTN_FROM_REF
    if(unitInfoListHeroDeck.size() <= btn->getTag()){
        return;
    }
    UnitInfo* info = unitInfoListHeroDeck.at(btn->getTag());
    showHeroDetail(info);
}
void HeroPage::onSortByRarity(){
    UDSetInt(KEY_HERO_ARRANGE_TYPE, 1);
    Node* layer = this->getChildByName("heroPage");
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    std::vector<int> indexList;UnitInfo* info;
    for (int rank = 4; rank >= 0; rank--) {
        for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
            info = unitInfoListHeroInventory.at(i);
            if (info->rank == rank) {
                indexList.push_back(i);
            }
        }
    }
    for (int i = 0; i < indexList.size(); i++) {
        Node* nd = sv->getChildByTag(indexList.at(i));
        nd->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), 122));
        
        Node* ndChar = sv->getChildByTag(indexList.at(i) + 100000);
        ndChar->setPosition(nd->getPosition() + Vec2(0, -45));
    }
}
void HeroPage::onSortBySlot(){
    UDSetInt(KEY_HERO_ARRANGE_TYPE, 0);
    Node* layer = this->getChildByName("heroPage");
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        Node* nd = sv->getChildByTag(i);
        nd->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), 122));
        
        Node* ndChar = sv->getChildByTag(i + 100000);
        ndChar->setPosition(nd->getPosition() + Vec2(0, -45));
    }
}
void HeroPage::onFusionClick(){
    Node* layer = CSLoader::createNodeWithVisibleSize("FusionPage.csb");
    this->addChild(layer, 4);
    layer->setName("fusionPage");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBack");
    imgBack->setContentSize(size);
    layer->getChildByName("imgResult")->setOpacity(0);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnFusion");
    btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onFusionButtonClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "fusion");
    
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "fusion");
    lbl = (Text*)layer->getChildByName("lblInventory");
    LM->setLocalizedString(lbl, "inventory");
    
    std::string str;
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    sv->setClippingEnabled(true);
//    sv->setContentSize(cocos2d::Size(size.width, 244));
    std::vector<UnitInfo*> candidateList;
    std::vector<UnitInfo*> candidatePoolList;
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        candidatePoolList.push_back(unitInfoListHeroInventory.at(i));
    }
    int sameUnitCounter = 0;
    for (int i = 0; i < candidatePoolList.size(); i++) {
        sameUnitCounter = 1;
        for (int j = i + 1; j < candidatePoolList.size(); j++) {
            if (candidatePoolList.at(i)->unitType == candidatePoolList.at(j)->unitType &&
                candidatePoolList.at(i)->rank == candidatePoolList.at(j)->rank &&
                candidatePoolList.at(i)->rank < 4) {
                sameUnitCounter++;
                if (sameUnitCounter > 2) {
                    int theUnitType = candidatePoolList.at(i)->unitType;
                    int theRank = candidatePoolList.at(i)->rank;
                    for (int k = 0; k < candidatePoolList.size(); k++) {
                        if (candidatePoolList.at(k)->rank == theRank && candidatePoolList.at(k)->unitType == theUnitType) {
                            candidateList.push_back(candidatePoolList.at(k));
                            candidatePoolList.erase(candidatePoolList.begin() + k);
                            k--;
                        }
                    }
                    i = 0;
                    break;
                }
            }
        }
    }
    
    
    unitInfoListHeroInventoryArrangedForFusion.clear();
    for (int i = 0; i < candidateList.size(); i++) {
        unitInfoListHeroInventoryArrangedForFusion.push_back(candidateList.at(i));
    }
    //    for (int i = 0; i < candidatePoolList.size(); i++) {
    //        unitInfoListHeroInventoryArrangedForFusion.push_back(candidatePoolList.at(i));
    //    }
    candidateList.clear();
    candidatePoolList.clear();
    unitInfoListHeroSelectedForFusion.clear();
    updateUnitListForFusion();
    
    for(int i = 0; i < 3; i++){
        Button* btn = (Button*)layer->getChildByName(strmake("btnEquip%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onSelectedHeroClickForFusion, this));
    }
}
void HeroPage::onCloseHeroPage(){
    GM->heroPage = nullptr;
    checkChangesForHeroes();
//    closePopup();
    clearVectors();
    if (this->getParent() && this->getParent()->getChildByName("trainLayer")) {
        this->getParent()->getChildByName("trainLayer")->setVisible(true);
    }
    closeThis();
}
void HeroPage::clearVectors(){
    unitInfoListHeroInventory.clear();
    unitInfoListHeroDeck.clear();
    unitInfoListHeroInventoryArrangedForFusion.clear();
    unitInfoListHeroSelectedForFusion.clear();
    pvp6RankInfoName.clear();
    pvp6RankInfoTrohpy.clear();
    pvp6RankInfoData.clear();
    pvp12RankInfoName.clear();
    pvp12RankInfoTrohpy.clear();
    pvp12RankInfoData.clear();
}
void HeroPage::onSelectedHeroClickForFusion(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("fusionPage");
    Node* spine = layer->getChildByName(strmake("selectedHeroSpine%d", btn->getTag()));
    if(spine){
        unitInfoListHeroInventoryArrangedForFusion.push_back(unitInfoListHeroSelectedForFusion.at(btn->getTag()));
        unitInfoListHeroSelectedForFusion.erase(unitInfoListHeroSelectedForFusion.begin() + btn->getTag());
        updateUnitListForFusion();
    }
}
void HeroPage::onHeroInFusionClick(Ref* ref){
    if(unitInfoListHeroSelectedForFusion.size() >= 3){
        return;
    }
    ImageView* img = (ImageView*)ref;
    unitInfoListHeroSelectedForFusion.push_back(unitInfoListHeroInventoryArrangedForFusion.at(img->getTag()));
    unitInfoListHeroInventoryArrangedForFusion.erase(unitInfoListHeroInventoryArrangedForFusion.begin() + img->getTag());
    updateUnitListForFusion();
}
void HeroPage::onFusionButtonClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    bool allRegistered = unitInfoListHeroSelectedForFusion.size() == 3;
    if(unitInfoListHeroSelectedForFusion.size() > 0){
        int theUnitType = unitInfoListHeroSelectedForFusion.at(0)->unitType;
        int theRank = unitInfoListHeroSelectedForFusion.at(0)->rank;
        for (int i = 1; i < unitInfoListHeroSelectedForFusion.size(); i++) {
            if (theUnitType != unitInfoListHeroSelectedForFusion.at(i)->unitType ||
                theRank != unitInfoListHeroSelectedForFusion.at(i)->rank) {
                allRegistered = false;
                break;
            }
        }
    }
    
    if(!allRegistered){
        btn->setEnabled(true);
        showInstanceMessage(LM->getText("fusion alert"));
        return;
    }
    
    int rank = unitInfoListHeroSelectedForFusion.at(0)->rank;
    int unitType = unitInfoListHeroSelectedForFusion.at(0)->unitType;
    int level = unitInfoListHeroSelectedForFusion.at(0)->level;
    int counter = 0;
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        if (unitInfoListHeroInventory.at(i)->rank == rank && unitInfoListHeroInventory.at(i)->unitType == unitType) {
            unitInfoListHeroInventory.erase(unitInfoListHeroInventory.begin() + i);
            i--;
            counter++;
            if(counter >= 3){
                break;
            }
        }
    }
    
    UnitInfo* info = new UnitInfo();
    info->unitType = unitType;
    info->level = level;
    info->rank = rank;
    unitInfoListHeroInventory.push_back(info);
    
    Node* layer = this->getChildByName("fusionPage");
    ImageView* imgResult = (ImageView*)layer->getChildByName("imgResult");
    imgResult->setLocalZOrder(10);
    Text* lbl = (Text*)imgResult->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "success");
    lbl = (Text*)imgResult->getChildByName("lblAttack");
    LM->setLocalizedString(lbl, "attack capital");
    lbl = (Text*)imgResult->getChildByName("lblMaxLevel");
    LM->setLocalizedString(lbl, "max level");
    lbl = (Text*)imgResult->getChildByName("lblSkill");
    LM->setLocalizedString(lbl, "skill");
    lbl = (Text*)imgResult->getChildByName("lblHp");
    LM->setLocalizedString(lbl, "HP");
    lbl = (Text*)imgResult->getChildByName("lblMaxLevelBefore");
    lbl->setString(Value(10 + info->rank*2).asString());
    lbl = (Text*)imgResult->getChildByName("lblHpBefore");
    lbl->setString(Value((int)(GM->getUnitHP(info->unitType, info->level)*(1 + 0.2f*info->rank))).asString());
    lbl = (Text*)imgResult->getChildByName("lblAttackBefore");
    lbl->setString(Value((int)(GM->getUnitATT(info->unitType, info->level)*(1 + 0.2f*info->rank))).asString());
    lbl = (Text*)imgResult->getChildByName("lblSkillBefore");
    lbl->setString(strmake("%s%%", Value(10 + 3*info->rank).asString().c_str()));
    
    info->rank++;
    
    lbl = (Text*)imgResult->getChildByName("lblMaxLevelAfter");
    lbl->setString(Value(10 + info->rank*2).asString());
    lbl = (Text*)imgResult->getChildByName("lblHpAfter");
    lbl->setString(Value((int)(GM->getUnitHP(info->unitType, info->level)*(1 + 0.2f*info->rank))).asString());
    lbl = (Text*)imgResult->getChildByName("lblAttackAfter");
    lbl->setString(Value((int)(GM->getUnitATT(info->unitType, info->level)*(1 + 0.2f*info->rank))).asString());
    lbl = (Text*)imgResult->getChildByName("lblSkillAfter");
    lbl->setString(strmake("%s%%", Value(10 + 3*info->rank).asString().c_str()));
    
    float delayTime = 3;
    imgResult->setScale(0.5f);
    imgResult->runAction(Sequence::create(DelayTime::create(delayTime), FadeIn::create(0.1f), NULL));
    imgResult->runAction(Sequence::create(DelayTime::create(delayTime), EaseBackOut::create(ScaleTo::create(0.3f, 1)), NULL));
    
    std::vector<int> datas;
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    
    Button* btnOk = (Button*)layer->getChildByName("btnOk");
    btnOk->addClickEventListener(CC_CALLBACK_0(HeroPage::onOkFusionResult, this));
    btnOk->runAction(Sequence::create(DelayTime::create(3), EaseBackOut::create(MoveTo::create(0.4f, layer->getChildByName("btnFusion")->getPosition())), NULL));
    lbl = (Text*)btnOk->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    layer->getChildByName("imgBlack")->runAction(FadeIn::create(0.5f));
    Button* btnEquip0 = (Button*)layer->getChildByName("btnEquip0");
    Vec2 targetPos = layer->getChildByName(strmake("btnEquip1"))->getPosition();
    for(int i = 0; i < 3; i++){
        layer->getChildByName(strmake("btnEquip%d", i))->runAction(MoveTo::create(1, targetPos));
        layer->getChildByTag(100000 + i)->runAction(MoveTo::create(1, targetPos + Vec2(0, -45)));
    }
    GM->addLightStormEffect(layer);
    //    btnEquip0->runAction(Sequence::create(DelayTime::create(0.5f), ScaleTo::create(1.5f, 0.8f), NULL));
    GM->makeItSiluk(btnEquip0);
    Button* btnTemp = (Button*)layer->getChildByName("btnTemp");
    Button* img = (Button*)btnTemp->clone();
    layer->addChild(img);
    img->setOpacity(0);
    img->setPosition(targetPos);
    img->setScale(0.5f);
    img->setColor(getRankColor(info->rank));
    img->runAction(Sequence::create(DelayTime::create(delayTime), FadeIn::create(0.1f), NULL));
    img->runAction(Sequence::create(DelayTime::create(delayTime), EaseBackOut::create(ScaleTo::create(0.3f, 1)), NULL));
    lbl = (Text*)img->getChildByName("lblLevel");
    lbl->setString(strmake("LV.%d", info->level + 1));
    for (int i = 0; i < 3; i++) {
        Sprite* spt = Sprite::create("whiteBigCircle.png");
        img->addChild(spt, -1);
        spt->setPosition(img->getContentSize()/2);
        spt->setScale(7, 2.5f);
        spt->setOpacity(0);
        spt->setRotation(i*120);
        spt->runAction(RepeatForever::create(RotateBy::create(1, 100)));
        spt->runAction(Sequence::create(DelayTime::create(delayTime), FadeIn::create(0.05f), NULL));
        spt->setBlendFunc(BlendFunc::ADDITIVE);
        spt->setColor(Color3B::YELLOW);
    }
}
void HeroPage::onOkFusionResult(){
    closePopup();
    updateHeroes();
}
void HeroPage::updateHeroes(){
    Node* layer = this->getChildByName("heroPage");
    std::string str;Button* btn;Text* lbl;
//    std::string strEquippedDeck = UDGetStr(KEY_UNITS_HERO_DECK, "");
//    ValueVector unitsDeck = GM->split(strEquippedDeck, "_");
    UnitInfo* info;
    int index = 0;
    unitInfoListHeroDeck.clear();
    unitInfoListHeroDeck = GM->getHeroDeck();
    for (int i = 0; i < unitInfoListHeroDeck.size(); i++){
//        str = unitsDeck.at(i).asString();
//        if(str.length() <= 1){
//            continue;
//        }
        btn = (Button*)layer->getChildByName(strmake("btnEquip%d", index));
//        if(str.length() > 0){
            info = unitInfoListHeroDeck.at(i);//GM->getUnitInfoFromString(unitsDeck.at(i).asString());
            if(info->unitType < 0){
                continue;
            }
//            unitInfoListHeroDeck.push_back(info);
//            btn->addClickEventListener(CC_CALLBACK_1(HeroPage::onEquipedHeroClick, this));
//        }else{
//            continue;
//        }
        
        lbl = (Text*)btn->getChildByName("lblLevel");
        
        if (unitInfoListHeroDeck.size() > index) {
            info = unitInfoListHeroDeck.at(index);
            lbl->setString(strmake("LV.%d", info->level + 1));
            layer->removeChildByName(strmake("hero%d", index));
            spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
            layer->addChild(spChar);
            spChar->setName(strmake("hero%d", index));
            spChar->setAnimation(0, "idle", true);
            //            spChar->setScale(0.75f);
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
            sptShadow->setOpacity(0);
            spChar->setPosition(btn->getPosition() + Vec2(0, -45));
            btn->setColor(getRankColor(info->rank));
        }else{
            //            lbl->setString("^ 0^/");
        }
        index++;
        if(index >= 12){
            break;
        }
    }
    for(int i = index; i < 12; i++){
        btn = (Button*)layer->getChildByName(strmake("btnEquip%d", i));
        lbl = (Text*)btn->getChildByName("lblLevel");
        layer->removeChildByName(strmake("hero%d", i));
        btn->setColor(Color3B::WHITE);
        lbl->setString("Empty");
    }
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    int inventoryScrollX = sv->getInnerContainerPosition().x;
    sv->removeAllChildren();
    //    sv->setClippingEnabled(true);
    //    UDSetStr(KEY_UNITS_HERO_INVENTORY, "_50/101_49/0_49/0_49/0_50/101_49/200_49/200_49/200"); // test
//    std::string strEquippedIvt = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
//    ValueVector unitsIvt = GM->split(strEquippedIvt, "_");
//    sv->setContentSize(cocos2d::Size(size.width, 244));
    
    unitInfoListHeroInventory.clear();
    unitInfoListHeroInventory = GM->getHeroInventory();
    ImageView* img;
    ImageView* imgTemp = (ImageView*)layer->getChildByName("btnEquip0");
    index = 0;
    for(int i = 0; i < unitInfoListHeroInventory.size(); i++){
//        str = unitsIvt.at(i).asString();
//        if(str.length() <= 1){
//            continue;
//        }
        UnitInfo* info;
//        if(str.length() > 1){
            info = unitInfoListHeroInventory.at(i);//GM->getUnitInfoFromString(unitsIvt.at(i).asString());
            if(info->unitType < 0){
                continue;
            }
//            unitInfoListHeroInventory.push_back(info);
//        }else{
//            continue;
//        }
        
        img = (ImageView*)imgTemp->clone();
        sv->addChild(img);
        img->setTag(index);
        img->setPosition(Vec2(img->getContentSize().width*(0.45f + index), 122));
        img->addClickEventListener(CC_CALLBACK_1(HeroPage::onHeroInInventoryClick, this));
        int rareness = info->rank;
        img->setColor(getRankColor(rareness));
        Text* lbl = (Text*)img->getChildByName("lblLevel");
        lbl->setLocalZOrder(10);
        lbl->setString(strmake("LV.%d", info->level + 1));
        
        
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        sv->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        //        spChar->setScale(0.75f);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition() + Vec2(0, -45));
        spChar->setTag(100000 + index);
        index++;
    }
    
    float totalWidth = (unitInfoListHeroInventory.size() + 0.3f)*261;
    if (totalWidth > size.width) {
        sv->setInnerContainerSize(cocos2d::Size(totalWidth, sv->getContentSize().height));
    }else{
        sv->setInnerContainerSize(cocos2d::Size(size.width, sv->getContentSize().height));
    }
    sv->setInnerContainerPosition(Vec2(inventoryScrollX, sv->getInnerContainerPosition().y));
    int arrangeType = UDGetInt(KEY_HERO_ARRANGE_TYPE, 0);
    if(arrangeType == 1){
        onSortByRarity();
    }
    
    int sameUnitCounter = 0;
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        sameUnitCounter = 1;
        for (int j = i + 1; j < unitInfoListHeroInventory.size(); j++) {
            if (unitInfoListHeroInventory.at(i)->unitType == unitInfoListHeroInventory.at(j)->unitType &&
                unitInfoListHeroInventory.at(i)->rank == unitInfoListHeroInventory.at(j)->rank &&
                unitInfoListHeroInventory.at(i)->rank < 4) {
                sameUnitCounter++;
                if (sameUnitCounter > 2) {
                    break;
                }
            }
        }
        if (sameUnitCounter > 2) {
            break;
        }
    }
    btn = (Button*)layer->getChildByName("btnFuse");
    btn->getChildByName("imgRedDot")->setVisible(sameUnitCounter > 2);
    
}
void HeroPage::updateUnitListForFusion(){
    Node* layer = this->getChildByName("fusionPage");
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    ImageView* img;
    ImageView* imgTemp = (ImageView*)layer->getChildByName("btnEquip0");
    int index = 0;
    sv->removeAllChildren();
    
    for(int i = 0; i < unitInfoListHeroInventoryArrangedForFusion.size(); i++){
        UnitInfo* info = unitInfoListHeroInventoryArrangedForFusion.at(i);
        img = (ImageView*)imgTemp->clone();
        sv->addChild(img);
        img->setTag(index);
        img->setPosition(Vec2(img->getContentSize().width*(0.45f + index), 122));
        img->addClickEventListener(CC_CALLBACK_1(HeroPage::onHeroInFusionClick, this));
        int rareness = info->rank;
        img->setColor(getRankColor(rareness));
        Text* lbl = (Text*)img->getChildByName("lblLevel");
        lbl->setLocalZOrder(10);
        lbl->setString(strmake("LV.%d", info->level + 1));
        
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        sv->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        spChar->setScale(0.75f);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(img->getPosition() + Vec2(0, -45));
        spChar->setTag(100000 + index);
        index++;
    }
    float totalWidth = (unitInfoListHeroInventory.size() + 0.3f)*261;
    if (totalWidth > size.width) {
        sv->setInnerContainerSize(cocos2d::Size(totalWidth, sv->getContentSize().height));
    }else{
        sv->setInnerContainerSize(cocos2d::Size(size.width, sv->getContentSize().height));
    }
    for (int i = 0; i < 3; i++) {
        Node* spine = layer->getChildByName(strmake("selectedHeroSpine%d", i));
        if(spine){
            spine->removeFromParent();
        }
    }
    for(int i = 0; i < 3; i++){
        layer->removeChildByTag(100000 + i);
    }
    for(int i = 0; i < unitInfoListHeroSelectedForFusion.size(); i++){
        UnitInfo* info = unitInfoListHeroSelectedForFusion.at(i);
        Button* btn = (Button*)layer->getChildByName(strmake("btnEquip%d", i));
        int rareness = info->rank;
        btn->setColor(getRankColor(rareness));
        Text* lbl = (Text*)btn->getChildByName("lblLevel");
        lbl->setLocalZOrder(10);
        lbl->setString(strmake("LV.%d", info->level + 1));
        
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
        layer->addChild(spChar, 10);
        spChar->setAnimation(0, "idle", true);
        spChar->setScale(0.75f);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        spChar->setPosition(btn->getPosition() + Vec2(0, -45));
        spChar->setTag(100000 + i);
        spChar->setName(strmake("selectedHeroSpine%d", i));
    }
    Button* btnFusion = (Button*)layer->getChildByName("btnFusion");
    bool allRegistered = unitInfoListHeroSelectedForFusion.size() == 3;
    if(unitInfoListHeroSelectedForFusion.size() > 0){
        int theUnitType = unitInfoListHeroSelectedForFusion.at(0)->unitType;
        int theRank = unitInfoListHeroSelectedForFusion.at(0)->rank;
        for (int i = 1; i < unitInfoListHeroSelectedForFusion.size(); i++) {
            if (theUnitType != unitInfoListHeroSelectedForFusion.at(i)->unitType ||
                theRank != unitInfoListHeroSelectedForFusion.at(i)->rank) {
                allRegistered = false;
                break;
            }
        }
    }
    
//    btnFusion->setEnabled(allRegistered);
}
void HeroPage::onHeroInInventoryClick(Ref* ref){
    ImageView* img = (ImageView*)ref;
    UnitInfo* info = unitInfoListHeroInventory.at(img->getTag());
    showHeroDetail(info);
}
void HeroPage::showHeroDetail(UnitInfo* info, bool showLightning){
    const char* popupName = "heroDetail";
    Node* previousPopup = this->getChildByName(popupName);
    if(previousPopup){
        closePopup();
    }
    bool isFromInventory = false;
    for(auto thisInfo : unitInfoListHeroInventory){
        if(thisInfo == info){
            isFromInventory = true;
            break;
        }
    }
    bool isFromDeck = false;
    for(auto thisInfo : unitInfoListHeroDeck){
        if(thisInfo == info){
            isFromDeck = true;
            break;
        }
    }
    selectedUnitInfo = info;
    Node* layer = CSLoader::createNode("UnitTrainInfo.csb");
    this->addChild(layer, 4);
    layer->setName(popupName);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    Button* btnUpgrade = (Button*)layer->getChildByName("btnUpgrade");
    btnUpgrade->addClickEventListener(CC_CALLBACK_0(HeroPage::onUpgradeUnitClick, this));
    btnUpgrade->setEnabled(info->rank < 20);
    int unit = info->unitType;
    int level = info->level;
    Text* lbl = (Text*)btnUpgrade->getChildByName("lblGold");
    lbl->setString(Value(GM->getMaxGold(level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTree");
    lbl->setString(Value(GM->getMaxTree(level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade");
    Button* btnHire = (Button*)layer->getChildByName("btnHire");
    btnHire->setVisible(false);
    Button* btnMoveToDeck = (Button*)layer->getChildByName("btnMoveToDeck");
    btnMoveToDeck->addClickEventListener(CC_CALLBACK_0(HeroPage::onMoveToDeckUnitClick, this));
    btnMoveToDeck->setVisible(isFromInventory);
    lbl = (Text*)btnMoveToDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "move to deck");
    Button* btnRemoveFromDeck = (Button*)layer->getChildByName("btnRemoveFromDeck");
    btnRemoveFromDeck->addClickEventListener(CC_CALLBACK_0(HeroPage::onRemoveFromDeckUnitClick, this));
    btnRemoveFromDeck->setVisible(isFromDeck);
    lbl = (Text*)btnRemoveFromDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "remove from deck");
    Button* btnFire = (Button*)layer->getChildByName("btnFire");
    btnFire->addClickEventListener(CC_CALLBACK_0(HeroPage::onFireHeroClick, this));
    btnFire->setVisible(isFromInventory);
//    btnFire->setPosition(Vec2(-300, 0) + btnFire->getPosition());
    btnFire->setPosition(btnMoveToDeck->getPosition());
    
    lbl = (Text*)btnFire->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "fire");
    btn = (Button*)layer->getChildByName("btnComplete");
//    btn->setVisible(false);
    //    btn->setPosition(btnUpgrade->getPosition());
    
    Node* nd = layer->getChildByName("ndUpgradeTime");
    nd->setVisible(false);
    lbl = (Text*)layer->getChildByName("lblHP");
    lbl->setString(Value((int)(GM->getUnitHP(unit, level)*(1 + info->rank*0.2f))).asString());
    lbl = (Text*)layer->getChildByName("lblLevel");
    lbl->setString(strmake("Lv.%d/%d", level + 1, 10 + info->rank*2));
    lbl = (Text*)lbl->clone();
    layer->addChild(lbl, 10);
    lbl->setAnchorPoint(Vec2(0.5, 0.5));
    lbl->setPosition(Vec2(955.5f, 761.5f));
    lbl->setFontSize(44);
    lbl->enableOutline(Color4B::BLACK, 4);
    if (info->rank == 0) {
        LM->setLocalizedString(lbl, "common hero");
    }else if (info->rank == 1) {
        LM->setLocalizedString(lbl, "good hero");
    }else if (info->rank == 2) {
        LM->setLocalizedString(lbl, "great hero");
    }else if (info->rank == 3) {
        LM->setLocalizedString(lbl, "rare hero");
    }else if (info->rank == 4) {
        LM->setLocalizedString(lbl, "epic hero");
    }
    lbl = (Text*)layer->getChildByName("lblName");
    lbl->setString(LM->getText(GM->getUnitName(unit)));
    lbl = (Text*)layer->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, strmake("%s desc", GM->getUnitName(unit).c_str()));
    Node* ndAttack = layer->getChildByName("ndAttack");
    lbl = (Text*)ndAttack->getChildByName("lblAttack");
    float att = GM->getUnitATT(unit, level)*(1 + info->rank*0.2f);
    if(info->unitType == UNIT_HERO_HEALER){
        att *= 10;
    }
    lbl->setString(Value((int)(att)).asString());
    if(info->unitType == GM->getMidMonthHeroType() ||
       info->unitType == GM->getMonthlyHeroType()){
        lbl->setString(lbl->getString() + strmake("(+%d)",(int)att/2));
    }
    
    lbl = (Text*)ndAttack->getChildByName("lblAttackAdd");
    lbl->setVisible(false);//popupType == 1);
    lbl->setString(strmake("+%d", GM->getUnitATT(unit, level+1) - GM->getUnitATT(unit, level)));
    lbl = (Text*)layer->getChildByName("lblHPAdd");
    lbl->setVisible(false);//popupType == 1);
    lbl->setString(strmake("+%d", GM->getUnitHP(unit, level+1) - GM->getUnitHP(unit, level)));
    Node* ndPopulation = layer->getChildByName("ndPopulation");
    ImageView* img = (ImageView*)ndPopulation->getChildByName("imgPopulation");
    img->loadTexture("iconSkill.png");
    img->setContentSize(cocos2d::Size(100, 100));
    lbl = (Text*)ndPopulation->getChildByName("lblPopulation");
    int skill = 10 + 3*info->rank;
    lbl->setString(strmake("%s%%", Value(skill).asString().c_str()));
    if(info->unitType == GM->getMidMonthHeroType() ||
       info->unitType == GM->getMonthlyHeroType()){
        lbl->setString(lbl->getString() + strmake("(+%d)",skill/2));
    }
    
    layer->getChildByName("ndUpgradeTimer")->setVisible(false);
    img = (ImageView*)layer->getChildByName("imgIconBack");
    img->setColor(getRankColor(info->rank));
    
    
    spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
    spChar->setAnimation(0, "idle", true);
    spChar->setScale(1.35f);
    Sprite* sptShadow = Sprite::create("shadow.png");
    spChar->addChild(sptShadow, -1);
    sptShadow->setOpacity(0);
    btn->setColor(getRankColor(info->rank));
    spChar->setName("spChar");
    layer->addChild(spChar);
    spChar->setPosition(layer->getChildByName("imgIconBack")->getPosition() + Vec2(0, -100));
    
    btnMoveToDeck->setPosition(btnHire->getPosition());
    btnRemoveFromDeck->setPosition(btnHire->getPosition());
    
    if(showLightning){
        
        Vec2 point = spChar->getPosition();
        Sprite* spt = Sprite::createWithSpriteFrameName("lightning0.png");
        spt->setAnchorPoint(Vec2(0.5, 0));
        layer->addChild(spt, 11);
        Animation* animation = AnimationCache::getInstance()->getAnimation("effectLightning");
        Animate* animate = Animate::create(animation);
        spt->runAction(Sequence::create(animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        spt->setPosition(point + Vec2(0, -49));
        
        spt = Sprite::create("whiteBigCircle.png");
        layer->addChild(spt);
        spt->setPosition(point + Vec2(0, 100));
        spt->setOpacity(0);
        spt->setColor(Color3B::BLACK);
        spt->setScale(4);
        spt->runAction(Sequence::create(DelayTime::create(0.06f*5), FadeTo::create(0, 60), DelayTime::create(0.1f), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        
        spt = Sprite::create("whiteBigCircle.png");
        layer->addChild(spt);
        spt->setPosition(point);// + Vec2(0, 49));
        spt->setOpacity(0);
        spt->setBlendFunc(BlendFunc::ADDITIVE);
        spt->setColor(Color3B(80, 250, 255));
        spt->runAction(Sequence::create(DelayTime::create(0.06f*4), FadeTo::create(0, 120), ScaleTo::create(0.1f, 2), FadeOut::create(0.2f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        
        for (int i = 0; i < 5; i++) {
            spt = Sprite::create("whiteBigCircle.png");
            layer->addChild(spt);
            spt->setPosition(point);// + Vec2(0, 49));
            spt->setOpacity(0);
            spt->setScale(0.2f);
            spt->setBlendFunc(BlendFunc::ADDITIVE);
            spt->setColor(Color3B(0, 240, 250));
            int distance = 180;
            float angle = 3.14f*(rand()%360)/180;
            spt->runAction(Sequence::create(DelayTime::create(0.06f*4 + 0.1f), FadeOut::create(0.2f), NULL));
            spt->runAction(Sequence::create(DelayTime::create(0.06f*4), FadeTo::create(0, 120), MoveBy::create(0.2f, Vec2(distance*cos(angle), distance*sin(angle))), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        }
        
        spt = Sprite::createWithSpriteFrameName("lightning5.png");
        spt->setOpacity(0);
        layer->addChild(spt, 1);
        spt->setAnchorPoint(Vec2(0.5, 0));
        spt->setPosition(point + Vec2(0, -49));
        spt->runAction(Sequence::create(DelayTime::create(0.06f*5), FadeTo::create(0, 30), DelayTime::create(0.1f), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }
    
    
    if (heroPageTutorialIndex == 3) {
        this->getChildByName("heroPage")->getChildByName("svInventory")->getChildByTag(0)->removeChildByName("arrow");
        addGreenArrowToButton(btnMoveToDeck);
    }
}
void HeroPage::onRemoveFromDeckUnitClick(){
    int index= 0;
    for(auto info: unitInfoListHeroDeck){
        if(info == selectedUnitInfo){
            unitInfoListHeroDeck.erase(unitInfoListHeroDeck.begin() + index);
            break;
        }
        index++;
    }
    unitInfoListHeroInventory.push_back(selectedUnitInfo);
    
    shouldSaveHeroInventoryInfo = true;
    updateHeroInventorySaveData();
    shouldSaveHeroDeckInfo = true;
    updateHeroDeckSaveData();
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_HERO_DECK);
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    
    closePopup();
    updateHeroes();
}

void HeroPage::onFireHeroClick(){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, 4);
    setAsPopup(layer);
    layer->setName("messageBox");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "no");
    btn = (Button*)layer->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onFireOk, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "yes");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "fire ask");
}
void HeroPage::onFireOk(){
    
    int index= 0;
    for(auto info: unitInfoListHeroInventory){
        if(info == selectedUnitInfo){
            unitInfoListHeroInventory.erase(unitInfoListHeroInventory.begin() + index);
            break;
        }
        index++;
    }
    
    shouldSaveHeroInventoryInfo = true;
    updateHeroInventorySaveData();
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    
    closePopup();
    closePopup();
    updateHeroes();
}
void HeroPage::onFireCancel(){
    
}
void HeroPage::onMoveToDeckUnitClick(){
    if(unitInfoListHeroDeck.size() >= 12){
        showInstanceMessage(LM->getText("not enough slot"));
        return;
    }
    int index= 0;
    for(auto info: unitInfoListHeroInventory){
        if(info == selectedUnitInfo){
            unitInfoListHeroInventory.erase(unitInfoListHeroInventory.begin() + index);
            break;
        }
        index++;
    }
    unitInfoListHeroDeck.push_back(selectedUnitInfo);
    
    shouldSaveHeroInventoryInfo = true;
    updateHeroInventorySaveData();
    shouldSaveHeroDeckInfo = true;
    updateHeroDeckSaveData();
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_HERO_DECK);
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    
    closePopup();
    updateHeroes();
    
    if (heroPageTutorialIndex == 3) {
        heroPageTutorialIndex++;
        showHeroPageTutorial(LM->getText("hero tutorial 2"));
    }
}
void HeroPage::onUpgradeUnitClick(){
    int goldPrice = GM->getMaxGold(selectedUnitInfo->level);
    int treePrice = GM->getMaxTree(selectedUnitInfo->level);
    if(GM->getCoin() < goldPrice){
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    if(GM->getTree() < treePrice){
        showInstanceMessage(LM->getText("not enough lumber"));
        return;
    }
    
    int maxLevel = 9 + selectedUnitInfo->rank*2;
    if(maxLevel <= selectedUnitInfo->level){
        showInstanceMessage(LM->getText("max level alert"));
        return;
    }
    
    GM->addCoin(-goldPrice);
    GM->addTree(-treePrice);
    selectedUnitInfo->level++;
    
    bool isFromInventory = false;
    int inventoryIndex = -1;
    for(auto thisInfo : unitInfoListHeroInventory){
        inventoryIndex++;
        if(thisInfo == selectedUnitInfo){
            isFromInventory = true;
            break;
        }
    }
    bool isFromDeck = false;
    int deckIndex = -1;
    for(auto thisInfo : unitInfoListHeroDeck){
        deckIndex++;
        if(thisInfo == selectedUnitInfo){
            isFromDeck = true;
            break;
        }
    }
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_TREE);
    if(isFromDeck){
        updateHeroDeckSaveData();
        datas.push_back(DATA_TYPE_HERO_DECK);
    }
    if (isFromInventory) {
        updateHeroInventorySaveData();
        datas.push_back(DATA_TYPE_HERO_INVENTORY);
    }
    BSM->saveUserData(datas);
    
    
    closePopup();
    updateHeroes();
    if (isFromDeck) {
        showHeroDetail(unitInfoListHeroDeck.at(deckIndex), true);
    }else if(isFromInventory){
        showHeroDetail(unitInfoListHeroInventory.at(inventoryIndex), true);
    }
    Node* spChar = this->getChildByName("heroDetail")->getChildByName("spChar");
    float originalScale = spChar->getScaleX();
    spChar->runAction(Sequence::create(ScaleTo::create(0.15f, originalScale*1.2f), EaseIn::create(ScaleTo::create(0.12f, originalScale*1), 2), nullptr));
}
void HeroPage::onChestClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("heroPage");
    Text* lbl;
    btn = (Button*)layer->getChildByName(strmake("btnGacha%d", btn->getTag()));
    lbl = (Text*)btn->getChildByName("lbl");
    time_t now = BSM->getCurrentTimeT();
    bool saveKey = false;
    bool saveGem = false;
    bool saveInventory = false;
    
    if(btn->getTag() == 0){
        if (GM->getWoodKey() > 0) {
            GM->addWoodKey(-1);
            pickHero(0, 2);
            saveKey = true;
            saveInventory = true;
        }else if(lbl->getTag() == 0){
            requestedChestType = CHEST_WOOD;
            requestTimeForGacha();
        }else{
            int price = Value(lbl->getString()).asInt();
            if(price <= GM->getGem()){
                showChestOpenWithGemAskPopup(price);
            }else{
                showInstanceMessage(LM->getText("not enough gems"));
            }
        }
    }else if(btn->getTag() == 1){
        if (GM->getGoldKey() > 0) {
            GM->addGoldKey(-1);
            pickHero(2, 4);
            saveKey = true;
            saveInventory = true;
        }else if(lbl->getTag() == 0){
            requestedChestType = CHEST_GOLD;
            requestTimeForGacha();
        }else{
            int price = Value(lbl->getString()).asInt();
            if(price <= GM->getGem()){
                showChestOpenWithGemAskPopup(price);
            }else{
                showInstanceMessage(LM->getText("not enough gems"));
            }
        }
    }
    if(saveKey || saveInventory || saveGem){
        std::vector<int> datas;
        if(saveGem){
            datas.push_back(DATA_TYPE_GEM);
        }
        if (saveKey) {
            datas.push_back(DATA_TYPE_KEYS);
        }
        if (saveInventory) {
            updateHeroInventorySaveData();
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
        }
        BSM->saveUserData(datas);
        updateHeroes();
    }
}
void HeroPage::showChestOpenWithGemAskPopup(int price){
    chestOpenPrice = price;
    Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("chestOpenWithGemAskPopup");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onConfirmChestOpenWithGem, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "yes");
    
    btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::closePopup, this));
    btn->setVisible(true);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "no");
    
    lbl = (Text*)layer->getChildByName("lblGold");
    Node* img = layer->getChildByName("imgGold");
    img->setVisible(false);
    lbl->setVisible(false);
    layer->getChildByName("lblGemCount")->setPositionY(img->getPositionY());
    layer->getChildByName("imgGem")->setPositionY(img->getPositionY());
    
    lbl = (Text*)layer->getChildByName("lblTree");
    img = layer->getChildByName("imgTree");
    img->setVisible(false);
    lbl->setVisible(false);
    
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(chestOpenPrice).asString());
    GM->alignToCenter(layer->getChildByName("imgGem"), lbl, 50, layer->getContentSize().width/2, 0);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "use gem to open chest");
}
void HeroPage::onConfirmChestOpenWithGem(){
    closePopup();
    GM->addGem(-chestOpenPrice);
    if(chestOpenPrice >= 300){
        pickHero(2, 4);
    }else{
        pickHero(0, 2);
    }
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GEM);
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    updateHeroes();
}
void HeroPage::requestTimeForGacha(){
    isTimeRequestedForGacha = true;
    BSM->getHttpTime();
}
void HeroPage::onTimeRecievedForGacha(){
    isTimeRequestedForGacha = false;
    isTimeReceivedForGacha = true;
}
void HeroPage::pickHero(int minRank, int maxRank){
    int rankRate[5];
    if (maxRank < 3) {
        rankRate[0] = 85;
        rankRate[1] = 10;
        rankRate[2] = 5;
//        rankRate[3] = 10;
//        rankRate[4] = 2;
    }else{
//        rankRate[0] = 80;
//        rankRate[1] = 50;
        rankRate[2] = 90;
        rankRate[3] = 8;
        rankRate[4] = 2;
    }
//    _64/417/22/9_54/417/21/8_64/417/21/9_64/417/21/9_47/417/28/10_55/417/19/9_54/417/19/11_64/417/19/17_65/215/-1/-1_58/417/26/15_56/213/-1/-1_52/417/22/9_
    int rateSum = 0;
    for (int i = minRank; i <= maxRank; i++) {
        rateSum += rankRate[i];
    }
    int pickedRank = -1;
    int randomNumber = rand()%rateSum;
    for (int i = minRank; i <= maxRank; i++) {
        randomNumber -= rankRate[i];
        if (randomNumber <= 0) {
            pickedRank = i;
            break;
        }
    }
    log("picked: %d", pickedRank);
    std::vector<int> array;
//    int kind = UNIT_HERO_REAPER - UNIT_HERO_ORC;
    
    int totalRate = 0;
    std::vector<int> list = getPickPossibleHeroes();
    for (int i = 0; i < list.size(); i++) {
        totalRate += getPickRate(list.at(i));
    }
    int pickedNum = rand()%(totalRate);
    int pickedHero = -1;
    for (int i = 0; i < list.size(); i++) {
        pickedNum -= getPickRate(list.at(i));
        if(pickedNum <= 0){
            pickedHero = list.at(i);
            break;
        }
    }
    
    array.push_back(pickedHero);
    
    for(int i = 0; i < array.size(); i++){
        UnitInfo* info = new UnitInfo();
        info->unitType = array.at(i);
        info->level = pickedRank*100;
        unitInfoListHeroInventory.push_back(info);
    }
    
    Node* layer = CSLoader::createNode("ItemGet.csb");
    Node* ndFront = layer->getChildByName("ndFront");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("itemGet");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    ndFront->setPositionX(-layer->getPositionX());
    GM->addLightStormEffect(ndFront);
    
    ImageView* img;ImageView* imgWhiteClone;
    ImageView* imgWhite = (ImageView*)layer->getChildByName("imgWhiteCard");
    int value;
    ImageView* imgTemp =(ImageView*)layer->getChildByName("imgTemp");
    float initTime = 0.3f;
    float fadeInTime = 0.3f;
    float fadeOutTime = 1.0f;
    float centerY = size.height/2 + 200;
    
    float gap = 130;
    int columnCount = 5;
    if (columnCount > array.size()) {
        columnCount = array.size();
    }
    float scale = 1;
    int a = (int)(array.size())/columnCount;
    int b = array.size()%columnCount==0?0:1;
    int rowCount = a + b;
    float y = centerY - (imgWhite->getContentSize().height*scale + gap)*(rowCount-1)/2;
    float centerX = size.width/2;
    int itemCountInThisRow = 0;
    float totalWidthInThisRow = 0;
    float itemWidth = imgTemp->getContentSize().width;
    float whiteStayTime = 0.5f;
    float showOffTime = 0.5f;
    float delayBetweenItems = 0.2f;
    float totalDelay = 0;
    Sprite* sptChest;
    Sprite* sptChestOpen;
    if (minRank == 0) {
        sptChest = Sprite::create("chestWoodClosed.png");
        sptChestOpen = Sprite::create("chestWoodOpen.png");
    }else{
        sptChest = Sprite::create("chestGoldClosed.png");
        sptChestOpen = Sprite::create("chestGoldOpen.png");
    }
    ndFront->addChild(sptChest);
    ndFront->addChild(sptChestOpen);
    sptChest->setPosition(Vec2(size.width/2, 260));
    sptChestOpen->setPosition(Vec2(size.width/2, 260));
    sptChestOpen->setOpacity(0);
    sptChest->runAction(Sequence::create(ScaleTo::create(0.5f, 0.7f), FadeOut::create(0), NULL));
    sptChestOpen->runAction(Sequence::create(DelayTime::create(0.5f), FadeIn::create(0), JumpBy::create(0.3f, Vec2(0, 0), 100, 1), DelayTime::create(1.0f), ScaleTo::create(0.2f, 0.1f), FadeOut::create(0), NULL));
    sptChestOpen->setScale(0.8f);
    sptChestOpen->runAction(Sequence::create(DelayTime::create(0.5f), EaseBackOut::create(ScaleTo::create(0.3f, 1)), NULL));
    float hundleDur = 0.03f;
    int hundleAngle = 5;
    sptChest->runAction(RepeatForever::create(Sequence::create(RotateBy::create(hundleDur, hundleAngle), RotateBy::create(hundleDur*2, -2*hundleAngle), RotateBy::create(hundleDur, hundleAngle), NULL)));
    for (int i = 0; i < array.size(); i++) {
        img = (ImageView*)imgTemp->clone();
        ndFront->addChild(img);
        value = array.at(i);
        spine::SkeletonAnimation* spChar = GM->getHeroSpine(value);
        ndFront->addChild(spChar, 0);
        spChar->setAnimation(0, "idle", true);
        //        spChar->setScale(0.75f);
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
        sptShadow->setOpacity(0);
        sptShadow->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        
        spChar->setOpacity(0);
        spChar->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        //        if(spChar->getChildren().size() > 0){
        //            spChar->getChildren().at(0)->setOpacity(0);
        //            spChar->getChildren().at(0)->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        //        }
        imgWhiteClone = (ImageView*)imgWhite->clone();
        img->getChildByName("lbl")->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeOut::create(fadeOutTime), NULL));
        int rareness = pickedRank;
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeIn::create(fadeInTime), DelayTime::create(whiteStayTime + rareness*0.1f), FadeOut::create(fadeOutTime), NULL));
        img->setColor(getRankColor(rareness));
        int angle = 5;
        float dur = 0.05f;
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeIn::create(0.1f), NULL));
        imgWhiteClone->runAction(RepeatForever::create(Sequence::create(RotateBy::create(dur, angle), RotateBy::create(dur*2, -2*angle), RotateBy::create(dur, angle), NULL)));
        
        
        img->getChildByName("lbl")->setPosition(img->getContentSize()/2);
        spChar->setPosition(Vec2(img->getContentSize().width/2, img->getContentSize().height/2 + 2));
        if(array.size() - i > 0){
            itemCountInThisRow = columnCount;
        }else{
            itemCountInThisRow = array.size()%columnCount;
        }
        totalWidthInThisRow = itemCountInThisRow*(itemWidth + gap) - gap;
        img->setPosition(Vec2(centerX - totalWidthInThisRow*scale/2 + (i%columnCount)*(itemWidth + gap)*scale + itemWidth*scale/2, y + (i/columnCount)*(img->getContentSize().height*scale + gap/2)));
        
        spChar->setPosition(img->getPosition() + Vec2(0, -60));
        
        ndFront->addChild(imgWhiteClone, 1);
        
        float dropDistance = -500;
        img->setPositionY(img->getPositionY() + dropDistance);
        imgWhiteClone->setPosition(img->getPosition());
        float openDelayTime = 0.51f;
        img->setOpacity(0);
        img->runAction(Sequence::create(DelayTime::create(totalDelay + openDelayTime),FadeIn::create(0.1f), nullptr));
        img->runAction(Sequence::create(DelayTime::create(totalDelay + openDelayTime), EaseBackOut::create(MoveBy::create(showOffTime, Vec2(0, -dropDistance))), NULL));
        imgWhiteClone->setOpacity(0);
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(totalDelay + openDelayTime),FadeIn::create(0.1f), nullptr));
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(totalDelay + openDelayTime), EaseBackOut::create(MoveBy::create(showOffTime, Vec2(0, -dropDistance))), NULL));
        
        showOffTime += delayBetweenItems;
    }
    
    Button* btnAgain = (Button*)layer->getChildByName("btnAgain");
    if (minRank == 0) {
        btnAgain->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroWoodChestGachaAgainClick, this));
    }else{
        btnAgain->addClickEventListener(CC_CALLBACK_0(HeroPage::onHeroGoldChestGachaAgainClick, this));
    }
    
    btnAgain->setOpacity(100);
    Vec2 btnPos = btnAgain->getPosition();
    btnAgain->setPosition(btnPos + Vec2(-700, 0));
    btnAgain->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime), MoveTo::create(fadeInTime, btnPos), NULL));
    btnAgain->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime), FadeIn::create(fadeInTime), NULL));
    Text* lblPickCount = (Text*)btnAgain->getChildByName("lblPickCount");
    lblPickCount->setString(strmake("x%d", (int)array.size()));
    btnAgain->setTag(array.size());
    Text* lblPrice = (Text*)btnAgain->getChildByName("lblPrice");
    if (minRank == 0) {
        lblPrice->setString("50");
    }else{
        lblPrice->setString("290");
    }
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(HeroPage::onClosePickHero, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime + 1), FadeIn::create(fadeInTime), NULL));
    
    imgTemp->removeFromParent();
    imgWhite->removeFromParent();
    
    //    BHUD->showIndicator();
    
    shouldSaveHeroInventoryInfo = true;
    list.clear();
}
void HeroPage::onClosePickHero(){
    closePopup();
    if(heroPageTutorialIndex == 1){
        heroPageTutorialIndex++;
        showHeroPageTutorial(LM->getText("hero tutorial 1"));
        this->getChildByName("heroPage")->getChildByName("btnGacha0")->removeChildByName("arrow");
    }
}
int HeroPage::getPickRate(int unit){
    if(unit == UNIT_HERO_ORC){
        return 581;
    }
    if(unit == UNIT_HERO_GOBLIN){
        return 881;
    }
    if(unit == UNIT_HERO_SPEARMAN){
        return 581;
    }
    if(unit == UNIT_HERO_LIZARDMAN){
        return 781;
    }
    if(unit == UNIT_HERO_ARCHER){
        return 211;
    }
    if(unit == UNIT_HERO_WEREWOLF){
        return 211;
    }
    if(unit == UNIT_HERO_MONK){
        return 211;
    }
    if(unit == UNIT_HERO_FIGHTER){
        return 211;
    }
    if(unit == UNIT_HERO_BEAR){
        return 211;
    }
    if(unit == UNIT_HERO_HEALER){
        return 11;
    }
    if(unit == UNIT_HERO_KNIGHT){
        return 211;
    }
    if(unit == UNIT_HERO_ELF_SWORDMAN){
        return 211;
    }
    if(unit == UNIT_HERO_ASSASSIN){
        return 23;
    }
    if(unit == UNIT_HERO_LION){
        return 23;
    }
    if(unit == UNIT_HERO_WIZARD){
        return 14;
    }
    if(unit == UNIT_HERO_TANKER){
        return 63;
    }
    if(unit == UNIT_HERO_SKELETON){
        return 525;
    }
    if(unit == UNIT_HERO_REAPER){
        return 14;
    }
    if(unit == UNIT_HERO_ENT){
        return 10;
    }
    if(unit == UNIT_HERO_SANTA){
        return 7;
    }
    if(unit == UNIT_HERO_RUDOLPH){
        return 14;
    }
    if(unit == UNIT_HERO_SANTADOG){
        return 14;
    }
    if(unit == UNIT_HERO_SALAMANDER){
        return 14;
    }
    if(unit == UNIT_HERO_UNDINE){
        return 9;
    }
    if(unit == UNIT_HERO_CRAZY_WEREWOLF){
        return 14;
    }
    if(unit == UNIT_HERO_CRAZY_BEAR){
        return 14;
    }
    if(unit == UNIT_HERO_CRAZY_LION){
        return 14;
    }
    if(unit == UNIT_HERO_LADY_WEREWOLF){
        return 23;
    }
    if(unit == UNIT_HERO_LADY_LION){
        return 23;
    }
    if(unit == UNIT_HERO_LADY_BEAR){
        return 23;
    }
    return 500;
}
std::vector<int> HeroPage::getPickPossibleHeroes(){
    std::vector<int> list;
    list.push_back(UNIT_HERO_ORC);
    list.push_back(UNIT_HERO_GOBLIN);
    list.push_back(UNIT_HERO_SPEARMAN);
    list.push_back(UNIT_HERO_LIZARDMAN);
    list.push_back(UNIT_HERO_ARCHER);
    list.push_back(UNIT_HERO_WEREWOLF);
    list.push_back(UNIT_HERO_MONK);
    list.push_back(UNIT_HERO_FIGHTER);
    list.push_back(UNIT_HERO_BEAR);
    list.push_back(UNIT_HERO_HEALER);
    list.push_back(UNIT_HERO_KNIGHT);
    list.push_back(UNIT_HERO_ELF_SWORDMAN);
    list.push_back(UNIT_HERO_ASSASSIN);
    list.push_back(UNIT_HERO_LION);
    list.push_back(UNIT_HERO_WIZARD);
    list.push_back(UNIT_HERO_TANKER);
    list.push_back(UNIT_HERO_SKELETON);
    list.push_back(UNIT_HERO_REAPER);
//    list.push_back(UNIT_HERO_ENT);
//    list.push_back(UNIT_HERO_SALAMANDER);
//    list.push_back(UNIT_HERO_UNDINE);
//    list.push_back(UNIT_HERO_CRAZY_WEREWOLF);
//    list.push_back(UNIT_HERO_CRAZY_BEAR);
//    list.push_back(UNIT_HERO_CRAZY_LION);
//    list.push_back(UNIT_HERO_LADY_WEREWOLF);
//    list.push_back(UNIT_HERO_LADY_LION);
//    list.push_back(UNIT_HERO_LADY_BEAR);
    return list;
}
Color3B HeroPage::getRankColor(int rank){
    return GM->getRankColor(rank);
}
void HeroPage::onHeroWoodChestGachaAgainClick(){
    int woodChestPrice = 60 - 10;
    if(GM->getGem() < woodChestPrice){
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
    
    closePopup();
    GM->addGem(-woodChestPrice);
    pickHero(0, 2);
    
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GEM);
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    updateHeroes();
}
void HeroPage::onHeroGoldChestGachaAgainClick(){
    int goldChestPrice = 300 - 10;
    if(GM->getGem() < goldChestPrice){
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
    
    closePopup();
    GM->addGem(-goldChestPrice);
    pickHero(2, 4);
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GEM);
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    updateHeroes();
}
void HeroPage::updateHeroInventorySaveData(){
    std::string strInventory = "_";
    for(auto unitInfo: unitInfoListHeroInventory){
        strInventory += strmake("%d/%d_",unitInfo->unitType, unitInfo->level + unitInfo->rank*100);
    }
    
    UDSetStr(KEY_UNITS_HERO_INVENTORY, strInventory + "");
    GM->saveHeroInventory(unitInfoListHeroInventory);
}
void HeroPage::updateHeroDeckSaveData(){
    std::string strDeck = "_";
    for(auto unitInfo: unitInfoListHeroDeck){
        strDeck += strmake("%d/%d/%d/%d_",unitInfo->unitType, unitInfo->level + unitInfo->rank*100, (int)unitInfo->x, (int)unitInfo->y);
    }
    
    UDSetStr(KEY_UNITS_HERO_DECK, strDeck + "");
    GM->saveHeroDeck(unitInfoListHeroDeck);
}
void HeroPage::onHeroShopClick(){
    if(this->getChildByName("shopLayer") != nullptr){
        return;
    }
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    ShopLayer* shopLayer = ShopLayer::create();
    this->addChild(shopLayer);
    this->getChildByName("heroPage")->setVisible(true);
    setAsPopup(shopLayer);
    shopLayer->hideTab(0);
    shopLayer->onShopTabClick(shopLayer->getChildByName("shopLayer")->getChildByName(strmake("btnTab%d", 2)));
}
void HeroPage::checkChangesForHeroes(){
    if(shouldSaveHeroDeckInfo || shouldSaveHeroInventoryInfo){
        std::vector<int> datas;
        if(shouldSaveHeroDeckInfo){
            shouldSaveHeroDeckInfo = false;
            datas.push_back(DATA_TYPE_HERO_DECK);
        }
        if (shouldSaveHeroInventoryInfo) {
            shouldSaveHeroInventoryInfo = false;
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
        }
        BSM->saveUserData(datas);
    }
}

