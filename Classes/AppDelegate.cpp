#include "cocos2d.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "MultiplayManager.h"
#include "HeroPage.h"
//#include "HelloWorldAstar.h"
//#include "BattleLobby.h" // test
//#ifdef SDKBOX_ENABLED
//#include "PluginUnityAds/PluginUnityAds.h"
//#endif
//#ifdef SDKBOX_ENABLED
//#include "PluginSdkboxAds/PluginSdkboxAds.h"
//#endif
#include "EditorWorld.h"
#include "Title.h"
//#ifdef SDKBOX_ENABLED
//#include "PluginFacebook/PluginFacebook.h"
//#endif
//#ifdef SDKBOX_ENABLED
//#include "Sdkbox/Sdkbox.h"
//#endif
#include "GameManager.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
#include "Intro.h"
USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(2368, 1334);
static cocos2d::Size smallResolutionSize = cocos2d::Size(2368, 1334);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(2368, 1334);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2368, 1334);
//static cocos2d::Size designResolutionSize = cocos2d::Size(890, 1402);
//static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
//static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
//static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    log("AppDelegate start");
#ifdef SDKBOX_ENABLED
//    sdkbox::PluginUnityAds::init();
//    sdkbox::PluginUnityAds::setListener(this);
#endif
#ifdef SDKBOX_ENABLED
    sdkbox::PluginSdkboxAds::init();
#endif
#ifdef SDKBOX_ENABLED
    log("PluginSdkboxPlay init");
    sdkbox::PluginSdkboxPlay::init();
    sdkbox::PluginSdkboxPlay::setListener(this);
    log("PluginSdkboxPlay init end");
//    sdkbox::PluginSdkboxPlay::signin();
//    log("PluginSdkboxPlay signin");
#endif
#ifdef SDKBOX_ENABLED
    sdkbox::IAP::init();
    sdkbox::IAP::setListener(this);
#endif
    
//#ifdef SDKBOX_ENABLED
//    sdkbox::PluginUnityAds::init();
//    sdkbox::PluginSdkboxAds::init();
//#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_ANDROID
    
//#ifdef SDKBOX_ENABLED
//    sdkbox::IAP::init();
//#endif
//#ifdef SDKBOX_ENABLED
////    sdkbox::PluginFacebook::init();
//#endif

#endif
    // initialize director
    auto director = Director::getInstance();
    log("%dx%d", (int)director->getWinSize().width, (int)director->getWinSize().height);
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
//        glview = GLViewImpl::createWithRect("Cartoon Craft", cocos2d::Rect(0, 0, 1334, 750));
        float rate = 0.8f;
//        glview = GLViewImpl::createWithRect("Cartoon Craft", cocos2d::Rect(0, 0, 1500*rate, 750*rate));
        glview = GLViewImpl::createWithRect("Cartoon Craft", cocos2d::Rect(0, 0, 1024*rate, 768*rate));
//        glview = GLViewImpl::createWithRect("Cartoon Craft", cocos2d::Rect(0, 0, 1500*rate, 750*rate));
//        glview = GLViewImpl::createWithRect("Cartoon Craft", cocos2d::Rect(0, 0, 1024, 768));
//        glview = GLViewImpl::createWithFullScreen("Cartoon Craft");
//        glview = GLViewImpl::create("Cartoon Craft");
#else
        glview = GLViewImpl::create("Cartoon Craft");
#endif
        director->setOpenGLView(glview);
    }
    
//    for (int i = 0; i < 6; i++) {
//        cache->addSpriteFrame(Sprite::create(__String::createWithFormat("laserThinBlueHit%d.png", i)->getCString())->getSpriteFrame(), __String::createWithFormat("laserThinBlueHit%d.png", i)->getCString());
//    }
    
    //    CCFileUtils::getInstance()->addSearchPath("animations");
    FileUtils::getInstance()->addSearchPath("UI");
    FileUtils::getInstance()->addSearchPath("Language");
    FileUtils::getInstance()->addSearchPath("fonts");
    FileUtils::getInstance()->addSearchPath("res");
    FileUtils::getInstance()->addSearchPath("spine");
    FileUtils::getInstance()->addSearchPath("crossPromotion");
    FileUtils::getInstance()->addSearchPath("sounds");
    FileUtils::getInstance()->addSearchPath("useMapSetting");
    FileUtils::getInstance()->addSearchPath("Logo");
    FileUtils::getInstance()->addSearchPath("tilemap");
    
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("CartoonCraftNV.plist");
    cache->addSpriteFramesWithFile("effect.plist");
//    cache->addSpriteFramesWithFile("cartoonCraft.plist");
//    cache->addSpriteFramesWithFile("cartoonCraftBuilding.plist");
//    cache->addSpriteFramesWithFile("cartoonCraftEffect.plist");
    cache->addSpriteFramesWithFile("CartoonCraftNumber.plist");
    
    AnimationCache::getInstance()->addAnimationsWithFile("animations.plist");
    
    /*
     log("init GameManagerInit done");
     
     log("time request ");*/
    
    // turn on display FPS
//    director->setDisplayStats(false); // test 
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    cocos2d::Size frameSize = glview->getFrameSize();
    if (frameSize.height/frameSize.width > designResolutionSize.height/designResolutionSize.width){
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
    }else{
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    }
    /*cocos2d::Size frameSize = glview->getFrameSize();
     // if the frame's height is larger than the height of medium size.
     if (frameSize.height > mediumResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is larger than the height of small size.
     else if (frameSize.height > smallResolutionSize.height)
     {
     director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
     }
     // if the frame's height is smaller than the height of medium size.
     else
     {
     director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
     }*/
    
    register_all_packages();
    
    GameManager::getInstance()->originalSize = director->getWinSize();
    GameManager::getInstance()->showVPad = false;
    GameManager::getInstance()->totalThemeCount = 4;
    GameManager::getInstance()->totalStage = 24*4;
    GameManager::getInstance()->developer = false; // test
    LanguageManager::getInstance()->loadLanguageSheet();
    
    if (UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        UserDefault::getInstance()->setBoolForKey(KEY_DOUBLE_EXP_GET, true);
        UserDefault::getInstance()->setBoolForKey(KEY_REMOVE_ADS_GET, true);
    }
    
    Director::getInstance()->setProjection(Director::Projection::_2D);
    
    if (GameManager::getInstance()->market == MARKET_MAC ||
        GameManager::getInstance()->market == MARKET_WINDOWS ||
        GameManager::getInstance()->market == MARKET_FUNBOX ||
        GameManager::getInstance()->market == MARKET_OUYA) {
        GameManager::getInstance()->isUsingController = true;
    }
    //    NativeInterface::NativeInterfacedestroyAds();
    //    NativeInterface::NativeInterfacedestroyRectAds();
    Director::getInstance()->setDepthTest(false);
//    GameManager::getInstance()->firstPlayed = true;
    
    srand((uint)time(NULL));
    
//    BSM->getHttpTime();
    
//    BSM->getAllUserData();
    GM->loadBattleData();
    GameSharing::init(); // for ios
    
//    log("about to init GameManager");
//    GameManager::getInstance()->initGameManager();
    
//    log("creating scene");
//    GM->nextScene = STAGE_FIELD;
//    GM->isPremiumRetry = true; // test
//    GM->isColosseum = true; // test
    
//    WORLD->addGold(15000);WORLD->addLumber(10000); // test
    
//    GM->nextScene = STAGE_INTRO;
//    auto scene = HelloWorld::scene(5, false); // test



//    auto scene = Scene::create();
//    scene->addChild(Title::create());
//
    // title
//    Scene* scene = Scene::create();
//    scene->addChild(Title::create());
    // title end
    
//    GM->nextScene = STAGE_FIELD; // test
//    auto scene = HelloWorld::scene(36, GAME_MODE_NORMAL); // test 
//    BSM->pvpTargetData = "_60/401/8/9_61/401/9/9_62/401/10/9_63/401/11/9_52/100/12/9_53/100/13/9_57/100/14/9_57/100/15/9_55/100/15/6_50/0/17/9_64/401/18/9_54/200/19/9_";
//    BSM->pvpTargetName ="박성필 테스트";
//    WORLD->setPvpMode(12);
//    GM->nextScene = STAGE_LOBBY; // test

//    auto scene = HelloWorld::scene(36, false); // test
//    GM->nextScene = STAGE_SINGLEPLAY; // test
//    GM->singlePlayStageIndex = 0;
//    auto scene = HelloWorld::scene(GM->nextScene, false); // test
    // intro for normal start
    GM->market = MARKET_PAID;
    GM->versionCode = 314;
    GM->version = "4.26";
    log("start version code: %d", GM->versionCode);
//    auto scene = HelloWorld::scene(7, DIFFICULTY_MODE_NORMAL); // test
//    auto scene = HelloWorld::scene(4, DIFFICULTY_MODE_NORMAL); // test
//    auto scene = EditorWorld::scene(0, false); // test
    auto scene = Scene::create();
    Intro* intro = Intro::create();
    scene->addChild(intro);
    // intro end

    if (GM->market == MARKET_SMARTPASS) {
        UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::JAPANESE);
    }

    std::string strName = "";
    for (int i = 0; i < 15; i++) {
        strName += (char)(rand()%10 + (int)'0');
    }
    
    log("strName: %s", strName.c_str());
    log("strName: %s", strName.c_str());
//    scene->addChild(FifteenSixLogo::create());
//    scene->addChild(WorldMap::create());
//    scene->addChild(PanZoomLayer::create());
//    scene->addChild(Title::create());
//    Scene* sceneTitle = Scene::create();
//    sceneTitle->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    
//    std::string savedRID = "5c9dae8b385e48337a34dc5b";
//    std::string data = "score="+Value(1100).asString() + "&name=tester1";
//    std::string arenaRID = "";
//    if(arenaRID.length() > 0){
//        data += "&arenarid=" + arenaRID;
//    }
//    BSM->sendPost("arnsv", "_id=" + savedRID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
//    
//    savedRID = "5db588363a61a02621ef2602";
//    data = "score="+Value(1200).asString() + "&name=tester2";
//    arenaRID = "5db588413a61a02621ef2603";
//    if(arenaRID.length() > 0){
//        data += "&arenarid=" + arenaRID;
//    }
//    BSM->sendPost("arnsv", "_id=" + savedRID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
//    
//    savedRID = "5c9dae4e385e48337a34dc5a";
//    data = "score="+Value(1300).asString() + "&name=tester3";
//    BSM->sendPost("arnsv", "_id=" + savedRID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
    
    
    log("time: %s", BSM->getStrFromTime(-12342352).c_str());
    
    // run
    director->runWithScene(scene);
//    WORLD->addGold(1000); // test
//    WORLD->addLumber(1000); // test
    return true;
}
// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
//#ifdef SDKBOX_ENABLED
//    sdkbox::sessionEnd();
//#endif
    log("background entered");
    Director::getInstance()->stopAnimation();

    if(BHUD && GM->nextScene == STAGE_LOBBY){
//        BHUD->saveInventoryAndDeck();
//        BHUD->saveBuildings();
//        auto scene = Scene::create();
//        Intro* intro = Intro::create();
//        scene->addChild(intro);
//        
//        Director::getInstance()->replaceScene(scene);
//        BHUD->checkUnsaved();
    }
    if (TITLE == nullptr && WORLD && GM->nextScene == STAGE_FIELD) {
        HUD->onMenuClick();
    }
    if (GM->titleLayer && GM->titleLayer != nullptr) {
        TITLE->onEnterBackground();
    }
    
    if (HEROPAGE) {
        HEROPAGE->checkChangesForHeroes();
    }
    
    if (WORLD && WORLD->isMultiplay) {
        backgroundEnterTime = BSM->getCurrentTimeT();
//        MM->paused();
    }
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
//    if (WORLD && WORLD->isMultiplay) {
//        time_t current = BSM->getCurrentTimeT();
//        if (current - backgroundEnterTime > 5000) {
//            WORLD->gameTimer += (float)(current - backgroundEnterTime)*0.001f;
//            MM->disconnect();
//            WORLD->endGame(false);
//        }
//    }
//#ifdef SDKBOX_ENABLED
//    sdkbox::sessionStart();
//#endif
    Director::getInstance()->startAnimation();
//    BSM->getHttpTime();
//    BSM->checkServer();
//    if(BHUD){
//        BHUD->getLastAccessTime();
//    }
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

#ifdef SDKBOX_ENABLED
//void AppDelegate::adViewDidReceiveAd ( const std::string & name ) {
//    if(name.compare("bottom_banner") == 0){
//        if (!UDGetBool(KEY_ADS_SUPERPASS, false)) {
//            sdkbox::PluginAdMob::show(name);
//        }
//    }
//}
//void AppDelegate::adViewDidFailToReceiveAdWithError ( const std::string & name ,
//                                                     const std::string & msg ) {
//    log("admob load error: %s", msg.c_str());
//}
//void AppDelegate::adViewWillPresentScreen ( const std::string & name ) {
//
//}
//void AppDelegate::adViewDidDismissScreen ( const std::string & name ) {
//
//}
//void AppDelegate::adViewWillDismissScreen ( const std::string & name ) {
//
//}
//void AppDelegate::adViewWillLeaveApplication ( const std::string & name ) {
//
//}
void AppDelegate::onInitialized(bool ok){
    
}
void AppDelegate::onSuccess(sdkbox::Product const& p) {
//    if (Application::getInstance()->getTargetPlatform() == Application::Platform::OS_IPHONE ||
//        Application::getInstance()->getTargetPlatform() == Application::Platform::OS_IPAD) {
//        SM->proveReceipt(p.id, p.receiptCipheredPayload);
//    }else{
//        SM->proveReceipt(p.id, p.receipt);
//    }
    log("iap onSuccess");
    
    if(p.name.compare("golden_ticket") == 0){
        TITLE->iapFlag = IAP_FLAG_TICKET;
    }
}
void AppDelegate::onFailure(sdkbox::Product const& p, const std::string &msg) {
    TITLE->iapFlag = IAP_FLAG_FAILED;
    log("iap onFailure");
}
void AppDelegate::onCanceled(sdkbox::Product const& p) {
    TITLE->iapFlag = IAP_FLAG_FAILED;
    log("iap onCanceled");
}
void AppDelegate::onRestored(sdkbox::Product const& p) {
//    TITLE->iapFlag = IAP_RESTORED;
//    HudLayer* hud = GM->getHudLayer();
    if(p.name.compare("buggysuperpass5") == 0){
//        UD->setBoolForKey(KEY_SUPER_PASS_ON, false);
//        UD->setBoolForKey(KEY_SUPER_PASS_UNLOCKED, true);
//        hud->iapFlag = IAP_SUPERPASS_PURCHASED;
    }else if(p.name.compare("buggymaterial2") == 0){
        
//        GM->getHudLayer()->iapFlag = IAP_SUPERPASS_METERIAL;
    }
}
void AppDelegate::onProductRequestSuccess(std::vector<sdkbox::Product> const &products) {
    log("iap onProductRequestSuccess");
}
void AppDelegate::onProductRequestFailure(const std::string &msg) {
    log("iap onProductRequestFailure");
}
void AppDelegate::onRestoreComplete(bool ok, const std::string &msg) {
    
}

void AppDelegate::unityAdsDidClick(const std::string& placementId) {
    CCLOG("unityads click %s", placementId.c_str());
}

void AppDelegate::unityAdsPlacementStateChanged(const std::string& placementId,
                                                PluginUnityAds::SBUnityAdsPlacementState oldState,
                                                PluginUnityAds::SBUnityAdsPlacementState newState) {
    CCLOG("unityads state change %s %d->%d", placementId.c_str(), oldState, newState);
}

void AppDelegate::unityAdsReady(const std::string& placementId) {
    CCLOG("unityads ready %s", placementId.c_str());
}

void AppDelegate::unityAdsDidError(sdkbox::PluginUnityAds::SBUnityAdsError error, const std::string& message) {
    CCLOG("unityads error %d %s", error, message.c_str());
    GM->showVideoFailed();
}

void AppDelegate::unityAdsDidStart(const std::string& placementId) {
    CCLOG("unityads start %s", placementId.c_str());
}

void AppDelegate::unityAdsDidFinish(const std::string& placementId, sdkbox::PluginUnityAds::SBUnityAdsFinishState state) {
    CCLOG("unityads finish %d %s", state, placementId.c_str());
    if(state == sdkbox::PluginUnityAds::SBUnityAdsFinishState::kUnityAdsFinishStateCompleted){
        GM->showVideoDone();
    }else{
        GM->showVideoFailed();
    }
}


void AppDelegate::onConnectionStatusChanged( int status ){
    log("sdkbox play state: %d", status);
    if(status == GPS_CONNECTED){
        std::string playerID = sdkbox::PluginSdkboxPlay::getPlayerId();
        log("sdkbox play id: %s", playerID.c_str());

//        SM->checkID(playerID);
    }
}
void AppDelegate::onScoreSubmitted( const std::string& leaderboard_name, long score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday ){

}
void AppDelegate::onIncrementalAchievementUnlocked( const std::string& achievement_name ){

}
void AppDelegate::onIncrementalAchievementStep( const std::string& achievement_name, double step ){

}
void AppDelegate::onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked ){

}




#endif

