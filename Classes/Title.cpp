//
//  Title.cpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 13/03/2018.
//
#include "Intro.h"
#include "Title.h"
#include "LanguageManager.h"
#include "GameManager.h"
#include "GameSharing.h"
//#include "ServerManager.h"
#include "BuggyServerManager.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
//#include "CrossPromotion.h"
//#include "NativeInterface.h"

enum CafeSdkTags {
    kTagHome,
    kTagNotice,
    kTagEvent,
    kTagMenu,
    kTagProfile,
    kTagArticle,
    kTagWrite1,
    kTagWrite2,
    kTagLogin,
    kTagLogout,
    kTagIsLogin,
    kTagGetProfile,
    kTagShowRecord,
    kTagShowScreenShot,
    kTagSendNewUser,
    kTagSendPayUser,
};
bool Title::init()
{
//    UDSetInt(KEY_GOLDEN_TICKET, 0); // test
//    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime() - 30); // test
    
    GM->setHudLayer(nullptr);
    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B(235, 229, 210, 255)))
    {
        return false;
    }
    GM->titleLayer = this;
    size = Director::getInstance()->getWinSize();
    float idleTime = 0.5f;
    float moveTime = 1;
    
    Node* title = CSLoader::createNode("Title.csb");
    this->addChild(title);
    Node* sptBackground = title->getChildByName("btnBackground");//Sprite::create("titleBackground.png");
//    this->addChild(sptBackground);
//    sptBackground->setPosition(Point(size.width/2, size.height/2 - 300));
//    sptBackground->setScale(defaultScale*2224.0f/2048);
    sptBackground->runAction(Sequence::create(DelayTime::create(idleTime), EaseInOut::create(MoveBy::create(moveTime, Point(0, -30)), 3), NULL));
    
    sptTitle = Sprite::create("title.png");
    title->addChild(sptTitle);
    sptTitle->setPosition(Point(size.width/2 - 420, size.height/2 + 280));
    sptTitle->setScale(defaultScale);
    sptTitle->runAction(Sequence::create(DelayTime::create(idleTime), EaseInOut::create(MoveBy::create(moveTime, Point(-200, 100)), 3), NULL));
    
//    float btnX = 570;
    Button* btn = (Button*)title->getChildByName("btnPlay");//Button::create("btnBox.png");
//    this->addChild(btn);
//    btn->setPosition(Point(btnX, 680));
    btn->addClickEventListener(CC_CALLBACK_0(Title::showChapterSelect, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    PPLabel* lbl = addLabelToButton(btn, "play", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
//    Sprite* spt = Sprite::create("weaponX.png");
//    btn->addChild(spt);
//    spt->setPosition(Point(590, 135));
//    spt->setOpacity(0);
//    spt->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    
    btn = (Button*)title->getChildByName("btnArena");//Button::create("btnBox.png");
//    this->addChild(btn);
//    btn->setPosition(Point(btnX, 415));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onArenaClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    lbl = addLabelToButton(btn, "arena", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
//    spt = Sprite::create("comingSoon.png");
//    btn->addChild(spt);
//    spt->setPosition(Point(590, 135));
//    spt->setOpacity(0);
//    spt->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    
    float tagScale = 0.7f;
    
//    Sprite* spt1 = Sprite::create("networkTag.png");
//    btn->addChild(spt1);
//    spt1->setScale(tagScale);
//    spt1->setPosition(Point(590, 170));
//    spt1->setOpacity(0);
//    spt1->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
//
//    Sprite* spt2 = Sprite::create("nonMultiplayTag.png");
//    btn->addChild(spt2);
//    spt2->setScale(tagScale);
//    spt2->setPosition(Point(590, 110));
//    spt2->setOpacity(0);
//    spt2->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    
    btn = (Button*)title->getChildByName("btnBattle");//Button::create("btnBox.png");
//    this->addChild(btn);
//    btn->setPosition(Point(btnX, 150));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onBattleClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    GM->makeItScaleUpAndDown(btn->getChildByName("imgNewTag"));
//    GM->makeItScaleUpAndDown(btn->getChildByName("imgTag"));


    lbl = addLabelToButton(btn, "battle", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
//    Sprite* sptComingSoonTag = Sprite::create("comingSoon.png");
//    btn->addChild(sptComingSoonTag);
//    sptComingSoonTag->setPosition(Point(610, 144));
//    sptComingSoonTag->setOpacity(0);
//    sptComingSoonTag->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    
    btn = (Button*)title->getChildByName("btnCommunity");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onCommunityClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    lbl = addLabelToButton(btn, "forum", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
    btn = (Button*)title->getChildByName("btnSetting");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onSettingClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    lbl = addLabelToButton(btn, "setting", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
    
    Sprite* spt = Sprite::createWithSpriteFrameName("helicopter0.png");
    this->addChild(spt);
    spt->setPosition(size.width/2 + 800, size.height + 300);
    GM->runAnimation(spt, "helicopterStand", true);
    spt->setScale(1.5);
    spt->runAction(Sequence::create(DelayTime::create(idleTime), EaseInOut::create(MoveBy::create(moveTime+0.15f, Point(0, -500)), 3), NULL));
    
    if(GM->market == MARKET_ONESTORE_FREE){
        
        Point pos = Point(size.width - 150, size.height - 150);
        for(int i = 0; i < 5; i++){
            Sprite* spt = Sprite::create("longLight.png");
            this->addChild(spt);
            spt->runAction(RepeatForever::create(RotateBy::create((rand()%10)*0.2 + 1, 150*(rand()%2==0?1:-1))));
            spt->setPosition(pos);
            spt->setScale(0.3f);
        }
        
        Button* btnFull = Button::create("miniRect.png");
        btnFull->setPosition(pos);
        this->addChild(btnFull);
        btnFull->addClickEventListener(CC_CALLBACK_0(Title::onClickFull, this));
        
        PPLabel* lbl = PPLabel::create("full", 50, DARK_GRAY_3B, false, false, TextHAlignment::CENTER, false);
        btnFull->addChild(lbl);
        lbl->setPosition(btnFull->getContentSize()/2);
    }
//    UDSetInt(KEY_LAST_CLEAR_STAGE, -1);// test 
    GM->playSoundEffect(SOUND_BGM_DUAL);
    GM->titleLayer = this;
    this->schedule(schedule_selector(Title::titleUpdate), 0.2f);
    
    listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(Title::onKeyReleased, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
#ifdef SDKBOX_ENABLED
    log("Title PluginSdkboxPlay init");
    if(sdkbox::PluginSdkboxPlay::isSignedIn()){
        log("signed in");
    }else{
        log("not signed in");
//        sdkbox::PluginSdkboxPlay::signin();
    }
    log("Title PluginSdkboxPlay signin");
#endif
    int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
    if(freeTicketUsedDay != BSM->getDay()){
        UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
    }
    
    if(UDGetBool(KEY_GOLDEN_TICKET_FREE, true)){
        UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
    }
//    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX); // test
    
    if(!GM->isCrossShown){
        GM->isCrossShown = true;
//        CrossPromotion* crossLayer = CrossPromotion::create();
//        this->addChild(crossLayer);
    }
    
    Label* lblVersion = LM->getLocalizedLabel();
    this->addChild(lblVersion);
    lblVersion->setAnchorPoint(Vec2::ONE);
    lblVersion->setPosition(size);
    lblVersion->setString(strmake("Ver.%s", GM->version));
    lblVersion->setTextColor(Color4B::BLACK);
    
    lblID = LM->getLocalizedLabel();
    this->addChild(lblID);
    lblID->setAnchorPoint(Vec2(0, 1));
    lblID->setPosition(Vec2(0, size.height));
    lblID->setString(strmake("ID: "));
    lblID->setTextColor(Color4B::BLACK);
    
    lblLoading = LM->getLocalizedLabel();
    this->addChild(lblLoading);
    lblLoading->setPosition(Vec2(size.width/2, 100));
    lblLoading->setString(strmake("Ver.%s", GM->version));
    lblLoading->setTextColor(Color4B::WHITE);
    lblLoading->enableOutline(Color4B::BLACK, 5);
    lblLoading->setVisible(false);
    
//    UDSetStr(KEY_SAVED_ID, "2"); // test 
//    UDSetStr(KEY_SAVED_ID, "-1"); // test
//    UDSetStr(KEY_SAVED_ID, "10"); // test
//    UDSetStr(KEY_BUILDINGS, ""); // test
//    std::string theID = "2";
//    UDSetStr(KEY_SAVED_ID, theID); // test
//    BSM->requestedID = theID; // test
    if (UDGetBool(KEY_FIRST_LAUNCH_AFTER_NEW_SERVER, true)) {
        UDSetBool(KEY_FIRST_LAUNCH_AFTER_NEW_SERVER, false);
        UDSetStr(KEY_SAVED_ID, "-1"); // test
        UDSetInt(KEY_COLOSSEUM_SCORE, 0);
    }
    
    if (UDGetBool(KEY_FIRST_LAUNCH, true)) {
//    if(true){ // test
        // check its free
        UDSetBool(KEY_FIRST_LAUNCH, false); // test  uncomment it when release
        showIndicator(); // test
        BSM->isThisAppFree(); // test
    }
//    isFreeApp = true; // test 
//    isFreeAppResultArrived = true; // test
//    UDSetStr(KEY_SAVED_ID,"-1"); // test
    
    BSM->getHttpTime();
//    UDSetInt(KEY_LAST_CLEAR_STAGE, 10); // test 
//    UDSetBool(KEY_CHAPTER_2_PURCHASED, false); // test
//    UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test 
    log("Title init done");
    
//    spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
//    this->addChild(spChar);
//    spChar->setAnimation(0, "idle", true);
//    spChar->setPosition(Vec2(150, 100));
    
    
//    showExitPopup(); // test
    return true;
}
void Title::onCommunityClick(){
    if(LM->getLanguageType() == LanguageType::KOREAN){
        Application::getInstance()->openURL("https://cafe.naver.com/cartooncraft");
    }else{
        Application::getInstance()->openURL("https://discordapp.com/channels/582944551614939269/582947227945664522");
    }
}
void Title::onSettingClick(){
    Node* layer = CSLoader::createNode("Setting.csb");
    this->addChild(layer, 4);
    layer->setName("settingPopup");
    setAsPopup(layer);
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnMusic");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onMusicInSettingClick, this));
    btn = (Button*)layer->getChildByName("btnSound");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onSoundClick, this));
    btn = (Button*)layer->getChildByName("btnNetworkReset");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onNetworkResetClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "network reset");
    btn = (Button*)layer->getChildByName("btnRestore");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onRestoreClick, this));
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    btn->setVisible(false);
//#endif
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "restore");
    
    lbl = (Text*)layer->getChildByName("lblGPSID");
    lbl->setString(strmake("PLAY ID: [%s]", GM->playerGPSID.c_str()));
    
    btn = (Button*)layer->getChildByName("btnLanguage");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onLanguageClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "language");
    
    updateSettingPopup();
}
void Title::onLanguageClick(){
    Node* layer = CSLoader::createNode("LanguagePopup.csb");
    this->addChild(layer, 4);
    layer->setName("languagePopup");
    setAsPopup(layer);
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    layer->setTag(-1);
    btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onLanguageOkClick, this));
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "language");
    
    ImageView* imgCheck = (ImageView*)layer->getChildByName("imgCheck");
    btn = (Button*)layer->getChildByName("btnKorean");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "korean");
    if (LM->getLanguageType() == LanguageType::KOREAN) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnEnglish");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "english");
    if (LM->getLanguageType() == LanguageType::ENGLISH) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnRussian");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "russian");
    if (LM->getLanguageType() == LanguageType::RUSSIAN) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnFrench");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "french");
    if (LM->getLanguageType() == LanguageType::FRENCH) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnSpanish");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "spanish");
    if (LM->getLanguageType() == LanguageType::SPANISH) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnTurkish");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "turkish");
    if (LM->getLanguageType() == LanguageType::TURKISH) {
        imgCheck->setPosition(btn->getPosition());
    }
    btn = (Button*)layer->getChildByName("btnGerman");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "german");
    if (LM->getLanguageType() == LanguageType::GERMAN) {
        imgCheck->setPosition(btn->getPosition());
    }
}
void Title::onLanguageButtonClick(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("languagePopup");
    layer->setTag(btn->getTag());
    ImageView* imgCheck = (ImageView*)layer->getChildByName("imgCheck");
    
    if(layer->getTag() == 0){
        imgCheck->setPosition(layer->getChildByName("btnKorean")->getPosition());
    }else if(layer->getTag() == 1){
        imgCheck->setPosition(layer->getChildByName("btnEnglish")->getPosition());
    }else if(layer->getTag() == 2){
        imgCheck->setPosition(layer->getChildByName("btnRussian")->getPosition());
    }else if(layer->getTag() == 3){
        imgCheck->setPosition(layer->getChildByName("btnFrench")->getPosition());
    }else if(layer->getTag() == 4){
        imgCheck->setPosition(layer->getChildByName("btnSpanish")->getPosition());
    }else if(layer->getTag() == 5){
        imgCheck->setPosition(layer->getChildByName("btnGerman")->getPosition());
    }else if(layer->getTag() == 6){
        imgCheck->setPosition(layer->getChildByName("btnTurkish")->getPosition());
    }else{
        imgCheck->setPosition(layer->getChildByName("btnEnglish")->getPosition());
    }
}
void Title::onLanguageOkClick(){
    Node* layer = this->getChildByName("languagePopup");
    if(layer->getTag() < 0){
        closePopup();
        return;
    }
    if(layer->getTag() == 0){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::KOREAN);
    }else if(layer->getTag() == 1){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
    }else if(layer->getTag() == 2){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::RUSSIAN);
    }else if(layer->getTag() == 3){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::FRENCH);
    }else if(layer->getTag() == 4){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::SPANISH);
    }else if(layer->getTag() == 5){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::GERMAN);
    }else if(layer->getTag() == 6){
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::TURKISH);
    }else{
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
    }
    restartTheGame();
}
void Title::onRestoreClick(){
    isRestoreRequested = true;
    GameSharing::restoreIAP();
//    GM->playerGPSID = "a_4835022602392990245"; // test
    checkPlayerIDExist();
}
void Title::updateSettingPopup(){
    Node* layer = this->getChildByName("settingPopup");
    Text* lbl = (Text*)layer->getChildByName("btnMusic")->getChildByName("lbl");
    LM->setLocalizedString(lbl, GM->getMusicVolumn()==0?"music off":"music on");
    ImageView* img = (ImageView*)layer->getChildByName("btnMusic")->getChildByName("img");
    img->loadTexture(GM->getMusicVolumn()==0?"iconMusicOff.png":"iconMusic.png");
    lbl = (Text*)layer->getChildByName("btnSound")->getChildByName("lbl");
    LM->setLocalizedString(lbl, GM->getSoundVolumn()==0?"effect off":"effect on");
    img = (ImageView*)layer->getChildByName("btnSound")->getChildByName("img");
    img->loadTexture(GM->getSoundVolumn()==0?"iconSoundOff.png":"iconSound.png");
}
void Title::onMusicInSettingClick(){
    GM->setMusicVolumn(!GM->getMusicVolumn());
    updateSettingPopup();
}
void Title::onSoundClick(){
    GM->setSoundVolumn(!GM->getSoundVolumn());
    updateSettingPopup();
}
void Title::onNetworkResetClick(){
    Node* layer = CSLoader::createNode("MessageBox.csb");
    this->addChild(layer, 4);
    layer->setName("messageBox");
    setAsPopup(layer);
    
    Button* btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Vec2 posLeft = btn->getPosition();
    Button* btnYES = (Button*)layer->getChildByName("btnYes");
    btnYES->addClickEventListener(CC_CALLBACK_0(Title::onNetworkResetConfirmClick, this));
    btn->setPosition(btnYES->getPosition());
    btnYES->setPosition(posLeft);
    
    Text* lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "network reset warning");
}
void Title::onNetworkResetConfirmClick(){
    UDSetStr(KEY_SAVED_ID, "-1");
    UDSetBool(KEY_FIRST_LAUNCH, true);
    UDSetBool(KEY_FIRST_LAUNCH_AFTER_NEW_SERVER, true);
    restartTheGame();
}
void Title::onBattleClick(){
//    log("sku cc_gem100 amount: %s", GameSharing::getPriceAmount("cc_gem100").c_str());
//    showInstanceMessage(LM->getText("coming soon")); // test
//    return; // test 
//    showInstanceMessage(LM->getText("beta alert")); // test
    std::string id = UDGetStr(KEY_SAVED_ID,"-1");
    BSM->requestedID = id;
    if(id.compare("-1") == 0){
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        hideIndicator();
        //            showUserSelect();
        closePopup();
        showRegisterName();
    }else{
        GM->titleLayer = nullptr;
        GM->nextScene = STAGE_LOBBY;
        GM->isColosseum = false;
        auto scene = HelloWorld::scene(STAGE_LOBBY, false);
        Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
    }
}
void Title::showExitPopup(){
    Node* layer = CSLoader::createNode("ExitPopup.csb");
    this->addChild(layer, 4);
    layer->setName("exitPopup");
    setAsPopup(layer);
    
    float moveY = 100;
    Button* btn = (Button*)layer->getChildByName("btnNo");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn->setOpacity(0);
    btn->setPositionY(btn->getPositionY() - moveY);
    btn->runAction(Sequence::create(DelayTime::create(0.5f), MoveBy::create(0.5f, Vec2(0, moveY)), NULL));
    btn->runAction(Sequence::create(DelayTime::create(0.5f), FadeIn::create(0.4f), NULL));
    
    btn = (Button*)layer->getChildByName("btnYes");
    btn->addClickEventListener(CC_CALLBACK_0(GameManager::exitGame, GM));
    btn->setOpacity(0);
    btn->setPositionY(btn->getPositionY() - moveY);
    btn->runAction(Sequence::create(DelayTime::create(1.2f), MoveBy::create(0.5f, Vec2(0, moveY)), NULL));
    btn->runAction(Sequence::create(DelayTime::create(1.2f), FadeIn::create(0.4f), NULL));

    EnemyBase* unit = EnemyBase::createEnemy(UNIT_TROLL, 0, 0, "trollStand0.png");
    layer->addChild(unit);
    unit->setPosition(Point(size.width/2, size.height/2 + 0));
    unit->runAnimation("trollAttack", true, 0.3f);
}
void Title::onMessageBoxReceived(){
    std::string msgBox = strMessageBox;
    log("onMessageBoxReceived: %s", msgBox.c_str());
    ValueVector msgs = GM->split(msgBox, ",");
    int usedIndex = -1;
    int rank = -1;
    int total = -1;
    for(int i = 0; i < msgs.size(); i++){
        if (msgs.at(i).asString().find("lr") != std::string::npos) {
            std::string subStr = msgs.at(i).asString().substr(2);
            ValueVector leagueInfo = GM->split(subStr, "of");
            if(leagueInfo.size() == 2){
                rank = leagueInfo.at(0).asInt();
                total = leagueInfo.at(1).asInt();
                log("league result reward %d of %d", rank, total);
                usedIndex = i;
            }
        }
    }
    std::string msgBoxLeft = "";
    for(int i = 0; i < msgs.size(); i++){
        if(i != usedIndex){
            msgBoxLeft += msgs.at(i).asString() + ",";
        }
    }
    msgBoxLeft = msgBoxLeft.substr(0, msgBoxLeft.size() - 1);
    if(usedIndex >= 0){
//        showLeagueResult(rank, total);
    }
//    BSM->updateMessageBox(getPlayerID(), msgBoxLeft);
    UDSetStr(KEY_MESSAGE_BOX, msgBoxLeft);
}
void Title::onUserInfoFailed(){
    onGetTimeFailed();
}
void Title::onArenaClick(){
//    GameSharing::buyItem("goldenticket1"); // test
//    return;
//    iapFlag = IAP_FLAG_TICKET_1;// test
//    bool isConnected = false;
//    if(!GameSharing::bIsGPGAvailable || GameSharing::getPlayerID().compare("notset") == 0){
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//        showInstanceMessage(LM->getText("sign in game center"));
//#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//        showInstanceMessage(LM->getText("sign in google play"));
//#endif
//        return; // test
//    }
    
    std::string id = UDGetStr(KEY_SAVED_ID,"-1");
    BSM->requestedID = id;
    if(id.compare("-1") == 0){
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        hideIndicator();
        //            showUserSelect();
        closePopup();
        showRegisterName();
    }else{
        showColosseum();
    }
    
}
void Title::showIndicator(){
    Button* btn = Button::create("uiBoxSmall.png");
    this->addChild(btn, 1000);
    btn->setScale(100);
    btn->setOpacity(150);
    btn->setColor(Color3B::BLACK);
    btn->setPosition(size/2);
    btn->setName("btnIndicator");
    
    Label* lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
    this->addChild(lbl, 1001);
    lbl->setPosition(size/2);
    lbl->setName("lblIndicator");
}
void Title::hideIndicator(){
    this->removeChildByName("btnIndicator");
    this->removeChildByName("lblIndicator");
}
void Title::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
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
            showExitPopup();
            if(GM->isAdsUser()){
                GameSharing::showBanner();
            }
        }
    }
}
void Title::onExitClick(){
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
}
void Title::onClickFull(){
    log("here!!");
    fullPackagePopup = Node::create();
    this->addChild(fullPackagePopup);
    
    Button* btnBack = Button::create("miniRect.png");
    fullPackagePopup->addChild(btnBack);
    btnBack->setColor(Color3B::BLACK);
    btnBack->setScale(20);
    btnBack->setOpacity(100);
    btnBack->setPosition(size/2);
    
    Sprite* spt = Sprite::create("fullPack.png");
    fullPackagePopup->addChild(spt);
    spt->setPosition(Point(size.width/2, size.height/2 + 160));
    
    Sprite* sptLight = Sprite::create("longLight.png");
    fullPackagePopup->addChild(sptLight);
    sptLight->setPosition(Point(size.width/2, size.height/2 + 310));
    sptLight->setScale(0.6f);
    
    PPLabel* lbl = PPLabel::create(LM->getText("getFullVersion"), 100, Color3B::WHITE, true, false, TextHAlignment::CENTER, true);
    fullPackagePopup->addChild(lbl);
    lbl->setPosition(Point(size.width/2, size.height/2 + 310));
    
    Button* btnFull = Button::create("btnBox.png");
    btnFull->setPosition(Point(size.width/2, size.height/2 - 320));
    fullPackagePopup->addChild(btnFull);
    btnFull->addClickEventListener(CC_CALLBACK_0(Title::buyFullPackage, this));
    
    Label* lblPrice = Label::createWithSystemFont("₩2,000", "Thonburi", 80);
    btnFull->addChild(lblPrice);
    lblPrice->setTextColor(Color4B(DARK_GRAY_3B, 255));
    lblPrice->setPosition(btnFull->getContentSize().width/2, btnFull->getContentSize().height/2 + 10);
}
void Title::buyFullPackage(){
    indicator = Node::create();
    this->addChild(indicator, 100);
    
    Button* btnBack = Button::create("miniRect.png");
    indicator->addChild(btnBack);
    btnBack->setColor(Color3B::BLACK);
    btnBack->setScale(20);
    btnBack->setOpacity(100);
    btnBack->setPosition(size/2);
    
    
    PPLabel* lbl = PPLabel::create("Wait...", 50, Color3B::WHITE, false, false, TextHAlignment::CENTER, false);
    indicator->addChild(lbl);
    lbl->setPosition(size/2);
    
//    NativeInterface::NativeInterface::purchaseOnestore("propack");
}
void Title::showChapterSelect(){
    // bug fix
    std::string key = "is_chapter_first_open";
    if(UDGetBool(key.c_str(), true)){
        UDSetBool(key.c_str(), false);
        int stageClearIndex = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
        if(stageClearIndex == 10){
            UDSetInt(KEY_LAST_CLEAR_STAGE, 11);
        }
    }
    
    Node* layer = CSLoader::createNode("ChapterSelect.csb");
    this->addChild(layer, 4);
    layer->setName("chapterSelect");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btnBlock = (Button*)layer->getChildByName("btnBlock");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    Text* lbl = (Text*)layer->getChildByName("imgTitle")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "chapter");
    doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->setClippingEnabled(true);
    sv->setInnerContainerSize(sv->getContentSize());
    for (int i = 0; i < 2; i++) {
        btn = (Button*)sv->getChildByName(strmake("btnChapter%d", i));
        lbl = (Text*)btn->getChildByName("lblTitle");
        Node* img = btn->getChildByName("img");
        if(i == 0){
            LM->setLocalizedString(lbl, "human");
            btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
            doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);
        }else if(i == 1){
//            spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
//            btn->addChild(spChar);
//            spChar->setAnimation(0, "idle", true);
//            spChar->setPosition(img->getPosition() + Vec2(-150, 0));
//            img->removeFromParent();
//            Sprite* sptShadow = Sprite::create("shadow.png");
//            spChar->addChild(sptShadow, -1);
//            spChar->setScaleX(-1);
//
//            spine::SkeletonAnimation* spWolf = spine::SkeletonAnimation::createWithJsonFile("werewolf.json", "werewolf.atlas", 1);
//            btn->addChild(spWolf);
//            spWolf->setAnimation(0, "idle", true);
//            spWolf->setPosition(spChar->getPosition() + Vec2(300, 0));
//            sptShadow = Sprite::create("shadow.png");
//            spWolf->addChild(sptShadow, -1);
//            spWolf->setScale(1.3f);
            int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
            if (UDGetBool(KEY_CHAPTER_2_PURCHASED) || lastClearStage >= 10) {
                LM->setLocalizedString(lbl, "orc");
                btn->getChildByName("lblOr")->setVisible(false);
                btn->getChildByName("imgConditionBack")->setVisible(false);
                btn->getChildByName("lblCondition")->setVisible(false);
                btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
            }else{
                lbl = (Text*)btn->getChildByName("lblOr");
                LM->setLocalizedString(lbl, "or");
                lbl = (Text*)btn->getChildByName("lblCondition");
                LM->setLocalizedString(lbl, "chapter2 unlock condition");
                btn->addClickEventListener(CC_CALLBACK_1(Title::onPurchaseChapterClick, this));
                lbl = (Text*)btn->getChildByName("lblTitle");
                LM->setLocalizedString(lbl, "or");
                lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_CHAPTER2));
            }
        }else if(i == 2){
            LM->setLocalizedString(lbl, "zombie");
        }else if(i == 3){
            LM->setLocalizedString(lbl, "werewolf");
            
            spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("werewolf.json", "werewolf.atlas", 1);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(img->getPosition());
            img->removeFromParent();
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
        }
//        if(i > 0){
//            lbl = (Text*)btn->getChildByName("lblComingSoon");
//            LM->setLocalizedString(lbl, "coming soon");
//            doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);
//        }
    }
    
    rollOpenScroll((ImageView*)layer->getChildByName("imgBackground"));
}
void Title::onPurchaseChapterClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(btn->getTag() == 1){
        GameSharing::buyItem(IAP_DETAIL_CHAPTER2);
    }
}
void Title::onChapterClick(Ref* ref){
    BTN_FROM_REF
    selectedChapter = btn->getTag();
    if(btn->getTag() == 0){
        showStageSelect(btn->getTag());
    }else if(btn->getTag() == 1){
        showStageSelect(btn->getTag());
//        showInstanceMessage(LM->getText("coming soon")); // test
    }
}
void Title::showStageSelect(int chapter){
    showInterstitialRequested = true;
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNode("StageSelect.csb");
    this->addChild(layer, 4);
    layer->setName("stageSelect");
    setAsPopup(layer);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btnBlock = (Button*)layer->getChildByName("btnBlock");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnLoad");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onLoadClick, this));
    addLabelToButton(btn, LM->getText("load"));
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->setClippingEnabled(true);
    rollOpenScroll((ImageView*)layer->getChildByName("imgBackground"));
    
    float itemHeight = 234;
    int itemCount = 12;
    sv->setInnerContainerSize(Size(sv->getContentSize().width, itemHeight*itemCount));
    for (int i = 0; i < itemCount; i++) {
        bool isHidden = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1 - chapter*12 < i; // test
        if(i == 0){
            isHidden = false;
        }
        btn = Button::create(isHidden?"btnHorizontalScrollItemHidden.png":"btnHorizontalScrollItem.png");
        sv->addChild(btn);
        btn->setScale9Enabled(true);
        btn->setContentSize(Size(1300-30*2, itemHeight-10));
        btn->setPosition(Point(30, (itemCount - (i))*itemHeight));
        btn->setAnchorPoint(Point(0, 1));
        btn->addClickEventListener(CC_CALLBACK_1(Title::onStageClick, this));
        btn->setPressedActionEnabled(false);
        btn->setZoomScale(0.01f);
        btn->setTag(i+chapter*itemCount);
        btn->setOpacity(0);
        btn->runAction(Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));
        
        std::string title = LM->getText(strmake("stage title %d_%d", chapter, i).c_str());
        if (isHidden) { // test
            title = "???";
            btn->setTouchEnabled(false); // test
        }

        PPLabel* lbl = PPLabel::create(title, 50, isHidden?Color3B(74, 72, 68):Color3B(29, 28, 26), true, false, TextHAlignment::LEFT, false);
        btn->addChild(lbl);
        lbl->setPosition(30, btn->getContentSize().height/2);
        doLabelFadeInLater(lbl, 0.3f, 0.5f);
    }
    
}

void Title::onOkFromLoadData(){
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
    
    this->runAction(Sequence::create(DelayTime::create(0.07f), CallFunc::create(CC_CALLBACK_0(Title::goToLoadedStage, this)), NULL));
}
void Title::goToLoadedStage(){
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
    clearAssets();
    GameManager::getInstance()->titleLayer = nullptr;
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    GM->nextScene = STAGE_FIELD;
    GM->isColosseum = false;
    auto scene = HelloWorld::scene(savedStage, false);
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::onLoadClick(){
    showInterstitialRequested = true;
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Point(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
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
    lblLoad->setPosition(Point(size.width/2, size.height/2 + 360));
    for (int i = 0; i < 4; i++) {
        btn = Button::create("uiBoxSmall.png");
        btn->setScale9Enabled(true);
        layer->addChild(btn);
        btn->setContentSize(Size(500, 200));
        btn->addClickEventListener(CC_CALLBACK_1(Title::onLoadSlotClick, this));
        btn->setTag(i);
        //        ValueVector datas = GameManager::getInstance()->split(UDGetStr(strmake("savedData%d", i).c_str()), ",");
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
            btn->setPosition(Point(size.width/2 - 300, firstLineY));
        }else if(i == 1){
            btn->setPosition(Point(size.width/2 + 300, firstLineY));
        }else if(i == 2){
            btn->setPosition(Point(size.width/2 - 300, firstLineY - gapY));
        }else if(i == 3){
            btn->setPosition(Point(size.width/2 + 300, firstLineY - gapY));
        }
        lblTitle->setPosition(btn->getPosition() + Point(0, 35));
        lblTime->setPosition(btn->getPosition() + Point(0, -35));
        if(savedTime == 0){
            lblTime->setVisible(false);
        }
    }
}
void Title::onLoadSlotClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    selectedSaveSlot = btn->getTag();
    
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Point(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lbl = PPLabel::create(LM->getText("load data"), 50, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lbl);
    lbl->setPosition(Point(size.width/2, size.height/2 + 300));
    lbl->setWidth(1000);
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Point(size.width/2 - 300, size.height/2 - 200));
    btn->setContentSize(Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onOkFromLoadData, this));
    
    lbl = PPLabel::create(LM->getText("ok"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("effect");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Point(size.width/2 + 300, size.height/2 - 200));
    btn->setContentSize(Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    lbl = PPLabel::create(LM->getText("cancel"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("music");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
}
void Title::onOptionClick(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    this->addChild(layer);
    setAsPopup(layer);
    
    Button* btnBlock = Button::create("uiBox.png");
    btnBlock->setPosition(Point(size.width/2, size.height/2));
    layer->addChild(btnBlock);
    btnBlock->setScale(20);
    btnBlock->setOpacity(100);
    btnBlock->setColor(Color3B::BLACK);
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setPosition(size/2);
    storyBack->setTouchEnabled(true);
    
    PPLabel* lbl = PPLabel::create(LM->getText("option"), 100, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    layer->addChild(lbl);
    lbl->setPosition(Point(size.width/2, size.height/2 + 300));
    
    Button* btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Point(size.width/2 - 300, size.height/2 - 100));
    btn->setContentSize(Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onEffectClick, this));
    
    lbl = PPLabel::create(LM->getText(GM->getSoundVolumn()==0?"effect off":"effect on"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("effect");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
    
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setPosition(Point(size.width/2 + 300, size.height/2 - 100));
    btn->setContentSize(Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onMusicClick, this));
    
    lbl = PPLabel::create(LM->getText(GM->getMusicVolumn()==0?"music off":"music on"), 60, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
    lbl->setName("music");
    layer->addChild(lbl);
    lbl->setPosition(btn->getPosition());
    
}
void Title::onEffectClick(){
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
void Title::onMusicClick(){
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

void Title::onUpgradeClick(){
    
}
void Title::onShopClick(){
    
}
void Title::showInstanceMessage(std::string msg, int offset){
    Label* lbl = LM->getLocalizedLabel(msg.c_str(), Color4B(255, 255, 255, 255), 50);//Label::createWithSystemFont(msg, "Thonburi", 50);
    this->addChild(lbl, 300);
    lbl->setName("lblInstanceMessage");
    lbl->enableShadow();
    lbl->setTag(77);
    lbl->setPosition(Point(size.width/2, -100 + offset));
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(EaseInOut::create(MoveBy::create(dur, Point(0, distanceToMove)), 3), DelayTime::create(2), EaseInOut::create(MoveBy::create(dur, Point(0, -distanceToMove)), 3), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, lbl)), nullptr));
}


void Title::onStageClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
//    if(btn->getTag() >= 5){ // test 
//        showInstanceMessage("Wait for the update!");
//        return;
//    }
    btn->setEnabled(false);
    if(btn->getTag() == 0){
        clearAssets();
        GameManager::getInstance()->titleLayer = nullptr;
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
        GM->nextScene = STAGE_INTRO;
        GM->isColosseum = false;
        auto scene = HelloWorld::scene(btn->getTag(), false);
        Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
        return;
    }
    showStageReady(btn->getTag());
}
void Title::showStageReady(int stage){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Layer* layer = Layer::create();
    setAsPopup(layer);
    this->addChild(layer);
    float mapZoomDelay = 2.0f;
    float showUIDelay = 3.5f;
    Button* btnBlockBack = Button::create("worldMap.png");
    btnBlockBack->setPosition(Point(size.width/2, size.height/2));
    layer->addChild(btnBlockBack);
    btnBlockBack->setColor(Color3B::BLACK);
    btnBlockBack->setScale(10*2224.0f/2048);
    Button* btnBlock = Button::create("worldMap.png");
    btnBlock->setPosition(Point(size.width/2, size.height/2 + 100));
    layer->addChild(btnBlock);
    btnBlock->setScale(1.2*2224.0f/2048);
    btnBlock->setZoomScale(0);
    btnBlock->setOpacity(0);
    btnBlock->runAction(FadeIn::create(0.5f));
    btnBlock->runAction(Sequence::create(DelayTime::create(mapZoomDelay/2), EaseInOut::create(ScaleTo::create(1, 1.9f*2224.0f/2048), 2), nullptr));
    if(stage >= 3){
        btnBlock->runAction(Sequence::create(DelayTime::create(mapZoomDelay/2), EaseInOut::create(MoveTo::create(1, Point(size.width/2 - 940, size.height/2 - 500)), 2), nullptr));
    }else{
        btnBlock->runAction(Sequence::create(DelayTime::create(mapZoomDelay/2), EaseInOut::create(MoveTo::create(1, Point(size.width/2 + 940, size.height/2 - 500)), 2), nullptr));
    }
    
    if(stage >= 0){
        btnBlock->runAction(Sequence::create(DelayTime::create(mapZoomDelay), EaseInOut::create(MoveTo::create(1, Point(size.width/2 - 200, size.height/2 + 400)), 2), nullptr));
    }
    btnBlock->setPressedActionEnabled(false);
    
    Button* btnBlackBack = Button::create("uiBox.png");
    layer->addChild(btnBlackBack);
    btnBlackBack->setPosition(size/2);
    btnBlackBack->setScale(20);
//    btn->addClickEventListener(CC_CALLBACK_1(Title::goToStage, this));
    btnBlackBack->setOpacity(0);
    btnBlackBack->runAction(Sequence::create(DelayTime::create(showUIDelay), FadeTo::create(2, 100), NULL));
    btnBlackBack->setColor(Color3B::BLACK);
    
    Button* btn = Button::create("uiBox.png");
    layer->addChild(btn);
    btn->setScale9Enabled(true);
    btn->setContentSize(Size(550, 234));
    btn->setPosition(Point(size.width - 400, 180));
    btn->setTag(stage);
    btn->addClickEventListener(CC_CALLBACK_1(Title::goToStage, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(showUIDelay), FadeIn::create(2), NULL));
    PPLabel* lbl = addLabelToButton(btn, "play", 60, Color3B(142, 45, 29));
    doLabelFadeInLater(lbl, showUIDelay, 2);
    
    ImageView* objectBack = ImageView::create("missionBoard.png");
    layer->addChild(objectBack);
    objectBack->setScale9Enabled(true);
    objectBack->setContentSize(Size(700, 600));
    objectBack->setPosition(Point(size.width - 100, size.height - 100));
    objectBack->setAnchorPoint(Point(1, 1));
    objectBack->setOpacity(0);
    objectBack->runAction(Sequence::create(DelayTime::create(showUIDelay), FadeTo::create(2, 255), NULL));
    
    int y = size.height - 260;
    int x = size.width - 740;
//    lbl = PPLabel::create("MISSION" , 50, DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
//    layer->addChild(lbl);
//    lbl->setPosition(Point(x + 280, y + 40));
    doLabelFadeInLater(lbl, showUIDelay, 2);
    if (GM->getStageObjective(stage) == CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND) {
        for (int i = 0; i < 3; i++) {
            if(i == 0){
                lbl = PPLabel::create(strmake(LM->getText("objective build farm").c_str(), 2), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
            }else if(i == 1){
                lbl = PPLabel::create(strmake(LM->getText("objective build barracks").c_str(), 1), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
            }else if(i == 2){
                lbl = PPLabel::create(strmake(LM->getText("objective train swordsman").c_str(), 4), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
            }
            
            layer->addChild(lbl);
            lbl->setPosition(Point(x, y - i*100 - 100));
            doLabelFadeInLater(lbl, showUIDelay, 2);
        }
    }else if (GM->getStageObjective(stage) == CLEAR_CONDITION_TERMINATE) {
        lbl = PPLabel::create(LM->getText("objective destroy them all").c_str(), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
        layer->addChild(lbl);
        lbl->setPosition(Point(x, y - 100));
        lbl->setAnchorPoint(Point(0, 0.5));
        doLabelFadeInLater(lbl, showUIDelay, 2);
    }else if (GM->getStageObjective(stage) == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
        for (int i = 0; i < 2; i++) {
            if(i == 0){
                lbl = PPLabel::create(strmake(LM->getText("objective protect all").c_str(), 2), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
            }else if(i == 1){
                lbl = PPLabel::create(strmake(LM->getText("objective destroy them all").c_str(), 1), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
            }
            
            layer->addChild(lbl);
            lbl->setPosition(Point(x, y - i*100 - 100));
            doLabelFadeInLater(lbl, showUIDelay, 2);
        }
    }else if (GM->getStageObjective(stage) == CLEAR_CONDITION_SURVIVE_FOR_10_MIN || GM->getStageObjective(stage) == CLEAR_CONDITION_SURVIVE_FOR_20_MIN) {
        int min = 10;
        if(GM->getStageObjective(stage) == CLEAR_CONDITION_SURVIVE_FOR_20_MIN){
            min = 20;
        }
        lbl = PPLabel::create(strmake(LM->getText("objective survive").c_str(), min), 50, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
        layer->addChild(lbl);
        lbl->setPosition(Point(x, y - 100));
        lbl->setAnchorPoint(Point(0, 0.5));
        lbl->setWidth(600);
        doLabelFadeInLater(lbl, showUIDelay, 2);
    }
    
    
    ImageView* storyBack = ImageView::create("verticalScrollBackground.png");
    layer->addChild(storyBack);
    storyBack->setScale9Enabled(true);
    storyBack->setContentSize(Size(1400, 1030));
    storyBack->setPosition(Point(100, size.height - 100));
    storyBack->setAnchorPoint(Point(0, 1));
    storyBack->setOpacity(0);
    storyBack->runAction(Sequence::create(DelayTime::create(showUIDelay), FadeTo::create(2, 255), NULL));
    
    ScrollView* sv = ScrollView::create();
    sv->setContentSize(Size(1350, 855));
    sv->setPosition(Point(150, size.height - 190));
    sv->setAnchorPoint(Point(0, 1));
    sv->setScrollBarEnabled(false);
    layer->addChild(sv);
    
//    lbl = LM->getLocalizedLabel(LM->getText(strmake("stage title story %d", stage).c_str()).c_str(), Color4B::BLACK, 70);
    if(selectedChapter < 0){
        selectedChapter = 0;
    }
    PPLabel* ppLabel = PPLabel::create(LM->getText(strmake("stage title story %d_%d", selectedChapter, stage%12).c_str()).c_str(), 70, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
    sv->addChild(ppLabel);
    ppLabel->setWidth(1200);
//    lbl->setDimensions(1300, 1000);
    ppLabel->setAnchorPoint(Point(0, 1));
    ppLabel->setPosition(Point(50, -50));
    float textHeight = ppLabel->getSize().height;
    ppLabel->runAction(Sequence::create(DelayTime::create(mapZoomDelay), MoveBy::create(40 + (textHeight - 1500)/100, Point(0, textHeight + 50 + sv->getContentSize().height)), nullptr));
//    ppLabel->disableEffect(LabelEffect::SHADOW);
    doLabelFadeInLater(ppLabel, showUIDelay, 2);
}
PPLabel* Title::addLabelToButton(Button* btn, std::string text, int fontSize, Color3B color, bool isBordered){
//    Label* lbl = LM->getLocalizedLabel(text.c_str(), Color4B::BLACK, fontSize);
//    btn->addChild(lbl);
//    lbl->setPosition(btn->getContentSize()/2);
//    lbl->setName("lbl");
//    lbl->disableEffect(LabelEffect::SHADOW);
//    return lbl;
    PPLabel* lbl = PPLabel::create(LM->getText(text.c_str()), btn->getContentSize().height*2/6, color, true, isBordered, TextHAlignment::CENTER, false);
    lbl->setPosition(Point(btn->getContentSize().width/2, btn->getContentSize().height/2));
    btn->addChild(lbl);
    return lbl;
}
PPLabel* Title::replaceTextToPPLabel(Text* lblText){
    PPLabel* lbl = PPLabel::create(lblText->getString(), lblText->getFontSize(), Color3B(lblText->getTextColor()), true, lblText->getOutlineSize(), TextHAlignment::CENTER, false);
    lbl->setPosition(lblText->getPosition());
    lblText->getParent()->addChild(lbl);
    lblText->removeFromParent();
    return lbl;
}
void Title::addLabelToButtonFromHud(std::string text, Button* btn, bool bordered, Color3B color, bool isSystemLabel){
    PPLabel* lbl = PPLabel::create(LM->getText(text.c_str()), btn->getContentSize().height*2/6, color, isSystemLabel, bordered, TextHAlignment::CENTER, false);
    lbl->setPosition(Point(btn->getContentSize().width/2, btn->getContentSize().height/2));
    btn->addChild(lbl);
}
void Title::goToStage(Ref* ref){
    clearAssets();
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    BTN_FROM_REF_AND_DISABLE
    btn->setEnabled(false);
    GM->isColosseum = false;
    GameManager::getInstance()->titleLayer = nullptr;
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    GM->nextScene = STAGE_FIELD;
    auto scene = HelloWorld::scene(btn->getTag(), false);
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::titleUpdate(float dt){
    if(isTitleEnd){
        return;
    }
//    if (GM->iapStateFlag == IAP_STATE_NOT_INIT) {
//        lblLoading->setString("Loading products...");
//        return;
//    }else if (GM->iapStateFlag == IAP_STATE_INIT_SUCCESS) {
//        lblLoading->setString("Setting products...");
//        GM->priceList.push_back(GameSharing::getIAPPrice("starter0"));// IAP_STARTER_0
//        GM->priceList.push_back(GameSharing::getIAPPrice("starter1"));// IAP_STARTER_1
//        GM->priceList.push_back(GameSharing::getIAPPrice("starter2"));// IAP_STARTER_2
//        GM->priceList.push_back(GameSharing::getIAPPrice("package0"));// IAP_PACKAGE_0
//        GM->priceList.push_back(GameSharing::getIAPPrice("package1"));// IAP_PACKAGE_1
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem0"));// IAP_GEM_0
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem1"));// IAP_GEM_1
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem2"));// IAP_GEM_2
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem3"));// IAP_GEM_3
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem4"));// IAP_GEM_4
//        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0"));// IAP_TICKET_0
//        GM->priceList.push_back(GameSharing::getIAPPrice("ticket1"));// IAP_TICKET_1
//        GM->priceList.push_back(GameSharing::getIAPPrice("ticket2"));// IAP_TICKET_2
//        GM->priceList.push_back(GameSharing::getIAPPrice("starter1original"));// IAP_STARTER_1_ORIGINAL 12
//        GM->priceList.push_back(GameSharing::getIAPPrice("starter2original"));// IAP_STARTER_2_ORIGINAL 13
//        GM->priceList.push_back(GameSharing::getIAPPrice("package0original"));// IAP_PACKAGE_0_ORIGINAL 14
//        GM->priceList.push_back(GameSharing::getIAPPrice("package1original"));// IAP_PACKAGE_1_ORIGINAL 15
//        GM->priceList.push_back(GameSharing::getIAPPrice("gem0original"));// IAP_GEM_0_ORIGINAL 16
//        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0original"));// IAP_TICKET_0_ORIGINAL 17
    
//        GM->iapStateFlag = IAP_STATE_INIT_DONE;
//        return;
//    }
    if(isUserIDExistCheckArrived){
        isUserIDExistCheckArrived = false;
        UDSetBool(KEY_ID_EXIST_CHECK_DONE, true);
        if (GM->playerIDWithGPSID.size() == 0) {
            if(UDGetStr(KEY_SAVED_ID, "-1").compare("-1") != 0){
                BSM->saveUserData(strmake("playid=%s", GM->playerGPSID.c_str()));
                settingLog("Restore code: 1");
            }else{
                settingLog("Restore code: 2");
            }
        }else{
            if (UDGetStr(KEY_SAVED_ID, "-1").compare(GM->playerIDWithGPSID) != 0) {
                UDSetStr(KEY_SAVED_ID, GM->playerIDWithGPSID);
                this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(Title::restartTheGame, this)), NULL));
                settingLog("Restore code: 3");
                return;
            }else{
                settingLog("Restore code: 4");
            }
        }
    }
    
    if (GM->playerGPSID.size() > 0 && !UDGetBool(KEY_ID_EXIST_CHECK_DONE, false) && !isUserIDExistCheckRequested) {
        checkPlayerIDExist(); // test
    }
    if(BSM->isServerFailed){
        BSM->isServerFailed = false;
        if(!this->getChildByName("lblInstanceMessage")){
            showInstanceMessage(LM->getText("network fail play offline"));
        }
        lblID->setString(strmake("ErrorCode: %d|%s", BSM->errorCode, BSM->errorDetail.c_str()));
        hideIndicator();
        return;
    }
    if(isRestoreRequested && GM->iapFlag == IAP_FLAG_RESTORED){
        isRestoreRequested = false;
        GM->iapFlag = IAP_FLAG_INIT;
        showInstanceMessage(LM->getText("restore complete"));
    }
    if(nameHandleState == NETWORK_HANDLE_STATE_NOT_READY){
        lblLoading->setString("Checking id...");
        std::string id = UDGetStr(KEY_SAVED_ID,"-1");
        BSM->requestedID = id;
        if(id.compare("-1") == 0){
            nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
            hideIndicator();
//            showUserSelect();
//            closePopup();
//            showRegisterName();
        }else{
            nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
            lblID->setString(strmake("ID: %s", id.c_str()));
            lblLoading->setString(strmake(LM->getText("welcome remember password").c_str(), UDGetStr(KEY_NAME).c_str()));
            log("name: %s", UDGetStr(KEY_NAME).c_str());
        }
    }
    if(nameHandleState == NETWORK_HANDLE_STATE_ARRIVED){
        hideIndicator();
        if(isSendingPassword){
            nameHandleState = NETWORK_HANDLE_STATE_HANDLED;
        }else{
            
            if(BSM->resultMessage.compare("same name exist") == 0){
                showInstanceMessage(LM->getText("same name exist"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else if(BSM->resultMessage.compare("save failed") == 0){
                showInstanceMessage(LM->getText("network retry"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else if(BSM->resultMessage.compare("database failure") == 0){
                showInstanceMessage(LM->getText("network retry"));
                nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
            }else{
                isSendingPassword = true;
                nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
                showIndicator();
                TextField* tfPassword = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfPassword");
                isNameRequested = true;
                BSM->saveUserData(strmake("password=%s", tfPassword->getString().c_str()));
                UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test
            }
        }
    }
    
    if (nameHandleState == NETWORK_HANDLE_STATE_HANDLED) {
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        
        std::string id = UDGetStr(KEY_SAVED_ID,"-1");
        lblID->setString(strmake("ID: %s", id.c_str()));
        
        LM->setLocalizedString(lblLoading, "touch to start");
        lblLoading->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f), DelayTime::create(0.5f), NULL)));
        if(nameToRegister.length() > 0){
            UDSetStr(KEY_NAME, nameToRegister);
        }
        lblLoading->setString(strmake(LM->getText("welcome remember password").c_str(), UDGetStr(KEY_NAME).c_str()));
        log("name: %s", UDGetStr(KEY_NAME).c_str());
//        lblLoading->setVisible(true);
        closePopup();
        closePopup();
    }
    
    if(isFreeAppResultArrived){
        isFreeAppResultArrived = false;
        hideIndicator();
        if(isFreeApp){
            UDSetBool(KEY_IS_FREE_USER, true);
            UD->flush();
            showFreeAppAlert();
        }
    }
    
    if(isVideoDone){
        isVideoDone = false;
        
        showInstanceMessage(LM->getText("ticket received"));
    }
    
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->iapFlag = IAP_FLAG_INIT;
        log("iap success!");
        if(GM->iapDetail.compare(IAP_DETAIL_TICKET_1) == 0){
            int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
            ticket += 12;
            UDSetInt(KEY_GOLDEN_TICKET, ticket);
            isColosseumInfoRefreshRequired = true;
            hideIndicator();
            showInstanceMessage(LM->getText("purchase success"));
        }else if(GM->iapDetail.compare(IAP_DETAIL_CHAPTER2) == 0){
            UDSetBool(KEY_CHAPTER_2_PURCHASED, true);
            closePopup();
            showChapterSelect();
        }
    }else if(GM->iapFlag == IAP_FLAG_FAILED){
        GM->iapFlag = IAP_FLAG_INIT;
        log("iap failed!");
        hideIndicator();
    }
    
    if(strMessageBox.size() > 0){
        onMessageBoxReceived();
        strMessageBox = "";
    }
    
    // test
//    if(true){
//    if(GameSharing::IsGPGAvailable() && GameSharing::getPlayerID().compare("notset") != 0){
//        if(isGameCenterLoginRequestedFromShowColosseum){
//            isGameCenterLoginRequestedFromShowColosseum = false;
//            showColosseum();
//        }
//
//        if(!isUserInfoCreatedOrNotKnow){
//            if(!isWaitingForUserInfoCreated){
//                log("set user info starts");
//                isWaitingForUserInfoCreated = true;
//
//                std::string requestData =
//                "id=" + GameSharing::getPlayerID() + "&" +
//                "level=" + Value(0).asString() + "&" +
//                "exp=" + Value(0).asString() + "&" +
//                "messageBox=" + "e" + "&" +
//                "inventory=" + "e";
//                log("set user info about to");
//
//                BSM->setUserInfo(requestData);
//                log("set user info ends");
//            }
//        }else if(!isUserInfoReceived){
//            if(!isWaitingForUserInfo){
//                isWaitingForUserInfo = true;
//                log("get user info starts");
//                BSM->getUserInfo(GameSharing::getPlayerID());
//                log("get user info ends");
//            }
//        }
//    }
    
//    if(stageRequested >= 0){
//        Button* btn = Button::create();
//        btn->setTag(stageRequested);
//        onStageClick(btn);
//        stageRequested = -1;
//    }
}
void Title::settingLog(std::string str){
    Node* layer = this->getChildByName("settingPopup");
    if (layer) {
        Text* lbl = (Text*)layer->getChildByName("lblLog");
        lbl->setString(str);
    }
}
void Title::setTitle(Node* titleBack, std::string menuKey){
    Text* lbl = (Text*)titleBack->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, menuKey);
    ImageView* img = (ImageView*)titleBack;
    img->setContentSize(Size(lbl->getContentSize().width*lbl->getScaleX() + 100, 104.39f));
    img->setScale9Enabled(true);
    lbl->setPositionX(titleBack->getContentSize().width/2);
}
void Title::showRegisterName(){
    Node* layer = CSLoader::createNode("RegisterName.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("namePopup");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    setTitle(layer->getChildByName("titleBack"), "name");
    lbl = (Text*)background->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "input name");
    
    lbl = (Text*)layer->getChildByName("lblAlert");
    LM->setLocalizedString(lbl, "change name alert");
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    lbl = (Text*)background->getChildByName("lblPassword");
    LM->setLocalizedString(lbl, "password");
    lbl = (Text*)background->getChildByName("lblConfirm");
    LM->setLocalizedString(lbl, "confirm password");
    
    TextField* tf = (TextField*)background->getChildByName("tfName");
    tf->setPlaceHolder(LM->getText("name"));
    tf = (TextField*)background->getChildByName("tfPassword");
    tf->setPlaceHolder(LM->getText("password"));
    tf = (TextField*)background->getChildByName("tfConfirm");
    tf->setPlaceHolder(LM->getText("password"));
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onOkNameClick, this));
}
void Title::onOkNameClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    TextField* tf = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfName");
    TextField* tfPassword = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfPassword");
    TextField* tfConfirm = (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfConfirm");
    if(tf->getString().size() <= 0){
        showInstanceMessage(LM->getText("input name"));
    }
    else if(tf->getString().find(',') != std::string::npos ||
            tf->getString().find('_') != std::string::npos ||
            tf->getString().find('/') != std::string::npos){
        showInstanceMessage(strmake("%s ( _ , / )", LM->getText("do not use special character").c_str()));
        return;
    }
//    else if(tfPassword->getString().compare(tfConfirm->getString()) != 0){
//        showInstanceMessage(LM->getText("password not match"));
//    }
    else{
        nameToRegister = tf->getString();
        nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
        isNameRequested = true;
        showIndicator();
        BSM->registerName(tf->getString());
    }
}
void Title::showUserSelect(){
    Node* layer = CSLoader::createNode("UserSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("userSelect");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    
    setTitle(layer->getChildByName("titleBack"), "new user");
    
    lbl = (Text*)background->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, "is new user");
    
    btn = (Button*)background->getChildByName("btnNew");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onNewUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "new user");
    btn = (Button*)background->getChildByName("btnExist");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onOldUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "existing user");
}
void Title::onNewUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    showRegisterName();
}
void Title::onOldUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    showInputUserIDAndName();
}
void Title::showInputUserIDAndName(){
    Node* layer = CSLoader::createNode("ExistNameAndID.csb");
    this->addChild(layer, POPUP_ZORDER);
    setAsPopup(layer);
    layer->setName("accountChange");
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");
    layer->getChildByName("btnClose")->setVisible(false);
    setTitle(layer->getChildByName("titleBack"), "existing user");
    
    lbl = (Text*)background->getChildByName("lblName");
    LM->setLocalizedString(lbl, "name");
    
    lbl = (Text*)background->getChildByName("lblPassword");
    LM->setLocalizedString(lbl, "password");
    
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    tfName->setPlaceHolder(LM->getText("name"));
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    tfPassword->setPlaceHolder(LM->getText("password"));
    
    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onCheckExistingUser, this));
}
void Title::onCheckExistingUser(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    Node* layer = this->getChildByName("accountChange");
    TextField* tfName = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfName");
    TextField* tfPassword = (TextField*)layer->getChildByName("imgBackground")->getChildByName("tfPassword");
    BSM->changeAccount(tfName->getString(), tfPassword->getString());
}
void Title::restartTheGame(){
    clearAssets();
    GM->titleLayer = nullptr;
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    auto scene = Scene::create();
    Intro* intro = Intro::create();
    scene->addChild(intro);
    Director::getInstance()->replaceScene(scene);
}
void Title::showFreeAppAlert(){
    Node* layer = CSLoader::createNode("FreeAdsAlert.csb");
    this->addChild(layer, 4);
    layer->setName("freeAdsAlert");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btnBlock = (Button*)layer->getChildByName("btnBlock");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    rollOpenScroll((ImageView*)layer->getChildByName("imgBackground"));
    
    Text* lbl = (Text*)layer->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, "free app alert");
    
    lbl = (Text*)layer->getChildByName("lblAsk");
    lbl->setFontName(LM->getLocalizedFont());
    lbl->setString(strmake("%s: sachoi1103@gmail.com", LM->getText("ask email").c_str()).c_str());
}
void Title::onCreateUserFailed(){
    log("onCreateUserFailed");
}
void Title::onCreateUserCreated(){
    isUserInfoCreatedOrNotKnow = true;
    isWaitingForUserInfoCreated = false;
    isUserInfoReceived = false;
    isWaitingForUserInfo = false;
}
void Title::onUserInfoNotFound(){
    isUserInfoCreatedOrNotKnow = false;
    isWaitingForUserInfoCreated = false;
}
void Title::doLabelFadeInLater(PPLabel* lbl, float delay, float fadeInDur){
    if(lbl->lblNormal != nullptr){
        for(auto unit: lbl->getChildren()){
            unit->setOpacity(0);
            unit->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(fadeInDur), NULL));
        }
    }else{
        for(auto unit: lbl->spriteBatch->getChildren()){
            unit->setOpacity(0);
            unit->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(fadeInDur), NULL));
        }
    }
}
Node* Title::getPopup(){
    if(popupArray.size() > 0){
        return popupArray.at(popupArray.size() - 1);
    }
    return nullptr;
}

void Title::closePopup(){
    if(popupArray.size() > 0){
        Node* popup = popupArray.at(popupArray.size() - 1);
        popupArray.eraseObject(popup);
        popup->removeFromParent();
    }
    
    if(popupArray.size() == 0){
        if(showInterstitialRequested && GM->isAdsUser()){
            GameSharing::showInterstitial();
        }
        
        GameSharing::hideBanner();
    }
}

bool Title::setAsPopup(Node* node){
    popupArray.pushBack(node);
    node->setLocalZOrder(200 + (int)popupArray.size());
    return true;
}

void Title::showColosseum(){
    showInterstitialRequested = true;
    Node* layer = CSLoader::createNode("Colosseum.csb");
    this->addChild(layer, 4);
    layer->setName("colosseum");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btnBlock = (Button*)layer->getChildByName("btnBlock");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closeColosseumPopup, this));
    btnBlock = (Button*)layer->getChildByName("btnClose");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closeColosseumPopup, this));
    
    rollOpenScroll((ImageView*)layer->getChildByName("imgBackground"));
    
    layer->getChildByName("lblRecommend")->setVisible(false);
    
    Text* lbl = (Text*)layer->getChildByName("lblLeague");
    lbl->setString(strmake("%s #%d", GetCurrentLeagueText().c_str(), UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX)));
    
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "colosseum");
    
    lbl = (Text*)layer->getChildByName("lblReward");
    LM->setLocalizedString(lbl, "reward");
    
    lbl = (Text*)layer->getChildByName("lblFirstPlace");
//    lbl->setString(strmake("x20 %s", LM->getText("next league").c_str()));
    
    lbl = (Text*)layer->getChildByName("lblYourID");
    lbl->setString(strmake("Your ID: %s", UDGetStr(KEY_SAVED_ID).c_str()));//GameSharing::getPlayerID().c_str()));
    
    int timeInSec = (int)(BSM->getCurrentTime()/1000);
    int timeLeft = timeInSec%(60*60*24);
    lbl = (Text*)layer->getChildByName("lblLeagueTimeLeft");
    lbl->setString(GM->getTimeLeftInString(timeLeft));
    
    lbl = (Text*)layer->getChildByName("lblTicketCount");
    int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
    lbl->setString(strmake("%d", ticketCount));
    
    ScrollView* scView = (ScrollView*)layer->getChildByName("scView");
    scView->getChildByName("imgTemplate")->setVisible(false);
    
    Button* btn = (Button*)layer->getChildByName("btnPlay");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onPlayColosseum, this));
    lbl = (Text*)btn->getChildByName("lblPlay");
    LM->setLocalizedString(lbl, "play");
    
    btn->setPressedActionEnabled(true);
    layer->getChildByName("lblTimer")->setVisible(false);
    
    bool isFree = UDGetBool(KEY_GOLDEN_TICKET_FREE, true);
    int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
    if(freeTicketUsedDay != BSM->getDay()){
        isFree = true;
    }
    
    lbl = (Text*)btn->getChildByName("lblTicketCost");
    lbl->setString(isFree?"FREE":"1");
    
    btn = (Button*)layer->getChildByName("btnVideo");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onVideoForTicketClick, this));
    layer->getChildByName("lblTimerForVideo")->setVisible(false);
    
    showIndicator();
    isColosseumRankInfoReceived = false;
    
    isWaitingForRankInfo = false;
    isRankInfoReceviedSuccessfully = false;
    isTimeGetFailed = false;
    isLeagueNotFound = false;
    
    this->schedule(schedule_selector(Title::updateColosseumPopup));
}
void Title::onVideoForTicketClick(){
    log("on vido click");
    if(GameSharing::isVideoAvailable()){
        log("on vido click");
        GM->showVideo(VIDEO_TICKET);
        UDSetInt(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, BSM->getCurrentTime());
    }else{
        log("on vido click");
        showInstanceMessage(LM->getText("video not available"));
    }
}
void Title::closeColosseumPopup(){
    colosseumRankInfoName.clear();
    colosseumRankInfoResult.clear();
    colosseumRankInfoTime.clear();
    colosseumRankInfoKill.clear();
    colosseumRankInfoWeapon.clear();
    closePopup();
    this->unschedule(schedule_selector(Title::updateColosseumPopup));
}
std::string Title::getPlayerID(){
//    std::string playerId = UDGetStr(KEY_PLAYER_ID, "notset");
//
//    if(playerId.compare("notset") == 0){
//        playerId = getRandomName();
//    }
//
//    std::string playerIDFromGameCenter = GameSharing::getPlayerID();
//    if(playerIDFromGameCenter.compare("notset") != 0){
//        playerId = playerIDFromGameCenter;
//    }
//    return playerId;
    return GameSharing::getPlayerID();
}
std::string Title::getPlayerName(){
    std::string playerName = UDGetStr(KEY_PLAYER_NAME, "nameNotSet");
    
    if(playerName.compare("nameNotSet") == 0){
        playerName = getRandomName();
    }
    
    std::string playerNameFromGameCenter = GameSharing::getPlayerName();
    if(playerNameFromGameCenter.length() > 0){
        playerName = playerNameFromGameCenter;
    }
    return playerName;
}
void Title::onGetTimeFailed(){
    PPLabel* lbl = PPLabel::create(LM->getText("server fail try again"), 60, Color3B::WHITE, true, false, TextHAlignment::CENTER, true);
    this->addChild(lbl, 200);
    lbl->setPosition(Point(size.width/2, -TILE_SIZE/2));
    float dur = 0.3f;
    float distanceToMove = 360;
    lbl->runAction(Sequence::create(MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(2), MoveBy::create(dur, Point(0, -distanceToMove)), CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, lbl)), nullptr));
    
    hideIndicator();
    closePopup();
}
void Title::updateColosseumPopup(float dt){
    if(isTitleEnd){
        return;
    }
    if(!getChildByName("colosseum")){
        this->unschedule(schedule_selector(Title::updateColosseumPopup));
        return;
    }
    if(isTimeGetFailed){
        isTimeGetFailed = false;
        onGetTimeFailed();
    }
    if(isGetRankFailed){
        isGetRankFailed = false;
        onGetTimeFailed();
    }
    if(BSM->timeEstablished && !isWaitingForRankInfo && !isRankInfoReceviedSuccessfully){
        isGetRankFailed = false;
        isWaitingForRankInfo = true;
        isRankInfoReceviedSuccessfully = false;
        int leagueIndex = UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
//        BSM->getColosseumRank(UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0), leagueIndex);
        BSM->getArenaRank();
    }
    if(isColosseumInfoRefreshRequired){
        isColosseumInfoRefreshRequired = false;
        ((Text*)getChildByName("colosseum")->getChildByName("lblTicketCount"))->setString(strmake("%d", UDGetInt(KEY_GOLDEN_TICKET, 1)));
//        getChildByName("colosseum")->getChildByName("lblTimer")->setVisible(false);
    }
    if(isArenaRecordRecieved){
        isArenaRecordRecieved = false;
        ValueVector list = GM->split(arenaRecord, "_");
        if(arenaRecord.size() > 0 && arenaRecord.compare("0") != 0 && list.size() == 2 && list[1].asInt() > 0){
            showLeagueResult();
        }
    }
    
    colosseumUITimer += dt;
    if(colosseumUITimer >= 1){
        colosseumUITimer -= 1;
    }
    if(isLeagueNotFound){
        isLeagueNotFound = false;
        hideIndicator();
        
    }else if (isColosseumRankInfoReceived){
        log("rank info recived");
        
        isRankInfoReceviedSuccessfully = true;
        hideIndicator();
        BSM->getArenaReward();
        isColosseumRankInfoReceived = false;
        Node* layer = this->getChildByName("colosseum");
        if(layer->getTag() > 0){
            return;
        }
        layer->setTag(1);
        
        ScrollView* scView = (ScrollView*)layer->getChildByName("scView");
        ImageView* imgTemp = (ImageView*)scView->getChildByName("imgTemplate");
        float startY = 779.64;
        int gap = 142;
        float y = startY;
        
        ValueVector names = GM->split(strRankInfo, ",");
        if(names.size() == 1){
            Text* lbl = (Text*)getChildByName("colosseum")->getChildByName("lblRecommend");
            lbl->setVisible(true);
            LM->setLocalizedString(lbl, "challenge arena");
        }
        for(int i = 0; i < names.size() - 1; i++){
//        for(int i = 0; i < colosseumRankInfoResult.size(); i++){
            ImageView* img = (ImageView*)imgTemp->clone();
            Text* lbl = (Text*)img->getChildByName("lblName");
//            lbl->setFontName("Helvetica");
//            log("%d. name: %s", i + 1, colosseumRankInfoName.at(i).c_str());
//            lbl->setString(strmake("%d. %s", i + 1, colosseumRankInfoName.at(i).c_str()));
            ValueVector info = GM->split(names.at(i).asString(), "_");
            lbl->setString(strmake("%d. %s", i + 1, info.at(0).asString().c_str()));
            
//            if(colosseumRankInfoName.at(i).compare(GameSharing::getPlayerName()) == 0){
//                img->setColor(Color3B(80, 190, 80));
//            }else{
//                img->setColor(Color3B::WHITE);
//            }
            lbl = (Text*)img->getChildByName("lblRecord");
            if(info.size() > 1){
                lbl->setString(GM->getTimeLeftInString(info.at(1).asInt()));
            }else{
                log("what? %d");
            }
//            if(colosseumRankInfoResult.at(i) == 0){
//                lbl->setString(Value(colosseumRankInfoKill.at(i)).asString());
//            }else{
//                lbl->setString(GM->getTimeLeftInString(colosseumRankInfoTime.at(i)/100));
//            }
            scView->addChild(img);
            startY = 610.0f;
            gap = 142;
            y = startY - gap*i;
            if(gap*names.size() > startY){
                img->setPosition(Point(imgTemp->getPositionX(), gap*names.size() - gap*i));
            }else{
                img->setPosition(Point(imgTemp->getPositionX(), y));
            }
            img->setVisible(true);
        }
        scView->setInnerContainerSize(Size(scView->getContentSize().width, names.size()*gap));
        
        int myScore = UDGetInt(KEY_COLOSSEUM_SCORE, 0);
        
        ImageView* imgMe = (ImageView*)(ImageView*)layer->getChildByName("imgYou");
        Text* lbl = (Text*)imgMe->getChildByName("lblName");
        lbl->setString("sdf)");
        if(colosseumTotal == 0){
            colosseumTotal = 1;
        }
        LM->setLocalizedString(lbl, strmake("%d. %s (%s%d%%)", colosseumMe+1, UDGetStr(KEY_NAME).c_str(), LM->getText("top").c_str(), colosseumMe*100/colosseumTotal));
        lbl = (Text*)imgMe->getChildByName("lblRecord");
        lbl->setString(GM->getTimeLeftInString(myScore));
        if(myScore == 0){
            lbl = (Text*)imgMe->getChildByName("lblName");
            lbl->setString(LM->getText("play colosseum"));
            lbl->setContentSize(Size(400, 79));
            imgMe->getChildByName("lblRecord")->setVisible(false);
        }
    }
    if(getChildByName("colosseum")){
        int timeInSec = (int)(BSM->getCurrentTime());
        int timeLeft = 60*60*24 - timeInSec%(60*60*24);
        Node* layer = getChildByName("colosseum");
        Text* lbl = (Text*)layer->getChildByName("lblLeagueTimeLeft");
        lbl->setString(GM->getTimeLeftInString(timeLeft));
        
        int lastPlayTime = UDGetInt(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, 0);
        if(lastPlayTime > timeInSec){
            UDSetInt(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, timeInSec);
            lastPlayTime = timeInSec;
        }
        int cooltime = 60*60;
        timeLeft = cooltime - (timeInSec - lastPlayTime);//60*60*24 - timeInSec%(60*60*24);
        lbl = (Text*)layer->getChildByName("lblTimerForVideo");
        Button* btn = (Button*)layer->getChildByName("btnVideo");
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
    }
}
void Title::showTicketOffer(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNode("TicketOffer.csb");
    this->addChild(layer, 5);
    layer->setName("ticket offer");
    setAsPopup(layer);
    
    Text* lbl = (Text*)layer->getChildByName("lblTitle");
    lbl->setString(LM->getText("not enough ticket"));
    
    Button* btn = (Button*)layer->getChildByName("btnBuy");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onTicketOfferButtonClick, this));
    btn->setTag(0);
    btn->setPressedActionEnabled(true);
    
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "buy");
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onTicketOfferButtonClick, this));
    btn->setTag(1);
    btn->setPressedActionEnabled(true);
    LM->setLocalizedString((Text*)btn->getChildByName("lbl"), "cancel");
}
void Title::onTicketOfferButtonClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    if(btn->getTag() == 0){ // buy
//        showIndicator();
        // iap code should be here
        GameSharing::buyItem(IAP_DETAIL_TICKET_1);
        
//        showInstanceMessage(LM->getText("coming soon"));
    }else if(btn->getTag() == 1){ // cancel
        closePopup();
    }
}
void Title::onPlayColosseum(){
    int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
    if(freeTicketUsedDay != BSM->getDay()){
        UDSetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, BSM->getDay());
    }else{
        int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
//        ticketCount = 1; // test 
        if(ticketCount < 1){
            showTicketOffer();
            return;
        }
        ticketCount--;
        UDSetInt(KEY_GOLDEN_TICKET, ticketCount);
    }
    
    UDSetBool(KEY_GOLDEN_TICKET_FREE, false);
//    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime());
//    BSM->forceResultLeague();
//    return; // test
    int day = (int)(BSM->getCurrentTime()/(60*60*24)) + 1;
    int stageIndex = day%7;
    if(day < 0 || day > 6){
        day = rand()%7;
    }
    
    this->unschedule(schedule_selector(Title::titleUpdate));
    this->unschedule(schedule_selector(Title::updateColosseumPopup));
    isTitleEnd = true;
    
    clearAssets();
    GameManager::getInstance()->titleLayer = nullptr;
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    GM->nextScene = STAGE_FIELD;
    GM->isColosseum = true;
    auto scene = HelloWorld::scene(stageIndex, false);
    Director::getInstance()->replaceScene(TransitionFade::create(2, scene, Color3B::BLACK));
}
std::string Title::GetCurrentLeagueText(){
    int rank = UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0);
    log("rank league: %d", rank);
    return LM->getText(GetLeagueText(rank).c_str());
}
std::string Title::GetLeagueText(int rank){
    if(rank == 0){
        return "BEGINNER";
    }else if(rank == 1){
        return "BRONZE";
    }else if(rank == 2){
        return "SILVER";
    }else if(rank == 3){
        return "GOLD";
    }else if(rank == 4){
        return "PLATINUM";
    }else if(rank == 5){
        return "DIAMOND";
    }else if(rank == 6){
        return "MASTER";
    }else if(rank == 7){
        return "GRAND MASTER";
    }else if(rank == 8){
        return "LEGENDARY";
    }
    return "GUESS WHO";
}
void Title::rollOpenScroll(ImageView* imgBack){
    float originalWidth = imgBack->getContentSize().width;
    imgBack->setContentSize(Size(308, imgBack->getContentSize().height));
    imgBack->runAction(EaseInOut::create(ResizeTo::create(0.3, Size(originalWidth, imgBack->getContentSize().height)), 2));
    
    for(auto child : imgBack->getParent()->getChildren()){
        if(child == imgBack || child->getName().compare("btnBlock") == 0) continue;
        child->setOpacity(0);
        child->runAction(Sequence::create(DelayTime::create(0.2), FadeIn::create(0.3), NULL));
    }
}
void Title::showGlowBack(Node* node){
    int count = 5;
    for(int i = 0; i < count; i++){
        Sprite* spt = Sprite::create("longLight.png");
        node->addChild(spt, -1);
        spt->setRotation(360*i/count);
        spt->runAction(RepeatForever::create(RotateBy::create(1, 10)));
        spt->setPosition(node->getContentSize()/2);
        spt->setScale(1.5);
    }
    if(node->getChildByName("btnBlock")){
        node->getChildByName("btnBlock")->setLocalZOrder(-100);
    }
}
void Title::showLeagueResult(){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNode("ColosseumReward.csb");
    this->addChild(layer);
    setAsPopup(layer);
    
    UDSetBool(KEY_GOLDEN_TICKET_FREE, true);
    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
    
    rollOpenScroll((ImageView*)layer->getChildByName("imgBackground"));
    
    showGlowBack(layer);
    
    Button* btn = (Button*)layer->getChildByName("btnOk");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onCloseColosseumResult, this));
    
    Text* lbl =(Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "colosseum result");
    
    ValueVector data = GM->split(arenaRecord, "_");
    int rank = data[0].asInt();
    int total = data[1].asInt();
    int percent = rank*100/total;
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    int ticketCount = 0;
    int gemCount = 0;
    if (percent < 1) {
        LM->setLocalizedString(lbl, "colosseum top 1");
        ticketCount = 20;
        gemCount = 50;
    }else if (percent < 10) {
        LM->setLocalizedString(lbl, "colosseum top 10");
        ticketCount = 12;
        gemCount = 30;
    }else if (percent < 30) {
        LM->setLocalizedString(lbl, "colosseum top 30");
        ticketCount = 6;
        gemCount = 20;
    }else{
        LM->setLocalizedString(lbl, "colosseum top 100");
        ticketCount = 1;
        gemCount = 10;
    }
    
    lbl = (Text*)layer->getChildByName("lblRank");
    lbl->setString(strmake("%s %d/%d", LM->getText("rank").c_str(), rank+1, total));
    lbl = (Text*)layer->getChildByName("lblPercent");
    lbl->setString(strmake("%s %d%%", LM->getText("top").c_str(), percent + 1));
    
    lbl = (Text*)layer->getChildByName("lblTicketCount");
    lbl->setString(Value(ticketCount).asString());
    int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
    ticket += ticketCount;
    UDSetInt(KEY_GOLDEN_TICKET, ticket);
    
    lbl = (Text*)this->getChildByName("colosseum")->getChildByName("lblTicketCount");
    lbl->setString(strmake("%d", ticket));
    
    lbl = (Text*)layer->getChildByName("lblGemCount");
    lbl->setString(Value(gemCount).asString());
    GM->addGem(gemCount);
    UDSetInt(KEY_COLOSSEUM_SCORE, 0);
    BSM->saveUserData("gem=" + Value(GM->getGem()).asString() + "&arena_reward=0&golden_ticket=" + Value(ticket).asString());
}

void Title::onCloseColosseumResult(){
    closePopup();
    closePopup();
    showColosseum();
}
std::string Title::getRandomWord(){
    int index = rand()%40;
    if(index == 0) return "Cat";
    if(index == 1) return "Kitten";
    if(index == 2) return "Kitty";
    if(index == 3) return "Doggy";
    if(index == 4) return "Puppy";
    if(index == 5) return "Darling";
    if(index == 6) return "Sweety";
    if(index == 7) return "Goblin";
    if(index == 8) return "Ogre";
    if(index == 9) return "Orc";
    if(index == 10) return "Oak";
    if(index == 11) return "Worrior";
    if(index == 12) return "Max";
    if(index == 13) return "Archer";
    if(index == 14) return "Brave";
    if(index == 15) return "Awesome";
    if(index == 16) return "Mighty";
    if(index == 17) return "Red";
    if(index == 18) return "Purple";
    if(index == 19) return "Super";
    if(index == 20) return "Yellow";
    if(index == 21) return "Blue";
    if(index == 22) return "Sky";
    if(index == 23) return "Orange";
    if(index == 24) return "Green";
    if(index == 25) return "Tree";
    if(index == 26) return "Moon";
    if(index == 27) return "Indigo";
    if(index == 28) return "Board";
    if(index == 29) return "Gamer";
    if(index == 30) return "Maker";
    if(index == 31) return "Breaker";
    if(index == 32) return "Rule";
    if(index == 33) return "House";
    if(index == 34) return "Home";
    if(index == 35) return "Beer";
    if(index == 36) return "Lover";
    if(index == 37) return "Crazy";
    if(index == 38) return "General";
    if(index == 39) return "Star";
    return "";
}
std::string Title::getRandomName(){
    return strmake("%s %s", getRandomWord().c_str(), getRandomWord().c_str());
}
std::string Title::getRandomID(){
    return strmake("%s %s %d", getRandomWord().c_str(), getRandomWord().c_str(), rand()%100000);
}
void Title::clearAssets(){
    popupArray.clear();
    colosseumRankInfoName.clear();
    colosseumRankInfoResult.clear();
    colosseumRankInfoTime.clear();
    colosseumRankInfoKill.clear();
    colosseumRankInfoWeapon.clear();
}
void Title::onVideoDone(){
    if(GM->videoIndex == VIDEO_TICKET){
        int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
        ticket += 1;
        UDSetInt(KEY_GOLDEN_TICKET, ticket);
        isColosseumInfoRefreshRequired = true;
        isVideoDone = true;
    }
}

void Title::checkPlayerIDExist(){
    isUserIDExistCheckRequested = true;
    
    BSM->checkPlayID(GM->playerGPSID);
}
