//
//  BuggyServerManager.cpp
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 2. 14..
//
//

#include "BuggyServerManager.h"
#include "GameManager.h"
#include "Title.h"
//#include "HUD.h"
//#include "HttpsManager.h"
#include "json/document.h"
#include <ctime>
#include <iomanip>
#include <sstream>
//#include <time.h>
//#include "Kakao/Common/GameUserInfo.h"
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#include "UrlJni.h"
//#endif

using namespace cocos2d;
using namespace cocos2d::network;
BuggyServerManager* BuggyServerManager::m_mySingleton = NULL;

BuggyServerManager::BuggyServerManager()
{
    timeEstablished = false;
    isTrying = false;
}

BuggyServerManager* BuggyServerManager::getInstance()
{
    if(NULL == m_mySingleton)
    {
        m_mySingleton = BuggyServerManager::create();
        m_mySingleton->webTime = -1;
        m_mySingleton->retain();
        //        m_mySingleton->HUDLayer = (HUDScene*)GameManager::sharedGameManager()->getHUDLayer();
    }
    
    return m_mySingleton;
}
bool BuggyServerManager::init()
{
    Layer::init();
    receivedWebTimeT = time(0);
//    serverUrl = "http://118.32.41.70:8091";
//    serverUrl = "http://192.168.123.165:8091";
//    serverUrl = "http://222.120.115.95:8101"; // cartoon craft server
//    serverUrl = "http://222.120.115.95:8102"; // buggyland server
    
//    serverUrl = "http://localhost:8103"; // cartoon-new server test
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//    serverUrl = "http://192.168.0.2:8103"; // cartoon-new server test
#endif
    
//    serverUrl = "http://222.120.115.95:8103"; // test
    serverUrl = "http://54.180.92.125:8103"; // aws
    
//    gameName = "cartooncraft";
    apiName = "cc";
    return true;
}

void BuggyServerManager::getHttpTime()
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
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onHttpRequestCompleted));
    request->setTag("Http time from fsfreetime");
    HttpClient::getInstance()->send(request);
    request->release();
}
int BuggyServerManager::getMonthIndex( std::string name )
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
static constexpr time_t const NULL_TIME = -1;
long tz_offset(time_t when = NULL_TIME)
{
    if (when == NULL_TIME)
        when = std::time(nullptr);
    auto const tm = *std::localtime(&when);
    std::ostringstream os;
    os << std::put_time(&tm, "%z");
    std::string s = os.str();
    // s is in ISO 8601 format: "±HHMM"
    int h = std::stoi(s.substr(0,3), nullptr, 10);
    int m = std::stoi(s[0]+s.substr(3), nullptr, 10);
    
    return h * 3600 + m * 60;
}
void BuggyServerManager::onHttpRequestCompleted(Node *sender, void *data)
{
    log("time complete request http");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
//    int size = (int)response->getResponseHeader()->size();
    std::string responseData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    
    if (!response->isSucceed())
    {
        log("!isSucceed %s", response->getErrorBuffer());
        if(HUD != nullptr){
//            HUD->isTimeGetFailed = true;
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
    log("resposeData: %s", responseData.c_str());
//    time_t currentLocalTime;
//    struct tm * timeinfo;
//    time (&currentLocalTime);
//    timeinfo = localtime (&currentLocalTime);
//    double theTime = currentLocalTime;
    
    setTime(responseData);
    
//    if(firstReceivedTime == 0){
//        setTime(responseData);
//        firstReceivedTime = webTime;
//        firstLocalTime = theTime;
//    }else{
//        double tempWebTime = getTimeFromStr(responseData);
//        int serverTimeDiff = tempWebTime - firstReceivedTime;
//
//        int localTimeDiff = theTime - firstLocalTime;
//        log("time diff %d/%d", serverTimeDiff, localTimeDiff);
//        timeEstablished = true;
//        if (serverTimeDiff*2 < localTimeDiff) {
//            banMe();
//        }
//    }
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

std::string BuggyServerManager::getStrFromTime(time_t timet){
    std::time_t now = std::time(NULL);
    std::tm * ptm = std::localtime(&now);
    char bufferr[32];
    // Format: Mo, 15.06.2009 20:20:00
    std::strftime(bufferr, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    
    struct tm * timeinfo;
    timeinfo = localtime (&timet);
    
    char buffer [19];
    strftime (buffer,19,"%F %X", timeinfo);
    return std::string(buffer);
}
time_t BuggyServerManager::getTimeTFromStr(std::string strTime){
    if(strTime.size() < 19){
        return receivedWebTimeT; // test 
    }
    std::string year = Value(strTime.substr(0, 4)).asString();
    std::string month = Value(strTime.substr(5, 2)).asString();
    std::string date = Value(strTime.substr(8, 2)).asString();
    
    std::string hour = Value(strTime.substr(11, 2)).asString();
    std::string min = Value(strTime.substr(14, 2)).asString();
    std::string sec = Value(strTime.substr(17, 2)).asString();
    
    struct tm theTime;
    theTime.tm_year = Value(year).asInt()-1900;
    theTime.tm_mon = Value(month).asInt()-1;
    theTime.tm_mday = Value(date).asInt();
    theTime.tm_hour = Value(hour).asInt();
    theTime.tm_min = Value(min).asInt();
    theTime.tm_sec = Value(sec).asInt();
    theTime.tm_isdst = 0;
    
    time_t what = mktime(&theTime); // UTC time - 1 hour can diff for summertime
    return what;
}
//double BuggyServerManager::getTimeFromStr(std::string strTime){
//    time_t what = getTimeTFromStr(strTime);//mktime(&theTime); // UTC time - 1 hour can diff for summertime
//    double tempWebTime = what;
//    tempWebTime += tz_offset(); // + local timezone diff
//    return tempWebTime;
//}return p;
//}
void BuggyServerManager::setTime(std::string strTime){
    receivedWebTimeT = getTimeTFromStr(strTime);
//    log("now: %s", strTime.c_str());
    
    
//    double tempWebTime = getTimeFromStr(strTime);
//    double tick = tempWebTime;
//    webTime = receivedWebTimeT;//tempWebTime;
//    log("** web time: %lf", receivedWebTimeT);
//    time_t now;
//    now = webTime;
    
//    receivedTime = *localtime(&now); // 지역표준시로 변환한다 (대한민국은 KST)
    
//    log("now2: %s", getStrFromTime(receivedWebTimeT).c_str());
    
    //ts = gmtime(&now);  // 국제표준시 GMT로 변환한다
    //    struct tm etet = *localtime(&
    uint64_t yesterdayTimeInInt = receivedWebTimeT - 60*60*24;
    time_t yesterdayNow;
    yesterdayNow = yesterdayTimeInInt;
    receivedYesterdayTime = *localtime(&yesterdayNow);

    startLocalTime = time(0);
//    log("now3: %s", getStrFromTime(receivedWebTimeT).c_str());
//    log("now4: %s", getStrFromTime(getCurrentTimeT()).c_str());
//    struct tm * timeinfo;
//    time (&startLocalTime);
//    timeinfo = localtime (&startLocalTime);
//    startLocalTimeDouble = startLocalTime;
    
    //    log("http time-> %d/%d/%d %d:%d:%d", receivedTime->tm_year,receivedTime->tm_mon, receivedTime->tm_mday, receivedTime->tm_hour, receivedTime->tm_min, receivedTime->tm_sec);
    timeEstablished = true;
}
time_t BuggyServerManager::getCurrentTimeT(){
    time_t currentLocalTime = time(0);
    double timeDiff = difftime(currentLocalTime, startLocalTime);
    
//    receivedWebTimeT = getTimeTFromStr(strTime);
//    log("now: %s, diff: %lf", getStrFromTime(receivedWebTimeT).c_str(), timeDiff);
    
    return receivedWebTimeT + timeDiff;
}
//double BuggyServerManager::getCurrentTime(){
//    if(!timeEstablished){
//        log("internet not connected");
//    }
//    time_t currentLocalTime;
//    struct tm * timeinfo;
//    time (&currentLocalTime);
//    timeinfo = localtime (&currentLocalTime);
//    double currentLocalTimeDouble = currentLocalTime;
//    if (getLocalTime) {
//        return currentLocalTime;
//    }
////        log("** get current time web: %lf, startLocalTime: %lf, currentLocalTime: %lf", webTime, startLocalTimeDouble, currentLocalTimeDouble);
//    return webTime + (currentLocalTimeDouble - startLocalTimeDouble);
//    //    return GameUserInfo::getInstance()->serverTime + (currentLocalTime - startLocalTime);
//}

int BuggyServerManager::getDay(){
    return receivedTime.tm_mday;
}
int BuggyServerManager::getMonth(){
    return receivedTime.tm_mon;
}
int BuggyServerManager::getYear(){
    return receivedTime.tm_year;
}

int BuggyServerManager::getYesterdayDay(){
    return receivedYesterdayTime.tm_mday;
}
int BuggyServerManager::getYesterdayMonth(){
    return receivedYesterdayTime.tm_mon;
}
int BuggyServerManager::getYesterdayYear(){
    return receivedYesterdayTime.tm_year;
}

rapidjson::Document BuggyServerManager::getDocument(cocos2d::Node *sender, void *data){
    log("sm request complete");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
//    int size = (int)response->getResponseHeader()->size();
    std::string resposeData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    if (!response->isSucceed())
    {
        log("sm %s", response->getErrorBuffer());
        errorDetail = response->getErrorBuffer();
        this->isServerFailed = true;
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
            errorDetail = "https json parsing error";
            return nullptr;
        }
//        log("league is array: %d", document.IsArray());
//        log("league kill: %d", document[0].GetObject()["kill"].GetInt());
        return document;
    }
    return nullptr;
}

void BuggyServerManager::sendPost(std::string method, std::string requestData, SEL_HttpResponse pSelector){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/%s/%s/", serverUrl.c_str(),apiName.c_str(), method.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, pSelector);
    
    // write the post data
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::getPostBoxItem(){
    sendPost("getuserdata", "id=" + requestedID + "&" + "post=1", httpresponse_selector(BuggyServerManager::onGetPostBoxItem));
}
void BuggyServerManager::onGetPostBoxItem(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("post")) {
//        HUD->postBoxTextFromServer = document["post"].GetString();
//        UDSetStr(KEY_POST_MESSAGES, document["post"].GetString());
//        HUD->networkStateSendData = NETWORK_HANDLE_STATE_ARRIVED;
//        HUD->updatePostBox();
    }else{
//        HUD->onServerFailed();
    }
}
void BuggyServerManager::saveUserData(std::string data){
    if (GM->getGem() > gemLimit || GM->getCoin() > goldLimit || GM->getTree() > treeLimit) {
//        isBannedUser = true;
//        banMe();
    }else{
        sendPost("saveuser", "id=" + requestedID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
    }
}
void BuggyServerManager::onSaveUserData(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("error")) {
        if (HUD) {
//            HUD->onServerFailed();
        }
    }else{
        if (TITLE) {
            TITLE->hideIndicator();
            if (TITLE->isNameRequested) {
                TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
            }
        }
        if(HUD){
            HUD->hideIndicator();
        }

//        if (MOBILE_TITLE) {
//            MOBILE_TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
//        }else{
//            if(HUD){
//                HUD->networkStateSendData = NETWORK_HANDLE_STATE_ARRIVED;
//            }
//        }
    }
}
void BuggyServerManager::findMatch(int trophy){
    sendPost("findmatch", "id=" + requestedID + "&" + strmake("trophy=%d", trophy), httpresponse_selector(BuggyServerManager::onFindMatches));
}
void BuggyServerManager::onFindMatches(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("error")) {
        log("error: %s", document["error"].GetString());
        GM->raidMatchState = NETWORK_HANDLE_STATE_ERROR;
    }else{
        GM->raidEnemyID = document["id"].GetString();
        GM->raidEnemyName = document["name"].GetString();
        GM->raidEnemyTrophy = document["trophy"].GetInt();
        GM->raidEnemyGold = document["gold"].GetInt()*0.3f;
        GM->raidEnemyTree = document["tree"].GetInt()*0.3f;
        if(document.HasMember("deck")){
            GM->raidEnemyDeck = document["deck"].GetString();
        }else{
            GM->raidEnemyDeck = "";
        }
        
        GM->raidEnemyBuildings = document["buildings"].GetString();
        GM->raidMatchState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::onActionNotNeededComplete(cocos2d::Node *sender, void *data){
//    SET_DOCUMENT_AND_CHECK_ERROR
}
void BuggyServerManager::setPostBoxItem(std::string items){
    sendPost("saveuser", "id=" + requestedID + "&" + "post=" + items, httpresponse_selector(BuggyServerManager::onGetPostBoxItem));
}
void BuggyServerManager::onSetPostBoxItem(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("post")) {
        
    }else{
        
    }
}
void BuggyServerManager::registerName(std::string name){
    sendPost("saveuser", "id=" + requestedID + "&" + "name=" + name, httpresponse_selector(BuggyServerManager::onRegisterNameComplete));
}
void BuggyServerManager::onRegisterNameComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("result")) {
        log("saved id: %s", document["result"].GetString());
        requestedID = document["result"].GetString();
        UDSetStr(KEY_SAVED_ID, requestedID);
        resultMessage = requestedID;
    }else{
        resultMessage = document["error"].GetString();
    }
    TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
}
void BuggyServerManager::changeAccount(std::string name, std::string password){
    sendPost("userlogin", strmake("name=%s&password=%s", name.c_str(), password.c_str()), httpresponse_selector(BuggyServerManager::onChangeAccountComplete));
}
void BuggyServerManager::onChangeAccountComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 0;
        return;
    }
    bool success = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        log("login failed");
        //        return;
    }else if(document.HasMember("result")) {
        std::string result = document["result"].GetString();
        requestedID = document["result"].GetString();
        UDSetStr(KEY_SAVED_ID, requestedID);
        success = true;
    }
    
//    if(HUD){
//        HUD->isChangingAccountSuccess = success;
//        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
//    }else if(MOBILE_TITLE){
//        MOBILE_TITLE->restartTheGame();
//    }
    TITLE->restartTheGame();
}
void BuggyServerManager::changeName(std::string name){
    sendPost("changename", strmake("id=%s&name=%s", requestedID.c_str(), name.c_str()), httpresponse_selector(BuggyServerManager::onChangeNameComplete));
}
void BuggyServerManager::onChangeNameComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 1;
        return;
    }
    bool success = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        log("login failed");
        //        return;
    }else if(document.HasMember("result")) {
        std::string result = document["result"].GetString();
        UDSetStr(KEY_NAME, document["name"].GetString());
//        if(HUD){
//            HUD->playerName = document["name"].GetString();
//        }
        success = true;
    }
//    if(HUD){
//        HUD->isChangingNameSuccess = success;
//        HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
//    }
}
void BuggyServerManager::getOtherUserData(std::string data){
    sendPost("getuserdata", data, httpresponse_selector(BuggyServerManager::onGetOtherUserDataComplete));
}
void BuggyServerManager::onGetOtherUserDataComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 2;
        return;
    }
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        //        return;
    }
//    if(document.HasMember("time")) setTime(document["time"].GetString());
    
    GM->raidEnemyID = document["id"].GetString();
    GM->raidEnemyName = document["name"].GetString();
    GM->raidEnemyTrophy = document["trophy"].GetInt();
    GM->raidEnemyGold = document["gold"].GetInt()*0.3f;
    int resourceMax = 500;
    if(GM->raidEnemyGold > resourceMax){
        GM->raidEnemyGold = resourceMax;
    }
    GM->raidEnemyTree = document["tree"].GetInt()*0.3f;
    if(GM->raidEnemyTree > resourceMax){
        GM->raidEnemyTree = resourceMax;
    }
    if(document.HasMember("shield_end")){
        std::string strTime = document["shield_end"].GetString();
        double shieldEndTime = getTimeTFromStr(strTime);
        double now = (double)getCurrentTimeT();
        if(shieldEndTime > now){
            GM->raidEnemyGold = 0;
            GM->raidEnemyTree = 0;
        }
    }
    if(document.HasMember("deck")){
        GM->raidEnemyDeck = document["deck"].GetString();
    }else{
        GM->raidEnemyDeck = "";
    }
    GM->raidEnemyBuildings = document["buildings"].GetString();
    
//    lastDocument = getDocument(sender, data);
    if(BHUD != nullptr){
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::getTopPlayers(int count){
    topPlayerCount = count;
    sendPost("findtopplayers", strmake("count=%d", count), httpresponse_selector(BuggyServerManager::onGetTopPlayersComplete));
}
void BuggyServerManager::onGetTopPlayersComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 3;
        return;
    }
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        //        return;
        
    }
    //    if(document.HasMember("time")) setTime(document["time"].GetString());
    BHUD->topRankIDs.clear();
    BHUD->topRankNames.clear();
    BHUD->topRankTrophys.clear();
    for (int i = 0; i < document["result"].GetArray().Size(); i++) {
        long trophy = document["result"].GetArray()[i]["trophy"].GetUint64();
        if(trophy > 100000){
            continue;
        }
        BHUD->topRankIDs.push_back(document["result"].GetArray()[i]["id"].GetString());
        BHUD->topRankNames.push_back(document["result"].GetArray()[i]["name"].GetString());
        BHUD->topRankTrophys.push_back((int)trophy);
    }
    //
    //    lastDocument = getDocument(sender, data);
    if(BHUD != nullptr){
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::getAllUserData(){
    sendPost("getuserdata", "id=" + requestedID +
             "&name=1"+
             "&trophy=1"+
             "&arena_clr_cnt=1"+
             "&stage_clr_idx=1"+
             "&buildings=1"+
             "&inventory=1"+
             "&deck=1"+
             "&gem=1"+
             "&gold=1"+
             "&tree=1"+
             "&arena_score=1"+
             "&arena_reward=1"+
             "&spc_units=1"+
             "&under_attack=1"+
             "&golden_ticket=1"+
             "&defence_record=1"+
             "&search_state=1"+
             "&search_items=1"+
             "&attend=1"+
             "&gift_day=1"+
             "&iap_list=1"+
             "&iap_total=1"+
             "&mission=1"+
             "&post=1"+
             "&ban=1"+
             "&access_reward_receive=1"+
             "&shield_end=1"+
             "&last_launch_date=1"+
             "&first_launch_date=1",
             httpresponse_selector(BuggyServerManager::onGetUserDataComplete));
}

void BuggyServerManager::getUserData(std::string data){
    sendPost("getuserdata", "id=" + requestedID + "&" + data.c_str(), httpresponse_selector(BuggyServerManager::onGetUserDataComplete));
}
void BuggyServerManager::onGetUserDataComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 4;
        return;
    }
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        this->isServerFailed = true;
        return;
    }
    if(document.HasMember("time")){
        setTime(document["time"].GetString());
    }
    //    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("name")) {
        UDSetStr(KEY_NAME, document["name"].GetString());
        log("name: %s", UDGetStr(KEY_NAME).c_str());
//        HUD->playerName = document["name"].GetString();
    }
    if(document.HasMember("trophy")) UDSetInt(KEY_TROPHY, document["trophy"].GetInt());
    if(document.HasMember("arena_clr_cnt")) {
        int count = document["arena_clr_cnt"].GetInt();
        int originalCount = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
        if(count > originalCount){
            UDSetInt(KEY_ARENA_CLEAR_COUNT, count);
        }
    }
    if(document.HasMember("stage_clr_idx")) {
        int index = document["stage_clr_idx"].GetInt();
        int originalIndex = UDGetInt(KEY_LAST_CLEAR_STAGE, -1);
        if(index > originalIndex){
            UDSetInt(KEY_LAST_CLEAR_STAGE, index);
        }
    }
    if(document.HasMember("spc_units")) UDSetStr(KEY_SPECIAL_UNITS, document["spc_units"].GetString());
    
    if(document.HasMember("mission")) {
        std::string str = document["mission"].GetString();
//        ValueVector list = GM->split(str, ",");
//        GM->dailyMissionComplete.clear();
//        for (int i = 0; i < 12; i++) {
//            GM->dailyMissionComplete.push_back((bool)str.at(i));
//        }
    }
    if(document.HasMember("gold")) GM->setCoin(document["gold"].GetInt());
    if(document.HasMember("tree")) GM->setTree(document["tree"].GetInt());
    if(document.HasMember("gem")) GM->setGem(document["gem"].GetInt());
    if(document.HasMember("buildings")){
        std::string str = document["buildings"].GetString();
        if(str.length() > 0){
            UDSetStr(KEY_BUILDINGS, str);
        }
    }
    if(document.HasMember("inventory")) UDSetStr(KEY_UNITS_INVENTORY, document["inventory"].GetString());
    if(document.HasMember("deck")) {
        std::string str = document["deck"].GetString();
        if(str.length() > 0){
            UDSetStr(KEY_UNITS_DECK, str);
        }
    }
    if(document.HasMember("golden_ticket")) {
//        UDSetInt(KEY_GOLDEN_TICKET, document["golden_ticket"].GetInt());
    }
    if(document.HasMember("defence_record")) UDSetStr(KEY_DEFENCE_RECORD, document["defence_record"].GetString());
    if(document.HasMember("search_state")) {
        UDSetStr(KEY_SEARCH_STATE, document["search_state"].GetString());
    }
    if(document.HasMember("search_items")) {
        UDSetStr(KEY_SEARCH_UNITS, document["search_items"].GetString());
    }
    
    std::string str;
//    if(document.HasMember("attend")) UDSetStr(KEY_ATTEND_RCV_INFO, document["attend"].GetString());
    if(document.HasMember("access_reward_receive")) {
        str = document["access_reward_receive"].GetString();
//        for(int i = 0; i < str.length(); i++){
//            if(i == 0){
//                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_AM79, Value(str.at(i)).asInt() == 1);
//            }else if(i == 1){
//                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1214, Value(str.at(i)).asInt() == 1);
//            }else if(i == 2){
//                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM1720, Value(str.at(i)).asInt() == 1);
//            }else if(i == 3){
//                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2022, Value(str.at(i)).asInt() == 1);
//            }else if(i == 4){
//                UDSetBool(KEY_ACCEESS_REWARD_RECEIVED_PM2224, Value(str.at(i)).asInt() == 1);
//            }
//        }
    }
    if(document.HasMember("gift_day")) UDSetInt(KEY_GIFT_DAY, document["gift_day"].GetInt());
    
    if(document.HasMember("iap_list")) {
        std::string strIAPList = document["iap_list"].GetString();
        UDSetStr(KEY_IAP_LIST, strIAPList);
        if(HUD && strIAPList.find("starter0") != std::string::npos){
//            HUD->isNoAdsPurchased = true;
        }
    }
    if(document.HasMember("iap_total")) {
        int total = document["iap_total"].GetInt();
        UDSetInt(KEY_IAP_TOTAL, total);
        if(total > 0){
             UDSetBool(KEY_IAP_USER, true);
        }
    }
    if(document.HasMember("post")) UDSetStr(KEY_POST_MESSAGE, document["post"].GetString());
    if(document.HasMember("ban")) isBannedUser = document["ban"].GetInt() == 1;
    if(document.HasMember("shield_end")) {
        UDSetStr(KEY_SHIELD_END_TIME, document["shield_end"].GetString());
    }
    if(document.HasMember("last_launch_date")) UDSetStr(KEY_LAST_LAUNCH_DATE, document["last_launch_date"].GetString());
    if(document.HasMember("first_launch_date")) {
        std::string strFirstDay = document["first_launch_date"].GetString();
        UDSetStr(KEY_FIRST_LAUNCH_DATE, strFirstDay);
        std::string strNow = document["time"].GetString();
        int dayCount = GM->getDayDiff(Value(strFirstDay.substr(0, 4)).asInt(), Value(strFirstDay.substr(5, 2)).asInt(), Value(strFirstDay.substr(8, 2)).asInt(), Value(strNow.substr(0, 4)).asInt(), Value(strNow.substr(5, 2)).asInt(), Value(strNow.substr(8, 2)).asInt());
        UDSetInt(KEY_DAY_COUNT, dayCount);
    }
    
    lastDocument = getDocument(sender, data);
    if(BHUD != nullptr){
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
int BuggyServerManager::getIntIfUndefineZero(std::string str){
    if(str.compare("undefined") == 0){
        return 0;
    }
    return Value(str).asInt();
}
void BuggyServerManager::getMyRank(int trophy){
    sendPost("getrank", strmake("id=%s&trophy=%d", requestedID.c_str(), trophy), httpresponse_selector(BuggyServerManager::onGetMyRankComplete));
}
void BuggyServerManager::onGetMyRankComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
//    if(HUD != nullptr){
//        HUD->isLoadingRankSuccess = false;
//    }
    if(document.HasMember("result")) {
//        if(HUD != nullptr){
//            HUD->myRank = document["result"].GetInt() + 1;
//            HUD->isLoadingRankSuccess = true;
//            HUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
//        }
        BHUD->myRank = document["result"].GetInt() + 1;
    }else{
        BHUD->myRank = 999;
    }
    if(BHUD != nullptr){
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::isThisAppFree(){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    std::string isThisAppFreeAddress = strmake("%s/%s/", serverUrl.c_str(), "isCartoonCraftFree");;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    isThisAppFreeAddress = strmake("%s/%s/", serverUrl.c_str(), "isCartoonCraftFree");
    log("here android url");
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    isThisAppFreeAddress = strmake("%s/%s/", serverUrl.c_str(), "isCartoonCraftIOSFree");
#endif
    log("isThisAppFreeAddress: %s", isThisAppFreeAddress.c_str());
    request->setUrl(isThisAppFreeAddress);
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onIsThisAppFree));
    
    // write the post data
//    std::string requestData = strmake("id=%s", requestedID.c_str());
//    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
//    sendPost("isCartoonCraftFree", strmake("id=%s", requestedID.c_str()), );
}
void BuggyServerManager::onIsThisAppFree(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 5;
        return;
    }
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
    }
    //    setTime(document["time"].GetString());
    bool isFree = false;
    if(document.HasMember("result")) {
        isFree = document["result"].GetBool();
        log("isFree: %d", isFree);
    }
    if(isFree){
        UDSetBool(KEY_IS_FREE_USER, true);
    }
    if(TITLE){
        TITLE->isFreeApp = isFree;
        TITLE->isFreeAppResultArrived = true;
    }
    log("should handle free");
}
void BuggyServerManager::getArenaRank(){
    sendPost("getarenarank", "id=" + UDGetStr(KEY_SAVED_ID), httpresponse_selector(BuggyServerManager::onGetArenaComplete));
}
void BuggyServerManager::onGetArenaComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 6;
        return;
    }
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
    }
//    setTime(document["time"].GetString());
    //    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("names")) {
//        UDSetStr(KEY_NAME, document["names"].GetString());
        
        log("names: %s", UDGetStr(KEY_NAME).c_str());
        //        HUD->playerName = document["name"].GetString();
        TITLE->colosseumTotal = document["total"].GetInt();
        if(document.HasMember("you")){
            TITLE->colosseumMe = document["you"].GetInt();
        }
        TITLE->isColosseumRankInfoReceived = true;
        
        TITLE->strRankInfo = document["names"].GetString();
    }else{
        this->isServerFailed = true;
        errorCode = 7;
    }
}
void BuggyServerManager::sendArenaScore(int score){
    int clearCount = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
    saveUserData("arena_score="+Value(score).asString() + "&arena_clr_cnt=" + Value(clearCount).asString());
}
void BuggyServerManager::onSENDArenaScoreComplete(cocos2d::Node *sender, void *data){
    log("send arena score complete");
    HUD->hideIndicator();
}
void BuggyServerManager::getArenaReward(){
    sendPost("getuserdata", "id=" + requestedID + "&arena_reward=1", httpresponse_selector(BuggyServerManager::onGetArenaRewardComplete));
}
void BuggyServerManager::onGetArenaRewardComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 8;
        return;
    }
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
    }
    if(TITLE && TITLE != nullptr && document.HasMember("arena_reward")) {
        TITLE->arenaRecord = document["arena_reward"].GetString();
        TITLE->isArenaRecordRecieved = true;
    }
}
void BuggyServerManager::addShield(int hour){
    log("addshield %d", hour);
    sendPost("addshield", "id=" + requestedID + "&hour=" + Value(hour).asString() + "&gem=" + Value(GM->getGem()).asString(), httpresponse_selector(BuggyServerManager::onAddShieldComplete));
}
void BuggyServerManager::onAddShieldComplete(cocos2d::Node *sender, void *data){
    log("onAddShieldComplete");
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 9;
        return;
    }
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
    }
    
    if(BHUD != nullptr){
        if(document.HasMember("result")) {
            std::string strTime = document["result"].GetString();
            BHUD->shieldEndTimeT = getTimeTFromStr(strTime);
            log("shield end time: %s", strTime.c_str());
        }
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
    }
}


void BuggyServerManager::verifyReceipt(std::string strSignedData, std::string strSignature){
    std::string strPlatform = "google";
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    strPlatform = "apple";
#endif
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/verifyReceipt", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onVerifyReceiptComplete));
    
    // write the post data
    std::string requestData = strmake("id=%s&signedData=%s&signature=%s&platform=%s", requestedID.c_str(), strSignedData.c_str(), strSignature.c_str(), strPlatform.c_str());
    log("verifyReceipt %s", requestData.c_str());
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::onVerifyReceiptComplete(cocos2d::Node *sender, void *data){
//    return; // test for verifying test
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        errorCode = 9;
        return;
    }
    isFailedToGetNetworkData = false;
    log("onVerifyReceiptComplete done");
    if (document.HasMember("error")) {
        log("onVerifyReceiptComplete done1");
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
    }
    
    log("onVerifyReceiptComplete done2");
    if (document.HasMember("result")) {
        log("onVerifyReceiptComplete done4");
        if(document["result"].GetBool()){
            GM->iapFlag = IAP_FLAG_SUCCESS;
            if(!UDGetBool(KEY_IAP_USER, false)){
                UDSetBool(KEY_IAP_USER, true);
            }
        }else{
            GM->iapFlag = IAP_FLAG_FAILED;
        }
    }else{
        GM->iapFlag = IAP_FLAG_FAILED;
        log("onVerifyReceiptComplete done3");
    }
}
void BuggyServerManager::banMe(){
    sendPost("saveuser", "id=" + requestedID + "&ban=1&trophy=0", httpresponse_selector(BuggyServerManager::onActionNotNeededComplete));
}


void BuggyServerManager::checkPlayID(std::string data){
    sendPost("checkplayid", "playid=" + data, httpresponse_selector(BuggyServerManager::onCheckPlayID));
}
void BuggyServerManager::onCheckPlayID(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("error")) {
        
    }else{
        std::string playerID = document["result"].GetString();
        if (playerID.compare("-1") == 0) {
            
        }else{
            GM->playerIDWithGPSID = Value(playerID).asString();
        }
        if(TITLE){
            TITLE->isUserIDExistCheckRequested = false;
            TITLE->isUserIDExistCheckArrived = true;
        }
    }
}
int BuggyServerManager::getTimeLeftToNewDay(){
    time_t now = getCurrentTimeT();
    struct tm nextDay;
    double seconds;
    time(&now);
    nextDay = *localtime(&now);
    nextDay.tm_hour = 0; nextDay.tm_min = 0; nextDay.tm_sec = 0;
    nextDay.tm_mon = 0;  nextDay.tm_mday = nextDay.tm_mday + 1;
    seconds = difftime(mktime(&nextDay), now);
    return (int)seconds;
}
