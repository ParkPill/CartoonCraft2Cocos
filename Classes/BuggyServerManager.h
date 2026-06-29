//
//  BuggyServerManager.h
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#ifndef __LegendDaryKakao__BuggyServerManager__
#define __LegendDaryKakao__BuggyServerManager__

#include <iostream>
#include <time.h>
#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "json/document.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"

#define DATA_TYPE_GEM 0
#define DATA_TYPE_GOLD 1
#define DATA_TYPE_TREE 2
#define DATA_TYPE_BUILDING 3
#define DATA_TYPE_INVENTORY 4
#define DATA_TYPE_DECK 5
#define DATA_TYPE_IAP 6
#define DATA_TYPE_SEARCH_STATE 7
#define DATA_TYPE_LAST_LAUNCH_DATE 8
#define DATA_TYPE_SEARCH_ITEMS 9
#define DATA_TYPE_HERO_INVENTORY 10
#define DATA_TYPE_HERO_DECK 11
#define DATA_TYPE_KEYS 12
#define DATA_TYPE_KEY_GET_STATE 13
#define DATA_TYPE_STAGE_CLEAR 14
#define DATA_TYPE_STAGE_HARD_CLEAR 15
#define DATA_TYPE_PLAYID 16

#define SET_DOCUMENT_AND_CHECK_ERROR rapidjson::Document document = getDocument(sender, data); if(document.IsNull()){ this->isServerFailed = true; log("docu null"); return; } if (document.HasMember("error")) { log("server data error: %s", document["error"].GetString()); isFailedToGetNetworkData = true;}

//#include "Title.h"

class BuggyServerManager: public cocos2d::Layer
{
private:
    //Constructor
    BuggyServerManager();
    
    //Instance of the singleton
    static BuggyServerManager* m_mySingleton;
//    time_t receivedTimeT;
    struct tm receivedTime;
    struct tm receivedYesterdayTime;
public:
    std::string apiName;
    bool getLocalTime = false;
    time_t getTimeTFromStr(std::string strTime);
    time_t receivedWebTimeT;
    time_t startLocalTime;
    double startLocalTimeDouble = 0;
    double webTime;
    bool timeEstablished;
    bool isTrying;
    int retryCount=0;
//    double getCurrentTime();
    time_t getCurrentTimeT();
    
    const char* getRemainTime(double targetTime);
    static BuggyServerManager* getInstance();
    
    virtual bool init();
    CREATE_FUNC(BuggyServerManager);
    void getHttpTime();
    void establishLocalTimeBaseline();
    void setServerUrl();
    void onHttpRequestCompleted(cocos2d::Node *sender, void *data);
    void getVer();
    void onGetVerCompleted(cocos2d::Node *sender, void *data);
    void updateTick(float dt);
    
    int getDay();
    int getMonth();
    int getYear();
    
    int getYesterdayDay();
    int getYesterdayMonth();
    int getYesterdayYear();
    int getMonthIndex( std::string name );
    
    
    rapidjson::Document getDocument(cocos2d::Node *sender, void *data);
    rapidjson::Document lastDocument;
//    bool isServerConnected = false;
    bool isServerFailed = false;
    std::string serverUrl;
    std::string gameName;
    

    std::string requestedID = "-2";
    std::string syncRequestKey;
    
    bool shouldRequestTime;
    float timeRetryTimer = 0;
        
    
    void setTime(std::string strTime);
    

    void getPostBoxItem();
    void onGetPostBoxItem(cocos2d::Node *sender, void *data);
    void setPostBoxItem(std::string items);
    void onSetPostBoxItem(cocos2d::Node *sender, void *data);
    void registerName(std::string text);
    void onRegisterNameComplete(cocos2d::Node *sender, void *data);
    void changeAccount(std::string name, std::string password);
    void onChangeAccountComplete(cocos2d::Node *sender, void *data);
    void changeName(std::string name);
    void onChangeNameComplete(cocos2d::Node *sender, void *data);
    void getAllUserData();
    void saveUserData(std::string data);
    void onSaveUserData(cocos2d::Node *sender, void *data);
    void getUserData(std::string data);
    void onGetUserDataComplete(cocos2d::Node *sender, void *data);
    void getOtherUserData(std::string data);
    void onGetOtherUserDataComplete(cocos2d::Node *sender, void *data);
    void getTopPlayers(int count);
    void onGetTopPlayersComplete(cocos2d::Node *sender, void *data);
    int topPlayerCount;
    void addShield(int hours);
    void onAddShieldComplete(cocos2d::Node *sender, void *data);
    void verifyReceipt(std::string strSignedData, std::string strSignature);
    void onVerifyReceiptComplete(cocos2d::Node *sender, void *data);
    void checkPlayID(std::string strID);
    void onCheckPlayID(cocos2d::Node *sender, void *data);
    std::string strOtherUserEquip="";
    std::string strOtherUserRuneEquip="";
    std::string strOtherUserResearch="";
    std::string strOtherUserArmor="";
    int otherUserAvatar = 0;
    int otherUserTrophy = 0;
    bool isFailedToGetNetworkData = false;
    std::string strOtherUserName = "";
    double firstReceivedTime = 0;
    double firstLocalTime = 0;
    
    void sendPost(std::string method, std::string requestData, cocos2d::network::SEL_HttpResponse pSelector);
    void sendGet(std::string method, cocos2d::network::SEL_HttpResponse pSelector);
    
//    double getTimeFromStr(std::string strTime);
    std::string getStrFromTime(time_t timet);
    int getIntIfUndefineZero(std::string str);
    
    std::string resultMessage;
    void findMatch(int trophy);
    void onFindMatches(cocos2d::Node *sender, void *data);
    void onActionNotNeededComplete(cocos2d::Node *sender, void *data);
    void getMyRank(int trophy);
    void onGetMyRankComplete(cocos2d::Node *sender, void *data);
    void isThisAppFree();
    void onIsThisAppFree(cocos2d::Node *sender, void *data);
    
    void getArenaRank();
    void onGetArenaComplete(cocos2d::Node *sender, void *data);
    void sendArenaScore(int score);
    void onSENDArenaScoreComplete(cocos2d::Node *sender, void *data);
    void getArenaReward();
    void onGetArenaRewardComplete(cocos2d::Node *sender, void *data);
    void banMe();
    
    int errorCode = -1;
    std::string errorDetail="errNotSet";
    bool isBannedUser = false;
    
    int gemLimit = 100000;
    int goldLimit = 1000000;
    int treeLimit = 1000000;
    
    int getTimeLeftToNewDay();
    void saveUserData(std::vector<int>& datas);
    
    void getGameInfo();
    void onGetGameInfoComplete(cocos2d::Node *sender, void *data);
    void getRewardInfo();
    void onGetRewardInfoComplete(cocos2d::Node *sender, void *data);
    
    int getTimeLeftToSunday();
    bool shouldCheckPvp6Result = true;
    bool shouldGetPvp6Info = true;
    void getPvp6ResultAndTicket();
    void onGetPvp6ResultAndTicketComplete(cocos2d::Node *sender, void *data);
    void getPvp6Rank();
    void onGetPvp6RankComplete(cocos2d::Node *sender, void *data);
    void findMatchForPvp6();
    void onFindMatchForPvp6Complete(cocos2d::Node *sender, void *data);
    void sendPvp6Result(int trophy);
    void onSendPvp6ResultComplete(cocos2d::Node *sender, void *data);
    void rename(std::string strName);
    void onRenameComplete(cocos2d::Node *sender, void *data);
    
    bool shouldCheckPvp12Result = true;
    bool shouldGetPvp12Info = true;
    void getPvp12ResultAndTicket();
    void onGetPvp12ResultAndTicketComplete(cocos2d::Node *sender, void *data);
    void getPvp12Rank();
    void onGetPvp12RankComplete(cocos2d::Node *sender, void *data);
    void findMatchForPvp12();
    void onFindMatchForPvp12Complete(cocos2d::Node *sender, void *data);
    void sendPvp12Result(int trophy);
    void onSendPvp12ResultComplete(cocos2d::Node *sender, void *data);
    std::string savedRID = "";
    int pvpTicketMax = 3;
    int pvp6TicketCount = 0;
    std::string pvp6TicketRefillStartTime = "0";
    int pvp12TicketCount = 0;
    std::string pvp12TicketRefillStartTime = "0";
    
    
    int pvpTargetTrophy = 1000;
    std::string pvpTargetData;
    std::string pvpTargetName;
    std::string getIDForPost();
    int month = 0;
    int day = 0;
    bool isCheckServerComplete = false;
    
    bool isThereSomethingToSaveForMonthlyEventProgress = false;
    bool shouldSaveGold = false;
    bool shouldSaveLumber = false;
    bool shouldSaveGem = false;
    void saveCheckedData();
    bool isInitUserDataRequested = false;
    bool isInitUserDataReceived = false;
    
    void checkServer();
    void onCheckServerCompleted(Node *sender, void *data);
    std::string postBoxConent = "";
    int getLastDayOfMonth(int month);
    bool isOffline = true;
    
    void uploadMap(std::string name, std::string mapData);
    int uploadState = -1; //-1 none, 0 err, 1 fail, 2 success, 3 same name exist
    void onUploadMapCompleted(Node *sender, void *data);
    
    void deleteMap(std::string mapID);
    void onDeleteMapCompleted(Node *sender, void *data);
    
    
    void downloadMapList(int index); // 0 recent, 1 by trophy, 2 by like
    int downloadMapListState = -1; //-1 none, 0 err, 1 fail, 2 success
    std::string mapList;
    void onDownloadMapListCompleted(Node *sender, void *data);
    
    void downloadMap(std::string mapID);
    int downloadMapState = -1; //-1 none, 0 err, 1 fail, 2 success
    std::string mapData;
    void onDownloadMapCompleted(Node *sender, void *data);
    
    void uploadCustomMapResult(std::string mapID, int like, int dislike, int success, int fail);
    void onUploadCustomMapResultCompleted(Node *sender, void *data);
    
    void resetMapLike();
    void onResetMapLikeCompleted(Node *sender, void *data);
};

#endif /* defined(__LegendDaryKakao__BuggyServerManager__) */


