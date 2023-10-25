#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#ifdef SDKBOX_ENABLED
//#include "PluginAdMob/PluginAdMob.h"
#include "PluginIAP/PluginIAP.h"
#include "PluginSdkboxPlay/PluginSdkboxPlay.h"
#include "PluginUnityAds/PluginUnityAds.h"

using namespace sdkbox;
#endif


#include "cocos2d.h"
/**
 @brief    The cocos2d Application.
 
 The reason for implement as private inheritance is to hide some interface call by Director.
 */
class  AppDelegate : private cocos2d::Application
#ifdef SDKBOX_ENABLED
,public sdkbox::IAPListener
//,sdkbox::AdMobListener
,public sdkbox::SdkboxPlayListener
,public sdkbox::UnityAdsListener
#endif
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    
    virtual void initGLContextAttrs() override;
    
    /**
     @brief    Implement Director and Scene init code here.
     @return true    Initialize success, app continue.
     @return false   Initialize failed, app terminate.
     */
    virtual bool applicationDidFinishLaunching() override;
    
    /**
     @brief  The function be called when the application enter background
     @param  the pointer of the application
     */
    virtual void applicationDidEnterBackground() override;
    
    /**
     @brief  The function be called when the application enter foreground
     @param  the pointer of the application
     */
    virtual void applicationWillEnterForeground()override;
    
    time_t backgroundEnterTime;
    

    
#ifdef SDKBOX_ENABLED
//    void adViewDidReceiveAd ( const std::string & name )  override;
//    void adViewDidFailToReceiveAdWithError ( const std::string & name ,const std::string & msg )  override;
//    void adViewWillPresentScreen ( const std::string & name )  override;
//    void adViewDidDismissScreen ( const std::string & name )  override;
//    void adViewWillDismissScreen ( const std::string & name )  override;
//    void adViewWillLeaveApplication ( const std::string & name )  override;
    
    virtual void onInitialized(bool ok) override;
    virtual void onSuccess(sdkbox::Product const& p) override;
    virtual void onFailure(sdkbox::Product const& p, const std::string &msg) override;
    virtual void onCanceled(sdkbox::Product const& p) override;
    virtual void onRestored(sdkbox::Product const& p) override;
    virtual void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;
    virtual void onProductRequestFailure(const std::string &msg) override;
    void onRestoreComplete(bool ok, const std::string &msg) override;
//
    void unityAdsDidClick(const std::string& placementId) ;
    void unityAdsPlacementStateChanged(const std::string& placementId,
                                       PluginUnityAds::SBUnityAdsPlacementState oldState,
                                       PluginUnityAds::SBUnityAdsPlacementState newState) ;
    void unityAdsReady(const std::string& placementId) ;
    void unityAdsDidError(sdkbox::PluginUnityAds::SBUnityAdsError error, const std::string& message) ;
    void unityAdsDidStart(const std::string& placementId);
    void unityAdsDidFinish(const std::string& placementId, sdkbox::PluginUnityAds::SBUnityAdsFinishState state) ;


    void onConnectionStatusChanged( int status );
    void onScoreSubmitted( const std::string& leaderboard_name, long score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday );
    void onIncrementalAchievementUnlocked( const std::string& achievement_name );
    void onIncrementalAchievementStep( const std::string& achievement_name, double step );
    void onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked );
    
//
#endif
  
    
};

#endif // _APP_DELEGATE_H_

