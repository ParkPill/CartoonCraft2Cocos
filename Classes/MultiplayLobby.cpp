//
//  MultiplayLobby.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 06/12/2019.
//

#include "MultiplayLobby.h"

#include "Title.h"
#include "HeroPage.h"
#include "TopBar.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
//#include "ShopLayer.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
#include "MultiplayManager.h"
bool MultiplayLobby::init()
{
    resetEnemyInfo();
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    MM->receivedMsgCode = -1;
    MM->enemyRace = -1;
    MM->enemyStateChanged = false;
    Node* layer = CSLoader::createNode("MultiplayReady.csb");
    this->addChild(layer, 4);
    layer->setName("multiplayReady");

    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
    ImageView* img = (ImageView*)layer->getChildByName("imgBackMap");
    img->loadTexture("worldMap.png");
    img->setContentSize(size);
    layer->getChildByName("imgHeroBack")->removeFromParent();
    layer->getChildByName("imgEnemyBack")->removeFromParent();
    
    Button* btn = (Button*)layer->getChildByName("btnClose");
    btn->addClickEventListener(CC_CALLBACK_0(MultiplayLobby::onCloseRoom, this));
//    for(auto child : layer->getChildren()){
//        log("child name: %s", child->getName().c_str());
//    }
//    tfRoomName = (TextField*)layer->getChildByName("TextField_1");
    
//    Text* lbl =(Text*)layer->getChildByName("lblRoomName");
//    LM->setLocalizedString(lbl, "room name");
    
    btn = (Button*)layer->getChildByName("btnStart");
//    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onEnterRoomClick, this));
    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onStartClick, this));
    
    Text* lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "quick match");
    lbl = (Text*)layer->getChildByName("lblRoomName");
    LM->setLocalizedString(lbl, "multiplay");
    Label* lblCountDown = LM->getLocalizedLabel("3", Color4B::WHITE);
    lblCountDown->enableOutline(Color4B::BLACK, 3);
    layer->addChild(lblCountDown);
    lblCountDown->setName("lblCountDown");
    lblCountDown->setPosition(btn->getPosition());
    lblCountDown->setVisible(false);
    lbl = (Text*)layer->getChildByName("lblStatus");
    lbl->setString("");
    
    btn = (Button*)layer->getChildByName("btnHumanHero");
    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onSelectedRace, this));
    btn = (Button*)layer->getChildByName("btnOrcHero");
    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onSelectedRace, this));
    
    for (int i = 0; i < 4; i++) {
        btn =(Button*)layer->getChildByName(strmake("btnEmoticon%d", i));
        btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onSelectedEmoticon, this));
        btn->removeAllChildren();
        spine::SkeletonAnimation* spChar = spine::SkeletonAnimation::createWithJsonFile("emoticons.json", "emoticons.atlas", 1);
        btn->addChild(spChar);
        spChar->setPosition(btn->getContentSize()/2);
        if (i == 0) {
            spChar->setAnimation(0, "hi", false);
        }else if (i == 1) {
            spChar->setAnimation(0, "scared", false);
        }else if (i == 2) {
            spChar->setAnimation(0, "angry", false);
        }else if (i == 3) {
            spChar->setAnimation(0, "sad", false);
        }
    }
    InitUI();
    
//    spine::SkeletonAnimation* spChar;
    
    update(0);
    this->schedule(schedule_selector(HeroPage::update));
    setHeroBackground(0);
    setEnemyBackground(0);
    // init done
    return true;
}
void MultiplayLobby::InitUI(){
    Node* layer = this->getChildByName("multiplayReady");
    bool isMatched = false;
    std::string strRoomName = "room name";
    Text* lblTitle = (Text*)layer->getChildByName("lblRoomName");
    lblTitle->setString(isMatched?strRoomName:LanguageManager::getInstance()->getText("multiplay"));
    
    float scale = 1.5f;
    ImageView* img = (ImageView*)layer->getChildByName("btnHumanEnemy")->getChildByName("img");
    img->loadTexture("humanTroops.png");
    img->setContentSize(Size(220*scale, 161*scale));
    Button* btn = (Button*)layer->getChildByName("btnHumanHero");
    img = (ImageView*)btn->getChildByName("img");
    img->loadTexture("humanTroops.png");
    img->setContentSize(Size(220*scale, 161*scale));
    btn = (Button*)layer->getChildByName("btnOrcEnemy");
    img = (ImageView*)btn->getChildByName("img");
    img->loadTexture("orcTroops.png");
    img->setContentSize(Size(220*scale, 161*scale));
    btn = (Button*)layer->getChildByName("btnOrcHero");
    img = (ImageView*)btn->getChildByName("img");
    img->loadTexture("orcTroops.png");
    img->setContentSize(Size(220*scale, 161*scale));
    
    layer->getChildByName("lblYou")->setVisible(false);
    layer->getChildByName("lblRecordYou")->setVisible(false);
    
    int trophy = UDGetInt(KEY_MULTIPLAY_TROPHY, 1000);
    Text* lbl = (Text*)layer->getChildByName("lblMe");
    lbl->setString(UDGetStr(KEY_NAME));
    lbl = (Text*)layer->getChildByName("lblRecordMe");
    lbl->setString(strmake("%d",trophy));
    
//    for (int i = 0; i < 4; i++) {
//        img = (ImageView*)layer->getChildByName(strmake("btnEmoticon%d", i))->getChildByName("img");
//        img->loadTexture(strmake("emoticon%d.png", i));
//    }
    
    img = (ImageView*)layer->getChildByName("imgTalkEnemy");
    img->loadTexture("multiplayTalkBox.png");
    img->setContentSize(Size(172, 87));
    img->setPosition(img->getPosition() + Vec2(230, -20));
    img->setScaleX(-1);
    img->setOpacity(0);
    img = (ImageView*)layer->getChildByName("imgTalkHero");
    img->loadTexture("multiplayTalkBox.png");
    img->setPosition(img->getPosition() + Vec2(-30, -20));
    img->setContentSize(Size(172, 87));
    img->setOpacity(0);
    
    img = (ImageView*)layer->getChildByName("imgHero");
    Sprite* spt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
    img->getParent()->addChild(spt);
    spt->setPosition(img->getPosition());
    spt->setScale(2);
    spt->setName("imgHero");
    img->removeFromParent();
    
    img = (ImageView*)layer->getChildByName("imgEnemy");
    spt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
    layer->addChild(spt);
    spt->setPosition(img->getPosition());
    spt->setScale(-2, 2);
    spt->setName("imgEnemy");
    img->removeFromParent();
    spt->setFlippedX(true);
    
    updateEnemyInfo();
    btn = (Button*)layer->getChildByName("btnStart");
    btn->setEnabled(true);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "quick match");
}
void MultiplayLobby::updateUI(){
    Node* layer = this->getChildByName("multiplayReady");
    bool isMatched = false;
//    std::string strRoomName = "room name";
//    Text* lblTitle = (Text*)layer->getChildByName("lblRoomName");
//    lblTitle->setString(isMatched?strRoomName:LanguageManager::getInstance()->getText("press match"));
    
    layer->getChildByName("btnHumanEnemy")->setVisible(isMatched);
    layer->getChildByName("btnOrcEnemy")->setVisible(isMatched);
    layer->getChildByName("imgSelectedEnemy")->setVisible(isMatched);
    layer->getChildByName("imgEnemy")->setVisible(isMatched);
}
void MultiplayLobby::onSelectedEmoticon(Ref* ref){
    BTN_FROM_REF
    int index = Value(btn->getName().substr(11)).asInt();
    log("index: %d", index);
    MM->sendMsg(MSG_CODE_MSG, Value(index).asString());
    showEmoticon(index, true);
}
void MultiplayLobby::showEmoticon(int index, bool isHero){
    Node* layer = this->getChildByName("multiplayReady");
    float dur = 3;
    ImageView* img = nullptr;
    log("index: %d, isHero: %d", index, (int)isHero);
    float scaleX = 1;
    if (isHero) {
        img = (ImageView*)layer->getChildByName("imgTalkHero");
    }else{
        img = (ImageView*)layer->getChildByName("imgTalkEnemy");
        scaleX = -1;
    }
    std::string strName = "emoticon";
//    Sprite* spt = (Sprite*)img->getChildByName(strName);
//    if(spt != nullptr){
//        spt->removeFromParent();
//    }
//    spt = Sprite::create(strmake("emoticon%d.png", index));
//    spt->setName(strName);
//    img->addChild(spt);
//    spt->setPosition(Vec2(80, 42));
    img->stopAllActions();
    img->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), NULL));
//    spt->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), SPT_REMOVE_FUNC, NULL));
    
    spine::SkeletonAnimation* spChar = (spine::SkeletonAnimation*)img->getChildByName(strName);
    if(spChar != nullptr){
        spChar->removeFromParent();
    }
    spChar = spine::SkeletonAnimation::createWithJsonFile("emoticons.json", "emoticons.atlas", 1);
    spChar->setName(strName);
    img->addChild(spChar);
    img->setScale(1.5f*scaleX, 1.5f);
    spChar->setPosition(Vec2(80, 42));
    if (index == 0) {
        spChar->setAnimation(0, "hi", false);
    }else if (index == 1) {
        spChar->setAnimation(0, "scared", false);
    }else if (index == 2) {
        spChar->setAnimation(0, "angry", false);
    }else if (index == 3) {
        spChar->setAnimation(0, "sad", false);
    }
    spChar->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(dur), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(spine::SkeletonAnimation::removeFromParent, spChar)), NULL));
//    spt->runAction(Sequence::create(ScaleTo::create(0.3f, 0.9f), ScaleTo::create(0.3f, 1.0f),
//                                    ScaleTo::create(0.3f, 0.9f), ScaleTo::create(0.3f, 1.0f),
//                                    ScaleTo::create(0.3f, 0.9f), ScaleTo::create(0.3f, 1.0f),
//                                    ScaleTo::create(0.3f, 0.9f), ScaleTo::create(0.3f, 1.0f),
//                                    ScaleTo::create(0.3f, 0.9f), ScaleTo::create(0.3f, 1.0f),
//                                    NULL));
    
}
void MultiplayLobby::updateEnemyRace(int race){
    Node* layer = this->getChildByName("multiplayReady");
    log("update enemy race: %d", race);
    Sprite* newSpt;
    if (race == 0) {
        layer->getChildByName("imgSelectedEnemy")->setPosition(layer->getChildByName("btnHumanEnemy")->getPosition());
        MultiplayManager::getInstance()->enemyRace = 0;
        newSpt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
    }else{
        layer->getChildByName("imgSelectedEnemy")->setPosition(layer->getChildByName("btnOrcEnemy")->getPosition());
        MultiplayManager::getInstance()->enemyRace = 1;
        newSpt = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
    }
    
    Sprite* spt = (Sprite*)layer->getChildByName("imgEnemy");
    newSpt->setPosition(spt->getPosition());
    newSpt->setScale(-2, 2);
    newSpt->setName("imgEnemy");
    layer->addChild(newSpt);
    spt->removeFromParent();
}
void MultiplayLobby::onSelectedRace(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("multiplayReady");
    Sprite* newSpt;
    if (btn->getName().compare("btnHumanHero") == 0) {
        MM->heroRace = 0;
        newSpt = Sprite::createWithSpriteFrameName("swordmanStand0.png");
    }else{
        MM->heroRace = 1;
        newSpt = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
    }
    layer->getChildByName("imgSelectedHero")->setPosition(btn->getPosition());
    
    Sprite* spt = (Sprite*)layer->getChildByName("imgHero");
    newSpt->setPosition(spt->getPosition());
    newSpt->setScale(2);
    newSpt->setName("imgHero");
    layer->addChild(newSpt);
    spt->removeFromParent();
    if (MM->enemyRace >= 0) {
        MM->sendMsg(MSG_CODE_MSG, strmake("race%d", MM->heroRace));
    }
}
void MultiplayLobby::onStartClick(Ref* ref){
    BTN_FROM_REF_AND_DISABLE
    log("start");
    if (!MultiplayManager::getInstance()->isConnected) {
        MultiplayManager::getInstance()->connect();
        log("try connect");
        isFindingMatch = true;
        Node* layer = getChildByName("multiplayReady");
        Text* lbl = (Text*)layer->getChildByName("lblStatus");
        LM->setLocalizedString(lbl, "finding match");
        return;
    }
//    if (!isInRoom) {
//        waitingMsg = MSG_CODE_JOIN_OR_CREATE;
//        MM->tryQuickMatch();
        
        
//    }else if(MM->enemyRace >= 0){
    if(MM->enemyRace >= 0){
        MM->requestGameStart();
        isGameStarted = true;
    }
}
void MultiplayLobby::onEnterRoomClick(Ref* ref){
    string roomName = tfRoomName->getString();
    if(roomName.length() < 2){
        showInstanceMessage(LM->getText("more than 2 letters"));
        return;
    }
    BTN_FROM_REF_AND_DISABLE
    
    waitingMsg = MSG_CODE_JOIN_OR_CREATE;
    MultiplayManager::getInstance()->sendMsg(MSG_CODE_JOIN_OR_CREATE, strmake("%s_%s", roomName.c_str(), UDGetStr(KEY_NAME).c_str()));
}
void MultiplayLobby::update(float dt){
    MultiplayManager::getInstance()->update(dt);
    if (MultiplayManager::getInstance()->isConnectionStateChanged) {
        MM->isConnectionStateChanged = false;
        MM->tryQuickMatch();
    }
    if (MultiplayManager::getInstance()->isMsgArrived) {
        MultiplayManager::getInstance()->isMsgArrived = false;
        log("0 %d", (int)MM->receivedMsgList.size());
        
        if (MM->receivedMsgList.size() > 0) {
            MM->receivedMsgList.erase(MM->receivedMsgList.begin());
        }
        
        if (MM->receivedMsgCode.compare(MSG_CODE_JOIN_OR_CREATE)==0) {
            log("1");
            string str = MultiplayManager::getInstance()->receivedMsg;
            if (str.compare("err_0") == 0) { // full
                log("room is full");
                userInfo = "";
            }else{
                if (MM->enemyRace >= 0) {
//                    isInRoom = false;
//                    isFindingMatch = false;
                }else{
                    MM->isHost = true;
                }
    //            if (isInRoom) {
                    userInfo = str;//strmake("%s%s", userInfo.c_str(), str.c_str());
                    updateUserInfo();
    //            }else{
    //                showReadyRoom(str);
    //            }
            }
        }else if (MM->receivedMsgCode.compare(MSG_CODE_MSG)==0){
            log("2");
            if (MM->receivedMsg.compare("0") == 0) {
                showEmoticon(0, false);
            } else if (MM->receivedMsg.compare("1") == 0) {
                showEmoticon(1, false);
            } else if (MM->receivedMsg.compare("2") == 0) {
                showEmoticon(2, false);
            } else if (MM->receivedMsg.compare("3") == 0) {
                showEmoticon(3, false);
            }
        
            
            if (MM->receivedMsg.length() > 4 && MM->receivedMsg.substr(0, 4).compare("race")==0) {
                updateEnemyRace(Value(MM->receivedMsg.substr(4)).asInt());

                isFindingMatch = false;
            }
            
            userInfo = "";
            updateUserInfo();
        }else if (MM->receivedMsgCode.compare(MSG_CODE_QUIT)==0) {
            
            isFindingMatch = true;
            userInfo = "";
            updateUserInfo();
            closeThis();
            TITLE->onMultiplayClickAndSearch();
            
        }
    }
    
    if ( MultiplayManager::getInstance()->receivedMsgList.size() > 0) {
        string str = MultiplayManager::getInstance()->receivedMsgList.at(0);
        if (str.substr(0, 1).compare("3")) {
            
        }else{
            log("** unexpected msg: %s", str.c_str());
        }
//        MultiplayManager::getInstance()->receivedMsgList.erase(MultiplayManager::getInstance()->receivedMsgList.begin());
    }
    if (MultiplayManager::getInstance()->enemyStateChanged){
        MultiplayManager::getInstance()->enemyStateChanged = false;
        updateEnemyInfo();
    }
    if (MultiplayManager::getInstance()->enemyEmoticon >= 0){
        showEmoticon(MultiplayManager::getInstance()->enemyEmoticon, false);
        MultiplayManager::getInstance()->enemyEmoticon = -1;
    }
    
    if (MultiplayManager::getInstance()->enemyGameReady && isGameStarted && countDown >= 0){
        countDown -= dt;
        Node* layer = getChildByName("multiplayReady");
        Text* lblCountDown = (Text*)layer->getChildByName("lblStatus");
        if (!lblCountDown->isVisible()) {
            lblCountDown->setVisible(true);
        }
//        log("count down 0: %s", lblCountDown->getString().c_str());
        lblCountDown->setString(strmake("Game starts in %d", (int)countDown));
//        log("count down 1: %s", lblCountDown->getString().c_str());
        Button* btn = (Button*)layer->getChildByName("btnStart");
        if (btn->isVisible()) {
            btn->setVisible(false);
            layer->getChildByName("btnClose")->setVisible(false);
            
            if(MM->isHost){
                MM->closeGame();
            }
        }
        if (countDown < 0) {
            MultiplayManager::getInstance()->isGameStarted = true;
             std::string strTime = Value((double)BSM->getCurrentTimeT()).asString();
            strTime = strTime.substr(0, strTime.find("."));
            log("strTime %s", strTime.c_str());

            log("multiplay started");
            MultiplayManager::getInstance()->isGameStarted = false;
            //  MultiplayManager::getInstance()->disconnect(); // test
            TITLE->goToMultiplayStage(7777);
            this->unschedule(schedule_selector(MultiplayLobby::update));

            MultiplayManager::getInstance()->gameStartedTime = (long)Value(strTime).asDouble();

            MultiplayManager::getInstance()->sendMsg(MSG_CODE_GAME_START, strTime.c_str());
        }
    }
    if(MM->isGameStarted && isGameStarted){
//        log("multiplay started");
//        MultiplayManager::getInstance()->isGameStarted = false;
////        MultiplayManager::getInstance()->disconnect(); // test
//        TITLE->goToMultiplayStage(7777);
//        this->unschedule(schedule_selector(MultiplayLobby::update));
        return;
    }
    if (MultiplayManager::getInstance()->enemyQuit){
        Node* layer = CSLoader::createNode("MultiplayReady.csb");
        countDown = 3;
        Label* lblCountDown = (Label*)layer->getChildByName("lblCountDown");
        lblCountDown->setVisible(false);
        lblCountDown->setString(Value((int)countDown + 1).asString());
        Button* btn = (Button*)layer->getChildByName("lblStart");
        btn->setVisible(true);
    }
    
    oneSecTimer += dt;
    if (oneSecTimer >= 1) {
        oneSecTimer -= 1;
        if (isFindingMatch) {
            dotCounter++;
            if (dotCounter > 3) {
                dotCounter = 1;
            }
            Node* layer = getChildByName("multiplayReady");
            Text* lbl = (Text*)layer->getChildByName("lblStatus");
            
            std::string str = "";
            for (int i = 0; i < dotCounter; i++) {
                str += ".";
            }
            lbl->setString(strmake("%s%s", LM->getText("finding match").c_str(), str.c_str()));

            if (!MultiplayManager::getInstance()->isConnected) {
                closeThis();
            }
        }
    }
}
void MultiplayLobby::updateEnemyInfo(){
    Node* layer = this->getChildByName("multiplayReady");
    bool isMatchFound = MultiplayManager::getInstance()->enemyRace >= 0;
    log("race: %d", MultiplayManager::getInstance()->enemyRace);
    layer->getChildByName("btnHumanEnemy")->setVisible(isMatchFound);
    layer->getChildByName("btnOrcEnemy")->setVisible(isMatchFound);
    layer->getChildByName("imgSelectedEnemy")->setVisible(isMatchFound);
    
    Button* btn;
    layer->getChildByName("imgEnemy")->setVisible(isMatchFound);
    if (isMatchFound) {
        updateEnemyRace(MultiplayManager::getInstance()->enemyRace);
        MM->sendMsg(MSG_CODE_MSG, strmake("race%d", MM->heroRace));

        btn = (Button*)layer->getChildByName("btnStart");
        btn->setEnabled(true);
    }
    for (int i = 0; i < 4; i++) {
        btn =(Button*)layer->getChildByName(strmake("btnEmoticon%d", i));
        btn->setEnabled(isMatchFound);
    }
    Text* lblYou = (Text*)layer->getChildByName("lblYou");
    lblYou->setString(MultiplayManager::getInstance()->enemyName);
    
    int trophy = MultiplayManager::getInstance()->enemyTrophy;
    Text* lblRecordYou = (Text*)layer->getChildByName("lblRecordYou");
    lblRecordYou->setString(strmake("%d",trophy));
    
    Text* lbl = (Text*)layer->getChildByName("lblStatus");
    lbl->setString("");
    log("*lbl status set");
    
    btn = (Button*)layer->getChildByName("btnStart");
    btn->setEnabled(isMatchFound);
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "battle");
}
void MultiplayLobby::showReadyRoom(std::string users){
//    Node* layer = CSLoader::createNode("MultiplayReady.csb");
//    this->addChild(layer, 4);
//    layer->setName("multiplayReady");
//    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
//    layer->setPositionY(size.height/2 - layer->getContentSize().height/2);
//    setAsPopup(layer);
//
//    Button* btn = (Button*)layer->getChildByName("btnClose");
//    btn->addClickEventListener(CC_CALLBACK_0(MultiplayLobby::onCloseRoom, this));
//    Vector<Node*> list = layer->getChildren();
//    log("list: %d", (int)list.size());
//    for(auto child : layer->getChildren()){
//        log("child name: %s", child->getName().c_str());
//    }
//    userInfo = strmake("%s%s", userInfo.c_str(), users.c_str());
//    updateUserInfo();
//
//    Text* lbl =(Text*)layer->getChildByName("lblRoomName");
//    lbl->setString(strmake("%s:%s", LM->getText("room name").c_str(), tfRoomName->getString().c_str()));
////    btn = (Button*)layer->getChildByName("btnEnter");
////    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onEnterRoomClick, this));
//
//    btn = (Button*)layer->getChildByName("btnStart");
//    btn->addClickEventListener(CC_CALLBACK_1(MultiplayLobby::onStartClick, this));
//
//
//
    isInRoom = true;
}

void MultiplayLobby::updateUserInfo(){
    
    Node* layer = this->getChildByName("multiplayReady");
//    ValueVector list = GM->split(userInfo, "_");
    Text* lblYou = (Text*)layer->getChildByName("lblYou");
    Text* lblRecordYou = (Text*)layer->getChildByName("lblRecordYou");
//    if(list.size() > 1){
    if(MM->enemyName.length() > 0){
        log("name: %s", MM->enemyName.c_str());
        lblYou->setString(MM->enemyName);
        log("lblYou: %s", lblYou->getString().c_str());
        lblRecordYou->setString(strmake("%d", MM->enemyTrophy));
        lblYou->setVisible(true);
        lblRecordYou->setVisible(true);
    }else{
//        lblYou->setString(LM->getText("finding match"));
//        lblRecordYou->setString("...");
        
    }
    
//    Text* lblMe = (Text*)layer->getChildByName("lblMe");
//    lblMe->setString(UDGetStr(KEY_NAME, "undefined"));
    
//    Text* lblRecordMe = (Text*)layer->getChildByName("lblRecordMe");
//    lblRecordMe->setString("");
}
void MultiplayLobby::onCloseRoom(){
    log("close this");
    closeThis();
}
void MultiplayLobby::closeThis() {
    log("close this!!");
    isInRoom = false;
    MM->sendQuit();
    MultiplayManager::getInstance()->disconnect();
    this->unschedule(schedule_selector(MultiplayLobby::update));
    resetEnemyInfo();
    
    PageBase::closeThis();
}
void MultiplayLobby::resetEnemyInfo(){
    MM->isGameStarted = false;
    MM->enemyGameReady = false;
    MM->enemyRace = -1;
    MM->enemyName = "";
    MM->enemyTrophy = -1;
    MM->isHost = false;
}

void MultiplayLobby::setHeroBackground(int level){
    Node* layer = this->getChildByName("multiplayReady");
    layer->getChildByName("imgHeroBackLeather")->setVisible(level == 0);
    layer->getChildByName("imgHeroBackStone")->setVisible(level == 1);
    layer->getChildByName("imgHeroBackIron")->setVisible(level == 2);
    layer->getChildByName("imgHeroBackGold")->setVisible(level == 3);
}
void MultiplayLobby::setEnemyBackground(int level){
    Node* layer = this->getChildByName("multiplayReady");
    layer->getChildByName("imgEnemyBackLeather")->setVisible(level == 0);
    layer->getChildByName("imgEnemyBackStone")->setVisible(level == 1);
    layer->getChildByName("imgEnemyBackIron")->setVisible(level == 2);
    layer->getChildByName("imgEnemyBackGold")->setVisible(level == 3);
}
