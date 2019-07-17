//
//  ServerManager.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#include "ServerManager.h"
#include "GameManager.h"
#include "Title.h"
//#include "HttpsManager.h"
#include "json/document.h"

//#include "Kakao/Common/GameUserInfo.h"
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#include "UrlJni.h"
//#endif

using namespace cocos2d;
using namespace cocos2d::network;
ServerManager* ServerManager::m_mySingleton = NULL;

ServerManager::ServerManager()
{
    timeEstablished = false;
    isTrying = false;
}

ServerManager* ServerManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = ServerManager::create();
        m_mySingleton->webTime = -1;
        m_mySingleton->retain();
        //        m_mySingleton->titleLayer = (TitleScene*)GameManager::sharedGameManager()->getTitleLayer();
    }
    
    return m_mySingleton;
}
bool ServerManager::init()
{
    Layer::init();
//    serverUrl = "http://118.32.41.70:8091";
//    serverUrl = "http://192.168.123.165:8091";
    serverUrl = "http://222.120.115.95:8101";
    
    return true;
}

void ServerManager::onLoadGameUserInfoErrorComplete(char const *status, char const *error) {
    //    CCMessageBox("네트워크 상태를 확인해주세요", "게임 사용자 정보를 불러올 수 없습니다.");
    //    log("onLoadGameUserInfoErrorComplete : %s, %s", status, error);
}
void ServerManager::onLoadGameUserInfoComplete() {
    
    timeEstablished = true;
    //    webTime = GameUserInfo::getInstance()->serverTime;
    log("web time: %d", (int)webTime);
    //    time_t startTime;
    struct tm * timeinfo;
    time (&startLocalTime);
    timeinfo = localtime (&startLocalTime);
}
void ServerManager::getHttpTime()
{
    if (getLocalTime) {
        time_t currentLocalTime;
        struct tm * timeinfo;
        time (&currentLocalTime);
        timeinfo = localtime (&currentLocalTime);
        
        double theTime = currentLocalTime;
        webTime = currentLocalTime;
        timeEstablished = true;
        
        uint64_t yesterdayTimeInInt = theTime - 60*60*24;
        time_t yesterdayNow;
        yesterdayNow = yesterdayTimeInInt;
        receivedYesterdayTime = *localtime(&yesterdayNow);
        
        return;
    }
    isTrying = true;
    timeEstablished = false;
    HttpRequest* request = new HttpRequest();
    //    request->setUrl("http://www.timeapi.org/utc/now.json");
    //    request->setUrl("https://fstimeserver.appspot.com/time");
//    request->setUrl("http://1-dot-fsjavatime.appspot.com/fsfreetime");
    request->setUrl(strmake("%s/time", serverUrl.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onHttpRequestCompleted));
    request->setTag("Http time from fsfreetime");
    HttpClient::getInstance()->send(request);
    request->release();
}
int ServerManager::getMonthIndex( std::string name )
{
    std::map<std::string, int> months
    {
        { "Jan", 1 },
        { "Feb", 2 },
        { "Mar", 3 },
        { "Apr", 4 },
        { "May", 5 },
        { "Jun", 6 },
        { "Jul", 7 },
        { "Aug", 8 },
        { "Sep", 9 },
        { "Oct", 10 },
        { "Nov", 11 },
        { "Dec", 12 },
        { "jan", 1 },
        { "feb", 2 },
        { "mar", 3 },
        { "apr", 4 },
        { "may", 5 },
        { "jun", 6 },
        { "jul", 7 },
        { "aug", 8 },
        { "sep", 9 },
        { "oct", 10 },
        { "nov", 11 },
        { "dec", 12 }
    };
    
    const auto iter = months.find( name );
    
    if( iter != months.cend() )
        return iter->second;
    return -1;
}
void ServerManager::onHttpRequestCompleted(Node *sender, void *data)
{
    log("time complete request http");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string resposeData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    
    if (!response->isSucceed())
    {
        log("%s", response->getErrorBuffer());
        if(TITLE != nullptr){
            TITLE->isTimeGetFailed = true;
        }
        return;
    }
    //    response->getResponseHeader()
    
    //    int countryIndex = (int)resposeData.find("google.co.");
    //    std::string countryStr = resposeData.substr(countryIndex + 10, 2);
    //    countryCode = countryStr;
    //    if (countryIndex < 0) {
    //        countryCode = "kr";
    //    }
    log("resposeData: %s", resposeData.c_str());
    
    std::string year = Value(resposeData.substr(0, 4)).asString();
    std::string month = Value(resposeData.substr(5, 2)).asString();
    std::string date = Value(resposeData.substr(8, 2)).asString();
    //    std::string timeStr = Value(resposeData.substr(0, 4)).asString();
    
    //    ValueVector timeVector = GameManager::getInstance()->split(timeStr, ":");
    //    log("timeVector size %d, %s, %s", (int)timeVector.size(), timeVector.at(0).asString().c_str(), timeStr.c_str());
    std::string hour = Value(resposeData.substr(11, 2)).asString();
    std::string min = Value(resposeData.substr(14, 2)).asString();
    std::string sec = Value(resposeData.substr(17, 2)).asString();
    
    struct tm theTime;
    theTime.tm_year = Value(year).asInt()-1900;
    theTime.tm_mon = Value(getMonthIndex(month)).asInt();
    theTime.tm_mday = Value(date).asInt();
    theTime.tm_hour = Value(hour).asInt();
    theTime.tm_min = Value(min).asInt();
    theTime.tm_sec = Value(sec).asInt();
    time_t what = mktime(&theTime);
    double tempWebTime = what + 32400;
    double tick = tempWebTime;
    
    webTime = tempWebTime;
    log("** web time: %ld", tempWebTime);
    time_t now;
    now = webTime;
//    startLocalTime = webTime;
    receivedTime = *localtime(&now); // 지역표준시로 변환한다 (대한민국은 KST)
    //ts = gmtime(&now);  // 국제표준시 GMT로 변환한다
    //    struct tm etet = *localtime(&
    uint64_t yesterdayTimeInInt = tick - 60*60*24;
    time_t yesterdayNow;
    yesterdayNow = yesterdayTimeInInt;
    receivedYesterdayTime = *localtime(&yesterdayNow);
    
    struct tm * timeinfo;
    time (&startLocalTime);
    timeinfo = localtime (&startLocalTime);
    
    //    log("http time-> %d/%d/%d %d:%d:%d", receivedTime->tm_year,receivedTime->tm_mon, receivedTime->tm_mday, receivedTime->tm_hour, receivedTime->tm_min, receivedTime->tm_sec);
    timeEstablished = true;
    /*
     //    printf("Http Test, dump data: ");
     //char numberInChar[12];
     int startIndex = -1;
     int endIndex = -1;
     for (unsigned int i = 0; i < buffer->size(); i++)
     {
     printf("%c", (*buffer)[i]);
     if ((*buffer)[i] == ':') {
     startIndex = i+1;
     }else if((*buffer)[i] == '}'){
     endIndex = i;
     }
     }
     long tick = 0;
     int num = 0;
     long p = 0;
     if (endIndex - startIndex > 10) {
     startIndex = endIndex - 10;
     }
     for (int i = startIndex; i < endIndex; i++) {
     num = ((int)((*buffer)[i]) - 48);
     //log("num: %d", num);
     p = pow(10, endIndex - i -1);
     //log("p: %d", p);
     tick += num*(p);
     }
     timeEstablished = true;
     webTime = tick;
     log("web time: %ld", webTime);
     //    time_t startTime;
     struct tm * timeinfo;
     time (&startLocalTime);
     timeinfo = localtime (&startLocalTime);*/
}

double ServerManager::getCurrentTime(){
    time_t currentLocalTime;
    struct tm * timeinfo;
    time (&currentLocalTime);
    timeinfo = localtime (&currentLocalTime);
    if (getLocalTime) {
        return currentLocalTime;
    }
    //    log("** get current time web: %ld, startLocalTime: %ld, currentLocalTime: %ld", webTime, startLocalTime, currentLocalTime);
    return webTime + (currentLocalTime - startLocalTime);
    //    return GameUserInfo::getInstance()->serverTime + (currentLocalTime - startLocalTime);
}

int ServerManager::getDay(){
    return receivedTime.tm_mday;
}
int ServerManager::getMonth(){
    return receivedTime.tm_mon;
}
int ServerManager::getYear(){
    return receivedTime.tm_year;
}

int ServerManager::getYesterdayDay(){
    return receivedYesterdayTime.tm_mday;
}
int ServerManager::getYesterdayMonth(){
    return receivedYesterdayTime.tm_mon;
}
int ServerManager::getYesterdayYear(){
    return receivedYesterdayTime.tm_year;
}



void ServerManager::sendColosseumScore(float time, int monsterKill, int sword, std::string playerId, std::string playerName, int league, int result){
    int timeInSecond = time*100;
    std::string requestData =
    "id=" + playerId + "&" +
    "name=" + playerName + "&" +
    "time=" + Value(timeInSecond).asString() + "&" +
    "kill=" + Value(monsterKill).asString() + "&" +
    "result=" + Value(result).asString() + "&" +
    "weapon=" + Value(sword).asString() + "&" +
    "league=" + Value(league).asString();
    
    log("sm request: %s", requestData.c_str());
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/api/leagues", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onColosseumScoreCompleted));
    // write the post data
    const char* postData = requestData.c_str();
    request->setRequestData(requestData.c_str(), strlen(postData));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onColosseumScoreCompleted(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerConnected = false;
//        WORLD->sendColosseumScore();
        return;
    }
    if (document.HasMember("error")) { // error
        log("sm colosseum score error");
//        WORLD->sendColosseumScore();
        
    }else { //
        WORLD->onSendComplete(document["result"].GetInt());
    }
}
void ServerManager::getColosseumRank(int league, int leagueIndex){
    isRankInfoReceived = false;
    HttpRequest* request = new HttpRequest();
    std::string leagueData = strmake("%d_%d", league, leagueIndex);
    request->setUrl(strmake("%s/api/leagues/%s", serverUrl.c_str(), leagueData.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onColosseumRankCompleted));
    HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onColosseumRankCompleted(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerConnected = false;
        // should display error message and tell your to try again or quit
        log("sm colosseum get rank error");
        if(TITLE != nullptr){
            TITLE->isGetRankFailed = true;
        }
        return;
    }
    if(document.IsArray()){
        if(TITLE != nullptr){
//            for(int i = document.GetArray().Size()-1; i >= 0; i--){
            for(int i = 0; i < document.GetArray().Size(); i++){
                TITLE->colosseumRankInfoName.push_back(document[i].GetObject()["name"].GetString());
                TITLE->colosseumRankInfoResult.push_back(document[i].GetObject()["result"].GetInt());
                TITLE->colosseumRankInfoTime.push_back(document[i].GetObject()["time"].GetInt());
                TITLE->colosseumRankInfoKill.push_back(document[i].GetObject()["kill"].GetInt());
                TITLE->colosseumRankInfoWeapon.push_back(document[i].GetObject()["weapon"].GetInt());
            }
            TITLE->isColosseumRankInfoReceived = true;
        }
    }else if (document.HasMember("error")) { // error
        log("sm colosseum get rank error");
        std::string msg = "league not found";
        if(std::string("league not found").compare(document["error"].GetString()) == 0){
            log("league not found");
            if(TITLE != nullptr){
                TITLE->isLeagueNotFound = true;
            }
        }
        // should display error message and tell your to try again or quit
    } else { //
//        log("league 0: %s", document[0].GetString());
        
        //        int rank = document["rank"].GetInt();
        //        WORLD->colosseum->onRankReceived(rank);
    }
}
rapidjson::Document ServerManager::getDocument(cocos2d::Node *sender, void *data){
    log("sm request complete");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string resposeData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    if (!response->isSucceed())
    {
        log("sm %s", response->getErrorBuffer());
        
        return nullptr;
    }
    
    log("sm resposeData: %s", resposeData.c_str());
    
    std::vector<char> *buffer = response->getResponseData();
    if (buffer->size() > 0) {
        std::string res;
        res.insert(res.begin(), buffer->begin(), buffer->end());
        log("sm ** res %s", res.c_str());
        //cocos2d::MessageBox(__String::createWithFormat("** res %s", res.c_str())->getCString(), "result");
        
        rapidjson::Document document;
        if(document.Parse(res.c_str()).HasParseError()){
            log("sm ** https json parsing error");
            //            isNetworkFail = true;
            return nullptr;
        }
//        log("league is array: %d", document.IsArray());
//        log("league kill: %d", document[0].GetObject()["kill"].GetInt());
        return document;
    }
    return nullptr;
}
void ServerManager::forceResultLeague(){
    HttpRequest* request = new HttpRequest();
    request->setUrl(strmake("%s/forceResultLeague", serverUrl.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    HttpClient::getInstance()->send(request);
    request->release();
}

void ServerManager::getUserInfo(std::string userID){
    log("sm getUserInfo starts");
    HttpRequest* request = new HttpRequest();
    request->setUrl(strmake("%s/api/users/%s", serverUrl.c_str(), userID.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onGetUserInfoCompleted));
    HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::setUserInfo(std::string userData){
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/api/users", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onSetUserCompleted));
    // write the post data
    const char* postData = userData.c_str();
    request->setRequestData(userData.c_str(), strlen(postData));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onSetUserCompleted(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
//        this->isServerConnected = false;
        //        WORLD->sendColosseumScore();
        if(TITLE != nullptr){
            TITLE->onCreateUserFailed();
        }
        return;
    }
    if(TITLE != nullptr){
        if (document.HasMember("error")) { // error
            log("sm onSetUserCompleted error");
            //        WORLD->sendColosseumScore();
            TITLE->onCreateUserFailed();
        }else { //
            TITLE->onCreateUserCreated();
        }
    }
}
void ServerManager::onGetUserInfoCompleted(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerConnected = false;
        // should display error message and tell your to try again or quit
        log("sm get user info error");
        if(TITLE != nullptr){
            TITLE->isGetRankFailed = true;
        }
        return;
    }
    if (document.HasMember("error")) { // error
//        log("sm colosseum get rank error");
//        std::string msg = "league not found";
        if(TITLE != nullptr){
            if(std::string("user not found").compare(document["error"].GetString()) == 0){
                TITLE->onUserInfoNotFound();
            }else{
                TITLE->onUserInfoFailed();
            }
        }
        // should display error message and tell your to try again or quit
        
    } else { //
        
//        log("league 0: %s", document[0].GetString());
        
        //        int rank = document["rank"].GetInt();
        //        WORLD->colosseum->onRankReceived(rank);
        if(TITLE != nullptr){
            TITLE->strMessageBox = document["messageBox"].GetString();
        }
    }
}
void ServerManager::updateMessageBox(std::string playerId, std::string msgBox){
    std::string requestData =
    "id=" + playerId + "&" +
    "messageBox=" + msgBox;
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/api/users/%s", serverUrl.c_str(), playerId.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::PUT);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onUpdateMessageBox));
    // write the post data
    const char* postData = requestData.c_str();
    request->setRequestData(requestData.c_str(), strlen(postData));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onUpdateMessageBox(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerConnected = false;
        // should display error message and tell your to try again or quit
        log("sm onUpdateMessageBox error");
        if(TITLE != nullptr){
            TITLE->isGetRankFailed = true;
        }
        return;
    }
    if (document.HasMember("error")) { // error
        log("update error");
    } else { //
        log("update complete");
    }
}
void ServerManager::isThisAppFree(std::string url){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(url);
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(ServerManager::onIsThisAppFree));
    // write the post data
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void ServerManager::onIsThisAppFree(cocos2d::Node *sender, void *data){
    log("sm request complete");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string resposeData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    
    log("sm resposeData: %s", resposeData.c_str());
    if(resposeData.find("<meta itemprop=\"price\" content=\"2000\"/>") != std::string::npos){
        log("price 2000 found (admob for tag)");
    }else if(resposeData.find("<meta itemprop=\"price\" content=\"0\"/>") != std::string::npos){
        log("price 0 found (admob for tag)");
        TITLE->isFreeApp = true;
    }else{
        log("price not found (admob for tag)");
        TITLE->isFreeApp = false;
    }
//    TITLE->isFreeApp = true; // test
    TITLE->isFreeAppResultArrived = true;
    log("should handle free");
}
