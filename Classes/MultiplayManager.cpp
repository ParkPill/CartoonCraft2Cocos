#include "MultiplayManager.h"
#include "LanguageManager.h"
#include "GameManager.h"
//#include "NativeInterface.h"
using namespace cocos2d;


MultiplayManager* MultiplayManager::m_mySingleton = NULL;

MultiplayManager::MultiplayManager()
{
    //    _webSocket = NetworkWebSocket::create("http://222.120.115.95:8105");
//    _webSocket = NetworkWebSocket::create("http://localhost:8105");
    
}
MultiplayManager* MultiplayManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = new MultiplayManager();
    }
    
    return m_mySingleton;
}
void MultiplayManager::disconnect(){
    isConnected = false;
    enemyRace = -1;
    if(_webSocket){
        _webSocket->close();
    }
}
void MultiplayManager::connect(){
    disconnect();
//    _webSocket = NetworkWebSocket::create("192.168.0.46:8105");
//    _webSocket = NetworkWebSocket::create("http://localhost:8105");
    _webSocket = NetworkWebSocket::create("http://220.79.132.112:8105");
    if (_webSocket->connect()) {
        log("connected");
        this->_webSocket->setOnConnectionOpenedCallBack(CC_CALLBACK_0(MultiplayManager::onConnectionOpened, this));
        this->_webSocket->setOnMessageReceivedCallBack(CC_CALLBACK_1(MultiplayManager::onMessageReceived, this));
        this->_webSocket->setOnConnectionClosedCallBack(CC_CALLBACK_0(MultiplayManager::onConnectionClosed, this));
        this->_webSocket->setOnErrorOccurredCallBack(CC_CALLBACK_1(MultiplayManager::onConnectionError, this));
        this->schedule(schedule_selector(MultiplayManager::update), 0.1f);
    }else{
        log("connection failed");
    }
}
void MultiplayManager::onConnectionOpened()
{
    log("INFO : Connection opened");
    isConnected = true;
    lastMsgTime = time(0);
    isConnectionStateChanged = true;
}

void MultiplayManager::sendMsg(std::string code, std::string msg){
    isMsgArrived = false;
    _webSocket->send(StringUtils::format("%s%s", code.c_str(), msg.c_str()));
}

void MultiplayManager::onMessageReceived(string message)
{
    log("Receive : %s", message.c_str());
    lastMsgTime = time(0);
    receivedMsgCode = message.substr(0, 1);
    isMsgArrived = true;
    if(receivedMsgCode.compare(MSG_CODE_JOIN_OR_CREATE) == 0){
        log("join->%s", message.c_str());
        receivedMsg = message.substr(1).c_str();
        receivedMsgList.push_back(receivedMsg);
        if(message.length() > 1){
            ValueVector list = GM->split(receivedMsg, "_");
            enemyName = list.at(0).asString();
            enemyTrophy = list.at(1).asInt();
            enemyID = list.at(2).asString();
            enemyRace = 0;
            enemyStateChanged = true;
        }else{
            isHost = false;
        }
    }else if(receivedMsgCode.compare(MSG_CODE_MSG) == 0){
        log("msg");
        receivedMsg = message.substr(1);
        receivedMsgList.push_back(receivedMsg);
        
    }else if(receivedMsgCode.compare(MSG_CODE_QUIT) == 0){
        log("quit->%s", message.c_str());
        receivedMsg = message.substr(1);
        receivedMsgList.push_back(receivedMsg);
        enemyName = "";
        enemyTrophy = -1;
        enemyRace = -1;
        enemyStateChanged = true;
//        isMsgArrived = true;
    }else if(receivedMsgCode.compare(MSG_CODE_GAME_START) == 0){
        if (!enemyGameReady) {
            enemyGameReady = true;
        }else{
            isGameStarted = true;
        }
        gameStartedTime = (long)Value(message.substr(1)).asDouble();
        log("game start requested");
    }else{//} if(receivedMsgCode.compare(MSG_CODE_UNIT_MOVE) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_UNIT_ATTACKED) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_CONSTRUCT) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_DESTROY) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_UNIT_DEAD) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_UNIT_CREATED) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_WORKER_GO_TO_TREE) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_WORKER_GO_TO_MINE) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_WORKER_BRING_GOLD_TO) == 0 ||
//             receivedMsgCode.compare(MSG_CODE_WORKER_BRING_LUMBER_TO) == 0 ||
//                         receivedMsgCode.compare(MSG_CODE_WORKER_BRING_LUMBER_TO) == 0){ // unit move
        rmsgLock.lock();
        ValueVector list = GM->split(message, "/");
        log("msg received message: %s,%d", message.c_str(), (int)list.size());
        for (int i = 0; i < list.size(); i++) {
            if(list.at(i).asString().length() <= 0){
                log("continue");
                continue;
            }
            ValueVector smallList = GM->split(list.at(i).asString(), "_");//GM->split(message, "_");
            if(smallList.size() >= 1 ){
                Msg msg;
                msg.msgType = Value(smallList.at(0).asString().substr(0, 1)).asString();
                log("msg.msgType: %s", msg.msgType.c_str());
                bool skipQueue = false;
                if ( msg.msgType.compare(MSG_CODE_UNIT_CREATED)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.value2 = smallList.at(3).asInt();
                    msg.startTime = smallList.at(4).asInt();
                    log("unit create received %s, %d, %d, %d", msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2);
                }
                else if ( msg.msgType.compare(MSG_CODE_UNIT_MOVE)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.startTime = smallList.at(3).asInt();
                    log("MSG_CODE_UNIT_MOVE %d", msg.startTime);
                }
                else if (msg.msgType.compare(MSG_CODE_ATTACK_DDANG)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
//                    msg.targetID = smallList.at(1).asInt();
//                    msg.startTime = smallList.at(2).asInt();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.startTime = smallList.at(3).asInt();
                    log("MSG_CODE_ATTACK_DDANG %d", msg.startTime);
                }else if ( msg.msgType.compare(MSG_CODE_ATTACK_TARGET)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.targetID = smallList.at(1).asInt();
                    msg.startTime = smallList.at(2).asInt();
                    log("MSG_CODE_ATTACK_TARGET");
                }else if ( msg.msgType.compare(MSG_CODE_WORKER_GO_TO_MINE)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.targetID = smallList.at(1).asInt();
                    msg.startTime = smallList.at(2).asInt();
                    log("go to mine %d", msg.startTime);
                }else if ( msg.msgType.compare(MSG_CODE_WORKER_GO_TO_TREE)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.targetID = smallList.at(1).asInt();
                    msg.startTime = smallList.at(2).asInt();
                    log("go to tree %d", msg.startTime);
                }else if ( msg.msgType.compare(MSG_CODE_CONSTRUCT)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.value2 = smallList.at(3).asInt();
                    msg.startTime = smallList.at(4).asInt();
                    log("building");
                }else if ( msg.msgType.compare(MSG_CODE_PING)==0) {
                    pingBack();
                    skipQueue = true;
                }else if ( msg.msgType.compare(MSG_CODE_PING_BACK)==0) {
                    time_t receiveTime = time(0);
                    int diff = (int)(receiveTime - pingStartTime);
                    log("ping diff %d", diff);
                    isPingSuccess = true;
                    skipQueue = true;
                }else if ( msg.msgType.compare(MSG_CODE_UNIT_DEAD)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.value2 = smallList.at(3).asInt();
                    msg.startTime = smallList.at(4).asInt();
                }else if ( msg.msgType.compare(MSG_CODE_UNIT_KILLED)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.value2 = smallList.at(3).asInt();
                    msg.startTime = smallList.at(4).asInt();
                }else if ( msg.msgType.compare(MSG_CODE_CUT_TREE)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.value2 = smallList.at(3).asInt();
                    msg.value3 = smallList.at(4).asInt();
                    msg.startTime = smallList.at(5).asInt();
                }else if ( msg.msgType.compare(MSG_CODE_UNIT_ATTACKED)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.targetID = Value(str.substr(1, str.length() - 1)).asInt();
                    msg.value0 = smallList.at(1).asInt();
                    log("4");
                }else if ( msg.msgType.compare(MSG_CODE_WORKER_BRING_GOLD_TO)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    log("bring back gold");
                }else if ( msg.msgType.compare(MSG_CODE_WORKER_BRING_LUMBER_TO)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    log("bring back lumber");
                }else if ( msg.msgType.compare(MSG_CODE_UNIT_STOP)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    log("unit stop");
                }else if ( msg.msgType.compare(MSG_CODE_MOVE_ATTACK)==0 ||
                          msg.msgType.compare(MSG_CODE_ENEMY_MOVE_ATTACK)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    msg.startTime = smallList.at(3).asInt();
                    log("unit attack");
                }else if ( msg.msgType.compare(MSG_CODE_MOVEFLAG_CHANGED)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.value0 = smallList.at(1).asInt();
                    msg.value1 = smallList.at(2).asInt();
                    log("MSG_CODE_MOVEFLAG_CHANGED");
                }else if (msg.msgType.compare(MSG_CODE_TARGET_CHANGED)==0) {
                    std::string str = smallList.at(0).asString();
                    msg.sourceIDs = Value(str.substr(1, str.length() - 1)).asString();
                    msg.targetID = smallList.at(1).asInt();
                    log("MSG_CODE_TARGET_CHANGED");
                }else if (msg.msgType.compare(MSG_CODE_PAUSED)==0) {
                    std::string str = smallList.at(0).asString();
                    log("MSG_CODE_PAUSED");
                }else if (msg.msgType.compare(MSG_CODE_RESUMED)==0) {
                    std::string str = smallList.at(0).asString();
                    log("MSG_CODE_RESUMED");
                }else{
                    log("5");
                }

                if(!skipQueue){
                    rmsgList.push_back(msg);
                }
            }
        }
        rmsgLock.unlock();
    }
//    else if(receivedMsgCode.compare(MSG_CODE_UNIT_DEAD) == 0){
//
//        log("unit dead");
//    }else if(receivedMsgCode.compare(MSG_CODE_UNIT_CREATED) == 0){
//        log("unit created");
//    }
}

void MultiplayManager::onConnectionClosed()
{
    log("INFO : Connection closed");
    isConnected = false;
    isConnectionStateChanged = false;
}
void MultiplayManager::onConnectionError(const cocos2d::network::WebSocket::ErrorCode & error)
{
    string errorCodeValue = "";
    switch (error)
    {
    case cocos2d::network::WebSocket::ErrorCode::CONNECTION_FAILURE:
        errorCodeValue = "CONNECTION_FAILURE";
        break;
    case cocos2d::network::WebSocket::ErrorCode::TIME_OUT:
        errorCodeValue = "TIME_OUT";
        break;
    case cocos2d::network::WebSocket::ErrorCode::UNKNOWN:
        errorCodeValue = "UNKNOWN";
        break;
    }
    log("chat room error: %s", errorCodeValue.c_str());
//    AddLine(LM->getText("connection failed"));
}
void MultiplayManager::update(float dt){
    if(!isConnected) {
        if (msgList.size() > 0) {
            msgList.clear();
        }
        return;
    }
    collectTime += dt;
    if (collectTime > 0.3f) {
        collectTime = 0;
        return;
    }
    time_t now = time(0);
    if (!isPingSuccess) {
        if(now - pingStartTime > 5000){
            isPingSuccess = true;
            disconnect();
            if (WORLD) {
                WORLD->endGame(true);
            }
        }
    }
    if (now - lastMsgTime > 5000) {
        ping();
        lastMsgTime = now;
    }
    
    std::string strCreated = "";
//    if (msgList.size() > 0) {
    for (int i = 0; i < msgList.size(); i++) {
        Msg msg = msgList.at(i);
        log("sends %s", msgList.at(i).msgType.c_str());
        if (msg.msgType.compare(MSG_CODE_QUICK_MATCH) == 0){
            strCreated += (strmake("%s%s/", msg.msgType.c_str(),msg.sourceIDs.c_str()));
        }else if (msg.msgType.compare(MSG_CODE_GAME_START) == 0){
            strCreated += (strmake("%s/", msg.msgType.c_str()));
        }else if (msg.msgType.compare(MSG_CODE_QUIT) == 0){
            strCreated += (strmake("%s/", msg.msgType.c_str()));
        }else{
            if(msg.msgType.compare(MSG_CODE_UNIT_CREATED)==0){
                log("* send through socket: %s", msg.sourceIDs.c_str());
                strCreated = (strmake("%s%s%s_%d_%d_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_UNIT_MOVE)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_ATTACK_TARGET)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_ATTACK_DDANG)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_WORKER_GO_TO_MINE)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_UNIT_DEAD)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_UNIT_KILLED)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_CUT_TREE)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2, msg.value3, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_UNIT_ATTACKED)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID, msg.value0));
            }else if(msg.msgType.compare(MSG_CODE_WORKER_GO_TO_TREE)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID, msg.startTime));
                log("send update go to tree: %d", msg.startTime);
            }else if(msg.msgType.compare(MSG_CODE_WORKER_BRING_GOLD_TO)==0){
                strCreated = (strmake("%s%s%s_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0));
            }else if(msg.msgType.compare(MSG_CODE_WORKER_BRING_LUMBER_TO)==0){
                strCreated = (strmake("%s%s%s_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0));
            }else if(msg.msgType.compare(MSG_CODE_UNIT_STOP)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0,msg.value1));
            }else if(msg.msgType.compare(MSG_CODE_MOVE_ATTACK)==0 ||
                     msg.msgType.compare(MSG_CODE_ENEMY_MOVE_ATTACK)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0,msg.value1, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_CONSTRUCT)==0){
                strCreated = (strmake("%s%s%s_%d_%d_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1, msg.value2, msg.startTime));
            }else if(msg.msgType.compare(MSG_CODE_MOVEFLAG_CHANGED)==0){
                strCreated = (strmake("%s%s%s_%d_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1));
            }else if(msg.msgType.compare(MSG_CODE_TARGET_CHANGED)==0){
                strCreated = (strmake("%s%s%s_%d/", strCreated.c_str(),msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID));
            }else if(msg.msgType.compare(MSG_CODE_PING)==0){
                strCreated = (strmake("%s%s/", strCreated.c_str(),msg.msgType.c_str()));
                pingStartTime = time(0);
                isPingSuccess = false;
            }else if(msg.msgType.compare(MSG_CODE_PING_BACK)==0){
                strCreated = (strmake("%s%s/", strCreated.c_str(),msg.msgType.c_str()));
            }else if(msg.msgType.compare(MSG_CODE_CLOSE_GAME)==0){
                strCreated = (strmake("%s%s/", strCreated.c_str(),msg.msgType.c_str()));
            }else if(msg.msgType.compare(MSG_CODE_PAUSED)==0){
                strCreated = (strmake("%s%s/", strCreated.c_str(),msg.msgType.c_str()));
            }else if(msg.msgType.compare(MSG_CODE_RESUMED)==0){
                strCreated = (strmake("%s%s/", strCreated.c_str(),msg.msgType.c_str()));
            }
        }
        
//        msgList.erase(msgList.begin());
//        } else {
//            int loopCounter = 0;
//            std::string previousMsgCode = "";
//            while(msgList.size() > 0){
//                Msg msg = msgList.at(0);
//                if (previousMsgCode.length() > 0 && msg.msgType.compare(previousMsgCode) != 0) {
//                    break;
//                }
//                if(msg.msgType.compare(MSG_CODE_UNIT_CREATED)==0){
//                    strCreated += strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1);//msgcode unitType_x_y
//                }else if(msg.msgType.compare(MSG_CODE_UNIT_DEAD)==0){
//                    strCreated += strmake("%s%s%s/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str());
//                }else if(msg.msgType.compare(MSG_CODE_UNIT_MOVE)==0){
//                    strCreated += strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.value0, msg.value1);
//                }else if(msg.msgType.compare(MSG_CODE_UNIT_ATTACKED)==0){
//                    strCreated += strmake("%s%s%s_%d_%d/", strCreated.c_str(), msg.msgType.c_str(), msg.sourceIDs.c_str(), msg.targetID, msg.value0);
//                }else if(msg.msgType.compare(MSG_CODE_QUICK_MATCH)==0){
//                    strCreated += strmake("%s%s/", strCreated.c_str(), msg.sourceIDs.c_str());
//                    log("sends quick match");
//                }
//                previousMsgCode = msg.msgType;
//                msgList.erase(msgList.begin());
//                loopCounter++;
//                if(loopCounter > 1000){
//                    break;
//                }
//            }
//            if(loopCounter > 1000){
//                msgList.clear();
//            }
//            strCreated = strCreated.substr(0, strCreated.length()-1);
//            log("sends msg: %s", strCreated.c_str());
//            _webSocket->send(strCreated);
//            msgList.clear();
//        }
    }
    if(strCreated.length() > 0){

        strCreated = strCreated.substr(0, strCreated.length()-1);
        log("sends msg: %s", strCreated.c_str());
        _webSocket->send(strCreated);
    }
    msgList.clear();
}

void MultiplayManager::tryQuickMatch()
{
    log("try quick match;");
    if (isConnected)
    {
        log("already open");
        requestQuickMatch();
    }
    else
    {
        _quickMatchRequet = true;
        connect();
    }
}
void MultiplayManager::requestQuickMatch()
{
    // requesting data -> 0username_unitdata(34_3:index_level),unitdata
    int trophy = UDGetInt(KEY_MULTIPLAY_TROPHY, 1000);
    std::string name = strmake("%s_%d_%s", UDGetStr(KEY_NAME).c_str(), trophy, UDGetStr(KEY_SAVED_ID, "-1").c_str());
//    std::string unitData = "";
    //int group, index, level;
//    UnitInfo info;
//    std::string strMsg = strmake("%s%s,%s", MSG_CODE_QUICK_MATCH, name.c_str(), unitData.c_str());

    log("send msg: quick match : %d, %s", (int)msgList.size(), name.c_str());
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_QUICK_MATCH;
    msg.sourceIDs = name;
    msgList.push_back(msg);
    msgLock = false;
}

void MultiplayManager::requestGameStart()
{
    log("send msg game start");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_GAME_START;
    msgList.push_back(msg);
    msgLock = false;
}

void MultiplayManager::sendQuit()
{
    log("send msg quit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_QUIT;
    msgList.push_back(msg);
    msgLock = false;
}

void MultiplayManager::moveUnit(std::string movingIDs, int tx, int ty, int timeOffset){
    log("send msg: move unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_MOVE;
    msg.sourceIDs = movingIDs;
    msg.value0 = tx;
    msg.value1 = ty;
    msg.startTime = GM->getWorld()->gameFrameTimer + LATANCY + timeOffset;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::createdUnit(int unitID, int unitType, int x, int y, int createTime){
    log("send msg: create unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_CREATED;
    msg.sourceIDs = Value(unitID).asString();
    msg.value0 = unitType;
    msg.value1 = x;
    msg.value2 = y;
    msg.startTime = createTime;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::moveAndAttackTo(std::string movingIDs, int tx, int ty){
    log("send msg: move and attack");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_ATTACK_DDANG;
    
    msg.sourceIDs = movingIDs;
    msg.value0 = tx;
    msg.value1 = ty;
    msg.startTime = WORLD->gameFrameTimer + LATANCY;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::enemyMoveAndAttackTo(std::string movingIDs, int tx, int ty){
    log("send msg: enemyMoveAndAttackTo");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_ENEMY_MOVE_ATTACK;
    msg.sourceIDs = movingIDs;
    msg.value0 = tx;
    msg.value1 = ty;
    msg.startTime = WORLD->gameFrameTimer;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::attackTarget(std::string  sourceIDs, int targetID){
    log("send msg: attackTarget");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_ATTACK_TARGET;
    msg.sourceIDs = sourceIDs;
    msg.targetID = targetID;
    msg.startTime = GM->getWorld()->gameFrameTimer + LATANCY;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::cutTree(int sourceID, int workerID, int x, int y, int tankID){
    log("send msg: move and attack");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_CUT_TREE;
    msg.sourceIDs = Value(sourceID).asString();
    msg.value0 = workerID;
    msg.value1 = x;
    msg.value2 = y;
    msg.value3 = tankID;
    msg.startTime = GM->getWorld()->gameFrameTimer;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::returnLumber(std::string movingIDs, int placeID){
    return;
    log("send msg: return tree");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_WORKER_BRING_LUMBER_TO;
    msg.sourceIDs = movingIDs;
    msg.value0 = placeID;
    msgList.push_back(msg);
    msgLock = false;
}

void MultiplayManager::returnGold(std::string movingIDs, int placeID){
    return;
    log("send msg: return gold");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_WORKER_BRING_GOLD_TO;
    msg.sourceIDs = movingIDs;
    msg.value0 = placeID;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::gatherTree(std::string movingIDs, int treeID){
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_WORKER_GO_TO_TREE;
    msg.sourceIDs = movingIDs;
    msg.targetID = treeID;
    msg.startTime = WORLD->gameFrameTimer + LATANCY;
    log("send msg: gather tree %d", msg.startTime);
    
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::gatherGold(std::string movingIDs, int mineID){
    log("send msg: gather gold");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_WORKER_GO_TO_MINE;
    msg.sourceIDs = movingIDs;
    msg.targetID = mineID;
    msg.startTime = WORLD->gameFrameTimer + LATANCY;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::workerBuilding(int sourceID, int buildingIndex, int tx, int ty){
    log("send msg: workerBuilding");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_CONSTRUCT;
    msg.sourceIDs = Value(sourceID).asString();
    msg.value0 = buildingIndex;
    msg.value1 = tx;
    msg.value2 = ty;
    msg.startTime = WORLD->gameFrameTimer + LATANCY;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::ping(){
    log("send msg: ping");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_PING;
    if (!WORLD) {
        return;
    }
    msg.startTime = WORLD->gameFrameTimer;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::pingBack(){
    log("send msg: pingBack");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_PING_BACK;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::closeGame(){
    log("send msg: closeGame");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_CLOSE_GAME;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::paused(){
    log("send msg: paused");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_PAUSED;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::resumed(){
    log("send msg: resumed(){");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_RESUMED;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::attackUnit(std::string attackerIDs, int attackeeID, int dmg){
    return;
    log("send msg: attack unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_ATTACKED;
    msg.sourceIDs = attackerIDs;
    msg.targetID = attackeeID;
    msg.value0 = dmg;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::killedUnit(int unitID, int attackerID, int x, int y){
    log("send msg: kill unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_DEAD;
    msg.sourceIDs = Value(unitID).asString();
    msg.value0 = attackerID;
    msg.value1 = x;
    msg.value2 = y;
    msg.startTime = WORLD->gameFrameTimer;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::deadUnit(int unitID, int attackerID, int x, int y){
    log("send msg: dead unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_KILLED;
    msg.sourceIDs = Value(unitID).asString();
    msg.value0 = attackerID;
    msg.value1 = x;
    msg.value2 = y;
    msg.startTime = WORLD->gameFrameTimer;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::stopUnit(std::string source, int x, int y)
{
    return;
    log("send msg stop unit");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_UNIT_STOP;
    msg.sourceIDs = source;
    msg.value0 = x;
    msg.value1 = y;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::attackDdang(int sourceID, int x, int y){
    return;
    log("send msg: attackDdaing");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_ATTACK_DDANG;
    msg.sourceIDs = Value(sourceID).asString();
    msg.value0 = x;
    msg.value1 = y;
    msgList.push_back(msg);
    msgLock = false;
}
void MultiplayManager::moveFlagChanged(int sourceID, int x, int y){
    return;
    log("send msg: moveFlagChanged");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_MOVEFLAG_CHANGED;
    msg.sourceIDs = Value(sourceID).asString();
    msg.value0 = x;
    msg.value1 = y;
    msgList.push_back(msg);
    msgLock = false;
}

void MultiplayManager::targetChanged(int sourceID, int targetID){
    return;
    log("send msg: targetChanged");
    msgLock = true;
    Msg msg;
    msg.msgType = MSG_CODE_TARGET_CHANGED;
    msg.sourceIDs = Value(sourceID).asString();
    msg.targetID = targetID;
    msgList.push_back(msg);
    msgLock = false;
}
