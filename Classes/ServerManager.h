//
//  ServerManager.h
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#ifndef __LegendDaryKakao__ServerManager__
#define __LegendDaryKakao__ServerManager__

#include <iostream>
#include <time.h>
#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "json/document.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"


//#include "Title.h"

class ServerManager: public cocos2d::Layer
{
private:
    //Constructor
    ServerManager();
    
    //Instance of the singleton
    static ServerManager* m_mySingleton;
   
    struct tm receivedTime;
    struct tm receivedYesterdayTime;
public:
    bool getLocalTime = false;
    time_t startLocalTime;
    double webTime;
    bool timeEstablished;
    bool isTrying;
    int retryCount=0;
    double getCurrentTime();
    const char* getRemainTime(double targetTime);
    static ServerManager* getInstance();
    
    virtual bool init();
    CREATE_FUNC(ServerManager);
    void getHttpTime();
    
    void onHttpRequestCompleted(cocos2d::Node *sender, void *data);
    void updateTick(float dt);
    
    void onLoadGameUserInfoErrorComplete(char const *status, char const *error);
    void onLoadGameUserInfoComplete();
    
    int getDay();
    int getMonth();
    int getYear();
    
    int getYesterdayDay();
    int getYesterdayMonth();
    int getYesterdayYear();
    int getMonthIndex( std::string name );
    
    void sendColosseumScore(float time, int monsterKill, int sword, std::string playerId, std::string playerName, int league, int result);
    void onColosseumScoreCompleted(cocos2d::Node *sender, void *data);
    
    float dataSyncTimer = 5;
    float dataSyncTime = 5;
    
    void getColosseumRank(int league, int leagueIndex);
    void onColosseumRankCompleted(cocos2d::Node *sender, void *data);
    bool isRankInfoReceived = false;
    
    rapidjson::Document getDocument(cocos2d::Node *sender, void *data);
    
    bool isServerConnected = false;
    std::string serverUrl;
    
    void forceResultLeague();
    
    void setUserInfo(std::string userData);
    void onSetUserCompleted(cocos2d::Node *sender, void *data);
    void getUserInfo(std::string userID);
    void onGetUserInfoCompleted(cocos2d::Node *sender, void *data);
    void updateMessageBox(std::string playerId, std::string msgBox);
    void onUpdateMessageBox(cocos2d::Node *sender, void *data);
    
    void isThisAppFree(std::string url);
    void onIsThisAppFree(cocos2d::Node *sender, void *data);
};

#endif /* defined(__LegendDaryKakao__ServerManager__) */


