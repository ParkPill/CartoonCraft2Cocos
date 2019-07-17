// Copyright (c) 2012 ursinepaw.com
#include "../NativeInterface.h"
#include "../GameManager.h"


#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"
using namespace cocos2d;

namespace NativeInterface
{
    void NativeInterface::postScore(int score, const char* leaderboardID){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "updateTopScoreLeaderboard",
                                           "(ILjava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(leaderboardID);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, (jint) score, arg);
            
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    
    bool NativeInterface::init(actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "init",
                                           "(J)Z"))
        {
            jlong arg = (long)(void*)callback;
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
            return ret == JNI_TRUE;
        }
        return false;
    }
    
    void NativeInterface::loadSKUItems(actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "loadSKUItems",
                                           "(J)V"))
        {
            jlong arg = (long)(void*)callback;
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    
    const char* NativeInterface::getSkuDescription(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuDescription",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
            
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuPrice(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuPrice",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuDetails(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuDetails",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuTitle(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuTitle",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    const char* NativeInterface::getSkuType(const char* key)
    {
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getSkuType",
                                           "(Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    
    bool NativeInterface::isPurchased(const char* item)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "isPurchased",
                                           "(Ljava/lang/String;)Z"))
        {
            jstring arg = t.env->NewStringUTF(item);
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
            return ret == JNI_TRUE;
        }
        return false;
    }
    void NativeInterface::trackEvent(const char* category, const char* name, const char* param1, const char* param2, const char* valueName, int value){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "trackEvent",
                                           "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V"))
        {
            jstring arg = t.env->NewStringUTF(category);
            jstring arg1 = t.env->NewStringUTF(name);
            jstring arg2 = t.env->NewStringUTF(param1);
            jstring arg3 = t.env->NewStringUTF(param2);
            jstring arg4 = t.env->NewStringUTF(valueName);
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg, arg1, arg2, arg3, arg4, (jint)value);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(arg1);
            t.env->DeleteLocalRef(arg2);
            t.env->DeleteLocalRef(arg3);
            t.env->DeleteLocalRef(arg4);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::sharePhoto(const char* fileName, const char* msg)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "sharePhoto",
                                           "(Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring arg0 = t.env->NewStringUTF(fileName);
            jstring arg1 = t.env->NewStringUTF(msg);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg0, arg1);
//            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, arg0, arg1);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::shareLink(const char* title, const char* link)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "shareLink",
                                           "(Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring arg0 = t.env->NewStringUTF(title);
            jstring arg1 = t.env->NewStringUTF(link);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg0, arg1);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::openUrl(const char* str)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "openUrl",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(str);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::purchaseOnestore(const char* productID)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "buyOnestore",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(productID);
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::purchase(const char* key, int amount, actionFinished callback)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "purchase",
                                           "(Ljava/lang/String;IJ)V"))
        {
            jstring arg = t.env->NewStringUTF(key);
            jlong arg2 = (long)(void*)callback;
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg,(jint) amount, arg2);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::loginPlayService()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "loginPlayService",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::exitGame()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "exitGame",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::logoutPlayService()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "logoutPlayService",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showLeaderboard(const char* leaderboardID){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showLeaderboards",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(leaderboardID);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showTapjoyOfferwall(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showTapjoyOfferwall",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::askAllowAlert(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "askAllowAlert",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showAppLovinVideo(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAppLovinVideo",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showUnityAdsVideo()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showUnityAdsVideo",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showTopAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showTopAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showChartboost()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showChartboost",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showChartboostMoreApps()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showChartboostMoreApps",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::showAdColonyVideo()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showAdColonyVideo",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::addAdColonyRewards()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "addAdColonyRewards",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::destroyAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "destroyAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::showRectAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "showRectAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
    void NativeInterface::destroyRectAds()
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "destroyRectAds",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::resetPushNotification(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "resetPushNotification",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::cancelNotifacation(const char* msg, int seconds){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "cancelNotifacation",
                                           "(Ljava/lang/String;I)V"))
        {
            jstring arg = t.env->NewStringUTF(msg);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg, (jint)seconds);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::Push_Notification(const char* msg, int seconds)
    {
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "Push_Notification",
                                           "(Ljava/lang/String;I)V"))
        {
            jstring arg = t.env->NewStringUTF(msg);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg, (jint)seconds);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::enablePush(bool enable){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "enablePush",
                                           "(Z)V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID, (jboolean)enable);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    const char* NativeInterface::getUUID(){
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getUUID",
                                           "()Ljava/lang/String;"))
        {
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    const char* NativeInterface::getUDID(){
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getUDID",
                                           "()Ljava/lang/String;"))
        {
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    const char* NativeInterface::getHash(char const* data, char const* secretKey){
        static std::string result;
        result = "";
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "getHash",
                                           "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
        {
            jstring arg = t.env->NewStringUTF(data);
            jstring arg1 = t.env->NewStringUTF(secretKey);
            jstring ret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, arg, arg1);
            t.env->DeleteLocalRef(arg);
            t.env->DeleteLocalRef(t.classID);
            if (ret)
            {
                jboolean is_copy = 0;
                const char* str = t.env->GetStringUTFChars(ret, &is_copy);
                if (str)
                    result = str;
                if (is_copy)
                    t.env->ReleaseStringUTFChars(ret, str);
                t.env->DeleteLocalRef(ret);
            }
        }
        return result.c_str();
    }
    void NativeInterface::copyToClipboard(std::string str){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "copyToClipboard",
                                           "(Ljava/lang/String;)V"))
        {
            jstring arg = t.env->NewStringUTF(str.c_str());
            t.env->CallStaticVoidMethod(t.classID, t.methodID, arg);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    void NativeInterface::cancelAllLocalNotification(){
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "org.cocos2dx.cpp/AppActivity",
                                           "cancelAllLocalNotification",
                                           "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    /*
     void NativeInterface::Push_Notification(const char* msg, int seconds)
     {
     
     }
     
     void NativeInterface::FB_CreateNewSession()
     {
     
     }
     void NativeInterface::FB_Login(actionFinished callBack)
     {
     
     }
     void NativeInterface::FB_Customize(actionFinishedLong idCallBack, actionFinishedUTF8 nameCallBack)
     {
     
     }
     void NativeInterface::FB_RequestFriendsInfo(actionFinishedCharArray arrayCallBack, actionFinishedLongArray longArrayCallBack ,actionFinished countCallBack, actionFinishedCharArray userNamesCallBack)
     {
     
     }
     void NativeInterface::FB_Invite(int score, long ids[], int count)
     {
     
     }
     void NativeInterface::FB_PostScore(int score, long fbid)
     {
     
     }
     void NativeInterface::FB_RequestPostScore()
     {
     
     }
     void NativeInterface::FB_RequestHelp()
     {
     
     }
     void NativeInterface::FB_SendLife()
     {
     
     }
     void NativeInterface::FB_RequestScores(actionFinishedCharArray namesCallBack, actionFinishedCharArray scoresCallBack)
     {
     
     }
     
     void NativeInterface::FB_ProcessIncomingURL()
     {
     
     }
     void NativeInterface::FB_ProcessIncomingRequest(const char* urlStringChar)
     {
     
     }
     void NativeInterface::FB_ProcessIncomingFeed(const char* urlStringChar)
     {
     
     }
     
     */
    void NativeInterface::restorePurchases(const char* item, actionFinished callback)
    {
        // Do nothing, not implemented for android
    }
    
    //void NativeInterface::consumePurchase(const char* item, actionFinished callback)
    //{
    //    JniMethodInfo t;
    //    if (JniHelper::getStaticMethodInfo(t,
    //                                       "org.cocos2dx.cpp/AppActivity",
    //                                       "consumePurchase",
    //                                       "(Ljava/lang/String;J)V"))
    //    {
    //        //jlong arg = (long)(void*)callback;
    //        jstring arg = t.env->NewStringUTF(item);
    //        jlong arg1 = (long)(void*)callback;
    //        t.env->CallStaticVoidMethod(t.classID, t.methodID, arg, arg1);
    //        t.env->DeleteLocalRef(t.classID);
    //     }
    //}
}
extern "C"
{
    void Java_org_cocos2dx_cpp_AppActivity_nativeactionFinished(JNIEnv* env, jobject thiz, jlong callback_address,jint error_code)
    {
        if (callback_address)
            (*reinterpret_cast<NativeInterface::actionFinished>(callback_address))(error_code);
    }
    void Java_org_cocos2dx_cpp_AppActivity_iapSuccessCallback(JNIEnv* env, jobject thiz)
    {
        GameManager::getInstance()->onIapSuccess();
    }
    void Java_org_cocos2dx_cpp_AppActivity_onVideoDone(JNIEnv* env, jobject thiz)
    {
        GameManager::getInstance()->showVideoDone();
    }
    
    void Java_org_cocos2dx_cpp_AppActivity_iapFailedCallback(JNIEnv* env, jobject thiz)
    {
        GameManager::getInstance()->onIapFailed();
    }
    
    void Java_org_cocos2dx_cpp_AppActivity_rewardUserForOfferWall(JNIEnv* env, jobject thiz, jint count)
    {
//        GameManager::getInstance()->rewardTapjoy(count);
    }
    void Java_org_cocos2dx_cpp_AppActivity_showDisposableMessage(JNIEnv* env, jobject thiz, jstring msg)
    {
        GameManager::getInstance()->getHudLayer()->showInstanceMessage(cocos2d::StringUtils::getStringUTFCharsJNI(env, msg));
    }
    void Java_org_cocos2dx_cpp_AppActivity_proveReciept(JNIEnv* env, jobject thiz, jstring purchase, jstring signature)
    {
//        GameManager::getInstance()->verifyReceipt(cocos2d::StringUtils::getStringUTFCharsJNI(env, purchase), cocos2d::StringUtils::getStringUTFCharsJNI(env, signature));
    }
    void Java_org_cocos2dx_cpp_AppActivity_playServiceConnected(JNIEnv* env, jobject thiz, jboolean login)
    {
//        GameManager::getInstance()->isPlayServiceLogIn = (bool)login;
//        if (GameManager::getInstance()->settingLayer != NULL ) {
//            SettingLayer* setting = (SettingLayer*)GameManager::getInstance()->settingLayer;
//            setting->onPlayServiceLogin((bool) login);
//        }
    }
    
    //	void Java_com_seventyfour_NativeInterface_nativeactionFinished(JNIEnv* env, jobject thiz, jlong callback_address)
    //	{
    //		if (callback_address)
    //			(*reinterpret_cast<NativeInterface::actionFinished>(callback_address))();
    //	}
}
