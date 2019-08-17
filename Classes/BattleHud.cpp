//
//  BattleHud.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 22/03/2019.
//
#include "Title.h"
#include "FindMatch.h"
#include "BattleHud.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "GameSharing.h"
#include "BuggyServerManager.h"
bool BattleHud::init()
{
    if ( !Layer::init())
    {
        return false;
    }
    
//    UDSetStr("newminebutton9_15", ""); // test
//    UDSetStr(KEY_SAVED_ID, "2"); // test
//    BSM->requestedID = UDGetStr(KEY_SAVED_ID, "-1"); // test
    isLoadAllUserDataRequested = true;
    isLastAccessTimeRequested = true;
    BSM->getAllUserData(); // test
    size = Director::getInstance()->getWinSize();
    
    BHUD = this;
    GM->setHudLayer(nullptr);
    GM->titleLayer = nullptr;
    hudLayer = CSLoader::createNodeWithVisibleSize("Battle.csb");
    this->addChild(hudLayer, 200);
//    hudLayer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    ndTopBar = hudLayer->getChildByName("ndTopBar");
    ndTopBar->retain();
    
    Button* btn = (Button*)hudLayer->getChildByName("btnBattle");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onBattleClick, this));
    setLocalizedStringForChildLbl(btn, "battle");
    btn = (Button*)hudLayer->getChildByName("btnVideoStore");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onVideoStoreClick, this));
    setLocalizedStringForChildLbl(btn, "shop");
    btn = (Button*)hudLayer->getChildByName("btnShop");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onShopClick, this));
    setLocalizedStringForChildLbl(btn, "shop");
    btn = (Button*)hudLayer->getChildByName("btnTrainBR");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onTrainBRClick, this));
    GM->makeItSiluk(btn->getChildByName("imgRedDot"));
    float dur = 0.5f;
    btn->getChildByName("imgRedDot")->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(dur, Vec2(-btn->getContentSize().width, 0)), 2), EaseBounceOut::create(MoveBy::create(dur*2, Vec2(btn->getContentSize().width, 0))), DelayTime::create(2), NULL)));
    setLocalizedStringForChildLbl(btn, "train");
    btn = (Button*)hudLayer->getChildByName("btnSpecialOffer");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onSpecialOfferClick, this));
    setLocalizedStringForChildLbl(btn, "special offer");
    if(UDGetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, false)){ // test
        btn->setVisible(false); // test 
    }
    btn = (Button*)hudLayer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onOkClick, this));
    btn = (Button*)hudLayer->getChildByName("btnCancel");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onCancelClick, this));
    btn = (Button*)hudLayer->getChildByName("btnInfo");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onInfoClick, this));
    setLocalizedStringForChildLbl(btn, "info");
    btn = (Button*)hudLayer->getChildByName("btnCutTree");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onCutTreeClick, this));
    setLocalizedStringForChildLbl(btn, "cut");
    btnComplete = (Button*)hudLayer->getChildByName("btnComplete");
    btnComplete->addClickEventListener(CC_CALLBACK_0(BattleHud::onCompleteClick, this));
    setLocalizedStringForChildLbl(btnComplete, "complete");
    btn = (Button*)hudLayer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onBackClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnPopulation");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onPopulationClick, this));
    btn = (Button*)hudLayer->getChildByName("btnUpgrade");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onUpgradeClick, this));
    setLocalizedStringForChildLbl(btn, "upgrade");
    btn = (Button*)hudLayer->getChildByName("btnMission");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onMissionClick, this));
    setLocalizedStringForChildLbl(btn, "daily mission");
    btn = (Button*)hudLayer->getChildByName("btnTrain");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onTrainClick, this));
    setLocalizedStringForChildLbl(btn, "training");
    btn = (Button*)hudLayer->getChildByName("btnDefence");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onDefenceRecordClick, this));
    setLocalizedStringForChildLbl(btn, "log");
    btn = (Button*)hudLayer->getChildByName("btnDailyReward");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onDailyRewardClick, this));
    setLocalizedStringForChildLbl(btn, "daily");
    
    
    lblGemInHud = (Text*)ndTopBar->getChildByName("lblGem");
    lblGemInHud->setString(Value(GM->getGem()).asString());
    lastGemUpdated = GM->getGem();
    lblGoldInHud = (Text*)ndTopBar->getChildByName("lblGold");
    lblGoldInHud->setString(Value(GM->getCoin()).asString());
    lastGoldUpdated = GM->getCoin() - 1;
    lblTreeInHud = (Text*)ndTopBar->getChildByName("lblTree");
    lblTreeInHud->setString(Value(GM->getTree()).asString());
    lastTreeUpdated = GM->getTree() - 1;
    
    btn = (Button*)ndTopBar->getChildByName("btnGem");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onMoreGemInLobbyClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnGold");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onMoreGoldClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnTree");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onMoreTreeClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnWorker");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onWorkerClick, this));
    ImageView* img = (ImageView*)ndTopBar->getChildByName("imgWorkerIcon");
    Sprite* spt = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
    img->getParent()->addChild(spt);
    spt->setScale(0.6f);
    spt->setPosition(img->getPosition());
    img->removeFromParent();
    
    this->schedule(schedule_selector(BattleHud::update), 0.1f);
    this->schedule(schedule_selector(BattleHud::updateUI), 0.1f);
//    GM->setCoin(10000); // test
//    UDSetInt(KEY_LAST_HIRE_REFRESH_DAY, 0); // test
//    UDSetStr(KEY_SEARCH_STATE, "000"); // test
    showIndicator();
    
    keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(BattleHud::onKeyReleased, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
    
    // test
//    for(int i = 0; i < 3; i++){
//        UDSetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, i).c_str(), false);
//    }
    
    // init done
    return true;
}

int BattleHud::getUnitIndexForVideoStore(int day){
    int dayIndex = day%11;
    if(dayIndex == 0){
        return UNIT_GOBLIN;
    }else if(dayIndex == 1){
        return UNIT_SWORDMAN;
    }else if(dayIndex == 2){
        return UNIT_ARCHER;
    }else if(dayIndex == 3){
        return UNIT_CATAPULT;
    }else if(dayIndex == 4){
        return UNIT_ZOMBIE_SWORDSMAN;
    }else if(dayIndex == 5){
        return UNIT_ZOMBIE_ORC_AXE;
    }else if(dayIndex == 6){
        return UNIT_ORC_AXE;
    }else if(dayIndex == 7){
        return UNIT_ORC_SPEAR;
    }else if(dayIndex == 8){
        return UNIT_TROLL;
    }else if(dayIndex == 9){
        return UNIT_HELICOPTER;
    }else if(dayIndex == 10){
        return UNIT_GOBLIN_BOMB;
    }
    return UNIT_SWORDMAN;
}
void BattleHud::onVideoStoreClick(){
    Node* layer = CSLoader::createNode("VideoStore.csb");
    this->addChild(layer, 4);
    layer->setName("videoStore");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->getChildByName("imgBackground")->setLocalZOrder(-100);
    moveTopBarTo(layer);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->setTag(i);
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onVideoTabClick, this));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(100);
    }
    layer->getChildByName("lblQuestion")->setLocalZOrder(100);
    Text* lbl;
    Node* nd = layer->getChildByName("ndTabRandom");
    for (int i = 0; i < 12; i++) {
        btn = (Button*)nd->getChildByName(strmake("btnGem%d", i));
        btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onVideoRandomClick, this));
        lbl = (Text*)btn->getChildByName("lblGold");
        lbl->setTag(Value(lbl->getString()).asInt());
    }
    
    nd = layer->getChildByName("ndTabUnit");
    for (int i = 0; i < 12; i++) {
        btn = (Button*)nd->getChildByName(strmake("btnGem%d", i));
        btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onVideoUnitClick, this));
        lbl = (Text*)btn->getChildByName("lblGold");
        lbl->setTag(Value(lbl->getString()).asInt());
    }
    ImageView* img = (ImageView*)nd->getChildByName("btnGem11")->getChildByName("imgIconGold");
    int dayIndex = UDGetInt(KEY_VIDEO_STORE_UNIT_DAY, 0);
    int unit = getUnitIndexForVideoStore(dayIndex);
    Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit));
    img->getParent()->addChild(spt);
    spt->setPosition(img->getPosition());
    img->removeFromParent();
    spt->setName("imgIconGold");
    // next
    unit = getUnitIndexForVideoStore(dayIndex + 1);
    spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit));
    img = (ImageView*)nd->getChildByName("btnGem11")->getChildByName("imgNext");
    img->getParent()->addChild(spt);
    spt->setPosition(img->getPosition());
    lbl = (Text*)nd->getChildByName("btnGem11")->getChildByName("lblNext");
    LM->setLocalizedString(lbl, "next");
    img->removeFromParent();
    
    nd = layer->getChildByName("ndTabGem");
    for (int i = 0; i < 12; i++) {
        btn = (Button*)nd->getChildByName(strmake("btnGem%d", i));
        btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onVideoGemClick, this));
        lbl = (Text*)btn->getChildByName("lblGold");
        lbl->setTag(Value(lbl->getString()).asInt());
    }
    
    onVideoTabClick(layer->getChildByName("btnTab0"));
}
void BattleHud::onVideoRandomClick(){
    int nextIndex = UDGetInt(KEY_VIDEO_STORE_RANDOM_INDEX, 0);
    
//    double now = BSM->getCurrentTime();
//    double watchedTime = UDGetDouble(KEY_VIDEO_STORE_RANDOM_WATCHED_TIME, 0);
//    bool isReady = now - watchedTime > videoStoreWaitForNextVideoTime;
    std::string strWatchedTime = UDGetStr(KEY_VIDEO_STORE_RANDOM_WATCHED_TIME, "");
    time_t watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
    bool isReady = difftime(BSM->getCurrentTimeT(), watchedTimeT) > videoStoreWaitForNextVideoTime;
    if(isReady){
        GM->showVideo(VIDEO_STORE_RANDOM);
    }else{
        showInstanceMessage(LM->getText("video not available"));
    }
//    GM->showVideoDone(); // test
}
void BattleHud::onVideoUnitClick(){
    int nextIndex;
    time_t now = BSM->getCurrentTimeT();
    std::string strWatchedTime = UDGetStr(KEY_VIDEO_STORE_UNIT_WATCHED_TIME, "");
    time_t watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
    nextIndex = UDGetInt(KEY_VIDEO_STORE_UNIT_INDEX, 0);
    bool isReady = difftime(now, watchedTimeT) > videoStoreWaitForNextVideoTime;
    if(isReady){
        GM->showVideo(VIDEO_STORE_UNIT);
    }else{
        showInstanceMessage(LM->getText("video not available"));
    }
//    GM->showVideoDone(); // test
}
void BattleHud::onVideoGemClick(){
    int nextIndex;
    time_t now = BSM->getCurrentTimeT();
    std::string strWatchedTime = UDGetStr(KEY_VIDEO_STORE_GEM_WATCHED_TIME, "");
    time_t watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
    nextIndex = UDGetInt(KEY_VIDEO_STORE_GEM_INDEX, 0);
    bool isReady = difftime(now, watchedTimeT) > videoStoreWaitForNextVideoTime;
    if(isReady){
        GM->showVideo(VIDEO_STORE_GEM);
    }else{
        showInstanceMessage(LM->getText("video not available"));
    }
//    GM->showVideoDone(); // test 
}
void BattleHud::onVideoTabClick(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("videoStore");
    int currentTab = btn->getTag();
    layer->setTag(currentTab);
    Node* tabBack = layer->getChildByName("tabBack");
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        
        btn->setContentSize(i == currentTab?Size(250.35f, 237.16f):Size(250.35f, 199.01));
        btn->setColor(i == currentTab?Color3B::WHITE:Color3B::GRAY);
        btn->setLocalZOrder(tabBack->getLocalZOrder() + (i == currentTab?1:-1));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(btn->getLocalZOrder());
    }
    
    layer->getChildByName("ndTabRandom")->setVisible(currentTab == 0);
    layer->getChildByName("ndTabUnit")->setVisible(currentTab == 1);
    layer->getChildByName("ndTabGem")->setVisible(currentTab == 2);
    Node* nd;int nextIndex;
    if(currentTab == 0){
        nd = layer->getChildByName("ndTabRandom");
        nextIndex = UDGetInt(KEY_VIDEO_STORE_RANDOM_INDEX, 0);
    }else if(currentTab == 1){
        nd = layer->getChildByName("ndTabUnit");
        nextIndex = UDGetInt(KEY_VIDEO_STORE_UNIT_INDEX, 0);
    }else if(currentTab == 2){
        nd = layer->getChildByName("ndTabGem");
        nextIndex = UDGetInt(KEY_VIDEO_STORE_GEM_INDEX, 0);
    }
    ImageView* img;
    for (int i = 0; i < 12; i++) {
        btn = (Button*)nd->getChildByName(strmake("btnGem%d", i));
        btn->setEnabled(nextIndex == i);
        btn->setOpacity(255);
        if (i < nextIndex) {
            img = (ImageView*)btn->getChildByName("img");
            img->loadTexture("check.png");
        }else {
            if(i > nextIndex){
                btn->setOpacity(150);
            }
        }
        if(i == 5 || i == 11){
            
        }else{
            GM->alignToCenter(btn->getChildByName("imgIconGold"), btn->getChildByName("lblGold"), 30, btn->getContentSize().width/2, 0);
        }
    }
    oneSecCheckerForUI = 0;
//    updateUI(0);
}
void BattleHud::showTreeReward(int amount){
    Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("treeRewardMessageBox");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(amount).asString());
    ImageView* img = (ImageView*)layer->getChildByName("imgGem");
    GM->replaceImageWithSprite(img, Sprite::create("lumberIcon.png"));
    
    layer->getChildByName("imgGold")->setVisible(false);
    layer->getChildByName("imgTree")->setVisible(false);
    layer->getChildByName("lblGold")->setVisible(false);
    layer->getChildByName("lblTree")->setVisible(false);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "reward");
    GM->addTree(amount);
    BSM->saveUserData(strmake("tree=%d", GM->getTree()));
}

void BattleHud::showGoldReward(int amount){
    Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("treeRewardMessageBox");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(amount).asString());
    ImageView* img = (ImageView*)layer->getChildByName("imgGem");
    GM->replaceImageWithSprite(img, Sprite::create("goldIcon.png"));
    
    layer->getChildByName("imgGold")->setVisible(false);
    layer->getChildByName("imgTree")->setVisible(false);
    layer->getChildByName("lblGold")->setVisible(false);
    layer->getChildByName("lblTree")->setVisible(false);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "reward");
    GM->addCoin(amount);
    BSM->saveUserData(strmake("gold=%d", GM->getCoin()));
}
void BattleHud::onTutorialBoxClick(){
    if (talkIndex < talkText.size()) {
        talkIndex = (int)talkText.size();
        return;
    }
    tutorialIndex++;
    if(tutorialIndex > 10){
        showGemRewardMessageBox("battle tutorial reward", 100, 3000, 3000);
        tutorialEnded = true;
        UDSetBool(KEY_BATTLE_TUTORIAL_DONE, true);
        return;
    }
    talkIndex = 0;
    talkText = LM->getText(strmake("battle tutorial %d", tutorialIndex));
    tutorialNode->removeChildByName("sptIcon");
    Sprite* spt = nullptr;
    if(tutorialIndex == 0){
        spt = Sprite::create("swordIcon.png");
    }else if(tutorialIndex == 1){
        spt = Sprite::createWithSpriteFrameName("mine.png");
    }else if(tutorialIndex == 2){
        spt = Sprite::create("lumberIcon.png");
    }else if(tutorialIndex == 3){
        spt = Sprite::createWithSpriteFrameName("barracks.png");
    }else if(tutorialIndex == 4){
        spt = Sprite::createWithSpriteFrameName("archerAttack0.png");
    }else if(tutorialIndex == 5){
        spt = Sprite::create("btnBuild.png");
    }else if(tutorialIndex == 6){
        spt = Sprite::createWithSpriteFrameName("watcherTower.png");
    }else if(tutorialIndex == 7){
        spt = Sprite::create("handIcon.png");
    }else if(tutorialIndex == 8){
        spt = Sprite::create("btnAttack.png");
    }else if(tutorialIndex == 9){
        spt = Sprite::create("iconShield.png");
    }else if(tutorialIndex == 10){
        spt = Sprite::create("iconStar.png");
    }
    if(spt != nullptr){
        tutorialNode->addChild(spt);
        spt->setPosition(Point(size.width/2 - 750, 330));
        spt->setName("sptIcon");
        Sprite* sptExplosion = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
        tutorialNode->addChild(sptExplosion, 5);
        sptExplosion->setPosition(spt->getPosition());
        GM->runAnimation(sptExplosion, "cartoonyExplosion", false);
        sptExplosion->runAction(Sequence::create(DelayTime::create(0.04*14), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptExplosion)), nullptr));
    }
    
}
void BattleHud::showGemRewardMessageBox(std::string text, int gemCount, int gold, int tree){
    Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("gemRewardMessageBox");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
    GM->rollOpenScroll(imgBack);
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onCloseGemRewardMessageBox, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "ok");
    
    gemRewardCount = gemCount;
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(gemCount).asString());
    GM->alignToCenter(layer->getChildByName("imgGem"), lbl, 50, size.width/2, 0);
    
    goldRewardCount = gold;
    lbl = (Text*)layer->getChildByName("lblGold");
    lbl->setString(Value(gold).asString());
    Node* img = layer->getChildByName("imgGold");
    GM->alignToCenter(img, lbl, 50, size.width/2, 0);
    img->setVisible(gold > 0);
    lbl->setVisible(gold);
    
    treeRewardCount = gold;
    lbl = (Text*)layer->getChildByName("lblTree");
    lbl->setString(Value(tree).asString());
    img = layer->getChildByName("imgTree");
    GM->alignToCenter(img, lbl, 50, size.width/2, 0);
    img->setVisible(tree > 0);
    lbl->setVisible(tree);
    
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(gemCount).asString());
    GM->alignToCenter(layer->getChildByName("imgGem"), lbl, 50, size.width/2, 0);
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, text);
    
}
void BattleHud::onCloseGemRewardMessageBox(){
    int gemGiven = 0;
    Node* layer = this->getChildByName("gemRewardMessageBox");
    Vec2 gemPos = layer->getPosition() + layer->getChildByName("imgGem")->getPosition();
    Vec2 gemTargetPos = ndTopBar->getPosition() + ndTopBar->getChildByName("imgGemIcon")->getPosition();
    float gemDelay = 0;
    float goldDelay = 0;
    float treeDelay = 0;
    int moveDistance = 160;
    
    int gemSptCount = 10;
    if(gemRewardCount == 1){
        gemSptCount = 1;
    }
    
    
    Vec2 goldPos = layer->getPosition() + layer->getChildByName("imgGold")->getPosition();
    Vec2 goldTargetPos = ndTopBar->getPosition() + ndTopBar->getChildByName("imgGoldIcon")->getPosition();
    int goldGiven = 0;
    int goldSptCount = 10;
    if(goldRewardCount == 1){
        goldSptCount = 1;
    }
    Vec2 treePos = layer->getPosition() + layer->getChildByName("imgTree")->getPosition();
    Vec2 treeTargetPos = ndTopBar->getPosition() + ndTopBar->getChildByName("imgTreeIcon")->getPosition();
    int treeGiven = 0;
    int treeSptCount = 10;
    if(treeRewardCount == 1){
        treeSptCount = 1;
    }
    Sprite* sptGem;
    Sprite* sptGold;
    Sprite* sptTree;
    
    for (int i = 0; i < gemSptCount; i++) {
        sptGem = Sprite::create("gem.png");
        this->addChild(sptGem, 300);
        sptGem->setPosition(gemPos);
        sptGem->runAction(Sequence::create(DelayTime::create(gemDelay), EaseInOut::create(MoveBy::create(0.3f, Vec2(rand()%moveDistance - moveDistance/2, rand()%moveDistance - moveDistance/2)), 2), DelayTime::create(0.5f), EaseInOut::create(MoveTo::create(0.6f, gemTargetPos), 2), CallFuncN::create(CC_CALLBACK_1(BattleHud::gemMoveToTopBarDone, this)),NULL));
        gemDelay += 0.07f;
        sptGem->setTag(gemRewardCount/gemSptCount);
        gemGiven += sptGem->getTag();
    }
    sptGem->setTag(sptGem->getTag() + (gemRewardCount - gemGiven));
    
    if(goldRewardCount > 0){
        for (int i = 0; i < goldSptCount; i++) {
            sptGold = Sprite::create("goldIcon.png");
            this->addChild(sptGold, 300);
            sptGold->setPosition(goldPos);
            sptGold->runAction(Sequence::create(DelayTime::create(goldDelay), EaseInOut::create(MoveBy::create(0.3f, Vec2(rand()%moveDistance - moveDistance/2, rand()%moveDistance - moveDistance/2)), 2), DelayTime::create(0.5f), EaseInOut::create(MoveTo::create(0.6f, goldTargetPos), 2), CallFuncN::create(CC_CALLBACK_1(BattleHud::goldMoveToTopBarDone, this)),NULL));
            goldDelay += 0.07f;
            sptGold->setTag(goldRewardCount/goldSptCount);
            goldGiven += sptGold->getTag();
        }
        sptGold->setTag(sptGold->getTag() + (goldRewardCount - goldGiven));
    }
    if(treeRewardCount > 0){
        for (int i = 0; i < treeSptCount; i++) {
            sptTree = Sprite::create("lumberIcon.png");
            this->addChild(sptTree, 300);
            sptTree->setPosition(treePos);
            sptTree->runAction(Sequence::create(DelayTime::create(treeDelay), EaseInOut::create(MoveBy::create(0.3f, Vec2(rand()%moveDistance - moveDistance/2, rand()%moveDistance - moveDistance/2)), 2), DelayTime::create(0.5f), EaseInOut::create(MoveTo::create(0.6f, treeTargetPos), 2), CallFuncN::create(CC_CALLBACK_1(BattleHud::treeMoveToTopBarDone, this)),NULL));
            treeDelay += 0.07f;
            sptTree->setTag(treeRewardCount/treeSptCount);
            treeGiven += sptTree->getTag();
        }
        sptTree->setTag(sptTree->getTag() + (treeRewardCount - treeGiven));
    }
    
    closePopup();
}
void BattleHud::gemMoveToTopBarDone(Ref* ref){
    Sprite* spt = (Sprite*) ref;
    GM->addGem(spt->getTag());
    gemRewardCount -= spt->getTag();
    if(gemRewardCount <= 0){
        BSM->saveUserData(strmake("gem=%d&gold=%d&tree=%d", GM->getGem(), GM->getCoin(), GM->getTree()));
    }
    spt->removeFromParent();
}
void BattleHud::goldMoveToTopBarDone(Ref* ref){
    Sprite* spt = (Sprite*) ref;
    GM->addCoin(spt->getTag());
    goldRewardCount -= spt->getTag();
    spt->removeFromParent();
}
void BattleHud::treeMoveToTopBarDone(Ref* ref){
    Sprite* spt = (Sprite*) ref;
    GM->addTree(spt->getTag());
    treeRewardCount -= spt->getTag();
    spt->removeFromParent();
}

void BattleHud::talkBoxUpdate(float dt){
    if(tutorialEnded){
        this->unschedule(schedule_selector(BattleHud::talkBoxUpdate));
        tutorialNode->removeFromParent();
        tutorialNode = nullptr;
        return;
    }
    if (talkIndex == 0) {
        tutorialNode->setVisible(true);
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
void BattleHud::removeUsedAssets(){
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(keyListener);
}
void BattleHud::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK){
        //        if(GM->crossPromotionLayer != nullptr){
        //            CrossPromotion* cross = (CrossPromotion*)GM->crossPromotionLayer;
        //            cross->closeThis();
        //            return;
        //        }
        if(popupArray.size() > 0){
            closePopup();
        }else{
            onBackClick();
        }
    }
}
void BattleHud::setLocalizedStringForChildLbl(Node* parent, std::string key){
    Text* lbl = (Text*)parent->getChildByName("lbl");
    LM->setLocalizedString(lbl, key);
}
void BattleHud::onCutTreeClick(){
    if (!isWorkerAvailable()) {
        showNotEnoughWorkerUseGem();
        return;
    }
    Text* lbl = (Text*)hudLayer->getChildByName("btnCutTree")->getChildByName("lblGold");
    int goldPrice = Value(lbl->getString()).asInt();//getUnitUpgradePriceGold(selectedUnit->unitType, selectedUnit->level);
    if(GM->getCoin() < goldPrice){
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    
    GM->addCoin(-goldPrice);
    
    selectedUnit->unitState = UNIT_STATE_REMOVING;
    int timeLeft = 10;
//
//    for (int i = 0; i < buildings.size(); i++) {
//        if(buildings.at(i)->unitType == UNIT_LUMBERMILL){
//            timeLeft += 60;
//        }
//    }
    selectedUnit->buildingCompleteTime = (double)BSM->getCurrentTimeT() + timeLeft;
    selectedUnit->buildingCompleteTimeLeft = timeLeft;
    
    selectedUnit->energy = 1;
    selectedUnit->maxEnergy = 100;
    selectedUnit->updateEnergy();
    Text* lblTimeLeft = Text::create("Upgrade...", "BMDOHYEON.ttf", 20);
    lblTimeLeft->enableOutline(Color4B::BLACK, 2);
    WORLD->addChild(lblTimeLeft, selectedUnit->getParent()->getLocalZOrder()+1);
    lblTimeLeft->setPosition(selectedUnit->getPosition() + Point(0, 60));
    selectedUnit->lblTimeLeft = lblTimeLeft;
    
    onUnitSelected(selectedUnit);
    updateAvailableWorkerCount();
    
    updateBuildingsSaveData();
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_BUILDING);
    saveUserData(datas);
}
void BattleHud::onMoreGemInLobbyClick(){
    if(!this->getChildByName("shopLayer")){
        onShopClick();
    }
    onTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab2"));
}
void BattleHud::onMoreGoldClick(){
    if(!this->getChildByName("shopLayer")){
        onShopClick();
    }
    onTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab1"));
}
void BattleHud::onMoreTreeClick(){
    if(!this->getChildByName("shopLayer")){
        onShopClick();
    }
    Node* layer = this->getChildByName("shopLayer");
    onTabClick(layer->getChildByName("btnTab1"));
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->getInnerContainer()->setPositionX(-2300);
}

void BattleHud::onWorkerClick(){
    Node* layer = CSLoader::createNode("WorkerPopup.csb");
    this->addChild(layer, 4);
    layer->setName("workerPopup");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    Node* imgBody = layer->getChildByName("imgBody");
    Text* lbl = (Text*)imgBody->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "worker");
    int current, max;
    int workerUnlockedCount = 1;
    for (int i = 0; i < 4; i++) {
        lbl = (Text*)imgBody->getChildByName(strmake("lblCondition%d", i));
        if(i == 0){
            LM->setLocalizedString(lbl, strmake("worker condition %d", i));
        }else{
            if(i == 1){
                current = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1;
                max = 12;
            }else if(i == 2){
                current = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
                max = 20;
            }else if(i == 3){
                current = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1 - 12;
                max = 12;
            }
            if(current < 0){
                current = 0;
            }
            if(current > max){
                current = max;
            }
            lbl->setString(strmake("%s(%d/%d)", LM->getText(strmake("worker condition %d", i)).c_str(), current, max));
            imgBody->getChildByName(strmake("imgClear%d", i))->setVisible(current >= max || UDGetBool(KEY_WORKER_BOUGHT, false));
            if(current >= max){
                workerUnlockedCount++;
            }
        }
        
        Sprite* spt = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
        imgBody->addChild(spt);
        Node* img = imgBody->getChildByName(strmake("img%d", i));
        spt->setPosition(img->getPosition());
        img->removeFromParent();
    }
    btn = (Button*)layer->getChildByName("btnBuy");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyWorkersClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    lbl->setString(Value((4-workerUnlockedCount)*200).asString());
    btn->setEnabled(workerUnlockedCount < 4);
    
    lbl = (Text*)layer->getChildByName("lblOr");
    LM->setLocalizedString(lbl, "or");
    if(workerCount >= 4){
        btn->setVisible(false);
        lbl->setVisible(false);
    }
}
void BattleHud::onBuyWorkersClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int price = Value(((Text*)btn->getChildByName("lbl"))->getString()).asInt();
    if (GM->getGem() >=  price) {
        GM->addGem(-price);
//        SplitCols
        UDSetBool(KEY_WORKER_BOUGHT, true);
        workerCount = 4;
        updateAvailableWorkerCount();
        closePopup();
        onWorkerClick();
        
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_GEM);
        saveUserData(datas);
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
}
void BattleHud::closePopup(){
    log("closePopup");
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        if(popup == ndTopBar->getParent() ){
            if (popupArray.size() == 0) {
                moveTopBarTo(hudLayer);
                ndTopBar->stopAllActions();
                ndTopBar->runAction(EaseInOut::create(MoveTo::create(1, Vec2(0, ndTopBar->getPositionY())), 2));
            }else{
                moveTopBarTo(popupArray.at(popupArray.size() - 1));
            }
        }
        log("closePopup success");
        popup->removeFromParent();
    }
}
void BattleHud::setAsPopup(Node* node){
    popupArray.pushBack(node);
    node->setLocalZOrder(200 + (int)popupArray.size());
}

void BattleHud::onBattleClick(){
    Node* layer = CSLoader::createNode("BattlePopup.csb");
    this->addChild(layer, 4);
    layer->setName("battlePopup");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    moveTopBarTo(layer, false);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnFindMatch");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onNetworkBattleClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "find match");
    
    btn = (Button*)layer->getChildByName("btnCompleteResting");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onCompleteRestingClick, this));
    lbl = (Text*)btn->getChildByName("lblResting");
    LM->setLocalizedString(lbl, "resting");
    lbl = (Text*)btn->getChildByName("lblComplete");
    LM->setLocalizedString(lbl, "complete");
    
    btn = (Button*)layer->getChildByName("btnRank");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onDefenceRecordClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "rank");
    
    lbl = (Text*)layer->getChildByName("lblSinglePlay");
    LM->setLocalizedString(lbl, "single play");
    lbl = (Text*)layer->getChildByName("lblMultiPlay");
    LM->setLocalizedString(lbl, "multi play");
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("svSinglePlay");
    Button* btnTemp = (Button*)layer->getChildByName("btnTemp");
    int stageCount = 10;
    float gapY = 246.22f;
    float gapX = 272.43f;
    float svHeight = (stageCount/3)*gapX + gapX;
    if(svHeight < sv->getContentSize().height){
        svHeight = sv->getContentSize().height;
    }
    float startY = svHeight - gapX/2;
    sv->setInnerContainerSize(Size(sv->getContentSize().width, svHeight));
    sv->setClippingEnabled(true);
    int starCount = 0;
    bool isLocked = false;
    ImageView* img;
    for (int i = 0; i < stageCount + 1; i++) {
        btn = (Button*)btnTemp->clone();
        sv->addChild(btn);
        btn->setTag(-1);
        btn->setPosition(Vec2(136.13f + (i%3)*gapX, startY - gapY*(i/3)));
        if(stageCount == i){
            btn->removeAllChildren();
            Sprite* spt = Sprite::create("comingSoon.png");
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
            break;
        }
        isLocked = !(UDGetBool(strmake(KEY_SINGLE_PLAY_CLEAR_FORMAT, i - 1).c_str(), false) || i == 0);
        starCount = UDGetInt(strmake(KEY_SINGLE_PLAY_STAR_FORMAT, i).c_str(), 0);
        if(isLocked){
            btn->removeAllChildren();
            Sprite* spt = Sprite::create("iconLock.png");
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
        }else{
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onLocalBattleClick, this));
            starCount = UDGetInt(strmake(KEY_SINGLE_PLAY_STAR_FORMAT, i).c_str(), 0);
            for(int j = 0; j < 3; j++){
                img = (ImageView*)btn->getChildByName(strmake("imgStar%d", j));
                img->setGrayScale(starCount <= j);
            }
            btn->setTag(i);
            lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString(Value(i+1).asString());
        }
    }
    btnTemp->removeFromParent();
    oneSecCheckerForUI = 0;
    updateUI(0);
}
void BattleHud::onLocalBattleClick(Ref* ref){
    BTN_FROM_REF
    int stageIndex = btn->getTag();
    if(stageIndex < 0){
        log("not available");
        return;
    }
    log("go stage %d", stageIndex);
    this->unschedule(schedule_selector(BattleHud::update));
    this->unschedule(schedule_selector(BattleHud::updateUI));
    checkUnsaved();
    WORLD->removeUsedAssets();
    BHUD = nullptr;
    removeUsedAssets();
    GM->isVisiting = false;
    GM->nextScene = STAGE_SINGLEPLAY; // test
    GM->singlePlayStageIndex = stageIndex;
    auto scene = HelloWorld::scene(GM->nextScene, false); // test
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
void BattleHud::onDefenceRecordClick(){
    showIndicator();
    isTopRankRequested = true;
    BSM->getTopPlayers(50);
}
void BattleHud::showDefenceRecord(){
    Node* layer = CSLoader::createNode("DefenceRecord.csb");
    this->addChild(layer, 4);
    layer->setName("defenceRecordPopup");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    moveTopBarTo(layer, false);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "defence record");
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("svRecord");
    ImageView* recordTemp = (ImageView*)sv->getChildByName("imgTemplate");
    std::string strRecord = UDGetStr(KEY_DEFENCE_RECORD, "");
    ValueVector entireList = GM->split(strRecord, "_");
    std::string str;
    Widget* item;ImageView* img;
    float itemHeight = 207.04f;
    float gapY = 10;
    float scrollViewHeight = (itemHeight + gapY)*(entireList.size()-1);
    if(scrollViewHeight < sv->getContentSize().height){
        scrollViewHeight = sv->getContentSize().height;
    }
    sv->setInnerContainerSize(Size(sv->getContentSize().width, scrollViewHeight));
    int count =0;
    for (int i = (int)entireList.size()-1; i >= 0 ; i--) {
        str = entireList.at(i).asString();
        if(str.length() < 2){
            continue;
        }
        ValueVector list = GM->split(str, "/");
        if(list.size() != 6){
            continue;
        }
        item = recordTemp->clone();
        sv->addChild(item);
        item->setPosition(Vec2(recordTemp->getPositionX(), scrollViewHeight - (itemHeight + gapY)/2 - (itemHeight + gapY)*count));
        lbl = (Text*)item->getChildByName("lblName");
        lbl->setString(list.at(1).asString());
        for (int j = 0; j < 3; j++) {
            img = (ImageView*)item->getChildByName(strmake("imgStar%d", j));
            img->setGrayScale(list.at(2).asInt() <= j);
        }
        lbl = (Text*)item->getChildByName("lblTrophy");
        lbl->setString(strmake("%d", list.at(3).asInt()));
        lbl = (Text*)item->getChildByName("lblGold");
        lbl->setString(strmake("%d", list.at(4).asInt()));
        lbl = (Text*)item->getChildByName("lblTree");
        lbl->setString(strmake("%d", list.at(5).asInt()));
        lbl = (Text*)item->getChildByName("lblRevenge");
        LM->setLocalizedString(lbl, "revenge");
        btn = (Button*)item->getChildByName("btnRevenge");
//        btn->setTitleText(LM->getText("revenge"));
        btn->setName(list.at(0).asString());
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onRevengeClick, this));
        count++;
    }
    recordTemp->removeFromParent();
    
    lbl = (Text*)layer->getChildByName("lblTopRank");
    LM->setLocalizedString(lbl, "top rank");
    
    sv = (ScrollView*)layer->getChildByName("svTopRank");
    ImageView* rankTemp = (ImageView*)sv->getChildByName("imgTemplate");
    itemHeight = 172.22f;
    gapY = 8;
    scrollViewHeight = (itemHeight + gapY)*topRankIDs.size();
    if(scrollViewHeight < sv->getContentSize().height){
        scrollViewHeight = sv->getContentSize().height;
    }
    sv->setInnerContainerSize(Size(sv->getContentSize().width, scrollViewHeight));
    for (int i = 0; i < topRankIDs.size(); i++) {
        item = rankTemp->clone();
        sv->addChild(item);
        item->setPosition(Vec2(rankTemp->getPositionX(), scrollViewHeight - (itemHeight + gapY)*i));
        lbl = (Text*)item->getChildByName("lblName");
        lbl->setString(strmake("%d. %s", i+1, topRankNames.at(i).c_str()));
        lbl = (Text*)item->getChildByName("lblTrophy");
        lbl->setString(strmake("%d", topRankTrophys.at(i)));
        lbl = (Text*)item->getChildByName("lblAttack");
        LM->setLocalizedString(lbl, "attack");
        btn = (Button*)item->getChildByName("btnVisit");
//        btn->setTitleText(LM->getText("visit"));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onVisitTopRankerClick, this));
        btn->setName(topRankIDs.at(i));
    }
    rankTemp->removeFromParent();
    
    lbl = (Text*)layer->getChildByName("lblMyName");
    lbl->setString(strmake("%d.%s", myRank, UDGetStr(KEY_NAME, "").c_str()));
    lbl = (Text*)layer->getChildByName("lblMyTrophy");
    lbl->setString(Value(UDGetInt(KEY_TROPHY, 1000)).asString());
    
    btn = (Button*)layer->getChildByName("btnBuyShieldGem");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyShieldWithGem, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "24 hours");
    btn = (Button*)layer->getChildByName("btnPlayForShield");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onPlayForShieldClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "6 hours");
    
    lbl = (Text*)layer->getChildByName("lblShieldDesc");
    LM->setLocalizedString(lbl, "shield desc");
    
    shieldEndTimeT = BSM->getTimeTFromStr(UDGetStr(KEY_SHIELD_END_TIME, ""));
}
void BattleHud::onBuyShieldWithGem(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Text* lbl = (Text*)btn->getChildByName("lblGemCount");
    int gem = Value(lbl->getString()).asInt();
    if (GM->getGem() >= gem) {
        GM->addGem(-gem);
        addShieldRequested = true;
        BSM->addShield(24);
        showIndicator();
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
}
void BattleHud::onPlayForShieldClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(GameSharing::isVideoAvailable()){
        GM->showVideo(VIDEO_SHIELD);
    }else{
        showInstanceMessage(LM->getText("video not available"));
    }
}
void BattleHud::onVisitTopRankerClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    GM->addCoin(-100);
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    saveUserData(datas);
    isVisitRequested = true;
//    GM->isVisiting = true;
    BSM->getOtherUserData(strmake("id=%s&name=1&trophy=1&gold=1&tree=1&deck=1&buildings=1", btn->getName().c_str()));
}
void BattleHud::onRevengeClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    log("revenge id: %s", btn->getName().c_str());
    GM->addCoin(-10);
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    saveUserData(datas);
    isVisitRequested = true;
    GM->isVisiting = false;
    BSM->getOtherUserData(strmake("id=%s&name=1&trophy=1&gold=1&tree=1&deck=1&buildings=1&shield_end=1", btn->getName().c_str()));
}
void BattleHud::onCompleteRestingClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Text* lbl = (Text*)btn->getChildByName("lblGem");
    int price = Value(lbl->getString()).asInt();
    if(price <= GM->getGem()){
        GM->addGem(-price);
        BSM->saveUserData(strmake("gem=%d", GM->getGem()));
        GM->restingForBattleEndTime = 0;
        oneSecCheckerForUI = 0;
        updateUI(0);
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
    }
}
void BattleHud::onNetworkBattleClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int price = Value(((Text*)btn->getChildByName("lblGold"))->getString()).asInt();
    if (GM->getCoin() >=  price) {
        GM->addCoin(-price);
        
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_GOLD);
        saveUserData(datas);
    }else{
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    if (unitInfoListDeck.size() == 0) {
        showInstanceMessage(LM->getText("not enough soliders"));
        return;
    }
    GM->isVisiting = false;
    this->unschedule(schedule_selector(BattleHud::update));
    this->unschedule(schedule_selector(BattleHud::updateUI));
    checkUnsaved();
    WORLD->removeUsedAssets();
    BHUD = nullptr;
    GM->nextScene = STAGE_RAID; // test
    
    removeUsedAssets();
    auto scene = Scene::create();
    FindMatch* findMatch = FindMatch::create();
    scene->addChild(findMatch);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
void BattleHud::getLastAccessTime(){
    isLastAccessTimeRequested = true;
    BSM->getUserData("last_launch_date=1");
}
void BattleHud::onShopClick(){
    Node* layer = CSLoader::createNode("Shop.csb");
    this->addChild(layer, 4);
    layer->setName("shopLayer");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->getChildByName("imgBackground")->setLocalZOrder(-100);
    moveTopBarTo(layer);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
//    btn->setPositionY(size.height - (layer->getContentSize().height - btn->getPositionY()) - layer->getPositionY()/2);
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    
    onTabClick(layer->getChildByName("btnTab0"));
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onTabClick, this));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(100);
    }
    updateStatus(layer);
}
void BattleHud::onTabClick(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("shopLayer");
    int currentTab = btn->getTag();
    Node* tabBack = layer->getChildByName("tabBack");
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        
        btn->setContentSize(i == currentTab?Size(250.35f, 237.16f):Size(250.35f, 199.01));
        btn->setColor(i == currentTab?Color3B::WHITE:Color3B::GRAY);
        btn->setLocalZOrder(tabBack->getLocalZOrder() + (i == currentTab?1:-1));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(btn->getLocalZOrder());
    }
    
    Button* btnTemp;Text* lbl;ImageView* img;
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->removeAllChildren();
    int x = 387.93f;
    int gapX = 740.45f;
    int y = 429.55f;
    if (currentTab == 0) {
        btnTemp = (Button*)layer->getChildByName("btnBuildingTemp");
        for (int i = BUILDING_MINE; i < BUILDING_TOTAL; i++) {
            btn = (Button*)btnTemp->clone();
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyBuilding, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            btn->setTag(i);
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            PPLabel* lblPP = replaceTextToPPLabel(lbl);
            lblPP->setString(LM->getText(getBuildingName(i)));
            lblPP->setWidth(btn->getContentSize().width*0.8f);
            
            int currentBuildingCount =  getCurrentBuildingCount(i);
            int maxBuildingCount =  getMaxBuildingCount(i);
            ImageView* sptBuilding = ImageView::create(getBuildingSpriteName(i), ui::Widget::TextureResType::PLIST);
            
            img = (ImageView*)btn->getChildByName("img");
            sptBuilding->setPosition(img->getPosition());
            btn->addChild(sptBuilding);
            img->removeFromParent();
            
            lbl = (Text*)btn->getChildByName("lblCount");
            lbl->setString(strmake("%d/%d",currentBuildingCount, maxBuildingCount));
            btn->setEnabled(currentBuildingCount < maxBuildingCount);
            lbl->setTextColor(currentBuildingCount >= maxBuildingCount?Color4B::RED:Color4B::WHITE);
            
            int goldPrice = getBuildingPriceGold(i);
            lbl = (Text*)btn->getChildByName("lblGold");
            lbl->setString(Value(goldPrice).asString());
            lbl->setTextColor(GM->getCoin() < goldPrice?Color4B::RED:Color4B::WHITE);
            
            int treePrice = getBuildingPriceTree(i);
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
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyGold, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "gold text");
            int perHour = getMineGoldPerHour(UDGetInt(KEY_CASTLE_LEVEL, 0));
            lbl = (Text*)btn->getChildByName("lblAmount");
            int amount = perHour*(i*2 + 1);
            lbl->setString(Value(amount).asString());
            lbl = (Text*)btn->getChildByName("lblGem");
            lbl->setString(Value(GM->getGemForHour()*(i*2 + 1)).asString());
        }
        for (int i = 0; i < 3; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnTree%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyTree, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
            LM->setLocalizedString(lbl, "tree");
            int perHour = 40*60/10;
            lbl = (Text*)btn->getChildByName("lblAmount");
            int amount = perHour*(i*2 + 1);
            lbl->setString(Value(amount).asString());
            lbl = (Text*)btn->getChildByName("lblGem");
            lbl->setString(Value(GM->getGemForHour()*(i*2 + 1)/2).asString());
        }
    }else if (currentTab == 2) {
        for (int i = 0; i < 4; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnGem%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuyGem, this));
            sv->addChild(btn);
            btn->setPosition(Vec2(x, y));
            x += gapX;
            
            lbl = (Text*)btn->getChildByName("lblTitle");
//            PPLabel* lblPP = replaceTextToPPLabel(lbl);
            lbl->setString(strmake("%s %d", LM->getText("gem pack").c_str(), i+1));
//            lblPP->setWidth(btn->getContentSize().width*0.8f);
            lbl = (Text*)btn->getChildByName("lblPrice");
            lbl->setFontName(LM->getLocalizedFont());
            if(i == 0){
                lbl->setString(GameSharing::getPriceLocale("cc_gem3"));
            }else if(i == 1){
                lbl->setString(GameSharing::getPriceLocale("cc_gem10"));
            }else if(i == 2){
                lbl->setString(GameSharing::getPriceLocale("cc_gem50"));
            }else if(i == 3){
                lbl->setString(GameSharing::getPriceLocale("cc_gem100"));
            }
        }
    }
    
    sv->setInnerContainerSize(Size(x, sv->getContentSize().height));
}
int BattleHud::getMaxGold(int level){
    string str = GM->castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(0)).asInt();
    }else{
        return 400000;
    }
}
int BattleHud::getMaxGold(){
    return getMaxGold(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int BattleHud::getMaxTree(){
    return getMaxTree(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int BattleHud::getMaxTree(int level){
    string str = GM->castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(1)).asInt();
    }else{
        return 600000;
    }
}
int BattleHud::getMineGoldPerHour(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["mine"].asString();
    return Value(GM->split(str, "_").at(0)).asInt();
}
int BattleHud::getMineGoldStorage(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["mine"].asString();
    return Value(GM->split(str, "_").at(1)).asInt();
}

int BattleHud::getLumbermillTreePerHour(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["lumbermill"].asString();
    return Value(GM->split(str, "_").at(0)).asInt();
}
int BattleHud::getLumbermillTreeStorage(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["lumbermill"].asString();
    return Value(GM->split(str, "_").at(1)).asInt();
}

int BattleHud::getUnitUpgradePriceGold(int index, int level){
    string str = GM->buildingUpgradePriceForLevelTable[Value(level).asString()].asValueMap()[GM->getUnitName(index)].asString();
    log("str: %s", str.c_str());
    if(str.length() > 1){
        return Value(GM->split(str, "_").at(0)).asInt();
    }else{
        return 0;
    }
}
int BattleHud::getUnitUpgradePriceTree(int index, int level){
    string str = GM->buildingUpgradePriceForLevelTable[Value(level).asString()].asValueMap()[GM->getUnitName(index)].asString();
    if(str.length() > 1){
        return Value(GM->split(str, "_").at(1)).asInt();
    }else{
        return 0;
    }
}
int BattleHud::getBuildingPriceGold(int buildingIndex){
    int count = getCurrentBuildingCount(buildingIndex);
    if(buildingIndex == BUILDING_TREE){
        count = 0;
    }
    string str = GM->buildingPriceForCastleLevelTable[Value(count).asString().c_str()].asValueMap()[getBuildingName(buildingIndex).c_str()].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(0)).asInt();
    }else{
        return 0;
    }
}
int BattleHud::getBuildingPriceTree(int buildingIndex){
    int count = getCurrentBuildingCount(buildingIndex);
    if(buildingIndex == BUILDING_TREE){
        count = 0;
    }
    string str = GM->buildingPriceForCastleLevelTable[Value(count).asString().c_str()].asValueMap()[getBuildingName(buildingIndex).c_str()].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(1)).asInt();
    }else{
        return 0;
    }
}
PPLabel* BattleHud::replaceTextToPPLabel(Text* lblText){
    PPLabel* lbl = PPLabel::create(lblText->getString(), lblText->getFontSize(), Color3B(lblText->getTextColor()), true, lblText->getOutlineSize(), TextHAlignment::CENTER, false);
    lbl->setPosition(lblText->getPosition());
    lblText->getParent()->addChild(lbl);
    lblText->removeFromParent();
    return lbl;
}
int BattleHud::getCurrentBuildingCount(int index){
    int buildingCount = 0;
    for(auto building: buildings){
        if (index == getBuildingIndex(building->unitType)) {
            buildingCount++;
        }
    }
    return buildingCount;
}
int BattleHud::getMaxBuildingCount(int index){
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL);
    return GM->buildingCountForCastleLevelTable[Value(castleLevel).asString().c_str()].asValueMap()[getBuildingName(index).c_str()].asInt();
}
std::string BattleHud::getBuildingName(int index){
    if(index == BUILDING_CASTLE){
        return "castle";
    }else if(index == BUILDING_MINE){
        return "mine";
    }else if(index == BUILDING_TREE){
        return "tree";
    }else if(index == BUILDING_FARM){
        return "farm";
    }else if(index == BUILDING_BARRACKS){
        return "barracks";
    }
//    else if(index == BUILDING_UNDERGROUND_BUNKER){
//        return "underground bunker";
//    }else if(index == BUILDING_TRIGGER){
//        return "trigger";
//    }
    else if(index == BUILDING_WATCHER_TOWER){
        return "watcher tower";
    }else if(index == BUILDING_LUMBURMILL){
        return "lumbermill";
    }else if(index == BUILDING_FACTORY){
        return "factory";
    }else if(index == BUILDING_AIRPORT){
        return "airport";
    }else if(index == BUILDING_ORC_HQ){
        return "orc hq";
    }else if(index == BUILDING_ORC_BARRACKS){
        return "orc barracks";
    }else if(index == BUILDING_ORC_BUNKER){
        return "orc bunker";
    }else if(index == BUILDING_ORC_TROLL_HOUSE){
        return "orc troll house";
    }
    return "castle";
}
int BattleHud::getUnitIndex(int buildingIndex){
    if(buildingIndex == BUILDING_CASTLE){
        return UNIT_CASTLE;
    }else if(buildingIndex == BUILDING_MINE){
        return UNIT_MINE;
    }else if(buildingIndex == BUILDING_TREE){
        return UNIT_TREE_FOR_BATTLE;
    }else if(buildingIndex == BUILDING_FARM){
        return UNIT_FARM;
    }else if(buildingIndex == BUILDING_BARRACKS){
        return UNIT_BARRACKS;
    }
//    else if(buildingIndex == BUILDING_UNDERGROUND_BUNKER){
//        return UNIT_UNDERGROUND_BUNKER;
//    }else if(buildingIndex == BUILDING_TRIGGER){
//        return UNIT_TRIGGER;
//    }
    else if(buildingIndex == BUILDING_WATCHER_TOWER){
        return UNIT_WATCHERTOWER;
    }else if(buildingIndex == BUILDING_LUMBURMILL){
        return UNIT_LUMBERMILL;
    }else if(buildingIndex == BUILDING_FACTORY){
        return UNIT_FACTORY;
    }else if(buildingIndex == BUILDING_AIRPORT){
        return UNIT_AIRPORT;
    }else if(buildingIndex == BUILDING_ORC_HQ){
        return UNIT_ORC_HQ;
    }else if(buildingIndex == BUILDING_ORC_BARRACKS){
        return UNIT_ORC_BARRACKS;
    }else if(buildingIndex == BUILDING_ORC_BUNKER){
        return UNIT_ORC_BUNKER;
    }else if(buildingIndex == BUILDING_ORC_TROLL_HOUSE){
        return UNIT_ORC_TROLL_HOUSE;
    }
    return UNIT_CASTLE;
}
std::string BattleHud::getBuildingSpriteName(int buildingIndex){
    return WORLD->getSpriteNameForUnit(getUnitIndex(buildingIndex));
}
void BattleHud::showNotEnoughWorkerUseGem(){
    Node* layer = CSLoader::createNode("NotEnoughWorkerUseGem.csb");
    this->addChild(layer, 4);
    layer->setName("notEnoughWorker");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("lbl");
    LM->setLocalizedString(lbl, "not enough worker use gem");
    
    btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUseGemToFinishWorkerClick, this));
    int timeLeft = -1;
    for(auto building: buildings){
        if(building->unitState == UNIT_STATE_BUILDING || building->unitState == UNIT_STATE_UPGRADE || building->unitState == UNIT_STATE_REMOVING){
            if(timeLeft == -1){
                timeLeft = building->buildingCompleteTimeLeft;
            }else if(building->buildingCompleteTimeLeft < timeLeft){
                timeLeft = building->buildingCompleteTimeLeft;
            }
        }
    }
    int gem = GM->getGemForTimeLeft(timeLeft);
    lbl = (Text*)btn->getChildByName("lbl");
    lbl->setString(Value(gem).asString());
}
void BattleHud::onUseGemToFinishWorkerClick(Ref* ref){
    int timeLeft = -1;
    Movable* theBuilding = nullptr;
    for(auto building: buildings){
        if(building->unitState == UNIT_STATE_BUILDING || building->unitState == UNIT_STATE_UPGRADE || building->unitState == UNIT_STATE_REMOVING){
            if(timeLeft == -1){
                timeLeft = building->buildingCompleteTimeLeft;
                theBuilding = building;
            }else if(building->buildingCompleteTimeLeft < timeLeft){
                timeLeft = building->buildingCompleteTimeLeft;
                theBuilding = building;
            }
        }
    }
    if(theBuilding != nullptr){
        int gem = GM->getGemForTimeLeft(timeLeft);
        if(gem <= GM->getGem()){
            GM->addGem(-gem);
            theBuilding->buildingCompleteTimeLeft = 0;
            closePopup();
        }else{
            showInstanceMessage(LM->getText("not enough gems"));
        }
    }
}
void BattleHud::showNotEnoughBarracksUseGem(){
    Node* layer = CSLoader::createNode("NotEnoughWorkerUseGem.csb");
    this->addChild(layer, 4);
    layer->setName("notEnoughWorker");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("lbl");
    LM->setLocalizedString(lbl, "not enough barracks use gem");
    
    btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUseGemToFinishBarracksClick, this));
    int timeLeft = -1;
    time_t now = BSM->getCurrentTimeT();
    for(auto unit : unitInfoListInventory){
        if(unit->unitState == UNIT_STATE_UPGRADE){
            if(timeLeft == -1){
                timeLeft = unit->endTime - (double)now;
            }else if(unit->endTime - (double)now < timeLeft){
                timeLeft = unit->endTime - (double)now;
            }
        }
    }
    int gem = GM->getGemForTimeLeft(timeLeft);
    lbl = (Text*)btn->getChildByName("lbl");
    lbl->setString(Value(gem).asString());
}
void BattleHud::onUseGemToFinishBarracksClick(Ref* ref){
//    UnitInfo* theInfo = nullptr;
//    int timeLeft = -1;
//    for(auto unit : unitInfoListInventory){
//        if(unit->unitState == UNIT_STATE_UPGRADE){
//            if(timeLeft == -1){
//                timeLeft = unit->endTime - BSM->getCurrentTime();
//                theInfo = unit;
//            }else if(unit->endTime - BSM->getCurrentTime() < timeLeft){
//                timeLeft = unit->endTime - BSM->getCurrentTime();
//                theInfo = unit;
//            }
//        }
//    }
//    if(theInfo != nullptr){
//        int gem = GM->getGemForTimeLeft(timeLeft);
//        GM->addGem(-gem);
//        theInfo->endTime = BSM->getCurrentTime();
//        theInfo->unitState = UNIT_STATE_IDLE;
//        theInfo->level++;
//
//        updateInventorySaveData();
//        std::vector<int> datas;
//        datas.push_back(DATA_TYPE_GEM);
//        datas.push_back(DATA_TYPE_INVENTORY);
//        saveUserData(datas);
//
//        closePopup();
//    }
}
void BattleHud::onBuyBuilding(Ref* ref){
    BTN_FROM_REF
    if (!isWorkerAvailable()) {
        showNotEnoughWorkerUseGem();
        return;
    }
    int index = btn->getTag();
    cocos2d::Size occupySize;
    if (index == BUILDING_MINE || index == BUILDING_LUMBURMILL || index == BUILDING_BARRACKS || index == BUILDING_FACTORY || index == BUILDING_AIRPORT) {
        occupySize = Size(3, 3);
    }else if(index == BUILDING_TREE){
        occupySize = Size(1, 1);
    }else if(index == BUILDING_FARM){// || index == BUILDING_UNDERGROUND_BUNKER){
        occupySize = Size(3, 2);
    }else if(index == BUILDING_WATCHER_TOWER){
        occupySize = Size(2, 2);
    }
//    else if(index == BUILDING_TRIGGER){
//        occupySize = Size(1, 1);
//    }
    closePopup();
    WORLD->createBuildingTemplate(getUnitIndex(index), occupySize.width, occupySize.height, WORLD->getSpriteNameForUnit(getUnitIndex(index)));
    currentJob = JOB_MOVE_BUILDING_TEMPATE;
    currentJobDetailIndex = index;
}
void BattleHud::hideCancelButton(){
    ndTopBar->setVisible(true);
    Button* btn = (Button*)hudLayer->getChildByName("btnCancel");
    btn->runAction(EaseBackIn::create(MoveBy::create(0.3f, Vec2(0, -btn->getContentSize().height*2))));
    btn = (Button*)hudLayer->getChildByName("btnOk");
    btn->runAction(EaseBackIn::create(MoveBy::create(0.3f, Vec2(0, -btn->getContentSize().height*2))));
    
    for(auto child : hudLayer->getChildren()){
        if(child->getName().compare("btnCancel") != 0 && child->getName().compare("btnOk") != 0){
            child->runAction(EaseBackOut::create(MoveTo::create(0.3f, Vec2(child->getPositionX(), child->getTag()))));
//            Widget* widget = (Widget*)child;
//            if(widget != nullptr){
//                widget->setEnabled(true);
//            }
        }
    }
}
void BattleHud::showCancelButton(){
    ndTopBar->setVisible(false);
    Button* btn = (Button*)hudLayer->getChildByName("btnCancel");
    btn->setPosition(Vec2(size.width/2, -btn->getContentSize().height/2));
    btn->runAction(EaseBackOut::create(MoveTo::create(0.3f, Vec2(size.width/2 - 150, btn->getContentSize().height/2 + 20))));
    
    btn = (Button*)hudLayer->getChildByName("btnOk");
    btn->setPosition(Vec2(size.width/2, -btn->getContentSize().height/2));
    btn->runAction(EaseBackOut::create(MoveTo::create(0.3f, Vec2(size.width/2 + 150, btn->getContentSize().height/2 + 20))));
    
    for(auto child : hudLayer->getChildren()){
        if(child->getName().compare("btnCancel") != 0 && child->getName().compare("btnOk") != 0){
            child->setTag(child->getPositionY());
            child->runAction(EaseBackIn::create(MoveTo::create(0.3f, Vec2(child->getPositionX(), child->getPositionY() > size.height/2?(size.height + child->getContentSize().height):(-child->getContentSize().height)))));
//            Widget* widget = (Widget*)child;
//            std::string strName = child->getName();
//            if(widget != nullptr){
//                widget->setEnabled(false);
//            }
        }
    }
}
void BattleHud::onResourceInShopClick(){
    
}
void BattleHud::onBuyResource(Ref* ref){
    BTN_FROM_REF
}
void BattleHud::onGemInShopClick(){
    
}
void BattleHud::onBuyTree(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
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
    saveUserData(datas);
}
void BattleHud::onBuyGold(Ref* ref){
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
    saveUserData(datas);
}
void BattleHud::onBuyGem(Ref* ref){
    BTN_FROM_REF
    int index = btn->getTag();
//
//#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//    GM->addCoin(10000);
//    GM->addTree(10000);
//    GM->addGem(10000);
//    saveInventoryAndDeck();
//    return; // test 
//#endif
//
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
void BattleHud::onSpecialOfferClick(){
    isFromLastChance = false;
    Node* layer = CSLoader::createNode("SpecialOffer.csb");
    this->addChild(layer, 4);
    layer->setName("specialOffer");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    moveTopBarTo(layer, false);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "special offer");
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "buy only once");
    
    std::string strOpenTime = UDGetStr(KEY_SPECIAL_OFFER_OPEN_TIME, "");
    time_t openTimeT = BSM->getTimeTFromStr(strOpenTime);
    if(strOpenTime.size() == 0){
        std::string value = "";
        value += BSM->getStrFromTime(BSM->getCurrentTimeT());
        UDSetStr(KEY_SPECIAL_OFFER_OPEN_TIME, value);
    }
    
    bool isBought = UDGetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 0).c_str(), false);
    btn = (Button*)layer->getChildByName("btn0");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setVisible(!isBought);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "buy");
    strStarterPrice0 = LM->getText("buy");
    strStarterPrice1 = LM->getText("buy");
    strStarterPrice2 = LM->getText("buy");
//#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    strStarterPrice0 = GameSharing::getPriceLocale("cc_starter1");
    strStarterPrice1 = GameSharing::getPriceLocale("cc_starter10");
    strStarterPrice2 = GameSharing::getPriceLocale("cc_starter3");
//#endif
    
    btn = (Button*)layer->getChildByName("btnImage0");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setEnabled(!isBought);
    Node* nd;Sprite* spt;
    for (int i = 0; i < 7; i++) {
        nd = btn->getChildByName(strmake("img%d", i));
        if (i < 3) {
            spt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
            spt->setFlippedX(true);
        }else if (i < 6) {
            spt = Sprite::createWithSpriteFrameName("archerStand0.png");
        }else{
            spt = Sprite::createWithSpriteFrameName("zombieHuman0.png");
        }
        spt->setScale(0.8f);
        spt->setPosition(nd->getPosition());
        btn->addChild(spt);
        nd->removeFromParent();
    }
    lbl = (Text*)btn->getChildByName("lblDesc");
    lbl->setString(strmake("%s*3 + %s*3 + %s + %s*60", LM->getText("swordman").c_str(), LM->getText("archer").c_str(), LM->getText("zombie swordman").c_str(), LM->getText("gem").c_str()));
    
    isBought = UDGetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 1).c_str(), false);
    btn = (Button*)layer->getChildByName("btn1");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setVisible(!isBought);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "buy");
    btn = (Button*)layer->getChildByName("btnImage1");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setEnabled(!isBought);
    for (int i = 0; i < 9; i++) {
        nd = btn->getChildByName(strmake("img%d", i));
        if (i < 3) {
            spt = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
            spt->setFlippedX(true);
        }else if (i < 6) {
            spt = Sprite::createWithSpriteFrameName("orcSpearStand0.png");
        }else{
            spt = Sprite::createWithSpriteFrameName("goblinBombRun0.png");
        }
        spt->setScale(0.8f);
        spt->setPosition(nd->getPosition());
        btn->addChild(spt);
        nd->removeFromParent();
    }
    lbl = (Text*)btn->getChildByName("lblDesc");
    lbl->setString(strmake("%s*3 + %s*3 + \n%s*3 + %s*1000", LM->getText("orc axe").c_str(), LM->getText("orc spear").c_str(), LM->getText("goblin bomb").c_str(), LM->getText("gem").c_str()));
    
    isBought = UDGetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 2).c_str(), false);
    btn = (Button*)layer->getChildByName("btn2");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setVisible(!isBought);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "buy");
    btn = (Button*)layer->getChildByName("btnImage2");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onBuySpecialOffer, this));
    btn->setEnabled(!isBought);
    for (int i = 0; i < 8; i++) {
        nd = btn->getChildByName(strmake("img%d", i));
        if (i < 6) {
            spt = Sprite::createWithSpriteFrameName("archerStand0.png");
        }else if(i == 7){
            spt = Sprite::createWithSpriteFrameName("trollStand0.png");
        }else{
            spt = Sprite::createWithSpriteFrameName("trollAttack1.png");
        }
        spt->setScale(0.8f);
        spt->setPosition(nd->getPosition());
        btn->addChild(spt);
        nd->removeFromParent();
    }
    layer->getChildByName("btnImage2")->getChildByName("lblGemCount")->setLocalZOrder(10);
    layer->getChildByName("btnImage2")->getChildByName("imgGem")->setLocalZOrder(10);
    lbl = (Text*)btn->getChildByName("lblDesc");
    lbl->setString(strmake("%s*6 + %s*2 + %s*200", LM->getText("archer").c_str(), LM->getText("troll").c_str(),  LM->getText("gem").c_str()));
}
void BattleHud::onBuySpecialOffer(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if (isFromLastChance) {
        GameSharing::firebaseLog("specialOffer", "specialOffer purchase", "last chance");
    }else{
        GameSharing::firebaseLog("specialOffer", "specialOffer purchase", "normal");
    }
    if (btn->getTag() == 0) {
        GameSharing::buyItem(IAP_DETAIL_STARTER_1);
    }else if(btn->getTag() == 1){
        GameSharing::buyItem(IAP_DETAIL_STARTER_10);
    }else if(btn->getTag() == 2){
        GameSharing::buyItem(IAP_DETAIL_STARTER_3);
    }
    showIndicator();
}
Node* BattleHud::onBackClick(){
    
    this->unschedule(schedule_selector(BattleHud::update));
    this->unschedule(schedule_selector(BattleHud::updateUI));
    checkUnsaved();
    WORLD->removeUsedAssets();
    BHUD = nullptr;
    Scene* scene = Scene::create();
    Node* title = Title::create();
    scene->addChild(title);
    removeUsedAssets();
    Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
    if(GM->isAdsUser()){
        GameSharing::showInterstitial();
    }
    return title;
}
void BattleHud::onMissionClick(){
    UDSetBool(KEY_DAILY_MISSION_OPEN, true);
    hudLayer->getChildByName("btnMission")->getChildByName("imgRedDot")->setVisible(false);
    Node* layer = CSLoader::createNode("DailyMission.csb");
    this->addChild(layer, 4);
    layer->setName("dailyMission");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    moveTopBarTo(layer, false);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "daily mission");
    lbl = (Text*)layer->getChildByName("lblTimeLeft");
    lbl->setString("Awesome Cartoon Craft!");
    for (int i = 0; i < 4; i++) {
        btn = (Button*)layer->getChildByName(strmake("btn%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onDailyMissionClick, this));
        lbl = (Text*)layer->getChildByName(strmake("lblMission%d", i));
        LM->setLocalizedString(lbl, strmake("daily mission %d", i));
        bool isClear = false;
        if (i == 0) {
            isClear = UDGetBool(KEY_DAILY_MISSION_ARENA, false);
        }else if (i == 1) {
            isClear = UDGetBool(KEY_DAILY_MISSION_CAMPAIGN, false);
        }else if (i == 2) {
            isClear = UDGetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, false);
        }else if (i == 3) {
            isClear = UDGetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, false);
        }
        lbl = (Text*)btn->getChildByName("lbl");
        if (isClear) {
            bool isReceived = UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, i).c_str(), false);
            btn->setVisible(!isReceived);
            LM->setLocalizedString(lbl, "receive");
        }else{
            LM->setLocalizedString(lbl, "challenge");
        }
    }
}
void BattleHud::onDailyMissionClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    int i = btn->getTag();
    bool isClear = false;
    if (i == 0) {
        isClear = UDGetBool(KEY_DAILY_MISSION_ARENA, false);
    }else if (i == 1) {
        isClear = UDGetBool(KEY_DAILY_MISSION_CAMPAIGN, false);
    }else if (i == 2) {
        isClear = UDGetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, false);
    }else if (i == 3) {
        isClear = UDGetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, false);
    }
    
    if (isClear) {
        if(!UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, i).c_str(), false)){
            UDSetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, i).c_str(), true);
            GM->addGem(5);
            BSM->saveUserData(strmake("gem=%d", GM->getGem()));
            btn->setVisible(false);
        }
    }else{
        if(i == 0){
            Title* title = (Title*)onBackClick();
            title->onArenaClick();
        }else if(i == 1){
            Title* title = (Title*)onBackClick();
            title->showChapterSelect();
            title->showStageSelect(0);
            GM->nextScene = STAGE_FIELD;
            int stage = UDGetInt(KEY_DAY_COUNT, 0)%12;
            title->onStageClick(title->getChildByName("stageSelect")->getChildByName("sv")->getChildByTag(stage));
        }else if(i == 2){
            closePopup();
            onBattleClick();
            onNetworkBattleClick(this->getChildByName("battlePopup")->getChildByName("btnFindMatch"));
        }else if(i == 3){
            closePopup();
            onBattleClick();
        }
    }
}
void BattleHud::onDailyRewardClick(){
    Node* layer = CSLoader::createNode("AttendReward.csb");
    this->addChild(layer, 4);
    layer->setName("dailyReward");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    GM->showNode(layer, 0);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "daily reward");
    
    std::string info = UDGetStr(KEY_ATTEND_RCV_INFO, "0_0");
    ValueVector infoData = GM->split(info, "_");
    int dayIndex = infoData.at(0).asInt()%16;
    int nextDayCount = infoData.at(1).asInt();
    int todaysDayCount = UDGetInt(KEY_DAY_COUNT, 0);
    ImageView* img;
    float delay = 0.3f;
    for (int i = 0; i < 16; i++) {
        btn = (Button*)layer->getChildByName(strmake("btn_%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onDailyRewardReceiveClick, this));
        btn->setTag(i);
        lbl = (Text*)btn->getChildByName("lbl");
        lbl->setLocalZOrder(1);
        btn->setEnabled(todaysDayCount >= nextDayCount && i == dayIndex);
        img = (ImageView*)btn->getChildByName("img");
        Sprite* sptUnit = nullptr;
        if (i == 6) {
            sptUnit = Sprite::createWithSpriteFrameName("trollStand0.png");
            sptUnit->setScale(0.5f);
        }else if (i == 13) {
            sptUnit = Sprite::createWithSpriteFrameName("zombieHuman0.png");
            sptUnit->setScale(0.7f);
        }else if (i == 15) {
            sptUnit = Sprite::createWithSpriteFrameName("goblinBombStand0.png");
            sptUnit->setScale(0.7f);
        }
        if(sptUnit != nullptr){
            btn->addChild(sptUnit);
            sptUnit->setPosition(img->getPosition());
            img->removeFromParent();
        }
        if(dayIndex > i){
            Sprite* spt = Sprite::create("check.png");
            btn->addChild(spt);
            spt->setPosition(lbl->getPosition());
            lbl->removeFromParent();
            spt->runAction(Sequence::create(DelayTime::create(delay), ScaleTo::create(0.1f, 0.5f), EaseBackOut::create(ScaleTo::create(0.3f, 1)), nullptr));
            delay += 0.15f;
        }else if(todaysDayCount >= nextDayCount && i == dayIndex){
            LM->setLocalizedString(lbl, "receive");
        }else{
            std::string day = LM->getText("day 1");
            int indexOfOne = day.find("1");
            std::string localizedStrDay = day.replace(indexOfOne, 1, Value(i+1).asString());
            lbl->setString(localizedStrDay);
        }
        lbl->setTextColor(btn->isEnabled()?Color4B::WHITE:Color4B::GRAY);
        lbl = (Text*)btn->getChildByName("lblCount");
        if(lbl) lbl->setTextColor(btn->isEnabled()?Color4B::WHITE:Color4B::GRAY);
        if (sptUnit == nullptr) {
            ImageView* img = (ImageView*)btn->getChildByName("img");
            img->setColor(btn->isEnabled()?Color3B::WHITE:Color3B::GRAY);
        }else{
            sptUnit->setColor(btn->isEnabled()?Color3B::WHITE:Color3B::GRAY);
        }
        
        if (btn->isEnabled()) {
            btn->runAction(Sequence::create(DelayTime::create(delay), ScaleTo::create(0.1f, 0.5f), EaseBackOut::create(ScaleTo::create(0.3f, 1)), nullptr));
            btn->runAction(Sequence::create(DelayTime::create(delay), EaseBackOut::create(RotateBy::create(0.5f, 360)), nullptr));
        }
    }
}
void BattleHud::onDailyRewardReceiveClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    int index = btn->getTag();
    std::vector<int> datas;
    if (index == 6 || index == 13 || index == 15) {
        if(unitInfoListInventory.size() >= inventoryCountMax){
            showInstanceMessage(LM->getText("inventory full"));
            return;
        }
        UnitInfo* info = new UnitInfo();
        if (index == 6) {
            info->unitType = UNIT_TROLL;
        }else if (index == 13) {
            info->unitType = UNIT_ZOMBIE_SWORDSMAN;
        }else{
            info->unitType = UNIT_GOBLIN_BOMB;
        }
        unitInfoListInventory.push_back(info);
        
        updateInventorySaveData();
        datas.push_back(DATA_TYPE_INVENTORY);
    }else{
        GM->addGem(10);
        datas.push_back(DATA_TYPE_GEM);
//        BSM->saveUserData(strmake("gem=%d&attend=%s", GM->getGem(), strAttend.c_str()));
    }
    std::string info = UDGetStr(KEY_ATTEND_RCV_INFO, "0_0");
    ValueVector infoData = GM->split(info, "_");
    int dayIndex = infoData.at(0).asInt();
    dayIndex++;
    dayIndex = dayIndex%16;
    int todaysDayCount = UDGetInt(KEY_DAY_COUNT, 0);
    std::string strAttend = strmake("%d_%d", dayIndex, todaysDayCount+1);
    UDSetStr(KEY_ATTEND_RCV_INFO, strAttend.c_str());
    closePopup();
    onDailyRewardClick();
    
    saveUserData(datas);
}
void BattleHud::onCollectGoldClick(int amount){
    
}
void BattleHud::onPopulationClick(){
    
}
void BattleHud::onCompleteClick(){
    if(selectedUnit == nullptr) return;
    int price = Value(((Text*)btnComplete->getChildByName("lblCount"))->getString()).asInt();
    if (GM->getGem() >= price) {
        GM->addGem(-price);
        selectedUnit->buildingCompleteTimeLeft = 0;
        if(selectedUnit->unitType == UNIT_TREE_FOR_BATTLE){
            WORLD->deselectAll();
        }
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
    }
}
void BattleHud::onUpgradeClick(){
    showInfoPopup(true);
}
void BattleHud::onUpgradeConfirmClick(){
    if (!isWorkerAvailable()) {
        showNotEnoughWorkerUseGem();
        return;
    }
    int goldPrice = getUnitUpgradePriceGold(selectedUnit->unitType, selectedUnit->level);
    int treePrice = getUnitUpgradePriceTree(selectedUnit->unitType, selectedUnit->level);
    if(GM->getCoin() < goldPrice){
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    if(GM->getTree() < treePrice){
        showInstanceMessage(LM->getText("not enough lumber"));
        return;
    }
    
    if (selectedUnit->level >= 19) {
        showInstanceMessage(LM->getText("max level alert"));
        return;
    }
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
    if (castleLevel <= selectedUnit->level && selectedUnit->unitType != UNIT_CASTLE) {
        showInstanceMessage(LM->getText("upgrade castle alert"));
        return;
    }
    
    GM->addCoin(-goldPrice);
    GM->addTree(-treePrice);
    
    selectedUnit->unitState = UNIT_STATE_UPGRADE;
    selectedUnit->buildingCompleteTimeLeft = getUnitCreatingTime(selectedUnit->unitType, selectedUnit->level+1);
    selectedUnit->buildingCompleteTime = (double)BSM->getCurrentTimeT() + selectedUnit->buildingCompleteTimeLeft;
    
    selectedUnit->energy = 1;
    selectedUnit->maxEnergy = 100;
    selectedUnit->updateEnergy();
    Text* lblTimeLeft = Text::create("Upgrade...", "BMDOHYEON.ttf", 20);
    lblTimeLeft->enableOutline(Color4B::BLACK, 2);
    WORLD->addChild(lblTimeLeft, selectedUnit->getParent()->getLocalZOrder()+1);
    lblTimeLeft->setPosition(selectedUnit->getPosition() + Point(0, 60));
//    timeLeftLabels.pushBack(lblTimeLeft);
    selectedUnit->lblTimeLeft = lblTimeLeft;
    addUpgradeEffect(selectedUnit, true);
    closePopup();
    
//    currentJob = JOB_NONE;
//    hideCancelButton();
    onUnitSelected(selectedUnit);
//    WORLD->deselectAll();
//    WORLD->selectUnit((EnemyBase*)selectedUnit);
    
    updateBuildingsSaveData();
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_TREE);
    datas.push_back(DATA_TYPE_BUILDING);
    saveUserData(datas);
    if(selectedUnit->unitType == UNIT_MINE){
        Node* ndMineButton = WORLD->getChildByName(strmake("newminebutton%d_%d", (int)selectedUnit->getPositionX()/TILE_SIZE, (int)selectedUnit->getPositionY()/TILE_SIZE));
        ndMineButton->setVisible(false);
    }
    if(selectedUnit->unitType == UNIT_LUMBERMILL){
        Node* ndTreeButton = WORLD->getChildByName(strmake("treeButton%d_%d", (int)selectedUnit->getPositionX()/TILE_SIZE, (int)selectedUnit->getPositionY()/TILE_SIZE));
        ndTreeButton->setVisible(false);
    }
}
void BattleHud::onInfoClick(){
    showInfoPopup(false);
}
void BattleHud::showInfoPopup(bool isUpgrade){
    Node* layer = CSLoader::createNode("CastleInfo.csb");
    this->addChild(layer, 4);
    layer->setName("infoLayer");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnUpgrade");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onUpgradeConfirmClick, this));
    btn->setVisible(isUpgrade);
    Text* lbl = (Text*)btn->getChildByName("lblGold");
    lbl->setString(Value(getUnitUpgradePriceGold(selectedUnit->unitType, selectedUnit->level)).asString());
    lbl = (Text*)btn->getChildByName("lblTree");
    lbl->setString(Value(getUnitUpgradePriceTree(selectedUnit->unitType, selectedUnit->level)).asString());
    
    Node* nd = layer->getChildByName("ndUpgradeTime");
    nd->setVisible(isUpgrade);
    nd->setLocalZOrder(100);
    lbl = (Text*)nd->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade time");
    lbl = (Text*)nd->getChildByName("lblTime");
    lbl->setString(GM->getTimeLeftInString(getUnitUpgradeTime(selectedUnit->unitType, selectedUnit->level)));
    
    layer->getChildByName("ndCastle")->setVisible(false);
    layer->getChildByName("ndFarm")->setVisible(false);
    layer->getChildByName("ndAttack")->setVisible(false);
    layer->getChildByName("ndMine")->setVisible(false);
    layer->getChildByName("ndPopulation")->setVisible(false);
    lbl = (Text*)layer->getChildByName("lblHP");
    lbl->setString(Value(GM->getUnitHP(selectedUnit->unitType, selectedUnit->level)).asString());
    lbl = (Text*)layer->getChildByName("lblLevel");
    lbl->setString(strmake("Lv.%d", selectedUnit->level+1));
    lbl = (Text*)layer->getChildByName("lblName");
    lbl->setString(LM->getText(GM->getUnitName(selectedUnit->unitType)));
    
    Sprite* spt;
    if(selectedUnit->unitType == UNIT_CASTLE){
        layer->getChildByName("ndCastle")->setVisible(true);
        Node* nd = layer->getChildByName("ndCastle");
        lbl = (Text*)nd->getChildByName("lblPopulationMax");
        lbl->setString(Value(getFoodProvide(UNIT_CASTLE)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldMax");
        int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
        lbl->setString(Value(getMaxGold(castleLevel)).asString());
        lbl = (Text*)nd->getChildByName("lblTreeMax");
        lbl->setString(Value(getMaxTree(castleLevel)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getMaxGold(castleLevel + 1) - getMaxGold(castleLevel)).asString());
        lbl->setPositionX(nd->getChildByName("lblGoldMax")->getPositionX() + nd->getChildByName("lblGoldMax")->getContentSize().width + 20);
        lbl = (Text*)nd->getChildByName("lblTreeAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getMaxTree(castleLevel + 1) - getMaxTree(castleLevel)).asString());
        lbl->setPositionX(nd->getChildByName("lblTreeMax")->getPositionX() + nd->getChildByName("lblTreeMax")->getContentSize().width + 20);
    }else if(selectedUnit->unitType == UNIT_FARM){
        layer->getChildByName("ndFarm")->setVisible(true);
        layer->getChildByName("ndFarm")->setPosition(Vec2::ZERO);
        lbl = (Text*)layer->getChildByName("ndFarm")->getChildByName("lblPopulation");
        lbl->setString(Value(getFoodProvide(UNIT_FARM)).asString());
    }else if(selectedUnit->unitType == UNIT_WATCHERTOWER || selectedUnit->unitType == UNIT_ORC_BUNKER){
        layer->getChildByName("ndAttack")->setVisible(true);
        lbl = (Text*)layer->getChildByName("ndAttack")->getChildByName("lblAttack");
        lbl->setString(Value(GM->getUnitATT(selectedUnit->unitType, selectedUnit->level)).asString());
    }else if(selectedUnit->unitType == UNIT_MINE){
        nd = layer->getChildByName("ndMine");
        nd->setPosition(Vec2::ZERO);
        nd->setVisible(true);
        
        Sprite* spt = Sprite::createWithSpriteFrameName("pickax.png");
        nd->addChild(spt);
        spt->setPosition(nd->getChildByName("imgGold")->getPosition() + Vec2(30, -30));
        spt->setScale(0.6f);
        lbl = (Text*)nd->getChildByName("lblGoldMax");
        lbl->setString(Value(getMineGoldStorage(selectedUnit->level)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getMineGoldStorage(selectedUnit->level + 1) - getMineGoldStorage(selectedUnit->level)).asString());
        lbl->setPositionX(nd->getChildByName("lblGoldMax")->getPositionX() + nd->getChildByName("lblGoldMax")->getContentSize().width + 20);
        
        lbl = (Text*)nd->getChildByName("lblGoldPerHour");
        lbl->setString(Value(getMineGoldPerHour(selectedUnit->level)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldPerHourAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getMineGoldPerHour(selectedUnit->level + 1) - getMineGoldPerHour(selectedUnit->level)).asString());
        lbl->setPositionX(nd->getChildByName("lblGoldPerHour")->getPositionX() + nd->getChildByName("lblGoldPerHour")->getContentSize().width + 20);
        
    }else if(selectedUnit->unitType == UNIT_LUMBERMILL){
        nd = layer->getChildByName("ndMine");
        nd->setPosition(Vec2::ZERO);
        nd->setVisible(true);
        
        ImageView* img = (ImageView*)nd->getChildByName("imgGold");
        img->loadTexture("lumberIcon.png");
        img->setContentSize(Size(78, 92));
        
        img = (ImageView*)nd->getChildByName("imgGoldStorage");
        img->loadTexture("lumberIcon.png");
        img->setContentSize(Size(78, 92));
        
        Sprite* spt = Sprite::createWithSpriteFrameName("pickax.png");
        nd->addChild(spt);
        spt->setPosition(nd->getChildByName("imgGold")->getPosition() + Vec2(30, -30));
        spt->setScale(0.6f);
        lbl = (Text*)nd->getChildByName("lblGoldMax");
        lbl->setString(Value(getLumbermillTreeStorage(selectedUnit->level)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getLumbermillTreeStorage(selectedUnit->level + 1) - getLumbermillTreeStorage(selectedUnit->level)).asString());
        lbl->setPositionX(nd->getChildByName("lblGoldMax")->getPositionX() + nd->getChildByName("lblGoldMax")->getContentSize().width + 20);
        
        lbl = (Text*)nd->getChildByName("lblGoldPerHour");
        lbl->setString(Value(getLumbermillTreePerHour(selectedUnit->level)).asString());
        lbl = (Text*)nd->getChildByName("lblGoldPerHourAdd");
        lbl->setVisible(isUpgrade);
        lbl->setString("+" + Value(getLumbermillTreePerHour(selectedUnit->level + 1) - getLumbermillTreePerHour(selectedUnit->level)).asString());
        lbl->setPositionX(nd->getChildByName("lblGoldPerHour")->getPositionX() + nd->getChildByName("lblGoldPerHour")->getContentSize().width + 20);
        
    }else if(!selectedUnit->isBuilding){
        nd = layer->getChildByName("ndAttack");
        nd->setVisible(true);
        lbl = (Text*)nd->getChildByName("lblAttack");
        lbl->setString(Value(GM->getUnitATT(selectedUnit->unitType, selectedUnit->level)).asString());
        nd = layer->getChildByName("ndPopulation");
        nd->setVisible(true);
        lbl = (Text*)nd->getChildByName("lblPopulation");
        lbl->setString(Value(GM->getFoodUseForUnit(selectedUnit->unitType)).asString());
    }
    spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(selectedUnit->unitType));
    
    lbl = (Text*)layer->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, strmake("%s desc", GM->getUnitName(selectedUnit->unitType).c_str()));
    
    layer->addChild(spt);
    spt->setPosition(layer->getChildByName("imgIconBack")->getPosition());
}
int BattleHud::getFoodProvide(int unitType){
    if(unitType == UNIT_CASTLE){
        return 8;
    }else if(unitType == UNIT_FARM){
        return 6;
    }
    return 0;
}
int BattleHud::getBuildingIndex(int unit){
    switch (unit) {
        case UNIT_CASTLE:
            return BUILDING_CASTLE;
        case UNIT_MINE:
            return BUILDING_MINE;
        case UNIT_TREE_FOR_BATTLE:
            return BUILDING_TREE;
        case UNIT_FARM:
            return BUILDING_FARM;
        case UNIT_BARRACKS:
            return BUILDING_BARRACKS;
        case UNIT_LUMBERMILL:
            return BUILDING_LUMBURMILL;
        case UNIT_WATCHERTOWER:
            return BUILDING_WATCHER_TOWER;
//        case UNIT_UNDERGROUND_BUNKER:
//            return BUILDING_UNDERGROUND_BUNKER;
//        case UNIT_TRIGGER:
//            return BUILDING_TRIGGER;
        case UNIT_FACTORY:
            return BUILDING_FACTORY;
        case UNIT_AIRPORT:
            return BUILDING_AIRPORT;
        case UNIT_ORC_HQ:
            return BUILDING_ORC_HQ;
        case UNIT_ORC_BUNKER:
            return BUILDING_ORC_BUNKER;
        case UNIT_ORC_BARRACKS:
            return BUILDING_ORC_BUNKER;
        case UNIT_ORC_TROLL_HOUSE:
            return BUILDING_ORC_TROLL_HOUSE;
        default:
            break;
    }
}
void BattleHud::onUnitSelected(Movable* unit){
    Vector<Node*> btnsToShow;
    if(unit != nullptr){
        selectedUnit = unit;
        btnsToShow.pushBack(hudLayer->getChildByName("btnInfo"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnUpgrade"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnComplete"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnTrain"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnCutTree"));
        for(auto btn : btnsToShow){
            btn->setPosition(Vec2(btn->getPositionX(), -btn->getContentSize().height*2));
        }
        btnsToShow.clear();
        
        if(unit->unitType == UNIT_TREE_FOR_BATTLE){
            if(unit->unitState == UNIT_STATE_IDLE){
                Button* btn = (Button*)hudLayer->getChildByName("btnCutTree");
                btnsToShow.pushBack(btn);
                Text* lbl = (Text*)btn->getChildByName("lblGold");
                int goldPrice = 10;
                for (int i = 0; i < buildings.size(); i++) {
                    if(buildings.at(i)->unitType == UNIT_LUMBERMILL){
                        goldPrice += 10;
                    }
                }
                lbl->setString(Value(goldPrice).asString());
            }
        }else{
            btnsToShow.pushBack(hudLayer->getChildByName("btnInfo"));
        }
        
        bool isInUpgradeOrBuilding = false;
        float completeTime = 0;
        for(auto building : buildings){
            if(building == unit){
                isInUpgradeOrBuilding = building->unitState == UNIT_STATE_BUILDING || building->unitState == UNIT_STATE_UPGRADE || building->unitState == UNIT_STATE_REMOVING;
                completeTime = building->buildingCompleteTimeLeft;
                break;
            }
        }
        if(isInUpgradeOrBuilding){
            btnsToShow.pushBack(hudLayer->getChildByName("btnComplete"));
            updateUI(0);
        }else{
            if(unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_MINE || unit->unitType == UNIT_LUMBERMILL || unit->unitType == UNIT_WATCHERTOWER){
                Node* btn = hudLayer->getChildByName("btnUpgrade");
                btnsToShow.pushBack(btn);
                int goldPrice = getUnitUpgradePriceGold(selectedUnit->unitType, selectedUnit->level);
                Text* lbl = (Text*)btn->getChildByName("lblGold");
                lbl->setString(Value(goldPrice).asString());
                lbl->setColor(GM->getCoin()>=goldPrice?Color3B::WHITE:Color3B::RED);
                int treePrice = getUnitUpgradePriceTree(selectedUnit->unitType, selectedUnit->level);
                lbl = (Text*)btn->getChildByName("lblTree");
                lbl->setString(Value(treePrice).asString());
                lbl->setColor(GM->getTree()>=treePrice?Color3B::WHITE:Color3B::RED);
            }else if(unit->unitType == UNIT_BARRACKS){
                btnsToShow.pushBack(hudLayer->getChildByName("btnTrain"));
            }
        }
        
        int count = 0;
        int gapX = 300;
        for(auto btn : btnsToShow){
            btn->stopAllActions();
            btn->setPosition(Vec2(size.width/2 - (btnsToShow.size()-1)*gapX/2 + gapX*count, btn->getContentSize().height/2 + 20 - btn->getContentSize().height*2));
            btn->runAction(EaseBackOut::create(MoveBy::create(0.3f, Vec2(0, btn->getContentSize().height*2))));
            count++;
        }
    }else{
        btnsToShow.pushBack(hudLayer->getChildByName("btnInfo"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnUpgrade"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnComplete"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnTrain"));
        btnsToShow.pushBack(hudLayer->getChildByName("btnCutTree"));
        for(auto btn : btnsToShow){
            btn->runAction(EaseBackOut::create(MoveBy::create(0.3f, Vec2(0, -btn->getContentSize().height*2))));
        }
        
        log("unit selected null");
    }
    btnsToShow.clear();
}
PPLabel* BattleHud::showInstanceMessage(std::string msg){
    this->removeChildByTag(77);
    this->removeChildByTag(78);
    float dur = 0.3f;
    float distanceToMove = 160;
    
    PPLabel* lbl = PPLabel::create(msg, 60, Color3B::WHITE, true, false, TextHAlignment::CENTER, true);
    this->addChild(lbl, 200 + popupArray.size() + 2);
    lbl->setTag(77);
    lbl->setPosition(Point(size.width/2, -TILE_SIZE/2));
    lbl->runAction(Sequence::create(MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(2), MoveBy::create(dur, Point(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, lbl)), nullptr));
    
    
    Sprite* sptBack = Sprite::create("messageBack.png");
    sptBack->setPosition(Point(size.width/2, -TILE_SIZE/2));
    sptBack->setScale((size.width-400)/sptBack->getContentSize().width, (120)/sptBack->getContentSize().height);
    sptBack->setTag(78);
//    sptBack->setColor(Color3B::BLACK);
//    sptBack->setOpacity(150);
    sptBack->runAction(Sequence::create(MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(2), MoveBy::create(dur, Point(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, sptBack)), nullptr));
    this->addChild(sptBack, 200 + popupArray.size() + 1);
    
    return lbl;
}
//void BattleHud::saveInventoryAndDeck(bool includeIap){
void BattleHud::updateInventorySaveData(){
    std::string strInventory = "_";
    int counter = 0;
    for(auto unitInfo: unitInfoListInventory){
//        strInventory += strmake("%d/%d/%d/%d/%d/%d_",unitInfo->unitType, unitInfo->level, (int)unitInfo->x, (int)unitInfo->y, (int)unitInfo->endTime, unitInfo->unitState);
        strInventory += strmake("%d/%d_",unitInfo->unitType, unitInfo->level);
        counter++;
        if(counter >= inventoryCountMax){
            break;
        }
    }
//    if (unitInfoListDeck.size() > 0) {
        UDSetStr(KEY_UNITS_INVENTORY, strInventory);
//    }
    
}
void BattleHud::updateDeckSaveData(){
    std::string strDeck = "_";
    for(auto unitInfo: unitInfoListDeck){
        strDeck += strmake("%d/%d/%d/%d_",unitInfo->unitType, unitInfo->level, (int)unitInfo->x, (int)unitInfo->y);
    }
    
//    if(unitInfoListDeck.size() > 0){
        UDSetStr(KEY_UNITS_DECK, strDeck);
//    }
}
void BattleHud::saveUserData(std::vector<int>& datas){
    std::string strData = "";
    for(int i = 0; i < datas.size();i++){
        if (datas.at(i) == DATA_TYPE_GEM) {
            strData = strmake("%s&gem=%d", strData.c_str(), GM->getGem());
        }else if (datas.at(i) == DATA_TYPE_GOLD) {
            strData = strmake("%s&gold=%d", strData.c_str(), GM->getCoin());
        }else if (datas.at(i) == DATA_TYPE_TREE) {
            strData = strmake("%s&tree=%d", strData.c_str(), GM->getTree());
        }else if (datas.at(i) == DATA_TYPE_BUILDING) {
            strData = strmake("%s&buildings=%s", strData.c_str(), UDGetStr(KEY_BUILDINGS, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_INVENTORY) {
            strData = strmake("%s&inventory=%s", strData.c_str(), UDGetStr(KEY_UNITS_INVENTORY, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_DECK) {
            strData = strmake("%s&deck=%s", strData.c_str(), UDGetStr(KEY_UNITS_DECK, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_IAP) {
            strData = strmake("%s&iap_list=%s&iap_total=%d", strData.c_str(), UDGetStr(KEY_IAP_LIST, "").c_str(), UDGetInt(KEY_IAP_TOTAL, 0));
        }else if (datas.at(i) == DATA_TYPE_SEARCH_STATE) {
            strData = strmake("%s&search_state=%s", strData.c_str(), UDGetStr(KEY_SEARCH_STATE, "000").c_str());
        }
    }
    strData = strData.substr(1); // remove first &
    BSM->saveUserData(strData);
    log("save data: %s", strData.c_str());
    datas.clear();
}
void BattleHud::updateBuildingsSaveData(){
    std::string strEquipped = "";
    for(auto unit: buildings){
        if (unit->unitState == UNIT_STATE_IDLE) {
            strEquipped += strmake("%d/%d/%d/%d_",unit->unitType, unit->level, (int)unit->getPositionX(), (int)unit->getPositionY());
        }else{
            strEquipped += strmake("%d/%d/%d/%d/%d/%d_",unit->unitType, unit->level, (int)unit->getPositionX(), (int)unit->getPositionY(), (int)unit->buildingCompleteTime, unit->unitState);
        }
        
    }
    log("strEquipped: %s", strEquipped.c_str());
    if (buildings.size() > 2) {
        UDSetStr(KEY_BUILDINGS, strEquipped);
    }
    
//    BSM->saveUserData("buildings=" + strEquipped + "&last_launch_date=" + Value(BSM->getCurrentTime()).asString() + "&gem=" + Value(GM->getGem()).asString() + "&gold=" + Value(GM->getCoin()).asString() + "&tree=" + Value(GM->getTree()).asString());
}
void BattleHud::loadData(){
//    UDSetStr(KEY_BUILDINGS, ""); // test
    mapSize = WORLD->theMap->getMapSize();
    std::string strEquipped = UDGetStr(KEY_BUILDINGS, "");
    log("strEquipped: %s", strEquipped.c_str());
    Text* lbl = (Text*)hudLayer->getChildByName("btnTrophy")->getChildByName("lbl");
    lbl->setString(Value(UDGetInt(KEY_TROPHY, 1000)).asString());
    if(strEquipped.length() == 0 && GM->getCoin() == 0 && GM->getTree() == 0 && GM->getGem() == 0){
        Vec2 pos = Vec2(1300, 1400);
        Vec2 sptPos = pos + Vec2(3*TILE_SIZE/2, 3*TILE_SIZE/2);
        Vec2 occupyPos = pos + Vec2(0, TILE_SIZE*3);
        EnemyBase* unit = WORLD->createUnit(UNIT_CASTLE, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "castle", 1, "castle.png");
        WORLD->setOccupy(occupyPos, 4, 3, true, unit);
        buildings.pushBack(unit);
        
        pos = Vec2(1600, 1700);
        sptPos = pos + Vec2(3*TILE_SIZE/2, 3*TILE_SIZE/2);
        occupyPos = pos + Vec2(0, TILE_SIZE*3);
        unit = WORLD->createUnit(UNIT_MINE, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "mine.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
        buildings.pushBack(unit);
        setupBuilding(unit);
        
        if (!UDGetBool(KEY_BATTLE_TUTORIAL_DONE, false)) {
            tutorialNode = Node::create();
            this->addChild(tutorialNode, 200);
            
            ImageView* img = ImageView::create("uiBox.png");
            tutorialNode->addChild(img);
            img->setScale9Enabled(true);
            img->setTouchEnabled(true);
            img->setContentSize(Size(2000, 500));
            img->setPosition(Point(size.width/2, 330));
            img->addClickEventListener(CC_CALLBACK_0(BattleHud::onTutorialBoxClick, this));
            
            Label* lbl = LM->getLocalizedLabel("", Color4B::BLACK, 80);
            tutorialNode->addChild(lbl);
            lbl->setPosition(img->getPosition() + Point(200, 0));
            lbl->setDimensions(1500, 450);
            lbl->setName("lbl");
            lbl->setVerticalAlignment(TextVAlignment::CENTER);
            
            talkIndex = 0;
            tutorialIndex = -1;
            onTutorialBoxClick();
            this->schedule(schedule_selector(BattleHud::talkBoxUpdate), 0.1f);
        }
        
//        for (int y = 29; y >= 22; y--) {
//            int x = 10;
//            addTreeToMap(x, y);
//            x = 21;
//            addTreeToMap(x, y);
//        }
//        for (int i = 0; i < 4; i++) {
//            int y = 21 - i;
//            int x = 9 - i;
//            addTreeToMap(x, y);
//            x = 22 + i;
//            addTreeToMap(x, y);
//        }
        for (int y = 17; y >= 5; y--) {
            int x = 5;
            addTreeToMap(x, y);
            x = 26;
            addTreeToMap(x, y);
        }
//        for (int i = 0; i < 3; i++) {
//            int y = 4 - i;
//            int x = 6 + i;
//            addTreeToMap(x, y);
//            x = 25 - i;
//            addTreeToMap(x, y);
//        }
//        for (int x = 9; x < 23; x++) {
//            addTreeToMap(x, 1);
//        }
        std::vector<int> datas;
        updateBuildingsSaveData();
        datas.push_back(DATA_TYPE_BUILDING);
        saveUserData(datas);
        return;
    }
    
    bool isCastleExist = false;
    ValueVector units = GM->split(strEquipped, "_");
    for(int i = 0; i < units.size(); i++){
        ValueVector datas = GM->split(units.at(i).asString(), "/");
        if(datas.size() > 1){
            int unitType = Value(datas.at(0)).asInt();
            
            Size occupySize = WORLD->getBuildingOccupySize(unitType);
            Vec2 sptPos = Vec2(Value(datas.at(2)).asInt(), Value(datas.at(3)).asInt());
            int tileIndex = getTileIndexForPos(sptPos);
            Vec2 tilePos = getPositionForTileIndex(tileIndex);
            sptPos = tilePos;
            Vec2 pos = sptPos - Vec2(occupySize.width*TILE_SIZE/2, occupySize.height*TILE_SIZE/2);
            Vec2 occupyPos = pos + Vec2(TILE_SIZE*(((int)occupySize.width)%2), TILE_SIZE*occupySize.height);
            EnemyBase* unit = WORLD->createUnit(unitType, WHICH_SIDE_HERO, isBuilding(unitType)?ITS_BUILDING:ITS_NOT_BUILDING, sptPos, GM->getUnitName(unitType), 1, WORLD->getSpriteNameForUnit(unitType));
            UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info->unitType == UNIT_CASTLE){
                UDSetInt(KEY_CASTLE_LEVEL, info->level);
                inventoryCountMax = 50 + info->level*2;
                isCastleExist = true;
            }
            if (unit->isBuilding) {
                unit->setPosition(unit->getPosition() + Vec2(TILE_SIZE*0.5f*(((int)occupySize.width)%2), TILE_SIZE*0.5f*(((int)occupySize.height)%2)));
                unit->buildingCompleteTime = info->endTime;
                unit->buildingCompleteTimeLeft = unit->buildingCompleteTime - (double)BSM->getCurrentTimeT();
                unit->unitState = info->unitState;
                setupBuilding(unit);
                unit->level = info->level;
                WORLD->setOccupy(occupyPos, occupySize.width, occupySize.height, true, unit);
                
                buildings.pushBack(unit);
                if (info->unitState == UNIT_STATE_BUILDING || info->unitState == UNIT_STATE_UPGRADE || unit->unitState == UNIT_STATE_REMOVING) {
                    unit->energy = 1;
                    unit->maxEnergy = 100;
                    unit->updateEnergy();
                    Text* lblTimeLeft = Text::create("Upgrade...", "BMDOHYEON.ttf", 20);
                    lblTimeLeft->enableOutline(Color4B::BLACK, 2);
                    WORLD->addChild(lblTimeLeft, unit->getParent()->getLocalZOrder()+1);
                    lblTimeLeft->setPosition(unit->getPosition() + Point(0, 60));
//                    timeLeftLabels.pushBack(lblTimeLeft);
                    unit->lblTimeLeft = lblTimeLeft;
                    if(unit->unitState == UNIT_STATE_UPGRADE){
                        addUpgradeEffect(unit, true);
//                        unit->buildingCompleteTime = info->endTime;// getUnitCreatingTime(unit->unitType, unit->level+1);
                    }else if(unit->unitState == UNIT_STATE_BUILDING){
                        if(unit->unitType == UNIT_TREE_FOR_BATTLE){
                            unit->setScale(0.5f);
                            unit->runAction(ScaleTo::create(unit->buildingCompleteTimeLeft, 1));
                            GM->makeItSiluk(unit);
                        }else{
                            unit->runAnimation("underConstrunction", true);
                        }
//                        unit->buildingCompleteTime = info->endTime;//getUnitCreatingTime(unit->unitType, unit->level);
                    }else if(unit->unitState == UNIT_STATE_REMOVING){
//                        unit->buildingCompleteTime = 60;
                    }
                    if (unit->unitState != UNIT_STATE_IDLE) {
                        unit->buildingCompleteTime = info->endTime;
                        unit->buildingCompleteTimeLeft = info->endTime - (double)BSM->getCurrentTimeT();
                    }
                }
            }else{
                setupNonBuilding(unit);
            }
        }
    }
    
    if (!isCastleExist) {
//        Vec2 pos = Vec2(1300, 1400);
//        Vec2 sptPos = pos + Vec2(3*TILE_SIZE/2, 3*TILE_SIZE/2);
//        Vec2 occupyPos = pos + Vec2(0, TILE_SIZE*3);
//        EnemyBase* unit = WORLD->createUnit(UNIT_CASTLE, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "castle", 1, "castle.png");
//        WORLD->setOccupy(occupyPos, 4, 3, true, unit);
//        buildings.pushBack(unit);
//        GM->addCoin(100);
//        GM->addTree(100);
//        std::vector<int> datas;
//        datas.push_back(DATA_TYPE_GOLD);
//        datas.push_back(DATA_TYPE_TREE);
//        saveUserData(datas);
    }
    
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
    inventoryCountMax = 50 + castleLevel*2;
    
    strEquipped = UDGetStr(KEY_UNITS_INVENTORY, "");
    units = GameManager::getInstance()->split(strEquipped, "_");
    std::string str;
    int counter = 0;
    for(int i = 0; i < units.size(); i++){
        str = units.at(i).asString();
        if(str.length() > 0){
            UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info->unitType < 0){
                continue;
            }
            unitInfoListInventory.push_back(info);
            if (counter >= inventoryCountMax) {
                break;
            }
        }
    }
    strEquipped = UDGetStr(KEY_UNITS_DECK, "");
    units = GameManager::getInstance()->split(strEquipped, "_");
    int extraX = -100;
    counter = 0;
    for(int i = 0; i < units.size(); i++){
        if(units.at(i).asString().length() > 0){
            Movable* unit = getUnitFromData(units.at(i).asString());
            if(unit != nullptr){
                UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
//                for (auto building:buildings) {
//                    if(building->unitType == UNIT_BARRACKS){
//                        unit->setPosition(building->getPosition() + Vec2(extraX, -200));
//                        info->x = unit->getPositionX();
//                        info->y = unit->getPositionY();
//                        extraX+=50;
//                        counter++;
//                        if(counter > 6){
//                            counter = 0;
//                            extraX = -100;
//                        }
//                        break;
//                    }
//                }
                unit->setPosition(Vec2(info->x, info->y));
                WORLD->addUnit(unit, true);
                unitsDeck.pushBack(unit);
                
//                info->belongTo = BELONG_TO_DECK;
//                info->index = i;
                unitInfoListDeck.push_back(info);
            }
        }
    }
    updateUnitInfoInList();
    WORLD->resetPathState();
    
    // worker
    int availableWorkerCount = 0;
    std::string strSpecialUnits = UDGetStr(KEY_SPECIAL_UNITS, "1000");
    int current, max;
    for(int i = 0; i < 4; i++){
        if(i == 1){
            current = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1;
            max = 12;
        }else if(i == 2){
            current = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
            max = 20;
        }else if(i == 3){
            current = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1 - 12;
            max = 12;
        }
        if (i == 0 || current >= max || strSpecialUnits.at(i) == '1') {
            availableWorkerCount++;
        }
    }
    
    workerCount = availableWorkerCount;
    if (UDGetBool(KEY_WORKER_BOUGHT, false)) {
        workerCount = 4;
    }
    updateAvailableWorkerCount();
}
int BattleHud::getTileIndexForPos(Vec2 pos){
    int tileIndex = (int)pos.x/TILE_SIZE + ((int)pos.y/(int)TILE_SIZE)*(int)mapSize.width;
    return tileIndex;
}
Vec2 BattleHud::getPositionForTileIndex(int index){
    Vec2 pos = Vec2((index%(int)mapSize.width)*TILE_SIZE, (index/(int)mapSize.height)*TILE_SIZE);
    return pos;
}
void BattleHud::updateAvailableWorkerCount(){
    int buildingsInProcess = 0;
    for(auto building: buildings){
        if(building->unitState == UNIT_STATE_BUILDING || building->unitState == UNIT_STATE_UPGRADE || building->unitState == UNIT_STATE_REMOVING){
            buildingsInProcess++;
        }
    }
    availableWorkerCount = workerCount - buildingsInProcess;
    Text* lbl = (Text*)ndTopBar->getChildByName("lblWorker");
    lbl->setString(strmake("%d/%d", availableWorkerCount, workerCount));
}
bool BattleHud::isWorkerAvailable(){
    updateAvailableWorkerCount();
    return availableWorkerCount > 0;
}
void BattleHud::addTreeToMap(int x, int y){
    Vec2 occupyPos = Vec2(x, y);
    EnemyBase* unit = WORLD->createUnit(UNIT_TREE_FOR_BATTLE, WHICH_SIDE_HERO, ITS_BUILDING, Vec2(x*TILE_SIZE + TILE_SIZE/2, (mapSize.height-y)*TILE_SIZE + TILE_SIZE/2), "tree", 1, strmake("tree3_%d.png", rand()%5));
    WORLD->setOccupy(occupyPos, 1, 1, true, unit);
    buildings.pushBack(unit);
    setupBuilding(unit);
}

Movable* BattleHud::getUnitFromData(std::string str){
//    ValueVector datas = GM->split(str, "/");
    UnitInfo* info = GM->getUnitInfoFromString(str);
//    if(datas.size() > 1){
    if(info->unitType >= 0){
        int unitType = info->unitType;//Value(datas.at(0)).asInt();
        Vec2 pos = Vec2(info->x, info->y);//Vec2(Value(datas.at(2)).asInt(), Value(datas.at(3)).asInt());
        int eng = GM->getUnitHP(info->unitType, info->level);//datas.at(0).asInt(), datas.at(1).asInt());
        int spd = WORLD->getUnitSP(info->unitType);//datas.at(0).asInt());
        EnemyBase* unit = EnemyBase::createEnemy(unitType, eng, spd, WORLD->getSpriteNameForUnit(unitType).c_str());
        unit->setName(GM->getUnitName(unitType));
        unit->unitName = GM->getUnitName(unitType);
        unit->level = info->level;//datas.at(1).asInt();
        unit->buildingCompleteTime = info->endTime;//Value(datas.at(4)).asInt();
        unit->buildingCompleteTimeLeft = unit->buildingCompleteTime - (double)BSM->getCurrentTimeT();
        
        return unit;
    }else{
        return nullptr;
    }
}

void BattleHud::setupBuilding(Movable* unit){
    if(unit->unitType == UNIT_MINE){
        Node* ndMineButton = CSLoader::createNode("MineButton.csb");
        Button* btn = (Button*)ndMineButton->getChildByName("btn");
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onMineButtonClick, this));
        WORLD->addChild(ndMineButton, 200);
        ndMineButton->setPosition(unit->getPosition() + Vec2(0, 150));
        ndMineButton->setName(strmake("newminebutton%d_%d", (int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE));
    }else if(unit->unitType == UNIT_LUMBERMILL){
        Node* ndMineButton = CSLoader::createNode("MineButton.csb");
        ImageView* img = (ImageView*)ndMineButton->getChildByName("imgIcon");
        img->loadTexture("lumberIcon.png");
        img->setContentSize(Size(78, 92));
        Button* btn = (Button*)ndMineButton->getChildByName("btn");
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onTreeButtonClick, this));
        WORLD->addChild(ndMineButton, 200);
        ndMineButton->setPosition(unit->getPosition() + Vec2(0, 150));
        ndMineButton->setName(strmake("treeButton%d_%d", (int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE));
        LoadingBar* pbCollect = (LoadingBar*)ndMineButton->getChildByName("pbCollect");
        pbCollect->setColor(Color3B(46, 161, 34));
    }
}
void BattleHud::setupNonBuilding(Movable* unit){
    
}
void BattleHud::onMineButtonClick(Ref* ref){
    Button* btn = (Button*)ref;
    Text* lbl = (Text*)btn->getParent()->getChildByName("lbl");
    int amount = Value(lbl->getString()).asInt();
    if(amount == 0) return;
    
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
    int max = getMaxGold(castleLevel);
    int current = GM->getCoin();
    if(current >= max){
        showInstanceMessage(LM->getText("gold reached max"));
        return;
    }
    if(current + amount > max){
        if(current < max){
            GM->setCoin(max);
        }
        showInstanceMessage(LM->getText("gold reached max"));
    }else{
        GM->addCoin(amount);
    }
    
    lbl->setString("0");
    time_t now = BSM->getCurrentTimeT();
//    lbl->setTag(now);
    
    Node* ndMineButton = btn->getParent();
    std::string key(ndMineButton->getName().c_str());
    std::string value = "";
    value += BSM->getStrFromTime(now);
    log("mine button name: %s, now: %s", key.c_str(), value.c_str());
    UDSetStr(key.c_str(), value);
    std::string newValue = UDGetStr(key.c_str(), "");
    log("mine button1 name: %s, now: %s", key.c_str(), newValue.c_str());
    
    PPLabel* lblPlus = PPLabel::create(strmake("+%d", amount), 60, Color3B::YELLOW, true, true, TextHAlignment::CENTER, false);
    WORLD->addChild(lblPlus, ndMineButton->getLocalZOrder());
    lblPlus->setPosition(ndMineButton->getPosition() + Vec2(0, 50));
    lblPlus->runAction(Sequence::create(EaseOut::create(MoveBy::create(1, Vec2(0, 100)), 2), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(BattleHud::removeFromParent, lblPlus)), NULL));
    for(auto child: lblPlus->getChildren()){
        child->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(1), NULL));
    }
    for(auto child: lblPlus->spriteBatch->getChildren()){
        child->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(1), NULL));
    }
    
    
//    std::string strPos = ndMineButton->getName().substr(10);
//    ValueVector map = GM->split(strPos, "_");
//    int x = map.at(0).asInt();
//    int y = map.at(1).asInt();
//    for(auto building : buildings){
//        if((int)building->getPositionX()/TILE_SIZE == x && (int)building->getPositionY()/TILE_SIZE == y){
//            building->buildingCompleteTimeLeft = BSM->getCurrentTime();
//        }
//    }
    updateBuildingsSaveData();
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_BUILDING);
    saveUserData(datas);
    
//    if (!isTimeHackChecked) {
//        isTimeHackChecked = true;
//        BSM->getHttpTime();
//    }
}
void BattleHud::onTreeButtonClick(Ref* ref){
    Button* btn = (Button*)ref;
    Text* lbl = (Text*)btn->getParent()->getChildByName("lbl");
    int amount = Value(lbl->getString()).asInt();
    if(amount == 0) return;
    
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
    int max = getMaxTree(castleLevel);
    int current = GM->getTree();
    if(current >= max){
        showInstanceMessage(LM->getText("tree reached max"));
        return;
    }
    if(current + amount > max){
        if(current < max){
            GM->setTree(max);
        }
        showInstanceMessage(LM->getText("tree reached max"));
    }else{
        GM->addTree(amount);
    }
    
    lbl->setString("0");
    time_t now = BSM->getCurrentTimeT();
//    lbl->setTag(now);
    
    Node* ndTreeButton = btn->getParent();
//    log("mine button name: %s, now: %lf", ndTreeButton->getName().c_str(), now);
    std::string key(ndTreeButton->getName().c_str());
    std::string value = "";
    value += BSM->getStrFromTime(now);
    UDSetStr(key.c_str(), value);
    
    PPLabel* lblPlus = PPLabel::create(strmake("+%d", amount), 60, Color3B::YELLOW, true, true, TextHAlignment::CENTER, false);
    WORLD->addChild(lblPlus, ndTreeButton->getLocalZOrder());
    lblPlus->setPosition(ndTreeButton->getPosition() + Vec2(0, 50));
    lblPlus->runAction(Sequence::create(EaseOut::create(MoveBy::create(1, Vec2(0, 100)), 2), DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(BattleHud::removeFromParent, lblPlus)), NULL));
    for(auto child: lblPlus->getChildren()){
        child->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(1), NULL));
    }
    for(auto child: lblPlus->spriteBatch->getChildren()){
        child->runAction(Sequence::create(DelayTime::create(1), FadeOut::create(1), NULL));
    }
    updateBuildingsSaveData();
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_TREE);
    datas.push_back(DATA_TYPE_BUILDING);
    saveUserData(datas);
}
void BattleHud::addUpgradeEffect(Movable* node, bool isWorld){
    Sprite* spt = Sprite::create("greenArrow.png");
    if(isWorld){
        WORLD->addChild(spt, WORLD->spriteBatchBuilding->getLocalZOrder()+1);
        spt->setPosition(node->getPosition() + Vec2(0, -40));
    }else{
        node->addChild(spt);
        spt->setPosition(node->getContentSize()/2);
    }
    node->sptUpgradeArrow = spt;
    spt->setName(strmake("upgradeArrow_%d_%d", (int)node->getPositionX(), (int)node->getPositionY()));
    float dur = 0.5f;
    spt->setOpacity(0);
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Vec2(0, 40)), MoveBy::create(0, Vec2(0, -40)), NULL)));
    spt->runAction(RepeatForever::create(Sequence::create(FadeIn::create(dur/3), DelayTime::create(dur/3), FadeOut::create(dur/3), NULL)));
    spt->setRotation(-90);
    spt->setScale(0.5f);
}
bool BattleHud::isBuilding(int unit){
    return unit == UNIT_CASTLE || unit == UNIT_FARM || unit == UNIT_MINE || unit == UNIT_WATCHERTOWER || unit == UNIT_BARRACKS || unit == UNIT_LUMBERMILL || unit == UNIT_FACTORY || unit == UNIT_AIRPORT || unit == UNIT_UNDERGROUND_BUNKER || unit == UNIT_TRIGGER || unit == UNIT_ORC_HQ || unit == UNIT_ORC_BUNKER || unit == UNIT_ORC_BARRACKS || unit == UNIT_ORC_TROLL_HOUSE || unit == UNIT_TEMPLE || unit == UNIT_TREE_FOR_BATTLE || unit == UNIT_BARBECUE;
}
void BattleHud::update(float dt){
    dt += timePassedFromLastAccess;
    for(int i = 0;i < buildings.size();i++){
        Movable* unit = buildings.at(i);
        if (unit->unitState == UNIT_STATE_BUILDING || unit->unitState == UNIT_STATE_UPGRADE || unit->unitState == UNIT_STATE_REMOVING) {
            unit->buildingCompleteTimeLeft -= dt;
            unit->maxEnergy = 100;
            double totalTime = getUnitUpgradeTime(unit->unitType, unit->level);
            if(unit->unitType == UNIT_TREE_FOR_BATTLE){
                totalTime = 60;
                for (int i = 0; i < buildings.size(); i++) {
                    if(buildings.at(i)->unitType == UNIT_LUMBERMILL){
                        totalTime += 60;
                    }
                }
            }
            if (unit->buildingCompleteTimeLeft > totalTime) {
                unit->buildingCompleteTimeLeft = totalTime;
                unit->buildingCompleteTime = totalTime + (double)BSM->getCurrentTimeT();
                updateBuildingsSaveData();
                std::vector<int> datas;
                datas.push_back(DATA_TYPE_BUILDING);
                saveUserData(datas);
            }
            unit->energy = 100 - unit->buildingCompleteTimeLeft*100.0f/ totalTime;//unit->buildingCompleteTime;
            unit->updateEnergy();
            unit->lblTimeLeft->setString(GM->getTimeLeftInString(unit->buildingCompleteTimeLeft));
            if(unit->buildingCompleteTimeLeft <= 0){
                unit->stopAllActions();
                unit->setSpriteFrame(WORLD->getSpriteNameForUnit(unit->unitType));
                unit->energyBar->setVisible(false);
                unit->energyBarContent->setVisible(false);
                unit->energyBarBack->setVisible(false);
                
                if(unit->unitType == UNIT_FARM){
                    updateStatus(hudLayer);
                }
                
                if(unit->lblTimeLeft){
                    unit->lblTimeLeft->removeFromParent();
                    unit->lblTimeLeft = nullptr;
                }
                if (unit->sptUpgradeArrow) {
                    unit->sptUpgradeArrow->removeFromParent();
                    unit->sptUpgradeArrow = nullptr;
                }
                
                std::vector<int> datas;
                if(unit->unitState == UNIT_STATE_UPGRADE){
//                    WORLD->removeChildByName(strmake("upgradeArrow_%d_%d", (int)unit->getPositionX(), (int)unit->getPositionY()));
                    unit->level++;
                    if(unit->unitType == UNIT_CASTLE){
                        UDSetInt(KEY_CASTLE_LEVEL, unit->level);
                        inventoryCountMax = 50 + unit->level*2;
                    }
                }
                if(unit->unitType == UNIT_TREE_FOR_BATTLE){
                    unit->stopAllActions();
                    unit->setScale(1);
                }
                if(unit->unitState == UNIT_STATE_REMOVING){
                    if(unit->unitType == UNIT_TREE_FOR_BATTLE){
                        int amount = 40;
                        for (int i = 0; i < buildings.size(); i++) {
                            if(buildings.at(i)->unitType == UNIT_LUMBERMILL){
                                amount += 120;
                            }
                        }
                        
                        int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
                        int max = getMaxTree(castleLevel);
                        int current = GM->getTree();
                        if(current + amount < max){
                            GM->addTree(amount);
                        }else{
                            if(current < max){
                                GM->setTree(max);
                            }
                        }
                        datas.push_back(DATA_TYPE_TREE);
                        if(GM->getTree() >= max){
                            showInstanceMessage(LM->getText("tree reached max"));
                        }
                    }
                    
                    WORLD->setOccupy(unit->getPosition(), 1, 1, false);
                    buildings.eraseObject(unit);
                    WORLD->removeCharacter((EnemyBase*)unit);
                }else{
                    unit->unitState = UNIT_STATE_IDLE;
                }
                if (unit->unitType == UNIT_MINE) {
                    
                }
                updateAvailableWorkerCount();
                updateBuildingsSaveData();
                datas.push_back(DATA_TYPE_BUILDING);
                datas.push_back(DATA_TYPE_GEM);
                saveUserData(datas);
                break;
            }else{
                
            }
        }
    }
    time_t now = BSM->getCurrentTimeT();
    if (GM->isVideoDone) {
        GM->isVideoDone = false;
        log("is video done in bhud");
        if(GM->videoIndex == VIDEO_GACHA){
            std::vector<int> array;
            array.push_back(getRandomUnit());
            showItemGetAndAddToBag(array);
            std::string value = "";
            value += BSM->getStrFromTime(now);
            UDSetStr(KEY_VIDEO_GACHA_WATCHED_TIME, value);
        }else if(GM->videoIndex == VIDEO_SHIELD){
            addShieldRequested = true;
            BSM->addShield(6);
            showIndicator();
        }else if(GM->videoIndex == VIDEO_STORE_RANDOM){
            std::string value = "";
            value += BSM->getStrFromTime(now);
            UDSetStr(KEY_VIDEO_STORE_RANDOM_WATCHED_TIME, value);
            int index = UDGetInt(KEY_VIDEO_STORE_RANDOM_INDEX, 0);
            std::vector<int> datas;
            if (index >= 0 && index <= 4) {
                showTreeReward(20);
                datas.push_back(DATA_TYPE_TREE);
            }else if (index >= 6 && index <= 10) {
                showTreeReward(40);
                datas.push_back(DATA_TYPE_TREE);
            }else if (index == 5 || index == 11){
                Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
                this->addChild(layer, 4);
                layer->setName("unitRewardMessageBox");
                setAsPopup(layer);
                layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
                layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
                ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
                GM->rollOpenScroll(imgBack);
                
                Button* btn = (Button*)layer->getChildByName("btnOk");
                btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
                Text* lbl = (Text*)btn->getChildByName("lbl");
                LM->setLocalizedString(lbl, "ok");
                
                lbl = (Text*)layer->getChildByName("lblGemCount");
                lbl->setString("1");
                ImageView* img = (ImageView*)layer->getChildByName("imgGem");
                int unit = getRandomUnit();
                
                UnitInfo* unitInfo = new UnitInfo();
                unitInfo->unitType = unit;
                unitInfoListInventory.push_back(unitInfo);
                
                updateInventorySaveData();
                datas.push_back(DATA_TYPE_INVENTORY);
                
                Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit));
                img->getParent()->addChild(spt);
                spt->setPosition(img->getPosition());
                img->removeFromParent();
                GM->alignToCenter(spt, lbl, 50, size.width/2, -20);
                
                lbl = (Text*)layer->getChildByName("lblDescription");
                LM->setLocalizedString(lbl, "reward");
            }
            saveUserData(datas);
            
            index++;
            UDSetInt(KEY_VIDEO_STORE_RANDOM_INDEX, index);
            onVideoTabClick(this->getChildByName("videoStore")->getChildByName("btnTab0"));
        }else if(GM->videoIndex == VIDEO_STORE_UNIT){
            std::string value = "";
            value += BSM->getStrFromTime(now);
            UDSetStr(KEY_VIDEO_STORE_UNIT_WATCHED_TIME, value);
            int index = UDGetInt(KEY_VIDEO_STORE_UNIT_INDEX, 0);
            std::vector<int> datas;
            if (index >= 0 && index <= 4) {
                showTreeReward(20);
                datas.push_back(DATA_TYPE_TREE);
            }else if (index >= 6 && index <= 10) {
                showTreeReward(40);
                datas.push_back(DATA_TYPE_TREE);
            }else if (index == 5) {
                showGoldReward(100);
                datas.push_back(DATA_TYPE_GOLD);
            }else if (index == 11){
                Node* layer = CSLoader::createNode("GemRewardMessageBox.csb");
                this->addChild(layer, 4);
                layer->setName("unitRewardMessageBox");
                setAsPopup(layer);
                layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
                layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
                ImageView* imgBack = (ImageView*)layer->getChildByName("imgBackground");
                GM->rollOpenScroll(imgBack);
                
                Button* btn = (Button*)layer->getChildByName("btnOk");
                btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
                Text* lbl = (Text*)btn->getChildByName("lbl");
                LM->setLocalizedString(lbl, "ok");
                
                lbl = (Text*)layer->getChildByName("lblGemCount");
                lbl->setString("1");
                ImageView* img = (ImageView*)layer->getChildByName("imgGem");
                int dayIndex = UDGetInt(KEY_VIDEO_STORE_UNIT_DAY, 0);
                int unit = getUnitIndexForVideoStore(dayIndex);
                
                dayIndex++;
                UDSetInt(KEY_VIDEO_STORE_UNIT_DAY, dayIndex);
                
                UnitInfo* unitInfo = new UnitInfo();
                unitInfo->unitType = unit;
                unitInfoListInventory.push_back(unitInfo);
                
                updateInventorySaveData();
                datas.push_back(DATA_TYPE_INVENTORY);
                
                Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit));
                img->getParent()->addChild(spt);
                spt->setPosition(img->getPosition());
                img->removeFromParent();
                GM->alignToCenter(spt, lbl, 50, size.width/2, 0);
                
                lbl = (Text*)layer->getChildByName("lblDescription");
                LM->setLocalizedString(lbl, "reward");
            }
            saveUserData(datas);
            
            index++;
            UDSetInt(KEY_VIDEO_STORE_UNIT_INDEX, index);
            onVideoTabClick(this->getChildByName("videoStore")->getChildByName("btnTab1"));
        }else if(GM->videoIndex == VIDEO_STORE_GEM){
            std::string value = "";
            value += BSM->getStrFromTime(now);
            UDSetStr(KEY_VIDEO_STORE_GEM_WATCHED_TIME, value);
            int index = UDGetInt(KEY_VIDEO_STORE_GEM_INDEX, 0);
            std::vector<int> datas;
            if (index >= 0 && index <= 4) {
                showGemRewardMessageBox("reward", 1);
            }else if (index >= 6 && index <= 10) {
                showGemRewardMessageBox("reward", 1);
            }else if (index == 5) {
                showGemRewardMessageBox("reward", 10);
            }else if (index == 11){
                showGemRewardMessageBox("reward", 10);
            }
            datas.push_back(DATA_TYPE_GEM);
            saveUserData(datas);
            
            index++;
            UDSetInt(KEY_VIDEO_STORE_GEM_INDEX, index);
            onVideoTabClick(this->getChildByName("videoStore")->getChildByName("btnTab2"));
        }
    }
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        // give reward before consume
        GM->consumeSkuNameList.push_back(GM->iapDetail);
        log("consumeSkuNameList push_back: %d", (int)GM->consumeSkuNameList.size());
        // set iapFlag = IAP_FLAG_INIT after giving rewards. See below.
    }
    if(GM->consumeSkuNameList.size() > 0 && isLoadComplete){
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
        updateBuildingsSaveData();
        std::vector<int> datas;
        int iapPoint = UDGetInt(KEY_IAP_TOTAL, 0);
        if (GM->iapDetail.compare(IAP_DETAIL_GEM_3) == 0) {
            GM->addGem(180);
            iapPoint += 3000;
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_10) == 0) {
            GM->addGem(660);
            iapPoint += 10000;
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_50) == 0) {
            GM->addGem(3600);
            iapPoint += 50000;
        }else if (GM->iapDetail.compare(IAP_DETAIL_GEM_100) == 0) {
            GM->addGem(7800);
            iapPoint += 100000;
        }else if(GM->iapDetail.compare(IAP_DETAIL_STARTER_1) == 0){
            GM->addGem(60);
            iapPoint += 1000;
            log("iap success starter 1!");
            for (int i = 0; i < 3; i++) {
                UnitInfo* info = new UnitInfo();
                info->unitType = UNIT_SWORDMAN;
//                addUnitToDeck(swordInfo);
                unitInfoListInventory.push_back(info);
                
                info = new UnitInfo();
                info->unitType = UNIT_ARCHER;
                unitInfoListInventory.push_back(info);
//                addUnitToDeck(archerInfo);
            }
            UnitInfo* zombieInfo = new UnitInfo();
            zombieInfo->unitType = UNIT_ZOMBIE_SWORDSMAN;
            addUnitToDeck(zombieInfo);
            
            shouldSaveDeck = true;
            updateInventorySaveData();
            datas.push_back(DATA_TYPE_INVENTORY);
            UDSetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 0).c_str(), true);
            closePopup();
            onSpecialOfferClick();
        }else if(GM->iapDetail.compare(IAP_DETAIL_STARTER_3) == 0){
            GM->addGem(200);
            iapPoint += 3000;
            log("iap success starter 3!");
            for (int i = 0; i < 6; i++) {
                UnitInfo* info = new UnitInfo();
                info->unitType = UNIT_ARCHER;
//                addUnitToDeck(archerInfo);
                unitInfoListInventory.push_back(info);
            }
            for(int i = 0; i < 2; i++){
                UnitInfo* trollInfo = new UnitInfo();
                trollInfo->unitType = UNIT_TROLL;
//                addUnitToDeck(trollInfo);
                unitInfoListInventory.push_back(trollInfo);
            }
            
            
            shouldSaveDeck = true;
            updateInventorySaveData();
            datas.push_back(DATA_TYPE_INVENTORY);
            UDSetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 2).c_str(), true);
            closePopup();
            onSpecialOfferClick();
        }else if(GM->iapDetail.compare(IAP_DETAIL_STARTER_10) == 0){
            GM->addGem(1000);
            iapPoint += 10000;
            log("iap success starter 2!");
            for (int i = 0; i < 3; i++) {
                UnitInfo* info = new UnitInfo();
                info->unitType = UNIT_ORC_AXE;
//                addUnitToDeck(axeInfo);
                unitInfoListInventory.push_back(info);
                
                info = new UnitInfo();
                info->unitType = UNIT_ORC_SPEAR;
//                addUnitToDeck(spearInfo);
                unitInfoListInventory.push_back(info);
            }
            for(int i = 0; i < 3; i++){
                UnitInfo* info = new UnitInfo();
                info->unitType = UNIT_GOBLIN_BOMB;
//                addUnitToDeck(bombInfo);
                unitInfoListInventory.push_back(info);
            }
            shouldSaveDeck = true;
            updateInventorySaveData();
            datas.push_back(DATA_TYPE_INVENTORY);
            UDSetBool(strmake(KEY_SPECIAL_OFFER_BOUGHT_FORMAT, 1).c_str(), true);
            closePopup();
            onSpecialOfferClick();
        }
        std::string iapList = UDGetStr(KEY_IAP_LIST, "");
        iapList += GM->iapDetail + "|";
        UDSetStr(KEY_IAP_LIST, iapList);
        UDSetInt(KEY_IAP_TOTAL, iapPoint);
        
        datas.push_back(DATA_TYPE_IAP);
        datas.push_back(DATA_TYPE_GEM);
        saveUserData(datas);
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
    
    oneSecChecker += dt;
    if (oneSecChecker < 0) {
        return;
    }
    if (BSM->isServerFailed){
        onBackClick();
        return;
    }
    oneSecChecker -= 1;
    if (GM->raidRewardTrophy != 0 || GM->raidRewardTree != 0 || GM->raidRewardGold != 0) {
        int finalTrophy = UDGetInt(KEY_TROPHY, 1000);
        finalTrophy += GM->raidRewardTrophy;
        if(finalTrophy > 1000000){
            finalTrophy = 1000;
        }
        UDSetInt(KEY_TROPHY, finalTrophy);
        
        int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
        int max = getMaxGold(castleLevel);
        int current = GM->getCoin();
        if(current + GM->raidRewardGold > max){
            if(current < max){
                GM->setCoin(max);
            }
        }else{
            GM->addCoin(GM->raidRewardGold);
        }
        
        max = getMaxTree(castleLevel);
        current = GM->getTree();
        if(current + GM->raidRewardTree > max){
            if(current < max){
                GM->setTree(max);
            }
        }else{
            GM->addTree(GM->raidRewardTree);
        }
        
        BSM->saveUserData("gold=" + Value(GM->getCoin()).asString() + "&tree=" + Value(GM->getTree()).asString() + "&trophy=" + Value(UDGetInt(KEY_TROPHY, 1000)).asString());
        GM->raidRewardTrophy = 0;
        GM->raidRewardGold = 0;
        GM->raidRewardTree = 0;
    }
    
    if(networkStateGetData == NETWORK_HANDLE_STATE_ARRIVED){
        networkStateGetData = NETWORK_HANDLE_STATE_HANDLED;
        
        if (isLoadAllUserDataRequested) {
            isLoadAllUserDataRequested = false;
            loadData();
            isLoadComplete = true;
            hideIndicator();
        }
        if(isLastAccessTimeRequested){
            isLastAccessTimeRequested = false;
        }
        if(isTopRankRequested){
            isTopRankRequested = false;
            isMyRankRequested = true;
            BSM->getMyRank(UDGetInt(KEY_TROPHY, 1000));
        }else if (isMyRankRequested) {
            isMyRankRequested = false;
            hideIndicator();
            showDefenceRecord();
        }
        if (isVisitRequested) {
            isVisitRequested = false;
            this->unschedule(schedule_selector(BattleHud::update));
            this->unschedule(schedule_selector(BattleHud::updateUI));
            checkUnsaved();
            WORLD->removeUsedAssets();
            BHUD = nullptr;
            GM->nextScene = STAGE_RAID; // test
            removeUsedAssets();
            auto scene = HelloWorld::scene(GM->nextScene, false); // test
            Director::getInstance()->replaceScene(scene);
        }
        if (addShieldRequested) {
            addShieldRequested = false;
            hideIndicator();
        }
    }
    if(isLoadComplete){
        if(isAnybodyMoving){
            bool stillMoving = false;
            for(auto unit: unitsDeck){
                if(unit->unitAct == UNIT_ACT_MOVE){
                    stillMoving = true;
                    break;
                }
            }
            if(!stillMoving){
                isAnybodyMoving = false;
                for(int i = 0; i < unitsDeck.size(); i++){
                    unitInfoListDeck.at(i)->x = (int)unitsDeck.at(i)->getPositionX();
                    unitInfoListDeck.at(i)->y = (int)unitsDeck.at(i)->getPositionY();
                }
                shouldSaveDeck = true;
//                updateDeckSaveData();
//                std::vector<int> datas;
//                datas.push_back(DATA_TYPE_DECK);
//                saveUserData(datas);
            }
        }else{
            for(auto unit: unitsDeck){
                if(unit->unitAct == UNIT_ACT_MOVE){
                    isAnybodyMoving = true;
                    break;
                }
            }
        }
    }
}
void BattleHud::checkUnsaved(){
    std::vector<int> datas;
    bool isAnythingChanged = false;
    if (shouldSaveBuilding) {
        updateBuildingsSaveData();
        datas.push_back(DATA_TYPE_BUILDING);
        isAnythingChanged = true;
    }
    
    if (shouldSaveDeck) {
        updateDeckSaveData();
        datas.push_back(DATA_TYPE_DECK);
        isAnythingChanged = true;
    }
    
    if(isAnythingChanged){
        saveUserData(datas);
    }
}
void BattleHud::onOkClick(){
    WORLD->deselectAll();
    std::vector<int> datas;
    
    if (currentJob == JOB_MOVE_BUILDING_TEMPATE) {
        if(!WORLD->isBuildingReadyToBuild){
            showInstanceMessage(LM->getText("you cannot build there"));
            return;
        }
        int goldPrice = getBuildingPriceGold(currentJobDetailIndex);
        GM->addCoin(-goldPrice);
        int treePrice = getBuildingPriceTree(currentJobDetailIndex);
        GM->addTree(-treePrice);
        datas.push_back(DATA_TYPE_GOLD);
        datas.push_back(DATA_TYPE_TREE);
        
        if(currentJobDetailIndex == BUILDING_MINE){
            selectedUnit = addBuildingCreating(UNIT_MINE, getUnitCreatingTime(UNIT_MINE));
        }else if(currentJobDetailIndex == BUILDING_FARM){
            selectedUnit = addBuildingCreating(UNIT_FARM, getUnitCreatingTime(UNIT_FARM));
        }else if(currentJobDetailIndex == BUILDING_TREE){
            selectedUnit = addBuildingCreating(UNIT_TREE_FOR_BATTLE, getUnitCreatingTime(UNIT_TREE_FOR_BATTLE));
        }else if(currentJobDetailIndex == BUILDING_WATCHER_TOWER){
            selectedUnit = addBuildingCreating(UNIT_WATCHERTOWER, getUnitCreatingTime(UNIT_WATCHERTOWER));
        }
//        else if(currentJobDetailIndex == BUILDING_UNDERGROUND_BUNKER){
//            selectedUnit = addBuildingCreating(UNIT_UNDERGROUND_BUNKER, getUnitCreatingTime(UNIT_UNDERGROUND_BUNKER));
//        }else if(currentJobDetailIndex == BUILDING_TRIGGER){
//            selectedUnit = addBuildingCreating(UNIT_TRIGGER, getUnitCreatingTime(UNIT_TRIGGER));
//        }
        else if(currentJobDetailIndex == BUILDING_LUMBURMILL){
            selectedUnit = addBuildingCreating(UNIT_LUMBERMILL, getUnitCreatingTime(UNIT_LUMBERMILL));
        }else if(currentJobDetailIndex == BUILDING_BARRACKS){
            selectedUnit = addBuildingCreating(UNIT_BARRACKS, getUnitCreatingTime(UNIT_BARRACKS));
        }else if(currentJobDetailIndex == BUILDING_FACTORY){
            selectedUnit = addBuildingCreating(UNIT_FACTORY, getUnitCreatingTime(UNIT_FACTORY));
        }else if(currentJobDetailIndex == BUILDING_AIRPORT){
            selectedUnit = addBuildingCreating(UNIT_AIRPORT, getUnitCreatingTime(UNIT_AIRPORT));
        }
    }
    if(selectedUnit->isBuilding){
        setupBuilding(selectedUnit);
    }else{
        setupNonBuilding(selectedUnit);
    }
    
    currentJob = JOB_NONE;
    hideCancelButton();
    onUnitSelected(selectedUnit);
    WORLD->selectUnit((EnemyBase*)selectedUnit);
    WORLD->resetPathState();
    updateAvailableWorkerCount();
    
    
    updateBuildingsSaveData();
    datas.push_back(DATA_TYPE_BUILDING);
    saveUserData(datas);
}
Movable* BattleHud::addBuildingCreating(int index, float timeLeft){
    Vec2 pos = WORLD->buildingTemplate->getPosition();
    Vec2 sptPos = pos + Vec2(WORLD->buildingTemplateSize.width*TILE_SIZE/2, WORLD->buildingTemplateSize.height*TILE_SIZE/2);
    Vec2 occupyPos = pos + Vec2(0, TILE_SIZE*WORLD->buildingTemplateSize.height);
    EnemyBase* unit;
    if(index == UNIT_MINE){
        unit = WORLD->createUnit(UNIT_MINE, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "mine.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
    }else if(index == UNIT_TREE_FOR_BATTLE){
        unit = WORLD->createUnit(UNIT_TREE_FOR_BATTLE, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "tree", 1, strmake("tree3_%d.png", rand()%5));
        WORLD->setOccupy(occupyPos, 1, 1, true, unit);
    }else if(index == UNIT_FARM){
        unit = WORLD->createUnit(UNIT_FARM, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "farm", 1, "farm.png");
        WORLD->setOccupy(occupyPos, 3, 2, true, unit);
    }else if(index == UNIT_WATCHERTOWER){
        unit = WORLD->createUnit(UNIT_WATCHERTOWER, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "watcher tower", 1, "watcherTower.png");
        WORLD->setOccupy(occupyPos, 2, 2, true, unit);
    }else if(index == UNIT_UNDERGROUND_BUNKER){
        unit = WORLD->createUnit(UNIT_UNDERGROUND_BUNKER, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "undergroundBunker.png");
        WORLD->setOccupy(occupyPos, 3, 2, true, unit);
    }else if(index == UNIT_TRIGGER){
        unit = WORLD->createUnit(UNIT_TRIGGER, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "trigger.png");
        WORLD->setOccupy(occupyPos, 1, 1, true, unit);
    }else if(index == UNIT_LUMBERMILL){
        unit = WORLD->createUnit(UNIT_LUMBERMILL, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "lumberMill.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
    }else if(index == UNIT_BARRACKS){
        unit = WORLD->createUnit(UNIT_BARRACKS, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "barracks.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
    }else if(index == UNIT_FACTORY){
        unit = WORLD->createUnit(UNIT_FACTORY, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "mine", 1, "factory.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
    }else if(index == UNIT_AIRPORT){
        unit = WORLD->createUnit(UNIT_AIRPORT, WHICH_SIDE_HERO, ITS_BUILDING, sptPos, "airport", 1, "airport.png");
        WORLD->setOccupy(occupyPos, 3, 3, true, unit);
    }
    if(index == UNIT_TREE_FOR_BATTLE){
        unit->setScale(0.5f);
        unit->runAction(ScaleTo::create(timeLeft, 1));
        GM->makeItSiluk(unit);
    }else{
        unit->runAnimation("underConstrunction", true);
    }
    
    unit->buildingCompleteTime = BSM->getCurrentTimeT() + timeLeft;
    unit->buildingCompleteTimeLeft = timeLeft;
    unit->unitState = UNIT_STATE_BUILDING;
    buildings.pushBack(unit);
    WORLD->buildingTemplate->removeFromParent();
    WORLD->buildingTemplate = nullptr;
    unit->energy = 0;
    unit->maxEnergy = 100;
    unit->updateEnergy();
    Text* lblTimeLeft = Text::create("Building...", "BMDOHYEON.ttf", 20);
    lblTimeLeft->enableOutline(Color4B::BLACK, 2);
    WORLD->addChild(lblTimeLeft, unit->getParent()->getLocalZOrder()+1);
    lblTimeLeft->setPosition(unit->getPosition() + Point(0, 60));
//    timeLeftLabels.pushBack(lblTimeLeft);
    unit->lblTimeLeft = lblTimeLeft;
    return unit;
}

int BattleHud::getUnitCreatingTime(int unit, int level){
    std::string name = GM->getUnitName(unit);
    string str = GM->unitCompleteTimeTable[name.c_str()].asValueMap()[Value(level).asString()].asString();
    return Value(str).asInt();
}
int BattleHud::getUnitUpgradeTime(int unit, int level){
    string str = GM->unitCompleteTimeTable[GM->getUnitName(unit).c_str()].asValueMap()[Value(level).asString().c_str()].asString();
    return Value(str).asInt();
}
void BattleHud::onCancelClick(){
    if (currentJob == JOB_MOVE_BUILDING_TEMPATE) {
        WORLD->buildingTemplate->removeFromParent();
        WORLD->buildingTemplate = nullptr;
    }
    currentJob = JOB_NONE;
    hideCancelButton();
}
void BattleHud::moveTopBarTo(Node* parent, bool moveToLeft){
    ndTopBar->removeFromParentAndCleanup(false);
    parent->addChild(ndTopBar);
    ndTopBar->stopAllActions();
    float totalHeight = size.height;
    float topBarNodeY = 1334;
    float parentY = parent->getPositionY();
    ndTopBar->setPositionY(totalHeight - topBarNodeY - parentY);
    if(moveToLeft){
        ndTopBar->runAction(EaseInOut::create(MoveTo::create(1, Vec2(-130, ndTopBar->getPositionY())), 2));
    }
}
void BattleHud::onTrainBRClick(){
    onTrainClick();
}
void BattleHud::onCloseTrainPopup(){
    closePopup();
    if (shouldSaveDeckAndInventoryWhenExitTrainPopup) {
        shouldSaveDeckAndInventoryWhenExitTrainPopup = false;
        updateInventorySaveData();
        updateDeckSaveData();
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_DECK);
        datas.push_back(DATA_TYPE_INVENTORY);
        saveUserData(datas);
    }
}
void BattleHud::onTrainClick(){
    svUpgardeX = 0;
    Node* layer = CSLoader::createNode("Train.csb");
    this->addChild(layer, 4);
    layer->setName("trainLayer");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    moveTopBarTo(layer);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onCloseTrainPopup, this));
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    btn = (Button*)layer->getChildByName("btnTab0");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onTrainTab0Click, this));
    int barracksCount = 0;
    for(auto building : buildings){
        if(building->unitType == UNIT_BARRACKS){
            barracksCount++;
        }
    }
    for (int i = 1; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onTrainTabClick, this));
        if (i == 1 && barracksCount == 0) {
            layer->getChildByName("imgTabIcon1")->setVisible(false);
            Sprite* spt = Sprite::create("iconLock.png");
            layer->addChild(spt);
            spt->setPosition(btn->getPosition() + Vec2(0, 123/2));
            spt->setScale(0.5f);
            btn->setEnabled(false);
        }
    }
//    Node* ndUnit = layer->getChildByName("ndUnits");
    // close and reopen for train tab 0
    
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    btn =(Button*)ndUpgrade->getChildByName("btnUpgrade");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onUpgradeClickInUpgradeTab, this));
    btn = (Button*)ndUpgrade->getChildByName("btnComplete");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onCompleteUpgradeClick, this));
    Text* lbl = (Text*)ndUpgrade->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade");
    lbl = (Text*)ndUpgrade->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "choose unit to upgrade");
    for (int i = 0; i < 2; i++) {
        btn =(Button*)ndUpgrade->getChildByName(strmake("btnUpgrade%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onSlotClickInUpgradeTab, this));
    }
    
    for (int i = 0; i < 4; i++) {
        btn =(Button*)ndUpgrade->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUpgradeTabClick, this));
        Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_BARRACKS));
        btn->addChild(spt);
        spt->setScale(0.7f);
        spt->setPosition(btn->getContentSize()/2);
        btn->setVisible(i < barracksCount);
    }
    onUpgradeTabClick(ndUpgrade->getChildByName("btnTab0"));
    
    Node* ndGacha = layer->getChildByName("ndGacha");
    Sprite* spt;
    for (int i = 0; i < 12; i++) {
        Text* lbl = (Text*)ndGacha->getChildByName(strmake("lbl%d", i));
        if(i == 0){
            LM->setLocalizedString(lbl, "swordman");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_SWORDMAN));
        }else if(i == 1){
            LM->setLocalizedString(lbl, "archer");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_ARCHER));
        }else if(i == 2){
            LM->setLocalizedString(lbl, "catapult");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_CATAPULT));
        }else if(i == 3){
            LM->setLocalizedString(lbl, "helicopter");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_HELICOPTER));
        }else if(i == 4){
            LM->setLocalizedString(lbl, "goblin");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_GOBLIN));
        }else if(i == 5){
            LM->setLocalizedString(lbl, "goblin bomb");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_GOBLIN_BOMB));
        }else if(i == 6){
            LM->setLocalizedString(lbl, "orc axe");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_ORC_AXE));
        }else if(i == 7){
            LM->setLocalizedString(lbl, "orc spear");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_ORC_SPEAR));
        }else if(i == 8){
            LM->setLocalizedString(lbl, "troll");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_TROLL));
        }else if(i == 9){
            LM->setLocalizedString(lbl, "zombie");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_ZOMBIE_SWORDSMAN));
        }else if(i == 10){
            LM->setLocalizedString(lbl, "zombie");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_ZOMBIE_ORC_AXE));
        }else if(i == 11){
            LM->setLocalizedString(lbl, "wizard");
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(UNIT_WIZARD));
        }
        
        lbl->getParent()->addChild(spt);
        spt->setPosition(lbl->getPosition() + Vec2(0, spt->getContentSize().height/2));
        lbl->setLocalZOrder(1);
    }
    lbl = (Text*)ndGacha->getChildByName("lblWizardDescription");
    LM->setLocalizedString(lbl, "wizard burn tree");
    
    for(int i = 0; i < 3;i++){
        btn = (Button*)ndGacha->getChildByName(strmake("btn%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onTrainGachaClick, this));
    }
    
    onTrainTabClick(layer->getChildByName("btnTab0"));
}
void BattleHud::onUpgradeClickInUpgradeTab(){
    Node* layer = this->getChildByName("trainLayer");
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    Button* btn0 = (Button*)ndUpgrade->getChildByName("btnUpgrade0");
    int tabIndex = layer->getTag();
    int unit = btn0->getTag()%1000;
    int level = btn0->getTag()/1000;
    
    UnitInfo* info;
    int counter = 0;
    for (int i = 0; i < unitInfoListInventory.size(); i++) {
        info = unitInfoListInventory.at(i);
        if (info->unitType == unit && info->level == level && counter < 2) {
            counter++;
            unitInfoListInventory.erase(unitInfoListInventory.begin() + i);
            i--;
        }
    }
    
    time_t now = BSM->getCurrentTimeT();
    log("now: %s", BSM->getStrFromTime(now).c_str());
    std::string value = "";
    value += BSM->getStrFromTime(now + getUnitUpgradeTime(unit, level));
    log("endtime1: %s", value.c_str());
    UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), value);
    log("endtime2: %s", UDGetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "novalue").c_str());
    UDSetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), btn0->getTag());
    
    
    std::vector<int> datas;
    updateInventorySaveData();
    datas.push_back(DATA_TYPE_INVENTORY);
    saveUserData(datas);
    
    updateUnitListForUpgrade();
}
void BattleHud::onFinishUpgradeClick(){
    Node* layer = this->getChildByName("trainLayer");
    
    int tabIndex = layer->getTag();
    UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "");
    int value = UDGetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), -1);
    UDSetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), -1);
    if(value < 0){
        updateUnitListForUpgrade();
        return;
    }
    UnitInfo* info = new UnitInfo();
    info->unitType = value%1000;
    info->level = value/1000 + 1;
    
    unitInfoListInventory.push_back(info);
    
    std::vector<int> datas;
    updateInventorySaveData();
    datas.push_back(DATA_TYPE_INVENTORY);
    saveUserData(datas);
    
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    Button* btn0 = (Button*)ndUpgrade->getChildByName("btnUpgrade0");
    Button* btn1 = (Button*)ndUpgrade->getChildByName("btnUpgrade1");
    btn0->setTag(-1);
    btn1->setTag(-1);
    updateUnitListForUpgrade();
}
void BattleHud::onSlotClickInUpgradeTab(Ref* ref){
    BTN_FROM_REF
    btn->setTag(-1);
    updateUnitListForUpgrade();
}
void BattleHud::onUpgradeTabClick(Ref* ref){
    BTN_FROM_REF
    int tabIndex = btn->getTag();
    Node* layer = this->getChildByName("trainLayer");
    layer->setTag(tabIndex);
    
    int upgradeValue = UDGetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), -1);
    
    Node* ndUpgrade = this->getChildByName("trainLayer")->getChildByName("ndUpgrade");
    Text* lbl;
    if (upgradeValue < 0) {
        for (int i = 0; i < 2; i++) {
            btn = (Button*)ndUpgrade->getChildByName(strmake("btnUpgrade%d", i));
            btn->setVisible(true);
            btn->removeChildByName("spt");
            lbl = (Text*)btn->getChildByName("lblLevel");
            lbl->setVisible(false);
            lbl->setLocalZOrder(1);
        }
        btn = (Button*)ndUpgrade->getChildByName("imgProcessing");
        btn->setVisible(false);
        btn = (Button*)ndUpgrade->getChildByName("btnUpgrade");
        btn->setVisible(false);
        Button* btn0 = (Button*)ndUpgrade->getChildByName("btnUpgrade0");
        btn0->setTag(-1);
        Button* btn1 = (Button*)ndUpgrade->getChildByName("btnUpgrade1");
        btn1->setTag(-1);
    }else{
        
    }
    updateUnitListForUpgrade();
}
void BattleHud::updateUnitListForUpgrade(){
    Node* layer = this->getChildByName("trainLayer");
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    Button* btn0 = (Button*)ndUpgrade->getChildByName("btnUpgrade0");
    Button* btn1 = (Button*)ndUpgrade->getChildByName("btnUpgrade1");
    int lvl0 = btn0->getTag() < 0?-1:btn0->getTag()/1000;
    int unit0 = btn0->getTag() < 0?-1:btn0->getTag()%1000;
    int lvl1 = btn1->getTag() < 0?-1:btn1->getTag()/1000;
    int unit1 = btn1->getTag() < 0?-1:btn1->getTag()%1000;
    for (int i = 0; i < 2; i++) {
        Button* btn = (Button*)ndUpgrade->getChildByName(strmake("btnUpgrade%d", i));
        Text* lbl = (Text*)btn->getChildByName("lblLevel");
        
        if(btn->getTag() < 0){
            btn->removeChildByName("spt");
            lbl->setString("");
        }else if(btn->getChildByName("spt") == nullptr){
            Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(btn->getTag()%1000));
            if(spt){
                btn->addChild(spt);
                spt->setPosition(Vec2(131, 145));
                spt->setName("spt");
                lbl->setString(strmake("Lv.%d", btn->getTag()/1000 + 1));
                lbl->setVisible(true);
            }
        }
    }
    std::vector<int> includedIndices;int unit;int level;bool isAlreadyIncluded;
    ScrollView* sv = (ScrollView*)ndUpgrade->getChildByName("sv");
    sv->setScrollBarEnabled(false);
    for (int i = 0; i < unitInfoListInventory.size();i++) {
        unit = unitInfoListInventory.at(i)->unitType;
        level = unitInfoListInventory.at(i)->level;
        isAlreadyIncluded = false;
        for(auto index: includedIndices){
            if(index == i){
                isAlreadyIncluded = true;
            }
        }
        if(isAlreadyIncluded){
            continue;
        }
        for (int j = i+1; j < unitInfoListInventory.size();j++) {
            isAlreadyIncluded = false;
            for(auto index: includedIndices){
                if(index == j){
                    isAlreadyIncluded = true;
                }
            }
            if(isAlreadyIncluded){
                continue;
            }
            if (unitInfoListInventory.at(j)->unitType == unit && unitInfoListInventory.at(j)->level == level) {
                includedIndices.push_back(i);
                includedIndices.push_back(j);
                break;
            }
        }
    }
    Widget* btnTemp = (Widget*)layer->getChildByName("btnTemp");
    int gapX = 200;
    int totalWidth = btnTemp->getContentSize().width + gapX*includedIndices.size();
    if(totalWidth > sv->getContentSize().width){
        sv->setInnerContainerSize(Size(totalWidth, sv->getContentSize().height));
    }else{
        sv->setInnerContainerSize(sv->getContentSize());
    }
    int x = btnTemp->getContentSize().width/2;
    sv->removeAllChildren();
    UnitInfo* info;
    for(auto index : includedIndices){
        info = unitInfoListInventory.at(index);
        if(info->unitType < 0){
            continue;
        }
        if (info->unitType == unit0 && info->level == lvl0) {
            unit0 = -1;
            lvl0 = -1;
            continue;
        }
        if (info->unitType == unit1 && info->level == lvl1) {
            unit1 = -1;
            lvl1 = -1;
            continue;
        }
        Button* btn = (Button*)btnTemp->clone();
        btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUnitClickInUpgradeTab, this));
        sv->addChild(btn);
        btn->setPosition(Vec2(x, sv->getContentSize().height/2));
        btn->setRotation(-5 - rand()%8);
        btn->setTag(index);
        Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(info->unitType));
        if(spt){
            btn->addChild(spt);
            spt->setName("spt");
            spt->setPosition(btn->getContentSize()/2);
        }
        Text* lbl = (Text*)btn->getChildByName("lblLevel");
        lbl->setString(strmake("Lv.%d", info->level + 1));
        lbl->setLocalZOrder(1);
        x += gapX;
    }
    
    sv->setInnerContainerPosition(Vec2(svUpgardeX, 0));
    
    int tabIndex = layer->getTag();
    for (int i = 0; i < 4; i++) {
        Node* btnTab = ndUpgrade->getChildByName(strmake("btnTab%d", i));
        btnTab->setColor(tabIndex == i?Color3B::WHITE:Color3B::GRAY);
    }
    std::string strEndTime = UDGetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "");
    time_t endTimeT = BSM->getTimeTFromStr(strEndTime);
    int upgradeValue = UDGetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), -1);
    if(upgradeValue < 0){
        strEndTime = "";
        UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "");
    }
    btn0->setVisible(upgradeValue < 0);
    btn1->setVisible(upgradeValue < 0);
    ImageView* imgProcessing = (ImageView*)ndUpgrade->getChildByName("imgProcessing");
    imgProcessing->setVisible(upgradeValue >= 0);
    imgProcessing->removeChildByName("spt");
    
    time_t now = BSM->getCurrentTimeT();
    Button* btn = (Button*)ndUpgrade->getChildByName("btnComplete");
    double timeLeft = difftime(endTimeT, now);
    btn->setVisible(upgradeValue >= 0 && timeLeft >= 0);
    Text* lbl = (Text*)btn->getChildByName("lbl");
    lbl->setString(strmake("%d", GM->getGemForTimeLeft((int)timeLeft)));
    btn = (Button*)ndUpgrade->getChildByName("btnFinish");
    btn->setVisible(upgradeValue >= 0 && timeLeft < 0);
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onFinishUpgradeClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "complete");
    
    Button* btnUpgrade = (Button*)ndUpgrade->getChildByName("btnUpgrade");
    if (upgradeValue < 0 && strEndTime.size() == 0) {
        bool isUpgradeReady = btn0->getTag() >= 0 && btn0->getTag() == btn1->getTag();
        btnUpgrade->setVisible(isUpgradeReady);
        btnUpgrade->setEnabled(isUpgradeReady);
        Text* lbl = (Text*)btnUpgrade->getChildByName("lbl");
        LM->setLocalizedString(lbl, "upgrade");
        Button* btnComplete = (Button*)ndUpgrade->getChildByName("btnComplete");
        btnComplete->setVisible(false);
    }else if(upgradeValue > 0){
        btnUpgrade->setVisible(false);
        Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(upgradeValue%1000));
        if(spt){
            imgProcessing->addChild(spt);
            spt->setName("spt");
            spt->setPosition(Vec2(131, 145));
            lbl = (Text*)imgProcessing->getChildByName("lblLevel");
            lbl->setString(strmake("Lv.%d", upgradeValue/1000 + 1));
            lbl->setLocalZOrder(10);
        }
    }
    updateUI(0);
}
void BattleHud::onCompleteUpgradeClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("trainLayer");
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    btn = (Button*)ndUpgrade->getChildByName("btnComplete");
    Text* lbl = (Text*)btn->getChildByName("lbl");
    int gem = Value(lbl->getString()).asInt();
    if (gem <= GM->getGem()) {
        GM->addGem(-gem);
        time_t now = BSM->getCurrentTimeT();
        int tabIndex = layer->getTag();
        std::string value = "";
        value += BSM->getStrFromTime(now);
        UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), value);
        BSM->saveUserData(strmake("gem=%d", GM->getGem()));
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
        return;
    }
}
void BattleHud::onUnitClickInUpgradeTab(Ref* ref){
    BTN_FROM_REF
    UnitInfo* info = unitInfoListInventory.at(btn->getTag());
    
    Node* layer = this->getChildByName("trainLayer");
    
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    ScrollView* sv = (ScrollView*)ndUpgrade->getChildByName("sv");
    svUpgardeX = sv->getInnerContainerPosition().x;
    Button* btn0 = (Button*)ndUpgrade->getChildByName("btnUpgrade0");
    Button* btn1 = (Button*)ndUpgrade->getChildByName("btnUpgrade1");
    bool isInserted = false;
    if (btn0->getTag() < 0) {
        isInserted = true;
        int tag = info->unitType + info->level*1000;
        btn0->setTag(tag);
    }else if (btn1->getTag() < 0) {
        isInserted = true;
        btn1->setTag(info->unitType + info->level*1000);
    }
    
    updateUnitListForUpgrade();
}
void BattleHud::onTrainGachaClick(Ref* ref){
    BTN_FROM_REF
    if (btn->getTag() == 0) {
        GM->showVideo(VIDEO_GACHA);
    }else if (btn->getTag() == 1) {
        if(unitInfoListInventory.size() >= inventoryCountMax){
            showInstanceMessage(LM->getText("inventory full"));
            return;
        }
        std::string strNextGachaFreeTime = UDGetStr(KEY_GACHA_NEXT_FREE_TIME, "");
        time_t nextGachaFreeTimeT = BSM->getTimeTFromStr(strNextGachaFreeTime);
        time_t now = BSM->getCurrentTimeT();
//        bool isFree = now >= nextGachaFreeTime;
        bool isFree = difftime(nextGachaFreeTimeT, now) < 0;
        
        if(GM->getGem() < 100 && !isFree){
            showInstanceMessage(LM->getText("not enough gems"));
            return;
        }
        
        if(isFree){
            std::string value = "";
            value += BSM->getStrFromTime(now + 60*60*23 + 60*30);
            UDSetStr(KEY_GACHA_NEXT_FREE_TIME, value);
        }else{
            GM->addGem(-100);
        }
        
        std::vector<int> array;
        array.push_back(getRandomUnit());
        showItemGetAndAddToBag(array);
    }else if (btn->getTag() == 2) {
        if(GM->getGem() < 1000){
            showInstanceMessage(LM->getText("not enough gems"));
            return;
        }if(unitInfoListInventory.size() + 11 > inventoryCountMax){
            showInstanceMessage(LM->getText("inventory full"));
            return;
        }
        GM->addGem(-1000);
        std::vector<int> array;
        for (int i = 0; i < 11; i++) {
            array.push_back(getRandomUnit());
        }
        showItemGetAndAddToBag(array);
    }
}
void BattleHud::showItemGetAndAddToBag(std::vector<int> array){
    for(int i = 0; i < array.size(); i++){
        UnitInfo* info = new UnitInfo();
        info->unitType = array.at(i);
        unitInfoListInventory.push_back(info);
    }
    
    closePopup();
    closePopup();
    onTrainClick();
    onTrainTabClick(this->getChildByName("trainLayer")->getChildByName("btnTab2"));
    
    Node* layer = CSLoader::createNode("ItemGet.csb");
    Node* ndFront = layer->getChildByName("ndFront");
//    ndFront->setContentSize(size);
    //    layer->setPosition(size/2);
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("itemGet");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    ndFront->setPositionX(-layer->getPositionX());
    GM->addLightStormEffect(ndFront);
    
    ImageView* img;ImageView* imgWhiteClone;
    ImageView* imgWhite = (ImageView*)layer->getChildByName("imgWhiteCard");
    int value;
    Sprite* spt;
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
    for (int i = 0; i < array.size(); i++) {
        img = (ImageView*)imgTemp->clone();
        ndFront->addChild(img);
        value = array.at(i);
        
        spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(array.at(i)));
        img->addChild(spt);
        spt->setOpacity(0);
        spt->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        if(spt->getChildren().size() > 0){
            spt->getChildren().at(0)->setOpacity(0);
            spt->getChildren().at(0)->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime), FadeIn::create(fadeInTime), NULL));
        }
        imgWhiteClone = (ImageView*)imgWhite->clone();
        imgWhiteClone->setOpacity(0);
        img->getChildByName("lbl")->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeOut::create(fadeOutTime), NULL));
        int rareness = 0;//GM->getItemRareness(value);
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeIn::create(fadeInTime), DelayTime::create(whiteStayTime + rareness*0.1f), FadeOut::create(fadeOutTime), NULL));
        int angle = 5;
        float dur = 0.05f;
        imgWhiteClone->runAction(Sequence::create(DelayTime::create(showOffTime + initTime), FadeIn::create(0.1f), NULL));
        imgWhiteClone->runAction(RepeatForever::create(Sequence::create(RotateBy::create(dur, angle), RotateBy::create(dur*2, -2*angle), RotateBy::create(dur, angle), NULL)));
//        img->runAction(RepeatForever::create(Sequence::create(RotateBy::create(dur, angle), RotateBy::create(dur*2, -2*angle), RotateBy::create(dur, angle), NULL)));
//        img->loadTexture(getCardNameForRareness(rareness));
//        img->setContentSize(Size(29, 26));
//        if(rareness == ITEM_RARENESS_NORMAL){
//            img->setCapInsets(Rect(5, 5, 5, 5));
//            addStarsToIcon(img, value/1000 + 1);
//        }else if(rareness == ITEM_RARENESS_5STAR){
//            img->setContentSize(Size(40, 30));
//        }
//        if(rareness >= ITEM_RARENESS_1STAR && rareness <= ITEM_RARENESS_5STAR){
//            addStarsToIcon(img, rareness + 1);
//        }
//        addGlowBack(img, rareness*0.15f);
        img->getChildByName("lbl")->setPosition(img->getContentSize()/2);
        spt->setPosition(Vec2(img->getContentSize().width/2, img->getContentSize().height/2 + 2));
        if(array.size() - i > 0){
            itemCountInThisRow = columnCount;
        }else{
            itemCountInThisRow = array.size()%columnCount;
        }
        totalWidthInThisRow = itemCountInThisRow*(itemWidth + gap) - gap;
        img->setPosition(Vec2(centerX - totalWidthInThisRow*scale/2 + (i%columnCount)*(itemWidth + gap)*scale + itemWidth*scale/2, y + (i/columnCount)*(img->getContentSize().height*scale + gap/2)));
        
        ndFront->addChild(imgWhiteClone);
        imgWhiteClone->setPosition(img->getPosition());
        
        float dropDistance = size.height;
        img->setPositionY(img->getPositionY() + dropDistance);
        img->runAction(Sequence::create(DelayTime::create(totalDelay), EaseBackOut::create(MoveBy::create(showOffTime, Vec2(0, -dropDistance))), NULL));
        
        showOffTime += delayBetweenItems;
    }
    
    Button* btnAgain = (Button*)layer->getChildByName("btnAgain");
    btnAgain->addClickEventListener(CC_CALLBACK_1(BattleHud::onTrainGachaAgainClick, this));
    btnAgain->setOpacity(0);
    btnAgain->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime), FadeIn::create(fadeInTime), NULL));
    Text* lblPickCount = (Text*)btnAgain->getChildByName("lblPickCount");
    lblPickCount->setString(strmake("x%d", (int)array.size()));
    btnAgain->setTag(array.size());
    Text* lblPrice = (Text*)btnAgain->getChildByName("lblPrice");
    if (array.size() == 1) {
        lblPrice->setString("90");
    }else{ // 11 ea
        lblPrice->setString("900");
    }
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime + 1), FadeIn::create(fadeInTime), NULL));
    
    
    imgTemp->removeFromParent();
    imgWhite->removeFromParent();
    
//    BHUD->showIndicator();
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_TREE);
    datas.push_back(DATA_TYPE_GEM);
    updateInventorySaveData();
    datas.push_back(DATA_TYPE_INVENTORY);
    datas.push_back(DATA_TYPE_SEARCH_STATE);
    saveUserData(datas);
}
void BattleHud::onTrainGachaAgainClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if (btn->getTag() == 1) {
        if(GM->getGem() < 90){
            showInstanceMessage(LM->getText("not enough gems"));
            return;
        }
        if(unitInfoListInventory.size() >= inventoryCountMax){
            showInstanceMessage(LM->getText("inventory full"));
            return;
        }
        std::string strNextGachaFreeTime = UDGetStr(KEY_GACHA_NEXT_FREE_TIME, "");
        time_t nextGachaFreeTimeT = BSM->getTimeTFromStr(strNextGachaFreeTime);
        time_t now = BSM->getCurrentTimeT();
        bool isFree = difftime(nextGachaFreeTimeT, now) < 0;//now >= nextGachaFreeTime;
        
        if(isFree){
            std::string value = "";
            value += BSM->getStrFromTime(now + 60*60*23 + 60*30);
            UDSetStr(KEY_GACHA_NEXT_FREE_TIME, value);
        }else{
            GM->addGem(-90);
        }
        
        std::vector<int> array;
        array.push_back(getRandomUnit());
        showItemGetAndAddToBag(array);
    }else { // 11 ea
        if(GM->getGem() < 900){
            showInstanceMessage(LM->getText("not enough gems"));
            return;
        }if(unitInfoListInventory.size() + 11 > inventoryCountMax){
            showInstanceMessage(LM->getText("inventory full"));
            return;
        }
        GM->addGem(-900);
        std::vector<int> array;
        for (int i = 0; i < 11; i++) {
            array.push_back(getRandomUnit());
        }
        showItemGetAndAddToBag(array);
    }
}
Color3B BattleHud::getCardColorForLevel(int level){
    if (level == 0) {
        return Color3B::WHITE;
    }else if (level == 1) {
        return Color3B(65, 154, 186);
    }else if (level == 2) {
        return Color3B(111, 180, 40);
    }else if (level == 3) {
        return Color3B(28 ,192, 185);
    }else if (level == 4) {
        return Color3B(215, 172, 79);
    }else if (level == 5) {
        return Color3B(168, 83, 45);
    }else if (level == 6) {
        return Color3B(236, 116, 10);
    }else{
        return Color3B(213, 0, 0);
    }
}
void BattleHud::onTrainTab0Click(){
    closePopup();
    onTrainClick();
}
void BattleHud::onTrainTabClick(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("trainLayer");
    int currentTab = btn->getTag();
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->setContentSize(i == currentTab?Size(250.35f, 151.09f):Size(250.35f, 123.62f));
        btn->setColor(i == currentTab?Color3B::WHITE:Color3B::GRAY);
//        btn->setLocalZOrder(tabBack->getLocalZOrder() + (i == currentTab?1:-1));
//        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(btn->getLocalZOrder());
        if(i == 2){
            layer->getChildByName("lblQuestion")->setLocalZOrder(btn->getLocalZOrder());
        }
    }
    
    Node* ndUnit = layer->getChildByName("ndUnits");
    ndUnit->setVisible(currentTab == 0);
    Node* ndUpgrade = layer->getChildByName("ndUpgrade");
    ndUpgrade->setVisible(currentTab == 1);
    Node* ndGacha = layer->getChildByName("ndGacha");
    ndGacha->setVisible(currentTab == 2);
    if(currentTab == 0){
        Widget* btnTemp = (Widget*)layer->getChildByName("btnTemp");
        
        Text* lbl = (Text*)ndUnit->getChildByName("lblDeck");
        int foodInUse = getFoodInUse();
        int foodMax = getFoodMax();
        LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/%d)", LM->getText("deck").c_str(), foodInUse, foodMax));
        lbl = (Text*)ndUnit->getChildByName("lblInventory");
        LM->setLocalizedStringNotKey(lbl, strmake("%s(%d/%d)", LM->getText("inventory").c_str(), (int)unitInfoListInventory.size(), inventoryCountMax));
        lbl = (Text*)ndUnit->getChildByName("lblHire");
        LM->setLocalizedString(lbl, "hire");
        
        Button* btn = (Button*)ndUnit->getChildByName("btnRefresh");
        btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onRefreshHireClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "refresh");
        
        ScrollView* svDeck = (ScrollView*)ndUnit->getChildByName("svDeck");
        int gapX = 289;
        for (int i = 0; i < unitsDeck.size(); i++) {
            btn = (Button*)btnTemp->clone();
            btn->setTag(i);
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUnitInDeckClick, this));
            svDeck->addChild(btn);
            btn->setPosition(Vec2(153.05f + 289*i, 118.47f));
            
            Sprite* spt = Sprite::create("uiBox.png");
            spt->setColor(getCardColorForLevel(unitsDeck.at(i)->level));
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
            
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unitsDeck.at(i)->unitType));
            btn->addChild(spt, 1);
            spt->setPosition(btn->getContentSize()/2);
            
            lbl = (Text*)btn->getChildByName("lblLevel");
            lbl->setString(strmake("Lv.%d", unitsDeck.at(i)->level + 1));
            lbl->setLocalZOrder(2);
        }
        if(gapX*(unitsDeck.size() + 1) < svDeck->getContentSize().width){
            svDeck->setInnerContainerSize(svDeck->getContentSize());
        }else{
            svDeck->setInnerContainerSize(Size(gapX*(unitsDeck.size()+1), svDeck->getContentSize().height));
        }
        
        ScrollView* svInventory = (ScrollView*)ndUnit->getChildByName("svInventory");
        UnitInfo* unitInfo;
        int counter = 0;
        for (int i = 0; i < unitInfoListInventory.size(); i++) {
            unitInfo = unitInfoListInventory.at(i);
            if(unitInfo->unitType < 0){
                continue;
            }
            btn = (Button*)btnTemp->clone();
            btn->setTag(i);
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onUnitInInventoryClick, this));
            svInventory->addChild(btn);
            btn->setPosition(Vec2(153.05f + gapX*counter, 118.47f));
            
            Sprite* spt = Sprite::create("uiBox.png");
            spt->setColor(getCardColorForLevel(unitInfo->level));
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
            
            spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unitInfo->unitType));
            btn->addChild(spt, 1);
            spt->setPosition(btn->getContentSize()/2);
            counter++;
            
            lbl = (Text*)btn->getChildByName("lblLevel");
            lbl->setString(strmake("Lv.%d", unitInfo->level + 1));
            lbl->setLocalZOrder(2);
        }
        if(gapX*(counter + 1) < svInventory->getContentSize().width){
            svInventory->setInnerContainerSize(svInventory->getContentSize());
        }else{
            svInventory->setInnerContainerSize(Size(gapX*(counter+1), svInventory->getContentSize().height));
        }
        
        for(int i = 0; i < 3; i++){
            btn = (Button*)ndUnit->getChildByName(strmake("btnHire%d", i));
            btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onHireClick, this));
        }
        updateStatus(ndUnit);
        updateSearchState();
        
        oneSecCheckerForUI = 0;
        updateUI(0);
    }else if(currentTab == 1){ // upgrade
        bool isBarrackExist = false;
        for (auto building:WORLD->heroArray) {
            if(building->unitType == UNIT_BARRACKS){
                isBarrackExist = true;
                break;
            }
        }
        if (!isBarrackExist && btn->getTag() == 1) {
            showInstanceMessage(LM->getText("build barracks first"));
            return;
        }
        
    }else if(currentTab == 2){ // Gacha
        
    }
    oneSecCheckerForUI = 0;
    updateUI(0);
}
void BattleHud::onRefreshHireClick(){
    Node* layer = this->getChildByName("trainLayer");
    Node* ndUnit = layer->getChildByName("ndUnits");
    Button* btn = (Button*)ndUnit->getChildByName("btnRefresh");
    Text* lbl = (Text*)btn->getChildByName("lblGem");
    int gem = Value(lbl->getString()).asInt();
    if (GM->getGem() >= gem) {
        GM->addGem(-gem);
        refreshSearchState();
        std::vector<int> datas;
        datas.push_back(DATA_TYPE_GEM);
        datas.push_back(DATA_TYPE_SEARCH_STATE);
        saveUserData(datas);
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
    }
}
void BattleHud::refreshSearchState(){
    
    std::vector<int> pickedUnitList;
    for (int i = 0; i < 3; i++) {
        pickedUnitList.push_back(getRandomUnit());
    }
    UDSetStr(KEY_SEARCH_UNITS, strmake("%d,%d,%d", pickedUnitList.at(0), pickedUnitList.at(1), pickedUnitList.at(2)));
    UDSetStr(KEY_SEARCH_STATE, "000");
    
    closePopup();
    onTrainClick();
    
    pickedUnitList.clear();
    BSM->saveUserData("search_state=" + UDGetStr(KEY_SEARCH_STATE, "000") + "&search_items=" + UDGetStr(KEY_SEARCH_UNITS, strmake("%d,%d,%d", UNIT_SWORDMAN, UNIT_ARCHER, UNIT_SWORDMAN)));
}
int BattleHud::getRandomUnit(){
    std::vector<int> unitList;
    std::vector<int> unitPossibilityPointList;
    unitList.push_back(UNIT_SWORDMAN);
    unitList.push_back(UNIT_ARCHER);
    unitList.push_back(UNIT_CATAPULT);
    unitList.push_back(UNIT_HELICOPTER);
    unitList.push_back(UNIT_GOBLIN);
    unitList.push_back(UNIT_GOBLIN_BOMB);
    unitList.push_back(UNIT_ORC_AXE);
    unitList.push_back(UNIT_ORC_SPEAR);
    unitList.push_back(UNIT_TROLL);
    unitList.push_back(UNIT_ZOMBIE_SWORDSMAN);
    unitList.push_back(UNIT_ZOMBIE_ORC_AXE);
    
    unitPossibilityPointList.push_back(30*2);
    unitPossibilityPointList.push_back(30*2);
    unitPossibilityPointList.push_back(12*2);
    unitPossibilityPointList.push_back(5*2);
    unitPossibilityPointList.push_back(18*2);
    unitPossibilityPointList.push_back(1);
    unitPossibilityPointList.push_back(7*2);
    unitPossibilityPointList.push_back(7*2);
    unitPossibilityPointList.push_back(1);
    unitPossibilityPointList.push_back(1);
    unitPossibilityPointList.push_back(1);
    
    int maxPP = 0;
    for(auto point: unitPossibilityPointList){
        maxPP += point;
    }
    int randomNumber;
    int index;
    
    randomNumber = rand()%maxPP + 1;
    index = 0;
    while(true){
        if (randomNumber >= unitPossibilityPointList.at(index)) {
            randomNumber -= unitPossibilityPointList.at(index);
        }else{
            break;
        }
        if(randomNumber == 0){
            break;
        }
        index++;
    }
    
    unitPossibilityPointList.clear();
    int pickedUnit = unitList.at(index);
    unitList.clear();
    return pickedUnit;
}
void BattleHud::onUnitInInventoryClick(Ref* ref){
    BTN_FROM_REF
    selectedUnitInfo = unitInfoListInventory.at(btn->getTag());
    showUnitTrainInfo(1, selectedUnitInfo->unitType, selectedUnitInfo->level);
}
void BattleHud::onUnitInDeckClick(Ref* ref){
    BTN_FROM_REF
    selectedUnitInfo = unitInfoListDeck.at(btn->getTag());
    showUnitTrainInfo(2, selectedUnitInfo->unitType, selectedUnitInfo->level);
}
void BattleHud::onHireClick(Ref* ref){
    BTN_FROM_REF
    hireIndex = btn->getTag();
    std::string searchState = UDGetStr(KEY_SEARCH_STATE, "000");
    if(searchState.at(hireIndex) == '1'){
        showInstanceMessage(LM->getText("already hired"));
        return;
    }
    
    std::string strUnits = UDGetStr(KEY_SEARCH_UNITS, strmake("%d,%d,%d", UNIT_SWORDMAN, UNIT_ARCHER, UNIT_SWORDMAN));
    ValueVector unitList = GM->split(strUnits, ",");
    selectedUnitInfo = new UnitInfo();
    selectedUnitInfo->unitType = unitList[hireIndex].asInt();
//    selectedUnitInfo->belongTo = BELONG_TO_HIRE;
    showUnitTrainInfo(0, selectedUnitInfo->unitType, 0);
}
// popupType 0-hire, 1-train, 2-deck
void BattleHud::showUnitTrainInfo(int popupType, int unit, int level){
    Node* layer = CSLoader::createNode("UnitTrainInfo.csb");
    this->addChild(layer, 4);
    layer->setName("unitTrainLayer");
    setAsPopup(layer);
    layer->setTag(popupType);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    Button* btnUpgrade = (Button*)layer->getChildByName("btnUpgrade");
    btnUpgrade->addClickEventListener(CC_CALLBACK_0(BattleHud::onUpgradeUnitClick, this));
    btnUpgrade->setVisible(false);//popupType == 1);
    Text* lbl = (Text*)btnUpgrade->getChildByName("lblGold");
    lbl->setString(Value(getUnitUpgradePriceGold(unit, level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTree");
    lbl->setString(Value(getUnitUpgradePriceTree(unit, level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade");
    Button* btnHire = (Button*)layer->getChildByName("btnHire");
    btnHire->addClickEventListener(CC_CALLBACK_1(BattleHud::onHireUnitClick, this));
    btnHire->setVisible(popupType == 0);
    lbl = (Text*)btnHire->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "hire");
    lbl = (Text*)btnHire->getChildByName("lblGold");
    lbl->setString(Value(getUnitUpgradePriceGold(unit, level)).asString());
    lbl = (Text*)btnHire->getChildByName("lblTree");
    lbl->setString(Value(getUnitUpgradePriceTree(unit, level)).asString());
    Button* btnMoveToDeck = (Button*)layer->getChildByName("btnMoveToDeck");
    btnMoveToDeck->addClickEventListener(CC_CALLBACK_0(BattleHud::onMoveToDeckUnitClick, this));
    btnMoveToDeck->setVisible(popupType == 1);
    lbl = (Text*)btnMoveToDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "move to deck");
    Button* btnRemoveFromDeck = (Button*)layer->getChildByName("btnRemoveFromDeck");
    btnRemoveFromDeck->addClickEventListener(CC_CALLBACK_0(BattleHud::onRemoveFromDeckUnitClick, this));
    btnRemoveFromDeck->setVisible(popupType == 2);
    lbl = (Text*)btnRemoveFromDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "remove from deck");
    Button* btnFire = (Button*)layer->getChildByName("btnFire");
    btnFire->addClickEventListener(CC_CALLBACK_0(BattleHud::onFireUnitClick, this));
    btnFire->setVisible(popupType == 1);
    lbl = (Text*)btnFire->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "fire");
    btn = (Button*)layer->getChildByName("btnComplete");
    btn->addClickEventListener(CC_CALLBACK_1(BattleHud::onCompleteUnitTrainClick, this));
    btn->setVisible(popupType == 1 && selectedUnitInfo->unitState == UNIT_STATE_UPGRADE);
    btn->setPosition(btnUpgrade->getPosition());
    
    Node* nd = layer->getChildByName("ndUpgradeTime");
    nd->setVisible(false);//popupType == 1);
    nd->setLocalZOrder(100);
    lbl = (Text*)nd->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade time");
    lbl = (Text*)nd->getChildByName("lblTime");
    lbl->setString(GM->getTimeLeftInString(getUnitUpgradeTime(unit, level)));
    nd = layer->getChildByName("ndUpgradeTimer");
    nd->setVisible(false);
    lbl = (Text*)layer->getChildByName("lblHP");
    lbl->setString(Value(GM->getUnitHP(unit, level)).asString());
    lbl = (Text*)layer->getChildByName("lblLevel");
    lbl->setString(strmake("Lv.%d", level + 1));
    lbl = (Text*)layer->getChildByName("lblName");
    lbl->setString(LM->getText(GM->getUnitName(unit)));
    lbl = (Text*)layer->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, strmake("%s desc", GM->getUnitName(unit).c_str()));
    Node* ndAttack = layer->getChildByName("ndAttack");
    lbl = (Text*)ndAttack->getChildByName("lblAttack");
    lbl->setString(Value(GM->getUnitATT(unit, level)).asString());
    lbl = (Text*)ndAttack->getChildByName("lblAttackAdd");
    lbl->setVisible(false);//popupType == 1);
    lbl->setString(strmake("+%d", GM->getUnitATT(unit, level+1) - GM->getUnitATT(unit, level)));
    lbl = (Text*)layer->getChildByName("lblHPAdd");
    lbl->setVisible(false);//popupType == 1);
    lbl->setString(strmake("+%d", GM->getUnitHP(unit, level+1) - GM->getUnitHP(unit, level)));
    Node* ndPopulation = layer->getChildByName("ndPopulation");
    lbl = (Text*)ndPopulation->getChildByName("lblPopulation");
    lbl->setString(Value(GM->getFoodUseForUnit(selectedUnitInfo->unitType)).asString());
    
    Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unit));
    layer->addChild(spt);
    spt->setPosition(layer->getChildByName("imgIconBack")->getPosition());
    
    if (popupType == 0) { // hire
        btnHire->setPosition(btnUpgrade->getPosition());
    }else if (popupType == 1) { // train
        btnFire->setPosition(btnMoveToDeck->getPosition());
        btnMoveToDeck->setPosition(btnHire->getPosition());
    }else if (popupType == 2) { // deck
        btnRemoveFromDeck->setPosition(btnUpgrade->getPosition());
    }
    oneSecCheckerForUI = 0;
    updateUI(0);
}
void BattleHud::onCompleteUnitTrainClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Text* lbl = (Text*)btn->getChildByName("lblCount");
    int gem = Value(lbl->getString()).asInt();
    if (GM->getGem() >= gem) {
        GM->addGem(-gem);
        selectedUnitInfo->endTime = BSM->getCurrentTimeT();
        reopenUnitTrainInfo();
    }else{
        showInstanceMessage(LM->getText("not enough gems"));
    }
}
void BattleHud::reopenUnitTrainInfo(){
    oneSecCheckerForUI = 0;
    reopenUnitTrainInfoRequested = true;
}
int BattleHud::getFoodInUse(){
    int food = 0;
    for(auto info : unitInfoListDeck){
        food += GM->getFoodUseForUnit(info->unitType);
    }
    return food;
}
int BattleHud::getFoodMax(){
    int foodMax = 0;
    for(auto building: buildings){
        if(building->unitState == UNIT_STATE_IDLE || building->unitState == UNIT_STATE_UPGRADE){
            foodMax += getFoodProvide(building->unitType);
        }
    }
    return foodMax;
}
int BattleHud::getBattleCooltime(){
    return 60;
}
void BattleHud::onUpgradeUnitClick(){
    int barracksCount = 0;
    for(auto building : buildings){
        if(building->unitType == UNIT_BARRACKS){
            barracksCount++;
        }
    }
    int upgradeCount = 0;
    for(auto unit : unitInfoListInventory){
        if(unit->unitState == UNIT_STATE_UPGRADE){
            upgradeCount++;
        }
    }
    if(upgradeCount >= barracksCount){
        showNotEnoughBarracksUseGem();
        return;
    }
    int castleLevel = UDGetInt(KEY_CASTLE_LEVEL, 0);
    if(selectedUnitInfo->level >= castleLevel){
        showInstanceMessage(LM->getText("upgrade castle alert"));
        return;
    }
    int goldPrice = getUnitUpgradePriceGold(selectedUnitInfo->unitType, selectedUnitInfo->level);
    int treePrice = getUnitUpgradePriceTree(selectedUnitInfo->unitType, selectedUnitInfo->level);
    if(GM->getCoin() < goldPrice){
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    if(GM->getTree() < treePrice){
        showInstanceMessage(LM->getText("not enough lumber"));
        return;
    }
    
    GM->addCoin(-goldPrice);
    GM->addTree(-treePrice);
    
    int timeLeft = getUnitUpgradeTime(selectedUnitInfo->unitType, selectedUnitInfo->level);
    selectedUnitInfo->unitState = UNIT_STATE_UPGRADE;
    selectedUnitInfo->endTime = BSM->getCurrentTimeT() + timeLeft;
    
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_TREE);
    updateInventorySaveData();
    datas.push_back(DATA_TYPE_INVENTORY);
    saveUserData(datas);
    
    reopenUnitTrainInfo();
}
void BattleHud::onHireUnitClick(Ref* ref){
    BTN_FROM_REF
    Text* lbl = (Text*)btn->getChildByName("lblGold");
    int gold = Value(lbl->getString()).asInt();
    if(gold > GM->getCoin()){
        showInstanceMessage(LM->getText("not enough gold"));
        return;
    }
    lbl = (Text*)btn->getChildByName("lblTree");
    int tree = Value(lbl->getString()).asInt();
    if(tree > GM->getTree()){
        showInstanceMessage(LM->getText("not enough lumber"));
        return;
    }
    if(unitInfoListInventory.size() >= inventoryCountMax){
        showInstanceMessage(LM->getText("inventory full"));
        return;
    }
    std::string strOld = UDGetStr(KEY_SEARCH_STATE, "000");
    int indices[3];
    for(int i = 0; i < 3; i++){
        if(i == hireIndex){
            indices[i] = 1;
        }else{
            indices[i] = Value(strOld.substr(i, 1)).asInt();
        }
    }
    std::string strNew = strmake("%d%d%d", indices[0], indices[1], indices[2]);
    UDSetStr(KEY_SEARCH_STATE, strNew);
    GM->addCoin(-gold);
    GM->addTree(-tree);
    unitInfoListInventory.push_back(selectedUnitInfo);
    closePopup();
    closePopup();
    onTrainClick();
    showUnitTrainInfo(1, selectedUnitInfo->unitType, selectedUnitInfo->level);
    
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_GOLD);
    datas.push_back(DATA_TYPE_TREE);
    updateInventorySaveData();
    datas.push_back(DATA_TYPE_INVENTORY);
    datas.push_back(DATA_TYPE_SEARCH_STATE);
    saveUserData(datas);
}

void BattleHud::updateUnitInfoInList(){
    int index = 0;
    for(auto unitInfo: unitInfoListDeck){
//        unitInfo->index = index;
        index++;
    }
    index = 0;
    for(auto unitInfo: unitInfoListInventory){
//        unitInfo->index = index;
        index++;
    }
}
void BattleHud::onMoveToDeckUnitClick(){
    int foodMax = getFoodMax();
    int foodInUse = getFoodInUse();
    if (foodInUse + GM->getFoodUseForUnit(selectedUnitInfo->unitType) > foodMax) {
        showInstanceMessage(LM->getText("not enough food"));
        return;
    }
    int index= 0;
    for(auto info: unitInfoListInventory){
        if(info == selectedUnitInfo){
            unitInfoListInventory.erase(unitInfoListInventory.begin() + index);
            break;
        }
        index++;
    }
    
    addUnitToDeck(selectedUnitInfo);
    
    closePopup();
    closePopup();
    onTrainClick();
    
    updateInventorySaveData();
    updateDeckSaveData();
//    std::vector<int> datas;
//    datas.push_back(DATA_TYPE_DECK);
//    datas.push_back(DATA_TYPE_INVENTORY);
//    saveUserData(datas);
    showUnitTrainInfo(2, selectedUnitInfo->unitType, selectedUnitInfo->level);
    shouldSaveDeckAndInventoryWhenExitTrainPopup = true;
}
void BattleHud::addUnitToDeck(UnitInfo* info){
    unitInfoListDeck.push_back(info);
    updateUnitInfoInList();
    
    Movable* unit = GM->getUnitFromData(info);
    bool isBarrackExist = false;
    for (auto building:WORLD->heroArray) {
        if(building->unitType == UNIT_BARRACKS){
            unit->setPosition(building->getPosition() + Vec2(0, -200));
            info->x = unit->getPositionX();
            info->y = unit->getPositionY();
            isBarrackExist = true;
            break;
        }
    }
    if(!isBarrackExist){
        for (auto building:WORLD->heroArray) {
            if(building->unitType == UNIT_CASTLE){
                unit->setPosition(building->getPosition() + Vec2(0, -200));
                info->x = unit->getPositionX();
                info->y = unit->getPositionY();
                break;
            }
        }
    }
    
    WORLD->addUnit(unit, false);
    unit->stop();
    unitsDeck.pushBack(unit);
}
void BattleHud::onRemoveFromDeckUnitClick(){
    if(unitInfoListInventory.size() >= inventoryCountMax){
        showInstanceMessage(LM->getText("inventory full"));
        return;
    }
    int index= 0;
    for(auto info: unitInfoListDeck){
        if(info == selectedUnitInfo){
            unitInfoListDeck.erase(unitInfoListDeck.begin() + index);
            break;
        }
        index++;
    }
    
    Movable* unit = unitsDeck.at(index);
    unitsDeck.eraseObject(unit);
//    unitInfoListDeck.erase(unitInfoListDeck.begin() + selectedUnitInfo->index);
    unitInfoListInventory.push_back(selectedUnitInfo);
    updateUnitInfoInList();
    updateInventorySaveData();
    updateDeckSaveData();
//    std::vector<int> datas;
//    datas.push_back(DATA_TYPE_DECK);
//    datas.push_back(DATA_TYPE_INVENTORY);
//    saveUserData(datas);
    closePopup();
    closePopup();
    onTrainClick();
    showUnitTrainInfo(1, unit->unitType, unit->level);
    
    WORLD->removeCharacter((EnemyBase*)unit);
    shouldSaveDeckAndInventoryWhenExitTrainPopup = true;
}
void BattleHud::onFireUnitClick(){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, 4);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "no");
    btn = (Button*)layer->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onConfirmFireClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "yes");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "fire ask");
}
void BattleHud::onConfirmFireClick(){
    int index= 0;
    for(auto info: unitInfoListInventory){
        if(info == selectedUnitInfo){
            unitInfoListInventory.erase(unitInfoListInventory.begin() + index);
            break;
        }
        index++;
    }
    updateUnitInfoInList();
    closePopup();// message box
    closePopup();// info popup
    closePopup();// train popup
    onTrainClick();
    
    updateInventorySaveData();
    std::vector<int> datas;
    datas.push_back(DATA_TYPE_INVENTORY);
    saveUserData(datas);
}
void BattleHud::updateSearchState(){
    std::string searchState = UDGetStr(KEY_SEARCH_STATE, "000");
    Button* btn;
    Node* layer = this->getChildByName("trainLayer");
    Node* ndUnits = layer->getChildByName("ndUnits");
    std::string strUnits = UDGetStr(KEY_SEARCH_UNITS, strmake("%d,%d,%d", UNIT_SWORDMAN, UNIT_ARCHER, UNIT_SWORDMAN));
    ValueVector unitList = GM->split(strUnits, ",");
    for(int i = 0; i < 3; i++){
        btn = (Button*)ndUnits->getChildByName(strmake("btnHire%d", i));
        btn->removeAllChildren();
        Sprite* spt = Sprite::createWithSpriteFrameName(WORLD->getSpriteNameForUnit(unitList.at(i).asInt()));
        btn->addChild(spt);
        spt->setPosition(Vec2(btn->getContentSize().width/2, btn->getContentSize().height/2));
        
        if(searchState.at(i) == '1'){
            Sprite* spt = Sprite::create("tagHired.png");
            btn->addChild(spt);
            spt->setPosition(btn->getContentSize()/2);
        }
    }
}
void BattleHud::onTrainUnitClick(int unitIndex){
    
}
void BattleHud::onCancelTrainUnitClick(Ref* ref){
    BTN_FROM_REF
}
void BattleHud::onExitGame(){
    
}
void BattleHud::onEnterGame(){
    
}
void BattleHud::handleReaccess(){
    
}
void BattleHud::updateTrainPopup(){
    
}
void BattleHud::onBanMessageBoxClosed(){
    onBackClick();
    GM->showVideo(VIDEO_SUPPORT_0);
}
void BattleHud::updateUI(float dt){
    if (BSM->isBannedUser) {
        BSM->isBannedUser = false;
        Node* layer = CSLoader::createNode("MessageBox.csb");
        this->addChild(layer, 4);
        setAsPopup(layer);
        layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
        layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
        
        Button* btn = (Button*)layer->getChildByName("btnNo");
        btn->removeFromParent();
        btn = (Button*)layer->getChildByName("btnYes");
        btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onBanMessageBoxClosed, this));
        Text* lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "ok");
        
        lbl = (Text*)layer->getChildByName("lblDescription");
        LM->setLocalizedString(lbl, "banned description");
        
        this->unschedule(schedule_selector(BattleHud::update));
        this->unschedule(schedule_selector(BattleHud::updateUI));
    }
    if(btnComplete->getPositionY() > 0 && selectedUnit != nullptr){
        bool isComplete = true;
        for(auto building : buildings){
            if(building == selectedUnit && (building->unitState == UNIT_STATE_BUILDING || building->unitState == UNIT_STATE_UPGRADE || building->unitState == UNIT_STATE_REMOVING)){
                Text* lbl = (Text*)hudLayer->getChildByName("btnComplete")->getChildByName("lblCount");
                lbl->setString(Value(GM->getGemForTimeLeft(building->buildingCompleteTimeLeft)).asString());
                isComplete = false;
            }
        }
        if(isComplete){
            onUnitSelected(selectedUnit);
        }
    }
    if(GM->getGem() != lastGemUpdated){
        lastGemUpdated = GM->makeNumberCloseTo(lastGemUpdated, GM->getGem());
        lblGemInHud->setString(Value(lastGemUpdated).asString());
    }
    if(GM->getCoin() != lastGoldUpdated){
        lastGoldUpdated = GM->makeNumberCloseTo(lastGoldUpdated, GM->getCoin());
        lblGoldInHud->setString(Value(lastGoldUpdated).asString());
        float rate = GM->getCoin()*1.0f/getMaxGold();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgGoldGuage")->setContentSize(Size(443.56f*rate, 128.90f));
    }
    if(GM->getTree() != lastTreeUpdated){
        lastTreeUpdated = GM->makeNumberCloseTo(lastTreeUpdated, GM->getTree());
        lblTreeInHud->setString(Value(lastTreeUpdated).asString());
        float rate = GM->getTree()*1.0f/getMaxTree();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgTreeGuage")->setContentSize(Size(443.56f*rate, 128.90f));
    }
    
    // below is for every 1 sec
    oneSecCheckerForUI -= dt;
    if (oneSecCheckerForUI > 0) {
        return;
    }
    oneSecCheckerForUI = 1;
    
    time_t now = BSM->getCurrentTimeT();
    for(auto building : buildings){
        if(building->unitType == UNIT_MINE){
            std::string key = strmake("newminebutton%d_%d", (int)building->getPositionX()/TILE_SIZE, (int)building->getPositionY()/TILE_SIZE);
            Node* ndMineButton = WORLD->getChildByName(key);
            if(ndMineButton == nullptr){
                continue;
            }
            ndMineButton->setVisible(building->unitState == UNIT_STATE_IDLE);
//            building->buildingCompleteTimeLeft += 1;
            std::string strCollectStartTime = UDGetStr(key.c_str(), "");
            if(strCollectStartTime.size() == 0){
                strCollectStartTime = "";
                strCollectStartTime += BSM->getStrFromTime(now - 60*60*24*50);
                UDSetStr(key.c_str(), strCollectStartTime);
            }
//            int collectStartTime = //building->buildingCompleteTimeLeft;
            double timePassed = now - BSM->getTimeTFromStr(strCollectStartTime);
            if(timePassed > 86400){
                timePassed = 86400;
            }else if(timePassed < 0){
                timePassed = 0;
                UDSetDouble(key.c_str(), now);
            }
            int amount = getMineGoldPerHour(building->level)*(timePassed/360.0f);
            int max = getMineGoldStorage(building->level);
            if(amount > max){
                amount = max;
            }
            if(amount < 0){
                amount = 0;
                UDSetDouble(key.c_str(), now);
            }
            Text* lbl = (Text*)ndMineButton->getChildByName("lbl");
            lbl->setString(Value(amount).asString());
            LoadingBar* pbCollect = (LoadingBar*)ndMineButton->getChildByName("pbCollect");
            pbCollect->setPercent(amount*100.0f/max);
        }else if(building->unitType == UNIT_LUMBERMILL){
            std::string key = strmake("treeButton%d_%d", (int)building->getPositionX()/TILE_SIZE, (int)building->getPositionY()/TILE_SIZE);
            Node* ndTreeButton = WORLD->getChildByName(key);
            if(ndTreeButton == nullptr){
                continue;
            }
            ndTreeButton->setVisible(building->unitState == UNIT_STATE_IDLE);
            
            std::string strCollectStartTime = UDGetStr(key.c_str(), "");
            if(strCollectStartTime.size() == 0){
                strCollectStartTime = "";
                strCollectStartTime += BSM->getStrFromTime(now - 60*60*24*50);
                UDSetStr(key.c_str(), strCollectStartTime);
            }
            double timePassed = now - BSM->getTimeTFromStr(strCollectStartTime);
            if(timePassed > 86400){
                timePassed = 86400;
            }else if(timePassed < 0){
                timePassed = 0;
                UDSetDouble(key.c_str(), now);
            }
            int amount = getLumbermillTreePerHour(building->level)*(timePassed/360.0f);
            int max = getLumbermillTreeStorage(building->level);
            if(amount > max){
                amount = max;
            }
            if(amount < 0){
                amount = 0;
                UDSetDouble(key.c_str(), now);
            }
            Text* lbl = (Text*)ndTreeButton->getChildByName("lbl");
            lbl->setString(Value(amount).asString());
            LoadingBar* pbCollect = (LoadingBar*)ndTreeButton->getChildByName("pbCollect");
            pbCollect->setPercent(amount*100.0f/max);
        }
    }
    
    for(auto info: unitInfoListInventory){
        if(info->unitState == UNIT_STATE_UPGRADE){
            double timeLeft = info->endTime - BSM->getCurrentTimeT();
            if(timeLeft > 0){
                int maxTime = getUnitUpgradeTime(info->unitType, info->level);
                if(timeLeft > maxTime){
                    selectedUnit->buildingCompleteTime = BSM->getCurrentTimeT() + maxTime;
                    selectedUnit->buildingCompleteTimeLeft = maxTime;
                    updateBuildingsSaveData();
                    std::vector<int> datas;
                    datas.push_back(DATA_TYPE_BUILDING);
                    saveUserData(datas);
                }
                continue;
            }
            
            info->unitState = UNIT_STATE_IDLE;
            info->level++;
            if(reopenUnitTrainInfoRequested){
                reopenUnitTrainInfoRequested = false;
                closePopup();
                showUnitTrainInfo(1, selectedUnitInfo->unitType, selectedUnitInfo->level);
                
                Node* layer = this->getChildByName("trainLayer");
                if(layer){
                    ScrollView* svInventory = (ScrollView*)layer->getChildByName("svInventory");
                    UnitInfo* unitInfo;
                    for (int i = 0; i < unitInfoListInventory.size(); i++) {
                        unitInfo = unitInfoListInventory.at(i);
                        Node* btn = svInventory->getChildByTag(i);
                        Text* lbl = (Text*)btn->getChildByName("lblLevel");
                        lbl->setString(strmake("Lv.%d", unitInfo->level + 1));
                    }
                }
            }
            
            updateInventorySaveData();
            std::vector<int> datas;
            datas.push_back(DATA_TYPE_INVENTORY);
            datas.push_back(DATA_TYPE_GEM);
            saveUserData(datas);
        }
        
    }
    Node* layer = this->getChildByName("unitTrainLayer");
    if(layer && layer->getTag() == 1){

    }
    
    
    
    int timeLeftToMidnight = 86400 - (long)now%86400;
    bool isNewDay = false;
    std::string strSavedToday = UDGetStr(KEY_TODAY_STRING, "").substr(0, 10);
    std::string strToday = std::string(BSM->getStrFromTime(now)).substr(0, 10);
    if(strToday.compare(strSavedToday) != 0){
        UDSetStr(KEY_TODAY_STRING, strToday);
        isNewDay = true;
    }
//    if(lastTimeLeftToMidnight < timeLeftToMidnight){
//        isNewDay = true;
//    }

    lastTimeLeftToMidnight = timeLeftToMidnight;
    layer = this->getChildByName("trainLayer");
    std::string strGachaWatchedTime = UDGetStr(KEY_VIDEO_GACHA_WATCHED_TIME, "");
    if(strGachaWatchedTime.size() == 0){
        strGachaWatchedTime = "";
        strGachaWatchedTime += BSM->getStrFromTime(now - 60*60*24*50);
        UDSetStr(KEY_VIDEO_GACHA_WATCHED_TIME, strGachaWatchedTime);
    }
    time_t gachaWatchedTimeT = BSM->getTimeTFromStr(strGachaWatchedTime);
    if(difftime(now, gachaWatchedTimeT) < 0){
        gachaWatchedTimeT = now;
        std::string value = "";
        value += BSM->getStrFromTime(now);
        UDSetStr(KEY_VIDEO_GACHA_WATCHED_TIME, value);
    }
    bool isGachaVideoReady = difftime(now, gachaWatchedTimeT) > videoGachaWaitForNextVideoTime;
    std::string strNextGachaFreeTime = UDGetStr(KEY_GACHA_NEXT_FREE_TIME, "");
    time_t nextGachaFreeTimeT = BSM->getTimeTFromStr(strNextGachaFreeTime);
    bool isGachaFreeReady = difftime(nextGachaFreeTimeT, now) <= 0;
    
    hudLayer->getChildByName("btnTrainBR")->getChildByName("imgRedDot")->setVisible(isGachaFreeReady || isGachaVideoReady);
    if(layer && layer != nullptr){
        Node* ndUnits = layer->getChildByName("ndUnits");
        layer->getChildByName("btnTab2")->getChildByName("imgRedDot")->setVisible(isGachaFreeReady || isGachaVideoReady);
        if(ndUnits->isVisible()){
//            int index = 0;Node* slot;Node* arrow;
//            for(auto info : unitInfoListInventory){
//                slot = ndUnits->getChildByName("svInventory")->getChildByTag(index);
//                arrow = slot->getChildByName(strmake("upgradeArrow_%d_%d", (int)slot->getPositionX(), (int)slot->getPositionY()));
//                if(info->endTime - BSM->getCurrentTime() > 0 && !arrow){
//                    addUpgradeEffect(slot, false);
//                    //                spt->setName(strmake("upgradeArrow_%d_%d", (int)node->getPositionX(), (int)node->getPositionY()));
//                }else if(info->endTime - BSM->getCurrentTime() <= 0 && arrow){
//                    slot->getChildByName(strmake("upgradeArrow_%d_%d", (int)slot->getPositionX(), (int)slot->getPositionY()))->removeFromParent();
//                }
//                index++;
//            }
            Text* lbl = (Text*)ndUnits->getChildByName("lblHireDescription");
            lbl->setString(strmake("%s\n(%s)", LM->getText("new soldiers").c_str(), GM->getTimeLeftInString(timeLeftToMidnight).c_str()));
            int lastHireRefreshDay = UDGetInt(KEY_LAST_HIRE_REFRESH_DAY, 0);
            int today = now/86400;
            if (lastHireRefreshDay != today) {
                UDSetInt(KEY_LAST_HIRE_REFRESH_DAY, today);
                refreshSearchState();
            }
        }
        Node* ndUpgrade = layer->getChildByName("ndUpgrade");
        if(ndUpgrade->isVisible()){
            Button* btn = (Button*)ndUpgrade->getChildByName("btnComplete");
            Text* lbl = (Text*)btn->getChildByName("lblTimeLeft");
            int tabIndex = layer->getTag();
            std::string strEndTime = UDGetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "");
            
            time_t endTime = BSM->getTimeTFromStr(strEndTime);
            int upgradeValue = UDGetInt(strmake(KEY_UPGRADE_UNIT_FORMAT, tabIndex).c_str(), -1);
            std::string str(ctime(&endTime));
            log("str: %s", str.c_str());
            log("endTime: %s, now: %s", BSM->getStrFromTime(endTime).c_str(), BSM->getStrFromTime(now).c_str());
            if(upgradeValue >= 0 && endTime < now){
                std::string value = "";
                value += BSM->getStrFromTime(now);
                UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), value);
                log("what: %s", UDGetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), "").c_str());
                endTime = now;
            }
            
            int level = upgradeValue/1000;
            int unitType = upgradeValue%1000;
            int upgradeTime = getUnitUpgradeTime(unitType, level);
            if (endTime - now > upgradeTime) {
                endTime = now + upgradeTime;
                std::string value = "";
                value += BSM->getStrFromTime(endTime);
                UDSetStr(strmake(KEY_UPGRADE_ENDTIME_FORMAT, tabIndex).c_str(), value);
            }
            if(strEndTime.size() > 0){
                if(endTime - now > 0){
                    lbl->setString(GM->getTimeLeftInString(endTime - now));
                    lbl = (Text*)btn->getChildByName("lbl");
                    lbl->setString(Value(GM->getGemForTimeLeft(endTime - now)).asString());
                }else{
                    btn->setVisible(false);
                    btn = (Button*)ndUpgrade->getChildByName("btnFinish");
                    btn->setVisible(true);
                }
            }
        }
        Node* ndGacha = layer->getChildByName("ndGacha");
        if(ndGacha->isVisible()){
            Button* btn = (Button*)ndGacha->getChildByName("btn0");
            Text* lbl = (Text*)btn->getChildByName("lbl");
            lbl->setString(isGachaVideoReady?"READY":GM->getTimeLeftInStringHMS(videoGachaWaitForNextVideoTime - difftime(now, gachaWatchedTimeT)));
            btn->setEnabled(isGachaVideoReady);
            btn->getChildByName("imgRedDot")->setVisible(isGachaVideoReady);
            
            btn = (Button*)ndGacha->getChildByName("btn1");
            btn->getChildByName("imgRedDot")->setVisible(isGachaFreeReady);
            lbl = (Text*)btn->getChildByName("lbl");
            
            Text* lblTimeLeft = (Text*)btn->getChildByName("lblTimeLeft");
            lbl->setString(isGachaFreeReady?"FREE":"100");
            lblTimeLeft->setVisible(!isGachaFreeReady);
            lblTimeLeft->setString(strmake("%s: %s", LM->getText("next free pick").c_str(), GM->getTimeLeftInStringHMS(difftime(nextGachaFreeTimeT, now)).c_str()));
        }
    }
    
    // daily mission
    if(this->getChildByName("dailyMission")){
        Text* lbl = (Text*)this->getChildByName("dailyMission")->getChildByName("lblTimeLeft");
        lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeftToMidnight).c_str()));
    }
    if(isNewDay){
        UDSetBool(KEY_DAILY_MISSION_OPEN, false);
        UDSetBool(KEY_DAILY_MISSION_ARENA, false);
        UDSetBool(KEY_DAILY_MISSION_CAMPAIGN, false);
        UDSetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, false);
        UDSetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, false);
        if(this->getChildByName("dailyMission")){
            closePopup();
            onMissionClick();
        }
        for(int i = 0; i < 4;i++){
            UDSetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, i).c_str(), false);
        }
        
        UDSetInt(KEY_VIDEO_STORE_RANDOM_INDEX, 0);
        UDSetInt(KEY_VIDEO_STORE_UNIT_INDEX, 0);
        UDSetInt(KEY_VIDEO_STORE_GEM_INDEX, 0);
    }
    
    if(this->getChildByName("defenceRecordPopup")){
        Node* layer = this->getChildByName("defenceRecordPopup");
        Text* lbl = (Text*)layer->getChildByName("lblShieldLeft");
        Button* btn = (Button*)layer->getChildByName("btnPlayForShield");
        if(difftime(shieldEndTimeT, now) < 0){
            LM->setLocalizedString(lbl, "expired");
            btn->setEnabled(true);
        }else{
            int timeLeft = difftime(shieldEndTimeT, now);
            lbl->setString(GM->getTimeLeftInString(timeLeft));
            btn->setEnabled(timeLeft < 3600);
        }
    }
    
    // battle popup
    layer = this->getChildByName("battlePopup");
    if(layer != nullptr){
        Text* lbl;
        int restingEndTime = GM->restingForBattleEndTime;
        if(now > restingEndTime){
            layer->getChildByName("btnCompleteResting")->setVisible(false);
            layer->getChildByName("btnFindMatch")->setVisible(true);
        }else{
            layer->getChildByName("btnFindMatch")->setVisible(false);
            Node* nd = layer->getChildByName("btnCompleteResting");
            nd->setVisible(true);
            lbl = (Text*)nd->getChildByName("lblRestingTimer");
            int timeLeft = (int)(restingEndTime - now);
            lbl->setString(GM->getTimeLeftInString(timeLeft));
            lbl = (Text*)nd->getChildByName("lblGem");
            lbl->setString(Value(GM->getGemForTimeLeft(timeLeft)).asString());
        }
    }
    
    // special offer
    if(!isSpecialOfferCheckOff && BSM->timeEstablished){
        std::string strOpenTime = UDGetStr(KEY_SPECIAL_OFFER_OPEN_TIME, "");
        time_t openTimeT = BSM->getTimeTFromStr(strOpenTime);
        int offerTime = 60*60*24;
        int timeLeft = offerTime - (difftime(now, openTimeT));
        if(this->getChildByName("specialOffer")){
            Node* layer = this->getChildByName("specialOffer");
            Text* lbl = (Text*)layer->getChildByName("lblTimeLeft");
            if(timeLeft < 0){
                LM->setLocalizedString(lbl, "last chance");
            }else{
                lbl->setString(strmake("(%s)", GM->getTimeLeftInString(timeLeft).c_str()));
            }
            lbl = (Text*)layer->getChildByName("btn0")->getChildByName("lbl");
            lbl->setString(strStarterPrice0);
            lbl = (Text*)layer->getChildByName("btn1")->getChildByName("lbl");
            lbl->setString(strStarterPrice1);
            lbl = (Text*)layer->getChildByName("btn2")->getChildByName("lbl");
            lbl->setString(strStarterPrice2);
        }
        if (timeLeft < 0 && openTimeT > 0) {
            if (!UDGetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, false) && popupArray.size() == 0) {
                UDGetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, false);
                showSpecialOfferLastChance();
            }
        }
    }
    // video store
    layer = this->getChildByName("videoStore");
    if(layer != nullptr){
        Text* lbl = (Text*)layer->getChildByName("lblTimer");
        lbl->setString(GM->getTimeLeftInString(timeLeftToMidnight));
        if (isNewDay && popupArray.at(popupArray.size() -1) == layer) {
            closePopup();
            onVideoStoreClick();
            return;
        }
        Node* nd;
        int nextIndex;
        std::string strWatchedTime;
        time_t watchedTimeT;
        if (layer->getTag() == 0) { // random
            strWatchedTime = UDGetStr(KEY_VIDEO_STORE_RANDOM_WATCHED_TIME, "");
            watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
            if(strWatchedTime.size() == 0){
                watchedTimeT = now - 60*60;
                std::string value = "";
                value += BSM->getStrFromTime(now);
                UDSetStr(KEY_VIDEO_STORE_RANDOM_WATCHED_TIME, value);
            }
            nd = layer->getChildByName("ndTabRandom");
            nextIndex = UDGetInt(KEY_VIDEO_STORE_RANDOM_INDEX, 0);
        }else if(layer->getTag() == 1){
            strWatchedTime = UDGetStr(KEY_VIDEO_STORE_UNIT_WATCHED_TIME, "");
            watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
            if(strWatchedTime.size() == 0){
                watchedTimeT = now - 60*60;
                std::string value = "";
                value += BSM->getStrFromTime(now);
                UDSetStr(KEY_VIDEO_STORE_UNIT_WATCHED_TIME, value);
            }
            nd = layer->getChildByName("ndTabUnit");
            nextIndex = UDGetInt(KEY_VIDEO_STORE_UNIT_INDEX, 0);
        }else if(layer->getTag() == 2){
            strWatchedTime = UDGetStr(KEY_VIDEO_STORE_GEM_WATCHED_TIME, "");
            watchedTimeT = BSM->getTimeTFromStr(strWatchedTime);
            if(strWatchedTime.size() == 0){
                watchedTimeT = now - 60*60;
                std::string value = "";
                value += BSM->getStrFromTime(now);
                UDSetStr(KEY_VIDEO_STORE_GEM_WATCHED_TIME, value);
            }
            nd = layer->getChildByName("ndTabGem");
            nextIndex = UDGetInt(KEY_VIDEO_STORE_GEM_INDEX, 0);
        }
        if(nextIndex < 12){
            Button* btn = (Button*)nd->getChildByName(strmake("btnGem%d", nextIndex));
            lbl = (Text*)btn->getChildByName("lblGold");
            Node* img = btn->getChildByName("imgIconGold");
            bool isReady = difftime(now, watchedTimeT) > videoStoreWaitForNextVideoTime;
            
            img->setVisible(isReady);
            if (isReady) {
                GM->alignToCenter(img, lbl, 60, btn->getContentSize().width/2, 0);
                lbl->setString(Value(lbl->getTag()).asString());
                Node* lblQuestion = btn->getChildByName("lblQuestion");
                if(lblQuestion){
                    lblQuestion->setPosition(img->getPosition());
                }
            }else{
                lbl->setPositionX(btn->getContentSize().width/2);
                lbl->setString(GM->getTimeLeftInString(videoStoreWaitForNextVideoTime - (difftime(now, watchedTimeT))));
            }
        }
    }
    
    // attend red dot
    std::string info = UDGetStr(KEY_ATTEND_RCV_INFO, "0_0");
    ValueVector infoData = GM->split(info, "_");
//    int dayIndex = infoData.at(0).asInt()%16;
    int nextDayCount = infoData.at(1).asInt();
    int todaysDayCount = UDGetInt(KEY_DAY_COUNT, 0);
    Node* ndRedDot = hudLayer->getChildByName("btnDailyReward")->getChildByName("imgRedDot");
    ndRedDot->setVisible(todaysDayCount >= nextDayCount);
    
    // mission red dot
    bool isThereSomething = false;
    if (UDGetBool(KEY_DAILY_MISSION_ARENA, false) && !UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, 0).c_str(), false)) {
        isThereSomething = true;
    }else if (UDGetBool(KEY_DAILY_MISSION_CAMPAIGN, false) && !UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, 1).c_str(), false)) {
        isThereSomething = true;
    }else if (UDGetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, false) && !UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, 2).c_str(), false)) {
        isThereSomething = true;
    }else if (UDGetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, false) && !UDGetBool(strmake(KEY_DAILY_MISSION_RECEIVED_FORMAT, 3).c_str(), false)) {
        isThereSomething = true;
    }
    hudLayer->getChildByName("btnMission")->getChildByName("imgRedDot")->setVisible(isThereSomething);
}
void BattleHud::updateMineName(Movable* mine){
    
}
void BattleHud::showSpecialOfferLastChance(){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, 4);
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onNoToSeeTheSpecialOfferForTheLastTime, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "no");
    btn = (Button*)layer->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(BattleHud::onYesToSeeTheSpecialOfferForTheLastTime, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "yes");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "ask special offer last");
}
void BattleHud::onYesToSeeTheSpecialOfferForTheLastTime(){
    closePopup();
    UDSetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, true);
    onSpecialOfferClick();
    isFromLastChance = true;
//    GameSharing::firebaseLog("specialOffer", "specialOffer lastchance", "yes");
    Button* btn = (Button*)hudLayer->getChildByName("btnSpecialOffer");
    if(UDGetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, false)){
        btn->setVisible(false);
    }
}
void BattleHud::onNoToSeeTheSpecialOfferForTheLastTime(){
    closePopup();
    UDSetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, true);
//    GameSharing::firebaseLog("specialOffer", "specialOffer lastchance", "no");
    Button* btn = (Button*)hudLayer->getChildByName("btnSpecialOffer");
    if(UDGetBool(KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED, false)){
        btn->setVisible(false);
    }
}
void BattleHud::onSpeedUpClick(){
    
}
void BattleHud::updateStatus(Node* layer){
//    Text* lbl = (Text*)layer->getChildByName("lblGem");
//    lbl->setString(Value(GM->getGem()).asString());
//
//    lbl = (Text*)layer->getChildByName("lblGold");
//    lbl->setString(Value(GM->getCoin()).asString());
//
//    lbl = (Text*)layer->getChildByName("lblTree");
//    lbl->setString(Value(GM->getTree()).asString());
//
//    lbl = (Text*)layer->getChildByName("lblPopulation");
//    if (lbl != nullptr) {
//        lbl->setString(strmake("%d/%d", getCurrentPopulation(), getPopulationMax()));
//    }
}

int BattleHud::getCurrentPopulation(){
    int count = 2;
    return count;
}

int BattleHud::getPopulationMax(){
    int max = 10;
    return max;
}
void BattleHud::showIndicator(){
    std::string name = "btnIndicator";
    if(this->getChildByName(name) != nullptr){
        return;
    }
    log("show indicator");
    Node* node = Node::create();
    this->addChild(node, 1000);
    setAsPopup(node);
    node->setName(name);
    Button* btn = Button::create("uiBoxSmall.png");
    node->addChild(btn);
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    btn->setPosition(size/2);
    
    Label* lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
    node->addChild(lbl);
    lbl->setPosition(Vec2(size.width/2, size.height/3));
}
void BattleHud::hideIndicator(){
    log("hide indicator");
    std::string strName = "btnIndicator";
    for (int i = 0; i < popupArray.size(); i++) {
        if(popupArray.at(i)->getName().compare(strName) == 0){
            popupArray.erase(popupArray.begin() + i);
            break;
        }
    }
    this->removeChildByName(strName);
}
