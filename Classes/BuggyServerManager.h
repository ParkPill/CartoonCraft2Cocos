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

//#define DATA_KEY_LEVEL 

#define SET_DOCUMENT_AND_CHECK_ERROR rapidjson::Document document = getDocument(sender, data); if(document.IsNull()){ this->isServerFailed = true; return; } if (document.HasMember("error")) { log("server data error: %s", document["error"].GetString()); isFailedToGetNetworkData = true;}

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
    
    void onHttpRequestCompleted(cocos2d::Node *sender, void *data);
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
};

#endif /* defined(__LegendDaryKakao__BuggyServerManager__) */


