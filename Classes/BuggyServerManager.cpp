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
#include "HeroPage.h"
#include "LanguageManager.h"
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
    
    serverUrl = "http://222.120.115.95:8103"; // yangpyung white test
//    serverUrl = "http://222.120.115.95:8093"; // yangpyung macbook test
    
////////////////////////////    serverUrl = "http://52.78.139.153:8103"; // aws test - now use anymore
    log("**serverUrl: %s", serverUrl.c_str());
//    gameName = "cartooncraft";
    savedRID = UDGetStr(KEY_RID, "");
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
    
    if (GM->heroPage != nullptr) {
        HeroPage* layer = (HeroPage*)GM->heroPage;
        if(layer->isTimeRequestedForGacha){
            layer->onTimeRecievedForGacha();
        }
    }
    
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
    if(timet < 0){
        timet = 0;
    }
//    std::time_t now = std::time(NULL);
//    std::tm * ptm = std::gmtime(&now);
//    char bufferr[32];
//    // Format: Mo, 15.06.2009 20:20:00
//    std::strftime(bufferr, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    
    struct tm * timeinfo;
    timeinfo = gmtime (&timet);
    
    char buffer [19];
    strftime (buffer,19,"%F %X", timeinfo);
    return std::string(buffer).substr(0, 19);
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
    
    time_t what = timegm(&theTime); // UTC time - 1 hour can diff for summertime
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
    month = Value(strTime.substr(5, 2)).asInt();
//    month = 2; // test
    day = Value(strTime.substr(8, 2)).asInt();
//    day = 18; // test
    receivedWebTimeT = getTimeTFromStr(strTime);
//    receivedWebTimeT += tz_offset();
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
    
    getTimeLeftToSunday();
}
time_t BuggyServerManager::getCurrentTimeT(){
    if (!timeEstablished) {
        return -1;
    }
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
//    return 1; // test 
    std::string strTime = getStrFromTime(getCurrentTimeT());
    day = Value(strTime.substr(8, 2)).asInt();
//    day = 10; // test
    return day;
}
int BuggyServerManager::getMonth(){
    std::string strTime = getStrFromTime(getCurrentTimeT());
    month = Value(strTime.substr(5, 2)).asInt();
//    month = 8; // test 
    return month;
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
        this->isOffline = true;
        
        return nullptr;
    }
    this->isOffline = false;
    
    log("sm resposeData: %s", resposeData.c_str());
    
    std::vector<char> *buffer = response->getResponseData();
    if (buffer->size() > 0) {
        std::string res;
        res.insert(res.begin(), buffer->begin(), buffer->end());
//        log("sm ** res %s", res.c_str());
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
//        response->release();
        buffer->clear();
        return document;
    }else{
        
    }
    return nullptr;
}

void BuggyServerManager::sendPost(std::string method, std::string requestData, SEL_HttpResponse pSelector){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/%s/%s/", serverUrl.c_str(),apiName.c_str(), method.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    log("send post: %s", requestData.c_str());
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, pSelector);
    
    // write the post data
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::sendGet(std::string method, SEL_HttpResponse pSelector){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/%s/%s/", serverUrl.c_str(),apiName.c_str(), method.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(this, pSelector);
    
    // write the post data
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
std::string BuggyServerManager::getIDForPost(){
    std::string strRID = "id=" + requestedID;
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 1){
        strRID = "_id=" + savedRID;
    }
    return strRID;
}
void BuggyServerManager::getPostBoxItem(){std::string savedRID = UDGetStr(KEY_RID, "");
    std::string strRID = getIDForPost();
    sendPost("getuserdata", strRID + "&" + "post=1", httpresponse_selector(BuggyServerManager::onGetPostBoxItem));
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
void BuggyServerManager::rename(std::string strName){
    if(strName.size() == 0){
        strName = "Noname";
    }
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 1){
        sendPost("rename", "_id=" + savedRID + "&" + "id=" + requestedID + "&name=" + strName, httpresponse_selector(BuggyServerManager::onRenameComplete));
    }else{
        sendPost("rename", "id=" + requestedID + "&name=" + strName, httpresponse_selector(BuggyServerManager::onRenameComplete));
    }
}
void BuggyServerManager::onRenameComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("result")) {
        UDSetStr(KEY_NAME, document["result"].GetString());
        resultMessage = "";
    }else{
        resultMessage = document["error"].GetString();
    }
    if(TITLE){
        TITLE->renameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::saveUserData(std::string data){
//    if (GM->getGem() > gemLimit || GM->getCoin() > goldLimit || GM->getTree() > treeLimit) {
////        isBannedUser = true;
////        banMe();
//    }else{
        std::string savedRID = UDGetStr(KEY_RID, "");
        if(savedRID.length() > 1){
            sendPost("saveuser", "_id=" + savedRID + "&" + "id=" + requestedID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
        }else{
            sendPost("saveuser", "id=" + requestedID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
        }
//    }
}
void BuggyServerManager::onSaveUserData(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("error")) {
        if (HUD) {
//            HUD->onServerFailed();
        }
    }else{
        Node* node = GM->titleLayer;
        if (TITLE && node != nullptr) {
            TITLE->hideIndicatorRequested = true;
            if (TITLE->isNameRequested) {
                TITLE->nameHandleState = NETWORK_HANDLE_STATE_ARRIVED;
            }
        }
        if(HUD){
            HUD->hideIndicatorRequested = true;
        }
        if (document.HasMember("arenarid")) {
            const char* arenarid = document["arenarid"].GetString();
            log("arena rid: %s", arenarid);
            UDSetStr(KEY_ARENA_RID, arenarid);
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
        GM->raidEnemyID = document["id"].GetString(); // test
//        GM->raidEnemyID = "97678"; // test
        GM->raidEnemyName = document["name"].GetString();
        GM->raidEnemyTrophy = document["trophy"].GetInt();
        GM->raidEnemyGold = document["gold"].GetInt()*0.5f;
        GM->raidEnemyTree = document["tree"].GetInt()*0.5f;
        GM->raidEnemyRID = "";
        if(document.HasMember("_id")){
            GM->raidEnemyRID = document["_id"].GetString(); // test
        }
        if(document.HasMember("deck")){
            GM->raidEnemyDeck = document["deck"].GetString();
        }else{
            GM->raidEnemyDeck = "";
        }
        if(document.HasMember("hdck")){
            GM->raidEnemyHeroDeck = document["hdck"].GetString();
        }else{
            GM->raidEnemyHeroDeck = "";
        }
        if (document.HasMember("heropos")) {
            GM->raidEnemyHeroPos = document["heropos"].GetString();
        }else{
            GM->raidEnemyHeroPos = "";
        }
        if(document.HasMember("buildings")){
            GM->raidEnemyBuildings = document["buildings"].GetString();
        }else{
            log("what?");
        }
        
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
        
//        log("saved id: %s", document["result"].GetInt());
        
        rapidjson::Value::ConstMemberIterator result = document.FindMember("result");
        if (result != document.MemberEnd())//if level is not found it will be ignored, not crash
        {
            requestedID = document["result"].GetString();
            UDSetStr(KEY_SAVED_ID, requestedID);
            resultMessage = requestedID;
            
            if (document.HasMember("_id")) {
                std::string strRID = "";
                strRID += document["_id"].GetString();
                UDSetStr(KEY_RID, strRID);
                savedRID = "" + strRID;
            }
        }else{
            resultMessage = document["error"].GetString();
        }
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
        this->isOffline = true;
        errorCode = 0;
        return;
    }
    this->isOffline = false;
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
        this->isOffline = true;
        errorCode = 1;
        return;
    }
    this->isOffline = false;
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
        this->isOffline = true;
        errorCode = 2;
        return;
    }
    this->isOffline = false;
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        //        return;
    }
//    if(document.HasMember("time")) setTime(document["time"].GetString());
    
    GM->raidEnemyID = document["id"].GetString();
    GM->raidEnemyName = document["name"].GetString();
    if(document.HasMember("trophy")){
        GM->raidEnemyTrophy = document["trophy"].GetInt();
    }
    GM->raidEnemyGold = document["gold"].GetInt()*0.3f;
    int resourceMax = 500;
    if(GM->raidEnemyGold > resourceMax){
        GM->raidEnemyGold = resourceMax;
    }
    GM->raidEnemyTree = document["tree"].GetInt()*0.3f;
    if(GM->raidEnemyTree > resourceMax){
        GM->raidEnemyTree = resourceMax;
    }
    
    if (GM->isVisiting) {
        GM->raidEnemyGold = 0;
        GM->raidEnemyTree = 0;
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
    if(document.HasMember("hdck")){
        GM->raidEnemyHeroDeck = document["hdck"].GetString();
    }else{
        GM->raidEnemyHeroDeck = "";
    }
    if (document.HasMember("heropos")) {
        GM->raidEnemyHeroPos = document["heropos"].GetString();
    }else{
        GM->raidEnemyHeroPos = "";
    }
    if(document.HasMember("buildings")){
        GM->raidEnemyBuildings = document["buildings"].GetString();
    }
    
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
        this->isOffline = true;
        errorCode = 3;
        return;
    }
    this->isOffline = false;
    isFailedToGetNetworkData = false;
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        isFailedToGetNetworkData = true;
        //        return;
        
    }
    //    if(document.HasMember("time")) setTime(document["time"].GetString());
    if(BHUD){
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
}
void BuggyServerManager::getAllUserData(){
    std::string strRID = "id=" + requestedID;
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 1){
        strRID = "_id=" + savedRID;
    }
//    isAllUserDataRequested = true;
    sendPost("getuserdata",
             strRID +
             "&name=1"+
             "&trophy=1"+
             "&arena_clr_cnt=1"+
             "&stage_clr_idx=1"+
             "&buildings=1"+
             "&inventory=1"+
             "&deck=1"+
             "&hdck=1"+
             "&hivt=1"+
             "&gem=1"+
             "&gold=1"+
             "&tree=1"+
//             "&arena_score=1"+
//             "&arena_reward=1"+
             "&spc_units=1"+
//             "&under_attack=1"+
//             "&golden_ticket=1"+
             "&defence_record=1"+
             "&search_state=1"+
             "&search_items=1"+
             "&attend=1"+
             "&gift_day=1"+
             "&iap_list=1"+
             "&iap_total=1"+
//             "&mission=1"+
             "&post=1"+
             "&ban=1"+
//             "&access_reward_receive=1"+
             "&shield_end=1" +
//             "&monthlyEvent=1" +
//             "&last_launch_date=1"+
             "&first_launch_date=1",
             httpresponse_selector(BuggyServerManager::onGetUserDataComplete));
}

void BuggyServerManager::getUserData(std::string data){
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 1){
        sendPost("getuserdata", "_id=" + savedRID + "&" + data.c_str(), httpresponse_selector(BuggyServerManager::onGetUserDataComplete));
    }else{
        sendPost("getuserdata", "id=" + requestedID + "&" + data.c_str(), httpresponse_selector(BuggyServerManager::onGetUserDataComplete));
    }
}
void BuggyServerManager::onGetUserDataComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        this->isOffline = true;
        errorCode = 4;
        return;
    }
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
        this->isServerFailed = true;
        this->isOffline = true;
        return;
    }
    this->isOffline = false;
    if(isInitUserDataRequested){
        isInitUserDataReceived = true;
    }
    
//    if(document.HasMember("time")){
//        setTime(document["time"].GetString());
//    }
    //    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("_id")) {
        UDSetStr(KEY_RID, document["_id"].GetString());
        std::string strRID = document["_id"].GetString();
        savedRID = "" + strRID;
    }
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
    if(document.HasMember("gem")){
        GM->setGem(document["gem"].GetInt());
    }
    if(document.HasMember("buildings")){
        std::string str = document["buildings"].GetString();
        if(str.length() > 0){
            UDSetStr(KEY_BUILDINGS, str);
        }
    }
    if(document.HasMember("inventory")) {
        std::string str = document["inventory"].GetString();
        if(str.length() > 1){
            std::string value = "";
            value += str;
            UDSetStr(KEY_UNITS_INVENTORY, value);
            
//            ValueVector units = GameManager::getInstance()->split(value, "_");
//            std::vector<UnitInfo*> unitInfoBattleUnitListInventory;
//            for(int i = 0; i < units.size(); i++){
//                if(units.at(i).asString().length() > 0){
//                    UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
//                    unitInfoBattleUnitListInventory.push_back(info);
//                }
//            }
//            GM->saveBattleUnitInventory(unitInfoBattleUnitListInventory);
        }
    }
    if(document.HasMember("deck")) {
        std::string str = document["deck"].GetString();
        if(str.length() > 1){
            std::string value = "";
            value += str;
            UDSetStr(KEY_UNITS_DECK, value);
            
//            ValueVector units = GameManager::getInstance()->split(value, "_");
//            std::vector<UnitInfo*> unitInfoBattleUnitListDeck;
//            for(int i = 0; i < units.size(); i++){
//                if(units.at(i).asString().length() > 0){
//                    UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
//                    unitInfoBattleUnitListDeck.push_back(info);
//                }
//            }
//            GM->saveBattleUnitDeck(unitInfoBattleUnitListDeck);
        }
    }
    if(document.HasMember("heropos")) {
        std::string str = document["heropos"].GetString();
        if(str.length() > 0){
            std::string value = "";
            ValueVector posList = GameManager::getInstance()->split(str, "_");
            int index = 0;
            for (int i = 0; i < posList.size(); i++) {
                ValueVector xAndY = GameManager::getInstance()->split(posList.at(i).asString(), "/");
                if(xAndY.size() > 1){
                    int x = xAndY.at(0).asInt();
                    int y = xAndY.at(1).asInt();
                    UDSetInt(strmake(KEY_HERO_POS_X_FORMAT, index).c_str(), (int)x);
                    UDSetInt(strmake(KEY_HERO_POS_Y_FORMAT, index).c_str(), (int)y);
                    index++;
                }
            }
        }
    }
    if(document.HasMember("hdck")) {
        std::string str = document["hdck"].GetString();
        if(str.length() > 0){
            std::string value = "";
            value += str;
            UDSetStr(KEY_UNITS_HERO_DECK, value);
            
            ValueVector units = GameManager::getInstance()->split(value, "_");
            std::vector<UnitInfo*> unitInfoHeroListDeck;
            for(int i = 0; i < units.size(); i++){
                if(units.at(i).asString().length() > 0){
                    UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
                    unitInfoHeroListDeck.push_back(info);
                }
            }
            GM->saveHeroDeck(unitInfoHeroListDeck);
        }
    }
    if(document.HasMember("hivt")) {
        std::string str = document["hivt"].GetString();
        if(str.length() > 0){
            std::string value = "";
            value += str;
            UDSetStr(KEY_UNITS_HERO_INVENTORY, str);
            
            ValueVector units = GameManager::getInstance()->split(value, "_");
            std::vector<UnitInfo*> unitInfoHeroListInventory;
            for(int i = 0; i < units.size(); i++){
                if(units.at(i).asString().length() > 0){
                    UnitInfo* info = GM->getUnitInfoFromString(units.at(i).asString());
                    unitInfoHeroListInventory.push_back(info);
                }
            }
            GM->saveHeroInventory(unitInfoHeroListInventory);
        }
    }
    if(document.HasMember("golden_ticket")) {
//        UDSetInt(KEY_GOLDEN_TICKET, document["golden_ticket"].GetInt());
    }
    if(document.HasMember("defence_record")) {
        UDSetStr(KEY_DEFENCE_RECORD, document["defence_record"].GetString());
    }
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
//    if(document.HasMember("gift_day")) UDSetInt(KEY_GIFT_DAY, document["gift_day"].GetInt());
    
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
    if(document.HasMember("post")) {
        postBoxConent = document["post"].GetString();
    }
    if(document.HasMember("ban")) {
        isBannedUser = document["ban"].GetInt() == 1;
        log("ban: %d", isBannedUser);
    }
    if(document.HasMember("rewarded_index")) {
        GM->rewardedCode = document["rewarded_index"].GetInt();
    }
    if(document.HasMember("shield_end")) {
        UDSetStr(KEY_SHIELD_END_TIME, document["shield_end"].GetString());
    }
    if(document.HasMember("last_launch_date")) {
        UDSetStr(KEY_LAST_LAUNCH_DATE, document["last_launch_date"].GetString());
    }
    if(document.HasMember("monthlyEvent")) {
        if(UDGetBool(KEY_MONTHLY_EVENT_INFO_INVALIDATE, false)){
            UDSetStr(KEY_MONTHLY_EVENT_INFO, document["monthlyEvent"].GetString());
        }
    }
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
    if (GM->titleLayer && GM->titleLayer != nullptr) {
        if(TITLE->isHeroInfoRequested){
            TITLE->isHeroInfoRequested = false;
            TITLE->isHeroInfoArrived = true;
        }else if(TITLE->isAllUserDataRequested){
            TITLE->isAllUserDataRequested = false;
            TITLE->isAllUserDataArrived = true;
        }
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
        if(BHUD != nullptr){
            BHUD->myRank = document["result"].GetInt() + 1;
        }
    }else{
        if(BHUD != nullptr){
            BHUD->myRank = 999;
        }
    }
    if(BHUD != nullptr){
        BHUD->networkStateGetData = NETWORK_HANDLE_STATE_ARRIVED;
        BHUD->isMyRankReceived = true;
    }
}
void BuggyServerManager::getGameInfo(){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/getgameinfo", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onGetGameInfoComplete));
    
    // write the post data
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::onGetGameInfoComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("iv") && TITLE) {
        TITLE->iv = document["iv"].GetInt();
    }
    if(document.HasMember("av") && TITLE) {
        TITLE->av = document["av"].GetInt();
    }
    if(document.HasMember("ri") && TITLE) {
        TITLE->rewardIndex = document["ri"].GetInt();
    }
    TITLE->isGameInfoRecieved = true;
}
void BuggyServerManager::getRewardInfo(){
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/getrewardcontent", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onGetRewardInfoComplete));
    
    // write the post data
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::onGetRewardInfoComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("rc") && TITLE) {
        TITLE->rewardInfo = document["rc"].GetString();
    }
    TITLE->isRewardInfoReceived = true;
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
        this->isOffline = true;
        errorCode = 5;
        return;
    }
    this->isOffline = false;
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
    sendPost("getarnrank", "_id=" + UDGetStr(KEY_RID), httpresponse_selector(BuggyServerManager::onGetArenaComplete));
}
void BuggyServerManager::onGetArenaComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        this->isOffline = true;
        errorCode = 6;
//        document.Clear();
        return;
    }
    this->isOffline = false;
    
    if (document.HasMember("error")) {
        log("server data error: %s", document["error"].GetString());
    }
//    setTime(document["time"].GetString());
    //    SET_DOCUMENT_AND_CHECK_ERROR
    if(document.HasMember("names")) {
//        UDSetStr(KEY_NAME, document["names"].GetString());
        
        log("names: %s", UDGetStr(KEY_NAME).c_str());
        //        HUD->playerName = document["name"].GetString();
        if(TITLE){
            TITLE->colosseumTotal = document["total"].GetInt();
            if(document.HasMember("you")){
                TITLE->colosseumMe = document["you"].GetInt();
            }
            TITLE->isColosseumRankInfoReceived = true;
            
            TITLE->strRankInfo = document["names"].GetString();
        }
    }else{
        this->isServerFailed = true;
        this->isOffline = true;
        errorCode = 7;
    }

}
void BuggyServerManager::sendArenaScore(int score){
    std::string savedRID = UDGetStr(KEY_RID, "");
    std::string data = "score="+Value(score).asString() + "&name=" + UDGetStr(KEY_NAME);
    std::string arenaRID = UDGetStr(KEY_ARENA_RID, "");
    if(arenaRID.length() > 0){
        data += "&arenarid=" + arenaRID;
    }
    BSM->sendPost("arnsv", "_id=" + savedRID + "&" + data, httpresponse_selector(BuggyServerManager::onSaveUserData));
    
//    saveUserData("arena_score="+Value(score).asString() + "&arena_clr_cnt=" + Value(clearCount).asString());
}
void BuggyServerManager::onSENDArenaScoreComplete(cocos2d::Node *sender, void *data){
    log("send arena score complete");
    HUD->hideIndicator();
}
void BuggyServerManager::getArenaReward(){
    std::string savedRID = UDGetStr(KEY_RID, "");
    if(savedRID.length() > 0){
        sendPost("getuserdata", "_id=" + savedRID + "&arena_reward=1", httpresponse_selector(BuggyServerManager::onGetArenaRewardComplete));
    }else{
        sendPost("getuserdata", "id=" + requestedID + "&arena_reward=1", httpresponse_selector(BuggyServerManager::onGetArenaRewardComplete));
    }
}
void BuggyServerManager::onGetArenaRewardComplete(cocos2d::Node *sender, void *data){
    rapidjson::Document document = getDocument(sender, data);
    if(document.IsNull()){
        this->isServerFailed = true;
        this->isOffline = true;
        errorCode = 8;
        return;
    }
    this->isOffline = false;
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
        this->isOffline = true;
        errorCode = 9;
        return;
    }
    this->isOffline = false;
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
    log("signedData:%s", strSignature.c_str());
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
        this->isOffline = true;
        errorCode = 9;
        return;
    }
    this->isOffline = false;
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
//    struct tm nextDay;
//    double seconds;
//    time(&now);
    int timeLeft = 86400 - now%86400;
    return timeLeft;
//    nextDay = *localtime(&now);
//    nextDay.tm_hour = 0; nextDay.tm_min = 0; nextDay.tm_sec = 0;
//    nextDay.tm_mon = 0;  nextDay.tm_mday = nextDay.tm_mday + 1;
//    seconds = difftime(mktime(&nextDay), now);
//    return (int)seconds;
}
void BuggyServerManager::saveUserData(std::vector<int>& datas){
    std::string strData = "";
    for(int i = 0; i < datas.size();i++){
        if (datas.at(i) == DATA_TYPE_GEM) {
            strData = strmake("%s&gem=%d", strData.c_str(), GM->getGem());
        }else if (datas.at(i) == DATA_TYPE_GOLD) {
            strData = strmake("%s&gold=%d", strData.c_str(), GM->getCoin());
        }else if (datas.at(i) == DATA_TYPE_TREE) {
            strData = strmake("%s&tree=%d", strData.c_str(), GM->getTree());
        }else if (datas.at(i) == DATA_TYPE_BUILDING) {
            strData = strmake("%s&buildings=%s", strData.c_str(), UDGetStr(KEY_BUILDINGS, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_INVENTORY) {
            strData = strmake("%s&inventory=%s", strData.c_str(), UDGetStr(KEY_UNITS_INVENTORY, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_DECK) {
            strData = strmake("%s&deck=%s", strData.c_str(), UDGetStr(KEY_UNITS_DECK, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_KEYS) {
            //            strData = strmake("%s&keys=%s", strData.c_str(), UDGetStr(KEY_KEYS, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_KEY_GET_STATE) {
            //            strData = strmake("%s&keygetstate=%s", strData.c_str(), UDGetStr(KEY_KEY_GET_STATE, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_IAP) {
            strData = strmake("%s&iap_list=%s&iap_total=%d", strData.c_str(), UDGetStr(KEY_IAP_LIST, "").c_str(), UDGetInt(KEY_IAP_TOTAL, 0));
        }else if (datas.at(i) == DATA_TYPE_SEARCH_STATE) {
            strData = strmake("%s&search_state=%s", strData.c_str(), UDGetStr(KEY_SEARCH_STATE, "000").c_str());
        }else if (datas.at(i) == DATA_TYPE_HERO_INVENTORY) {
            strData = strmake("%s&hivt=%s", strData.c_str(), UDGetStr(KEY_UNITS_HERO_INVENTORY, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_HERO_DECK) {
            strData = strmake("%s&hdck=%s", strData.c_str(), UDGetStr(KEY_UNITS_HERO_DECK, "").c_str());
        }else if (datas.at(i) == DATA_TYPE_STAGE_CLEAR) {
            strData = strmake("%s&stage_clr_idx=%d", strData.c_str(), UDGetInt(KEY_LAST_CLEAR_STAGE, -1));
        }
    }
    strData = strData.substr(1); // remove first &
    saveUserData(strData);
    log("save data: %s", strData.c_str());
    datas.clear();
}

void BuggyServerManager::saveCheckedData(){
    std::string strData = "";
//    if(isThereSomethingToSaveForMonthlyEventProgress){
//        strData += "monthlyEvent="+GM->getMonthlyEventInfoString();
//        isThereSomethingToSaveForMonthlyEventProgress = false;
//    }
    if(shouldSaveGold){
        if (strData.length() > 0) {
            strData += "&";
        }
        shouldSaveGold = false;
        strData += "gold="+Value(GM->getCoin()).asString();
    }
    if(shouldSaveLumber){
        if (strData.length() > 0) {
            strData += "&";
        }
        shouldSaveLumber = false;
        strData += "tree="+Value(GM->getTree()).asString();
    }
    if(shouldSaveGem){
        if (strData.length() > 0) {
            strData += "&";
        }
        shouldSaveGem = false;
        strData += "gem="+Value(GM->getGem()).asString();
    }
    if (strData.length() > 1) {
        saveUserData(strData);
    }
}
void BuggyServerManager::getPvp6ResultAndTicket(){
    if(savedRID.length() > 0){
        sendPost("getuserdata", "_id=" + savedRID + "&pvp6Ticket=1&pvp6_refill_start_time=1&pvp_rwd_6=1", httpresponse_selector(BuggyServerManager::onGetPvp6ResultAndTicketComplete));
    }else{
        sendPost("getuserdata", "id=" + requestedID + "&pvp6Ticket=1&pvp6_refill_start_time=1&pvp_rwd_6=1", httpresponse_selector(BuggyServerManager::onGetPvp6ResultAndTicketComplete));
    }
}
void BuggyServerManager::onGetPvp6ResultAndTicketComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("pvp_rwd_6")) {
        std::string strReward = document["pvp_rwd_6"].GetString();
        HEROPAGE->pvp6Reward = "" + strReward;
        if(HEROPAGE->pvp6Reward.length() > 1){
            std::string strRID = "id=" + requestedID;
            std::string savedRID = UDGetStr(KEY_RID, "");
            if(savedRID.length() > 1){
                strRID = "_id=" + savedRID;
            }
            sendPost("saveuser", strRID + "&pvp_rwd_6=_", httpresponse_selector(BuggyServerManager::onSaveUserData));
        }
    }
    if (document.HasMember("pvp6Ticket")) {
        BSM->pvp6TicketCount = document["pvp6Ticket"].GetInt();
        BSM->pvp6TicketRefillStartTime = document["pvp6_refill_start_time"].GetString();
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::getPvp6Rank(){
    sendPost("getp6rank", "_id=" + UDGetStr(KEY_RID, "_"), httpresponse_selector(BuggyServerManager::onGetPvp6RankComplete));
}
void BuggyServerManager::onGetPvp6RankComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("names")) {
        std::string strData = document["names"].GetString();
        ValueVector list = GM->split(strData, ",");
        HEROPAGE->pvp6RankInfoName.clear();
        HEROPAGE->pvp6RankInfoTrohpy.clear();
        HEROPAGE->pvp6RankInfoData.clear();
        
        for (int i = 0; i < list.size(); i++) {
            ValueVector smallList = GM->split(list.at(i).asString(), "|");
            if(smallList.size() > 0){
                HEROPAGE->pvp6RankInfoName.push_back(smallList.at(0).asString());
            }
            if(smallList.size() > 1){
                HEROPAGE->pvp6RankInfoTrohpy.push_back(smallList.at(1).asInt());
            }
            if(smallList.size() > 2){
                HEROPAGE->pvp6RankInfoData.push_back(smallList.at(2).asString());
            }
        }
    }
    if (document.HasMember("you")) {
        std::string meInfo = document["you"].GetString();
        ValueVector list = GM->split(meInfo, "|");
        HEROPAGE->pvp6MyRank = list.at(0).asInt();
        UDSetInt(KEY_PVP6_TROPHY, list.at(1).asInt());
    }
    if (document.HasMember("total")) {
        HEROPAGE->pvp6TotalUserCount = document["total"].GetInt();
    }
    
    if (shouldCheckPvp6Result) {
        shouldCheckPvp6Result = false;
        getPvp6ResultAndTicket();
    }else{
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::findMatchForPvp6(){
    sendPost("p6findmatch", "trophy=" + Value(UDGetInt(KEY_PVP6_TROPHY)).asString(), httpresponse_selector(BuggyServerManager::onFindMatchForPvp6Complete));
}
void BuggyServerManager::onFindMatchForPvp6Complete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("userid")) {
        pvpTargetTrophy = document["trophy"].GetInt();
        pvpTargetData = document["data"].GetString();
//        pvpTargetData = "_50/100/13/13_62/0/14/14_50/201/17/14_50/200/14/13_48/200/16/13_58/0/18/14_55/0/18/13_54/0/16/15_55/0/21/11_58/0/20/11_49/200/18/15_57/0/22/11_"; // test
        pvpTargetName = document["name"].GetString();
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::sendPvp6Result(int trophy){
    std::string strName = "" + UDGetStr(KEY_NAME, "Unknown");
    sendPost("p6sv", "_id=" + UDGetStr(KEY_RID, "_") + "&name=" + strName + "&trophy=" + Value(trophy).asString() + "&data=" + UDGetStr(KEY_UNITS_HERO_DECK, "_"), httpresponse_selector(BuggyServerManager::onSendPvp6ResultComplete));
}
void BuggyServerManager::onSendPvp6ResultComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("result")) {
        
    }
}
void BuggyServerManager::getPvp12ResultAndTicket(){
    if(savedRID.length() > 0){
        sendPost("getuserdata", "_id=" + savedRID + "&pvp12Ticket=1&pvp12_refill_start_time=1&pvp_rwd_12=1", httpresponse_selector(BuggyServerManager::onGetPvp12ResultAndTicketComplete));
    }else{
        sendPost("getuserdata", "id=" + requestedID + "&pvp12Ticket=1&pvp12_refill_start_time=1&pvp_rwd_12=1", httpresponse_selector(BuggyServerManager::onGetPvp12ResultAndTicketComplete));
    }
}
void BuggyServerManager::onGetPvp12ResultAndTicketComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("pvp_rwd_12")) {
        std::string strReward = document["pvp_rwd_12"].GetString();
        HEROPAGE->pvp12Reward = "" + strReward;
        if(HEROPAGE->pvp12Reward.length() > 1){
            std::string strRID = "id=" + requestedID;
            std::string savedRID = UDGetStr(KEY_RID, "");
            if(savedRID.length() > 1){
                strRID = "_id=" + savedRID;
            }
            sendPost("saveuser", strRID + "&pvp_rwd_12=_", httpresponse_selector(BuggyServerManager::onSaveUserData));
        }
    }
    if (document.HasMember("pvp12Ticket")) {
        BSM->pvp12TicketCount = document["pvp12Ticket"].GetInt();
        BSM->pvp12TicketRefillStartTime = document["pvp12_refill_start_time"].GetString();
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::getPvp12Rank(){
    sendPost("getp12rank", "_id=" + UDGetStr(KEY_RID, "_"), httpresponse_selector(BuggyServerManager::onGetPvp12RankComplete));
}
void BuggyServerManager::onGetPvp12RankComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("names")) {
        std::string strData = document["names"].GetString();
        ValueVector list = GM->split(strData, ",");
        HEROPAGE->pvp12RankInfoName.clear();
        HEROPAGE->pvp12RankInfoTrohpy.clear();
        HEROPAGE->pvp12RankInfoData.clear();
        for (int i = 0; i < list.size(); i++) {
            std::string str = list.at(i).asString();
            if(str.length() == 0){
                continue;
            }
            ValueVector smallList = GM->split(str, "|");
            if(smallList.size() > 0){
                if(smallList.at(0).asString().length() == 0){
                    HEROPAGE->pvp12RankInfoName.push_back("Unknown");
                }else{
                    HEROPAGE->pvp12RankInfoName.push_back(smallList.at(0).asString());
                }
            }
            if(smallList.size() > 1){
                HEROPAGE->pvp12RankInfoTrohpy.push_back(smallList.at(1).asInt());
            }
            if(smallList.size() > 2){
                HEROPAGE->pvp12RankInfoData.push_back(smallList.at(2).asString());
            }
        }
    }
    if (document.HasMember("you")) {
        std::string meInfo = document["you"].GetString();
        ValueVector list = GM->split(meInfo, "|");
        HEROPAGE->pvp12MyRank = list.at(0).asInt();
        UDSetInt(KEY_PVP12_TROPHY, list.at(1).asInt());
    }
    if (document.HasMember("total")) {
        HEROPAGE->pvp12TotalUserCount = document["total"].GetInt();
    }
    
    if (shouldCheckPvp12Result) {
        shouldCheckPvp12Result = false;
        getPvp12ResultAndTicket();
    }else{
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }
}
void BuggyServerManager::findMatchForPvp12(){
    sendPost("p12findmatch", "trophy=" + Value(UDGetInt(KEY_PVP12_TROPHY)).asString(), httpresponse_selector(BuggyServerManager::onFindMatchForPvp12Complete));
}
void BuggyServerManager::onFindMatchForPvp12Complete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("data")) {
        pvpTargetTrophy = document["trophy"].GetInt();
        pvpTargetData = document["data"].GetString();
//        pvpTargetData = "_55/203/13/12_62/103/16/15_57/2/15/16_49/202/22/12_63/202/11/13_54/203/10/15_54/102/11/18_55/103/12/18_49/202/21/9_49/102/13/19_57/2/12/19_53/102/-1/-1_"; // test
        pvpTargetName = document["name"].GetString();
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ARRIVED;
    }else{
        HEROPAGE->pvpNetworkState = NETWORK_HANDLE_STATE_ERROR;
    }
}
void BuggyServerManager::sendPvp12Result(int trophy){
    sendPost("p12sv", "_id=" + UDGetStr(KEY_RID, "_") + "&name=" + UDGetStr(KEY_NAME, "Unknown") + "&trophy=" + Value(trophy).asString() + "&data=" + UDGetStr(KEY_UNITS_HERO_DECK, "_"), httpresponse_selector(BuggyServerManager::onSendPvp12ResultComplete));
}
void BuggyServerManager::onSendPvp12ResultComplete(cocos2d::Node *sender, void *data){
    SET_DOCUMENT_AND_CHECK_ERROR
    if (document.HasMember("result")) {
        
    }
}
int BuggyServerManager::getTimeLeftToSunday(){
    struct tm *tminfo;
    time_t rawTime = getCurrentTimeT();
    tminfo = gmtime ( &rawTime);
    
    int timeLeft = 86400 - (tminfo->tm_hour*60*60 + tminfo->tm_min*60 + tminfo->tm_sec);
    timeLeft += 86400*(6 - tminfo->tm_wday);
    
    return timeLeft;
}

void BuggyServerManager::checkServer(){
    HttpRequest* request = new HttpRequest();
    request->setUrl(strmake("%s/checkVersion", serverUrl.c_str()));
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(this, httpresponse_selector(BuggyServerManager::onCheckServerCompleted));
    HttpClient::getInstance()->send(request);
    request->release();
}
void BuggyServerManager::onCheckServerCompleted(Node *sender, void *data)
{
    log("time complete request http");
    HttpResponse *response = (HttpResponse*)data;
    std::string str = std::string(response->getResponseHeader()->begin(), response->getResponseHeader()->end());
    int size = (int)response->getResponseHeader()->size();
    std::string responseData = std::string(response->getResponseData()->begin(), response->getResponseData()->end());
    rapidjson::Document document = getDocument(sender, data);
    if (!response->isSucceed())
    {
        log("!isSucceed %s", response->getErrorBuffer());
        return;
    }
    if(document.IsNull()){
        this->isServerFailed = true;
        this->isOffline = true;
        return;
    }
    this->isOffline = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    if (document.HasMember("iosVersionCode")) {
        int versionCode = Value(document["iosVersionCode"].GetString()).asInt();
        log("version code: %d", versionCode);
        GM->receivedVersionCode = versionCode;
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (document.HasMember("aosVersionCode")) {
        int versionCode = Value(document["aosVersionCode"].GetString()).asInt();
        log("version code: %d", versionCode);
        GM->receivedVersionCode = versionCode;
    }
#endif
    
    
    if (document.HasMember("serverMaintenance")) {
        int maintenance = Value(document["serverMaintenance"].GetString()).asInt();
        log("serverMaintenance: %d", maintenance);
        GM->serverMaintenance = maintenance;
    }
    
    if (document.HasMember("rewardCode")) {
        std::string str = document["rewardCode"].GetString();
        ValueVector list = GM->split(str, "_");
        if(list.size() > 1){
            GM->rewardCode = list.at(0).asInt();
            GM->rewardGemCount = list.at(1).asInt();
        }
    }
    if (document.HasMember("rewardMsg")) {
        std::string str = document["rewardMsg"].GetString();
        ValueVector list = GM->split(str, "/");
        for (int i = 0; i < list.size(); i++) {
            std::string strData = list.at(i).asString();
            ValueVector dataList = GM->split(strData, "_");
            if (dataList.size() > 1) {
                if (dataList.at(0).asString().compare("ko") == 0 && LM->getLanguageType() == LanguageType::KOREAN) {
                    GM->rewardMsg = dataList.at(1).asString();
                    break;
                }else if (dataList.at(0).asString().compare("en") == 0 && LM->getLanguageType() != LanguageType::KOREAN){
                    GM->rewardMsg = dataList.at(1).asString().c_str();
                    break;
                }
            }else{
                GM->rewardCode = 0; // cancel if data is wrong
            }
        }
    }
    
    log("resposeData: %s", responseData.c_str());
    
    if (document.HasMember("time")) {
        setTime(document["time"].GetString());
    }
    isCheckServerComplete = true;
}
int BuggyServerManager::getLastDayOfMonth(int month){
    int lastDay = 30;
    if (month == 1 ||
        month == 3 ||
        month == 5 ||
        month == 7 ||
        month == 8 ||
        month == 10 ||
        month == 12) {
        lastDay = 31;
    }else if(month == 2){
        lastDay = 28;
    }
    return lastDay;
}

