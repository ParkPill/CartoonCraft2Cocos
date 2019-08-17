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
    title->setPositionX(size.width/2 - title->getContentSize().width/2);
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
    
    btn = (Button*)title->getChildByName("btnHero");//Button::create("btnBox.png");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    lbl = addLabelToButton(btn, "hero", 60, Color3B(4, 90, 4));
    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    btn->setVisible(false); // test now
    
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
//    GM->makeItScaleUpAndDown(btn->getChildByName("imgNewTag"));
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
    if(LM->getLanguageType() == LanguageType::KOREAN){
        
    }else{
        btn->setVisible(false);
    }
//    lbl = addLabelToButton(btn, "forum", 60, Color3B(4, 90, 4));
//    lbl->setPosition(lbl->getPosition() + Point(-110, 10));
//    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
    
    btn = (Button*)title->getChildByName("btnFacebook");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onFacebookClick, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime), FadeIn::create(0.5f), NULL));
    
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
    
//    BSM->getHttpTime();
//    UDSetInt(KEY_LAST_CLEAR_STAGE, 11); // test
//    UDSetBool(KEY_CHAPTER_2_PURCHASED, false); // test
//    UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test
//    UDSetBool(KEY_HERO_ALERT_NEVER_SHOW, false); // test
    log("Title init done");
    
//    spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
//    this->addChild(spChar);
//    spChar->setAnimation(0, "idle", true);
//    spChar->setPosition(Vec2(150, 100));
    
    
//    showExitPopup(); // test
    return true;
}
void Title::onFacebookClick(){
    Application::getInstance()->openURL("https://www.facebook.com/StudioNAP/");
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
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
    Node* layer = CSLoader::createNode("ChapterSelect.csb");
    this->addChild(layer, 4);
    layer->setName("chapterSelect");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    Button* btnBlock = (Button*)layer->getChildByName("btnBlock");
    btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
//    btn = (Button*)layer->getChildByName("btnHardMode");
//    btn->addClickEventListener(CC_CALLBACK_0(Title::onHardModeClick, this));
    
    btn = (Button*)layer->getChildByName("btnHero");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroClick, this));
//    btn->setVisible(false); // test
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "hero");
    
    int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
    bool isUnlocked = UDGetBool(KEY_CHAPTER_2_PURCHASED) || UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11;
    btn->getChildByName("imgLock")->setVisible(!isUnlocked);
    
    for (int i = 0; i < 3; i++) {
        ImageView* img = (ImageView*)btn->getChildByName(strmake("img%d", i));
        spine::SkeletonAnimation* spChar;
        if(i == 0){
            spChar = spine::SkeletonAnimation::createWithJsonFile("lizard.json", "lizard.atlas", 1);
        }else if(i == 1){
            spChar = spine::SkeletonAnimation::createWithJsonFile("spearMan.json", "spearMan.atlas", 1);
        }else if(i == 2){
            spChar = spine::SkeletonAnimation::createWithJsonFile("werewolf.json", "werewolf.atlas", 1);
        }
        btn->addChild(spChar, 0);
//        spChar->setAnimation(0, "idle", true);
        spChar->setPosition(img->getPosition());
        spChar->setScale(0.75f);
        img->removeFromParent();
        Sprite* sptShadow = Sprite::create("shadow.png");
        spChar->addChild(sptShadow, -1);
    }
    btn->getChildByName("imgLock")->setLocalZOrder(10);
    lbl = (Text*)layer->getChildByName("imgTitle")->getChildByName("lbl");
    LM->setLocalizedString(lbl, "chapter");
    doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->setClippingEnabled(true);
//    sv->setInnerContainerSize(sv->getContentSize());
    for (int i = 0; i < 3; i++) {
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
                LM->setLocalizedString(lbl, "chapter unlock condition");
                btn->addClickEventListener(CC_CALLBACK_1(Title::onPurchaseChapterClick, this));
                lbl = (Text*)btn->getChildByName("lblTitle");
                LM->setLocalizedString(lbl, "or");
                lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_CHAPTER2));
            }
        }else if(i == 2){
            Node* img = btn->getChildByName("img");
            
            spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
            btn->addChild(spChar, 0);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(img->getPosition());
            img->removeFromParent();
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
            
            img->removeFromParent();
            btn->getChildByName("lblCondition")->setLocalZOrder(2);
            btn->getChildByName("lblOr")->setLocalZOrder(2);
            btn->getChildByName("imgConditionBack")->setLocalZOrder(1);
            
            int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
            if (UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 23) {
                LM->setLocalizedString(lbl, "khalan");
                btn->getChildByName("lblOr")->setVisible(false);
                btn->getChildByName("imgConditionBack")->setVisible(false);
                btn->getChildByName("lblCondition")->setVisible(false);
                btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
            }else{
                lbl = (Text*)btn->getChildByName("lblOr");
                LM->setLocalizedString(lbl, "or");
                lbl = (Text*)btn->getChildByName("lblCondition");
                LM->setLocalizedString(lbl, "chapter unlock condition");
                btn->addClickEventListener(CC_CALLBACK_1(Title::onPurchaseChapterClick, this));
                lbl = (Text*)btn->getChildByName("lblTitle");
                LM->setLocalizedString(lbl, "or");
                lbl->setString(GameSharing::getPriceLocale(IAP_DETAIL_CHAPTER3));
            }
        }else if(i == 77){
            LM->setLocalizedString(lbl, "coming soon");
            spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("orc.json", "orc.atlas", 1);
            btn->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
            spChar->setPosition(img->getPosition());
            img->removeFromParent();
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
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
void Title::onHeroClick(){
    int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
    bool isUnlocked = UDGetBool(KEY_CHAPTER_2_PURCHASED) || UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11;
    if (isUnlocked) {
        showHeroPage(true);
    }else{
        showInstanceMessage(LM->getText("worker condition 1")); // test
    }
}
void Title::onCloseShop(){
    Node* heroPage = this->getChildByName("heroPage");
    if(heroPage != nullptr){
        ndTopBar->removeFromParentAndCleanup(false);
        heroPage->addChild(ndTopBar);
        closePopup();
    }
}
void Title::onHeroMoreResourceClick(){
    onHeroShopClick();
    onHeroShopTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab1"));
}
void Title::onHeroMoreGemClick(){
    onHeroShopClick();
    onHeroShopTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab2"));
}
void Title::showHeroPage(bool showAlert){
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNodeWithVisibleSize("HeroPage.csb");
    this->addChild(layer, 4);
    layer->setName("heroPage");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    ndTopBar = layer->getChildByName("ndTopBar");
    ndTopBar->retain();
    Button* btn;
    btn = (Button*)ndTopBar->getChildByName("btnGold");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroMoreResourceClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnTree");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroMoreResourceClick, this));
    btn = (Button*)ndTopBar->getChildByName("btnGem");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroMoreGemClick, this));
    lblGemInHud = (Text*)ndTopBar->getChildByName("lblGem");
    lblGemInHud->setString(Value(GM->getGem()).asString());
    lastGemUpdated = GM->getGem();
    lblGoldInHud = (Text*)ndTopBar->getChildByName("lblGold");
    lblGoldInHud->setString(Value(GM->getCoin()).asString());
    lastGoldUpdated = GM->getCoin() - 1;
    lblTreeInHud = (Text*)ndTopBar->getChildByName("lblTree");
    lblTreeInHud->setString(Value(GM->getTree()).asString());
    lastTreeUpdated = GM->getTree() - 1;
    
    btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn = (Button*)layer->getChildByName("btnShop");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroShopClick, this));
    
    btn = (Button*)layer->getChildByName("btnGacha0");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onChestClick, this));
    Text* lbl = (Text*)btn->getChildByName("lblDescription");
    lbl->setString(strmake("%s, %s, %s", LM->getText("common hero").c_str(), LM->getText("good hero").c_str(), LM->getText("great hero").c_str()));
    btn = (Button*)layer->getChildByName("btnGacha1");
    btn->addClickEventListener(CC_CALLBACK_1(Title::onChestClick, this));
    lbl = (Text*)btn->getChildByName("lblDescription");
    lbl->setString(strmake("%s, %s, %s", LM->getText("great hero").c_str(), LM->getText("rare hero").c_str(), LM->getText("epic hero").c_str()));
    
    btn = (Button*)layer->getChildByName("btnArrangeRarity");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onSortByRarity, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "sort by rarity");
    
    btn = (Button*)layer->getChildByName("btnArrangeSlot");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onSortBySlot, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "sort by slot");
    
    btn = (Button*)layer->getChildByName("btnFuse");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onFusionClick, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "fusion");
    
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "deck");
    lbl = (Text*)layer->getChildByName("lblInventory");
    LM->setLocalizedString(lbl, "inventory");
    
    std::string str;
    std::string strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
    ValueVector units = GM->split(strEquipped, "_");
    UnitInfo* info;
    int index = 0;
    unitInfoListHeroDeck.clear();
    for (int i = 0; i < units.size(); i++){
        str = units.at(i).asString();
        btn = (Button*)layer->getChildByName(strmake("btnEquip%d", index));
        if(str.length() > 0){
            info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info->unitType < 0){
                continue;
            }
            unitInfoListHeroDeck.push_back(info);
            btn->addClickEventListener(CC_CALLBACK_1(Title::onEquipedHeroClick, this));
        }else{
            continue;
        }
        
        lbl = (Text*)btn->getChildByName("lblLevel");
        
        if (unitInfoListHeroDeck.size() > index) {
            info = unitInfoListHeroDeck.at(index);
            lbl->setString(strmake("LV.%d", info->level + 1));
            std::string strFile = GM->getSpineFileName(info->unitType);
            spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
            layer->addChild(spChar);
            spChar->setAnimation(0, "idle", true);
//            spChar->setScale(0.75f);
            Sprite* sptShadow = Sprite::create("shadow.png");
            spChar->addChild(sptShadow, -1);
            sptShadow->setOpacity(0);
            spChar->setPosition(btn->getPosition() + Vec2(0, -45));
            btn->setColor(getRankColor(info->rank));
        }else{
//            btn->setColor(Color3B::WHITE);
//            lbl->setString("^ 0^/");
        }
        index++;
        if(index >= 6){
            break;
        }
    }
    for(int i = index; i < 6; i++){
        btn = (Button*)layer->getChildByName(strmake("btnEquip%d", i));
        lbl = (Text*)btn->getChildByName("lblLevel");
        lbl->setString("^ 0^/");
    }
    
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
//    sv->setClippingEnabled(true);
//    UDSetStr(KEY_UNITS_HERO_INVENTORY, "_50/101_49/0_49/0_49/0_50/101_49/200_49/200_49/200"); // test
    strEquipped = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
    units = GM->split(strEquipped, "_");
    sv->setContentSize(Size(size.width, 244));
    
    unitInfoListHeroInventory.clear();
    ImageView* img;
    ImageView* imgTemp = (ImageView*)layer->getChildByName("btnEquip0");
    index = 0;
    for(int i = 0; i < units.size(); i++){
        str = units.at(i).asString();
        UnitInfo* info;
        if(str.length() > 0){
            info = GM->getUnitInfoFromString(units.at(i).asString());
            if(info->unitType < 0){
                continue;
            }
            unitInfoListHeroInventory.push_back(info);
        }else{
            continue;
        }
        
        img = (ImageView*)imgTemp->clone();
        sv->addChild(img);
        img->setTag(index);
        img->setPosition(Vec2(img->getContentSize().width*(0.45f + index), sv->getContentSize().height/2));
        img->addClickEventListener(CC_CALLBACK_1(Title::onHeroInInventoryClick, this));
        int rareness = info->rank;
        img->setColor(getRankColor(rareness));
        Text* lbl = (Text*)img->getChildByName("lblLevel");
        lbl->setLocalZOrder(10);
        lbl->setString(strmake("LV.%d", info->level + 1));
        
        std::string strFile = GM->getSpineFileName(info->unitType);
        spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
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
        sv->setInnerContainerSize(Size(totalWidth, sv->getContentSize().height));
    }else{
        sv->setInnerContainerSize(Size(size.width, sv->getContentSize().height));
    }
    int arrangeType = UDGetInt(KEY_HERO_ARRANGE_TYPE, 0);
    if(arrangeType == 1){
        onSortByRarity();
    }
    
    
    int sameUnitCounter = 0;
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        sameUnitCounter = 1;
        for (int j = i + 1; j < unitInfoListHeroInventory.size(); j++) {
            if (unitInfoListHeroInventory.at(i)->unitType == unitInfoListHeroInventory.at(j)->unitType &&
                unitInfoListHeroInventory.at(i)->rank == unitInfoListHeroInventory.at(j)->rank) {
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
    
    if(showAlert && !UDGetBool(KEY_HERO_ALERT_NEVER_SHOW, false)){
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
        btn->addClickEventListener(CC_CALLBACK_0(Title::onNeverShowHeroAlertClick, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "never show");
        
        btn = (Button*)img->getChildByName("btnOk");
        btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
        lbl = (Text*)btn->getChildByName("lbl");
        LM->setLocalizedString(lbl, "ok");
    }
    titleUpdate(0);
}
void Title::onNeverShowHeroAlertClick(){
    UDSetBool(KEY_HERO_ALERT_NEVER_SHOW, true);
    closePopup();
}
void Title::onEquipedHeroClick(Ref* ref){
    BTN_FROM_REF
    UnitInfo* info = unitInfoListHeroDeck.at(btn->getTag());
    showHeroDetail(info);
}
void Title::onSortByRarity(){
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
        nd->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), sv->getContentSize().height/2));
        
        Node* ndChar = sv->getChildByTag(indexList.at(i) + 100000);
        ndChar->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), sv->getContentSize().height/2 - 45));
    }
}
void Title::onSortBySlot(){
    UDSetInt(KEY_HERO_ARRANGE_TYPE, 0);
    Node* layer = this->getChildByName("heroPage");
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    for (int i = 0; i < unitInfoListHeroInventory.size(); i++) {
        Node* nd = sv->getChildByTag(i);
        nd->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), sv->getContentSize().height/2));
        
        Node* ndChar = sv->getChildByTag(i + 100000);
        ndChar->setPosition(Vec2(nd->getContentSize().width*(0.45f + i), sv->getContentSize().height/2 - 45));
    }
}
void Title::onFusionClick(){
    Node* layer = CSLoader::createNodeWithVisibleSize("FusionPage.csb");
    this->addChild(layer, 4);
    layer->setName("fusionPage");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    layer->getChildByName("imgResult")->setOpacity(0);
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnBack");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    
    btn = (Button*)layer->getChildByName("btnFusion");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onFusionButtonClick, this));
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "fusion");
    
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "fusion");
    lbl = (Text*)layer->getChildByName("lblInventory");
    LM->setLocalizedString(lbl, "inventory");
    
    std::string str;
    ScrollView* sv = (ScrollView*)layer->getChildByName("svInventory");
    sv->setClippingEnabled(true);
    sv->setContentSize(Size(size.width, 244));
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
                candidatePoolList.at(i)->rank == candidatePoolList.at(j)->rank) {
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
        btn->addClickEventListener(CC_CALLBACK_1(Title::onSelectedHeroClickForFusion, this));
    }
}
void Title::onSelectedHeroClickForFusion(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("fusionPage");
    Node* spine = layer->getChildByName(strmake("selectedHeroSpine%d", btn->getTag()));
    if(spine){
        unitInfoListHeroInventoryArrangedForFusion.push_back(unitInfoListHeroSelectedForFusion.at(btn->getTag()));
        unitInfoListHeroSelectedForFusion.erase(unitInfoListHeroSelectedForFusion.begin() + btn->getTag());
        updateUnitListForFusion();
    }
}
void Title::onHeroInFusionClick(Ref* ref){
    if(unitInfoListHeroSelectedForFusion.size() >= 3){
        return;
    }
    ImageView* img = (ImageView*)ref;
    unitInfoListHeroSelectedForFusion.push_back(unitInfoListHeroInventoryArrangedForFusion.at(img->getTag()));
    unitInfoListHeroInventoryArrangedForFusion.erase(unitInfoListHeroInventoryArrangedForFusion.begin() + img->getTag());
    updateUnitListForFusion();
}
void Title::onFusionButtonClick(){
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
    saveUserData(datas);
    
    Button* btnOk = (Button*)layer->getChildByName("btnOk");
    btnOk->addClickEventListener(CC_CALLBACK_0(Title::onOkFusionResult, this));
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
void Title::onOkFusionResult(){
    closePopup();
    closePopup();
    showHeroPage();
}
void Title::updateUnitListForFusion(){
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
        img->setPosition(Vec2(img->getContentSize().width*(0.45f + index), sv->getContentSize().height/2));
        img->addClickEventListener(CC_CALLBACK_1(Title::onHeroInFusionClick, this));
        int rareness = info->rank;
        img->setColor(getRankColor(rareness));
        Text* lbl = (Text*)img->getChildByName("lblLevel");
        lbl->setLocalZOrder(10);
        lbl->setString(strmake("LV.%d", info->level + 1));
        
        std::string strFile = GM->getSpineFileName(info->unitType);
        spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
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
        sv->setInnerContainerSize(Size(totalWidth, sv->getContentSize().height));
    }else{
        sv->setInnerContainerSize(Size(size.width, sv->getContentSize().height));
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
        
        std::string strFile = GM->getSpineFileName(info->unitType);
        spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
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
    btnFusion->setEnabled(allRegistered);
}
void Title::onHeroInInventoryClick(Ref* ref){
    ImageView* img = (ImageView*)ref;
    UnitInfo* info = unitInfoListHeroInventory.at(img->getTag());
    showHeroDetail(info);
}
void Title::showHeroDetail(UnitInfo* info, bool showLightning){
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
    layer->setName("unitTrainLayer");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn = (Button*)layer->getChildByName("btnBlock");
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    Button* btnUpgrade = (Button*)layer->getChildByName("btnUpgrade");
    btnUpgrade->addClickEventListener(CC_CALLBACK_0(Title::onUpgradeUnitClick, this));
    btnUpgrade->setEnabled(info->rank < 20);
    int unit = info->unitType;
    int level = info->level;
    Text* lbl = (Text*)btnUpgrade->getChildByName("lblGold");
    lbl->setString(Value(getMaxGold(level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTree");
    lbl->setString(Value(getMaxTree(level)).asString());
    lbl = (Text*)btnUpgrade->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "upgrade");
    Button* btnHire = (Button*)layer->getChildByName("btnHire");
    btnHire->setVisible(false);
    Button* btnMoveToDeck = (Button*)layer->getChildByName("btnMoveToDeck");
    btnMoveToDeck->addClickEventListener(CC_CALLBACK_0(Title::onMoveToDeckUnitClick, this));
    btnMoveToDeck->setVisible(isFromInventory);
    lbl = (Text*)btnMoveToDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "move to deck");
    Button* btnRemoveFromDeck = (Button*)layer->getChildByName("btnRemoveFromDeck");
    btnRemoveFromDeck->addClickEventListener(CC_CALLBACK_0(Title::onRemoveFromDeckUnitClick, this));
    btnRemoveFromDeck->setVisible(isFromDeck);
    lbl = (Text*)btnRemoveFromDeck->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "remove from deck");
    Button* btnFire = (Button*)layer->getChildByName("btnFire");
    btnFire->setVisible(false);
    lbl = (Text*)btnFire->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "fire");
    btn = (Button*)layer->getChildByName("btnComplete");
    btn->setVisible(false);
//    btn->setPosition(btnUpgrade->getPosition());
    
    Node* nd = layer->getChildByName("ndUpgradeTime");
    nd->setVisible(false);
    lbl = (Text*)layer->getChildByName("lblHP");
    lbl->setString(Value((int)(GM->getUnitHP(unit, level)*(1 + info->rank*0.2f))).asString());
    lbl = (Text*)layer->getChildByName("lblLevel");
    lbl->setString(strmake("Lv.%d", level + 1));
    lbl = (Text*)lbl->clone();
    lbl->setAnchorPoint(Vec2(0.5, 0.5));
    lbl->setPosition(Vec2(955.5f, 761.5f));
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
    lbl->setFontSize(44);
    lbl->enableOutline(Color4B::BLACK, 4);
    layer->addChild(lbl, 10);
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
    ImageView* img = (ImageView*)ndPopulation->getChildByName("imgPopulation");
    img->loadTexture("iconSkill.png");
    img->setContentSize(Size(100, 100));
    lbl = (Text*)ndPopulation->getChildByName("lblPopulation");
    lbl->setString(strmake("%s%%", Value(10 + 3*info->rank).asString().c_str()));
    layer->getChildByName("ndUpgradeTimer")->setVisible(false);
    img = (ImageView*)layer->getChildByName("imgIconBack");
    img->setColor(getRankColor(info->rank));
    
    
    std::string strFile = GM->getSpineFileName(info->unitType);
    spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
    spChar->setAnimation(0, "idle", true);
    spChar->setScale(1.35f);
    Sprite* sptShadow = Sprite::create("shadow.png");
    spChar->addChild(sptShadow, -1);
    sptShadow->setOpacity(0);
    btn->setColor(getRankColor(info->rank));
    spChar->setName("spChar");
    layer->addChild(spChar);
    spChar->setPosition(layer->getChildByName("imgIconBack")->getPosition() + Vec2(0, -100));
    
//    btnMoveToDeck->setPosition(btnHire->getPosition());
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
    
}
void Title::onRemoveFromDeckUnitClick(){
    int index= 0;
    for(auto info: unitInfoListHeroDeck){
        if(info == selectedUnitInfo){
            unitInfoListHeroDeck.erase(unitInfoListHeroDeck.begin() + index);
            break;
        }
        index++;
    }
    
    unitInfoListHeroInventory.push_back(selectedUnitInfo);
    
    std::vector<int> datas;
    updateHeroDeckSaveData();
    datas.push_back(DATA_TYPE_HERO_DECK);
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    saveUserData(datas);
    
    closePopup();
    closePopup();
    showHeroPage();
}
void Title::onMoveToDeckUnitClick(){
    if(unitInfoListHeroDeck.size() >= 6){
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
    
    std::vector<int> datas;
    updateHeroDeckSaveData();
    datas.push_back(DATA_TYPE_HERO_DECK);
    updateHeroInventorySaveData();
    datas.push_back(DATA_TYPE_HERO_INVENTORY);
    saveUserData(datas);
    
    closePopup();
    closePopup();
    showHeroPage();
}
void Title::onUpgradeUnitClick(){
    int goldPrice = getMaxGold(selectedUnitInfo->level);
    int treePrice = getMaxTree(selectedUnitInfo->level);
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
        datas.push_back(DATA_TYPE_DECK);
    }
    if (isFromInventory) {
        updateHeroInventorySaveData();
        datas.push_back(DATA_TYPE_HERO_INVENTORY);
    }
    saveUserData(datas);
    
    
    closePopup();
    closePopup();
    showHeroPage();
    if (isFromDeck) {
        showHeroDetail(unitInfoListHeroDeck.at(deckIndex), true);
    }else if(isFromInventory){
        showHeroDetail(unitInfoListHeroInventory.at(inventoryIndex), true);
    }
    Node* spChar = this->getChildByName("unitTrainLayer")->getChildByName("spChar");
    float originalScale = spChar->getScaleX();
    spChar->runAction(Sequence::create(ScaleTo::create(0.15f, originalScale*1.2f), EaseIn::create(ScaleTo::create(0.12f, originalScale*1), 2), nullptr));
}
void Title::onChestClick(Ref* ref){
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
        }else if(lbl->getString().compare("FREE") == 0){
            std::string value = "";
            value += BSM->getStrFromTime(now + 60*60*23 + 60*30);
            log("value: %s", value.c_str());
            
            UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value);
            UserDefault::getInstance()->setStringForKey(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, value);
            
            std::string strNew = UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
            log("strNew: %s", strNew.c_str());
            pickHero(0, 2);
            saveInventory = true;
        }else{
            int price = Value(lbl->getString()).asInt();
            if(price <= GM->getGem()){
                GM->addGem(-price);
                pickHero(0, 2);
                saveGem = true;
                saveInventory = true;
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
        }else if(lbl->getString().compare("FREE") == 0){
            std::string value = "";
            value += BSM->getStrFromTime(now + 60*60*24*7 - 60*30);
            UDSetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, value);
            log("value: %s", value.c_str());
            std::string strNew = UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
            log("strNew: %s", strNew.c_str());
            pickHero(2, 4);
            saveInventory = true;
        }else{
            int price = Value(lbl->getString()).asInt();
            if(price <= GM->getGem()){
                GM->addGem(-price);
                pickHero(2, 4);
                saveInventory = true;
                saveGem = true;
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
        saveUserData(datas);
    }
}
void Title::pickHero(int minRank, int maxRank){
    int rankRate[5];
    rankRate[0] = 80;
    rankRate[1] = 50;
    rankRate[2] = 30;
    rankRate[3] = 10;
    rankRate[4] = 2;
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
    int index = rand()%6;
    if(index == 0){
        array.push_back(UNIT_HERO_GOBLIN);
    }else if(index == 1){
        array.push_back(UNIT_HERO_ORC);
    }else if(index == 2){
        array.push_back(UNIT_HERO_LIZARDMAN);
    }else if(index == 3){
        array.push_back(UNIT_HERO_ARCHER);
    }else if(index == 4){
        array.push_back(UNIT_HERO_SPEARMAN);
    }else if(index == 5){
        array.push_back(UNIT_HERO_WEREWOLF);
    }
    
    for(int i = 0; i < array.size(); i++){
        UnitInfo* info = new UnitInfo();
        info->unitType = array.at(i);
        info->level = pickedRank*100;
        unitInfoListHeroInventory.push_back(info);
    }
    updateHeroInventorySaveData();
    
    closePopup();
    showHeroPage();
    
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
        std::string strFile = GM->getSpineFileName(value);
        spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
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
        btnAgain->addClickEventListener(CC_CALLBACK_0(Title::onHeroWoodChestGachaAgainClick, this));
    }else{
        btnAgain->addClickEventListener(CC_CALLBACK_0(Title::onHeroGoldChestGachaAgainClick, this));
    }
    
    btnAgain->setOpacity(0);
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
    btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
    btn->setOpacity(0);
    btn->runAction(Sequence::create(DelayTime::create(showOffTime + initTime + fadeInTime + whiteStayTime + 1), FadeIn::create(fadeInTime), NULL));
    
    imgTemp->removeFromParent();
    imgWhite->removeFromParent();
    
    //    BHUD->showIndicator();
    
}
Color3B Title::getRankColor(int rank){
    if (rank == 0) {
        return Color3B(255, 255, 255);
    }else if (rank == 1) {
        return Color3B(57, 211, 205);
    }else if (rank == 2) {
        return Color3B(0, 180, 241);
    }else if (rank == 3) {
        return Color3B(255, 148, 183);
    }else if (rank == 4) {
        return Color3B(179, 22, 135);
    }
    return Color3B::WHITE;
}
void Title::onHeroWoodChestGachaAgainClick(){
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
    saveUserData(datas);
    
}
void Title::onHeroGoldChestGachaAgainClick(){
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
    saveUserData(datas);
}
void Title::updateHeroInventorySaveData(){
    std::string strInventory = "_";
    for(auto unitInfo: unitInfoListHeroInventory){
        strInventory += strmake("%d/%d_",unitInfo->unitType, unitInfo->level + unitInfo->rank*100);
    }
    UDSetStr(KEY_UNITS_HERO_INVENTORY, strInventory);
}

void Title::updateHeroDeckSaveData(){
    std::string strDeck = "_";
    for(auto unitInfo: unitInfoListHeroDeck){
        strDeck += strmake("%d/%d_",unitInfo->unitType, unitInfo->level + unitInfo->rank*100);
    }
    
    UDSetStr(KEY_UNITS_HERO_DECK, strDeck);
}
void Title::onHeroShopClick(){
    if(this->getChildByName("shopLayer") != nullptr){
        return;
    }
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNodeWithVisibleSize("Shop.csb");
    this->addChild(layer, 4);
    layer->setName("shopLayer");
    setAsPopup(layer);
    layer->setTag(0);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    
    layer->getChildByName("imgBackground")->setLocalZOrder(-100);
    layer->getChildByName("btnTab0")->setVisible(false);
    layer->getChildByName("imgTabIcon0")->setVisible(false);
//    moveTopBarTo(layer);
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(Title::onCloseShop, this));
    btn->setPositionY(btn->getPositionY() + layer->getPositionY());
    
    ndTopBar->removeFromParentAndCleanup(false);
    layer->addChild(ndTopBar);
    
//    onHeroShopTabClick(layer->getChildByName("btnTab0"));
    for (int i = 0; i < 3; i++) {
        btn = (Button*)layer->getChildByName(strmake("btnTab%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(Title::onHeroShopTabClick, this));
        layer->getChildByName(strmake("imgTabIcon%d", i))->setLocalZOrder(100);
    }
    onHeroShopTabClick(layer->getChildByName(strmake("btnTab%d", 1)));
}
int Title::getMineGoldPerHour(int level){
    string str = GM->buildingAbilityForCastleLevelTable[Value(level).asString()].asValueMap()["mine"].asString();
    return Value(GM->split(str, "_").at(0)).asInt();
}
void Title::onHeroShopTabClick(Ref* ref){
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
    
    Button* btnTemp;Text* lbl;
    ScrollView* sv = (ScrollView*)layer->getChildByName("sv");
    sv->removeAllChildren();
    int x = 387.93f;
    int gapX = 740.45f;
    int y = 429.55f;
    if (currentTab == 0) {
        
    }else if (currentTab == 1) {
        for (int i = 0; i < 3; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnGold%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(Title::onBuyGold, this));
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
            btn->addClickEventListener(CC_CALLBACK_1(Title::onBuyTree, this));
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
        btn = (Button*)((Button*)layer->getChildByName("btnStarterKeys"))->clone();
        btn->addClickEventListener(CC_CALLBACK_0(Title::onBuyStartKeys, this));
        sv->addChild(btn);
        btn->setPosition(Vec2(x, y));
        x += gapX;
        
        lbl = (Text*)btn->getChildByName("lblTitle");
        lbl->setString(LM->getText("starter pack"));
        lbl = (Text*)btn->getChildByName("lblPrice");
        lbl->setFontName(LM->getLocalizedFont());
        lbl->setString(GameSharing::getPriceLocale("cc_starterkey"));
        
        for (int i = 0; i < 4; i++) {
            btn = (Button*)((Button*)layer->getChildByName(strmake("btnGem%d", i)))->clone();
            btn->addClickEventListener(CC_CALLBACK_1(Title::onBuyGem, this));
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
void Title::onBuyStartKeys(){
    GameSharing::buyItem(IAP_DETAIL_GEM_3);
    showIndicator();
}
void Title::onBuyTree(Ref* ref){
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
void Title::onBuyGold(Ref* ref){
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
void Title::saveUserData(std::vector<int>& datas){
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
        }else if (datas.at(i) == DATA_TYPE_KEYS) {
            strData = strmake("%s&keys=%s", strData.c_str(), UDGetStr(KEY_KEYS, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_KEY_GET_STATE) {
            strData = strmake("%s&keygetstate=%s", strData.c_str(), UDGetStr(KEY_KEY_GET_STATE, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_IAP) {
            strData = strmake("%s&iap_list=%s&iap_total=%d", strData.c_str(), UDGetStr(KEY_IAP_LIST, "").c_str(), UDGetInt(KEY_IAP_TOTAL, 0));
        }else if (datas.at(i) == DATA_TYPE_SEARCH_STATE) {
            strData = strmake("%s&search_state=%s", strData.c_str(), UDGetStr(KEY_SEARCH_STATE, "000").c_str());
        }else if (datas.at(i) == DATA_TYPE_HERO_INVENTORY) {
            strData = strmake("%s&hivt=%d", strData.c_str(), GM->getTree());
        }else if (datas.at(i) == DATA_TYPE_HERO_DECK) {
            strData = strmake("%s&hdck=%d", strData.c_str(), GM->getTree());
        }
    }
    strData = strData.substr(1); // remove first &
    BSM->saveUserData(strData);
    log("save data: %s", strData.c_str());
    datas.clear();
}
void Title::onBuyGem(Ref* ref){
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
void Title::onHardModeClick(){
    showInstanceMessage(LM->getText("coming soon")); // test
}
void Title::onPurchaseChapterClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    if(btn->getTag() == 1){
        GameSharing::buyItem(IAP_DETAIL_CHAPTER2);
    }else if(btn->getTag() == 2){
        GameSharing::buyItem(IAP_DETAIL_CHAPTER3);
    }
}
void Title::onChapterClick(Ref* ref){
    BTN_FROM_REF
    selectedChapter = btn->getTag();
    if(btn->getTag() == 0){
        showStageSelect(btn->getTag());
    }else if(btn->getTag() == 1){
        showStageSelect(btn->getTag());
    }else if(btn->getTag() == 2){
        showStageSelect(btn->getTag());
    }else if(btn->getTag() == 3){
        showInstanceMessage(LM->getText("coming soon")); // test
    }
}
void Title::showStageSelect(int chapter){
    showInterstitialRequested = true;
    GM->playSoundEffect(SOUND_PAPER_FLIP);
    Node* layer = CSLoader::createNode("StageSelect.csb");
    this->addChild(layer, 4);
    layer->setName("stageSelect");
    setAsPopup(layer);
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    if(lblGemInHud && GM->getGem() != lastGemUpdated){
        lastGemUpdated = GM->makeNumberCloseTo(lastGemUpdated, GM->getGem());
        lblGemInHud->setString(Value(lastGemUpdated).asString());
    }
    if(lblGoldInHud && GM->getCoin() != lastGoldUpdated){
        lastGoldUpdated = GM->makeNumberCloseTo(lastGoldUpdated, GM->getCoin());
        lblGoldInHud->setString(Value(lastGoldUpdated).asString());
        float rate = GM->getCoin()*1.0f/getMaxGold();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgGoldGuage")->setContentSize(Size(443.56f*rate, 128.90f));
    }
    if(lblTreeInHud && GM->getTree() != lastTreeUpdated){
        lastTreeUpdated = GM->makeNumberCloseTo(lastTreeUpdated, GM->getTree());
        lblTreeInHud->setString(Value(lastTreeUpdated).asString());
        float rate = GM->getTree()*1.0f/getMaxTree();
        if(rate > 1){
            rate = 1;
        }
        ndTopBar->getChildByName("imgTreeGuage")->setContentSize(Size(443.56f*rate, 128.90f));
    }
    
    time_t now = BSM->getCurrentTimeT();
    std::string strNextWoodChestGachaFreeTime = UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
    time_t nextWoodChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextWoodChestGachaFreeTime);
    bool isWoodChestGachaFreeReady = difftime(nextWoodChestGachaFreeTimeT, now) <= 0;
    
    std::string strNextGoldChestGachaFreeTime = UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
    time_t nextGoldChestGachaFreeTimeT = BSM->getTimeTFromStr(strNextGoldChestGachaFreeTime);
    bool isGoldChestGachaFreeReady = difftime(nextGoldChestGachaFreeTimeT, now) <= 0;
    
    Node* layer = this->getChildByName("heroPage");
    
    Node* ndChapterSelect = this->getChildByName("chapterSelect");
    if(ndChapterSelect && ndChapterSelect != nullptr){
        ndChapterSelect->getChildByName("btnHero")->getChildByName("imgRedDot")->setVisible(isWoodChestGachaFreeReady || isGoldChestGachaFreeReady);
    }
    
//    layer->getChildByName("btnTrainBR")->getChildByName("imgRedDot")->setVisible(isGachaFreeReady || isGachaVideoReady);
    if(layer && layer != nullptr){
        Button* btn = (Button*)layer->getChildByName("btnGacha0");
        btn->getChildByName("imgRedDot")->setVisible(isWoodChestGachaFreeReady);
        
        Text* lbl = (Text*)btn->getChildByName("lbl");
        ImageView* img = (ImageView*)btn->getChildByName("imgIcon");
        if(GM->getWoodKey() > 0){
            lbl->setString(Value(GM->getWoodKey()).asString());
            img->loadTexture("keyWood.png");
            img->setContentSize(Size(31, 60));
        }else{
            lbl->setString(isWoodChestGachaFreeReady?"FREE":"60");
            img->loadTexture("gem.png");
            img->setContentSize(Size(97, 78));
        }
        
        Text* lblTimeLeft = (Text*)btn->getChildByName("lblTimeLeft");
        lblTimeLeft->setVisible(!isWoodChestGachaFreeReady);
        lblTimeLeft->setString(strmake("%s: %s", LM->getText("next free pick").c_str(), GM->getTimeLeftInStringHMS(difftime(nextWoodChestGachaFreeTimeT, now)).c_str()));
        
        btn = (Button*)layer->getChildByName("btnGacha1");
        btn->getChildByName("imgRedDot")->setVisible(isGoldChestGachaFreeReady);
        lbl = (Text*)btn->getChildByName("lbl");
        img = (ImageView*)btn->getChildByName("imgIcon");
        if(GM->getGoldKey() > 0){
            lbl->setString(Value(GM->getGoldKey()).asString());
            img->loadTexture("keyGold.png");
            img->setContentSize(Size(31, 60));
        }else{
            lbl->setString(isGoldChestGachaFreeReady?"FREE":"300");
            img->loadTexture("gem.png");
            img->setContentSize(Size(97, 78));
        }
        
        lblTimeLeft = (Text*)btn->getChildByName("lblTimeLeft");
        lblTimeLeft->setVisible(!isGoldChestGachaFreeReady);
        lblTimeLeft->setString(strmake("%s: %s", LM->getText("next free pick").c_str(), GM->getTimeLeftInStringHMS(difftime(nextGoldChestGachaFreeTimeT, now)).c_str()));
    }
    if(GM->iapFlag == IAP_FLAG_SUCCESS){
        GM->consumeSkuNameList.push_back(GM->iapDetail);
        log("consumeSkuNameList push_back: %d", (int)GM->consumeSkuNameList.size());
    }
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
        if (GM->iapDetail.compare(IAP_DETAIL_STARTER_KEY) == 0) {
            GM->addGem(660);
            GM->addWoodKey(5);
            GM->addGoldKey(5);
            iapPoint += 10000;
        }
        std::string iapList = UDGetStr(KEY_IAP_LIST, "");
        iapList += GM->iapDetail + "|";
        UDSetStr(KEY_IAP_LIST, iapList);
        UDSetInt(KEY_IAP_TOTAL, iapPoint);
        
        datas.push_back(DATA_TYPE_KEYS);
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
                std::string gpsID = "";
                gpsID += GM->playerIDWithGPSID;
                UDSetStr(KEY_SAVED_ID, gpsID);
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
        }else if(GM->iapDetail.compare(IAP_DETAIL_CHAPTER3) == 0){
            UDSetBool(KEY_CHAPTER_3_PURCHASED, true);
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
}
int Title::getMaxGold(int level){
    string str = GM->castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(0)).asInt();
    }else{
        return 400000;
    }
}
int Title::getMaxGold(){
    return getMaxGold(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int Title::getMaxTree(){
    return getMaxTree(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int Title::getMaxTree(int level){
    string str = GM->castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if(str.length() > 0){
        return Value(GM->split(str, "_").at(1)).asInt();
    }else{
        return 600000;
    }
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
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
    
//    int timeInSec = (int)(BSM->getCurrentTimeT());
    
    int timeLeft = BSM->getTimeLeftToNewDay();
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
        std::string value = "";
        value += BSM->getStrFromTime(BSM->getCurrentTimeT());
        UDSetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, value);
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
//        int timeInSec = (int)(BSM->getCurrentTime());
//        int timeLeft = 60*60*24 - timeInSec%(60*60*24);
        int timeLeft = BSM->getTimeLeftToNewDay();
        Node* layer = getChildByName("colosseum");
        Text* lbl = (Text*)layer->getChildByName("lblLeagueTimeLeft");
        lbl->setString(GM->getTimeLeftInString(timeLeft));
        
        std::string strLastPlayTime = UDGetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, "");
        time_t lastPlayTimeT = BSM->getTimeTFromStr(strLastPlayTime);
        time_t now = BSM->getCurrentTimeT();
        if(difftime(now, lastPlayTimeT) < 0){
            std::string value = "";
            value += BSM->getStrFromTime(now);
            UDSetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, value);
            lastPlayTimeT = now;
        }
        int cooltime = 60*60;
        timeLeft = cooltime - (difftime(now, lastPlayTimeT));//60*60*24 - timeInSec%(60*60*24);
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
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
    int day = (int)(BSM->getCurrentTimeT()/(60*60*24)) + 1;
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
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    
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
