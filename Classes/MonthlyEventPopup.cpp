//
//  MonthlyEventPopup.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 25/10/2019.
//

#include "MonthlyEventPopup.h"
#include "BuggyServerManager.h"
#include "LanguageManager.h"
bool MonthlyEventPopup::init()
{
    //    UDSetInt(KEY_GOLDEN_TICKET, 0); // test
    //    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime() - 30); // test
    
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    
    month = BSM->getMonth();
    day = BSM->getDay();
    
    if (month == 11) { // test
        layer = CSLoader::createNode("EntPopup.csb");
    }else if (month == 12) {
        layer = CSLoader::createNode("SantaPopup.csb");
    }else if (month == 1) {
        layer = CSLoader::createNode("SalamanderPopup.csb");
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            if(child->getContentSize().width > 30){
                child->setRotation(rand()%360);
                child->runAction(RepeatForever::create(Sequence::create(RotateBy::create(range*0.1f/2, range), RotateBy::create(range*0.1f/2, -range), NULL)));
            }else {
                range = 10 + rand()%10;
                child->runAction(RepeatForever::create(Sequence::create(RotateBy::create(range*0.1f/10, range), RotateBy::create(range*0.1f/10, -range), NULL)));
            }
        }
    }else if (month == 2) {
        layer = CSLoader::createNode("SalamanderPopup.csb");
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            ImageView* img = (ImageView*)child;
            img->loadTexture("waterSplash0.png", ImageView::TextureResType::PLIST);
            if(child->getContentSize().width > 30){
                child->setRotation(0);
                int bubbleSize = 70 + rand()%30;
                child->setContentSize(Size(bubbleSize, bubbleSize));
                GM->makeItSiluk(child, (rand()%10)*0.1f, 0.2f + (rand()%10)*0.1f);
//                child->runAction(RepeatForever::create(Sequence::create(RotateBy::create(range*0.1f/2, range), RotateBy::create(range*0.1f/2, -range), NULL)));
            }else {
                range = 10 + rand()%10;
                child->setContentSize(Size(child->getContentSize().width, child->getContentSize().width));
                child->runAction(RepeatForever::create(Sequence::create(MoveBy::create(range*0.1f/2, Vec2(0, range)), MoveBy::create(range*0.1f/2, Vec2(0, -range)), NULL)));
            }
        }
    }else if (month == 3) {
        layer = CSLoader::createNode("SalamanderPopup.csb");
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            ImageView* img = (ImageView*)child;
            img->loadTexture("werewolfFurball.png", ImageView::TextureResType::PLIST);
            if(child->getContentSize().width > 30){
                child->setRotation(0);
                int bubbleSize = 70 + rand()%30;
                child->setContentSize(Size(bubbleSize, bubbleSize));
                GM->makeItSiluk(child, (rand()%10)*0.1f, 0.2f + (rand()%10)*0.1f);
            }else {
                range = 10 + rand()%10;
                child->setContentSize(Size(child->getContentSize().width, child->getContentSize().width));
                child->runAction(RepeatForever::create(Sequence::create(MoveBy::create(range*0.1f/2, Vec2(0, range)), MoveBy::create(range*0.1f/2, Vec2(0, -range)), NULL)));
            }
        }
    }else if (month == 4) {
        layer = CSLoader::createNode("SalamanderPopup.csb");
        ImageView* img = (ImageView*)layer->getChildByName("imgContentBack");
        img->loadTexture("uiBoxStoneWhite.png");
        img->setColor(Color3B(255, 177, 223));
        Button* btn = (Button*)layer->getChildByName("imgTitle");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(255, 177, 223));
        btn = (Button*)layer->getChildByName("imgDescription");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(255, 177, 223));
        Text* lblTitle = (Text*)layer->getChildByName("lblTitle");
        lblTitle->setTextColor(Color4B(255, 52, 181, 255));
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            img = (ImageView*)child;
            img->loadTexture("heart.png", ImageView::TextureResType::PLIST);
            if(child->getContentSize().width > 30){
                child->setRotation(0);
                int bubbleSize = 70 + rand()%30;
                child->setContentSize(Size(bubbleSize, bubbleSize));
                GM->makeItSiluk(child, (rand()%10)*0.1f, 0.2f + (rand()%10)*0.1f);
            }else {
                range = 10 + rand()%10;
                child->setContentSize(Size(child->getContentSize().width, child->getContentSize().width));
                child->runAction(RepeatForever::create(Sequence::create(MoveBy::create(range*0.1f/2, Vec2(0, range)), MoveBy::create(range*0.1f/2, Vec2(0, -range)), NULL)));
            }
        }
    }else if (month == 5) {
        layer = CSLoader::createNode("SalamanderPopup.csb");
        ImageView* img = (ImageView*)layer->getChildByName("imgContentBack");
        img->loadTexture("uiBoxStoneWhite.png");
        img->setColor(Color3B(240, 193, 103));
        Button* btn = (Button*)layer->getChildByName("imgTitle");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(240, 193, 103));
        btn = (Button*)layer->getChildByName("imgDescription");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(240, 193, 103));
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            ImageView* img = (ImageView*)child;
            img->loadTexture("eventItem5.png");
            if(child->getContentSize().width > 30){
                child->setRotation(0);
                int bubbleSize = 70 + rand()%30;
                child->setContentSize(Size(bubbleSize, bubbleSize));
                GM->makeItSiluk(child, (rand()%10)*0.1f, 0.2f + (rand()%10)*0.1f);
            }else {
                range = 10 + rand()%10;
                child->setContentSize(Size(child->getContentSize().width, child->getContentSize().width));
                child->runAction(RepeatForever::create(Sequence::create(MoveBy::create(range*0.1f/2, Vec2(0, range)), MoveBy::create(range*0.1f/2, Vec2(0, -range)), NULL)));
            }
        }
    }else{
        layer = CSLoader::createNode("SalamanderPopup.csb");
        ImageView* img = (ImageView*)layer->getChildByName("imgContentBack");
        img->loadTexture("uiBoxStoneWhite.png");
        img->setColor(Color3B(240, 193, 103));
        Button* btn = (Button*)layer->getChildByName("imgTitle");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(240, 193, 103));
        btn = (Button*)layer->getChildByName("imgDescription");
        btn->loadTextureNormal("uiBoxStoneWhite.png");
        btn->setColor(Color3B(240, 193, 103));
        Node* ndDeco = layer->getChildByName("ndDeco");
        for(auto child: ndDeco->getChildren()){
            int range = 20 + rand()%50;
            ImageView* img = (ImageView*)child;
            img->loadTexture(StringUtils::format("eventItem%d.png", month));
            if(child->getContentSize().width > 30){
                child->setRotation(0);
                int bubbleSize = 70 + rand()%30;
                child->setContentSize(Size(bubbleSize, bubbleSize));
                GM->makeItSiluk(child, (rand()%10)*0.1f, 0.2f + (rand()%10)*0.1f);
            }else {
                range = 10 + rand()%10;
                child->setContentSize(Size(child->getContentSize().width, child->getContentSize().width));
                child->runAction(RepeatForever::create(Sequence::create(MoveBy::create(range*0.1f/2, Vec2(0, range)), MoveBy::create(range*0.1f/2, Vec2(0, -range)), NULL)));
            }
        }
    }
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    TopBar* topBar = TopBar::create();
    layer->addChild(topBar, POPUP_ZORDER);
    topBar->setPosition(Vec2(2273.42, 1235.74) + Vec2(-98, 3));
    
    ImageView* img = (ImageView*)layer->getChildByName("imgBack");
    img->addClickEventListener(CC_CALLBACK_0(MonthlyEventPopup::closeThis, this));
    
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(PageBase::closeThis, this));
    Text* lbl = (Text*)layer->getChildByName("lblBuff");
    LM->setLocalizedString(lbl, "month buff");
    
    addMissions();
    
    this->schedule(schedule_selector(MonthlyEventPopup::update));
    // init done
    return true;
}
void MonthlyEventPopup::addMissions(){
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    Button *btn;
    Text* lbl;
//    std::string strData = GM->getMonthlyEventInfoString();
//    ValueVector eventProgressList = GM->split(strData, "_");
//    ValueVector monthList = GM->split(eventProgressList.at(0).asString(), "/");
    lbl = (Text*)layer->getChildByName("lblTitle");
    std::string strHeroName = GM->getUnitName(GM->getMonthlyHeroType());
//    if (month == 11) { // test
//        strHeroName = "hero ent";
//    }else if (month == 12) {
//        strHeroName = "hero santa";
//    }else if (month == 1) {
//        strHeroName = "hero salamander";
//    }else if (month == 2) {
//        strHeroName = "hero undine";
//    }
    
    LM->setLocalizedString(lbl, strHeroName);
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, strmake("%s desc", strHeroName.c_str()));
    float height = lbl->getContentSize().height;
    lbl->setContentSize(Size(lbl->getContentSize().width, height*1.5f));
    lbl = (Text*)layer->getChildByName("lblMission");
    LM->setLocalizedString(lbl, "mission");
    lbl = (Text*)layer->getChildByName("lblRewardDescription");
    int receivedCount =UDGetInt(KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT, 0);
    LM->setLocalizedStringNotKey(lbl, LM->getText("two time for the account") + strmake("(%d/2)", receivedCount));
    
    btn = (Button*)layer->getChildByName("btnAccept");
    btn->setEnabled(false);
    btn->addClickEventListener(CC_CALLBACK_1(MonthlyEventPopup::onReceiveHeroClick, this));
    
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "receive");
    
    Node* imgIcon = layer->getChildByName("imgIcon");
    
    spine::SkeletonAnimation* unit = GM->getHeroSpine(GM->getMonthlyHeroType());
    
    unit->setAnimation(0, "idle", true);
    unit->setPosition(imgIcon->getPosition());
    unit->setScaleX(-1);
    imgIcon->getParent()->addChild(unit, 1);
    Sprite* sptShadow = Sprite::createWithSpriteFrameName("shadow.png");
    imgIcon->getParent()->addChild(sptShadow);
    sptShadow->setPosition(unit->getPosition());
    sptShadow->setScale(2);
    imgIcon->removeFromParent();
    
    ImageView* imgTemp = (ImageView*)sv->getChildByName("imgTemp");
    ImageView* imgItem;
    float y = 0;
    float x = 922;
    int gapY = 360;
    ImageView* img;
    int gemReward = 100;
    int odd = month%2;
    int missionIndex = 0;
    int missionCompleteCount = 0;
    Sprite* spt =  nullptr;
    GM->checkIsNewMonthlyEvent();
    
    float unitScale = 1.2f;
    float swordScale = 0.4f;
    for (int i = 0; i < 6; i++) {
        spt =  nullptr;
        imgItem = (ImageView*)imgTemp->clone();
        imgItem->setName(strmake("imgItem%d", i));
        sv->addChild(imgItem);
        imgItem->setPosition(Vec2(imgTemp->getPositionX(), y));
        lbl = (Text*)imgItem->getChildByName("lblTitle");
        img = (ImageView*)imgItem->getChildByName("imgIcon");
        missionIndex = i;
        if (i > 1) {
            missionIndex = (i-2)*2 + odd + 2;
        }
        LM->setLocalizedString(lbl, strmake("monthly event mission %d", missionIndex));
        
        if (missionIndex == EVENT_MISSION_ATTEND) {
            spt = Sprite::create("iconCalendar.png");
        }else if (missionIndex == EVENT_MISSION_COLLECT_EVENT_HERO_PART) {
            spt = Sprite::create(strmake("eventItem%d.png", month));
        }else if (missionIndex == EVENT_MISSION_KILL_GOBLIN) {
            spt = Sprite::createWithSpriteFrameName("goblinStand0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_KILL_SWORDSMAN) {
            spt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_KILL_TROLL) {
            spt = Sprite::createWithSpriteFrameName("trollStand0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_KILL_ORC_SPEAR) {
            spt = Sprite::createWithSpriteFrameName("orcSpearStand0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_KILL_ORC_AXE) {
            spt = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_KILL_ZOMBIE) {
            spt = Sprite::createWithSpriteFrameName("zombieHuman0.png");
            spt->setScale(unitScale);
        }else if (missionIndex == EVENT_MISSION_ARENA_CLEAR) {
            spt = Sprite::create("swordIcon.png");
            spt->setScale(swordScale);
        }else if (missionIndex == EVENT_MISSION_BATTLE_NETWORK_WIN ||
                  missionIndex == EVENT_MISSION_PVP_6_WIN ||
                  missionIndex == EVENT_MISSION_PVP_12_WIN) {
            spt = Sprite::create("swordIcon.png");
            Sprite* sptSecond = Sprite::create("swordIcon.png");
            spt->addChild(sptSecond);
            sptSecond->setPosition(spt->getContentSize()/2);
            sptSecond->setScaleX(-1);
            spt->setScale(swordScale);
        }
        if (spt != nullptr) {
            img->getParent()->addChild(spt);
            spt->setPosition(img->getPosition());
            img->removeFromParent();
        }
        lbl = (Text*)imgItem->getChildByName("lblGemCount");
        lbl->setString(Value(gemReward).asString());
        lbl = (Text*)imgItem->getChildByName("lblProgress");
//        int current = eventProgressList.at(missionIndex+1).asInt();
        int current = UDGetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, missionIndex).c_str(), 0);
        int max = GM->getMonthlyEventProgressMax(missionIndex);
        bool isReceived = current < 0;
        if (current > max || isReceived) {
            current = max;
        }
        lbl->setString(strmake("%d/%d", current, max));
        
        Node* progressBar = imgItem->getChildByName("imgProgress");
        progressBar->setContentSize(cocos2d::Size(480.0f*current/max, 84));
        Node* imgCheck = imgItem->getChildByName("imgCheck");
        imgCheck->setVisible(isReceived);
        
        btn = (Button*)imgItem->getChildByName("btnReceive");
        btn->setTag(missionIndex);
        btn->addClickEventListener(CC_CALLBACK_1(MonthlyEventPopup::onReceiveRewardForItem, this));
        btn->setEnabled(!isReceived && current >= max);
        if (isReceived) {
            btn->getParent()->getChildByName("imgCheck")->setVisible(true);
            btn->setVisible(false);
            missionCompleteCount++;
        }
        btn = (Button*)imgItem->getChildByName("btnTest");
        btn->setVisible(false);
        if(false){ // test
            btn->setVisible(true);
            btn->setTag(missionIndex);
            btn->addClickEventListener(CC_CALLBACK_1(MonthlyEventPopup::onTestClick, this));
        }
//        btn->setVisible(true); // test
        y -= gapY;
    }
    imgTemp->removeFromParent();
    int scrollHeight = sv->getContentSize().height;
    if (scrollHeight < -y) {
        scrollHeight = -y;
    }
    sv->setInnerContainerSize(cocos2d::Size(sv->getContentSize().width, scrollHeight));
    for(auto btn: sv->getChildren()){
        btn->setPositionY(btn->getPositionY() + scrollHeight - gapY/2);
    }
    
    btn = (Button*)layer->getChildByName("btnAccept");
    lbl = (Text*)btn->getChildByName("lblMission");
    LM->setLocalizedStringNotKey(lbl, strmake("(%s %d/%d)", LM->getText("mission complete").c_str(), missionCompleteCount, 6));
    btn->setEnabled(isAllMissionClearAndReceived());
    
    updateLayer();
}
bool MonthlyEventPopup::isAllMissionClearAndReceived(){
//    bool isAllClear = true;
//    std::string strData = GM->getMonthlyEventInfoString();
//    ValueVector eventProgressList = GM->split(strData, "_");
    int shouldClearCount = 6;
//    for (int i = 1; i < eventProgressList.size(); i++) {
    for (int i = 0; i < EVENT_MISSION_TOTAL_COUNT; i++) {
//        if (eventProgressList.at(i).asInt() < GM->getMonthlyEventProgressMax(i-1)) {
//        if (eventProgressList.at(i).asInt() >= 0) {
        if(UDGetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, i).c_str(), 0) >= 0){
//            isAllClear = false;
//            break;
        }else{
            shouldClearCount--;
        }
    }
    return shouldClearCount <= 0;
}
void MonthlyEventPopup::onTestClick(Ref* ref){
    BTN_FROM_REF
    int missionIndex = btn->getTag();
    GM->addMonthlyEventProgress(missionIndex, 3);
}
void MonthlyEventPopup::onReceiveRewardForItem(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int missionIndex = btn->getTag();
//    std::string strData = GM->getMonthlyEventInfoString();
//    ValueVector eventProgressList = GM->split(strData, "_");
//    std::string strNewData = "" + eventProgressList.at(0).asString();
    int completeCount = 0;
//    for (int i = 1; i < eventProgressList.size(); i++) {
    for(int i = 0; i < EVENT_MISSION_TOTAL_COUNT; i++){
        std::string key = strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, i).c_str();
        if (i == missionIndex) {
//            strNewData += "_-1";
            UDSetInt(key.c_str(), -1);
            btn->getParent()->getChildByName("imgCheck")->setVisible(true);
            btn->setVisible(false);
//            completeCount++;
        }else{
//            strNewData += "_" + eventProgressList.at(i).asString();
        }
//        if (eventProgressList.at(i).asInt() < 0) {
        if(UDGetInt(key.c_str(), 0) < 0){
            completeCount++;
        }
    }
//    log("newData: %s", strNewData.c_str());
//    UDSetStr(KEY_MONTHLY_EVENT_INFO, strNewData);
    BSM->isThereSomethingToSaveForMonthlyEventProgress = true;
    
    Text* lbl = (Text*)btn->getParent()->getChildByName("lblGemCount");
    int reward = Value(lbl->getString()).asInt();
    GM->addGem(reward);
    BSM->saveUserData("gem=" + Value(GM->getGem()).asString());
    
    btn = (Button*)layer->getChildByName("btnAccept");
    btn->setEnabled(isAllMissionClearAndReceived());
    lbl = (Text*)btn->getChildByName("lblMission");
    LM->setLocalizedStringNotKey(lbl, strmake("(%s %d/%d)", LM->getText("mission complete").c_str(), completeCount, 6));
}
void MonthlyEventPopup::updateLayer(){
//    std::string strData = GM->getMonthlyEventInfoString();
//    ValueVector eventProgressList = GM->split(strData, "_");
//    ValueVector monthList = GM->split(eventProgressList.at(0).asString(), "/");
    int heroGetCount = UDGetInt(KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT, 0);//monthList.at(1).asInt();
    int completeCount = 0;
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    Button* btn;ImageView* imgItem;Text* lbl;
    for (int i = 0; i < 6; i++) {
//        const char* key = strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, i).c_str();
        imgItem = (ImageView*)sv->getChildByName(strmake("imgItem%d", i));
        btn = (Button*)imgItem->getChildByName("btnReceive");
//        btn->setVisible(eventProgressList.at(i).asInt() >= 0 && heroGetCount < 2);
        int missionIndex = btn->getTag();
        int current = UDGetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, missionIndex).c_str(), 0);
        btn->setVisible(current >= 0 && heroGetCount < 2);
        if (current < 0) {
            completeCount++;
        }
        imgItem->getChildByName("imgCheck")->setVisible(!btn->isVisible());
//        int current = eventProgressList.at(btn->getTag()+1).asInt();
        int max = GM->getMonthlyEventProgressMax(missionIndex);
        bool isReceived = current < 0;
        if (current > max || isReceived || heroGetCount >= 2) {
            current = max;
        }
        lbl = (Text*)imgItem->getChildByName("lblProgress");
        lbl->setString(strmake("%d/%d", current, max));
        Node* progressBar = imgItem->getChildByName("imgProgress");
        progressBar->setContentSize(cocos2d::Size(480.0f*current/max, 84));
    }
    
    btn = (Button*)layer->getChildByName("btnAccept");
    btn->setEnabled(isAllMissionClearAndReceived());
    lbl = (Text*)btn->getChildByName("lblMission");
    LM->setLocalizedStringNotKey(lbl, strmake("(%s %d/%d)", LM->getText("mission complete").c_str(), completeCount, 6));
    btn->setVisible(heroGetCount < 2);
    
    lbl = (Text*)layer->getChildByName("lblRewardDescription");
    LM->setLocalizedStringNotKey(lbl, LM->getText("two time for the account") + strmake("(%d/2)", heroGetCount));
}
void MonthlyEventPopup::onReceiveHeroClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
//    std::string strInfo = GM->getMonthlyEventInfoString();
//    ValueVector list = GM->split(strInfo, "_");
//    ValueVector receiveInfo = GM->split(list.at(0).asString(), "/");
    int receivedCount = UDGetInt(KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT, 0);//receiveInfo.at(1).asInt();
    receivedCount++;
//    if (receivedCount > 1) {
//
//    }
    GM->resetMonthlyEventProgress(receivedCount);
    
    // add hero to inventory
    std::string strData = UDGetStr(KEY_UNITS_HERO_INVENTORY, "_");
    int hero = GM->getMonthlyHeroType();
    strData += "" + strmake("_%d/0", hero);
    UDSetStr(KEY_UNITS_HERO_INVENTORY, strData + "");
    
    std::vector<UnitInfo*> infoList = GM->getHeroInventory();
    UnitInfo* info = new UnitInfo();
    info->unitType = hero;
    infoList.push_back(info);
    GM->saveHeroInventory(infoList);
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    BSM->saveUserData(datas);
    
    updateLayer();
}
void MonthlyEventPopup::update(float dt){
    Text* lbl = (Text*)layer->getChildByName("lblTimeLeft");
    time_t now = BSM->getCurrentTimeT();
    int timeLeft = 86400 - (long)now%86400;
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
    timeLeft += (lastDay - day)*(60*60*24);
    lbl->setString(strmake("%s", GM->getTimeLeftInString(timeLeft).c_str()));
}
