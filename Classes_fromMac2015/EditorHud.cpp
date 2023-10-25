//
//  EditorHud.cpp
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//

#include "EditorHud.h"
#include "LegendDaryButton.h"
#include "GameManager.h"
#include "EditorWorld.h"

//#include "NativeInterface.h"
#include "LanguageManager.h"

#include "BuggyServerManager.h"
#include "Title.h"
#include "ui/UITextBMFont.h"
#include "ui/UIScrollView.h"
bool EditorHud::init()
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
    listener->onKeyPressed = CC_CALLBACK_2(EditorHud::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(EditorHud::onKeyReleased, this);
    
    
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
    this->schedule(schedule_selector(EditorHud::oneSecUpdate), 1);
    
    //    lblCommand = Label::createWithSystemFont("COMMAND: ", "Thonburi", 100);
    //    this->addChild(lblCommand);
    //    lblCommand->setPosition(Vec2(size.width/2, lblCommand->getBoundingBox().size.height/2 + 50));
    
    Button* btn;
    
    
    if(GM->nextScene == STAGE_FIELD){
        int offsetX = 0;
        if(size.height/size.width < 700.0f/1334.0f){
            offsetX = 100;
        }
        btn = Button::create("uiBoxSmallBlue.png");
        this->addChild(btn, 99);
        //        btn->setTitleText("Menu");
        //        btn->setTitleFontName(LM->getLocalizedFont());
        //        btn->setTitleFontSize(50);
        //        btn->setTitleColor(Color3B::BLACK);
        btn->setContentSize(cocos2d::Size(220, 100));
        btn->setScale9Enabled(true);
        btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onMenuClick, this));
        btn->setPosition(Vec2(offsetX + 327, 480));
        addLabelToButton("menu", btn, false, DARK_GRAY_3B);
        
        ImageView* img = ImageView::create("uiBoxSmallBlue.png");
        this->addChild(img, 99);
        img->setContentSize(cocos2d::Size(250, 100));
        img->setScale9Enabled(true);
        img->setPosition(Vec2(offsetX + 125, 480));
        
        lblTimer = PPLabel::create("00:00:00", 50, DARK_GRAY_3B, false, false, TextHAlignment::CENTER, false);//LM->getLocalizedLabel();
        lblTimer->setPosition(Vec2(offsetX + 125, 480));
        lblTimer->setScaleX(0.8f);
        this->addChild(lblTimer, 99);
    }
    
    GM->setEditorHud(this);
    svBrushes = ScrollView::create();
    this->addChild(svBrushes);
    svBrushes->setColor(Color3B::RED);
    
    svBrushes->setContentSize(cocos2d::Size(menuWidth, size.height));
    svBrushes->setPosition(Vec2(size.width - menuWidth, 0));
    
    int menuGap = 5;
    svBrushes->setInnerContainerSize(cocos2d::Size(menuWidth, (menuWidth+menuGap)*brushCount));
    for(int i = BRUSH_TREE; i < brushCount + BRUSH_TREE; i++){
        btn = Button::create("uiBox.png", "uiBox.png");
        btn->setPosition(Vec2(0, (menuWidth + menuGap)*(brushCount - i)));
        btn->setAnchorPoint(Vec2(0, 0));
        btn->setScale(menuWidth/btn->getContentSize().width);
        Sprite* spt;
        if(i ==  BRUSH_TREE ){
            spt = world->getSpriteForIcon(UNIT_TREE);
        }else if(i ==  BRUSH_MINE ){
            spt = world->getSpriteForIcon(UNIT_MINE);
        }else if(i ==  BRUSH_SWORDSMAN ){
            spt = world->getSpriteForIcon(UNIT_SWORDMAN);
        }else if(i ==  BRUSH_ARCHER ){
            spt = world->getSpriteForIcon(UNIT_ARCHER);
        }else if(i ==  BRUSH_WORKER ){
            spt = world->getSpriteForIcon(UNIT_WORKER);
        }else if(i ==  BRUSH_CATAPULT ){
            spt = world->getSpriteForIcon(UNIT_CATAPULT);
        }else if(i ==  BRUSH_HELICOPTER ){
            spt = world->getSpriteForIcon(UNIT_HELICOPTER);
        }else if(i ==  BRUSH_CASTLE ){
            spt = world->getSpriteForIcon(UNIT_CASTLE);
        }else if(i ==  BRUSH_FARM ){
            spt = world->getSpriteForIcon(UNIT_FARM);
        }else if(i == BRUSH_BARRACKS ){
            spt = world->getSpriteForIcon(UNIT_BARRACKS);
        }else if(i == BRUSH_LUMBERMILL ){
            spt = world->getSpriteForIcon(UNIT_LUMBERMILL);
        }else if(i == BRUSH_WATCHER_TOWER ){
            spt = world->getSpriteForIcon(UNIT_WATCHERTOWER);
        }else if(i == BRUSH_FACTORY ){
            spt = world->getSpriteForIcon(UNIT_FACTORY);
        }else if(i == BRUSH_AIRPORT ){
            spt = world->getSpriteForIcon(UNIT_AIRPORT);
        }else if(i == BRUSH_ORC_AXE ){
            spt = world->getSpriteForIcon(UNIT_ORC_AXE);
        }else if(i == BRUSH_ORC_SPEAR ){
            spt = world->getSpriteForIcon(UNIT_ORC_SPEAR);
        }else if(i == BRUSH_TROLL ){
            spt = world->getSpriteForIcon(UNIT_TROLL);
        }else if(i == BRUSH_GOLBIN ){
            spt = world->getSpriteForIcon(UNIT_GOBLIN);
        }else if(i == BRUSH_GOLBIN_BOMB ){
            spt = world->getSpriteForIcon(UNIT_GOBLIN_BOMB);
        }else if(i == BRUSH_BUNKER ){
            spt = world->getSpriteForIcon(UNIT_ORC_BUNKER);
        }else if(i == BRUSH_HQ ){
            spt = world->getSpriteForIcon(UNIT_ORC_HQ);
        }else if(i == BRUSH_ZOMBIE_SWORDSMAN ){
            spt = world->getSpriteForIcon(UNIT_ZOMBIE_SWORDSMAN);
        }else if(i == BRUSH_ZOMBIE_ORC_AXE ){
            spt = world->getSpriteForIcon(UNIT_ZOMBIE_ORC_AXE);
        }else if(i == BRUSH_START_POINT ){
            spt = Sprite::create("startPoint.png");
        }else if(i == BRUSH_EVENT_POINT ){
            spt = Sprite::create("eventPoint.png");
        }else{
            log("because its non");
        }
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(EditorHud::onBrushClick, this));
        btn->addChild(spt);
        spt->setPosition(btn->getContentSize()/2);
        brushMenuArray.pushBack(btn);
        svBrushes->addChild(btn);
    }
    onBrushClick(brushMenuArray.at(0));
    
    for(int i = 0; i < 3; i++){
        btn = Button::create("uiBox.png", "uiBox.png");
        btn->setPosition(Vec2(menuGap, size.height - (menuWidth + menuGap)*i - menuGap));
        btn->setAnchorPoint(Vec2(0, 1));
        btn->setScale(menuWidth/btn->getContentSize().width);
        if(i == 0){
            addSpriteToButton("pencil.png", btn, Button::TextureResType::LOCAL);
        }else if(i == 1){
            addSpriteToButton("eraser.png", btn, Button::TextureResType::LOCAL);
        }else if(i == 2){
            addSpriteToButton("handIcon.png", btn, Button::TextureResType::LOCAL);
        }
        btn->addClickEventListener(CC_CALLBACK_1(EditorHud::onLeftMenuClick, this));
        btn->setPressedActionEnabled(true);
        this->addChild(btn);
        btn->setTag(i);
        menuArray.pushBack(btn);
    }
    onLeftMenuClick(menuArray.at(0));
    
    onLoadClick(0);
    
    log("init done");
    // init done
    return true;
}
void EditorHud::onLeftMenuClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    for (auto button: menuArray) {
        if(button == btn){
            button->setColor(Color3B::GREEN);
        }else{
            button->setColor(Color3B::WHITE);
        }
    }
    svBrushes->stopAllActions();
    if(btn->getTag() == 0){
        svBrushes->runAction(EaseInOut::create(MoveTo::create(0.3f, Vec2(size.width - menuWidth, 0)), 2));
        selectedMode = MODE_PENCIL;
    }else{
        svBrushes->runAction(EaseInOut::create(MoveTo::create(0.3f, Vec2(size.width, 0)), 2));
        if(btn->getTag() == 1){
            selectedMode = MODE_ERASER;
            
        }else if(btn->getTag() == 2){
            selectedMode = MODE_SELECT;
            
        }
    }
}
void EditorHud::onBrushClick(Ref* ref){
    Button* btn = (Button*)ref;
    log("btn tag: %d", btn->getTag());
    selectedBrush = btn->getTag();
    for (int i = 0; i < brushCount; i++) {
        btn = brushMenuArray.at(i);
        if (i + BRUSH_TREE == selectedBrush) {
            btn->setColor(Color3B::GREEN);
        }else{
            btn->setColor(Color3B::WHITE);
        }
    }
}
void EditorHud::onMenuClick(){
    //    GM->playSoundEffect(SOUND_PAPER_FLIP);
    GM->playSoundEffect(SOUND_WOOD_HIT);
    world->pauseLayer();
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
    btnBlock->addClickEventListener(CC_CALLBACK_0(EditorHud::onResumeClick, this));
    
    ImageView* img = ImageView::create("uiBox.png");
    layer->addChild(img, 99);
    img->setContentSize(cocos2d::Size(700, 500));
    img->setScale9Enabled(true);
    img->setPosition(size/2);
    
    Button* btn = Button::create("uiBoxSmall.png");
    layer->addChild(btn, 99);
    btn->setContentSize(cocos2d::Size(520, 150));
    btn->setScale9Enabled(true);
    btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onSaveClick, this));
    btn->setPosition(Vec2(size.width/2, size.height/2 + 240));
    addLabelToButton("RESUME", btn, false, DARK_GRAY_3B);
    
    tfMapName = TextField::create("Enter Map Name Here", "Arial", 44);
    layer->addChild(tfMapName);
    tfMapName->setPosition(Vec2(size.width/2 - 500, size.height/2 + 240));
    
    btn = Button::create("uiBoxSmall.png");
    layer->addChild(btn, 99);
    btn->setContentSize(cocos2d::Size(520, 150));
    btn->setScale9Enabled(true);
    btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onResumeClick, this));
    btn->setPosition(Vec2(size.width/2, size.height/2 + 80));
    addLabelToButton("RESUME", btn, false, DARK_GRAY_3B);
    
    btn = Button::create("uiBoxSmall.png");
    layer->addChild(btn, 99);
    btn->setTitleColor(Color3B::BLACK);
    btn->setContentSize(cocos2d::Size(520, 150));
    btn->setScale9Enabled(true);
    btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onExitClick, this));
    btn->setPosition(Vec2(size.width/2, size.height/2 - 80));
    addLabelToButton("EXIT", btn, false, DARK_GRAY_3B);
    
}
void EditorHud::onSaveClick(){
    std::string str = tfMapName->getString();
    log("map name: %s", str.c_str());
    if(str.size() <= 0){
        showInstanceMessage("Enter MAP NAME before SAVE");
        return;
    }
    
    int index = UDGetInt(KEY_NEXT_MAP_CREATING_INDEX, 0);
    
    std::string mapData = world->getMapData();
    UDSetStr(strmake(KEY_MAP_DATA_SAVED_FORMAT, index).c_str(), mapData.c_str());
    log("saved %d: %s", index, mapData.c_str());
    std::string loadedData = UDGetStr(strmake(KEY_MAP_DATA_SAVED_FORMAT, index).c_str());
    world->setMapData(mapData);
    
//    HttpsManager::getInstance()->echoPost(mapData);
//    HttpsManager::getInstance()->echoPost(mapData);
    
//    std::string ssmapData = UDGetStr(strmake(KEY_MAP_DATA_SAVED_FORMAT, index).c_str());
    
//    log("ssloaded map data: %s", ssmapData.c_str());

    onLoadClick(index);
    index++;
    UDSetInt(KEY_NEXT_MAP_CREATING_INDEX, index);
    
    showInstanceMessage("Map saved");
}

void EditorHud::onLoadClick(int index){
    std::string mapData = UDGetStr(strmake(KEY_MAP_DATA_SAVED_FORMAT, index).c_str());
    
    log("loaded map data: %s", mapData.c_str());
}

void EditorHud::onResumeClick(){
    closePopup();
    world->resumeLayer();
}
void EditorHud::onExitClick(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    removeListener();
    auto scene = Scene::create();
    scene->addChild(Title::create());
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void EditorHud::talkBoxUpdate(float dt){
    if(tutorialEnded){
        this->unschedule(schedule_selector(EditorHud::talkBoxUpdate));
        tutorialNode->removeFromParent();
        tutorialNode = nullptr;
        return;
    }
    if (talkIndex == 0) {
        tutorialNode->setVisible(true);
        if(tutorialHowTo != nullptr){
            tutorialHowTo->setVisible(false);
            this->unschedule(schedule_selector(EditorHud::updateTutorialHowTo));
        }
    }
    Label* lbl = (Label*)tutorialNode->getChildByName("lbl");
    if(talkIndex > talkText.size()){
        lbl->setString(talkText);
        return;
    }
    lbl->setString(talkText.substr(0, talkIndex));
    talkIndex++;
}
void EditorHud::showPriceInfo(std::string msg, int gold, int lumber, int food){
    priceInfo->stopAllActions();
    priceInfo->setVisible(true);
    priceInfo->setScale(0.8f);
    priceInfo->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1)));
    priceInfo->runAction(Sequence::create(DelayTime::create(3), CallFunc::create(CC_CALLBACK_0(EditorHud::hidePriceInfo, this)), NULL));
    
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
void EditorHud::hidePriceInfo(){
    priceInfo->setVisible(false);
}
Label* EditorHud::getLabel(std::string txt, int fontSize){
    Label* lbl = Label::createWithSystemFont(txt, "Thonburi", fontSize);
    lbl->enableShadow();
    lbl->setHorizontalAlignment(TextHAlignment::CENTER);
    lbl->setVerticalAlignment(TextVAlignment::CENTER);
    //    lbl->setTextColor(Color4B::RED);
    return lbl;
}
void EditorHud::setMenu(int index, int btnType){
    return;
    Button* btn = (Button*)world->getChildByName(strmake("btnMenu%d", index));
    btn->setTag(btnType);
    btn->setVisible(btnType != BTN_TYPE_NONE);
    btn->removeChildByName("icon");
    Sprite* sptBuilding = nullptr;
    //    btn->getChildByName("lblGold")->setVisible(btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL);
    //    btn->getChildByName("lblLumber")->setVisible(btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL);
    int unitIndex = -1;
    btn->loadTextures("uiBox.png", "uiBox.png");
    PPLabel* lbl = (PPLabel*)btn->getChildByName("lbl");
    lbl->setString("");
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
        lbl->setString("Worker");
    }
    else if(btnType ==  BTN_TYPE_SWORDMAN ){
        unitIndex = UNIT_SWORDMAN;
        lbl->setString("Swordsman");
    }
    else if(btnType ==  BTN_TYPE_ARCHER  ){
        unitIndex = UNIT_ARCHER;
        lbl->setString("Archer");
    }
    else if(btnType ==  BTN_TYPE_HELICOPTER  ){
        unitIndex = UNIT_HELICOPTER;
        lbl->setString("Helicopter");
    }
    else if(btnType ==  BTN_TYPE_CATAPULT  ){
        unitIndex = UNIT_CATAPULT;
        lbl->setString("Catapult");
    }
    else if(btnType ==  BTN_TYPE_CASTLE  ){
        unitIndex = UNIT_CASTLE;
        lbl->setString("Castle");
    }
    else if(btnType ==  BTN_TYPE_FARM  ){
        unitIndex = UNIT_FARM;
        lbl->setString("Farm");
    }
    else if(btnType ==  BTN_TYPE_BARRACKS  ){
        unitIndex = UNIT_BARRACKS;
        lbl->setString("Barracks");
    }
    else if(btnType ==  BTN_TYPE_LUMBER  ){
        unitIndex = UNIT_LUMBERMILL;
        lbl->setString("Lumbermill");
    }
    else if(btnType ==  BTN_TYPE_WATCHER_TOWER  ){
        unitIndex = UNIT_WATCHERTOWER;
        lbl->setString("Watcher Tower");
    }
    else if(btnType ==  BTN_TYPE_FACTORY  ){
        unitIndex = UNIT_FACTORY;
        lbl->setString("Factory");
    }
    else if(btnType ==  BTN_TYPE_AIRPORT  ){
        unitIndex = UNIT_AIRPORT;
        lbl->setString("Airport");
    }
    lbl->setVisible(lbl->text.length() > 0);
    if(btnType >= BTN_TYPE_WORKER && btnType < BTN_TYPE_CANCEL){
        //        LM->setLocalizedString((Label*)btn->getChildByName("lblName"), world->getUnitName(unitIndex));
        //        ((Label*)btn->getChildByName("lblGold"))->setString(strmake("G:%s", Value(world->getGoldPriceForUnit(unitIndex)).asString().c_str()));
        //        ((Label*)btn->getChildByName("lblLumber"))->setString(strmake("L:%s", Value(world->getLumberPriceForUnit(unitIndex)).asString().c_str()));
        sptBuilding = world->getSpriteForIcon(unitIndex);
    }
    if(sptBuilding != nullptr){
        btn->addChild(sptBuilding);
        sptBuilding->setName("icon");
        sptBuilding->setPosition(btn->getContentSize()/2);
    }
}
void EditorHud::onCommandClick(Ref* ref){
    if(world->buildingTemplete != nullptr){
        world->buildingTemplete->removeFromParent();
        world->buildingTemplete = nullptr;
    }
    Button* btn = (Button*)ref;
    std::string name = btn->getName();
    int index = Value(name.substr(7)).asInt();
    index = btn->getTag();
    bool shouldHideMenu = false;
    if(index == BTN_TYPE_MOVE){
        world->onMoveClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_STOP){
        world->onStopClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_ATTACK){
        world->onAttackClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_GATHER){
        world->onGatherClick();
        shouldHideMenu = true;
    }else if(index == BTN_TYPE_BUILD){
        world->onBuildingClick();
        if (world->stageIndex == 0 && HUD->tutorialIndex == 5) {
            HUD->tutorialIndex++;
            HUD->talkIndex = 0;
            HUD->talkText = LM->getText("tutorial 6");
            Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
            spt->stopAllActions();
            spt->setSpriteFrame("farm.png");
            world->addGold(400);
            world->addLumber(200);
        }
    }else if(index == BTN_TYPE_BUILD_BETTER){
        world->onBuildingBetterClick();
    }else if(index == BTN_TYPE_FARM){
        if(world->tryBuilding(UNIT_FARM)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_CASTLE){
        if(world->tryBuilding(UNIT_CASTLE)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_BARRACKS){
        if(world->tryBuilding(UNIT_BARRACKS)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_WATCHER_TOWER){
        if(world->tryBuilding(UNIT_WATCHERTOWER)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_LUMBER){
        if(world->tryBuilding(UNIT_LUMBERMILL)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_AIRPORT){
        if(world->tryBuilding(UNIT_AIRPORT)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_FACTORY){
        if(world->tryBuilding(UNIT_FACTORY)){
            shouldHideMenu = true;
        }
    }else if(index == BTN_TYPE_CANCEL){
        setMenu(0, BTN_TYPE_BUILD);
        setMenu(1, BTN_TYPE_BUILD_BETTER);
        setMenu(2, BTN_TYPE_NONE);
        setMenu(3, BTN_TYPE_NONE);
        setMenu(4, BTN_TYPE_NONE);
        setMenu(5, BTN_TYPE_NONE);
        world->selectedCommand = COMMAND_NOTHING;
    }else if(index == BTN_TYPE_WORKER){
        world->tryCreateUnit(UNIT_WORKER);
    }else if(index == BTN_TYPE_SWORDMAN){
        world->tryCreateUnit(UNIT_SWORDMAN);
    }else if(index == BTN_TYPE_ARCHER){
        world->tryCreateUnit(UNIT_ARCHER);
    }else if(index == BTN_TYPE_HELICOPTER){
        world->tryCreateUnit(UNIT_HELICOPTER);
    }else if(index == BTN_TYPE_CATAPULT){
        world->tryCreateUnit(UNIT_CATAPULT);
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
void EditorHud::updateQuestInfo(){
    return;
}
void EditorHud::setExp(int level, int exp, int expMax){
    LoadingBar* pb = (LoadingBar*)this->getChildByName("pbExp");
    float percent = exp*100.0f/expMax;
    pb->setPercent(percent);
    
    Text* lbl = (Text*)this->getChildByName("lblLevel");
    lbl->setString(Value(level).asString());
}
void EditorHud::setEnergy(int hp, int max){
    LoadingBar* pbHP = (LoadingBar*)this->getChildByName("pbHP");
    float percent = hp*100.0f/max;
    pbHP->setPercent(percent);
    pbHP->stopAllActions();
    pbHP->setOpacity(255);
    if(percent < 30){
        pbHP->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    
    Text* lblHP = (Text*)this->getChildByName("lblHP");
    lblHP->setString(strmake("%d/%d", hp, max));
}
void EditorHud::initQuest(int index, std::string itemName, int count, int max){
    Text* lbl = (Text*)this->getChildByName(strmake("lblQuest%d", index));
    lbl->setString(strmake("%d/%d", count, max));
    lbl->setTag(0);
    lbl->setVisible(max > 0);
    Button* btn = (Button*)this->getChildByName(strmake("questIcon%d", index));
    btn->setTag(max);
    btn->setVisible(max > 0);
    lbl->setPositionX(btn->getPositionX());// btn->getContentSize().width/2);
    if(max <= 0){
        return;
    }
    ImageView* sptIcon = (ImageView*)btn->getChildByTag(0);
    sptIcon->loadTexture(world->getItemSpriteName(ITEM_TYPE_ETC, itemName), ImageView::TextureResType::PLIST);
    sptIcon->setName(itemName);
    
    if(index == 0){
        questItemName0 = itemName;
    }else if(index == 1){
        questItemName1 = itemName;
    }else if(index == 2){
        questItemName2 = itemName;
    }
}
void EditorHud::checkQuestForCollectingItem(std::string name){
    for(int i = 0;i < 3; i++){
        Node* btn = this->getChildByName(strmake("questIcon%d", i));
        ImageView* sptIcon = (ImageView*)btn->getChildByTag(0);
        if(sptIcon->getName().compare(name) == 0){
            Text* lbl = (Text*)this->getChildByName(strmake("lblQuest%d", i));
            int count = lbl->getTag();
            count++;
            int max = btn->getTag();
            lbl->setString((strmake("%d/%d", count, max)));
        }
    }
}
void EditorHud::showInfo(std::string info){
    lblInfo->setVisible(true);
    lblInfo->setString(strmake("[%s]", info.c_str()));
}
void EditorHud::hideInfo(){
    lblInfo->setVisible(false);
}
void EditorHud::showMap(){
    
}
void EditorHud::onSelectClick(){
    if(isInScene) return;
    if(GM->currentStageIndex == STAGE_TITLE){
        return;
    }
    if(!world->isInEvent && !inputShouldWait && inventoryLayer == nullptr && menuLayer == nullptr && GM->currentStageIndex == STAGE_FIELD && firstPopup == nullptr){
        showInventory();
    }else if(inventoryLayer != nullptr){
        
    }
}
void EditorHud::showMenus(){
    
}
void EditorHud::onResetClick(){
    showABDialog(LM->getText("reset ask"), LM->getText("yes"), LM->getText("no"), DIALOG_TYPE_AB_RESET);
    
}
void EditorHud::processReset(){
    world->setHeroLevel(0, 0);
    world->setHeroExp(0, 0);
    inventory->reset();
    world->setHeroWeapon(0, "sword");
    world->setHeroHelmet(0, "");
    world->setHeroShoes(0, "");
    world->setHeroShield(0, "");
    UD->setStringForKey(KEY_LAST_SAVE_POINT, "0_4_2");
//    UD->setIntegerForKey(KEY_NEXT_SPAWN_DIRECTION, DIRECTION_E);
    UD->setIntegerForKey(KEY_LAST_SAVE_HEALTH, 60);
//    UDSetInt(KEY_CURRENT_QUEST_ID, -1);
    
    UDSetStr(KEY_ALCHY_COMBINING_ITEM_NAME, "");
    UDSetStr(KEY_ALCHY_COMBINE_START_TIME, "");
    world->unscheduleAll();
    world->pauseLayer();
    HUD->removeListener();
    
    for (int mapCount = 0; mapCount < 10; mapCount++) {
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                UDSetBool(strmake(KEY_POT_BROKEN_FORMAT, mapCount, x, y).c_str(), false);
            }
        }
    }
    UDSetInt(KEY_POT_SOUL_COUNT, 0);
    
    auto scene = EditorWorld::scene(STAGE_FIELD, false);
    Director::getInstance()->replaceScene(scene);
}
void EditorHud::showQuest(){
//    selectMenu(menuLayer->getChildByName("btnMenu0"));
////    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
////    if(questID < 0){
////        showInstanceMessage(LM->getText("no quest"));
////        return;
////    }
//    Node* layer = CSLoader::createNode("Quest.csb");
//    this->addChild(layer, 5);
//    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
//    setAsPopup(layer);
//    questLayer = layer;
//    addBToCloseToLayer(layer)->setColor(Color3B(78, 78, 78));
//    
//    Node* imgBack = layer->getChildByName("imgBack");
//    ropeAround(imgBack);
//    
//    ImageView* imgTitle = (ImageView*)layer->getChildByName("imgTitle");
//    imgTitle->runAction(EaseOut::create(ResizeTo::create(0.3f, cocos2d::Size(92, 21)), 2));
//    
//    Label* lblTitle = LM->getLocalizedLabel(LM->getText("quest").c_str(), Color4B(78, 78, 78, 255));
//    lblTitle->setOpacity(0);
//    imgTitle->addChild(lblTitle);
//    lblTitle->setPosition(Vec2(46, 10));
//    lblTitle->setScale(0.12f);
//    lblTitle->runAction(Sequence::create(DelayTime::create(0.3f), FadeIn::create(0.08f), NULL));
//    
//    Label* lbl = changeTextWithLabel((Text*)layer->getChildByName("lblClient"));
//    lbl->setTextColor(Color4B::WHITE);
//    LM->setLocalizedString(lbl, "client");
//    
//    Button* btn = (Button*)layer->getChildByName("btnClient");
//    std::string file = world->npcTable[Value(questID).asString()].asValueMap().at("file").asString();
//    addSpriteToButton(file, btn, Button::TextureResType::PLIST);
//    
//    lbl = changeTextWithLabel((Text*)layer->getChildByName("lblClientName"));
//    lbl->setTextColor(Color4B::WHITE);
//    LM->setLocalizedString(lbl, world->npcTable[Value(questID).asString()].asValueMap().at("npc name").asString());
//    
//    lbl = changeTextWithLabel((Text*)layer->getChildByName("lblItems"));
//    lbl->setTextColor(Color4B::WHITE);
//    LM->setLocalizedString(lbl, "client request");
//    
//    for(int i = 0; i < 3; i++){
//        std::string itemName = world->npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d", i)).asString();
//        int itemCount = world->npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d count", i)).asInt();
//        int itemHaveCount = inventory->getTotalItemCount(itemName);
//        
//        btn = (Button*)layer->getChildByName(strmake("btnItem%d", i));
//        
//        lbl = changeTextWithLabel((Text*)layer->getChildByName(strmake("lblItem%d", i)));
//        lbl->setTextColor(Color4B::WHITE);
//        lbl->setHorizontalAlignment(TextHAlignment::CENTER);
//        
//        if(world->getItemType(itemName) == ITEM_TYPE_ETC){
//            if(itemCount > 0){
//                std::string file = world->itemStatTable[itemName].asValueMap().at("sprite").asString();
//                addSpriteToButton(file, btn, Button::TextureResType::PLIST);
//                
//                lbl->setString(strmake("%s\n%d/%d", LM->getText(itemName.c_str()).c_str(), itemHaveCount, itemCount));
//            }else{
//                btn->setColor(Color3B::BLACK);
//                lbl->setString("N/A");
//            }
//        }else if(world->unitStatTable.find(itemName) != world->unitStatTable.end()){
//            if(itemCount > 0){
//                EnemyBase* unit = EnemyBase::createEnemy(UNIT_NPC, 0, 0, "slime0.png");
////                unit->movingDirection = DIRECTION_S;
////                unit->runMoveAnimation(DIRECTION_STAY);
//                unit->setName(itemName);
//                btn->addChild(unit);
//                unit->setPosition(btn->getContentSize()/2);
//                
//                lbl->setString(strmake("%s\n%d/%d", LM->getText(itemName.c_str()).c_str(), UDGetInt(itemName.c_str(), 0), itemCount));
//            }else{
//                btn->setColor(Color3B::BLACK);
//                lbl->setString("N/A");
//            }
//            
//        }
//    }
//    
}
void EditorHud::addLabelToButton(std::string text, Button* btn, bool bordered, Color3B color){
    Node* lbl = PPLabel::create(text, btn->getContentSize().height*2/6, color, false, bordered, TextHAlignment::CENTER, false);
    lbl->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2));
    btn->addChild(lbl);
}
Sprite* EditorHud::addSpriteToButton(std::string imgFileName, Button* btn, Button::TextureResType type){
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
Sprite* EditorHud::addSpriteToButton(std::string itemName, Button* btn){
    Sprite* spt = getIconSprite(itemName);
    spt->setName("sprite");
    btn->addChild(spt);
    spt->setPosition(btn->getContentSize()/2);
    return spt;
}
Sprite* EditorHud::getIconSprite(std::string itemName){
    int itemType = world->getItemType(itemName);
    Sprite* spt = Sprite::createWithSpriteFrameName(world->getItemStat(itemName, "sprite"));
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
void EditorHud::onStartClick(){
    if(isInScene) return;
    
    if(GM->currentStageIndex == STAGE_TITLE){
        if(!world->isInEvent){
            Node* lbl = this->getChildByName("lblPressStart");
            lbl->stopAllActions();
            lbl->setOpacity(255);
            GM->nextScene = STAGE_FIELD;
            lbl->runAction(Sequence::create(Blink::create(1, 4), CallFunc::create(CC_CALLBACK_0(EditorHud::showBIAndReplaceScene, this)), nullptr));
            world->isInEvent = true;
        }
        
        return;
    }
    if(mapLayer != nullptr){
        return;
    }
    if(firstPopup == nullptr && !inputShouldWait && menuLayer == nullptr && GM->currentStageIndex == STAGE_FIELD && firstPopup == nullptr){
        showMenus();
    }
}

void EditorHud::showBtns(){
    for (auto btn: menus){
        btn->runAction(MoveBy::create(0.1f, Vec2(0, 1300)));
    }
}
void EditorHud::hideBtns(){
    for (auto btn: menus){
        btn->runAction(MoveBy::create(0.1f, Vec2(0, -1300)));
    }
    //    joystickLR->runAction(MoveBy::create(0.1f, Vec2(0, -300)));
}
void EditorHud::toggleAuto(bool showMsg){
    bool isAuto = !world->isAutoTargetingOn;
    world->isAutoTargetingOn = isAuto;
    
    //    Sprite* spt = Sprite::create(isAuto?"UI/btnToggleManual.png":"UI/btnToggleAuto.png");
    //    btnToggle->setSpriteFrame(spt->getSpriteFrame());
    UserDefault::getInstance()->setBoolForKey(KEY_AUTO_TARGET_SELECTED, isAuto);
    if (showMsg) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText(isAuto?"auto targeting":"manual targeting").c_str(), this);
    }
}

void EditorHud::registerControllerListener()
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
        _listener->onConnected = CC_CALLBACK_2(EditorHud::onConnectController,this);
        
        //bind disconnect event call function
        _listener->onDisconnected = CC_CALLBACK_2(EditorHud::onDisconnectedController,this);
        
        //bind onKeyDown event call function
        _listener->onKeyDown = CC_CALLBACK_3(EditorHud::onKeyDown, this);
        
        //bind onKeyUp event call function
        _listener->onKeyUp = CC_CALLBACK_3(EditorHud::onKeyUp, this);
        
        //bind onAxis event call function, onAxis will be called when analog stick is changed
        _listener->onAxisEvent = CC_CALLBACK_3(EditorHud::onAxisEvent, this);
        
        //Activate the listener into the event dispatcher
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, this);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //This function should be called for iOS platform
        Controller::startDiscoveryController();
#endif
        
    }
}

//Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
void EditorHud::onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    CCLOG("KeyDown:%d", keyCode);
}

void EditorHud::onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event)
{
    //You can get the controller by tag, deviceId or devicename if there are multiple controllers
    CCLOG("tag:%d DeviceId:%d DeviceName:%s", controller->getTag(), controller->getDeviceId(), controller->getDeviceName().c_str());
    CCLOG("KeyUp:%d", keyCode);
}

//The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
void EditorHud::onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event)
{
    //    const auto& keyStatus = controller->getKeyStatus(keyCode);
    //    CCLOG("Axis KeyCode:%d Axis Value:%f", keyCode, keyStatus.value);
}

void EditorHud::onConnectController(Controller* controller, Event* event)
{
    CCLOG("Game controller connected");
}

void EditorHud::onDisconnectedController(Controller* controller, Event* event)
{
    CCLOG("Game controller disconnected");
}
void EditorHud::addHeartAnimated(Vec2 pos){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    Sprite* spt = Sprite::create("heart.png");
    spt->setPosition(pos);
    this->addChild(spt);
    spt->runAction(Sequence::create(EaseIn::create(MoveTo::create(1, heart->getPosition() + Vec2(50, 0)), 3), CallFuncN::create(CC_CALLBACK_1(EditorHud::addHeartDone, this)), CallFuncN::create(CC_CALLBACK_1(EditorHud::spriteMoveDone, this)), NULL));
}
void EditorHud::addHeartDone(Ref* obj){
    Sprite* heart = lifeArray.at(lifeArray.size() - 1);
    
    Sprite* spt = Sprite::create("backLight.png");
    spt->setScale(0.5);
    spt->runAction(Sequence::create(RotateBy::create(1, 300), CallFuncN::create(CC_CALLBACK_1(EditorHud::spriteMoveDone, this)), NULL));
    
    heart->addChild(spt);
    spt->setPosition(Vec2(heart->getContentSize().width/2, heart->getContentSize().height/2));
    
    
}
// Implementation of the keyboard event callback function prototype
void EditorHud::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || world->indexToWarp != -9999) {
        return;
    }
    if (dialogBox != nullptr) {
        dialogBox->showNextTalk();
        return;
    }
    //    CCLOG("Key with keycode %d pressed", keyCode);
    
    if(keyCode == EventKeyboard::KeyCode::KEY_A){
        world->onAttackClick();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_S){
        world->onStopClick();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_M){
        world->onMoveClick();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_B){
        world->onBuildingClick();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_V){
        world->onBuildingBetterClick();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_U){
        //        world->gameTimer += 60;// test
        //        for(auto unit: world->enemyArray){
        //            unit->timeCollapse += 60;
        //        }
    }else if(keyCode == EventKeyboard::KeyCode::KEY_I){
        //        world->gameTimer += 6;// test
        //        for(auto unit: world->enemyArray){
        //            unit->timeCollapse += 6;
        //        }
    }else if(keyCode == EventKeyboard::KeyCode::KEY_C ||
             keyCode == EventKeyboard::KeyCode::KEY_Y){
        GameManager::getInstance()->cPressed = true;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP){
        world->isMapMoveUp = true;
    }else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
              keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN ){
        world->isMapMoveDown = true;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT ){
        world->isMapMoveLeft = true;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT ){
        world->isMapMoveRight = true;
    }else if(keyCode >= EventKeyboard::KeyCode::KEY_1 &&
             keyCode <= EventKeyboard::KeyCode::KEY_9){
        world->changeWeapon((int)keyCode - (int)EventKeyboard::KeyCode::KEY_1);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        world->showBigMiniMap(true);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ENTER){
        tryResultOK();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
        
        removeListener();
    }
}
void EditorHud::tryResultOK()
{
    
}
void EditorHud::showTyping(){
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
    
    this->schedule(schedule_selector(EditorHud::typingCursorBlinking), 1);
}
void EditorHud::typing(std::string str){
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
void EditorHud::typingCursorBlinking(float dt){
    if (lblTyping->getString().size() != originalString.size()) {
        lblTyping->setString(originalString);
    }else{
        lblTyping->setString(originalString + "_");
    }
}
void EditorHud::showStageTitle(){
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
    int stageIndex = world->indexToWarp;//GameManager::getInstance()->currentStageIndex;
    if (stageIndex == -1) {
        sprintf(bufStage, "%s", LanguageManager::getInstance()->getText("stageLobby").c_str());
    }else{
        if (world->stageIndex >= 0 && !world->isBossMap) {
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
        if (world->stageIndex >= 0 && !world->isBossMap) {
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
    sptWhiteGun->runAction(Sequence::create(RotateBy::create(dur, 360), CallFunc::create(CC_CALLBACK_0(EditorHud::whiteGunFired, this)), EaseOut::create(RotateBy::create(0.1, -30), 2), RotateBy::create(0.4, 30), DelayTime::create(2.0f), FadeOut::create(1.0f), NULL));
    
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
void EditorHud::stageTitleLineUpdate(float dt){
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
        this->unschedule(schedule_selector(EditorHud::stageTitleLineUpdate));
    }
}
void EditorHud::whiteGunFired(){
    this->schedule(schedule_selector(EditorHud::stageTitleLineUpdate));
}
void EditorHud::showDialog(std::string str){
    if (dialogBox == nullptr) {
        dialogBox = AwesomeDialogBox::create();
        this->addChild(dialogBox);
    }
    CallFunc* call = CallFunc::create(CC_CALLBACK_0(EditorHud::onDialogDone, this));
    call->retain();
    dialogBox->ShowDialog(str.c_str(), this, call);
    hideBtns();
}
void EditorHud::onDialogDone(){
    if(dialogBox != nullptr){
        dialogBox->removeFromParentAndCleanup(true);
        dialogBox = nullptr;
    }
    if(UserDefault::getInstance()->getBoolForKey(__String::createWithFormat(KEY_DOES_IT_TALKED_FORMAT, "story4")->getCString(), false)){
        //        showEnding();
    }
}


void EditorHud::onEndCredit(){
    removeListener();
    
    
    //    scene->addChild(EditorWorld::scene(STAGE_LOBBY, false));
    
}
void EditorHud::showNextTalk(){
    dialogBox->showNextTalk();
}
void EditorHud::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (isInScene || world->indexToWarp != -9999) {
        return;
    }
    if(keyCode == EventKeyboard::KeyCode::KEY_Z ||
       keyCode == EventKeyboard::KeyCode::KEY_J ||
       keyCode == EventKeyboard::KeyCode::KEY_DPAD_CENTER){
//        GameManager::getInstance()->buttonUp(BUTTON_A);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_E){
        world->reload();
    }else if(keyCode == EventKeyboard::KeyCode::KEY_K || keyCode == EventKeyboard::KeyCode::KEY_X){
//        GameManager::getInstance()->buttonUp(BUTTON_B);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_C){
        GameManager::getInstance()->cPressed = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP){
        world->isMapMoveUp = false;
    }else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
              keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN ){
        world->isMapMoveDown = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT ){
        world->isMapMoveLeft = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT ){
        world->isMapMoveRight = false;
    }else if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        resumeGame(NULL, ui::Widget::TouchEventType::ENDED);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_TAB){
        world->showBigMiniMap(false);
    }else if(keyCode == EventKeyboard::KeyCode::KEY_Y){
        onDialogDone();
    }
    //    CCLOG("Key with keycode %d released", keyCode);
}
void EditorHud::useBomb(){
    if (GameManager::getInstance()->developer) {
        world->useBomb();
        return;
    }
    int bombCount = GameManager::getInstance()->getBombCount();
    if (bombCount > 0) {
        world->useBomb();
        bombCount--;
        GameManager::getInstance()->setBombCount(bombCount);
    }else{
        suggestBombShop();
    }
}
void EditorHud::suggestBombShop(){
    // show Bomb layer
    CCLOG("show bomb layer");
}
void EditorHud::buyBomb(){
    
}

void EditorHud::usePotion(){
    if (GameManager::getInstance()->developer) {
        world->usePotion();
        return;
    }
    int potionCount = GameManager::getInstance()->getPotionCount();
    if (potionCount > 0) {
        world->usePotion();
        potionCount--;
        GameManager::getInstance()->setPotionCount(potionCount);
    }else{
        suggestPotionShop();
    }
}
void EditorHud::suggestPotionShop(){
    // show Potion layer
    CCLOG("show potion layer");
}
void EditorHud::buyPotion(){
    
}
void EditorHud::buyGems(){
    
}
void EditorHud::keyDown(int key) {
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

void EditorHud::keyUp(int key) {
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
void EditorHud::showTalk(const char* talk){
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
void EditorHud::hideTalk(){
    talkLabel->runAction(FadeOut::create(0.5));
    talkLabel->runAction(MoveBy::create(0.5, Vec2(0, 40)));
}
void EditorHud::showCoinShopLayer(){
    
    //    ((ShopLayer*)GameManager::getInstance()->getShopLayer())->openShop();
}

void EditorHud::showAchievementComplete(const char* text){
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
    
    achieveNode->runAction(Sequence::create(EaseIn::create(MoveBy::create(0.5f, Vec2(0, -achieveNode->getContentSize().height)),0.2f), CallFuncN::create(CC_CALLBACK_1(EditorHud::achievementEffect, this)), DelayTime::create(1), EaseIn::create(MoveBy::create(0.5f, Vec2(0, achieveNode->getContentSize().height)),4), CallFuncN::create(CC_CALLBACK_1(EditorHud::nodeMoveDone, this)), NULL));
}

void EditorHud::achievementEffect(Node* node){
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
void EditorHud::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Node* node = (Node*)obj;
    node->stopAllActions();
    node->removeFromParentAndCleanup(true);
}
void EditorHud::setButtonSize(float siz) // 0 ~ 1
{
}

void EditorHud::updateCoin(int howMuch)
{
}

void EditorHud::enableJoystick(bool enable)
{
    for(auto btn: buttonArray)
    {
        btn->enableButton(enable);
    }
}
void EditorHud::addEnergyEffect(){
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
        spt->runAction(Sequence::create(DelayTime::create(i*interval), EaseIn::create(MoveTo::create(dur, Vec2(x, y)), 3), CallFuncN::create(CC_CALLBACK_1(EditorHud::spriteMoveDone, this)), NULL));
    }
    this->scheduleOnce(schedule_selector(EditorHud::addEnergyDone), dur + interval*effectCount);
}
void EditorHud::addEnergyDone(float dt){
    
    float x = 10 + 47*(lifeArray.size() + 1);
    float y = size.height - 28;
    Sprite* spt = Sprite::create("hudHeart.png");
    this->addChild(spt);
    spt->setPosition(Vec2(x, y));
    float dur =0.5;
    spt->runAction(ScaleTo::create(dur, 3));
    spt->runAction(Sequence::create(FadeOut::create(dur), CallFuncN::create(CC_CALLBACK_1(EditorHud::spriteMoveDone, this)), NULL));
    
}
void EditorHud::initializeSreen(int lifeCount)
{
    
    //    btnA->setVisible(false);
    //    btnB->setVisible(false);
    //    btnLeftRight->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //    recordItem->setVisible(true);w
#endif
    
    //    recordItem->setPosition(pauseItem->getPosition() + Vec2(-pauseItem->getContentSize().width - 15, 0));
}

void EditorHud::reduceLife()
{
    Sprite* sprite = (Sprite*)lifeArray.at(lifeArray.size()-1);
    lifeArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}

void EditorHud::setWeapon(int weapon){
}
void EditorHud::setTotalBulletCount(int count, int maxCount){
    //    Node* checkBoard = this->getChildByName("sptCheckBoard");
    //    TextBMFont* lblCount = (TextBMFont*)checkBoard->getChildByName("lblBulletLeft");
    //    if (maxCount < 0) {
    //        lblCount->setString("00");
    //    }else{
    //        lblCount->setString(__String::createWithFormat("%d/%d", count, maxCount)->getCString());
    //    }
}
void EditorHud::reduceBulletCount(){
    if (bulletArray.size() <= 0) {
        return;
    }
    Sprite* sptBullet = bulletArray.at(bulletArray.size() - 1);
    bulletArray.eraseObject(sptBullet);
    sptBullet->removeFromParentAndCleanup(true);
}
void EditorHud::setBulletCount(int count, int maxCount){
    //    Node* checkBoard = this->getChildByName("sptCheckBoard");
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
    //        this->addChild(sptBullet);
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

void EditorHud::keyCollected(int count)
{
    lblKeyCollected->setString(__String::createWithFormat("x %d", count)->getCString());
    GameManager::getInstance()->showParticleExplosion(this, "blueKey.png",lblKeyCollected->getPosition(), 2);
}
void EditorHud::starCollected(int count)
{
    Vec2 pos;
    
    
    GameManager::getInstance()->showParticleExplosion(this, "goldStar.png", pos, 2);
    
    starCount = count;
}


void EditorHud::starCollectedFancy(int count, Vec2 collectedPos)
{
    
}

void EditorHud::starChange(float dt)
{
    
}

void EditorHud::spriteMoveDone(Node* sprite)
{
    sprite->removeFromParentAndCleanup(true);
}

void EditorHud::resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType){
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
    
    
    world->resumeLayer();
    this->enableButtons(true);
}
void EditorHud::showPauseLayer()
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
    btnResume->addTouchEventListener(CC_CALLBACK_2(EditorHud::resumeGame, this));
    
    Button* btnRestart = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_0"));
    btnRestart->addTouchEventListener(CC_CALLBACK_2(EditorHud::retryStage, this));
    
    Button* btnStages = dynamic_cast<Button*>(Helper::seekWidgetByName(pauseLayer, "Button_14_1"));
    btnStages->addTouchEventListener(CC_CALLBACK_2(EditorHud::toStageSelect, this));
    
    //    NativeInterface::NativeInterface::showRectAds();
    GameManager::getInstance()->isInMiddleOfGame = false;
    
    GameManager::getInstance()->saveCoin();
    
    enableButtons(false);
    
    world->pauseLayer();
}

void EditorHud::showShopLayer(std::string shopName)
{
    world->isInEvent = false;
    world->pauseLayer();
    Node* layer = CSLoader::createNode("SmithShop.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setTag(SELECT_DIALOG_GO_TO_NPC);
    setAsPopup(layer);
    ddiyong(layer);
    shopLayer = layer;
    inventoryLayer = layer;
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    Label* newLbl = changeTextWithLabel(lbl);
    std::string str = strmake("%s(%s)", LM->getText("shop").c_str(), LM->getText(shopName.c_str()).c_str());
    newLbl->setScale(newLbl->getScale()*1.5f);
    log("str: %s", str.c_str());
    newLbl->setString(str);
    
    Text* lblCoin = (Text*)layer->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    Sprite* sptCoin = changeCoinToAnimation(layer->getChildByName("sptPriceCoin"));
    sptCoin->setScale(4.5f);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    changeTextWithLabel(lbl, 608);
    
    lbl = (Text*)layer->getChildByName("lblTip");
    newLbl = changeTextWithLabel(lbl, 313);
    LM->setLocalizedString(newLbl, "press a to see the detail");
    
    Button* btn = (Button*)layer->getChildByName("btnLeft");
    btn->addClickEventListener(CC_CALLBACK_1(EditorHud::selectInventorySlot, this));
    btn->setPressedActionEnabled(true);
    btn = (Button*)layer->getChildByName("btnRight");
    btn->addClickEventListener(CC_CALLBACK_1(EditorHud::selectInventorySlot, this));
    btn->setPressedActionEnabled(true);
    
    //    addBToCloseToLayer(layer);
    
    lbl= (Text*)firstPopup->getChildByName("lblPage");
    lbl->setTag(0);
    lbl->setFontName("visitor1.ttf");
    
    
    for (int i = 0; i < 9; i++) {
        btn = (Button*)firstPopup->getChildByName(strmake("btnSlot%d", i));
        Label* lbl = Label::createWithSystemFont("0", "visitor1.ttf", 80);
        lbl->setTextColor(Color4B::BLACK);
        lbl->setName("lblCount");
        btn->addChild(lbl, 1);
        lbl->setAnchorPoint(Vec2(1, 0));
        lbl->enableOutline(Color4B::WHITE, 8);
        lbl->setScale(0.08);
        lbl->setPosition(Vec2(btn->getContentSize().width - 2, 1));
        lbl->setVisible(false);
        btn->addClickEventListener(CC_CALLBACK_1(EditorHud::selectInventorySlot, this));
        btn->setPressedActionEnabled(true);
    }
    
    std::string itemName;
    std::string firstItemName;
    for (int i = 0; i < 9; i++) {
        btn = (Button*)firstPopup->getChildByName(strmake("btnSlot%d_0", i));
        btn->addClickEventListener(CC_CALLBACK_1(EditorHud::selectShopItem, this));
        btn->setPressedActionEnabled(true);
        itemName = world->shopTable[shopName].asValueMap().at(strmake("item %d", i)).asString();
        btn->setName(itemName);
        if(itemName.size() <= 0){
            btn->setVisible(false);
            continue;
        }
        if(i == 0){
            firstItemName = itemName;
        }
        //        std::string fileName = getFileNameForInventory(world->getItemType(itemName), itemName);
        //        if(fileName.size() > 0){
        //            addSpriteToButton(fileName, btn, Button::TextureResType::LOCAL);
        //        }else{
        //            addSpriteToButton(world->getItemSpriteName(world->getItemType(itemName), itemName), btn, Button::TextureResType::PLIST);
        //        }
        addSpriteToButton(itemName, btn);
    }
    
    
    //    selectInventorySlot(layer->getChildByName("btnSlot0"));
    //    rearrangeInventoryItems();
    updateInventory();
    showInstanceMessage(LM->getText("a to select b to cancel"));
    selectShopItem(firstPopup->getChildByName(firstItemName));
}
void EditorHud::onBuyItemClick(){
    std::string itemName = buyLayer->getName();
    int itemType = world->getItemType(itemName);
    
    inventory->addItem(itemName, getInventoryTypeForItemType(itemType));
    
    int price = world->getItemPrice(itemType, itemName);
    GM->addCoin(-price);
    
    
    closePopup();
    buyLayer = nullptr;
    updateInventory();
}
void EditorHud::selectShopItem(Ref* ref){
    if(ref == nullptr) return;
    Button* btn = (Button*)ref;
    if(btn->getName().size() <= 0){
        return;
    }
    if(btn->getTag() < 11){
        selectInventorySlot(ref);
        return;
    }
    Node* selectBox = firstPopup->getChildByName("sptSelect");
    selectedShopItem = btn;
    selectedInventorySlot = btn;
    
    Label* lbl = (Label*)inventoryLayer->getChildByName("lblDescription");
    if (selectBox->getPosition() != btn->getPosition()) {
        selectBox->setPosition(btn->getPosition());
        std::string name = LM->getText(btn->getName().c_str());
        std::string desc;
        if(world->getItemType(btn->getName()) == ITEM_TYPE_ETC &&  world->itemStatTable[btn->getName()].asValueMap().at("ability").asInt() > 0){
            desc = strmake(LM->getText(strmake("%s desc", btn->getName().c_str()).c_str()).c_str(), world->itemStatTable[btn->getName()].asValueMap().at("ability").asInt());
        }else{
            desc = LM->getText(strmake("%s desc", btn->getName().c_str()).c_str());
        }
        
        std::string str = strmake("[%s]\n%s", name.c_str(), desc.c_str());
        lbl->setString(str);
        selectBox->setScale(6.3);
        
        int itemType = world->getItemType(btn->getName());
        int price = world->getItemPrice(itemType, btn->getName());
        Text* txt = (Text*)shopLayer->getChildByName("lblPrice");
        txt->setString(Value(price).asString());
    }else{
        showBuyLayer(btn, btn->getName());
    }
    updateInventory();
}
void EditorHud::showBuyLayer(Button* btn, std::string itemName){
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
    
    int itemType = world->getItemType(itemName);
    //    std::string fileName = "";
    //    if(itemType != ITEM_TYPE_ETC){
    //        fileName = getFileNameForInventory(itemType, itemName);
    //    }
    //    if(itemType == ITEM_TYPE_ETC || fileName.size() <= 0){
    //        Sprite* sptIcon = Sprite::createWithSpriteFrameName(world->getItemSpriteName(itemType, itemName));
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
    int price = world->getItemPrice(itemType, itemName);
    lbl->setString(Value(price).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    
    lbl = (Text*)layer->getChildByName("lblStatHP");
    std::string strStat = world->getItemStat(itemType, itemName, "hp");
    if(strStat.size() > 0){
        lbl->setString(strmake("HP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatAP");
    strStat = world->getItemStat(itemType, itemName, "ap");
    if(strStat.size() > 0){
        lbl->setString(strmake("AP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatDP");
    strStat = world->getItemStat(itemType, itemName, "dp");
    if(strStat.size() > 0){
        lbl->setString(strmake("DP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatGP");
    strStat = world->getItemStat(itemType, itemName, "gp");
    if(strStat.size() > 0){
        lbl->setString(strmake("GP +%d", Value(strStat).asInt()));
    }else{
        lbl->setString("N/A");
        lbl->setColor(Color3B(127, 127, 127));
    }
    
    lbl = (Text*)layer->getChildByName("lblStatSP");
    strStat = world->getItemStat(itemType, itemName, "sp");
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
void EditorHud::onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        
        return;
    }
    
    GameManager::getInstance()->showVideo(VIDEO_GAME_OVER);
}
void EditorHud::videoDone(){
//    if(GameManager::getInstance()->getEditorHud()){
//        if (GameManager::getInstance()->videoIndex == VIDEO_GAME_OVER) {
//            GameManager::getInstance()->getEditorHud()->scheduleOnce(schedule_selector(EditorHud::closeVideoDone), 0.05);
//        }else if(GameManager::getInstance()->videoIndex == VIDEO_WEAPON_RENT){
//            messageBox->removeFromParent();
//        }
//    }
}
void EditorHud::videoFailed(){
//    if(GameManager::getInstance()->getEditorHud()){
//        GameManager::getInstance()->getEditorHud()->scheduleOnce(schedule_selector(EditorHud::closeVideoFailed), 0.1);
//    }
}
void EditorHud::closeVideoDone(float dt){
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

void EditorHud::closeVideoFailed(float dt){
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

void EditorHud::playCoinSoundLater(){
    
}
void EditorHud::coinLabelScheduler(float dt){
    CCLOG("coinLabelScheduler");
    increasingCoinCount += coinCount/10;
    increasingCounter++;
    if (increasingCounter >= increasingCounterMax) {
        increasingCoinCount = coinCount;
        increasingCounter = 0;
        this->schedule(schedule_selector(EditorHud::enemyLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(EditorHud::coinLabelScheduler));
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
void EditorHud::enemyLabelScheduler(float dt){
    increasingEnemyCount += enemyKilledCount/10.0f;
    increasingCounter++;
    char buf[20];
    if (increasingCounter >= increasingCounterMax) {
        increasingEnemyCount = enemyKilledCount;
        increasingCounter = 0;
        
        sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
        lblEnemyCount->setString(buf);
        this->schedule(schedule_selector(EditorHud::expLabelScheduler), 0.05f);
        this->unschedule(schedule_selector(EditorHud::enemyLabelScheduler));
        return;
    }
    
    sprintf(buf, "%d/%d", (int)increasingEnemyCount, enemyMaxCount);
    lblEnemyCount->setString(buf);
    
}
void EditorHud::expLabelScheduler(float dt){
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
        this->unschedule(schedule_selector(EditorHud::expLabelScheduler));
        
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
void EditorHud::tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    int count = reviveCount;
    if (count == 0) {
        GameManager::getInstance()->showVideo(VIDEO_GAME_CONTINUE);
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
        
        this->scheduleOnce(schedule_selector(EditorHud::reviveLater), duration*2/3);
        
        Button* btnRevive = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnContinue"));
        btnRevive->removeFromParentAndCleanup(true);
        Button* btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(reviveLayer, "btnClose"));
        btnClose->removeFromParentAndCleanup(true);
    }else{
        CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(EditorHud::recommandCoinShopClosed, this));
        
    }
}

void EditorHud::recommandCoinShopClosed(Node* sender){
    
}
void EditorHud::notEnoughGemClosed(Ref* obj){
    
}
void EditorHud::closeWaitMessageBoxLater(){
    this->scheduleOnce(schedule_selector(EditorHud::closeWaitMessageBox), 0.01);
}
void EditorHud::closeWaitMessageBox(float dt){
    
}
void EditorHud::closeWaitMessageBoxLaterAndShowSucess(){
    this->scheduleOnce(schedule_selector(EditorHud::closeWaitMessageBoxAndShowSuccess), 0.01);
}
void EditorHud::closeWaitMessageBoxAndShowSuccess(float dt){
    
}
void EditorHud::reviveFromVideo(){
    //    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(EditorHud::playBackgroundMusicLater), 2);
    world->revive();
    
    int count = reviveCount;
    count++;
    reviveCount = count;
    GameManager::getInstance()->saveCoin();
}
void EditorHud::reviveLater(float dt){
    //    blackScreen->setVisible(false);
    GameManager::getInstance()->animateFadeOut(reviveLayer);
    reviveLayer = NULL;
    pauseItem->setVisible(true);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    recordItem->setVisible(true);
#endif
    this->scheduleOnce(schedule_selector(EditorHud::playBackgroundMusicLater), 2);
    world->revive();
    
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
void EditorHud::playBackgroundMusicLater(float dt){
    
}
void EditorHud::buyGemClosed(Ref* obj){
    
}
void EditorHud::removeUsedAssets(){
    world->removeUsedAssets();
    
    _eventDispatcher->removeEventListener(touchListener);
    _eventDispatcher->removeEventListener(listener);
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
     
     world->removeUsedAssets();*/
}
void EditorHud::toMain(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
//    ServerManager::getInstance()->getHttpTime();
    world->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_MAIN;
    this->scheduleOnce(schedule_selector(EditorHud::closeSchedule), 0.01);
}
void EditorHud::toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
//    ServerManager::getInstance()->getHttpTime();
    world->saveCoinIfStarCollected();
    
    whereToGo = CLOSE_TO_STAGES;
    this->scheduleOnce(schedule_selector(EditorHud::closeSchedule), 0.01);
}
void EditorHud::closeSchedule(float dt){
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
        
    }else if(toWhat == CLOSE_TO_MAIN){
        GameManager::getInstance()->page = PAGE_TITLE;
        
    }
}
void EditorHud::nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
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
        this->scheduleOnce(schedule_selector(EditorHud::closeSchedule), 0.01);
    }else{
        whereToGo = CLOSE_TO_STAGES;
        this->scheduleOnce(schedule_selector(EditorHud::closeSchedule), 0.01);
    }
}
void EditorHud::retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    
    world->saveCoinIfStarCollected();
    whereToGo = CLOSE_TO_RETRY;
    this->scheduleOnce(schedule_selector(EditorHud::closeSchedule), 0.01);
}
void EditorHud::onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType){
    if(TouchEventType::TOUCH_EVENT_ENDED != (TouchEventType)eEventType) return;
    if (GameManager::getInstance()->market == MARKET_CHINA_NO_IAP) {
        
        return;
    }
    
}

void EditorHud::enableButtons(bool enb)
{
}

void EditorHud::displayBossEnergy(const char* name)
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

void EditorHud::setBossEnergy(int percent)
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
void EditorHud::removeListener(){
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(_listener);
    this->unschedule(schedule_selector(EditorHud::oneSecUpdate));
    world->removeUsedAssets();
    //    js2->removeListener();
}
void EditorHud::onTutorialBoxClick(){
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
        }
        if (tutorialIndex < 11) {
            tutorialHowTo->setVisible(true);
            ImageView* imgContent = (ImageView*)tutorialHowTo->getChildByName("imgContent");
            if (tutorialIndex == 1 || tutorialIndex == 10) {
                this->schedule(schedule_selector(EditorHud::updateTutorialHowTo), 0.1f);
            }else{
                imgContent->loadTexture(strmake("tutorial%d.png", tutorialIndex));
            }
        }
    }
}
void EditorHud::addListener(){
    touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesBegan");
        //        log("test touch for find path");// test
        //        GM->getPath(GM->testStartPos, GM->testEndPos);
        
        this->onAClick();
        if(tutorialNode != nullptr){
            onTutorialBoxClick();
        }
        //        auto last = touches.back();
        
        //        chopad->TouchesBegan(touches, evt);
        // check if last touch point is in which button
        
        //        world->showTalkText("what? asdf sadf dsfa dsfdasf asdf sadf dsaf ad fasd fasd f adsf dsaf adsf asd fsad fadsf asdf ", WHOSE_TALK_HERO);
        
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
void EditorHud::updateTutorialHowTo(float dt){
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
void EditorHud::showDialog(const char* message, const char* btn1, const char* btn2)
{
    CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(EditorHud::messageBoxClosed, this));
    
//    GameManager::getInstance()->getEditorHud()->enableJoystick(false);
    
    this->setTouchEnabled(false);
    
    ((EditorWorld*)world)->pauseLayer();
}

void EditorHud::messageBoxClosed(Node* node)
{
    ((EditorWorld*)world)->resumeLayer();
    this->setTouchEnabled(true);
    //    _dialogBox->setVisible(false);
    
//    GameManager::getInstance()->getEditorHud()->enableJoystick(true);
    
    if (this->getTag() == 0) {
        //        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void EditorHud::showSingleNPCMessage(std::string msg){
    isReadyToShowNextTalk = false;
    world->isInEvent = true;
    talkIndex = 0;
    //    talkState = TALK_STATE_ASKING;
    this->schedule(schedule_selector(EditorHud::onDisposableMessageEvent));
    world->showTalkText(msg, WHOSE_TALK_NPC);
    showBlackTopAndBottom();
}
void EditorHud::onDisposableMessageEvent(float dt){
    if(!isReadyToShowNextTalk){
        return;
    }
    isReadyToShowNextTalk = false;
    endEvent();
}
void EditorHud::showEvent(int index, bool isQuest){
    eventIndex = index;
    isThisEventQuest = isQuest;
    isReadyToShowNextTalk = true;
    talkIndex = 0;// test
    
    talkState = TALK_STATE_ASKING;
    this->schedule(schedule_selector(EditorHud::onEvent));
    
    showBlackTopAndBottom();
}
void EditorHud::onEvent(float dt){
    if(!isReadyToShowNextTalk){
        return;
    }
    if(sptTalkBox != nullptr){
        sptTalkBox->removeFromParent();
        sptTalkBox = nullptr;
    }
    isReadyToShowNextTalk = false;
    //    EditorWorld* stage = world;
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
            world->showTalkText(text.substr(5), WHOSE_TALK_HERO);
        }else if(text.find(">") != std::string::npos){
            world->showTalkText(text.substr((int)text.find(">") + 1), WHOSE_TALK_NPC, text.substr(0, (int)text.find(">")));
        }
        //        else if(text.find("fairy>") != std::string::npos){
        //            world->showTalkText(text.substr(6), WHOSE_TALK_NPC);
        //        }else if(text.find("fairy>") != std::string::npos){
        //            world->showTalkText(text.substr(5), WHOSE_TALK_HERO);
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
            world->showTalkText(text.substr(5), WHOSE_TALK_HERO);
        }else if(text.find(">") != std::string::npos){
            world->showTalkText(text.substr((int)text.find(">") + 1), WHOSE_TALK_NPC);
        }
    }
    talkIndex++;
}
void EditorHud::showQuestComplete(int index){
    questIndex = index;
    isReadyToShowNextTalk = true;
    talkIndex = 0;
    world->isInEvent = true;
    world->claimQuestReward();
    world->onQuestComplete();
    this->schedule(schedule_selector(EditorHud::onQuestCompleteEvent));
    showBlackTopAndBottom();
}
void EditorHud::onQuestCompleteEvent(float dt){
    if(!isReadyToShowNextTalk){
        return;
    }
    if(sptTalkBox != nullptr){
        sptTalkBox->removeFromParent();
        sptTalkBox = nullptr;
    }
    isReadyToShowNextTalk = false;
    std::string text;
    std::string key = StringUtils::format("event%d_after_%d", questIndex, talkIndex);
    text = LanguageManager::getInstance()->getText(key.c_str());
    if(text.size() == 0){
        endEvent();
    }else if(text.find("adin>") != std::string::npos){
        world->showTalkText(text.substr(5), WHOSE_TALK_HERO);
    }else if(text.find(">") != std::string::npos){
        world->showTalkText(text.substr((int)text.find(">") + 1), WHOSE_TALK_NPC);
    }
    talkIndex++;
}
void EditorHud::showBlackTopAndBottom(){
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
        lblSkip->addClickEventListener(CC_CALLBACK_0(EditorHud::onSkipClick, this));
        lblSkip->setTouchEnabled(true);
        lblSkip->setOpacity(0);
        PPLabel* lbl = PPLabel::create("SKIP", 80);
        blackBottom->addChild(lbl);
        lbl->setPosition(lblSkip->getPosition());
    }
    
    hideBtns();
}
void EditorHud::onSkipClick(){
    if(isSceneChanging)return;
    isSceneChanging = true;
    
    GM->nextScene = STAGE_FIELD;
    auto scene = Scene::create();
    Title* title = Title::create();
    scene->addChild(title);
    int stage = world->stageIndex;
    Node* temp = Node::create();
    temp->setTag(stage);
    title->showStageReady(stage);
    this->removeListener();
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void EditorHud::hideBlackTopAndBottom(){
    if(blackTop == nullptr) return;
    blackTop->runAction(Sequence::create(MoveBy::create(1, Vec2(0, 80)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, blackTop)), nullptr));
    blackBottom->runAction(Sequence::create(MoveBy::create(1, Vec2(0, -80)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, blackBottom)), nullptr));
    showBtns();
}
void EditorHud::updateTalkBoxRope(float dt){
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
void EditorHud::onTalkBoxResizeDone(){
    this->unschedule(schedule_selector(EditorHud::updateTalkBoxRope));
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

void EditorHud::readyToShowNextTalk(){
    isReadyToShowNextTalk = true;
}

void EditorHud::onLeftRelease(){
    isCursorReadyToMove = true;
}
void EditorHud::onRightRelease(){
    isCursorReadyToMove = true;
}
void EditorHud::onUpRelease(){
    isCursorReadyToMove = true;
}
void EditorHud::onDownRelease(){
    isCursorReadyToMove = true;
}
void EditorHud::onLeftPress(){
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
void EditorHud::selectMenu(Node* selectedItem){
    if(selectedItem == nullptr) return;
    Node* spt = menuLayer->getChildByName("sptSelect");
    spt->setPosition(selectedItem->getPosition());
    selectedMenu = selectedItem;
}
void EditorHud::onRightPress(){
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
void EditorHud::onUpPress(){
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

void EditorHud::onDownPress(){
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
void EditorHud::selectABCOption(Node* node){
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
                    this->scheduleOnce(schedule_selector(EditorHud::closeABCLayerLater), 1);
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
                int ability = world->itemStatTable[name].asValueMap().at("ability").asInt();
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
            this->scheduleOnce(schedule_selector(EditorHud::closeABCLayerLater), 1);
        }else if(actionType == ACTION_TYPE_USE){
            abcLayer->getChildByName("lblOption2")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(EditorHud::closeABCLayerLater), 1);
        } else if(actionType == ACTION_TYPE_SELL){
            int price = world->getItemPrice(getItemTypeInSlot(slot), name)/10;
            int count = inventory->getItemCount(slot);
            GM->addCoin(price*count);
            inventory->removeItem(name, count);
            updateInventory();
            abcLayer->getChildByName("lblOption1")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(EditorHud::closeABCLayerLater), 1);
        } else if (actionType == ACTION_TYPE_THROW_AWAY) {
            int count = inventory->getItemCount(slot);
            inventory->removeItem(name, count);
            updateInventory();
            abcLayer->getChildByName("lblOption1")->runAction(Blink::create(0.5, 2));
            this->scheduleOnce(schedule_selector(EditorHud::closeABCLayerLater), 1);
            Label* lbl = (Label*)inventoryLayer->getChildByName("lblDescription");
            lbl->setString("");
        }else{
            closePopup();
            abcLayer = nullptr;
        }
    }
}
void EditorHud::equipItem(int slot){
    std::string name = inventory->getItemName(slot);
    int itemType = getItemTypeInSlot(slot);
    std::string equiped;
    if(itemType == ITEM_TYPE_WEAPON){
        equiped = world->getHeroWeapon(0);
        world->setHeroWeapon(0, name);
    }else if(itemType == ITEM_TYPE_SHIELD){
        equiped = world->getHeroShield(0);
        world->setHeroShield(0, name);
    }else if(itemType == ITEM_TYPE_SHOES){
        equiped = world->getHeroShoes(0);
        world->setHeroShoes(0, name);
    }else if(itemType == ITEM_TYPE_HELMET){
        equiped = world->getHeroHelmet(0);
        world->setHeroHelmet(0, name);
    }else if(itemType == ITEM_TYPE_ETC){
        
    }
    inventory->removeItem(slot);
    if(equiped.size() == 0){
        
    }else{
        inventory->addItem(equiped, INVENTORY_ITEM_TYPE_EQUIPMENT);
        updateInventory();
    }
}
void EditorHud::closeABCLayerLater(float dt){
    
}
void EditorHud::ddiyong(Node* node){
    node->setScale(0.1f);
    node->runAction(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.05, 0.95),ScaleTo::create(0.05, 1), NULL));
}
void EditorHud::showQuestDoneAlert(){
    world->pauseLayer();
    Node* layer = CSLoader::createNode("SelectDialog.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setTag(SELECT_DIALOG_GO_TO_NPC);
    setAsPopup(layer);
    ddiyong(layer);
    selectDialog = layer;
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    Label* newLbl = changeTextWithLabel(lbl);
    newLbl->setWidth(550/newLbl->getScale());
    LM->setLocalizedString(newLbl, "go to quest npc");
    
    lbl = (Text*)layer->getChildByName("lblOptionA");
    lbl->setString(strmake("A. %s", LM->getText("yes").c_str()));
    
    lbl = (Text*)layer->getChildByName("lblOptionB");
    lbl->setString(strmake("B. %s", LM->getText("no").c_str()));
}
void EditorHud::onAClick(){
    //    world->addHeroExp(0, 10); // test
    //    setExp(world->getHeroLevel(0), world->getHeroExp(0), world->getMaxExp(world->getHeroLevel(0)));
    if(inputShouldWait)return;
    //    world->draw->clear();
    
    EditorWorld* stage = world;
    if(stage->isInEvent){
        if(talkState == TALK_STATE_QUESTIONING){
            answer = 0;
            talkState = TALK_STATE_CHOOSED;
            talkIndex = 0;
            sptTalkBox->getChildByTag(0)->runAction(Sequence::create(Blink::create(0.8, 4), DelayTime::create(1),CallFunc::create(CC_CALLBACK_0(EditorHud::readyToShowNextTalk, this)), NULL));
            world->setQuest();
        }else{
            if(stage->imgTalkBox != nullptr && stage->imgTalkBox->getChildByName("TOUCH") == nullptr){
                stage->talkIndex = (int)stage->talkText.size();
            }else{
                isReadyToShowNextTalk = true;
            }
        }
    }else if(goToNPCLayer != nullptr){
        if(goToNPCLayer->getTag() >= 0){
//            int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//            UD->setStringForKey(KEY_LAST_SAVE_POINT, world->npcTable[Value(questID).asString()].asValueMap().at("stage").asString().c_str());
            GM->isTransported = true;
            showBIAndReplaceScene();
        }
    }else if(buyLayer != nullptr){
        std::string itemName = buyLayer->getName();
        int itemType = world->getItemType(itemName);
        if (inventory->isSlotAvailableForThisItem(itemName, getInventoryTypeForItemType(itemType), 1)) {
            int price = world->getItemPrice(itemType, itemName);
            int coin = GM->getCoin();
            if(coin >= price){
                buyLayer->getChildByName("lblYes")->runAction(Blink::create(0.5, 2));
                buyLayer->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(EditorHud::onBuyItemClick, this)), nullptr));
            }else{
                showInstanceMessage(LM->getText("not enough money"));
                GM->playSoundEffect(SOUND_NAGATIVE);
            }
        }else{
            showInstanceMessage(LM->getText("not enough inventory slot"));
        }
    }else if(selectDialog != nullptr){
        selectDialog->getChildByName("lblOptionA")->runAction(Blink::create(0.5, 2));
        this->scheduleOnce(schedule_selector(EditorHud::selectSelectDialog), 1);
    }else if(abcLayer != nullptr){
        selectABCOption(selectedAbcOption);
    }else if(doctorShopLayer != nullptr && abcLayer == nullptr){
        std::string tier = UDGetStr(strmake("hero_tier_%d", 0).c_str(), "0");
        int nextTier = Value(tier).asInt() + 1;
        std::string maxLevel = world->getEvolutionStat(tier, "max level");
        std::string fee = world->getEvolutionStat(tier, "bug");
        std::string ingredient0 = world->getEvolutionStat(tier, "ingredient 0");
        std::string ingredient0Count = world->getEvolutionStat(tier, "ingredient 0 count");
        std::string ingredient1 = world->getEvolutionStat(tier, "ingredient 1");
        std::string ingredient1Count = world->getEvolutionStat(tier, "ingredient 1 count");
        std::string ingredient2 = world->getEvolutionStat(tier, "ingredient 2");
        std::string ingredient2Count = world->getEvolutionStat(tier, "ingredient 2 count");
        std::string ingredient3 = world->getEvolutionStat(tier, "ingredient 3");
        std::string ingredient3Count = world->getEvolutionStat(tier, "ingredient 3 count");
        
        bool feeEnough = GM->getCoin() >= Value(fee).asInt();
        bool ingredient0Enough = inventory->getTotalItemCount(ingredient0) >= Value(ingredient0Count).asInt();
        bool ingredient1Enough = inventory->getTotalItemCount(ingredient1) >= Value(ingredient1Count).asInt();
        bool ingredient2Enough = inventory->getTotalItemCount(ingredient2) >= Value(ingredient2Count).asInt();
        bool ingredient3Enough = inventory->getTotalItemCount(ingredient3) >= Value(ingredient3Count).asInt();
        bool isMaxLevel = world->getHeroLevel(0) >= Value(maxLevel).asInt();
        
        if(!feeEnough || !isMaxLevel || !ingredient0Enough || !ingredient1Enough || !ingredient2Enough || !ingredient3Enough){
            doctorShopLayer->getChildByName("lblDescription")->runAction(Blink::create(0.5, 2));
        }else{
            doctorShopLayer = nullptr;
            this->schedule(schedule_selector(EditorHud::evolving), 0.07);
        }
    }else if(abDialog != nullptr){
        if(abDialog->getTag() == DIALOG_TYPE_AB_RESET){
            abDialog->getChildByName("lblOptionA")->runAction(Sequence::create(Blink::create(0.8, 4), DelayTime::create(1),CallFunc::create(CC_CALLBACK_0(EditorHud::processReset, this)), NULL));
        }
        abDialog = nullptr;
        
    }else if(deadPopup != nullptr){
        onReviveByGem();
    }else if(alchyShopLayer != nullptr){
        
    }else if(peterShopLayer != nullptr){
        onChoosedInsurancePlan(selectedPeterItem);
    }else if(samShopLayer != nullptr && abcLayer == nullptr){
        selectSamShopItem(selectedSamShopItem);
    }else if(shopLayer != nullptr && abcLayer == nullptr){
        selectShopItem(selectedInventorySlot);
    }else if(inventoryLayer != nullptr && abcLayer == nullptr){
        selectInventorySlot(selectedInventorySlot);
    }else if(menuLayer != nullptr){
        if(selectedMenu->getTag() == 0){
            showQuest();
        }else if(selectedMenu->getTag() == 1){
            showMap();
        }else if(selectedMenu->getTag() == 2){
            //            showShop();
        }else if(selectedMenu->getTag() == 3){
            showPVP();
        }else if(selectedMenu->getTag() == 4){
            showPeterInsuranceShop();
        }else if(selectedMenu->getTag() == 11){
            onResetClick();
        }
        
    }
}

int EditorHud::getInventoryTypeForItemType(int itemType){
    if(itemType == ITEM_TYPE_ETC){
        return INVENTORY_ITEM_TYPE_ETC;
    }else{
        return INVENTORY_ITEM_TYPE_EQUIPMENT;
    }
}
void EditorHud::showGoToNPC(){
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
    this->schedule(schedule_selector(EditorHud::showGoToNPCProgress), 0.09f);
}
void EditorHud::showGoToNPCProgress(float dt){
    goToNPCLayer->setTag(goToNPCLayer->getTag() + 1);
    
    Label* lbl = (Label*)goToNPCLayer->getChildByName("lbl1");
    lbl->setString(strmake("%d%%", goToNPCLayer->getTag()));
    if(goToNPCLayer->getTag() >= 100){
        this->unschedule(schedule_selector(EditorHud::showGoToNPCProgress));
        lbl = (Label*)goToNPCLayer->getChildByName("lbl0");
        LM->setLocalizedString(lbl, "transported");
        
        lbl = (Label*)goToNPCLayer->getChildByName("lbl1");
        LM->setLocalizedString(lbl, "press a");
        
        lbl = (Label*)goToNPCLayer->getChildByName("lbl2");
        lbl->setString("");
    }
}
void EditorHud::selectSelectDialog(float dt){
    int tag = selectDialog->getTag();
    closePopup();
    selectDialog = nullptr;
    if(tag == SELECT_DIALOG_GO_TO_NPC){
        showGoToNPC();
    }
}
void EditorHud::endEvent(){
    this->unschedule(schedule_selector(EditorHud::onEvent));
    this->unschedule(schedule_selector(EditorHud::onDisposableMessageEvent));
    this->unschedule(schedule_selector(EditorHud::onQuestCompleteEvent));
    hideBlackTopAndBottom();
    world->endEvent();
    if(isThisEventQuest){
        isThisEventQuest = false;
    }
    
    if(sptTalkBox != nullptr){
        sptTalkBox->removeFromParent();
        sptTalkBox = nullptr;
    }
    
    onSkipClick();
}
void EditorHud::onBClick(){
    //    world->addHeroExp(0, 100); // test
    setExp(world->getHeroLevel(0), world->getHeroExp(0), world->getMaxExp(world->getHeroLevel(0)));
    if(inputShouldWait)return;
    EditorWorld* stage = world;
    if(stage->isInEvent){
        if(eventIndex == 100){
            return;
        }
        if(talkState == TALK_STATE_ASKING){
            int currentIndex = talkIndex;
            
            std::string key = StringUtils::format("event%d_option_0", eventIndex);
            std::string text = LanguageManager::getInstance()->getText(key.c_str());
            if(text.size() == 0){
                endEvent();
            }else{
                talkState = TALK_STATE_QUESTIONING;
                isReadyToShowNextTalk = true;
                onEvent(0);
            }
        }else if(talkState == TALK_STATE_QUESTIONING){
            answer = 1;
            talkState = TALK_STATE_CHOOSED;
            talkIndex = 0;
            sptTalkBox->getChildByTag(1)->runAction(Sequence::create(Blink::create(0.8, 4), DelayTime::create(1),CallFunc::create(CC_CALLBACK_0(EditorHud::readyToShowNextTalk, this)), NULL));
        }else{//} if(talkState == TALK_STATE_CHOOSED){
            endEvent();
        }
    }else{
        if(abDialog != nullptr){
            abDialog = nullptr;
        }else if(buyLayer != nullptr){
            buyLayer = nullptr;
        }else if(abcLayer != nullptr){
            abcLayer = nullptr;
        }else if(selectDialog != nullptr){
            selectDialog = nullptr;
        }else if(questLayer != nullptr){
            questLayer = nullptr;
        }else if(doctorShopLayer != nullptr){
            doctorShopLayer = nullptr;
        }else if(peterShopLayer != nullptr){
            peterShopLayer = nullptr;
        }else if(alchyShopLayer != nullptr){
            alchyShopLayer = nullptr;
            
        }else if(samShopLayer != nullptr){
            samShopLayer = nullptr;
        }else if(deadPopup != nullptr){
            onReviveByVideo();
        }else if(shopLayer != nullptr){
            shopLayer = nullptr;
            inventoryLayer = nullptr;
        }else if(mapLayer != nullptr){
            mapLayer = nullptr;
        }else if(menuLayer != nullptr){
            menuLayer = nullptr;
        }else if(abcLayer != nullptr){
            abcLayer = nullptr;
        }else if(inventoryLayer != nullptr && abcLayer == nullptr){
            inventoryLayer = nullptr;
            world->updatePlayerStat();
        }
        
        closePopup();
        if(firstPopup == nullptr){
            world->resumeLayer();
            world->isInEvent = false;
        }
        
    }
}

void EditorHud::showOptions(std::string option0, std::string option1){
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
    
    sptTalkBox->runAction(Sequence::create(ResizeTo::create(0.2f, cocos2d::Size(8, 257)), ResizeTo::create(0.5f, cocos2d::Size(640, height)), CallFunc::create(CC_CALLBACK_0(EditorHud::onTalkBoxResizeDone, this)), nullptr));
    inputShouldWait = true;
    //    sptTalkBox->runAction(Sequence::create(ScaleTo::create(0.2f, 0.1f, 1),ScaleTo::create(0.5f, 1), CallFunc::create(CC_CALLBACK_0(EditorHud::onTalkBoxResizeDone, this)), nullptr));
    
    this->schedule(schedule_selector(EditorHud::updateTalkBoxRope));
}

void EditorHud::showEnding(int index){
    blackTop->setVisible(false);
    blackBottom->setVisible(false);
    this->setVisible(false);
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
    
    this->schedule(schedule_selector(EditorHud::updateEnding), 1);
    updateEnding(0);
}
void EditorHud::updateEnding(float dt){
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
            this->unschedule(schedule_selector(EditorHud::updateEnding));
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
PPLabel* EditorHud::showInstanceMessage(std::string msg){
    this->removeChildByTag(77);
    PPLabel* lbl = PPLabel::create(msg, 60, Color3B::WHITE, false, false, TextHAlignment::CENTER, true);
    this->addChild(lbl, 200);
    lbl->setTag(77);
    lbl->setPosition(Vec2(size.width/2, -TILE_SIZE/2));
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(MoveBy::create(dur, Vec2(0, distanceToMove)), DelayTime::create(2), MoveBy::create(dur, Vec2(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, lbl)), nullptr));
    return lbl;
}
//bool EditorHud::addItemToInventory(std::string name){
//    int count = getItemCountInInventory();
//
//    int index = -1;
//    if(world->getItemType(name) == ITEM_TYPE_ETC){
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
//int EditorHud::inventory->getItemCount(int slot){
//    return UD->getIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), 0);
//}
//void EditorHud::setItemCountForInventorySlot(int slot, int count){
//    UDSetInt(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//    if(count == 0){
//        UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "deleted");
//        rearrangeInventoryItems();
//    }
//}
//void EditorHud::addItemCountForInventorySlot(int slot){
//    int count = inventory->getItemCount(slot);
//    count++;
//    UD->setIntegerForKey(strmake(KEY_INVENTORY_ITEM_COUNT_FORMAT, slot).c_str(), count);
//}
//void EditorHud::setItemToInventory(std::string name, int slot){
//    UD->setStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), name);
//}
//int EditorHud::getItemCountInInventory(){
//    int count = 0;
//    while(true){
//        if(UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, count).c_str(), "").size() == 0){
//            break;
//        }
//        count++;
//    }
//    return count;
//}
//bool EditorHud::isSlotAvailableForThisItem(std::string itemName, int count){
//    int currentSlotOccupiedCount = getItemCountInInventory();
//    int index = currentSlotOccupiedCount;
//    if(world->getItemType(itemName) == ITEM_TYPE_ETC){
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
//void EditorHud::rearrangeInventoryItems(){
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
//void EditorHud::inventory->removeItem(std::string itemName){
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
//void EditorHud::inventory->removeItem(int slot){
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
Label* EditorHud::changeTextWithLabel(Text* txt, float width){
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
Sprite* EditorHud::changeCoinToAnimation(Node* imgCoin){
    return nullptr;
}
Sprite* EditorHud::changeGemToAnimation(Node* imgCoin){
    return nullptr;
}
void EditorHud::showInventory(){
    
}
Sprite* EditorHud::addBToCloseToLayer(Node* layer){
    Sprite* sptBToClose = Sprite::createWithSpriteFrameName("bToCloseReleased.png");
    layer->addChild(sptBToClose);
    GameManager::getInstance()->runAnimation(sptBToClose, "bToCloseAni", true);
    sptBToClose->setAnchorPoint(Vec2(1, 0));
    sptBToClose->setPosition(Vec2(196, 660));
    sptBToClose->setScale(4.5f);
    return sptBToClose;
}
//int EditorHud::getInventoryMaxSlotCount(){
//    return 9*4;
//}

void EditorHud::updateInventory(){
    
}
int EditorHud::getHPFromItem(std::string itemName){
    int itemType = world->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return world->getHelmetStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return world->getWeaponStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return world->getShieldStat(itemName, "hp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return world->getShoesStat(itemName, "hp");
    }
    return 0;
}
int EditorHud::getAPFromItem(std::string itemName){
    int itemType = world->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return world->getHelmetStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return world->getWeaponStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return world->getShieldStat(itemName, "ap");
    }else if(itemType == ITEM_TYPE_SHOES){
        return world->getShoesStat(itemName, "ap");
    }
    return 0;
}
int EditorHud::getDPFromItem(std::string itemName){
    int itemType = world->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return world->getHelmetStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return world->getWeaponStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return world->getShieldStat(itemName, "dp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return world->getShoesStat(itemName, "dp");
    }
    return 0;
}
int EditorHud::getGPFromItem(std::string itemName){
    int itemType = world->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return world->getHelmetStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return world->getWeaponStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return world->getShieldStat(itemName, "gp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return world->getShoesStat(itemName, "gp");
    }
    return 0;
}
int EditorHud::getSPFromItem(std::string itemName){
    int itemType = world->getItemType(itemName);
    if(itemType == ITEM_TYPE_HELMET){
        return world->getHelmetStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_WEAPON){
        return world->getWeaponStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_SHIELD){
        return world->getShieldStat(itemName, "sp");
    }else if(itemType == ITEM_TYPE_SHOES){
        return world->getShoesStat(itemName, "sp");
    }
    return 0;
}
//std::string EditorHud::getItemNameInInventorySlot(int slot){
//    return UD->getStringForKey(strmake(KEY_INVENTORY_ITEM_FORMAT, slot).c_str(), "");
//}
void EditorHud::selectInventorySlot(Ref* ref){
    if(ref == nullptr) return;
    Button* btn = (Button*)ref;
    Node* selectBox = firstPopup->getChildByName("sptSelect");
    int page = btn->getParent()->getChildByName("lblPage")->getTag();
    selectedInventorySlot = btn;
    Text* lbl = (Text*)inventoryLayer->getChildByName("lblDescription");
    int slot = btn->getTag() + page*9;
    std::string name = inventory->getItemName(slot);
    if (selectBox->getPosition() != btn->getPosition()) {
        selectBox->setPosition(btn->getPosition());
        if(selectedInventorySlot->getTag() == 9){ // left
            lbl->setString("");
            selectBox->setScale(4.2);
        }else if(selectedInventorySlot->getTag() == 10){ // right
            lbl->setString("");
            selectBox->setScale(4.2);
        }else{
            std::string desc;
            
            if(getItemTypeInSlot(slot) == ITEM_TYPE_ETC &&  world->itemStatTable[inventory->getItemName(slot)].asValueMap().at("ability").asInt() > 0){
                desc = strmake(LM->getText(strmake("%s desc", name.c_str()).c_str()).c_str(), world->itemStatTable[inventory->getItemName(slot)].asValueMap().at("ability").asInt());
            }else{
                desc = LM->getText(strmake("%s desc", name.c_str()).c_str());
            }
            
            std::string str = strmake("[%s]x%d\n%s", LM->getText(name.c_str()).c_str(), inventory->getItemCount(slot), desc.c_str());
            lbl->setString(str);
            selectBox->setScale(6.3);
        }
    }else{
        if(selectedInventorySlot->getTag() == 9){ // left
            page--;
            if(page < 0){
                page = 0;
            }
            btn->getParent()->getChildByName("lblPage")->setTag(page);
            updateInventory();
        }else if(selectedInventorySlot->getTag() == 10){ // right
            page++;
            if(page > inventory->getMaxSlotCount()/9-1){
                page = inventory->getMaxSlotCount()/9-1;
            }
            btn->getParent()->getChildByName("lblPage")->setTag(page);
            updateInventory();
        }else{
            if(shopLayer != nullptr){
                int price = world->getItemPrice(getItemTypeInSlot(slot), name)/10;
                int count = inventory->getItemCount(slot);
                showABCDialog(LM->getText("select action"), LM->getText("cancel"), strmake("%s (%dx%d=%d%s)", LM->getText("sell").c_str(), price, count, price*count, LM->getText("bug").c_str()).c_str(), "");
            }else{
                int slot = btn->getTag() + page*9;
                int itemType = getItemTypeInSlot(slot);
                if (itemType == ITEM_TYPE_SHIELD ||
                    itemType == ITEM_TYPE_WEAPON ||
                    itemType == ITEM_TYPE_SHOES ||
                    itemType == ITEM_TYPE_HELMET) {
                    showABCDialog(LM->getText("select action"), LM->getText("cancel"), LM->getText("throw away"), LM->getText("equip"));
                }else if(itemType == ITEM_TYPE_ETC){
                    std::string strAbility = world->itemStatTable[name].asValueMap().at("ability").asString();
                    if(strAbility.size() > 0){
                        showABCDialog(LM->getText("select action"), LM->getText("cancel"), LM->getText("throw away"), LM->getText("use"));
                    }else{
                        showABCDialog(LM->getText("select action"), LM->getText("cancel"), LM->getText("throw away"), "");
                    }
                }
            }
        }
    }
    updateInventory();
}
int EditorHud::getItemTypeInSlot(int slot){
    std::string name = inventory->getItemName(slot);
    return world->getItemType(name);
}
void EditorHud::showABCDialog(std::string msg, std::string option0, std::string option1, std::string option2){
    Node* layer = CSLoader::createNode("ThreeOptionDialog.csb");
    this->addChild(layer, 9);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    abcLayer = layer;
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    sptCursor = (Sprite*)layer->getChildByName("sptCursor");
    sptCursor->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Vec2(-5, 0)), MoveBy::create(0.2f, Vec2(5, 0)), NULL)));
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    lbl->setString(msg);
    changeTextWithLabel(lbl, 533);
    
    lbl = (Text*)layer->getChildByName("lblOption0");
    lbl->setString(strmake("%s", option0.c_str()));
    changeTextWithLabel(lbl);
    lbl->setTextAreaSize(cocos2d::Size(462, 68));
    selectABCOption(layer->getChildByName("btn0"));
    lbl = (Text*)layer->getChildByName("lblOption1");
    lbl->setString(strmake("%s", option1.c_str()));
    changeTextWithLabel(lbl);
    lbl->setTextAreaSize(cocos2d::Size(462, 68));
    lbl = (Text*)layer->getChildByName("lblOption2");
    lbl->setString(strmake("%s", option2.c_str()));
    changeTextWithLabel(lbl);
    lbl->setTextAreaSize(cocos2d::Size(462, 68));
    if(option2.size() == 0){
        layer->removeChildByName("lblOption2");
    }
}

void EditorHud::showABDialog(std::string msg, std::string option0, std::string option1, int dialogType){
    Node* layer = CSLoader::createNode("Dialog.csb");
    this->addChild(layer, 10);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    abDialog = layer;
    layer->setTag(dialogType);
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    lbl->setString(msg);
    changeTextWithLabel(lbl, 550);
    
    lbl = (Text*)layer->getChildByName("lblOptionA");
    lbl->addClickEventListener(CC_CALLBACK_0(EditorHud::onAClick, this));
    lbl->setString(strmake("A.%s", option0.c_str()));
    
    lbl = (Text*)layer->getChildByName("lblOptionB");
    lbl->addClickEventListener(CC_CALLBACK_0(EditorHud::onBClick, this));
    lbl->setString(strmake("B.%s", option1.c_str()));
}

bool EditorHud::isPopupAvailable(){
    if (firstPopup == nullptr) {
        return true;
    }else if(secondPopup == nullptr){
        return true;
    }else{
        return false;
    }
}
void EditorHud::closePopup(){
    if(secondPopup != nullptr){
        secondPopup->removeFromParent();
        secondPopup = nullptr;
    }else if (firstPopup != nullptr) {
        firstPopup->removeFromParent();
        firstPopup = nullptr;
    }
}
bool EditorHud::setAsPopup(Node* node){
    if (firstPopup == nullptr) {
        firstPopup = node;
    }else if(secondPopup == nullptr){
        secondPopup = node;
    }else{
        return false;
    }
    return true;
}
void EditorHud::ropeAround(Node* node){
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
void EditorHud::showBIAndReplaceScene(){
    ImageView* sptRect = ImageView::create("res/258_gray_rect.png");
    sptRect->setPosition(Vec2(size.width/2, size.height - 714));
    sptRect->setContentSize(cocos2d::Size(750, 750));
    sptRect->setAnchorPoint(Vec2(0.5, 0));
    sptRect->setCapInsets(cocos2d::Rect(3, 3, sptRect->getContentSize().width - 6, sptRect->getContentSize().height - 6));
    sptRect->setScale9Enabled(true);
    this->addChild(sptRect, 9);
    sptRect->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.5f, Vec2(0, 750)), 2), CallFunc::create(CC_CALLBACK_0(ImageView::removeFromParent, sptRect)), CallFunc::create(CC_CALLBACK_0(EditorHud::letsReplaceScene, this)), NULL));
    sptRect->setPosition(sptRect->getPosition() - Vec2(0, 750));
    
    Sprite* sptBI = Sprite::create("title_bi.png");
    sptBI->setPosition(sptRect->getContentSize()/2);
    sptBI->setScale(2);
    sptRect->addChild(sptBI);
}
void EditorHud::letsReplaceScene(){
    removeListener();
    Director::getInstance()->replaceScene(EditorWorld::scene(GM->nextScene, false));
}
//void EditorHud::removeItemFromInventory(std::string name, int count){
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
//int EditorHud::getTotalItemCount(std::string name){
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

void EditorHud::showIntro(){
//    HUD->this->setVisible(false);
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
    this->runAction(Sequence::create(DelayTime::create(dur - 2), CallFunc::create(CC_CALLBACK_0(EditorHud::showBIAndReplaceScene, this)), NULL));
    
    Node* layer = CSLoader::createNode("Intro.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    
    this->schedule(schedule_selector(EditorHud::onLoading));
}
void EditorHud::onLoading(float dt){
    loadingTime += dt;
    if(loadingTime < 0)
        return;
    float loadingCompleteTime = 3;
    
    LoadingBar* lb = (LoadingBar*)firstPopup->getChildByName("pbLoading");
    Text* lblLoading = (Text*)firstPopup->getChildByName("lblLoading");
    int percent = loadingTime*100/loadingCompleteTime;
    lblLoading->setString(strmake("LOADING... %d%%", percent));
    if(percent >= 101){
        percent = 100;
        this->unschedule(schedule_selector(EditorHud::onLoading));
        lblLoading->setString("PRESS A TO CONTINUE");
        lblLoading->runAction(RepeatForever::create(Blink::create(1, 2)));
    }
    lb->setPercent(percent);
}

void EditorHud::showSAMShop(){
    world->isInEvent = false;
    world->pauseLayer();
    Node* layer = CSLoader::createNode("SamShop.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setTag(SELECT_DIALOG_GO_TO_NPC);
    setAsPopup(layer);
    ddiyong(layer);
    samShopLayer = layer;
    
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    //    Label* newLbl = changeTextWithLabel(lbl);
    //    newLbl->setScale(newLbl->getScale()*1.5f);
    //    log("str: %s", str.c_str());
    //    newLbl->setString(str);
    lbl = (Text*)layer->getChildByName("lblFeeTitle");
    LM->setLocalizedString(lbl, "fee");
    
    Text* lblCoin = (Text*)layer->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    changeCoinToAnimation(layer->getChildByName("sptCoin"));
    
    lblCoin = (Text*)layer->getChildByName("lblFee");
    Sprite* sptCoin = changeCoinToAnimation(layer->getChildByName("sptCoinFee"));
    sptCoin->setScale(4.5f);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    changeTextWithLabel(lbl, 285.24f);
    
    lbl = (Text*)layer->getChildByName("lblIngredient");
    changeTextWithLabel(lbl);
    
    lbl = (Text*)layer->getChildByName("lblResult");
    changeTextWithLabel(lbl);
    
    lbl = (Text*)layer->getChildByName("lblResultDescription");
    changeTextWithLabel(lbl, 642.77f);
    
    
    lbl = (Text*)layer->getChildByName("lblTip");
    Label* newLbl = changeTextWithLabel(lbl, 313);
    LM->setLocalizedString(newLbl, "press a to see the detail");
    
    Button* btn;
    
    
    std::string firstItemName;
    std::string itemName;
    for (int i = 0; i < 6; i++) {
        btn = (Button*)firstPopup->getChildByName(strmake("btnSlot%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(EditorHud::selectSamShopItem, this));
        btn->setPressedActionEnabled(true);
        
        if(i == 0){
            itemName = "stone";
        }else if(i == 1){
            itemName = "iron";
        }else if(i == 2){
            itemName = "copper";
        }else if(i == 3){
            itemName = "gold";
        }else if(i == 4){
            itemName = "crystal";
        }else if(i == 5){
            itemName = "diamond";
        }
        
        btn->setName(itemName);
        if(i == 0){
            firstItemName = itemName;
        }
        if(itemName.size() <= 0){
            btn->setVisible(false);
            continue;
        }
        //        std::string fileName = getFileNameForInventory(world->getItemType(itemName), itemName);
        //        if(fileName.size() > 0){
        //            addSpriteToButton(fileName, btn, Button::TextureResType::LOCAL);
        //        }else{
        //            addSpriteToButton(world->getItemSpriteName(world->getItemType(itemName), itemName), btn, Button::TextureResType::PLIST);
        //        }
        addSpriteToButton(itemName, btn);
    }
    
    selectSamShopItem(firstPopup->getChildByName(firstItemName));
}
void EditorHud::showAlchyShop(){
    world->isInEvent = false;
    world->pauseLayer();
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
void EditorHud::updateAlchyShop(){
    std::string weaponName = "dagger";
    int lastWeaponPrice = 0;
    int weaponPrice = 0;
    int weaponIndex = 0;
    std::string combiningWeaponName = UDGetStr(KEY_ALCHY_COMBINING_ITEM_NAME, "");
    if(combiningWeaponName.size() != 0){
        weaponName = combiningWeaponName;
    }else{
        for (int i = 0; i < inventory->getMaxSlotCount(); i++) {
            if(world->getItemType(inventory->getItemName(i)) == ITEM_TYPE_WEAPON){
                if((weaponPrice = Value(world->getItemStat(ITEM_TYPE_WEAPON, inventory->getItemName(i), "price")).asInt()) > lastWeaponPrice){
                    weaponName = inventory->getItemName(i);
                    lastWeaponPrice = weaponPrice;
                    weaponIndex = Value(world->getItemStat(ITEM_TYPE_WEAPON, inventory->getItemName(i), "index")).asInt();
                }
            }
        }
    }
    alchyIngredientWeaponName = weaponName;
    Text* lblCoin = (Text*)firstPopup->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    
    
    int nextWeaponIndex = -1;
    std::string nextWeaponName;
    for(auto& keyval : world->weaponStatTable) {
        std::string key = keyval.first;
        ValueMap val = keyval.second.asValueMap(); /* could use Value& if need to modify dict  */
        if(val.at("index").asInt() == weaponIndex + 1){
            nextWeaponIndex = weaponIndex + 1;
            nextWeaponName = key;
            break;
        }
    }
    int ingredientItemCount = inventory->getTotalItemCount(weaponName);
    alchyIngredientNextWeaponName = nextWeaponName;
    Text* lbl = (Text*)alchyShopLayer->getChildByName("lblIngredientName");
    std::string wName = LM->getText(weaponName.c_str());
    lbl->setString(strmake("%s(%d/2)", wName.c_str(), ingredientItemCount));
    ImageView* img;
    for (int i = 0; i < 2; i++) {
        img = (ImageView*)firstPopup->getChildByName(strmake("imgIngredient%d", i));
        img->removeChildByName("icon");
        Sprite* spt = getIconSprite(weaponName);
        spt->setPosition(img->getContentSize()/2);
        img->addChild(spt);
        if(ingredientItemCount < i + 1 && combiningWeaponName.size() == 0){
            spt->getChildByName("sprite")->setOpacity(100);
        }
    }
    int fee;
    if(nextWeaponIndex < 0){
        lbl = (Text*)firstPopup->getChildByName("lblDescription");
        lbl->setString(LM->getText("no next weapon"));
    }else{
        alchyNextWeaponLevel = nextWeaponIndex;
        lbl = (Text*)firstPopup->getChildByName("lblResultName");
        lbl->setString(LM->getText(nextWeaponName.c_str()));
        
        img = (ImageView*)firstPopup->getChildByName("imgResult");
        img->removeChildByName("icon");
        Sprite* spt = getIconSprite(nextWeaponName);
        spt->setPosition(img->getContentSize()/2);
        img->addChild(spt);
        Text* text = (Text*)firstPopup->getChildByName("lblFee");
        fee = Value(world->getItemStat(nextWeaponName, "price")).asInt();
        text->setString(strmake("%d", fee));
        
        Text* txt = (Text*)firstPopup->getChildByName("lblFeeTitle");
        Node* sptCoin = firstPopup->getChildByName("sptCoinFee");
        GM->alignToCenter(text, sptCoin, 15, txt->getPositionX(), 0);
    }
    
    lbl = (Text*)firstPopup->getChildByName("lblResultDescription");
    isReadyCombineInAlchyShop = false;
    if(ingredientItemCount < 2 && combiningWeaponName.size() == 0){
        LM->setLocalizedString(lbl, "not enough ingredient");
    }else if(nextWeaponIndex < 0){
        LM->setLocalizedString(lbl, "no next weapon");
        firstPopup->getChildByName("sptCoinFee")->setVisible(false);
        LM->setLocalizedString(lbl, "-");
    }else if(GM->getCoin() < fee){
        LM->setLocalizedString(lbl, "not enough money");
    }else{
        std::string startTime = UDGetStr(KEY_ALCHY_COMBINE_START_TIME, "");
        if(startTime.size() == 0){
            LM->setLocalizedString(lbl, "press a to make exchange");
        }else{
            firstPopup->getChildByName("lblFee")->setVisible(false);
            firstPopup->getChildByName("lblFeeTitle")->setVisible(false);
            firstPopup->getChildByName("sptCoinFee")->setVisible(false);
        }
        isReadyCombineInAlchyShop = true;
    }
    lbl = (Text*)firstPopup->getChildByName("lblGemCount");
    lbl->setString(Value(GM->getGem()).asString());
    changeGemToAnimation(alchyShopLayer->getChildByName("sptGem"));
}
int EditorHud::getGemForMinute(int min){
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
int EditorHud::getAlchyWeaponCombineMinutes(int weaponIndex){
    // 0 to ice 6 end 24
    if( weaponIndex < 6){
        return 1 + weaponIndex*5;
    }else if( weaponIndex < 18){
        return (weaponIndex - 5)*60;
    }else{
        return (weaponIndex - 17)*60*24;
    }
}

void EditorHud::selectSamShopItem(Ref* ref){
    if(ref == nullptr) return;
    Button* btn = (Button*)ref;
    if(btn->getName().size() <= 0){
        return;
    }
    Node* selectBox = firstPopup->getChildByName("sptSelect");
    selectedSamShopItem = btn;
    std::string itemName = btn->getName();
    samShopResultStoneName = itemName;
    std::string name = LM->getText(btn->getName().c_str());
    samShopStoneLevel = 0;
    if(itemName.compare("stone") == 0){
        samShopIngredientName = "wood";
    }else if(itemName.compare("iron") == 0){
        samShopIngredientName = "stone";
        samShopStoneLevel = 1;
    }else if(itemName.compare("copper") == 0){
        samShopIngredientName = "iron";
        samShopStoneLevel = 2;
    }else if(itemName.compare("gold") == 0){
        samShopIngredientName = "copper";
        samShopStoneLevel = 3;
    }else if(itemName.compare("crystal") == 0){
        samShopIngredientName = "gold";
        samShopStoneLevel = 4;
    }else if(itemName.compare("diamond") == 0){
        samShopIngredientName = "crystal";
        samShopStoneLevel = 5;
    }
    Label* lbl;
    
    if (selectBox->getPosition() != btn->getPosition()) {
        selectBox->setPosition(btn->getPosition());
        
        std::string desc;
        if(world->getItemType(btn->getName()) == ITEM_TYPE_ETC &&  world->itemStatTable[btn->getName()].asValueMap().at("ability").asInt() > 0){
            desc = strmake(LM->getText(strmake("%s desc", btn->getName().c_str()).c_str()).c_str(), world->itemStatTable[btn->getName()].asValueMap().at("ability").asInt());
        }else{
            desc = LM->getText(strmake("%s desc", btn->getName().c_str()).c_str());
        }
        
        std::string str = strmake("[%s]\n%s", name.c_str(), desc.c_str());
        lbl = (Label*)firstPopup->getChildByName("lblDescription");
        lbl->setString(str);
        selectBox->setScale(6.3);
        
    }else{
        
        int ingredientCount = inventory->getTotalItemCount(samShopIngredientName);
        ingredientCount = (ingredientCount/samShopExchangeRate)*samShopExchangeRate;
        int fee = 10 + pow(5, samShopStoneLevel);
        int totalFee = (ingredientCount/samShopExchangeRate)*fee;
        bool isReadyToExchange = false;
        if(samShopExchangeRate > ingredientCount){
            
        }else if(GM->getCoin() < totalFee){
            
        }else{
            isReadyToExchange = true;
        }
        if(isReadyToExchange){
            showABCDialog(LM->getText("select action"), LM->getText("cancel"), strmake("%s(%s %d%s)",LM->getText("exchange").c_str(), LM->getText("fee").c_str(), totalFee, LM->getText("bug").c_str()), "");
        }else{
            firstPopup->getChildByName("lblResultDescription")->runAction(Blink::create(0.4f, 2));
            
        }
    }
    updateSamShop();
}
void EditorHud::updateSamShop(){
    std::string fileName;
    std::string itemName = selectedSamShopItem->getName();
    std::string name = LM->getText(itemName.c_str());
    
    Text* lblCoin = (Text*)firstPopup->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    
    Label* lbl = (Label*)firstPopup->getChildByName("lblIngredient");
    lbl->setString(LM->getText(samShopIngredientName.c_str()));
    
    lbl = (Label*)firstPopup->getChildByName("lblResult");
    lbl->setString(name);
    
    ImageView* img = (ImageView*)firstPopup->getChildByName("imgIngredient");
    img->removeChildByName("icon");
    Sprite* spt = Sprite::createWithSpriteFrameName(world->getItemStat(ITEM_TYPE_ETC, samShopIngredientName, "sprite"));
    img->addChild(spt);
    spt->setName("icon");
    spt->setPosition(img->getContentSize()/2);
    
    img = (ImageView*)firstPopup->getChildByName("imgResult");
    img->removeChildByName("icon");
    spt = Sprite::createWithSpriteFrameName(world->getItemStat(ITEM_TYPE_ETC, itemName, "sprite"));
    img->addChild(spt);
    spt->setName("icon");
    spt->setPosition(img->getContentSize()/2);
    
    int ingredientCount = inventory->getTotalItemCount(samShopIngredientName);
    ingredientCount = (ingredientCount/samShopExchangeRate)*samShopExchangeRate;
    Text* text = (Text*)firstPopup->getChildByName("lblIngredientCount");
    text->setString(strmake("x%d", ingredientCount));
    
    text = (Text*)firstPopup->getChildByName("lblResultCount");
    text->setString(strmake("x%d", ingredientCount/samShopExchangeRate));
    
    text = (Text*)firstPopup->getChildByName("lblFee");
    int fee = 10 + pow(5, samShopStoneLevel);
    int totalFee = (ingredientCount/samShopExchangeRate)*fee;
    text->setString(strmake("%d", totalFee));
    //    GM->alignToCenter(text, firstPopup->getChildByName("sptCoinFee"), 5, firstPopup->getChildByName("lblFeeTitle")->getPositionX(), 0);
    
    lbl = (Label*)firstPopup->getChildByName("lblResultDescription");
    bool isReadyToExchange = false;
    if(samShopExchangeRate > ingredientCount){
        LM->setLocalizedString(lbl, "not enough ingredient");
    }else if(GM->getCoin() < totalFee){
        LM->setLocalizedString(lbl, "not enough money");
    }else{
        LM->setLocalizedString(lbl, "press a to make exchange");
        isReadyToExchange = true;
    }
}
void EditorHud::showDoctorShop(){
    world->isInEvent = false;
    world->pauseLayer();
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
void EditorHud::updateDoctorShop(){
    std::string tier = UDGetStr(strmake("hero_tier_%d", 0).c_str(), "0");
    int nextTier = Value(tier).asInt() + 1;
    std::string maxLevel = world->getEvolutionStat(tier, "max level");
    std::string fee = world->getEvolutionStat(tier, "bug");
    std::string ingredient0 = world->getEvolutionStat(tier, "ingredient 0");
    std::string ingredient0Count = world->getEvolutionStat(tier, "ingredient 0 count");
    std::string ingredient1 = world->getEvolutionStat(tier, "ingredient 1");
    std::string ingredient1Count = world->getEvolutionStat(tier, "ingredient 1 count");
    std::string ingredient2 = world->getEvolutionStat(tier, "ingredient 2");
    std::string ingredient2Count = world->getEvolutionStat(tier, "ingredient 2 count");
    std::string ingredient3 = world->getEvolutionStat(tier, "ingredient 3");
    std::string ingredient3Count = world->getEvolutionStat(tier, "ingredient 3 count");
    
    Text* lblCoin = (Text*)firstPopup->getChildByName("lblCoin");
    lblCoin->setString(Value(GM->getCoin()).asString());
    
    lblCoin = (Text*)firstPopup->getChildByName("lblFee");
    lblCoin->setString(fee);
    
    Button* btn;
    for (int i = 0; i < 4; i++) {
        btn = (Button*)firstPopup->getChildByName(strmake("btnSlot%d", i));
        btn->removeAllChildren();
        std::string str = world->getEvolutionStat(tier, strmake("ingredient %d", i));
        std::string file = world->getItemStat(world->getItemType(str), str, "sprite");
        //        itemStatTable[str].asValueMap().at("sprite").asString();
        addSpriteToButton(file, btn, Button::TextureResType::PLIST);
        
        Label* lbl = Label::createWithSystemFont(world->getEvolutionStat(tier, strmake("ingredient %d count", i)), "visitor1.ttf", 80);
        lbl->setTextColor(Color4B::BLACK);
        lbl->setName("lblCount");
        btn->addChild(lbl, 1);
        lbl->setAnchorPoint(Vec2(1, 0));
        lbl->enableOutline(Color4B::WHITE, 8);
        lbl->setScale(0.08);
        lbl->setPosition(Vec2(btn->getContentSize().width - 2, 1));
    }
    bool feeEnough = GM->getCoin() >= Value(fee).asInt();
    bool ingredient0Enough = inventory->getTotalItemCount(ingredient0) >= Value(ingredient0Count).asInt();
    bool ingredient1Enough = inventory->getTotalItemCount(ingredient1) >= Value(ingredient1Count).asInt();
    bool ingredient2Enough = inventory->getTotalItemCount(ingredient2) >= Value(ingredient2Count).asInt();
    bool ingredient3Enough = inventory->getTotalItemCount(ingredient3) >= Value(ingredient3Count).asInt();
    bool isMaxLevel = world->getHeroLevel(0) >= Value(maxLevel).asInt();
    
    Text* lbl = (Text*)firstPopup->getChildByName("lblDescription");
    lbl->setTextVerticalAlignment(TextVAlignment::CENTER);
    lbl->ignoreContentAdaptWithSize(false);
    lbl->setContentSize(cocos2d::Size(635, 68));
    if(!feeEnough){
        lbl->setString(LM->getText("not enough money"));
    }else if(!isMaxLevel){
        lbl->setString(LM->getText("need to reach max level"));
    }else if(!ingredient0Enough || !ingredient1Enough || !ingredient2Enough || !ingredient3Enough){
        lbl->setString(LM->getText("not enough ingredient"));
    }else{
        lbl->setString(LM->getText("ready to evolution"));
    }
    
    int slot = 0;
    lbl = (Text*)firstPopup->getChildByName("lblLevel");
    lbl->setString(strmake("LV %d/%s->%d/%s", world->getHeroLevel(0), maxLevel.c_str(), world->getHeroLevel(0), world->getEvolutionStat(Value(nextTier).asString(), "max level").c_str()));
    
    
}
void EditorHud::evolving(float dt){
    evolvingPercent++;
    if(evolvingPercent >= 100){
        std::string tier = UDGetStr(strmake("hero_tier_%d", 0).c_str(), "0");
        int nextTier = Value(tier).asInt() + 1;
        UDSetStr(strmake("hero_tier_%d", 0).c_str(), Value(nextTier).asString());
        
        std::string fee = world->getEvolutionStat(tier, "bug");
        std::string ingredient0 = world->getEvolutionStat(tier, "ingredient 0");
        std::string ingredient0Count = world->getEvolutionStat(tier, "ingredient 0 count");
        std::string ingredient1 = world->getEvolutionStat(tier, "ingredient 1");
        std::string ingredient1Count = world->getEvolutionStat(tier, "ingredient 1 count");
        std::string ingredient2 = world->getEvolutionStat(tier, "ingredient 2");
        std::string ingredient2Count = world->getEvolutionStat(tier, "ingredient 2 count");
        std::string ingredient3 = world->getEvolutionStat(tier, "ingredient 3");
        std::string ingredient3Count = world->getEvolutionStat(tier, "ingredient 3 count");
        GM->addCoin(-Value(fee).asInt());
        inventory->removeItem(ingredient0, Value(ingredient0Count).asInt());
        inventory->removeItem(ingredient1, Value(ingredient1Count).asInt());
        inventory->removeItem(ingredient2, Value(ingredient2Count).asInt());
        inventory->removeItem(ingredient3, Value(ingredient3Count).asInt());
        
        
        closePopup();
        world->resumeLayer();
        this->unschedule(schedule_selector(EditorHud::evolving));
        return;
    }
    Text* lbl = (Text*)firstPopup->getChildByName("lblDescription");
    lbl->setString(strmake("%d%%", evolvingPercent));
}
void EditorHud::selectDoctorShopItem(Ref* ref){
    if(ref == nullptr) return;
    Button* btn = (Button*)ref;
    selectedDoctorShopItem = btn;
    std::string tier = UDGetStr(strmake("hero_tier_%d", 0).c_str(), "0");
    std::string ingredient0 = world->getEvolutionStat(tier, strmake("ingredient %d", btn->getTag()));
    doctorShopLayer->getChildByName("sptSelect")->setPosition(btn->getPosition());
    Text* lbl = (Text*)doctorShopLayer->getChildByName("lblItemName");
    lbl->setString(LM->getText(ingredient0.c_str()));
}
void EditorHud::showCredit(){
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
    this->runAction(Sequence::create(DelayTime::create(3*6 + 1), CallFunc::create(CC_CALLBACK_0(EditorHud::showBIAndReplaceScene, this)), NULL));
}
void EditorHud::showPeterInsuranceShop(){
    if(firstPopup != nullptr){
        onBClick();
    }
    world->isInEvent = false;
    world->pauseLayer();
    Node* layer = CSLoader::createNode("PeterShop.csb");
    this->addChild(layer, 5);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    setAsPopup(layer);
    ddiyong(layer);
    peterShopLayer = layer;
    
    Node* imgBack = layer->getChildByName("imgBack");
    ropeAround(imgBack);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "peter insurance");
    
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "insurance desc");
    GM->makeLabelShrink(lbl);
    
    lbl = (Text*)layer->getChildByName("lblPlanTitle");
    LM->setLocalizedString(lbl, "insurance plans");
    
    for(int i = 0; i < 3; i++){
        lbl = (Text*)layer->getChildByName(strmake("lblOption%d", i));
        if(i == 2){
            std::string price = "$9.99"; // test
            lbl->setString(strmake(LM->getText(strmake("insurance plan%d", i).c_str()).c_str(), price.c_str()));
        }else{
            LM->setLocalizedString(lbl, strmake("insurance plan%d", i));
        }
        
        lbl->addClickEventListener(CC_CALLBACK_1(EditorHud::onChoosedInsurancePlan, this));
    }
    lbl = (Text*)firstPopup->getChildByName("lblGemCount");
    lbl->setString(Value(GM->getGem()).asString());
    changeGemToAnimation(peterShopLayer->getChildByName("sptGem"));
    
    Sprite* sptCharacter = (Sprite*)peterShopLayer->getChildByName("sptCharacter");
    sptCharacter->setSpriteFrame("peter0.png");
    GM->runAnimation(sptCharacter, "peter", true);
    sptCharacter->setScale(4);
    
    Node* cursor = peterShopLayer->getChildByName("sptCursor");
    cursor->setPositionX(peterShopLayer->getChildByName("lblOption0")->getPositionX() - 50);
    cursor->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Vec2(-5, 0)), MoveBy::create(0.2f, Vec2(5, 0)), NULL)));
    
    lbl = (Text*)layer->getChildByName("lblTip");
    LM->setLocalizedString(lbl, "a to select b to cancel");
    
    selectPeterInsurancePlan(0);
}
void EditorHud::onChoosedInsurancePlan(Ref* ref){
    if(ref == nullptr) return;
    int index = ((Text*)ref)->getTag();
    selectPeterInsurancePlan(index);
}
void EditorHud::selectPeterInsurancePlan(int index){
    Node* newlySelectedPlan = peterShopLayer->getChildByName(strmake("lblOption%d", index));
    if(newlySelectedPlan == selectedPeterItem){
        if(index == 1 && GM->getGem() < 5){
            showInstanceMessage(LM->getText("not enough gem"));
            
            return;
        }
        newlySelectedPlan->runAction(Sequence::create(Blink::create(0.5, 2), CallFuncN::create(CC_CALLBACK_1(EditorHud::onConfirmInsurancePlan, this)), nullptr));
    }else{
        Node* cursor = peterShopLayer->getChildByName("sptCursor");
        cursor->setPositionY(newlySelectedPlan->getPosition().y);
    }
    selectedPeterItem = newlySelectedPlan;
}
void EditorHud::onConfirmInsurancePlan(Ref* ref){
    Node* lbl = (Node*)ref;
    if(lbl->getTag() == 0){
        
    }else if(lbl->getTag() == 1){
        GM->addGem(-5);
        
    }else if(lbl->getTag() == 2){
        // purchase
    }
}
void EditorHud::showPVP(){
    
}
void EditorHud::oneSecUpdate(float dt){
    
}


void EditorHud::onPotSoulMoveDone(){
    Node* counter = this->getChildByName("potCounter");
    Text* lbl = (Text*)counter->getChildByName("lblCount");
    int potCount = UDGetInt(KEY_POT_SOUL_COUNT, 0);
    lbl->setString(Value(potCount).asString());
    lbl->runAction(Sequence::create(ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1), NULL));
}



void EditorHud::onReviveByGem(){
    int gemPrice = Value(((Text*)deadPopup->getChildByName("lblGemPrice"))->getString()).asInt();
    if(GM->getGem() < gemPrice){
        showInstanceMessage(LM->getText("not enough gem"));
        
    }else{
        GM->addGem(-gemPrice);
        world->isGameOver = false;
        world->isInEvent = false;
        world->resumeLayer();
        world->immortalForSec(10);
        this->unschedule(schedule_selector(EditorHud::updateResultPopup));
        world->resumeLayer();
        closePopup();
        deadPopup = nullptr;
    }
}
void EditorHud::onReviveByVideo(){
    GM->showVideo(VIDEO_GAME_OVER);
    deadPopup->getChildByName("btnClose")->setVisible(true);
    this->unschedule(schedule_selector(EditorHud::updateResultPopup));
}
void EditorHud::onReviveByDead(){
    float exp = world->getHeroExp(0);
    int level = world->getHeroLevel(0);
    float expToReduce = world->getMaxExp(level)*0.3f;
    if(expToReduce > 3000){
        expToReduce = 3000;
    }
    exp -= expToReduce;
    if(exp < 0){
        exp = 0;
    }
    GM->isReviving = true;
    world->setHeroExp(0, exp);
    HUD->setExp(level, world->getHeroExp(0), world->getMaxExp(level));
    
    std::string checkPoint = UDGetStr(KEY_LAST_CHECK_POINT, "0_4_2");
    UDSetStr(KEY_LAST_SAVE_POINT, checkPoint.c_str());
    showBIAndReplaceScene();
}
void EditorHud::showWinPopup(bool win){
    Node* popup = Node::create();
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
    
    std::string strResult = "MISSION CLEAR";
    if (!win) {
        strResult = "MISSION FAILED";
    }
    //    Label* lbl = LM->getLocalizedLabel(strResult.c_str(), Color4B::BLACK, 70);
    Node* title = PPLabel::create(strResult, 70, DARK_GRAY_3B, false, false, TextHAlignment::CENTER, false);
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
            customLabel = PPLabel::create("Time", fontSize, DARK_GRAY_3B, false,false, TextHAlignment::LEFT, false);
            customLabel->setName("lblTimeTitle");
            spt = Sprite::create("iconTime.png");
            spt->setName("sptTime");
            //            lbl = LM->getLocalizedLabel("Time", Color4B::BLACK, fontSize);
            //            lbl->setName("lblTimeTitle");
        }else if (i == 1) {
            customLabel = PPLabel::create("Gold (Used/Earnd)", fontSize, DARK_GRAY_3B, false,false, TextHAlignment::LEFT, false);
            customLabel->setName("lblGoldTitle");
            spt = Sprite::create("iconGold.png");
            spt->setName("sptGold");
            //            lbl = LM->getLocalizedLabel("Gold (Used/Earnd)", Color4B::BLACK, fontSize);
            //            lbl->setName("lblGoldTitle");
        }else if (i == 2) {
            customLabel = PPLabel::create("Lumber (Used/Earnd)", fontSize, DARK_GRAY_3B, false,false, TextHAlignment::LEFT, false);
            customLabel->setName("lblLumberTitle");
            spt = Sprite::create("iconLumber.png");
            spt->setName("sptLumber");
            //            lbl = LM->getLocalizedLabel("Lumber (Used/Earnd)", Color4B::BLACK, fontSize);
            //            lbl->setName("lblLumberTitle");
        }else if (i == 3) {
            customLabel = PPLabel::create("Produced Unit", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::LEFT, false);
            customLabel->setName("lblProduceTitle");
            spt = Sprite::create("iconPopulation.png");
            spt->setName("sptPopulation");
            //            lbl = LM->getLocalizedLabel("Produced Unit", Color4B::BLACK, fontSize);
            //            lbl->setName("lblProduceTitle");
        }else if (i == 4) {
            customLabel = PPLabel::create("Killed Unit", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::LEFT, false);
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
            customLabel = PPLabel::create("00:00:00", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::RIGHT, false);
            customLabel->setName("lblTime");
            //            lbl = LM->getLocalizedLabel("00:00:00", Color4B::BLACK, fontSize);
            //            lbl->setName("lblTime");
        }else if (i == 1) {
            customLabel = PPLabel::create("0/0", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::RIGHT, false);
            customLabel->setName("lblGold");
            //            lbl = LM->getLocalizedLabel("0/0", Color4B::BLACK, fontSize);
            //            lbl->setName("lblGold");
        }else if (i == 2) {
            customLabel = PPLabel::create("0/0", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::RIGHT, false);
            customLabel->setName("lblLumber");
            //            lbl = LM->getLocalizedLabel("0/0", Color4B::BLACK, fontSize);
            //            lbl->setName("lblLumber");
        }else if (i == 3) {
            customLabel = PPLabel::create("0", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::RIGHT, false);
            customLabel->setName("lblProduce");
            //            lbl = LM->getLocalizedLabel("0", Color4B::BLACK, fontSize);
            //            lbl->setName("lblProduce");
        }else if (i == 4) {
            customLabel = PPLabel::create("0", fontSize, DARK_GRAY_3B, false, false, TextHAlignment::RIGHT, false);
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
    btn->addClickEventListener(CC_CALLBACK_1(EditorHud::onOkFromWinPopup, this));
    btn->setPosition(Vec2(size.width/2 + 500, size.height/2 - 450));
    //    btn->setColor(Color3B(244, 236, 5));
    addLabelToButton("OK", btn, false, DARK_GRAY_3B);
    
    spt = Sprite::create("redFlag.png");
    spt->setPosition(Vec2(size.width/2, size.height/2 + 850/2 + 110 - spt->getContentSize().height/2));
    spt->setAnchorPoint(Vec2(0.18, 0.1));
    popup->addChild(spt);
    
    if(!win){
        spt->runAction(Sequence::create(DelayTime::create(1.5f), EaseIn::create(RotateBy::create(0.5f, -90), 2), NULL));
    }
    
    this->schedule(schedule_selector(EditorHud::updateResultPopup), 0.1f);
}
void EditorHud::updateResultPopup(float dt){
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
    if(resultTime > world->gameTimer){
        resultTime = world->gameTimer;
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
    if (resultUsedGold > world->totalUsedGold) {
        resultUsedGold = world->totalUsedGold;
    }
    if (resultEarndGold > world->totalEarnedGold) {
        resultEarndGold = world->totalEarnedGold;
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
    if (resultUsedLumber > world->totalUsedLumber) {
        resultUsedLumber = world->totalUsedLumber;
    }
    if (resultEarndLumber > world->totalEarnedLumber) {
        resultEarndLumber = world->totalEarnedLumber;
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
    if(resultProduce > world->totalProducedUnit){
        resultProduce = world->totalProducedUnit;
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
    if(resultKill > world->totalKillUnit){
        resultKill = world->totalKillUnit;
        lbl->setString(Value(resultKill).asString());
    }else{
        lbl->setString(Value(resultKill).asString());
        return;
    }
    
    resultPopup->getChildByName("btnOk")->setVisible(true);
    
    this->unschedule(schedule_selector(EditorHud::updateResultPopup));
}

void EditorHud::onOkFromWinPopup(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    btn->setEnabled(false);
    
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    auto scene = Scene::create();
    Title* title = Title::create();
    scene->addChild(title);
    if (!world->isGameOver) {
        int stage = world->stageIndex;
        int clearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
        if(clearStage < stage){
            UDSetInt(KEY_LAST_CLEAR_STAGE, stage);
        }
        stage++;
        if (stage < 13) { // test  5 for google indie festival 13(12 + bonus) for launch
            Node* temp = Node::create();
            temp->setTag(stage);
            title->onStageClick(temp);
        }else{
            // show ending
        }
    }
    this->removeListener();
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void EditorHud::arrangeMenu(cocos2d::Vec2 pos){
    for (int i = 0; i < 6; i++) {
        Button* btn = (Button*)world->getChildByName(strmake("btnMenu%d", i));
        int radius = 270;
        btn->setPosition(pos + Vec2(120, -100));
        btn->stopAllActions();
        btn->setOpacity(0);
        btn->runAction(FadeIn::create(0.16f));
        float angle = i*60 - 120;
        btn->runAction(EaseOut::create(MoveBy::create(0.16f, Vec2(cos(angle*3.14f/-180)*radius, sin(angle*3.14f/-180)*radius)), 2));
    }
    
}
void EditorHud::showDetailPopup(cocos2d::Vec2 pos){
    Vec2 coordinate = world->getCoordinateFromPosition(pos);
    cocos2d::Rect rect;
    for(auto spt: world->spriteBatch->getChildren()){
        rect = spt->getBoundingBox();
        if(rect.containsPoint(pos)){
            coordinate = Vec2(spt->getTag()%world->mapSizeWidth, spt->getTag()/world->mapSizeWidth);
        }
    }
    
    int clickedBrushType = world->placedArray[(int)coordinate.x][(int)coordinate.y];
    if (clickedBrushType == BRUSH_TREE || clickedBrushType == BRUSH_MINE) {
        return;
    }
    
    Movable* selectedUnit = nullptr;
    for(auto unit: world->spriteBatch->getChildren()){
        if(unit->getTag() == coordinate.x + coordinate.y*world->mapSizeWidth){
            selectedUnit = (Movable*)unit;
            break;
        }
    }
    
    if(selectedUnit != nullptr){
        Layer* layer = Layer::create();
        this->addChild(layer, 100);
        firstPopup = layer;
        
        ImageView* imgBack = ImageView::create("horizontalScrollBackground.png");
        layer->addChild(imgBack);
        imgBack->setScale9Enabled(true);
        imgBack->setContentSize(cocos2d::Size(308, 1170));
        imgBack->runAction(EaseInOut::create(ResizeTo::create(0.3, cocos2d::Size(1598, 1170)), 2));
        imgBack->setAnchorPoint(Vec2(0.5, 0.5));
        imgBack->setPosition(Vec2(size.width/2, size.height/2));
        
        PPLabel* lbl = PPLabel::create("This is", 30, DARK_GRAY_3B, false, false, TextHAlignment::LEFT, false);
        layer->addChild(lbl);
        lbl->setPosition(Vec2(size.width/2 - 628, size.height/2 + 350));
        
        Button* btn = Button::create("btnBox.png");
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 - 328, size.height/2 + 350));
        btn->setScale9Enabled(true);
        btn->setContentSize(cocos2d::Size(308, 120));
        btn->setName("btnPia");
        btn->setTag(PIA_HEROES);
        addLabelToButton(getPIAName(btn->getTag()), btn, false, DARK_GRAY_3B);
        btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onChangePiaClick, this));
        
        ScrollView* sv = ScrollView::create();
        sv->setContentSize(cocos2d::Size(1300, 650));
        layer->addChild(sv);
        sv->setPosition(Vec2(size.width/2 - sv->getContentSize().width/2, size.height/2 - 400));
        
        float itemHeight = 120;
        float gapY = 10;
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        selectedUnit->eventInfoList.push_back(new EventInfo(SUBJECT_TIME, ACTION_AFTER, OBJECT_AMOUNT, SUBJECT_BUILDING, ACTION_PRODUCE, OBJECT_UNIT));
        int itemCount = selectedUnit->eventInfoList.size();
        sv->setInnerContainerSize(cocos2d::Size(1300, itemHeight*itemCount));
        for (int i = 0; i < itemCount; i++) {
            Node* row = Node::create();
            
            btn = Button::create("btnBox.png");
            sv->addChild(btn);
            if(itemCount > 5){
                btn->setPosition(Vec2(228, itemHeight*(itemCount - i)));
            }else{
                btn->setPosition(Vec2(228, sv->getInnerContainerSize().height - itemHeight*(i) - gapY));
            }
            btn->setScale9Enabled(true);
            btn->setAnchorPoint(Vec2(0, 1));
            btn->setContentSize(cocos2d::Size(308, 120));
            btn->setName("ifSubject");
            addLabelToButton("subject", btn, false, DARK_GRAY_3B);
            btn->addClickEventListener(CC_CALLBACK_1(EditorHud::onIfSubjectClick, this));
        }
        
        btn = Button::create("btnBox.png");
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 + 450, size.height/2 + 350));
        btn->setScale9Enabled(true);
        btn->setContentSize(cocos2d::Size(338, 120));
        btn->setName("btnAddEvent");
        addLabelToButton("Add Event", btn, false, DARK_GRAY_3B);
        btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onAddEventClick, this));
        
        btn = Button::create("btnBox.png");
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 + 230, size.height/2 - 500));
        btn->setScale9Enabled(true);
        btn->setContentSize(cocos2d::Size(308, 120));
        btn->setName("btnAddEvent");
        addLabelToButton("OK", btn, false, DARK_GRAY_3B);
        btn->addClickEventListener(CC_CALLBACK_0(EditorHud::onOkOnDetailPopup, this));
        
        btn = Button::create("btnBox.png");
        layer->addChild(btn);
        btn->setPosition(Vec2(size.width/2 + 530, size.height/2 - 500));
        btn->setScale9Enabled(true);
        btn->setContentSize(cocos2d::Size(308, 120));
        btn->setName("btnAddEvent");
        addLabelToButton("Cancel", btn, false, DARK_GRAY_3B);
        btn->addClickEventListener(CC_CALLBACK_0(EditorHud::closePopup, this));
        
        if(selectedUnit->isBuilding){
            if (clickedBrushType == BRUSH_START_POINT) {
                
            }
        }else{
            
        }
        
        for(auto child:layer->getChildren()){
            child->setOpacity(0);
            child->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.3f), NULL));
            
            for(auto c:child->getChildren()){
                c->setOpacity(0);
                c->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.3f), NULL));
            }
        }
    }
    
}

void EditorHud::onIfSubjectClick(Ref* ref){
    
}
void EditorHud::onAddEventClick(){
    
}
void EditorHud::onChangePiaClick(){
    
}
void EditorHud::onOkOnDetailPopup(){
    
}
std::string EditorHud::getPIAName(int pia){
    if(pia == PIA_HEROES){
        return "Hero Side";
    }else if(pia == PIA_ALLY){
        return "Ally";
    }else if(pia == PIA_TO_RESCUE){
        return "To Rescure";
    }else if(pia == PIA_ENEMY){
        return "Enemy Side";
    }
}
void EditorHud::doBrush(cocos2d::Vec2 pos){
    Vec2 coordinate = world->getCoordinateFromPosition(pos);
    if(selectedMode == MODE_PENCIL){
        int unitType = world->getPlacedUnit(coordinate);
        if(unitType != selectedBrush){
            world->brushTile(selectedBrush, coordinate);
        }
    }else if(selectedMode == MODE_ERASER){
        cocos2d::Rect rect;
        bool somethingClicked = false;
        for(auto spt: world->spriteBatch->getChildren()){
            rect = spt->getBoundingBox();
            if(rect.containsPoint(pos)){
                somethingClicked = true;
                coordinate = Vec2(spt->getTag()%world->mapSizeWidth, spt->getTag()/world->mapSizeWidth);
                world->eraseTile(coordinate);
            }
        }
        if(!somethingClicked){
            world->eraseTile(coordinate);
        }
    }
}
/*
 
 Pause Menu

 Unit Actions
 -move to point
 -attack
 
 Condition
 -0,1,2,3,4,5 min
 
 Unit Menu
 -which side
 -(action) on (condition)
 

 Start Vec2 Menu
 -clear condition
    -gold over (amount)
    -lumber over (amount)
    -ellimination
    -survive for 20 min
    -none
 -game over condition
    -gold under (amount)
    -lumber under (amount)
    -ellimination
    -none
 -init gold and lumber -> action to add gold on time 0 min
 
 Event Vec2
 -if (who) arrived then (system event) hanppens.
 
 Start Vec2
 -on (condition) then (system event) happens
 
 Condition
 -arrive at (where)
 -time elapsed (how long)
 -(unit) is killed
 
 System Actions
 -Game Win
 -Game Over
 -add gold
 -add lumber
 -produce (unit)x(count) on (where)
 
 
*/
