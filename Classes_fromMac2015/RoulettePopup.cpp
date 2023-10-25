//
//  RoulettePopup.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 03/12/2019.
//

#include "RoulettePopup.h"
#include "BuggyServerManager.h"
#include "LanguageManager.h"
#include "GameManager.h"
#include "Title.h"
bool RoulettePopup::init()
{
    GM->setHudLayer(nullptr);
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    
    size = Director::getInstance()->getWinSize();
    
    month = BSM->month;
    day = BSM->day;
    
    std::string strName, strDesc;
    int heroType = GM->getMidMonthHeroType();
//    if ((month == 12 && day > 15) || (month == 1 && day < 15)) {
        layer = CSLoader::createNode("RudolphRoulette.csb");
        Node* nd;
        spine::SkeletonAnimation* spChar;
        layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
        for (int i = 0; i < 3; i++) {
            nd = layer->getChildByName(strmake("imgRudolph%d", i));
            spChar = GM->getHeroSpine(heroType);
            layer->addChild(spChar, 0);
            spChar->setPosition(nd->getPosition());
            nd->removeFromParent();
            if(i == 0){
                spChar->setScaleX(-1);
                spChar->setAnimation(0, "idle", true);
            }else if(i == 1){
                spChar->setAnimation(0, "attack", true);
            }else if(i == 2){
                spChar->setAnimation(0, "run", true);
            }
        }
        nd = layer->getChildByName("imgSanta");
        if(month > 11 || month < 3){
            spChar = GM->getHeroSpine(UNIT_HERO_SANTA);
            spChar->setScaleX(-1);
            layer->addChild(spChar, 0);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(nd->getPosition());
        }
        nd->removeFromParent();
        strName = LM->getText(GM->getUnitName(heroType));
        strDesc = LM->getText(strmake("%s desc", GM->getUnitName(heroType).c_str()));
//    }
    imgRoulette = (ImageView*)layer->getChildByName("imgRoulette");
    
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
    }
    for (int i =0; i < 2; i++) {
        Sprite* sptHero = Sprite::create(strFileName);
        imgRoulette->addChild(sptHero);
        sptHero->setScale(0.4f);
        if (i == 0) {
            sptHero->setPosition(Vec2(120, 160));
            sptHero->setRotation(220);
        }else if (i == 1) {
            sptHero->setPosition(Vec2(580, 330));
            sptHero->setRotation(90);
        }
    }
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, strName);
    lbl = (Text*)layer->getChildByName("lblName");
    LM->setLocalizedString(lbl, strName);
    lbl = (Text*)layer->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, strDesc);
    
    this->addChild(layer, 4);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(PageBase::closeThis, this));
    ImageView* img = (ImageView*)layer->getChildByName("imgBack");
    img->addClickEventListener(CC_CALLBACK_0(RoulettePopup::closeThis, this)); // test
    
    
    btn = (Button*)layer->getChildByName("btnTestLeft");
    btn->addTouchEventListener(CC_CALLBACK_2(RoulettePopup::onTestLeft, this));
    btn = (Button*)layer->getChildByName("btnTestRight");
    btn->addTouchEventListener(CC_CALLBACK_2(RoulettePopup::onTestRight, this));
    btn = (Button*)layer->getChildByName("btnRoll");
    btn->addClickEventListener(CC_CALLBACK_1(RoulettePopup::onRollClick, this));
    btn->setEnabled(false);
    btn = (Button*)layer->getChildByName("btnVideo");
    btn->addClickEventListener(CC_CALLBACK_1(RoulettePopup::onVideoClick, this));
    btn->setEnabled(false);
    
    btn = (Button*)layer->getChildByName("btnTestLeft");
    btn->removeFromParent();
    btn = (Button*)layer->getChildByName("btnTestRight");
    btn->removeFromParent();
    
    if (GM->market == MARKET_SMARTPASS) {
        layer->getChildByName("btnVideo")->setVisible(false);
        layer->getChildByName("lblVideoTime")->setVisible(false);
    }
    
    btn = (Button*)layer->getChildByName("btnRollForVideo");
    btn->addClickEventListener(CC_CALLBACK_0(RoulettePopup::rollTheRoulette, this));
    btn->setPosition(Vec2(1670.04f, -2200.55f));
    if(UDGetBool(KEY_ROULETTE_VIDEO_BUTTON_ENABLED, false)){
        btn->setPosition(Vec2(1670.04f, 220.55f));
    }
    
    this->schedule(schedule_selector(RoulettePopup::update));
    
//    UDSetInt(KEY_MID_MONTH_ROULETTE_ROLL_DAY, 0); // test
//    GM->saveTime(KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME, BSM->getCurrentTimeT() - 60*60*6); // test 
    // init done
    return true;
}
void RoulettePopup::onTestLeft(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::BEGAN) {
        isTestLeftPressed = true;
    }else if (type == Widget::TouchEventType::ENDED) {
        isTestLeftPressed = false;
    }
}
void RoulettePopup::onTestRight(Ref* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::BEGAN) {
        isTestRightPressed = true;
    }else if (type == Widget::TouchEventType::ENDED) {
        isTestRightPressed = false;
    }
}

void RoulettePopup::update(float dt){
    if(!this){
        return;
    }
    if (isTestLeftPressed || isTestRightPressed) {
        float angle = imgRoulette->getRotation();
        if(isTestLeftPressed){
            angle -= 0.5f;
        }else if(isTestRightPressed){
            angle += 0.5f;
        }
        imgRoulette->setRotation(angle);
        log("angle: %f", angle);
    }
    int lastDay = 30;
    if (month == 1 ||
        month == 3 ||
        month == 5 ||
        month == 7 ||
        month == 8 ||
        month == 10 ||
        month == 12) {
        lastDay = 31;
    }else if(month == 2){
        lastDay = 28;
    }
    
    Text* lbl = (Text*)layer->getChildByName("lblTimeLeft");
    time_t now = BSM->getCurrentTimeT();
    int timeLeft = BSM->getTimeLeftToNewDay();
    
    if (day > 15) {
        timeLeft += (lastDay - day + 14)*(60*60*24);
    }else{
        timeLeft += (15 - day)*(60*60*24);
    }
    
    lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeft).c_str()));
    if (timeLeft < 0) {
        this->unscheduleAllCallbacks();
        closePopup();
        return;
    }

    // roll day time left    
    lbl = (Text*)layer->getChildByName("lblRollTime");
    lbl->setString(GM->getTimeLeftInString(timeLeft));
    int rollDay = UDGetInt(KEY_MID_MONTH_ROULETTE_ROLL_DAY, 0);
    day = BSM->getDay();
    Button* btn = (Button*)layer->getChildByName("btnRoll");
    btn->setEnabled(rollDay != day);
    lbl->setVisible(rollDay == day);
    
    // roll video time left
    if(GM->videoIndex == VIDEO_MIDMONTH_ROULETTE && GM->isVideoDone){
        GM->isVideoDone = false;
        GM->saveTime(KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME, now);
        btn = (Button*)layer->getChildByName("btnRollForVideo");
        btn->setPosition(Vec2(1670.04f, 220.55f));
        UDSetBool(KEY_ROULETTE_VIDEO_BUTTON_ENABLED, true);
    }
    time_t lastPlayTimeT = GM->getSavedTime(KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME);
    if(difftime(now, lastPlayTimeT) < 0){
        GM->saveTime(KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME, now);
        lastPlayTimeT = now;
    }
    int cooltime = 60*60*6;
    timeLeft = cooltime - (difftime(now, lastPlayTimeT));
    lbl = (Text*)layer->getChildByName("lblVideoTime");
    btn = (Button*)layer->getChildByName("btnVideo");
    if(timeLeft >= 0){
        lbl->setVisible(true);
        lbl->setString(GM->getTimeLeftInString(timeLeft));
        btn->setEnabled(false);
        btn->setOpacity(100);
    }else{
        lbl->setVisible(false);
        btn->setEnabled(true);
        btn->setOpacity(255);
    }
    if(GM->market == MARKET_SMARTPASS){
        lbl->setVisible(false);
    }
}
void RoulettePopup::onRollClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    UDSetInt(KEY_MID_MONTH_ROULETTE_ROLL_DAY, BSM->getDay());
    rollTheRoulette();
}
void RoulettePopup::onVideoClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    GM->showVideo(VIDEO_MIDMONTH_ROULETTE);
}
void RoulettePopup::rollTheRoulette(){
    Button* btn = (Button*)layer->getChildByName("btnRollForVideo");
    btn->setPosition(Vec2(1670.04f, -2200.55f));
    UDSetBool(KEY_ROULETTE_VIDEO_BUTTON_ENABLED, false);
    
    Vector<FiniteTimeAction*> list;
    float targetAngle = (rand()%3600)*0.1f;
//    list.pushBack(RotateBy::create(1, 120));// test
    list.pushBack(RotateBy::create(1, 360 + targetAngle*0.45f));
    list.pushBack(RotateBy::create(1.1f, 240 + targetAngle*0.25f));
    list.pushBack(RotateBy::create(1.3f, 120 + targetAngle*0.20f));
    list.pushBack(RotateBy::create(1.9f, 50 + targetAngle*0.10f));
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(RoulettePopup::showReward, this));
    list.pushBack(call);
    
    Sequence* seq = Sequence::create(list);
    imgRoulette->runAction(seq);
    log("roll the roulette");
    
    Widget* widget = (Widget*)layer->getChildByName("imgBack");
    widget->addClickEventListener(nullptr);
    widget = (Widget*)layer->getChildByName("btnClose");
    widget->addClickEventListener(nullptr);
    widget = (Widget*)layer->getChildByName("btnVideo");
    widget->addClickEventListener(nullptr);
    widget = (Widget*)layer->getChildByName("btnRoll");
    widget->addClickEventListener(nullptr);
}
void RoulettePopup::showReward(){
    float angle = imgRoulette->getRotation();
    log("target angle: %f", angle);
    angle = (((int)(angle*10))%3600)*0.1f;
    log("target angle fix: %f", angle);
    int rudolph = 3;
    int reward = -1;
    int rewardCount = 0;
    if (angle >= 12.5f && angle < 66) {
        reward = RESOURCE_TYPE_LUMBER;
        rewardCount = 100;
        log("lumber 100");
    }else if (angle >= 66 && angle < 115.5f) {
        reward = RESOURCE_TYPE_GOLD;
        rewardCount = 50;
        log("gold 50");
    }else if (angle >= 115.5f && angle < 136.5f) {
        reward = rudolph;
        log("rudolph");
    }else if (angle >= 136.5f && angle < 192.5f) {
        reward = RESOURCE_TYPE_GEM;
        rewardCount = 5;
        log("gem 5");
    }else if (angle >= 192.5f && angle < 263.5f) {
        reward = RESOURCE_TYPE_LUMBER;
        rewardCount = 50;
        log("lumber 50");
    }else if (angle >= 263.5f && angle < 286.5f) {
        reward = rudolph;
        log("rudolph");
    }else if (angle >= 286.5f && angle < 339.0f) {
        reward = RESOURCE_TYPE_GOLD;
        rewardCount = 100;
        log("gold 100");
    }else{
        reward = RESOURCE_TYPE_GEM;
        rewardCount = 10;
        log("gem 10");
    }
    if (reward == rudolph) {
        strUnitReward = strmake("_%d/0", GM->getMidMonthHeroType());
        Button* btn = showUnitRewardPopup(strUnitReward, LM->getText(GM->getUnitName(GM->getMidMonthHeroType())).c_str());
        btn->addClickEventListener(CC_CALLBACK_0(RoulettePopup::onRewardUnitOkClick, this));
    }else{
        Button* btn = showResourceRewardPopup(reward, rewardCount, LM->getText("reward").c_str());
        btn->addClickEventListener(CC_CALLBACK_0(RoulettePopup::closeResourceRewardPopup, this));
    }
}
Button* RoulettePopup::showResourceRewardPopup(int resourceType, int resourceCount, const char* msg){
    Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("rewardPopup");
    setAsPopup(layer);
    layer->setTag(resourceType*10000 + resourceCount);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    Button* btn = (Button*)layer->getChildByName("btnOk");
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    ImageView* img;
    layer->getChildByName("imgGold")->setVisible(resourceType == RESOURCE_TYPE_GOLD);
    layer->getChildByName("imgTree")->setVisible(resourceType == RESOURCE_TYPE_LUMBER);
    layer->getChildByName("imgGem")->setVisible(resourceType == RESOURCE_TYPE_GEM);
    layer->getChildByName("lblGold")->setVisible(resourceType == RESOURCE_TYPE_GOLD);
    layer->getChildByName("lblTree")->setVisible(resourceType == RESOURCE_TYPE_LUMBER);
    layer->getChildByName("lblGemCount")->setVisible(resourceType == RESOURCE_TYPE_GEM);
    if(resourceType == RESOURCE_TYPE_GOLD){
        img = (ImageView*)layer->getChildByName("imgGold");
        lbl = (Text*)layer->getChildByName("lblGold");
    }else if(resourceType == RESOURCE_TYPE_LUMBER){
        img = (ImageView*)layer->getChildByName("imgTree");
        lbl = (Text*)layer->getChildByName("lblTree");
    }else if(resourceType == RESOURCE_TYPE_GEM){
        img = (ImageView*)layer->getChildByName("imgGem");
        lbl = (Text*)layer->getChildByName("lblGemCount");
    }
    img->setPositionY(layer->getChildByName("imgGold")->getPositionY());
    lbl->setPositionY(layer->getChildByName("lblGold")->getPositionY());
    lbl->setGlobalZOrder(10);
    Vec2 pos = lbl->getPosition();
    lbl->setString(Value(resourceCount).asString());
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedStringNotKey(lbl, msg);
    
    layer->getChildByName("imgGem")->setPosition(layer->getChildByName("imgGold")->getPosition());
    layer->getChildByName("lblGemCount")->setPosition(layer->getChildByName("lblGold")->getPosition());
    
    return btn;
}

void RoulettePopup::closeResourceRewardPopup(){
    Node* layer = this->getChildByName("rewardPopup");
    int resourceType = layer->getTag()/10000;
    int resourceCount = layer->getTag()%10000;
    Vec2 resourcePos = layer->getPosition() + layer->getChildByName("imgGold")->getPosition();
    
    TopBar* ndTopBar = TopBar::create();
    ndTopBar->setName("ndTopBar");
    TITLE->addChild(ndTopBar, 200 + (int)popupArray.size()+1);
    ndTopBar->setGlobalZOrder(1);
    ndTopBar->setPosition(Vec2(size.width - 98, size.height - 50));
    float yMove = 500;
    ndTopBar->setPositionY(ndTopBar->getPositionY() + yMove);
    ndTopBar->runAction(Sequence::create(EaseInOut::create(MoveBy::create(1, Vec2(0, -yMove)), 3), DelayTime::create(3), EaseInOut::create(MoveBy::create(1, Vec2(0, yMove)), 3), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, ndTopBar)), NULL));
    std::string strImgName;
    if(resourceType == RESOURCE_TYPE_GOLD){
        strImgName = "imgGoldIcon";
    }else if(resourceType == RESOURCE_TYPE_LUMBER){
        strImgName = "imgTreeIcon";
    }else if(resourceType == RESOURCE_TYPE_GEM){
        strImgName = "imgGemIcon";
    }
    
    Vec2 resourceTargetPos = layer->getPosition() + ndTopBar->getPosition() + ndTopBar->ndTopBar->getChildByName(strImgName)->getPosition() + Vec2(0, -yMove);
    float gemDelay = 0;
    float goldDelay = 0;
    float treeDelay = 0;
    int moveDistance = 160;
    int gemSptCount = 10;
    
    Sprite* sptResource;
    
    int bulkCount = 10;
    if(bulkCount > resourceCount){
        bulkCount = resourceCount;
    }
    int resourceGiven = 0;
    for (int i = 0; i < bulkCount; i++) {
        CallFuncN* resourceCall;
        if (resourceType == RESOURCE_TYPE_GEM) {
            sptResource = Sprite::create("gem.png");
            resourceCall = CallFuncN::create(CC_CALLBACK_1(TopBar::addGem, ndTopBar));
        }else if (resourceType == RESOURCE_TYPE_GOLD) {
            sptResource = Sprite::create("goldIcon.png");
            resourceCall = CallFuncN::create(CC_CALLBACK_1(TopBar::addGold, ndTopBar));
        }else if (resourceType == RESOURCE_TYPE_LUMBER) {
            sptResource = Sprite::create("lumberIcon.png");
            resourceCall = CallFuncN::create(CC_CALLBACK_1(TopBar::addLumber, ndTopBar));
        }
        TITLE->addChild(sptResource, 300);
        sptResource->setPosition(resourcePos);
        sptResource->runAction(Sequence::create(DelayTime::create(gemDelay), EaseInOut::create(MoveBy::create(0.3f, Vec2(rand()%moveDistance - moveDistance/2, rand()%moveDistance - moveDistance/2)), 2), DelayTime::create(0.5f), EaseInOut::create(MoveTo::create(0.6f, resourceTargetPos), 2), resourceCall,CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, sptResource)),NULL));
        gemDelay += 0.07f;
        int amount = resourceCount/bulkCount;
        sptResource->setTag(amount);
        resourceGiven += amount;
        sptResource->setGlobalZOrder(1);
    }
    sptResource->setTag(sptResource->getTag() + (resourceCount - resourceGiven));
    this->unscheduleAllCallbacks();
    closePopup();
    closePopup();
}

Button* RoulettePopup::showUnitRewardPopup(std::string units, const char* msg){
    Node* layer = CSLoader::createNode("UnitRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("unitRewardMessageBox");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->addGlowBack(imgBack);
    GM->rollOpenScroll(imgBack);
    Button* btn = (Button*)layer->getChildByName("btnOk");
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedStringNotKey(lbl, msg);
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    
    ValueVector unitList = GM->split(units, "_");
    int x = 192;
    int xGap = 281;
    for (int i = 0; i < unitList.size(); i++) {
        ValueVector dataList = GM->split(unitList.at(i).asString(), "/");
        if(dataList.size() > 1){
            ImageView* img = (ImageView*)((ImageView*)sv->getChildByName("imgTemp"))->clone();
            sv->addChild(img);
            img->setPosition(Vec2(x, sv->getContentSize().height/2 - 10));
            UnitInfo* info = new UnitInfo();
            Vec2 pos = img->getPosition();
            info->unitType = dataList.at(0).asInt();
            info->level = dataList.at(1).asInt()%100;
            info->rank = dataList.at(1).asInt()/100;
            lbl = (Text*)img->getChildByName("lblLevel");
            lbl->setString(strmake("LV.%d", info->level + 1));
            spine::SkeletonAnimation* spChar = GM->getHeroSpine(info->unitType);
            sv->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
            sptShadow->setOpacity(0);
            spChar->setPosition(img->getPosition() + Vec2(0, -45));
            img->setColor(GM->getRankColor(info->rank));
            
            x += xGap;
        }
    }
    sv->getChildByName("imgTemp")->setVisible(false);
    x -= xGap;
    if (x < sv->getContentSize().width) {
        int offsetX = (sv->getContentSize().width - x)/2 - 110;
        for(auto child : sv->getInnerContainer()->getChildren()){
            child->setPositionX(child->getPositionX() + offsetX);
        }
    }else{
        sv->setInnerContainerSize(cocos2d::Size(x + xGap/2, 287));
    }
    
    return btn;
}

void RoulettePopup::onRewardUnitOkClick(){
    std::vector<UnitInfo*> unitInfoListHeroInventory = GM->getHeroInventory();
    
    ValueVector unitList = GM->split(strUnitReward, "_");
    for (int i = 0; i < unitList.size(); i++) {
        ValueVector dataList = GM->split(unitList.at(i).asString(), "/");
        if(dataList.size() > 1){
            UnitInfo* info = new UnitInfo();
            info->unitType = dataList.at(0).asInt();
            info->level = dataList.at(1).asInt()%100;
            info->rank = dataList.at(1).asInt()/100;
            
            unitInfoListHeroInventory.push_back(info);
        }
    }
    GM->saveHeroInventory(unitInfoListHeroInventory);
    this->unscheduleAllCallbacks();
    closePopup();
    closePopup();
}
