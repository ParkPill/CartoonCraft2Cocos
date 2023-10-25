//
//  ShopLayer.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#include "ShopLayer.h"
#include "HeroPage.h"
#include "TopBar.h"
#include "BuggyServerManager.h"
#include "LanguageManager.h"
#include "BattleHud.h"
#include "Title.h"
bool ShopLayer::init()
{
    if ( !PageBase::init())
    {
        return false;
    }
    
    size = Director::getInstance()->getWinSize();
    this->setName("shopLayer");
    
    Node* layer = CSLoader::createNodeWithVisibleSize("Shop.csb");
    ndShopLayer = layer;
    this->addChild(layer, 4);
    layer->setName("shopLayer");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    layer->getChildByName("imgBackground")->setLocalZOrder(-100);
    
    //    moveTopBarTo(layer);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::closeShop, this));
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    btn->setPositionX(btn->getPositionX() - 100);
    
    TopBar* topBar = TopBar::create();
    layer->addChild(topBar);
    topBar->setPosition(btn->getPosition() + Vec2(-98, 3));
    
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(ShopLayer::onShopTabClick, this));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(100);
    }
    onShopTabClick(layer->getChildByName(strmake("btnTab%d", 1)));
    this->schedule(schedule_selector(ShopLayer::update), 0.1f);
    
    if (GM->market == MARKET_SMARTPASS) {
        this->hideTab(2);
    }
    midMonthHeroType = GM->getMidMonthHeroType();
    log("init done");
    return true;
}
void ShopLayer::hideTab(int tab){
    ndShopLayer->getChildByName(strmake("btnTab%d", tab))->setVisible(false);
    ndShopLayer->getChildByName(strmake("imgTabIcon%d", tab))->setVisible(false);
}
void ShopLayer::update(float dt){
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->consumeSkuNameList.push_back(GM->iapDetail);
        log("consumeSkuNameList push_back: %d", (int)GM->consumeSkuNameList.size());
    }
    bool closeRequired = false;
    if(GM->consumeSkuNameList.size() > 0){
        // remove sku ID from consomeSkuNameList
        for (int i = 0; i < GM->consumeSkuNameList.size(); i++) {
            //            if(GM->consumeSkuNameList.at(i).compare(GM->skuID) == 0){
            GM->iapDetail = GM->consumeSkuNameList.at(i);
            GM->consumeSkuNameList.erase(GM->consumeSkuNameList.begin() + i);
            break;
            //            }
        }
        
        hideIndicator();
        log("iap success gem!");
        
        std::vector<int> datas;
        int iapPoint = UDGetInt(KEY_IAP_TOTAL, 0);
        std::string iapList = UDGetStr(KEY_IAP_LIST, "");
        
        if (GM->iapDetail.compare(IAP_DETAIL_STARTER_KEY) == 0) {
            GM->addGem(1000);
            GM->addWoodKey(5);
            GM->addGoldKey(5);
            iapPoint += 10000;
            closeRequired = true;
        }else if (GM->iapDetail.compare(IAP_DETAIL_EVENT_HERO_PACKAGE) == 0) {
            GM->addGem(1000);
            GM->addWoodKey(3);
            GM->addGoldKey(3);
            int monthlyBoughtCount = getEventHeroBoughtCount();
            monthlyBoughtCount++;
            int count = UDGetInt(KEY_EVENT_ITEM_BOUGHT_COUNT, 0);
            count++;
            UDSetInt(KEY_EVENT_ITEM_BOUGHT_COUNT, count);
            ScrollView* sv = (ScrollView*)ndShopLayer->getChildByName("sv");
            Button* btn = (Button*)sv->getChildByName("btnEventHeroPackage");
            if(monthlyBoughtCount < 2){
                Text* lbl = (Text*)btn->getChildByName("lblDesc");
                LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/2)", LM->getText("two time buy for the account").c_str(), getEventHeroBoughtCount()));
            }else{
                btn->setVisible(false);
            }
            
            std::string strInventory = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
            int heroType = GM->getMonthlyHeroType();
            strInventory += strmake("%d/%d_", heroType, 200);
            strInventory += strmake("%d/%d_", heroType, 200);
            UDSetStr(KEY_UNITS_HERO_INVENTORY, strInventory);
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
            std::vector<UnitInfo*> list = GM->getHeroInventory();
            UnitInfo* info = new UnitInfo();
            info->unitType = heroType;
            info->rank = 2;
            list.push_back(info);
            info = new UnitInfo();
            info->unitType = heroType;
            info->rank = 2;
            list.push_back(info);
            GM->saveHeroInventory(list);
            
            heroesAreChanged = true;
            iapPoint += 8900;
            closeRequired = true;
        }else if (GM->iapDetail.compare(IAP_DETAIL_MID_MONTH_PACKAGE) == 0) {
            GM->addGem(264);
            GM->addWoodKey(2);
            GM->addGoldKey(1);
            int monthlyBoughtCount = getEventHeroBoughtCount();
            monthlyBoughtCount++;
            int count = getEventHeroSetBoughtCount();
            count++;
            UDSetInt(KEY_MID_MONTH_ITEM_BOUGHT_COUNT, count);
            ScrollView* sv = (ScrollView*)ndShopLayer->getChildByName("sv");
            Button* btn = (Button*)sv->getChildByName("btnEventHeroSet");
            Text* lbl = (Text*)btn->getChildByName("lblDesc");
            LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/3)", LM->getText("n time buy for day").c_str(), getEventHeroBoughtCount()));
            int heroType = GM->getMidMonthHeroType();
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
            std::vector<UnitInfo*> list = GM->getHeroInventory();
            for (int i = 0; i < 3; i++) {
                UnitInfo* info = new UnitInfo();
                info->unitType = heroType;
                info->rank = 0;
                list.push_back(info);
            }
            GM->saveHeroInventory(list);
            heroesAreChanged = true;
            iapPoint += 3900;
            closeRequired = true;
        }else if (GM->iapDetail.compare(IAP_DETAIL_MARTIAL_ART_PACKAGE) == 0) {
            GM->addGem(1000);
            GM->addWoodKey(3);
            GM->addGoldKey(3);
            
            std::string strInventory = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
            strInventory += strmake("%d/%d_", UNIT_HERO_FIGHTER, 0 + 3*100);
            strInventory += strmake("%d/%d_", UNIT_HERO_MONK, 0 + 3*100);
            UDSetStr(KEY_UNITS_HERO_INVENTORY, strInventory);
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
            
            std::vector<UnitInfo*> list = GM->getHeroInventory();
            UnitInfo* info = new UnitInfo();
            info->unitType = UNIT_HERO_FIGHTER;
            info->rank = 3;
            list.push_back(info);
            info = new UnitInfo();
            info->unitType = UNIT_HERO_MONK;
            info->rank = 3;
            list.push_back(info);
            GM->saveHeroInventory(list);
            
            heroesAreChanged = true;
            iapPoint += 10000;
            closeRequired = true;
        }else if (GM->iapDetail.compare(IAP_DETAIL_CLASSIC_WAR_PACKAGE) == 0) {
            GM->addGem(1000);
            GM->addWoodKey(2);
            GM->addGoldKey(2);
            
            std::string strInventory = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
            strInventory += strmake("%d/%d_", UNIT_HERO_ARCHER, 0 + 3*100);
            strInventory += strmake("%d/%d_", UNIT_HERO_KNIGHT, 0 + 3*100);
            strInventory += strmake("%d/%d_", UNIT_HERO_SPEARMAN, 0 + 3*100);
            UDSetStr(KEY_UNITS_HERO_INVENTORY, strInventory);
            datas.push_back(DATA_TYPE_HERO_INVENTORY);
            heroesAreChanged = true;
            
            std::vector<UnitInfo*> list = GM->getHeroInventory();
            UnitInfo* info = new UnitInfo();
            info->unitType = UNIT_HERO_ARCHER;
            info->rank = 3;
            list.push_back(info);
            info = new UnitInfo();
            info->unitType = UNIT_HERO_KNIGHT;
            info->rank = 3;
            list.push_back(info);
            info = new UnitInfo();
            info->unitType = UNIT_HERO_SPEARMAN;
            info->rank = 3;
            list.push_back(info);
            GM->saveHeroInventory(list);
            
            iapPoint += 10000;
            closeRequired = true;
        }else if (GM->iapDetail.compare(IAP_DETAIL_CREW_MANAGER_PACKAGE) == 0) {
            GM->addGem(1500);
            GM->addWoodKey(5);
            GM->addGoldKey(5);
            iapPoint += 20000;
            closeRequired = true;
            
            time_t now = BSM->getCurrentTimeT();
            UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, BSM->getStrFromTime(now));
        }else if (GM->iapDetail.compare(IAP_DETAIL_MASTER_CREW_PACKAGE) == 0) {
            GM->addGem(5000);
            GM->addWoodKey(10);
            GM->addGoldKey(10);
            iapPoint += 50000;
            closeRequired = true;
            time_t now = BSM->getCurrentTimeT();
            UDSetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, BSM->getStrFromTime(now));
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_3) == 0) {
            GM->addGem(180);
            iapPoint += 3000;
            if (iapList.find(GM->iapDetail) == std::string::npos) {
                GM->addGem(180);
                closeRequired = true;
            }
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_10) == 0) {
            GM->addGem(660);
            iapPoint += 10000;
            if (iapList.find(GM->iapDetail) == std::string::npos) {
                GM->addGem(660);
                closeRequired = true;
            }
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_50) == 0) {
            GM->addGem(3600);
            iapPoint += 50000;
            if (iapList.find(GM->iapDetail) == std::string::npos) {
                GM->addGem(3600);
                closeRequired = true;
            }
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_100) == 0) {
            GM->addGem(9000);
            iapPoint += 100000;
            if (iapList.find(GM->iapDetail) == std::string::npos) {
                GM->addGem(9000);
                closeRequired = true;
            }
        }
        iapList += GM->iapDetail + "|";
        UDSetStr(KEY_IAP_LIST, iapList);
        UDSetInt(KEY_IAP_TOTAL, iapPoint);
        
        datas.push_back(DATA_TYPE_KEYS);
        datas.push_back(DATA_TYPE_IAP);
        datas.push_back(DATA_TYPE_GEM);
        BSM->saveUserData(datas);
        showInstanceMessage(LM->getText("purchase success"));
    }else if(GM->iapFlag == IAP_FLAG_FAILED){
        GM->iapFlag = IAP_FLAG_INIT;
        log("iap failed in BHUD!");
        hideIndicator();
    }
    
    // consume after give reward
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->iapFlag = IAP_FLAG_INIT;
        GameSharing::consumePurchased();
    }
    
    if(closeRequired){
        if (this->getParent()->getName().compare("heroPage") == 0) {
            HeroPage* layer = (HeroPage*)this->getParent();
            layer->updateHeroes();
        }
        this->unschedule(schedule_selector(ShopLayer::update));
        closeThis();
        return;
    }
    ScrollView* sv = (ScrollView*)ndShopLayer->getChildByName("sv");
    if (sv != nullptr) {
        Button* btn = (Button*)sv->getChildByName("btnEventHeroPackage");
        if(btn != nullptr){
            Text* lbl = (Text*)btn->getChildByName("lblTimeLeft");
            time_t now = BSM->getCurrentTimeT();
            int timeLeft = 86400 - (long)now%86400;
            
            int month = BSM->getMonth();
            int day = BSM->getDay();
            int lastDay = BSM->getLastDayOfMonth(month);
            timeLeft += (lastDay - day)*(60*60*24);
            lbl->setString(strmake("%s", GM->getTimeLeftInString(timeLeft).c_str()));
        }
        
        btn = (Button*)sv->getChildByName("btnEventHeroSet");
        if(btn != nullptr){
            Text* lbl = (Text*)btn->getChildByName("lblTimeLeft");
            int timeLeft = GM->getMidMonthHeroTimeLeft();
            lbl->setString(strmake("%s", GM->getTimeLeftInString(timeLeft).c_str()));
        }
    }
}
void ShopLayer::onShopTabClick(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("shopLayer");
    currentTab = btn->getTag();
    Node* tabBack = layer->getChildByName("tabBack");
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->setContentSize(i == currentTab?cocos2d::Size(250.35f, 237.16f):cocos2d::Size(250.35f, 199.01));
        btn->setColor(i == currentTab?Color3B::WHITE:Color3B::GRAY);
        btn->setLocalZOrder(tabBack->getLocalZOrder() + (i == currentTab?1:-1));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(btn->getLocalZOrder());
    }
    
    Button* btnTemp;Text* lbl;ImageView* img;
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->removeAllChildren();
    int x = 487.93f;
    int gapX = 740.45f;
    int y = 429.55f;
    if (currentTab == 0) {
        btnTemp = (Button*)layer->getChildByName("btnBuildingTemp");
        for (int i = BUILDING_MINE; i < BUILDING_TOTAL; i++) {
            btn = (Button*)btnTemp->clone();
            btn->addClickEventListener(CC_CALLBACK_1(ShopLayer::onBuyBuilding, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            btn->setTag(i);
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            PPLabel* lblPP = BHUD->replaceTextToPPLabel(lbl);
            lblPP->setString(LM->getText(BHUD->getBuildingName(i)));
            lblPP->setWidth(btn->getContentSize().width*0.8f);
            
            int currentBuildingCount =  BHUD->getCurrentBuildingCount(i);
            int maxBuildingCount =  BHUD->getMaxBuildingCount(i);
            ImageView* sptBuilding = ImageView::create(BHUD->getBuildingSpriteName(i), ui::Widget::TextureResType::PLIST);
            sptBuilding->setScale(1/WORLD->imageScale);
            img = (ImageView*)btn->getChildByName("img");
            sptBuilding->setPosition(img->getPosition());
            btn->addChild(sptBuilding);
            img->removeFromParent();
            
            lbl = (Text*)btn->getChildByName("lblCount");
            lbl->setString(strmake("%d/%d",currentBuildingCount, maxBuildingCount));
            btn->setEnabled(currentBuildingCount < maxBuildingCount);
            lbl->setTextColor(currentBuildingCount >= maxBuildingCount?Color4B::RED:Color4B::WHITE);
            
            int goldPrice = BHUD->getBuildingPriceGold(i);
            lbl = (Text*)btn->getChildByName("lblGold");
            lbl->setString(Value(goldPrice).asString());
            lbl->setTextColor(GM->getCoin() < goldPrice?Color4B::RED:Color4B::WHITE);
            
            int treePrice = BHUD->getBuildingPriceTree(i);
            lbl = (Text*)btn->getChildByName("lblTree");
            lbl->setString(Value(treePrice).asString());
            lbl->setTextColor(GM->getTree() < treePrice?Color4B::RED:Color4B::WHITE);
            
            if(GM->getCoin() < goldPrice || GM->getTree() < treePrice){
                btn->setEnabled(false);
            }
            sptBuilding->setGrayScale(!btn->isEnabled());
            if(i >= BUILDING_ORC_HQ){
                PPLabel* lblComing = PPLabel::create("Coming soon!", 60, Color3B::ORANGE, false, true, TextHAlignment::CENTER, false);
                btn->addChild(lblComing);
                lblComing->setPosition(sptBuilding->getPosition());
            }
        }
    }else if (currentTab == 1) {
        for (int i = 0; i < 3; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnGold%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(ShopLayer::onBuyGold, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "gold text");
            int perHour = getMineGoldPerHour(UDGetInt(KEY_CASTLE_LEVEL, 0));
            lbl = (Text*)btn->getChildByName("lblAmount");
            int amount = perHour*(i*2 + 1);
            lbl->setString(Value(amount).asString());
            GM->alignToCenter(btn->getChildByName("imgGold"), lbl, 50, btn->getContentSize().width/2, 0);
            lbl = (Text*)btn->getChildByName("lblGem");
            lbl->setString(Value(GM->getGemForHour()*(i*2 + 1)/4).asString());
        }
        for (int i = 0; i < 3; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnTree%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(ShopLayer::onBuyTree, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "tree");
            int perHour = 40*60/10;
            lbl = (Text*)btn->getChildByName("lblAmount");
            int amount = perHour*(i*2 + 1);
            lbl->setString(Value(amount).asString());
            GM->alignToCenter(btn->getChildByName("imgTree"), lbl, 50, btn->getContentSize().width/2, 0);
            lbl = (Text*)btn->getChildByName("lblGem");
            lbl->setString(Value(GM->getGemForHour()*(i*2 + 1)/8).asString());
        }
    }else if (currentTab == 2) {
        std::string iapList = UDGetStr(KEY_IAP_LIST, "");
        int monthlyBoughtCount = getEventHeroBoughtCount();
        int heroType = GM->getMonthlyHeroType();
        if(monthlyBoughtCount < 2 && heroType >= 0){
            Node* child = layer->getChildByName("btnEventHeroPackageTemp");
            btn = (Button*)((Button*)child)->clone();
            btn->setName("btnEventHeroPackage");
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyEventHeroPackage, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            GM->addYellowRisingBallEffect(btn);
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "event hero package");
            lbl->setLocalZOrder(1);
            lbl = (Text*)btn->getChildByName("lblTimeLeft");
            lbl->setLocalZOrder(1);
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_EVENT_HERO_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
            lbl = (Text*)btn->getChildByName("lblDesc");
            LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/2)", LM->getText("two time buy for the account").c_str(), getEventHeroBoughtCount()));
            
            spine::SkeletonAnimation* spChar = GM->getHeroSpine(heroType);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(243.95f, 335.0f));
            spChar = GM->getHeroSpine(heroType);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(493.15f, 335.0f));
        }
        int midMonthBoughtCount = getEventHeroSetBoughtCount(); 
        heroType = midMonthHeroType;
        if(midMonthHeroType > 0 && heroType >= 0){
            Node* child = layer->getChildByName("btnEventHeroSetTemp");
            btn = (Button*)((Button*)child)->clone();
            btn->setName("btnEventHeroSet");
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyMidMonthEventHeroSet, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            GM->addYellowRisingBallEffect(btn);
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "event hero package");
            lbl->setLocalZOrder(1);
            lbl = (Text*)btn->getChildByName("lblTimeLeft");
            lbl->setLocalZOrder(1);
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_MID_MONTH_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
            lbl = (Text*)btn->getChildByName("lblDesc");
            std::string strLimit = LM->getText("n time buy for day");
            strLimit = strLimit.replace(strLimit.find("7"), 1, "3");
            LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/3)", strLimit.c_str(), getEventHeroBoughtCount()));
            btn->setEnabled(midMonthBoughtCount < 3);
            
            for (int i = 0; i < 3; i++) {
                spine::SkeletonAnimation* spChar = GM->getHeroSpine(heroType);
                btn->addChild(spChar);
                spChar->setAnimation(0, "idle", true);
                spChar->setPosition(btn->getChildByName(strmake("imgHero%d", i))->getPosition()+Vec2(0, -70));
            }
        }
        if(iapList.find(IAP_DETAIL_STARTER_KEY) == std::string::npos){
            Node* child = layer->getChildByName("btnStarterKeys");
            btn = (Button*)((Button*)child)->clone();
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyStartKeys, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "starter pack");
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale("cc_starterkey"));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
        }
        if(iapList.find(IAP_DETAIL_MARTIAL_ART_PACKAGE) == std::string::npos){
            Node* child = layer->getChildByName("btnMartialArtPackage");
            btn = (Button*)((Button*)child)->clone();
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyMartialArtPackage, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "martial art package");
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_MARTIAL_ART_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
            
            lbl = (Text*)btn->getChildByName("lblRank0");
            LM->setLocalizedString(lbl, "rare hero");
            lbl = (Text*)btn->getChildByName("lblRank1");
            LM->setLocalizedString(lbl, "rare hero");
            spine::SkeletonAnimation* spChar = GM->getHeroSpine(UNIT_HERO_FIGHTER);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(217.95f, 410.0f));
            spChar = GM->getHeroSpine(UNIT_HERO_MONK);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(467.15f, 410.0f));
        }
        if(iapList.find(IAP_DETAIL_CLASSIC_WAR_PACKAGE) == std::string::npos){
            Node* child = layer->getChildByName("btnClassicWarPackage");
            btn = (Button*)((Button*)child)->clone();
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyClassicWarPackage, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "classic war package");
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_CLASSIC_WAR_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
            
            lbl = (Text*)btn->getChildByName("lblRank0");
            LM->setLocalizedString(lbl, "rare hero");
            lbl = (Text*)btn->getChildByName("lblRank1");
            LM->setLocalizedString(lbl, "rare hero");
            lbl = (Text*)btn->getChildByName("lblRank2");
            LM->setLocalizedString(lbl, "rare hero");
            spine::SkeletonAnimation* spChar = GM->getHeroSpine(UNIT_HERO_KNIGHT);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(141.32f, 410.0f));
            spChar = GM->getHeroSpine(UNIT_HERO_SPEARMAN);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(338.90f, 410.0f));
            spChar = GM->getHeroSpine(UNIT_HERO_ARCHER);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(Vec2(536.48f, 410.0f));
        }
        if(iapList.find(IAP_DETAIL_CREW_MANAGER_PACKAGE) == std::string::npos){
            Node* child = layer->getChildByName("btnCrewManager");
            btn = (Button*)((Button*)child)->clone();
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyCrewManagerPackage, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            GM->addYellowRisingBallEffect(btn);
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "crew manager package");
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_CREW_MANAGER_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
        }
        if(iapList.find(IAP_DETAIL_MASTER_CREW_PACKAGE) == std::string::npos){
            Node* child = layer->getChildByName("btnMasterCrew");
            btn = (Button*)((Button*)child)->clone();
            btn->addClickEventListener(CC_CALLBACK_0(ShopLayer::onBuyMasterCrewPackage, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            GM->addYellowRisingBallEffect(btn);
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "master crew package");
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_MASTER_CREW_PACKAGE));
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
        }
        
        for (int i = 0; i < 4; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnGem%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(ShopLayer::onBuyGem, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            bool firstBonusAvailable = true;
            if ((i == 0 && iapList.find(IAP_DETAIL_GEM_3) != std::string::npos) ||
                (i == 1 && iapList.find(IAP_DETAIL_GEM_10) != std::string::npos) ||
                (i == 2 && iapList.find(IAP_DETAIL_GEM_50) != std::string::npos) ||
                (i == 3 && iapList.find(IAP_DETAIL_GEM_100) != std::string::npos)){
                firstBonusAvailable = false;
            }
            btn->getChildByName("imgFirstBonusBack")->setVisible(firstBonusAvailable);
            lbl = (Text*)btn->getChildByName("lblFirstBonus");
            std::string str = LM->getText("first purchase bonus");
            if(str.length() > 3){
                LM->setLocalizedStringNotKey(lbl, (str.replace(str.find("-77"), 3, ((Text*)btn->getChildByName("lblGemCount"))->getString())));
                lbl->setVisible(firstBonusAvailable);
            }
            if (GM->market == MARKET_SMARTPASS) {
                btn->setVisible(false);
                x -= gapX;
            }
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            //            PPLabel* lblPP = replaceTextToPPLabel(lbl);
            
            LM->setLocalizedString(lbl, strmake("%s %d", LM->getText("gem pack").c_str(), i+1));
            //            lblPP->setWidth(btn->getContentSize().width*0.8f);
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            lbl->setFontSize(lbl->getFontSize());
            if(i == 0){
                lbl->setString(GameSharing::getPriceLocale("cc_gem3"));
            }else if(i == 1){
                lbl->setString(GameSharing::getPriceLocale("cc_gem10"));
            }else if(i == 2){
                lbl->setString(GameSharing::getPriceLocale("cc_gem50"));
            }else if(i == 3){
                lbl->setString(GameSharing::getPriceLocale("cc_gem100"));
            }
            if(lbl->getFontName().compare("Helveticas") == 0) lbl->setTextColor(Color4B::BLACK);
        }
    }
//    x -= gapX*4/5;
    float maxX = 0;
    for(auto item : sv->getChildren()){
        if(item->getPositionX() > maxX){
            maxX = item->getPositionX() + item->getBoundingBox().size.width/2;
        }
    }
    sv->setInnerContainerSize(cocos2d::Size(maxX, sv->getContentSize().height));
    sv->setPositionX(0);
    sv->setContentSize(cocos2d::Size(size.width, sv->getContentSize().height));
}
int ShopLayer::getEventHeroBoughtCount(){
    int monthlyBoughtCount = 0;
    std::string strKey = "lastEventItemMonth";
    int lastEventMonth = UDGetInt(strKey.c_str(), 0);
    int month = BSM->getMonth();
    if (lastEventMonth != month) {
        UDSetInt(strKey.c_str(), month);
        UDSetInt(KEY_EVENT_ITEM_BOUGHT_COUNT, 0);
    }
    monthlyBoughtCount = UDGetInt(KEY_EVENT_ITEM_BOUGHT_COUNT, 0);
    
//    std::string iapList = UDGetStr(KEY_IAP_LIST, "");
//    string::size_type pos = iapList.find( IAP_DETAIL_EVENT_HERO_PACKAGE, 0 );
//    if(pos != std::string::npos){
//        string::size_type pos2 = iapList.find( IAP_DETAIL_EVENT_HERO_PACKAGE, pos + 24 );
//        monthlyBoughtCount++;
//        if (pos2 != std::string::npos) {
//            monthlyBoughtCount++;
//        }
//    }
    return monthlyBoughtCount;
}

int ShopLayer::getEventHeroSetBoughtCount(){
    const char* strWhatDay = "lastEventSetItemMonthWhatDay";
    int lastDay = UDGetInt(strWhatDay, 0);
    int today = BSM->getDay();
    if (lastDay != today) {
        UDSetInt(KEY_MID_MONTH_ITEM_BOUGHT_COUNT, 0);
        UDSetInt(strWhatDay, today);
        return 0;
    }else{
        return UDGetInt(KEY_MID_MONTH_ITEM_BOUGHT_COUNT, 0);
    }
}
int ShopLayer::getMineGoldPerHour(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["mine"].asString();
    return Value(GM->split(str, "_").at(0)).asInt();
}
void ShopLayer::onGoldClickFromTopBar(){
    int tab = 1;
    if(currentTab != tab){
        onShopTabClick(ndShopLayer->getChildByName(strmake("btnTab%d", tab)));
    }
    ScrollView* sv = (ScrollView*)ndShopLayer->getChildByName("sv");
    sv->getInnerContainer()->runAction(EaseOut::create(MoveTo::create(0.3f, Vec2(0, 0)), 2));
}
void ShopLayer::onTreeClickFromTopBar(){
    int tab = 1;
    if(currentTab != tab){
        onShopTabClick(ndShopLayer->getChildByName(strmake("btnTab%d", tab)));
    }
    ScrollView* sv = (ScrollView*)ndShopLayer->getChildByName("sv");
    float innerWidth = sv->getInnerContainerSize().width;
    float targetX = -2300;
    if(-targetX > innerWidth - sv->getContentSize().width){
        targetX = -(innerWidth - sv->getContentSize().width);
    }
    sv->getInnerContainer()->runAction(EaseOut::create(MoveTo::create(0.3f, Vec2(targetX, 0)), 2));
}
void ShopLayer::onGemClickFromTopBar(){
    int tab = 2;
    if(currentTab != tab){
        onShopTabClick(ndShopLayer->getChildByName(strmake("btnTab%d", tab)));
    }
}

void ShopLayer::onBuyStartKeys(){
    GameSharing::buyItem(IAP_DETAIL_STARTER_KEY);
    showIndicator();
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    //    GM->iapFlag = IAP_FLAG_SUCCESS; // test
#endif
}
void ShopLayer::onBuyMartialArtPackage(){
    GameSharing::buyItem(IAP_DETAIL_MARTIAL_ART_PACKAGE);
    showIndicator();
}
void ShopLayer::onBuyClassicWarPackage(){
    GameSharing::buyItem(IAP_DETAIL_CLASSIC_WAR_PACKAGE);
    showIndicator();
}
void ShopLayer::onBuyCrewManagerPackage(){
    GameSharing::buyItem(IAP_DETAIL_CREW_MANAGER_PACKAGE);
    showIndicator();
}
void ShopLayer::onBuyMasterCrewPackage(){
    GameSharing::buyItem(IAP_DETAIL_MASTER_CREW_PACKAGE);
    showIndicator();
}
void ShopLayer::onBuyEventHeroPackage(){
    GameSharing::buyItem(IAP_DETAIL_EVENT_HERO_PACKAGE);
    showIndicator();
//    GM->consumeSkuNameList.push_back(IAP_DETAIL_EVENT_HERO_PACKAGE); // test
//    GM->iapDetail.compare(IAP_DETAIL_EVENT_HERO_PACKAGE); // test
}
void ShopLayer::onBuyMidMonthEventHeroSet(){
    GameSharing::buyItem(IAP_DETAIL_MID_MONTH_PACKAGE);
    showIndicator();
}
void ShopLayer::onBuyTree(Ref* ref){
    BTN_FROM_REF
    Text* lbl = (Text*)btn->getChildByName("lblGem");
    int gem = Value(lbl->getString()).asInt();
    lbl = (Text*)btn->getChildByName("lblAmount");
    int amount = Value(lbl->getString()).asInt();
    if (GM->getGem() >= gem) {
        GM->addGem(-gem);
        GM->addTree(amount);
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GEM);
    datas.push_back(DATA_TYPE_TREE);
    BSM->saveUserData(datas);
}
void ShopLayer::onBuyGold(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    Text* lbl = (Text*)btn->getChildByName("lblGem");
    int gem = Value(lbl->getString()).asInt();
    lbl = (Text*)btn->getChildByName("lblAmount");
    int amount = Value(lbl->getString()).asInt();
    if (GM->getGem() >= gem) {
        GM->addGem(-gem);
        GM->addCoin(amount);
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GEM);
    datas.push_back(DATA_TYPE_GOLD);
    BSM->saveUserData(datas);
}

void ShopLayer::onBuyGem(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
    
    if(index == 0){
        GameSharing::buyItem(IAP_DETAIL_GEM_3);
    }else if(index == 1){
        GameSharing::buyItem(IAP_DETAIL_GEM_10);
    }else if(index == 2){
        GameSharing::buyItem(IAP_DETAIL_GEM_50);
    }else if(index == 3){
        GameSharing::buyItem(IAP_DETAIL_GEM_100);
    }
    
    showIndicator();
}
void ShopLayer::onBuyBuilding(Ref* ref){
    BTN_FROM_REF
    if (!BHUD->isWorkerAvailable()) {
        BHUD->showNotEnoughWorkerUseGem();
        return;
    }
    int index = btn->getTag();
    cocos2d::Size occupySize;
    if (index == BUILDING_MINE || index == BUILDING_LUMBURMILL || index == BUILDING_BARRACKS || index == BUILDING_FACTORY || index == BUILDING_AIRPORT) {
        occupySize = cocos2d::Size(3, 3);
    }else if(index == BUILDING_TREE){
        occupySize = cocos2d::Size(1, 1);
    }else if(index == BUILDING_FARM){// || index == BUILDING_UNDERGROUND_BUNKER){
        occupySize = cocos2d::Size(3, 2);
    }else if(index == BUILDING_WATCHER_TOWER){
        occupySize = cocos2d::Size(2, 2);
    }
    //    else if(index == BUILDING_TRIGGER){
    //        occupySize = cocos2d::Size(1, 1);
    //    }
//    closePopup();
    closeShop();
    WORLD->createBuildingTemplate(BHUD->getUnitIndex(index), occupySize.width, occupySize.height, WORLD->getSpriteNameForUnit(BHUD->getUnitIndex(index)));
    BHUD->currentJob = JOB_MOVE_BUILDING_TEMPATE;
    BHUD->currentJobDetailIndex = index;
}
void ShopLayer::closeShop(){
    if(heroesAreChanged){
        if (GM->heroPage && GM->heroPage != nullptr) {
            HeroPage* layer = (HeroPage*)GM->heroPage;
            layer->updateHeroes();
        }
    }
    closeThis();
}
