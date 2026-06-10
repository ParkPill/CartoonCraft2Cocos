
#include "GameManager.h"
#include "LanguageManager.h"
// #include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "Location_Node_Class.h"
#include "Node_Class.h"
#include "BuggyServerManager.h"

#include <algorithm>
#include <cctype>
#include <locale>
// #include "StageClearLayer.h"
// #include "StageSelectScene.h"
// #include "ThemeSelectScene.h"
// #include "GameStartLayer.h"
// #ifdef SDKBOX_ENABLED
// #include "PluginUnityAds/PluginUnityAds.h"
// #endif
#include "HeroPage.h"
#include "Title.h"
#include "PageBase.h"
#include "EditorHud.h"
#include "HudLayer.h"
#include "editor-support/cocostudio/CocoStudio.h"

using namespace cocostudio;

#include "Movable.h"

static void registrationHostShowMessage(Node* host, std::string msg)
{
    if (host == nullptr) {
        return;
    }
    if (PageBase* page = dynamic_cast<PageBase*>(host)) {
        page->showInstanceMessage(msg);
    } else if (HudLayer* hud = dynamic_cast<HudLayer*>(host)) {
        hud->showInstanceMessage(msg);
    }
}

static void registrationHostShowIndicator(Node* host)
{
    if (host == nullptr) {
        return;
    }
    if (PageBase* page = dynamic_cast<PageBase*>(host)) {
        page->showIndicator();
    } else if (HudLayer* hud = dynamic_cast<HudLayer*>(host)) {
        hud->showIndicator();
    }
}

static void registrationHostHideIndicator(Node* host)
{
    if (host == nullptr) {
        return;
    }
    if (PageBase* page = dynamic_cast<PageBase*>(host)) {
        page->hideIndicator();
    } else if (HudLayer* hud = dynamic_cast<HudLayer*>(host)) {
        hud->hideIndicator();
    }
}

static void registrationHostCloseNamePopup(Node* host)
{
    if (host == nullptr) {
        return;
    }
    Node* popup = host->getChildByName("namePopup");
    if (popup != nullptr) {
        popup->removeFromParent();
    }
    if (PageBase* page = dynamic_cast<PageBase*>(host)) {
        page->closePopup();
    }
}

// #include "NativeInterface.h"
using namespace cocos2d;
using namespace experimental;
// using namespace CocosDenshion;

GameManager *GameManager::m_mySingleton = NULL;

bool GameManager::isWin32DeferredAuth()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    return true;
#else
    return false;
#endif
}

bool GameManager::isWin32Offline()
{
    return isWin32DeferredAuth();
}

void GameManager::ensureWin32StartupDefaults()
{
    if (!isWin32DeferredAuth()) {
        return;
    }

    if (UDGetInt(KEY_SERVER, -1) < 0) {
        UDSetInt(KEY_SERVER, 0);
    }

    std::string id = UDGetStr(KEY_SAVED_ID, "-1");
    BuggyServerManager *bsm = BuggyServerManager::getInstance();
    bsm->requestedID = id;
    bsm->isOffline = false;
    bsm->getLocalTime = false;
    bsm->establishLocalTimeBaseline();
    bsm->getHttpTime();
}

void GameManager::ensureWin32LocalAccount()
{
    ensureWin32StartupDefaults();
}

bool GameManager::hasLocalAccount()
{
    return UDGetStr(KEY_SAVED_ID, "-1").compare("-1") != 0;
}

void GameManager::requestOnlineAccount(Node* host, int pendingAction)
{
    pendingOnlineAction = pendingAction;
    accountRegistrationNode = host;

    if (hasLocalAccount()) {
        BSM->requestedID = UDGetStr(KEY_SAVED_ID, "-1");
        fulfillPendingOnlineAction();
        return;
    }

    deferredAccountRegistrationActive = true;
    deferredNameHandleState = NETWORK_HANDLE_STATE_NOT_READY;

    if (TITLE != nullptr) {
        TITLE->showRegisterName();
    } else if (host != nullptr) {
        showAccountRegistrationOnNode(host);
    }
}

void GameManager::showAccountRegistrationOnNode(Node* host)
{
    if (host == nullptr || host->getChildByName("namePopup") != nullptr) {
        return;
    }

    Size hostSize = Director::getInstance()->getWinSize();
    Node* layer = CSLoader::createNode("RegisterName.csb");
    host->addChild(layer, POPUP_ZORDER);
    layer->setName("namePopup");
    layer->setPositionX(hostSize.width / 2 - layer->getContentSize().width / 2);
    layer->setPositionY(hostSize.height / 2 - layer->getContentSize().height / 2);
    layer->getChildByName("btnBlock")->setScale(2);

    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->setVisible(false);

    Node* background = layer->getChildByName("imgBackground");
    Text* lbl = (Text*)background->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "input name");

    TextField* tf = (TextField*)background->getChildByName("tfName");
    tf->setPlaceHolder(LM->getText("name"));

    btn = (Button*)background->getChildByName("btnOk");
    btn->addClickEventListener([host](Ref*) {
        TextField* nameField = (TextField*)host->getChildByName("namePopup")
                                   ->getChildByName("imgBackground")
                                   ->getChildByName("tfName");
        if (nameField->getString().size() <= 0) {
            registrationHostShowMessage(host, LM->getText("input name"));
            return;
        }
        if (nameField->getString().find(',') != std::string::npos ||
            nameField->getString().find('_') != std::string::npos ||
            nameField->getString().find('/') != std::string::npos) {
            registrationHostShowMessage(
                host, strmake("%s ( _ , / )",
                              LM->getText("do not use special character").c_str()));
            return;
        }
        registrationHostShowIndicator(host);
        GM->deferredNameToRegister = nameField->getString();
        GM->deferredNameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
        BSM->registerName(nameField->getString());
    });

    btn = (Button*)background->getChildByName("btnRandom");
    btn->addClickEventListener([host](Ref*) {
        registrationHostShowIndicator(host);
        std::string strName = "";
        srand((unsigned int)time(0));
        for (int i = 0; i < 15; i++) {
            int index = rand() % 3;
            if (index == 0) {
                strName += (char)(rand() % 26 + (int)'A');
            } else if (index == 1) {
                strName += (char)(rand() % 26 + (int)'a');
            } else {
                strName += (char)(rand() % 10 + (int)'0');
            }
        }
        GM->deferredNameToRegister = strName;
        GM->deferredNameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
        BSM->registerName(strName);
    });

    btn = (Button*)background->getChildByName("btnApple");
    btn->setVisible(false);

    if (PageBase* page = dynamic_cast<PageBase*>(host)) {
        page->setAsPopup(layer);
    }
}

void GameManager::completeDeferredAccountRegistration(bool success)
{
    deferredAccountRegistrationActive = false;
    accountRegistrationNode = nullptr;
    deferredNameHandleState = NETWORK_HANDLE_STATE_COMPLETE;

    if (success) {
        BSM->requestedID = UDGetStr(KEY_SAVED_ID, "-1");
        fulfillPendingOnlineAction();
    } else {
        pendingOnlineAction = PENDING_ONLINE_ACTION_NONE;
        if (HUD) {
            HUD->hideIndicator();
        }
        if (EHUD) {
            EHUD->hideIndicator();
        }
    }
}

void GameManager::fulfillPendingOnlineAction()
{
    int action = pendingOnlineAction;
    pendingOnlineAction = PENDING_ONLINE_ACTION_NONE;

    switch (action) {
    case PENDING_ONLINE_ACTION_ARENA_SCORE:
        if (pendingArenaScore > 60) {
            BSM->sendArenaScore(pendingArenaScore);
            UDSetInt(KEY_COLOSSEUM_SCORE, pendingArenaScore);
        } else if (HUD) {
            HUD->hideIndicator();
        }
        pendingArenaScore = 0;
        break;
    case PENDING_ONLINE_ACTION_MAP_UPLOAD:
        if (pendingMapUploadName.length() > 0 && pendingMapUploadData.length() > 0 && EHUD) {
            EHUD->uploadHandleState = NETWORK_HANDLE_STATE_REQUESTED;
            EHUD->isUploadRequested = true;
            BSM->uploadMap(pendingMapUploadName, pendingMapUploadData);
        } else if (EHUD) {
            EHUD->hideIndicator();
        }
        pendingMapUploadName = "";
        pendingMapUploadData = "";
        break;
    case PENDING_ONLINE_ACTION_MAP_LIKE:
        if (pendingMapLikeMapId.length() > 0) {
            BSM->uploadCustomMapResult(pendingMapLikeMapId, pendingMapLike, pendingMapDislike,
                                       pendingMapSuccess, pendingMapFail);
        }
        pendingMapLikeMapId = "";
        break;
    default:
        break;
    }
}

void GameManager::tickDeferredAccountRegistration(Node* host)
{
    if (!deferredAccountRegistrationActive || accountRegistrationNode != host) {
        return;
    }

    if (deferredNameHandleState == NETWORK_HANDLE_STATE_ARRIVED) {
        registrationHostHideIndicator(host);
        if (BSM->resultMessage.compare("same name exist") == 0) {
            registrationHostShowMessage(host, LM->getText("same name exist"));
            deferredNameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
        } else if (BSM->resultMessage.compare("save failed") == 0 ||
                   BSM->resultMessage.compare("database failure") == 0) {
            registrationHostShowMessage(host, LM->getText("network retry"));
            deferredNameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
        } else {
            if (deferredNameToRegister.length() > 0) {
                UDSetStr(KEY_NAME, deferredNameToRegister);
            }
            deferredNameHandleState = NETWORK_HANDLE_STATE_HANDLED;
        }
    }

    if (deferredNameHandleState == NETWORK_HANDLE_STATE_HANDLED) {
        deferredNameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        registrationHostCloseNamePopup(host);
        completeDeferredAccountRegistration(true);
    }
}

GameManager::GameManager()
{
    astar = AStar::create();
    this->addChild(astar);
    size = Director::getInstance()->getVisibleSize();
    life = UserDefault::getInstance()->getIntegerForKey(KEY_LIFE, 5);
    s0 = (char)0;
    s1 = (char)0;
    s2 = (char)0;
    s3 = (char)0;
    s4 = (char)0;

    g0 = (char)0;
    g1 = (char)0;
    g2 = (char)0;
    g3 = (char)0;
    g4 = (char)0;
    g5 = (char)0;
    g6 = (char)0;

    c0 = (char)0;
    c1 = (char)0;
    c2 = (char)0;
    c3 = (char)0;
    c4 = (char)0;
    c5 = (char)0;
    c6 = (char)0;

    isInMiddleOfGame = false;
    isGoogleSigned = false;
    isPaidGame = false;
    gameStarted = false;
    isUsingController = false;

    appAlreadyLaunched = false;
    //    pushedLayer = NULL;
    initComplete = false;

    totalThemeCount = 7;
    pauseLayer = NULL;

    isGuestPlay = false;

    optionLayer = NULL;
    gameOverLayer = NULL;
    gameOverScene = NULL;
    currentStageLayer = NULL;
    achievementLayer = NULL;
    titleLayer = NULL;

    jumpPressed = false;
    cPressed = false;
    firePressed = false;
    leftPressed = false;
    rightPressed = false;
    downPressed = false;
    upPressed = false;
    paidUser = UserDefault::getInstance()->getBoolForKey(KEY_IAP_USER, false);
    settingLayer = NULL;

    isVideoRewardAttack = false;
    isVideoRewardEnergy = false;
    isVideoRewardCoins = false;
    //    originallyDisabledMenusWhenLayerPushed = NULL;

    myPhoto = NULL;

    hudLayer = NULL;

    //    priceArray = __Dictionary::create();
    //    priceArray->retain();
    CCLOG("GameManager almost created");
    initGameManager();
    CCLOG("GameManager created");
}
void GameManager::initGameManager()
{
    preLoadAllSoundEffect();
    float musicVolumn = UserDefault::getInstance()->getFloatForKey(KEY_MUSIC_VOLUMN, 0.3f);
    setMusicVolumn(musicVolumn);
    float soundVolumn = UserDefault::getInstance()->getFloatForKey(KEY_SOUND_VOLUMN, 1);
    setSoundVolumn(soundVolumn);

    //    log("init game music: %f, sound: %f", CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume(), CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume());

    initAchievement();
}
int GameManager::getGemForCoin(int coinCount)
{
    if (coinCount / 100 == coinCount / 100.0f)
    {
        return coinCount / 100;
    }
    else
    {
        return coinCount / 100 + 1;
    }
}
void GameManager::setFontName(Label *lbl, const char *name, float fontSize)
{
    if (FileUtils::getInstance()->isFileExist(name))
    {
        TTFConfig config = lbl->getTTFConfig();
        config.fontFilePath = name;
        config.fontSize = fontSize;
        lbl->setTTFConfig(config);
    }
    else
    {
        lbl->setSystemFontName(name);
    }
}
ValueVector GameManager::split(const std::string &str, const std::string &delim)
{
    ValueVector res;
    size_t current = 0, found, quote;
    bool lastQuoteShouldBeRemoved = false;
    while ((found = str.find_first_of(delim, current)) != std::string::npos)
    {
        if (delim.compare(",") == 0)
        {
            if ((quote = str.find_first_of("\"", current)) != std::string::npos && current >= quote)
            {
                if (current == quote)
                {
                    current++;
                }
                if ((quote = str.find_first_of("\"", quote + 1)) != std::string::npos)
                {
                    found = str.find_first_of(delim, quote);
                    if (found == quote + 1)
                    {
                        lastQuoteShouldBeRemoved = true;
                    }
                }
            }
        }
        if (lastQuoteShouldBeRemoved)
        {
            res.push_back(Value(std::string(str, current, found - current - 1)));
            current = found + delim.size();
        }
        else
        {
            res.push_back(Value(std::string(str, current, found - current)));
            current = found + delim.size();
        }

        bool isLast = found == std::string::npos;
        if (isLast)
        {
            return res;
        }
    }
    res.push_back(Value(std::string(str, current, str.size() - current)));
    return res;
}
const char *GameManager::getFont(int font)
{
    if (font == FONT_DEFAULT)
    {
        if (LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN)
        {
            return "NotoSansCJKjp-Bold.otf";
        }
        else if (LanguageManager::getInstance()->getLanguageType() == LanguageType::ENGLISH)
        {
            return "visitor1.ttf";
        }
        else
        {
            return "SourceHanSansKR-Bold.otf";
        }
    }
    else if (font == FONT_ARIAL)
    {
        return "Arial";
    }
    else if (font == FONT_BITDUST_ONE)
    {
        return "bitdust1.ttf";
    }
    else if (font == FONT_BITDUST_TWO)
    {
        return "bitdust2.ttf";
    }
    else if (font == FONT_VISITOR)
    {
        return "visitor1.ttf";
    }
    return "BMDOHYEON.ttf";
}
int GameManager::getBombCount()
{
    return UserDefault::getInstance()->getIntegerForKey("Bomb_Count", 3);
}
void GameManager::setBombCount(int bombCount)
{
    UserDefault::getInstance()->setIntegerForKey("Bomb_Count", bombCount);
}
int GameManager::getPotionCount()
{
    return UserDefault::getInstance()->getIntegerForKey("Potion_Count", 3);
}
void GameManager::setPotionCount(int potionCount)
{
    UserDefault::getInstance()->setIntegerForKey("Potion_Count", potionCount);
}
GameManager *GameManager::getInstance()
{
    if (NULL == m_mySingleton)
    {
        m_mySingleton = new GameManager();
    }

    return m_mySingleton;
}
void GameManager::exitGame()
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

const char *GameManager::getShortenedKoreanString(std::string str, int length)
{
    const char *nick = "";
    if (str.length() > length)
    {
        CCLOG("original: %s", str.c_str());
        int normalIndex = -1;
        for (int k = 0; k < length; k++)
        {
            if (str[k] >= 0 && (int)str[k] < 128)
            {
                normalIndex = k;
            }
        }

        CCLOG("normal: %d", normalIndex);
        if (normalIndex < 0 || normalIndex == length)
        {
            nick = __String::createWithFormat("%s...", str.substr(0, length).c_str())->getCString();
            CCLOG("all korean: normalIndex: %d", normalIndex);
        }
        else
        {
            int last = normalIndex;
            for (int i = last; i < length - 3; i += 3)
            {
                last += 3;
            }
            CCLOG("ugly/ last: %d, normalIndex: %d", last, normalIndex);
            nick = __String::createWithFormat("%s...", str.substr(0, (last + 1)).c_str())->getCString();
        }
    }
    return nick;
}

const char *GameManager::getWeaponImageFileName(int infoType)
{
    switch (infoType)
    {
    case WEAPON_PISTOL:
        return __String::createWithFormat("pistol.png")->getCString();
    case WEAPON_MAGNUM:
        return __String::createWithFormat("magnum.png")->getCString();
    case WEAPON_UZI:
        return __String::createWithFormat("uzi.png")->getCString();
    case WEAPON_SNIPER_RIFLE:
        return __String::createWithFormat("sniperRifle.png")->getCString();
    case WEAPON_SLUGGER:
        return __String::createWithFormat("slugger.png")->getCString();
    case WEAPON_SLUGGER_NG:
        return __String::createWithFormat("sluggerNG.png")->getCString();
    case WEAPON_ROCKET_LAUNCHER:
        return __String::createWithFormat("rocketLauncher.png")->getCString();
    case WEAPON_RIFLE_GRENADE:
        return __String::createWithFormat("rifleGrenade.png")->getCString();
    case WEAPON_RIFLE:
        return __String::createWithFormat("rifle.png")->getCString();
    case WEAPON_MATT_A4:
        return __String::createWithFormat("mattA4.png")->getCString();
    case WEAPON_NIFE_THROWER:
        return __String::createWithFormat("nifeThrower.png")->getCString();
    case WEAPON_MACHINE_GUN:
        return __String::createWithFormat("machineGun.png")->getCString();
    case WEAPON_LASER_GUN:
        return __String::createWithFormat("laserGun.png")->getCString();
    case WEAPON_GUIDED_ROCKET_LAUNCHER:
        return __String::createWithFormat("guidedRocketLauncher.png")->getCString();
    case WEAPON_FIRE_BOTTLER:
        return __String::createWithFormat("fireBottler.png")->getCString();
    case WEAPON_ENERGY_GUN:
        return __String::createWithFormat("energyGun.png")->getCString();
    case WEAPON_DRAGON_RIFLE:
        return __String::createWithFormat("dragonRifle.png")->getCString();
    case WEAPON_CROSS_BOW:
        return __String::createWithFormat("crossBow.png")->getCString();
    case WEAPON_BAZOOKA:
        return __String::createWithFormat("bazooka.png")->getCString();
    case WEAPON_BALL_SHOOTER:
        return __String::createWithFormat("ballShooter.png")->getCString();
    case WEAPON_FLAME_THROWER:
        return __String::createWithFormat("flameThrower.png")->getCString();
    case WEAPON_ANTI_BAT:
        return __String::createWithFormat("antiBat.png")->getCString();
    case WEAPON_STRIKER:
        return __String::createWithFormat("striker.png")->getCString();
    case WEAPON_LIGHTNING_GUN:
        return __String::createWithFormat("lightningGun.png")->getCString();
    case WEAPON_SHARK_GUN:
        return __String::createWithFormat("sharkGun.png")->getCString();
    case WEAPON_LASER_COIN:
        return __String::createWithFormat("laserCoin.png")->getCString();
    default:
        return __String::createWithFormat("pistol.png")->getCString();
        break;
    }
}
double GameManager::getAngle(Vec2 pos1, Vec2 pos2)
{
    float xGap = pos1.x - pos2.x;
    float yGap = pos1.y - pos2.y;
    return atan2(yGap, xGap) * 180 / 3.14;
}
void GameManager::makeItScaleUpAndDown(Node *node)
{
    float duration = 0.6f;
    node->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(duration, 1.1f), ScaleTo::create(duration, 0.9f), NULL)));
}
void GameManager::makeItSiluk(Node *node, float delay, float dur)
{
    float originalScaleX = node->getScaleX();
    float originalScaleY = node->getScaleY();

    float duration = dur;
    float scaleBit;
    if (node->getContentSize().width > node->getContentSize().height)
    {
        scaleBit = node->getContentSize().height * 0.05;
    }
    else
    {
        scaleBit = node->getContentSize().width * 0.05;
    }
    node->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(duration, (1 + scaleBit / node->getContentSize().width) * originalScaleX, (1 - scaleBit / node->getContentSize().width) * originalScaleY), ScaleTo::create(duration, (1 - scaleBit / node->getContentSize().width) * originalScaleX, (1 + scaleBit / node->getContentSize().width) * originalScaleY), NULL)));
}
void GameManager::makeItSiluk(Node *node)
{
    makeItSiluk(node, 0, 0.3f);
}
bool GameManager::DoesWeaponExist(int weaponType)
{
    switch (weaponType)
    {
    case WEAPON_PISTOL:
        break;
    case WEAPON_MAGNUM:
        break;
    case WEAPON_UZI:
        break;
    case WEAPON_SNIPER_RIFLE:
        break;
    case WEAPON_SLUGGER:
    case WEAPON_SLUGGER_NG:
        break;
    case WEAPON_ROCKET_LAUNCHER:
        break;
    case WEAPON_RIFLE_GRENADE:
        break;
    case WEAPON_RIFLE:
        break;
    case WEAPON_MATT_A4:
        break;
    case WEAPON_NIFE_THROWER:
        break;
    case WEAPON_MACHINE_GUN:
        break;
    case WEAPON_LASER_GUN:
        break;
    case WEAPON_GUIDED_ROCKET_LAUNCHER:
        break;
    case WEAPON_FIRE_BOTTLER:
        break;
    case WEAPON_ENERGY_GUN:
        break;
    case WEAPON_DRAGON_RIFLE:
        break;
    case WEAPON_CROSS_BOW:
        break;
    case WEAPON_BAZOOKA:
        break;
    case WEAPON_BALL_SHOOTER:
        break;
    case WEAPON_FLAME_THROWER:
        break;
    case WEAPON_ANTI_BAT:
        break;
    case WEAPON_STRIKER:
        break;
    default:
        return false;
        break;
    }
    return true;
}
int GameManager::getNextToThisWeapon(int weaponType)
{
    for (int i = weaponType + 1; i < WEAPON_NO_MORE; i++)
    {
        if (DoesWeaponExist(i))
        {
            return i;
        }
    }
    return WEAPON_NO_MORE;
}
int GameManager::getNextWeaponToUnlock(int weaponType)
{
    for (int i = WEAPON_NOT_EXIST + 1; i < WEAPON_NO_MORE; i++)
    {
        if (DoesWeaponExist(i) && !getWeaponUnlocked(i))
        {
            return i;
        }
    }
    return WEAPON_NO_MORE;
}
// void GameManager::setStageLayer(Layer* layer)
//{
//     stageLayer = layer;
//     stageLayer->retain();
// }

// HelloWorld* GameManager::getStageLayer()
//{
//     if (!stageLayer) {
//         return NULL;
//     }
//     return stageLayer;

//}

void GameManager::setHudLayer(HudLayer *layer)
{
    hudLayer = layer;
    //    hudLayer->retain();
}
HudLayer *GameManager::getHudLayer()
{
    if (!hudLayer)
    {
        return NULL;
    }
    return hudLayer;
}
void GameManager::setEditorHud(EditorHud *layer)
{
    editorHud = layer;
}
EditorHud *GameManager::getEditorHud()
{
    if (!editorHud)
    {
        return NULL;
    }
    return editorHud;
}

void GameManager::setStageScene(Scene *scene)
{
    isStageSetOnce = true;
    //    stageScene->retain();
}

void GameManager::nativeControllerButtonEvent(int controller, int button, bool isPressed, float value, bool isAnalog)
{
    //    EventKeyboard::KeyCode keyCode = (EventKeyboard::KeyCode)button;
    if (isPressed)
    {
        if (button == 29 ||
            button == 54 ||
            button == 96 ||
            button == 99)
        {
            GameManager::getInstance()->buttonDown(BUTTON_A);
        }
        else if (button == 30 ||
                 button == 52 ||
                 button == 58 ||
                 button == 23 ||
                 button == 97 ||
                 button == 100)
        {
            if (downPressed)
            {
                if (getHudLayer())
                {
                    getWorld()->getDown();
                }
            }
            else
            {
                GameManager::getInstance()->buttonDown(BUTTON_B);
            }
        }
        else if (button == 31)
        {
            GameManager::getInstance()->cPressed = true;
        }
        else if (button == 19)
        {
            GameManager::getInstance()->buttonDown(BUTTON_B);
        }
        else if (button == 20)
        {
            GameManager::getInstance()->downPressed = true;
        }
        else if (button == 21)
        {
            GameManager::getInstance()->buttonDown(BUTTON_LEFT);
        }
        else if (button == 22)
        {
            GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
        }
        //        CCLOG("Key with keycode %d pressed/ And left: %d", keyCode, (int)EventKeyboard::KeyCode::KEY_DPAD_LEFT);
    }
    else
    {
        if (button == 29 ||
            button == 54 ||
            button == 96 ||
            button == 99)
        {
            GameManager::getInstance()->buttonUp(BUTTON_A);
        }
        else if (button == 30 ||
                 button == 52 ||
                 button == 58 ||
                 button == 23 ||
                 button == 97 ||
                 button == 100)
        {
            GameManager::getInstance()->buttonUp(BUTTON_B);
        }
        else if (button == 31)
        {
            GameManager::getInstance()->cPressed = false;
        }
        else if (button == 19)
        {
            GameManager::getInstance()->buttonUp(BUTTON_B);
        }
        else if (button == 20)
        {
            GameManager::getInstance()->downPressed = false;
        }
        else if (button == 21)
        {
            GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        }
        else if (button == 22)
        {
            GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
        }
        else if (button == 108 ||
                 button == 109)
        {
            if (GameManager::getInstance()->isInMiddleOfGame)
            {
                GameManager::getInstance()->getHudLayer()->showPauseLayer();
            }
        }
        //        CCLOG("Key with keycode %d released", keyCode);
    }
}

void GameManager::nativeControllerLAxisXEvent(int controller, float value)
{
    //    CCLOG("Analog Key with X keycode: %d, %f", controller, value);
    if (value < -0.5)
    {
        //        GameManager::getInstance()->buttonDown(BUTTON_LEFT);
    }
    else if (value > 0.5)
    {
        //        GameManager::getInstance()->buttonDown(BUTTON_RIGHT);
    }
    else
    {
        //        GameManager::getInstance()->buttonUp(BUTTON_LEFT);
        //        GameManager::getInstance()->buttonUp(BUTTON_RIGHT);
    }
}
void GameManager::nativeControllerLAxisYEvent(int controller, float value)
{
    //    CCLOG("Analog Key with Y keycode: %d, %f", controller, value);
    if (value < -0.9)
    {
        //        GameManager::getInstance()->cPressed = true;
    }
    else if (value > 0.5)
    {
        //        GameManager::getInstance()->buttonDown(BUTTON_B);
    }
    else
    {
        //        GameManager::getInstance()->buttonUp(BUTTON_B);
        //        GameManager::getInstance()->cPressed = false;
    }
}

void GameManager::buttonDown(int buttonType)
{
    if (getWorld() != nullptr && getWorld()->blockKeys)
    {
        return;
    }
    if (!this->isInMiddleOfGame)
    {
        return;
    }
    switch (buttonType)
    {
    case BUTTON_LEFT:
        leftPressed = true;
        break;
    case BUTTON_RIGHT:
        rightPressed = true;
        break;
    case BUTTON_UP:
        upPressed = true;
        break;
    case BUTTON_DOWN:

        downPressed = true;
        break;
    case BUTTON_A:
        if (!this->getWorld()->isInEvent && HUD->getPopup() == nullptr)
        {
            this->getWorld()->playerFireCoolTime = 0;

            firePressed = true;
        }
        break;
    case BUTTON_B:
        jumpPressed = true;

        break;
    default:
        break;
    }
}

void GameManager::buttonUp(int buttonType)
{
    switch (buttonType)
    {
    case BUTTON_LEFT:
        leftPressed = false;
        break;
    case BUTTON_RIGHT:
        rightPressed = false;
        break;
    case BUTTON_UP:
        upPressed = false;
        break;
    case BUTTON_DOWN:
        downPressed = false;
        break;
    case BUTTON_A:
        firePressed = false;
        break;
    case BUTTON_B:
        jumpPressed = false;
        if (!getWorld()->isPaused)
        {
        }

        break;
    default:
        break;
    }
    if (!leftPressed && !rightPressed && !upPressed && !downPressed && !WORLD->isInEvent)
    {
    }
}
const char *GameManager::getAchievementId(int achievementIndex)
{
    //    if (GameManager::getInstance()->market == MARKET_APPSTORE_PAID ||
    //        GameManager::getInstance()->market == MARKET_APPSTORE_FREE) {
    //
    //        switch (achievementIndex) {
    //            case ACHIEVEMENT_COMPLETE_TUTORIAL:
    //                return "starcollecter";
    //            case ACHIEVEMENT_BEAT_BOSS:
    //                return "brkeeper";
    //            case ACHIEVEMENT_OPEN_GOLD_SUITCASE:
    //                return "brfirstfruit";
    //            case ACHIEVEMENT_PERFECT_SCORE:
    //                return "brrichman";
    //            case ACHIEVEMENT_WHO_IS_THE_HOOD :
    //                return "brkiller";
    //            case ACHIEVEMENT_WEAPON_MASTER:
    //                return "brangrykettel";
    //            case ACHIEVEMENT_DUNGEON_MASTER:
    //                return "brwhosboss";
    //            case ACHIEVEMENT_WHERE_IS_THE_PRINCESS:
    //                return "brweaponmaster";
    //            case ACHIEVEMENT_OPEN_JEWELRY_BOX:
    //                return "brleadership";
    //            case ACHIEVEMENT_COLLECT_RELICS:
    //                return "belegendary";
    //            default:
    //                break;
    //        }
    //    }else{
    //        switch (achievementIndex) {
    //            case ACHIEVEMENT_COMPLETE_TUTORIAL:
    //                return "CgkIjOjFydEdEAIQAQ";
    //            case ACHIEVEMENT_BEAT_BOSS:
    //                return "CgkIjOjFydEdEAIQAg";
    //            case ACHIEVEMENT_OPEN_GOLD_SUITCASE:
    //                return "CgkIjOjFydEdEAIQAw";
    //            case ACHIEVEMENT_PERFECT_SCORE:
    //                return "CgkIjOjFydEdEAIQBA";
    //            case ACHIEVEMENT_WHO_IS_THE_HOOD :
    //                return "CgkIjOjFydEdEAIQBQ";
    //            case ACHIEVEMENT_WEAPON_MASTER:
    //                return "CgkIjOjFydEdEAIQBg";
    //            case ACHIEVEMENT_DUNGEON_MASTER:
    //                return "CgkIjOjFydEdEAIQBw";
    //            case ACHIEVEMENT_WHERE_IS_THE_PRINCESS:
    //                return "CgkIjOjFydEdEAIQCA";
    //            case ACHIEVEMENT_OPEN_JEWELRY_BOX:
    //                return "CgkIjOjFydEdEAIQCQ";
    //            case ACHIEVEMENT_COLLECT_RELICS:
    //                return "CgkIjOjFydEdEAIQCg";
    //            default:
    //                break;
    //        }
    //    }
    return "";
}
void GameManager::saveCoin()
{
    CCLOG("save coin");
    UserDefault::getInstance()->setIntegerForKey("g0", (int)g0);
    UserDefault::getInstance()->setIntegerForKey("g1", (int)g1);
    UserDefault::getInstance()->setIntegerForKey("g2", (int)g2);
    UserDefault::getInstance()->setIntegerForKey("g3", (int)g3);
    UserDefault::getInstance()->setIntegerForKey("g4", (int)g4);
    UserDefault::getInstance()->setIntegerForKey("g5", (int)g5);
    UserDefault::getInstance()->setIntegerForKey("g6", (int)g6);

    UserDefault::getInstance()->setIntegerForKey("c0", (int)c0);
    UserDefault::getInstance()->setIntegerForKey("c1", (int)c1);
    UserDefault::getInstance()->setIntegerForKey("c2", (int)c2);
    UserDefault::getInstance()->setIntegerForKey("c3", (int)c3);
    UserDefault::getInstance()->setIntegerForKey("c4", (int)c4);
    UserDefault::getInstance()->setIntegerForKey("c5", (int)c5);
    UserDefault::getInstance()->setIntegerForKey("c6", (int)c6);

    UserDefault::getInstance()->setIntegerForKey("s0", (int)s0);
    UserDefault::getInstance()->setIntegerForKey("s1", (int)s1);
    UserDefault::getInstance()->setIntegerForKey("s2", (int)s2);
    UserDefault::getInstance()->setIntegerForKey("s3", (int)s3);
    UserDefault::getInstance()->setIntegerForKey("s4", (int)s4);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    checkDataSecure();
#endif
    UserDefault::getInstance()->flush();
}

void GameManager::setFacebookReady(bool ready)
{
    isFacebookReady = ready;
    if (ready)
    {
        CCLOG("setFacebookReaday true");
    }
    else
    {
        CCLOG("setFacebookReaday false");
    }
}
void GameManager::addWoodKey(int amount)
{
    int value = getWoodKey();
    value += amount;
    UDSetInt(KEY_WOOD_KEY_COUNT, value);
    UDSetStr(KEY_KEYS, strmake("%d/%d", getWoodKey(), getGoldKey()));
}
void GameManager::addGoldKey(int amount)
{
    int value = getGoldKey();
    value += amount;
    UDSetInt(KEY_GOLD_KEY_COUNT, value);
    UDSetStr(KEY_KEYS, strmake("%d/%d", getWoodKey(), getGoldKey()));
}
void GameManager::setWoodKey(int amount)
{
    UDSetInt(KEY_WOOD_KEY_COUNT, amount);
    UDSetStr(KEY_KEYS, strmake("%d/%d", getWoodKey(), getGoldKey()));
}
void GameManager::setGoldKey(int amount)
{
    UDSetInt(KEY_GOLD_KEY_COUNT, amount);
    UDSetStr(KEY_KEYS, strmake("%d/%d", getWoodKey(), getGoldKey()));
}
int GameManager::getWoodKey()
{
    return UDGetInt(KEY_WOOD_KEY_COUNT, 0);
}
int GameManager::getGoldKey()
{
    return UDGetInt(KEY_GOLD_KEY_COUNT, 0);
}

void GameManager::addCoin(int howMuch)
{
    int coin = getCoin();
    coin += howMuch;
    UserDefault::getInstance()->setIntegerForKey(KEY_COIN_COUNT, coin);
    BSM->shouldSaveGold = true;
    //    int one = (char)c0;
    //    int ten = (char)c1;
    //    int hun = (char)c2;
    //    int tho = (char)c3;
    //    int tth = (char)c4;
    //    int hth = (char)c5;
    //    int mil = (char)c6;
    //    int total = one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
    //    total += howMuch;
    //
    //    c0 = (int)(total%10);
    //    c1 = (int)((total/10)%10);
    //    c2 = (int)((total/100)%10);
    //    c3 = (int)((total/1000)%10);
    //    c4 = (int)((total/10000)%10);
    //    c5 = (int)((total/100000)%10);
    //    c6 = (int)((total/1000000)%10);
}
void GameManager::setCoin(int howMuch)
{
    //    int one = (char)c0;
    //    int ten = (char)c1;
    //    int hun = (char)c2;
    //    int tho = (char)c3;
    //    int tth = (char)c4;
    //    int hth = (char)c5;
    //    int mil = (char)c6;
    //    int total = one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
    //    total = howMuch;
    //
    //    c0 = (int)(total%10);
    //    c1 = (int)((total/10)%10);
    //    c2 = (int)((total/100)%10);
    //    c3 = (int)((total/1000)%10);
    //    c4 = (int)((total/10000)%10);
    //    c5 = (int)((total/100000)%10);
    //    c6 = (int)((total/1000000)%10);
    UserDefault::getInstance()->setIntegerForKey(KEY_COIN_COUNT, howMuch);
}

int GameManager::getCoin()
{
    //    int one = (char)c0;
    //    int ten = (char)c1;
    //    int hun = (char)c2;
    //    int tho = (char)c3;
    //    int tth = (char)c4;
    //    int hth = (char)c5;
    //    int mil = (char)c6;
    //    return one + ten*10 + hun*100 + tho*1000 + tth*10000 + hth*100000 + mil*1000000;
    return UserDefault::getInstance()->getIntegerForKey(KEY_COIN_COUNT, 0);
}

void GameManager::addSoul(int howMuch)
{
    int one = (char)s0;
    int ten = (char)s1;
    int hun = (char)s2;
    int tho = (char)s3;
    int tth = (char)s4;
    int total = one + ten * 10 + hun * 100 + tho * 1000 + tth * 10000;
    total += howMuch;

    s0 = (int)(total % 10);
    s1 = (int)((total / 10) % 10);
    s2 = (int)((total / 100) % 10);
    s3 = (int)((total / 1000) % 10);
    s4 = (int)((total / 10000) % 10);
}

int GameManager::getSoul()
{
    int one = (char)s0;
    int ten = (char)s1;
    int hun = (char)s2;
    int tho = (char)s3;
    int tth = (char)s4;
    return one + ten * 10 + hun * 100 + tho * 1000 + tth * 10000;
}
void GameManager::addGem(int howMuch)
{
    int gem = getGem();
    gem += howMuch;
    lastGem = gem;
    UDSetInt(KEY_GEM_COUNT, gem);
    BSM->shouldSaveGem = true;
}
void GameManager::setGem(int howMuch)
{
    lastGem = howMuch;
    UD->setIntegerForKey(KEY_GEM_COUNT, howMuch);
}
int GameManager::getGem()
{
    int gem = UD->getIntegerForKey(KEY_GEM_COUNT, 0);
    if (lastGem < gem && gem > 100000)
    {
        //        BSM->banMe();
    }
    return gem;
}
void GameManager::addTree(int howMuch)
{
    int tree = getTree();
    tree += howMuch;
    UDSetInt(KEY_TREE_COUNT, tree);
    BSM->shouldSaveLumber = true;
}
void GameManager::setTree(int howMuch)
{
    UDSetInt(KEY_TREE_COUNT, howMuch);
}
int GameManager::getTree()
{
    return UDGetInt(KEY_TREE_COUNT, 0);
}
void GameManager::unscheduleAllNotifications()
{
    //    NativeInterface::NativeInterfacecancelAllLocalNotification();
}
void GameManager::scheduleLocalNotification(const char *title, const char *msg, int time)
{
    //    NativeInterface::NativeInterfaceregisterLocalNotification(time, msg, title);
}
void GameManager::scheduleLocalNotification()
{
    //    NativeInterface::NativeInterfacecancelAllLocalNotification();
    if (!this->getNotificationOn())
    {
        return;
    }

    CCLOG("schedule");

    if (LanguageManager::getInstance()->getLanguageType() == LanguageType::KOREAN)
    {
        /*NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*1, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*2, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*3, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*4, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*5, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*6, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*7, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*8, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*9, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*10, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*11, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*12, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*13, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*14, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*15, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*16, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*17, "공주가 기다려요! 어서 공주를 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*18, "공주가 기다려요! 대리님 구해주세요!", "공주가 기다려요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*19, "깃발이 가득 찼어요! 맘껏 대리의 전설을 즐겨볼까요?", "깃발이 가득 찼어요!");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*20, "공주가 기다려요!어서 공주를 구해주세요!", "공주가 기다려요!");
    }else{
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*1, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*3, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*5, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*7, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*9, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*11, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*13, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*15, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*17, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*19, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*21, "Come and save the princess!", "The princess is waiting for you");
        NativeInterface::NativeInterfaceregisterLocalNotification(60*60*24*30, "Come and save the princess!", "The princess is waiting for you");*/
    }
}
void GameManager::setMusicVolumn(float vol)
{
    float value = 0;
    float percent = vol * 100;
    if (percent > 30)
    {
        value += 0.1;
        value += (percent - 30) * 0.9f / 70;
    }
    else
    {
        value = percent / 300.0f;
    }

    //    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(vol);
    UserDefault::getInstance()->setFloatForKey(KEY_MUSIC_VOLUMN, vol);
    AudioEngine::setVolume(currentBGM, vol);
    //    GameManager::getInstance()->saveCoin();
}
void GameManager::setSoundVolumn(float vol)
{
    //    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(vol);
    UserDefault::getInstance()->setFloatForKey(KEY_SOUND_VOLUMN, vol);
    //    GameManager::getInstance()->saveCoin();
}
void GameManager::setNotificationOn(bool onOff)
{
    UserDefault::getInstance()->setBoolForKey(KEY_NOTIFICATION_ON, onOff);
    //    GameManager::getInstance()->saveCoin();
}
void GameManager::setButtonSize(float siz)
{
    if (hudLayer && isInMiddleOfGame)
    {
        hudLayer->setButtonSize(siz);
    }
    UserDefault::getInstance()->setFloatForKey(KEY_BUTTON_SIZE, siz);
    GameManager::getInstance()->saveCoin();
}
float GameManager::getButtonSize()
{
    if (Director::getInstance()->getWinSize().width < 960)
    {
        return UserDefault::getInstance()->getFloatForKey(KEY_BUTTON_SIZE, 0.4);
    }
    else
    {
        return UserDefault::getInstance()->getFloatForKey(KEY_BUTTON_SIZE, 0.9);
    }
}
float GameManager::getMusicVolumn()
{
    float volume = UserDefault::getInstance()->getFloatForKey(KEY_MUSIC_VOLUMN, 1);
    return volume;
    //    float value = 0;
    //    float percent = SimpleAudioEngine::getInstance()->getBackgroundMusicVolume() * 100;
    //    if(percent > 30){
    //        value += 0.1;
    //        value += (percent - 30)*0.9f/70;
    //    }else{
    //        value = percent/300.0f;
    //    }
    //
    //    return value;
}
float GameManager::getSoundVolumn()
{
    //    return SimpleAudioEngine::getInstance()->getEffectsVolume();
    return UserDefault::getInstance()->getFloatForKey(KEY_SOUND_VOLUMN, 1);
    ;
}
bool GameManager::getNotificationOn()
{
    return UserDefault::getInstance()->getBoolForKey(KEY_NOTIFICATION_ON, true);
}
void GameManager::preLoadAllSoundEffect()
{
    experimental::AudioEngine::preload("att-sound1-2.wav");
    experimental::AudioEngine::preload("att-sound1-3.wav");
    experimental::AudioEngine::preload("att-sound1-4.wav");
    experimental::AudioEngine::preload("woodHitSmall.wav");
    experimental::AudioEngine::preload("woodHit.wav");
    experimental::AudioEngine::preload("DdalKack.wav");
    experimental::AudioEngine::preload("cartoonyNagative.wav");
    experimental::AudioEngine::preload("pencilShort.wav");
    experimental::AudioEngine::preload("helicopter.mp3");
    experimental::AudioEngine::preload("page-flip-1.wav");
    experimental::AudioEngine::preload("explosionDoong.wav");
    experimental::AudioEngine::preload("arrowShoot.mp3");
    experimental::AudioEngine::preload("bensound-epic.mp3");
    experimental::AudioEngine::preload("mayday.mp3");
}
void GameManager::stopSoundEffect(int sound)
{

    experimental::AudioEngine::stop(sustainSoundTag);
    //    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
    /*switch(sound)
    {
        case SOUND_LASER_BUZZ:
        case SOUND_LASER_WOONG:
            CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(sustainSoundTag);
            break;
    }*/
}
long GameManager::getTodaysRandom()
{
    int seed = ((int)(double)BSM->getCurrentTimeT()) / (60 * 60 * 24);
    log("today's seed: %d", seed);
    return 1234 + seed;
}
float GameManager::getSoundVolumnByDistance(Node *target, Node *source)
{
    float distance = getDistance(target, this);
    float minDistance = 300;
    float maxDistance = 1100;
    float volumn = 0;
    if (distance < minDistance)
    {
        volumn = 1;
    }
    else if (distance > maxDistance)
    {
        volumn = 0.4f;
    }
    else
    {
        float range = maxDistance - minDistance;
        volumn = (range - distance) / range;
    }
    return volumn;
}
float GameManager::getDistance(Node *target, Node *source)
{
    return getDistance(target->getPosition(), source->getPosition());
}
float GameManager::getDistance(Vec2 target, Vec2 source)
{
    return sqrtf(powf(target.x - source.x, 2) + powf(target.y - source.y, 2));
}
void GameManager::playSoundEffect(int sound, float gain, float pan)
{
    if (sound == SOUND_BGM_DUAL || sound == SOUND_BGM_MAYDAY)
    {
    }
    else
    {
        if (getSoundVolumn() <= 0)
        {
            return;
        }
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//    return;
#endif
    //    log("music: %f, sound: %f", CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume(), CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume());
    switch (sound)
    {
    case SOUND_HIT:
        experimental::AudioEngine::play2d(strmake("att-sound1-%d.wav", rand() % 3 + 2).c_str(), false);
        break;
    case SOUND_WOOD_HIT:
        if (rand() % 2 == 0)
        {
            experimental::AudioEngine::play2d("woodHitSmall.wav", false);
        }
        else
        {
            experimental::AudioEngine::play2d("woodHit.wav", false);
        }

        break;
    case SOUND_DDALKACK:
        experimental::AudioEngine::play2d("DdalKack.wav", false);
        break;
    case SOUND_NAGATIVE:
        experimental::AudioEngine::play2d("cartoonyNagative.wav", false);
        break;
    case SOUND_PENCIL_SHORT:
        experimental::AudioEngine::play2d("pencilShort.wav", false);
        break;
    case SOUND_HELICOPTER:
        experimental::AudioEngine::play2d("helicopter.mp3", false);
        break;
    case SOUND_PAPER_FLIP:
        experimental::AudioEngine::play2d("page-flip-1.wav", false);
        break;
    case SOUND_EXPLOSION_MIDDLE:
        experimental::AudioEngine::play2d("explosionDoong.wav", false);
        break;
    case SOUND_ARROW:
        experimental::AudioEngine::play2d("arrowShoot.mp3", false);
        break;
    case SOUND_BGM_DUAL:
        experimental::AudioEngine::stop(currentBGM);
        //            currentBGM = CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        currentBGM = experimental::AudioEngine::play2d("bensound-epic.mp3", true);
        AudioEngine::setVolume(currentBGM, getMusicVolumn());
        break;
    case SOUND_BGM_MAYDAY:
        experimental::AudioEngine::stop(currentBGM);
        //            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        currentBGM = experimental::AudioEngine::play2d("mayday.mp3", true);
        AudioEngine::setVolume(currentBGM, getMusicVolumn());
        break;
    }
}
void GameManager::makeLabelEllipsis(Label *lbl, float width)
{
    std::string originalStr = lbl->getString();
    int index = originalStr.size();
    while (lbl->getContentSize().width > width)
    {
        index--;
        lbl->setString(originalStr.substr(0, index) + "...");
    }
}
Vec2 GameManager::getGemCountPosition()
{
    cocos2d::Size size = Director::getInstance()->getWinSize();
    return Vec2(size.width / 2 + 190, size.height - 30);
}

Layer *GameManager::getShopLayer()
{
    //    if (!shopLayer) {
    //        shopLayer = ShopLayer::create();
    //        shopLayer->retain();
    //    }
    return NULL; // shopLayer;
}

HelloWorld *GameManager::getWorld()
{
    return currentStageLayer;
}
void GameManager::setCurrentStageLayer(HelloWorld *layer)
{
    currentStageLayer = layer;
}
int GameManager::getWeaponPower(int weaponType)
{
    int damage = 10;
    switch (weaponType)
    {
    case WEAPON_PISTOL:
        damage = 14;
        break;
    case WEAPON_MAGNUM:
        damage = 20;
        break;
    case WEAPON_UZI:
        damage = 16;
        break;
    case WEAPON_SNIPER_RIFLE:
        damage = 120;
        break;
    case WEAPON_SLUGGER:
    case WEAPON_SLUGGER_NG:
        damage = 80;
        break;
    case WEAPON_ROCKET_LAUNCHER:
        damage = 60;
        break;
    case WEAPON_RIFLE_GRENADE:
        damage = 50;
        break;
    case WEAPON_RIFLE:
        damage = 30;
        break;
    case WEAPON_MATT_A4:
        damage = 45;
        break;
    case WEAPON_NIFE_THROWER:
        damage = 30;
        break;
    case WEAPON_MACHINE_GUN:
        damage = 20;
        break;
    case WEAPON_LASER_GUN:
        damage = 2;
        break;
    case WEAPON_GUIDED_ROCKET_LAUNCHER:
        damage = 70;
        break;
    case WEAPON_FIRE_BOTTLER:
        damage = 20;
        break;
    case WEAPON_ENERGY_GUN:
        damage = 40;
        break;
    case WEAPON_DRAGON_RIFLE:
        damage = 100;
        break;
    case WEAPON_CROSS_BOW:
        damage = 40;
        break;
    case WEAPON_BAZOOKA:
        damage = 170;
        break;
    case WEAPON_BALL_SHOOTER:
        damage = 30;
        break;
    case WEAPON_FLAME_THROWER:
        damage = 7;
        break;
    case WEAPON_ANTI_BAT:
        damage = 14;
        break;
    case WEAPON_STRIKER:
        damage = 40;
        break;
    case WEAPON_LIGHTNING_GUN:
        damage = 55;
        break;
    case WEAPON_SHARK_GUN:
        damage = 50;
        break;
    case WEAPON_LASER_COIN:
        damage = 4;
        break;
    default:
        break;
    }
    int level = GameManager::getInstance()->getWeaponPowerLevel(weaponType);
    if (damage > 10)
    {
        damage = damage * 0.6 + damage * (0.2 * (level + 1)) + rand() % (int)(damage * 0.2f);
    }

    return damage;
}
/*
bool GameManager::isShieldPurcahsed(){
    const time_t rawtime = (const time_t)ServerManager::getInstance()->webTime;
    const time_t shieldTime = (const time_t)GameManager::getInstance()->getShieldPurchasedTime();
    int time = (int)difftime(shieldTime, rawtime);

    return UserDefault::getInstance()->getBoolForKey(KEY_SHIELD, false) && ServerManager::getInstance()->timeEstablished && time > 0;
}
void GameManager::setShield(long time){
    UserDefault::getInstance()->setBoolForKey(KEY_SHIELD, true);
    UserDefault::getInstance()->setDoubleForKey(KEY_SHIELD_TIME, time + 60*60*24);
    UserDefault::getInstance()->flush();
}
long GameManager::getShieldPurchasedTime(){
    return UserDefault::getInstance()->getDoubleForKey(KEY_SHIELD_TIME, 0);
}*/

bool GameManager::getWeaponUnlocked(int weaponType)
{
    bool unlocked = false;

    switch (weaponType)
    {
    case WEAPON_PISTOL:
        return true;
    case WEAPON_MAGNUM:
        return UserDefault::getInstance()->getBoolForKey(ldMagnumUnlockedKey, false);
    case WEAPON_UZI:
        return UserDefault::getInstance()->getBoolForKey(ldMagnumUnlockedKey, false);
    case WEAPON_SNIPER_RIFLE:
        return UserDefault::getInstance()->getBoolForKey(ldUziUnlockedKey, false);
    case WEAPON_SLUGGER:
        return UserDefault::getInstance()->getBoolForKey(ldSluggerUnlockedKey, false);
    case WEAPON_SLUGGER_NG:
        return UserDefault::getInstance()->getBoolForKey(ldSluggerNGUnlockedKey, false);
    case WEAPON_ROCKET_LAUNCHER:
        return UserDefault::getInstance()->getBoolForKey(ldRocketLauncherUnlockedKey, false);
    case WEAPON_RIFLE_GRENADE:
        return UserDefault::getInstance()->getBoolForKey(ldRifleGrenadeUnlockedKey, false);
    case WEAPON_RIFLE:
        return UserDefault::getInstance()->getBoolForKey(ldRifleUnlockedKey, false);
    case WEAPON_MATT_A4:
        return UserDefault::getInstance()->getBoolForKey(ldMattA4UnlockedKey, false);
    case WEAPON_NIFE_THROWER:
        return UserDefault::getInstance()->getBoolForKey(ldNifeThrowerUnlockedKey, false);
    case WEAPON_MACHINE_GUN:
        return UserDefault::getInstance()->getBoolForKey(ldMachineGunUnlockedKey, false);
    case WEAPON_LASER_GUN:
        return UserDefault::getInstance()->getBoolForKey(ldLaserGunUnlockedKey, false);
    case WEAPON_GUIDED_ROCKET_LAUNCHER:
        return UserDefault::getInstance()->getBoolForKey(ldGuidedRocketLauncherUnlockedKey, false);
    case WEAPON_FIRE_BOTTLER:
        return UserDefault::getInstance()->getBoolForKey(ldFireBottlerUnlockedKey, false);
    case WEAPON_ENERGY_GUN:
        return UserDefault::getInstance()->getBoolForKey(ldEnergyGunUnlockedKey, false);
    case WEAPON_DRAGON_RIFLE:
        return UserDefault::getInstance()->getBoolForKey(ldDragonRifleUnlockedKey, false);
    case WEAPON_CROSS_BOW:
        return UserDefault::getInstance()->getBoolForKey(ldCrossBowUnlockedKey, false);
    case WEAPON_BAZOOKA:
        return UserDefault::getInstance()->getBoolForKey(ldBazookaUnlockedKey, false);
    case WEAPON_BALL_SHOOTER:
        return UserDefault::getInstance()->getBoolForKey(ldBallShooterUnlockedKey, false);
    case WEAPON_ANTI_BAT:
        return UserDefault::getInstance()->getBoolForKey(ldAntiBatUnlockedKey, false);
    case WEAPON_STRIKER:
        return UserDefault::getInstance()->getBoolForKey(ldStrikerUnlockedKey, false);
    default:
        break;
    }

    return unlocked;
}

bool GameManager::getCharacterUnlocked(int costume)
{
    return false;
}

const char *GameManager::getCharacterUnlockedKey(int costume)
{
    return "dary_unlocked";
}

const char *GameManager::getCharacterName(int costume)
{

    return "Dary";
}

const char *GameManager::getCharacterDesc(int costume)
{

    return "Dary";
}

void GameManager::showSpriteExplosion(Node *parent, const char *sptName, Vec2 pos, int sptCount, float radius, float dur)
{
    for (int i = 0; i < sptCount; i++)
    {
        Sprite *spt = Sprite::createWithSpriteFrameName(sptName);
        parent->addChild(spt);
        spt->setPosition(pos);
        int angle = rand() % 360;
        spt->setRotation(angle);
        float radian = angle * 3.14f / 180;
        //        dur = dur + dur*(rand()%10)*0.1f;
        spt->runAction(EaseOut::create(MoveBy::create(dur, Vec2(cos(radian) * radius, sin(radian) * radius)), 2));
        spt->runAction(RotateBy::create(dur, -180 + rand() % 360));
        spt->runAction(Sequence::create(DelayTime::create(dur * 2 / 3), FadeOut::create(dur / 3), SPT_REMOVE_FUNC, NULL));
    }
}
void GameManager::showParticleExplosion(Node *prt, const char *sptName, Vec2 pos, float scale)
{
    ParticleExplosion *particle = ParticleExplosion::create();
    particle->setPosition(pos);     // 위치
    particle->setLife(0.4 * scale); // 지속시간
    particle->setLifeVar(0.3 * scale);
    particle->setSpeed(70); // 속도
    particle->setStartSize(scale);
    particle->setTotalParticles(15);
    particle->setStartColor(Color4F(205, 205, 0, 155));
    particle->setEndColor(Color4F(205, 205, 0, 0));
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage(sptName));
    prt->addChild(particle);
}
void GameManager::pushLayer(Layer *parent, Layer *layer)
{
    //    disableLayer(parent);
    //    parent->addChild(layer, 1000);
    animateFadeIn(layer, parent);
}

void GameManager::pushLayerWithoutDisable(Layer *parent, Layer *layer)
{
    //    parent->addChild(layer, 1000);
    animateFadeIn(layer, parent);
}
void GameManager::googleSignInOrOut(bool sign)
{
    if (sign)
    {
        //        MessageBox("Signed in", "Google Play Service");
        CCLOG("** google Signed in");
    }
    else
    {
        //        MessageBox("Signed out", "Google Play Service");
        CCLOG("** google not signed in");
    }
    if (settingLayer != NULL)
    {
    }

    //    if(titleLayer != NULL && ((TitleLayer*)titleLayer)->titleMain != NULL){
    //        ((TitleLayer*)titleLayer)->removeSignInButton();
    //    }
    isGoogleSigned = sign;
}
void GameManager::showNode(Node *node, float delay)
{
    node->setOpacity(0);
    node->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.3f), nullptr));
    node->setScale(0.1f);
    node->runAction(Sequence::create(DelayTime::create(delay), EaseBackOut::create(ScaleTo::create(0.4f, 1)), nullptr));
}
void GameManager::animateFadeIn(Node *layer, Node *parent)
{
    parent->addChild(layer, 1000);
    layer->setScale(0.1f);
    layer->runAction(Sequence::create(ScaleTo::create(0.2, 1.1), ScaleTo::create(0.05, 0.95), ScaleTo::create(0.05, 1), NULL));

    Sprite *blackBack = Sprite::create("whiteRect.png");
    parent->addChild(blackBack, 999);
    blackBack->setOpacity(0.6f);
    blackBack->setColor(Color3B::BLACK);
    blackBack->setTag(7899);
    blackBack->setScaleX(size.width);
    blackBack->setScaleY(size.height);
    blackBack->setPosition(Vec2(size.width / 2, size.height / 2));
}
void GameManager::animateFadeOut(Node *layer)
{
    //    layer->setScale(0.1f);
    layer->runAction(Sequence::create(ScaleTo::create(0.06, 0.85), ScaleTo::create(0.1, 1.2), ScaleTo::create(0.1, 0.1), CallFuncN::create(CC_CALLBACK_1(GameManager::animationFadeOutDone, this)), NULL));
}

void GameManager::animationFadeInDone(Node *layer)
{
    //    CCLOG("fade in done");
}
void GameManager::animationFadeOutDone(Node *layer)
{
    Layer *parent = (Layer *)layer->getParent();
    parent->removeChildByTag(7899);
    layer->removeFromParentAndCleanup(true);
    CCLOG("layer removed");
    //    enableLayer(parent);
}
/*
void GameManager::disableLayer(Layer* layer){
    layer->setTouchEnabled(false);
//    CCLOG("layer retain count: %d", layer->retainCount());
    CCMenu* menu;
    Layer* childLayer;
//    CCLOG("disable layer children count: %d", layer->getChildren().size());
    for (int i = 0; i < layer->getChildren().size(); i++) {
        menu = dynamic_cast<CCMenu*>(layer->getChildren().at(i));
        if (menu) {
            if (!menu->isEnabled()) {
                Vector<Movable*>disabledMenusArray = dynamic_cast<CCArray*>((CCArray*)layer->getUserData());
                if (disabledMenusArray == NULL) {
                    disabledMenusArray = CCArray::create();
                    disabledMenusArray->retain();
                    layer->setUserData((void*)disabledMenusArray);
                }
                disabledMenusArray.pushBack(menu);
            }else{
                menu->setEnabled(false);
            }
        }else{
            childLayer = dynamic_cast<Layer*>(layer->getChildren().at(i));
            if (childLayer != NULL) {
                disableLayer(childLayer);
            }
        }
    }
}

void GameManager::popLayer(Layer* layer){
    animateFadeOut(layer);
}

void GameManager::popLayerWithoutAnimation(Layer* layer){
    animationFadeOutDone(layer);
}

void GameManager::enableLayer(Layer* layer){
    layer->setTouchEnabled(true);
    CCMenu* menu;
    Layer* childLayer;
    Vector<Movable*>disabledMenusArray = NULL;
    for (int i = 0; i < layer->getChildren().size(); i++) {
        menu = dynamic_cast<CCMenu*>(layer->getChildren().at(i));
        if (menu) {
            disabledMenusArray = dynamic_cast<CCArray*>((CCArray*)layer->getUserData());

            if (disabledMenusArray != NULL && disabledMenusArray->containsObject(menu)) {

            }else{
                menu->setEnabled(true);
            }
        }
        childLayer = dynamic_cast<Layer*>(layer->getChildren().at(i));
        if (childLayer != NULL) {
            enableLayer(childLayer);
        }
    }

    if (disabledMenusArray != NULL) {
        disabledMenusArray->release();
        layer->setUserData(NULL);
    }
}*/

long GameManager::strToLong(const char *str)
{
    std::stringstream strValue;
    strValue << str;

    long longValue;
    strValue >> longValue;

    return longValue;
}
Sprite *GameManager::getGrayScaleImage(std::string src)
{
    Image *img = new Image();
    img->initWithImageFile(src);

    //    II) From Sprite :
    //    II.1) Sprite -> RenderTexture2D
    //    II.2) RenderTexture2D -> Image (Image *testImage = RenderText2D->newImage();)
    //    2.Then You can do what you need :
    //        Image *img= ... // make Image from Sprite
    int x = 3;
    if (img->hasAlpha())
        x = 4;

    unsigned char *data = new unsigned char[img->getDataLen() * x];
    data = img->getData();
    // [0][0] => Left-Top Pixel !
    // But cocos2d Location Y-axis is Bottom(0) to Top(max)

    //    unsigned char *newData = new unsigned char[img->getDataLen()*x];

    for (int i = 0; i < img->getWidth(); i++)
    {
        for (int j = 0; j < img->getHeight(); j++)
        {
            unsigned char *pixel = data + (i + j * img->getWidth()) * x;

            // You can see/change pixels' RGBA value(0-255) here !
            unsigned char r = *pixel;
            unsigned char g = *(pixel + 1);
            unsigned char b = *(pixel + 2);
            //            unsigned char a = *(pixel + 3);

            *pixel = (r + g + b) / 3;
            *(pixel + 1) = (r + g + b) / 3;
            *(pixel + 2) = (r + g + b) / 3;
        }
    }
    //    3.Then, convert it to texture_2D
    // Image -> Texture2d
    Texture2D *texture_2D = new Texture2D();
    texture_2D->initWithImage(img);
    //    4.And Finally back to Sprite
    return Sprite::createWithTexture(texture_2D);
}

Sprite *GameManager::getSpriteShapeSolidColorSprite(const char *src, Color3B color)
{
    Image *img = new Image();
    img->initWithImageFile(src);

    int x = 3;
    if (img->hasAlpha())
        x = 4;

    unsigned char *data = new unsigned char[img->getDataLen() * x];
    data = img->getData();

    for (int i = 0; i < img->getWidth(); i++)
    {
        for (int j = 0; j < img->getHeight(); j++)
        {
            unsigned char *pixel = data + (i + j * img->getWidth()) * x;

            if (img->hasAlpha())
            {
                if (*(pixel + 3) == 0)
                {
                    continue;
                }
            }
            *pixel = color.r;
            *(pixel + 1) = color.g;
            *(pixel + 2) = color.b;
        }
    }
    Texture2D *texture_2D = new Texture2D();
    texture_2D->initWithImage(img);

    return Sprite::createWithTexture(texture_2D);
}
void GameManager::SendAchievement(const char *text, int count, int goal)
{
    const char *theText;
    if (GameManager::getInstance()->market == MARKET_MAC)
    {
        theText = __String::createWithFormat("%s_Mac", text)->getCString();
    }
    else
    {
        theText = text;
    }
}
int GameManager::getWeaponPrice(int index)
{
    if (index == 0)
        return 200;
    else if (index == 1)
        return 1000;
    else if (index == 2)
        return 1500;
    else if (index == 3)
        return 2000;
    else if (index == 4)
        return 2500;
    else if (index == 5)
        return 2900;
    else if (index == 6)
        return 3500;
    else if (index == 7)
        return 4000;
    else if (index == 8)
        return 5000;
    else if (index == 9)
        return 5800;
    else if (index == 10)
        return 6500;
    else if (index == 11)
        return 7500;
    else if (index == 12)
        return 8000;
    else if (index == 13)
        return 9300;
    else if (index == 14)
        return 10900;
    else if (index == 15)
        return 12500;
    else if (index == 16)
        return 13000;
    else if (index == 17)
        return 14500;
    else if (index == 18)
        return 15800;
    else if (index == 19)
        return 17000;
    else if (index == 20)
        return 55000;
    else if (index == 21)
        return 99000;
    else if (index == 22)
        return 92000;

    int priceUnit = 500;
    return index * priceUnit;
}

void GameManager::showInterstitialAds()
{
    if (UserDefault::getInstance()->getBoolForKey(KEY_IAP_USER, false))
    {
        //        return;
    }
    if (UserDefault::getInstance()->getBoolForKey(KEY_REMOVE_ADS_GET, false))
    {
        return;
    }
    int adsShowCount = 1;
    int count = UserDefault::getInstance()->getIntegerForKey(KEY_ADS_COUNTER, adsShowCount);
    count++;
    if (count >= adsShowCount)
    {
        count = 0;
        //        NativeInterface::NativeInterfaceshowInterstitial();
    }
    UserDefault::getInstance()->setIntegerForKey(KEY_ADS_COUNTER, count);
}
void GameManager::showVideo(int whichVideo)
{
    //    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    AudioEngine::pause(currentBGM);
    videoIndex = whichVideo;
    isVideoButtonAvailable = false;
    this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(GameManager::enableVideoButton, this)), NULL));
    // #ifdef SDKBOX_ENABLED
    //     if (sdkbox::PluginUnityAds::isReady(UNITY_ADS_REWARD)) {
    //         sdkbox::PluginUnityAds::show(UNITY_ADS_REWARD);
    //     } else {
    //         CCLOG("unityads is not ready");
    //     }
    // #endif

    if (whichVideo == VIDEO_TICKET)
    {
        //        GameSharing::logFB("video ticket");
        GameSharing::firebaseLog("video", "ticket", "");
    }
    else if (whichVideo == VIDEO_SUPPORT_0)
    {
        //        GameSharing::logFB("video support workers");
        GameSharing::firebaseLog("video", "support", "workers");
    }
    else if (whichVideo == VIDEO_SUPPORT_1)
    {
        //        GameSharing::logFB("video support sword archer");
        GameSharing::firebaseLog("video", "support", "sword");
    }
    else if (whichVideo == VIDEO_SUPPORT_2)
    {
        //        GameSharing::logFB("video support heli catapalt");
        GameSharing::firebaseLog("video", "support", "heli");
    }
    else if (whichVideo == VIDEO_STORE_UNIT)
    {
        //        GameSharing::logFB("video store unit");
        GameSharing::firebaseLog("video", "store", "unit");
    }
    else if (whichVideo == VIDEO_STORE_RANDOM)
    {
        //        GameSharing::logFB("video store random");
        GameSharing::firebaseLog("video", "store", "random");
    }
    else if (whichVideo == VIDEO_STORE_GEM)
    {
        //        GameSharing::logFB("video store gem");
        GameSharing::firebaseLog("video", "store", "gem");
    }
    else if (whichVideo == VIDEO_SHIELD)
    {
        //        GameSharing::logFB("video shield");
        GameSharing::firebaseLog("video", "shield", "");
    }
    else if (whichVideo == VIDEO_PVP6_TICKET || whichVideo == VIDEO_PVP12_TICKET)
    {
        //        GameSharing::logFB("video shield");
        GameSharing::firebaseLog("video", "pvp", "");
    }
    //    NativeInterface::NativeInterface::showUnityAdsVideo();
    GameSharing::showRewardedVideoAds(); // test
    //    showVideoDone();
}
void GameManager::enableVideoButton()
{
    isVideoButtonAvailable = true;
}
void GameManager::showVideoDone()
{
    log("video done: %d", videoIndex);
    isVideoDone = true;
    if (videoIndex == VIDEO_GAME_OVER)
    {
        getHudLayer()->videoDone();
    }
    else if (videoIndex == VIDEO_WEAPON_RENT)
    {
    }
    else if (videoIndex == VIDEO_GAME_CONTINUE)
    {
        hudLayer->reviveFromVideo();
        hudLayer->showPauseLayer();
    }
    else if (videoIndex == VIDEO_TICKET)
    {
        ((Title *)titleLayer)->onVideoDone();
    }
    else if (videoIndex == VIDEO_STORE_RANDOM ||
             videoIndex == VIDEO_STORE_UNIT ||
             videoIndex == VIDEO_STORE_GEM)
    {
        // handle with GM->isVideoDone in BHUD
    }
    else if (videoIndex == VIDEO_SUPPORT_0 ||
             videoIndex == VIDEO_SUPPORT_1 ||
             videoIndex == VIDEO_SUPPORT_2)
    {
        getHudLayer()->videoDone();
    }
    else if (videoIndex == VIDEO_PVP6_TICKET || videoIndex == VIDEO_PVP12_TICKET)
    {
        if (heroPage)
        {
            HEROPAGE->onPvpTicketVideoDone();
        }
    }

    //    ((TitleLayer*)titleLayer)->showVideoDone();
    //    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    AudioEngine::resume(currentBGM);
}
bool GameManager::checkDataSecure()
{
    int total = 0;
    total += (int)UserDefault::getInstance()->getBoolForKey(KEY_STARTER_PACK_PURCHASED, false);

    // Stage
    for (int th = 0; th < totalThemeCount; th++)
    {
        for (int st = 0; st < 24; st++)
        {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            total += UserDefault::getInstance()->getIntegerForKey(buf, 0) * 7;

            // mastery
            char masterBuf[50];
            sprintf(masterBuf, KEY_STAGE_MASTER_FORMAT, th * 24 + st);
            total += UserDefault::getInstance()->getIntegerForKey(masterBuf, 0) * 7;
        }
    }

    // coin
    total += getCoin();
    total += 479;
    total *= 571;

    // laset clear
    total += (UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, 0) + 7) * 17;

    // Exp
    total += UserDefault::getInstance()->getIntegerForKey("GExp", 0) * 3;

    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++)
    {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        total += UserDefault::getInstance()->getBoolForKey(wbuf, false) * 3;

        // Weapon Power
        total += UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0) * 5;

        // Weapon Critical
        total += UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0) * 2;
    }

    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++)
    {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        total += UserDefault::getInstance()->getBoolForKey(buf, false) * 19;
    }

    //    CCLOG("Data total: %d", total);

    int tn = 797;
    int tnt = 9;
    int shn = total += DATA_SECURE_OFFSET;
    //    CCLOG("Data shn: %d", total);
    int data = (shn / tn) * 331 + (shn % tnt) * 619;

    //    CCLOG("Data log: %d", data);

    return true;
}
void GameManager::showVideoFailed()
{
    if (getHudLayer())
    {
        getHudLayer()->videoFailed();
    }
    //    ((TitleLayer*)titleLayer)->showVideoFailed();
    //    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    AudioEngine::resume(currentBGM);
}
void GameManager::clickAdsFailed()
{
}
void GameManager::clickAdsDone()
{
}
/*
RenderTexture* GameManager::createAdditiveBorder( Sprite* label, int size, Color3B color, GLubyte opacity )
{
    RenderTexture* rt = RenderTexture::create(label->getTexture()->getContentSize().width + size * 2,
                                                  label->getTexture()->getContentSize().height+size * 2
                                                  );

    Vec2 originalPos = label->getPosition();

    Color3B originalColor = label->getColor();

    GLubyte originalOpacity = label->getOpacity();

    bool originalVisibility = label->isVisible();

    label->setColor(color);

    label->setOpacity(opacity);

    label->setVisible(true);

    ccBlendFunc originalBlend = label->getBlendFunc();

    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE}; // GL_SRC_ALPHA for glow effect
    label->setBlendFunc(bf);

    Vec2 bottomLeft = Vec2(
                             label->getTexture()->getContentSize().width * label->getAnchorPoint().x + size,
                             label->getTexture()->getContentSize().height * label->getAnchorPoint().y + size);

    Vec2 positionOffset= Vec2(
                                label->getTexture()->getContentSize().width  * label->getAnchorPoint().x - label->getTexture()->getContentSize().width / 2,
                                label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height / 2);


    Vec2 position = ccpSub(originalPos, positionOffset);

    //rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();

    for (int i=0; i<360; i+= 15) // you should optimize that for your needs
    {
        label->setPosition(Vec2(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i))*size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i))*size)
                           );
        label->visit();
    }
    rt->end();

    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);

    rt->setPosition(position);

    return rt;
}


RenderTexture* GameManager::createAdditive( Sprite* label, Color3B color, GLubyte opacity, int additiveCount)
{
    RenderTexture* rt = RenderTexture::create(label->getTexture()->getContentSize().width ,
                                                  label->getTexture()->getContentSize().height);

    Vec2 originalPos = label->getPosition();

    Color3B originalColor = label->getColor();

    GLubyte originalOpacity = label->getOpacity();

    bool originalVisibility = label->isVisible();

    label->setColor(color);

    label->setOpacity(opacity);

    label->setVisible(true);

    ccBlendFunc originalBlend = label->getBlendFunc();

    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE}; // GL_SRC_ALPHA for glow effect
    label->setBlendFunc(bf);

    Vec2 bottomLeft = Vec2(
                             label->getTexture()->getContentSize().width * label->getAnchorPoint().x,
                             label->getTexture()->getContentSize().height * label->getAnchorPoint().y);

    Vec2 positionOffset= Vec2(
                                label->getTexture()->getContentSize().width  * label->getAnchorPoint().x - label->getTexture()->getContentSize().width / 2,
                                label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height / 2);


    Vec2 position = originalPos - positionOffset;

    //rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();

    label->setPosition(Vec2(bottomLeft.x, bottomLeft.y));
    for(int i = 0;i < additiveCount;i++){
        label->visit();
    }

    rt->end();

    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);

    rt->setPosition(position);

    return rt;
}
*/

bool GameManager::isBoss(int theme, int stage)
{
    if (theme == 0 && stage == 23)
        return true;
    if (theme == 1 && stage == 23)
        return true;
    if (theme == 2 && stage == 23)
        return true;
    if (theme == 3 && stage == 23)
        return true;
    return false;
}

void GameManager::initAchievement()
{
    setAchievementGoal(ACHIEVEMENT_COMPLETE_TUTORIAL, 1);
    setAchievementGoal(ACHIEVEMENT_BEAT_BOSS, 1);
    setAchievementGoal(ACHIEVEMENT_OPEN_GOLD_SUITCASE, 1);
    setAchievementGoal(ACHIEVEMENT_PERFECT_SCORE, 1);
    setAchievementGoal(ACHIEVEMENT_WHO_IS_THE_HOOD, 1);
    setAchievementGoal(ACHIEVEMENT_WEAPON_MASTER, WEAPON_NO_MORE);
    setAchievementGoal(ACHIEVEMENT_DUNGEON_MASTER, 10);
    setAchievementGoal(ACHIEVEMENT_WHERE_IS_THE_PRINCESS, 1);
    setAchievementGoal(ACHIEVEMENT_OPEN_JEWELRY_BOX, 1);
    setAchievementGoal(ACHIEVEMENT_COLLECT_RELICS, 4);
}

//__String* GameManager::getInAppItemPrice(const char* name){
//    CCObject* obj = priceArray->objectForKey(name);
//    return (__String*)obj;
//}

// void GameManager::setInAppItemPrice(const char* name, const char* price){
//     GameManager::getInstance()->priceArray->setObject(__String::create(price), name);
//     CCLOG("cpp price: %s", ((__String*)GameManager::getInstance()->priceArray->objectForKey(name))->getCString());
// }
void GameManager::setAchievementGoal(int achievementIndex, int goalCount)
{
    char buf[50];
    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
    UserDefault::getInstance()->setIntegerForKey(buf, goalCount);
    saveCoin();
}
bool GameManager::setAchievement(int achievementIndex, int countToAdd)
{
    char buf[50];
    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
    int goalCount = UserDefault::getInstance()->getIntegerForKey(buf, 0);

    char buff[50];
    sprintf(buff, KEY_ACHIEVEMENT_CURRENT, achievementIndex);
    int original = UserDefault::getInstance()->getIntegerForKey(buff, 0);
    int currentCount = countToAdd; //

    if (currentCount > goalCount)
    {
        currentCount = goalCount;
    }
    UserDefault::getInstance()->setIntegerForKey(buff, currentCount);
    return goalCount <= currentCount && original < currentCount;
}
int GameManager::getAchievementGoalCount(int achievementIndex)
{
    char buf[50];
    sprintf(buf, KEY_ACHIEVEMENT_GOAL, achievementIndex);
    return UserDefault::getInstance()->getIntegerForKey(buf, 0);
}
int GameManager::getAchievementCurrentCount(int achievementIndex)
{
    char buf[50];
    sprintf(buf, KEY_ACHIEVEMENT_CURRENT, achievementIndex);
    return UserDefault::getInstance()->getIntegerForKey(buf, 0);
}
void GameManager::arrangeInvetory()
{

    saveCoin();
}
int GameManager::getInventoryCount()
{
    int count = 0;
    return count;
}

int GameManager::getTotalStarCount()
{
    int starCount = 0;
    char buf[50];
    for (int i = 0; i < GameManager::getInstance()->totalThemeCount; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            sprintf(buf, KEY_STAR_COUNT_FORMAT, i, j);
            starCount += UserDefault::getInstance()->getIntegerForKey(buf, 0);

            int mastery = GameManager::getInstance()->getMastery(i * 24 + j);
            if (mastery >= 7)
            {
                starCount++;
            }
        }
    }

    return starCount;
}

int GameManager::getCurrentExp()
{
    int expUnit = GAME_EXP_UNIT;

    int originalExp = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
    int expLeft = originalExp;
    int level = 0;
    for (int i = 0; i < 100; i++)
    {
        expLeft -= expUnit * i;
        level++;
        if (expLeft < 0)
        {
            level--;
            expLeft += expUnit * i;
            break;
        }
        else if (expLeft == 0)
        {
            break;
        }
    }
    if (getGameLevel() >= MAX_LEVEL)
    {
        UserDefault::getInstance()->setIntegerForKey("GExp", originalExp - (expLeft - getCurrentExpMax()));
        saveCoin();
        expLeft = getCurrentExpMax();
    }
    return expLeft;
}
int GameManager::getCurrentExpMax()
{
    return GAME_EXP_UNIT * getGameLevel();
}

int GameManager::getGameLevel()
{
    int expUnit = GAME_EXP_UNIT;
    int expLeft = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
    int level = 0;

    for (int i = 0; i < MAX_LEVEL; i++)
    {
        expLeft -= expUnit * i;
        level++;
        if (expLeft < 0)
        {
            level--;
            break;
        }
        else if (expLeft == 0)
        {
            break;
        }
    }
    return level;
}
void GameManager::addGameExp(int exp)
{
    int expLeft = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
    if (getGameLevel() < 100)
    {
        expLeft += exp;
    }
    UserDefault::getInstance()->setIntegerForKey("GExp", expLeft);
    saveCoin();
}

int GameManager::getWeaponPowerLevel(int weaponType)
{
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weaponType)->getCString(), 0);
    return level;
}

void GameManager::addWeaponPowerLevel(int weapon)
{
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weapon)->getCString(), 0);
    level++;
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, weapon)->getCString(), level);
    GameManager::getInstance()->saveCoin();
}

int GameManager::getWeaponCriticalLevel(int weaponType)
{
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weaponType)->getCString(), 0);
    return level;
}

void GameManager::addWeaponCriticalLevel(int weapon)
{
    int level = UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weapon)->getCString(), 0);
    level++;
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, weapon)->getCString(), level);
    GameManager::getInstance()->saveCoin();
}

void GameManager::addMastery(int stageIndex)
{
    char masterBuf[50];
    sprintf(masterBuf, KEY_STAGE_MASTER_FORMAT, stageIndex);
    int mastery = UserDefault::getInstance()->getIntegerForKey(masterBuf, 0);
    mastery++;
    if (mastery == 6)
    {
    }
    if (mastery > 7)
    {
        mastery = 7;
    }
    UserDefault::getInstance()->setIntegerForKey(masterBuf, mastery);
    saveCoin();
}
int GameManager::getMastery(int stageIndex)
{
    char masterBuf[50];
    sprintf(masterBuf, KEY_STAGE_MASTER_FORMAT, stageIndex);
    int mastery = UserDefault::getInstance()->getIntegerForKey(masterBuf, 0);
    return mastery;
}

void GameManager::loadCSV(ValueMap &map, std::string fileName)
{
    //    std::string fileName = "santamountainSheet/unitStat-Table 1.csv";
    std::string csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    ValueVector rows = GameManager::getInstance()->split(csvStr, "\r\n");

    ValueVector keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    for (int i = 1; i < (int)rows.size(); i++)
    {
        ValueMap userParams;
        std::string strRow = rows.at(i).asString();
        log("%s", strRow.c_str());
        ValueVector params = GameManager::getInstance()->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j++)
        {
            std::string value = params.at(j).asString();
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
            log("%s:%s", theKey.c_str(), value.c_str());
        }
        std::string rowKey = params.at(0).asString();
        map[rowKey] = userParams;
    }
}
void GameManager::setLoadedData(const char *key, const char *value)
{
    if (std::strcmp(value, "true") == 0 || std::strcmp(value, "false") == 0)
    {
        UserDefault::getInstance()->setBoolForKey(key, Value(value).asBool());
    }
    else
    {
        UserDefault::getInstance()->setIntegerForKey(key, Value(value).asInt());
    }

    // Money
    /*if(std::strcmp(key, "GemCount") == 0){
        GameManager::getInstance()->setGem(Value(value).asInt());
        return;
    }else if(std::strcmp(key, "CoinCount") == 0){
        GameManager::getInstance()->setCoin(Value(value).asInt());
        return;
    }*/
    // laset clear
    /*if(std::strcmp(key, KEY_LAST_CLEAR_STAGE) == 0){
        UserDefault::getInstance()->setIntegerForKey(KEY_LAST_CLEAR_STAGE, Value(value).asInt());
        return;
    }

    // Exp
    if(std::strcmp(key, "GExp") == 0){
        UserDefault::getInstance()->setIntegerForKey("GExp", Value(value).asInt());
        return;
    }

    // Stage
    for (int th = 0; th < totalThemeCount; th++) {
        for (int st = 0; st < 24; st++) {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            if (std::strcmp(key, buf) == 0) {
                UserDefault::getInstance()->setIntegerForKey(buf, Value(value).asInt());
                return;
            }

            // mastery
            char masterBuf[50];
            sprintf( masterBuf, KEY_STAGE_MASTER_FORMAT, th*24 + st);
            if (std::strcmp(key, masterBuf) == 0) {
                UserDefault::getInstance()->setIntegerForKey(masterBuf, Value(value).asInt());
                return;
            }
        }
    }

    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++) {
        char cbuf[25];
        sprintf(cbuf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        if (std::strcmp(key, cbuf)) {
            UserDefault::getInstance()->setBoolForKey(cbuf, Value(value).asBool());
            return;
        }
    }

    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++) {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        if (std::strcmp(key, wbuf) == 0) {
            UserDefault::getInstance()->setBoolForKey(wbuf, Value(value).asBool());
            return;
        }

        // Weapon Power
        if (std::strcmp(key, __String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString()) == 0) {
            UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), Value(value).asInt());
            return;
        }

        // Weapon Critical
        if (std::strcmp(key, __String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString()) == 0) {
            UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), Value(value).asInt());
            return;
        }
    }
    */
}
void GameManager::reset()
{
    UserDefault::getInstance()->setBoolForKey(KEY_STARTER_PACK_PURCHASED, false);

    // Money
    //    GameManager::getInstance()->setGem(0);
    //    GameManager::getInstance()->setCoin(0);

    // Stage
    for (int th = 0; th < totalThemeCount; th++)
    {
        for (int st = 0; st < 24; st++)
        {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            UserDefault::getInstance()->setIntegerForKey(buf, 0);

            // mastery
            char masterBuf[50];
            sprintf(masterBuf, KEY_STAGE_MASTER_FORMAT, th * 24 + st);
            UserDefault::getInstance()->setIntegerForKey(masterBuf, 0);
        }
    }

    // laset clear
    UserDefault::getInstance()->setIntegerForKey(KEY_LAST_CLEAR_STAGE, 0);

    // Exp
    UserDefault::getInstance()->setIntegerForKey("GExp", 0);

    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++)
    {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        UserDefault::getInstance()->setBoolForKey(wbuf, false);

        // Weapon Power
        UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0);

        // Weapon Critical
        UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0);
    }

    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++)
    {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        UserDefault::getInstance()->setBoolForKey(buf, false);
    }
    saveCoin();
}
void GameManager::saveGameData()
{
    if (UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON, WEAPON_PISTOL) != WEAPON_PISTOL)
    {
        //        NativeInterface::NativeInterfacesetIntForSavedGame(KEY_SELECTED_WEAPON, UserDefault::getInstance()->getIntegerForKey(KEY_SELECTED_WEAPON));
    }

    // Stage
    for (int th = 0; th < totalThemeCount; th++)
    {
        for (int st = 0; st < 24; st++)
        {
            // star
            char buf[50];
            sprintf(buf, KEY_STAR_COUNT_FORMAT, th, st);
            //            if (UserDefault::getInstance()->getIntegerForKey(buf, 0) != 0) {
            //                NativeInterface::NativeInterfacesetIntForSavedGame(buf, UserDefault::getInstance()->getIntegerForKey(buf, 0));
            //            }

            // mastery
            char masterBuf[50];
            sprintf(masterBuf, KEY_STAGE_MASTER_FORMAT, th * 24 + st);
            //            if (UserDefault::getInstance()->getIntegerForKey(masterBuf) != 0) {
            //                NativeInterface::NativeInterfacesetIntForSavedGame(masterBuf, UserDefault::getInstance()->getIntegerForKey(masterBuf));
            //            }
        }
    }

    // laset clear
    if (UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, -1) != 0)
    {
        //        NativeInterface::NativeInterfacesetIntForSavedGame(KEY_LAST_CLEAR_STAGE, UserDefault::getInstance()->getIntegerForKey(KEY_LAST_CLEAR_STAGE, 0));
    }

    // Exp
    if (UserDefault::getInstance()->getIntegerForKey("GExp", 0) != 0)
    {
        //        NativeInterface::NativeInterfacesetStringForSavedGame("GExp", Value(UserDefault::getInstance()->getIntegerForKey("GExp", 0)).asString().c_str());
        //        NativeInterface::NativeInterfacesetIntForSavedGame("GExp", UserDefault::getInstance()->getIntegerForKey("GExp", 0));
    }

    // Character
    for (int i = 0; i < TOTAL_CHARACTER_COUNT; i++)
    {
        char buf[25];
        sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, i);
        if (UserDefault::getInstance()->getBoolForKey(buf, false))
        {
            //            NativeInterface::NativeInterfacesetIntForSavedGame(buf, (int)UserDefault::getInstance()->getBoolForKey(buf, false));
            //            NativeInterface::NativeInterfacesetStringForSavedGame(buf, UserDefault::getInstance()->getBoolForKey(buf, false)?"true":"false");
        }
    }

    // Weapon
    char wbuf[25];
    for (int i = 0; i < TOTAL_WEAPON_COUNT; i++)
    {
        sprintf(wbuf, KEY_WEAPON_PURCHASED_FORMAT, i);
        if (UserDefault::getInstance()->getBoolForKey(wbuf, false))
        {
            //            NativeInterface::NativeInterfacesetIntForSavedGame(wbuf, (int)UserDefault::getInstance()->getBoolForKey(wbuf, false));
            //            NativeInterface::NativeInterfacesetStringForSavedGame(wbuf, UserDefault::getInstance()->getBoolForKey(wbuf, false)?"true":"false");
        }

        // Weapon Power
        if (UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0) != 0)
        {
            //            NativeInterface::NativeInterfacesetIntForSavedGame(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_POWER_LEVEL, i)->getCString(), 0));
        }

        // Weapon Critical
        if (UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0) != 0)
        {
            //            NativeInterface::NativeInterfacesetIntForSavedGame(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_WEAPON_CRITICAL_LEVEL, i)->getCString(), 0));
        }
    }
    //    NativeInterface::NativeInterfacesaveGame();
}

void GameManager::setCharacterPurchased(int character)
{
    char buf[25];
    sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, character);
    UserDefault::getInstance()->setBoolForKey(buf, true);
    saveCoin();
}
void GameManager::showDisposableMessage(std::string msg, Node *parent)
{
    if (parent->getChildByTag(TAG_DISPOSABLE_MESSAGE) != nullptr)
    {
        parent->getChildByTag(TAG_DISPOSABLE_MESSAGE)->removeFromParent();
    }
    Label *disposableLabel = Label::createWithTTF(msg, GameManager::getInstance()->getFont(FONT_VISITOR), 26);
    parent->addChild(disposableLabel, 2002);
    disposableLabel->enableOutline(Color4B::BLACK, 8);
    //    disposableLabel->enableShadow();
    disposableLabel->setTextColor(Color4B::WHITE);
    disposableLabel->setTag(TAG_DISPOSABLE_MESSAGE);

    disposableLabel->setString(msg);
    float labelWidth = disposableLabel->getBoundingBox().size.width;
    cocos2d::Size size = Director::getInstance()->getWinSize();
    if (labelWidth > size.width)
    {
        disposableLabel->setWidth(size.width / disposableLabel->getScale());
    }
    float dur = 3;
    disposableLabel->setPosition(size.width / 2 - parent->getPositionX(), size.height / 4);
    disposableLabel->runAction(Sequence::create(MoveBy::create(0.5, Vec2(0, 10)), DelayTime::create(dur), FadeOut::create(1), CallFuncN::create(CC_CALLBACK_0(Node::removeFromParent, disposableLabel)), NULL));
    lastDisposableLabel = disposableLabel;

    //    Sprite* sptDisposableBack = Sprite::create("whiteBigCircle.png");
    //    parent->addChild(sptDisposableBack, 2001);
    //
    //    sptDisposableBack->setColor(Color3B::BLACK);
    //    sptDisposableBack->setOpacity(100);
    //    sptDisposableBack->setScale(0.8, 0.2);
    //    sptDisposableBack->stopAllActions();
    //    sptDisposableBack->setPosition(disposableLabel->getPosition());
    //    sptDisposableBack->runAction(Sequence::create(MoveBy::create(0.5, Vec2(0, 10)), DelayTime::create(dur), FadeOut::create(0.3), NULL));
    //    lastDisposableLabelBack = sptDisposableBack;
}
bool GameManager::isCharacterPurchased(int character)
{
    char buf[25];
    sprintf(buf, KEY_CHARACTER_PURCHASED_FORMAT, character);
    bool isPurchased = UserDefault::getInstance()->getBoolForKey(buf, false);
    return isPurchased;
}
void GameManager::setWeaponPurchased(int weapon)
{
    char buf[25];
    sprintf(buf, KEY_WEAPON_PURCHASED_FORMAT, weapon);
    UserDefault::getInstance()->setBoolForKey(buf, true);
    saveCoin();
}
bool GameManager::isWeaponPurchased(int weapon)
{
    if (weapon == WEAPON_PISTOL)
    {
        return true;
    }
    char buf[25];
    sprintf(buf, KEY_WEAPON_PURCHASED_FORMAT, weapon);
    bool isPurchased = UserDefault::getInstance()->getBoolForKey(buf, false);
    return isPurchased;
}

/*void GameManager::refreshTopBar(Widget* topBar){
    Text* lblGem = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblGem"));
    lblGem->setString(Value(getGem()).asString());
    Text* lblCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblCoin"));
    lblCoin->setString(Value(getCoin()).asString());

    Text* lblLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblLevel"));
    lblLevel->setString(Value(getGameLevel()).asString());

    int expUnit = GAME_EXP_UNIT;

    int expLeft = UserDefault::getInstance()->getIntegerForKey("GExp", 0);
    int level = 0;
    int currentMaxExp = 0;
    for (int i = 0; i < 100; i++) {
        currentMaxExp = expUnit*(i+1);
        expLeft -= currentMaxExp;
        level++;
        if (expLeft <= 0) {
            level--;
            expLeft += currentMaxExp;
            break;
        }
    }

    Text* lblExpPercent = dynamic_cast<Text*>(Helper::seekWidgetByName(topBar, "lblExpPercent"));
    char buf[50];
    CCLOG("cExp:%d, mExp: %d", expLeft, currentMaxExp);
    float percent = expLeft*100.0f/currentMaxExp;
    sprintf( buf, "%.1f%%", percent);
    LoadingBar* sptPercent = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(topBar, "ProgressBar_15"));
    sptPercent->setPercent(percent);
    lblExpPercent->setString(buf);
    lblExpPercent->setLocalZOrder(10);
}*/
int GameManager::getRandomFood()
{
    return rand() % 24;
}
int GameManager::getFoodCount(int food)
{
    char buf[50];
    sprintf(buf, KEY_FOOD_FORMAT, food);
    const char *key = buf;
    return UserDefault::getInstance()->getIntegerForKey(key, 0);
}
void GameManager::addFoodCount(int food, int count)
{
    char buf[50];
    sprintf(buf, KEY_FOOD_FORMAT, food);
    const char *key = buf;
    int foodCount = getFoodCount(food);
    foodCount += count;
    UserDefault::getInstance()->setIntegerForKey(key, foodCount);
    saveCoin();
}

const char *GameManager::getFoodSpriteName(int food)
{
    switch (food)
    {
    case FOOD_APPLE:
        return "apple.png";
        break;
    case FOOD_BANANA:
        return "banana.png";
        break;
    case FOOD_BISCKET:
        return "orange.png";
        break;
    case FOOD_BREAD:
        return "bread.png";
        break;
    case FOOD_CANDY:
        return "candy.png";
        break;
    case FOOD_CARROT:
        return "carrot.png";
        break;
    case FOOD_CHERRY:
        return "cherry.png";
        break;
    case FOOD_CHICKEN:
        return "barbeque.png";
        break;
    case FOOD_CORN:
        return "corn.png";
        break;
    case FOOD_DONUT:
        return "donut.png";
        break;
    case FOOD_FISH:
        return "fish.png";
        break;
    case FOOD_GREEN_APPLE:
        return "greemApple.png";
        break;
    case FOOD_HAMBURGER:
        return "hamburger.png";
        break;
    case FOOD_HOTDOG:
        return "hotDog.png";
        break;
    case FOOD_ICECREAM:
        return "iceCream.png";
        break;
    case FOOD_LOLLYPOP:
        return "lollypop.png";
        break;
    case FOOD_MUSHROOM:
        return "mushroom.png";
        break;
    case FOOD_ONIGIRI:
        return "onigiri.png";
        break;
    case FOOD_PEAR:
        return "pear.png";
        break;
    case FOOD_PINE_APPLE:
        return "pineApple.png";
        break;
    case FOOD_POTATO_CHIPS:
        return "potatoChips.png";
        break;
    case FOOD_STRAWBERRY:
        return "strawberry.png";
        break;
    case FOOD_TOAST:
        return "sausage.png";
        break;
    case FOOD_WATERMELON:
        return "waterMelon.png";
        break;
    default:
        return "apple.png";
        break;
    }
}

void GameManager::scrollTheLayer(ui::ScrollView *scrollLayer, bool isLeft, bool isHorizontal, int howMuch)
{
    Vec2 pos;
    if (isHorizontal)
    {
        float x = scrollLayer->getInnerContainer()->getPosition().x + scrollLayer->getContentSize().width * (isLeft ? 1 : -1) * howMuch;
        if (x > 0)
        {
            x = 0;
        }
        else if (x < -(scrollLayer->getInnerContainerSize().width - scrollLayer->getContentSize().width))
        {
            x = -(scrollLayer->getInnerContainerSize().width - scrollLayer->getContentSize().width);
        }
        scrollLayer->getInnerContainer()->runAction(MoveTo::create(0.3, Vec2(x, scrollLayer->getInnerContainer()->getPosition().y)));
    }
    else
    {
    }
    /*
    return;
    int currentX = scrollLayer->getInnerContainer()->getPosition().x;
    int width = scrollLayer->getContentSize().width;
    int totalWidth = scrollLayer->getInnerContainerSize().width - width;
    if (isLeft){
        currentX += width;
        if(currentX > 0){
            currentX = 0;
        }
        if(isHorizontal)
            scrollLayer->scrollToPercentHorizontal(-currentX*100/totalWidth, 0.1, true);
        else
            scrollLayer->scrollToPercentVertical(-currentX*100/totalWidth, 0.1, true);
    }else{
        float rightEndX = -currentX + width;
        if(rightEndX > totalWidth){
            rightEndX = totalWidth;
        }
        int percent = rightEndX*100/totalWidth;
        if(isHorizontal)
            scrollLayer->scrollToPercentHorizontal(percent, 0.1, true);
        else
            scrollLayer->scrollToPercentVertical(percent, 0.1, true);
    }*/
}
Layer *GameManager::getTitleLayer()
{
    return titleLayer;
}
void GameManager::setFontSize(Label *lbl, float size)
{
    TTFConfig config = lbl->getTTFConfig();
    config.fontSize = size;
    lbl->setTTFConfig(config);
}

void GameManager::showWaiting(Node *parent, cocos2d::Size size)
{
    Layout *intersectionLayer = Layout::create();
    intersectionLayer->setTouchEnabled(true);
    intersectionLayer->setContentSize(size);
    Button *background = Button::create("1506_logoBackground.png");
    background->setPosition(Vec2(size.width / 2, size.height / 2));
    background->setScaleX(size.width / background->getContentSize().width);
    background->setScaleY(size.height / background->getContentSize().height);
    intersectionLayer->addChild(background);
    background->setOpacity(180);

    Sprite *spt = Sprite::create("flyingDary.png");
    Vec2 sptPos = Vec2(size.width / 2, size.height / 2 + 100);
    spt->setPosition(sptPos);
    intersectionLayer->addChild(spt);
    float dur = 0.3f;
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(dur, Vec2(0, 10)), MoveBy::create(dur, Vec2(0, -10)), NULL)));

    Label *lblTip = Label::createWithTTF("Please, wait...", GameManager::getInstance()->getFont(FONT_DEFAULT), 25);
    intersectionLayer->addChild(lblTip);
    Vec2 lblPos = Vec2(size.width / 2, size.height / 2 - 60);
    lblTip->setPosition(lblPos);
    lblTip->setAlignment(TextHAlignment::CENTER, TextVAlignment::TOP);

    parent->addChild(intersectionLayer, 9000);
    waitingLayer = intersectionLayer;
}
void GameManager::closeWaiting()
{
    if (waitingLayer)
    {
        waitingLayer->removeFromParentAndCleanup(true);
        waitingLayer = NULL;
    }
}

void GameManager::setWeaponCollected(int index)
{
    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_WEAPON_COLLECTED_FORMAT, index)->getCString(), true);
}

Node *GameManager::findUp(Node *currentNode)
{
    Vec2 pos = currentNode->getPosition();
    int unit = 10;
    Node *nodeFound = findWithDirect(pos, DIRECTION_UP, currentNode);
    if (nodeFound != nullptr)
        return nodeFound;
    while (pos.y < size.height)
    {
        pos.y += unit;
        nodeFound = findWithDirect(pos, DIRECTION_LEFT, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionY() != currentNode->getPositionY())
        {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_RIGHT, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionY() != currentNode->getPositionY())
        {
            break;
        }
    }
    return nodeFound;
}
Node *GameManager::findDown(Node *currentNode)
{
    Vec2 pos = currentNode->getPosition();
    int unit = 10;
    Node *nodeFound = findWithDirect(pos, DIRECTION_DOWN, currentNode);
    if (nodeFound != nullptr)
        return nodeFound;
    while (pos.y > 0)
    {
        pos.y -= unit;
        nodeFound = findWithDirect(pos, DIRECTION_LEFT, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionY() != currentNode->getPositionY())
        {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_RIGHT, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionY() != currentNode->getPositionY())
        {
            break;
        }
    }
    return nodeFound;
}
Node *GameManager::findLeft(Node *currentNode)
{
    Vec2 pos = currentNode->getPosition();
    int unit = 10;
    Node *nodeFound = findWithDirect(pos, DIRECTION_LEFT, currentNode);
    if (nodeFound != nullptr)
        return nodeFound;
    while (pos.x > 0)
    {
        pos.x -= unit;
        nodeFound = findWithDirect(pos, DIRECTION_UP, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionX() != currentNode->getPositionX())
        {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_DOWN, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionX() != currentNode->getPositionX())
        {
            break;
        }
    }
    return nodeFound;
}
Node *GameManager::findRight(Node *currentNode)
{
    Vec2 pos = currentNode->getPosition();
    int unit = 10;
    Node *nodeFound = findWithDirect(pos, DIRECTION_RIGHT, currentNode);
    if (nodeFound != nullptr)
        return nodeFound;
    while (pos.x < size.width)
    {
        pos.x += unit;
        nodeFound = findWithDirect(pos, DIRECTION_UP, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionX() != currentNode->getPositionX())
        {
            break;
        }
        nodeFound = findWithDirect(pos, DIRECTION_DOWN, currentNode);
        if (nodeFound != nullptr && nodeFound->getPositionX() != currentNode->getPositionX())
        {
            break;
        }
    }
    return nodeFound;
}

Node *GameManager::findWithDirect(Vec2 pos, int direction, Node *currentNode)
{
    Node *currentPanel = currentNode->getParent();
    Vec2 findingPos = Vec2(pos.x, pos.y);
    int unit = 10;
    Node *nodeFound = nullptr;
    while (true)
    {
        if (direction == DIRECTION_UP && findingPos.y > size.height)
            break;
        else if (direction == DIRECTION_RIGHT && findingPos.x > size.width)
            break;
        else if (direction == DIRECTION_DOWN && findingPos.y < 0)
            break;
        else if (direction == DIRECTION_LEFT && findingPos.x < 0)
            break;

        if (direction == DIRECTION_UP)
            findingPos.y += unit;
        else if (direction == DIRECTION_RIGHT)
            findingPos.x += unit;
        else if (direction == DIRECTION_DOWN)
            findingPos.y -= unit;
        else if (direction == DIRECTION_LEFT)
            findingPos.x -= unit;
        nodeFound = getNodeAtThisPoint(findingPos, currentNode);
        if (nodeFound != nullptr)
        {
            break;
        }
    }
    return nodeFound;
}
Node *GameManager::getNodeAtThisPoint(Vec2 pos, Node *currentNode)
{
    Node *currentPanel = currentNode->getParent();
    int unitTagLimit = 101;
    for (auto child : currentPanel->getChildren())
    {
        if (child->getTag() < unitTagLimit && child->getBoundingBox().containsPoint(pos) && child != currentNode && child->isVisible())
        {

            if (typeid(*child) == typeid(Button) || (typeid(*child) == typeid(Text) && ((Text *)child)->isTouchEnabled()))
            {
                return child;
            }
        }
    }
    return nullptr;
}

void GameManager::runAnimation(Sprite *spt, const char *name, bool repeat, bool deleteAfterPlay)
{
    spt->stopAllActionsByTag(ACTION_TAG_ANIMATION);
    Animation *animation = AnimationCache::getInstance()->getAnimation(name);
    Animate *animate = Animate::create(animation);
    if (repeat)
    {
        RepeatForever *forever = RepeatForever::create(animate);
        forever->setTag(ACTION_TAG_ANIMATION);
        spt->runAction(forever);
    }
    else
    {
        animate->setTag(ACTION_TAG_ANIMATION);
        if (deleteAfterPlay)
        {
            spt->runAction(Sequence::create(animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        }
        else
        {
            spt->runAction(animate);
        }
    }
}
bool GameManager::isAdsUser()
{
    //    return true; // test
    bool isFreeUser = UDGetBool(KEY_IS_FREE_USER);
    bool isIapUser = UDGetBool(KEY_IAP_USER);
    log("isFreeUser: %d (admob for tag)", isFreeUser);
    log("isIapUser: %d (admob for tag)", isIapUser);
    return isFreeUser && !isIapUser;
    //    return true; // test
}

PointArray *GameManager::getPath(cocos2d::Vec2 start, cocos2d::Vec2 end)
{
    //    log("start get path");
    //    return PointArray::create(100); // test
    //    PointArray* ar = PointArray::create(2);
    //    ar->addControlPoint(start);
    //    ar->addControlPoint(end);
    //    return  ar;
    // stage 4.tmx test
    getPathCall++;
    deque<Cell *> _result = astar->getPath(start.x, start.y, end.x, end.y);
    PointArray *pointArray = PointArray::create(_result.size());
    if (_result.size() == 1)
    {
        return pointArray;
    }

    Cell *cell;
    for (int i = _result.size() - 1; i >= 0; i--)
    {
        cell = _result[i];
        pointArray->addControlPoint(Vec2(cell->getX(), cell->getY()));
    }
    //    log("end get path");
    return pointArray;

    //    astarMap m;
    //    astarPoint s(start.x, start.y), e( end.x, end.y );
    //
    //    if( astar.search( s, e, m ) ) {
    //        std::list<astarPoint> path;
    //        int c = astar.path( path );
    //        for( int y = -1; y < MAP_HEIGHT; y++ ) {
    //            for( int x = -1; x < MAP_WIDTH; x++ ) {
    //                if( x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT || m( x, y ) == 1 )
    //                    std::cout << char(0xdb);
    //                else {
    //                    if( std::find( path.begin(), path.end(), astarPoint( x, y ) )!= path.end() )
    //                        std::cout << "x";
    //                    else std::cout << ".";
    //                }
    //            }
    //            std::cout << "\n";
    //        }
    //
    //        std::cout << "\nPath cost " << c << ": ";
    //        for( std::list<astarPoint>::iterator i = path.begin(); i != path.end(); i++ ) {
    //            std::cout<< "(" << ( *i ).x << ", " << ( *i ).y << ") ";
    //        }
    //        PointArray* pointArray = PointArray::create(path.size());
    //        for (astarPoint node : path) {
    //            pointArray->addControlPoint(Vec2(node.x, node.y));
    //        }
    //        return pointArray;
    //    }
    //    std::cout << "\n\n";
    //    return nullptr;

    //    PathNode player;
    //    player.x = start.x;
    //    player.y = start.y;
    //
    //    PathNode destination;
    //    destination.x = end.x;
    //    destination.y = end.y;
    //
    //
    //
    ////    std::vector<PathNode> list = astar->aStar(player, destination);
    //    std::vector<PathNode> list = ASTAR::Cordinate::aStar(player, destination);
    //    PointArray* pointArray = PointArray::create(list.size());
    //    for (PathNode node : list) {
    //        pointArray->addControlPoint(Vec2(node.x, node.y));
    //    }
    //    return pointArray;
}
void GameManager::setPathState(int x, int y, int state)
{
    //    tileState[x + y*(int)mapSize.width] = state;
    if (x < 0 || y < 0 || x >= mapSize.width || y >= mapSize.height)
    {
        return;
    }
    tileState[x][y] = state;
    astar->setPathState(x, y, state > 0 ? PATH_OCCUPIED : PATH_EMPTY);
    //    ASTAR::Cordinate::blockState[x][y] = state==1;
    // reset tile state
    //    for (int i = 0; i < mapSize.width; i++) {
    //        for (int j = 0; j < mapSize.height; j++) {
    //            tileState[x + i + (y + j)*(int)mapSize.width] = state;
    //        }
    //    }
    //    log("x: %d, y: %d, index: %d, state: %d", x, y, x + y*(int)mapSize.width, state);
}
PointArray *GameManager::getPathOld(cocos2d::Vec2 start, cocos2d::Vec2 end)
{
    //    testStartPos = start; // test
    //    testEndPos = end;

    isPathFound = false;
    pathFindingCounter = 0;
    foundPathDistance = 10000;

    //    int length = mapSize.width*mapSize.height;
    for (int i = 0; i < mapSize.width; i++)
    {
        for (int j = 0; j < mapSize.height; j++)
        {
            blockState[i][j] = ASTAR_STATE_INIT;
        }
    }

    pathEnd = end;
    pathStart = start;
    inspectPathStackCount = 0;
    closeStateTrackIndex = -1;
    //    inspectSurrounded(start.x, start.y, 0);
    //    inspectSurroundedNew(start.x, start.y, 0);
    inspectSurroundedStack(start.x, start.y, 0);

    std::vector<int> xList;
    std::vector<int> yList;
    int index = 0;
    if (isPathFound)
    {
        int x = finalEnd.x;
        int y = finalEnd.y;
        int direction = -1;
        bool shouldBreak = false;
        ;
        while (x != start.x || y != start.y)
        {
            xList.push_back(x);
            yList.push_back(y);
            //            log("x: %d, y: %d", x, y);
            //
            //            int tempX = pathEnd.x - x;
            //            int tempY = pathEnd.y - y;
            //            if (tempX < 0) {
            //                tempX *= -1;
            //            }
            //            if (tempY < 0) {
            //                tempY *= -1;
            //            }
            //            // is 1 block away to destination
            //            if(tempX + tempY < 2){
            //                break;
            //            }
            int lastSuccessX = x;
            int lastSuccessY = y;
            int startX = x;
            int startY = y;
            while (x != start.x || y != start.y)
            {
                direction = blockStateLastPosition[x][y];
                //            x += (direction%3)-1;
                //            y += (direction/3)-1;
                if (direction == 1)
                {
                    y++;
                }
                else if (direction == 2)
                {
                    y++;
                    x--;
                }
                else if (direction == 3)
                {
                    x--;
                }
                else if (direction == 4)
                {
                    y--;
                    x--;
                }
                else if (direction == 5)
                {
                    y--;
                }
                else if (direction == 6)
                {
                    y--;
                    x++;
                }
                else if (direction == 7)
                {

                    x++;
                }
                else if (direction == 8)
                {
                    y++;
                    x++;
                }

                bool isOpen = true;
                int nextX = startX;
                int nextY = startY;
                float theta = atan2f(startY - y, startX - x);
                float dy = sinf(theta) * 0.5f;
                float dx = cosf(theta) * 0.5f;
                float nextXf = startX;
                float dxLeft = startX - x;
                float nextYf = startY;
                float dyLeft = startY - y;
                while (true)
                {
                    //                    if(x == pathEnd.x && y == pathEnd.y){
                    //                        isPathFound = true;
                    //                        finalEnd = Vec2(x, y);
                    //                        log("path found!");
                    //                        break;
                    //                    }
                    dxLeft -= dx;
                    dyLeft -= dy;
                    if (abs(dxLeft) < 1 && abs(dyLeft) < 1)
                    {
                        lastSuccessX = x;
                        lastSuccessY = y;
                        shouldBreak = true;
                        break;
                    }
                    nextXf -= dx;
                    nextYf -= dy;
                    nextX = ceilf(nextXf);
                    nextY = ceilf(nextYf);
                    //                    if(nextX < x) nextX++;
                    //                    else if(nextX > x) nextX--;
                    //                    if(nextY < y) nextY++;
                    //                    else if(nextY > y) nextY--;
                    //

                    char tile = tileState[nextX][nextY];
                    if (tile == TILE_STATE_OCCUPIED)
                    {
                        isOpen = false;

                        index++;
                        break;
                    }
                    if (nextX == x && nextY == y)
                    { // is open
                        lastSuccessX = x;
                        lastSuccessY = y;
                        break;
                    }
                    if (x == pathStart.x && y == pathStart.y)
                        break;
                }
                if (!isOpen)
                {
                    x = lastSuccessX;
                    y = lastSuccessY;
                    break;
                }
                if (x == pathStart.x && y == pathStart.y)
                {
                    x = lastSuccessX;
                    y = lastSuccessY;
                    break;
                }
                if (shouldBreak)
                {
                    break;
                }
            }
            //            if(x + y*mapSize.width >= 10000){
            //                break;
            //            }
            if (shouldBreak)
            {
                break;
            }
            index++;
        }
    }
    else
    {
        //        log("path not fount %d", rand()%100); // test
    }
    PointArray *array = PointArray::create(xList.size());

    for (int i = 0; i < xList.size(); i++)
    {
        array->addControlPoint(Vec2(xList.at(i), yList.at(i)));
    }

    xList.clear();
    yList.clear();
    //    pathResult[index] = nullptr;
    //    return pathResult;
    return array;
}

void GameManager::pushInspectInfo(int x, int y, int distanceFromStart)
{
    inspectXArray[inspectPathStackCount] = x;
    inspectYArray[inspectPathStackCount] = y;
    inspectDistanceArray[inspectPathStackCount] = distanceFromStart;
    inspectPathStackCount++;
}
void GameManager::popFirstInspectInfo(int &x, int &y, int &distanceFromStart)
{
    x = inspectXArray[0];
    y = inspectYArray[0];
    distanceFromStart = inspectDistanceArray[0];
    inspectPathStackCount--;
    for (int i = 0; i < inspectPathStackCount; i++)
    {
        inspectXArray[i] = inspectXArray[i + 1];
        inspectYArray[i] = inspectYArray[i + 1];
        inspectDistanceArray[i] = inspectDistanceArray[i + 1];
    }
    inspectXArray[inspectPathStackCount] = -1;
    inspectYArray[inspectPathStackCount] = -1;
    inspectDistanceArray[inspectPathStackCount] = -1;
}

void GameManager::inspectSurroundedStack(int x, int y, int distanceFromStart)
{
    pushInspectInfo(x, y, distanceFromStart);
    inspectSurroundedStack();
}
void GameManager::inspectSurroundedStack()
{
    int x, y, distanceFromStart;
    popFirstInspectInfo(x, y, distanceFromStart);
    if (isPathFound)
        return;
    //    int blockIndex =x + y*mapSize.width;
    if (x == pathEnd.x && y == pathEnd.y)
    {
        isPathFound = true;
        finalEnd = Vec2(x, y);
        //        log("path found!");
        return;
    }
    blockState[x][y] = ASTAR_STATE_CLOSE;
    int nextX = x;
    int nextY = y;
    if (nextX < pathEnd.x)
        nextX++;
    else if (nextX > pathEnd.x)
        nextX--;
    //    if(x < 0) x = 0;  // out of map
    //    if(x >= mapSize.width ) x = mapSize.width - 1;
    if (nextY < pathEnd.y)
        nextY++;
    else if (nextY > pathEnd.y)
        nextY--;
    //    if(y < 0) y = 0;  // out of map
    //    if(y >= mapSize.height ) y = mapSize.height - 1;

    //    log("nextx: %d, nextY: %d", nextX, nextY);
    pathFindingCounter++;
    if (pathFindingCounter > 10000)
    {
        log("path finding time over : %d", pathFindingCounter);
        return;
    }
    //    if(testStepLimit < pathFindingCounter){ // test
    //        log("path test count: %d", pathFindingCounter);
    //        testStepLimit++;
    //        return;
    //    }

    int distanceFromLast = 10;
    if (x != nextX && y != nextY)
    {
        distanceFromLast = 14;
    }
    char inspectedBlockState = blockState[nextX][nextY]; // get block state
    char tile = tileState[nextX][nextY];

    int direction = 0;
    int i = nextX - x;
    int j = nextY - y;
    if (i == 0 && j == -1)
    {
        direction = 1;
    }
    else if (i == 1 && j == -1)
    {
        direction = 2;
    }
    else if (i == 1 && j == 0)
    {
        direction = 3;
    }
    else if (i == 1 && j == 1)
    {
        direction = 4;
    }
    else if (i == 0 && j == 1)
    {
        direction = 5;
    }
    else if (i == -1 && j == 1)
    {
        direction = 6;
    }
    else if (i == -1 && j == 0)
    {
        direction = 7;
    }
    else if (i == -1 && j == -1)
    {
        direction = 8;
    }

    if (inspectedBlockState == ASTAR_STATE_INIT && tile != TILE_STATE_OCCUPIED)
    { // || inspectedBlockState == ASTAR_STATE_OPEN){
        blockStateLastPosition[nextX][nextY] = direction;

        pushInspectInfo(nextX, nextY, distanceFromStart + distanceFromLast);
        inspectSurroundedStack();
        return;
    }
    else
    { //} if(tile == TILE_STATE_OCCUPIED){
        int firstX, firstY, secondX, secondY, firstDirection, secondDirection;
        getNextAvailableWayAlongWall(firstX, firstY, secondX, secondY, direction, x, y, firstDirection, secondDirection);
        if (firstX < 0)
            return;
        if (firstX >= mapSize.width)
            return;
        if (firstY < 0)
            return;
        if (firstY >= mapSize.height)
            return;
        if (secondX < 0)
            return;
        if (secondX >= mapSize.width)
            return;
        if (secondY < 0)
            return;
        if (secondY >= mapSize.height)
            return;
        bool isFirstOpen = blockState[firstX][firstY] == ASTAR_STATE_INIT;
        bool isSecondOpen = blockState[secondX][secondY] == ASTAR_STATE_INIT;
        if (isFirstOpen && isSecondOpen)
        {
            blockStateLastPosition[firstX][firstY] = firstDirection;
            blockStateLastPosition[secondX][secondY] = secondDirection;
            pushInspectInfo(firstX, firstY, distanceFromStart);
            pushInspectInfo(secondX, secondY, distanceFromStart);
            inspectSurroundedStack();
            inspectSurroundedStack();
        }
        else if (isSecondOpen)
        {
            blockStateLastPosition[secondX][secondY] = secondDirection;
            pushInspectInfo(secondX, secondY, distanceFromStart);
            inspectSurroundedStack();
        }
        else if (isFirstOpen)
        {
            blockStateLastPosition[firstX][firstY] = firstDirection;
            pushInspectInfo(firstX, firstY, distanceFromStart);
            inspectSurroundedStack();
        }
        else
        {
            if (getNextAvailableWay(firstX, firstY, direction, x, y, firstDirection))
            {
                blockStateLastPosition[firstX][firstY] = firstDirection;
                pushInspectInfo(firstX, firstY, distanceFromStart);
                inspectSurroundedStack();
            }
        }
    }
}

void GameManager::inspectSurroundedNew(int x, int y, int distanceFromStart)
{
    //    int blockIndex =x + y*mapSize.width;
    if (x == pathEnd.x && y == pathEnd.y)
    {
        isPathFound = true;
        finalEnd = Vec2(x, y);
        log("path found!");
        return;
    }
    blockState[x][y] = ASTAR_STATE_CLOSE;
    int nextX = x;
    int nextY = y;
    if (nextX < pathEnd.x)
        nextX++;
    else if (nextX > pathEnd.x)
        nextX--;
    //    if(x < 0) x = 0;  // out of map
    //    if(x >= mapSize.width ) x = mapSize.width - 1;
    if (nextY < pathEnd.y)
        nextY++;
    else if (nextY > pathEnd.y)
        nextY--;
    //    if(y < 0) y = 0;  // out of map
    //    if(y >= mapSize.height ) y = mapSize.height - 1;

    //    log("nextx: %d, nextY: %d", nextX, nextY);
    pathFindingCounter++;
    //    if(testStepLimit < pathFindingCounter){ // test
    //        log("path test count: %d", pathFindingCounter);
    //        testStepLimit++;
    //        return;
    //    }

    int distanceFromLast = 10;
    if (x != nextX && y != nextY)
    {
        distanceFromLast = 14;
    }
    char inspectedBlockState = blockState[nextX][nextY]; // get block state
    char tile = tileState[nextX][nextY];

    int direction = 0;
    int i = nextX - x;
    int j = nextY - y;
    if (i == 0 && j == -1)
    {
        direction = 1;
    }
    else if (i == 1 && j == -1)
    {
        direction = 2;
    }
    else if (i == 1 && j == 0)
    {
        direction = 3;
    }
    else if (i == 1 && j == 1)
    {
        direction = 4;
    }
    else if (i == 0 && j == 1)
    {
        direction = 5;
    }
    else if (i == -1 && j == 1)
    {
        direction = 6;
    }
    else if (i == -1 && j == 0)
    {
        direction = 7;
    }
    else if (i == -1 && j == -1)
    {
        direction = 8;
    }

    if (inspectedBlockState == ASTAR_STATE_INIT && tile != TILE_STATE_OCCUPIED)
    { // || inspectedBlockState == ASTAR_STATE_OPEN){
        blockStateLastPosition[nextX][nextY] = direction;

        inspectSurroundedNew(nextX, nextY, distanceFromStart + distanceFromLast);
        return;
    }
    else
    { //} if(tile == TILE_STATE_OCCUPIED){
        int firstX, firstY, secondX, secondY, firstDirection, secondDirection;
        getNextAvailableWayAlongWall(firstX, firstY, secondX, secondY, direction, x, y, firstDirection, secondDirection);
        bool isFirstOpen = blockState[firstX][firstY] == ASTAR_STATE_INIT;
        bool isSecondOpen = blockState[secondX][secondY] == ASTAR_STATE_INIT;
        if (isFirstOpen && isSecondOpen)
        {
            inspectSurroundedNew(firstX, firstY, 0);
            blockStateLastPosition[firstX][firstY] = firstDirection;
            inspectSurroundedNew(secondX, secondY, 0);
            blockStateLastPosition[secondX][secondY] = secondDirection;
        }
        else if (!isFirstOpen && isSecondOpen)
        {
            inspectSurroundedNew(secondX, secondY, 0);
            blockStateLastPosition[secondX][secondY] = secondDirection;
        }
        else if (isFirstOpen && !isSecondOpen)
        {
            inspectSurroundedNew(firstX, firstY, 0);
            blockStateLastPosition[firstX][firstY] = firstDirection;
        }
        else
        {
            log("what?");
        }
    }
}
bool GameManager::getNextAvailableWay(int &firstX, int &firstY, int direction, int x, int y, int &firstDirection)
{
    for (int i = 0; i < 7; i++)
    {
        int inspectX = x;
        int inspectY = y;
        getNextXYForDirection(direction + i + 1, x, y, inspectX, inspectY, firstDirection);
        if (tileState[inspectX][inspectY] == TILE_STATE_EMPTY && blockState[inspectX][inspectY] != ASTAR_STATE_CLOSE)
        {
            firstX = inspectX;
            firstY = inspectY;
            return true;
        }
    }
    return false;
}
void GameManager::getNextAvailableWayAlongWall(int &firstX, int &firstY, int &secondX, int &secondY, int wallDirection, int x, int y, int &firstDirection, int &secondDirection)
{
    // take a rotation and inspect the first and second way along the wall
    for (int i = 0; i < 7; i++)
    {
        int inspectX = x;
        int inspectY = y;
        getNextXYForDirection(wallDirection + i + 1, x, y, inspectX, inspectY, firstDirection);
        if (tileState[inspectX][inspectY] == TILE_STATE_EMPTY)
        {
            firstX = inspectX;
            firstY = inspectY;
            break;
        }
    }
    for (int i = 0; i < 7; i++)
    {
        int inspectX = x;
        int inspectY = y;
        getNextXYForDirection(wallDirection - i - 1, x, y, inspectX, inspectY, secondDirection);
        if (tileState[inspectX][inspectY] == TILE_STATE_EMPTY)
        {
            secondX = inspectX;
            secondY = inspectY;
            break;
        }
    }
}

void GameManager::getNextXYForDirection(int direction, int centerX, int centerY, int &x, int &y, int &newDirection)
{
    if (direction < 1)
    {
        direction += 8;
    }
    else if (direction > 8)
    {
        direction -= 8;
    }
    newDirection = direction;
    if (direction == 1)
    {
        x = centerX;
        y = centerY - 1;
    }
    else if (direction == 2)
    {
        x = centerX + 1;
        y = centerY - 1;
    }
    else if (direction == 3)
    {
        x = centerX + 1;
        y = centerY;
    }
    else if (direction == 4)
    {
        x = centerX + 1;
        y = centerY + 1;
    }
    else if (direction == 5)
    {
        x = centerX;
        y = centerY + 1;
    }
    else if (direction == 6)
    {
        x = centerX - 1;
        y = centerY + 1;
    }
    else if (direction == 7)
    {
        x = centerX - 1;
        y = centerY;
    }
    else if (direction == 8)
    {
        x = centerX - 1;
        y = centerY - 1;
    }
}

void GameManager::inspectSurrounded(int x, int y, int distanceFromStart)
{
    //    int blockIndex =x + y*mapSize.width;
    if (isPathFound)
        return;

    blockState[x][y] = ASTAR_STATE_CLOSE;
    closeStateTrackIndex++;
    closeStateTracker[closeStateTrackIndex] = x + y * mapSize.width;
    pathFindingCounter++;
    //    if(testStepLimit < pathFindingCounter){ // test
    //        log("path test count: %d", pathFindingCounter);
    //        testStepLimit++;
    //        return;
    //    }
    if (pathFindingCounter > 3000)
    {
        log("path finding time over : %d", pathFindingCounter);
        return;
    }
    int inspectingX, inspectingY, indexInArray, distanceFromLast, minDistanceToEnd;
    int minI = -2;
    int minJ = -2;
    bool concluded = false;
    minDistanceToEnd = 10000; //(mapSize.width+mapSize.height)*10;
    int index = 0;
    int inspectedBlockState;
    int shortest = 10000;
    int secondShortest = 10000;
    bool encounterdWall = false;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            //            if(i==j || j+i == 0) continue;// pass diagonal
            inspectingX = x + i;
            inspectingY = y + j;
            if (i == 0 && j == 0)
                continue;
            if (inspectingX < 0 || inspectingY < 0 || inspectingX >= mapSize.width || inspectingY >= mapSize.height)
            { // out of map
                continue;
            }
            indexInArray = inspectingX + inspectingY * mapSize.width;
            inspectedBlockState = blockState[inspectingX][inspectingY];
            if (tileState[inspectingX][inspectingY] == TILE_STATE_OCCUPIED || inspectedBlockState == ASTAR_STATE_DEAD_END)
            {
                continue;
            }
            // avoid diagnal move if block is near
            if (i == -1 && j == -1)
            {
                if (tileState[x][y - 1] == TILE_STATE_OCCUPIED || tileState[x - 1][y] == TILE_STATE_OCCUPIED)
                {
                    continue;
                }
            }
            else if (i == 1 && j == -1)
            {
                if (tileState[x][y - 1] == TILE_STATE_OCCUPIED || tileState[x + 1][y] == TILE_STATE_OCCUPIED)
                {
                    continue;
                }
            }
            else if (i == 1 && j == 1)
            {
                if (tileState[x][y + 1] == TILE_STATE_OCCUPIED || tileState[x + 1][y] == TILE_STATE_OCCUPIED)
                {
                    continue;
                }
            }
            else if (i == -1 && j == 1)
            {
                if (tileState[x][y + 1] == TILE_STATE_OCCUPIED || tileState[x - 1][y] == TILE_STATE_OCCUPIED)
                {
                    continue;
                }
            }

            // distance define
            distanceFromLast = i == 0 || j == 0 ? 10 : 14;

            if (distanceFromStart + distanceFromLast > foundPathDistance)
            {
                concluded = true;
                break;
            }
            if (inspectedBlockState == ASTAR_STATE_CLOSE)
            {
                continue;
            }
            int tempX = (pathEnd.x - (x + i));
            int tempY = (pathEnd.y - (y + j));
            if (tempX < 0)
            {
                tempX *= -1;
            }
            if (tempY < 0)
            {
                tempY *= -1;
            }
            int distanceToEnd = (tempX + tempY) * 10;
            blockStateToEnd[inspectingX][inspectingY] = distanceToEnd;
            if (distanceToEnd < minDistanceToEnd)
            {
                minDistanceToEnd = distanceToEnd;
                minI = i;
                minJ = j;
            }
            // for test drawing
            //            int direction = 2 - (i + 1) + 3*(2 - (j+1));
            // 8 1 2
            // 7 0 3
            // 6 5 4
            int direction = 0;
            if (i == 0 && j == -1)
            {
                direction = 1;
            }
            else if (i == 1 && j == -1)
            {
                direction = 2;
            }
            else if (i == 1 && j == 0)
            {
                direction = 3;
            }
            else if (i == 1 && j == 1)
            {
                direction = 4;
            }
            else if (i == 0 && j == 1)
            {
                direction = 5;
            }
            else if (i == -1 && j == 1)
            {
                direction = 6;
            }
            else if (i == -1 && j == 0)
            {
                direction = 7;
            }
            else if (i == -1 && j == -1)
            {
                direction = 8;
            }

            if (inspectedBlockState == ASTAR_STATE_OPEN)
            {
                if (blockStateFromStart[inspectingX][inspectingY] > distanceFromStart + distanceFromLast)
                {
                    blockStateFromStart[inspectingX][inspectingY] = distanceFromStart + distanceFromLast;
                    blockStateLastPosition[inspectingX][inspectingY] = direction;

                    //                    WORLD->draw->drawLine(Vec2(i*TILE_SIZE, j*TILE_SIZE), Vec2(i*TILE_SIZE, j*TILE_SIZE) + directionPos, Color4F::RED);
                }
            }
            else if (inspectedBlockState == ASTAR_STATE_INIT)
            {
                blockState[inspectingX][inspectingY] = ASTAR_STATE_OPEN;
                blockStateFromStart[inspectingX][inspectingY] = distanceFromStart + distanceFromLast;
                blockStateLastPosition[inspectingX][inspectingY] = direction;

                //                WORLD->draw->drawLine(getTilePosition(x+i, y+j) + Vec2(0, 5), getTilePosition(x+i, y+j) + Vec2(0, 5) + directionPos, Color4F::BLUE);
            }

            if (tempX + tempY < 2)
            {
                finalEnd = Vec2(x + i, y + j);
                isPathFound = true;
                concluded = true;
                break;
            }

            if (x + i == pathEnd.x && y + j == pathEnd.y)
            {
                isPathFound = true;
                if (foundPathDistance > blockStateFromStart[inspectingX][inspectingY])
                {
                    foundPathDistance = blockStateFromStart[inspectingX][inspectingY];
                }
                concluded = true;
                //                    break;// test
            }
            //                log("x: %d, y: %d, dir: %d, toEnd: %d", x + i, y + j, direction, (tempX + tempY)*10);
            index++;
        }
    }
    if (concluded)
    {
        return;
    }
    if (minI > -2 || minJ > -2)
    { // test

        //        log("min x: %d, min y: %d", minI, minJ);
        // dead end -> if no open path or min path is already closed
        if (blockState[x + minI][y + minJ] == ASTAR_STATE_CLOSE)
        {
            blockState[x][y] = ASTAR_STATE_DEAD_END;
            for (int i = 0; i < closeStateTrackIndex; i++)
            {
                int index = closeStateTracker[i];
                blockState[index % (int)mapSize.width][index / (int)mapSize.width] = ASTAR_STATE_INIT;
            }
            closeStateTrackIndex = -1;
            inspectSurrounded(pathStart.x, pathStart.y, 0); // start over
        }
        else
        {
            inspectSurrounded(x + minI, y + minJ, distanceFromStart + (minI == 0 || minJ == 0 ? 10 : 14));
        }
    }
    else
    { // dead end -> if no open path
        blockState[x][y] = ASTAR_STATE_DEAD_END;
        for (int i = 0; i < closeStateTrackIndex; i++)
        {
            int index = closeStateTracker[i];
            blockState[index % (int)mapSize.width][index / (int)mapSize.width] = ASTAR_STATE_INIT;
        }
        closeStateTrackIndex = -1;
        inspectSurrounded(pathStart.x, pathStart.y, 0); // start over
    }
}
void GameManager::drawPath()
{
    for (int i = 0; i < mapSize.width; i++)
    {
        for (int j = 0; j < mapSize.height; j++)
        {
            int direction = blockStateLastPosition[i][j];
            Vec2 directionPos;
            if (direction == 0)
            {
                directionPos = Vec2(0, 1);
            }
            if (direction == 1)
            {
                directionPos = Vec2(0, -3);
            }
            if (direction == 2)
            {
                directionPos = Vec2(-3, -3);
            }
            if (direction == 3)
            {
                directionPos = Vec2(-3, 0);
            }
            if (direction == 4)
            {
                directionPos = Vec2(-3, 3);
            }
            if (direction == 5)
            {
                directionPos = Vec2(0, 3);
            }
            if (direction == 6)
            {
                directionPos = Vec2(3, 3);
            }
            if (direction == 7)
            {
                directionPos = Vec2(3, 0);
            }
            if (direction == 8)
            {
                directionPos = Vec2(3, -3);
            }

            Vec2 center = Vec2(i * TILE_SIZE + TILE_SIZE / 2, (mapSize.width - j - 1) * TILE_SIZE) + Vec2(0, TILE_SIZE / 2);
            int state = blockState[i][j];
            if (state == ASTAR_STATE_OPEN || state == ASTAR_STATE_CLOSE)
            {
                //                Vec2 center = WORLD->getTilePosition(Vec2(i, j));
                WORLD->draw->drawLine(center, center + directionPos * 15, Color4F::WHITE);
                WORLD->draw->drawCircle(center, 25, 360, 20, false, 1, 1, Color4F::BLUE);
            }
            else if (state == ASTAR_STATE_DEAD_END)
            {
                WORLD->draw->drawCircle(center, 25, 360, 20, false, 1, 1, Color4F::RED);
            }
        }
    }
}
void GameManager::alignToCenter(Node *node0, Node *node1, float gap, float centerX, float offsetX)
{
    float totalWidth = node0->getContentSize().width * node0->getScaleX() + node1->getContentSize().width * node1->getScaleX() + gap;
    //    float centerX = centerNode->getPosition().x;
    node0->setPositionX(centerX - totalWidth / 2 + node0->getContentSize().width * node0->getScaleX() / 2 + offsetX);
    node1->setPositionX(centerX - totalWidth / 2 + node0->getContentSize().width * node0->getScaleX() + gap + node1->getContentSize().width * node1->getScaleX() / 2 + offsetX);
}
void GameManager::shakeIt(Node *node)
{
    // node->stopAllActionsByTag(ACTION_TAG_ANIMATION);
    // node->setPosition(Vec2::ZERO);
    float shakeWidth = node->getContentSize().width * 0.05f;
    Sequence *seq = Sequence::create(MoveBy::create(0.05, Vec2(shakeWidth, shakeWidth)), MoveBy::create(0.05, Vec2(-shakeWidth * 2, -shakeWidth)), MoveBy::create(0.03, Vec2(shakeWidth * 1.5f, 0)), MoveBy::create(0.05, Vec2(-shakeWidth, shakeWidth)), MoveBy::create(0.05, Vec2(shakeWidth * 1.5f, -shakeWidth)), MoveBy::create(0.03, Vec2(shakeWidth * -0.5f, 0)), NULL);

    seq->setTag(ACTION_TAG_ANIMATION);
    node->runAction(seq);
}
void GameManager::shakeIt(Node *node, float shakeWidth, int shakeCount, float shakeTime)
{
    Vector<FiniteTimeAction *> array;
    array.pushBack(MoveBy::create(shakeTime, Vec2(-shakeWidth / 2, 0)));
    for (int i = 0; i < shakeCount; i++)
    {
        array.pushBack(MoveBy::create(shakeTime, Vec2(shakeWidth, 0)));
        array.pushBack(MoveBy::create(shakeTime, Vec2(-shakeWidth, 0)));
    }
    array.pushBack(MoveBy::create(shakeTime, Vec2(shakeWidth / 2, 0)));
    node->runAction(Sequence::create(array));
}
cocos2d::Vec2 GameManager::getRandomPosInCicle(cocos2d::Vec2 center, float radius)
{
    return center + Vec2(cos((rand() % 360) * 3.14f / 180) * radius, sin((rand() % 360) * 3.14f / 180) * radius);
}

void GameManager::setTimeLeft(Label *lbl, int time)
{
    lbl->setString(getTimeLeftInString(time));
}
void GameManager::setTimeLeft(Text *lbl, int time)
{
    lbl->setString(getTimeLeftInString(time));
}
void GameManager::setTimeLeft(TextBMFont *lbl, int time)
{
    lbl->setString(getTimeLeftInString(time));
}
std::string GameManager::getTimeLeftInString(int time)
{
    int oneDay = 60 * 60 * 24;
    int oneHour = 60 * 60;
    int oneMin = 60;
    if (time >= oneDay * 2)
    {
        int hour = (int)((int)time % oneDay) / oneHour;
        return __String::createWithFormat("%dD %dH", (int)(time / oneDay), hour)->getCString();
    }
    else if (time >= oneDay)
    {
        int hour = (int)((int)time % oneDay) / oneHour;
        if (hour == 0)
        {
            return __String::createWithFormat("1D")->getCString();
        }
        else
        {
            return __String::createWithFormat("1D %dH", hour)->getCString();
        }
    }
    else
    {
        int sec = (int)((time % oneHour) % oneMin);
        if (time > 3600)
        {
            return __String::createWithFormat("%02d:%02d:%02d", (int)(time / oneHour), (int)((time % oneHour) / oneMin), sec)->getCString();
        }
        else
        {
            return __String::createWithFormat("%02d:%02d", (int)((time % oneHour) / oneMin), sec)->getCString();
        }
    }
}
std::string GameManager::getTimeLeftInStringHMS(int time)
{
    int oneDay = 60 * 60 * 24;
    int oneHour = 60 * 60;
    int oneMin = 60;
    if (time >= oneDay * 2)
    {
        int hour = (int)((int)time % oneDay) / oneHour;
        return __String::createWithFormat("%dD %dH", (int)(time / oneDay), hour)->getCString();
    }
    else if (time >= oneDay)
    {
        int hour = (int)(time % oneDay) / oneHour;
        if (hour == 0)
        {
            return __String::createWithFormat("1D")->getCString();
        }
        else
        {
            return __String::createWithFormat("1D%dH", hour)->getCString();
        }
    }
    else
    {
        int sec = (int)((time % oneHour) % oneMin);
        if (sec % 2 == 0)
        {
            return __String::createWithFormat("%02d:%02d:%02d", (int)(time / oneHour), (int)((time % oneHour) / oneMin), sec)->getCString();
        }
        else
        {
            return __String::createWithFormat("%02d %02d %02d", (int)(time / oneHour), (int)((time % oneHour) / oneMin), sec)->getCString();
        }
    }
}
void GameManager::makeLabelShrink(Text *lbl)
{
    Label *label = (Label *)lbl->getVirtualRenderer();
    label->setOverflow(Label::Overflow::SHRINK);
}
void GameManager::makeLabelShrink(Label *lbl)
{
    lbl->setOverflow(Label::Overflow::SHRINK);
}
void GameManager::makeAdditive(Sprite *spt, Color3B color)
{
    Sprite *clonedSprite;
    if ((clonedSprite = (Sprite *)spt->getChildByName("clone")) == nullptr)
    {
        clonedSprite = Sprite::createWithSpriteFrameName("coin.png");
        spt->addChild(clonedSprite);
    }

    clonedSprite->setColor(color);
    clonedSprite->setSpriteFrame(spt->getSpriteFrame());
    clonedSprite->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
    clonedSprite->setName("clone");

    clonedSprite->setPosition(spt->getContentSize() / 2);

    //    spt->setColor(color);
    //    spt->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
}
int GameManager::getStageObjective(int stage)
{
    if (GM->isColosseum)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    if (stage == 0)
    {
        return CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND;
    }
    else if (stage == 1)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 2)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 3)
    {
        return CLEAR_CONDITION_SURVIVE_FOR_20_MIN;
    }
    else if (stage == 4)
    {
        return CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE;
    }
    else if (stage == 5)
    {
        return CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE;
    }
    else if (stage == 6)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 7)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 8)
    {
        return CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE;
    }
    else if (stage == 9)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 10)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else if (stage == 11)
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    else
    {
        return CLEAR_CONDITION_TERMINATE;
    }
    return -1;
}

std::string GameManager::compress_string(const std::string &str,
                                         int compressionlevel)
{
    z_stream zs; // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK)
        throw(std::runtime_error("deflateInit failed while compressing."));

    zs.next_in = (Bytef *)str.data();
    zs.avail_in = str.size(); // set the z_stream's input

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out)
        {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END)
    { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

/** Decompress an STL string using zlib and return the original data. */
std::string GameManager::decompress_string(const std::string &str)
{
    z_stream zs; // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef *)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // get the decompressed bytes blockwise using repeated calls to inflate
    do
    {
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out)
        {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    { // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

// std::string GameManager::compressString(std::string data){
//     std::string compressed = "";
//     int sameChCounter = 0;
//     char ch = '`';
//     std::string strChar = "";
//     for(int i = 0; i < data.size(); i++){
//         if(data[i] != ch){
//             if(sameChCounter < 3){
//                 for(int j = 0; j < sameChCounter; j++){
//                     compressed.append(strChar);
//                 }
//             }else{
//                 compressed.append(strChar);
//                 compressed.append(std::to_string(sameChCounter));
//             }
//             sameChCounter = 1;
//             ch = data[i];
//             strChar = data.substr(i, 1);
//         }else{
//             sameChCounter++;
//         }
//     }
//     if(sameChCounter < 3){
//         for(int j = 0; j < sameChCounter; j++){
//             compressed.append(strChar);
//         }
//     }else{
//         compressed.append(strChar);
//         compressed.append("[");
//         compressed.append(std::to_string(sameChCounter));
//         compressed.append("]");
//     }
//     return compressed;
// }
// std::string GameManager::decompressString(std::string data){
//     std::string decompressed = "";
//     int startIndex = -1;
//     for(int i = 0; i < data.size(); i++){
//         if(data[i] >= '0' && data[i] <= '9' && startIndex < 0){
//             startIndex = i;
//         }else if((data[i] < '0' || data[i] > '9') && startIndex >= 0){
//             int count = Value(data.substr(startIndex, i-startIndex)).asInt() - 1;
//             for(int j = 0; j < count; j++){
//                 decompressed.append(data.substr(startIndex - 1, 1));
//             }
//             startIndex = -1;
//             decompressed.append(data.substr(i, 1));
//         }else if(startIndex < 0){
//             decompressed.append(data.substr(i, 1));
//         }
//     }
//     return decompressed;
// }

void GameManager::onIapSuccess()
{
    log("onIapSuccess");
}
void GameManager::onIapFailed()
{
    log("onIapFailed");
}
int GameManager::makeNumberCloseTo(int source, int target)
{
    if (source + 10000 < target)
    {
        return source + 10000;
    }
    else if (source + 1000 < target)
    {
        return source + 1000;
    }
    else if (source + 100 < target)
    {
        return source + 100;
    }
    else if (source + 10 < target)
    {
        return source + 10;
    }
    else if (source + 1 <= target)
    {
        return source + 1;
    }
    else if (source - 10000 > target)
    {
        return source - 10000;
    }
    else if (source - 1000 > target)
    {
        return source - 1000;
    }
    else if (source - 100 > target)
    {
        return source - 100;
    }
    else if (source - 10 > target)
    {
        return source - 10;
    }
    else if (source - 1 >= target)
    {
        return source - 1;
    }
    else
    {
        return target;
    }
}
void GameManager::enableButton(Ref *ref)
{
    BTN_FROM_REF
    btn->setEnabled(true);
}
void GameManager::loadBattleData()
{
    std::string fileName = "cartoonCraftLanguageSheet/building_count-Table 1.csv";
    std::string csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    ValueVector rows = GameManager::getInstance()->split(csvStr, "\r\n");
    ValueVector keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    ValueMap userParams;
    for (int i = 1; i < (int)rows.size(); i++)
    {
        std::string strRow = rows.at(i).asString();

        ValueVector params = GameManager::getInstance()->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j++)
        {
            std::string value = params.at(j).asString();
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
        }
        std::string rowKey = params.at(0).asString();
        buildingCountForCastleLevelTable[rowKey] = userParams;
    }

    fileName = "cartoonCraftLanguageSheet/building_price-Table 1.csv";
    csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    rows = GameManager::getInstance()->split(csvStr, "\r\n");
    keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    for (int i = 1; i < (int)rows.size(); i++)
    {
        std::string strRow = rows.at(i).asString();
        ValueVector params = GM->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j++)
        {
            std::string value = params.at(j).asString();
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
        }
        std::string rowKey = params.at(0).asString();
        buildingPriceForCastleLevelTable[rowKey] = userParams;
    }
    fileName = "cartoonCraftLanguageSheet/building_upgrade_price-Table 1.csv";
    csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
    rows = GameManager::getInstance()->split(csvStr, "\r\n");
    keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
    for (int i = 1; i < (int)rows.size(); i++)
    {
        std::string strRow = rows.at(i).asString();
        ValueVector params = GM->split(rows.at(i).asString(), ",");
        for (int j = 1; j < (int)params.size(); j++)
        {
            std::string value = params.at(j).asString();
            std::string theKey = keys.at(j).asString();
            userParams[theKey] = value;
        }
        std::string rowKey = params.at(0).asString();
        buildingUpgradePriceForLevelTable[rowKey] = userParams;
    }

    for (int k = 0; k < 5; k++)
    {
        if (k == 0)
        {
            fileName = "cartoonCraftLanguageSheet/unit_hp-Table 1.csv";
        }
        else if (k == 1)
        {
            fileName = "cartoonCraftLanguageSheet/unit_att-Table 1.csv";
        }
        else if (k == 2)
        {
            fileName = "cartoonCraftLanguageSheet/unit_complete_time-Table 1.csv";
        }
        else if (k == 3)
        {
            fileName = "cartoonCraftLanguageSheet/castle_storage-Table 1.csv";
        }
        else if (k == 4)
        {
            fileName = "cartoonCraftLanguageSheet/building_ability-Table 1.csv";
        }

        csvStr = FileUtils::getInstance()->getStringFromFile(fileName);
        rows = GameManager::getInstance()->split(csvStr, "\r\n");
        keys = GameManager::getInstance()->split(rows.at(0).asString(), ",");
        for (int i = 1; i < (int)rows.size(); i++)
        {
            std::string strRow = rows.at(i).asString();
            ValueVector params = GM->split(rows.at(i).asString(), ",");
            for (int j = 1; j < (int)params.size(); j++)
            {
                std::string value = params.at(j).asString();
                std::string theKey = keys.at(j).asString();
                userParams[theKey] = value;
            }
            std::string rowKey = params.at(0).asString();
            if (k == 0)
            {
                unitHPTable[rowKey] = userParams;
            }
            else if (k == 1)
            {
                unitATTTable[rowKey] = userParams;
            }
            else if (k == 2)
            {
                unitCompleteTimeTable[rowKey] = userParams;
            }
            else if (k == 3)
            {
                castleStorageForCastleLevelTable[rowKey] = userParams;
            }
            else if (k == 4)
            {
                buildingAbilityForCastleLevelTable[rowKey] = userParams;
            }
        }
    }
}

int GameManager::getUnitHP(int index, int level)
{
    string str = unitHPTable[GM->getUnitName(index).c_str()].asValueMap()[Value(level).asString()].asString();
    return Value(str).asInt();
}
int GameManager::getUnitATT(int index, int level)
{
    string str = unitATTTable[GM->getUnitName(index).c_str()].asValueMap()[Value(level).asString()].asString();
    return Value(str).asInt();
}
Movable *GameManager::getUnitFromData(UnitInfo *info)
{
    int eng = getUnitHP(info->unitType, info->level);
    int spd = WORLD->getUnitSP(info->unitType);
    EnemyBase *unit = WORLD->createUnit(info->unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, Vec2(info->x, info->y), getUnitName(info->unitType)); // EnemyBase::createEnemy(info->unitType, eng,spd, WORLD->getSpriteNameForUnit(info->unitType).c_str());
    unit->level = info->level;
    unit->setName(getUnitName(info->unitType));
    unit->buildingCompleteTime = info->endTime;
    unit->buildingCompleteTimeLeft = info->endTime - (double)BSM->getCurrentTimeT();
    unit->energy = getUnitHP(info->unitType, info->level);
    unit->maxEnergy = unit->energy;
    unit->ap = getUnitATT(info->unitType, info->level);
    //    unit->setPosition(Vec2(info->x, info->y));
    return unit;
}
UnitInfo *GameManager::getUnitInfoFromString(std::string str)
{
    ValueVector datas = GM->split(str, "/");
    if (datas.size() < 2)
    {
        return nullptr;
    }
    UnitInfo *info = new UnitInfo();
    info->unitType = datas.at(0).asInt();
    info->level = datas.at(1).asInt() % 100;
    info->rank = datas.at(1).asInt() / 100;
    if (datas.size() > 2)
    {
        info->x = datas.at(2).asInt();
        info->y = datas.at(3).asInt();
    }
    if (datas.size() > 4)
    {
        info->endTime = datas.at(4).asInt();
        info->unitState = datas.at(5).asInt();
    }

    return info;
}
std::string GameManager::getSpineFileName(int unitType)
{
    if (unitType == UNIT_HERO_LIZARDMAN)
    {
        return "lizard";
    }
    else if (unitType == UNIT_HERO_SPEARMAN)
    {
        return "spearMan";
    }
    else if (unitType == UNIT_HERO_WEREWOLF)
    {
        return "werewolf";
    }
    else if (unitType == UNIT_HERO_ORC)
    {
        return "orc";
    }
    else if (unitType == UNIT_HERO_GOBLIN)
    {
        return "goblin";
    }
    else if (unitType == UNIT_HERO_ARCHER)
    {
        return "archer";
    }
    else if (unitType == UNIT_HERO_MONK)
    {
        return "monk";
    }
    else if (unitType == UNIT_HERO_FIGHTER)
    {
        return "fighter";
    }
    else if (unitType == UNIT_HERO_BEAR)
    {
        return "bear";
    }
    else if (unitType == UNIT_HERO_HEALER)
    {
        return "healer";
    }
    else if (unitType == UNIT_HERO_KNIGHT)
    {
        return "knight";
    }
    else if (unitType == UNIT_HERO_ELF_SWORDMAN)
    {
        return "elfSwordMan";
    }
    else if (unitType == UNIT_HERO_ASSASSIN)
    {
        return "assassin";
    }
    else if (unitType == UNIT_HERO_LION)
    {
        return "lion";
    }
    else if (unitType == UNIT_HERO_WIZARD)
    {
        return "wizard";
    }
    else if (unitType == UNIT_HERO_TANKER)
    {
        return "tanker";
    }
    else if (unitType == UNIT_HERO_SKELETON)
    {
        return "skeleton";
    }
    else if (unitType == UNIT_HERO_REAPER)
    {
        return "necromancer";
    }
    else if (unitType == UNIT_HERO_ENT)
    {
        return "ent";
    }
    else if (unitType == UNIT_HERO_SALAMANDER)
    {
        return "salamander";
    }
    else if (unitType == UNIT_HERO_UNDINE)
    {
        return "undine";
    }
    else if (unitType == UNIT_HERO_CRAZY_WEREWOLF)
    {
        return "werewolf";
    }
    else if (unitType == UNIT_HERO_CRAZY_LION)
    {
        return "lion";
    }
    else if (unitType == UNIT_HERO_CRAZY_BEAR)
    {
        return "bear";
    }
    else if (unitType == UNIT_HERO_SANTA)
    {
        return "santa";
    }
    else if (unitType == UNIT_HERO_RUDOLPH)
    {
        return "rudolph";
    }
    else if (unitType == UNIT_HERO_SANTADOG)
    {
        return "santadog";
    }
    else if (unitType == UNIT_HERO_PENGUIN)
    {
        return "penguin";
    }
    else if (unitType == UNIT_HERO_LADY_WEREWOLF)
    {
        return "werewolfFemale";
    }
    else if (unitType == UNIT_HERO_CATINBOOTS)
    {
        return "catinboots";
    }
    else if (unitType == UNIT_HERO_LADY_BEAR)
    {
        return "ladybear";
    }
    else if (unitType == UNIT_HERO_MOLE)
    {
        return "mole";
    }
    else if (unitType == UNIT_HERO_LADY_LION)
    {
        return "femaleLion";
    }
    else if (unitType == UNIT_HERO_TOYMOUSE)
    {
        return "robotMouse";
    }
    else if (unitType == UNIT_HERO_SAVAGEARCHER)
    {
        return "savageArcher";
    }
    else if (unitType == UNIT_HERO_BATMONSTER)
    {
        return "batmonster";
    }
    else if (unitType == UNIT_HERO_MEMEAT)
    {
        return "green_meat";
    }
    else if (unitType == UNIT_HERO_PARASITE)
    {
        return "parasite";
    }
    else if (unitType == UNIT_HERO_WATERMELON)
    {
        return "watermelon";
    }
    else if (unitType == UNIT_HERO_BABYMINO)
    {
        return "minobaby";
    }
    else if (unitType == UNIT_HERO_MINO)
    {
        return "mino";
    }
    else if (unitType == UNIT_HERO_KERBEROS)
    {
        return "kerberos";
    }
    else if (unitType == UNIT_HERO_LAMIA)
    {
        return "lamia";
    }
    else if (unitType == UNIT_HERO_CHUNJA)
    {
        return "chunja";
    }
    else if (unitType == UNIT_HERO_GOLEM)
    {
        return "golem";
    }

    return "";
}
// 게임 내 들어갈 캐릭터 일러스트 제작 요청입니다.
// 원본은 카툰 버전이고 요청하는 이미지는 정상체형(6~8등신)의 실사체 이미지입니다.
//
// 1. 여검사
// 아래는 카툰 버전 이미지 링크입니다.
// https://drive.google.com/file/d/1FB_G4w-YgiUAOLdTWxjR8tivnN7qv5yE/view?usp=sharing
//
// 아래는 전에 메일로 보내드린 포인트 사용처 체크 엑셀파일입니다.
// https://drive.google.com/file/d/10z_CclraSqNU6HvkXMx2QqeXH4wd02Is/view?usp=sharing
std::string GameManager::getUnitName(int index)
{
    //    log("get unit name: %d", index);
    if (index == UNIT_CASTLE ||
        index == UNIT_ZOMBIE_CASTLE)
    {
        log("get unit castlename");
        return "castle";
    }
    else if (index == UNIT_MINE)
    {
        return "mine";
    }
    else if (index == UNIT_TREE_FOR_BATTLE)
    {
        return "tree";
    }
    else if (index == UNIT_FARM)
    {
        return "farm";
    }
    else if (index == UNIT_BARRACKS)
    {
        return "barracks";
    }
    else if (index == UNIT_LUMBERMILL)
    {
        return "lumbermill";
    }
    else if (index == UNIT_FACTORY)
    {
        return "factory";
    }
    else if (index == UNIT_AIRPORT)
    {
        return "airport";
    }
    else if (index == UNIT_WATCHERTOWER)
    {
        return "watcher tower";
    }
    else if (index == UNIT_WORKER)
    {
        log("get unit worker");
        return "worker";
    }
    else if (index == UNIT_GOBLIN_WORKER)
    {
        return "orc worker";
    }
    else if (index == UNIT_SWORDMAN)
    {
        return "swordman";
    }
    else if (index == UNIT_ARCHER)
    {
        return "archer";
    }
    else if (index == UNIT_CATAPULT)
    {
        return "catapult";
    }
    else if (index == UNIT_HELICOPTER)
    {
        return "helicopter";
    }
    else if (index == UNIT_WIZARD)
    {
        return "wizard";
    }
    else if (index == UNIT_UNDERGROUND_BUNKER)
    {
        return "underground bunker";
    }
    else if (index == UNIT_TRIGGER)
    {
        return "trigger";
    }
    else if (index == UNIT_TEMPLE)
    {
        return "temple";
    }
    else if (index == UNIT_BARBECUE)
    {
        return "barbecue";
    }
    else if (index == UNIT_ORC_AXE)
    {
        return "orc axe";
    }
    else if (index == UNIT_ORC_SPEAR)
    {
        return "orc spear";
    }
    else if (index == UNIT_GOBLIN)
    {
        return "goblin";
    }
    else if (index == UNIT_GOBLIN_BOMB)
    {
        return "goblin bomb";
    }
    else if (index == UNIT_TROLL)
    {
        return "troll";
    }
    else if (index == UNIT_ORC_HQ ||
             index == UNIT_ZOMBIE_HQ)
    {
        return "orc hq";
    }
    else if (index == UNIT_ORC_BARRACKS)
    {
        return "orc barracks";
    }
    else if (index == UNIT_ORC_BUNKER)
    {
        return "orc bunker";
    }
    else if (index == UNIT_ORC_TROLL_HOUSE)
    {
        return "orc troll house";
    }
    else if (index == UNIT_ZOMBIE_ORC_AXE)
    {
        return "zombie orc axe";
    }
    else if (index == UNIT_ZOMBIE_SWORDSMAN)
    {
        return "zombie swordman";
    }
    else if (index == UNIT_HERO_ORC)
    {
        return "hero orc";
    }
    else if (index == UNIT_HERO_GOBLIN)
    {
        return "hero goblin";
    }
    else if (index == UNIT_HERO_SPEARMAN)
    {
        return "hero spearman";
    }
    else if (index == UNIT_HERO_LIZARDMAN)
    {
        return "hero lizardman";
    }
    else if (index == UNIT_HERO_WEREWOLF)
    {
        return "hero werewolf";
    }
    else if (index == UNIT_HERO_ARCHER)
    {
        return "hero archer";
    }
    else if (index == UNIT_HERO_MONK)
    {
        return "hero monk";
    }
    else if (index == UNIT_HERO_FIGHTER)
    {
        return "hero fighter";
    }
    else if (index == UNIT_HERO_BEAR)
    {
        return "hero bear";
    }
    else if (index == UNIT_HERO_HEALER)
    {
        return "hero healer";
    }
    else if (index == UNIT_HERO_KNIGHT)
    {
        return "hero knight";
    }
    else if (index == UNIT_HERO_ELF_SWORDMAN)
    {
        return "hero elf swordman";
    }
    else if (index == UNIT_HERO_ASSASSIN)
    {
        return "hero assassin";
    }
    else if (index == UNIT_HERO_LION)
    {
        return "hero lion";
    }
    else if (index == UNIT_HERO_CRAZY_WEREWOLF)
    {
        return "hero crazy werewolf";
    }
    else if (index == UNIT_HERO_CRAZY_BEAR)
    {
        return "hero crazy bear";
    }
    else if (index == UNIT_HERO_CRAZY_LION)
    {
        return "hero crazy lion";
    }
    else if (index == UNIT_HERO_LADY_WEREWOLF)
    {
        return "hero lady werewolf";
    }
    else if (index == UNIT_HERO_LADY_LION)
    {
        return "hero lady lion";
    }
    else if (index == UNIT_HERO_LADY_BEAR)
    {
        return "hero lady bear";
    }
    else if (index == UNIT_HERO_WIZARD)
    {
        return "hero wizard";
    }
    else if (index == UNIT_HERO_TANKER)
    {
        return "hero tanker";
    }
    else if (index == UNIT_HERO_SKELETON)
    {
        return "hero skeleton";
    }
    else if (index == UNIT_HERO_REAPER)
    {
        return "hero reaper";
    }
    else if (index == UNIT_HERO_ENT)
    {
        return "hero ent";
    }
    else if (index == UNIT_HERO_SANTA)
    {
        return "hero santa";
    }
    else if (index == UNIT_HERO_SALAMANDER)
    {
        return "hero salamander";
    }
    else if (index == UNIT_HERO_UNDINE)
    {
        return "hero undine";
    }
    else if (index == UNIT_HERO_RUDOLPH)
    {
        return "hero rudolph";
    }
    else if (index == UNIT_HERO_SANTADOG)
    {
        return "hero santadog";
    }
    else if (index == UNIT_HERO_PENGUIN)
    {
        return "hero penguin";
    }
    else if (index == UNIT_HERO_CATINBOOTS)
    {
        return "hero catinboots";
    }
    else if (index == UNIT_HERO_MOLE)
    {
        return "hero mole";
    }
    else if (index == UNIT_HERO_TOYMOUSE)
    {
        return "hero toymouse";
    }
    else if (index == UNIT_HERO_SAVAGEARCHER)
    {
        return "hero savagearcher";
    }
    else if (index == UNIT_HERO_BATMONSTER)
    {
        return "hero batmonster";
    }
    else if (index == UNIT_HERO_MEMEAT)
    {
        return "hero memeat";
    }
    else if (index == UNIT_HERO_PARASITE)
    {
        return "hero parasite";
    }
    else if (index == UNIT_HERO_WATERMELON)
    {
        return "hero watermelon";
    }
    else if (index == UNIT_HERO_BABYMINO)
    {
        return "hero babymino";
    }
    else if (index == UNIT_HERO_MINO)
    {
        return "hero mino";
    }
    else if (index == UNIT_HERO_KERBEROS)
    {
        return "hero kerberos";
    }
    else if (index == UNIT_HERO_LAMIA)
    {
        return "hero lamia";
    }
    else if (index == UNIT_HERO_CHUNJA)
    {
        return "hero chunja";
    }
    else if (index == UNIT_HERO_GOLEM)
    {
        return "hero golem";
    }
    return "worker";
}
int GameManager::getGemForTimeLeft(int timeLeft)
{
    if (timeLeft < 60 * 2)
    {
        return timeLeft * 120.0f / 3600 + 1;
    }
    else if (timeLeft < 60 * 30)
    {
        return timeLeft * 100.0f / 3600 + 1;
    }
    else if (timeLeft < 60 * 60 * 2)
    {
        return timeLeft * 80.0f / 3600 + 1;
    }
    else if (timeLeft < 60 * 60 * 10)
    {
        return timeLeft * 60.0f / 3600 + 1;
    }
    else if (timeLeft < 60 * 60 * 24)
    {
        return timeLeft * 40.0f / 3600 + 1;
    }
    else
    {
        return timeLeft * 30.0f / 3600;
    }
}
int GameManager::getGemForHour()
{
    return 60;
}
// Make a tm structure representing this date
std::tm make_tm(int year, int month, int day)
{
    std::tm tm = {0};
    tm.tm_year = year - 1900; // years count from 1900
    tm.tm_mon = month - 1;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    return tm;
}
int GameManager::getDayDiff(int fromY, int fromM, int fromD, int toY, int toM, int toD)
{

    // Structures representing the two dates
    std::tm tm1 = make_tm(toY, toM, toD);       // April 2nd, 2012
    std::tm tm2 = make_tm(fromY, fromM, fromD); // February 2nd, 2003

    // Arithmetic time values.
    // On a posix system, these are seconds since 1970-01-01 00:00:00 UTC
    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);

    // Divide by the number of seconds in a day
    const int seconds_per_day = 60 * 60 * 24;
    std::time_t difference = (time1 - time2) / seconds_per_day;

    // To be fully portable, we shouldn't assume that these are Unix time;
    // instead, we should use "difftime" to give the difference in seconds:
    double portable_difference = std::difftime(time1, time2) / seconds_per_day;
    return portable_difference;
}

cocos2d::Size GameManager::getBuildingOccupySize(int unit)
{
    if (unit == UNIT_ORC_BUNKER)
    {
        return cocos2d::Size(2, 2);
    }
    else if (unit == UNIT_ORC_HQ)
    {
        return cocos2d::Size(4, 3);
    }
    else if (unit == UNIT_FACTORY)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_FARM)
    {
        return cocos2d::Size(3, 2);
    }
    else if (unit == UNIT_TREE_FOR_BATTLE ||
             unit == UNIT_TREE)
    {
        return cocos2d::Size(1, 1);
    }
    else if (unit == UNIT_LUMBERMILL)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_AIRPORT)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_CASTLE)
    {
        return cocos2d::Size(4, 3);
    }
    else if (unit == UNIT_MINE)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_BARRACKS)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_WATCHERTOWER)
    {
        return cocos2d::Size(2, 2);
    }
    else if (unit == UNIT_ORC_BARRACKS)
    {
        return cocos2d::Size(2, 2);
    }
    else if (unit == UNIT_TEMPLE)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_ORC_TROLL_HOUSE)
    {
        return cocos2d::Size(3, 3);
    }
    else if (unit == UNIT_BARBECUE)
    {
        return cocos2d::Size(3, 2);
    }
    return cocos2d::Size(1, 1);
}
int GameManager::getFoodUseForUnit(int index)
{
    if (index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER)
    {
        return 1;
    }
    else if (index == UNIT_HELICOPTER)
    {
        return 2;
    }
    else if (index == UNIT_SWORDMAN)
    {
        return 1;
    }
    else if (index == UNIT_ARCHER)
    {
        return 1;
    }
    else if (index == UNIT_CATAPULT)
    {
        return 4;
    }
    else if (index == UNIT_GOBLIN)
    {
        return 1;
    }
    else if (index == UNIT_GOBLIN_BOMB)
    {
        return 5;
    }
    else if (index == UNIT_ORC_AXE)
    {
        return 2;
    }
    else if (index == UNIT_ORC_SPEAR)
    {
        return 2;
    }
    else if (index == UNIT_TROLL)
    {
        return 5;
    }
    else if (index == UNIT_ZOMBIE_ORC_AXE)
    {
        return 1;
    }
    else if (index == UNIT_ZOMBIE_SWORDSMAN)
    {
        return 1;
    }
    else if (index == UNIT_WIZARD)
    {
        return 2;
    }
    return 0;
}
void GameManager::addConsumedItem(std::string strSkuName)
{
    log("consumed name: %s", strSkuName.c_str());
    consumeSkuNameList.push_back(strSkuName);
}
void GameManager::onRestored(std::string strSkuID)
{
    if (strSkuID.compare(IAP_DETAIL_CHAPTER2) == 0)
    {
        UDSetBool(KEY_CHAPTER_2_PURCHASED, true);
    }
    else if (strSkuID.compare(IAP_DETAIL_CHAPTER3) == 0)
    {
        UDSetBool(KEY_CHAPTER_3_PURCHASED, true);
    }
    else if (strSkuID.compare(IAP_DETAIL_PREMIUM_RETRY) == 0)
    {
        UDSetBool(KEY_PREMIUM_START, true);
    }
    GM->iapFlag = IAP_FLAG_RESTORED;
}

void GameManager::rollOpenScroll(ImageView *imgBack)
{
    float originalWidth = imgBack->getContentSize().width;
    imgBack->setContentSize(cocos2d::Size(308, imgBack->getContentSize().height));
    imgBack->runAction(EaseInOut::create(ResizeTo::create(0.3, cocos2d::Size(originalWidth, imgBack->getContentSize().height)), 2));

    for (auto child : imgBack->getParent()->getChildren())
    {
        if (child == imgBack || child->getName().compare("btnBlock") == 0)
            continue;
        child->setOpacity(0);
        child->runAction(Sequence::create(DelayTime::create(0.2), FadeIn::create(0.3), NULL));
    }
}
void GameManager::replaceImageWithSprite(ImageView *img, Sprite *spt)
{
    spt->setPosition(img->getPosition());
    img->getParent()->addChild(spt);
    img->removeFromParent();
}
int GameManager::getIntForOldDouble(std::string key, int defaultValue)
{
    int intValue = UDGetInt(key.c_str(), defaultValue);
    if (intValue != defaultValue)
    {
        return intValue;
    }
    double doubleValue = UserDefault::getInstance()->getDoubleForKey(key.c_str(), defaultValue);
    if (doubleValue != defaultValue)
    {
        UDSetInt(key.c_str(), (int)doubleValue);
        return (int)doubleValue;
    }
    return defaultValue;
}
void GameManager::addLightStormEffect(Node *node)
{
    int frontLightCount = 100;
    float frontLightStartTime = 0.5f;
    float frontLightEndTime = 1.8f;
    float frontLightMoveTimeMin = 0.07f;
    float frontLightMoveTimeMax = 0.4f;
    for (int i = 0; i < frontLightCount; i++)
    {
        Sprite *spt = Sprite::create("whiteBigCircle.png");
        node->addChild(spt, 1000);
        spt->setOpacity(200 + rand() % 55);
        float scale = (rand() % 130 + 70) * 0.02f;
        spt->setScale(1 * scale, 0.5f * scale);
        float radius = spt->getContentSize().width * spt->getScaleX();
        bool isFromRight = rand() % 2 == 0;
        spt->setPosition(Vec2(isFromRight ? (size.width + radius) : -radius, rand() % (int)size.height));
        spt->setColor(Color3B(180 + rand() % 50, 170 + rand() % 50, 80 + rand() % 50));
        spt->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
        float moveScale = (rand() % 30 + 70) * 0.01f;
        spt->runAction(Sequence::create(DelayTime::create(frontLightStartTime + (frontLightEndTime - frontLightStartTime) * (rand() % 100) * 0.01f), MoveBy::create((frontLightMoveTimeMax - frontLightMoveTimeMin) * moveScale + frontLightMoveTimeMin, Vec2((size.width + radius * 2) * (isFromRight ? -1 : 1), 0)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }

    int backLightCount = 100;
    float backLightStartTime = 0;
    float backLightEndTime = 1.2f;
    float backLightMoveTimeMin = 0.1f;
    float backLightMoveTimeMax = 0.7f;
    for (int i = 0; i < backLightCount; i++)
    {
        Sprite *spt = Sprite::create("whiteCircle.png");
        node->addChild(spt, -10);
        spt->setOpacity(150 + rand() % 55);
        spt->setColor(Color3B(180 + rand() % 50, 170 + rand() % 50, 80 + rand() % 50));
        spt->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
        float scale = (rand() % 130 + 70) * 0.01f;
        spt->setScale(1 * scale, 0.5f * scale);
        float radius = spt->getContentSize().width * spt->getScaleX();
        bool isFromRight = rand() % 2 == 0;
        spt->setPosition(Vec2(isFromRight ? (size.width + radius) : -radius, rand() % (int)size.height));
        float moveScale = (rand() % 30 + 70) * 0.01f;
        spt->runAction(Sequence::create(DelayTime::create(backLightStartTime + (backLightEndTime - backLightStartTime) * (rand() % 100) * 0.01f), MoveBy::create((backLightMoveTimeMax - backLightMoveTimeMin) * moveScale + backLightMoveTimeMin, Vec2((size.width + radius * 2) * (isFromRight ? -1 : 1), 0)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }
}

spine::SkeletonAnimation *GameManager::getHeroSpine(int unitType)
{
    std::string strFile = GM->getSpineFileName(unitType);
    spine::SkeletonAnimation *spChar = spine::SkeletonAnimation::createWithJsonFile(strmake("%s.json", strFile.c_str()), strmake("%s.atlas", strFile.c_str()), 1);
    if (unitType == UNIT_HERO_WEREWOLF)
    {
        spChar->setSkin("werewolf");
    }
    else if (unitType == UNIT_HERO_BEAR)
    {
        spChar->setSkin("bear");
    }
    else if (unitType == UNIT_HERO_LION)
    {
        spChar->setSkin("lion");
    }
    else if (unitType == UNIT_HERO_CRAZY_BEAR)
    {
        spChar->setSkin("crazy_bear");
    }
    else if (unitType == UNIT_HERO_CRAZY_WEREWOLF)
    {
        spChar->setSkin("crazy_werewolf");
    }
    else if (unitType == UNIT_HERO_CRAZY_LION)
    {
        spChar->setSkin("crazy_lion");
    }
    spChar->setName(strFile);
    return spChar;
}
int GameManager::getMaxGold(int level)
{
    string str = castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if (str.length() > 0)
    {
        return Value(GM->split(str, "_").at(0)).asInt();
    }
    else
    {
        return 400000;
    }
}
int GameManager::getMaxGold()
{
    return getMaxGold(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int GameManager::getMaxTree()
{
    return getMaxTree(UDGetInt(KEY_CASTLE_LEVEL, 0));
}
int GameManager::getMaxTree(int level)
{
    string str = castleStorageForCastleLevelTable[Value(level).asString().c_str()].asValueMap()["resources"].asString();
    if (str.length() > 0)
    {
        return Value(GM->split(str, "_").at(1)).asInt();
    }
    else
    {
        return 600000;
    }
}
int GameManager::getElement(int unit)
{
    if (unit == UNIT_HERO_WEREWOLF ||
        unit == UNIT_HERO_ARCHER ||
        unit == UNIT_HERO_MONK ||
        unit == UNIT_HERO_ELF_SWORDMAN ||
        unit == UNIT_HERO_CRAZY_WEREWOLF ||
        unit == UNIT_HERO_LADY_WEREWOLF ||
        unit == UNIT_HERO_TANKER ||
        unit == UNIT_HERO_MOLE ||
        unit == UNIT_HERO_ENT ||
        unit == UNIT_HERO_BABYMINO ||
        unit == UNIT_HERO_GOLEM)
    {
        return ELEMENT_GROUND;
    }
    else if (unit == UNIT_HERO_LIZARDMAN ||
             unit == UNIT_HERO_SPEARMAN ||
             unit == UNIT_HERO_ASSASSIN ||
             unit == UNIT_HERO_LADY_BEAR ||
             unit == UNIT_HERO_LADY_LION ||
             unit == UNIT_HERO_SANTA ||
             unit == UNIT_HERO_UNDINE ||
             unit == UNIT_HERO_LAMIA ||
             unit == UNIT_HERO_PENGUIN ||
             unit == UNIT_HERO_SAVAGEARCHER)
    {
        return ELEMENT_WATER;
    }
    else if (unit == UNIT_HERO_GOBLIN ||
             unit == UNIT_HERO_BEAR ||
             unit == UNIT_HERO_LION ||
             unit == UNIT_HERO_WIZARD ||
             unit == UNIT_HERO_SANTADOG ||
             unit == UNIT_HERO_CATINBOOTS ||
             unit == UNIT_HERO_SALAMANDER ||
             unit == UNIT_HERO_TOYMOUSE ||
             unit == UNIT_HERO_MINO ||
             unit == UNIT_HERO_BATMONSTER ||
             unit == UNIT_HERO_KERBEROS ||
             unit == UNIT_HERO_CHUNJA)
    {
        return ELEMENT_FIRE;
    }
    else if (unit == UNIT_HERO_ORC ||
             unit == UNIT_HERO_FIGHTER ||
             unit == UNIT_HERO_CRAZY_BEAR ||
             unit == UNIT_HERO_CRAZY_LION ||
             unit == UNIT_HERO_RUDOLPH ||
             unit == UNIT_HERO_MEMEAT ||
             unit == UNIT_HERO_KNIGHT ||
             unit == UNIT_HERO_PARASITE ||
             unit == UNIT_HERO_WATERMELON)
    {
        return ELEMENT_LIGHTNING;
    }
    else if (unit == UNIT_HERO_SKELETON ||
             unit == UNIT_HERO_REAPER)
    {
        return ELEMENT_DARK;
    }
    else if (unit == UNIT_HERO_HEALER)
    {
        return ELEMENT_LIGHT;
    }
    return ELEMENT_NONE;
}
void GameManager::addYellowRisingBallEffect(Node *parent)
{
    Sprite *spt;
    for (int i = 0; i < 50; i++)
    {
        if (rand() % 2 == 0)
        {
            spt = Sprite::create("whiteSmallCircle.png");
        }
        else
        {
            spt = Sprite::create("whiteGlowCircle.png");
        }
        parent->addChild(spt);
        spt->setScale((rand() % 10) * 0.1f);
        int opa = rand() % 200;
        spt->setOpacity(opa);
        Vec2 pos = Vec2(rand() % (int)parent->getBoundingBox().size.width, rand() % (int)parent->getBoundingBox().size.height);
        spt->setPosition(pos);
        float riseDur = (rand() % 10) * 0.1f + 1;
        spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(riseDur, Vec2(0, 60 + rand() % 60)), MoveTo::create(0, pos), nullptr)));
        if (rand() % 2 == 0)
        {
            spt->runAction(RepeatForever::create(Sequence::create(FadeTo::create(riseDur / 4, opa), DelayTime::create(riseDur / 4), FadeOut::create(riseDur / 2), nullptr)));
        }
        else
        {
            spt->runAction(RepeatForever::create(Sequence::create(FadeIn::create(riseDur / 4), DelayTime::create(riseDur / 4), FadeOut::create(riseDur / 2), nullptr)));
        }
        spt->setColor(Color3B(255, 251, 141));
        spt->setBlendFunc(BlendFunc::ADDITIVE);
    }
}
Color3B GameManager::getRankColor(int rank)
{
    if (rank == 0)
    {
        return Color3B(255, 255, 255);
    }
    else if (rank == 1)
    {
        return Color3B(57, 211, 205);
    }
    else if (rank == 2)
    {
        return Color3B(0, 180, 241);
    }
    else if (rank == 3)
    {
        return Color3B(255, 148, 183);
    }
    else if (rank == 4)
    {
        return Color3B(179, 22, 135);
    }
    return Color3B::WHITE;
}
void GameManager::setTextToNumberByTag(Ref *ref)
{
    Text *lbl = (Text *)ref;
    int originalNumber = Value(lbl->getString()).asInt();
    int targetNumber = lbl->getTag();
    int diff = 0;
    if (targetNumber - originalNumber > 100)
    {
        diff = 100;
    }
    else if (targetNumber - originalNumber > 10)
    {
        diff = 10;
    }
    else if (targetNumber - originalNumber > 0)
    {
        diff = 1;
    }
    else if (targetNumber - originalNumber < -100)
    {
        diff = -100;
    }
    else if (targetNumber - originalNumber < -10)
    {
        diff = -10;
    }
    else if (targetNumber - originalNumber < 0)
    {
        diff = -1;
    }
    originalNumber += diff;
    lbl->setString(Value(originalNumber).asString());
    if (originalNumber != targetNumber)
    {
        lbl->runAction(CallFuncN::create(CC_CALLBACK_1(GameManager::setTextToNumberByTag, GM)));
    }
}
void GameManager::addGlowBack(Node *node)
{
    int glowCount = 5;
    float width = node->getContentSize().width;
    for (int i = 0; i < glowCount; i++)
    {
        Sprite *spt = Sprite::create("whiteBigCircle.png");
        spt->setScale(width * 1.7f / spt->getContentSize().width, width * 0.45f / spt->getContentSize().width);
        node->addChild(spt, -1);
        spt->setColor(Color3B::YELLOW);
        spt->setBlendFunc(BlendFunc::ADDITIVE);
        spt->setRotation(i * 360 / glowCount);
        spt->runAction(RepeatForever::create(RotateBy::create(1, 40)));
        spt->setOpacity(210);
        spt->setPosition(node->getContentSize() / 2);
    }

    for (int i = 0; i < glowCount; i++)
    {
        Sprite *spt = Sprite::create("whiteBigCircle.png");
        spt->setScale(width * 2.8f / spt->getContentSize().width, width * 0.8f / spt->getContentSize().width);
        node->addChild(spt, -1);
        spt->setColor(Color3B::YELLOW);
        spt->setBlendFunc(BlendFunc::ADDITIVE);
        spt->setRotation(i * 360 / glowCount);
        spt->setOpacity(120);
        spt->runAction(RepeatForever::create(RotateBy::create(1, -20)));
        spt->setPosition(node->getContentSize() / 2);
    }
}
int GameManager::getHeroSkillRate(int unitType, int rank)
{
    if (unitType == UNIT_HERO_HEALER)
    {
        return 35 + 3 * rank;
    }
    else if (unitType == UNIT_HERO_UNDINE)
    {
        return 25 + 3 * rank;
    }
    else if (unitType == UNIT_HERO_PENGUIN ||
             unitType == UNIT_HERO_CATINBOOTS ||
             unitType == UNIT_HERO_MOLE)
    {
        return 20 + 3 * rank;
    }
    else
    {
        return 10 + 3 * rank;
    }
}
// std::string GameManager::getMonthlyEventInfoString(){
//  first item is [month/receive count]
//  -1 is received reward
//    return UDGetStr(KEY_MONTHLY_EVENT_INFO, "-1/0_0_0_0_0_0_0_0_0_0_0_0");
//}
void GameManager::addMonthlyEventProgress(int index, int count)
{
    int current = UDGetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, index).c_str(), 0);
    if (current < 0)
    {
        return;
    }
    current += count;
    UDSetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, index).c_str(), current);
    //    std::string strData = getMonthlyEventInfoString();
    //    checkIsNewMonthlyEvent();
    //    ValueVector list = GM->split(strData, "_");
    //    int missionCount = 12;
    //    int values[missionCount];
    //    for (int i = 0; i < missionCount; i++) {
    //        values[i] = 0;
    //    }
    //    bool isSomthingAdded = false;
    //    for (int i = 0; i < missionCount; i++) {
    //        values[i] = list.at(i + 1).asInt();
    //        if (index == i && getMonthlyEventProgressMax(index) > values[i] && values[i] >= 0) {
    //            values[i] += count;
    //            BSM->isThereSomethingToSaveForMonthlyEventProgress = true;
    //            isSomthingAdded = true;
    //            break;
    //        }
    //    }
    //    if (isSomthingAdded) {
    //        strData = list.at(0).asString();
    //        for (int i = 0; i < 12; i++) {
    //            strData += "_" + Value(values[i]).asString();
    //        }
    //        UDSetStr(KEY_MONTHLY_EVENT_INFO, strData);
    //    }
    //
    //    return values[index];
}
void GameManager::resetMonthlyEventProgress(int receivedCount)
{
    //    UDSetStr(KEY_MONTHLY_EVENT_INFO, strmake("%d/%d_0_0_0_0_0_0_0_0_0_0_0", BSM->getMonth(), receivedCount));
    //    BSM->isThereSomethingToSaveForMonthlyEventProgress = true;
    UDSetInt(KEY_MONTHLY_EVENT_INFO_MONTH, BSM->getMonth());
    UDSetInt(KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT, receivedCount);
    for (int i = 0; i < EVENT_MISSION_TOTAL_COUNT; i++)
    {
        UDSetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT, i).c_str(), 0);
    }
}
int GameManager::getMonthlyEventProgressMax(int index)
{
    if (index == EVENT_MISSION_ATTEND)
    {
        return 12;
    }
    else if (index == EVENT_MISSION_KILL_SWORDSMAN)
    {
        return 80;
    }
    else if (index == EVENT_MISSION_KILL_GOBLIN)
    {
        return 200;
    }
    else if (index == EVENT_MISSION_KILL_ORC_AXE)
    {
        return 150;
    }
    else if (index == EVENT_MISSION_KILL_ORC_SPEAR)
    {
        return 150;
    }
    else if (index == EVENT_MISSION_KILL_TROLL)
    {
        return 50;
    }
    else if (index == EVENT_MISSION_KILL_ZOMBIE)
    {
        return 400;
    }
    else if (index == EVENT_MISSION_ARENA_CLEAR)
    {
        return 14;
    }
    else if (index == EVENT_MISSION_PVP_6_WIN)
    {
        return 40;
    }
    else if (index == EVENT_MISSION_PVP_12_WIN)
    {
        return 40;
    }
    else if (index == EVENT_MISSION_BATTLE_NETWORK_WIN)
    {
        return 50;
    }
    else if (index == EVENT_MISSION_COLLECT_EVENT_HERO_PART)
    {
        return 30;
    }
    return 100;
}
void GameManager::checkIsNewMonthlyEvent()
{
    if (!BSM->timeEstablished)
    {
        return;
    }
    //    std::string strData = getMonthlyEventInfoString();
    //    ValueVector list = GM->split(strData, "_");
    int month = UDGetInt(KEY_MONTHLY_EVENT_INFO_MONTH, -1);
    //    if (list.at(0).asInt() != BSM->getMonth()) {
    if (month != BSM->getMonth())
    {
        resetMonthlyEventProgress(0);
    }
}
int GameManager::getTimeLeftForCampaignChest(int index)
{
    //    std::string strCampaignChestGetTime = UDGetStr(strmake(KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT, index).c_str(), "2019-10-30T15:46:00.933Z");
    //    time_t getTime = BSM->getTimeTFromStr(strCampaignChestGetTime);
    time_t getTime = getSavedTime(strmake(KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT, index).c_str());
    time_t now = BSM->getCurrentTimeT();
    if (now < 0)
    {
        BSM->getHttpTime();
        return 60 * 60 * 3;
    }
    int timeLeft = (int)difftime(getTime + 60 * 60 * 3, now);
    if (timeLeft > 60 * 60 * 4)
    {
        GM->saveTime(strmake(KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT, index).c_str(), now + 60 * 60 * 3);
        getTime = getSavedTime(strmake(KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT, index).c_str());
        timeLeft = (int)difftime(getTime + 60 * 60 * 3, now);
    }
    return timeLeft;
}

void GameManager::saveTime(const char *key, time_t time)
{
    std::string strTime = BSM->getStrFromTime(time);
    std::string keyYear = strmake("%s_year", key);
    std::string keyMonth = strmake("%s_month", key);
    std::string keyDay = strmake("%s_day", key);
    std::string keyHour = strmake("%s_hour", key);
    std::string keyMin = strmake("%s_min", key);
    std::string keySec = strmake("%s_sec", key);
    if (strTime.size() < 19)
    {
        UDSetInt(keyYear.c_str(), 0);
        UDSetInt(keyMonth.c_str(), 0);
        UDSetInt(keyDay.c_str(), 0);
        UDSetInt(keyHour.c_str(), 0);
        UDSetInt(keyMin.c_str(), 0);
        UDSetInt(keySec.c_str(), 0);
        return;
    }
    std::string year = Value(strTime.substr(0, 4)).asString();
    std::string month = Value(strTime.substr(5, 2)).asString();
    std::string day = Value(strTime.substr(8, 2)).asString();

    std::string hour = Value(strTime.substr(11, 2)).asString();
    std::string min = Value(strTime.substr(14, 2)).asString();
    std::string sec = Value(strTime.substr(17, 2)).asString();

    UDSetInt(keyYear.c_str(), Value(year).asInt() - 1900);
    UDSetInt(keyMonth.c_str(), Value(month).asInt() - 1);
    UDSetInt(keyDay.c_str(), Value(day).asInt());
    UDSetInt(keyHour.c_str(), Value(hour).asInt());
    UDSetInt(keyMin.c_str(), Value(min).asInt());
    UDSetInt(keySec.c_str(), Value(sec).asInt());
}

time_t GameManager::getSavedTime(const char *key)
{

    std::string keyYear = strmake("%s_year", key);
    std::string keyMonth = strmake("%s_month", key);
    std::string keyDay = strmake("%s_day", key);
    std::string keyHour = strmake("%s_hour", key);
    std::string keyMin = strmake("%s_min", key);
    std::string keySec = strmake("%s_sec", key);

    //    std::string year = Value(strTime.substr(0, 4)).asString();
    //    std::string month = Value(strTime.substr(5, 2)).asString();
    //    std::string date = Value(strTime.substr(8, 2)).asString();
    //
    //    std::string hour = Value(strTime.substr(11, 2)).asString();
    //    std::string min = Value(strTime.substr(14, 2)).asString();
    //    std::string sec = Value(strTime.substr(17, 2)).asString();

    struct tm theTime;
    theTime.tm_year = UDGetInt(keyYear.c_str(), 0);
    theTime.tm_mon = UDGetInt(keyMonth.c_str(), 0);
    theTime.tm_mday = UDGetInt(keyDay.c_str(), 0);
    theTime.tm_hour = UDGetInt(keyHour.c_str(), 0);
    theTime.tm_min = UDGetInt(keyMin.c_str(), 0);
    theTime.tm_sec = UDGetInt(keySec.c_str(), 0);
    theTime.tm_isdst = 0;

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    time_t what = _mkgmtime(&theTime);
    #else
    time_t what = timegm(&theTime); // UTC time - 1 hour can diff for summertime
    #endif
    return what;
}
void GameManager::saveUnitInfo(const char *key, UnitInfo *info)
{
    if (info->rank > 4)
    {
        info->rank = 4;
    }
    UDSetInt(strmake("%s_unitType", key).c_str(), info->unitType);
    UDSetInt(strmake("%s_levelAndRank", key).c_str(), info->level + info->rank * 100);
    UDSetInt(strmake("%s_x", key).c_str(), info->x);
    UDSetInt(strmake("%s_y", key).c_str(), info->y);
}
UnitInfo *GameManager::getSavedUnitInfo(const char *key)
{
    UnitInfo *info = new UnitInfo();
    info->unitType = UDGetInt(strmake("%s_unitType", key).c_str());
    int value = UDGetInt(strmake("%s_levelAndRank", key).c_str());
    info->level = value % 100;
    info->rank = value / 100;
    info->x = UDGetInt(strmake("%s_x", key).c_str());
    info->y = UDGetInt(strmake("%s_y", key).c_str());
    return info;
}
std::vector<UnitInfo *> GameManager::getHeroInventory()
{
    int count = UDGetInt(KEY_UNITS_HERO_INVENTORY_COUNT, 0);
    std::vector<UnitInfo *> list;
    for (int i = 0; i < count; i++)
    {
        list.push_back(getSavedUnitInfo(strmake(KEY_UNITS_HERO_INVENTORY_FORMAT, i).c_str()));
    }
    return list;
}
void GameManager::saveHeroInventory(std::vector<UnitInfo *> list)
{
    int count = (int)list.size();
    UDSetInt(KEY_UNITS_HERO_INVENTORY_COUNT, count);
    for (int i = 0; i < count; i++)
    {
        UnitInfo *info = list.at(i);
        saveUnitInfo(strmake(KEY_UNITS_HERO_INVENTORY_FORMAT, i).c_str(), info);
    }
    UD->flush();
    list.clear();
}
std::vector<UnitInfo *> GameManager::getHeroDeck()
{
    int count = UDGetInt(KEY_UNITS_HERO_DECK_COUNT, 0);
    std::vector<UnitInfo *> list;
    for (int i = 0; i < count; i++)
    {
        list.push_back(getSavedUnitInfo(strmake(KEY_UNITS_HERO_DECK_FORMAT, i).c_str()));
    }
    return list;
}
void GameManager::saveHeroDeck(std::vector<UnitInfo *> list)
{
    int count = (int)list.size();
    UDSetInt(KEY_UNITS_HERO_DECK_COUNT, count);
    for (int i = 0; i < count; i++)
    {
        UnitInfo *info = list.at(i);
        saveUnitInfo(strmake(KEY_UNITS_HERO_DECK_FORMAT, i).c_str(), info);
    }
    UD->flush();
}

std::vector<UnitInfo *> GameManager::getBattleUnitInventory()
{
    int count = UDGetInt(KEY_UNITS_INVENTORY_COUNT, 0);
    std::vector<UnitInfo *> list;
    for (int i = 0; i < count; i++)
    {
        list.push_back(getSavedUnitInfo(strmake(KEY_UNITS_INVENTORY_FORMAT, i).c_str()));
    }
    return list;
}
void GameManager::saveBattleUnitInventory(std::vector<UnitInfo *> list)
{

    int count = (int)list.size();
    UDSetInt(KEY_UNITS_INVENTORY_COUNT, count);
    for (int i = 0; i < count; i++)
    {
        UnitInfo *info = list.at(i);
        saveUnitInfo(strmake(KEY_UNITS_INVENTORY_FORMAT, i).c_str(), info);
    }
    UD->flush();
}
std::vector<UnitInfo *> GameManager::getBattleUnitDeck()
{
    int count = UDGetInt(KEY_UNITS_DECK_COUNT, 0);
    std::vector<UnitInfo *> list;
    for (int i = 0; i < count; i++)
    {
        list.push_back(getSavedUnitInfo(strmake(KEY_UNITS_DECK_FORMAT, i).c_str()));
    }
    return list;
}
void GameManager::saveBattleUnitDeck(std::vector<UnitInfo *> list)
{
    int count = (int)list.size();
    UDSetInt(KEY_UNITS_DECK_COUNT, count);
    if (count == 0)
    {
        log("save deck count 0");
    }
    for (int i = 0; i < count; i++)
    {
        UnitInfo *info = list.at(i);
        saveUnitInfo(strmake(KEY_UNITS_DECK_FORMAT, i).c_str(), info);
    }
    //    UD->flush();
}
int GameManager::getDailyMissionCampaignStageIndex()
{
    int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
    int stage = 1;
    int dayCount = UDGetInt(KEY_DAY_COUNT, 0);
    if (lastClearStage >= 35)
    { // if all clear and 35 pixed.
        stage = dayCount % 36;
    }
    else
    {
        stage = lastClearStage + 1;
    }

    return stage;
}
int GameManager::getMonthlyHeroType()
{
    if (BSM->month == 1)
    {
        return UNIT_HERO_SALAMANDER;
    }
    else if (BSM->month == 2)
    {
        return UNIT_HERO_UNDINE;
    }
    else if (BSM->month == 3)
    {
        return UNIT_HERO_LADY_WEREWOLF;
    }
    else if (BSM->month == 4)
    {
        return UNIT_HERO_LADY_BEAR;
    }
    else if (BSM->month == 5)
    {
        return UNIT_HERO_LADY_LION;
    }
    else if (BSM->month == 6)
    {
        return UNIT_HERO_SAVAGEARCHER;
    }
    else if (BSM->month == 7)
    {
        return UNIT_HERO_MEMEAT;
    }
    else if (BSM->month == 8)
    {
        return UNIT_HERO_WATERMELON;
    }
    else if (BSM->month == 9)
    {
        return UNIT_HERO_MINO;
    }
    else if (BSM->month == 10)
    {
        return UNIT_HERO_LAMIA;
    }
    else if (BSM->month == 11)
    {
        return UNIT_HERO_ENT;
    }
    else if (BSM->month == 12)
    {
        return UNIT_HERO_SANTA;
    }
    return -1;
}
int GameManager::getMidMonthHeroType()
{
    int month = BSM->month;
    int day = BSM->day;
    if ((month == 12 && day > 15) || (month == 1 && day <= 15))
    {
        return UNIT_HERO_RUDOLPH;
    }
    else if ((month == 1 && day > 15) || (month == 2 && day <= 15))
    {
        return UNIT_HERO_SANTADOG;
    }
    else if ((month == 2 && day > 15) || (month == 3 && day <= 15))
    {
        return UNIT_HERO_PENGUIN;
    }
    else if ((month == 3 && day > 15) || (month == 4 && day <= 15))
    {
        return UNIT_HERO_CATINBOOTS;
    }
    else if ((month == 4 && day > 15) || (month == 5 && day <= 15))
    {
        return UNIT_HERO_MOLE;
    }
    else if ((month == 5 && day > 15) || (month == 6 && day <= 15))
    {
        return UNIT_HERO_TOYMOUSE;
    }
    else if ((month == 6 && day > 15) || (month == 7 && day <= 15))
    {
        return UNIT_HERO_BATMONSTER;
    }
    else if ((month == 7 && day > 15) || (month == 8 && day <= 15))
    {
        return UNIT_HERO_PARASITE;
    }
    else if ((month == 8 && day > 15) || (month == 9 && day <= 15))
    {
        return UNIT_HERO_BABYMINO;
    }
    else if ((month == 9 && day > 15) || (month == 10 && day <= 15))
    {
        return UNIT_HERO_KERBEROS;
    }
    else if ((month == 10 && day > 15) || (month == 11 && day <= 15))
    {
        return UNIT_HERO_CHUNJA;
    }
    else if ((month == 11 && day > 15) || (month == 12 && day <= 15))
    {
        return UNIT_HERO_GOLEM;
    }
    return -1;
}
int GameManager::getMidMonthHeroTimeLeft()
{
    time_t now = BSM->getCurrentTimeT();
    int month = BSM->getMonth();
    int day = BSM->getDay();
    int lastDay = BSM->getLastDayOfMonth(month);
    int timeLeft = 86400 - (long)now % 86400;
    if (day > 15)
    {
        timeLeft += (lastDay - day + 14) * (60 * 60 * 24);
    }
    else
    {
        timeLeft += (15 - day) * (60 * 60 * 24);
    }
    return timeLeft;
}
void GameManager::resetAsset()
{
    //    Director::getInstance()->purgeCachedData();
    //    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    //        cache->addSpriteFramesWithFile("CartoonCraftNV.plist");
    //        cache->addSpriteFramesWithFile("effect.plist");
    //    //    cache->addSpriteFramesWithFile("cartoonCraft.plist");
    //    //    cache->addSpriteFramesWithFile("cartoonCraftBuilding.plist");
    //    //    cache->addSpriteFramesWithFile("cartoonCraftEffect.plist");
    //        cache->addSpriteFramesWithFile("CartoonCraftNumber.plist");
}
int GameManager::getUnitAP(int unit)
{
    if (unit == UNIT_WORKER)
    {
        return 3;
    }
    else if (unit == UNIT_GOBLIN_WORKER)
    {
        return 3;
    }
    else if (unit == UNIT_ARCHER)
    {
        return 13;
    }
    else if (unit == UNIT_HELICOPTER)
    {
        return 10;
    }
    else if (unit == UNIT_SWORDMAN)
    {
        return 10;
    }
    else if (unit == UNIT_WATCHERTOWER)
    {
        return 15;
    }
    else if (unit == UNIT_CATAPULT)
    {
        return 40;
    }
    else if (unit == UNIT_GOBLIN)
    {
        return 8;
    }
    else if (unit == UNIT_GOBLIN_BOMB)
    {
        return 200;
    }
    else if (unit == UNIT_TROLL)
    {
        return 25;
    }
    else if (unit == UNIT_ORC_AXE)
    {
        return 13;
    }
    else if (unit == UNIT_ORC_SPEAR)
    {
        return 18;
    }
    else if (unit == UNIT_ORC_BUNKER)
    {
        return 18;
    }
    else if (unit == UNIT_ORC_HQ)
    {
        return 18;
    }
    else if (unit == UNIT_ZOMBIE_SWORDSMAN)
    {
        return 10;
    }
    else if (unit == UNIT_ZOMBIE_ORC_AXE)
    {
        return 12;
    }
    else if (unit == UNIT_WIZARD)
    {
        return 40;
    }
    else if (unit == UNIT_HERO_ORC)
    {
        return 25;
    }
    return 5;
}

int GameManager::getUnitMaxHP(int unit)
{
    if (unit == UNIT_WORKER || unit == UNIT_GOBLIN_WORKER)
    {
        return 60;
    }
    else if (unit == UNIT_ARCHER)
    {
        return 90;
    }
    else if (unit == UNIT_SWORDMAN)
    {
        return 150;
    }
    else if (unit == UNIT_CATAPULT)
    {
        return 400;
    }
    else if (unit == UNIT_HELICOPTER)
    {
        return 70;
    }
    else if (unit == UNIT_CASTLE || unit == UNIT_ZOMBIE_CASTLE)
    {
        return 4000;
    }
    else if (unit == UNIT_FARM)
    {
        return 700;
    }
    else if (unit == UNIT_BARRACKS)
    {
        return 1400;
    }
    else if (unit == UNIT_LUMBERMILL)
    {
        return 1000;
    }
    else if (unit == UNIT_FACTORY)
    {
        return 1500;
    }
    else if (unit == UNIT_AIRPORT)
    {
        return 2000;
    }
    else if (unit == UNIT_WATCHERTOWER)
    {
        return 500;
    }
    else if (unit == UNIT_BARBECUE)
    {
        return 400;
    }
    else if (unit == UNIT_ORC_BUNKER)
    {
        return 800;
    }
    else if (unit == UNIT_ORC_HQ || unit == UNIT_ZOMBIE_HQ)
    {
        return 2000;
    }
    else if (unit == UNIT_TREE)
    {
        return 20; // test
    }
    else if (unit == UNIT_MINE)
    {
        return 50000; // test
                      //        return 50; // test
    }
    else if (unit == UNIT_TROLL)
    {
        return 580;
    }
    else if (unit == UNIT_GOBLIN || unit == UNIT_GOBLIN_BOMB)
    {
        return 80;
    }
    else if (unit == UNIT_ORC_AXE)
    {
        return 180;
    }
    else if (unit == UNIT_ORC_SPEAR)
    {
        return 120;
    }
    else if (unit == UNIT_ZOMBIE_ORC_AXE)
    {
        return 150;
    }
    else if (unit == UNIT_ZOMBIE_SWORDSMAN)
    {
        return 120;
    }
    else if (unit == UNIT_WIZARD)
    {
        return 40;
    }
    else if (unit == UNIT_HERO_ORC)
    {
        return 550;
    }
    return 60;
}
bool GameManager::isThisBuilding(int unitType)
{
    if (unitType == UNIT_MINE ||
        unitType == UNIT_ZOMBIE_CASTLE ||
        unitType == UNIT_ZOMBIE_HQ ||
        unitType == UNIT_FARM ||
        unitType == UNIT_CASTLE ||
        unitType == UNIT_BARRACKS ||
        unitType == UNIT_WATCHERTOWER ||
        unitType == UNIT_LUMBERMILL ||
        unitType == UNIT_FACTORY ||
        unitType == UNIT_AIRPORT ||
        unitType == UNIT_BARBECUE ||
        unitType == UNIT_ORC_HQ ||
        unitType == UNIT_ORC_BARRACKS ||
        unitType == UNIT_ORC_BUNKER ||
        unitType == UNIT_ORC_TROLL_HOUSE ||
        unitType == UNIT_TEMPLE)
    {
        return true;
    }
    return false;
}
