        //
//  HudLayer.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//
#include "FindMatch.h"
#include "HudLayer.h"
#include "LegendDaryButton.h"
#include "GameManager.h"
#include "HelloWorldScene.h"

//#include "NativeInterface.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
#include "Title.h"
#include "ui/UITextBMFont.h"
#include "ui/UIScrollView.h"
#include "EndingCredit.h"
bool HudLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    weaponIndex = 0;
    size = Director::getInstance()->getWinSize();
    isRecording = false;
    listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HudLayer::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HudLayer::onKeyReleased, this);
    
    
//    ImageView* sptRect = ImageView::create("res/258_gray_rect.png");
//    sptRect->setPosition(Vec2(size.width/2, size.height - 714));
//    sptRect->setContentSize(cocos2d::Size(750, 750));
//    sptRect->setAnchorPoint(Vec2(0.5, 0));
//    sptRect->setCapInsets(cocos2d::Rect(3, 3, sptRect->getContentSize().width - 6, sptRect->getContentSize().height - 6));
//    sptRect->setScale9Enabled(true);
//    this->addChild(sptRect, 9);
//    sptRect->runAction(Sequence::create(DelayTime::create(0.5f), EaseIn::create(MoveBy::create(0.5f, Vec2(0, -750)), 2), CallFunc::create(CC_CALLBACK_0(ImageView::removeFromParent, sptRect)), NULL));
//
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    talkLayer = NULL;
    talkLabel = NULL;
    
    gameOverLayer = NULL;
    reviveLayer = NULL;
    pauseLayer = NULL;
    gameClearLayer = NULL;
    
    float gap = 5;
    bool isRankingEnabled = false;
   
    
    GameManager::getInstance()->page = PAGE_STAGE;
    
    registerControllerListener();

    bool forceTouchPossible = false;
#if defined(__IPHONE_9_0) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_9_0)
    // force touch enbled
//    forceTouchPossible = true;
//    if (UserDefault::getInstance()->getBoolForKey(KEY_FIRST_LAUNCH, true)) {
//        UserDefault::getInstance()->setBoolForKey(KEY_FIRST_LAUNCH, false);
//        UserDefault::getInstance()->setBoolForKey(KEY_AUTO_TARGET_SELECTED, false);
//    }
#endif
    
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//    joystickAim->setPositionY(-2000);
    hideBtns();
#endif
    
    bool lastJewelCollected = UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString());
    
    lblInfo = LM->getLocalizedLabel("", Color4B::BLACK);
    lblInfo->setScale(0.4);
    lblInfo->enableOutline(Color4B::WHITE, 5);
    lblInfo->setPosition(Vec2(size.width/2, size.height - 714 + 150));
    lblInfo->setWidth(size.width);
    lblInfo->setHorizontalAlignment(TextHAlignment::CENTER);
    lblInfo->setAnchorPoint(Vec2(0.5, 0.5));
    this->addChild(lblInfo);
//
//    inventory = new Inventory();
//    inventory->reset();
//
    draw = DrawNode::create();
    this->addChild(draw, 10);
//
//    inventory->printItems();
//    inventory->getData();
//    UDSetInt(strmake(KEY_HERO_LEVEL_FORMAT, 0).c_str(), 10);
//    GM->addCoin(190000);
    UDSetStr(strmake("hero_tier_%d", 0).c_str(), "0");
    this->schedule(schedule_selector(HudLayer::oneSecUpdate), 1);
    
//    lblCommand = Label::createWithSystemFont("COMMAND: ", "Thonburi", 100);
//    this->addChild(lblCommand);
//    lblCommand->setPosition(Vec2(size.width/2, lblCommand->getBoundingBox().size.height/2 + 50));
    
    Button* btn;
    for (int i = 0; i < 6; i++) {
//        Vec2 pos = Vec2(size.width - 600 + (i%3)*200, 400 - 200*(i/3)); // two rows
//        Vec2 pos = Vec2(size.width - 10 - 200*(5-i), 10); // reverse
        Vec2 pos = Vec2(size.width - 10 - 200*(5 - i), 10);
//        if (i == 0) {
//            btn = Button::create("btnMove.png");
//        }else if (i == 1) {
//            btn = Button::create("btnStop.png");
//        }else if (i == 2) {
//            btn = Button::create("btnAttack.png");
//        }else if (i == 3) {
//            btn = Button::create("btnGather.png");
//        }else if (i == 4) {
//            btn = Button::create("btnBuild.png");
//        }else if (i == 5) {
//            btn = Button::create("btnBuildBetter.png");
//        }else if (i == 6) {
//            btn = Button::create("whiteCircle.png");
//            pos = Vec2(1200, 600);
//        }else if (i == 7) {
//            btn = Button::create("whiteCircle.png");
//            pos = Vec2(1300, 600);
//        }
        Node* btnMenu = (Widget*)CSLoader::createNode("MenuButton.csb");
        btnMenu->setTag(i);
        btnMenu->setName(strmake("btnMenu%d", i));
        btn = (Button*)btnMenu->getChildByName("btn");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onCommandClick, this));
        btnMenu->setAnchorPoint(Vec2(1, 0));
        btnMenu->setPosition(pos);
        
        WORLD->addChild(btnMenu, 10000);
        
//        PPLabel* lbl = PPLabel::create("", 30, Color3B::WHITE, true, true, TextHAlignment::CENTER, true);
//        btn->addChild(lbl, 10);
//        lbl->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height*5/6));
//        lbl->setName("lbl");
//        if(LM->getLanguageType() == LanguageType::ENGLISH){
//            lbl->setWidth(btn->getContentSize().width);
//        }else{
//            log("menu set korean");
//            lbl->lblNormal->setWidth(btn->getContentSize().width);
//            lbl->lblNormal->setAlignment(TextHAlignment::CENTER);
//            lbl->lblNormal->enableOutline(Color4B(DARK_GRAY_3B, 255));
//        }
    }
    
    isRaid = GM->nextScene == STAGE_SINGLEPLAY || GM->nextScene == STAGE_RAID;
    ImageView* img;Sprite* spt;
    img = ImageView::create("uiBoxSmall.png");
    img->setName("imgResourceBack");
    this->addChild(img);
    img->setScale9Enabled(true);
    img->setTouchEnabled(true);
    img->setContentSize(cocos2d::Size(950, 105));
    img->setAnchorPoint(Vec2(1, 1));
    img->setOpacity(150);
    img->setPosition(Vec2(size.width, size.height));
    
    lblGold = PPLabel::create("0", 40, Color3B::WHITE, false, false, TextHAlignment::LEFT, true);
    this->addChild(lblGold);
    lblGold->setPosition(Vec2(size.width - 800, size.height - 50));
    Sprite* sptGold = Sprite::create("goldIcon.png");
    this->addChild(sptGold);
    sptGold->setName("iconGold");
    sptGold->setPosition(lblGold->getPosition() + Vec2(-60, 0));
    
    lblLumber = PPLabel::create("0", 40, Color3B::WHITE, false, false, TextHAlignment::LEFT, true);
    this->addChild(lblLumber);
    lblLumber->setPosition(Vec2(size.width - 500, size.height - 50));
    lblLumber->setAnchorPoint(Vec2(0, 0.5));
    Sprite* sptTree = Sprite::create("lumberIcon.png");
    this->addChild(sptTree);
    sptTree->setName("iconLumber");
    sptTree->setPosition(lblLumber->getPosition() + Vec2(-50, 0));
    
    lblFood = PPLabel::create("0", 40, Color3B::WHITE, false, false, TextHAlignment::LEFT, true);
    this->addChild(lblFood);
    lblFood->setPosition(Vec2(size.width - 200, size.height - 50));
    lblFood->setAnchorPoint(Vec2(0, 0.5));
    Sprite* sptMan = Sprite::create("manIcon.png");
    this->addChild(sptMan);
    sptMan->setName("iconFood");
    sptMan->setPosition(lblFood->getPosition() + Vec2(-50, 0));
    if(isRaid){
        img->setVisible(false);
        lblGold->setVisible(false);
        lblLumber->setVisible(false);
        lblFood->setVisible(false);
        sptGold->setVisible(false);
        sptTree->setVisible(false);
        sptMan->setVisible(false);
    }
    
    
    float y = 0;
    priceInfo = Node::create();
    this->addChild(priceInfo);
    priceInfo->setPosition(Vec2(size.width/2, 130));
    PPLabel* lbl = PPLabel::create("0", 40, Color3B::WHITE, false, true, TextHAlignment::LEFT, false);//LM->getLocalizedLabel("0", Color4B::WHITE);
    priceInfo->addChild(lbl);
    lbl->setName("lblGold");
    lbl->setPosition(Vec2(0 - 300, y));
    lbl->setAnchorPoint(Vec2(0, 0.5));
    spt = Sprite::create("goldIcon.png");
    spt->setName("sptGold");
    priceInfo->addChild(spt);
    spt->setPosition(lbl->getPosition() + Vec2(-50, 0));
    
    lbl = PPLabel::create("0", 40, Color3B::WHITE, false, true, TextHAlignment::LEFT, false);
    priceInfo->addChild(lbl);
    lbl->setName("lblLumber");
    lbl->setPosition(Vec2(0 - 0, y));
    lbl->setAnchorPoint(Vec2(0, 0.5));
    spt = Sprite::create("lumberIcon.png");
    spt->setName("sptLumber");
    priceInfo->addChild(spt);
    spt->setPosition(lbl->getPosition() + Vec2(-50, 0));
    
    lbl = PPLabel::create("0", 40, Color3B::WHITE, false, true, TextHAlignment::LEFT, false);
    priceInfo->addChild(lbl);
    lbl->setName("lblFood");
    lbl->setPosition(Vec2(0 + 300, y));
    lbl->setAnchorPoint(Vec2(0, 0.5));
    spt = Sprite::create("manIcon.png");
    spt->setName("sptFood");
    priceInfo->addChild(spt);
    spt->setPosition(lbl->getPosition() + Vec2(-50, 0));
    
    lbl = PPLabel::create("not enough food", 50, Color3B::WHITE, true, true, TextHAlignment::CENTER, false);
    priceInfo->addChild(lbl);
    lbl->setName("lblDescription");
    lbl->setPosition(Vec2(0, y + 100));
    
    priceInfo->setVisible(false);
    
    if(isRaid){
        Node* ndBottom = CSLoader::createNode("BottomUnitBar.csb");
        this->addChild(ndBottom, 5);
        ndBottom->setPositionX(size.width/2);
        bottomUnitBar = ndBottom;
        Text* lblSelectGround = (Text*)ndBottom->getChildByName("lbl");
        LM->setLocalizedString(lblSelectGround, "select ground to place soldiers");
        lblSelectGround->runAction(Blink::create(20, 20));
        std::string strEquipped = UDGetStr(KEY_UNITS_DECK, "");
        ValueVector units = GameManager::getInstance()->split(strEquipped, "_");
        for(int i = 0; i < units.size(); i++){
            if(units.at(i).asString().length() > 0){
                UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
                deckUnitList.push_back(info);
            }
        }
        
        ScrollView* sv = (ScrollView*)ndBottom->getChildByName("svDeck");
        Button* btnTemp = (Button*)ndBottom->getChildByName("btnTemp");
        int counter = 0;
        for(auto info : deckUnitList){
            Button* btn = (Button*)btnTemp->clone();
            sv->addChild(btn);
            btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onUnitInBottomDeckClick, this));
            btn->setTag(counter);
            btn->setPosition(Vec2(137.05f + counter*267.3f, 122.38f));
            Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(info->unitType));
            btn->addChild(spt);
            spt->setScale(2);
            spt->setPosition(btn->getContentSize()/2);
            counter++;
        }
        
        strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
        units = GameManager::getInstance()->split(strEquipped, "_");
        for(int i = 0; i < units.size(); i++){
            if(units.at(i).asString().length() > 0){
                UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
                deckUnitList.push_back(info);
            }
        }
        
        for(auto info : heroDeckUnitList){
            Button* btn = (Button*)btnTemp->clone();
            sv->addChild(btn);
            btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onUnitInBottomDeckClick, this));
            btn->setTag(counter);
            btn->setPosition(Vec2(137.05f + counter*267.3f, 122.38f));
            EnemyBase* unit = EnemyBase::createEnemy(info->unitType, 0, 0, WORLD->getSpriteNameForUnit(info->unitType).c_str());
            btn->addChild(unit);
            unit->setPosition(Vec2(btn->getContentSize().width/2, 20));
            counter++;
        }
        btnTemp->removeFromParent();
        sv->setInnerContainerSize(cocos2d::Size(counter*267.3f, sv->getContentSize().height));
    }
    if(GM->nextScene == STAGE_FIELD || isRaid){
        int offsetX = 120;
        
        if(size.height/size.width < 700.0f/1334.0f){
//            offsetX = 100;
        }
        if(GM->nextScene == STAGE_FIELD){
            btn = Button::create("uiBoxSmallBlue.png");
            this->addChild(btn, 99);
            btn->setContentSize(cocos2d::Size(220, 100));
            btn->setName("btnMenu");
            btn->setScale9Enabled(true);
            btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMenuClick, this));
            btn->setPosition(Vec2(offsetX + 327, 480));
            addLabelToButton("menu", btn, false, DARK_GRAY_3B);
        }
        
        ImageView* img = ImageView::create("uiBoxSmallBlue.png");
        this->addChild(img, 99);
        img->setName("imgTimeBack");
        img->setContentSize(cocos2d::Size(250, 100));
        img->setScale9Enabled(true);
        img->setPosition(Vec2(offsetX + 125, 480));
        
        lblTimer = PPLabel::create("00:00:00", 50, DARK_GRAY_3B, false, false, TextHAlignment::CENTER, false);//LM->getLocalizedLabel();
        lblTimer->setPosition(Vec2(offsetX + 125, 480));
        lblTimer->setScaleX(0.8f);
        this->addChild(lblTimer, 99);
        if(isRaid){
            img->setContentSize(cocos2d::Size(440, 100));
            img->setPositionX(offsetX + 220);
            lblTimer->setPositionX(img->getPositionX());
        }
        
        offsetX = size.width - 440;
        float offsetY = -85;
        
        
        Node* ndPanel = CSLoader::createNode("RightBottomPanelForCampaign.csb");
        rightBottomPanelForCampaign = ndPanel;
        this->addChild(ndPanel, 99);
        offsetRight = 100;
        ndPanel->setPosition(Vec2(size.width - offsetRight, 0));
        
        btn = (Button*)ndPanel->getChildByName("btnSelectAll");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSelectAllForces, this));
        
        btn = (Button*)ndPanel->getChildByName("btnSelectDrag");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onDragSelectClick, this));
        
        btn = (Button*)ndPanel->getChildByName("btn2xFast");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::on2xFastClick, this));
        ndPanel->getChildByName("img2xSelected")->setVisible(false);
        btn = (Button*)ndPanel->getChildByName("btn3xFast");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::on3xFastClick, this));
        ndPanel->getChildByName("img3xSelected")->setVisible(false);
        
        btn = (Button*)ndPanel->getChildByName("btnSelectScreen");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSelectForcesInScreen, this));
        
        btn = (Button*)ndPanel->getChildByName("btnShowHide");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onShowMoreMenuClick, this));
        
        rightBottomPanel = CSLoader::createNodeWithVisibleSize("RightBottomPanel.csb");
        this->addChild(rightBottomPanel, 5);
        
        btn = (Button*)rightBottomPanel->getChildByName("btnSelectAll");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSelectAllForces, this));
        
        btn = (Button*)rightBottomPanel->getChildByName("btnSelectDrag");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onDragSelectClick, this));
        
        btn = (Button*)rightBottomPanel->getChildByName("btnSelectScreen");
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSelectForcesInScreen, this));
        
        
        rightBottomPanel->setContentSize(size);
        Node* ndBattle = rightBottomPanel->getChildByName("ndBattle");
        for (int i =0; i < 3; i++) {
            img = (ImageView*)ndBattle->getChildByName(strmake("imgStar%d", i));
            img->setGrayScale(true);
        }
        btn = (Button*)ndBattle->getChildByName("btnFindMatch");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onNextMatchClick, this));
        if (GM->isVisiting) {
            btn->setVisible(false);
        }
        Text* lblGold = (Text*)btn->getChildByName("lblGold");
        lblGold->setString(Value((int)(10 * pow(2, GM->matchFindCount+1))).asString());
        GM->alignToCenter(btn->getChildByName("imgGold"), lblGold, 20, btn->getContentSize().width/2, 0);
        
        rightBottomPanel->getChildByName("btnSelectDrag")->setVisible(false);
        rightBottomPanel->getChildByName("btnSelectScreen")->setVisible(false);
        rightBottomPanel->getChildByName("btnSelectAll")->setVisible(false);
        
        Text* lblTitle = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lblTitle, "next match");
        if(GM->nextScene == STAGE_SINGLEPLAY){
            btn->setVisible(false);
        }
        btn = (Button*)rightBottomPanel->getChildByName("btnSurrender");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSurrenderClick, this));
        btn->setPositionY(560); // test
        lblTitle = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lblTitle, "surrender");
        Node* ndInfo = rightBottomPanelForCampaign->getChildByName("ndInfo");
        if(isRaid){
            rightBottomPanel->getChildByName("ndMission")->setVisible(false);
        }else{
            ndBattle->setVisible(false);
            btn->setVisible(false);
        }
        ndInfo->setVisible(false);
        ScrollView* sv = (ScrollView*)ndInfo->getChildByName("sv");
        Label* lblDescription = LM->getLocalizedLabel("", Color4B(77,77,77, 255), 30);
//        lblDescription->enableOutline(Color4B::BLACK, 3);
        lblDescription->setName("lblDescription");
        sv->removeAllChildren();
        sv->addChild(lblDescription);
        lblDescription->setWidth(sv->getContentSize().width);
        lblDescription->setAnchorPoint(Vec2(0.5, 1));
        lblDescription->setPosition(Vec2(sv->getContentSize().width/2, 0));
        
        if (GM->nextScene == STAGE_FIELD && !isRaid) {
            rightBottomPanel->setVisible(false);
            imgDragSelected = rightBottomPanelForCampaign->getChildByName("imgDrawSelected");
            imgDragSelected->setVisible(false);
        }
        if(isRaid){
            rightBottomPanelForCampaign->setVisible(false);
            imgDragSelected = rightBottomPanel->getChildByName("imgDrawSelected");
            imgDragSelected->setVisible(false);
        }
    }
    
    
    if (WORLD->stageIndex == 0 && GM->nextScene == STAGE_FIELD && !GM->isColosseum && !WORLD->isHardMode) {
        tutorialIndex = 0;
        tutorialNode = Node::create();
        this->addChild(tutorialNode, 200);
        
        ImageView* img = ImageView::create("uiBox.png");
        tutorialNode->addChild(img, -1);
        img->setTouchEnabled(true);
        img->setScale(100, 100);
        img->setColor(Color3B::BLACK);
        img->setOpacity(50);
        img->setPosition(Vec2(size.width/2, size.height/2));
        img->addClickEventListener(CC_CALLBACK_0(HudLayer::onTutorialBoxClick, this));
        
        img = ImageView::create("uiBox.png");
        tutorialNode->addChild(img);
        img->setScale9Enabled(true);
        img->setTouchEnabled(true);
        img->setContentSize(cocos2d::Size(2000, 500));
        img->setPosition(Vec2(size.width/2, 330));
        img->addClickEventListener(CC_CALLBACK_0(HudLayer::onTutorialBoxClick, this));
        
        Label* lbl = LM->getLocalizedLabel("", Color4B::BLACK, 60);
        tutorialNode->addChild(lbl);
        lbl->setPosition(img->getPosition() + Vec2(200, 0));
        lbl->setDimensions(1500, 450);
        lbl->setName("lbl");
        lbl->setVerticalAlignment(TextVAlignment::CENTER);
        
        Sprite* sptIcon = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
        tutorialNode->addChild(sptIcon);
        GM->runAnimation(sptIcon, "workerStand", true);
        sptIcon->setPosition(Vec2(size.width/2 - 750, 330));
        sptIcon->setName("sptIcon");
        sptIcon->setScale(1/WORLD->imageScale);
        
        talkIndex = 0;
        talkText = LM->getText(strmake("tutorial %d", 0).c_str());
        this->schedule(schedule_selector(HudLayer::talkBoxUpdate), 0.1f);
    }
    
    this->schedule(schedule_selector(HudLayer::update), 0.1f);
    
    log("init done");
    // init done
    return true;
}
void HudLayer::on2xFastClick(){
    rightBottomPanelForCampaign->getChildByName("img3xSelected")->setVisible(false);
    if(WORLD->gameSpeed == 2){
        WORLD->gameSpeed = 1;
        rightBottomPanelForCampaign->getChildByName("img2xSelected")->setVisible(false);
    }else{
        WORLD->gameSpeed = 2;
        rightBottomPanelForCampaign->getChildByName("img2xSelected")->setVisible(true);
    }
    Director::getInstance()->setTimeScale(WORLD->gameSpeed);
}
void HudLayer::on3xFastClick(){
    rightBottomPanelForCampaign->getChildByName("img2xSelected")->setVisible(false);
    if(WORLD->gameSpeed == 3){
        WORLD->gameSpeed = 1;
        rightBottomPanelForCampaign->getChildByName("img3xSelected")->setVisible(false);
    }else{
        WORLD->gameSpeed = 3;
        rightBottomPanelForCampaign->getChildByName("img3xSelected")->setVisible(true);
    }
    Director::getInstance()->setTimeScale(WORLD->gameSpeed);
}
void HudLayer::onShowMoreMenuClick(Ref* ref){
    BTN_FROM_REF
    bool shouldShow = btn->getTag() == 0;
    btn->setTag(shouldShow?1:0);
    btn->getParent()->stopAllActions();
    if (shouldShow) {
        btn->getParent()->runAction(EaseOut::create(MoveTo::create(0.3f, Vec2(size.width - offsetRight, 567.30f)), 2));
    }else{
        btn->getParent()->runAction(EaseOut::create(MoveTo::create(0.3f, Vec2(size.width - offsetRight, 0)), 2));
    }
    btn->getChildByName("img")->runAction(RotateTo::create(0.3f, shouldShow?0:180));
}
void HudLayer::onNextMatchClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int price = Value(((Text*)btn->getChildByName("lblGold"))->getString()).asInt();
    if (GM->getCoin() >=  price) {
        GM->addCoin(-price);
        BSM->saveUserData("gold=" + Value(GM->getCoin()).asString());
        this->unscheduleAllCallbacks();
        removeListener();
        GM->setHudLayer(nullptr);
        GM->matchFindCount++;
        GM->nextScene = STAGE_RAID; // test
        auto scene = Scene::create();
        FindMatch* findMatch = FindMatch::create();
        scene->addChild(findMatch);
        Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
    }else{
        showInstanceMessage(LM->getText("not enough gold"));
    }
}
void HudLayer::onSurrenderClick(Ref* ref){
//    showRaidResult(false);
    goToBattleScene();
//    removeListener();
//    if(GM->isAdsUser()){
//        GameSharing::showInterstitial();
//    }
//
//    GM->setHudLayer(nullptr);
//
//    GM->nextScene = STAGE_LOBBY;
//    auto scene = HelloWorld::scene(STAGE_LOBBY, false);
//    Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
}
void HudLayer::setRaid(){
    std::string strEquipped = GM->raidEnemyBuildings;//UDGetStr(KEY_BUILDINGS, "");
    log("strEquipped: %s", strEquipped.c_str());
    
    Node* layer = CSLoader::createNodeWithVisibleSize("BattleEnemyInfo.csb");
    
    this->addChild(layer, 5);
    Text* lbl = (Text*)layer->getChildByName("lblName");
    lbl->setString(GM->raidEnemyName);
    lbl = (Text*)layer->getChildByName("lblTrophy");
    lbl->setString(Value(GM->raidEnemyTrophy).asString());
    lbl = (Text*)layer->getChildByName("lblGold");
    lbl->setString(Value(GM->raidEnemyGold).asString());
    lbl = (Text*)layer->getChildByName("lblTree");
    lbl->setString(Value(GM->raidEnemyTree).asString());
    
    ValueVector units = GM->split(strEquipped, "_");
    Vec2 castlePos;
    for(int i = 0; i < units.size(); i++){
        ValueVector datas = GM->split(units.at(i).asString(), "/");
        if(datas.size() > 1){
            int unitType = Value(datas.at(0)).asInt();
            
            UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
            cocos2d::Size occupySize = WORLD->getBuildingOccupySize(unitType);
            float x = Value(datas.at(2)).asInt();
            float y = Value(datas.at(3)).asInt();
            Vec2 sptPos = Vec2(x, y);
            if(x > 100 || y > 100){
                sptPos = Vec2(x/2, y/2);
            }else{
                float extraX = 0;
//                if ((int)WORLD->getBuildingOccupySize(unitType).width%2 == 1) {
//                    extraX = 50;
//                }
                float extraY = 0;
//                if ((int)WORLD->getBuildingOccupySize(unitType).height%2 == 1) {
//                    extraY = 50;
//                }
                sptPos = Vec2(x*TILE_SIZE + extraX, y*TILE_SIZE + extraY);
            }
            Vec2 pos = sptPos - Vec2(occupySize.width*TILE_SIZE/2, occupySize.height*TILE_SIZE/2);
            Vec2 occupyPos = pos + Vec2(TILE_SIZE*(((int)occupySize.width)%2), TILE_SIZE*occupySize.height);
            EnemyBase* unit = WORLD->createUnit(unitType, info->unitType == UNIT_TREE_FOR_BATTLE?WHICH_SIDE_MUTUAL:WHICH_SIDE_ENEMY, ITS_BUILDING, sptPos, "name", 1, WORLD->getSpriteNameForUnit(unitType));
            if(info->unitType != UNIT_TREE_FOR_BATTLE){
                unit->energy = GM->getUnitHP(info->unitType, info->level);
                unit->maxEnergy = unit->energy;
                unit->ap = GM->getUnitATT(info->unitType, info->level);
            }else{
                
            }
            if(i == 0 || unit->unitType == UNIT_CASTLE){
                castlePos = unit->getPosition();
            }
            if (unit->isBuilding) {
                unit->setPosition(unit->getPosition() + Vec2(TILE_SIZE*0.5f*(((int)occupySize.width)%2), TILE_SIZE*0.5f*(((int)occupySize.height)%2)));
//                setupBuilding(unit);
                unit->level = info->level;
                WORLD->setOccupy(occupyPos, occupySize.width, occupySize.height, true, unit);
                
                unit->buildingCompleteTimeLeft = info->endTime - BSM->getCurrentTimeT();
                unit->unitState = info->unitState;
//                buildings.pushBack(unit);
                if (info->unitState == UNIT_STATE_BUILDING || info->unitState == UNIT_STATE_UPGRADE || unit->unitState == UNIT_STATE_REMOVING) {
                    unit->energy = 1;
                    unit->maxEnergy = 100;
                    unit->updateEnergy();
                    Text* lblTimeLeft = Text::create("Upgrade...", "BMDOHYEON.ttf", 20);
                    lblTimeLeft->enableOutline(Color4B::BLACK, 2);
                    WORLD->addChild(lblTimeLeft, unit->getParent()->getLocalZOrder()+1);
                    lblTimeLeft->setPosition(unit->getPosition() + Vec2(0, 60));
                    //                    timeLeftLabels.pushBack(lblTimeLeft);
                    unit->lblTimeLeft = lblTimeLeft;
                    if(unit->unitState == UNIT_STATE_UPGRADE){
//                        addUpgradeEffect(unit);
//                        unit->buildingCompleteTime = getUnitCreatingTime(unit->unitType, unit->level+1);
                    }else if(unit->unitState == UNIT_STATE_BUILDING){
                        if(unit->unitType == UNIT_TREE_FOR_BATTLE){
                            unit->setScale(0.5f);
                            unit->runAction(ScaleTo::create(unit->buildingCompleteTimeLeft, 1));
                            GM->makeItSiluk(unit);
                        }else{
                            unit->runAnimation("underConstrunction", true);
                        }
//                        unit->buildingCompleteTime = getUnitCreatingTime(unit->unitType, unit->level);
                    }else if(unit->unitState == UNIT_STATE_REMOVING){
                        unit->buildingCompleteTime = 60;
                    }
                }
                unit->setLocalZOrder(-unit->getPositionY());
            }else{
//                setupNonBuilding(unit);
            }
        }
    }
    
    strEquipped = GM->raidEnemyDeck;
    units = GameManager::getInstance()->split(strEquipped, "_");
    for(int i = 0; i < units.size(); i++){
        if(units.at(i).asString().length() > 0){
            UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
            Movable* unit = GM->getUnitFromData(info);
//            for (auto building:buildings) {
//                if(building->unitType == UNIT_BARRACKS){
//                    unit->setPosition(building->getPosition() + Vec2(0, -200));
//                    info->x = unit->getPositionX();
//                    info->y = unit->getPositionY();
//                    break;
//                }
//            }
            
            WORLD->addUnit(unit, false);
//            unitsDeck.pushBack(unit);
            Vec2 pos;
            if(info->x > 100 || info->y > 100){
                pos = Vec2(info->x/2, info->y/2);
            }else{
//                pos = Vec2(info->x*TILE_SIZE, info->y*TILE_SIZE);
                pos = WORLD->getPositionFromTileCoordinate(info->x, info->y);
            }
            if(pos.x > 30*TILE_SIZE){
                pos = Vec2(28*TILE_SIZE, pos.y);
            }else if(pos.x < 0){
                pos = Vec2(2*TILE_SIZE, pos.y);
            }
            if(pos.y > 30*TILE_SIZE){
                pos = Vec2(pos.x, 28*TILE_SIZE);
            }else if(pos.y < 0){
                pos = Vec2(pos.x, 2*TILE_SIZE);
            }
            unit->setPosition(pos);
            unit->stop();
            unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
//            info->belongTo = BELONG_TO_DECK;
//            info->index = i;
//            unitInfoListDeck.push_back(info);
        }
    }
    
    strEquipped = GM->raidEnemyHeroDeck;
    units = GameManager::getInstance()->split(strEquipped, "_");
    
    std::string str = GM->raidEnemyHeroPos;
    std::vector<int> raidHeroPosXList;
    std::vector<int> raidHeroPosYList;
    if(str.length() > 0){
        std::string value = "";
        ValueVector posList = GameManager::getInstance()->split(str, "_");
        for (int i = 0; i < posList.size(); i++) {
            ValueVector xAndY = GameManager::getInstance()->split(posList.at(i).asString(), "/");
            if(xAndY.size() > 1){
                int x = xAndY.at(0).asInt();
                int y = xAndY.at(1).asInt();
                raidHeroPosXList.push_back(x);
                raidHeroPosYList.push_back(y);
            }
        }
    }
    int index = 0;
    for(int i = 0; i < units.size(); i++){
        if(units.at(i).asString().length() > 0){
            UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info == nullptr){
                continue;
            }
            Movable* unit = GM->getUnitFromData(info);
            WORLD->addUnit(unit, false);
            WORLD->setHeroLevelInfo((EnemyBase*)unit, info->level%100);
            Vec2 pos;
            if (unit->getPositionX() < 0 || unit->getPositionY() < 0) {
                unit->setPosition(castlePos - Vec2(200, 200) + Vec2(i*100, 0));
                if (unit->getPositionX() < 0 || unit->getPositionY() < 0) {
                    unit->setPosition(Vec2(TILE_SIZE*2, TILE_SIZE*2));
                }
                WORLD->moveTo((EnemyBase*)unit, unit->getPosition());
            }else{
                if(info->x > 100 || info->y > 100){
                    pos = Vec2(info->x/2, info->y/2);
                }else{
//                    pos = Vec2(info->x*TILE_SIZE, info->y*TILE_SIZE);
                    pos = WORLD->getPositionFromTileCoordinate(info->x, info->y);
                }
            }
            if(pos.x > 30*TILE_SIZE){
                pos = Vec2(28*TILE_SIZE, pos.y);
            }else if(pos.x < 0){
                pos = Vec2(2*TILE_SIZE, pos.y);
            }
            if(pos.y > 30*TILE_SIZE){
                pos = Vec2(pos.x, 28*TILE_SIZE);
            }else if(pos.y < 0){
                pos = Vec2(pos.x, 2*TILE_SIZE);
            }
            if (raidHeroPosXList.size() > index && raidHeroPosYList.size() > index) {
                pos = WORLD->getPositionFromTileCoordinate(raidHeroPosXList.at(index), raidHeroPosYList.at(index));
            }
            unit->setPosition(pos);
            unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
            unit->spine->setLocalZOrder(unit->getLocalZOrder());
            index++;
        }
    }
//    updateUnitInfoInList();
    WORLD->resetPathState();
    
    for(auto unit:WORLD->unitsToCreateArray){
        if(unit->alliSide == WHICH_SIDE_HERO){
            if(unit->unitType == UNIT_MINE || unit->unitType == UNIT_TREE_FOR_BATTLE){
                unit->alliSide = WHICH_SIDE_MUTUAL;
                unit->isEnemy = false;
            }else{
                unit->alliSide = WHICH_SIDE_ENEMY;
                unit->isEnemy = true;
            }
        }
    }
}
void HudLayer::findMatchForRaid(){
    
}
void HudLayer::showStarForRaid(int starIndex){
    ImageView* imgStar = (ImageView*)rightBottomPanel->getChildByName("ndBattle")->getChildByName(strmake("imgStar%d", starIndex));
    imgStar->setScale(0.1f);
    imgStar->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1)));
    imgStar->setGrayScale(false);
}
void HudLayer::onUnitInBottomDeckClick(Ref* ref){
    BTN_FROM_REF
    Node* sv = btn->getParent();
    for (auto child : sv->getChildren()) {
        
    }
}
void HudLayer::showRaidResultWithSaveRecord(){
    showRaidResult(true);
}
void HudLayer::showRaidResult(bool saveRecord){
    int gold = 0;
    int tree = 0;
    int trophy = 10;
    bool isWin = WORLD->starCountForRaid > 0;
    if(GM->currentStageIndex == STAGE_RAID && saveRecord){
        int myTrophy = UDGetInt(KEY_TROPHY, 1000);
        if (isWin) {
            if(GM->raidEnemyTrophy < myTrophy){
                trophy = -10;
            }else{
                int max = GM->raidEnemyTrophy - myTrophy;
                if(max > 100){
                    max = 100;
                }else if( max < -100){
                    max = -100;
                }
                trophy = -(max)*0.3f - 1;
            }
            if (trophy > 0) {
                trophy *= -1;
            }
            GM->addMonthlyEventProgress(EVENT_MISSION_BATTLE_NETWORK_WIN, 1);
        }else{
            if(GM->raidEnemyTrophy > myTrophy){
                trophy = 10;
            }else{
                int max = GM->raidEnemyTrophy - myTrophy;
                if(max > 50){
                    max = 50;
                }
                trophy = (max)*0.3f + 1;
            }
            if (trophy < 0) {
                trophy *= -1;
            }
//            if (!isWin) {
//                trophy = 0; // no bonus trophy for opponent when player lose
//            }
        }
        if(trophy < -100){
            trophy = -100;
        }else if(trophy > 100){
            trophy = 100;
        }
        
        int trophyGet = -trophy;
        
        GM->raidEnemyTrophy = trophy;
        if(GM->raidEnemyTrophy < 0){
            GM->raidEnemyTrophy = 0;
        }
        
        gold = GM->raidEnemyGold*WORLD->starCountForRaid*0.33f;
        tree = GM->raidEnemyTree*WORLD->starCountForRaid*0.33f;
        if(WORLD->starCountForRaid == 3){
            gold = GM->raidEnemyGold;
            tree = GM->raidEnemyTree;
        }
        if (isWin) {
            gold *= -1;
            tree *= -1;
        }
        if(GM->raidEnemyRID.length() > 2){
            
        }else{
            
        }
        BSM->sendPost("adddefencerecord", "id=" + GM->raidEnemyID + "&enemyid=" + UDGetStr(KEY_SAVED_ID, "-1") + "&name=" + UDGetStr(KEY_NAME, "noname") + "&trophy=" + Value(isWin?-1:0).asString() + "&gold="+Value(gold).asString()+"&tree="+Value(tree).asString() + "&star=" + Value(WORLD->starCountForRaid).asString(), httpresponse_selector(BuggyServerManager::onActionNotNeededComplete));
        
        GM->raidRewardGold = -gold;
        GM->raidRewardTree = -tree;
        GM->raidRewardTrophy = trophyGet;
    }else if(GM->currentStageIndex == STAGE_SINGLEPLAY){
        if (isWin) {
            bool isRewardGiven = UDGetBool(strmake(KEY_SINGLE_PLAY_CLEAR_FORMAT, GM->singlePlayStageIndex).c_str(), false);
            trophy = 0;
            if (!isRewardGiven) {
                gold = -100*GM->singlePlayStageIndex;
                tree = -40*GM->singlePlayStageIndex;
                
                GM->addCoin(-gold);
                GM->addTree(-tree);
                BSM->saveUserData("gold=" + Value(GM->getCoin()).asString() + "&tree=" + Value(GM->getTree()).asString());
            }
            
            UDSetBool(strmake(KEY_SINGLE_PLAY_CLEAR_FORMAT, GM->singlePlayStageIndex).c_str(), true);
            UDSetInt(strmake(KEY_SINGLE_PLAY_STAR_FORMAT, GM->singlePlayStageIndex).c_str(), WORLD->starCountForRaid);
        }
    }
    Node* layer = CSLoader::createNode("WinPopup.csb");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    GM->animateFadeIn(layer, this);
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onOkFromRaidWinPopup, this));
    Node* imgTitle = layer->getChildByName("imgTitle");
    ImageView* img;
    float delayTime = 0.5f;
    
    Node* imgSwordL = layer->getChildByName("imgTitle")->getChildByName("imgSwordL");
    Node* imgSwordR = layer->getChildByName("imgTitle")->getChildByName("imgSwordR");
    if(!isWin){
        imgSwordL->runAction(FadeOut::create(0.5f));
        imgSwordL->runAction(RotateBy::create(0.5f, 360));
        imgSwordL->runAction(MoveBy::create(0.5f, Vec2(0, -200)));
        imgSwordR->runAction(FadeOut::create(0.5f));
        imgSwordR->runAction(RotateBy::create(0.5f, -360));
        imgSwordR->runAction(MoveBy::create(0.5f, Vec2(0, -200)));
    }else{
        imgSwordL->runAction(EaseBackIn::create(RotateBy::create(0.5f, 360)));
        imgSwordR->runAction(EaseBackIn::create(RotateBy::create(0.5f, -360)));
    }
    
    for (int i =0; i < 3; i++) {
        img = (ImageView*)imgTitle->getChildByName(strmake("imgStar%d", i));
        
        if(i+1 > WORLD->starCountForRaid){
            img->setOpacity(0);
        }else{
            GM->showNode(img, delayTime);
            delayTime += 0.3f;
        }
    }
    Text* lbl = (Text*)imgTitle->getChildByName("lbl");
    if (isWin) {
        LM->setLocalizedString(lbl, "win");
    }else{
        LM->setLocalizedString(lbl, "lose");
    }
    
    GM->showNode(lbl, delayTime);
    delayTime += 0.3f;
    
    Node* imgBody = layer->getChildByName("imgBody");
    if(isWin){
        lbl = (Text*)imgBody->getChildByName("lblReward");
        LM->setLocalizedString(lbl, "reward");
        GM->showNode(lbl, delayTime);
        delayTime += 0.3f;
        lbl = (Text*)imgBody->getChildByName("lblTrophy");
        lbl->setString(Value(-trophy).asString());
        GM->showNode(lbl, delayTime);
        delayTime += 0.3f;
        lbl = (Text*)imgBody->getChildByName("lblGold");
        lbl->setString(Value(-gold).asString());
        GM->showNode(lbl, delayTime);
        delayTime += 0.3f;
        lbl = (Text*)imgBody->getChildByName("lblTree");
        lbl->setString(Value(-tree).asString());
        GM->showNode(lbl, delayTime);
        delayTime += 0.4f;
    }else{
        lbl = (Text*)imgBody->getChildByName("lblTrophy");
        lbl->setString(Value(-trophy).asString());
        GM->showNode(lbl, delayTime);
        delayTime += 0.3f;
    }
    
    GM->showNode(layer->getChildByName("btnOk"), delayTime); // test
    lbl = (Text*)layer->getChildByName("btnOk")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
}

void HudLayer::onDragSelectClick(Ref* ref){
    BTN_FROM_REF
    imgDragSelected->setVisible(!imgDragSelected->isVisible());
    if(imgDragSelected->isVisible()){
        showInstanceMessage(LM->getText("drag to select"));
    }
}
void HudLayer::showCancelBuildingButton(){
    Button* btn = Button::create("btnStop.png");
    btn->setName("btnCancelBuilding");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onCancelBuildingClick, this));
    this->addChild(btn);
    btn->setPosition(Vec2(size.width/2, btn->getContentSize().height/2 + 20));
}
void HudLayer::onCancelBuildingClick(){
    this->getChildByName("btnCancelBuilding")->removeFromParent();
    WORLD->selectCommand(COMMAND_NOTHING);
    if(WORLD->buildingTemplate != nullptr){
        WORLD->buildingTemplate->removeFromParent();
        WORLD->buildingTemplate = nullptr;
    }
}
void HudLayer::onSelectAllForces(){
    showInstanceMessage(LM->getText("select all"));
    imgDragSelected->setVisible(false);
    WORLD->selectAllForces();
}
void HudLayer::onSelectForcesInScreen(){
    showInstanceMessage(LM->getText("select screen"));
    imgDragSelected->setVisible(false);
    WORLD->onSelectForcesInScreen();
}
void HudLayer::showSupportOffer(){
    GM->playSoundEffect(SOUND_WOOD_HIT);
    WORLD->pauseLayer();
    Layer* layer = Layer::create();
    setAsPopup(layer);
    this->addChild(layer, 200);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Vec2(size.width/2, size.height/2 + 100));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->setPressedActionEnabled(false);
    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
    
    Button* btn = Button::create("horizontalScrollBackground.png");
    btn->setScale9Enabled(true);
    btn->setContentSize(cocos2d::Size(308, 1270));
    
//    imgBack->setAnchorPoint(Vec2(0.5, 0.5));
    btn->setPosition(Vec2(size.width/2, size.height/2));
    
    layer->addChild(btn);
//    btn->setContentSize(cocos2d::Size(1800, 1000));
    btn->setScale9Enabled(true);
    btn->setPosition(size/2);
    btn->setZoomScale(0);
    btn->runAction(EaseInOut::create(ResizeTo::create(0.3, cocos2d::Size(2098, 1270)), 2));
    
    PPLabel* lbl = PPLabel::create(LM->getText("choose support"), 60, Color3B(45, 45, 45), LM->getLanguageType() != LanguageType::ENGLISH, false, TextHAlignment::CENTER, false);
    lbl->setPosition(Vec2(size.width/2, size.height/2 + 400));
    layer->addChild(lbl);
    
    float y = size.height/2 + 320;
    
    for(int i = 0; i < 3; i++){
        btn = Button::create("uiBoxSmall.png");
        layer->addChild(btn, 99);
        btn->setTitleColor(Color3B::BLACK);
        btn->setContentSize(cocos2d::Size(520, 520));
        btn->setScale9Enabled(true);
        btn->setPosition(Vec2(size.width/2 + (i-1)*560, size.height/2 + 60));
        
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSupportSelected, this));
        
        btn->setTag(i);
        if(i == 0){
            float unitGap = 90;
            int unitType = UNIT_WORKER;
            if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                unitType = UNIT_GOBLIN_WORKER;
            }
            for(int j = 0; j < 4; j++){
                Sprite* spt = WORLD->getSpriteForUnit(unitType);
                btn->addChild(spt);
                spt->setPosition(Vec2(btn->getContentSize().width/2 - (unitGap*3/2) + unitGap*j, btn->getContentSize().height/2));
            }
        }else if(i == 1){
            float unitGap = 80;
            int unitType = UNIT_SWORDMAN;
            if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                unitType = UNIT_GOBLIN;
            }
            for(int j = 0; j < 3; j++){
                Sprite* spt = WORLD->getSpriteForUnit(unitType);
                btn->addChild(spt);
                spt->setPosition(Vec2(btn->getContentSize().width/2 - (unitGap) + unitGap*j, btn->getContentSize().height/2 + 70));
            }
            unitType = UNIT_ARCHER;
            if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                unitType = UNIT_GOBLIN_BOMB;
            }
            for(int j = 0; j < 3; j++){
                Sprite* spt = WORLD->getSpriteForUnit(unitType);
                btn->addChild(spt);
                spt->setPosition(Vec2(btn->getContentSize().width/2 - (unitGap) + unitGap*j, btn->getContentSize().height/2 - 70));
            }
        }else if(i == 2){
            int unitType = UNIT_HELICOPTER;
            if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                unitType = UNIT_TROLL;
            }
            Sprite* spt = WORLD->getSpriteForUnit(unitType);
            btn->addChild(spt);
            spt->setPosition(Vec2(150, 350));
            unitType = UNIT_CATAPULT;
            if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                unitType = UNIT_WIZARD;
            }
            spt = WORLD->getSpriteForUnit(unitType);
            btn->addChild(spt);
            spt->setPosition(Vec2(300, 200));
        }
        
        if(GM->isColosseum){
            lbl = PPLabel::create(LM->getText("select support"), btn->getContentSize().height*2/6, DARK_GRAY_3B, LM->getLanguageType() != LanguageType::ENGLISH, false, TextHAlignment::CENTER, false);
            lbl->setPosition(Vec2(size.width/2 + (i-1)*560 - 520/2, size.height/2 - 320));
            layer->addChild(lbl);
            lbl->setAnchorPoint(Vec2(0, 0.5));
            lbl->setTextHAlignment(TextHAlignment::LEFT);
            lbl->setScale(0.4f);
            
            int xOffset = -140;
            cocos2d::Size btnSize = cocos2d::Size(160, 110);
            float sptScale = 0.8f;
            Button* btnVideo = Button::create("uiBoxSmall.png");
            layer->addChild(btnVideo);
            btnVideo->setContentSize(btnSize);
            btnVideo->setScale9Enabled(true);
            btnVideo->setPosition(Vec2(size.width/2 + (i-1)*560 + 150 + xOffset, size.height/2 - 320));
            btnVideo->addClickEventListener(CC_CALLBACK_1(HudLayer::onSupportSelected, this));
            btnVideo->setTag(i);
            
            Sprite* spt = Sprite::create("videoIcon.png");
            btnVideo->addChild(spt);
            spt->setPosition(btnVideo->getContentSize()/2);
            spt->setScale(sptScale);
            
            Button* btnTicket = Button::create("uiBoxSmall.png");
            layer->addChild(btnTicket);
            btnTicket->setContentSize(btnSize);
            btnTicket->setScale9Enabled(true);
            btnTicket->setPosition(Vec2(size.width/2 + (i-1)*560 + 320 + xOffset, size.height/2 - 320));
            btnTicket->addClickEventListener(CC_CALLBACK_1(HudLayer::onSupportSelectedByTicket, this));
            btnTicket->setTag(i);
            
            spt = Sprite::create("goldenTicket.png");
            btnTicket->addChild(spt);
            spt->setPosition(btnTicket->getContentSize()/2);
            spt->setScale(sptScale);
            
            if(LM->getLanguageType() == LanguageType::SPANISH){
                lbl->setPositionY(lbl->getPositionY() + 48);
                btnVideo->setPositionY(btnVideo->getPositionY() - 62);
                btnTicket->setPositionY(btnTicket->getPositionY() - 62);
            }
            
            int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
            lbl = PPLabel::create(Value(ticketCount).asString(), 50, Color3B::WHITE, false, true, TextHAlignment::RIGHT, false);
            lbl->setPosition(Vec2(size.width/2 + 860, size.height/2 + 550));
            layer->addChild(lbl);
            lbl->setAnchorPoint(Vec2(1, 0.5));
            lbl->setTextHAlignment(TextHAlignment::RIGHT);
//            lbl->setScale(0.4f);
            
            spt = Sprite::create("goldenTicket.png");
            layer->addChild(spt);
            spt->setPosition(lbl->getPosition() + Vec2(-lbl->totalWidth/2 - spt->getContentSize().width/2 - 50, 0));
        }else{
            
            btn = Button::create("uiBoxSmall.png");
            layer->addChild(btn);
            btn->setContentSize(cocos2d::Size(520, 150));
            btn->setScale9Enabled(true);
            btn->setPosition(Vec2(size.width/2 + (i-1)*560, size.height/2 - 320));
            btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSupportSelected, this));
            btn->setTag(i);
            
            lbl = PPLabel::create(LM->getText("select support"), btn->getContentSize().height*2/6, DARK_GRAY_3B, LM->getLanguageType() != LanguageType::ENGLISH, false, TextHAlignment::CENTER, false);
            lbl->setPosition(Vec2(btn->getContentSize().width/2 + 70, btn->getContentSize().height/2));
            btn->addChild(lbl);
            
            Sprite* spt = Sprite::create("videoIcon.png");
            btn->addChild(spt);
            spt->setPosition(Vec2(150, 75));
            
            if(LM->getLanguageType() == LanguageType::SPANISH){
                spt->setPositionX(spt->getPositionX() - 40);
            }
        }
        btn = Button::create("btnStop.png");
        layer->addChild(btn);
        btn->setScale(0.45);
        btn->setPosition(Vec2(size.width/2 + 800, size.height/2 + 400));
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
        
    }
}
void HudLayer::onSupportSelectedByTicket(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
    if(ticketCount > 0){
        ticketCount--;
        UDSetInt(KEY_GOLDEN_TICKET, ticketCount);
        WORLD->getSupportFromVideo(VIDEO_SUPPORT_0 + btn->getTag());
        onResumeClick();
    }else{
        showInstanceMessage(LM->getText("not enough ticket"));
    }
}
void HudLayer::onSupportSelected(Ref* ref){
    log("on vido click");
    BTN_FROM_REF_AND_DISABLE
    bool isVideoPass = false;
    if (GM->market == MARKET_SMARTPASS) {
        isVideoPass = true;
    }
    if (isVideoPass) {
        videoDone();
    }else{
        if(GameSharing::isVideoAvailable()){
            GM->showVideo(VIDEO_SUPPORT_0 + btn->getTag());
        }else{
            showInstanceMessage(LM->getText("video not available"));
        }
    }
//    GM->showVideo(VIDEO_SUPPORT_0 + btn->getTag()); // test
//    GM->showVideoDone(); // test
}
void HudLayer::onMenuClick(){
//    showWinPopup(true);
//    return; // test 
//    GM->playSoundEffect(SOUND_PAPER_FLIP);
//    WORLD->sendTestScore();// test
//    GM->showVideo(VIDEO_SUPPORT_0 + 1); // test 
    GM->playSoundEffect(SOUND_WOOD_HIT);
    WORLD->pauseLayer();
    
//    showWinPopup(true); // test
    Node* layer = CSLoader::createNode("Menu.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    
    Button* btn =(Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
    
    btn =(Button*)layer->getChildByName("btnSave");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSaveClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "save");
    btn =(Button*)layer->getChildByName("btnLoad");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onLoadClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "load");
    btn =(Button*)layer->getChildByName("btnSound");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOptionClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "sound");
    btn =(Button*)layer->getChildByName("btnPremium");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::showPremiumRetry, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "premium retry");
    if (GM->isColosseum || isRaid || UDGetBool(KEY_PREMIUM_START)) {
        btn->setVisible(false);
    }
    
    btn =(Button*)layer->getChildByName("btnQuit");
    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onExitClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "exit");
    btn =(Button*)layer->getChildByName("btnContinue");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "resume");
    
    if(GM->isColosseum){
        layer->getChildByName("btnSave")->setVisible(false);
        layer->getChildByName("btnLoad")->setVisible(false);
    }
//
//    Layer* layer = Layer::create();
//    setAsPopup(layer);
//    this->addChild(layer, 200);
//
//    Button* btnBlock = Button::create("uiBox.png");
//    btnBlock->setPosition(Vec2(size.width/2, size.height/2 + 100));
//    layer->addChild(btnBlock);
//    btnBlock->setScale(20);
//    btnBlock->setOpacity(100);
//    btnBlock->setColor(Color3B::BLACK);
//    btnBlock->setPressedActionEnabled(false);
//    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
//
//    ImageView* img = ImageView::create("uiBox.png");
//    layer->addChild(img, 99);
//    img->setContentSize(cocos2d::Size(700, 1000));
//    img->setScale9Enabled(true);
//    img->setPosition(size/2);
//
//    float y = size.height/2 + 320;
//    Button* btn = Button::create("uiBoxSmall.png");
//    layer->addChild(btn, 99);
//    btn->setTitleColor(Color3B::BLACK);
//    btn->setContentSize(cocos2d::Size(520, 150));
//    btn->setScale9Enabled(true);
//    if (GM->isColosseum){
//        btn->setColor(Color3B::GRAY);
//    }else{
//        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onSaveClick, this));
//    }
//    btn->setPosition(Vec2(size.width/2, y));
//    addLabelToButton("save", btn, false, DARK_GRAY_3B);
//
//    y -= 160;
//
//    btn = Button::create("uiBoxSmall.png");
//    layer->addChild(btn, 99);
//    btn->setTitleColor(Color3B::BLACK);
//    btn->setContentSize(cocos2d::Size(520, 150));
//    btn->setScale9Enabled(true);
//    if (GM->isColosseum){
//        btn->setColor(Color3B::GRAY);
//    }else{
//        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onLoadClick, this));
//    }
//    btn->setPosition(Vec2(size.width/2, y));
//    addLabelToButton("load", btn, false, DARK_GRAY_3B);
//    y -= 160;
//
//    btn = Button::create("uiBoxSmall.png");
//    layer->addChild(btn, 99);
//    btn->setTitleColor(Color3B::BLACK);
//    btn->setContentSize(cocos2d::Size(520, 150));
//    btn->setScale9Enabled(true);
//    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOptionClick, this));
//    btn->setPosition(Vec2(size.width/2, y));
//    addLabelToButton("option", btn, false, DARK_GRAY_3B);
//    y -= 160;
//
//    btn = Button::create("uiBoxSmall.png");
//    layer->addChild(btn, 99);
//    btn->setContentSize(cocos2d::Size(520, 150));
//    btn->setScale9Enabled(true);
//    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onResumeClick, this));
//    btn->setPosition(Vec2(size.width/2, y));
//    addLabelToButton("resume", btn, false, DARK_GRAY_3B);
//    y -= 180;
//
//    btn = Button::create("uiBoxSmall.png");
//    layer->addChild(btn, 99);
//    btn->setTitleColor(Color3B::BLACK);
//    btn->setContentSize(cocos2d::Size(520, 150));
//    btn->setScale9Enabled(true);
//    btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onExitClick, this));
//    btn->setPosition(Vec2(size.width/2, y));
//    addLabelToButton("exit", btn, false, DARK_GRAY_3B);
}
void HudLayer::onOptionClick(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Vec2(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lbl = PPLabel::create(LM->getText("option"), 100, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lbl);
    lbl->setPosition(Vec2(size.width/2, size.height/2 + 300));
    
    Button* btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Vec2(size.width/2 - 300, size.height/2 - 100));
    btn->setContentSize(cocos2d::Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onEffectClick, this));
    
    lbl = PPLabel::create(LM->getText(GM->getSoundVolumn()==0?"effect off":"effect on"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("effect");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Vec2(size.width/2 + 300, size.height/2 - 100));
    btn->setContentSize(cocos2d::Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onMusicClick, this));
    
    lbl = PPLabel::create(LM->getText(GM->getMusicVolumn()==0?"music off":"music on"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("music");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
}

void HudLayer::onSaveClick(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Vec2(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lblTitle;
    PPLabel* lblTime;
    Button* btn;
    float firstLineY = size.height/2;
    float gapY = 260;
    PPLabel* lblSave = PPLabel::create(LM->getText("save"), 100, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lblSave);
    lblSave->setPosition(Vec2(size.width/2, size.height/2 + 360));
    for (int i = 0; i < 4; i++) {
        btn = Button::create("uiBoxSmall.png");
        btn->setScale9Enabled(true);
        layer->addChild(btn);
        btn->setContentSize(cocos2d::Size(500, 200));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onSaveSlotClick, this));
        btn->setTag(i);
        std::string data = UDGetStr(strmake("savedData%d", btn->getTag()).c_str());
        int firstComma = (int)data.find(',');
        int secondComma = (int)data.find(',', firstComma+1);
        data = data.substr(0, secondComma);
        ValueVector datas = GameManager::getInstance()->split(data, ",");
        
        int savedStage = -1;
        int savedTime = 0;
        if(datas.size() > 1){
            log("std: %s", datas.at(0).asString().c_str());
            savedStage = datas.at(0).asInt();
            savedTime = datas.at(1).asInt();
        }
        std::string savedStageTitle;
        if(savedStage < 0){
            savedStageTitle = "Empty";
            btn->setColor(Color3B(200,200,200));
        }else{
            btn->setColor(Color3B::WHITE);
            savedStageTitle = strmake("%d. %s", savedStage + 1, LM->getText(strmake("stage title %d", savedStage).c_str()).c_str());
            if(savedStageTitle.size() > 6){
                savedStageTitle = strmake("%d. %s...", savedStage + 1, LM->getText(strmake("stage title %d", savedStage).c_str()).substr(0, 6).c_str());
            }
        }
        
        lblTitle = PPLabel::create(savedStageTitle, 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        lblTitle->setName("effect");
        layer->addChild(lblTitle);
        
        lblTime = PPLabel::create(GM->getTimeLeftInString(savedTime), 40, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        layer->addChild(lblTime);
        if(i == 0){
            btn->setPosition(Vec2(size.width/2 - 300, firstLineY));
        }else if(i == 1){
            btn->setPosition(Vec2(size.width/2 + 300, firstLineY));
        }else if(i == 2){
            btn->setPosition(Vec2(size.width/2 - 300, firstLineY - gapY));
        }else if(i == 3){
            btn->setPosition(Vec2(size.width/2 + 300, firstLineY - gapY));
        }
        lblTitle->setPosition(btn->getPosition() + Vec2(0, 35));
        lblTime->setPosition(btn->getPosition() + Vec2(0, -35));
        if(savedTime == 0){
            lblTime->setVisible(false);
        }
        
        std::string strData = UDGetStr(strmake("savedDataExtra%d", btn->getTag()).c_str());
        datas = GameManager::getInstance()->split(strData.c_str(), ",");
        if(Value(datas.at(0)).asInt() > 0){
            btn->setColor(Color3B(215, 0, 0));
            lblTitle->setFontColor(Color3B::WHITE);
            lblTime->setFontColor(Color3B::WHITE);
        }
    }
}
void HudLayer::onSaveSlotClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    
    std::string data = UDGetStr(strmake("savedData%d", btn->getTag()).c_str());
    int firstComma = (int)data.find(',');
    int secondComma = (int)data.find(',', firstComma+1);
    data = data.substr(0, secondComma);
    ValueVector datas = GameManager::getInstance()->split(data, ",");
    
    int savedStage = -1;
    int savedTime = 0;
    if(datas.size() > 1){
        log("std: %s", datas.at(0).asString().c_str());
        savedStage = datas.at(0).asInt();
        savedTime = datas.at(1).asInt();
    }
    if (savedStage >= 0) {
        selectedSaveSlot = btn->getTag();
        
        GM->playSoundEffect(SOUND_PAPER_FLIP);
        Layer* layer = Layer::create();
        this->addChild(layer);
        setAsPopup(layer);
        
        Button* btnBlock = Button::create("uiBox.png");
        btnBlock->setPosition(Vec2(size.width/2, size.height/2));
        layer->addChild(btnBlock);
        btnBlock->setScale(20);
        btnBlock->setOpacity(100);
        btnBlock->setColor(Color3B::BLACK);
        btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
        
        ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
        layer->addChild(storyBack);
        storyBack->setScale9Enabled(true);
        storyBack->setPosition(size/2);
        storyBack->setTouchEnabled(true);
        
        PPLabel* lbl = PPLabel::create(LM->getText("overwrite save data"), 50, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        layer->addChild(lbl);
        lbl->setPosition(Vec2(size.width/2, size.height/2 + 300));
        lbl->setWidth(1000);
        
        Button* btn = Button::create("uiBoxSmall.png");
        btn->setScale9Enabled(true);
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 - 300, size.height/2 - 200));
        btn->setContentSize(cocos2d::Size(500, 200));
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOkFromSaveOverwrite, this));
        
        lbl = PPLabel::create(LM->getText("ok"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        layer->addChild(lbl);
        lbl->setPosition(btn->getPosition());
        
        btn = Button::create("uiBoxSmall.png");
        btn->setScale9Enabled(true);
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 + 300, size.height/2 - 200));
        btn->setContentSize(cocos2d::Size(500, 200));
        btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
        
        lbl = PPLabel::create(LM->getText("cancel"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        layer->addChild(lbl);
        lbl->setPosition(btn->getPosition());
        
        return;
    }
    
    
    
    saveCurrentStageData(btn->getTag());
    closePopup();
    onSaveClick();
}

void HudLayer::onOkFromSaveOverwrite(){
    closePopup();
    saveCurrentStageData(selectedSaveSlot);
    closePopup();
    onSaveClick();
}
void HudLayer::saveCurrentStageData(int slot){
    std::string data = "";
    data.append(Value(GM->currentStageIndex).asString() + ",");
    data.append(Value(WORLD->lastTick).asString() + ",");
    data.append(Value(WORLD->gold).asString() + ",");
    data.append(Value(WORLD->lumber).asString() + ",");
    data.append(Value(WORLD->getPosition().x).asString() + ",");
    data.append(Value(WORLD->getPosition().y).asString() + ",");
    
    for (auto enemy: WORLD->heroArray){
        data.append(Value(enemy->unitType).asString() + ",");
        data.append(enemy->sptName + ",");
        data.append(Value(enemy->maxEnergy).asString() + ",");
        data.append(Value(enemy->energy).asString() + ",");
        data.append(Value(enemy->getName()).asString() + ",");
        data.append(Value(enemy->alliSide).asString() + ",");
        data.append(Value((int)enemy->getPosition().x).asString() + ",");
        data.append(Value((int)enemy->getPosition().y).asString() + ",");
        data.append(Value(enemy->isBuilding).asString() + ",");
        data.append(Value(enemy->scheduledAttackTime).asString() + ",");
        data.append(Value(enemy->isAlli).asString() + ",");
        data.append(Value(enemy->canFindTarget).asString() + ",");
        data.append(Value(enemy->scheduledProductUnit).asString() + ",");
        data.append(Value(enemy->scheduledProductUnitCount).asString() + ",");
        data.append(Value(enemy->scheduledProductTime).asString() + ",");
        data.append(Value(enemy->btns.size() > 0?enemy->btns.at(0)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 1?enemy->btns.at(1)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 2?enemy->btns.at(2)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 3?enemy->btns.at(3)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 4?enemy->btns.at(4)->getTag():-1).asString() + ",");
        data.append(Value(enemy->isZombie).asString() + ",");
    }
    for (auto enemy: WORLD->mutualArray){
        data.append(Value(enemy->unitType).asString() + ",");
        data.append(enemy->sptName + ",");
        data.append(Value(enemy->maxEnergy).asString() + ",");
        data.append(Value(enemy->energy).asString() + ",");
        data.append(Value(enemy->getName()).asString() + ",");
        data.append(Value(enemy->alliSide).asString() + ",");
        data.append(Value((int)enemy->getPosition().x).asString() + ",");
        data.append(Value((int)enemy->getPosition().y).asString() + ",");
        data.append(Value(enemy->isBuilding).asString() + ",");
        data.append(Value(enemy->scheduledAttackTime).asString() + ",");
        data.append(Value(enemy->isAlli).asString() + ",");
        data.append(Value(enemy->canFindTarget).asString() + ",");
        data.append(Value(enemy->scheduledProductUnit).asString() + ",");
        data.append(Value(enemy->scheduledProductUnitCount).asString() + ",");
        data.append(Value(enemy->scheduledProductTime).asString() + ",");
        data.append(Value(enemy->btns.size() > 0?enemy->btns.at(0)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 1?enemy->btns.at(1)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 2?enemy->btns.at(2)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 3?enemy->btns.at(3)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 4?enemy->btns.at(4)->getTag():-1).asString() + ",");
        data.append(Value(enemy->isZombie).asString() + ",");
    }
    for (auto enemy: WORLD->readyHeroArray){
        data.append(Value(enemy->unitType).asString() + ",");
        data.append(enemy->sptName + ",");
        data.append(Value(enemy->maxEnergy).asString() + ",");
        data.append(Value(enemy->energy).asString() + ",");
        data.append(Value(enemy->getName()).asString() + ",");
        data.append(Value(enemy->alliSide).asString() + ",");
        data.append(Value((int)enemy->getPosition().x).asString() + ",");
        data.append(Value((int)enemy->getPosition().y).asString() + ",");
        data.append(Value(enemy->isBuilding).asString() + ",");
        data.append(Value(enemy->scheduledAttackTime).asString() + ",");
        data.append(Value(enemy->isAlli).asString() + ",");
        data.append(Value(enemy->canFindTarget).asString() + ",");
        data.append(Value(enemy->scheduledProductUnit).asString() + ",");
        data.append(Value(enemy->scheduledProductUnitCount).asString() + ",");
        data.append(Value(enemy->scheduledProductTime).asString() + ",");
        data.append(Value(enemy->btns.size() > 0?enemy->btns.at(0)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 1?enemy->btns.at(1)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 2?enemy->btns.at(2)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 3?enemy->btns.at(3)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 4?enemy->btns.at(4)->getTag():-1).asString() + ",");
        data.append(Value(enemy->isZombie).asString() + ",");
    }
    
    int counter = 0;
    for (auto enemy: WORLD->enemyArray){
        data.append(Value(enemy->unitType).asString() + ",");
        data.append(enemy->sptName + ",");
        data.append(Value(enemy->maxEnergy).asString() + ",");
        data.append(Value(enemy->energy).asString() + ",");
        data.append(Value(enemy->getName()).asString() + ",");
        data.append(Value(enemy->alliSide).asString() + ",");
        data.append(Value((int)enemy->getPosition().x).asString() + ",");
        data.append(Value((int)enemy->getPosition().y).asString() + ",");
        data.append(Value(enemy->isBuilding).asString() + ",");
        data.append(Value(enemy->scheduledAttackTime).asString() + ",");
        data.append(Value(enemy->isAlli).asString() + ",");
        data.append(Value(enemy->canFindTarget).asString() + ",");
        data.append(Value(enemy->scheduledProductUnit).asString() + ",");
        data.append(Value(enemy->scheduledProductUnitCount).asString() + ",");
        data.append(Value(enemy->scheduledProductTime).asString() + ",");
        data.append(Value(enemy->btns.size() > 0?enemy->btns.at(0)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 1?enemy->btns.at(1)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 2?enemy->btns.at(2)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 3?enemy->btns.at(3)->getTag():-1).asString() + ",");
        data.append(Value(enemy->btns.size() > 4?enemy->btns.at(4)->getTag():-1).asString() + ",");
        
        data.append(Value(enemy->isZombie).asString());
        counter++;
        if(counter < WORLD->enemyArray.size()){
            data.append(",");
        }
    }
    
    UDSetStr(strmake("savedData%d", slot).c_str(), data);
    
    std::string strDataExtra = strmake("%d,", WORLD->isHardMode);
    std::string strFog = "";
    for(auto fog: WORLD->fogArray){
        strFog += Value(fog->appliedState).asString();
    }
    strFog += ",";
    strDataExtra = strmake("%s%s", strDataExtra.c_str(), strFog.c_str());
    UDSetStr(strmake("savedDataExtra%d", slot).c_str(), strDataExtra);
}

void HudLayer::onOkFromLoadData(){
    GM->isLoadingGame = true;
    GM->loadingSlot = selectedSaveSlot;
    
//    removeListener();
    
    Button* btn = Button::create("uiBoxSmall.png");
    this->addChild(btn, 1000);
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    btn->setPosition(size/2);
    
    Label* lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
    this->addChild(lbl, 1001);
    lbl->setPosition(size/2);
    
    this->runAction(Sequence::create(DelayTime::create(0.07f), CallFunc::create(CC_CALLBACK_0(HudLayer::goToLoadedStage, this)), NULL));
}
void HudLayer::goToLoadedStage(){
    std::string data = UDGetStr(strmake("savedData%d", selectedSaveSlot).c_str());
    int firstComma = (int)data.find(',');
    int secondComma = (int)data.find(',', firstComma+1);
    data = data.substr(0, secondComma);
    ValueVector datas = GameManager::getInstance()->split(data, ",");
    int savedStage = -1;
    int savedTime = 0;
    if(datas.size() > 1){
        savedStage = datas.at(0).asInt();
        savedTime = datas.at(1).asInt();
    }
    setGameSpeed(1);
    this->removeListener();
    GM->nextScene = STAGE_FIELD;
    GM->setHudLayer(nullptr);
    
    std::string strData = UDGetStr(strmake("savedDataExtra%d", selectedSaveSlot).c_str());
    datas = GameManager::getInstance()->split(strData.c_str(), ",");
    bool isHardMode = false;
    if(datas.size() > 0){
        isHardMode = Value(datas.at(0)).asInt();
    }
    GM->isThisCampaignFromDailyMission = false;
    auto scene = HelloWorld::scene(savedStage, isHardMode);
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void HudLayer::setGameSpeed(float speed){
    WORLD->gameSpeed = speed;
    Director::getInstance()->setTimeScale(speed);
}
void HudLayer::onLoadClick(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Vec2(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lblTitle;
    PPLabel* lblTime;
    Button* btn;
    float firstLineY = size.height/2;
    float gapY = 260;
    PPLabel* lblLoad = PPLabel::create(LM->getText("load"), 100, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lblLoad);
    lblLoad->setPosition(Vec2(size.width/2, size.height/2 + 360));
    for (int i = 0; i < 4; i++) {
        btn = Button::create("uiBoxSmall.png");
        btn->setScale9Enabled(true);
        layer->addChild(btn);
        btn->setContentSize(cocos2d::Size(500, 200));
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onLoadSlotClick, this));
        btn->setTag(i);

        std::string data = UDGetStr(strmake("savedData%d", btn->getTag()).c_str());
        int firstComma = (int)data.find(',');
        int secondComma = (int)data.find(',', firstComma+1);
        data = data.substr(0, secondComma);
        ValueVector datas = GameManager::getInstance()->split(data, ",");
        
        int savedStage = -1;
        int savedTime = 0;
        if(datas.size() > 1){
            log("std: %s", datas.at(0).asString().c_str());
            savedStage = datas.at(0).asInt();
            savedTime = datas.at(1).asInt();
        }else{
            btn->setEnabled(false);
        }
        std::string savedStageTitle;
        if(savedStage < 0){
            savedStageTitle = "Empty";
            btn->setColor(Color3B(200,200,200));
        }else{
            btn->setColor(Color3B::WHITE);
            savedStageTitle = strmake("%d. %s", savedStage + 1, LM->getText(strmake("stage title %d", savedStage).c_str()).c_str());
            if(savedStageTitle.size() > 6){
                savedStageTitle = strmake("%d. %s...", savedStage + 1, LM->getText(strmake("stage title %d", savedStage).c_str()).substr(0, 6).c_str());
            }
        }
        
        lblTitle = PPLabel::create(savedStageTitle, 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        lblTitle->setName("effect");
        layer->addChild(lblTitle);
        
        lblTime = PPLabel::create(GM->getTimeLeftInString(savedTime), 40, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        layer->addChild(lblTime);
        if(i == 0){
            btn->setPosition(Vec2(size.width/2 - 300, firstLineY));
        }else if(i == 1){
            btn->setPosition(Vec2(size.width/2 + 300, firstLineY));
        }else if(i == 2){
            btn->setPosition(Vec2(size.width/2 - 300, firstLineY - gapY));
        }else if(i == 3){
            btn->setPosition(Vec2(size.width/2 + 300, firstLineY - gapY));
        }
        lblTitle->setPosition(btn->getPosition() + Vec2(0, 35));
        lblTime->setPosition(btn->getPosition() + Vec2(0, -35));
        if(savedTime == 0){
            lblTime->setVisible(false);
        }
        std::string strData = UDGetStr(strmake("savedDataExtra%d", btn->getTag()).c_str());
        datas = GameManager::getInstance()->split(strData.c_str(), ",");
        if(datas.size() > 0 && Value(datas.at(0)).asInt()){
            btn->setColor(Color3B(215, 0, 0));
            lblTitle->setFontColor(Color3B::WHITE);
            lblTime->setFontColor(Color3B::WHITE);
        }
    }
}
void HudLayer::onLoadSlotClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    selectedSaveSlot = btn->getTag();
    
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Vec2(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lbl = PPLabel::create(LM->getText("load data"), 50, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lbl);
    lbl->setPosition(Vec2(size.width/2, size.height/2 + 300));
    lbl->setWidth(1000);
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Vec2(size.width/2 - 300, size.height/2 - 200));
    btn->setContentSize(cocos2d::Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOkFromLoadData, this));
    
    lbl = PPLabel::create(LM->getText("ok"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("effect");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Vec2(size.width/2 + 300, size.height/2 - 200));
    btn->setContentSize(cocos2d::Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    
    lbl = PPLabel::create(LM->getText("cancel"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("music");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
}

void HudLayer::onEffectClick(){
    float vol = GM->getSoundVolumn();
    PPLabel* lbl = (PPLabel*)getPopup()->getChildByName("effect");
    if(vol == 0){
        GM->setSoundVolumn(1);
        lbl->setString(LM->getText("effect on"));
    }else{
        GM->setSoundVolumn(0);
        lbl->setString(LM->getText("effect off"));
    }
}
void HudLayer::onMusicClick(){
    float vol = GM->getMusicVolumn();
    PPLabel* lbl = (PPLabel*)getPopup()->getChildByName("music");
    if(vol == 0){
        GM->setMusicVolumn(0.3f);
        lbl->setString(LM->getText("music on"));
    }else{
        GM->setMusicVolumn(0);
        lbl->setString(LM->getText("music off"));
    }
}
void HudLayer::onResumeClick(){
    closePopup();
    WORLD->resumeLayer();
}
void HudLayer::goToTitleScene(){
    setGameSpeed(1);
    removeListener();
    if(GM->isAdsUser()){
        GameSharing::showInterstitial();
    }
    GM->setHudLayer(nullptr);
    auto scene = Scene::create();
    scene->addChild(Title::create());
    GM->isThisCampaignFromDailyMission = false;
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void HudLayer::onOkFromRaidWinPopup(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    goToBattleScene();
}
void HudLayer::goToBattleScene(){
    setGameSpeed(1);
    removeListener();
    if(GM->isAdsUser()){
        GameSharing::showInterstitial();
    }
    GM->isThisCampaignFromDailyMission = false;
    GM->setHudLayer(nullptr);
    GM->nextScene = STAGE_LOBBY;
    auto scene = HelloWorld::scene(STAGE_LOBBY, false);
    Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
}
void HudLayer::onExitClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    GameSharing::logFB(strmake("STAGE %d QUIT", GM->currentStageIndex).c_str());
//    Director::getInstance()->popScene();
    goToTitleScene();
}
void HudLayer::talkBoxUpdate(float dt){
    if(tutorialEnded){
        this->unschedule(schedule_selector(HudLayer::talkBoxUpdate));
        tutorialNode->removeFromParent();
        tutorialNode = nullptr;
        return;
    }
    if (talkIndex == 0) {
        tutorialNode->setVisible(true);
        Label* lbl = (Label*)tutorialNode->getChildByName("lbl");
        lbl->setString("");
        if(tutorialHowTo != nullptr){
            tutorialHowTo->setVisible(false);
            this->unschedule(schedule_selector(HudLayer::updateTutorialHowTo));
        }
    }
    Label* lbl = (Label*)tutorialNode->getChildByName("lbl");
    if(talkIndex > talkText.size()){
        lbl->setString(talkText);
        return;
    }
    std::string strPrevious = lbl->getString();
    lbl->setString(talkText.substr(0, talkIndex));
    if(lbl->getBoundingBox().size.width == 0){
        lbl->setString(strPrevious);
    }
    
    
//    talkIndex++;
    talkIndex += 2;
}
void HudLayer::showPriceInfo(std::string msg, int gold, int lumber, int food){
    priceInfo->stopAllActions();
    priceInfo->setVisible(true);
    priceInfo->setScale(0.8f);
    priceInfo->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1)));
    priceInfo->runAction(Sequence::create(DelayTime::create(3), CallFunc::create(CC_CALLBACK_0(HudLayer::hidePriceInfo, this)), NULL));
    
    PPLabel* lbl = (PPLabel*)priceInfo->getChildByName("lblDescription");
    lbl->setString(msg);
    lbl->setVisible(msg.size() > 0);
    
    lbl = (PPLabel*)priceInfo->getChildByName("lblGold");
    lbl->setString(Value(gold).asString());
    lbl->setVisible(gold > 0);
    priceInfo->getChildByName("sptGold")->setVisible(gold > 0);
    
    lbl = (PPLabel*)priceInfo->getChildByName("lblLumber");
    lbl->setString(Value(lumber).asString());
    lbl->setVisible(lumber > 0);
    priceInfo->getChildByName("sptLumber")->setVisible(lumber > 0);
    
    lbl = (PPLabel*)priceInfo->getChildByName("lblFood");
    lbl->setString(Value(food).asString());
    lbl->setVisible(food > 0);
    priceInfo->getChildByName("sptFood")->setVisible(food > 0);
}
void HudLayer::hidePriceInfo(){
    priceInfo->setVisible(false);
}
Label* HudLayer::getLabel(std::string txt, int fontSize){
    Label* lbl = Label::createWithSystemFont(txt, "Thonburi", fontSize);
    lbl->enableShadow();
    lbl->setHorizontalAlignment(TextHAlignment::CENTER);
    lbl->setVerticalAlignment(TextVAlignment::CENTER);
//    lbl->setTextColor(Color4B::RED);
    return lbl;
}
void HudLayer::setMenu(int index, int btnType){
    if(GM->currentStageIndex == STAGE_LOBBY) return;
    Node* btnMenu = WORLD->getChildByName(strmake("btnMenu%d", index));
    Button* btn = (Button*)btnMenu->getChildByName("btn");
    btn->removeChildByName("sptSelected");
    btn->setTag(btnType);
    btnMenu->setVisible(btnType != BTN_TYPE_NONE);
    btn->removeChildByName("icon");
    Sprite* sptBuilding = nullptr;
    btnMenu->getChildByName("lblGold")->setVisible(btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL);
    btnMenu->getChildByName("lblLumber")->setVisible(btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL);
    int unitIndex = -1;
    btn->loadTextures("uiBox.png", "uiBox.png");
    Text* lbl = (Text*)btnMenu->getChildByName("lbl");
    LM->setLocalizedStringNotKey(lbl, "");
    selectedMenuName = "-1";
    if(btnType ==  BTN_TYPE_MOVE ){
        btn->loadTextures("btnMove.png", "btnMove.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "move");
    }
    else if(btnType ==  BTN_TYPE_STOP  ){
        btn->loadTextures("btnStop.png", "btnStop.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "stop");
    }else if(btnType ==  BTN_TYPE_ATTACK  ){
        btn->loadTextures("btnAttack.png", "btnAttack.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "attack");
    }
    else if(btnType ==  BTN_TYPE_GATHER  ){
        btn->loadTextures("btnGather.png", "btnGather.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "gather");
    }
    else if(btnType ==  BTN_TYPE_BUILD  ){
        btn->loadTextures("btnBuild.png", "btnBuild.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "build");
    }
    else if(btnType ==  BTN_TYPE_BUILD_BETTER  ){
        btn->loadTextures("btnBuildBetter.png", "btnBuildBetter.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "build");
    }else if(btnType ==  BTN_TYPE_CANCEL){
        btn->loadTextures("btnStop.png", "btnStop.png");
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), "cancel");
    }
    else if(btnType ==  BTN_TYPE_WORKER  ){
        unitIndex = UNIT_WORKER;
        lbl->setString(LM->getText("worker"));
    }else if(btnType ==  BTN_TYPE_GOBLIN_WORKER  ){
        unitIndex = UNIT_GOBLIN_WORKER;
        lbl->setString(LM->getText("worker"));
    }
    else if(btnType ==  BTN_TYPE_SWORDMAN ){
        unitIndex = UNIT_SWORDMAN;
        lbl->setString(LM->getText("swordman"));
    }
    else if(btnType ==  BTN_TYPE_ARCHER  ){
        unitIndex = UNIT_ARCHER;
        lbl->setString(LM->getText("archer"));
    }
    else if(btnType ==  BTN_TYPE_HELICOPTER  ){
        unitIndex = UNIT_HELICOPTER;
        lbl->setString(LM->getText("helicopter"));
    }
    else if(btnType ==  BTN_TYPE_CATAPULT  ){
        unitIndex = UNIT_CATAPULT;
        lbl->setString(LM->getText("catapult"));
    }
    else if(btnType ==  BTN_TYPE_CASTLE  ){
        unitIndex = UNIT_CASTLE;
        lbl->setString(LM->getText("castle"));
    }
    else if(btnType ==  BTN_TYPE_FARM  ){
        unitIndex = UNIT_FARM;
        lbl->setString(LM->getText("farm"));
    }
    else if(btnType ==  BTN_TYPE_BARRACKS  ){
        unitIndex = UNIT_BARRACKS;
        lbl->setString(LM->getText("barracks"));
    }
    else if(btnType ==  BTN_TYPE_LUMBER  ){
        unitIndex = UNIT_LUMBERMILL;
        lbl->setString(LM->getText("lumbermill"));
    }
    else if(btnType ==  BTN_TYPE_WATCHER_TOWER  ){
        unitIndex = UNIT_WATCHERTOWER;
        lbl->setString(LM->getText("watcher tower"));
    }
    else if(btnType ==  BTN_TYPE_FACTORY  ){
        unitIndex = UNIT_FACTORY;
        lbl->setString(LM->getText("factory"));
    }
    else if(btnType ==  BTN_TYPE_AIRPORT  ){
        unitIndex = UNIT_AIRPORT;
        lbl->setString(LM->getText("airport"));
    }
    else if(btnType ==  BTN_TYPE_HQ  ){
        unitIndex = UNIT_ORC_HQ;
        lbl->setString(LM->getText("orc hq"));
    }else if(btnType ==  BTN_TYPE_BUNKER){
        unitIndex = UNIT_ORC_BUNKER;
        lbl->setString(LM->getText("orc bunker"));
    }else if(btnType ==  BTN_TYPE_ORC_BARRACKS){
        unitIndex = UNIT_ORC_BARRACKS;
        lbl->setString(LM->getText("orc barracks"));
    }else if(btnType ==  BTN_TYPE_TROLL_HOUSE){
        unitIndex = UNIT_ORC_TROLL_HOUSE;
        lbl->setString(LM->getText("orc troll house"));
    }else if(btnType ==  BTN_TYPE_TEMPLE){
        unitIndex = UNIT_TEMPLE;
        lbl->setString(LM->getText("temple"));
    }else if(btnType ==  BTN_TYPE_BARBECUE){
        unitIndex = UNIT_BARBECUE;
        lbl->setString(LM->getText("barbecue"));
    }else if(btnType ==  BTN_TYPE_GOBLIN){
        unitIndex = UNIT_GOBLIN;
        lbl->setString(LM->getText("goblin"));
    }else if(btnType ==  BTN_TYPE_GOBLIN_BOMB){
        unitIndex = UNIT_GOBLIN_BOMB;
        lbl->setString(LM->getText("goblin bomb"));
    }else if(btnType ==  BTN_TYPE_ORC_AXE){
        unitIndex = UNIT_ORC_AXE;
        lbl->setString(LM->getText("orc axe"));
    }else if(btnType ==  BTN_TYPE_ORC_SPEAR){
        unitIndex = UNIT_ORC_SPEAR;
        lbl->setString(LM->getText("orc spear"));
    }else if(btnType ==  BTN_TYPE_TROLL){
        unitIndex = UNIT_TROLL;
        lbl->setString(LM->getText("troll"));
    }else if(btnType ==  BTN_TYPE_MAGE){
        unitIndex = UNIT_WIZARD;
        lbl->setString(LM->getText("wizard"));
    }
    lbl->setVisible(lbl->getString().length() > 0);
    float isToBuy = btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL;
    btnMenu->getChildByName("lblGold")->setVisible(isToBuy);
    btnMenu->getChildByName("lblLumber")->setVisible(isToBuy);
    btnMenu->getChildByName("imgGold")->setVisible(isToBuy);
    btnMenu->getChildByName("imgLumber")->setVisible(isToBuy);
    btnMenu->getChildByName("imgResourceBack")->setVisible(isToBuy);
    ImageView* imgTitleBack = (ImageView*)btnMenu->getChildByName("imgTitleBack");
    imgTitleBack->setVisible(isToBuy);
    lbl->ignoreContentAdaptWithSize(true);
    
    if (lbl->getBoundingBox().size.width > 238.28) {
        log("size overflow %s", lbl->getString().c_str());
        imgTitleBack->setContentSize(cocos2d::Size(238.28, 49.56*2));
        lbl->setTextAreaSize(cocos2d::Size(322.82, 217.05));
        lbl->ignoreContentAdaptWithSize(false);
    }else{
        imgTitleBack->setContentSize(cocos2d::Size(238.28, 49.56));
    }
    
    if(isToBuy){
//        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), WORLD->getUnitName(unitIndex));
        ((Text*)btn->getParent()->getChildByName("lblGold"))->setString(Value(WORLD->getGoldPriceForUnit(unitIndex)).asString());
        ((Text*)btn->getParent()->getChildByName("lblLumber"))->setString(Value(WORLD->getLumberPriceForUnit(unitIndex)).asString());
        
        sptBuilding = WORLD->getSpriteForIcon(unitIndex);
    }
    if(sptBuilding != nullptr){
        btn->addChild(sptBuilding);
        sptBuilding->setName("icon");
        sptBuilding->setPosition(btn->getContentSize()/2);
    }
}
void HudLayer::onCommandClick(Ref* ref){
    if(WORLD->buildingTemplate != nullptr){
        WORLD->buildingTemplate->removeFromParent();
        WORLD->buildingTemplate = nullptr;
    }
    Button* btn = (Button*)ref;
    std::string name = btn->getParent()->getName();
    int index = Value(name.substr(7)).asInt();
    index = btn->getTag();
    
    if(index != BTN_TYPE_BUILD && index != BTN_TYPE_BUILD_BETTER && index != BTN_TYPE_CANCEL){
//        if(selectedMenuName.compare(name) != 0){
//            if(selectedMenuName.compare("-1") != 0){
//                Button* btnPrevious = (Button*)WORLD->getChildByName(selectedMenuName);
//                btnPrevious->removeChildByName("sptSelected");
//            }
//
//            Sprite* sptSelected = Sprite::create("uiBoxSelected.png");
//            sptSelected->setName("sptSelected");
//            btn->addChild(sptSelected);
//            sptSelected->setPosition(btn->getContentSize()/2);
//            selectedMenuName = name;
//
//            rightBottomPanelForCampaign->getChildByName("ndMission")->setVisible(false);
//            rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(true);
//            WORLD->setPriceInfo(index);
//            return;
//        }
    }
    
    bool shouldHideMenu = false;
    if(index == BTN_TYPE_MOVE){
        WORLD->onMoveClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_STOP){
        WORLD->onStopClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_ATTACK){
        WORLD->onAttackClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_GATHER){
        WORLD->onGatherClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_BUILD){
        WORLD->onBuildingClick();
        if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 5) {
            HUD->tutorialIndex++;
            HUD->talkIndex = 0;
            HUD->talkText = LM->getText("tutorial 6");
            Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
            spt->stopAllActions();
            spt->setSpriteFrame("farm.png");
            spt->setScale(1/WORLD->imageScale);
            WORLD->addGold(400);
            WORLD->addLumber(200);
        }
    }else if(index == BTN_TYPE_BUILD_BETTER){
        WORLD->onBuildingBetterClick();
    }else if(index == BTN_TYPE_FARM){
        if(WORLD->tryBuilding(UNIT_FARM)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_CASTLE){
        if(WORLD->tryBuilding(UNIT_CASTLE)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_BARRACKS){
        if(WORLD->tryBuilding(UNIT_BARRACKS)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_WATCHER_TOWER){
        if(WORLD->tryBuilding(UNIT_WATCHERTOWER)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_LUMBER){
        if(WORLD->tryBuilding(UNIT_LUMBERMILL)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_AIRPORT){
        if(WORLD->tryBuilding(UNIT_AIRPORT)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_FACTORY){
        if(WORLD->tryBuilding(UNIT_FACTORY)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_HQ){
        if(WORLD->tryBuilding(UNIT_ORC_HQ)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_BUNKER){
        if(WORLD->tryBuilding(UNIT_ORC_BUNKER)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_ORC_BARRACKS){
        if(WORLD->tryBuilding(UNIT_ORC_BARRACKS)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_TROLL_HOUSE){
        if(WORLD->tryBuilding(UNIT_ORC_TROLL_HOUSE)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_TEMPLE){
        if(WORLD->tryBuilding(UNIT_TEMPLE)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_BARBECUE){
        if(WORLD->tryBuilding(UNIT_BARBECUE)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_CANCEL){
        setMenu(0, BTN_TYPE_BUILD);
        setMenu(1, BTN_TYPE_BUILD_BETTER);
        setMenu(2, BTN_TYPE_NONE);
        setMenu(3, BTN_TYPE_NONE);
        setMenu(4, BTN_TYPE_NONE);
        setMenu(5, BTN_TYPE_NONE);
        WORLD->selectedCommand = COMMAND_NOTHING;
        rightBottomPanelForCampaign->getChildByName("ndMission")->setVisible(true);
        rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(false);
    }else if(index == BTN_TYPE_WORKER){
        WORLD->tryCreateUnit(UNIT_WORKER);
    }else if(index == BTN_TYPE_GOBLIN_WORKER){
        WORLD->tryCreateUnit(UNIT_GOBLIN_WORKER);
    }else if(index == BTN_TYPE_SWORDMAN){
        WORLD->tryCreateUnit(UNIT_SWORDMAN);
    }else if(index == BTN_TYPE_ARCHER){
        WORLD->tryCreateUnit(UNIT_ARCHER);
    }else if(index == BTN_TYPE_HELICOPTER){
        WORLD->tryCreateUnit(UNIT_HELICOPTER);
    }else if(index == BTN_TYPE_CATAPULT){
        WORLD->tryCreateUnit(UNIT_CATAPULT);
    }else if(index == BTN_TYPE_GOBLIN_BOMB){
        WORLD->tryCreateUnit(UNIT_GOBLIN_BOMB);
    }else if(index == BTN_TYPE_GOBLIN){
        WORLD->tryCreateUnit(UNIT_GOBLIN);
    }else if(index == BTN_TYPE_TROLL){
        WORLD->tryCreateUnit(UNIT_TROLL);
    }else if(index == BTN_TYPE_MAGE){
        WORLD->tryCreateUnit(UNIT_WIZARD);
    }else if(index == BTN_TYPE_ORC_AXE){
        WORLD->tryCreateUnit(UNIT_ORC_AXE);
    }else if(index == BTN_TYPE_ORC_SPEAR){
        WORLD->tryCreateUnit(UNIT_ORC_SPEAR);
    }
    if(shouldHideMenu){
        HUD->setMenu(0, BTN_TYPE_NONE);
        HUD->setMenu(1, BTN_TYPE_NONE);
        HUD->setMenu(2, BTN_TYPE_NONE);
        HUD->setMenu(3, BTN_TYPE_NONE);
        HUD->setMenu(4, BTN_TYPE_NONE);
        HUD->setMenu(5, BTN_TYPE_NONE);
    }
}
void HudLayer::setExp(int level, int exp, int expMax){
    LoadingBar* pb = (LoadingBar*)hudLayer->getChildByName("pbExp");
    float percent = exp*100.0f/expMax;
    pb->setPercent(percent);
    
    Text* lbl = (Text*)hudLayer->getChildByName("lblLevel");
    lbl->setString(Value(level).asString());
}
void HudLayer::setEnergy(int hp, int max){
    LoadingBar* pbHP = (LoadingBar*)hudLayer->getChildByName("pbHP");
    float percent = hp*100.0f/max;
    pbHP->setPercent(percent);
    pbHP->stopAllActions();
    pbHP->setOpacity(255);
    if(percent < 30){
        pbHP->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    
    Text* lblHP = (Text*)hudLayer->getChildByName("lblHP");
    lblHP->setString(strmake("%d/%d", hp, max));
}

void HudLayer::showInfo(std::string info){
    lblInfo->setVisible(true);
    lblInfo->setString(strmake("[%s]", info.c_str()));
}
void HudLayer::hideInfo(){
    lblInfo->setVisible(false);
}
void HudLayer::showMap(){
    
}
void HudLayer::onSelectClick(){
    
}

void HudLayer::onResetClick(){
    
    
}
void HudLayer::processReset(){
    
}

void HudLayer::addLabelToButton(std::string text, Button* btn, bool bordered, Color3B color, bool isSystemLabel){
    PPLabel* lbl = PPLabel::create(LM->getText(text.c_str()), btn->getContentSize().height*2/6, color, isSystemLabel, bordered, TextHAlignment::CENTER, false);
    lbl->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2));
    btn->addChild(lbl);
}
Sprite* HudLayer::addSpriteToButton(std::string imgFileName, Button* btn, Button::TextureResType type){
    Sprite* spt;
    if(type == Button::TextureResType::PLIST){
        spt = Sprite::createWithSpriteFrameName(imgFileName);
    }else{
        spt = Sprite::create(imgFileName);
    }
    spt->setName("sprite");
    btn->addChild(spt);
    spt->setPosition(btn->getContentSize()/2);
    return spt;
}
Sprite* HudLayer::addSpriteToButton(std::string itemName, Button* btn){
    Sprite* spt = getIconSprite(itemName);
    spt->setName("sprite");
    btn->addChild(spt);
    spt->setPosition(btn->getContentSize()/2);
    return spt;
}
Sprite* HudLayer::getIconSprite(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getItemStat(itemName, "sprite"));
    spt->setName("sprite");
    cocos2d::Rect textureRect = spt->getTextureRect();
    if(textureRect.size.width > TILE_SIZE || textureRect.size.height > TILE_SIZE){
        float textureWidth = textureRect.size.width;
        if(textureWidth > TILE_SIZE){
            textureWidth = TILE_SIZE;
        }
        float textureHeight = textureRect.size.height;
        if(textureHeight > TILE_SIZE){
            textureHeight = TILE_SIZE;
        }
        spt->setTextureRect(cocos2d::Rect(textureRect.origin.x + spt->getContentSize().width/2 - textureWidth/2, textureRect.getMaxY() - textureHeight, textureWidth, textureHeight));
    }
    
    Sprite* frameBack = Sprite::createWithSpriteFrameName("icon_frame.png");
    frameBack->addChild(spt);
    spt->setPosition(frameBack->getContentSize()/2);
    Sprite* frameFront = Sprite::createWithSpriteFrameName("iconBorder.png");
    frameBack->addChild(frameFront);
    frameFront->setPosition(frameBack->getContentSize()/2);
    frameBack->setName("icon");
    return frameBack;
}
void HudLayer::onStartClick(){
    
}

void HudLayer::showBtns(){
    for (auto btn: menus){
        btn->runAction(MoveBy::create(0.1f, Vec2(0, 1300)));
    }
}
void HudLayer::hideBtns(){
    for (auto btn: menus){
        btn->runAction(MoveBy::create(0.1f, Vec2(0, -1300)));
    }
//    joystickLR->runAction(MoveBy::create(0.1f, Vec2(0, -300)));
}
void HudLayer::toggleAuto(bool showMsg){
    bool isAuto = !GameManager::getInstance()->getWorld()->isAutoTargetingOn;
    GameManager::getInstance()->getWorld()->isAutoTargetingOn = isAuto;
    
//    Sprite* spt = Sprite::create(isAuto?"UI/btnToggleManual.png":"UI/btnToggleAuto.png");
//    btnToggle->setSpriteFrame(spt->getSpriteFrame());
    UserDefault::getInstance()->setBoolForKey(KEY_AUTO_TARGET_SELECTED, isAuto);
    if (showMsg) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText(isAuto?"auto targeting":"manual targeting").c_str(), this);
    }
}

void HudLayer::registerControllerListener()
{
    
    bool okay = true;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    float version = NativeInterface::NativeInterface::getSystemVersion();
//    CCLOG("** iOS Version: %f", version);
//    if (version < 7) {
//        okay = false;
//    }
#endif
    if (okay) {
        //create an evnetListenerController
        _listener = EventListenerController::create();
        
        //bind onConneected event call function
        _listener->onConnected = CC_CALLBACK_2(HudLayer::onConnectController,this);
        
        //bind disconnect event call function
        _listener->onDisconnected = CC_CALLBACK_2(HudLayer::onDisconnectedController,this);
        
        //bind onKeyDown event call function
        _listener->onKeyDown = CC_CALLBACK_3(HudLayer::onKeyDown, this);
        
        //bind onKeyUp event call function
        _listener->onKeyUp = CC_CALLBACK_3(HudLayer::onKeyUp, this);
        
        //bind onAxis event call function, onAxis will be called when analog stick is changed
        _listener->onAxisEvent = CC_CALLBACK_3(HudLayer::onAxisEvent, this);
        
        //Activate the listener into the event dispatcher
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, this);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //This function should be called for iOS platform
        Controller::startDiscoveryController();
#endif
        
    }
}

//Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
void HudLayer::onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    CCLOG("KeyDown:%d", keyCode);
}

void HudLayer::onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    //You can get the controller by tag, deviceId or devicename if there are multiple controllers
    CCLOG("tag:%d DeviceId:%d DeviceName:%s", controller->getTag(), controller->getDeviceId(), controller->getDeviceName().c_str());
    CCLOG("KeyUp:%d", keyCode);
}

//The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
void HudLayer::onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
//    const auto& keyStatus = controller->getKeyStatus(keyCode);
//    CCLOG("Axis KeyCode:%d Axis Value:%f", keyCode, keyStatus.value);
}

void HudLayer::onConnectController(Controller* controller, Event* event)
{
    CCLOG("Game controller connected");
}

void HudLayer::onDisconnectedController(Controller* controller, Event* event)
{
    CCLOG("Game controller disconnected");
}
void HudLayer::addHeartAnimated(Vec2 pos){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    Sprite* spt = Sprite::create("heart.png");
    spt->setPosition(pos);
    this->addChild(spt);
    spt->runAction(Sequence::create(EaseIn::create(MoveTo::create(1, heart->getPosition() + Vec2(50, 0)), 3), CallFuncN::create(CC_CALLBACK_1(HudLayer::addHeartDone, this)), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
}
void HudLayer::addHeartDone(Ref* obj){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    
    Sprite* spt = Sprite::create("backLight.png");
    spt->setScale(0.5);
    spt->runAction(Sequence::create(RotateBy::create(1, 300), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    
    heart->addChild(spt);
    spt->setPosition(Vec2(heart->getContentSize().width/2, heart->getContentSize().height/2));
    
    
}
// Implementation of the keyboard event callback function prototype
void HudLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || GameManager::getInstance()->getWorld()->indexToWarp != -9999) {
        return;
    }
    if (dialogBox != nullptr) {
        dialogBox->showNextTalk();
        return;
    }
//    CCLOG("Key with keycode %d pressed", keyCode);
    
}
void HudLayer::tryResultOK()
{
   
}
void HudLayer::showTyping(){
    lblTyping = Label::createWithSystemFont("", GameManager::getInstance()->getFont(FONT_VISITOR), 30);
    this->addChild(lblTyping, 1);
    lblTyping->setPosition(Vec2(size.width/2, size.height/4));
    
    Sprite* sptBack = Sprite::create("whiteBigCircle.png");
    this->addChild(sptBack, 0);
    sptBack->setColor(Color3B::BLACK);
    sptBack->setOpacity(160);
    sptBack->setScale(2, 0.5);
    sptBack->stopAllActions();
    sptBack->setPosition(lblTyping->getPosition());
    
    this->schedule(schedule_selector(HudLayer::typingCursorBlinking), 1);
}
void HudLayer::typing(std::string str){
    if (originalString.size() > 9 && str.compare("<-") != 0) {
        return;
    }
    if (str.compare("<-") == 0) {
        originalString = originalString.substr(0, originalString.size() - 1);
    }else{
        originalString += str;
    }
    lblTyping->setString(originalString);
}
void HudLayer::typingCursorBlinking(float dt){
    if (lblTyping->getString().size() != originalString.size()) {
        lblTyping->setString(originalString);
    }else{
        lblTyping->setString(originalString + "_");
    }
}
void HudLayer::showStageTitle(){
    Sprite* sptBlack = Sprite::create("whiteBigCircle.png");
    sptBlack->setColor(Color3B::BLACK);
    this->addChild(sptBlack, 10000);
    sptBlack->setPosition(size/2);
    sptBlack->setScale(5);
    sptBlack->runAction(ScaleTo::create(0.5f, 15));
    
    hideBtns();
    
    this->setVisible(true);
    
    char bufTitle[100];
    char bufStage[50];
    int stageIndex = GameManager::getInstance()->getWorld()->indexToWarp;//GameManager::getInstance()->currentStageIndex;
    if (stageIndex == -1) {
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageLobby").c_str());
    }else{
        if (GameManager::getInstance()->getWorld()->stageIndex >= 0 && !GameManager::getInstance()->getWorld()->isBossMap) {
            sprintf(bufStage, "BOSS");
        }else{
            sprintf(bufStage, "%s %d", LanguageManager::getInstance()->getText("stage").c_str(), stageIndex + 1);
        }
    }
    
    if(stageIndex >= TUTORIAL_STAGE_START && stageIndex <= TUTORIAL_STAGE_END){
        sprintf(bufStage, "%s %d", LanguageManager::getInstance()->getText("lesson").c_str(), stageIndex - TUTORIAL_STAGE_START + 1);
        if (stageIndex == TUTORIAL_STAGE_END) {
            sprintf(bufTitle, "");
            sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("theLastLesson").c_str());
        }else{
            
        }
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("tutorialTitle%d", stageIndex - TUTORIAL_STAGE_START)->getCString()).c_str());
    }else if(stageIndex == STAGE_NAMING){
        sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageTypeYourName").c_str());
        showTyping();
    }else{
        if (GameManager::getInstance()->getWorld()->stageIndex >= 0 && !GameManager::getInstance()->getWorld()->isBossMap) {
            sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("bossTitle%d", stageIndex)->getCString()).c_str());
        }else{
            sprintf(bufTitle, "%s", LanguageManager::getInstance()->getText(__String::createWithFormat("stageTitle%d", stageIndex)->getCString()).c_str());
        }
        
    }
    float y = size.height*3/4 + 20;
    Label* lblTitle = LanguageManager::getInstance()->getLocalizedLabel(bufTitle, Color4B(235, 235, 235, 255));
    lblTitle->setAnchorPoint(Vec2(0.5, 0.5));
    lblTitle->setPosition(Vec2(size.width/2, y));
    this->addChild(lblTitle, 10000);
    lblTitle->setOpacity(0);
    
    
    Label* lblStage = LanguageManager::getInstance()->getLocalizedLabel(bufStage, Color4B(235, 235, 235, 255));
    lblStage->setAnchorPoint(Vec2(0.5, 0.5));
    lblStage->setPosition(Vec2(size.width/2, y - lblTitle->getBoundingBox().size.height - 2));
    this->addChild(lblStage, 10000);
    lblStage->setOpacity(0);
    
    lineLength = lblTitle->getBoundingBox().size.width>lblStage->getBoundingBox().size.width?lblTitle->getBoundingBox().size.width:lblStage->getBoundingBox().size.width;
    currentLineLength =0;
 
    dnTitleLine = DrawNode::create();
    this->addChild(dnTitleLine, 10000);
    dnTitleLine->setPosition(lblTitle->getBoundingBox().origin + Vec2(0, -4));
    
    
    float dur = 0.5f;
    sptWhiteGun = Sprite::create("whiteGun.png");
    sptWhiteGun->setAnchorPoint(Vec2(0.2, 0.3));
    sptWhiteGun->setScale(3);
    sptWhiteGun->setPosition(Vec2(dnTitleLine->getPosition() + Vec2(-60, -13)));
    this->addChild(sptWhiteGun, 10000);
    sptWhiteGun->runAction(Sequence::create(RotateBy::create(dur, 360), CallFunc::create(CC_CALLBACK_0(HudLayer::whiteGunFired, this)), EaseOut::create(RotateBy::create(0.1, -30), 2), RotateBy::create(0.4, 30), DelayTime::create(2.0f), FadeOut::create(1.0f), NULL));
    
    Sprite* sptGunEffect = Sprite::create("whiteGunFireEffect.png");
    sptGunEffect->setAnchorPoint(Vec2(0, 0.5));
    sptGunEffect->setScale(3);
    sptGunEffect->setOpacity(0);
    sptGunEffect->setPosition(sptWhiteGun->getPosition() + Vec2(50, 15));
    sptGunEffect->runAction(Sequence::create(DelayTime::create(dur), FadeIn::create(0.05f), DelayTime::create(0.1f), FadeOut::create(0.05f), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptGunEffect)), NULL));
    this->addChild(sptGunEffect, 10000);
    
    lblTitle->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(1.5f), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblTitle)), NULL));
    lblStage->runAction(Sequence::create(DelayTime::create(dur + 0.5f), FadeIn::create(0.5f), DelayTime::create(1.5f), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, lblStage)), NULL));
    
}
void HudLayer::stageTitleLineUpdate(float dt){
    dnTitleLine->clear();
    float bulletWidth = 10;
    float gap = 4;
    if (currentLineLength > 10) {
        dnTitleLine->drawSolidRect(Vec2::ZERO, Vec2(currentLineLength, 4), Color4F(235.0f/255, 235.0f/255, 235.0f/255, sptWhiteGun->getOpacity()/255.0f));
    }
    if (currentLineLength < lineLength) {
        currentLineLength += lineLength*dt/0.5f;
    }
    if (currentLineLength > lineLength) {
        currentLineLength = lineLength;
    }
    dnTitleLine->drawSolidRect(Vec2(currentLineLength + gap, 0), Vec2(currentLineLength + bulletWidth + gap, 4), Color4F(235.0f/255, 235.0f/255, 235.0f/255, sptWhiteGun->getOpacity()/255.0f));
    
    if (sptWhiteGun->getOpacity() == 0) {
        sptWhiteGun->removeFromParentAndCleanup(true);
        dnTitleLine->removeFromParentAndCleanup(true);
        this->unschedule(schedule_selector(HudLayer::stageTitleLineUpdate));
    }
}
void HudLayer::whiteGunFired(){
    this->schedule(schedule_selector(HudLayer::stageTitleLineUpdate));
}
void HudLayer::showDialog(std::string str){
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(HudLayer::onDialogDone, this));
    call->retain();
    dialogBox->ShowDialog(str.c_str(), this, call);
    hideBtns();
}
void HudLayer::onDialogDone(){
    if(dialogBox != nullptr){
        dialogBox->removeFromParentAndCleanup(true);
        dialogBox = nullptr;
    }
    if(UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false)){
//        showEnding();
    }
}


void HudLayer::onEndCredit(){
    removeListener();
    
    
//    scene->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    
}
void HudLayer::showNextTalk(){
    dialogBox->showNextTalk();
}
void HudLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || GameManager::getInstance()->getWorld()->indexToWarp != -9999) {
        return;
    }
    if(keyCode == EventKeyboard::KeyCode::KEY_Z ||
       keyCode == EventKeyboard::KeyCode::KEY_J ||
       keyCode == EventKeyboard::KeyCode::KEY_DPAD_CENTER){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_E){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_K || keyCode == EventKeyboard::KeyCode::KEY_X){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_C){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP){
        
    }else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
              keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN ){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT ){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT ){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        if(popupArray.size() == 0){
            if (isRaid) {
                goToBattleScene();
            }else{
                if (WORLD->gameMode == GAME_MODE_PVP6 || WORLD->gameMode == GAME_MODE_PVP12) {
                    
                }else{
                    onMenuClick();
                }
            }
        }else{
            closePopup();
            if(popupArray.size() == 0){
                if(WORLD->isPaused){
                    WORLD->resumeLayer();
                }
            }
        }
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        
    }else if(keyCode == EventKeyboard::KeyCode::KEY_Y){
        
    }
//    CCLOG("Key with keycode %d released", keyCode);
}
void HudLayer::useBomb(){
//    if (GameManager::getInstance()->developer) {
//        GameManager::getInstance()->getWorld()->useBomb();
//        return;
//    }
//    int bombCount = GameManager::getInstance()->getBombCount();
//    if (bombCount > 0) {
//        GameManager::getInstance()->getWorld()->useBomb();
//        bombCount--;
//        GameManager::getInstance()->setBombCount(bombCount);
//    }else{
//        suggestBombShop();
//    }
}
void HudLayer::suggestBombShop(){
    // show Bomb layer
    CCLOG("show bomb layer");
}
void HudLayer::suggestPotionShop(){
    // show Potion layer
    CCLOG("show potion layer");
}
void HudLayer::buyPotion(){
    
}
void HudLayer::buyGems(){
    
}
void HudLayer::keyDown(int key) {
    switch (key) {
        case 6: // Z
            GameManager::getInstance()->firePressed = true;
            break;
        case 7: // X
            GameManager::getInstance()->jumpPressed = true;
            break;
        case 8: // C
            GameManager::getInstance()->cPressed = true;
            break;
        case 123:
            GameManager::getInstance()->leftPressed = true;
            break;
        case 124:
            GameManager::getInstance()->rightPressed = true;
            break;
        case 125:
            GameManager::getInstance()->downPressed = true;
            break;
        case 126:
            GameManager::getInstance()->upPressed = true;
            break;
    }
//    CCLOG("%d pressed", key);
}

void HudLayer::keyUp(int key) {
    switch (key) {
        case 6: // Z
            GameManager::getInstance()->firePressed = false;
            break;
        case 7: // X
            GameManager::getInstance()->jumpPressed = false;
            break;
        case 8: // C
            GameManager::getInstance()->cPressed = false;
            break;
        case 123:
            GameManager::getInstance()->leftPressed = false;
            break;
        case 124:
            GameManager::getInstance()->rightPressed = false;
            break;
        case 125:
            GameManager::getInstance()->downPressed = false;
            break;
        case 126:
            GameManager::getInstance()->upPressed = false;
            break;
    }
//    CCLOG("%d released", key);
}
void HudLayer::showTalk(const char* talk){
    if (talkLabel == NULL) {
//        talkLayer = Layer::create();
        talkLabel = Label::createWithSystemFont(talk, GameManager::getInstance()->getFont(FONT_DEFAULT), 30);//Label::createWithSystemFont(talk, GameManager::getInstance()->getFont(FONT_ARIAL), 30);
        
//        talkLayer->addChild(talkLabel);
//        talkLabel->enableOutline(Color4B(80,80,80,255), 7);
        talkLabel->enableShadow();
//        talkLabel->enableGlow(Color4B(200,200,200,255));
//        talkLabel->setColor(Color3B::BLACK);
        this->addChild(talkLabel);
//        talkLabel->setVisible(false);
        
    }else{
//        hideTalk();
    }
//    hideTalk();
//    CCLOG("showTalk: %s", talk);
    talkLabel->stopAllActions();
    talkLabel->setString(talk);
    talkLabel->setPosition(Vec2(size.width/2, size.height/2 + 75));
    talkLabel->setScale(0.1);
//    talkLabel->runAction(Sequence::create(CCFadeIn::create(0.4), DelayTime::create(2), CCFadeOut::create(0.4), NULL));
    talkLabel->runAction(Sequence::create(EaseOut::create(ScaleTo::create(0.4, 1), 8), DelayTime::create(2), EaseIn::create(ScaleTo::create(0.4, 0.1), 8), NULL));
    talkLabel->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.4, Vec2(0, 50)), 8), DelayTime::create(2), EaseIn::create(MoveBy::create(0.4, Vec2(0, -50)), 8), MoveBy::create(0.01, Vec2(0, 1000)),NULL));
//    talkLabel->runAction(Sequence::create(MoveBy::create(0.4, Vec2(0, 50)), DelayTime::create(2), MoveBy::create(0.4, Vec2(0, 50)),MoveBy::create(0.01, Vec2(0, 500)), NULL));
}
void HudLayer::hideTalk(){
    talkLabel->runAction(FadeOut::create(0.5));
    talkLabel->runAction(MoveBy::create(0.5, Vec2(0, 40)));
}
void HudLayer::showCoinShopLayer(){
    
//    ((ShopLayer*)GameManager::getInstance()->getShopLayer())->openShop();
}

void HudLayer::showAchievementComplete(const char* text){
    CCLOG("show achievement");
    Sprite* achieveNode = Sprite::create("achievementBar.png");
    Sprite* spt = Sprite::create("achievementIconEnabled.png");
    achieveNode->addChild(spt);
    spt->setPosition(Vec2(80, achieveNode->getContentSize().height/2));
    this->addChild(achieveNode, 10000);
    
    Label* lblText = Label::createWithSystemFont(text, "Arial", 30);
    lblText->setAnchorPoint(Vec2(0, 0.5));
    lblText->setPosition(Vec2(spt->getPosition().x + 80, achieveNode->getContentSize().height/2));
    lblText->setTag(722);
    achieveNode->addChild(lblText);
    
    ((Label*)achieveNode->getChildByTag(722))->setString(text);
    achieveNode->setPosition(Vec2(size.width/2 + 15, size.height + (achieveNode->getContentSize().height/2)));
    
    achieveNode->runAction(Sequence::create(EaseIn::create(MoveBy::create(0.5f, Vec2(0, -achieveNode->getContentSize().height)),0.2f), CallFuncN::create(CC_CALLBACK_1(HudLayer::achievementEffect, this)), DelayTime::create(1), EaseIn::create(MoveBy::create(0.5f, Vec2(0, achieveNode->getContentSize().height)),4), CallFuncN::create(CC_CALLBACK_1(HudLayer::nodeMoveDone, this)), NULL));
}

void HudLayer::achievementEffect(Node* node){
    Sprite* achieveNode = (Sprite*)node;
    Sprite* checkSprite = Sprite::create("checkImage.png");
    checkSprite->setPosition(Vec2(achieveNode->getContentSize().width - 80, achieveNode->getContentSize().height/2));
    
    
    
    ParticleExplosion* particle = ParticleExplosion::create();
    particle->setLife(0.4);                          // 지속시간
    particle->setLifeVar(0.3);
    particle->setPosition(checkSprite->getPosition());        // 위치
    particle->setSpeed(70);                      // 속도
    particle->setTotalParticles(15);
    //    particle->setScale(0.3);                          // 크기
    particle->setStartColor(Color4F(205, 205, 0, 155));
    particle->setEndColor(Color4F(205, 205, 0, 0));
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage("checkImage.png"));          // 이미지 변경
    achieveNode->addChild(particle);
    
    achieveNode->addChild(checkSprite);
}
void HudLayer::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Node* node = (Node*)obj;
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
}

void HudLayer::setButtonSize(float siz) // 0 ~ 1
{
}

void HudLayer::updateCoin(int howMuch)
{
}

void HudLayer::enableJoystick(bool enable)
{
    for(auto btn: buttonArray)
    {
        btn->enableButton(enable);
    }
}
void HudLayer::addEnergyEffect(){
    int effectCount = 15;
    float dur = 0.5;
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    float interval = 0.05f;
    Sprite* spt;
    int spread = 80;
    for (int i = 0; i < effectCount; i++) {
        spt = Sprite::createWithSpriteFrameName("particle.png");
        spt->setPosition(Vec2(size.width/2, size.height/2) + Vec2(rand()%spread - spread/2, rand()%spread - spread/2));
        Vec2 pos = spt->getPosition();
        spt->setScale(3 + (rand()%10)*0.4);
        this->addChild(spt, 100);
        spt->runAction(FadeOut::create(0));
        spt->runAction(Sequence::create(DelayTime::create(i*interval), FadeIn::create(interval), NULL));
        spt->runAction(Sequence::create(DelayTime::create(i*interval), EaseIn::create(MoveTo::create(dur, Vec2(x, y)), 3), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    }
    this->scheduleOnce(schedule_selector(HudLayer::addEnergyDone), dur + interval*effectCount);
}
void HudLayer::addEnergyDone(float dt){
    
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    Sprite* spt = Sprite::create("hudHeart.png");
    this->addChild(spt);
    spt->setPosition(Vec2(x, y));
    float dur =0.5;
    spt->runAction(ScaleTo::create(dur, 3));
    spt->runAction(Sequence::create(FadeOut::create(dur), CallFuncN::create(CC_CALLBACK_1(HudLayer::spriteMoveDone, this)), NULL));
    
}
void HudLayer::initializeSreen(int lifeCount)
{
    
//    btnA->setVisible(false);
//    btnB->setVisible(false);
//    btnLeftRight->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    recordItem->setVisible(true);w
#endif

//    recordItem->setPosition(pauseItem->getPosition() + Vec2(-pauseItem->getContentSize().width - 15, 0));
}

void HudLayer::reduceLife()
{
    Sprite* sprite = (Sprite*)lifeArray.at(lifeArray.size()-1);
    lifeArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}

void HudLayer::setWeapon(int weapon){
}
void HudLayer::setTotalBulletCount(int count, int maxCount){
//    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
//    TextBMFont* lblCount = (TextBMFont*)checkBoard->getChildByName("lblBulletLeft");
//    if (maxCount < 0) {
//        lblCount->setString("00");
//    }else{
//        lblCount->setString(__String::createWithFormat("%d/%d", count, maxCount)->getCString());
//    }
}
void HudLayer::reduceBulletCount(){
    if (bulletArray.size() <= 0) {
        return;
    }
    Sprite* sptBullet = bulletArray.at(bulletArray.size() - 1);
    bulletArray.eraseObject(sptBullet);
    sptBullet->removeFromParentAndCleanup(true);
}
void HudLayer::setBulletCount(int count, int maxCount){
//    Node* checkBoard = hudLayer->getChildByName("sptCheckBoard");
//    int bulletCountInColumn = 5;
//    if (maxCount > 20) {
//        bulletCountInColumn = 10;
//    }
//    if (bulletArray.size() > 0) {
//        for (int i = 0; i < bulletArray.size(); i++) {
//            Sprite* sptBullet = bulletArray.at(i);
//            sptBullet->removeFromParentAndCleanup(true);
//        }
//        bulletArray.clear();
//    }
//    int countLeft = count;
//    int counter = 0;
//    float x = checkBoard->getPosition().x + checkBoard->getContentSize().width*checkBoard->getScale()/2 + 2;
//    float y = checkBoard->getPosition().y + checkBoard->getContentSize().height*checkBoard->getScale()/2;
//    while(countLeft > 0){
//        Sprite* sptBullet = Sprite::createWithSpriteFrameName("bulletPi.png");
//        sptBullet->setAnchorPoint(Vec2(0, 1));
//        sptBullet->setPosition(x, y);
//        sptBullet->setScale((checkBoard->getContentSize().height*checkBoard->getScale() - 1*bulletCountInColumn)/(sptBullet->getContentSize().height*bulletCountInColumn));
//        hudLayer->addChild(sptBullet);
//        bulletArray.pushBack(sptBullet);
//        countLeft--;
//        counter++;
//        y -= sptBullet->getContentSize().height*sptBullet->getScale() + 1;
//        if (counter >= bulletCountInColumn) {
//            x += sptBullet->getContentSize().width*sptBullet->getScale() + 2;
//            y = checkBoard->getPosition().y + checkBoard->getContentSize().height*checkBoard->getScale()/2;
//            counter = 0;
//        }
//    }
}

void HudLayer::keyCollected(int count)
{
    lblKeyCollected->setString(__String::createWithFormat("x %d", count)->getCString());
    GameManager::getInstance()->showParticleExplosion(this, "blueKey.png",lblKeyCollected->getPosition(), 2);
}
void HudLayer::starCollected(int count)
{
    Vec2 pos;
    
    
    GameManager::getInstance()->showParticleExplosion(this, "goldStar.png", pos, 2);

    starCount = count;
}


void HudLayer::starCollectedFancy(int count, Vec2 collectedPos)
{
    
}

void HudLayer::starChange(float dt)
{
    
}

void HudLayer::spriteMoveDone(Node* sprite)
{
    sprite->removeFromParentAndCleanup(true);
}

void HudLayer::resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(pauseLayer == NULL) return;
    
//    blackScreen->setVisible(false);
    
    if (pauseLayer){
        pauseLayer->removeFromParentAndCleanup(true);
        pauseLayer = NULL;
    }
    
    GameManager::getInstance()->isInMiddleOfGame = true;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    NativeInterface::NativeInterface::destroyAds();
#endif
    
    
    GameManager::getInstance()->getWorld()->resumeLayer();
    this->enableButtons(true);
}
void HudLayer::showPauseLayer()
{
    recordItem->setVisible(false);
    
    if (isRecording) {
        isRecording = false;
        if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
            return;
        }
//        NativeInterface::NativeInterface::stopRecording();
    }
    
    if(pauseLayer != NULL) return;
    GameManager::getInstance()->firePressed = false;
    
//    blackScreen->setVisible(true);
    
    pauseLayer = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("LegendDary_UI_Pause.json"));
    pauseLayer->setPosition(Vec2(size.width/2 - pauseLayer->getContentSize().width/2, 0));
    this->addChild(pauseLayer);
    
    Button* btnResume = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14"));
    btnResume->addTouchEventListener(CC_CALLBACK_2(HudLayer::resumeGame, this));
    
    Button* btnRestart = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_0"));
    btnRestart->addTouchEventListener(CC_CALLBACK_2(HudLayer::retryStage, this));
    
    Button* btnStages = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_1"));
    btnStages->addTouchEventListener(CC_CALLBACK_2(HudLayer::toStageSelect, this));
    
//    NativeInterface::NativeInterface::showRectAds();
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->saveCoin();
    
    enableButtons(false);

    GameManager::getInstance()->getWorld()->pauseLayer();
}

void HudLayer::showShopLayer(std::string shopName)
{
    
}
void HudLayer::onBuyItemClick(){
    std::string itemName = buyLayer->getName();
    int itemType = WORLD->getItemType(itemName);
    
    inventory->addItem(itemName, getInventoryTypeForItemType(itemType));
    
    int price = WORLD->getItemPrice(itemType, itemName);
    GM->addCoin(-price);
    
    
    closePopup();
    buyLayer = nullptr;
    updateInventory();
}
void HudLayer::selectShopItem(Ref* ref){
    
}
void HudLayer::showBuyLayer(Button* btn, std::string itemName){
    Node* layer = CSLoader::createNode("ItemBuyPopup.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setTag(SELECT_DIALOG_GO_TO_NPC);
    setAsPopup(layer);
    ddiyong(layer);
    buyLayer = layer;
    buyLayer->setName(btn->getName());
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    Label* newLbl = changeTextWithLabel(lbl);
    LM->setLocalizedString(newLbl, itemName);
    
    int itemType = WORLD->getItemType(itemName);
//    std::string fileName = "";
//    if(itemType != ITEM_TYPE_ETC){
//        fileName = getFileNameForInventory(itemType, itemName);
//    }
//    if(itemType == ITEM_TYPE_ETC || fileName.size() <= 0){
//        Sprite* sptIcon = Sprite::createWithSpriteFrameName(WORLD->getItemSpriteName(itemType, itemName));
//        btn->addChild(sptIcon);
//        sptIcon->setName("sptIcon");
//        sptIcon->setPosition(btn->getContentSize()/2);
//        fileName = "icon_frame.png";
//    }
    Button* imgIcon = (Button*)buyLayer->getChildByName("btnIcon");
//    imgIcon->loadTexture(fileName);
    addSpriteToButton(itemName, btn);
    
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    newLbl = changeTextWithLabel(lbl, 327);
    LM->setLocalizedString(newLbl, strmake("%s desc", itemName.c_str()));
    
    lbl = (Text*)layer->getChildByName("lblCoin");
    int price = WORLD->getItemPrice(itemType, itemName);
    lbl->setString(Value(price).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    
    lbl = (Text*)layer->getChildByName("lblStatHP");
    std::string strStat = WORLD->getItemStat(itemType, itemName, "hp");
    if(strStat.size() > 0){
        lbl->setString(strmake("HP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatAP");
    strStat = WORLD->getItemStat(itemType, itemName, "ap");
    if(strStat.size() > 0){
        lbl->setString(strmake("AP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatDP");
    strStat = WORLD->getItemStat(itemType, itemName, "dp");
    if(strStat.size() > 0){
        lbl->setString(strmake("DP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatGP");
    strStat = WORLD->getItemStat(itemType, itemName, "gp");
    if(strStat.size() > 0){
        lbl->setString(strmake("GP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatSP");
    strStat = WORLD->getItemStat(itemType, itemName, "sp");
    if(strStat.size() > 0){
        lbl->setString(strmake("SP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblQuestion");
    newLbl = changeTextWithLabel(lbl, 515);
    LM->setLocalizedString(newLbl, "want to buy");
    
    lbl = (Text*)layer->getChildByName("lblYes");
    newLbl = changeTextWithLabel(lbl);
    newLbl->setString(strmake("A.%s", LM->getText("yes").c_str()));
    
    lbl = (Text*)layer->getChildByName("lblNo");
    newLbl = changeTextWithLabel(lbl);
    newLbl->setString(strmake("B.%s", LM->getText("no").c_str()));
}
void HudLayer::onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        
        return;
    }
    
//    GameManager::getInstance()->showVideo(VIDEO_GAME_OVER);
}
void HudLayer::videoDone(){
//    if(GameManager::getInstance()->getHudLayer()){
//        if (GameManager::getInstance()->videoIndex == VIDEO_GAME_OVER) {
//            GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(HudLayer::closeVideoDone), 0.05);
//        }else if(GameManager::getInstance()->videoIndex == VIDEO_WEAPON_RENT){
//            messageBox->removeFromParent();
//        }
//    }else
    
    if(GM->videoIndex >= VIDEO_SUPPORT_0 && GM->videoIndex <= VIDEO_SUPPORT_2){
        isVideoDone = true;
    }
}
void HudLayer::videoFailed(){
    if(GameManager::getInstance()->getHudLayer()){
        GameManager::getInstance()->getHudLayer()->scheduleOnce(schedule_selector(HudLayer::closeVideoFailed), 0.1);
    }
}
void HudLayer::closeVideoDone(float dt){
    if (btnFreeCoin == NULL) {
        return;
    }
    Sprite* sptCoin = Sprite::create("coin.png");
    btnFreeCoin->getParent()->addChild(sptCoin);
    sptCoin->setPosition(btnFreeCoin->getPosition() + Vec2(-40, 0));
    
    Label* lbl = Label::createWithSystemFont(__String::createWithFormat("+%d", coinCount)->getCString(), GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    sptCoin->addChild(lbl);
    lbl->setPosition(Vec2(45, 19));
    lbl->setAnchorPoint(Vec2(0, 0.5));
    
    lbl = Label::createWithSystemFont(__String::createWithFormat("EXP %d", expCount)->getCString(), GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    sptCoin->addChild(lbl);
    lbl->setPosition(Vec2(55, -10));
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;
    GameManager::getInstance()->addCoin(coinCount);
    GameManager::getInstance()->addGameExp(expCount);
    GameManager::getInstance()->saveCoin();
}

void HudLayer::closeVideoFailed(float dt){
    if (btnFreeCoin == NULL) {
        return;
    }
//    Sprite* sptCoin = Sprite::create("coin.png");
//    btnFreeCoin->getParent()->addChild(sptCoin);
//    sptCoin->setPosition(btnFreeCoin->getPosition() + Vec2(-40, 0));
    
    Label* lbl = Label::createWithSystemFont("ADS FAILED", GameManager::getInstance()->getFont(FONT_BITDUST_ONE), 30);
    btnFreeCoin->getParent()->addChild(lbl);
//    sptCoin->addChild(lbl);
//    lbl->setPosition(Vec2(80, 19));
    lbl->setPosition(btnFreeCoin->getPosition());
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;
//    GameManager::getInstance()->addCoin(50);
//    GameManager::getInstance()->saveCoin();
    
    /*Label* lbl = Label::createWithSystemFont(LanguageManager::getInstance()->getText(STR_VIDEO_FAILED), GameManager::getInstance()->getFont(FONT_DEFAULT), 24);
    sptBackLight2->getParent()->addChild(lbl);
    lbl->setPosition(sptBackLight2->getPosition() + Vec2(80, 115));
    
    btnFreeCoin->removeFromParent();
    btnFreeCoin = NULL;*/
}

void HudLayer::playCoinSoundLater(){
    
}
void HudLayer::coinLabelScheduler(float dt){
    CCLOG("coinLabelScheduler");
    increasingCoinCount += coinCount/10;
    increasingCounter++;
    if (increasingCounter >= increasingCounterMax) {
        increasingCoinCount = coinCount;
        increasingCounter = 0;
        this->schedule(schedule_selector(HudLayer::enemyLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(HudLayer::coinLabelScheduler));
    }
    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false)) {
        if (increasingCounter == 0) {
            lblCoinCount->setString(__String::createWithFormat("%d\n+%d", increasingCoinCount/2, increasingCoinCount/2)->getCString());
        }else{
            lblCoinCount->setString(Value(increasingCoinCount/2).asString());
        }
    }else{
        lblCoinCount->setString(Value(increasingCoinCount).asString());
    }
    
}
void HudLayer::enemyLabelScheduler(float dt){
    increasingEnemyCount += enemyKilledCount/10.0f;
    increasingCounter++;
    char buf[20];
    if (increasingCounter >= increasingCounterMax) {
        increasingEnemyCount = enemyKilledCount;
        increasingCounter = 0;

        sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
        lblEnemyCount->setString(buf);
        this->schedule(schedule_selector(HudLayer::expLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(HudLayer::enemyLabelScheduler));
        return;
    }
    
    sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
    lblEnemyCount->setString(buf);
    
}
void HudLayer::expLabelScheduler(float dt){
    increasingExpCount += expCount/10;
    increasingCounter++;
    
    bool isDoubleExp = UserDefault::getInstance()->getBoolForKey(KEY_DOUBLE_EXP_GET, false);
    if (isDoubleExp) {
        lblExpCount->setString(Value(expCount/2).asString());
    }else{
        lblExpCount->setString(Value(expCount).asString());
    }
    pbExp->setPercent(increasingExpCount*100.0f/maxExpCount);
    
    if (increasingCounter >= increasingCounterMax) {
        increasingExpCount = expCount;
        increasingCounter = 0;
        if (isDoubleExp) {
            lblExpCount->setString(__String::createWithFormat("%d\n+%d", expCount/2, expCount/2)->getCString());
        }else{
            lblExpCount->setString(__String::createWithFormat("%d", expCount)->getCString());
        }
        pbExp->setPercent(increasingExpCount*100.0f/maxExpCount);
        this->unschedule(schedule_selector(HudLayer::expLabelScheduler));
        
        if (expCount >= maxExpCount) {
            sptMaxTalkBalloon->setPosition(sptMaxTalkBalloon->getPosition() + Vec2(0, 1000));
            sptMaxTalkBalloon->runAction(Sequence::create(ScaleTo::create(0.001, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.05, 1.0f), NULL));
        }
        
        btnNext->setVisible(true);
        btnNext->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
        btnRetry->setVisible(true);
        btnRetry->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
        btnHome->setVisible(true);
        btnHome->runAction(Sequence::create(ScaleTo::create(0.01, 0.5), ScaleTo::create(0.1, 1.1), ScaleTo::create(0.06, 1.0), NULL));
    }
    
    // sound
    
}
void HudLayer::tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;

    int count = reviveCount;
    if (count == 0) {
//        GameManager::getInstance()->showVideo(VIDEO_GAME_CONTINUE);
        return;
    }
    int price = count*100 + 500;
    if(price > 2000){
        price = 2000;
    }
    if (GameManager::getInstance()->getCoin() >= price) {
        
        float duration = 3;
        ImageView* darySleep = dynamic_cast<ImageView*>(Helper::seekWidgetByName(reviveLayer, "sptDary"));
        darySleep->runAction(EaseOut::create(MoveBy::create(duration, Vec2(0, 50)), 3));
        darySleep->loadTexture("backLight.png");
        darySleep->runAction(RepeatForever::create(RotateBy::create(duration, 360)));
        
        Sprite* dary = Sprite::create("daryWithGun.png");
        darySleep->getParent()->addChild(dary);
        dary->setScale(4);
        dary->getTexture()->setAliasTexParameters();
        dary->setPosition(darySleep->getPosition());
        dary->runAction(EaseOut::create(MoveBy::create(duration, Vec2(0, 50)), 3));
        
        this->scheduleOnce(schedule_selector(HudLayer::reviveLater), duration*2/3);
        
        Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
        btnRevive->removeFromParentAndCleanup(true);
        Button* btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnClose"));
        btnClose->removeFromParentAndCleanup(true);
    }else{
        CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HudLayer::recommandCoinShopClosed, this));
        
    }
}

void HudLayer::recommandCoinShopClosed(Node* sender){
    
}
void HudLayer::notEnoughGemClosed(Ref* obj){
    
}
void HudLayer::closeWaitMessageBoxLater(){
    this->scheduleOnce(schedule_selector(HudLayer::closeWaitMessageBox), 0.01);
}
void HudLayer::closeWaitMessageBox(float dt){
    
}
void HudLayer::closeWaitMessageBoxLaterAndShowSucess(){
    this->scheduleOnce(schedule_selector(HudLayer::closeWaitMessageBoxAndShowSuccess), 0.01);
}
void HudLayer::closeWaitMessageBoxAndShowSuccess(float dt){
    
}
void HudLayer::reviveFromVideo(){
//    blackScreen->setVisible(false);
//    GameManager::getInstance()->animateFadeOut(reviveLayer);
//    reviveLayer = NULL;
//    pauseItem->setVisible(true);
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    recordItem->setVisible(true);
//#endif
//    this->scheduleOnce(schedule_selector(HudLayer::playBackgroundMusicLater), 2);
//    GameManager::getInstance()->getWorld()->revive();
//
//    int count = reviveCount;
//    count++;
//    reviveCount = count;
//    GameManager::getInstance()->saveCoin();
}
void HudLayer::reviveLater(float dt){
//    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(HudLayer::playBackgroundMusicLater), 2);
//    GameManager::getInstance()->getWorld()->revive();
    
    int count = reviveCount;
    int price = count*100 + 500;
    if(price > 2000){
        price = 2000;
    }
    GameManager::getInstance()->addCoin(-price);
    count++;
    reviveCount = count;
    GameManager::getInstance()->saveCoin();

}
void HudLayer::playBackgroundMusicLater(float dt){
    
}
void HudLayer::buyGemClosed(Ref* obj){
    
}
void HudLayer::removeUsedAssets(){
    WORLD->removeUsedAssets();
    
    popupArray.clear();
    endingTalkArray.clear();
    buttonArray.clear();
    lifeArray.clear();
    menus.clear();
    bulletArray.clear();
//    this->removeAllChildren();
//    _eventDispatcher->removeEventListener(touchListener);
//    _eventDispatcher->removeEventListener(listener);
    /*if (gameOverLayer) {
        gameOverLayer->removeFromParentAndCleanup(true);
    }
    if (reviveLayer) {
        reviveLayer->removeFromParentAndCleanup(true);
    }
    if (pauseLayer) {
        pauseLayer->removeFromParentAndCleanup(true);
    }
    if (gameClearLayer) {
        gameClearLayer->removeFromParentAndCleanup(true);
    }
    this->removeFromParentAndCleanup(true);
    
    GameManager::getInstance()->getWorld()->removeUsedAssets();*/
}
void HudLayer::toMain(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    BSM->getHttpTime();
    GameManager::getInstance()->getWorld()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_MAIN;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    BSM->getHttpTime();
    GameManager::getInstance()->getWorld()->saveCoinIfStarCollected();
    
    whereToGo = CLOSE_TO_STAGES;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::closeSchedule(float dt){
//    Director::getInstance()->popScene();
    removeUsedAssets();
//    ((TitleLayer*)GameManager::getInstance()->titleLayer)->closeTo(whereToGo);
    int toWhat = whereToGo;
    if(toWhat == CLOSE_TO_NEXT){
        
        if(GameManager::getInstance()->theme*24 + GameManager::getInstance()->currentStageIndex == GameManager::getInstance()->totalStage - 1){
            
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
        if (!UserDefault::getInstance()->getBoolForKey(KEY_AD_NOT_TODAY, false) &&
            GameManager::getInstance()->market == MARKET_FREE) {
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
        if(GameManager::getInstance()->currentStageIndex == 23 ){
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->backFromStages();
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setThemeSelect();
        }else{
            GameManager::getInstance()->page = PAGE_STAGE_SELECT;
            
//            int stage = GameManager::getInstance()->currentStageIndex + 1;
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->goStage(stage);
//            ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
            return;
        }
    }else if(toWhat == CLOSE_TO_RETRY){
        Director::getInstance()->popScene();
//        int stage = GameManager::getInstance()->currentStageIndex;
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->stageTouched();
        
    }else if(toWhat == CLOSE_TO_STAGES){
        if (GameManager::getInstance()->market == MARKET_FREE) {
            GameManager::getInstance()->showInterstitialAds();
        }
        
        GameManager::getInstance()->page = PAGE_STAGE_SELECT;
        
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->topBar->updateLabels();
//        ((TitleLayer*)GameManager::getInstance()->titleLayer)->setStageSelect();
        
    }else if(toWhat == CLOSE_TO_MAIN){
        GameManager::getInstance()->page = PAGE_TITLE;
        
    }
}
void HudLayer::nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    removeListener();
    
    if (GameManager::getInstance()->theme == 3 && GameManager::getInstance()->currentStageIndex == 23) {
        
        return;
    }
    if (GameManager::getInstance()->life > 0 || UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        if (!UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
//            GameManager::getInstance()->life--;
//            if (GameManager::getInstance()->life == 4) {
//                UserDefault::getInstance()->setIntegerForKey(KEY_LIFE_SPENT_TIME, ServerManager::getInstance()->getCurrentTime());
//            }
        }
        whereToGo = CLOSE_TO_NEXT;
        this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
    }else{
        whereToGo = CLOSE_TO_STAGES;
        this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
    }
}
void HudLayer::retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    GameManager::getInstance()->getWorld()->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_RETRY;
    this->scheduleOnce(schedule_selector(HudLayer::closeSchedule), 0.01);
}
void HudLayer::onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        
        return;
    }
    
}

void HudLayer::enableButtons(bool enb)
{
}

void HudLayer::displayBossEnergy(const char* name)
{
//    bossEnergyBackground = Sprite::create("dialogBox.png");
//    this->addChild(bossEnergyBackground);
//    bossEnergyBackground->setPosition(Vec2(size.width/2, size.height/2 - 60));
//    bossEnergyBackground->setScaleY(0.5);
    bossEnergyBarWidth = 600;
    
    bossName = Label::createWithSystemFont(name, GameManager::getInstance()->getFont(FONT_VISITOR), 30);
    bossName->setColor(Color3B(255, 255, 255));
    bossName->enableShadow();
//    bossName->getTexture()->setAliasTexParameters();
    this->addChild(bossName);
    bossName->setAnchorPoint(Vec2(0,0));
    bossName->setPosition(Vec2(size.width/6, size.height*2/3));
    
    
    
    bossEnergyBarBack = DrawNode::create();
    this->addChild(bossEnergyBarBack);
    
    //bossEnergyBarRed->setAnchorPoint(Vec2(0, 0.5f));
    //bossEnergyBarRed->setScaleX(bossEnergyBarWidth);
    //bossEnergyBarRed->setScaleY(2);
    
    bossEnergyBar = DrawNode::create();
    this->addChild(bossEnergyBar);
    //bossEnergyBarYellow->setPosition(Vec2(size.width/2 - bossEnergyBarWidth/2, size.height/2 + 180));
    //bossEnergyBarYellow->setAnchorPoint(Vec2(0, 0.5f));
    //bossEnergyBarYellow->setScaleX(bossEnergyBarWidth);
    //bossEnergyBarYellow->setScaleY(2);
}

void HudLayer::setBossEnergy(int percent)
{
    bossEnergyBarBack->clear();
    bossEnergyBar->clear();
    float width = size.width*4/6;
    float energyLeft = width*percent/100.0f;
    bossEnergyBarBack->drawSolidRect(Vec2(size.width/6 - 1, size.height*2/3 - 3), Vec2(size.width/6 + width + 1, size.height*2/3 + 3), Color4F(235.0f/255, 235.0f/255, 235.0f/255, 100/255.0f));
    Color4F color;
    if (percent < 20) {
        color = Color4F(235.0f/255, 50.0f/255, 50.0f/255, 255/255.0f);
    }else{
        color = Color4F(255.0f/255, 208.0f/255, 59.0f/255, 255/255.0f);
    }
    bossEnergyBar->drawSolidRect(Vec2(size.width/6, size.height*2/3 - 2), Vec2(size.width/6 + energyLeft, size.height*2/3 + 2), color);
    if (percent <= 0) {
        bossEnergyBar->setVisible(false);
        bossEnergyBarBack->setVisible(false);
    }
}
void HudLayer::removeListener(){
    removeUsedAssets();
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(_listener);
    this->unscheduleAllCallbacks();
    this->unschedule(schedule_selector(HudLayer::oneSecUpdate));
//    WORLD->removeUsedAssets();
//    js2->removeListener();
}
void HudLayer::onTutorialBoxClick(){
    if (talkIndex < talkText.size()) {
        talkIndex = (int)talkText.size();
    }else{
        tutorialNode->setVisible(false);
        if (tutorialHowTo == nullptr) {
            tutorialHowTo = Node::create();
            this->addChild(tutorialHowTo);
            ImageView* img = ImageView::create("uiBox.png");
            img->setContentSize(cocos2d::Size(690, 290));
            img->setScale9Enabled(true);
            img->setPosition(Vec2(size.width/2, size.height - 150));
            tutorialHowTo->addChild(img);
            ImageView* imgContent = ImageView::create();
            imgContent->setPosition(img->getPosition());
            tutorialHowTo->addChild(imgContent);
            imgContent->setName("imgContent");
            PPLabel* lbl = PPLabel::create("Tutorial", 34);
            lbl->setName("lbl");
            lbl->setTextHAlignment(TextHAlignment::CENTER);
            tutorialHowTo->addChild(lbl);
            lbl->setPosition(Vec2(size.width/2, size.height - 320));
            lbl->setShadowEnabled(true);
            if(lbl->lblNormal != nullptr){
                lbl->lblNormal->enableShadow(Color4B(DARK_GRAY_3B, 255), cocos2d::Size(4, -4));
            }
            lbl->setWidth(img->getContentSize().width);
        }
        if (tutorialIndex < 11) {
            tutorialHowTo->setVisible(true);
            ImageView* imgContent = (ImageView*)tutorialHowTo->getChildByName("imgContent");
            if (tutorialIndex == 1 || tutorialIndex == 10) {
                this->schedule(schedule_selector(HudLayer::updateTutorialHowTo), 0.1f);
            }else{
                imgContent->loadTexture(strmake("tutorial%d.png", tutorialIndex));
            }
            PPLabel* lbl = (PPLabel*)tutorialHowTo->getChildByName("lbl");
            lbl->setString(LM->getText(strmake("tutorialHowTo%d", tutorialIndex).c_str()));
        }
    }
}
void HudLayer::addListener(){
    touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesBegan");
//        log("test touch for find path");// test 
//        GM->getPath(GM->testStartPos, GM->testEndPos);
        
        
//        if(tutorialNode != nullptr){
//            onTutorialBoxClick();
//        }
//        auto last = touches.back();
        readyToShowNextTalk();
//        chopad->TouchesBegan(touches, evt);
        // check if last touch point is in which button
        
//        WORLD->showTalkText("what? asdf sadf dsfa dsfdasf asdf sadf dsaf ad fasd fasd f adsf dsaf adsf asd fsad fadsf asdf ", WHOSE_TALK_HERO);
        
    };
    touchListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesMoved");
//        log("x: %f, y: %f", touches.at(0)->getLocation().x, touches.at(0)->getLocation().y);
//        chopad->TouchesMoved(touches, evt);
    };
    touchListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesEnded");
        
        //        chopad->TouchesEnded(touches, evt);
    };
    touchListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesCancelled");
        //        chopad->TouchesCancelled(touches, evt);
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 10);
    
}
void HudLayer::updateTutorialHowTo(float dt){
    tutorialHowToIndex++;
    ImageView* imgContent = (ImageView*)tutorialHowTo->getChildByName("imgContent");
    if(tutorialIndex == 1){
        if(tutorialHowToIndex == 0){
            imgContent->loadTexture("tutorial1_1.png");
        }else if(tutorialHowToIndex == 2){
            imgContent->loadTexture("tutorial1.png");
        }
        if(tutorialHowToIndex > 12){
            tutorialHowToIndex = 0;
        }
    }else if(tutorialIndex == 10){
        if(tutorialHowToIndex == 0){
            imgContent->loadTexture("tutorial1_1.png");
        }else if(tutorialHowToIndex == 1){
            imgContent->loadTexture("tutorial1.png");
        }else if(tutorialHowToIndex == 2){
            imgContent->loadTexture("tutorial10_0.png");
        }else if(tutorialHowToIndex == 3){
            imgContent->loadTexture("tutorial10_1.png");
        }
        if(tutorialHowToIndex > 12){
            tutorialHowToIndex = -1;
        }
    }
}
void HudLayer::showDialog(const char* message, const char* btn1, const char* btn2)
{
    CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HudLayer::messageBoxClosed, this));
    
    GameManager::getInstance()->getHudLayer()->enableJoystick(false);

    this->setTouchEnabled(false);
    
    ((HelloWorld*)GameManager::getInstance()->getWorld())->pauseLayer();
}

void HudLayer::messageBoxClosed(Node* node)
{
    ((HelloWorld*)GameManager::getInstance()->getWorld())->resumeLayer();
	this->setTouchEnabled(true);
    //	_dialogBox->setVisible(false);
	
    GameManager::getInstance()->getHudLayer()->enableJoystick(true);
	
    if (this->getTag() == 0) {
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void HudLayer::showSingleNPCMessage(std::string msg){
    isReadyToShowNextTalk = false;
    WORLD->isInEvent = true;
    talkIndex = 0;
//    talkState = TALK_STATE_ASKING;
    this->schedule(schedule_selector(HudLayer::onDisposableMessageEvent));
    WORLD->showTalkText(msg, WHOSE_TALK_NPC);
    showBlackTopAndBottom();
}
void HudLayer::onDisposableMessageEvent(float dt){
    if(!isReadyToShowNextTalk){
        return;
    }
    isReadyToShowNextTalk = false;
    endEvent();
}
void HudLayer::showEvent(int index, bool isQuest){
    eventIndex = index;
//    isThisEventQuest = isQuest;
    isReadyToShowNextTalk = true;
    talkIndex = 0;// test
    
    talkState = TALK_STATE_ASKING;
    this->schedule(schedule_selector(HudLayer::onEvent));
    
    showBlackTopAndBottom();
}
void HudLayer::onEvent(float dt){
    if(!isReadyToShowNextTalk){
        return;
    }
    if(sptTalkBox != nullptr){
        sptTalkBox->removeFromParent();
        sptTalkBox = nullptr;
    }
    isReadyToShowNextTalk = false;
//    HelloWorld* stage = GameManager::getInstance()->getWorld();
    std::string text;
    std::string key;
    if(talkState == TALK_STATE_ASKING){
        key = StringUtils::format("event%d_%d", eventIndex,talkIndex);
        text = LanguageManager::getInstance()->getText(key.c_str());
        if(text.size() == 0 || text.compare(key) == 0){
            std::string key = StringUtils::format("event%d_option_0", eventIndex);
            std::string text = LanguageManager::getInstance()->getText(key.c_str());
            if(text.size() == 0 || true){
                endEvent();
            }else{
                talkState = TALK_STATE_QUESTIONING;
                isReadyToShowNextTalk = true;
                onEvent(dt);
            }
            
        }else if(text.find("adin>") != std::string::npos){
            GameManager::getInstance()->getWorld()->showTalkText(text.substr(5), WHOSE_TALK_HERO);
        }else if(text.find(">") != std::string::npos){
            std::string strText = text.substr((int)text.find(">") + 1);
            WORLD->showTalkText(strText, WHOSE_TALK_NPC, text.substr(0, (int)text.find(">")));
        }
        //        else if(text.find("fairy>") != std::string::npos){
        //            GameManager::getInstance()->getWorld()->showTalkText(text.substr(6), WHOSE_TALK_NPC);
        //        }else if(text.find("fairy>") != std::string::npos){
        //            GameManager::getInstance()->getWorld()->showTalkText(text.substr(5), WHOSE_TALK_HERO);
        //        }
    }else if(talkState == TALK_STATE_QUESTIONING){
        talkIndex = 0;
        showOptions(LanguageManager::getInstance()->getText(StringUtils::format("event%d_option_0", eventIndex).c_str()),  LanguageManager::getInstance()->getText(StringUtils::format("event%d_option_1", eventIndex).c_str()));
    }else if(talkState == TALK_STATE_CHOOSED){
        std::string key = StringUtils::format("event%d_choose_%d_%d", eventIndex, answer,talkIndex);
        text = LanguageManager::getInstance()->getText(key.c_str());
        if(text.size() == 0){
            endEvent();
        }else if(text.find("system>") != std::string::npos){
            text = text.substr(6);
            if(text.find("ending") != std::string::npos){
                int endingNumber = Value(text.substr(6)).asInt();
                showEnding(endingNumber);
            }
        }else if(text.find("adin>") != std::string::npos){
            GameManager::getInstance()->getWorld()->showTalkText(text.substr(5), WHOSE_TALK_HERO);
        }else if(text.find(">") != std::string::npos){
            WORLD->showTalkText(text.substr((int)text.find(">") + 1), WHOSE_TALK_NPC);
        }
    }
    talkIndex++;
}

void HudLayer::showBlackTopAndBottom(){
    int frameWidth = size.width+20;
    int barHeight = 200;
    blackTop = Sprite::create("whiteRect.png");
    this->addChild(blackTop, 5);
    blackTop->setContentSize(cocos2d::Size(frameWidth, barHeight));
    blackTop->setPosition(Vec2(size.width/2, size.height + barHeight));
    blackTop->setAnchorPoint(Vec2(0.5, 1));
    blackTop->runAction(MoveBy::create(0.6f, Vec2(0, -barHeight)));
    blackTop->setColor(Color3B::BLACK);
    
    blackBottom = Sprite::create("whiteRect.png");
    this->addChild(blackBottom , 5);
    blackBottom->setContentSize(cocos2d::Size(frameWidth, barHeight));
    blackBottom->setPosition(Vec2(size.width/2, -barHeight));
    blackBottom->setAnchorPoint(Vec2(0.5, 0));
    blackBottom->runAction(Sequence::create(MoveBy::create(0.6f, Vec2(0, barHeight)), nullptr));
    blackBottom->setColor(Color3B::BLACK);
    
    if(eventIndex != 100 || true){
//        Sprite* sptBSkip = Sprite::createWithSpriteFrameName("bSkipPressed.png");
//        blackBottom->addChild(sptBSkip);
//        sptBSkip->setScale(4);
//        sptBSkip->setPosition(Vec2(blackBottom->getContentSize().width - sptBSkip->getContentSize().width*sptBSkip->getScale()/2 - 60, blackBottom->getContentSize().height/2 + sptBSkip->getContentSize().height*sptBSkip->getScale()/2 + 4));
//        GM->runAnimation(sptBSkip, "bSkip", true);
        Text* lblSkip = Text::create("SKIP", LM->getLocalizedFont(), 80);
//        lblSkip->enableShadow();
        blackBottom->addChild(lblSkip);
        lblSkip->setPosition(Vec2(blackBottom->getContentSize().width - lblSkip->getContentSize().width*lblSkip->getScale()/2 - 60,blackBottom->getContentSize().height/2 + lblSkip->getContentSize().height*lblSkip->getScale()/2 - 50));
        lblSkip->addClickEventListener(CC_CALLBACK_0(HudLayer::onSkipClick, this));
        lblSkip->setTouchEnabled(true);
        lblSkip->setOpacity(0);
        PPLabel* lbl = PPLabel::create("SKIP", 80, Color3B::WHITE, false);
        blackBottom->addChild(lbl);
        lbl->setTextHAlignment(TextHAlignment::CENTER);
        lbl->setPosition(lblSkip->getPosition());
    }
    
    hideBtns();
}
void HudLayer::onSkipClick(){
    if(isSceneChanging)return;
    isSceneChanging = true;
    
    GM->setHudLayer(nullptr);
    GM->nextScene = STAGE_FIELD;
    auto scene = Scene::create();
    
    if(GM->currentStageIndex == 12){
        scene->addChild(EndingCredit::create());
    }else{
        Title* title = Title::create();
        scene->addChild(title);
        int stage = WORLD->stageIndex;
        Node* temp = Node::create();
        temp->setTag(stage);
        title->showStageReady(stage);
    }
    this->removeListener();
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void HudLayer::hideBlackTopAndBottom(){
    if(blackTop == nullptr) return;
    blackTop->runAction(Sequence::create(MoveBy::create(1, Vec2(0, 80)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, blackTop)), nullptr));
    blackBottom->runAction(Sequence::create(MoveBy::create(1, Vec2(0, -80)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, blackBottom)), nullptr));
    showBtns();
}
void HudLayer::updateTalkBoxRope(float dt){
    if(sptTalkBox->getContentSize().height < 257){
        return;
    }
    // rope 6, 5
    int ropeWidth = 5*4;
    int ropeheight = 4*4;
    int x = 0;
    int y = 0;
    // horizontal
    for(int i = 0; i < sptTalkBox->getContentSize().width + ropeWidth;i+=ropeWidth){
        bool topExist = false;
        bool bottomExist = false;
        x = i;
        Vec2 topPos = Vec2(x, sptTalkBox->getContentSize().height);
        Vec2 bottomPos = Vec2(x, 0);
        for (auto rope : sptTalkBox->getChildren()) {
            if(rope->getTag() != 7){
                continue;
            }
            if(rope->getBoundingBox().containsPoint(topPos)){
                topExist = true;
            }else if(rope->getBoundingBox().containsPoint(bottomPos)){
                bottomExist = true;
            }
        }
        if(!topExist){
            Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
            sptTalkBox->addChild(sptRope);
            sptRope->setPosition(topPos);
            sptRope->setScale(4);
            sptRope->setTag(7);
        }
        if(!bottomExist){
            Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
            sptTalkBox->addChild(sptRope);
            sptRope->setPosition(bottomPos);
            sptRope->setScale(4);
            sptRope->setTag(7);
        }
    }
    // vertical
//    for(int i = 0; i < sptTalkBox->getContentSize().height;i+=ropeheight){
//        bool leftExist = false;
//        bool rightExist = false;
//        y = i;
//        Vec2 leftPos = Vec2(0, y);
//        Vec2 rightPos = Vec2(sptTalkBox->getContentSize().width, y);
//        for (auto rope : sptTalkBox->getChildren()) {
//            if(rope->getBoundingBox().containsPoint(leftPos)){
//                leftExist = true;
//            }
//            if(rope->getBoundingBox().containsPoint(rightPos)){
//                rightExist = true;
//            }
//        }
//        if(!leftExist){
//            Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
//            sptTalkBox->addChild(sptRope);
//            sptRope->setPosition(leftPos);
//            sptRope->setScale(4);
//        }
//        if(!rightExist){
//            Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
//            sptTalkBox->addChild(sptRope);
//            sptRope->setPosition(rightPos);
//            sptRope->setScale(4);
//        }
//    }
}
void HudLayer::onTalkBoxResizeDone(){
    this->unschedule(schedule_selector(HudLayer::updateTalkBoxRope));
    log("onTalkBoxResizeDone");
    sptTalkBox->getChildByTag(0)->setVisible(true);
    sptTalkBox->getChildByTag(1)->setVisible(true);
    inputShouldWait = false;
//    // vertical
//    for(int i = 0; i < sptTalkBox->getContentSize().height;i+=ropeheight){
//        int y = i;
//        Vec2 leftPos = Vec2(0, y);
//        Vec2 rightPos = Vec2(sptTalkBox->getContentSize().width, y);
//        Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
//        sptTalkBox->addChild(sptRope);
//        sptRope->setPosition(rightPos);
//        sptRope->setScale(4);
//    }
}

void HudLayer::readyToShowNextTalk(){
    HelloWorld* stage = WORLD;
    if(stage->isInEvent){
        if(talkState == TALK_STATE_QUESTIONING){
            answer = 0;
            talkState = TALK_STATE_CHOOSED;
            talkIndex = 0;
            sptTalkBox->getChildByTag(0)->runAction(Sequence::create(Blink::create(0.8, 4), DelayTime::create(1),CallFunc::create(CC_CALLBACK_0(HudLayer::readyToShowNextTalk, this)), NULL));
        }else{
            if(stage->imgTalkBox != nullptr && stage->imgTalkBox->getChildByName("TOUCH") == nullptr){
                stage->talkIndex = (int)stage->talkText.size();
            }else{
                isReadyToShowNextTalk = true;
            }
        }
    }
}

void HudLayer::onLeftRelease(){
    isCursorReadyToMove = true;
}
void HudLayer::onRightRelease(){
    isCursorReadyToMove = true;
}
void HudLayer::onUpRelease(){
    isCursorReadyToMove = true;
}
void HudLayer::onDownRelease(){
    isCursorReadyToMove = true;
}
void HudLayer::onLeftPress(){
    if(!isCursorReadyToMove) return;
    isCursorReadyToMove = false;
    if(abcLayer != nullptr){
        
    }else if(samShopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findLeft(selectedSamShopItem);
        selectSamShopItem(next);
    }else if(shopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findLeft(selectedInventorySlot);
        selectShopItem(next);
    }else if(inventoryLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findLeft(selectedInventorySlot);
        selectInventorySlot(next);
    }else if(doctorShopLayer != nullptr){
        Node* next = GM->findLeft(selectedDoctorShopItem);
        selectDoctorShopItem(next);
    }else if(menuLayer != nullptr){
        selectMenu(GM->findLeft(selectedMenu));
    }
}
void HudLayer::selectMenu(Node* selectedItem){
    if(selectedItem == nullptr) return;
    Node* spt = menuLayer->getChildByName("sptSelect");
    spt->setPosition(selectedItem->getPosition());
    selectedMenu = selectedItem;
}
void HudLayer::onRightPress(){
    if(!isCursorReadyToMove) return;
    isCursorReadyToMove = false;
    if(abcLayer != nullptr){
        
    }else if(samShopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findRight(selectedSamShopItem);
        selectSamShopItem(next);
    }else if(shopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findRight(selectedInventorySlot);
        selectShopItem(next);
    }else if(doctorShopLayer != nullptr){
        Node* next = GM->findRight(selectedDoctorShopItem);
        selectDoctorShopItem(next);
    }else if(inventoryLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findRight(selectedInventorySlot);
        selectInventorySlot(next);
    }else if(menuLayer != nullptr){
        selectMenu(GM->findRight(selectedMenu));
    }
}
void HudLayer::onUpPress(){
    if(!isCursorReadyToMove) return;
    isCursorReadyToMove = false;
    if(abcLayer != nullptr){
        Node* next = GM->findUp(selectedAbcOption);
        selectABCOption(next);
    }else if(samShopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findUp(selectedSamShopItem);
        selectSamShopItem(next);
    }else if(peterShopLayer != nullptr){
        Node* next = GM->findUp(selectedPeterItem);
        onChoosedInsurancePlan(next);
    }else if(shopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findUp(selectedInventorySlot);
        selectShopItem(next);
    }else if(inventoryLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findUp(selectedInventorySlot);
        selectInventorySlot(next);
    }else if(menuLayer != nullptr){
        selectMenu(GM->findUp(selectedMenu));
    }
}

void HudLayer::onDownPress(){
    if(!isCursorReadyToMove) return;
    isCursorReadyToMove = false;
    if(abcLayer != nullptr){
        Node* next = GM->findDown(selectedAbcOption);
        selectABCOption(next);
    }else if(peterShopLayer != nullptr){
        Node* next = GM->findDown(selectedPeterItem);
        onChoosedInsurancePlan(next);
    }else if(samShopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findDown(selectedSamShopItem);
        selectSamShopItem(next);
    }else if(shopLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findDown(selectedInventorySlot);
        selectShopItem(next);
    }else if(inventoryLayer != nullptr && abcLayer == nullptr){
        Node* next = GM->findDown(selectedInventorySlot);
        selectInventorySlot(next);
    }else if(menuLayer != nullptr){
        selectMenu(GM->findDown(selectedMenu));
    }
}
void HudLayer::selectABCOption(Node* node){
    if(node == nullptr) return;
    int index = node->getTag();
    if(selectedAbcOption != node && node != nullptr){
        selectedAbcOption = node;
        sptCursor->setPosition(node->getBoundingBox().origin + Vec2(-sptCursor->getContentSize().width/2 - 10, node->getBoundingBox().size.height/2));
    }else if(samShopLayer != nullptr){
        int actionType = ACTION_TYPE_CANCEL;
        if(index == 1){
            actionType = ACTION_TYPE_EXCHANGE;
        }
        if(actionType == ACTION_TYPE_EXCHANGE){
            int ingredientCount = inventory->getTotalItemCount(samShopIngredientName);
            ingredientCount = (ingredientCount/samShopExchangeRate)*samShopExchangeRate;
            if(inventory->isSlotAvailableForThisItem(samShopResultStoneName, getInventoryTypeForItemType(ITEM_TYPE_ETC), ingredientCount/samShopExchangeRate)){
                int fee = 10 + pow(5, samShopStoneLevel);
                int totalFee = (ingredientCount/samShopExchangeRate)*fee;
                if(GM->getCoin() >= totalFee){
                    GM->addCoin(-totalFee);
                    inventory->removeItem(samShopIngredientName, ingredientCount);
                    inventory->addItem(samShopResultStoneName, getInventoryTypeForItemType(ITEM_TYPE_ETC), ingredientCount/samShopExchangeRate);
                    Node* sItem = selectedSamShopItem;
                    selectedSamShopItem = nullptr;
                    selectSamShopItem(sItem);
                    
                    abcLayer->getChildByName("lblOption1")->runAction(Blink::create(0.5, 2));
                    this->scheduleOnce(schedule_selector(HudLayer::closeABCLayerLater), 1);
                }else{
                    showInstanceMessage(LM->getText("not enough money"));
                    GM->playSoundEffect(SOUND_NAGATIVE);
                }
            }else{
                showInstanceMessage(LM->getText("not enough inventory slot"));
            }
        }else{
            closePopup();
            abcLayer = nullptr;
        }
    }else{
        
        int page = selectedInventorySlot->getParent()->getChildByName("lblPage")->getTag();
        int slot = selectedInventorySlot->getTag() + page*9;
        std::string name = inventory->getItemName(slot);
        abcLayer->setName(name);
        int itemType = getItemTypeInSlot(slot);
        int actionType = ACTION_TYPE_CANCEL;
        if(index == 0){
            actionType = ACTION_TYPE_CANCEL;
        }else if(index == 1){
            if(shopLayer != nullptr){
                actionType = ACTION_TYPE_SELL;
            }else{
                actionType = ACTION_TYPE_THROW_AWAY;
            }
        }else{
            if(itemType == ITEM_TYPE_ETC){
                int ability = WORLD->itemStatTable[name].asValueMap().at("ability").asInt();
                if(ability > 0){
                    actionType = ACTION_TYPE_USE;
                }
            }else{
                actionType = ACTION_TYPE_EQUIP;
            }
        }
        abcLayer->setTag(actionType);
        
        if (actionType == ACTION_TYPE_EQUIP) {
            equipItem(slot);
            abcLayer->getChildByName("lblOption2")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(HudLayer::closeABCLayerLater), 1);
        }else if(actionType == ACTION_TYPE_USE){
            abcLayer->getChildByName("lblOption2")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(HudLayer::closeABCLayerLater), 1);
        } else if(actionType == ACTION_TYPE_SELL){
            int price = WORLD->getItemPrice(getItemTypeInSlot(slot), name)/10;
            int count = inventory->getItemCount(slot);
            GM->addCoin(price*count);
            inventory->removeItem(name, count);
            updateInventory();
            abcLayer->getChildByName("lblOption1")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(HudLayer::closeABCLayerLater), 1);
        } else if (actionType == ACTION_TYPE_THROW_AWAY) {
            int count = inventory->getItemCount(slot);
            inventory->removeItem(name, count);
            updateInventory();
            abcLayer->getChildByName("lblOption1")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(HudLayer::closeABCLayerLater), 1);
            Label* lbl = (Label*)inventoryLayer->getChildByName("lblDescription");
            lbl->setString("");
        }else{
            closePopup();
            abcLayer = nullptr;
        }
    }
}
void HudLayer::equipItem(int slot){
    
}
void HudLayer::closeABCLayerLater(float dt){
    
}
void HudLayer::ddiyong(Node* node){
    node->setScale(0.1f);
    node->runAction(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.05, 0.95),ScaleTo::create(0.05, 1), NULL));
}
int HudLayer::getInventoryTypeForItemType(int itemType){
    if(itemType == ITEM_TYPE_ETC){
        return INVENTORY_ITEM_TYPE_ETC;
    }else{
        return INVENTORY_ITEM_TYPE_EQUIPMENT;
    }
}
void HudLayer::showGoToNPC(){
    Node* layer = CSLoader::createNode("MoveToNPC.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    ddiyong(layer);
    goToNPCLayer = layer;
    goToNPCLayer->setTag(0);
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lbl0");
    Label* newLbl = changeTextWithLabel(lbl);
    LM->setLocalizedString(newLbl, "transporting");
    
    lbl = (Text*)layer->getChildByName("lbl1");
    newLbl = changeTextWithLabel(lbl);
    newLbl->setString("0%%");
    
    lbl = (Text*)layer->getChildByName("lbl2");
    newLbl = changeTextWithLabel(lbl);
    LM->setLocalizedString(newLbl, "please wait");
    this->schedule(schedule_selector(HudLayer::showGoToNPCProgress), 0.09f);
}
void HudLayer::showGoToNPCProgress(float dt){
    goToNPCLayer->setTag(goToNPCLayer->getTag() + 1);
    
    Label* lbl = (Label*)goToNPCLayer->getChildByName("lbl1");
    lbl->setString(strmake("%d%%", goToNPCLayer->getTag()));
    if(goToNPCLayer->getTag() >= 100){
        this->unschedule(schedule_selector(HudLayer::showGoToNPCProgress));
        lbl = (Label*)goToNPCLayer->getChildByName("lbl0");
        LM->setLocalizedString(lbl, "transported");
        
        lbl = (Label*)goToNPCLayer->getChildByName("lbl1");
        LM->setLocalizedString(lbl, "press a");
        
        lbl = (Label*)goToNPCLayer->getChildByName("lbl2");
        lbl->setString("");
    }
}
void HudLayer::selectSelectDialog(float dt){
    int tag = selectDialog->getTag();
    closePopup();
    selectDialog = nullptr;
    if(tag == SELECT_DIALOG_GO_TO_NPC){
        showGoToNPC();
    }
}
void HudLayer::endEvent(){
    this->unschedule(schedule_selector(HudLayer::onEvent));
    this->unschedule(schedule_selector(HudLayer::onDisposableMessageEvent));
    
    hideBlackTopAndBottom();
    WORLD->endEvent();
    
    if(sptTalkBox != nullptr){
        sptTalkBox->removeFromParent();
        sptTalkBox = nullptr;
    }
    
    onSkipClick();
}
void HudLayer::showOptions(std::string option0, std::string option1){
    sptTalkBox = Sprite::createWithSpriteFrameName("whiteRect.png");
    this->addChild(sptTalkBox, 6);
    sptTalkBox->setPosition(Vec2(size.width/2, size.height/2 + 100));
//    sptTalkBox->setScale(1);
//    sptTalkBox->setContentSize(cocos2d::Size(640, 257));
    sptTalkBox->setContentSize(cocos2d::Size(10, 10));
    int ropeWidth = 5*4;
    int ropeheight = 4*4;
    int x = 0;
    int y = 0;
    
    float height = 257;
    float scale = 0.3f;
    Label* lblOption0 = LanguageManager::getInstance()->getLocalizedLabel(StringUtils::format("A. %s", option0.c_str()).c_str(), Color4B(78, 78, 78, 255));
    sptTalkBox->addChild(lblOption0);
    lblOption0->setPosition(Vec2(320, height*4/5));
    lblOption0->setScale(scale);
    lblOption0->setTag(0);
    lblOption0->setVisible(false);
    lblOption0->setWidth(600/scale);
    
    Label* lblOption1 = LanguageManager::getInstance()->getLocalizedLabel(StringUtils::format("B. %s", option1.c_str()).c_str(), Color4B(78, 78, 78, 255));
    sptTalkBox->addChild(lblOption1);
    lblOption1->setPosition(Vec2(320, height*2/5));
    lblOption1->setVisible(false);
    lblOption1->setTag(1);
    lblOption1->setScale(scale);
    lblOption1->setWidth(600/scale);
    
    sptTalkBox->runAction(Sequence::create(ResizeTo::create(0.2f, cocos2d::Size(8, 257)), ResizeTo::create(0.5f, cocos2d::Size(640, height)), CallFunc::create(CC_CALLBACK_0(HudLayer::onTalkBoxResizeDone, this)), nullptr));
    inputShouldWait = true;
//    sptTalkBox->runAction(Sequence::create(ScaleTo::create(0.2f, 0.1f, 1),ScaleTo::create(0.5f, 1), CallFunc::create(CC_CALLBACK_0(HudLayer::onTalkBoxResizeDone, this)), nullptr));
    
    this->schedule(schedule_selector(HudLayer::updateTalkBoxRope));
}

void HudLayer::showEnding(int index){
    blackTop->setVisible(false);
    blackBottom->setVisible(false);
    hudLayer->setVisible(false);
    for(auto btn: menus){
        btn->setVisible(false);
    }
    ImageView* sptBack = ImageView::create("whiteRect.png");
    sptBack->setColor(Color3B::BLACK);
    sptBack->setCapInsets(cocos2d::Rect(3, 3, sptBack->getContentSize().width - 6, sptBack->getContentSize().height - 6));
    sptBack->setScale9Enabled(true);
    sptBack->setContentSize(cocos2d::Size(750, 750));
    sptBack->setPosition(Vec2(size.width/2, size.height - 750/2));
    this->addChild(sptBack,-1);
    sptBack->setName("EndingFrame");
    endingIndex = index;
    lastEndingTalkY = size.height - 90;
    
    this->schedule(schedule_selector(HudLayer::updateEnding), 1);
    updateEnding(0);
}
void HudLayer::updateEnding(float dt){
//    Node* frame = this->getChildByName("EndingFrame");
    
    int padding = 10;
    std::string text = LM->getText(StringUtils::format("ending%d_%d", endingIndex, endingTalkIndex).c_str());
    bool isLeft = true;
    std::string profile;
    std::string name;
    if(text.find("system>") != std::string::npos){
        text = text.substr(7);
        profile = "cumputer.png";
        name = LM->getText("system");
    }else if(text.find("mom>") != std::string::npos){
        text = text.substr(4);
        profile = "questGirl.png";
        name = LM->getText("mom");
    }else if(text.find("adin>") != std::string::npos){
        text = text.substr(5);
        isLeft = false;
        profile = "adin_idle_front_0.png";
        name = LM->getText("adin");
    }else{
        if(text.size() > 0){
            Sprite* notice = Sprite::create("whiteRect.png");
            notice->setContentSize(cocos2d::Size(800, 40));
            notice->setPosition(Vec2(size.width/2, lastEndingTalkY));
            endingTalkArray.pushBack(notice);
            notice->setColor(Color3B(78, 78, 78));
            notice->setAnchorPoint(Vec2(0.5, 1));
            this->addChild(notice,-1);
            
            Label* lbl = LM->getLocalizedLabel();
            notice->addChild(lbl);
            lbl->setTextColor(Color4B(228, 228, 228, 255));
            lbl->setString(text);
            lbl->setPosition(notice->getContentSize()/2);
            lbl->setScale(0.35f);
            
            endingTalkIndex++;
            lastEndingTalkY -= utils::getCascadeBoundingBox(notice).size.height + padding;
        }else{
            this->unschedule(schedule_selector(HudLayer::updateEnding));
        }
        return;
    }
    
    
    Sprite* profilePic = Sprite::createWithSpriteFrameName(profile);
    profilePic->setPosition(Vec2(isLeft?80:670, lastEndingTalkY - padding));
    profilePic->setAnchorPoint(Vec2(0.5, 1));
    profilePic->setScale(4);
    this->addChild(profilePic, -1);
    endingTalkArray.pushBack(profilePic);
    log("box width before: %f", utils::getCascadeBoundingBox(profilePic).size.width);
    Label* lbl = LM->getLocalizedLabel(name.c_str(), Color4B::WHITE);
    lbl->setAnchorPoint(Vec2(isLeft?0:1, 1));
    lbl->setPosition(Vec2(isLeft?profilePic->getContentSize().width + padding:-padding, profilePic->getContentSize().height));
    profilePic->addChild(lbl);
    lbl->setScale(lbl->getScale());
    
    if (text.find("shyface") !=  std::string::npos) {
        Sprite* sptFace = Sprite::createWithSpriteFrameName("emoticonShyFace.png");
        profilePic->addChild(sptFace);
        sptFace->setAnchorPoint(Vec2(0.5, 1));
        sptFace->runAction(Sequence::create(DelayTime::create(0.3f), FlipX::create(true) , DelayTime::create(0.3f), FlipX::create(false) ,DelayTime::create(0.3f), FlipX::create(true), NULL));
        sptFace->setPosition(Vec2(lbl->getPositionX() + sptFace->getContentSize().width/2*(isLeft?1:-1), lbl->getPositionY() - lbl->getContentSize().height*lbl->getScaleY() - padding));
    }else{
        ImageView* imgTalkBox = ImageView::create("talkBoxFrame.png");
        imgTalkBox->setAnchorPoint(Vec2(isLeft?0:1, 1));
        imgTalkBox->setPosition(Vec2(lbl->getPositionX(), lbl->getPositionY() - lbl->getContentSize().height*lbl->getScaleY()));
        imgTalkBox->setCapInsets(cocos2d::Rect(3, 3, imgTalkBox->getContentSize().width - 6, imgTalkBox->getContentSize().height - 6));
        imgTalkBox->setScale9Enabled(true);
        profilePic->addChild(imgTalkBox);
        
        Sprite* sptPointer = Sprite::create("talkBoxPointer.png");
        profilePic->addChild(sptPointer);
        
        Label* lblTalkShadow = LM->getLocalizedLabel();
        profilePic->addChild(lblTalkShadow);
        lblTalkShadow->setTextColor(Color4B(78, 78, 78, 255));
        // resize talkbox
        lblTalkShadow->setString(text);
        cocos2d::Size talkSize = cocos2d::Size(lblTalkShadow->getContentSize().width*lblTalkShadow->getScale(), lblTalkShadow->getContentSize().height*lblTalkShadow->getScale());
        float width = 66;
        if(talkSize.width > width){
            lblTalkShadow->setWidth(width/lblTalkShadow->getScale());
        }else{
            lblTalkShadow->setWidth(talkSize.width/lblTalkShadow->getScale());
        }
        
        talkSize = cocos2d::Size(lblTalkShadow->getContentSize().width*lblTalkShadow->getScale(), lblTalkShadow->getContentSize().height*lblTalkShadow->getScale());
        float padding = 10;
        imgTalkBox->setContentSize(cocos2d::Size(talkSize.width + padding, talkSize.height + padding));
        lblTalkShadow->setPosition(imgTalkBox->getPosition() + Vec2((isLeft?1:-1)*imgTalkBox->getContentSize().width/2, - padding/2 - talkSize.height/2));
        // resize talkbox end
        
        sptPointer->setPosition(imgTalkBox->getPosition() + Vec2((isLeft?-1:1)*sptPointer->getContentSize().height/2 + (isLeft?3:-3), -5));
        sptPointer->setRotation(isLeft?90:-90);
    }
    log("box width after: %f", utils::getCascadeBoundingBox(profilePic).size.width);
    endingTalkIndex++;
    lastEndingTalkY -= utils::getCascadeBoundingBox(profilePic).size.height + padding;
    
    float bottomY = size.height - 700;
    
    if(lastEndingTalkY < bottomY){
        float nextY = bottomY;
        for(int i = (int)endingTalkArray.size() - 1; i >= 0; i--){
            Node* child = endingTalkArray.at(i);
            float childHeight = utils::getCascadeBoundingBox(child).size.height;
            child->setPositionY(nextY + childHeight);
            nextY += childHeight + padding;
        }
    }
}
PPLabel* HudLayer::showInstanceMessage(std::string msg){
    this->removeChildByTag(77);
    PPLabel* lbl = PPLabel::create(msg, 60, Color3B::WHITE, true, false, TextHAlignment::CENTER, true);
    this->addChild(lbl, 200);
    lbl->setTag(77);
    lbl->setPosition(Vec2(size.width/2, -TILE_SIZE/2));
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(MoveBy::create(dur, Vec2(0, distanceToMove)), DelayTime::create(2), MoveBy::create(dur, Vec2(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, lbl)), nullptr));
    return lbl;
}
//bool HudLayer::addItemToInventory(std::string name){
//    int count = getItemCountInInventory();
//
//    int index = -1;
//    if(WORLD->getItemType(name) == ITEM_TYPE_ETC){
//        for(int i = 0; i < getInventoryMaxSlotCount(); i++){
//            std::string item = getItemNameInInventorySlot(i);
//            if(item.compare(name) == 0){
//                int itemCount = inventory->getItemCount(i);
//                if(itemCount < INVENTORY_MAX_COUNT_FOR_SLOT){
//                    index = i;
//                    break;
//                }
//            }
//        }
//    }
//    if(index < 0 && count >= INVENTORY_MAX_COUNT_FOR_SLOT){
//        return false;
//    }
//    if(index >= getInventoryMaxSlotCount()){
//        return false;
//    }
//    UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, index).c_str(), name);
//    addItemCountForInventorySlot(index);
//
//    return true;
//}
//int HudLayer::inventory->getItemCount(int slot){
//    return UD->getIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), 0);
//}
//void HudLayer::setItemCountForInventorySlot(int slot, int count){
//    UDSetInt(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//    if(count == 0){
//        UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "deleted");
//        rearrangeInventoryItems();
//    }
//}
//void HudLayer::addItemCountForInventorySlot(int slot){
//    int count = inventory->getItemCount(slot);
//    count++;
//    UD->setIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//}
//void HudLayer::setItemToInventory(std::string name, int slot){
//    UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), name);
//}
//int HudLayer::getItemCountInInventory(){
//    int count = 0;
//    while(true){
//        if(UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count).c_str(), "").size() == 0){
//            break;
//        }
//        count++;
//    }
//    return count;
//}
//bool HudLayer::isSlotAvailableForThisItem(std::string itemName, int count){
//    int currentSlotOccupiedCount = getItemCountInInventory();
//    int index = currentSlotOccupiedCount;
//    if(WORLD->getItemType(itemName) == ITEM_TYPE_ETC){
//        for(int i = 0; i < getMaxSlotCount(); i++){
//            std::string item = getItemNameInInventorySlot(i);
//            if(item.compare(itemName) == 0){
//                int itemCount = inventory->getItemCount(i);
//                if(itemCount < INVENTORY_MAX_COUNT_FOR_SLOT){
//                    index = i;
//                    break;
//                }
//            }
//        }
//    }
//    if(index >= getInventoryMaxSlotCount()){
//        return false;
//    }
//    UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, index).c_str(), itemName);
//    addItemCountForInventorySlot(index);
//
//    return true;
//}
//
//void HudLayer::rearrangeInventoryItems(){
//    int count = 0;
//    bool deletedItemDetected = false;
//    while(true){
//        if(deletedItemDetected){
//            std::string item = UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count).c_str(), "");
//            UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count-1).c_str(), item);
//            int itemCount = inventory->getItemCount(count);
//            UD->setIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, count-1).c_str(), itemCount);
//        }else{
//            if(UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count).c_str(), "").compare("deleted") == 0){
//                deletedItemDetected = true;
//            }
//        }
//
//        if(UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count).c_str(), "").size() == 0){
//            break;
//        }
//        count++;
//    }
//    if(deletedItemDetected){
//        rearrangeInventoryItems();
//    }
//}
//void HudLayer::inventory->removeItem(std::string itemName){
//    int slotCount = getItemCountInInventory();
//    int slot = -1;
//    for(int i = 0; i < slotCount; i++){
//        std::string item = getItemNameInInventorySlot(i);
//        if(item.compare(itemName) == 0){
//            slot = i;
//            break;
//        }
//    }
//    if(slot < 0){
//        assert(strmake("not slot found for the item name: %s", itemName.c_str()).c_str());
//    }
//
//    int count = inventory->getItemCount(slot);
//    count--;
//    if(count <= 0){
//        UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "deleted");
//    }else{
//
//    }
//    UD->setIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//
//    rearrangeInventoryItems();
//    updateInventory();
//}
//void HudLayer::inventory->removeItem(int slot){
//    int count = inventory->getItemCount(slot);
//    count--;
//    if(count <= 0){
//        UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "deleted");
//    }else{
//
//    }
//    UD->setIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//
//    rearrangeInventoryItems();
//    updateInventory();
//}
Label* HudLayer::changeTextWithLabel(Text* txt, float width){
    Label* lblTemp = LM->getLocalizedLabel("", txt->getTextColor());
    lblTemp->setName(txt->getName());
    txt->getParent()->addChild(lblTemp);
    lblTemp->setPosition(txt->getPosition());
    lblTemp->setScale(25/80.0f);
    lblTemp->setAnchorPoint(txt->getAnchorPoint());
    lblTemp->setHorizontalAlignment(txt->getTextHorizontalAlignment());
    if(txt->getTextHorizontalAlignment() == TextHAlignment::RIGHT){
        lblTemp->setHorizontalAlignment(TextHAlignment::LEFT);
    }
    lblTemp->setVerticalAlignment(txt->getTextVerticalAlignment());
    log("alignment: %d, %d", (int)txt->getTextHorizontalAlignment(), (int)txt->getTextVerticalAlignment());
    if(txt->getTextVerticalAlignment() == TextVAlignment::CENTER){
        log("center");
    }else if(txt->getTextVerticalAlignment() == TextVAlignment::TOP){
        lblTemp->setVerticalAlignment(TextVAlignment::CENTER);
    }else if(txt->getTextVerticalAlignment() == TextVAlignment::BOTTOM){
        log("bottom");
    }else{
        lblTemp->setVerticalAlignment(TextVAlignment::TOP);
    }
    lblTemp->setString(txt->getString());
    if(width > 0){
        lblTemp->setWidth(width/lblTemp->getScale());
        lblTemp->setOverflow(Label::Overflow::SHRINK);
    }
    txt->removeFromParent();
    return lblTemp;
}
Sprite* HudLayer::changeCoinToAnimation(Node* imgCoin){
    return nullptr;
}
Sprite* HudLayer::changeGemToAnimation(Node* imgCoin){
    return nullptr;
}
void HudLayer::showInventory(){
   
}
Sprite* HudLayer::addBToCloseToLayer(Node* layer){
    Sprite* sptBToClose = Sprite::createWithSpriteFrameName("bToCloseReleased.png");
    layer->addChild(sptBToClose);
    GameManager::getInstance()->runAnimation(sptBToClose, "bToCloseAni", true);
    sptBToClose->setAnchorPoint(Vec2(1, 0));
    sptBToClose->setPosition(Vec2(196, 660));
    sptBToClose->setScale(4.5f);
    return sptBToClose;
}
//int HudLayer::getInventoryMaxSlotCount(){
//    return 9*4;
//}

void HudLayer::updateInventory(){
    
}
int HudLayer::getHPFromItem(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return WORLD->getHelmetStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return WORLD->getWeaponStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return WORLD->getShieldStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return WORLD->getShoesStat(itemName, "hp");
    }
    return 0;
}
int HudLayer::getAPFromItem(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return WORLD->getHelmetStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return WORLD->getWeaponStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return WORLD->getShieldStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_SHOES){
        return WORLD->getShoesStat(itemName, "ap");
    }
    return 0;
}
int HudLayer::getDPFromItem(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return WORLD->getHelmetStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return WORLD->getWeaponStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return WORLD->getShieldStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return WORLD->getShoesStat(itemName, "dp");
    }
    return 0;
}
int HudLayer::getGPFromItem(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return WORLD->getHelmetStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return WORLD->getWeaponStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return WORLD->getShieldStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return WORLD->getShoesStat(itemName, "gp");
    }
    return 0;
}
int HudLayer::getSPFromItem(std::string itemName){
    int itemType = WORLD->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return WORLD->getHelmetStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return WORLD->getWeaponStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return WORLD->getShieldStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return WORLD->getShoesStat(itemName, "sp");
    }
    return 0;
}
//std::string HudLayer::getItemNameInInventorySlot(int slot){
//    return UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "");
//}
void HudLayer::selectInventorySlot(Ref* ref){

}
int HudLayer::getItemTypeInSlot(int slot){
    std::string name = inventory->getItemName(slot);
    return WORLD->getItemType(name);
}

bool HudLayer::isPopupAvailable(){
    return true;
}
Node* HudLayer::getPopup(){
    if(popupArray.size() > 0){
        return popupArray.at(popupArray.size() - 1);
    }
    return nullptr;
}
void HudLayer::closePopup(){
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        popup->removeFromParent();
    }
}
bool HudLayer::setAsPopup(Node* node){
    popupArray.pushBack(node);
    node->setLocalZOrder(200 + popupArray.size());
    return true;
}
void HudLayer::ropeAround(Node* node){
    int ropeWidth = 5*4;
    int ropeheight = 4*4;
    int x = 0;
    int y = 0;
    int ropeScale = 4;
    // horizontal
    for(int i = 0; i < node->getContentSize().width + ropeWidth;i+=ropeWidth){
        x = i;
        Vec2 topPos = Vec2(x, node->getContentSize().height);
        Vec2 bottomPos = Vec2(x, 0);
        
            Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
            node->addChild(sptRope);
            sptRope->setPosition(topPos);
            sptRope->setScale(ropeScale);
            sptRope->setTag(7);
        
            sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
            node->addChild(sptRope);
            sptRope->setPosition(bottomPos);
            sptRope->setScale(ropeScale);
            sptRope->setTag(7);
        
    }
    // vertical
        for(int i = 0; i < node->getContentSize().height;i+=ropeheight){
            y = i;
            Vec2 leftPos = Vec2(0, y);
            Vec2 rightPos = Vec2(node->getContentSize().width, y);
            
                Sprite* sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
                node->addChild(sptRope);
                sptRope->setPosition(leftPos);
                sptRope->setScale(ropeScale);
            
                sptRope = Sprite::createWithSpriteFrameName("ropeParticle.png");
                node->addChild(sptRope);
                sptRope->setPosition(rightPos);
                sptRope->setScale(ropeScale);
            
        }
}
//void HudLayer::removeItemFromInventory(std::string name, int count){
//    int slotCount = getItemCountInInventory();
//
//    for(int i = 0; i < slotCount; i++){
//        std::string item = getItemNameInInventorySlot(i);
//        if(item.compare(name) == 0){
//            int itemCount = inventory->getItemCount(i);
//            if(itemCount >= count){
//                itemCount -= count;
//                HUD->setItemCountForInventorySlot(i, itemCount);
//                break;
//            }else{
//                count -= itemCount;
//                HUD->setItemCountForInventorySlot(i, 0);
//            }
//        }
//    }
//}
//int HudLayer::getTotalItemCount(std::string name){
//    int slotCount = getItemCountInInventory();
//    int totalCount = 0;
//    for(int i = 0; i < slotCount; i++){
//        std::string item = getItemNameInInventorySlot(i);
//        if(item.compare(name) == 0){
//            int itemCount = inventory->getItemCount(i);
//            totalCount += itemCount;
//        }
//    }
//    return totalCount;
//}

void HudLayer::showIntro(){
    HUD->hudLayer->setVisible(false);
    HUD->hideBtns();
    
    Sprite* sptFrame = Sprite::create("icon_frame.png");
    sptFrame->setColor(Color3B::BLACK);
    this->addChild(sptFrame, 1);
    sptFrame->setScale(50);
    sptFrame->setOpacity(0);
    sptFrame->runAction(Sequence::create(DelayTime::create(1), FadeTo::create(2, 150), NULL));
    sptFrame->setPosition(Vec2(size.width/2, size.height - size.width/2));
    
    float dur = 50;
    Label* lbl = Label::createWithSystemFont("text", LM->getLocalizedFont(), 30);
    lbl->setScale(0.08);
    lbl->setString(LM->getText("intro story desc"));
    lbl->setWidth(size.width - 150);
    this->addChild(lbl, 1);
    lbl->setScale(1);
    
    lbl->setAnchorPoint(Vec2(0.5, 1));
    lbl->setPosition(Vec2(size.width/2, size.height - size.width));
    lbl->runAction(MoveBy::create(dur, Vec2(0, lbl->getBoundingBox().size.height + size.width)));
    
    GM->nextScene = STAGE_TITLE;
//    this->runAction(Sequence::create(DelayTime::create(dur - 2), CallFunc::create(CC_CALLBACK_0(HudLayer::showBIAndReplaceScene, this)), NULL));
    
    Node* layer = CSLoader::createNode("Intro.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    
    this->schedule(schedule_selector(HudLayer::onLoading));
}
void HudLayer::onLoading(float dt){
//    loadingTime += dt;
//    if(loadingTime < 0)
//        return;
//    float loadingCompleteTime = 3;
//
//    LoadingBar* lb = (LoadingBar*)firstPopup->getChildByName("pbLoading");
//    Text* lblLoading = (Text*)firstPopup->getChildByName("lblLoading");
//    int percent = loadingTime*100/loadingCompleteTime;
//    lblLoading->setString(strmake("LOADING... %d%%", percent));
//    if(percent >= 101){
//        percent = 100;
//        this->unschedule(schedule_selector(HudLayer::onLoading));
//        lblLoading->setString("PRESS A TO CONTINUE");
//        lblLoading->runAction(RepeatForever::create(Blink::create(1, 2)));
//    }
//    lb->setPercent(percent);
}

void HudLayer::showSAMShop(){
    
}
void HudLayer::showAlchyShop(){
    WORLD->isInEvent = false;
    WORLD->pauseLayer();
    Node* layer = CSLoader::createNode("AlchyShop.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setTag(SELECT_DIALOG_GO_TO_NPC);
    setAsPopup(layer);
    ddiyong(layer);
    alchyShopLayer = layer;
    
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "alchy room");
    lbl = (Text*)layer->getChildByName("lblFeeTitle");
    LM->setLocalizedString(lbl, "fee");
    
    Text* lblCoin = (Text*)layer->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    
    lblCoin = (Text*)layer->getChildByName("lblFee");
    changeCoinToAnimation(layer->getChildByName("sptCoinFee"));
    
    lbl = (Text*)layer->getChildByName("lblDescriptionTitle");
    LM->setLocalizedString(lbl, "description");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "alchy description");
    
    
    lbl = (Text*)layer->getChildByName("lblIngredient");
    lbl->setString(LM->getText("ingredient"));
    
    lbl = (Text*)layer->getChildByName("lblResult");
    lbl->setString(LM->getText("result"));
    
    lbl = (Text*)layer->getChildByName("lblResultDescription");
//    changeTextWithLabel(lbl, 642.77f);
    
    
    lbl = (Text*)layer->getChildByName("lblTip");
//    Label* newLbl = changeTextWithLabel(lbl, 313);
    lbl->setString(strmake("A.%s   B.%s", LM->getText("combine").c_str(), LM->getText("cancel").c_str()));
    
    
    updateAlchyShop();
    
}
void HudLayer::updateAlchyShop(){
    
}
int HudLayer::getGemForMinute(int min){
    float rate = 0;
    if(min < 60*1){
        rate = 3;
    }else if(min < 60*2){
        rate = 4;
    }else if(min < 60*3){
        rate = 4.39f;
    }else if(min < 60*4){
        rate = 4.71f;
    }else if(min < 60*6){
        rate = 5.0f;
    }else if(min < 60*8){
        rate = 5.1f;
    }else if(min < 60*12){
        rate = 5.33f;
    }else if(min < 60*24*1){
        rate = 5.54f;
    }else if(min < 60*24*2){
        rate = 7.51f;
    }else if(min < 60*24*3){
        rate = 8.52f;
    }else if(min < 60*24*4){
        rate = 9.1f;
    }else if(min < 60*24*5){
        rate = 9.56f;
    }else if(min < 60*24*6){
        rate = 9.8f;
    }else{//} if(min < 60*24*7){
        rate = 10.1f;
    }
    int gem = (int)(min/rate) + 2;
    if(gem <= 0){
        return 1;
    }else{
        return gem;
    }
}
int HudLayer::getAlchyWeaponCombineMinutes(int weaponIndex){
    // 0 to ice 6 end 24
    if( weaponIndex < 6){
        return 1 + weaponIndex*5;
    }else if( weaponIndex < 18){
        return (weaponIndex - 5)*60;
    }else{
        return (weaponIndex - 17)*60*24;
    }
}
void HudLayer::onAcceptCombineInAlchyShop(){
    
}
void HudLayer::selectSamShopItem(Ref* ref){
    
}
void HudLayer::updateSamShop(){
    
}
void HudLayer::showDoctorShop(){
    WORLD->isInEvent = false;
    WORLD->pauseLayer();
    Node* layer = CSLoader::createNode("DoctorShop.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    ddiyong(layer);
    doctorShopLayer = layer;
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "evolution");
    
    
    Text* lblCoin = (Text*)layer->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    
    lblCoin = (Text*)layer->getChildByName("lblFee");
    Sprite* sptCoin = changeCoinToAnimation(layer->getChildByName("sptCoinFee"));
    sptCoin->setScale(4.5f);
    
    lbl = (Text*)layer->getChildByName("lblTip");
    lbl->setTextVerticalAlignment(TextVAlignment::BOTTOM);
    lbl->ignoreContentAdaptWithSize(false);
    lbl->setContentSize(cocos2d::Size(635, 46));
    lbl->setString(strmake("A.%s   B.%s", LM->getText("evolution").c_str(), LM->getText("cancel").c_str()));
    lbl->runAction(RepeatForever::create(Blink::create(1, 2)));
//    changeTextWithLabel(lbl, 285.24f);
    
    lbl = (Text*)layer->getChildByName("lblIngredient");
    lbl->setString(LM->getText("ingredient"));
//    changeTextWithLabel(lbl);
    
    updateDoctorShop();
    selectDoctorShopItem(doctorShopLayer->getChildByName("btnSlot0"));
}
void HudLayer::updateDoctorShop(){
   
}
void HudLayer::evolving(float dt){
    
}
void HudLayer::selectDoctorShopItem(Ref* ref){
    if(ref == nullptr) return;
    Button* btn = (Button*)ref;
    selectedDoctorShopItem = btn;
    std::string tier = UDGetStr(strmake("hero_tier_%d", 0).c_str(), "0");
    std::string ingredient0 = WORLD->getEvolutionStat(tier, strmake("ingredient %d", btn->getTag()));
    doctorShopLayer->getChildByName("sptSelect")->setPosition(btn->getPosition());
    Text* lbl = (Text*)doctorShopLayer->getChildByName("lblItemName");
    lbl->setString(LM->getText(ingredient0.c_str()));
}
void HudLayer::showCredit(){
    isInScene = true;
//    ImageView* sptRect = ImageView::create("res/258_gray_rect.png");
//    sptRect->setColor(Color3B::BLACK);
//    sptRect->setPosition(Vec2(size.width/2, size.height - 714));
//    sptRect->setContentSize(cocos2d::Size(750, 750));
//    sptRect->setAnchorPoint(Vec2(0.5, 0));
//    sptRect->setCapInsets(cocos2d::Rect(3, 3, sptRect->getContentSize().width - 6, sptRect->getContentSize().height - 6));
//    sptRect->setScale9Enabled(true);
//    this->addChild(sptRect, 8);
    
    Text* lbl;
    for (int i = 0; i < 5; i++) {
        lbl = Text::create();
        lbl->setFontSize(40);
        ((Label*)lbl->getVirtualRenderer())->setWidth(680);
        lbl->setTextHorizontalAlignment(TextHAlignment::CENTER);
        ((Label*)lbl->getVirtualRenderer())->enableShadow();
//        lbl->setColor(Color3B(38, 43, 67));
        if(i == 1 || i == 2){
            lbl->setString(strmake("%s\n%s", LMText(strmake("credit%d", i).c_str()).c_str(), LMText(strmake("credit%d_1", i).c_str()).c_str()));
        }else{
            LM->setLocalizedString(lbl, strmake("credit%d", i));
        }
        lbl->setOpacity(0);
        lbl->runAction(Sequence::create(DelayTime::create(1), DelayTime::create(i*3), FadeIn::create(1), DelayTime::create(1), FadeOut::create(1), NULL));
        this->addChild(lbl, 9);
        lbl->setPosition(Vec2(size.width/2, size.height - 714/2 - 100));
    }
    GM->nextScene = STAGE_FIELD;
//    this->runAction(Sequence::create(DelayTime::create(3*6 + 1), CallFunc::create(CC_CALLBACK_0(HudLayer::showBIAndReplaceScene, this)), NULL));
}
void HudLayer::showPeterInsuranceShop(){
    
}
void HudLayer::onChoosedInsurancePlan(Ref* ref){
    if(ref == nullptr) return;
    int index = ((Text*)ref)->getTag();
    selectPeterInsurancePlan(index);
}
void HudLayer::selectPeterInsurancePlan(int index){
    Node* newlySelectedPlan = peterShopLayer->getChildByName(strmake("lblOption%d", index));
    if(newlySelectedPlan == selectedPeterItem){
        if(index == 1 && GM->getGem() < 5){
            showInstanceMessage(LM->getText("not enough gem"));
            
            return;
        }
        newlySelectedPlan->runAction(Sequence::create(Blink::create(0.5, 2), CallFuncN::create(CC_CALLBACK_1(HudLayer::onConfirmInsurancePlan, this)), nullptr));
    }else{
        Node* cursor = peterShopLayer->getChildByName("sptCursor");
        cursor->setPositionY(newlySelectedPlan->getPosition().y);
    }
    selectedPeterItem = newlySelectedPlan;
}
void HudLayer::onConfirmInsurancePlan(Ref* ref){
    Node* lbl = (Node*)ref;
    if(lbl->getTag() == 0){
        
    }else if(lbl->getTag() == 1){
        GM->addGem(-5);
        
    }else if(lbl->getTag() == 2){
        // purchase
    }
}
void HudLayer::showPVP(){
    
}
void HudLayer::oneSecUpdate(float dt){
//    log("getPath count: %d", GM->getPathCall);
    GM->getPathCall = 0;
}
void HudLayer::showPotSoul(cocos2d::Vec2 pos){
    Sprite* spt = Sprite::createWithSpriteFrameName("potSoul.png");
    this->addChild(spt, 100);
    spt->setScale(WORLD->getScale());
    spt->setPosition(pos);
    spt->setOpacity(0);
    spt->runAction(FadeIn::create(1));
    Vec2 targetPos = Vec2(603.17f, 1209.88f);
    spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(1, Vec2(0, 100)), 2), EaseInOut::create(MoveTo::create(0.7f, targetPos), 2), CallFunc::create(CC_CALLBACK_0(HudLayer::onPotSoulMoveDone, this)), SPT_REMOVE_FUNC,NULL));
    Node* counter = hudLayer->getChildByName("potCounter");
    counter->setPosition(Vec2(900, 1166));
    counter->runAction(Sequence::create(DelayTime::create(1), MoveTo::create(0.4f, Vec2(710, 1166)), DelayTime::create(1), MoveTo::create(0.3f, Vec2(900, 1166)), NULL));
    counter->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0.4f), DelayTime::create(1), FadeOut::create(0.3f), NULL));
    Text* lbl = (Text*)counter->getChildByName("lblCount");
    int potCount = UDGetInt(KEY_POT_SOUL_COUNT, 0);
    lbl->setString(Value(potCount).asString());
    potCount++;
    UDSetInt(KEY_POT_SOUL_COUNT, potCount);
}

void HudLayer::onPotSoulMoveDone(){
    Node* counter = hudLayer->getChildByName("potCounter");
    Text* lbl = (Text*)counter->getChildByName("lblCount");
    int potCount = UDGetInt(KEY_POT_SOUL_COUNT, 0);
    lbl->setString(Value(potCount).asString());
    lbl->runAction(Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1), NULL));
}



void HudLayer::onReviveByGem(){
    int gemPrice = Value(((Text*)deadPopup->getChildByName("lblGemPrice"))->getString()).asInt();
    if(GM->getGem() < gemPrice){
        showInstanceMessage(LM->getText("not enough gem"));
        
    }else{
        GM->addGem(-gemPrice);
        WORLD->isGameOver = false;
        WORLD->isInEvent = false;
        WORLD->resumeLayer();
        WORLD->immortalForSec(10);
        this->unschedule(schedule_selector(HudLayer::updateResultPopup));
        WORLD->resumeLayer();
        closePopup();
        deadPopup = nullptr;
    }
}
void HudLayer::onReviveByVideo(){
//    GM->showVideo(VIDEO_GAME_OVER);
    deadPopup->getChildByName("btnClose")->setVisible(true);
    this->unschedule(schedule_selector(HudLayer::updateResultPopup));
}
void HudLayer::onReviveByDead(){
    float exp = WORLD->getHeroExp(0);
    int level = WORLD->getHeroLevel(0);
    float expToReduce = WORLD->getMaxExp(level)*0.3f;
    if(expToReduce > 3000){
        expToReduce = 3000;
    }
    exp -= expToReduce;
    if(exp < 0){
        exp = 0;
    }
    GM->isReviving = true;
    
    HUD->setExp(level, WORLD->getHeroExp(0), WORLD->getMaxExp(level));
    
    std::string checkPoint = UDGetStr(KEY_LAST_CHECK_POINT, "0_4_2");
    UDSetStr(KEY_LAST_SAVE_POINT, checkPoint.c_str());
}
void HudLayer::showWinPopup(bool win){
    isWon = win;
    if (WORLD->gameMode == GAME_MODE_PVP6 || WORLD->gameMode == GAME_MODE_PVP12) {
        Node* layer = this->getChildByName("pvpUI");
        Node* ndResult = layer->getChildByName("imgResult");
        ndResult->retain();
        ndResult->removeFromParentAndCleanup(false);
        GM->animateFadeIn(ndResult, layer);
        ndResult->release();
        ndResult->setVisible(true);
        Text* lblLose = (Text*)ndResult->getChildByName("lblLose");
        lblLose->setVisible(!win);
        LM->setLocalizedString(lblLose, "lose");
        Text* lblWin = (Text*)ndResult->getChildByName("lblWin");
        lblWin->setVisible(win);
        LM->setLocalizedString(lblWin, "win");
        if(win){
            GM->addGlowBack(lblWin);
        }
        Button* btn = (Button*)ndResult->getChildByName("btnOk");
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onPvpResultOk, this));
        btn->setOpacity(0);
        btn->setScale(0.1f);
        btn->runAction(Sequence::create(DelayTime::create(1), EaseBackOut::create(ScaleTo::create(0.3f, 1)), nullptr));
        btn->runAction(Sequence::create(DelayTime::create(1), FadeIn::create(0), nullptr));
        ImageView* img; int rank;Text* lbl;
        std::string strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
        ValueVector units = GM->split(strEquipped, "_");
        UnitInfo* info;
        int index = 0;
        lbl = (Text*)ndResult->getChildByName("lblPlayerName");
        lbl->setString(UDGetStr(KEY_NAME, ""));
        lbl = (Text*)ndResult->getChildByName("lblTrophyPlayer");
        int playerTrophy = UDGetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_TROPHY:KEY_PVP12_TROPHY, 1000);
        
        
        int timeLeftToSeasonOff = BSM->getTimeLeftToSunday();
        int lastTimeLeftToSeaonOff = UDGetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_LAST_TIME_LEFT:KEY_PVP12_LAST_TIME_LEFT, -1 );
        if(lastTimeLeftToSeaonOff < 0){
            UDSetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_LAST_TIME_LEFT:KEY_PVP12_LAST_TIME_LEFT, timeLeftToSeasonOff);
        }else if(lastTimeLeftToSeaonOff < timeLeftToSeasonOff){ // season changed
            playerTrophy = 1000;
            UDSetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_TROPHY:KEY_PVP12_TROPHY, 1000);
            UDSetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_LAST_TIME_LEFT:KEY_PVP12_LAST_TIME_LEFT, timeLeftToSeasonOff);
        }
        
        lbl->setString(Value(playerTrophy).asString());
        for(int i = 0; i < units.size(); i++){
            std::string str = units.at(i).asString();
            if(str.length() <= 0){
                continue;
            }
            
            info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info != nullptr && info){
                int unitType = info->unitType;
                int unitLevel = info->level;
                if(unitType >= 0){
//                    EnemyBase* unit = EnemyBase::createEnemy(unitType, 1, 0, GM->getUnitName(unitType).c_str());
                    Node* unit = GM->getHeroSpine(unitType);
//                    playerHpMax += unit->maxEnergy;
                    
                    unit->setScale(-0.5f, 0.5f);
                    img = (ImageView*)ndResult->getChildByName(strmake("img_%d", index));
                    info = GM->getUnitInfoFromString(units.at(i).asString());
                    rank = info->rank;
                    img->setColor(GM->getRankColor(rank));
                    ndResult->addChild(unit);
                    unit->setPosition(img->getPosition() + Vec2(0, -img->getContentSize().height/4));
                    lbl = (Text*)img->getChildByName("lblLevel_5");
                    lbl->setString(strmake("Lv.%d", unitLevel + 1));
                    
                    bool isAlive = false;
                    for (int j = 0; j < WORLD->heroArray.size(); j++) {
                        EnemyBase* unitLeft = WORLD->heroArray.at(j);
                        
                        if (unitLeft->getTag() == index){
                            isAlive = true;
                            break;
                        }
                    }
                    if (!isAlive) {
                        unit->setOpacity(100);
                        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
                        img->addChild(spt, 5);
                        spt->setScale(0.7f);
                        spt->setOpacity(0);
                        spt->setPosition(img->getContentSize()/2);
                        Animation* animation = AnimationCache::getInstance()->getAnimation("cartoonyExplosion");
                        Animate* animate = Animate::create(animation);
                        spt->runAction(Sequence::create(DelayTime::create(0.5f), FadeIn::create(0), animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                    }
                }
            }
            index++;
        }
        
        strEquipped = BSM->pvpTargetData;
        units = GM->split(strEquipped, "_");
        index = 0;
        lbl = (Text*)ndResult->getChildByName("lblEnemyName");
        lbl->setString(BSM->pvpTargetName);
        lbl = (Text*)ndResult->getChildByName("lblTrophyEnemy");
        lbl->setString(Value(BSM->pvpTargetTrophy).asString());
        for(int i = 0; i < units.size(); i++){
            std::string str = units.at(i).asString();
            if(str.length() <= 0){
                continue;
            }
            
            info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info != nullptr && info){
                int unitType = info->unitType;
                int unitLevel = info->level;
                if(unitType >= 0){
//                    EnemyBase* unit = EnemyBase::createEnemy(unitType, 1, 0, GM->getUnitName(unitType).c_str());
                    Node* unit = GM->getHeroSpine(unitType);
                    unit->setScale(0.5f);
                    img = (ImageView*)ndResult->getChildByName(strmake("imgEnemy_%d", index));
                    info = GM->getUnitInfoFromString(units.at(i).asString());
                    rank = info->rank;
                    
                    img->setColor(GM->getRankColor(rank));
                    ndResult->addChild(unit);
                    unit->setPosition(img->getPosition() + Vec2(0, -img->getContentSize().height/4));
                    lbl = (Text*)img->getChildByName("lblLevel_5");
                    lbl->setString(strmake("Lv.%d", unitLevel + 1));
                    
                    bool isAlive = false;
                    for (int j = 0; j < WORLD->enemyArray.size(); j++) {
                        EnemyBase* unitLeft = WORLD->enemyArray.at(j);
                        if(unitLeft->getTag() == i){
                            isAlive = true;
                            break;
                        }
                    }
                    if (!isAlive) {
                        unit->setOpacity(100);
                        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
                        img->addChild(spt, 5);
                        spt->setScale(0.7f);
                        spt->setOpacity(0);
                        spt->setPosition(img->getContentSize()/2);
                        Animation* animation = AnimationCache::getInstance()->getAnimation("cartoonyExplosion");
                        Animate* animate = Animate::create(animation);
                        spt->runAction(Sequence::create(DelayTime::create(0.5f), FadeIn::create(0), animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
                    }
                }
            }
            index++;
        }
        
        int upDown = 0;
        if (win) {
            if (playerTrophy < BSM->pvpTargetTrophy) {
                upDown = 10;
            }else{
                upDown = 3;
            }
        }else{
            if (playerTrophy > BSM->pvpTargetTrophy) {
                upDown = -10;
            }else{
                upDown = -3;
            }
        }
        playerTrophy += upDown;
        lbl = (Text*)ndResult->getChildByName("lblTrophyUpDownPlayer");
        lbl->setString((upDown>0?"+":"") + Value(upDown).asString());
        lbl->setTextColor(upDown>0?Color4B::GREEN:Color4B::RED);
        
        UDSetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_TROPHY:KEY_PVP12_TROPHY, playerTrophy);
        log("%d/%d", GM->pvpOpenDate , BSM->getDay());
        if(WORLD->gameMode == GAME_MODE_PVP6){
            if(GM->pvpOpenDate == BSM->getDay()){
                BSM->sendPvp6Result(playerTrophy);
            }
            if(win){
                GM->addMonthlyEventProgress(EVENT_MISSION_PVP_6_WIN, 1);
            }
        }else{
            if(GM->pvpOpenDate == BSM->getDay()){
                BSM->sendPvp12Result(playerTrophy);
            }
            if(win){
                GM->addMonthlyEventProgress(EVENT_MISSION_PVP_12_WIN, 1);
            }
        }
    }else{
        GameSharing::logFB(strmake("STAGE %d %s", GM->currentStageIndex, win?"WIN":"LOSE").c_str());
        Node* popup = Node::create();
        popup->setName("winPopup");
        popup->setTag(win?1:0);
        this->addChild(popup, 200);
        resultPopup = popup;
        ImageView* imgBack = ImageView::create("uiBox.png");
        popup->addChild(imgBack, -1);
        imgBack->setColor(Color3B::BLACK);
        imgBack->setOpacity(100);
        imgBack->setScale(20);
        imgBack->setTouchEnabled(true);
        
        ImageView* img = ImageView::create("uiBox.png");
        img->setContentSize(cocos2d::Size(1300, 850));
        img->setScale9Enabled(true);
        img->setPosition(Vec2(size.width/2, size.height/2 + 100));
        popup->addChild(img);
        
        std::string strResult = LM->getText("mission cleared");
        if (!win) {
            strResult = LM->getText("mission failed");
        }
        //    Label* lbl = LM->getLocalizedLabel(strResult.c_str(), Color4B::BLACK, 70);
        Node* title = PPLabel::create(strResult, 70, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        title->setPosition(Vec2(size.width/2, size.height/2 + 400));
        popup->addChild(title);
        
        int leftX = size.width/2 - 350;
        int iconX = size.width/2 - 450;
        int rightX = size.width/2 + 500;
        int startY = size.height/2 + 250;
        int gapY = 100;
        int fontSize = 40;
        Node* customLabel;
        Sprite* spt;
        for(int i = 0; i < 5; i++){
            if (i == 0) {
                customLabel = PPLabel::create(LM->getText("time"), fontSize, DARK_GRAY_3B, true, false, TextHAlignment::LEFT, false);
                customLabel->setName("lblTimeTitle");
                spt = Sprite::create("iconTime.png");
                spt->setName("sptTime");
                //            lbl = LM->getLocalizedLabel("Time", Color4B::BLACK, fontSize);
                //            lbl->setName("lblTimeTitle");
            }else if (i == 1) {
                customLabel = PPLabel::create(strmake("%s (%s/%s)", LM->getText("gold text").c_str(), LM->getText("used").c_str(), LM->getText("earned").c_str()), fontSize, DARK_GRAY_3B, true, false, TextHAlignment::LEFT, false);
                customLabel->setName("lblGoldTitle");
                spt = Sprite::create("iconGold.png");
                spt->setName("sptGold");
                //            lbl = LM->getLocalizedLabel("Gold (Used/Earnd)", Color4B::BLACK, fontSize);
                //            lbl->setName("lblGoldTitle");
            }else if (i == 2) {
                customLabel = PPLabel::create(strmake("%s (%s/%s)", LM->getText("lumber text").c_str(), LM->getText("used").c_str(), LM->getText("earned").c_str()), fontSize, DARK_GRAY_3B, true, false, TextHAlignment::LEFT, false);
                customLabel->setName("lblLumberTitle");
                spt = Sprite::create("iconLumber.png");
                spt->setName("sptLumber");
                //            lbl = LM->getLocalizedLabel("Lumber (Used/Earnd)", Color4B::BLACK, fontSize);
                //            lbl->setName("lblLumberTitle");
            }else if (i == 3) {
                customLabel = PPLabel::create(LM->getText("produced unit"), fontSize, DARK_GRAY_3B, true, false, TextHAlignment::LEFT, false);
                customLabel->setName("lblProduceTitle");
                spt = Sprite::create("iconPopulation.png");
                spt->setName("sptPopulation");
                //            lbl = LM->getLocalizedLabel("Produced Unit", Color4B::BLACK, fontSize);
                //            lbl->setName("lblProduceTitle");
            }else if (i == 4) {
                customLabel = PPLabel::create(LM->getText("killed unit"), fontSize, DARK_GRAY_3B, true, false, TextHAlignment::LEFT, false);
                customLabel->setName("lblKillTitle");
                spt = Sprite::create("iconKilled.png");
                spt->setName("sptKilled");
                //            lbl = LM->getLocalizedLabel("Killed Unit", Color4B::BLACK, fontSize);
                //            lbl->setName("lblKillTitle");
            }
            customLabel->setVisible(false);
            customLabel->setPosition(Vec2(leftX, startY - gapY*i));
            popup->addChild(customLabel);
            spt->setPosition(Vec2(iconX, customLabel->getPositionY()));
            popup->addChild(spt);
            spt->setVisible(false);
            
            if (i == 0) {
                customLabel = PPLabel::create("00:00:00", fontSize, DARK_GRAY_3B, true, false, TextHAlignment::RIGHT, false);
                customLabel->setName("lblTime");
                //            lbl = LM->getLocalizedLabel("00:00:00", Color4B::BLACK, fontSize);
                //            lbl->setName("lblTime");
            }else if (i == 1) {
                customLabel = PPLabel::create("0/0", fontSize, DARK_GRAY_3B, true, false, TextHAlignment::RIGHT, false);
                customLabel->setName("lblGold");
                //            lbl = LM->getLocalizedLabel("0/0", Color4B::BLACK, fontSize);
                //            lbl->setName("lblGold");
            }else if (i == 2) {
                customLabel = PPLabel::create("0/0", fontSize, DARK_GRAY_3B, true, false, TextHAlignment::RIGHT, false);
                customLabel->setName("lblLumber");
                //            lbl = LM->getLocalizedLabel("0/0", Color4B::BLACK, fontSize);
                //            lbl->setName("lblLumber");
            }else if (i == 3) {
                customLabel = PPLabel::create("0", fontSize, DARK_GRAY_3B, true, false, TextHAlignment::RIGHT, false);
                customLabel->setName("lblProduce");
                //            lbl = LM->getLocalizedLabel("0", Color4B::BLACK, fontSize);
                //            lbl->setName("lblProduce");
            }else if (i == 4) {
                customLabel = PPLabel::create("0", fontSize, DARK_GRAY_3B, true, false, TextHAlignment::RIGHT, false);
                customLabel->setName("lblKill");
                //            lbl = LM->getLocalizedLabel("0", Color4B::BLACK, fontSize);
                //            lbl->setName("lblKill");
            }
            customLabel->setPosition(Vec2(rightX, startY - gapY*i));
            //        lbl->setAnchorPoint(Vec2(1, 0.5));
            customLabel->setVisible(false);
            popup->addChild(customLabel);
        }
        
        Button* btn = Button::create("uiBoxSmall.png");
        popup->addChild(btn);
        //    btn->setColor(Color3B::GRAY);
        btn->setName("btnOk");
        btn->setVisible(false);
        //    btn->setTitleText("OK");
        //    btn->setTitleFontName(LM->getLocalizedFont());
        //    btn->setTitleFontSize(50);
        btn->setContentSize(cocos2d::Size(300, 200));
        btn->setScale9Enabled(true);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onOkFromWinPopup, this));
        btn->setPosition(Vec2(size.width/2 + 500, size.height/2 - 450));
        //    btn->setColor(Color3B(244, 236, 5));
        addLabelToButton("ok", btn, false, DARK_GRAY_3B);
        
        spt = Sprite::create("redFlag.png");
        spt->setPosition(Vec2(size.width/2, size.height/2 + 850/2 + 110 - spt->getContentSize().height/2));
        spt->setAnchorPoint(Vec2(0.18, 0.1));
        popup->addChild(spt);
        
        if(!win){
            spt->runAction(Sequence::create(DelayTime::create(1.5f), EaseIn::create(RotateBy::create(0.5f, -90), 2), NULL));
        }
        
        this->schedule(schedule_selector(HudLayer::updateResultPopup), 0.1f);
        
        int chapter = WORLD->stageIndex/12;
//        UDSetInt(strmake(KEY_`_STAGE_INDEX_FORMAT, chapter).c_str(), 1); // test
        int stageIndex = WORLD->stageIndex%12;
        int chestIndex = UDGetInt(strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(), 0);
        int timeLeft = GM->getTimeLeftForCampaignChest(chapter);
        bool isArena = GM->isColosseum;
        if (win && timeLeft <= 0 && stageIndex == chestIndex && !isArena) {
//        if (win && stageIndex == chestIndex) { // test
            time_t now = BSM->getCurrentTimeT();
//            const char* strNow = BSM->getStrFromTime(now).c_str();
            std::string strTimeKey = strmake(KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT, chapter).c_str();
//            UDSetStr(strTimeKey.c_str(), strNow);
            GM->saveTime(strTimeKey.c_str(), now);
            
            chestIndex++;
            UDSetInt(strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(), chestIndex);
            
            Node* layer = CSLoader::createNode("CampaignChest.csb");
            this->addChild(layer, POPUP_ZORDER);
            layer->setPosition(Vec2(size.width/2 - layer->getContentSize().width/2, size.height/2 - layer->getContentSize().height/2));
            layer->setName("campaignChest");
            Button* btn = (Button*)layer->getChildByName("btnOpen");
            btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onOpenCampaignChestClick, this));
            btn->setOpacity(0);
            Node* imgChest = layer->getChildByName("imgChest");
            GM->makeItScaleUpAndDown(imgChest);
            GM->addGlowBack(imgChest);
            GM->shakeIt(imgChest, 10, 5);
            for (int i = 0; i < 3; i++) {
                layer->getChildByName(strmake("imgPos%d", i))->setOpacity(0);
            }
            btn = (Button*)layer->getChildByName("btnOk");
            btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onOkCampaignChestClick, this));
        }
    }
}
void HudLayer::onOpenCampaignChestClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    Node* layer = this->getChildByName("campaignChest");
    Node* imgChest = layer->getChildByName("imgChest");
    imgChest->stopAllActions();
    imgChest->runAction(Sequence::create(EaseOut::create(MoveBy::create(1.0f, Vec2(0, 100)), 2), JumpBy::create(0.4f, Vec2(0, -100), 200, 1), CallFunc::create(CC_CALLBACK_0(HudLayer::onOpenCampaignChest, this)),NULL));
}
void HudLayer::onOpenCampaignChest(){
    Node* layer = this->getChildByName("campaignChest");
    ImageView* imgChest = (ImageView*)layer->getChildByName("imgChest");
    imgChest->loadTexture("chestWoodOpen.png");
    ImageView* img; ImageView* imgTemp;
    bool saveGem = false;
    for (int i = 0; i < 3; i++) {
        int reward = rand()%100;
        if (reward < 20) {
            imgTemp = (ImageView*)layer->getChildByName("imgGold");
            GM->addCoin(150);
        }else if (reward < 50) {
            imgTemp = (ImageView*)layer->getChildByName("imgLumber");
            GM->addTree(100);
        }else if (reward < 90) {
            imgTemp = (ImageView*)layer->getChildByName("imgEventItem");
            ImageView* imgIcon = (ImageView*)imgTemp->getChildByName("imgIcon");
            int month = BSM->getMonth();
            imgIcon->loadTexture(strmake("eventItem%d.png", month));
            if(month == 12){
                imgIcon->setContentSize(cocos2d::Size(73, 88));
            }else if(month == 1){
                imgIcon->setContentSize(cocos2d::Size(85, 60));
            }else if(month == 2){
                imgIcon->setContentSize(cocos2d::Size(88, 88));
            }else if(month == 3){
                imgIcon->setContentSize(cocos2d::Size(75, 76));
            }
            GM->addMonthlyEventProgress(EVENT_MISSION_COLLECT_EVENT_HERO_PART, 1);
        }else{
            imgTemp = (ImageView*)layer->getChildByName("imgGem");
            GM->addGem(10);
            saveGem = true;
        }
            
        img = (ImageView*)imgTemp->clone();
        layer->addChild(img);
        img->setPosition(layer->getChildByName(strmake("imgPos%d", i))->getPosition());
        img->setOpacity(0);
        img->runAction(Sequence::create(DelayTime::create(0.5f*i), FadeIn::create(0), JumpBy::create(0.3f, Vec2(0, 0), 50, 1), NULL));
    }
    if(saveGem){
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_GEM);
        BSM->saveUserData(datas);
    }
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->runAction(Sequence::create(DelayTime::create(1.2f), JumpBy::create(0.5f, Vec2(0, 650), 200, 1), NULL));
}
void HudLayer::onOkCampaignChestClick(){
    this->removeChildByName("campaignChest");
}
void HudLayer::onPvpResultOk(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    this->removeListener();
    Scene* scene = Scene::create();
    if(GM->isPvpFromBHUD){
        GM->isPvpFromBHUD = false;
        GM->titleLayer = nullptr;
        GM->nextScene = STAGE_LOBBY;
        GM->isColosseum = false;
        auto scene = HelloWorld::scene(STAGE_LOBBY, false);
        BHUD->onTrainClick();
        BHUD->onHeroClick();
        Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
    }else{
        Title* title = Title::create();
        scene->addChild(title);
        Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
        title->showChapterSelect();
        title->onHeroClick();
    }
}
void HudLayer::showPremiumRetry(){
    Node* layer = CSLoader::createNode("PremiumRetry.csb");
    this->addChild(layer, 5);
    layer->setPosition(Vec2(size.width/2 - layer->getContentSize().width/2, size.height/2 - layer->getContentSize().height/2));
    setAsPopup(layer);
    
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    imgBack->setContentSize(cocos2d::Size(300, 1038));
    imgBack->runAction(EaseInOut::create(ResizeTo::create(0.3, cocos2d::Size(1334, 1038)), 2));
    
    ImageView* contentBox = (ImageView*)layer->getChildByName("contentBox");
    contentBox->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    lbl->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));
    LM->setLocalizedString(lbl, "premium retry");
    
    lbl = (Text*)contentBox->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, "premium retry desc");
    
    Button* btn = (Button*)layer->getChildByName("btnBuy");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::onBuyPremiumRetryClick, this));
    btn->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "buy");
    
    btn = (Button*)layer->getChildByName("btnQuit");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::closePopup, this));
    btn->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "exit");
}
void HudLayer::onBuyPremiumRetryClick(){
    showIndicator();
    GameSharing::buyItem(IAP_DETAIL_PREMIUM_RETRY);
}
void HudLayer::updateResultPopup(float dt){
    PPLabel* lbl = (PPLabel*)resultPopup->getChildByName("lblTimeTitle");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        resultPopup->getChildByName("sptTime")->setVisible(true);
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblTime");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        return;
    }
    resultTime += 30;
    if(resultTime > WORLD->gameTimer){
        resultTime = WORLD->gameTimer;
        lbl->setString(GM->getTimeLeftInString(resultTime));
    }else{
        lbl->setString(GM->getTimeLeftInString(resultTime));
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblGoldTitle");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        resultPopup->getChildByName("sptGold")->setVisible(true);
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblGold");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        return;
    }
    resultUsedGold += 1000;
    resultEarndGold += 1000;
    if (resultUsedGold > WORLD->totalUsedGold) {
        resultUsedGold = WORLD->totalUsedGold;
    }
    if (resultEarndGold > WORLD->totalEarnedGold) {
        resultEarndGold = WORLD->totalEarnedGold;
        lbl->setString(strmake("%d/%d", resultUsedGold, resultEarndGold));
    }else{
        lbl->setString(strmake("%d/%d", resultUsedGold, resultEarndGold));
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblLumberTitle");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        resultPopup->getChildByName("sptLumber")->setVisible(true);
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblLumber");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        return;
    }
    resultUsedLumber += 1000;
    resultEarndLumber += 1000;
    if (resultUsedLumber > WORLD->totalUsedLumber) {
        resultUsedLumber = WORLD->totalUsedLumber;
    }
    if (resultEarndLumber > WORLD->totalEarnedLumber) {
        resultEarndLumber = WORLD->totalEarnedLumber;
        lbl->setString(strmake("%d/%d", resultUsedLumber, resultEarndLumber));
    }else{
        lbl->setString(strmake("%d/%d", resultUsedLumber, resultEarndLumber));
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblProduceTitle");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        resultPopup->getChildByName("sptPopulation")->setVisible(true);
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblProduce");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        return;
    }
    resultProduce += 5;
    if(resultProduce > WORLD->totalProducedUnit){
        resultProduce = WORLD->totalProducedUnit;
        lbl->setString(Value(resultProduce).asString());
    }else{
        lbl->setString(Value(resultProduce).asString());
        return;
    }
    
    lbl = (PPLabel*)resultPopup->getChildByName("lblKillTitle");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        resultPopup->getChildByName("sptKilled")->setVisible(true);
        return;
    }
    lbl = (PPLabel*)resultPopup->getChildByName("lblKill");
    if(!lbl->isVisible()){
        lbl->setVisible(true);
        return;
    }
    resultKill += 5;
    if(resultKill > WORLD->totalKillUnit){
        resultKill = WORLD->totalKillUnit;
        lbl->setString(Value(resultKill).asString());
    }else{
        lbl->setString(Value(resultKill).asString());
        return;
    }
    
    resultPopup->getChildByName("btnOk")->setVisible(true);
    if(WORLD->isGameOver && !UDGetBool(KEY_PREMIUM_START, false) && (WORLD->gameMode == GAME_MODE_NORMAL || WORLD->gameMode == GAME_MODE_HARD)){
        showPremiumRetry();
    }
    
    this->unschedule(schedule_selector(HudLayer::updateResultPopup));
}

void HudLayer::onOkFromWinPopup(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    btn->setEnabled(false);
    
    if(!UDGetBool(KEY_RATE_POPUP_NEVER_SHOW, false) && (GM->market == MARKET_PAID || GM->market == MARKET_FREE)){ // review test
//    if(true){ // test 
        Node* popup = Node::create();
        this->addChild(popup, 200);
        ImageView* imgBack = ImageView::create("uiBox.png");
        popup->addChild(imgBack, -1);
        imgBack->setColor(Color3B::BLACK);
        imgBack->setOpacity(100);
        imgBack->setScale(20);
        imgBack->setTouchEnabled(true);
        
        ImageView* img = ImageView::create("uiBox.png");
        img->setContentSize(cocos2d::Size(1300, 850));
        img->setScale9Enabled(true);
        img->setPosition(Vec2(size.width/2, size.height/2 + 100));
        popup->addChild(img);
        
        Node* title = PPLabel::create(LM->getText("enjoy game"), 70, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        title->setPosition(Vec2(size.width/2, size.height/2));
        popup->addChild(title);
        PPLabel* lbl = (PPLabel*)title;
        lbl->setWidth(1100);
        
        title = PPLabel::create(LM->getText("rate game"), 50, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
        title->setPosition(Vec2(size.width/2, size.height/2 - 150));
        popup->addChild(title);
        
        EnemyBase* unit = EnemyBase::createEnemy(UNIT_TROLL, 0, 0, "trollStand0.png");
        popup->addChild(unit);
        unit->setPosition(Vec2(size.width/2, size.height/2 + 280));
        unit->runAnimation("trollStand", true);
        
        Button* btn = Button::create("uiBoxSmall.png");
        popup->addChild(btn);
        btn->setContentSize(cocos2d::Size(300, 200));
        btn->setTag(0);
        btn->setScale9Enabled(true);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onReivewPopupButtonClick, this));
        btn->setPosition(Vec2(size.width/2 + 500, size.height/2 - 450));
        addLabelToButton("ok", btn, false, DARK_GRAY_3B);
        
        btn = Button::create("uiBoxSmall.png");
        popup->addChild(btn);
        btn->setTag(1);
        btn->setContentSize(cocos2d::Size(300, 200));
        btn->setScale9Enabled(true);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onReivewPopupButtonClick, this));
        btn->setPosition(Vec2(size.width/2 + 200, size.height/2 - 450));
        addLabelToButton("later", btn, false, DARK_GRAY_3B);
        
        btn = Button::create("uiBoxSmall.png");
        popup->addChild(btn);
        btn->setTag(2);
        btn->setContentSize(cocos2d::Size(640, 200));
        btn->setScale9Enabled(true);
        btn->addClickEventListener(CC_CALLBACK_1(HudLayer::onReivewPopupButtonClick, this));
        btn->setPosition(Vec2(size.width/2 - 320, size.height/2 - 450));
        addLabelToButton("never show", btn, false, DARK_GRAY_3B);
        return;
    }
    
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    GM->setHudLayer(nullptr);
    int stage = WORLD->stageIndex;
    
    Node* popup = this->getChildByName("winPopup");
    if(popup != nullptr && popup->getTag() == 1 && !WORLD->isGameOver){
        
        if (WORLD->isHardMode) {
            int clearStage = UDGetInt(KEY_HARD_MODE_CLEAR_STAGE, -1);
            if(clearStage < stage){
                UDSetInt(KEY_HARD_MODE_CLEAR_STAGE, stage);
            }
        }else{
            int clearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
            if(clearStage < stage){
                UDSetInt(KEY_LAST_CLEAR_STAGE, stage);
                std::vector<int> datas;
                datas.push_back(DATA_TYPE_STAGE_CLEAR);
                BSM->saveUserData(datas);
            }
        }
    }
    setGameSpeed(1);
    bool isWin = !WORLD->isGameOver;
    this->removeListener();
    Scene* scene;
    if(GM->currentStageIndex == 11 && isWin){
        GM->nextScene = STAGE_INTRO;
        scene = HelloWorld::scene(12, false);
    }else{
        scene = Scene::create();
        Title* title = Title::create();
        scene->addChild(title);
        if (isWin) {
            stage++;
            if (stage < 13) { // test  5 for google indie festival 13(12 + bonus) for launch
//                Node* temp = Node::create();
//                temp->setTag(stage);
//                title->onStageClick(temp);
                title->stageRequested = stage;
            }else{
                // show ending
            }
        }
    }
    
    if(GM->isAdsUser()){
        GameSharing::showInterstitial();
    }
    GM->isThisCampaignFromDailyMission = false;
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}

void HudLayer::onReivewPopupButtonClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    if(btn->getTag() == 0){
        if (GM->market == MARKET_PAID) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            Application::getInstance()->openURL("itms-apps://itunes.apple.com/app/id1389133177?action=write-review");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            Application::getInstance()->openURL("https://play.google.com/store/apps/details?id=com.magmon.cartooncraft");
#endif
        }else if (GM->market == MARKET_FREE) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            Application::getInstance()->openURL("itms-apps://itunes.apple.com/app/id1389133177?action=write-review");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            Application::getInstance()->openURL("https://play.google.com/store/apps/details?id=com.magmon.cartooncraftf");
#endif
        }

    }else if(btn->getTag() == 1){
        
    }else if(btn->getTag() == 2){
        UDSetBool(KEY_RATE_POPUP_NEVER_SHOW, true);
    }
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    GM->setHudLayer(nullptr);
    setGameSpeed(1);
    Scene* scene;
    if(GM->currentStageIndex == 11 && !WORLD->isGameOver){
        GM->nextScene = STAGE_INTRO;
        scene = HelloWorld::scene(12, false);
    }else{
        
        int stage = WORLD->stageIndex;
        if (!WORLD->isGameOver) {
            if (WORLD->isHardMode) {
                int clearStage = UDGetInt(KEY_HARD_MODE_CLEAR_STAGE, -1);
                if(clearStage < stage){
                    UDSetInt(KEY_HARD_MODE_CLEAR_STAGE, stage);
                }
            }else{
                int clearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
                if(clearStage < stage){
                    UDSetInt(KEY_LAST_CLEAR_STAGE, stage);
                }
            }
            stage++;
        }
        scene = Scene::create();
        Title* title = Title::create();
        scene->addChild(title);
        if (stage < 13) { // test  5 for google indie festival 13(12 + bonus) for launch
            //                Node* temp = Node::create();
            //                temp->setTag(stage);
            //                title->onStageClick(temp);
            title->stageRequested = stage;
        }else{
            // show ending
        }
    }
    this->removeListener();
    GM->isThisCampaignFromDailyMission = false;
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}

void HudLayer::arrangeMenu(cocos2d::Vec2 pos){
    if(GM->currentStageIndex == STAGE_LOBBY) return;
    for (int i = 0; i < 6; i++) {
        Button* btn = (Button*)WORLD->getChildByName(strmake("btnMenu%d", i));
        int radius = 270/WORLD->layerScale;
        btn->setPosition(pos + Vec2(0, 40));
        btn->stopAllActions();
        btn->setOpacity(0);
        btn->runAction(FadeIn::create(0.16f));
        btn->setScale(1/WORLD->layerScale);
        float angle = i*60 - 120;
//        btn->setEnabled(false);
        btn->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.16f, Vec2(cos(angle*3.14f/-180)*radius, sin(angle*3.14f/-180)*radius)), 2), nullptr));
        btn->setLocalZOrder(1000);
    }
}
void HudLayer::showIndicator(){
    Button* btn = Button::create("uiBoxSmall.png");
    this->addChild(btn, 1000);
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    btn->setPosition(size/2);
    btn->setName("btnIndicator");
    btn->addClickEventListener(CC_CALLBACK_0(HudLayer::hideIndicator, this));
    
    Label* lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
    this->addChild(lbl, 1001);
    lbl->setPosition(size/2);
    lbl->setName("lblIndicator");
}
void HudLayer::hideIndicator(){
    this->removeChildByName("btnIndicator");
    this->removeChildByName("lblIndicator");
}
void HudLayer::update(float dt){
    if(isVideoDone){
        isVideoDone = false;
        WORLD->getSupportFromVideo(GM->videoIndex);
        closePopup();
        onResumeClick();
        onMenuClick();
    }
    if(hideIndicatorRequested){
        hideIndicatorRequested = false;
        hideIndicator();
    }
    
    if(lblVs != nullptr){
        int timeLeft = 60 - GM->getWorld()->gameTimer;
        if(timeLeft < 0){
            lblVs->setString("0");
            GM->getWorld()->endGame(imgPvpPlayerEnergy->getContentSize().width >= imgPvpEnemyEnergy->getContentSize().width);
        }else if(timeLeft < 60){
            lblVs->setString(strmake("%d", timeLeft));
        }
    }
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->iapFlag = IAP_FLAG_INIT;
        if(GM->iapDetail.compare(IAP_DETAIL_PREMIUM_RETRY) == 0){
            log("iap success!");
            GM->iapFlag = IAP_FLAG_INIT;
            showInstanceMessage(LM->getText("purchase success"));
            UDSetBool(KEY_PREMIUM_START, true);
            setGameSpeed(1);
            removeListener();
            GM->nextScene = STAGE_FIELD;
            GM->isColosseum = false;
            GM->setHudLayer(nullptr);
            auto scene = HelloWorld::scene(GM->currentStageIndex, WORLD->isHardMode);
            Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
        }
    }else if(GM->iapFlag == IAP_FLAG_FAILED){
        GM->iapFlag = IAP_FLAG_INIT;
        log("iap failed!");
        hideIndicator();
    }
    
    if (GM->raidMatchState == NETWORK_HANDLE_STATE_ARRIVED) {
        GM->raidMatchState = NETWORK_HANDLE_STATE_HANDLED;
        removeListener();
        
        GM->nextScene = STAGE_RAID;
        auto scene = HelloWorld::scene(GM->nextScene, false);
        Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
    }
}

void HudLayer::setPvpUI(int playerHPMax, int enemyHPMax){
    Node* layer = CSLoader::createNodeWithVisibleSize("PvpUI.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setName("pvpUI");
    Node* imgVSBack = layer->getChildByName("imgVSBack");
    Text* lbl = (Text*)imgVSBack->getChildByName("lblPlayerName");
    lbl->setString(UDGetStr(KEY_NAME, "ME"));
    lbl = (Text*)imgVSBack->getChildByName("lblEnemyName");
    lbl->setString(BSM->pvpTargetName);
    imgPvpPlayerEnergy = (ImageView*)layer->getChildByName("imgPlayerEnergy");
    imgPvpEnemyEnergy = (ImageView*)layer->getChildByName("imgEnemyEnergy");
    lblPvpCount = (Text*)layer->getChildByName("lblCount");
    lblVs = (Text*)layer->getChildByName("lblVs");
    
    playerHpMax = playerHPMax;
    enemyHpMax = enemyHPMax;
    
    lbl = (Text*)imgVSBack->getChildByName("lblPlayerTrophy");
    lbl->setString(Value(UDGetInt(WORLD->gameMode == GAME_MODE_PVP6?KEY_PVP6_TROPHY:KEY_PVP12_TROPHY, 1000)).asString());
    lbl = (Text*)imgVSBack->getChildByName("lblEnemyTrophy");
    lbl->setString(Value(BSM->pvpTargetTrophy).asString());
    
    Node* ndResult = layer->getChildByName("imgResult");
    ndResult->setVisible(false); // test
    
}
void HudLayer::updatePvpUI(int playerHP, int enemyHP){
    imgPvpPlayerEnergy->setContentSize(cocos2d::Size(1000.0f*playerHP/playerHpMax,115.82f));
    if(playerHP/playerHpMax < 0.3){
        imgPvpPlayerEnergy->setColor(Color3B(248, 59, 7));
    }
    imgPvpEnemyEnergy->setContentSize(cocos2d::Size(1000.0f*enemyHP/enemyHpMax,115.82f));
    if(enemyHP/enemyHpMax < 0.3){
        imgPvpEnemyEnergy->setColor(Color3B(248, 59, 7));
    }
}
