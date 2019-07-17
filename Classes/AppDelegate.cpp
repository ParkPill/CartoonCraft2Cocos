#include "cocos2d.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"

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
#include "ServerManager.h"
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
        glview = GLViewImpl::createWithRect("Cartoon Craft", Rect(0, 0, 1334, 750));
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
    FileUtils::getInstance()->addSearchPath("sounds");
    FileUtils::getInstance()->addSearchPath("useMapSetting");
    FileUtils::getInstance()->addSearchPath("Logo");
    FileUtils::getInstance()->addSearchPath("tilemap");
    
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("effect.plist");
    cache->addSpriteFramesWithFile("cartoonCraft.plist");
    cache->addSpriteFramesWithFile("cartoonCraftBuilding.plist");
    cache->addSpriteFramesWithFile("cartoonCraftEffect.plist");
    cache->addSpriteFramesWithFile("CartoonCraftNumber.plist");
    
    AnimationCache::getInstance()->addAnimationsWithFile("animations.plist");
    
    /*
     log("init GameManagerInit done");
     
     log("time request ");*/
    
    // turn on display FPS
    director->setDisplayStats(false);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 50);
    
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
//    Size frameSize = glview->getFrameSize();
//    if (frameSize.height/frameSize.width > designResolutionSize.height/designResolutionSize.width){
//        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
//    }else{
//        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
//    }
    /*Size frameSize = glview->getFrameSize();
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
    GameManager::getInstance()->market = MARKET_PLAYSTORE_PAID;
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
    
    ServerManager::getInstance()->getHttpTime();
    GM->loadBattleData();
    GameSharing::init(); // for ios
    
//    log("about to init GameManager");
//    GameManager::getInstance()->initGameManager();
    
//    log("creating scene");
//    GM->nextScene = STAGE_FIELD;
//    GM->isPremiumRetry = true; // test
//    GM->isColosseum = true; // test
//    auto scene = HelloWorld::scene(4, false); // test
//    WORLD->addGold(15000);WORLD->addLumber(10000); // test
    
//    GM->nextScene = STAGE_INTRO;
//    auto scene = HelloWorld::scene(5, false); // test


//    auto scene = EditorWorld::scene(0, false);

//    auto scene = Scene::create();
//    scene->addChild(Title::create());
//
    // title
//    Scene* scene = Scene::create();
//    scene->addChild(Title::create());
    // title end
    
    
    int startX = 0;
    int max=0;
    int x=startX;
    int y=max;
    int direction = -1;//0:E 1:S 2:W 3:N
    bool changeDirection = false;
    for(int i = 0; i < 100; i++){
        log("%d, %d", x, y);
        
        if (direction < 0 || (x == 0 && y == max)) {
            y++;
            x++;
            max++;
            if(direction < 0){
                direction = 0;
            }
        }else if(direction == 0){
            x++;
            if(x > max){
                changeDirection = true;
            }
        }else if(direction == 1){
            y--;
            if(y < -max){
                changeDirection = true;
            }
        }else if(direction == 2){
            x--;
            if(x < -max){
                changeDirection = true;
            }
        }else if(direction == 3){
            y++;
            if(y > max){
                changeDirection = true;
            }
        }
        if(changeDirection){
            changeDirection = false;
            if(direction == 0){
                x--;
                y--;
            }else if(direction == 1){
                x--;
                y++;
            }else if(direction == 2){
                x++;
                y++;
            }else if(direction == 3){
                x++;
                y--;
            }
            direction++;
            if(direction>3){
                direction = 0;
            }
        }
    }
    
    GM->nextScene = STAGE_FIELD; // test
    auto scene = HelloWorld::scene(26, false); // test now
//    GM->nextScene = STAGE_LOBBY; // test
//    auto scene = HelloWorld::scene(GM->nextScene, false); // test
//    GM->nextScene = STAGE_SINGLEPLAY; // test
//    GM->singlePlayStageIndex = 0;
//    auto scene = HelloWorld::scene(GM->nextScene, false); // test
    // intro for normal start
    GM->version = "2.41";
//    auto scene = Scene::create();
//    Intro* intro = Intro::create();
//    scene->addChild(intro);
    // intro end

//    scene->addChild(FifteenSixLogo::create());
//    scene->addChild(WorldMap::create());
//    scene->addChild(PanZoomLayer::create());
//    scene->addChild(Title::create());
//    Scene* sceneTitle = Scene::create();
//    sceneTitle->addChild(HelloWorld::scene(STAGE_LOBBY, false));
    
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
        BHUD->checkUnsaved();
    }
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
//#ifdef SDKBOX_ENABLED
//    sdkbox::sessionStart();
//#endif
    Director::getInstance()->startAnimation();
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

