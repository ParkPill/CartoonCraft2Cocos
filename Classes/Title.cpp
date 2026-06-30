//
//  Title.cpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 13/03/2018.
//
#include "Title.h"
#include "BuggyServerManager.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "Intro.h"
#include "LanguageManager.h"
#include <editor-support/spine/SkeletonAnimation.h>
#include <editor-support/spine/spine-cocos2dx.h>

// #include "CrossPromotion.h"
// #include "NativeInterface.h"
#include "ChatRoom.h"
#include "EventPopup.h"
#include "HeroPage.h"
#include "MultiplayLobby.h"
#include "ShopLayer.h"
#include "UploadedMaps.h"
#include "MapEditor.h"

bool Title::usesAccountSystem() { return !GameManager::isWin32Offline(); }

bool Title::init() {
  //    UDSetInt(KEY_GOLDEN_TICKET, 0); // test
  //    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime() - 30); // test

  GM->setHudLayer(nullptr);
  //////////////////////////////
  // 1. super init first
  if (!PageBase::init()) {
    return false;
  }
  difficultyMode = UDGetInt(KEY_DIFFICULTY_MODE, DIFFICULTY_MODE_NORMAL);
  LayerColor *colorLayer = LayerColor::create(Color4B(235, 229, 210, 255));
  this->addChild(colorLayer, -1);
  GM->titleLayer = this;
  size = Director::getInstance()->getWinSize();
  float idleTime = 0.5f;
  float moveTime = 1;

  Node *title = CSLoader::createNodeWithVisibleSize("Title.csb");
  titleLayer = title;
  this->addChild(title);
  title->setName("titleLayer");
  title->setPositionX(size.width / 2 - title->getContentSize().width / 2);
  Node *sptBackground = title->getChildByName(
      "btnBackground"); // Sprite::create("titleBackground.png");
  //    this->addChild(sptBackground);
  //    sptBackground->setPosition(Vec2(size.width/2, size.height/2 - 300));
  //    sptBackground->setScale(defaultScale*2224.0f/2048);
  sptBackground->runAction(Sequence::create(
      DelayTime::create(idleTime),
      EaseInOut::create(MoveBy::create(moveTime, Vec2(0, -30)), 3), NULL));

  sptTitle = Sprite::create("title.png");
  title->addChild(sptTitle);
  sptTitle->setPosition(Vec2(size.width / 2 - 420, size.height / 2 + 280));
  sptTitle->setScale(defaultScale);
  sptTitle->runAction(Sequence::create(
      DelayTime::create(idleTime),
      EaseInOut::create(MoveBy::create(moveTime, Vec2(-200, 100)), 3), NULL));

  //    float btnX = 570;
  Button *btn = (Button *)title->getChildByName(
      "btnPlay"); // Button::create("btnBox.png");
  //    this->addChild(btn);
  //    btn->setPosition(Vec2(btnX, 680));
  btn->addClickEventListener(CC_CALLBACK_0(Title::showChapterSelect, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  PPLabel *lbl = addLabelToButton(btn, "play", 60, Color3B(4, 90, 4));
  lbl->setPosition(lbl->getPosition() + Vec2(-110, 10));
  doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);

  btn->getChildByName("imgRedDot")->setVisible(false);

  btn = (Button *)title->getChildByName("btnMap");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onMapClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  btn->setPosition(title->getChildByName("btnMultiplay")->getPosition());
  btn->setContentSize(title->getChildByName("btnMultiplay")->getContentSize());
  for (auto child : btn->getChildren()) {
    child->setPosition(child->getPosition() + Vec2(100, 0));
  }
  lbl = addLabelToButton(btn, "map editor", 60, Color3B(4, 90, 4));
  lbl->setPosition(lbl->getPosition() + Vec2(-130, 10));
  doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);

  btn = (Button *)title->getChildByName("btnUploaded");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onUploadedClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  btn->setPosition(title->getChildByName("btnMultiplay")->getPosition() +
                   Vec2(650, 0));
  btn->setContentSize(title->getChildByName("btnMultiplay")->getContentSize());
  // print child names
  for (auto child : btn->getChildren()) {
    child->setPosition(child->getPosition() + Vec2(100, 0));
  }
  lbl = addLabelToButton(btn, "use map", 60, Color3B(4, 90, 4));
  lbl->setPosition(lbl->getPosition() + Vec2(-130, 10));
  doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);

  btn = (Button *)title->getChildByName(
      "btnHero"); // Button::create("btnBox.png");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  lbl = addLabelToButton(btn, "hero", 60, Color3B(4, 90, 4));
  lbl->setPosition(lbl->getPosition() + Vec2(-110, 10));
  doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
  btn->setVisible(false); // test

  btn = (Button *)title->getChildByName("btnEvent");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onEventClick, this));
  //    btn->setVisible(false);
  GM->addGlowBack(btn);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  btn->setVisible(false);
#endif

  btn = (Button *)title->getChildByName("btnServer");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onServerSelect, this));

  btn = (Button *)title->getChildByName("btnChat");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onChatClick, this));

  btn = (Button *)title->getChildByName("btnCommunity");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onCommunityClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  if (LM->getLanguageType() == LanguageType::KOREAN) {

  } else {
    btn->setVisible(false);
  }

  btn = (Button *)title->getChildByName("btnFacebook");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onFacebookClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  if (GM->market == MARKET_SMARTPASS) {
    btn->setVisible(false);
  }
  btn->setVisible(false);
  //    Sprite* spt = Sprite::create("weaponX.png");
  //    btn->addChild(spt);
  //    spt->setPosition(Vec2(590, 135));
  //    spt->setOpacity(0);
  //    spt->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));

  btn = (Button *)title->getChildByName(
      "btnArena"); // Button::create("btnBox.png");
  btn->setVisible(false);
  // //    this->addChild(btn);
  // //    btn->setPosition(Vec2(btnX, 415));
  // btn->addClickEventListener(CC_CALLBACK_0(Title::onArenaClick, this));
  // btn->setOpacity(0);
  // btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //                                 FadeIn::create(0.5f), NULL));
  // //    lbl = addLabelToButton(btn, "arena", 60, Color3B(4, 90, 4));
  // //    lbl->setPosition(lbl->getPosition() + Vec2(-110, 10));
  // //    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);

  // btn = (Button *)title->getChildByName("btnMultiplay");
  // btn->addClickEventListener(CC_CALLBACK_0(Title::onMultiplayClick, this));
  // btn->setOpacity(0);
  // btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //                                 FadeIn::create(0.5f), NULL));
  // lbl = addLabelToButton(btn, "multiplay", 40, Color3B(4, 90, 4));
  // lbl->setPosition(lbl->getPosition() + Vec2(0, 10));
  // doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
  // btn->getChildByName("img0")->setVisible(false);

  btn = (Button *)title->getChildByName("btnMultiplay");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onArenaClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  lbl = addLabelToButton(btn, "arena", 40, Color3B(4, 90, 4));
  lbl->setPosition(lbl->getPosition() + Vec2(0, 10));
  doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);
  btn->getChildByName("img0")->setVisible(false);
  btn->setPosition(
      Vec2(title->getChildByName("btnMultiplay")->getPosition().x,
           title->getChildByName("btnCommunity")->getPosition().y));

  Sprite *spt;
  //    spt = Sprite::create("comingSoon.png");
  //    btn->addChild(spt);
  //    spt->setPosition(Vec2(800, 225));
  //    spt->setOpacity(0);
  //    spt->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));

  btn = (Button *)title->getChildByName("btnYoutube");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onYoutubeChannelClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));

  float tagScale = 0.7f;

  //    Sprite* spt1 = Sprite::create("networkTag.png");
  //    btn->addChild(spt1);
  //    spt1->setScale(tagScale);
  //    spt1->setPosition(Vec2(590, 170));
  //    spt1->setOpacity(0);
  //    spt1->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));
  //
  //    Sprite* spt2 = Sprite::create("nonMultiplayTag.png");
  //    btn->addChild(spt2);
  //    spt2->setScale(tagScale);
  //    spt2->setPosition(Vec2(590, 110));
  //    spt2->setOpacity(0);
  //    spt2->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));

  btn = (Button *)title->getChildByName(
      "btnBattle"); // Button::create("btnBox.png");
  // Lobby/Raid battle mode has been removed - this button has no destination
  // anymore, so it stays hidden on every platform (it used to be Win32-only).
  btn->setVisible(false);
  btn->setEnabled(false);

  //    Sprite* sptComingSoonTag = Sprite::create("comingSoon.png");
  //    btn->addChild(sptComingSoonTag);
  //    sptComingSoonTag->setPosition(Vec2(610, 144));
  //    sptComingSoonTag->setOpacity(0);
  //    sptComingSoonTag->runAction(Sequence::create(DelayTime::create(idleTime
  //    + moveTime), FadeIn::create(0.5f), NULL));

  btn = (Button *)title->getChildByName("btnSetting");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onSettingClick, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));
  //    lbl = addLabelToButton(btn, "setting", 60, Color3B(4, 90, 4));
  //    lbl->setPosition(lbl->getPosition() + Vec2(-110, 10));
  //    doLabelFadeInLater(lbl, idleTime + moveTime, 0.5f);

  spt = Sprite::createWithSpriteFrameName("helicopter0.png");
  titleLayer->addChild(spt);
  spt->setPosition(size.width / 2 + 740, size.height + 300);
  GM->runAnimation(spt, "helicopterStand", true);
  spt->setScale(3);
  spt->runAction(Sequence::create(
      DelayTime::create(idleTime),
      EaseInOut::create(MoveBy::create(moveTime + 0.15f, Vec2(0, -500)), 3),
      NULL));

  if (GM->market == MARKET_ONESTORE_FREE) {

    Vec2 pos = Vec2(size.width - 150, size.height - 150);
    for (int i = 0; i < 5; i++) {
      Sprite *spt = Sprite::create("longLight.png");
      this->addChild(spt);
      spt->runAction(RepeatForever::create(RotateBy::create(
          (rand() % 10) * 0.2 + 1, 150 * (rand() % 2 == 0 ? 1 : -1))));
      spt->setPosition(pos);
      spt->setScale(0.3f);
    }

    Button *btnFull = Button::create("miniRect.png");
    btnFull->setPosition(pos);
    this->addChild(btnFull);
    btnFull->addClickEventListener(CC_CALLBACK_0(Title::onClickFull, this));

    PPLabel *lbl = PPLabel::create("full", 50, DARK_GRAY_3B, false, false,
                                   TextHAlignment::CENTER, false);
    btnFull->addChild(lbl);
    lbl->setPosition(btnFull->getContentSize() / 2);
  }

  GM->playSoundEffect(SOUND_BGM_DUAL);
  GM->titleLayer = this;
  this->schedule(schedule_selector(Title::titleUpdate), 0.1f);

  listener = EventListenerKeyboard::create();
  listener->onKeyPressed = CC_CALLBACK_2(Title::onKeyReleased, this);

  Director::getInstance()
      ->getEventDispatcher()
      ->addEventListenerWithSceneGraphPriority(listener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#else

  int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
  if (freeTicketUsedDay != BSM->getDay()) {
    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
  }

  if (UDGetBool(KEY_GOLDEN_TICKET_FREE, true)) {
    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
  }
  //    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX); //
  //    test

  if (!GM->isCrossShown) {
    GM->isCrossShown = true;
    //        CrossPromotion* crossLayer = CrossPromotion::create();
    //        this->addChild(crossLayer);
  }
#endif

  lblID = LM->getLocalizedLabel();
  this->addChild(lblID);
  lblID->setAnchorPoint(Vec2(0, 1));
  lblID->setPosition(Vec2(130, size.height));
  if (usesAccountSystem()) {
    lblID->setString(strmake("GAME ID: "));
  } else {
    lblID->setVisible(false);
  }
  lblID->setTextColor(Color4B::BLACK);

  lblName = LM->getLocalizedLabel();
  this->addChild(lblName);
  lblName->setAnchorPoint(Vec2(0, 0));
  lblName->setPosition(Vec2(130, 0));
  lblName->setString(strmake("Ver. %s", GM->version));
  lblName->setTextColor(Color4B::WHITE);

  lblLoading = LM->getLocalizedLabel();
  this->addChild(lblLoading);
  lblLoading->setPosition(Vec2(size.width / 2, 100));
  lblLoading->setString(strmake("Ver.%s", GM->version));
  lblLoading->setTextColor(Color4B::WHITE);
  lblLoading->enableOutline(Color4B::BLACK, 5);
  lblLoading->setVisible(false);

  //    UDSetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, ""); // test
  //    UDSetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, ""); // test
  //    UDSetStr(KEY_BUILDINGS, ""); // test
  //    UDSetStr(KEY_UNITS_INVENTORY, ""); // test
  //    UDSetStr(KEY_UNITS_DECK, ""); // test
  //    UDSetStr(KEY_SAVED_ID, "2"); // test
  //    UDSetStr(KEY_SAVED_ID, "-1"); // test
  //    UDSetStr(KEY_SAVED_ID, "10"); // test
  //    UDSetStr(KEY_BUILDINGS, ""); // test

  if (usesAccountSystem() && UDGetStr(KEY_SAVED_ID, "-1").compare("25") == 0) {
    UDSetStr(KEY_SAVED_ID, "-1");
    UDSetBool(KEY_ID_EXIST_CHECK_DONE, false);
    checkPlayerIDExist(); // test
  }

  if (UDGetBool(KEY_FIRST_LAUNCH, true)) {
    //    if(true){ // test
    // check its free
    UDSetBool(KEY_FIRST_LAUNCH, false); // test  uncomment it when release
                                        //        showIndicator(); // test
                                        //        BSM->isThisAppFree(); // test
  }

  if (GM->playedStageIndexForTitle >= 0) {
    showChapterSelect();
    selectedChapter = GM->playedStageIndexForTitle / 12;
    showStageSelect(selectedChapter);
    GM->playedStageIndexForTitle = -1;
  }

  if (usesAccountSystem()) {
    std::string name = UDGetStr(KEY_NAME, "");
    std::string id = UDGetStr(KEY_SAVED_ID, "-1");
    BSM->requestedID = id;

    int stage = UDGetInt(KEY_LAST_CLEAR_STAGE, 0);
    if (name.length() == 0 && id.compare("-1") == 0 &&
        (stage >= _registerNameLimitStage || !isBeginnerLockOn)) {
      nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
      showRegisterName();
    }
  } else {
    GM->ensureWin32StartupDefaults();
    std::string id = UDGetStr(KEY_SAVED_ID, "-1");
    BSM->requestedID = id;
    nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
    if (!BSM->timeEstablished) {
      BSM->getHttpTime();
    }
  }

  if (BSM->timeEstablished) {
    //        firstIndicatorDisposed = true;
  } else {
    //        showIndicator();
  }

  //    btn = (Button*)title->getChildByName("btnPlay");
  //    if (GM->getTimeLeftForCampaignChest(0) <= 0 ||
  //        GM->getTimeLeftForCampaignChest(1) <= 0 ||
  //        GM->getTimeLeftForCampaignChest(2) <= 0) { // new chest
  //        btn->getChildByName("imgRedDot")->setVisible(true);
  //    }else{
  //        btn->getChildByName("imgRedDot")->setVisible(false);
  //    }
  //    const char* keyConvertHeroStrToInt = "key_convertHeroStrToInt";
  //    if(UDGetBool(keyConvertHeroStrToInt, false) == false){
  //        UDSetBool(keyConvertHeroStrToInt, true);
  //        std::string strEquipped = UDGetStr(KEY_UNITS_INVENTORY, "");
  //        ValueVector units = GameManager::getInstance()->split(strEquipped,
  //        "_"); std::string str; int counter = 0; std::vector<UnitInfo*>
  //        unitInfoListInventory; for(int i = 0; i < units.size(); i++){
  //            str = units.at(i).asString();
  //            if(str.length() > 0){
  //                UnitInfo* info =
  //                GM->getUnitInfoFromString(units.at(i).asString());
  //                if(info->unitType < 0){
  //                    continue;
  //                }
  //                unitInfoListInventory.push_back(info);
  //            }
  //        }
  //        GM->saveBattleUnitInventory(unitInfoListInventory);
  //
  //        strEquipped = UDGetStr(KEY_UNITS_DECK, "");
  //        units = GameManager::getInstance()->split(strEquipped, "_");
  //        counter = 0;
  //        std::vector<UnitInfo*> unitInfoListDeck;
  //        for(int i = 0; i < units.size(); i++){
  //            if(units.at(i).asString().length() > 0){
  //                UnitInfo* info =
  //                GM->getUnitInfoFromString(units.at(i).asString());
  //                unitInfoListDeck.push_back(info);
  //            }
  //        }
  //        GM->saveBattleUnitDeck(unitInfoListDeck);
  //
  //        strEquipped = UDGetStr(KEY_UNITS_HERO_INVENTORY, "");
  //        units = GameManager::getInstance()->split(strEquipped, "_");
  //        counter = 0;
  //        std::vector<UnitInfo*> unitInfoHeroListInventory;
  //        for(int i = 0; i < units.size(); i++){
  //            if(units.at(i).asString().length() > 0){
  //                UnitInfo* info =
  //                GM->getUnitInfoFromString(units.at(i).asString());
  //                unitInfoHeroListInventory.push_back(info);
  //            }
  //        }
  //        GM->saveHeroInventory(unitInfoHeroListInventory);
  //
  //        strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
  //        units = GameManager::getInstance()->split(strEquipped, "_");
  //        counter = 0;
  //        std::vector<UnitInfo*> unitInfoHeroListDeck;
  //        for(int i = 0; i < units.size(); i++){
  //            if(units.at(i).asString().length() > 0){
  //                UnitInfo* info =
  //                GM->getUnitInfoFromString(units.at(i).asString());
  //                unitInfoHeroListDeck.push_back(info);
  //            }
  //        }
  //        GM->saveHeroDeck(unitInfoHeroListDeck);
  //
  //        std::string strMonthly = UDGetStr(KEY_MONTHLY_EVENT_INFO,
  //        "-1/0_0_0_0_0_0_0_0_0_0_0_0");
  ////        std::string strData = getMonthlyEventInfoString();
  ////        checkIsNewMonthlyEvent();
  //        ValueVector list = GM->split(strMonthly, "_");
  //        ValueVector monthAndReceive = GM->split(list.at(0).asString(), "/");
  //        UDSetInt(KEY_MONTHLY_EVENT_INFO_MONTH,
  //        monthAndReceive.at(0).asInt());
  //        UDSetInt(KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT,
  //        monthAndReceive.at(1).asInt()); for (int i = 0; i <
  //        EVENT_MISSION_TOTAL_COUNT; i++) {
  //            if (i >= list.size()-1) {
  //                UDSetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT,
  //                i).c_str(), 0);
  //            }else{
  //                UDSetInt(strmake(KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT,
  //                i).c_str(), list.at(i+1).asInt());
  //            }
  //        }
  //    }

  btn = (Button *)title->getChildByName("btnNotice");
  btn->addClickEventListener(CC_CALLBACK_0(Title::showPostPopup, this));
  btn->getChildByName("imgRedDot")->setVisible(false);
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
                                  FadeIn::create(0.5f), NULL));

  //    showIndicator();
  //    BSM->getGameInfo();

  //    for (int i = 35; i >= 0; i--) {
  //        std::string key = strmake("KEY_HARD_MODE_CLEAR_FORMAT_%d", i);
  //        bool isClear = UDGetBool(key.c_str(), false);
  //        if (isClear) {
  //            int previousClearStage = UDGetInt(KEY_HELL_MODE_CLEAR_STAGE,
  //            -1); if (previousClearStage < i) {
  //                UDSetInt(KEY_HELL_MODE_CLEAR_STAGE, previousClearStage);
  //                break;
  //            }
  //        }
  //    }

  //    isFreeApp = true; // test
  //    isFreeAppResultArrived = true; // test
  //    log("rid: %s", UDGetStr(KEY_RID).c_str()); // test

  //    BSM->getHttpTime();
  //    UDSetBool(KEY_PREMIUM_START, false); // test
  //    UDSetInt(KEY_LAST_CLEAR_STAGE, 35); // test
  //    UDSetBool(KEY_CHAPTER_2_PURCHASED, false); // test
  //    UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test
  //    UDSetBool(KEY_HERO_ALERT_NEVER_SHOW, false); // test
  //    UDSetStr(KEY_UNITS_HERO_DECK, "_60/400"); // test
  //    UDSetStr(KEY_UNITS_HERO_INVENTORY, ""); // test
  //    UDSetBool(KEY_HERO_PAGE_TUTORIAL_DONE, false); // test

  //    onHeroClick(); // test

  //    UDSetStr(KEY_DAILY_REWARD_RECEIVED_DAY, "2019-10-22"); // test
  //    BSM->sendArenaScore(60*4 + 35); // test
  //    UDSetStr(KEY_MONTHLY_EVENT_INFO, "-1/0_0_0_0_0_0_0_0_0_0_0_0"); // test

  //    UDSetInt(KEY_LAST_CLEAR_SupdateTrainLayerTAGE, -1); // test
  //    GM->strSmartPassError = "sfsdfdsf"; // test

  // spine test
  for (int i = UNIT_HERO_ORC; i <= UNIT_HERO_GOLEM; i++) {
    if (i > UNIT_HERO_UNDINE && i <= UNIT_HERO_CRAZY_LION) {
      continue;
    }
    spine::SkeletonAnimation *spChar = GM->getHeroSpine(i);
    this->addChild(spChar);
    spChar->setPosition(Vec2(100, -1000));
  }
  // spine test

#if !(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//  if (name.length() == 0 && id.compare("-1") == 0) {
//
//  } else {
//    //        showIndicator();
//    BSM->checkServer();
//  }
#endif

  //    Button* btnCross = Button::create("uiBox.png");
  //    float gap = 170;
  //    btnCross->setPosition(Vec2(size.width - gap, gap));
  //    crossSpriteName = "woorimerge.jpg";
  //    Sprite* sptIcon = Sprite::create(crossSpriteName);
  //    sptIcon->setPosition(btnCross->getContentSize()/2);
  //    sptIcon->setScale(btnCross->getContentSize().width*0.7f/(sptIcon->getContentSize().width));
  //    btnCross->addChild(sptIcon);
  //    btnCross->addClickEventListener(CC_CALLBACK_0(Title::onCrossClick,
  //    this)); this->addChild(btnCross);

  //    UDSetInt(KEY_LAST_CLEAR_STAGE, 20); // test

  //    BSM->getVer();
  //    showRegisterName(); // test
  log("Title init done"); // test

  //    showExitPopup(); // test
  return true;
}
void Title::onServerSelect() { showServerSelect(false); }
void Title::onMapClick() {
  int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
  bool isUnlocked =
      true; // UDGetBool(KEY_CHAPTER_2_PURCHASED) ||
            // UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11;
  if (isUnlocked) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    showInstanceMessage(LM->getText("beta alert")); // test
#endif

  } else {
    showInstanceMessage(LM->getText("worker condition 1")); // test
    return;
  }

  clearAssets();
  GameManager::getInstance()->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  GM->nextScene = STAGE_FIELD;
  GM->isColosseum = false;

  auto scene = MapEditor::scene();
  Director::getInstance()->replaceScene(
      TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::onUploadedClick() {

  if (!BSM->timeEstablished || BSM->isOffline) { // test
    showInstanceMessage(LM->getText("network fail play offline"));
    return; // test
  }

  //    GameSharing::buyItem("goldenticket1"); // test
  //    return;
  //    iapFlag = IAP_FLAG_TICKET_1;// test
  //    bool isConnected = false;
  //    if(!GameSharing::bIsGPGAvailable ||
  //    GameSharing::getPlayerID().compare("notset") == 0){
  // #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM ==
  // CC_PLATFORM_MAC)
  //        showInstanceMessage(LM->getText("sign in game center"));
  // #elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  //        showInstanceMessage(LM->getText("sign in google play"));
  // #endif
  //        return; // test
  //    }
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Node *layer = UploadedMaps::create();
  this->addChild(layer, 4);
  layer->setName("uploaded");
  setAsPopup(layer);
  layer->setTag(0);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);
}
void Title::onCrossClick() {
  Node *layer = CSLoader::createNode("MessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("messageBox");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnNo");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Vec2 posLeft = btn->getPosition();
  Button *btnYES = (Button *)layer->getChildByName("btnYes");
  btnYES->addClickEventListener(CC_CALLBACK_0(Title::goToCross, this));
  btn->setPosition(btnYES->getPosition());
  btnYES->setPosition(posLeft);

  Sprite *sptBox = Sprite::create("uiBox.png");
  layer->addChild(sptBox);
  float gap = 250;
  sptBox->setPosition(Vec2(layer->getContentSize().width / 2,
                           layer->getContentSize().height / 2 + gap));
  Sprite *spt = Sprite::create(crossSpriteName);
  spt->setPosition(Vec2(layer->getContentSize().width / 2,
                        layer->getContentSize().height / 2 + gap));
  layer->addChild(spt);
  spt->setScale(sptBox->getContentSize().width * 0.7f /
                (spt->getContentSize().width));

  Text *lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "game by studio nap");
}
void Title::goToCross() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  Application::getInstance()->openURL(
      "itms-apps://itunes.apple.com/app/id1534482944");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  Application::getInstance()->openURL(
      "https://play.google.com/store/apps/details?id=com.studionap.woorimerge");
#endif
}
void Title::onExitClick() { GM->exitGame(); }
void Title::onUpdateClick() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  Application::getInstance()->openURL(
      "itms-apps://itunes.apple.com/app/id1389133177");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  Application::getInstance()->openURL(
      "https://play.google.com/store/apps/details?id=com.magmon.cartooncraft");
#endif
}
void Title::checkMonthlyEventAttend() {
  GM->checkIsNewMonthlyEvent();
  if (isMonthlyEventAttendChecked)
    return;
  //    std::string strData = GM->getMonthlyEventInfoString();
  //    ValueVector list = GM->split(strData, "_");
  //    UDSetInt(KEY_MONTHLY_EVENT_ATTEND_DAY, -1); // test
  int monthlyEventCheckedDay = UDGetInt(KEY_MONTHLY_EVENT_ATTEND_DAY, -1);
  time_t now = BSM->getCurrentTimeT();
  int today = Value(std::string(BSM->getStrFromTime(now)).substr(8, 2)).asInt();
  if (today != monthlyEventCheckedDay) {
    GM->addMonthlyEventProgress(EVENT_MISSION_ATTEND, 1);
    UDSetInt(KEY_MONTHLY_EVENT_ATTEND_DAY, BSM->getDay()); // test
  }

  isMonthlyEventAttendChecked = true;
}
void Title::onEventClick() {
  int stage = UDGetInt(KEY_LAST_CLEAR_STAGE, 0);
  if (stage < _registerNameLimitStage && isBeginnerLockOn) {
    showInstanceMessage(strmake(
        "%s\n%d-%d %s", LM->getText("unlock condition").c_str(),
        _registerNameLimitStage / 12 + 1, _registerNameLimitStage + 1,
        LM->getText(strmake("stage title 0_%d", _registerNameLimitStage))
            .c_str()));
    return;
  }
  if (!BSM->timeEstablished || BSM->isOffline) {
    showInstanceMessage(LM->getText("network fail play offline"));
    return;
  }
  checkMonthlyEventAttend();
  int month = BSM->getMonth();
  log("month: %d", month);

  showEventPopup();
}
void Title::onChatClick() {
  int stage = UDGetInt(KEY_LAST_CLEAR_STAGE, 0);
  if (stage < _registerNameLimitStage && isBeginnerLockOn) {
    showInstanceMessage(strmake(
        "%s\n%d-%d %s", LM->getText("unlock condition").c_str(),
        _registerNameLimitStage / 12 + 1, _registerNameLimitStage + 1,
        LM->getText(strmake("stage title 0_%d", _registerNameLimitStage))
            .c_str()));
    return;
  }
  if (!BSM->timeEstablished || BSM->isOffline) {
    showInstanceMessage(LM->getText("network fail play offline"));
    return;
  }
  ChatRoom *layer = ChatRoom::create();
  this->addChild(layer, POPUP_ZORDER);
  layer->setName("chatPopup");
  setAsPopup(layer);
}
void Title::showEventPopup() {
  EventPopup *layer = EventPopup::create();
  this->addChild(layer, POPUP_ZORDER);
  layer->setName("eventPopup");
  setAsPopup(layer);
}
void Title::onFacebookClick() {
//    Application::getInstance()->openURL("https://www.facebook.com/StudioNAP/");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  Application::getInstance()->openURL("fb://profile/792734024259632");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  Application::getInstance()->openURL(
      "fb://facewebmodal/f?href=https://www.facebook.com/StudioNAP/"
      "?ref=bookmarks");
#else
  Application::getInstance()->openURL("https://www.facebook.com/StudioNAP");
#endif
}
void Title::onCommunityClick() {
  // if (LM->getLanguageType() == LanguageType::KOREAN) {
  //   Application::getInstance()->openURL("https://cafe.naver.com/cartooncraft");
  // } else {
  showDiscordPopup();
  // }
}
void Title::showDiscordPopup() {
  Node *layer = CSLoader::createNode("DiscordPopup.csb");
  this->addChild(layer, 4);
  layer->setName("discordPopup");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnInvitation");
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::onDiscordInvitationClick, this));
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "invitation");

  btn = (Button *)layer->getChildByName("btnOpen");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onDiscordOpenClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "open");
}
void Title::onDiscordInvitationClick() {
  Application::getInstance()->openURL("https://discord.gg/ewyae8T");
}
void Title::onDiscordOpenClick() {
  Application::getInstance()->openURL(
      "https://discordapp.com/channels/582944551614939269/582944551648493607");
}
void Title::onSettingClick() {
  Node *layer = CSLoader::createNode("Setting.csb");
  this->addChild(layer, 4);
  layer->setName("settingPopup");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Text *lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "setting");

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  btn = (Button *)layer->getChildByName("btnMusic");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onMusicInSettingClick, this));
  btn = (Button *)layer->getChildByName("btnSound");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onSoundClick, this));
  btn = (Button *)layer->getChildByName("btnNetworkReset");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onNetworkResetClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "network reset");
  btn = (Button *)layer->getChildByName("btnRestore");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onRestoreClick, this));
  if (GM->market == MARKET_SMARTPASS) {
    btn->setVisible(false);
  }
  // #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  //     btn->setVisible(false);
  // #endif
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "restore");

  btn = (Button *)layer->getChildByName("btnRename");
  btnGoogle = (Button *)btn->clone();
  btnGoogle->retain();
  btn->addClickEventListener(CC_CALLBACK_0(Title::onRenameClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "rename");

  btn = (Button *)layer->getChildByName("btnCommunity");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onCommunityClick, this));
  //    btn->setOpacity(0);
  //    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));
  // if (LM->getLanguageType() == LanguageType::KOREAN) {

  // } else {
  //   btn->setVisible(false);
  // }

  btn = (Button *)layer->getChildByName("btnFacebook");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onFacebookClick, this));
  btn->setVisible(false);
  //    btn->setOpacity(0);
  //    btn->runAction(Sequence::create(DelayTime::create(idleTime + moveTime),
  //    FadeIn::create(0.5f), NULL));

  lbl = (Text *)layer->getChildByName("lblGPSID");
  lbl->setString(strmake("PLAY ID: [%s]", GM->playerGPSID.c_str()));

  btnGoogle->addClickEventListener(CC_CALLBACK_0(Title::onGoogleClick, this));
  lbl = (Text *)btnGoogle->getChildByName("lbl");
  lbl->setString("GOOGLE");
  btnGoogle->setPosition(
      Vec2(size.width / 2, btnGoogle->getContentSize().height));

  //    LM->setLocalizedString(lbl, "google");

  btn = (Button *)layer->getChildByName("btnLanguage");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onLanguageClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "language");
  if (GM->market == MARKET_SMARTPASS) {
    layer->getChildByName("lblGPSID")->setVisible(false);
    btn->setVisible(false);
    Button *btnPrivacy = (Button *)layer->getChildByName("btnPrivacy");
    btnPrivacy->setPosition(btn->getPosition());
    btnPrivacy->addClickEventListener(
        CC_CALLBACK_0(Title::onPrivacyPolicySmartPassClick, this));
  }

  lbl = (Text *)layer->getChildByName("lblVersion");
  lbl->setString(strmake("Version: %s", GM->version));
  lbl = (Text *)layer->getChildByName("lblID");
  if (usesAccountSystem()) {
    lbl->setString(
        strmake("Game ID: %s", UDGetStr(KEY_SAVED_ID, "-1").c_str()));
  } else {
    lbl->setVisible(false);
  }
  lbl = (Text *)layer->getChildByName("lblName");
  if (usesAccountSystem()) {
    lbl->setString(strmake("Name: %s", UDGetStr(KEY_NAME, "unknown").c_str()));
  } else {
    lbl->setVisible(false);
  }
  btn = (Button *)layer->getChildByName("btnRename");
  if (!usesAccountSystem()) {
    btn->setVisible(false);
    btn->setEnabled(false);
  }
  btn = (Button *)layer->getChildByName("btnNetworkReset");
  if (!usesAccountSystem()) {
    btn->setVisible(false);
    btn->setEnabled(false);
  }
  updateSettingPopup();
}
void Title::onGoogleClick() { GameSharing::buyItem(IAP_DETAIL_STARTER_KEY); }
void Title::onRenameClick() {
  if (!usesAccountSystem()) {
    return;
  }
  btnGoogle->setPosition(btnGoogle->getPosition() + Vec2(-50, -50));
  if (GM->getGem() < 100) {
    showInstanceMessage(LM->getText("not enough gems"));
    return;
  }
  isRename = true;
  showRegisterName();
}
void Title::onPrivacyPolicySmartPassClick() {
  Application::getInstance()->openURL(
      "https://img.au-market.com/mapi/policy/7718310000035");
}
void Title::onLanguageClick() {

  Node *layer = CSLoader::createNode("LanguagePopup.csb");
  this->addChild(layer, 4);
  layer->setName("languagePopup");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  layer->setTag(-1);
  btn = (Button *)layer->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onLanguageOkClick, this));
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "ok");

  lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "language");

  int selectedLanguage = (int)LM->getLanguageType();

  ImageView *imgCheck = (ImageView *)layer->getChildByName("imgCheck");
  if (selectedLanguage == (int)LanguageType::KOREAN) {
    imgCheck->setPosition(layer->getChildByName("btnKorean")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::ENGLISH) {
    imgCheck->setPosition(layer->getChildByName("btnEnglish")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::RUSSIAN) {
    imgCheck->setPosition(layer->getChildByName("btnRussian")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::FRENCH) {
    imgCheck->setPosition(layer->getChildByName("btnFrench")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::SPANISH) {
    imgCheck->setPosition(layer->getChildByName("btnSpanish")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::TURKISH) {
    imgCheck->setPosition(layer->getChildByName("btnTurkish")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::GERMAN) {
    imgCheck->setPosition(layer->getChildByName("btnGerman")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::JAPANESE) {
    imgCheck->setPosition(layer->getChildByName("btnJapanese")->getPosition());
  }
  if (selectedLanguage == (int)LanguageType::PORTUGUESE) {
    imgCheck->setPosition(layer->getChildByName("btnPt")->getPosition());
  }
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::KOREAN);
  btn = (Button *)layer->getChildByName("btnKorean");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "korean");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
  btn = (Button *)layer->getChildByName("btnEnglish");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "english");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::RUSSIAN);
  btn = (Button *)layer->getChildByName("btnRussian");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "russian");
  lbl->setFontName("arial");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::FRENCH);
  btn = (Button *)layer->getChildByName("btnFrench");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "french");
  lbl->setFontName("arial");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::SPANISH);
  btn = (Button *)layer->getChildByName("btnSpanish");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "spanish");
  lbl->setFontName("arial");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::TURKISH);
  btn = (Button *)layer->getChildByName("btnTurkish");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "turkish");
  lbl->setFontName("arial");
  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::GERMAN);
  btn = (Button *)layer->getChildByName("btnGerman");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "german");
  lbl->setFontName("arial");
  btn = (Button *)layer->getChildByName("btnPt");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "portuguese");
  lbl->setFontName("arial");

  UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::JAPANESE);
  btn = (Button *)layer->getChildByName("btnJapanese");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onLanguageButtonClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "japanese");

  UDSetInt(KEY_SELECTED_LANGUAGE, selectedLanguage);
}
void Title::onLanguageButtonClick(Ref *ref) {
  BTN_FROM_REF
  Node *layer = this->getChildByName("languagePopup");
  layer->setTag(btn->getTag());
  ImageView *imgCheck = (ImageView *)layer->getChildByName("imgCheck");

  if (layer->getTag() == 0) {
    imgCheck->setPosition(layer->getChildByName("btnKorean")->getPosition());
  } else if (layer->getTag() == 1) {
    imgCheck->setPosition(layer->getChildByName("btnEnglish")->getPosition());
  } else if (layer->getTag() == 2) {
    imgCheck->setPosition(layer->getChildByName("btnRussian")->getPosition());
  } else if (layer->getTag() == 3) {
    imgCheck->setPosition(layer->getChildByName("btnFrench")->getPosition());
  } else if (layer->getTag() == 4) {
    imgCheck->setPosition(layer->getChildByName("btnSpanish")->getPosition());
  } else if (layer->getTag() == 5) {
    imgCheck->setPosition(layer->getChildByName("btnGerman")->getPosition());
  } else if (layer->getTag() == 6) {
    imgCheck->setPosition(layer->getChildByName("btnTurkish")->getPosition());
  } else if (layer->getTag() == 7) {
    imgCheck->setPosition(layer->getChildByName("btnJapanese")->getPosition());
  } else if (layer->getTag() == 8) {
    imgCheck->setPosition(layer->getChildByName("btnPt")->getPosition());
  } else {
    imgCheck->setPosition(layer->getChildByName("btnEnglish")->getPosition());
  }
}
void Title::onLanguageOkClick() {
  Node *layer = this->getChildByName("languagePopup");
  if (layer->getTag() < 0) {
    closePopup();
    return;
  }
  if (layer->getTag() == 0) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::KOREAN);
  } else if (layer->getTag() == 1) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
  } else if (layer->getTag() == 2) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::RUSSIAN);
  } else if (layer->getTag() == 3) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::FRENCH);
  } else if (layer->getTag() == 4) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::SPANISH);
  } else if (layer->getTag() == 5) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::GERMAN);
  } else if (layer->getTag() == 6) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::TURKISH);
  } else if (layer->getTag() == 7) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::JAPANESE);
  } else if (layer->getTag() == 8) {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::PORTUGUESE);
  } else {
    UDSetInt(KEY_SELECTED_LANGUAGE, (int)LanguageType::ENGLISH);
  }
  restartTheGame();
}
void Title::onRestoreClick() {
  isRestoreRequested = true;

  GameSharing::restoreIAP();
  //    GM->playerGPSID = "a_4835022602392990245"; // test
  //    checkPlayerIDExist();
  UDSetBool(KEY_BATTLE_DATA_INVALIDATE, true);
  isAllUserDataRequested = true;
  BSM->getAllUserData();

  GameSharing::signIn();

  showIndicator();
}
void Title::updateSettingPopup() {
  Node *layer = this->getChildByName("settingPopup");
  Text *lbl = (Text *)layer->getChildByName("btnMusic")->getChildByName("lbl");
  LM->setLocalizedString(lbl,
                         GM->getMusicVolumn() == 0 ? "music off" : "music on");
  ImageView *img =
      (ImageView *)layer->getChildByName("btnMusic")->getChildByName("img");
  img->loadTexture(GM->getMusicVolumn() == 0 ? "iconMusicOff.png"
                                             : "iconMusic.png");
  lbl = (Text *)layer->getChildByName("btnSound")->getChildByName("lbl");
  LM->setLocalizedString(lbl, GM->getSoundVolumn() == 0 ? "effect off"
                                                        : "effect on");
  img = (ImageView *)layer->getChildByName("btnSound")->getChildByName("img");
  img->loadTexture(GM->getSoundVolumn() == 0 ? "iconSoundOff.png"
                                             : "iconSound.png");
}
void Title::onMusicInSettingClick() {
  GM->setMusicVolumn(!GM->getMusicVolumn());
  updateSettingPopup();
}
void Title::onSoundClick() {
  GM->setSoundVolumn(!GM->getSoundVolumn());
  updateSettingPopup();
  this->saveAll();
}
void Title::saveAll() {}
void Title::onNetworkResetClick() {
  Node *layer = CSLoader::createNode("MessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("messageBox");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnNo");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Vec2 posLeft = btn->getPosition();
  Button *btnYES = (Button *)layer->getChildByName("btnYes");
  btnYES->addClickEventListener(
      CC_CALLBACK_0(Title::onNetworkResetConfirmClick, this));
  btn->setPosition(btnYES->getPosition());
  btnYES->setPosition(posLeft);

  Text *lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "network reset warning");
}
void Title::onNetworkResetConfirmClick() {
  if (!usesAccountSystem()) {
    return;
  }
  UDSetStr(KEY_SAVED_ID, "-1");
  UDSetStr(KEY_NAME, "");
  UDSetStr(KEY_RID, "_");
  UDSetInt(KEY_PVP6_TROPHY, 1000);
  UDSetInt(KEY_PVP12_TROPHY, 1000);
  UDSetBool(KEY_BATTLE_TUTORIAL_DONE, false);
  UDSetBool(KEY_FIRST_LAUNCH, true);
  UDSetBool(KEY_FIRST_LAUNCH_AFTER_NEW_SERVER, true);
  UDSetBool(KEY_ID_EXIST_CHECK_DONE, false);
  restartTheGame();
}
void Title::showUpdate() {
  Node *layer = CSLoader::createNode("MessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("messageBox");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnNo");
  btn->setVisible(false);
  Vec2 posLeft = btn->getPosition();
  Button *btnYES = (Button *)layer->getChildByName("btnYes");
  Text *lbl = (Text *)btnYES->getChildByName("lbl");
  LM->setLocalizedString(lbl, "ok");
  btnYES->addClickEventListener(CC_CALLBACK_0(Title::onUpdateConfirm, this));
  btn->setPosition(btnYES->getPosition());
  btnYES->setPosition(posLeft);

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "update required");
}
void Title::onUpdateConfirm() {}
void Title::showExitPopup() {
  Node *layer = CSLoader::createNode("ExitPopup.csb");
  this->addChild(layer, 4);
  layer->setName("exitPopup");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  float moveY = 100;
  Button *btn = (Button *)layer->getChildByName("btnNo");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn->setOpacity(0);
  btn->setPositionY(btn->getPositionY() - moveY);
  btn->runAction(Sequence::create(DelayTime::create(0.5f),
                                  MoveBy::create(0.5f, Vec2(0, moveY)), NULL));
  btn->runAction(
      Sequence::create(DelayTime::create(0.5f), FadeIn::create(0.4f), NULL));
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "no");

  btn = (Button *)layer->getChildByName("btnYes");
  btn->addClickEventListener(CC_CALLBACK_0(GameManager::exitGame, GM));
  btn->setOpacity(0);
  btn->setPositionY(btn->getPositionY() - moveY);
  btn->runAction(Sequence::create(DelayTime::create(1.2f),
                                  MoveBy::create(0.5f, Vec2(0, moveY)), NULL));
  btn->runAction(
      Sequence::create(DelayTime::create(1.2f), FadeIn::create(0.4f), NULL));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "yes");

  //    EnemyBase* unit = EnemyBase::createEnemy(UNIT_TROLL, 0, 0,
  //    "trollStand0.png");
  Sprite *unit = Sprite::createWithSpriteFrameName("trollStand0.png");
  GM->runAnimation(unit, "trollAttack", true);
  unit->setAnchorPoint(Vec2(0.5, 0));
  unit->setScale(2);
  layer->addChild(unit);
  unit->setPosition(Point(layer->getContentSize().width / 2,
                          layer->getContentSize().height / 2 + 0));

  lbl = (Text *)layer->getChildByName("lblAsk");
  LM->setLocalizedString(lbl, "exit game");
}
void Title::onMessageBoxReceived() {
  std::string msgBox = strMessageBox;
  log("onMessageBoxReceived: %s", msgBox.c_str());
  ValueVector msgs = GM->split(msgBox, ",");
  int usedIndex = -1;
  int rank = -1;
  int total = -1;
  for (int i = 0; i < msgs.size(); i++) {
    if (msgs.at(i).asString().find("lr") != std::string::npos) {
      std::string subStr = msgs.at(i).asString().substr(2);
      ValueVector leagueInfo = GM->split(subStr, "of");
      if (leagueInfo.size() == 2) {
        rank = leagueInfo.at(0).asInt();
        total = leagueInfo.at(1).asInt();
        log("league result reward %d of %d", rank, total);
        usedIndex = i;
      }
    }
  }
  std::string msgBoxLeft = "";
  for (int i = 0; i < msgs.size(); i++) {
    if (i != usedIndex) {
      msgBoxLeft += msgs.at(i).asString() + ",";
    }
  }
  msgBoxLeft = msgBoxLeft.substr(0, msgBoxLeft.size() - 1);
  if (usedIndex >= 0) {
    //        showLeagueResult(rank, total);
  }
  //    BSM->updateMessageBox(getPlayerID(), msgBoxLeft);
  UDSetStr(KEY_MESSAGE_BOX, msgBoxLeft);
}
void Title::onUserInfoFailed() { onGetTimeFailed(); }
void Title::onArenaClick() {
  int stage = UDGetInt(KEY_LAST_CLEAR_STAGE, 0);
  if (stage < _registerNameLimitStage && isBeginnerLockOn) {
    showInstanceMessage(strmake(
        "%s\n%d-%d %s", LM->getText("unlock condition").c_str(),
        _registerNameLimitStage / 12 + 1, _registerNameLimitStage + 1,
        LM->getText(strmake("stage title 0_%d", _registerNameLimitStage))
            .c_str()));
    return;
  }
  if (!BSM->timeEstablished || BSM->isOffline) {
    showInstanceMessage(LM->getText("network fail play offline"));
    return;
  }
  //    GameSharing::buyItem("goldenticket1"); // test
  //    return;
  //    iapFlag = IAP_FLAG_TICKET_1;// test
  //    bool isConnected = false;
  //    if(!GameSharing::bIsGPGAvailable ||
  //    GameSharing::getPlayerID().compare("notset") == 0){
  // #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM ==
  // CC_PLATFORM_MAC)
  //        showInstanceMessage(LM->getText("sign in game center"));
  // #elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  //        showInstanceMessage(LM->getText("sign in google play"));
  // #endif
  //        return; // test
  //    }
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  if (usesAccountSystem()) {
    std::string id = UDGetStr(KEY_SAVED_ID, "-1");
    BSM->requestedID = id;
    if (id.compare("-1") == 0) {
      nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
      hideIndicator();
      closePopup();
      showRegisterName();
      return;
    }
  }
  showColosseum();
}
void Title::onMultiplayClickAndSearch() {
  onMultiplayClick();
  log("reopen");
  MultiplayLobby *layer =
      (MultiplayLobby *)this->getChildByName("multiplayLobby");
  log("and start");
  layer->onStartClick(
      layer->getChildByName("multiplayReady")->getChildByName("btnStart"));
}
void Title::onMultiplayClick() {
  int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
  bool isUnlocked =
      true; // UDGetBool(KEY_CHAPTER_2_PURCHASED) ||
            // UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11;
  if (isUnlocked) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    showInstanceMessage(LM->getText("beta alert")); // test
#endif

  } else {
    showInstanceMessage(LM->getText("worker condition 1")); // test
    return;
  }

  // #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  //     if (GM->playerGPSID.size() <= 0) {
  //         showInstanceMessage(LM->getText("sign in game center"));
  //         return;
  //     }
  // #elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  //     if (GM->playerGPSID.size() <= 0) {
  //         showInstanceMessage(LM->getText("sign in google play"));
  //         return;
  //     }
  // #endif

  GM->playSoundEffect(SOUND_PAPER_FLIP);
  MultiplayLobby *layer = MultiplayLobby::create();
  this->addChild(layer);
  layer->setName("multiplayLobby");
  setAsPopup(layer);
}
void Title::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event) {
  if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
    //        if(GM->crossPromotionLayer != nullptr){
    //            CrossPromotion* cross =
    //            (CrossPromotion*)GM->crossPromotionLayer; cross->closeThis();
    //            return;
    //        }
    if (popupArray.size() > 0) {
      closePopup();
    } else {
      showExitPopup();
      if (GM->isAdsUser()) {
        GameSharing::showBanner();
      }
    }
  }
}
void Title::onClickFull() {
  log("here!!");
  fullPackagePopup = Node::create();
  this->addChild(fullPackagePopup);

  Button *btnBack = Button::create("miniRect.png");
  fullPackagePopup->addChild(btnBack);
  btnBack->setColor(Color3B::BLACK);
  btnBack->setScale(20);
  btnBack->setOpacity(100);
  btnBack->setPosition(size / 2);

  Sprite *spt = Sprite::create("fullPack.png");
  fullPackagePopup->addChild(spt);
  spt->setPosition(Point(size.width / 2, size.height / 2 + 160));

  Sprite *sptLight = Sprite::create("longLight.png");
  fullPackagePopup->addChild(sptLight);
  sptLight->setPosition(Point(size.width / 2, size.height / 2 + 310));
  sptLight->setScale(0.6f);

  PPLabel *lbl =
      PPLabel::create(LM->getText("getFullVersion"), 100, Color3B::WHITE, true,
                      false, TextHAlignment::CENTER, true);
  fullPackagePopup->addChild(lbl);
  lbl->setPosition(Point(size.width / 2, size.height / 2 + 310));

  Button *btnFull = Button::create("btnBox.png");
  btnFull->setPosition(Point(size.width / 2, size.height / 2 - 320));
  fullPackagePopup->addChild(btnFull);
  btnFull->addClickEventListener(CC_CALLBACK_0(Title::buyFullPackage, this));

  Label *lblPrice = Label::createWithSystemFont("₩2,000", "Thonburi", 80);
  btnFull->addChild(lblPrice);
  lblPrice->setTextColor(Color4B(DARK_GRAY_3B, 255));
  lblPrice->setPosition(btnFull->getContentSize().width / 2,
                        btnFull->getContentSize().height / 2 + 10);
}
void Title::buyFullPackage() {
  indicator = Node::create();
  this->addChild(indicator, 100);

  Button *btnBack = Button::create("miniRect.png");
  indicator->addChild(btnBack);
  btnBack->setColor(Color3B::BLACK);
  btnBack->setScale(20);
  btnBack->setOpacity(100);
  btnBack->setPosition(size / 2);

  PPLabel *lbl = PPLabel::create("Wait...", 50, Color3B::WHITE, false, false,
                                 TextHAlignment::CENTER, false);
  indicator->addChild(lbl);
  lbl->setPosition(size / 2);

  //    NativeInterface::NativeInterface::purchaseOnestore("propack");
}
void Title::showChapterSelect() {
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Node *layer = CSLoader::createNode("ChapterSelect.csb");
  this->addChild(layer, 4);
  layer->setName("chapterSelect");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);

  Button *btnBlock = (Button *)layer->getChildByName("btnBlock");
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Button *btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  btn = (Button *)layer->getChildByName("btnHardMode");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onDifficultyClick, this));
  ImageView *img = (ImageView *)btn->getChildByName("imgIcon");
  Text *lbl = (Text *)btn->getChildByName("lblTitle");
  if (difficultyMode == DIFFICULTY_MODE_EASY) {
    img->loadTexture("manSmileIcon.png");
    LM->setLocalizedString(lbl, "easy mode");
    lbl->setTextColor(Color4B::GREEN);
  } else if (difficultyMode == DIFFICULTY_MODE_NORMAL) {
    img->loadTexture("manIcon.png");
    LM->setLocalizedString(lbl, "normal mode");
    lbl->setTextColor(Color4B::WHITE);
  } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
    img->loadTexture("manSurpriseIcon.png");
    LM->setLocalizedString(lbl, "hard mode");
    lbl->setTextColor(Color4B::ORANGE);
  } else {
    img->loadTexture("evilIcon.png");
    LM->setLocalizedString(lbl, "hell mode");
    lbl->setTextColor(Color4B(213, 0, 0, 255));
  }
  lbl->setFontSize(lbl->getFontSize() * 1.5f);

  btn = (Button *)layer->getChildByName("btnHero");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onHeroClick, this));
  btn->setVisible(false); // test
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "hero");

  int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
  //    bool isUnlocked = UDGetBool(KEY_CHAPTER_2_PURCHASED) ||
  //    UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11;
  btn->getChildByName("imgLock")->setVisible(false);

  for (int i = 0; i < 3; i++) {
    ImageView *img = (ImageView *)btn->getChildByName(strmake("img%d", i));
    spine::SkeletonAnimation *spChar;
    if (i == 0) {
      spChar = GM->getHeroSpine(UNIT_HERO_LIZARDMAN);
    } else if (i == 1) {
      spChar = GM->getHeroSpine(UNIT_HERO_SPEARMAN);
    } else if (i == 2) {
      spChar = GM->getHeroSpine(UNIT_HERO_WEREWOLF);
    }
    btn->addChild(spChar, 0);
    //        spChar->setAnimation(0, "idle", true);
    spChar->setPosition(img->getPosition());
    spChar->setScale(0.75f);
    img->removeFromParent();
    Sprite *sptShadow = Sprite::create("shadow.png");
    spChar->addChild(sptShadow, -1);
  }
  btn->getChildByName("imgLock")->setLocalZOrder(10);
  lbl = (Text *)layer->getChildByName("imgTitle")->getChildByName("lbl");
  LM->setLocalizedString(lbl, "chapter");
  //    std::string modeKey = isHardMode?"hard mode":"normal mode";
  std::string modeKey;
  if (difficultyMode == DIFFICULTY_MODE_EASY) {
    modeKey = "easy mode";
  } else if (difficultyMode == DIFFICULTY_MODE_NORMAL) {
    modeKey = "normal mode";
  } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
    modeKey = "hard mode";
  } else if (difficultyMode == DIFFICULTY_MODE_HELL) {
    modeKey = "hell mode";
  }
  LM->setLocalizedStringNotKey(lbl, strmake("%s (%s)",
                                            LM->getText("chapter").c_str(),
                                            LM->getText(modeKey).c_str()));
  doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);

  btn = (Button *)layer->getChildByName("btnCollection");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onCollectionClick, this));
  btn->setVisible(false); // test

  ScrollView *sv = (ScrollView *)layer->getChildByName("sv");
  sv->setClippingEnabled(true);
  //    sv->setInnerContainerSize(sv->getContentSize());
  for (int i = 0; i < 3; i++) {
    btn = (Button *)sv->getChildByName(strmake("btnChapter%d", i));
    lbl = (Text *)btn->getChildByName("lblTitle");
    //        btn->getChildByName("imgTitleBack")->setColor(isHardMode?Color3B::RED:Color3B::WHITE);
    Node *img = btn->getChildByName("img");
    if (i == 0) {
      LM->setLocalizedString(lbl, "human");
      btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
      doLabelFadeInLater(replaceTextToPPLabel(lbl), 0.3f, 0.5f);
    } else if (i == 1) {
      int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);

      bool isOpen =
          UDGetBool(KEY_CHAPTER_2_PURCHASED) || lastClearStage >= 10; // normal
      if (difficultyMode == DIFFICULTY_MODE_HELL) {
        isOpen = UDGetInt(KEY_HELL_MODE_CLEAR_STAGE, 0) >= 11;
      } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
        isOpen = UDGetInt(KEY_HAD_MODE_CLEAR_STAGE, 0) >= 11;
      } else if (difficultyMode == DIFFICULTY_MODE_EASY) {
        isOpen = UDGetInt(KEY_EASY_MODE_CLEAR_STAGE, 0) >= 11;
      }
      if (isOpen) {
        LM->setLocalizedString(lbl, "orc");
        btn->getChildByName("lblOr")->setVisible(false);
        btn->getChildByName("imgConditionBack")->setVisible(false);
        btn->getChildByName("lblCondition")->setVisible(false);
        btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
      } else {
        lbl = (Text *)btn->getChildByName("lblOr");
        LM->setLocalizedString(lbl, "or");
        lbl = (Text *)btn->getChildByName("lblCondition");
        LM->setLocalizedString(lbl, "chapter unlock condition");
        btn->addClickEventListener(
            CC_CALLBACK_1(Title::onPurchaseChapterClick, this));
        lbl = (Text *)btn->getChildByName("lblTitle");

        //                lbl->setVisible(!isHardMode);
        //                btn->getChildByName("lblOr")->setVisible(!isHardMode);

        if (GM->market == MARKET_SMARTPASS) {
          btn->getChildByName("lblOr")->setVisible(false);
          btn->getChildByName("lblTitle")->setVisible(false);
          btn->getChildByName("imgTitleBack")->setVisible(false);
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        btn->getChildByName("lblOr")->setVisible(false);
        LM->setLocalizedString(lbl, "orc");
#else
        LM->setLocalizedStringNotKey(
            lbl, GameSharing::getPriceLocale(IAP_DETAIL_CHAPTER2));
#endif
        //                if (isHardMode) {
        //                    btn->setEnabled(false);
        //                }
      }
    } else if (i == 2) {
      Node *img = btn->getChildByName("img");
      spine::SkeletonAnimation *spChar = GM->getHeroSpine(UNIT_HERO_ORC);
      btn->addChild(spChar, 0);
      spChar->setAnimation(0, "idle", true);
      spChar->setPosition(img->getPosition());
      img->removeFromParent();
      Sprite *sptShadow = Sprite::create("shadow.png");
      spChar->addChild(sptShadow, -1);

      img->removeFromParent();
      btn->getChildByName("lblCondition")->setLocalZOrder(2);
      btn->getChildByName("lblOr")->setLocalZOrder(2);
      btn->getChildByName("imgConditionBack")->setLocalZOrder(1);

      btn->getChildByName("imgChestBack")->setLocalZOrder(2);
      btn->getChildByName("imgChest")->setLocalZOrder(3);
      btn->getChildByName("lblChestTimeLeft")->setLocalZOrder(4);
      btn->getChildByName("imgRedDot")->setLocalZOrder(5);

      int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);

      bool isOpen =
          UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 23; // normal
      if (difficultyMode == DIFFICULTY_MODE_HELL) {
        isOpen = UDGetInt(KEY_HELL_MODE_CLEAR_STAGE, 0) >= 23;
      } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
        isOpen = UDGetInt(KEY_HAD_MODE_CLEAR_STAGE, 0) >= 23;
      } else if (difficultyMode == DIFFICULTY_MODE_EASY) {
        isOpen = UDGetInt(KEY_EASY_MODE_CLEAR_STAGE, 0) >= 23;
      }
      if (isOpen) {
        LM->setLocalizedString(lbl, "khalan");
        btn->getChildByName("lblOr")->setVisible(false);
        btn->getChildByName("imgConditionBack")->setVisible(false);
        btn->getChildByName("lblCondition")->setVisible(false);
        btn->addClickEventListener(CC_CALLBACK_1(Title::onChapterClick, this));
      } else {
        lbl = (Text *)btn->getChildByName("lblOr");
        LM->setLocalizedString(lbl, "or");
        lbl = (Text *)btn->getChildByName("lblCondition");
        LM->setLocalizedString(lbl, "chapter unlock condition");
        btn->addClickEventListener(
            CC_CALLBACK_1(Title::onPurchaseChapterClick, this));
        lbl = (Text *)btn->getChildByName("lblTitle");
        //                lbl->setVisible(!isHardMode);
        //                btn->getChildByName("lblOr")->setVisible(!isHardMode);
        //                if (isHardMode) {
        //                    btn->setEnabled(false);
        //                }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        btn->getChildByName("lblOr")->setVisible(false);
        LM->setLocalizedString(lbl, "khalan");
#else
        LM->setLocalizedStringNotKey(
            lbl, GameSharing::getPriceLocale(IAP_DETAIL_CHAPTER3));
#endif
      }
      if (GM->market == MARKET_SMARTPASS) {
        btn->getChildByName("lblOr")->setVisible(false);
        btn->getChildByName("lblTitle")->setVisible(false);
        btn->getChildByName("imgTitleBack")->setVisible(false);
      }
    } else if (i == 77) {
      LM->setLocalizedString(lbl, "coming soon");
      spine::SkeletonAnimation *spChar = GM->getHeroSpine(UNIT_HERO_ORC);
      btn->addChild(spChar);
      spChar->setAnimation(0, "idle", true);
      spChar->setPosition(img->getPosition());
      img->removeFromParent();
      Sprite *sptShadow = Sprite::create("shadow.png");
      spChar->addChild(sptShadow, -1);
    } else if (i == 77) {
      LM->setLocalizedString(lbl, "werewolf");
      spine::SkeletonAnimation *spChar = GM->getHeroSpine(UNIT_HERO_WEREWOLF);
      btn->addChild(spChar);
      spChar->setSkin("werewolf");
      spChar->setAnimation(0, "idle", true);
      spChar->setPosition(img->getPosition());
      img->removeFromParent();
      Sprite *sptShadow = Sprite::create("shadow.png");
      spChar->addChild(sptShadow, -1);
    }
    int timeLeftForChest = GM->getTimeLeftForCampaignChest(i);
    lbl = (Text *)btn->getChildByName("lblChestTimeLeft");
    btn->getChildByName("imgRedDot")->setVisible(timeLeftForChest <= 0);
    if (timeLeftForChest <= 0) {
      lbl->setVisible(false);
    } else {
      lbl->setString(GM->getTimeLeftInString(timeLeftForChest));
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

    btn->getChildByName("imgChestBack")->setVisible(false);
    btn->getChildByName("imgChest")->setVisible(false);
    btn->getChildByName("lblChestTimeLeft")->setVisible(false);
    btn->getChildByName("imgRedDot")->setVisible(false);
#endif
  }

  rollOpenScroll((ImageView *)layer->getChildByName("imgBackground"));
}
void Title::onHeroClick() {
  //    showHeroPage(true);
  //    return; // test
  int stage = UDGetInt(KEY_LAST_CLEAR_STAGE, 0);
  if (stage < _registerNameLimitStage && isBeginnerLockOn) {
    showInstanceMessage(strmake(
        "%s\n%d-%d %s", LM->getText("unlock condition").c_str(),
        _registerNameLimitStage / 12 + 1, _registerNameLimitStage + 1,
        LM->getText(strmake("stage title 0_%d", _registerNameLimitStage))
            .c_str()));
    return;
  }
  //    if(!BSM->timeEstablished){
  //        showInstanceMessage(LM->getText("network fail play offline"));
  //        return;
  //    }
  if (usesAccountSystem()) {
    std::string id = UDGetStr(KEY_SAVED_ID, "-1");
    BSM->requestedID = id;
    if (id.compare("-1") == 0) {
      nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
      hideIndicator();
      closePopup();
      showRegisterName();
      return;
    }
  }
  bool isHeroInvalidated = UDGetBool(KEY_HERO_INVALIDATE, false);
  if (isHeroInvalidated) {
    UDSetBool(KEY_HERO_INVALIDATE, false);
    getHeroInfoFromServer();
  } else {
    showHeroPage(true);
  }

  //    int lastClearStage = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
  //    bool isUnlocked = UDGetBool(KEY_CHAPTER_2_PURCHASED) ||
  //    UDGetBool(KEY_CHAPTER_3_PURCHASED) || lastClearStage >= 11; if
  //    (isUnlocked) {
  //        showHeroPage(true);
  //    }else{
  //        showInstanceMessage(LM->getText("worker condition 1")); // test
  //    }
}
void Title::onCollectionClick() {
  Node *layer = CSLoader::createNode("Collection.csb");
  this->addChild(layer, POPUP_ZORDER);
  layer->setPosition(
      Vec2(size.width / 2 - layer->getContentSize().width / 2,
           size.height / 2 - layer->getContentSize().height / 2));
  layer->setName("collection");
  setAsPopup(layer);

  Button *btnBlock = (Button *)layer->getChildByName("btnBlock");
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Button *btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  Text *lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "collection");

  ScrollView *scrollView = (ScrollView *)layer->getChildByName("scrollView");
  scrollView->setClippingEnabled(true);
  ImageView *stoneTemp = (ImageView *)scrollView->getChildByName("stoneTemp");
  ImageView *goldTemp = (ImageView *)scrollView->getChildByName("goldTemp");
  float startX = stoneTemp->getPositionX();
  float xGap = (goldTemp->getPositionX() - stoneTemp->getPositionX()) / 4;
  float yGap = xGap * 280.0f / 250;
  float startY = 0; // stoneTemp->getPositionY();
  float imgYOffset = -80;
  int count = 0;
  std::vector<ImageView *> cardList;
  for (int i = 0; i < UNIT_HERO_LAMIA; i++) {
    if (UNIT_WORKER == i || UNIT_SWORDMAN == i || UNIT_ARCHER == i ||
        UNIT_CATAPULT == i || UNIT_HELICOPTER == i || UNIT_ORC_AXE == i ||
        UNIT_ORC_SPEAR == i || UNIT_GOBLIN == i || UNIT_GOBLIN_BOMB == i ||
        UNIT_TROLL == i || UNIT_ZOMBIE_ORC_AXE == i ||
        UNIT_ZOMBIE_SWORDSMAN == i || UNIT_WIZARD == i ||
        UNIT_GOBLIN_WORKER == i || UNIT_HERO_ORC == i ||
        UNIT_HERO_GOBLIN == i || UNIT_HERO_SPEARMAN == i ||
        UNIT_HERO_LIZARDMAN == i || UNIT_HERO_ARCHER == i ||
        UNIT_HERO_WEREWOLF == i || UNIT_HERO_MONK == i ||
        UNIT_HERO_FIGHTER == i || UNIT_HERO_BEAR == i ||
        UNIT_HERO_HEALER == i || UNIT_HERO_KNIGHT == i ||
        UNIT_HERO_ELF_SWORDMAN == i || UNIT_HERO_ASSASSIN == i ||
        UNIT_HERO_LION == i || UNIT_HERO_WIZARD == i || UNIT_HERO_TANKER == i ||
        UNIT_HERO_SKELETON == i || UNIT_HERO_REAPER == i ||
        UNIT_HERO_ENT == i || UNIT_HERO_SALAMANDER == i ||
        UNIT_HERO_UNDINE == i || UNIT_HERO_LADY_WEREWOLF == i ||
        UNIT_HERO_LADY_LION == i || UNIT_HERO_LADY_BEAR == i ||
        UNIT_HERO_SANTA == i || UNIT_HERO_RUDOLPH == i ||
        UNIT_HERO_SANTADOG == i || UNIT_HERO_PENGUIN == i ||
        UNIT_HERO_CATINBOOTS == i || UNIT_HERO_MOLE == i ||
        UNIT_HERO_TOYMOUSE == i || UNIT_HERO_SAVAGEARCHER == i ||
        UNIT_HERO_BATMONSTER == i || UNIT_HERO_MEMEAT == i ||
        UNIT_HERO_PARASITE == i || UNIT_HERO_WATERMELON == i ||
        UNIT_HERO_BABYMINO == i || UNIT_HERO_MINO == i ||
        UNIT_HERO_KERBEROS == i || UNIT_HERO_LAMIA == i ||
        UNIT_HERO_CHUNJA == i || UNIT_HERO_GOLEM == i) {
      ImageView *card;
      if (i >= UNIT_HERO_ORC) {
        card = (ImageView *)goldTemp->clone();
      } else {
        card = (ImageView *)stoneTemp->clone();
      }
      scrollView->addChild(card);

      EnemyBase *unit = EnemyBase::createEnemy(i, 1, 1, "workerAxeStand0.png");
      unit->runAnimation(strmake("%sStand", GM->getUnitName(i).c_str()).c_str(),
                         true);
      unit->setScale(1.5f);
      if (i >= UNIT_HERO_ORC && unit->spine) {
        card->addChild(unit->spine);
        unit->spine->setPosition(card->getChildByName("img")->getPosition() +
                                 Vec2(0, imgYOffset));
        unit->spine->setScale(1);
      }
      card->addChild(unit);
      unit->setPosition(card->getChildByName("img")->getPosition() +
                        Vec2(0, imgYOffset));
      card->removeChildByName("img");
      card->setPosition(
          Vec2(startX + xGap * (count % 5), startY - yGap * (count / 5)));
      count++;
      log("count: %d", count);
      cardList.push_back(card);

      lbl = (Text *)card->getChildByName("lbl");
      LM->setLocalizedString(lbl, GM->getUnitName(i));
    }
    stoneTemp->removeFromParent();
    goldTemp->removeFromParent();
  }
  for (auto card : cardList) {
    card->setPositionY(card->getPositionY() + (count / 5) * yGap - 60);
  }
  cardList.clear();
  scrollView->setInnerContainerSize(Size(
      scrollView->getInnerContainerSize().width, (count / 5) * yGap + 140));
}
void Title::getHeroInfoFromServer() {
  showIndicator();
  isHeroInfoRequested = true;
  BSM->getUserData("hdck=1&hivt=1&tree=1&gold=1&gem=1");
}
// void Title::onCloseShop(){
//     Node* heroPage = this->getChildByName("heroPage");
//     if(heroPage != nullptr){
//         ndTopBar->removeFromParentAndCleanup(false);
//         heroPage->addChild(ndTopBar);
//         closePopup();
//     }
// }
// void Title::onHeroMoreResourceClick(){
//     onHeroShopClick();
//     onHeroShopTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab1"));
// }
// void Title::onHeroMoreGemClick(){
//     onHeroShopClick();
//     onHeroShopTabClick(this->getChildByName("shopLayer")->getChildByName("btnTab2"));
// }
void Title::showHeroPage(bool showAlert) {
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  HeroPage *layer = HeroPage::create();
  this->addChild(layer);
  layer->setName("heroPage");
  setAsPopup(layer);
}

void Title::onDifficultyClick() {
  if (difficultyMode == DIFFICULTY_MODE_EASY) {
    difficultyMode = DIFFICULTY_MODE_NORMAL;
  } else if (difficultyMode == DIFFICULTY_MODE_NORMAL) {
    difficultyMode = DIFFICULTY_MODE_HARD;
  } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
    difficultyMode = DIFFICULTY_MODE_HELL;
  } else if (difficultyMode == DIFFICULTY_MODE_HELL) {
    difficultyMode = DIFFICULTY_MODE_EASY;
  }
  UDSetInt(KEY_DIFFICULTY_MODE, difficultyMode);
  //    isHardMode = !isHardMode;
  closePopup();
  showChapterSelect();
}
void Title::onPurchaseChapterClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  if (btn->getTag() == 1) {
    GameSharing::buyItem(IAP_DETAIL_CHAPTER2);
  } else if (btn->getTag() == 2) {
    GameSharing::buyItem(IAP_DETAIL_CHAPTER3);
  }
}
void Title::onChapterClick(Ref *ref) {
  BTN_FROM_REF
  selectedChapter = btn->getTag();
  if (btn->getTag() == 0) {
    showStageSelect(btn->getTag());
  } else if (btn->getTag() == 1) {
    showStageSelect(btn->getTag());
  } else if (btn->getTag() == 2) {
    showStageSelect(btn->getTag());
  } else if (btn->getTag() == 3) {
    showInstanceMessage(LM->getText("coming soon")); // test
  }
}
void Title::showStageSelect(int chapter) {
  showInterstitialRequested = true;
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Node *layer = CSLoader::createNode("StageSelect.csb");
  this->addChild(layer, 4);
  layer->setTag(chapter);
  layer->setName("stageSelect");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);

  Button *btnBlock = (Button *)layer->getChildByName("btnBlock");
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Button *btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  btn = (Button *)layer->getChildByName("btnLoad");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onLoadClick, this));
  addLabelToButton(btn, LM->getText("load"));

  btn = (Button *)layer->getChildByName("btnCampaignChestShop");
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::onCampaignChestShopClick, this));

  ScrollView *sv = (ScrollView *)layer->getChildByName("sv");
  sv->setClippingEnabled(true);
  rollOpenScroll((ImageView *)layer->getChildByName("imgBackground"));

  float itemHeight = 234;
  int itemCount = 12;
  sv->setInnerContainerSize(
      cocos2d::Size(sv->getContentSize().width, itemHeight * itemCount));
  int chestIndex = UDGetInt(
      strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(), 0);
  bool shouldSaveChestIndex = false;
  if (chestIndex >= 12) {
    chestIndex = 0;
    shouldSaveChestIndex = true;
  }
  if (chapter == 0 && chestIndex == 0) {
    chestIndex = 1;
    shouldSaveChestIndex = true;
  }
  if (shouldSaveChestIndex) {
    UDSetInt(strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(),
             chestIndex);
  }
  for (int i = 0; i < itemCount; i++) {
    bool isHidden =
        UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1 - chapter * 12 < i; // test
    if (difficultyMode == DIFFICULTY_MODE_HELL) {
      isHidden = UDGetInt(KEY_HELL_MODE_CLEAR_STAGE, -1) + 1 - chapter * 12 < i;
    } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
      isHidden = UDGetInt(KEY_HAD_MODE_CLEAR_STAGE, -1) + 1 - chapter * 12 < i;
    } else if (difficultyMode == DIFFICULTY_MODE_EASY) {
      isHidden = UDGetInt(KEY_EASY_MODE_CLEAR_STAGE, -1) + 1 - chapter * 12 < i;
    }
    if (i == 0) {
      isHidden = false;
    }
    btn = Button::create(isHidden ? "btnHorizontalScrollItemHidden.png"
                                  : "btnHorizontalScrollItem.png");
    btn->setName(strmake("btn%d", i));
    sv->addChild(btn);
    btn->setScale9Enabled(true);
    btn->setContentSize(cocos2d::Size(1300 - 30 * 2, itemHeight - 10));
    btn->setPosition(Vec2(30, (itemCount - (i)) * itemHeight));
    btn->setAnchorPoint(Vec2(0, 1));
    btn->addClickEventListener(CC_CALLBACK_1(Title::onStageClick, this));
    btn->setPressedActionEnabled(false);
    btn->setZoomScale(0.01f);
    btn->setTag(i + chapter * itemCount);
    btn->setOpacity(0);
    btn->runAction(
        Sequence::create(DelayTime::create(0.3), FadeIn::create(0.5f), NULL));

    Button *btnYoutube = nullptr;
    if ((chapter == 0 && i == 0) || (chapter == 0 && i == 1) ||
        (chapter == 0 && i == 2) || (chapter == 0 && i == 3) ||
        (chapter == 0 && i == 4) || (chapter == 0 && i == 5) ||
        (chapter == 0 && i == 6) || (chapter == 0 && i == 7) ||
        (chapter == 0 && i == 8) || (chapter == 0 && i == 9) ||
        (chapter == 1 && i == 4) || (chapter == 1 && i == 7) ||
        (chapter == 1 && i == 8) || (chapter == 1 && i == 10) ||
        (chapter == 1 && i == 11) || (chapter == 2 && i == 0) ||
        (chapter == 2 && i == 1)) {
      btnYoutube = Button::create("btnYoutube.png");
      btnYoutube->setTag(chapter * 12 + i);
      btnYoutube->addClickEventListener(
          CC_CALLBACK_1(Title::onYoutubeClick, this));
      btnYoutube->setScale9Enabled(true);
      btnYoutube->setContentSize(
          cocos2d::Size(btnYoutube->getContentSize().width, itemHeight - 10));
      btnYoutube->setPosition(
          Vec2(btn->getContentSize().width, btn->getContentSize().height));
      btnYoutube->setAnchorPoint(Point(1, 1));
      btn->addChild(btnYoutube);
    }

    if (chestIndex == i && false) {
      float extraX = 0;
      if (btnYoutube != nullptr) {
        extraX = -160;
      }
      Sprite *spt = Sprite::create("campaignChest.png");
      spt->setContentSize(cocos2d::Size(116.52f, 99.65f));
      spt->setPosition(Vec2(1100 + extraX, 120));
      btn->addChild(spt);
      Text *lblTimeLeft = Text::create("::", "BMDOHYEON.ttf", 30);
      lblTimeLeft->setName("lblTimeLeft");
      lblTimeLeft->enableOutline(Color4B::BLACK, 3);
      lblTimeLeft->setPosition(
          Vec2(spt->getPositionX(), spt->getPositionY() - 50));
      btn->addChild(lblTimeLeft);
      int timeLeft = GM->getTimeLeftForCampaignChest(chapter);
      lblTimeLeft->setVisible(timeLeft > 0);
      if (timeLeft > 0) {
        lblTimeLeft->setString(GM->getTimeLeftInString(timeLeft));
      }
    }

    std::string title =
        LM->getText(strmake("stage title %d_%d", chapter, i).c_str());
    if (isHidden) { // test
      title = "???";
      btn->setTouchEnabled(false); // test
    } else {
      //            btn->setColor(isHardMode?Color3B::RED:Color3B::WHITE);
    }

    PPLabel *lbl =
        PPLabel::create(strmake("%d.%s", i + 1, title.c_str()), 50,
                        isHidden ? Color3B(74, 72, 68) : Color3B(29, 28, 26),
                        true, false, TextHAlignment::LEFT, false);
    btn->addChild(lbl);
    lbl->setPosition(30, btn->getContentSize().height / 2);
    doLabelFadeInLater(lbl, 0.3f, 0.5f);
  }

  // Scroll to show the next stage to clear
  int nextStageIndex;
  if (difficultyMode == DIFFICULTY_MODE_HELL) {
    nextStageIndex = UDGetInt(KEY_HELL_MODE_CLEAR_STAGE, -1) + 1 - chapter * itemCount;
  } else if (difficultyMode == DIFFICULTY_MODE_HARD) {
    nextStageIndex = UDGetInt(KEY_HAD_MODE_CLEAR_STAGE, -1) + 1 - chapter * itemCount;
  } else if (difficultyMode == DIFFICULTY_MODE_EASY) {
    nextStageIndex = UDGetInt(KEY_EASY_MODE_CLEAR_STAGE, -1) + 1 - chapter * itemCount;
  } else {
    nextStageIndex = UDGetInt(KEY_LAST_CLEAR_STAGE, -1) + 1 - chapter * itemCount;
  }
  nextStageIndex = std::max(0, std::min(itemCount - 1, nextStageIndex));

  float innerH = sv->getInnerContainerSize().height;
  float svH = sv->getContentSize().height;
  float itemCenterY = (itemCount - nextStageIndex - 0.5f) * itemHeight;
  float targetY = svH * 0.5f - itemCenterY;
  float minY = svH - innerH;
  targetY = std::max(minY, std::min(0.0f, targetY));
  float scrollRange = innerH - svH;
  float scrollPercent = (scrollRange > 0.0f) ? 100.0f * (targetY - minY) / scrollRange : 0.0f;
  scrollPercent = std::max(0.0f, std::min(100.0f, scrollPercent));
  sv->scheduleOnce([sv, scrollPercent](float) {
    sv->scrollToPercentVertical(scrollPercent, 0.5f, true);
  }, 0.4f, "scrollToNextStage");
}
void Title::onCampaignChestShopClick() {
  Node *layer = CSLoader::createNode("CampaignChestShop.csb");
  this->addChild(layer, POPUP_ZORDER);
  layer->setPosition(
      Vec2(size.width / 2 - layer->getContentSize().width / 2,
           size.height / 2 - layer->getContentSize().height / 2));
  layer->setName("campaignChestShop");
  setAsPopup(layer);

  TopBar *topBar = TopBar::create();
  layer->addChild(topBar);
  topBar->setPosition(Vec2(layer->getContentSize().width - 100,
                           layer->getContentSize().height - 100));
  Node *stageSelectLayer = this->getChildByName("stageSelect");
  int chapter = stageSelectLayer->getTag();
  Text *lblStage = (Text *)layer->getChildByName("lblStage");
  int startStageIndex = 0;
  if (chapter == 0) {
    startStageIndex = 1;
  }
  LM->setLocalizedStringNotKey(
      lblStage, strmake("%d.%s", startStageIndex + 1,
                        LM->getText(strmake("stage title %d_%d", chapter,
                                            startStageIndex))
                            .c_str()));
  Button *btn = (Button *)layer->getChildByName("btnAccept");
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::onForceOpenCampaignChestClick, this));
  btn = (Button *)layer->getChildByName("btnAccept_0");
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::onResetCampaignChestClick, this));
  btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
}
void Title::onResetCampaignChestClick() {
  int gemPrice = 5;
  if (gemPrice > GM->getGem()) {
    showInstanceMessage(LM->getText("not enough gems"));
    return;
  }
  GM->addGem(-gemPrice);
  Node *layer = this->getChildByName("stageSelect");
  int chapter = layer->getTag();
  if (chapter == 0) {
    UDSetInt(strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(),
             1);
  } else {
    UDSetInt(strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, chapter).c_str(),
             0);
  }

  closePopup();
  closePopup();
  showStageSelect(chapter);
}
void Title::onForceOpenCampaignChestClick() {
  int gemPrice = 10;
  if (gemPrice > GM->getGem()) {
    showInstanceMessage(LM->getText("not enough gems"));
    return;
  }
  GM->addGem(-gemPrice);
  openCampaignChest();
}
void Title::openCampaignChest() {
  Node *layer = CSLoader::createNode("CampaignChest.csb");
  this->addChild(layer, POPUP_ZORDER);
  layer->setPosition(
      Vec2(size.width / 2 - layer->getContentSize().width / 2,
           size.height / 2 - layer->getContentSize().height / 2));
  layer->setName("campaignChest");
  setAsPopup(layer);
  Button *btn = (Button *)layer->getChildByName("btnOpen");
  btn->addClickEventListener(
      CC_CALLBACK_1(Title::onOpenCampaignChestClick, this));
  btn->setOpacity(0);
  Node *imgChest = layer->getChildByName("imgChest");
  GM->makeItScaleUpAndDown(imgChest);
  GM->addGlowBack(imgChest);
  GM->shakeIt(imgChest, 10, 5);
  for (int i = 0; i < 3; i++) {
    layer->getChildByName(strmake("imgPos%d", i))->setOpacity(0);
  }
  btn = (Button *)layer->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
}

void Title::onOpenCampaignChestClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  Node *layer = this->getChildByName("campaignChest");
  Node *imgChest = layer->getChildByName("imgChest");
  imgChest->stopAllActions();
  imgChest->runAction(Sequence::create(
      EaseOut::create(MoveBy::create(1.0f, Vec2(0, 100)), 2),
      JumpBy::create(0.4f, Vec2(0, -100), 200, 1),
      CallFunc::create(CC_CALLBACK_0(Title::onOpenCampaignChest, this)), NULL));
}
void Title::onOpenCampaignChest() {
  Node *layer = this->getChildByName("campaignChest");
  ImageView *imgChest = (ImageView *)layer->getChildByName("imgChest");
  imgChest->loadTexture("chestWoodOpen.png");
  ImageView *img;
  ImageView *imgTemp;
  bool saveGem = false;
  for (int i = 0; i < 3; i++) {
    int reward = rand() % 100;
    if (reward < 20) {
      imgTemp = (ImageView *)layer->getChildByName("imgGold");
      GM->addCoin(150);
    } else if (reward < 50) {
      imgTemp = (ImageView *)layer->getChildByName("imgLumber");
      GM->addTree(100);
    } else if (reward < 90) {
      imgTemp = (ImageView *)layer->getChildByName("imgEventItem");
      ImageView *imgIcon = (ImageView *)imgTemp->getChildByName("imgIcon");
      int month = BSM->getMonth();
      imgIcon->loadTexture(strmake("eventItem%d.png", month));
      if (month == 12) {
        imgIcon->setContentSize(cocos2d::Size(73, 88));
      } else if (month == 1) {
        imgIcon->setContentSize(cocos2d::Size(85, 60));
      } else if (month == 2) {
        imgIcon->setContentSize(cocos2d::Size(88, 88));
      } else if (month == 3) {
        imgIcon->setContentSize(cocos2d::Size(75, 76));
      }
      GM->addMonthlyEventProgress(EVENT_MISSION_COLLECT_EVENT_HERO_PART, 1);
    } else {
      imgTemp = (ImageView *)layer->getChildByName("imgGem");
      GM->addGem(10);
      saveGem = true;
    }

    img = (ImageView *)imgTemp->clone();
    layer->addChild(img);
    img->setPosition(
        layer->getChildByName(strmake("imgPos%d", i))->getPosition());
    img->setOpacity(0);
    img->runAction(
        Sequence::create(DelayTime::create(0.5f * i), FadeIn::create(0),
                         JumpBy::create(0.3f, Vec2(0, 0), 50, 1), NULL));
  }
  std::vector<int> datas;
  datas.push_back(DATA_TYPE_GEM);
  BSM->saveUserData(datas);

  Button *btn = (Button *)layer->getChildByName("btnOk");
  btn->runAction(Sequence::create(DelayTime::create(1.2f),
                                  JumpBy::create(0.5f, Vec2(0, 650), 100, 1),
                                  NULL));
}
void Title::onYoutubeChannelClick() {
  Application::getInstance()->openURL(
      "https://www.youtube.com/channel/UCTTv1RLxLGz44WtvtDEJ_EQ");
}
void Title::onYoutubeClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  int stageIndex = btn->getTag();
  Node *layer = CSLoader::createNode("MessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("messageBox");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  btn = (Button *)layer->getChildByName("btnBlock");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn = (Button *)layer->getChildByName("btnNo");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "no");
  Vec2 posLeft = btn->getPosition();
  Button *btnYES = (Button *)layer->getChildByName("btnYes");
  lbl = (Text *)btnYES->getChildByName("lbl");
  LM->setLocalizedString(lbl, "yes");
  btnYES->setTag(stageIndex);
  btnYES->addClickEventListener(CC_CALLBACK_1(Title::onYoutubeConfirm, this));
  btn->setPosition(btnYES->getPosition());
  btnYES->setPosition(posLeft);

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "video warning");
}
void Title::onYoutubeConfirm(Ref *ref) {
  BTN_FROM_REF
  if (btn->getTag() == 0) {
    Application::getInstance()->openURL("https://youtu.be/cFnBiY3sRzU");
  } else if (btn->getTag() == 1) {
    Application::getInstance()->openURL("https://youtu.be/V1Sm1BqZOs4");
  } else if (btn->getTag() == 2) {
    Application::getInstance()->openURL("https://youtu.be/30HoVekOL4s");
  } else if (btn->getTag() == 3) {
    Application::getInstance()->openURL("https://youtu.be/dMTFeLVodsU");
  } else if (btn->getTag() == 4) {
    Application::getInstance()->openURL("https://youtu.be/Punq4_LTRHQ");
  } else if (btn->getTag() == 5) {
    Application::getInstance()->openURL("https://youtu.be/5EjQg-dR_g8");
  } else if (btn->getTag() == 6) {
    Application::getInstance()->openURL("https://youtu.be/l1a-W5YTLL8");
  } else if (btn->getTag() == 7) {
    Application::getInstance()->openURL("https://youtu.be/AeYBwvAuje4");
  } else if (btn->getTag() == 8) {
    Application::getInstance()->openURL("https://youtu.be/vWe8qFjeX0Y");
  } else if (btn->getTag() == 9) {
    Application::getInstance()->openURL("https://youtu.be/SiDFdHuF5rQ");
  } else if (btn->getTag() == 12 + 4) {
    Application::getInstance()->openURL("https://youtu.be/VSNI-OqQz7Q");
  } else if (btn->getTag() == 12 + 7) {
    Application::getInstance()->openURL("https://youtu.be/whmsnQzyOqo");
  } else if (btn->getTag() == 12 + 8) {
    Application::getInstance()->openURL("https://youtu.be/iC0UqgZHBB0");
  } else if (btn->getTag() == 12 + 10) {
    Application::getInstance()->openURL("https://youtu.be/IwCi7bHmNSk");
  } else if (btn->getTag() == 12 + 11) {
    Application::getInstance()->openURL("https://youtu.be/cLCGxHYQjXE");
  } else if (btn->getTag() == 24) {
    Application::getInstance()->openURL("https://youtu.be/lHMzZBIk6gU");
  } else if (btn->getTag() == 25) {
    Application::getInstance()->openURL("https://youtu.be/fEI_Jj9RG1o");
  }
}

void Title::onOkFromLoadData() {
  GM->isLoadingGame = true;
  GM->loadingSlot = selectedSaveSlot;

  //    removeListener();

  Button *btn = Button::create("uiBoxSmall.png");
  this->addChild(btn, 1000);
  btn->setScale(100);
  btn->setOpacity(150);
  btn->setColor(Color3B::BLACK);
  btn->setPosition(size / 2);

  Label *lbl = LM->getLocalizedLabel("Loading...", Color4B::WHITE, 80);
  this->addChild(lbl, 1001);
  lbl->setPosition(size / 2);

  this->runAction(Sequence::create(
      DelayTime::create(0.07f),
      CallFunc::create(CC_CALLBACK_0(Title::goToLoadedStage, this)), NULL));
}
void Title::goToLoadedStage() {
  std::string data = UDGetStr(strmake("savedData%d", selectedSaveSlot).c_str());
  int firstComma = (int)data.find(',');
  int secondComma = (int)data.find(',', firstComma + 1);
  data = data.substr(0, secondComma);
  ValueVector datas = GameManager::getInstance()->split(data, ",");
  int savedStage = -1;
  int savedTime = 0;
  if (datas.size() > 1) {
    savedStage = datas.at(0).asInt();
    savedTime = datas.at(1).asInt();
  }
  clearAssets();
  GameManager::getInstance()->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  GM->nextScene = STAGE_FIELD;
  GM->isColosseum = false;

  std::string strData =
      UDGetStr(strmake("savedDataExtra%d", selectedSaveSlot).c_str());
  datas = GameManager::getInstance()->split(strData.c_str(), ",");
  //    bool isHard = false;
  int difficultyMode = 0;
  if (datas.size() > 0) {
    difficultyMode = Value(datas.at(0)).asInt();
  }
  auto scene = GameScene::scene(savedStage, difficultyMode);
  Director::getInstance()->replaceScene(
      TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::goToMultiplayStage(int stageNumber) {
  clearAssets();
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  GM->isColosseum = false;
  GM->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  GM->nextScene = STAGE_FIELD;
  auto scene = GameScene::scene(stageNumber, GAME_MODE_NORMAL, true);
  WORLD->difficultyMode = DIFFICULTY_MODE_NORMAL;
  Director::getInstance()->replaceScene(
      TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::onLoadClick() {
  showInterstitialRequested = true;
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Layer *layer = Layer::create();
  this->addChild(layer);
  setAsPopup(layer);

  Button *btnBlock = Button::create("uiBox.png");
  btnBlock->setPosition(Point(size.width / 2, size.height / 2));
  layer->addChild(btnBlock);
  btnBlock->setScale(20);
  btnBlock->setOpacity(100);
  btnBlock->setColor(Color3B::BLACK);
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  ImageView *storyBack = ImageView::create("verticalScrollBackground.png");
  layer->addChild(storyBack);
  storyBack->setScale9Enabled(true);
  storyBack->setPosition(size / 2);
  storyBack->setTouchEnabled(true);

  PPLabel *lblTitle;
  PPLabel *lblTime;
  Button *btn;
  float firstLineY = size.height / 2;
  float gapY = 260;
  PPLabel *lblLoad =
      PPLabel::create(LM->getText("load"), 100, DARK_GRAY_3B, true, false,
                      TextHAlignment::CENTER, false);
  layer->addChild(lblLoad);
  lblLoad->setPosition(Point(size.width / 2, size.height / 2 + 360));
  for (int i = 0; i < 4; i++) {
    btn = Button::create("uiBoxSmall.png");
    btn->setScale9Enabled(true);
    layer->addChild(btn);
    btn->setContentSize(cocos2d::Size(500, 200));
    btn->addClickEventListener(CC_CALLBACK_1(Title::onLoadSlotClick, this));
    btn->setTag(i);
    //        ValueVector datas =
    //        GameManager::getInstance()->split(UDGetStr(strmake("savedData%d",
    //        i).c_str()), ",");
    std::string data = UDGetStr(strmake("savedData%d", btn->getTag()).c_str());
    int firstComma = (int)data.find(',');
    int secondComma = (int)data.find(',', firstComma + 1);
    data = data.substr(0, secondComma);
    ValueVector datas = GameManager::getInstance()->split(data, ",");

    int savedStage = -1;
    int savedTime = 0;
    if (datas.size() > 1) {
      log("std: %s", datas.at(0).asString().c_str());
      savedStage = datas.at(0).asInt();
      savedTime = datas.at(1).asInt();
    } else {
      btn->setEnabled(false);
    }
    std::string savedStageTitle;
    if (savedStage < 0) {
      savedStageTitle = "Empty";
      btn->setColor(Color3B(200, 200, 200));
    } else {
      btn->setColor(Color3B::WHITE);
      savedStageTitle = strmake(
          "%d. %s", savedStage + 1,
          LM->getText(strmake("stage title %d", savedStage).c_str()).c_str());
      if (savedStageTitle.size() > 6) {
        savedStageTitle =
            strmake("%d. %s...", savedStage + 1,
                    LM->getText(strmake("stage title %d", savedStage).c_str())
                        .substr(0, 6)
                        .c_str());
      }
    }

    lblTitle = PPLabel::create(savedStageTitle, 60, DARK_GRAY_3B, true, false,
                               TextHAlignment::CENTER, false);
    lblTitle->setName("effect");
    layer->addChild(lblTitle);

    lblTime =
        PPLabel::create(GM->getTimeLeftInString(savedTime), 40, DARK_GRAY_3B,
                        true, false, TextHAlignment::CENTER, false);
    layer->addChild(lblTime);
    if (i == 0) {
      btn->setPosition(Point(size.width / 2 - 300, firstLineY));
    } else if (i == 1) {
      btn->setPosition(Point(size.width / 2 + 300, firstLineY));
    } else if (i == 2) {
      btn->setPosition(Point(size.width / 2 - 300, firstLineY - gapY));
    } else if (i == 3) {
      btn->setPosition(Point(size.width / 2 + 300, firstLineY - gapY));
    }
    lblTitle->setPosition(btn->getPosition() + Point(0, 35));
    lblTime->setPosition(btn->getPosition() + Point(0, -35));
    if (savedTime == 0) {
      lblTime->setVisible(false);
    }

    std::string strData =
        UDGetStr(strmake("savedDataExtra%d", btn->getTag()).c_str());
    datas = GameManager::getInstance()->split(strData.c_str(), ",");
    if (datas.size() > 0 && Value(datas.at(0)).asInt()) {
      btn->setColor(Color3B(215, 0, 0));
      lblTitle->setFontColor(Color3B::WHITE);
      lblTime->setFontColor(Color3B::WHITE);
    }
  }
}
void Title::onLoadSlotClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  selectedSaveSlot = btn->getTag();

  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Layer *layer = Layer::create();
  this->addChild(layer);
  setAsPopup(layer);

  Button *btnBlock = Button::create("uiBox.png");
  btnBlock->setPosition(Point(size.width / 2, size.height / 2));
  layer->addChild(btnBlock);
  btnBlock->setScale(20);
  btnBlock->setOpacity(100);
  btnBlock->setColor(Color3B::BLACK);
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  ImageView *storyBack = ImageView::create("verticalScrollBackground.png");
  layer->addChild(storyBack);
  storyBack->setScale9Enabled(true);
  storyBack->setPosition(size / 2);
  storyBack->setTouchEnabled(true);

  PPLabel *lbl = PPLabel::create(LM->getText("load data"), 50, DARK_GRAY_3B,
                                 true, false, TextHAlignment::CENTER, false);
  layer->addChild(lbl);
  lbl->setPosition(Point(size.width / 2, size.height / 2 + 300));
  lbl->setWidth(1000);

  btn = Button::create("uiBoxSmall.png");
  btn->setScale9Enabled(true);
  layer->addChild(btn);
  btn->setPosition(Point(size.width / 2 - 300, size.height / 2 - 200));
  btn->setContentSize(cocos2d::Size(500, 200));
  btn->addClickEventListener(CC_CALLBACK_0(Title::onOkFromLoadData, this));

  lbl = PPLabel::create(LM->getText("ok"), 60, DARK_GRAY_3B, true, false,
                        TextHAlignment::CENTER, false);
  lbl->setName("effect");
  layer->addChild(lbl);
  lbl->setPosition(btn->getPosition());

  btn = Button::create("uiBoxSmall.png");
  btn->setScale9Enabled(true);
  layer->addChild(btn);
  btn->setPosition(Point(size.width / 2 + 300, size.height / 2 - 200));
  btn->setContentSize(cocos2d::Size(500, 200));
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  lbl = PPLabel::create(LM->getText("cancel"), 60, DARK_GRAY_3B, true, false,
                        TextHAlignment::CENTER, false);
  lbl->setName("music");
  layer->addChild(lbl);
  lbl->setPosition(btn->getPosition());
}
void Title::onOptionClick() {
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Layer *layer = Layer::create();
  this->addChild(layer);
  setAsPopup(layer);

  Button *btnBlock = Button::create("uiBox.png");
  btnBlock->setPosition(Point(size.width / 2, size.height / 2));
  layer->addChild(btnBlock);
  btnBlock->setScale(20);
  btnBlock->setOpacity(100);
  btnBlock->setColor(Color3B::BLACK);
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  ImageView *storyBack = ImageView::create("verticalScrollBackground.png");
  layer->addChild(storyBack);
  storyBack->setScale9Enabled(true);
  storyBack->setPosition(size / 2);
  storyBack->setTouchEnabled(true);

  PPLabel *lbl = PPLabel::create(LM->getText("option"), 100, DARK_GRAY_3B, true,
                                 false, TextHAlignment::CENTER, false);
  layer->addChild(lbl);
  lbl->setPosition(Point(size.width / 2, size.height / 2 + 300));

  Button *btn = Button::create("uiBoxSmall.png");
  btn->setScale9Enabled(true);
  layer->addChild(btn);
  btn->setPosition(Point(size.width / 2 - 300, size.height / 2 - 100));
  btn->setContentSize(cocos2d::Size(500, 200));
  btn->addClickEventListener(CC_CALLBACK_0(Title::onEffectClick, this));

  lbl = PPLabel::create(
      LM->getText(GM->getSoundVolumn() == 0 ? "effect off" : "effect on"), 60,
      DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
  lbl->setName("effect");
  layer->addChild(lbl);
  lbl->setPosition(btn->getPosition());

  btn = Button::create("uiBoxSmall.png");
  btn->setScale9Enabled(true);
  layer->addChild(btn);
  btn->setPosition(Point(size.width / 2 + 300, size.height / 2 - 100));
  btn->setContentSize(cocos2d::Size(500, 200));
  btn->addClickEventListener(CC_CALLBACK_0(Title::onMusicClick, this));

  lbl = PPLabel::create(
      LM->getText(GM->getMusicVolumn() == 0 ? "music off" : "music on"), 60,
      DARK_GRAY_3B, true, false, TextHAlignment::CENTER, false);
  lbl->setName("music");
  layer->addChild(lbl);
  lbl->setPosition(btn->getPosition());
}
void Title::onEffectClick() {
  float vol = GM->getSoundVolumn();
  PPLabel *lbl = (PPLabel *)getPopup()->getChildByName("effect");
  if (vol == 0) {
    GM->setSoundVolumn(1);
    lbl->setString(LM->getText("effect on"));
  } else {
    GM->setSoundVolumn(0);
    lbl->setString(LM->getText("effect off"));
  }
}
void Title::onMusicClick() {
  float vol = GM->getMusicVolumn();
  PPLabel *lbl = (PPLabel *)getPopup()->getChildByName("music");
  if (vol == 0) {
    GM->setMusicVolumn(0.3f);
    lbl->setString(LM->getText("music on"));
  } else {
    GM->setMusicVolumn(0);
    lbl->setString(LM->getText("music off"));
  }
}

void Title::onStageClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  //    if(btn->getTag() >= 5){ // test
  //        showInstanceMessage("Wait for the update!");
  //        return;
  //    }
  btn->setEnabled(false);
  if (btn->getTag() == 0) {
    clearAssets();
    GM->titleLayer = nullptr;
    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
        listener);
    GM->nextScene = STAGE_INTRO;
    GM->isColosseum = false;
    auto scene = GameScene::scene(btn->getTag(), difficultyMode);
    Director::getInstance()->replaceScene(
        TransitionFade::create(2, scene, Color3B::BLACK));
    return;
  }
  showStageReady(btn->getTag());
}
void Title::onEnterBackground() {}
void Title::showStageReady(int stage) {
  GM->playedStageIndexForTitle = stage;
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Layer *layer = Layer::create();
  setAsPopup(layer);
  this->addChild(layer);
  float mapZoomDelay = 2.0f;
  float showUIDelay = 3.5f;
  Button *btnBlockBack = Button::create("worldMap.png");
  btnBlockBack->setPosition(Point(size.width / 2, size.height / 2));
  layer->addChild(btnBlockBack);
  btnBlockBack->setColor(Color3B::BLACK);
  btnBlockBack->setScale(10 * 2224.0f / 2048);
  Button *btnBlock = Button::create("worldMap.png");
  btnBlock->setPosition(Point(size.width / 2, size.height / 2 + 100));
  layer->addChild(btnBlock);
  btnBlock->setScale(1.2 * 2224.0f / 2048);
  btnBlock->setZoomScale(0);
  btnBlock->setOpacity(0);
  btnBlock->runAction(FadeIn::create(0.5f));
  btnBlock->runAction(Sequence::create(
      DelayTime::create(mapZoomDelay / 2),
      EaseInOut::create(ScaleTo::create(1, 1.9f * 2224.0f / 2048), 2),
      nullptr));
  if (stage >= 3) {
    btnBlock->runAction(Sequence::create(
        DelayTime::create(mapZoomDelay / 2),
        EaseInOut::create(MoveTo::create(1, Point(size.width / 2 - 940,
                                                  size.height / 2 - 500)),
                          2),
        nullptr));
  } else {
    btnBlock->runAction(Sequence::create(
        DelayTime::create(mapZoomDelay / 2),
        EaseInOut::create(MoveTo::create(1, Point(size.width / 2 + 940,
                                                  size.height / 2 - 500)),
                          2),
        nullptr));
  }

  if (stage >= 0) {
    btnBlock->runAction(Sequence::create(
        DelayTime::create(mapZoomDelay),
        EaseInOut::create(MoveTo::create(1, Point(size.width / 2 - 200,
                                                  size.height / 2 + 400)),
                          2),
        nullptr));
  }
  btnBlock->setPressedActionEnabled(false);

  Button *btnBlackBack = Button::create("uiBox.png");
  layer->addChild(btnBlackBack);
  btnBlackBack->setPosition(size / 2);
  btnBlackBack->setScale(20);
  //    btn->addClickEventListener(CC_CALLBACK_1(Title::goToStage, this));
  btnBlackBack->setOpacity(0);
  btnBlackBack->runAction(Sequence::create(DelayTime::create(showUIDelay),
                                           FadeTo::create(2, 100), NULL));
  btnBlackBack->setColor(Color3B::BLACK);

  Button *btn = Button::create("uiBox.png");
  layer->addChild(btn);
  btn->setScale9Enabled(true);
  btn->setContentSize(cocos2d::Size(550, 234));
  btn->setPosition(Point(size.width - 400, 180));
  btn->setTag(stage);
  btn->addClickEventListener(CC_CALLBACK_1(Title::goToStage, this));
  btn->setOpacity(0);
  btn->runAction(Sequence::create(DelayTime::create(showUIDelay),
                                  FadeIn::create(2), NULL));
  PPLabel *lbl = addLabelToButton(btn, "play", 60, Color3B(142, 45, 29));
  doLabelFadeInLater(lbl, showUIDelay, 2);

  ImageView *objectBack = ImageView::create("missionBoard.png");
  layer->addChild(objectBack);
  objectBack->setScale9Enabled(true);
  objectBack->setContentSize(cocos2d::Size(700, 600));
  objectBack->setPosition(Point(size.width - 100, size.height - 100));
  objectBack->setAnchorPoint(Point(1, 1));
  objectBack->setOpacity(0);
  objectBack->runAction(Sequence::create(DelayTime::create(showUIDelay),
                                         FadeTo::create(2, 255), NULL));

  int y = size.height - 260;
  int x = size.width - 740;
  //    lbl = PPLabel::create("MISSION" , 50, DARK_GRAY_3B, true, false,
  //    TextHAlignment::CENTER, false); layer->addChild(lbl);
  //    lbl->setPosition(Point(x + 280, y + 40));
  doLabelFadeInLater(lbl, showUIDelay, 2);
  if (GM->getStageObjective(stage) ==
      CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND) {
    for (int i = 0; i < 3; i++) {
      if (i == 0) {
        lbl = PPLabel::create(
            strmake(LM->getText("objective build farm").c_str(), 2), 50,
            Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
      } else if (i == 1) {
        lbl = PPLabel::create(
            strmake(LM->getText("objective build barracks").c_str(), 1), 50,
            Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
      } else if (i == 2) {
        lbl = PPLabel::create(
            strmake(LM->getText("objective train swordsman").c_str(), 4), 50,
            Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
      }

      layer->addChild(lbl);
      lbl->setPosition(Point(x, y - i * 100 - 100));
      doLabelFadeInLater(lbl, showUIDelay, 2);
    }
  } else if (GM->getStageObjective(stage) == CLEAR_CONDITION_TERMINATE) {
    lbl = PPLabel::create(LM->getText("objective destroy them all").c_str(), 50,
                          Color3B(142, 45, 29), true, false,
                          TextHAlignment::LEFT, false);
    layer->addChild(lbl);
    lbl->setPosition(Point(x, y - 100));
    lbl->setAnchorPoint(Point(0, 0.5));
    doLabelFadeInLater(lbl, showUIDelay, 2);
  } else if (GM->getStageObjective(stage) ==
             CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
    for (int i = 0; i < 2; i++) {
      if (i == 0) {
        lbl = PPLabel::create(
            strmake(LM->getText("objective protect all").c_str(), 2), 50,
            Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
      } else if (i == 1) {
        lbl = PPLabel::create(
            strmake(LM->getText("objective destroy them all").c_str(), 1), 50,
            Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
      }

      layer->addChild(lbl);
      lbl->setPosition(Point(x, y - i * 100 - 100));
      doLabelFadeInLater(lbl, showUIDelay, 2);
    }
  } else if (GM->getStageObjective(stage) ==
                 CLEAR_CONDITION_SURVIVE_FOR_10_MIN ||
             GM->getStageObjective(stage) ==
                 CLEAR_CONDITION_SURVIVE_FOR_20_MIN) {
    int min = 10;
    if (GM->getStageObjective(stage) == CLEAR_CONDITION_SURVIVE_FOR_20_MIN) {
      min = 20;
    }
    lbl = PPLabel::create(
        strmake(LM->getText("objective survive").c_str(), min), 50,
        Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
    layer->addChild(lbl);
    lbl->setPosition(Point(x, y - 100));
    lbl->setAnchorPoint(Point(0, 0.5));
    lbl->setWidth(600);
    doLabelFadeInLater(lbl, showUIDelay, 2);
  }

  ImageView *storyBack = ImageView::create("verticalScrollBackground.png");
  layer->addChild(storyBack);
  storyBack->setScale9Enabled(true);
  storyBack->setContentSize(cocos2d::Size(1400, 1030));
  storyBack->setPosition(Point(100, size.height - 100));
  storyBack->setAnchorPoint(Point(0, 1));
  storyBack->setOpacity(0);
  storyBack->runAction(Sequence::create(DelayTime::create(showUIDelay),
                                        FadeTo::create(2, 255), NULL));

  ScrollView *sv = ScrollView::create();
  sv->setContentSize(cocos2d::Size(1350, 855));
  sv->setPosition(Point(150, size.height - 190));
  sv->setAnchorPoint(Point(0, 1));
  sv->setScrollBarEnabled(false);
  layer->addChild(sv);

  //    lbl = LM->getLocalizedLabel(LM->getText(strmake("stage title story %d",
  //    stage).c_str()).c_str(), Color4B::BLACK, 70);
  if (selectedChapter < 0) {
    selectedChapter = 0;
  }
  PPLabel *ppLabel = PPLabel::create(
      LM->getText(
            strmake("stage title story %d_%d", selectedChapter, stage % 12)
                .c_str())
          .c_str(),
      70, Color3B(142, 45, 29), true, false, TextHAlignment::LEFT, false);
  sv->addChild(ppLabel);
  ppLabel->setWidth(1200);
  //    lbl->setDimensions(1300, 1000);
  ppLabel->setAnchorPoint(Point(0, 1));
  ppLabel->setPosition(Point(50, -50));
  float textHeight = ppLabel->getSize().height;
  ppLabel->runAction(Sequence::create(
      DelayTime::create(mapZoomDelay),
      MoveBy::create(40 + (textHeight - 1500) / 100,
                     Point(0, textHeight + 50 + sv->getContentSize().height)),
      nullptr));
  //    ppLabel->disableEffect(LabelEffect::SHADOW);
  doLabelFadeInLater(ppLabel, showUIDelay, 2);
}
PPLabel *Title::addLabelToButton(Button *btn, std::string text, int fontSize,
                                 Color3B color, bool isBordered) {
  //    Label* lbl = LM->getLocalizedLabel(text.c_str(), Color4B::BLACK,
  //    fontSize); btn->addChild(lbl);
  //    lbl->setPosition(btn->getContentSize()/2);
  //    lbl->setName("lbl");
  //    lbl->disableEffect(LabelEffect::SHADOW);
  //    return lbl;
  PPLabel *lbl = PPLabel::create(
      LM->getText(text.c_str()), btn->getContentSize().height * 2 / 6, color,
      true, isBordered, TextHAlignment::CENTER, false);
  lbl->setPosition(
      Point(btn->getContentSize().width / 2, btn->getContentSize().height / 2));
  btn->addChild(lbl);
  return lbl;
}
PPLabel *Title::replaceTextToPPLabel(Text *lblText) {
  PPLabel *lbl =
      PPLabel::create(lblText->getString(), lblText->getFontSize(),
                      Color3B(lblText->getTextColor()), true,
                      lblText->getOutlineSize(), TextHAlignment::CENTER, false);
  lbl->setPosition(lblText->getPosition());
  lblText->getParent()->addChild(lbl);
  lblText->removeFromParent();
  return lbl;
}
void Title::addLabelToButtonFromHud(std::string text, Button *btn,
                                    bool bordered, Color3B color,
                                    bool isSystemLabel) {
  PPLabel *lbl = PPLabel::create(
      LM->getText(text.c_str()), btn->getContentSize().height * 2 / 6, color,
      isSystemLabel, bordered, TextHAlignment::CENTER, false);
  lbl->setPosition(
      Point(btn->getContentSize().width / 2, btn->getContentSize().height / 2));
  btn->addChild(lbl);
}
void Title::goToStage(Ref *ref) {
  clearAssets();
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  BTN_FROM_REF_AND_DISABLE
  btn->setEnabled(false);
  GM->isColosseum = false;
  GM->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  GM->nextScene = STAGE_FIELD;
  auto scene = GameScene::scene(btn->getTag(), difficultyMode);
  WORLD->difficultyMode = difficultyMode;
  Director::getInstance()->replaceScene(
      TransitionFade::create(2, scene, Color3B::BLACK));
}
void Title::onServerChecked() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  return;
#endif
  Node *title = this->getChildByName("titleLayer");
  Button *btn = (Button *)title->getChildByName("btnPlay");
  if (GM->getTimeLeftForCampaignChest(0) <= 0 ||
      GM->getTimeLeftForCampaignChest(1) <= 0 ||
      GM->getTimeLeftForCampaignChest(2) <= 0) { // new chest
    btn->getChildByName("imgRedDot")->setVisible(true);
  } else {
    btn->getChildByName("imgRedDot")->setVisible(false);
  }
}
void Title::titleUpdate(float dt) {
  if (isTitleEnd) {
    return;
  }
  if (!isServerCheckArrived && GM->receivedVersionCode >= 0) {
    isServerCheckArrived = true;
    onServerChecked();
  }
  if (GM->market != MARKET_SMARTPASS) {
    if (GM->versionCode < GM->receivedVersionCode &&
        !this->getChildByName("forceUpdate")) {
      Node *layer = CSLoader::createNode("UnderConstruction.csb");
      layer->setName("forceUpdate");
      this->addChild(layer, POPUP_ZORDER);
      layer->setGlobalZOrder(100);
      layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
      Text *lbl = (Text *)layer->getChildByName("lblDescription");
      LM->setLocalizedString(lbl, "update required");
      Button *btn = (Button *)layer->getChildByName("btnOk");
      btn->addClickEventListener(CC_CALLBACK_0(Title::onUpdateClick, this));
      ImageView *img = (ImageView *)layer->getChildByName("imgWorker");
      Sprite *spt = Sprite::createWithSpriteFrameName("workerAxeAttack0.png");
      layer->addChild(spt);
      spt->setScale(2);
      spt->setPosition(img->getPosition());
      img->setVisible(false);
    }
    if (GM->serverMaintenance > 0 &&
        !this->getChildByName("underMaintenance")) { // test
      Node *layer = CSLoader::createNode("UnderConstruction.csb");
      this->addChild(layer, POPUP_ZORDER);
      layer->setName("underMaintenance");
      layer->setGlobalZOrder(100);
      layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
      Text *lbl = (Text *)layer->getChildByName("lblDescription");
      std::string str =
          strmake(LM->getText("server maintenance description format").c_str(),
                  GM->serverMaintenance);
      LM->setLocalizedStringNotKey(lbl, str);
      Button *btn = (Button *)layer->getChildByName("btnOk");
      btn->addClickEventListener(CC_CALLBACK_0(Title::onExitClick, this));
      ImageView *img = (ImageView *)layer->getChildByName("imgWorker");
      Sprite *spt = Sprite::createWithSpriteFrameName("workerAxeAttack0.png");
      spt->setScale(2);
      layer->addChild(spt);
      spt->setPosition(img->getPosition());
      img->setVisible(false);
    }
  }

  if (GM->rewardCode > 0 || BSM->postBoxConent.length() > 1) {
    int lastRewardCode = GM->rewardedCode;
    Node *ndRedDot = this->getChildByName("titleLayer")
                         ->getChildByName("btnNotice")
                         ->getChildByName("imgRedDot");
    ndRedDot->setVisible(lastRewardCode != GM->rewardCode ||
                         BSM->postBoxConent.length() > 1);
  }

  time_t now = BSM->getCurrentTimeT();
  std::string strNextWoodChestGachaFreeTime =
      UDGetStr(KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME, "");
  time_t nextWoodChestGachaFreeTimeT =
      BSM->getTimeTFromStr(strNextWoodChestGachaFreeTime);
  bool isWoodChestGachaFreeReady =
      difftime(nextWoodChestGachaFreeTimeT, now) <= 0;

  std::string strNextGoldChestGachaFreeTime =
      UDGetStr(KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME, "");
  time_t nextGoldChestGachaFreeTimeT =
      BSM->getTimeTFromStr(strNextGoldChestGachaFreeTime);
  bool isGoldChestGachaFreeReady =
      difftime(nextGoldChestGachaFreeTimeT, now) <= 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#else
  Node *ndChapterSelect = this->getChildByName("chapterSelect");
  if (ndChapterSelect && ndChapterSelect != nullptr) {
    ndChapterSelect->getChildByName("btnHero")
        ->getChildByName("imgRedDot")
        ->setVisible(isWoodChestGachaFreeReady || isGoldChestGachaFreeReady);
    ScrollView *sv = (ScrollView *)ndChapterSelect->getChildByName("sv");
    Button *btn;
    Text *lbl;
    for (int i = 0; i < 3; i++) {
      btn = (Button *)sv->getChildByName(strmake("btnChapter%d", i));
      int timeLeftForChest = GM->getTimeLeftForCampaignChest(i);
      lbl = (Text *)btn->getChildByName("lblChestTimeLeft");
      btn->getChildByName("imgRedDot")->setVisible(timeLeftForChest <= 0);
      if (timeLeftForChest <= 0) {
        lbl->setVisible(false);
      } else {
        lbl->setString(GM->getTimeLeftInString(timeLeftForChest));
      }
    }
  }
#endif
  Node *layer = this->getChildByName("stageSelect");
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//
//#elif
//
//  if (layer && layer != nullptr) {
//    int chestIndex = UDGetInt(
//        strmake(KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT, layer->getTag()).c_str(),
//        0);
//    ScrollView *sv = (ScrollView *)layer->getChildByName("sv");
//    Button *btn = (Button *)sv->getChildByName(strmake("btn%d", chestIndex));
//    Text *lblTimeLeft = (Text *)btn->getChildByName("lblTimeLeft");
//    int timeLeft = GM->getTimeLeftForCampaignChest(layer->getTag());
//    lblTimeLeft->setVisible(timeLeft > 0);
//    if (timeLeft > 0) {
//      lblTimeLeft->setString(GM->getTimeLeftInString(timeLeft));
//    }
//  }
//#endif

  if (isGameInfoRecieved) {
    isGameInfoRecieved = false;
    int receivedVersion = -1;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS ||                                  \
     CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    receivedVersion = iv;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    receivedVersion = av;
#endif
    hideIndicator();
    if (receivedVersion > GM->versionCode) {
      // should update the game
    } else {
      if (UDGetInt(KEY_RECEIVED_REWARD_INDEX, 0) < rewardIndex) {
        showIndicator();
        BSM->getRewardInfo();
      }
    }
  }
  if (isRewardInfoReceived) {
    isRewardInfoReceived = false;
    log("reward info: %s", rewardInfo);
  }

  //    layer->getChildByName("btnTrainBR")->getChildByName("imgRedDot")->setVisible(isGachaFreeReady
  //    || isGachaVideoReady);

  //    if (GM->iapStateFlag == IAP_STATE_NOT_INIT) {
  //        lblLoading->setString("Loading products...");
  //        return;
  //    }else if (GM->iapStateFlag == IAP_STATE_INIT_SUCCESS) {
  //        lblLoading->setString("Setting products...");
  //        GM->priceList.push_back(GameSharing::getIAPPrice("starter0"));//
  //        IAP_STARTER_0
  //        GM->priceList.push_back(GameSharing::getIAPPrice("starter1"));//
  //        IAP_STARTER_1
  //        GM->priceList.push_back(GameSharing::getIAPPrice("starter2"));//
  //        IAP_STARTER_2
  //        GM->priceList.push_back(GameSharing::getIAPPrice("package0"));//
  //        IAP_PACKAGE_0
  //        GM->priceList.push_back(GameSharing::getIAPPrice("package1"));//
  //        IAP_PACKAGE_1
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem0"));//
  //        IAP_GEM_0
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem1"));//
  //        IAP_GEM_1
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem2"));//
  //        IAP_GEM_2
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem3"));//
  //        IAP_GEM_3
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem4"));//
  //        IAP_GEM_4
  //        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0"));//
  //        IAP_TICKET_0
  //        GM->priceList.push_back(GameSharing::getIAPPrice("ticket1"));//
  //        IAP_TICKET_1
  //        GM->priceList.push_back(GameSharing::getIAPPrice("ticket2"));//
  //        IAP_TICKET_2
  //        GM->priceList.push_back(GameSharing::getIAPPrice("starter1original"));//
  //        IAP_STARTER_1_ORIGINAL 12
  //        GM->priceList.push_back(GameSharing::getIAPPrice("starter2original"));//
  //        IAP_STARTER_2_ORIGINAL 13
  //        GM->priceList.push_back(GameSharing::getIAPPrice("package0original"));//
  //        IAP_PACKAGE_0_ORIGINAL 14
  //        GM->priceList.push_back(GameSharing::getIAPPrice("package1original"));//
  //        IAP_PACKAGE_1_ORIGINAL 15
  //        GM->priceList.push_back(GameSharing::getIAPPrice("gem0original"));//
  //        IAP_GEM_0_ORIGINAL 16
  //        GM->priceList.push_back(GameSharing::getIAPPrice("ticket0original"));//
  //        IAP_TICKET_0_ORIGINAL 17

  //        GM->iapStateFlag = IAP_STATE_INIT_DONE;
  //        return;
  //    }
  if (usesAccountSystem()) {
    if (isUserIDExistCheckArrived) {
      isUserIDExistCheckArrived = false;
      UDSetBool(KEY_ID_EXIST_CHECK_DONE, true);
      if (GM->playerIDWithGPSID.size() == 0) {
        if (UDGetStr(KEY_SAVED_ID, "-1").compare("-1") != 0) {
          BSM->saveUserData(strmake("playid=%s", GM->playerGPSID.c_str()));
          settingLog("Restore code: 1");
        } else {
          settingLog("Restore code: 2");
        }
      } else {
        if (UDGetStr(KEY_SAVED_ID, "-1").compare(GM->playerIDWithGPSID) != 0) {
          std::string gpsID = "";
          gpsID += GM->playerIDWithGPSID;
          UDSetStr(KEY_SAVED_ID, gpsID);
          UDSetBool(KEY_HERO_INVALIDATE, true);
          UDSetBool(KEY_BATTLE_DATA_INVALIDATE, true);
          UDSetBool(KEY_MONTHLY_EVENT_INFO_INVALIDATE, true);
          BSM->requestedID = gpsID;
          BSM->getAllUserData();
          showIndicator();
          this->runAction(Sequence::create(
              DelayTime::create(4),
              CallFunc::create(CC_CALLBACK_0(Title::restartTheGame, this)),
              NULL));
          settingLog("Restore code: 3");
          return;
        } else {
          settingLog("Restore code: 4");
        }
      }
    }
    if (GM->playerGPSID.size() > 0 &&
        !UDGetBool(KEY_ID_EXIST_CHECK_DONE, false) &&
        !isUserIDExistCheckRequested) {
      checkPlayerIDExist(); // test
    }
  }
  if (BSM->isServerFailed) {
    BSM->isServerFailed = false;
    if (!this->getChildByName("lblInstanceMessage")) {
      hideIndicator();
      showInstanceMessage(LM->getText("network fail play offline"));
    }
    if (usesAccountSystem() && lblID != nullptr) {
      lblID->setString(strmake("ErrorCode: %d|%s", BSM->errorCode,
                               BSM->errorDetail.c_str()));
    }
    hideIndicator();
    //        BSM->checkServer();
    return;
  }
  if (BSM->isCheckServerComplete) {
    BSM->isCheckServerComplete = false;

    BSM->isInitUserDataRequested = true;
    firstIndicatorDisposed = false;
    BSM->getUserData(
        "gem=1&trophy=1&post=1&ban=1&name=1&rewarded_index=1&defence_record=1");
  }
  if (isRestoreRequested && GM->iapFlag == IAP_FLAG_RESTORED) {
    isRestoreRequested = false;
    GM->iapFlag = IAP_FLAG_INIT;
    showInstanceMessage(LM->getText("restore complete"));
  }
  if (isHeroInfoArrived) {
    hideIndicator();
    isHeroInfoArrived = false;
    //        GM->isHeroLoaded = true;
    showHeroPage();
  }
  if (isAllUserDataArrived) {
    isAllUserDataArrived = false;
    hideIndicator();
    showInstanceMessage(LM->getText("restore complete"));
  }
  if (usesAccountSystem()) {
    if (nameHandleState == NETWORK_HANDLE_STATE_NOT_READY) {
      lblLoading->setString("Checking id...");
      std::string id = UDGetStr(KEY_SAVED_ID, "-1");
      BSM->requestedID = id;
      if (id.compare("-1") == 0) {
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        hideIndicator();
      } else {
        nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
        lblID->setString(strmake("GAME ID: %s", id.c_str()));
        lblLoading->setString(
            strmake(LM->getText("welcome remember password").c_str(),
                    UDGetStr(KEY_NAME).c_str()));
        log("name: %s", UDGetStr(KEY_NAME).c_str());
      }
    }
  }
  if ((usesAccountSystem() || GM->deferredAccountRegistrationActive) &&
      nameHandleState == NETWORK_HANDLE_STATE_ARRIVED) {
    hideIndicator();
    if (isSendingPassword) {
      isSendingPassword = false;
      nameHandleState = NETWORK_HANDLE_STATE_HANDLED;
    } else {
      if (BSM->resultMessage.compare("same name exist") == 0) {
        showInstanceMessage(LM->getText("same name exist"));
        nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      } else if (BSM->resultMessage.compare("save failed") == 0) {
        showInstanceMessage(LM->getText("network retry"));
        nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      } else if (BSM->resultMessage.compare("database failure") == 0) {
        showInstanceMessage(LM->getText("network retry"));
        nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      } else if (GM->deferredAccountRegistrationActive &&
                 GameManager::isWin32DeferredAuth()) {
        if (nameToRegister.length() > 0) {
          UDSetStr(KEY_NAME, nameToRegister);
        }
        nameHandleState = NETWORK_HANDLE_STATE_HANDLED;
      } else {
        if (isRename) {
          isRename = false;
          isNameRequested = false;
          GM->addGem(-100);
        } else {
          isSendingPassword = true;
        }
        int gem = GM->getGem();

        nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
        showIndicator();
        TextField *tfPassword = (TextField *)this->getChildByName("namePopup")
                                    ->getChildByName("imgBackground")
                                    ->getChildByName("tfPassword");
        isNameRequested = true;
        BSM->saveUserData(strmake("password=%s&gem=%d",
                                  tfPassword->getString().c_str(), gem));
        UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test
      }
    }
  }

  if ((usesAccountSystem() || GM->deferredAccountRegistrationActive) &&
      nameHandleState == NETWORK_HANDLE_STATE_HANDLED) {
    nameHandleState = NETWORK_HANDLE_STATE_COMPLETE;
    isNameRequested = false;
    if (nameToRegister.length() > 0) {
      UDSetStr(KEY_NAME, nameToRegister);
    }
    closePopup();
    if (GM->deferredAccountRegistrationActive) {
      GM->completeDeferredAccountRegistration(true);
    } else {
      std::string id = UDGetStr(KEY_SAVED_ID, "-1");
      lblID->setString(strmake("GAME ID: %s", id.c_str()));

      LM->setLocalizedString(lblLoading, "touch to start");
      lblLoading->runAction(RepeatForever::create(
          Sequence::create(FadeOut::create(0.5f), FadeIn::create(0.5f),
                           DelayTime::create(0.5f), NULL)));
      lblLoading->setString(
          strmake(LM->getText("welcome remember password").c_str(),
                  UDGetStr(KEY_NAME).c_str()));
      log("name: %s", UDGetStr(KEY_NAME).c_str());
      closePopup();
    }
  }
  if (usesAccountSystem() &&
      renameHandleState == NETWORK_HANDLE_STATE_ARRIVED) {
    renameHandleState = NETWORK_HANDLE_STATE_HANDLED;
    if (BSM->resultMessage.compare("same name exist") == 0) {
      showInstanceMessage(LM->getText("same name exist"));
      nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      hideIndicator();
    } else if (BSM->resultMessage.compare("save failed") == 0) {
      showInstanceMessage(LM->getText("network retry"));
      nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      hideIndicator();
    } else if (BSM->resultMessage.compare("database failure") == 0) {
      showInstanceMessage(LM->getText("network retry"));
      nameHandleState = NETWORK_HANDLE_STATE_NOT_READY;
      hideIndicator();
    } else {
      closePopup();
      closePopup();
      isNameRequested = false;
      GM->addGem(-100);
      //            renameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
      showIndicator();
      //            BSM->saveUserData(strmake("password=%s&gem=%d",
      //            tfPassword->getString().c_str(), gem));
      //            UDSetBool(KEY_ID_EXIST_CHECK_DONE, false); // test
      BSM->saveUserData("gem=" + Value(GM->getGem()).asString());
    }
  }

  //    if(isFreeAppResultArrived){
  //        isFreeAppResultArrived = false;
  //        hideIndicator();
  //        if(isFreeApp){
  //            UDSetBool(KEY_IS_FREE_USER, true);
  //            UD->flush();
  //            showFreeAppAlert();
  //        }
  //    }

  if (isVideoDone) {
    isVideoDone = false;
    showInstanceMessage(LM->getText("ticket received"));
  }
  if (getChildByName("shopLayer") == nullptr &&
      getChildByName("heroPage") == nullptr) {
    if (GM->iapFlag == IAP_FLAG_SUCCESS) {
      GM->iapFlag = IAP_FLAG_INIT;
      log("iap success!");
      if (GM->iapDetail.compare(IAP_DETAIL_TICKET_1) == 0) {
        int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
        ticket += 12;
        UDSetInt(KEY_GOLDEN_TICKET, ticket);
        isColosseumInfoRefreshRequired = true;
        hideIndicator();
        showInstanceMessage(LM->getText("purchase success"));
        GameSharing::consumePurchased();
      } else if (GM->iapDetail.compare(IAP_DETAIL_CHAPTER2) == 0) {
        UDSetBool(KEY_CHAPTER_2_PURCHASED, true);
        closePopup();
        showChapterSelect();
      } else if (GM->iapDetail.compare(IAP_DETAIL_CHAPTER3) == 0) {
        UDSetBool(KEY_CHAPTER_3_PURCHASED, true);
        closePopup();
        showChapterSelect();
      }
    } else if (GM->iapFlag == IAP_FLAG_FAILED) {
      GM->iapFlag = IAP_FLAG_INIT;
      log("iap failed!");
      hideIndicator();
    }
  }

  if (strMessageBox.size() > 0) {
    onMessageBoxReceived();
    strMessageBox = "";
  }

  if (BSM->isInitUserDataReceived) {
    checkMonthlyEventAttend();
    if (!firstIndicatorDisposed) {
      firstIndicatorDisposed = true;
      hideIndicator();
    }

    if (BSM->isBannedUser) {
      Node *layer = CSLoader::createNode("MessageBox.csb");
      this->addChild(layer, 4);
      setAsPopup(layer);
      layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
      layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);

      Button *btn = (Button *)layer->getChildByName("btnNo");
      btn->removeFromParent();
      btn = (Button *)layer->getChildByName("btnYes");
      btn->setVisible(false);
      Text *lbl = (Text *)btn->getChildByName("lbl");
      LM->setLocalizedString(lbl, "ok");

      lbl = (Text *)layer->getChildByName("lblDescription");
      LM->setLocalizedString(lbl, "banned description");

      if (usesAccountSystem()) {
        lbl->setString(strmake("%s\nID:%s", lbl->getString().c_str(),
                               UDGetStr(KEY_SAVED_ID).c_str()));
      }
    }
  }
  //    if (GM->market == MARKET_SMARTPASS) {
  //        if (GM->strSmartPassError.length() > 0) {
  //            showSmartPassError(GM->strSmartPassError);
  //            GM->strSmartPassError = "";
  //        }
  //    }
  // titleupdate done
}
void Title::showPostPopup() {
  this->getChildByName("titleLayer")
      ->getChildByName("btnNotice")
      ->getChildByName("imgRedDot")
      ->setVisible(false);
  Node *layer = CSLoader::createNode("NoticePopup.csb");
  this->addChild(layer, 4);
  layer->setName("noticePopup");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);
  Button *btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(PageBase::closePopup, this));
  ImageView *img = (ImageView *)layer->getChildByName("imgBack");
  img->addClickEventListener(CC_CALLBACK_0(EventPopup::closePopup, this));
  Text *lbl = (Text *)layer->getChildByName("lblNoNotice");
  LM->setLocalizedString(lbl, "no notice");
  lbl = (Text *)layer->getChildByName("imgTitle")->getChildByName("lbl");
  LM->setLocalizedString(lbl, "postbox");

  Button *btnTemp;

  ScrollView *sv = (ScrollView *)layer->getChildByName("sv");
  float y = 0;
  float x = 922;
  int gapY = 360;
  // notice
  int lastRewardCode = GM->rewardedCode;
  log("rewardedCode: %d, GM->rewardCode: %d", lastRewardCode, GM->rewardCode);
  if (GM->rewardCode != lastRewardCode && GM->rewardCode > 0) {
    btnTemp = (Button *)layer->getChildByName("btnGemTemp");
    btn = (Button *)btnTemp->clone();
    sv->addChild(btn);
    btn->setPosition(Vec2(x, y));
    btn->addClickEventListener(CC_CALLBACK_0(Title::onRewardNoticeClick, this));
    lbl = (Text *)btn->getChildByName("lblTitle");
    ValueVector list = GM->split(GM->rewardMsg, "|");
    std::string title = "";
    if (list.size() > 0) {
      title = list.at(0).asString();
      LM->setLocalizedStringNotKey(lbl, title);
    }

    y -= gapY;
  }

  //    UDSetStr(KEY_POST_MESSAGE, "unit-_54/101_55/101-Reward is
  //    good|gem-22-Reward is also good"); // test BSM->postBoxConent =
  //    UDGetStr(KEY_POST_MESSAGE, "");
  // post box
  if (BSM->postBoxConent.length() > 0) {
    std::string postBox = BSM->postBoxConent.c_str();
    UDSetStr(KEY_POST_MESSAGE, postBox);
    BSM->saveUserData("post=|");

    ValueVector list = GM->split(postBox, "|");
    for (int i = 0; i < list.size(); i++) {
      std::string strContent = list.at(i).asString();
      if (strContent.length() < 1) {
        continue;
      }
      ValueVector contentList = GM->split(strContent, "-");
      if (contentList.size() == 3) {
        if (contentList.at(0).asString().compare("gem") == 0) {
          int gemCount = contentList.at(1).asInt();

          btnTemp = (Button *)layer->getChildByName("btnGemTemp");
          btn = (Button *)btnTemp->clone();
          sv->addChild(btn);
          btn->setPosition(Vec2(x, y));
          btn->setName(strContent);
          btn->setTag(gemCount);
          btn->addClickEventListener(
              CC_CALLBACK_1(Title::onPostItemGemClick, this));
          lbl = (Text *)btn->getChildByName("lblTitle");
          std::string title = contentList.at(2).asString();
          LM->setLocalizedStringNotKey(lbl, title);
        } else if (contentList.at(0).asString().compare("unit") == 0) {
          btnTemp = (Button *)layer->getChildByName("btnUnitTemp");
          btn = (Button *)btnTemp->clone();
          sv->addChild(btn);
          btn->setPosition(Vec2(x, y));
          btn->setName(strContent);
          btn->addClickEventListener(
              CC_CALLBACK_1(Title::onPostItemUnitClick, this));
          lbl = (Text *)btn->getChildByName("lblTitle");
          std::string title = contentList.at(2).asString();
          LM->setLocalizedStringNotKey(lbl, title);
        }

        y -= gapY;
      }
    }
  }

  int scrollHeight = sv->getContentSize().height;
  if (scrollHeight < -y) {
    scrollHeight = -y;
  }
  sv->setInnerContainerSize(
      cocos2d::Size(sv->getContentSize().width, scrollHeight));
  for (auto btn : sv->getChildren()) {
    btn->setPositionY(btn->getPositionY() + scrollHeight - gapY / 2);
  }

  lbl = (Text *)layer->getChildByName("lblNoNotice");
  lbl->setVisible(sv->getChildren().size() <= 0);
}
void Title::onPostItemUnitClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  ValueVector contentList = GM->split(btn->getName(), "-");
  selectedPostItem = btn->getName() + "";
  Button *btnOk = showUnitRewardPopup(contentList.at(1).asString(),
                                      contentList.at(2).asString().c_str());
  btnOk->addClickEventListener(
      CC_CALLBACK_0(Title::onPostItemUnitOkClick, this));
}
Button *Title::showUnitRewardPopup(std::string units, const char *msg) {
  Node *layer = CSLoader::createNode("UnitRewardMessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("unitRewardMessageBox");
  setAsPopup(layer);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  ImageView *imgBack = (ImageView *)layer->getChildByName("imgBackground");
  GM->rollOpenScroll(imgBack);

  Button *btn = (Button *)layer->getChildByName("btnOk");
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "ok");

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedStringNotKey(lbl, msg);

  ScrollView *sv = (ScrollView *)layer->getChildByName("sv");

  ValueVector unitList = GM->split(units, "_");
  int x = 192;
  int xGap = 281;
  for (int i = 0; i < unitList.size(); i++) {
    ValueVector dataList = GM->split(unitList.at(i).asString(), "/");
    if (dataList.size() > 1) {
      ImageView *img =
          (ImageView *)((ImageView *)sv->getChildByName("imgTemp"))->clone();
      sv->addChild(img);
      img->setPosition(Vec2(x, sv->getContentSize().height / 2 - 10));
      UnitInfo *info = new UnitInfo();
      Vec2 pos = img->getPosition();
      info->unitType = dataList.at(0).asInt();
      info->level = dataList.at(1).asInt() % 100;
      info->rank = dataList.at(1).asInt() / 100;
      lbl = (Text *)img->getChildByName("lblLevel");
      lbl->setString(strmake("LV.%d", info->level + 1));
      spine::SkeletonAnimation *spChar = GM->getHeroSpine(info->unitType);
      sv->addChild(spChar);
      spChar->setAnimation(0, "idle", true);
      Sprite *sptShadow = Sprite::create("shadow.png");
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
    int offsetX = (sv->getContentSize().width - x) / 2 - 110;
    for (auto child : sv->getInnerContainer()->getChildren()) {
      child->setPositionX(child->getPositionX() + offsetX);
    }
  } else {
    sv->setInnerContainerSize(cocos2d::Size(x + xGap / 2, 287));
  }

  return btn;
}
void Title::onPostItemUnitOkClick() {
  std::string post = UDGetStr(KEY_POST_MESSAGE, "");
  log("post %s: ", post.c_str());
  post = post.erase(post.find(selectedPostItem), selectedPostItem.length());
  log("after post %s: ", post.c_str());
  UDSetStr(KEY_POST_MESSAGE, post.c_str());
  log("saved post %s: ", UDGetStr(KEY_POST_MESSAGE, "_").c_str());
  BSM->postBoxConent = post + "";

  std::vector<UnitInfo *> unitInfoListHeroInventory = GM->getHeroInventory();

  ValueVector list = GM->split(selectedPostItem, "-");
  if (list.size() > 1) {
    std::string strUnits = list.at(1).asString();
    ValueVector unitList = GM->split(strUnits, "_");
    for (int i = 0; i < unitList.size(); i++) {
      ValueVector dataList = GM->split(unitList.at(i).asString(), "/");
      if (dataList.size() > 1) {
        //                strInventory += "_" + unitList.at(i).asString();
        UnitInfo *info = new UnitInfo();
        info->unitType = dataList.at(0).asInt();
        info->level = dataList.at(1).asInt() % 100;
        info->rank = dataList.at(1).asInt() / 100;

        unitInfoListHeroInventory.push_back(info);
      }
    }
  }
  GM->saveHeroInventory(unitInfoListHeroInventory);

  std::string strInventory2 = UDGetStr(KEY_UNITS_HERO_INVENTORY);
  log("strInventory: %s", strInventory2.c_str());
  closePopup();
  closePopup();
}
void Title::onPostItemGemClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  ValueVector contentList = GM->split(btn->getName(), "-");
  selectedPostItem = btn->getName() + "";
  Button *btnOk =
      showGemRewardPopup(btn->getTag(), contentList.at(2).asString().c_str());
  btnOk->addClickEventListener(
      CC_CALLBACK_0(Title::onPostItemGemOkClick, this));
}
void Title::onPostItemGemOkClick() {
  std::string post = UDGetStr(KEY_POST_MESSAGE, "");
  log("post %s: ", post.c_str());
  post = post.erase(post.find(selectedPostItem), selectedPostItem.length());
  log("after post %s: ", post.c_str());
  UDSetStr(KEY_POST_MESSAGE, post.c_str());
  log("saved post %s: ", UDGetStr(KEY_POST_MESSAGE, "_").c_str());
  BSM->postBoxConent = post + "";
  closeGemRewardPopup();
}
void Title::onRewardNoticeClick() {
  ValueVector list = GM->split(GM->rewardMsg, "|");
  std::string content = "Thank you for playing Cartoon Craft";
  if (list.size() > 0) {
    content = list.at(1).asString();
  }

  Button *btn = showGemRewardPopup(GM->rewardGemCount, content.c_str());
  btn->addClickEventListener(CC_CALLBACK_0(Title::onRewardNoticeOkClick, this));
}
Button *Title::showGemRewardPopup(int gemCount, const char *msg) {
  Node *layer = CSLoader::createNode("GemRewardMessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("gemRewardMessageBox");
  setAsPopup(layer);
  layer->setTag(gemCount);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  ImageView *imgBack = (ImageView *)layer->getChildByName("imgBackground");
  GM->rollOpenScroll(imgBack);

  Button *btn = (Button *)layer->getChildByName("btnOk");
  Text *lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "ok");

  lbl = (Text *)layer->getChildByName("lblGemCount");
  lbl->setString(Value(gemCount).asString());

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedStringNotKey(lbl, msg);

  layer->getChildByName("imgGem")->setPosition(
      layer->getChildByName("imgGold")->getPosition());
  layer->getChildByName("lblGemCount")
      ->setPosition(layer->getChildByName("lblGold")->getPosition());
  ImageView *img = (ImageView *)layer->getChildByName("imgGold");
  img->setVisible(false);
  img = (ImageView *)layer->getChildByName("imgTree");
  img->setVisible(false);
  lbl = (Text *)layer->getChildByName("lblGold");
  lbl->setVisible(false);
  lbl = (Text *)layer->getChildByName("lblTree");
  lbl->setVisible(false);

  return btn;
}
void Title::onRewardNoticeOkClick() {
  GM->rewardedCode = GM->rewardCode; // test
  log("GM->rewardedCode %d", GM->rewardedCode);
  BSM->saveUserData(strmake("rewarded_index=%d", GM->rewardedCode));
  closeGemRewardPopup();
}
void Title::closeGemRewardPopup() {
  int gemGiven = 0;

  Node *layer = this->getChildByName("gemRewardMessageBox");
  Vec2 gemPos =
      layer->getPosition() + layer->getChildByName("imgGem")->getPosition();

  TopBar *ndTopBar = TopBar::create();
  ndTopBar->setName("ndTopBar");
  titleLayer->addChild(ndTopBar, 200 + (int)popupArray.size() + 1);
  ndTopBar->setPosition(titleLayer->getChildByName("btnNotice")->getPosition() +
                        Vec2(-98, 3));
  float yMove = 500;
  ndTopBar->setPositionY(ndTopBar->getPositionY() + yMove);
  ndTopBar->runAction(EaseInOut::create(MoveBy::create(1, Vec2(0, -yMove)), 3));

  Vec2 gemTargetPos =
      layer->getPosition() + ndTopBar->getPosition() +
      ndTopBar->ndTopBar->getChildByName("imgGemIcon")->getPosition() +
      Vec2(0, -yMove);
  float gemDelay = 0;
  float goldDelay = 0;
  float treeDelay = 0;
  int moveDistance = 160;
  int gemSptCount = 10;

  Vec2 goldPos =
      layer->getPosition() + layer->getChildByName("imgGold")->getPosition();
  Vec2 goldTargetPos =
      ndTopBar->getPosition() +
      ndTopBar->ndTopBar->getChildByName("imgGoldIcon")->getPosition() +
      Vec2(0, -yMove);
  int goldGiven = 0;
  int goldSptCount = 10;

  Vec2 treePos =
      layer->getPosition() + layer->getChildByName("imgTree")->getPosition();
  Vec2 treeTargetPos =
      ndTopBar->getPosition() +
      ndTopBar->ndTopBar->getChildByName("imgTreeIcon")->getPosition() +
      Vec2(0, -yMove);
  int treeGiven = 0;
  int treeSptCount = 10;

  Sprite *sptGem;
  Sprite *sptGold;
  Sprite *sptTree;

  gemRewardCount = layer->getTag();

  for (int i = 0; i < gemSptCount; i++) {
    sptGem = Sprite::create("gem.png");
    this->addChild(sptGem, 300);
    sptGem->setPosition(gemPos);
    sptGem->runAction(Sequence::create(
        DelayTime::create(gemDelay),
        EaseInOut::create(
            MoveBy::create(0.3f,
                           Vec2(rand() % moveDistance - moveDistance / 2,
                                rand() % moveDistance - moveDistance / 2)),
            2),
        DelayTime::create(0.5f),
        EaseInOut::create(MoveTo::create(0.6f, gemTargetPos), 2),
        CallFuncN::create(CC_CALLBACK_1(Title::gemMoveToTopBarDone, this)),
        NULL));
    gemDelay += 0.07f;
    sptGem->setTag(gemRewardCount / gemSptCount);
    gemGiven += sptGem->getTag();
  }
  sptGem->setTag(sptGem->getTag() + (gemRewardCount - gemGiven));

  if (goldRewardCount > 0) {
    for (int i = 0; i < goldSptCount; i++) {
      sptGold = Sprite::create("goldIcon.png");
      this->addChild(sptGold, 300);
      sptGold->setPosition(goldPos);
      sptGold->runAction(Sequence::create(
          DelayTime::create(goldDelay),
          EaseInOut::create(
              MoveBy::create(0.3f,
                             Vec2(rand() % moveDistance - moveDistance / 2,
                                  rand() % moveDistance - moveDistance / 2)),
              2),
          DelayTime::create(0.5f),
          EaseInOut::create(MoveTo::create(0.6f, goldTargetPos), 2),
          CallFuncN::create(CC_CALLBACK_1(Title::goldMoveToTopBarDone, this)),
          NULL));
      goldDelay += 0.07f;
      sptGold->setTag(goldRewardCount / goldSptCount);
      goldGiven += sptGold->getTag();
    }
    sptGold->setTag(sptGold->getTag() + (goldRewardCount - goldGiven));
  }
  if (treeRewardCount > 0) {
    for (int i = 0; i < treeSptCount; i++) {
      sptTree = Sprite::create("lumberIcon.png");
      this->addChild(sptTree, 300);
      sptTree->setPosition(treePos);
      sptTree->runAction(Sequence::create(
          DelayTime::create(treeDelay),
          EaseInOut::create(
              MoveBy::create(0.3f,
                             Vec2(rand() % moveDistance - moveDistance / 2,
                                  rand() % moveDistance - moveDistance / 2)),
              2),
          DelayTime::create(0.5f),
          EaseInOut::create(MoveTo::create(0.6f, treeTargetPos), 2),
          CallFuncN::create(CC_CALLBACK_1(Title::treeMoveToTopBarDone, this)),
          NULL));
      treeDelay += 0.07f;
      sptTree->setTag(treeRewardCount / treeSptCount);
      treeGiven += sptTree->getTag();
    }
    sptTree->setTag(sptTree->getTag() + (treeRewardCount - treeGiven));
  }

  closePopup();
  closePopup();
}
void Title::gemMoveToTopBarDone(Ref *ref) {
  Sprite *spt = (Sprite *)ref;
  GM->addGem(spt->getTag());
  gemRewardCount -= spt->getTag();
  if (gemRewardCount <= 0) {
    BSM->saveUserData(strmake("gem=%d&gold=%d&tree=%d", GM->getGem(),
                              GM->getCoin(), GM->getTree()));
    Node *ndTopBar = titleLayer->getChildByName("ndTopBar");
    float yMove = 500;
    ndTopBar->runAction(Sequence::create(
        EaseInOut::create(MoveBy::create(1, Vec2(0, yMove)), 3),
        CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, ndTopBar)),
        NULL));
  }
  spt->removeFromParent();
}
void Title::goldMoveToTopBarDone(Ref *ref) {
  Sprite *spt = (Sprite *)ref;
  GM->addCoin(spt->getTag());
  goldRewardCount -= spt->getTag();
  spt->removeFromParent();
}
void Title::treeMoveToTopBarDone(Ref *ref) {
  Sprite *spt = (Sprite *)ref;
  GM->addTree(spt->getTag());
  treeRewardCount -= spt->getTag();
  spt->removeFromParent();
}
void Title::settingLog(std::string str) {
  Node *layer = this->getChildByName("settingPopup");
  if (layer) {
    Text *lbl = (Text *)layer->getChildByName("lblLog");
    lbl->setString(str);
  }
}
void Title::setTitle(Node *titleBack, std::string menuKey) {
  Text *lbl = (Text *)titleBack->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, menuKey);
  ImageView *img = (ImageView *)titleBack;
  img->setContentSize(cocos2d::Size(
      lbl->getContentSize().width * lbl->getScaleX() + 100, 104.39f));
  img->setScale9Enabled(true);
  lbl->setPositionX(titleBack->getContentSize().width / 2);
}
void Title::showRegisterName() {
  if (!usesAccountSystem() && !GM->deferredAccountRegistrationActive) {
    return;
  }
  Node *layer = CSLoader::createNode("RegisterName.csb");
  this->addChild(layer, POPUP_ZORDER);
  setAsPopup(layer);
  layer->setName("namePopup");
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);
  layer->getChildByName("btnBlock")->setScale(2);
  Button *btn;
  Text *lbl;
  Node *background = layer->getChildByName("imgBackground");
  lbl = (Text *)background->getChildByName("lblTitle");

  btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  btn->setVisible(isRename);

  setTitle(layer->getChildByName("titleBack"), "name");
  lbl = (Text *)background->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "input name");

  lbl = (Text *)layer->getChildByName("lblAlert");
  LM->setLocalizedString(lbl, "change name alert");
  lbl = (Text *)background->getChildByName("lblName");
  LM->setLocalizedString(lbl, "name");
  lbl = (Text *)background->getChildByName("lblPassword");
  LM->setLocalizedString(lbl, "password");
  lbl = (Text *)background->getChildByName("lblConfirm");
  LM->setLocalizedString(lbl, "confirm password");

  TextField *tf = (TextField *)background->getChildByName("tfName");
  tf->setPlaceHolder(LM->getText("name"));
  tf = (TextField *)background->getChildByName("tfPassword");
  tf->setPlaceHolder(LM->getText("password"));
  tf = (TextField *)background->getChildByName("tfConfirm");
  tf->setPlaceHolder(LM->getText("password"));

  btn = (Button *)background->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onOkNameClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "ok");
  btn = (Button *)background->getChildByName("btnRandom");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onRandomNameClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "random");
  btn = (Button *)background->getChildByName("btnApple");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onSignInWithAppleID, this));
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  btn->setVisible(true);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  btn->setVisible(false);
#endif

  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "Apple ID");
}
void Title::onRandomNameClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  showIndicator();

  std::string strName = "";
  srand(time(0));
  for (int i = 0; i < 15; i++) {
    int index = rand() % 3;
    if (index == 0) {
      strName += (char)(rand() % 26 + (int)'A');
    } else if (index == 1) {
      strName += (char)(rand() % 26 + (int)'a');
    } else if (index == 2) {
      strName += (char)(rand() % 10 + (int)'0');
    }
  }
  //    log("std: %s", strName.c_str());
  requestRegisterName(strName);
}
void Title::onSignInWithAppleID(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC

  if (GM->playerGPSID.length() > 0) {
    showIndicator();
    requestRegisterName(GM->playerGPSID);
  } else {
    showInstanceMessage("Log-in Game Center, first.");
  }
}
void Title::onOkNameClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  TextField *tf = (TextField *)this->getChildByName("namePopup")
                      ->getChildByName("imgBackground")
                      ->getChildByName("tfName");
  //    TextField* tfPassword =
  //    (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfPassword");
  //    TextField* tfConfirm =
  //    (TextField*)this->getChildByName("namePopup")->getChildByName("imgBackground")->getChildByName("tfConfirm");
  if (tf->getString().size() <= 0) {
    showInstanceMessage(LM->getText("input name"));
  } else if (tf->getString().find(',') != std::string::npos ||
             tf->getString().find('_') != std::string::npos ||
             tf->getString().find('/') != std::string::npos) {
    showInstanceMessage(strmake(
        "%s ( _ , / )", LM->getText("do not use special character").c_str()));
    return;
  }
  //    else if(tfPassword->getString().compare(tfConfirm->getString()) != 0){
  //        showInstanceMessage(LM->getText("password not match"));
  //    }
  else {

    showIndicator();
    requestRegisterName(tf->getString());
  }
}
void Title::requestRegisterName(std::string strName) {
  if (!usesAccountSystem() && !GM->deferredAccountRegistrationActive) {
    hideIndicator();
    closePopup();
    return;
  }
  if (isRename) {
    renameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
    BSM->rename(strName);
  } else {
    nameToRegister = strName;
    nameHandleState = NETWORK_HANDLE_STATE_REQUESTED;
    isNameRequested = true;
    BSM->registerName(strName);
  }
}
void Title::showServerSelect(bool isNewUser) {
  Node *layer = CSLoader::createNode("ServerSelect.csb");
  this->addChild(layer, POPUP_ZORDER);
  setAsPopup(layer);
  layer->setName("serverSelect");
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  Button *btn;
  Text *lbl;
  //    Node* background = layer->getChildByName("imgBackground");
  lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "server select");

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedString(lbl, "server tip");

  btn = (Button *)layer->getChildByName("btn0");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onServerSelected, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "alfred");

  btn = (Button *)layer->getChildByName("btn1");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onServerSelected, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "bungee");

  btn = (Button *)layer->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "close");

  int server = UDGetInt(KEY_SERVER, 1);

  btn = (Button *)layer->getChildByName("btnMoveServer");
  btn->addClickEventListener(CC_CALLBACK_0(Title::onMoveServerClick, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "change server");
  btn->setVisible(server != 0);

  Node *imgCheck = layer->getChildByName("imgCheck");
  btn->setEnabled(server != selectedServer);
  if (server == 0) {
    imgCheck->setPosition(layer->getChildByName("btn0")->getPosition());
  } else if (server == 1) {
    imgCheck->setPosition(layer->getChildByName("btn1")->getPosition());
  } else {
    imgCheck->setPosition(layer->getChildByName("btn1")->getPosition());
  }
}
void Title::onMoveServerClick() {
  if (!usesAccountSystem()) {
    return;
  }
  if (GM->getGem() >= 1000 && selectedServer >= 0) {
    BSM->saveUserData(strmake("buildings=7/3/14/15&name=%s&playid=%s",
                              getRandomName().c_str(),
                              getRandomName().c_str()));
    GM->addGem(-1000);
    UDSetInt(KEY_SERVER, selectedServer);
    UDSetStr(KEY_PREVIOUS_ID, UDGetStr(KEY_SAVED_ID));
    UDSetStr(KEY_SAVED_ID, "-1");
    UDSetStr(KEY_NAME, "");
    UD->flush();
    BSM->setServerUrl();
    restartTheGame();
  } else {
    showInstanceMessage(LM->getText("not enough gems"));
  }
}
void Title::onServerSelected(Ref *ref) {
  BTN_FROM_REF
  Node *layer = this->getChildByName("serverSelect");

  int clickedIndex = Value(btn->getName().substr(3)).asInt();
  if (clickedIndex == 0) {
    showInstanceMessage(LM->getText("server is full"));
    return;
  }
  log("server selected %d", clickedIndex);
  Node *imgCheck = layer->getChildByName("imgCheck");
  imgCheck->setPosition(btn->getPosition());
  selectedServer = clickedIndex;
  int previousServer = UDGetInt(KEY_SERVER, 1);

  btn = (Button *)layer->getChildByName("btnMoveServer");
  btn->setVisible(previousServer != clickedIndex);
  btn->setEnabled(previousServer != selectedServer);
}
void Title::showUserSelect() {
  if (!usesAccountSystem()) {
    return;
  }
  Node *layer = CSLoader::createNode("UserSelect.csb");
  this->addChild(layer, POPUP_ZORDER);
  setAsPopup(layer);
  layer->setName("userSelect");
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  Button *btn;
  Text *lbl;
  Node *background = layer->getChildByName("imgBackground");
  lbl = (Text *)background->getChildByName("lblTitle");

  setTitle(layer->getChildByName("titleBack"), "new user");

  lbl = (Text *)background->getChildByName("lblDesc");
  LM->setLocalizedString(lbl, "is new user");

  btn = (Button *)background->getChildByName("btnNew");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onNewUserSelect, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "new user");
  btn = (Button *)background->getChildByName("btnExist");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onOldUserSelect, this));
  lbl = (Text *)btn->getChildByName("lbl");
  LM->setLocalizedString(lbl, "existing user");
}
void Title::onNewUserSelect(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  showRegisterName();
}
void Title::onOldUserSelect(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  showInputUserIDAndName();
}
void Title::showInputUserIDAndName() {
  if (!usesAccountSystem()) {
    return;
  }
  Node *layer = CSLoader::createNode("ExistNameAndID.csb");
  this->addChild(layer, POPUP_ZORDER);
  setAsPopup(layer);
  layer->setName("accountChange");
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  Button *btn;
  Text *lbl;
  Node *background = layer->getChildByName("imgBackground");
  lbl = (Text *)background->getChildByName("lblTitle");
  layer->getChildByName("btnClose")->setVisible(false);
  setTitle(layer->getChildByName("titleBack"), "existing user");

  lbl = (Text *)background->getChildByName("lblName");
  LM->setLocalizedString(lbl, "name");

  lbl = (Text *)background->getChildByName("lblPassword");
  LM->setLocalizedString(lbl, "password");

  TextField *tfName = (TextField *)layer->getChildByName("imgBackground")
                          ->getChildByName("tfName");
  tfName->setPlaceHolder(LM->getText("name"));
  TextField *tfPassword = (TextField *)layer->getChildByName("imgBackground")
                              ->getChildByName("tfPassword");
  tfPassword->setPlaceHolder(LM->getText("password"));

  btn = (Button *)background->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onCheckExistingUser, this));
}
void Title::onCheckExistingUser(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
  Node *layer = this->getChildByName("accountChange");
  TextField *tfName = (TextField *)layer->getChildByName("imgBackground")
                          ->getChildByName("tfName");
  TextField *tfPassword = (TextField *)layer->getChildByName("imgBackground")
                              ->getChildByName("tfPassword");
  BSM->changeAccount(tfName->getString(), tfPassword->getString());
}
void Title::restartTheGame() {
  clearAssets();
  GM->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  auto scene = Scene::create();
  Intro *intro = Intro::create();
  scene->addChild(intro);
  Director::getInstance()->replaceScene(scene);
}
void Title::showFreeAppAlert() {
  Node *layer = CSLoader::createNode("FreeAdsAlert.csb");
  this->addChild(layer, 4);
  layer->setName("freeAdsAlert");
  setAsPopup(layer);
  layer->setTag(0);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);

  Button *btnBlock = (Button *)layer->getChildByName("btnBlock");
  btnBlock->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));
  Button *btn = (Button *)layer->getChildByName("btnOk");
  btn->addClickEventListener(CC_CALLBACK_0(Title::closePopup, this));

  rollOpenScroll((ImageView *)layer->getChildByName("imgBackground"));

  Text *lbl = (Text *)layer->getChildByName("lblDesc");
  LM->setLocalizedString(lbl, "free app alert");

  lbl = (Text *)layer->getChildByName("lblAsk");
  lbl->setFontName(LM->getLocalizedFont());
  lbl->setString(
      strmake("%s: sachoi1103@gmail.com", LM->getText("ask email").c_str())
          .c_str());
}
void Title::onCreateUserFailed() { log("onCreateUserFailed"); }
void Title::onCreateUserCreated() {
  isUserInfoCreatedOrNotKnow = true;
  isWaitingForUserInfoCreated = false;
  isUserInfoReceived = false;
  isWaitingForUserInfo = false;
}
void Title::onUserInfoNotFound() {
  isUserInfoCreatedOrNotKnow = false;
  isWaitingForUserInfoCreated = false;
}
void Title::doLabelFadeInLater(PPLabel *lbl, float delay, float fadeInDur) {
  if (lbl->lblNormal != nullptr) {
    for (auto unit : lbl->getChildren()) {
      unit->setOpacity(0);
      unit->runAction(Sequence::create(DelayTime::create(delay),
                                       FadeIn::create(fadeInDur), NULL));
    }
  } else {
    for (auto unit : lbl->spriteBatch->getChildren()) {
      unit->setOpacity(0);
      unit->runAction(Sequence::create(DelayTime::create(delay),
                                       FadeIn::create(fadeInDur), NULL));
    }
  }
}

void Title::showColosseum() {
  if (!BSM->timeEstablished || BSM->startLocalTime == 0) {
    BSM->establishLocalTimeBaseline();
  }
  BSM->getHttpTime();
  showInterstitialRequested = true;
  Node *layer = CSLoader::createNode("Colosseum.csb");
  this->addChild(layer, 4);
  layer->setName("colosseum");
  setAsPopup(layer);
  layer->setTag(0);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);
  layer->setPositionY(size.height / 2 - layer->getContentSize().height / 2);

  Button *btnBlock = (Button *)layer->getChildByName("btnBlock");
  btnBlock->addClickEventListener(
      CC_CALLBACK_0(Title::closeColosseumPopup, this));
  btnBlock = (Button *)layer->getChildByName("btnClose");
  btnBlock->addClickEventListener(
      CC_CALLBACK_0(Title::closeColosseumPopup, this));

  rollOpenScroll((ImageView *)layer->getChildByName("imgBackground"));

  layer->getChildByName("lblRecommend")->setVisible(false);

  Text *lbl = (Text *)layer->getChildByName("lblLeague");
  lbl->setString(strmake(
      "%s #%d", GetCurrentLeagueText().c_str(),
      UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX)));

  lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "colosseum");

  lbl = (Text *)layer->getChildByName("lblReward");
  LM->setLocalizedString(lbl, "reward");

  lbl = (Text *)layer->getChildByName("lblFirstPlace");
  //    lbl->setString(strmake("x20 %s", LM->getText("next league").c_str()));

  lbl = (Text *)layer->getChildByName("lblYourID");
  if (usesAccountSystem()) {
    lbl->setString(
        strmake("Your ID: %s",
                UDGetStr(KEY_SAVED_ID)
                    .c_str())); // GameSharing::getPlayerID().c_str()));
  } else {
    lbl->setVisible(false);
  }

  //    int timeInSec = (int)(BSM->getCurrentTimeT());

  int timeLeft = BSM->getTimeLeftToNewDay();
  lbl = (Text *)layer->getChildByName("lblLeagueTimeLeft");
  lbl->setString(GM->getTimeLeftInString(timeLeft));

  lbl = (Text *)layer->getChildByName("lblTicketCount");
  int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
  lbl->setString(strmake("%d", ticketCount));

  ScrollView *scView = (ScrollView *)layer->getChildByName("scView");
  scView->getChildByName("imgTemplate")->setVisible(false);

  Button *btn = (Button *)layer->getChildByName("btnPlay");
  btn->addClickEventListener(CC_CALLBACK_1(Title::onPlayColosseum, this));
  lbl = (Text *)btn->getChildByName("lblPlay");
  LM->setLocalizedString(lbl, "play");

  btn->setPressedActionEnabled(true);
  layer->getChildByName("lblTimer")->setVisible(false);

  bool isFree = UDGetBool(KEY_GOLDEN_TICKET_FREE, true);
  int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
  if (freeTicketUsedDay != BSM->getDay()) {
    isFree = true;
  }

  lbl = (Text *)btn->getChildByName("lblTicketCost");
  if (isFree) {
    LM->setLocalizedString(lbl, "free");
  } else {
    lbl->setString("1");
  }

  btn = (Button *)layer->getChildByName("btnVideo");
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
void Title::onVideoForTicketClick() {
  log("on vido click");
  if (GameSharing::isVideoAvailable()) {
    log("on vido click");
    GM->showVideo(VIDEO_TICKET);
    //        std::string value = "";
    //        value += BSM->getStrFromTime(BSM->getCurrentTimeT());
    //        UDSetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, value);
    GM->saveTime(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME,
                 BSM->getCurrentTimeT());
  } else {
    log("on vido click");
    showInstanceMessage(LM->getText("video not available"));
  }
}
void Title::closeColosseumPopup() {
  colosseumRankInfoName.clear();
  colosseumRankInfoResult.clear();
  colosseumRankInfoTime.clear();
  colosseumRankInfoKill.clear();
  colosseumRankInfoWeapon.clear();
  closePopup();
  this->unschedule(schedule_selector(Title::updateColosseumPopup));
}
std::string Title::getPlayerID() {
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
std::string Title::getPlayerName() {
  std::string playerName = UDGetStr(KEY_PLAYER_NAME, "nameNotSet");

  if (playerName.compare("nameNotSet") == 0) {
    playerName = getRandomName();
  }

  std::string playerNameFromGameCenter = GameSharing::getPlayerName();
  if (playerNameFromGameCenter.length() > 0) {
    playerName = playerNameFromGameCenter;
  }
  return playerName;
}
void Title::onGetTimeFailed() {
  PPLabel *lbl =
      PPLabel::create(LM->getText("server fail try again"), 60, Color3B::WHITE,
                      true, false, TextHAlignment::CENTER, true);
  this->addChild(lbl, 200);
  lbl->setPosition(Point(size.width / 2, -TILE_SIZE / 2));
  float dur = 0.3f;
  float distanceToMove = 360;
  lbl->runAction(Sequence::create(
      MoveBy::create(dur, Point(0, distanceToMove)), DelayTime::create(2),
      MoveBy::create(dur, Point(0, -distanceToMove)),
      CallFunc::create(CC_CALLBACK_0(PPLabel::removeFromParent, lbl)),
      nullptr));

  hideIndicator();
  closePopup();
}
void Title::updateColosseumPopup(float dt) {
  if (isTitleEnd) {
    return;
  }
  if (!getChildByName("colosseum")) {
    this->unschedule(schedule_selector(Title::updateColosseumPopup));
    return;
  }
  if (isTimeGetFailed) {
    isTimeGetFailed = false;
    onGetTimeFailed();
  }
  if (isGetRankFailed) {
    isGetRankFailed = false;
    onGetTimeFailed();
  }
  if (BSM->timeEstablished && !isWaitingForRankInfo &&
      !isRankInfoReceviedSuccessfully) {
    isGetRankFailed = false;
    isWaitingForRankInfo = true;
    isRankInfoReceviedSuccessfully = false;
    int leagueIndex =
        UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);
    //        BSM->getColosseumRank(UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0),
    //        leagueIndex);
    BSM->getArenaRank();
  }
  if (isColosseumInfoRefreshRequired) {
    isColosseumInfoRefreshRequired = false;
    ((Text *)getChildByName("colosseum")->getChildByName("lblTicketCount"))
        ->setString(strmake("%d", UDGetInt(KEY_GOLDEN_TICKET, 1)));
    //        getChildByName("colosseum")->getChildByName("lblTimer")->setVisible(false);
  }
  if (isArenaRecordRecieved) {
    isArenaRecordRecieved = false;
    ValueVector list = GM->split(arenaRecord, "_");
    if (arenaRecord.size() > 0 && arenaRecord.compare("0") != 0 &&
        list.size() == 2 && list[1].asInt() > 0) {
      showLeagueResult();
    }
    list.clear();
  }

  colosseumUITimer += dt;
  if (colosseumUITimer >= 1) {
    colosseumUITimer -= 1;
  }
  if (isLeagueNotFound) {
    isLeagueNotFound = false;
    hideIndicator();

  } else if (isColosseumRankInfoReceived) {
    log("rank info recived");

    isRankInfoReceviedSuccessfully = true;
    hideIndicator();
    BSM->getArenaReward();
    isColosseumRankInfoReceived = false;
    Node *layer = this->getChildByName("colosseum");
    if (layer->getTag() > 0) {
      return;
    }
    layer->setTag(1);

    ScrollView *scView = (ScrollView *)layer->getChildByName("scView");
    ImageView *imgTemp = (ImageView *)scView->getChildByName("imgTemplate");
    float startY = 779.64;
    int gap = 142;
    float y = startY;

    ValueVector names = GM->split(strRankInfo, ",");
    if (names.size() == 1) {
      Text *lbl =
          (Text *)getChildByName("colosseum")->getChildByName("lblRecommend");
      lbl->setVisible(true);
      LM->setLocalizedString(lbl, "challenge arena");
    }
    for (int i = 0; i < names.size() - 1; i++) {
      //        for(int i = 0; i < colosseumRankInfoResult.size(); i++){
      ImageView *img = (ImageView *)imgTemp->clone();
      Text *lbl = (Text *)img->getChildByName("lblName");
      //            lbl->setFontName("Helvetica");
      //            log("%d. name: %s", i + 1,
      //            colosseumRankInfoName.at(i).c_str());
      //            lbl->setString(strmake("%d. %s", i + 1,
      //            colosseumRankInfoName.at(i).c_str()));
      ValueVector info = GM->split(names.at(i).asString(), "_");
      lbl->setString(strmake("%d. %s", i + 1, info.at(0).asString().c_str()));

      //            if(colosseumRankInfoName.at(i).compare(GameSharing::getPlayerName())
      //            == 0){
      //                img->setColor(Color3B(80, 190, 80));
      //            }else{
      //                img->setColor(Color3B::WHITE);
      //            }
      lbl = (Text *)img->getChildByName("lblRecord");
      if (info.size() > 1) {
        lbl->setString(GM->getTimeLeftInString(info.at(1).asInt()));
      } else {
      }
      //            if(colosseumRankInfoResult.at(i) == 0){
      //                lbl->setString(Value(colosseumRankInfoKill.at(i)).asString());
      //            }else{
      //                lbl->setString(GM->getTimeLeftInString(colosseumRankInfoTime.at(i)/100));
      //            }
      scView->addChild(img);
      startY = 610.0f;
      gap = 142;
      y = startY - gap * i;
      if (gap * names.size() > startY) {
        img->setPosition(
            Point(imgTemp->getPositionX(), gap * names.size() - gap * i));
      } else {
        img->setPosition(Point(imgTemp->getPositionX(), y));
      }
      img->setVisible(true);
      info.clear();
    }
    scView->setInnerContainerSize(
        cocos2d::Size(scView->getContentSize().width, names.size() * gap));

    int myScore = UDGetInt(KEY_COLOSSEUM_SCORE, 0);
    if (colosseumMe < 0) {
      UDSetInt(KEY_COLOSSEUM_SCORE, 0);
      myScore = 0;
    }
    ImageView *imgMe =
        (ImageView *)(ImageView *)layer->getChildByName("imgYou");
    Text *lbl = (Text *)imgMe->getChildByName("lblName");
    lbl->setString("sdf)");
    if (colosseumTotal == 0) {
      colosseumTotal = 1;
    }
    if (usesAccountSystem()) {
      LM->setLocalizedString(lbl, strmake("%d. %s (%s%d%%)", colosseumMe + 1,
                                          UDGetStr(KEY_NAME).c_str(),
                                          LM->getText("top").c_str(),
                                          colosseumMe * 100 / colosseumTotal));
    } else {
      LM->setLocalizedString(lbl, strmake("%d. (%s%d%%)", colosseumMe + 1,
                                          LM->getText("top").c_str(),
                                          colosseumMe * 100 / colosseumTotal));
    }
    lbl = (Text *)imgMe->getChildByName("lblRecord");
    lbl->setString(GM->getTimeLeftInString(myScore));
    if (myScore == 0) {
      lbl = (Text *)imgMe->getChildByName("lblName");
      lbl->setString(LM->getText("play colosseum"));
      lbl->setContentSize(cocos2d::Size(400, 79));
      imgMe->getChildByName("lblRecord")->setVisible(false);
    }
    names.clear();
  }
  if (getChildByName("colosseum")) {
    //        int timeInSec = (int)(BSM->getCurrentTime());
    //        int timeLeft = 60*60*24 - timeInSec%(60*60*24);
    int timeLeft = BSM->getTimeLeftToNewDay();
    Node *layer = getChildByName("colosseum");
    Text *lbl = (Text *)layer->getChildByName("lblLeagueTimeLeft");
    lbl->setString(GM->getTimeLeftInString(timeLeft));

    //        std::string strLastPlayTime =
    //        UDGetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, "");

    time_t lastPlayTimeT = GM->getSavedTime(
        KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME); // BSM->getTimeTFromStr(strLastPlayTime);
    time_t now = BSM->getCurrentTimeT();
    if (difftime(now, lastPlayTimeT) < 0) {
      //            std::string value = "";
      //            value += BSM->getStrFromTime(now);
      //            UDSetStr(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, value);
      GM->saveTime(KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME, now);
      lastPlayTimeT = now;
    }
    int cooltime = 60 * 60;
    timeLeft =
        cooltime -
        (difftime(now, lastPlayTimeT)); // 60*60*24 - timeInSec%(60*60*24);
    lbl = (Text *)layer->getChildByName("lblTimerForVideo");
    Button *btn = (Button *)layer->getChildByName("btnVideo");
    if (timeLeft >= 0) {
      lbl->setVisible(true);
      lbl->setString(GM->getTimeLeftInString(timeLeft));
      btn->setEnabled(false);
      btn->setOpacity(100);
    } else {
      lbl->setVisible(false);
      btn->setEnabled(true);
      btn->setOpacity(255);
    }
    if (GM->market == MARKET_SMARTPASS) {
      lbl->setVisible(false);
      btn->setVisible(false);
    }
  }
}
void Title::showTicketOffer() {
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Node *layer = CSLoader::createNode("TicketOffer.csb");
  this->addChild(layer, 5);
  layer->setName("ticket offer");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Text *lbl = (Text *)layer->getChildByName("lblTitle");
  lbl->setString(LM->getText("not enough ticket"));

  Button *btn = (Button *)layer->getChildByName("btnBuy");
  btn->addClickEventListener(
      CC_CALLBACK_1(Title::onTicketOfferButtonClick, this));
  btn->setTag(0);
  btn->setPressedActionEnabled(true);

  LM->setLocalizedString((Text *)btn->getChildByName("lbl"), "buy");

  btn = (Button *)layer->getChildByName("btnClose");
  btn->addClickEventListener(
      CC_CALLBACK_1(Title::onTicketOfferButtonClick, this));
  btn->setTag(1);
  btn->setPressedActionEnabled(true);
  LM->setLocalizedString((Text *)btn->getChildByName("lbl"), "cancel");
}
void Title::onTicketOfferButtonClick(Ref *ref) {
  BTN_FROM_REF_AND_DISABLE
  if (btn->getTag() == 0) { // buy
                            //        showIndicator();
    // iap code should be here
    GameSharing::buyItem(IAP_DETAIL_TICKET_1);

    //        showInstanceMessage(LM->getText("coming soon"));
  } else if (btn->getTag() == 1) { // cancel
    closePopup();
  }
}
void Title::onPlayColosseum(Ref *ref) {

  int freeTicketUsedDay = UDGetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, 0);
  if (freeTicketUsedDay != BSM->getDay()) {
    UDSetInt(KEY_GOLDEN_TICKET_FREE_USED_DAY, BSM->getDay());
  } else {
    int ticketCount = UDGetInt(KEY_GOLDEN_TICKET, 1);
    //        ticketCount = 1; // test
    if (ticketCount < 1) {
      showTicketOffer();
      return;
    }
    ticketCount--;
    UDSetInt(KEY_GOLDEN_TICKET, ticketCount);
  }
  BTN_FROM_REF_AND_DISABLE
  UDSetBool(KEY_GOLDEN_TICKET_FREE, false);
  //    UDSetInt(KEY_LAST_TICKET_USED_TIME, BSM->getCurrentTime());
  //    BSM->forceResultLeague();
  //    return; // test
  int day = (int)(BSM->getCurrentTimeT() / (60 * 60 * 24)) + 1;
  int totalStageCount = 8;
  int stageIndex = day % totalStageCount;
  if (day < 0) {
    stageIndex = rand() % totalStageCount;
  }

  this->unschedule(schedule_selector(Title::titleUpdate));
  this->unschedule(schedule_selector(Title::updateColosseumPopup));
  isTitleEnd = true;

  clearAssets();
  GameManager::getInstance()->titleLayer = nullptr;
  cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(
      listener);
  GM->nextScene = STAGE_FIELD;
  GM->isColosseum = true;
  auto scene = GameScene::scene(stageIndex, DIFFICULTY_MODE_NORMAL);
  Director::getInstance()->replaceScene(
      TransitionFade::create(2, scene, Color3B::BLACK));
}
std::string Title::GetCurrentLeagueText() {
  int rank = UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0);
  log("rank league: %d", rank);
  return LM->getText(GetLeagueText(rank).c_str());
}
std::string Title::GetLeagueText(int rank) {
  if (rank == 0) {
    return "BEGINNER";
  } else if (rank == 1) {
    return "BRONZE";
  } else if (rank == 2) {
    return "SILVER";
  } else if (rank == 3) {
    return "GOLD";
  } else if (rank == 4) {
    return "PLATINUM";
  } else if (rank == 5) {
    return "DIAMOND";
  } else if (rank == 6) {
    return "MASTER";
  } else if (rank == 7) {
    return "GRAND MASTER";
  } else if (rank == 8) {
    return "LEGENDARY";
  }
  return "GUESS WHO";
}
void Title::rollOpenScroll(ImageView *imgBack) {
  float originalWidth = imgBack->getContentSize().width;
  imgBack->setContentSize(cocos2d::Size(308, imgBack->getContentSize().height));
  imgBack->runAction(EaseInOut::create(
      ResizeTo::create(
          0.3, cocos2d::Size(originalWidth, imgBack->getContentSize().height)),
      2));

  for (auto child : imgBack->getParent()->getChildren()) {
    if (child == imgBack || child->getName().compare("btnBlock") == 0)
      continue;
    child->setOpacity(0);
    child->runAction(
        Sequence::create(DelayTime::create(0.2), FadeIn::create(0.3), NULL));
  }
}
void Title::showGlowBack(Node *node) {
  int count = 5;
  for (int i = 0; i < count; i++) {
    Sprite *spt = Sprite::create("longLight.png");
    node->addChild(spt, -1);
    spt->setRotation(360 * i / count);
    spt->runAction(RepeatForever::create(RotateBy::create(1, 10)));
    spt->setPosition(node->getContentSize() / 2);
    spt->setScale(1.5);
  }
  if (node->getChildByName("btnBlock")) {
    node->getChildByName("btnBlock")->setLocalZOrder(-100);
  }
}
void Title::showLeagueResult() {
  GM->playSoundEffect(SOUND_PAPER_FLIP);
  Node *layer = CSLoader::createNode("ColosseumReward.csb");
  this->addChild(layer);
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  UDSetBool(KEY_GOLDEN_TICKET_FREE, true);
  UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, DEFAULT_LEAGUE_INDEX);

  rollOpenScroll((ImageView *)layer->getChildByName("imgBackground"));

  showGlowBack(layer);

  Button *btn = (Button *)layer->getChildByName("btnOk");
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::onCloseColosseumResult, this));

  Text *lbl = (Text *)layer->getChildByName("lblTitle");
  LM->setLocalizedString(lbl, "colosseum result");

  ValueVector data = GM->split(arenaRecord, "_");
  int rank = data[0].asInt();
  int total = data[1].asInt();
  int percent = rank * 100.0f / total;

  lbl = (Text *)layer->getChildByName("lblDescription");
  int ticketCount = 0;
  int gemCount = 0;
  if (percent < 1) {
    LM->setLocalizedString(lbl, "colosseum top 1");
    ticketCount = 20;
    gemCount = 50;
  } else if (percent < 10) {
    LM->setLocalizedString(lbl, "colosseum top 10");
    ticketCount = 12;
    gemCount = 30;
  } else if (percent < 30) {
    LM->setLocalizedString(lbl, "colosseum top 30");
    ticketCount = 6;
    gemCount = 20;
  } else {
    LM->setLocalizedString(lbl, "colosseum top 100");
    ticketCount = 1;
    gemCount = 10;
  }

  lbl = (Text *)layer->getChildByName("lblRank");
  lbl->setString(
      strmake("%s %d/%d", LM->getText("rank").c_str(), rank + 1, total));
  lbl = (Text *)layer->getChildByName("lblPercent");
  lbl->setString(strmake("%s %d%%", LM->getText("top").c_str(), percent + 1));

  lbl = (Text *)layer->getChildByName("lblTicketCount");
  lbl->setString(Value(ticketCount).asString());
  int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
  ticket += ticketCount;
  UDSetInt(KEY_GOLDEN_TICKET, ticket);

  lbl = (Text *)this->getChildByName("colosseum")
            ->getChildByName("lblTicketCount");
  lbl->setString(strmake("%d", ticket));

  lbl = (Text *)layer->getChildByName("lblGemCount");
  lbl->setString(Value(gemCount).asString());
  GM->addGem(gemCount);
  UDSetInt(KEY_COLOSSEUM_SCORE, 0);
  int clearCount = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
  BSM->saveUserData(
      "gem=" + Value(GM->getGem()).asString() +
      "&arena_reward=0&golden_ticket=" + Value(ticket).asString() +
      "&arena_clr_cnt=" + Value(clearCount).asString());
  UDSetStr(KEY_ARENA_RID, "");
  data.clear();
}

void Title::onCloseColosseumResult() {
  closePopup();
  closePopup();
  showColosseum();
}
std::string Title::getRandomWord() {
  int index = rand() % 50;
  if (index == 0)
    return "Cat";
  if (index == 1)
    return "Kitten";
  if (index == 2)
    return "Kitty";
  if (index == 3)
    return "Doggy";
  if (index == 4)
    return "Puppy";
  if (index == 5)
    return "Darling";
  if (index == 6)
    return "Sweety";
  if (index == 7)
    return "Goblin";
  if (index == 8)
    return "Ogre";
  if (index == 9)
    return "Orc";
  if (index == 10)
    return "Oak";
  if (index == 11)
    return "Worrior";
  if (index == 12)
    return "Max";
  if (index == 13)
    return "Archer";
  if (index == 14)
    return "Brave";
  if (index == 15)
    return "Awesome";
  if (index == 16)
    return "Mighty";
  if (index == 17)
    return "Red";
  if (index == 18)
    return "Purple";
  if (index == 19)
    return "Super";
  if (index == 20)
    return "Yellow";
  if (index == 21)
    return "Blue";
  if (index == 22)
    return "Sky";
  if (index == 23)
    return "Orange";
  if (index == 24)
    return "Green";
  if (index == 25)
    return "Tree";
  if (index == 26)
    return "Moon";
  if (index == 27)
    return "Indigo";
  if (index == 28)
    return "Board";
  if (index == 29)
    return "Gamer";
  if (index == 30)
    return "Maker";
  if (index == 31)
    return "Breaker";
  if (index == 32)
    return "Rule";
  if (index == 33)
    return "House";
  if (index == 34)
    return "Home";
  if (index == 35)
    return "Beer";
  if (index == 36)
    return "Lover";
  if (index == 37)
    return "Crazy";
  if (index == 38)
    return "General";
  if (index == 39)
    return "Star";
  if (index == 40)
    return "Pet";
  if (index == 41)
    return "Hero";
  if (index == 42)
    return "RPG";
  if (index == 43)
    return "Slow";
  if (index == 44)
    return "Fast";
  if (index == 45)
    return "Crown";
  if (index == 46)
    return "Sunny";
  if (index == 47)
    return "Breeze";
  if (index == 48)
    return "Summer";
  if (index == 49)
    return "Winter";
  return "";
}
std::string Title::getRandomName() {
  return strmake("%s %s %s %s", getRandomWord().c_str(),
                 getRandomWord().c_str(), getRandomWord().c_str(),
                 getRandomWord().c_str());
}
std::string Title::getRandomID() {
  return strmake("%s %s %s %s %d", getRandomWord().c_str(),
                 getRandomWord().c_str(), getRandomWord().c_str(),
                 getRandomWord().c_str(), rand() % 100000);
}
void Title::clearAssets() {
  popupArray.clear();
  colosseumRankInfoName.clear();
  colosseumRankInfoResult.clear();
  colosseumRankInfoTime.clear();
  colosseumRankInfoKill.clear();
  colosseumRankInfoWeapon.clear();
}
void Title::onVideoDone() {
  if (GM->videoIndex == VIDEO_TICKET) {
    int ticket = UDGetInt(KEY_GOLDEN_TICKET, 1);
    ticket += 1;
    UDSetInt(KEY_GOLDEN_TICKET, ticket);
    isColosseumInfoRefreshRequired = true;
    isVideoDone = true;
  }
}

void Title::checkPlayerIDExist() {
  if (!usesAccountSystem()) {
    return;
  }
  isUserIDExistCheckRequested = true;

  BSM->checkPlayID(GM->playerGPSID);
}
void Title::showSmartPassError(std::string msg) {
  Node *layer = CSLoader::createNode("MessageBox.csb");
  this->addChild(layer, 4);
  layer->setName("messageBox");
  setAsPopup(layer);
  layer->setPositionX(size.width / 2 - layer->getContentSize().width / 2);

  Button *btn = (Button *)layer->getChildByName("btnBlock");
  btn = (Button *)layer->getChildByName("btnNo");
  Text *lbl = (Text *)btn->getChildByName("lbl");
  lbl->setString("Clear Cache");
  lbl->setFontSize(30);
  btn->addClickEventListener(
      CC_CALLBACK_0(Title::clearCacheForSmartPass, this));
  log("smart pass clear cache");

  //    Vec2 posLeft = btn->getPosition();
  Button *btnYES = (Button *)layer->getChildByName("btnYes");
  btnYES->addClickEventListener(CC_CALLBACK_0(Title::onStartPassErrorOk, this));
  //    btn->setPosition(btnYES->getPosition());
  //    btnYES->setPosition(posLeft);
  lbl = (Text *)btnYES->getChildByName("lbl");
  LM->setLocalizedStringNotKey(lbl, "Exit");

  lbl = (Text *)layer->getChildByName("lblDescription");
  LM->setLocalizedStringNotKey(lbl, msg);
}
void Title::clearCacheForSmartPass() { GameSharing::clearCacheForSmartPass(); }
void Title::onStartPassErrorOk() { GM->exitGame(); }

