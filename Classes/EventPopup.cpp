//
//  EventPopup.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 25/10/2019.
//

#include "EventPopup.h"
#include "BuggyServerManager.h"
#include "MonthlyEventPopup.h"
#include "LanguageManager.h"
#include "RoulettePopup.h"
bool EventPopup::init()
{
    //    UDSetInt(KEY_GOLDEN_TICKET, 0); // test
    //    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime() - 30); // test
    
    GM->setHudLayer(nullptr);
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    layer = CSLoader::createNode("EventPopup.csb");
    this->addChild(layer, 4);
    layer->setName("eventPopup");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(PageBase::closeThis, this));
    ImageView* img = (ImageView*)layer->getChildByName("imgBack");
    img->addClickEventListener(CC_CALLBACK_0(EventPopup::closeThis, this));
    Text* lbl = (Text*)layer->getChildByName("lblNoEvent");
    LM->setLocalizedString(lbl, "no event");
    
    month = BSM->getMonth();
    day = BSM->getDay();
    
    addMonthlyEvent();
    update(0);
    this->schedule(schedule_selector(EventPopup::update), 1);
    return true;
}
void EventPopup::addMonthlyEvent(){
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    Button *btn, *btnTemp;
    
    float y = 0;
    float x = 922;
    int gapY = 360;
    Text* lbl;
    if (GM->getMonthlyHeroType() > 0) {
        if(month == 11){
            btnTemp = (Button*)layer->getChildByName("btnEntTemp");
        }else if(month == 12){
            btnTemp = (Button*)layer->getChildByName("btnSantaTemp");
        }else if(month == 1){
            btnTemp = (Button*)layer->getChildByName("btnSalamanderTemp");
        }else{//} if(month == 2){
            btnTemp = (Button*)layer->getChildByName("btnSalamanderTemp");
        }
        btn = (Button*)btnTemp->clone();
        if(month == 2){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerUndine.png");
        }else if(month == 3){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerLadyWerewolf.png");
        }else if(month == 4){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerLadyBear.png");
        }else if(month == 5){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerLadyLion.png");
        }else if(month == 6){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerSavageArcher.png");
        }else if(month == 7){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerMemeat.png");
        }else if(month == 8){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerWatermelon.png");
        }else if(month == 9){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerMino.png");
        }else if(month == 10){
            ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
            img->loadTexture("eventBannerLamia.png");
        }
        
        btn->setName("btnMonthly");
        sv->addChild(btn);
        btn->setPosition(Vec2(x, y));
        log("btn1: %f, %f", btn->getPositionX(), btn->getPositionY());
        btn->addClickEventListener(CC_CALLBACK_0(EventPopup::onMonthlyEventClick, this));
        lbl = (Text*)btn->getChildByName("lblTitle");
        LM->setLocalizedString(lbl, GM->getUnitName(GM->getMonthlyHeroType()));
        
        lbl = (Text*)btn->getChildByName("lblTimeLeft");
        lbl->setString("");
        y -= gapY;
    }
//    if (month == 11){ // test
//
//    }else if (month == 12){ // test
//        btnTemp = (Button*)layer->getChildByName("btnSantaTemp");
//        btn = (Button*)btnTemp->clone();
//        btn->setName("btnMonthly");
//        sv->addChild(btn);
//        btn->setPosition(Vec2(x, y));
//        btn->addClickEventListener(CC_CALLBACK_0(EventPopup::onMonthlyEventClick, this));
//        lbl = (Text*)btn->getChildByName("lblTitle");
//        LM->setLocalizedString(lbl, "hero santa");
//
//        lbl = (Text*)btn->getChildByName("lblTimeLeft");
//        lbl->setString("");
//        y -= gapY;
//    }else if (month == 1){ // test
//        btnTemp = (Button*)layer->getChildByName("btnSalamanderTemp");
//        btn = (Button*)btnTemp->clone();
//        btn->setName("btnMonthly");
//        sv->addChild(btn);
//        btn->setPosition(Vec2(x, y));
//        btn->addClickEventListener(CC_CALLBACK_0(EventPopup::onMonthlyEventClick, this));
//        lbl = (Text*)btn->getChildByName("lblTitle");
//        LM->setLocalizedString(lbl, "hero salamander");
//
//        lbl = (Text*)btn->getChildByName("lblTimeLeft");
//        lbl->setString("");
//        y -= gapY;
//    }
    
    // extra event
    int heroType = GM->getMidMonthHeroType();
    if (heroType > 0) {
        btnTemp = (Button*)layer->getChildByName("btnRudolphTemp");
        btn = (Button*)btnTemp->clone();
        btn->setName("btnMidMonth");
        sv->addChild(btn);
        log("btn2: %f, %f", btn->getPositionX(), btn->getPositionY());
        btn->setPosition(Vec2(x, y));
        btn->addClickEventListener(CC_CALLBACK_0(EventPopup::onMidMonthEventClick, this));
        lbl = (Text*)btn->getChildByName("lblTitle");
        LM->setLocalizedString(lbl, GM->getUnitName(GM->getMidMonthHeroType()));
        
        std::string strFileName;
        if(heroType == UNIT_HERO_RUDOLPH){
            strFileName = "rudolphSprite.png";
        }else if(heroType == UNIT_HERO_SANTADOG){
            strFileName = "santadogSprite.png";
        }else if(heroType == UNIT_HERO_PENGUIN){
            strFileName = "penguinSprite.png";
        }else if(heroType == UNIT_HERO_CATINBOOTS){
            strFileName = "catinbootsSprite.png";
        }else if(heroType == UNIT_HERO_MOLE){
            strFileName = "moleSprite.png";
        }else if(heroType == UNIT_HERO_TOYMOUSE){
            strFileName = "toyMouseSprite.png";
        }else if(heroType == UNIT_HERO_BATMONSTER){
            strFileName = "batMonsterSprite.png";
        }else if(heroType == UNIT_HERO_PARASITE){
            strFileName = "parasiteSprite.png";
        }else if(heroType == UNIT_HERO_BABYMINO){
            strFileName = "babyminoSprite.png";
        }else if(heroType == UNIT_HERO_KERBEROS){
            strFileName = "kerberosSprite.png";
        }else if(heroType == UNIT_HERO_CHUNJA){
            strFileName = "chunjaSprite.png";
        }else if(heroType == UNIT_HERO_GOLEM){
            strFileName = "golemSprite.png";
        }
        Sprite* sptBigHero = Sprite::create(strFileName);
        btn->addChild(sptBigHero);
        sptBigHero->setPosition(200, 170);
        
        for (int i =0; i < 2; i++) {
            Sprite* sptHero = Sprite::create(strFileName);
            btn->addChild(sptHero);
            sptHero->setScale(0.2f);
            if (i == 0) {
                sptHero->setPosition(Vec2(454, 250));
            }else if (i == 1) {
                sptHero->setPosition(Vec2(460, 110));
                sptHero->setRotation(180);
            }
        }
        
        lbl = (Text*)btn->getChildByName("lblTimeLeft");
        lbl->setString("");
        y -= gapY;
    }
    
    int scrollHeight = sv->getContentSize().height;
    if (scrollHeight < -y) {
        scrollHeight = -y;
    }
    sv->setInnerContainerSize(cocos2d::Size(sv->getContentSize().width, scrollHeight));
    for(auto btn: sv->getChildren()){
        btn->setPositionY(btn->getPositionY() + scrollHeight - gapY/2);
        log("btn: %f, %f", btn->getPositionX(), btn->getPositionY());
    }
    
    lbl = (Text*)layer->getChildByName("lblNoEvent");
    lbl->setVisible(sv->getChildren().size() <= 0);
}

void EventPopup::onMonthlyEventClick(){
    MonthlyEventPopup* layer = MonthlyEventPopup::create();
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("monthlyEventPopup");
    setAsPopup(layer);
}
void EventPopup::onMidMonthEventClick(){
    RoulettePopup* layer = RoulettePopup::create();
    this->addChild(layer, POPUP_ZORDER);
    layer->setName("midMonthEventPopup");
    setAsPopup(layer);
}
void EventPopup::update(float dt){
    if (BSM->isOffline) {
        this->unschedule(schedule_selector(EventPopup::update));
        closePopup();
        closeThis();
        return;
    }
    int lastDay = BSM->getLastDayOfMonth(month);
    if (GM->getMonthlyHeroType() > 0){
        Button* btn = (Button*)layer->getChildByName("sv")->getChildByName("btnMonthly");
        Text* lbl = (Text*)btn->getChildByName("lblTimeLeft");
        time_t now = BSM->getCurrentTimeT();
        int timeLeft = 86400 - (long)now%86400;
        
        timeLeft += (lastDay - day)*(60*60*24);
        lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeft).c_str()));
        if (timeLeft < 0) {
            btn->setVisible(false);
        }
    }
    if (GM->getMidMonthHeroType() > 0) {
        Button* btn = (Button*)layer->getChildByName("sv")->getChildByName("btnMidMonth");
        Text* lbl = (Text*)btn->getChildByName("lblTimeLeft");
        time_t now = BSM->getCurrentTimeT();
        int timeLeft = 86400 - (long)now%86400;
        
        if (day > 15) {
            timeLeft += (lastDay - day + 14)*(60*60*24);
        }else{
            timeLeft += (15 - day)*(60*60*24);
        }
        
        lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeft).c_str()));
        if (timeLeft < 0) {
            btn->setVisible(false);
        }
    }
}
