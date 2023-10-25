#ifndef MultiplayManager_hpp
#define MultiplayManager_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "PageBase.h"
#include "NetworkWebSocket.h"

#define MSG_CODE_JOIN_OR_CREATE "0"
#define MSG_CODE_MSG "1"
#define MSG_CODE_QUIT "2"
#define MSG_CODE_GAME_START "3"
#define MSG_CODE_UNIT_MOVE "4"
#define MSG_CODE_UNIT_ATTACKED "5"
#define MSG_CODE_CONSTRUCT "6"
#define MSG_CODE_DESTROY "7"
#define MSG_CODE_UNIT_DEAD "8"
#define MSG_CODE_UNIT_CREATED "9"
#define MSG_CODE_QUICK_MATCH "a"
#define MSG_CODE_WORKER_GO_TO_MINE "b"
#define MSG_CODE_WORKER_BRING_GOLD_TO "c"
#define MSG_CODE_WORKER_GO_TO_TREE "d"
#define MSG_CODE_WORKER_BRING_LUMBER_TO "e"
#define MSG_CODE_UNIT_ATTACK_ORDER_TARGET "f"
#define MSG_CODE_UNIT_STOP "g"
#define MSG_CODE_MOVE_ATTACK "h"
#define MSG_CODE_ENEMY_MOVE_ATTACK "i"
#define MSG_CODE_ATTACK_TARGET "j"
#define MSG_CODE_MOVEFLAG_CHANGED "k"
#define MSG_CODE_TARGET_CHANGED "l"
#define MSG_CODE_ATTACK_DDANG "m"
#define MSG_CODE_PING "o"
#define MSG_CODE_PING_BACK "p"
#define MSG_CODE_CLOSE_GAME "q"
#define MSG_CODE_PAUSED "r"
#define MSG_CODE_RESUMED "s"
#define MSG_CODE_CUT_TREE "t"
#define MSG_CODE_CONSTRUCT_COMPLETE "u"
#define MSG_CODE_UNIT_KILLED "v"

#define LATANCY 42


//#define MSG_CODE_ ""

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocos2d;

struct Msg {
    std::string msgType;
    std::string sourceIDs;
    int targetID;
    int value0;
    int value1;
    int value2;
    int value3;
    int startTime;
} ;

class MultiplayManager : public Node
{
private:
    NetworkWebSocket* _webSocket = nullptr;
    MultiplayManager();
    
    //Instance of the singleton
    static MultiplayManager* m_mySingleton;
public:
    bool isHost = false;
    bool isGameStarted = false;
    time_t gameStartedTime;
    bool isMsgArrived = false;
    float collectTime = 0;
    std::string receivedMsgCode;
    std::string receivedMsg;
    std::vector<std::string> receivedMsgList;
    std::vector<Msg> msgList;
    std::vector<std::string> rmsgStrList;
    std::vector<Msg> rmsgList;
    bool msgLock = false;
    std::mutex rmsgLock;
    bool isConnected = false;
    bool isConnectionStateChanged =false;
    bool _quickMatchRequet = false;
    static MultiplayManager* getInstance();
    
    int heroRace=0;
    int enemyRace=-1;
    bool enemyStateChanged = false;
    std::string enemyName;
    int enemyTrophy = -1;
    int enemyEmoticon = -1;
    std::string enemyID = "";
    bool enemyGameReady = false;
    bool enemyQuit = false;
    void updateEnemyInfo();
    
    void onConnectionOpened();
    void onMessageReceived(string message);
    void onConnectionClosed();
    void onConnectionError(const cocos2d::network::WebSocket::ErrorCode & error);
    
    void sendMsg(std::string code, std::string msg);
    
    void moveUnit(std::string movingIDs, int tx, int ty, int timeOffset = 0);
    void attackUnit(std::string attackerIDs, int attackeeID, int dmg);
    void createdUnit(int unitID, int unitType, int x, int y, int createTime);
    void killedUnit(int unitID, int attackerID, int x, int y);
    void deadUnit(int unitID, int attackerID, int x, int y);
    
    void update(float dt);
    void disconnect();
    void connect();
    
    void tryQuickMatch();
    void requestQuickMatch();
    void requestGameStart();
    void sendQuit();
    
    void gatherTree(std::string movingIDs, int treeID);
    void returnLumber(std::string movingIDs, int placeID);
    void gatherGold(std::string movingIDs, int mineID);
    void returnGold(std::string movingIDs, int placeID);
    void stopUnit(std::string movingIDs, int x, int y);
    void moveAndAttackTo(std::string movingIDs, int tx, int ty);
    void enemyMoveAndAttackTo(std::string movingIDs, int tx, int ty);
    void workerBuilding(int sourceID, int buildingIndex, int tx, int ty);
    void attackTarget(std::string sourceIDs, int targetID);
    void moveFlagChanged(int sourceID, int x, int y);
    void targetChanged(int sourceID, int targetID);
    void attackDdang(int sourceID, int x, int y);
    void cutTree(int sourceID, int workerID, int x, int y, int tankID);
    void ping();
    void pingBack();
    void paused();
    void resumed();
    void closeGame();
    time_t pingStartTime;
    time_t lastMsgTime;
    bool isPingSuccess = true;
};

#endif
