/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
Copyright (c) 2015 Adrian Dawid
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.cpp.billing.BillingManager;
import org.cocos2dx.cpp.billing.BillingProvider;
import org.cocos2dx.lib.Cocos2dxActivity;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;
import com.google.android.gms.common.api.GoogleApiClient;
import com.magmon.cartooncraft.*;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.VisibleForTesting;
import android.widget.Toast;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.Games.*;
import android.content.Context;
import android.util.Log;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.games.GamesStatusCodes;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;
import com.google.android.gms.games.leaderboard.Leaderboards;
import com.magmon.cartooncraft.R;
import com.unity3d.ads.IUnityAdsListener;
import com.unity3d.ads.UnityAds;

import android.content.Intent;
import android.app.Activity;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import com.facebook.FacebookSdk;
import com.facebook.appevents.AppEventsLogger;
public class AppActivity extends BaseGameActivity  implements BillingProvider, IUnityAdsListener{
    static int currentID;
    static int currentScore;
    static int currentAchievementID;
    static boolean gpgAvailable;
    static String TAG = "CartoonCraft";
    static String[] leaderboardIDs;
    static String[] achievementIDs;
    static Context currentContext;
    public static native void callCppCallback();

    private MainViewController mViewController;
    private BillingManager mBillingManager;
    public static AppActivity mActivity;
    public static native void onIAPSuccess();
    public static native void onIAPFailed();

    public static native void onVideoDone();
    public AppEventsLogger logger;
    private String PlayerID = "";
    public String StrArg0 = "";
    private static Handler handler;
    private final static int HANDLER_FB_LOG = 0;
    @Override
    public void onSignInSucceeded(){
        gpgAvailable = true;
    }
    
    @Override
    public void onSignInFailed(){
        gpgAvailable = false;
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        String leaderboardIdsRaw = getString(R.string.leaderboards);
        String achievementIdsRaw = getString(R.string.achievements);
        
        leaderboardIDs = leaderboardIdsRaw.split(";");
        achievementIDs =  achievementIdsRaw.split(";");
        
        currentContext = this;
        
        super.onCreate(savedInstanceState);
        mActivity = this;
        // Start the controller and load game data
        mViewController = new MainViewController(this);
        // Create and initialize BillingManager which talks to BillingLibrary
        mBillingManager = new BillingManager(this, mViewController.getUpdateListener());
        logger = AppEventsLogger.newLogger(this);
        UnityAds.initialize(this, "2751873", this);

        handler = new Handler()
        {
            public void handleMessage(Message msg)
            {
                switch(msg.what)
                {
                    case HANDLER_FB_LOG:
                        logFBHandler();
                        break;
                }
            }
        };
        logger.logEvent("Android Launch");
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == 10001) {
            // Make sure the request was successful
            if (resultCode == RESULT_OK) {
                Log.d("IAP", "IAP success on AppActiviy.OnActivityResult");
            } else if (resultCode == RESULT_CANCELED) {
                Log.d("IAP", "IAP cancel on AppActiviy.OnActivityResult");
                // } else if (resultCode == RESULT_CANCELED) {

//                Toast.makeText(AppMainTest.this,
//                        "Sorry, you have canceled purchase Subscription.",
//                        Toast.LENGTH_SHORT).show();

            }
        }
    }

    /**
     * This function assumes logger is an instance of AppEventsLogger and has been
     * created using AppEventsLogger.newLogger() call.
     */
    public void logSentFriendRequestEvent () {
        logger.logEvent("sentFriendRequest");
    }

    public static void logFB(String msgToLog){
        Message msg = new Message();
        msg.what = HANDLER_FB_LOG;
        mActivity.StrArg0 = msgToLog;
        handler.sendMessage(msg);
    }
    private void logFBHandler(){
        logger.logEvent(StrArg0);
        Log.d("FB LOG", "log facebook " + StrArg0);
    }
    public static void showUnityAds(){
        if (UnityAds.isReady()) {
            Log.e(TAG, "showUnityAds");
            UnityAds.show(mActivity, "rewardedVideo");
        }
    }

    /* LISTENER */


    @Override
    public void onUnityAdsReady(final String zoneId) {
        Log.e(mActivity.TAG, "unityAdsReady: " + zoneId);
//            TextView statusText = (TextView) findViewById(R.id.unityads_example_statustext);
//            statusText.setText("");
//
//            DeviceLog.debug("onUnityAdsReady: " + zoneId);
//            Utilities.runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    // look for various default placement ids over time
//                    switch (zoneId) {
//                        case "video":
//                        case "defaultZone":
//                        case "defaultVideoAndPictureZone":
//                            interstitialPlacementId = zoneId;
//                            enableButton((Button) findViewById(R.id.unityads_example_interstitial_button));
//                            break;
//
//                        case "rewardedVideo":
//                        case "rewardedVideoZone":
//                        case "incentivizedZone":
//                            incentivizedPlacementId = zoneId;
//                            enableButton((Button) findViewById(R.id.unityads_example_incentivized_button));
//                            break;
//                    }
//                }
//            });
//
//            toast("Ready", zoneId);
    }

    @Override
    public void onUnityAdsStart(String zoneId) {
//            DeviceLog.debug("onUnityAdsStart: " + zoneId);
//            toast("Start", zoneId);
    }

    @Override
    public void onUnityAdsFinish(String zoneId, UnityAds.FinishState result) {
        Log.e(mActivity.TAG, "onUnityAdsFinish: " + zoneId);
        onVideoDone();
//            DeviceLog.debug("onUnityAdsFinish: " + zoneId + " - " + result);
//            toast("Finish", zoneId + " " + result);
    }

    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError error, String message) {
//            DeviceLog.debug("onUnityAdsError: " + error + " - " + message);
//            toast("Error", error + " " + message);
//
//            TextView statusText = (TextView) findViewById(R.id.unityads_example_statustext);
//            statusText.setText(error + " - " + message);
    }

    private void toast(String callback, String msg) {
//            Toast.makeText(getApplicationContext(), callback + ": " + msg, Toast.LENGTH_SHORT).show();
    }

    // unityads listener ends

    public static void buyItem(String skuID){
//        ArrayList<String> currentSubscriptionSku = new ArrayList<>();
//        currentSubscriptionSku.add(skuID);
        try {
            String purchaseToken = "inapp_" + "com_magmon_cartooncraft_" + skuID;
            mActivity.getBillingManager().consumeAsync(purchaseToken);
//        int response = mService.consumePurchase(3, "com.magmon.cartooncraft", purchaseToken);
//        if (response == 0){
//            Log.e(TAG, "Consume complete");
//        }


//        Log.e("Cartton Craft", "buyItem: " + skuID);
            mActivity.getBillingManager().initiatePurchaseFlow(skuID, BillingClient.SkuType.INAPP);
            
        }catch (Exception e) {

        }
    }
    @Override
    protected void onResume() {
        super.onResume();
        // Note: We query purchases in onResume() to handle purchases completed while the activity
        // is inactive. For example, this can happen if the activity is destroyed during the
        // purchase flow. This ensures that when the activity is resumed it reflects the user's
        // current purchases.
        try
        {
            if (mBillingManager != null
                    && mBillingManager.getBillingClientResponseCode() == BillingClient.BillingResponse.OK) {
                mBillingManager.queryPurchases();
            }
        }
        catch(Exception e){

        }
    }

    @Override
    public BillingManager getBillingManager() {
        return mBillingManager;
    }

    @Override
    public void onDestroy() {
        if (mBillingManager != null) {
            mBillingManager.destroy();
        }
        super.onDestroy();
    }

    void onBillingManagerSetupFinished() {

    }

    @VisibleForTesting
    public MainViewController getViewController() {
        return mViewController;
    }
    /*@brief Changes the actvie leaderboard
      @param The index of the leaderboard
    */
    static public void openLeaderboard(int leaderboardID){
         currentID = leaderboardID;
    }
    
    /*@brief This function opens the leaderboards ui for an leaderboard id*/
    static public void openLeaderboardUI(){
        if(gpgAvailable){
                ((AppActivity)currentContext).runOnUiThread(new Runnable() {
            public void run() {
                ((AppActivity)currentContext).startActivityForResult(Games.Leaderboards.getLeaderboardIntent(((AppActivity)currentContext).getGameHelper().getApiClient(), leaderboardIDs[currentID]),2);
            }
                });
        }
    }
    
    static public boolean isGPGSupported(){
//        Log.e("Cartton Craft", "gpgAcailable " + gpgAvailable);
        return gpgAvailable;
    }
    
    /*@brief Submits a score to the leaderboard that is currently actvie*/
    static public void submitScoreToLeaderboard(int score)
    {
        if(gpgAvailable){
        Games.Leaderboards.submitScore(((AppActivity)currentContext).getGameHelper().getApiClient(),leaderboardIDs[currentID],score);
        }
    }
    static public String getPlayerID(){
//        Log.e("Cartoon Craft", "AppActivity getPlayerID");
        try
        {
            if(gpgAvailable){
//            Log.e("Cartoon Craft", "AppActivity getPlayerID about to");
                if(mActivity.PlayerID.length() > 0){
                    return mActivity.PlayerID;
                }else{
                    GoogleApiClient client = mActivity.getGameHelper().getApiClient();
                    if(client == null){
                        return "Noname";
                    }else{
                        mActivity.PlayerID = Games.Players.getCurrentPlayerId(client);
                        return mActivity.PlayerID;
                    }

                }
            }
        }
        catch(Exception e){

        }
        return "notset";
    }
    static public boolean isVideoAvailable(){
        Log.e(TAG, "isVideoAvailable?");
        return UnityAds.isReady();
    }
    static public String getPlayerName(){
        if(gpgAvailable){
            GoogleApiClient client = mActivity.getGameHelper().getApiClient();
            if(client == null) {
                return "Noname";
            }else{
                return Games.Players.getCurrentPlayer(client).getDisplayName();
            }
        }
        return "noName";
    }
    static public void requestScoreFromLeaderboard()
    {
        if(gpgAvailable){
            Games.Leaderboards.loadCurrentPlayerLeaderboardScore(((AppActivity)currentContext).getGameHelper().getApiClient(), leaderboardIDs[currentID], LeaderboardVariant.TIME_SPAN_ALL_TIME, LeaderboardVariant.COLLECTION_PUBLIC).setResultCallback(new ResultCallback<Leaderboards.LoadPlayerScoreResult>() {
                @Override
                public void onResult(final Leaderboards.LoadPlayerScoreResult scoreResult) {
                    if (scoreResult.getStatus().getStatusCode() == GamesStatusCodes.STATUS_OK) {
                        AppActivity.currentScore = (int)scoreResult.getScore().getRawScore();
                        AppActivity.callCppCallback();
                    }
                }
            });
        }
    }

    static public int collectScore()
    {
        return AppActivity.currentScore;
    }
    
     /*@brief Shows the achievements ui*/
    static public void showAchievements() {
        if(gpgAvailable){
        ((AppActivity)currentContext).runOnUiThread(new Runnable() {
            public void run() {
                ((AppActivity)currentContext).startActivityForResult(Games.Achievements.getAchievementsIntent(((AppActivity)currentContext).getGameHelper().getApiClient()), 5);
            }
        });
        }
    }
    
    /*@brief Changes the actvie Achievement
      @param The index of the achievement in the list*/
    static public void openAchievement(int achievementID){
        currentAchievementID = achievementID;
    }
    
    static public void updateAchievement(int percentage){
        if(gpgAvailable){
       Games.Achievements.unlock(((AppActivity)currentContext).getGameHelper().getApiClient(), achievementIDs[currentAchievementID]);
        }
    }
    
    static public void exitGame()
    {
        Intent intent = new Intent(currentContext, MainActivity.class);
        MainActivity.exiting=true;
        currentContext.startActivity(intent);
    }
}


