/*
 GameSharing.cpp
 
 Copyright 2015 Adrian Dawid
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 
 Created by Adrian Dawid on 17.08.14.
 */

#include "GameSharing.h"
#include "GameManager.h"
#include "BuggyServerManager.h"
#include "Title.h"
USING_NS_CC;

bool GameSharing::bIsGPGAvailable = true;
bool GameSharing::wasGPGAvailableCalled = false;
std::function<void()> GameSharing::errorHandler;
int GameSharing::localPlayerScore = -1;
bool GameSharing::requestIsBeingProcessed = false;
std::function<void()> GameSharing::requestCallback;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
std::map<int,std::string> GameSharing::iosLeaderboardIds = std::map<int,std::string>();
std::map<int,std::string> GameSharing::iosAchievementIds = std::map<int,std::string>();
int GameSharing::numberOfAchievements = 0;
int GameSharing::numberOfLeaderboards = 0;

#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
extern "C"
{
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_callCppCallback(JNIEnv* env, jobject thiz);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onIAPSuccess(JNIEnv* env, jobject thiz);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onIAPFailed(JNIEnv* env, jobject thiz);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onVideoDone(JNIEnv* env, jobject thiz);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onConsumeFinished(JNIEnv* env, jobject thiz, jstring skuName);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onRestored(JNIEnv* env, jobject thiz, jstring skuName);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onSignInPlayStore(JNIEnv* env, jobject thiz, jstring skuName);
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onSmartPassError(JNIEnv* env, jobject thiz, jstring strError);
};

JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_callCppCallback(JNIEnv* env, jobject thiz)
{
    int newScore = 0;
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                     , "org.cocos2dx.cpp/AppActivity"
                                     , "collectScore"
                                     , "()I"))
    {
        newScore = t.env->CallStaticIntMethod(t.classID, t.methodID);
    }
    GameSharing::localPlayerScore = newScore;
    GameSharing::requestIsBeingProcessed = false;
    if(GameSharing::requestCallback)
    {
          GameSharing::requestCallback();
    }

}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onIAPSuccess(JNIEnv* env, jobject thiz)
{
    GameSharing::onIAPSuccess();
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onIAPFailed(JNIEnv* env, jobject thiz)
{
    GameSharing::onIAPFailed();
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onVideoDone(JNIEnv* env, jobject thiz)
{
    GM->showVideoDone();
}

std::string jstring2string(JNIEnv *env, jstring jStr) {
    if (!jStr)
        return "";
    
    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));
    
    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);
    
    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);
    
    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onConsumeFinished(JNIEnv* env, jobject thiz, jstring skuName)
{
    std::string strSkuName = jstring2string(env, skuName);
    GM->addConsumedItem(strSkuName);
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onRestored(JNIEnv* env, jobject thiz, jstring skuName)
{
    std::string strSkuName = jstring2string(env, skuName);
    GM->onRestored(strSkuName);
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onSignInPlayStore(JNIEnv* env, jobject thiz, jstring skuName)
{
    std::string strSkuName = jstring2string(env, skuName);
    GM->playerGPSID = strSkuName;
}
JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_onSmartPassError(JNIEnv* env, jobject thiz, jstring strError)
{
    std::string str = jstring2string(env, strError);
    GM->strSmartPassError = str;
}
#endif
void GameSharing::clearCacheForSmartPass(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "clearCacheForSmartPass"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
#endif
}
void GameSharing::restoreIAP(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "restoreIAB"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    restoreIOS();
#endif
}
void GameSharing::onIAPSuccess(){
    log("iap success onIAPSuccess");
//    const char* sku = GameSharing::getSkuByItemIndex(GM->iapDetail).c_str();
    if(GM->developer){ // test for verifying test
        GM->iapFlag = IAP_FLAG_SUCCESS;
        if(!UDGetBool(KEY_IAP_USER, false)){
            UDSetBool(KEY_IAP_USER, true);
        }
//        BSM->verifyReceipt(GameSharing::getSignedData(), GameSharing::getSignature(), ); // test for verifying test
    }else{
        BSM->verifyReceipt(GameSharing::getSignedData(), GameSharing::getSignature());
    }
}
void GameSharing::onIAPFailed(){
    log("iap failed onIAPFailed");
    GM->iapFlag = IAP_FLAG_FAILED;
}
void GameSharing::initGameSharing(void *initObj){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //iOS only:
    //Load the data from ios_ids.plist.
    ValueMap ids = FileUtils::getInstance()->getValueMapFromFile("ios_ids.plist");
    //Request the "Achievements" vector from the file.
    ValueVector aIds = ids.at("Achievements").asValueVector();
    //Request the "Leaderboards" vector from the file.
    ValueVector lIds = ids.at("Leaderboards").asValueVector();
    for (int i=0;i<aIds.size();i++) {
        //Copy the achievement IDs for later use.
        iosAchievementIds[i] = aIds.at(i).asString();
        numberOfAchievements=i+1;
    }
    for (int i=0;i<lIds.size();i++) {
        //Copy the leaderboard IDs for later use.
        iosLeaderboardIds[i] = lIds.at(i).asString();
        numberOfLeaderboards=i+1;
    }
    //Show the "Sign In" window.
    signInPlayer();

    initGameSharing_iOS(initObj);
#endif
    
}

void GameSharing::SubmitScore(int score,int leaderboardID)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    if(IsGPGAvailable()){
        JniMethodInfo t;
        //Is the Java method org.cocos2dx.cpp.AppActivty.openLeaderboard available?
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "openLeaderboard"
                                           , "(I)V"))
        {
            //Call the method with the leaderboardID as parameter
            t.env->CallStaticVoidMethod(t.classID, t.methodID, leaderboardID);
            
        }
        //Is the Java method org.cocos2dx.cpp.AppActivty.submitScoreToLeaderboard available?
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "submitScoreToLeaderboard"
                                           , "(I)V"))
        {
            //Call the method, this actually uploads the score to the leaderboard.
            t.env->CallStaticVoidMethod(t.classID, t.methodID, score);
        }
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //iOS only:
    //Call the submit Score function (implemented in Objective-C++).
    submitScoreToLeaderboard(score,leaderboardID);
#endif
}

void GameSharing::ShowLeaderboards(int id){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    if(IsGPGAvailable()){
        JniMethodInfo t;
        //Is the Java method org.cocos2dx.cpp.AppActivty.openLeaderboard available?
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "openLeaderboard"
                                           , "(I)V"))
        {
            //Call the method, over the JNI.
            //This changes the current leaderboard ID to id.
            t.env->CallStaticVoidMethod(t.classID, t.methodID, id);
        }
        //Is the Java method org.cocos2dx.cpp.AppActivty.openLeaderboardUI available?
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "openLeaderboardUI"
                                           , "()V"))
        {
            //Call the method, this actually shows the UI.
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
    }
    else{
        //Call the user defined error handler.
//        errorHandler();
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //Call the Objective-c++ code.
    openGameCenterLeaderboardsUI(id);
#endif
}

void GameSharing::UnlockAchivement(int ID)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    if(IsGPGAvailable())
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "openAchievement"
                                           , "(I)V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID , ID);
        }
        JniMethodInfo tmp;
        if (JniHelper::getStaticMethodInfo(tmp
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "updateAchievement"
                                           , "(I)V"))
        {
            tmp.env->CallStaticVoidMethod(tmp.classID, tmp.methodID , 100);
        }
    }
    else{
//        errorHandler();
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    unlockAchievement(ID);
#endif
}

void GameSharing::ShowAchievementsUI(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    if(IsGPGAvailable()){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "showAchievements"
                                           , "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
    }
    else{
//        errorHandler();
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    openAchievementUI();
#endif
}

bool GameSharing::IsGPGAvailable(){
    bool tmp = false;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    JniMethodInfo t;
//    log("isGPGSupported 1");
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "isGPGSupported"
                                       , "()Z"))
    {
//        log("isGPGSupported 2");
        tmp = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
//        log("isGPGSupported 3 %d", tmp);
    }
#endif
//    log("isGPGSupported 4");
    return tmp;
}

void GameSharing::ExitGame(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    //Android only:
    //Can the Google Play API be used?
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "exitGame"
                                       , "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
    }
#else
    Director::getInstance()->end();
#endif
}


void GameSharing::ActivateStdErrorHandler(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    SetErrorHandler([]() -> void{
        MessageBox("A problem with Google Play Games was encountered.", "Error");
        return;
    });
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    SetErrorHandler([]() -> void{
        MessageBox("A problem with Game Center was encountered.", "Error");
        return;
    });
#endif
}

void GameSharing::RequestCurrentScoreFromLeaderboard(int leaderboardID,std::function<void()> callback)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    GameSharing::startScoreRequest(leaderboardID);
    requestIsBeingProcessed = true;
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                   , "org.cocos2dx.cpp/AppActivity"
                                   , "requestScoreFromLeaderboard"
                                   , "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
    }
    requestIsBeingProcessed = true;
#endif
    if(callback)
    {
        requestCallback = callback;
    }
}
void GameSharing::logFB(const char* msgToLog){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "logFB"
                                       , "(Ljava/lang/String;)V"))
    {
        jstring _str = mInfo.env->NewStringUTF(msgToLog);
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, _str);
        mInfo.env->DeleteLocalRef(mInfo.classID);
        mInfo.env->DeleteLocalRef(_str);
    }
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
#endif
}
void GameSharing::firebaseLog(const char* logID, const char* logName, const char* logDetail){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "firebaseLog"
                                       , "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring _str = mInfo.env->NewStringUTF(logID);
        jstring _str1 = mInfo.env->NewStringUTF(logName);
        jstring _str2 = mInfo.env->NewStringUTF(logDetail);
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, _str, _str1, _str2);
        mInfo.env->DeleteLocalRef(mInfo.classID);
        mInfo.env->DeleteLocalRef(_str);
        mInfo.env->DeleteLocalRef(_str1);
        mInfo.env->DeleteLocalRef(_str2);
    }
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
#endif
}
//std::string GameSharing::getSkuByItemIndex(int item){
//    std::string skuID = "goldenticket1";
//    if(item == IAP_DETAIL_TICKET_1){
//        skuID = "goldenticket1";
//    }else if(item == IAP_DETAIL_PREMIUM_RETRY){
//        skuID = "premium_retry";
//    }else if(item == IAP_DETAIL_STARTER_1){
//        skuID = "cc_starter1";
//    }else if(item == IAP_DETAIL_STARTER_3){
//        skuID = "cc_starter3";
//    }else if(item == IAP_DETAIL_STARTER_10){
//        skuID = "cc_starter10";
//    }else if(item == IAP_DETAIL_GEM_3){
//        skuID = "cc_gem3";
//    }else if(item == IAP_DETAIL_GEM_10){
//        skuID = "cc_gem10";
//    }else if(item == IAP_DETAIL_GEM_50){
//        skuID = "cc_gem50";
//    }else if(item == IAP_DETAIL_GEM_100){
//        skuID = "cc_gem100";
//    }
//    log("buyITEM-> skuID: %s", skuID.c_str());
//    return skuID;
//}
void GameSharing::addSkuItemForStore(std::string skuID){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "addSkuID"
                                       , "(Ljava/lang/String;)V"))
    {
        jstring _sku = mInfo.env->NewStringUTF(skuID.c_str());
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, _sku);
        mInfo.env->DeleteLocalRef(mInfo.classID);
        mInfo.env->DeleteLocalRef(_sku);
    }
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    addSkuItemForIOS(skuID);
#endif
}
void GameSharing::addNonConsumableSkuItemForStore(std::string skuID){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "addNonConsumableSkuID"
                                       , "(Ljava/lang/String;)V"))
    {
        jstring _sku = mInfo.env->NewStringUTF(skuID.c_str());
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, _sku);
        mInfo.env->DeleteLocalRef(mInfo.classID);
        mInfo.env->DeleteLocalRef(_sku);
    }
#endif
}
void GameSharing::buyItem(std::string item){
    std::string skuID = item;//GameSharing::getSkuByItemIndex(item);
    log("buyITEM-> skuID1: %s", skuID.c_str());
    GM->iapDetail = item;
    GM->iapFlag = IAP_FLAG_INIT;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "buyItem"
                                       , "(Ljava/lang/String;)V"))
    {
        jstring _sku = mInfo.env->NewStringUTF(skuID.c_str());
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID, _sku);
        mInfo.env->DeleteLocalRef(mInfo.classID);
        mInfo.env->DeleteLocalRef(_sku);
    }
#endif
    
    log("buyITEM-> skuID2: %s", skuID.c_str());
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    buyItemIOS(skuID);
#endif
}
void GameSharing::init(){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    initForIOS();
#endif
    setupIAB();
}
std::string GameSharing::getPlayerID(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    if(IsGPGAvailable()){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "getPlayerID"
                                           , "()Ljava/lang/String;"))
        {
//            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
            std::string appHashStr = JniHelper::jstring2string(jstr);
            t.env->DeleteLocalRef(jstr);
            return appHashStr;
        }
    }
    else{
//        errorHandler();
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//    openAchievementUI();
    return "";
#endif
    return "";
}

std::string GameSharing::getSignedData(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "getReceiptSignedData"
                                       , "()Ljava/lang/String;"))
    {
        //            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        std::string appHashStr = JniHelper::jstring2string(jstr);
        t.env->DeleteLocalRef(jstr);
        return appHashStr;
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return getSignedDataForIOS();
#endif
    return "notsetGameSharing_getSignedData";
}
std::string GameSharing::getSignature(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "getReceiptSignature"
                                       , "()Ljava/lang/String;"))
    {
        //            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        std::string appHashStr = JniHelper::jstring2string(jstr);
        t.env->DeleteLocalRef(jstr);
        return appHashStr;
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //    openAchievementUI();
    return "notset2";
#endif
    return "notset_getSignature";
}
std::string GameSharing::getPriceAmount(const char* sku){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "getPriceAmount"
                                       , "(Ljava/lang/String;)Ljava/lang/String;"))
    {
        jstring _str = t.env->NewStringUTF(sku);
        jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, _str);
        std::string appHashStr = JniHelper::jstring2string(jstr);
        t.env->DeleteLocalRef(jstr);
        t.env->DeleteLocalRef(_str);
        return appHashStr;
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    getPriceAmountIOS(sku);
#endif
    return "notset_getPriceAmount";
}
std::string GameSharing::getPriceCurrency(const char* sku){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "getPriceCurrency"
                                       , "(Ljava/lang/String;)Ljava/lang/String;"))
    {
        jstring _str = t.env->NewStringUTF(sku);
        jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, _str);
        std::string appHashStr = JniHelper::jstring2string(jstr);
        t.env->DeleteLocalRef(jstr);
        t.env->DeleteLocalRef(_str);
        return appHashStr;
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //    openAchievementUI();
    getPriceCurrencyIOS(sku);
#endif
    return "notset_getPriceCurrency";
}
std::string GameSharing::getPriceLocale(const char* sku){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "getPriceLocale"
                                       , "(Ljava/lang/String;)Ljava/lang/String;"))
    {
        jstring _str = t.env->NewStringUTF(sku);
        jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, _str);
        std::string appHashStr = JniHelper::jstring2string(jstr);
        t.env->DeleteLocalRef(jstr);
        t.env->DeleteLocalRef(_str);
        return appHashStr;
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return getPriceLocaleIOS(sku);
#endif
    return "notset_getPriceLocale";
}
void GameSharing::showRewardedVideoAds(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "showRewardedVideoAds"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    showVideoAdsIOS();
#endif
}

void GameSharing::showBanner(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "showBanner"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#endif
}
void GameSharing::hideBanner(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "hideBanner"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
#endif
}
void GameSharing::loadBanner(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "loadBanner"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
#endif
}
void GameSharing::showInterstitial(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "showInterstitial"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    showInterstitialForIOS();
#endif
}
void GameSharing::setupIAB(){
    std::vector<std::string> list;
    list.push_back(IAP_DETAIL_PREMIUM_RETRY);
    list.push_back(IAP_DETAIL_TICKET_1);
    list.push_back(IAP_DETAIL_STARTER_1);
    list.push_back(IAP_DETAIL_STARTER_3);
    list.push_back(IAP_DETAIL_STARTER_10);
    list.push_back(IAP_DETAIL_GEM_3);
    list.push_back(IAP_DETAIL_GEM_10);
    list.push_back(IAP_DETAIL_GEM_50);
    list.push_back(IAP_DETAIL_GEM_100);
    list.push_back(IAP_DETAIL_CHAPTER2);
    list.push_back(IAP_DETAIL_CHAPTER3);
    list.push_back(IAP_DETAIL_UNIT_PACK_2);
    list.push_back(IAP_DETAIL_UNIT_PACK_5);
    list.push_back(IAP_DETAIL_UNIT_PACK_8);
    list.push_back(IAP_DETAIL_STARTER_KEY);
    list.push_back(IAP_DETAIL_MARTIAL_ART_PACKAGE);
    list.push_back(IAP_DETAIL_CLASSIC_WAR_PACKAGE);
    list.push_back(IAP_DETAIL_CREW_MANAGER_PACKAGE);
    list.push_back(IAP_DETAIL_MASTER_CREW_PACKAGE);
    list.push_back(IAP_DETAIL_EVENT_HERO_PACKAGE);
    list.push_back(IAP_DETAIL_MID_MONTH_PACKAGE);
    for (int i = 0; i < list.size(); i++) {
        addSkuItemForStore(list.at(i));
        if(list.at(i).compare(IAP_DETAIL_CHAPTER2) == 0 ||
           list.at(i).compare(IAP_DETAIL_CHAPTER3) == 0 ||
           list.at(i).compare(IAP_DETAIL_PREMIUM_RETRY) == 0){ // non-consumable
            addNonConsumableSkuItemForStore(list.at(i));
        }
    }
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "setupIAB"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    initForIOS();
#endif
}
void GameSharing::consumePurchased(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo mInfo;
    if (JniHelper::getStaticMethodInfo(mInfo
                                       , "org/cocos2dx/cpp/AppActivity"
                                       , "consumePurchased"
                                       , "()V"))
    {
        mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
        mInfo.env->DeleteLocalRef(mInfo.classID);
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
#endif
}
std::string GameSharing::getPlayerName(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android only:
    //Can the Google Play API be used?
    log("1");
    if(IsGPGAvailable()){
        log("2");
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t
                                           , "org.cocos2dx.cpp/AppActivity"
                                           , "getPlayerName"
                                           , "()Ljava/lang/String;"))
        {
            log("3");
            //            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            jstring jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
            log("4");
            std::string appHashStr = JniHelper::jstring2string(jstr);
            log("5");
            t.env->DeleteLocalRef(jstr);
            log("6");
            return appHashStr;
        }
    }
    else{
//        errorHandler();
    }
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //    openAchievementUI();
    return "ios Name";
#endif
    return "";
}
bool GameSharing::isVideoAvailable(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t
                                       , "org.cocos2dx.cpp/AppActivity"
                                       , "isVideoAvailable"
                                       , "()Z"))
    {
        jboolean result = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
        return (bool)result;
    }
#endif
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //    openAchievementUI();
    return true;
#endif
    return false;
}
void GameSharing::SetErrorHandler(std::function<void ()> handler){
    errorHandler = handler;
}
