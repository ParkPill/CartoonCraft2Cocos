// Copyright (c) 2012 ursinepaw.com
//#pragma once
#include "cocos2d.h"

namespace NativeInterface
{
    typedef void (*actionFinished)(int);
    typedef void (*actionFinishedLong)(long);
    typedef void (*actionFinishedLongArray)(long[]);
    typedef void (*actionFinishedUTF8)(const char*);
    
    
    /*
     * Messages
     */
    
    static const int INIT_BILLING_NO = 0;
    static const int INIT_BILLING_YES = 1;
    static const int LOADSKUITEMS_ITEMS_YES = 2;
    static const int LOADSKUITEMS_ITEMS_NO = 3;
    static const int PURCHASE_SUCCESS = 4;
    static const int PURCHASE_ALREADY_PURCHASED = 5;
    static const int PURCHASE_FAIL = 6;
    
    struct NativeInterface
    {
        static bool init(actionFinished callback);
        static void loadSKUItems(actionFinished callback);
        static const char* getSkuDescription(const char* item);
        static const char* getSkuPrice(const char* item);
        static const char* getSkuDetails(const char* item);
        static const char* getSkuTitle(const char* item);
        static const char* getSkuType(const char* item);
        static bool isPurchased(const char* item);
        
        static void restorePurchases(const char* item, actionFinished callback);
        static void openUrl(const char* str);
        static void showAds();
        static void showTopAds();
        static void destroyAds();
        static void showRectAds();
        static void destroyRectAds();
        static int getIntegerForKey(const char* key);
        static void purchase(const char* item, int code, actionFinished callback);
        static void requestPaymentTStore(const char* item);
        static void naverPurchase(const char* item);
        static void ktPurchase(const char* item, actionFinished callback);
        static void samsungPurchase(const char* item, actionFinished callback);
        static void registerLocalNotification(int sec, const char* msg, const char* title);
        static void cancelAllLocalNotification();
        static void showChartboost();
        static void showChartboostMoreApps();
        static void showAdColonyVideo();
        static void showUnityAdsVideo();
        static void addAdColonyRewards();
        static void showLeaderboard(const char* leaderboardID);
        static void postScore(int score, const char* leaderboardID);
        static void sharePhoto(const char* fileName, const char* msg);
        static void shareLink(const char* title, const char* link);
        static void showAchievements();
        static void sendAchievement(const char* id, int percent);
        static void unlockAchievement(const char* id);
        static void saveGame();
        static void loadGame();
        static void setIntForSavedGame(const char* key, int value);
        static void setStringForSavedGame(const char* key, const char* value);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        static void loginPlayService();
        static void logoutPlayService();
        static void displayInterstitial();
        
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        static void cancelFullscreen();
        static const char* getProductPrice(int index);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        
        static void Push_Notification(const char* msg, int seconds);
        static void StartGameCenter();
        
#endif
    };
};