/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.

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
import android.Manifest;
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.ProgressDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ComponentName;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentSender;
import android.content.ServiceConnection;
//import org.cocos2dx.cpp.MyCTAdEventListener;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Handler;
import android.provider.MediaStore;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.NotificationCompat;
import android.support.v4.content.FileProvider;
import android.telephony.TelephonyManager;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

//import com.facebook.appevents.AppEventsLogger;
//import com.facebook.applinks.AppLinkData;
//import com.chartboost.sdk.CBLocation;
//import com.chartboost.sdk.Chartboost;
//import com.facebook.appevents.AppEventsLogger;

import com.android.vending.billing.IInAppBillingService;
//import com.applovin.adview.AppLovinInterstitialAd;
//import com.applovin.adview.AppLovinInterstitialAdDialog;
//import com.applovin.sdk.AppLovinAd;
//import com.applovin.sdk.AppLovinAdDisplayListener;
//import com.applovin.sdk.AppLovinSdk;

import com.cloudtech.ads.callback.VideoAdLoadListener;
import com.cloudtech.ads.core.AdTemplateConfig;
//import com.cloudtech.ads.core.CTAdEventListener;
import com.cloudtech.ads.callback.CTAdEventListener;
import com.cloudtech.ads.core.CTError;
import com.cloudtech.ads.core.CTNative;
import com.cloudtech.ads.core.CTService;
import com.cloudtech.ads.core.CTVideo;
import com.cloudtech.ads.vo.AdsNativeVO;
import com.cloudtech.videoads.core.CTServiceVideo;
import com.cloudtech.videoads.core.VideoAdListener;
import com.facebook.FacebookSdk;
import com.facebook.appevents.AppEventsLogger;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.PendingResult;
import com.google.android.gms.common.api.ResultCallback;
//import com.google.android.gms.drive.Drive;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesStatusCodes;
import com.google.android.gms.games.leaderboard.Leaderboards;
import com.google.android.gms.games.snapshot.Snapshot;
import com.google.android.gms.games.snapshot.SnapshotMetadata;
import com.google.android.gms.games.snapshot.SnapshotMetadataChange;
import com.google.android.gms.games.snapshot.Snapshots;
//import com.google.android.gms.plus.Plus;

//import com.igaworks.adpopcorn.IgawAdpopcorn;
//import com.igaworks.interfaces.IgawRewardItem;
//import com.igaworks.interfaces.IgawRewardItemEventListener;
//import android.net.wifi.WifiInfo;
//import android.net.wifi.WifiManager;
//import android.telephony.TelephonyManager;
//import com.igaworks.IgawCommon;
//import com.igaworks.interfaces.IgawRewardItemEventListener;

//import com.nanoo.puzzleroyale.R;

import com.tapjoy.TJActionRequest;
import com.tapjoy.TJConnectListener;
import com.tapjoy.TJEarnedCurrencyListener;
import com.tapjoy.TJError;
import com.tapjoy.TJGetCurrencyBalanceListener;
import com.tapjoy.TJPlacement;
import com.tapjoy.TJPlacementListener;
import com.tapjoy.TJSpendCurrencyListener;
import com.tapjoy.Tapjoy;
import com.tapjoy.TapjoyConnectFlag;
import com.tapjoy.TapjoyLog;
import com.unity3d.ads.IUnityAdsListener;
import com.unity3d.ads.UnityAds;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Hashtable;
import java.util.Locale;
import java.util.Map;
import java.util.Random;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;


public class AppActivity extends Cocos2dxActivity implements IUnityAdsListener,
        GoogleApiClient.ConnectionCallbacks,
        GoogleApiClient.OnConnectionFailedListener, TJGetCurrencyBalanceListener,
        TJConnectListener/*,
        CTAdEventListener */{
    // in app billing native
    IInAppBillingService mService;

    ServiceConnection mServiceConn = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            mService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name,
                                       IBinder service) {
            mService = IInAppBillingService.Stub.asInterface(service);
        }
    };


    String TAG = "TapSabana";
    Boolean isFreeVersion = true;
    int googlePlayRetry = 0;
    Context mContext;
    int videoAdsKind = 0;
    int errorCounter = 0;
    //InterstitialAd mInterstitialAd;
    Boolean showImmediatly = false;
    static public AppActivity me;

//    public GoogleApiClient mGoogleApiClient;
    public static native void rewardUserForOfferWall(int coinCount);
    public static native void unityAdsCallback();
    public static native void playServiceConnected(boolean login);
    public static native void nativeactionFinished(long callback_address, int result);
    public static native void proveReciept(String purchase, String signature);
        public static native void alertToUser(String str);
    //private static native void setLoadedData(String key, String value);
    //private static native void resetGameData();
    private boolean insteadOfUnityAds = false;
    private static Handler handler;
    private String strArg0;
    private String strArg1;
    private int intArg0;
    private int intArg1;
    private int spendingBalance;
    public static int INIT_BILLING_NO = 0;
    public static int INIT_BILLING_YES = 1;
    public static int LOADSKUITEMS_ITEMS_YES = 2;
    public static int LOADSKUITEMS_ITEMS_NO = 3;
    public static int PURCHASE_SUCCESS = 4;
    public static int PURCHASE_ALREADY_PURCHASED = 5;
    public static int PURCHASE_FAIL = 6;
    public static int CONSUMEPURCHASE_YES = 9;
    public static int CONSUMEPURCHASE_NO = 10;
    public static int CONSUMEPURCHASE_NONCONSUMABLE = 11;
    public static long mCallback_address;
    public static boolean isPurchasing=false;
    private final static int HANDLER_LOAD_INTERSTITIAL = 0;
    private final static int HANDLER_SHOW_INTERSTITIAL = 1;
    private final static int HANDLER_SHOW_ACHIEVEMENTS = 2;
    private final static int HANDLER_UPDATE_ACHIEVEMENT = 3;
    private final static int HANDLER_UNLOCK_ACHIEVEMENT = 4;
    private final static int HANDLER_SHARE_PHOTO = 5;
    private final static int HANDLER_SHARE_LINK = 6;
    private final static int HANDLER_LOGIN_PLAY_SERVICE = 7;
    private final static int HANDLER_LOGOUT_PLAY_SERVICE = 8;
    private final static int HANDLER_OPEN_URL = 9;
    private final static int HANDLER_SHOW_UNITY_ADS_VIDEO = 10;
    private final static int HANDLER_SHOW_LEADERBOARDS = 11;
    private final static int HANDLER_UPDATE_TOP_SCORE = 12;
    private final static int HANDLER_SAVE_SNAPSHOT = 13;
    private final static int HANDLER_LOAD_SNAPSHOT = 14;
    private final static int HANDLER_ADPOPCORN= 15;
    private final static int HANDLER_TAPJOY= 16;
    private final static int HANDLER_SHOW_APPLOVIN_ADS_VIDEO = 17;
    private final static int HANDLER_ASK_ALERT = 18;
    private final static int NOTIFICATION_SERVICE = 19;
    private final static int HANDLER_CONSUME_IAP = 20;
    private final static int HANDLER_CLIPBOARD = 21;
    private final static int HANDLER_VIDEO_CAPTURE = 22;
    public String strCopyToClip;
    public static int OVERLAY_PERMISSION_REQ_CODE = 1234;
    private TJPlacement examplePlacement;
    private boolean _isUnityReady = true;
    private boolean _isTapjoyReady = true;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e(TAG, "onCreate.");
        mContext = this;
        me = this;


        handler = new Handler() {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case HANDLER_LOAD_INTERSTITIAL:
                        requestNewInterstitial();
                        break;
                    case HANDLER_SHOW_INTERSTITIAL:
                        me.showInterstitial();
                        break;
                    case HANDLER_SHOW_ACHIEVEMENTS:
                        showAchievementsHandler();
                        break;
                    case HANDLER_UPDATE_ACHIEVEMENT:
                        updateAchievementHandler();
                        break;
                    case HANDLER_UNLOCK_ACHIEVEMENT:
                        unlockAchievementHandler();
                        break;
                    case HANDLER_SHARE_PHOTO:
                        sharePhotoHandler();
                        break;
                    case HANDLER_SHARE_LINK:
                        shareLinkHandler();
                        break;
                    case HANDLER_LOGIN_PLAY_SERVICE:
                        loginPlayServiceHandler();
                        break;
                    case HANDLER_LOGOUT_PLAY_SERVICE:
                        logoutPlayServiceHandler();
                        break;
                    case HANDLER_OPEN_URL:
                        openUrlHandler();
                        break;
                    case HANDLER_SHOW_UNITY_ADS_VIDEO:
                        showUnityAdsVideoHandler();
                        break;
                    case HANDLER_SHOW_LEADERBOARDS:
                        showLeaderboardsHandler();
                        break;
                    case HANDLER_UPDATE_TOP_SCORE:
                        updateTopScoreLeaderboardHandler();
                        break;
                    case HANDLER_SAVE_SNAPSHOT:
//                        savedGamesUpdate();
                        break;
                    case HANDLER_LOAD_SNAPSHOT:
//                        savedGamesSelect();
                        break;
                    case HANDLER_ADPOPCORN:
//                        openIGAOfferWallHandler();
                        showTapjoyHandler();
                        break;
                    case HANDLER_TAPJOY:
                        showTapjoyHandler();
                        break;
                    case HANDLER_SHOW_APPLOVIN_ADS_VIDEO:
//                        showAppLovinVideoHandler();
                        break;
                    case HANDLER_ASK_ALERT:
                        askAllowAlertHandler();
                        break;
                    case HANDLER_CONSUME_IAP:
                        consumeItemHandler();
                        break;
                    case HANDLER_CLIPBOARD:
                        copyToClip();
                        break;
                    case HANDLER_VIDEO_CAPTURE:
                        captureVideo();
                        break;
                }
            }
        };

        // keep alive
        setKeepScreenOn(true);

//        if(_isGoogleReady) {
//            mGoogleApiClient = new GoogleApiClient.Builder(me)
//                    .addConnectionCallbacks(me)
//                    .addOnConnectionFailedListener(me)
//                .addApi(Plus.API).addScope(Plus.SCOPE_PLUS_LOGIN)
//                    .addApi(Games.API).addScope(Games.SCOPE_GAMES)
//                .addApi(Drive.API).addScope(Drive.SCOPE_APPFOLDER)
//                             add other APIs and scopes here as needed
//                    .build();
//        }


        if(_isUnityReady) {

            try {
//                Apple App Store: 1634533
//                Google Play Store: 1634534
                if (isFreeVersion) {
                    UnityAds.initialize(me, "1634534", me);
                } else {
//                UnityAds.init(me, "80617", this);
                }
//        UnityAds.setZone("rewardedVideoZone");
//        UnityAds.setDebugMode(true);
//        UnityAds.setTestMode(true);
            } catch (Exception e) {
                AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Ads not available").setMessage("Ads maybe blocked")
                        .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                dialog.dismiss();
                            }
                        }).create();
                dialog.show();
            }
        }
        //mInterstitialAd = new InterstitialAd(this);
        if (isFreeVersion){
            // playstore free ca-app-pub-4597682064467102/9705624671 // 1506
            //mInterstitialAd.setAdUnitId("ca-app-pub-3464491635186082/3014890254"); //tfjoy
//            mInterstitialAd.setAdUnitId("ca-app-pub-4597682064467102/9705624671"); // 1506
        }else{
//            mInterstitialAd.setAdUnitId("ca-app-pub-4597682064467102/1413315075");
        }

        /*mInterstitialAd.setAdListener(new AdListener() {
            @Override
            public void onAdClosed() {
                Message msg = new Message();
                msg.what = HANDLER_LOAD_INTERSTITIAL;
                handler.sendMessage(msg);
            }

            @Override
            public void onAdLoaded() {

                if (showImmediatly) {
                    Log.e("Admob", "ads loaded");
                    Message msg = new Message();
                    msg.what = HANDLER_SHOW_INTERSTITIAL;
                    handler.sendMessage(msg);

                    showImmediatly = false;
                }
            }
        });*/

        if (isFreeVersion) {
//            Message msg = new Message();
//            msg.what = HANDLER_LOAD_INTERSTITIAL;
            //handler.sendMessage(msg);
        }

//        IgawCommon.startApplication(AppActivity.this);

        if(_isTapjoyReady) {


            connectTapjoy();

//            examplePlacement = new TJPlacement(this, "PR Android placement", new TJPlacementListener() {
//                @Override
//                public void onRequestSuccess(TJPlacement placement) {
////                updateTextInUI("onRequestSuccess for placement " + placement.getName());
//                    Log.e("Tapjoy", "onRequestSuccess");
//                    if (!placement.isContentAvailable()) {
//
//                        Log.e("Tapjoy", "onRequestSuccess content not available");
////                    updateTextInUI("No content available for placement " + placement.getName());
//                    } else {
//                        Log.e("Tapjoy", "onRequestSuccess available");
//                        placement.showContent();
//                    }
//
////                setButtonEnabledInUI(buttonRequestPlacement, true);
//
//
//                }
//
//                @Override
//                public void onRequestFailure(TJPlacement placement, TJError error) {
//                    Log.e("Tapjoy", "onRequestFailure");
////                setButtonEnabledInUI(buttonRequestPlacement, true);
////                updateTextInUI("onRequestFailure for placement " + placement.getName() + " -- error: " + error.message);
//                }
//
//                @Override
//                public void onContentReady(TJPlacement placement) {
////                updateTextInUI("onContentReady for placement " + placement.getName());
////                setButtonEnabledInUI(buttonShowPlacement, true);
//                    if (placement.isContentReady()) {
////                    placement.showContent();
//                        Log.e("Tapjoy", "onRequestSuccesspp");
//                    } else {
//                        Log.e("Tapjoy", "onRequestSuccessoo");
//                        //handle situation where there is no content to show, or it has not yet downloaded.
//                    }
//                }
//
//                @Override
//                public void onContentShow(TJPlacement placement) {
//                    TapjoyLog.i(TAG, "onContentShow for placement " + placement.getName());
//                }
//
//                @Override
//                public void onContentDismiss(TJPlacement placement) {
////                setButtonEnabledInUI(buttonRequestPlacement, true);
////                setButtonEnabledInUI(buttonShowPlacement, false);
//
//                    TapjoyLog.e(TAG, "onContentDismiss for placement " + placement.getName());
//
//                    // Best Practice: We recommend calling getCurrencyBalance as often as possible so the user's balance is always up-to-date.
//                    Tapjoy.getCurrencyBalance(AppActivity.this);
//                }
//
//                @Override
//                public void onPurchaseRequest(TJPlacement placement, TJActionRequest request, String productId) {
//                    // Dismiss the placement content
//                    Intent intent = new Intent(getApplicationContext(), AppActivity.class);
//                    intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
//                    startActivity(intent);
//
//                    String message = "onPurchaseRequest -- product id: " + productId + ", token: " + request.getToken() + ", request id: " + request.getRequestId();
//                    AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Got on purchase request").setMessage(message)
//                            .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
//                                public void onClick(DialogInterface dialog, int whichButton) {
//                                    dialog.dismiss();
//                                }
//                            }).create();
//                    dialog.show();
//
//                    // Your app must call either callback.completed() or callback.cancelled() to complete the lifecycle of the request
//                    request.completed();
//                }
//
//                @Override
//                public void onRewardRequest(TJPlacement placement, TJActionRequest request, String itemId, int quantity) {
//                    // Dismiss the placement content
//                    Intent intent = new Intent(getApplicationContext(), AppActivity.class);
//                    intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
//                    startActivity(intent);
//
//                    String message = "onRewardRequest -- item id: " + itemId + ", quantity: " + quantity + ", token: " + request.getToken() + ", request id: " + request.getRequestId();
//                    AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Got on reward request").setMessage(message)
//                            .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
//                                public void onClick(DialogInterface dialog, int whichButton) {
//                                    dialog.dismiss();
//                                }
//                            }).create();
//                    dialog.show();
//
//                    // Your app must call either callback.completed() or callback.cancelled() to complete the lifecycle of the request
//                    request.completed();
//                }
//            });

//        updateTextInUI("Requesting placement content");

//            try {
//                // Get notifications whenever Tapjoy currency is earned.
//                Tapjoy.setEarnedCurrencyListener(new TJEarnedCurrencyListener() {
//                    @Override
//                    public void onEarnedCurrency(String currencyName, int amount) {
//                        Log.i("Tapjoy", "You've just earned " + amount + " " + currencyName);
//
//                        Tapjoy.spendCurrency(amount, new TJSpendCurrencyListener() {
//                            @Override
//                            public void onSpendCurrencyResponse(String currencyName, int balance) {
//                                Log.i("Tapjoy", currencyName + ": " + balance);
//                            }
//
//                            @Override
//                            public void onSpendCurrencyResponseFailure(String error) {
//                                Log.i("Tapjoy", "spendCurrency error: " + error);
//                            }
//                        });
//
//                    }
//                });
//            } catch (Exception e) {
//                AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Ads not available").setMessage("Ads maybe blocked")
//                        .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
//                            public void onClick(DialogInterface dialog, int whichButton) {
//                                dialog.dismiss();
//                            }
//                        }).create();
//                dialog.show();
//            }


            // facebook
        /*AppLinkData.fetchDeferredAppLinkData(this, "1058014817592758", new AppLinkData.CompletionHandler() {
            @Override
            public void onDeferredAppLinkDataFetched(AppLinkData appLinkData) {
//                App.setAppLinkData(appLinkData);  // Handles appLinkData. appLinkData is always null here.
            }
        });*/
            FacebookSdk.sdkInitialize(getApplicationContext());
            AppEventsLogger.activateApp(this);

            connectGoogle();

            // applovin
//        AppLovinSdk.initializeSdk(me);
//        AppLovinInterstitialAdDialog adDialog = AppLovinInterstitialAd.create(AppLovinSdk.getInstance(me), me);
            //adDialog.setAdDisplayListener(null);
            // Alternatively, you can create a new listener.
//
//        adDialog.setAdDisplayListener(new AppLovinAdDisplayListener() {
//            @Override
//            public void adDisplayed(AppLovinAd appLovinAd) {
//                // An interstitial ad was displayed.
//                Log.e(TAG, "Applovin displayed");
//            }
//            @Override
//            public void adHidden(AppLovinAd appLovinAd) {
//                // An interstitial ad was hidden.
//                unityAdsCallback();
//            }
//        });

            // in app billing native
            Intent serviceIntent =
                    new Intent("com.android.vending.billing.InAppBillingService.BIND");
            serviceIntent.setPackage("com.android.vending");
            bindService(serviceIntent, mServiceConn, Context.BIND_AUTO_CREATE);

            Tapjoy.setGcmSender("147786441351");
        }

        // yemobi

//        CTService.init(getApplicationContext(), yemobiSlotId);


//
//        CloudmobiSDK.setRewardedVideoListener(new RewardedVideoListener() {
//
//            @Override
//            public void onRewardedVideoAdOpened() {
//            }
//
//            @Override
//            public void onRewardedVideoAdClosed() {
//            }
//
//            @Override
//            public void onRewardedVideoAvailabilityChanged(boolean isAvailable) {
//            }
//
//            @Override
//            public void onRewardedVideoAdStarted() {
//            }
//
//            @Override
//            public void onRewardedVideoAdEnded() {
//
//            }
//
//            @Override
//            public void onRewardedVideoAdRewarded(Reward reward) {
//
//                unityAdsCallback();
//            }
//
//            @Override
//            public void onRewardedVideoAdShowFailed(CloudmobiError error) {
//            }
//        });
//        loadYemobi();
//        CloudmobiSDK.initRewardVideo(AppActivity.this);
        Log.e(TAG, "onCreate done");
    }

    // yemobi

    public CTVideo loadedCtVideo;
    public String yemobiSlotId = "83472931";
/*
    @Override
    public void onAdviewGotAdSucceed(CTNative result) {
        showMsg("onAdviewGotAdSucceed");
    }

    @Override
    public void onAdsVoGotAdSucceed(AdsNativeVO result) {
        showMsg("onAdsVoGotAdSucceed");
    }

    @Override
    public void onInterstitialLoadSucceed(CTNative result) {
        showMsg("onInterstitialLoadSucceed");
    }

    @Override
    public void onAdviewGotAdFail(CTNative result) {
        showMsg(result.getErrorsMsg());
        Log.i("sdksample", "errorMsg:" + result.getErrorsMsg());
    }

    @Override
    public void onAdviewIntoLandpage(CTNative result) {
        showMsg("onAdviewIntoLandpage");
    }

    @Override
    public void onStartLandingPageFail(CTNative result) {
        showMsg("onStartLandingPageFail");
    }

    @Override
    public void onAdviewDismissedLandpage(CTNative result) {
        showMsg("onAdviewDismissedLandpage");
    }

    @Override
    public void onAdviewClicked(CTNative result) {
        showMsg("onAdviewClicked");
    }

    @Override
    public void onAdviewClosed(CTNative result) {
        showMsg("onAdviewClosed");
    }

    @Override
    public void onAdviewDestroyed(CTNative result) {
        showMsg("onAdviewDestroyed");
    }


    private void showMsg(String msg){
        Toast.makeText(me, msg, Toast.LENGTH_SHORT).show();
    }
*/
    // yemobi ends

    private void captureVideo(){
        Log.e(me.TAG, "show video recording");
//        if(mGoogleApiClient == null){
//            Log.e(me.TAG, "mGoogleApiClient  is null");
//            alertToUser("need to login");
//            return;
//        }
//        if (!mGoogleApiClient.isConnected()){
//            Log.e(me.TAG, "mGoogleApiClient  is not connected");
//            alertToUser("need to login");
//            Log.e(me.TAG, "Alert user to login");
//            connectGoogle();
//            return;
//        }
//        Intent intent = Games.Videos.getCaptureOverlayIntent(me.mGoogleApiClient);
//        if(intent == null){
//            Log.e(me.TAG, "intent  is null");
//            return;
//        }
//        me.startActivityForResult(intent, 777);
//        Log.e(me.TAG, "show video recording done");
    }
    public static void showVideoRecording(){
        Log.e("showVideoRecording", "before handler");
        Message msg = new Message();
        msg.what = HANDLER_VIDEO_CAPTURE;
        handler.sendMessage(msg);
    }
    public static void purchase(final String item, final int amount,
                                final long callback_address) {
        mCallback_address = callback_address;
//        showPreviousPurchases();
        try {
            ArrayList<String> skuList = new ArrayList<String> ();
            skuList.add("premiumUpgrade");
            skuList.add("gas");
            Bundle querySkus = new Bundle();
            querySkus.putStringArrayList("ITEM_ID_LIST", skuList);

            Bundle skuDetails = me.mService.getSkuDetails(3, me.getPackageName(), "inapp", querySkus);

            int response = skuDetails.getInt("RESPONSE_CODE");
            if (response == 0) {
                ArrayList<String> ownedSkus = skuDetails
                        .getStringArrayList("INAPP_PURCHASE_ITEM_LIST");
                ArrayList<String> purchaseDataList = skuDetails
                        .getStringArrayList("INAPP_PURCHASE_DATA_LIST");
                ArrayList<String> signatureList = skuDetails
                        .getStringArrayList("INAPP_DATA_SIGNATURE");
                String continuationToken = skuDetails
                        .getString("INAPP_CONTINUATION_TOKEN");

                for (int i = 0; i < purchaseDataList.size(); ++i) {
                    String purchaseData = (String) purchaseDataList.get(i);
                    // String signature = (String)signatureList.get(i);
                    // String sku = (String)ownedSkus.get(i);

                    try {
                        JSONObject jo = new JSONObject(purchaseData);
                        String sku = jo.getString("productId");

                        String token = jo.getString("purchaseToken");
                        try {
                            // response =
                            // GoogleBilling.mService.consumePurchase(3,
                            // packageName, token);

                            // alert("You have bought the " + sku +
                            // ". Excellent choice, adventurer!");
                            // nativeactionFinished(GoogleBilling.mCallback_address,
                            // GoogleBilling.PURCHASE_SUCCESS);
                        } catch (Exception e) {
                            Log.d("AppActivity.java",
                                    "Exception in onActivityResult: "
                                            + e.toString());
                            nativeactionFinished(
                                    AppActivity.mCallback_address,
                                    AppActivity.PURCHASE_FAIL);
                            e.printStackTrace();
                        }

                        // alert("You have bought the " + sku +
                        // ". Excellent choice, adventurer!");
                    } catch (JSONException e) {
                        // alert("Failed to parse purchase data.");
                        e.printStackTrace();
                    }
                    // do something with this purchase information
                    // e.g. display the updated list of products owned by user
                }

                // if continuationToken != null, call getPurchases again
                // and pass in the token to retrieve more items
            }

        } catch (Exception e) {

        }

        try {
            mCallback_address = callback_address;
            isPurchasing = true;
            Log.d("GoogleBilling", "mCallback_address set/item: " + item);

            Bundle buyIntentBundle = me.mService.getBuyIntent(3, me.getPackageName(),
                    item, "inapp", "bGoa+V7g/yqDXvKRqq+JTFn4uQZbPiQJo4pf9RzJ");

//            String payload = random();
//            Bundle buyIntentBundle = mService.getBuyIntent(3,
//                    packageName, item, "inapp", payload);

            PendingIntent pendingIntent = buyIntentBundle.getParcelable("BUY_INTENT");

            me.startIntentSenderForResult(pendingIntent.getIntentSender(),
                    1001, new Intent(), Integer.valueOf(0), Integer.valueOf(0),
                    Integer.valueOf(0));
        } catch (Exception e) {
            Log.d("GoogleBilling", "Execption: " + e.toString());
            nativeactionFinished(mCallback_address, AppActivity.PURCHASE_FAIL);
//            AppActivity.dismissProgressDialog();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // in app billing native
        if (mService != null) {
            unbindService(mServiceConn);
        }
    }

    // tapjoy
    static public void trackEvent(String category, String name, String param1, String param2, String valueName, int value){
        Tapjoy.trackEvent(category, name, param1, param2, valueName, value);
        Log.d("Tapjoy", "Log: " + name);
    }
    public void connectTapjoy(){
        try {
            Hashtable<String, Object> connectFlags = new Hashtable<String, Object>();
            connectFlags.put(TapjoyConnectFlag.ENABLE_LOGGING, "true");
//            Tapjoy.setDebugEnabled(true);
            Tapjoy.connect(this.getApplicationContext(),
                    "lnlZdeGUSsm7IdWqeOxf9QECiZq2pmhd2nChvtPwnpmWww8mqEM_y8LEi1Kj",
                    connectFlags,
                    new TJConnectListener() {
                        @Override
                        public void onConnectSuccess() {
                            AppActivity.this.onConnectSuccess();
                        }

                        @Override
                        public void onConnectFailure() {
                            AppActivity.this.onConnectFailure();
                        }
                    });
        }catch(Exception e){
            AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Ads not available").setMessage("Ads maybe blocked")
                    .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            dialog.dismiss();
                        }
                    }).create();
            dialog.show();
        }
    }
    // called when Tapjoy connect call succeed
    @Override
    public void onConnectSuccess() {
        Log.d(TAG, "Tapjoy connect Succeeded");
    }
    // called when Tapjoy connect call failed
    @Override
    public void onConnectFailure() {
        Log.d("Tapjoy", "Tapjoy connect Failed");
    }
    @Override
    public void onGetCurrencyBalanceResponse(String currencyName, int balance) {
        Log.e("Tapjoy", "currencyName: " + currencyName);
        Log.e("Tapjoy", "balance: " + balance);

        spendingBalance = balance;
//        if (earnedCurrency) {
//            updateTextInUI(displayText + "\n" + currencyName + ": " + balance);
//            earnedCurrency = false;
//        } else {
//            updateTextInUI(currencyName + ": " + balance);
//        }
//        setButtonEnabledInUI(getCurrencyBalanceButton, true);

        Tapjoy.spendCurrency(balance, new TJSpendCurrencyListener() {
            @Override
            public void onSpendCurrencyResponse(String currencyName, int balance) {
                Log.e("Tapjoy", currencyName + ": " + balance);
                rewardUserForOfferWall(spendingBalance);
                dismissProgressDialog();
            }

            @Override
            public void onSpendCurrencyResponseFailure(String error) {
                Log.e("Tapjoy", "spendCurrency error: " + error);
            }
        });

    }

    @Override
    public void onGetCurrencyBalanceResponseFailure(String error) {
        Log.e("Tapjoy", "currency balance response failure: " + error);
//        updateTextInUI("getCurrencyBalance error: " + error);
//        setButtonEnabledInUI(getCurrencyBalanceButton, true);
    }

    public void showTapjoyHandler(){
        Log.e("Offerwall", "open Tapjoy");
        showProgressDialog("Loading...");
        if(Tapjoy.isConnected()) {
            examplePlacement.requestContent();
        } else {
            connectTapjoy();
            Log.e("%s", "Tapjoy SDK must finish connecting before requesting content.");
        }
    }

    static public void resetPushNotification(){
        NotificationManager mNotificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.cancelAll();
        Log.e("PUsh", "cancel all notis!");
    }
    static public void cancelNotifacation(String msg, int seconds){
        NotificationManager mNotificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.cancel(msg.length());
    }

    public static void Push_Notification(String message, int interval) {
        Log.v(me.TAG, "showLocalNotification tag: " + notiIndex + " : " + interval);
        int tag = notiIndex;
        PendingIntent sender = getPendingIntent(message, tag);
//        PendingIntent sender = getPendingIntent("index: " + notiIndex + " sec: " + interval, tag);
//        interval = 5; // test now
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.add(Calendar.SECOND, interval);

        AlarmManager am = (AlarmManager)me.getSystemService(ALARM_SERVICE);
        am.set(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), sender);
        notiIndex++;
    }
    public static int notiIndex = 0;
    public static void cancelLocalNotification(int tag) {
        Log.v(me.TAG, "cancelLocalNotification");
//        PendingIntent sender = getPendingIntent(null, tag);
//        AlarmManager am = (AlarmManager)me.getSystemService(ALARM_SERVICE);
//        am.cancel(sender);
        for(int i = 0;i<notiIndex;i++){
            PendingIntent sender = getPendingIntent(null, i);
            AlarmManager am = (AlarmManager)me.getSystemService(ALARM_SERVICE);
            am.cancel(sender);
        }
        notiIndex = 0;
    }

    private static PendingIntent getPendingIntent(String message, int tag) {
        Intent i = new Intent(me.getApplicationContext(), LocalNotificationReceiver.class);
        i.putExtra("notification_id", tag);
        i.putExtra("message", message);
        PendingIntent sender = PendingIntent.getBroadcast(me, 0, i, 0);
        return sender;
    }
    static public void Push_Notification_old(String msg, int seconds)
    {
        Log.e("PushNoti", "Sec: " + seconds);
//        resetPushNotification();
// Use NotificationCompat.Builder to set up our notification.
//        NotificationCompat.Builder builder = new NotificationCompat.Builder(me);
//
//        //icon appears in device notification bar and right hand corner of notification
//        builder.setSmallIcon(R.drawable.ic_action_noti_icon);
//        builder.setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE);
//        // This intent is fired when notification is clicked
//        Intent intent = new Intent(me, AppActivity.class);
//        PendingIntent pendingIntent = PendingIntent.getActivity(me, 0, intent, 0);
//
//        // Set the intent that will fire when the user taps the notification.
//        builder.setContentIntent(pendingIntent);
//
//        // Large icon appears on the left of the notification
//        boolean useWhiteIcon = (android.os.Build.VERSION.SDK_INT == android.os.Build.VERSION_CODES.LOLLIPOP);
//        builder.setLargeIcon(BitmapFactory.decodeResource(me.getResources(), useWhiteIcon ? R.mipmap.icon_silhouette : R.mipmap.ic_launcher));
//        builder.setAutoCancel(true);
//        // Content title, which appears in large type at the top of the notification
//        builder.setContentTitle(me.getResources().getString(R.string.app_name));
//        builder.setWhen(System.currentTimeMillis() + seconds * 1000);
//        // Content text, which appears in smaller text below the title
//        builder.setContentText(msg);

        // The subtext, which appears under the text on newer devices.
        // This will show-up in the devices with Android 4.2 and above only
        //builder.setSubText("Tap to view documentation about notifications.");


        // from blog
//        AlarmManager am = (AlarmManager)me.getSystemService(Context.ALARM_SERVICE);
//        Intent intent = new Intent(me, BroadcastD.class);
//
//        PendingIntent sender = PendingIntent.getBroadcast(me, 0, intent, 0);
//
//        Calendar calendar = Calendar.getInstance();
//        //알람시간 calendar에 set해주기
//
////        calendar.set(calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DATE), 23, 12, 0);
//
//        calendar.add(Calendar.SECOND, 5);
//        //알람 예약
//        am.set(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), sender);
        Log.e("Noti thread", "set alarm");

//        Calendar calendar = Calendar.getInstance();
//// 8.00 (8 AM)
////        calendar.set(Calendar.HOUR_OF_DAY, 8);
////        calendar.set(Calendar.MINUTE, 0);
////        calendar.set(Calendar.SECOND, 0);
//        calendar.add(Calendar.SECOND, 5);
//        PendingIntent pi = PendingIntent.getService(me.getContext(), 0 , new Intent(me.getContext(), AppActivity.class),PendingIntent.FLAG_UPDATE_CURRENT);
//        AlarmManager am = (AlarmManager) me.getContext().getSystemService(Context.ALARM_SERVICE);
//
//        am.set(AlarmManager.RTC_WAKEUP, System.currentTimeMillis() + 5000, pi);
////        am.setRepeating(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(),
////                AlarmManager.INTERVAL_FIFTEEN_MINUTES, pi);

//        Intent i = new Intent("singh.ajit.action.DISPLAY_NOTIFICATION");//new Intent(getContext(), BroadcastD.class);
////        i.putExtra(RemindersDbAdapter.KEY_ROWID, (long)taskId);
//        Calendar when= Calendar.getInstance();
//        when.add(Calendar.SECOND, 5);
//        AlarmManager mAlarmManager = (AlarmManager) me.getContext().getSystemService(Context.ALARM_SERVICE);
//        int broadcastID = 6666;//Integer.parseInt(taskId+""+when.getTime().getDate()+""+when.getTime().getYear()+""+taskId);
//        broadcastID = (int)when.getTimeInMillis();
//////        System.out.println("Alarm when : "+ when.getTimeInMillis());
//////
//////        PendingIntent pi = PendingIntent.getBroadcast(me, broadcastID, i, PendingIntent.FLAG_UPDATE_CURRENT);
//////        System.out.println("Alarm Set ID : "+broadcastID);
//////
//////        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
//////            mAlarmManager = (AlarmManager)getContext().getSystemService(Context.ALARM_SERVICE);
//////            AlarmManager.AlarmClockInfo ac= new AlarmManager.AlarmClockInfo(when.getTimeInMillis(), pi);
//////            mAlarmManager.setAlarmClock(ac, pi);
//////            System.out.println("Android 6.0 Marshmallow and above.");
//////
//////        }else if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
//////            mAlarmManager.setExact(AlarmManager.RTC_WAKEUP, when.getTimeInMillis(), pi);
//////            System.out.println("Android 4.4 Kikkat and above.");
//////        }else{
//////            mAlarmManager.set(AlarmManager.RTC_WAKEUP, when.getTimeInMillis(), pi);
//////            System.out.println("Android 4.3 Jelly Bean and below.");
//////        }
////
//////        // end blog
//////
//////        me.strArg0 = msg;
//////        me.intArg0 = seconds;
//////        new Thread(
//////                new Runnable() {
//////                    @Override
//////                    public void run() {
//////                        int incr;
//////                        NotificationCompat.Builder builder = new NotificationCompat.Builder(me);
//////                        NotificationManager notificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
//////
//////                        //icon appears in device notification bar and right hand corner of notification
//////                        builder.setSmallIcon(R.drawable.ic_action_noti_icon);
//                        builder.setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE);
//                        // This intent is fired when notification is clicked
//                        Intent intent = new Intent(me, AppActivity.class);
//                        PendingIntent pendingIntent = PendingIntent.getActivity(me, 0, intent, 0);
//
//                        // Set the intent that will fire when the user taps the notification.
//                        builder.setContentIntent(pendingIntent);
//
//                        // Large icon appears on the left of the notification
//                        boolean useWhiteIcon = (android.os.Build.VERSION.SDK_INT == android.os.Build.VERSION_CODES.LOLLIPOP);
//                        builder.setLargeIcon(BitmapFactory.decodeResource(me.getResources(), useWhiteIcon ? R.mipmap.icon_silhouette : R.mipmap.ic_launcher));
//                        builder.setAutoCancel(true);
//                        // Content title, which appears in large type at the top of the notification
//                        builder.setContentTitle(me.getResources().getString(R.string.app_name));
//                        builder.setWhen(System.currentTimeMillis() + me.intArg0 * 1000);
//                        // Content text, which appears in smaller text below the title
//                        builder.setContentText(me.strArg0);
//
//                        // Do the "lengthy" operation 20 times
////                        for (incr = 0; incr <= 100; incr+=5) {
//                            // Sets the progress indicator to a max value, the
//                            // current completion percentage, and "determinate"
//                            // state
////                            builder.setProgress(100, incr, false);
//                            // Displays the progress bar for the first time.
////                            notificationManager.notify(0, builder.build());
//                            // Sleeps the thread, simulating an operation
//                            // that takes time
//                            try {
//                                // Sleep for 5 seconds
//                                Log.e("Noti thread", "about to sleep");
////                                Thread.sleep(10*1000);
//                                Thread.sleep(me.intArg0*1000);
//                            } catch (InterruptedException e) {
//                                Log.d("Noti thread", "sleep failure");
//                            }
////                        }
//                        Log.e("Noti thread", "here2");
//                        // When the loop is finished, updates the notification
////                        builder.setContentText("Download complete")
//                                // Removes the progress bar
////                                .setProgress(0,0,false);
//                        notificationManager.notify(me.strArg0.length(), builder.build());
//                    }
//                }
//// Starts the thread by calling the run() method in its Runnable
//        ).start();
//        return;

//        NotificationManager notificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
//        // Will display the notification in the notification bar
//        Notification notification = builder.build();
//        notification.when = new Date().getTime() + seconds;
//        notificationManager.notify(msg.length(), notification);

        /*return;
        NotificationCompat.Builder builder = new NotificationCompat.Builder(
                this);
        Notification notification = builder.setContentIntent(contentIntent)
                .setSmallIcon(icon).setTicker(msg).setWhen(seconds)
                .setAutoCancel(true).setContentTitle(R.string.app_name)
                .setContentText(msg).build();
        NotificationManager mNotificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.notify(6789, notification);

        mNotificationManager.notify(7776, mNoti);
        //vibe.vibrate(1000);
return;
        Intent intent = new Intent(me, AppActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(me, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        Notification notification = new Notification();
//        notification.icon = R.drawable.ic_launcher;
        notification.tickerText = "tickerText";
        notification.when = new Date().getTime() + seconds;
        notification.number = 10;
        notification.setLatestEventInfo(me, "contentTitle", "contentText", pendingIntent);
        int notiId = msg.length();

        mNotificationManager.notify(notiId, notification);*/
    }
    // googlebilling
    public static boolean init(final long callback_address) {
        return true;
    }
    static public void enablePush(boolean enable){
        if (enable){

        }else{
            cancelAllLocalNotification();
            //Tapjoy.setPushNotificationDisabled(enable);
        }
    }
    static public String getUUID(){
        TelephonyManager tManager = (TelephonyManager)me.getSystemService(Context.TELEPHONY_SERVICE);
        String uuid = tManager.getDeviceId();
        return uuid;
    }
    static public String getUDID(){
        String android_id = Settings.Secure.getString(getContext().getContentResolver(),
                Settings.Secure.ANDROID_ID);

        return android_id;
    }
    static public String getHash(String data, String secret){
//        String message = "Sorry I Failed";
        String hash="";
        try {
//            message = "Message";

            Mac sha256_HMAC = Mac.getInstance("HmacSHA256");
            SecretKeySpec secret_key = new SecretKeySpec(secret.getBytes(), "HmacSHA256");
            sha256_HMAC.init(secret_key);

            hash = Base64.encodeToString(sha256_HMAC.doFinal(data.getBytes()), Base64.DEFAULT);
            System.out.println(hash);
            return hash;
        }
        catch (Exception e){
            System.out.println("Error");
        }
        return hash;
//        return message;
    }


    public void copyToClip(){

        Log.e("copyToClipboard", "str: " + strCopyToClip);
        ClipboardManager clipboard = (ClipboardManager) me.getSystemService(CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("label", strCopyToClip);
        clipboard.setPrimaryClip(clip);
    }
    static public void copyToClipboard(String str){
        Log.e("copyToClipboard", str);
        Message msg = new Message();
        msg.what = HANDLER_CLIPBOARD;
        me.strCopyToClip = str;
        handler.sendMessage(msg);
    }

    static public void cancelAllLocalNotification(){
        NotificationManager mNotificationManager = (NotificationManager) me.getSystemService(Context.NOTIFICATION_SERVICE);
        mNotificationManager.cancelAll();
        Log.e("PUsh", "cancel all notis!");
    }

    static public void showTapjoyOfferwall() {
        Log.e("Offerwall", "show Tapjoy");
        Message msg = new Message();
        msg.what = HANDLER_TAPJOY;
        handler.sendMessage(msg);
    }

    static public void showAdpopcornOfferwall(){
        Log.e("Offerwall", "show Adpopcorn");
        Message msg = new Message();
        msg.what = HANDLER_ADPOPCORN;
        handler.sendMessage(msg);
    }
    public String getAdpopcornDeviceId(Context context) {
        String adpopcornDeviceId = "";
        TelephonyManager tm = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
        if(tm != null){
            adpopcornDeviceId = tm.getDeviceId();

        } else {

            // Wifi 전용 기기의 경우 Mac Address를 사용.
            WifiManager wm = (WifiManager)context.getSystemService(Context.WIFI_SERVICE);
            if(wm != null) {

                WifiInfo wifiInfo = wm.getConnectionInfo();

                adpopcornDeviceId = wifiInfo.getMacAddress();
            }
        }
        return adpopcornDeviceId;
    }
    /*

        private void openIGAOfferWallHandler(){
            Log.d("Offerwall", "open Adpopcorn");
    //	    	this.openTapjoyOfferwall();

            IgawCommon.setUserId(this.getAdpopcornDeviceId(this));

            IgawCommon.setClientRewardEventListener(new IgawRewardItemEventListener() {

                @Override
                public void onGetRewardInfo(boolean isSuccese, String resultMessage, IgawRewardItem[] rewardItems) {

                    Log.d("Offerwall", "IGA rewardItems: "+rewardItems.toString());

                    for (int i = 0;i < rewardItems.length;i++){
                        Log.d("Offerwall", "IGA reward giving: "+ rewardItems[i].getRewardQuantity());

                        rewardUserForOfferWall(rewardItems[i].getRewardQuantity());
                        rewardItems[i].didGiveRewardItem();
                    }
                }

                @Override
                public void onDidGiveRewardItemResult(boolean isSuccess, String resultMessage, int resultCode, String completedRewardKey) {
                    // receive reward providing result
                    Log.d("Offerwall", "rewrad provided: " + resultMessage);
                }
            });

            IgawAdpopcorn.openOfferWall(AppActivity.this);
        }*/
    static public void showAds(){
        Log.e("Admob", "Show ads");
        Message msg = new Message();
        msg.what = HANDLER_SHOW_INTERSTITIAL;
        handler.sendMessage(msg);
    }
    private void showInterstitial(){
        /*if (me.mInterstitialAd.isLoaded()) {
            Log.e("Admob", "try to show ads");
            me.mInterstitialAd.show();
        }else{
            Log.e("Admob", "ads not loaded");
        }*/
    }
    private void requestNewInterstitial() {
        Log.e("Admob", "requestNewInterstitial");
        AdRequest adRequest = new AdRequest.Builder()
                .addTestDevice("SEE_YOUR_LOGCAT_TO_GET_YOUR_DEVICE_ID")
                .build();

        //mInterstitialAd.loadAd(adRequest);

        if (insteadOfUnityAds){
            insteadOfUnityAds = false;
//            unityAdsCallback();
        }
    }

    private void showAchievementsHandler(){
//        if(me.mGoogleApiClient.isConnected()){
//            me.startActivityForResult(Games.Achievements.getAchievementsIntent(me.mGoogleApiClient), 2);
//        }else{
//            loginPlayService();
//        }
    }
    static public void showAchievements(){
        Message msg = new Message();
        msg.what = HANDLER_SHOW_ACHIEVEMENTS;
        handler.sendMessage(msg);
    }
    private void updateAchievementHandler(){
//        if(me.mGoogleApiClient.isConnected()) {
//            Log.e("PD Achieve", "Achievement: " + strArg0 + ", " + intArg0);
//            if(intArg0 > 0){
//                Games.Achievements.setSteps(me.mGoogleApiClient, strArg0, intArg0);
//            }
//        }
    }
    static public void updateAchievement(String id, int count) {
        me.strArg0 = id;
        me.intArg0 = count;
        Message msg = new Message();
        msg.what = HANDLER_UPDATE_ACHIEVEMENT;
        handler.sendMessage(msg);
    }
    private void unlockAchievementHandler(){
//        if(me.mGoogleApiClient.isConnected()) {
//            Log.e("PD Unlock Achieve", "Achievement: " +  strArg0);
//            Games.Achievements.unlock(me.mGoogleApiClient, strArg0);
//        }
    }
    static public void unlockAchievement(String id){
        me.strArg0 = id;
        Message msg = new Message();
        msg.what = HANDLER_UNLOCK_ACHIEVEMENT;
        handler.sendMessage(msg);
    }
    public  boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(android.Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                Log.v(TAG,"Permission is granted");
                return true;
            } else {

                Log.v(TAG,"Permission is revoked");
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
                return false;
            }
        }
        else { //permission is automatically granted on sdk<23 upon installation
            Log.v(TAG,"Permission is granted");
            return true;
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if(grantResults[0]== PackageManager.PERMISSION_GRANTED){
            Log.v(TAG,"Permission: "+permissions[0]+ "was "+grantResults[0]);
            //resume tasks needing this permission
        }
    }
    private void sharePhotoHandler(){
        Log.e(me.TAG, "zabob sharePhoto: " + strArg0 + ", text: " + strArg1);
        if(!isStoragePermissionGranted()){ // permissino request in this function

        }

//        final String path = fileName;
//        final String tweetMessage = text +  "http://onelink.to/platformdefense";
//        shareMsg = "";
        me.ShareSNS(strArg1, strArg0);
//        Intent share = new Intent(android.content.Intent.ACTION_SEND);
//        share.setType("image/jpeg");
//
//        File imageFileToShare = new File(fileName);
//        Uri uri = Uri.fromFile(imageFileToShare);
//
//        share.putExtra(Intent.EXTRA_STREAM, uri);
//        me.startActivity(Intent.createChooser(share, "Share Image!"));

//        share.putExtra(Intent.EXTRA_TEXT, "http://onelink.to/platformdefense");
//        share.putExtra(Intent.EXTRA_TITLE, "http://onelink.to/platformdefense");
//        share.putExtra(Intent.EXTRA_SUBJECT,  "http://onelink.to/platformdefense");
    }
    // Zabob sns
    static public void sharePhoto(final String fileName, String text) {
        me.strArg0 = fileName;
        me.strArg1 = text;
        Message msg = new Message();
        msg.what = HANDLER_SHARE_PHOTO;
        handler.sendMessage(msg);
    }

    //SNS공유
    public void ShareSNS(String p_str, String p_img_path)
    {
        final String post_str = p_str;
        final String post_img_path = p_img_path;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                shareIntent(post_str, post_img_path);
            }
        });
    }


    private Intent shareIntent(String msg_str, String post_img_path) {
        //  List<Intent> targetedShareIntents = new ArrayList<Intent>();
        Intent shareIntent = new Intent(Intent.ACTION_SEND);
        shareIntent.setType("image/jpeg");


        shareIntent.putExtra(android.content.Intent.EXTRA_SUBJECT,
                "Tap Tap Sabana");


        this.deletePrevImage();

        if (!"".equals(post_img_path)) {
            this.addImageIntent(shareIntent, post_img_path);
        }
        shareIntent.putExtra(android.content.Intent.EXTRA_TEXT, msg_str );

        Intent chooserIntent = Intent.createChooser(shareIntent, "");

        startActivity(chooserIntent);
        return shareIntent;
    }

    private String tempImagePath = "";

    private void deletePrevImage()
    {
        if (!"".equals(tempImagePath)) {
            try
            {
                File file = new File(tempImagePath);
                if(file.exists()) {
                    boolean result = file.delete();
                    if (!result){
                        Log.e(me.TAG, "deletePrevImage failed");
//                        shareMsg += ",deletePrevImage failed";
                    }
                }

                getContentResolver().delete(Uri.parse(tempImagePath), null, null);
            }
            catch (Exception e)
            {
                Log.e(me.TAG, "deletePrevImage: " + e.toString());
//                shareMsg += ",deletePrevImage: " + e.toString();
            }
        }
    }

    //이미지를 인텐트에 붙입니다~!
    private void addImageIntent(Intent intent,String img_Path)
    {
        File imgFile = new  File(img_Path);
        if(imgFile.exists()){
            try {
//                shareMsg += ",1";
                Bitmap myBitmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath());

//                shareMsg += ",2," + imgFile.getAbsolutePath();
                ByteArrayOutputStream bytes = new ByteArrayOutputStream();
//                shareMsg += ",3";
                myBitmap.compress(Bitmap.CompressFormat.JPEG, 10, bytes);
//                shareMsg += ",4";
                String path = MediaStore.Images.Media.insertImage(getContentResolver(), myBitmap, "ScreenShot", null);
//                shareMsg += ",5";

//                if (false && Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
//                    ContentValues values=new ContentValues();
////                    values.put(MediaStore.Images.Media.TITLE, "Title");
////                    values.put(MediaStore.Images.Media.DESCRIPTION, "From Camera");
//                    Uri uriPath = getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
//                    intent.putExtra(Intent.EXTRA_STREAM, uriPath);
//                    shareMsg += ",new: " + uriPath;
//                }else{
                    tempImagePath = path;
//                    shareMsg += ",file: " + path;
                    intent.putExtra(Intent.EXTRA_STREAM, Uri.parse(path));
//                }

//                Bitmap bitmap =BitmapFactory.decodeFile(imgFile.getAbsolutePath());
//                try {
//                    File file = new File(this.getExternalCacheDir(),"logicchip.png");
//                    FileOutputStream fOut = new FileOutputStream(file);
//                    bitmap.compress(Bitmap.CompressFormat.PNG, 100, fOut);
//                    fOut.flush();
//                    fOut.close();
//                    file.setReadable(true, false);
//                    final Intent nintent = new Intent(android.content.Intent.ACTION_SEND);
//                    nintent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//                    nintent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file));
//                    nintent.setType("image/png");
//                    startActivity(Intent.createChooser(nintent, "Share image via"));
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }


//                        Log.e("shareimage", "shareimage added: " + shareMsg);
//                shareMsg += ",shareimage added";
            } catch (Exception e) {
                Log.e("shareimage", "addImageIntent: " + e.toString());
//                shareMsg += ",addImageIntent: " + e.toString();
            }
        }else{
            Log.e("shareimage", "shareimage not exist");
//            shareMsg += ",shareimage not exist";
        }
    }
//    public String shareMsg = "";
    // Zabob sns end

    private void shareLinkHandler(){
        Log.e(me.TAG, "shareLink: " + strArg0 + ", link: " + strArg1);
        Intent share = new Intent(android.content.Intent.ACTION_SEND);
        share.setType("text/plain");

        share.putExtra(Intent.EXTRA_SUBJECT, strArg0);
        share.putExtra(Intent.EXTRA_TITLE, strArg0);
        share.putExtra(Intent.EXTRA_TEXT, strArg1);

        me.startActivity(Intent.createChooser(share, "Share Game!"));
    }
    static public void shareLink(final String title, String link) {
        me.strArg0 = title;
        me.strArg1 = link;

        Message msg = new Message();
        msg.what = HANDLER_SHARE_LINK;
        handler.sendMessage(msg);
    }

    public static byte[] convertFileToByteArray(File f)
    {
        byte[] byteArray = null;
        try
        {
            InputStream inputStream = new FileInputStream(f);
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            byte[] b = new byte[1024*8];
            int bytesRead;

            while ((bytesRead = inputStream.read(b)) != -1)
            {
                bos.write(b, 0, bytesRead);
            }

            byteArray = bos.toByteArray();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        return byteArray;
    }
    private void loginPlayServiceHandler(){
        Log.e(me.TAG, "loginPlayService");
        me.signInClicked();
    }
    static public void loginPlayService(){
        Message msg = new Message();
        msg.what = HANDLER_LOGIN_PLAY_SERVICE;
        handler.sendMessage(msg);
    }
    private void logoutPlayServiceHandler(){
        Log.e(me.TAG, "logoutPlayService");
        me.signOutclicked();
    }
    static public void logoutPlayService(){
        Message msg = new Message();
        msg.what = HANDLER_LOGOUT_PLAY_SERVICE;
        handler.sendMessage(msg);
    }

    // saved game
    private ProgressDialog mProgressDialog;
    private static final int RC_SELECT_SNAPSHOT = 9002;
    private static final int APP_STATE_KEY = 0;
    //SaveGame mSaveGame = new SaveGame();
    private int dataCount = 0;
    public static void setIntForSavedGame(String key, int value){
        Log.e("SavedGame", "setInt: " + key + ", " + value);
//        me.mSaveGame.setIntValue(key, value);
    }
    public static void setStringForSavedGame(String key, String value) {
        Log.e("SavedGame", "setString: " + key + ", " + value);
//        me.mSaveGame.setStringValue(key, value);
    }
    public static void saveGame() {
        Message msg = new Message();
        msg.what = HANDLER_SAVE_SNAPSHOT;
        handler.sendMessage(msg);
    }
    public static void loadGame() {
        Message msg = new Message();
        msg.what = HANDLER_LOAD_SNAPSHOT;
        handler.sendMessage(msg);
    }
    /*private void savedGamesLoad(String snapshotName) {
        if(!mGoogleApiClient.isConnected()){
            signInClicked();
            return;
        }
        PendingResult<Snapshots.OpenSnapshotResult> pendingResult = Games.Snapshots.open(
                mGoogleApiClient, snapshotName, false);

        showProgressDialog("Loading Saved Game");
        ResultCallback<Snapshots.OpenSnapshotResult> callback =
                new ResultCallback<Snapshots.OpenSnapshotResult>() {
                    @Override
                    public void onResult(Snapshots.OpenSnapshotResult openSnapshotResult) {
                        if (openSnapshotResult.getStatus().isSuccess()) {
                            Log.e("SavedGame", "saved_games_load_success");
                            byte[] data = new byte[0];
                            try {
                                data = openSnapshotResult.getSnapshot().getSnapshotContents().readFully();
                            } catch (IOException e) {
                                Log.e("SavedGame", "Exception reading snapshot: " + e.getMessage());
                            }
                            mSaveGame.loadFromJson(new String(data));

//                            resetGameData();

                            dataCount = 0;
                            for (String key : mSaveGame.mKeyValueDic.keySet()) {
                                setLoadedData(key, mSaveGame.mKeyValueDic.get(key));
                                dataCount++;
                            }
                            Log.e("SavedGame", "Total Data:" + dataCount + ", Length: " + mSaveGame.toString().length());
//                            setData(new String(data));
//                            displaySnapshotMetadata(openSnapshotResult.getSnapshot().getMetadata());
                        } else {
//                            displayMessage(getString(R.string.saved_games_load_failure), true);
//                            clearDataUI();
                        }

                        dismissProgressDialog();
                    }
                };
        pendingResult.setResultCallback(callback);
    }
    private String makeSnapshotName(int appStateKey) {
        return "Snapshot-" + String.valueOf(appStateKey);
    }
    private void savedGamesUpdate() {
        if(!mGoogleApiClient.isConnected()){
            signInClicked();
            return;
        }
        dataCount = 0;
        for (String key : mSaveGame.mKeyValueDic.keySet()) {
            Log.e("SavedGame", ".setLoadedData: " + key + " ," + mSaveGame.mKeyValueDic.get(key));
            dataCount++;
        }
        Log.e("SavedGame", "Total Data:" + dataCount + ", Length: " + mSaveGame.toString().length());

        final String snapshotName = makeSnapshotName(APP_STATE_KEY);
        final boolean createIfMissing = true;

        // Use the data from the EditText as the new Snapshot data.
        final byte[] data = mSaveGame.toBytes();
        Log.e("SavedGame", "doInBackground 22");

        AsyncTask<Void, Void, Boolean> updateTask = new AsyncTask<Void, Void, Boolean>() {
            @Override
            protected void onPreExecute() {
                showProgressDialog("Updating Saved Game");
                Log.e("SavedGame", "Updating Saved Game");
            }

            @Override
            protected Boolean doInBackground(Void... params) {

                Snapshots.OpenSnapshotResult open = Games.Snapshots.open(
                        mGoogleApiClient, snapshotName, createIfMissing).await();

                Log.e("SavedGame", "data: " + mSaveGame.toString());

                if (!open.getStatus().isSuccess()) {
                    Log.e("SavedGame", "Could not open Snapshot for update.");
                    return false;
                }
                // Change data but leave existing metadata
                Snapshot snapshot = open.getSnapshot();
                snapshot.getSnapshotContents().writeBytes(data);

                Snapshots.CommitSnapshotResult commit = Games.Snapshots.commitAndClose(
                        mGoogleApiClient, snapshot, SnapshotMetadataChange.EMPTY_CHANGE).await();

                if (!commit.getStatus().isSuccess()) {
                    Log.e("SavedGame", "Failed to commit Snapshot.");
                    return false;
                }
                // No failures
                return true;
            }

            @Override
            protected void onPostExecute(Boolean result) {
                if (result) {
                    Log.e("SavedGame", "saved_games_update_success");
                } else {
                    Log.e("SavedGame", "saved_games_update_failure");
                }

                dismissProgressDialog();
//                clearDataUI();
            }
        };
        updateTask.execute();
    }
    private void savedGamesSelect() {
        if(!mGoogleApiClient.isConnected()){
            signInClicked();
            return;
        }
        final boolean allowAddButton = false;
        final boolean allowDelete = false;
        Intent intent = Games.Snapshots.getSelectSnapshotIntent(
                mGoogleApiClient, "Saved Games", allowAddButton, allowDelete,
                Snapshots.DISPLAY_LIMIT_NONE);

        showProgressDialog("Loading");
        startActivityForResult(intent, RC_SELECT_SNAPSHOT);
    }*/
    private void showProgressDialog(String msg) {
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(this);
            mProgressDialog.setIndeterminate(true);
        }

        mProgressDialog.setMessage(msg);
        mProgressDialog.show();
    }
    private void dismissProgressDialog() {
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
    }
    // saved game end
    private static final int REQUEST_RESOLVE_ERROR = 1001;
    private boolean mResolvingError = false;
    private String mCurrentSaveName = "snapshotTemp";
    private String consumeToken = "token";
    private String consumePurchase = "token";
    private String consumeSignature = "token";
    private void consumeItem(){
        Message msg = new Message();
        msg.what = HANDLER_CONSUME_IAP;
        handler.sendMessage(msg);
    }
    private void consumeItemHandler(){
        try {
            int response = me.mService.consumePurchase(3, me.getPackageName(), consumeToken);
//                        nativeactionFinished(AppActivity.mCallback_address,AppActivity.PURCHASE_SUCCESS);
            /*ClipboardManager clipboard = (ClipboardManager) me.getSystemService(CLIPBOARD_SERVICE);
            ClipData clip = ClipData.newPlainText("label", "purchase:" + consumePurchase +"/signature:" + consumeSignature);
            clipboard.setPrimaryClip(clip);*/

            proveReciept(consumePurchase, consumeSignature);

//                        AppActivity.dismissProgressDialog();
//                        IgawAdbrix.buy(sku);
        } catch (Exception e) {
            Log.d("AlmostLegendary.java",
                    "Exception in onActivityResult: "
                            + e.toString());
            nativeactionFinished(AppActivity.mCallback_address,
                    AppActivity.PURCHASE_FAIL);
//                        AppActivity.dismissProgressDialog();
            e.printStackTrace();
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.e("onActivityResult", "requestCode = " + requestCode);
        if(data != null){
            Log.e("onActivityResult", "data= " + data.toString());
        }else{
            Log.e("onActivityResult", "data null");
        }

        if (requestCode == 1001 && data != null) {
            int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
            String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
            String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");

            if (resultCode == RESULT_OK) {
                try {
                    JSONObject jo = new JSONObject(purchaseData);
                    String sku = jo.getString("productId");
                    String token = jo.getString("purchaseToken");
                    consumeToken = token;

                    String converted = Base64.encodeToString(purchaseData.getBytes(), Base64.DEFAULT);
                    consumePurchase = purchaseData;
//                    converted = Base64.encodeToString(dataSignature.getBytes(), Base64.DEFAULT);
                    byte[] array = Base64.decode(dataSignature, Base64.DEFAULT);
                    consumeSignature = dataSignature;
                    consumeItem();
                } catch (JSONException e) {
                    // alert("Failed to parse purchase data.");
                    nativeactionFinished(mCallback_address,
                            AppActivity.PURCHASE_FAIL);
//                    AppActivity.dismissProgressDialog();
                    e.printStackTrace();

                }
//                try {
//                    JSONObject jo = new JSONObject(purchaseData);
//                    String sku = jo.getString("productId");

//                    proveReciept(purchaseData, dataSignature);
//                    alert("You have bought the " + sku + ". Excellent choice,
//                            adventurer!");
//                }
//                catch (JSONException e) {
//                    alert("Failed to parse purchase data.");
//                    e.printStackTrace();
//                    nativeactionFinished(AppActivity.mCallback_address, AppActivity.PURCHASE_FAIL);
//                }
            }else{
                nativeactionFinished(mCallback_address,
                        AppActivity.PURCHASE_FAIL);
            }
        }
        try {

            /*if (requestCode == RC_SELECT_SNAPSHOT) {
                dismissProgressDialog();
                Log.d(TAG, "onActivityResult: RC_SELECT_SNAPSHOT, resultCode = " + resultCode);
                if (resultCode == RESULT_OK) {
                    // Successfully returned from Snapshot selection UI
                    if (intent != null) {
                        Bundle bundle = intent.getExtras();
                        SnapshotMetadata selected = Games.Snapshots.getSnapshotFromBundle(bundle);
                        if (selected == null) {
                            // No snapshot in the Intent bundle, display error message
                            Log.e("SavedGame", "saved_games_select_failure");
                            //setData(null); text box data
                            //displaySnapshotMetadata(null); brief mata data for the snapshot
                        } else {
                            // Found Snapshot Metadata in Intent bundle.  Load Snapshot by name.
                            String snapshotName = selected.getUniqueName();
                            savedGamesLoad(snapshotName);
                        }
                    }
                } else {
                    // User canceled the select intent or it failed for some other reason
                    Log.e("SavedGame","saved_games_select_cancel");
                    //setData(null); text box data
                    //displaySnapshotMetadata(null); brief mata data for the snapshot
                }
            }*/
            // saved game
      /*  boolean signedIn = (mGoogleApiClient != null && mGoogleApiClient.isConnected());
        if (intent != null && signedIn) {
            if (intent.hasExtra(Snapshots.EXTRA_SNAPSHOT_METADATA)) {
                // Load a snapshot.
                SnapshotMetadata snapshotMetadata = (SnapshotMetadata)
                        intent.getParcelableExtra(Snapshots.EXTRA_SNAPSHOT_METADATA);
                currentSaveName = snapshotMetadata.getUniqueName();


                this.loadFromSnapshot(snapshotMetadata);

                // Load the game data from the Snapshot
                // ...
            } else if (intent.hasExtra(Snapshots.EXTRA_SNAPSHOT_NEW)) {
                // Create a new snapshot named with a unique string
                String unique = new BigInteger(281, new Random()).toString(13);
                currentSaveName = "snapshotTemp-" + unique;


                // Create the new snapshot
                // ...
                // Load a snapshot.
                String conflictId = intent.getStringExtra(SelectSnapshotActivity.CONFLICT_ID);
                int retryCount = intent.getIntExtra(SelectSnapshotActivity.RETRY_COUNT,
                        3);
                SnapshotMetadata snapshotMetadata =
                        intent.getParcelableExtra(SelectSnapshotActivity.SNAPSHOT_METADATA);
                if (conflictId == null) {
                    saveSnapshot(null);
                } else {
                    Log.d(TAG,"resolving " + snapshotMetadata);
                    resolveSnapshotConflict(requestCode, conflictId, retryCount, snapshotMetadata);
                }
            }
        }
*/

            super.onActivityResult(requestCode, resultCode, data);
            if (requestCode == REQUEST_RESOLVE_ERROR) {
                mResolvingError = false;
                if (resultCode == RESULT_OK) {
                    // Make sure the app is not already connected or attempting to connect
//                    if (!mGoogleApiClient.isConnecting() &&
//                            !mGoogleApiClient.isConnected()) {

//                        connectGoogle();
//                    }
                }
            }
        }catch(Exception e){
            if (errorCounter > 1){
                return;
            }
            errorCounter++;
            AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Please email rightguy22@gmail.com").setMessage("Error: " + e.toString())
                    .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            dialog.dismiss();
                        }
                    }).create();
            dialog.show();
        }
        Log.e("onActivityResult", "onActivityResult Ended");
    }

    // Call when the sign-in button is clicked
    public void signInClicked() {
        mSignInClicked = true;
        googlePlayRetry = 0;
        connectGoogle();
    }

    // Call when the sign-out button is clicked
    public void signOutclicked() {
        mSignInClicked = false;
//        Games.signOut(mGoogleApiClient);
//        mGoogleApiClient.disconnect();
        playServiceConnected(false);
    }
    private void openUrlHandler(){
        Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(strArg0));
        me.startActivity(browserIntent);
    }
    static public void exitGame(){
        System.exit(0);
    }
    static public void openUrl(String url){
        me.strArg0 = url;
        Message msg = new Message();
        msg.what = HANDLER_OPEN_URL;
        handler.sendMessage(msg);
    }
    /*static public void logFacebookEent(int event){
        AppEventsLogger logger = AppEventsLogger.newLogger(me);
        if (event == 0){
            logger.logEvent("small_coin");
        }else if(event == 1){
            logger.logEvent("middle_coin");
        }else if(event == 2){
            logger.logEvent("big_coin");
        }else if(event == 3){
            logger.logEvent("huge_coin");
        }else if(event == 4){
            logger.logEvent("pd_all_characters");
        }else if(event == 5){
            logger.logEvent("pdf_remove_ads");
        }else if(event == 6){
            logger.logEvent("bree");
        }else if(event == 7){
            logger.logEvent("tom");
        }else if(event == 8){
            logger.logEvent("karen");
        }else if(event == 9){
            logger.logEvent("karl");
        }else if(event == 10){
            logger.logEvent("andrew");
        }else if(event == 11){
            logger.logEvent("unlockfriends");
        }
    }*/
    static public void showAppLovinVideo(){
        Message msg = new Message();
        msg.what = HANDLER_SHOW_APPLOVIN_ADS_VIDEO;
        handler.sendMessage(msg);
    }

//
//
//    private void showAppLovinVideoHandler(){
//
//        if(AppLovinInterstitialAd.isAdReadyToDisplay(me)){
//            // An ad is available to display.  It's safe to call show.
//            //AppLovinInterstitialAd.show(me);
//            final WeakReference<AppActivity> weakRef = new WeakReference<AppActivity>(this);
//            AppLovinInterstitialAdDialog interstitialAdDialog = AppLovinInterstitialAd.create(AppLovinSdk.getInstance(me.getApplicationContext()), weakRef.get());
//
//            Log.e("AppLovin", "Trying to show");
//            interstitialAdDialog.setAdDisplayListener(new AppLovinAdDisplayListener() {
//                @Override
//                public void adDisplayed(AppLovinAd appLovinAd) {
//                    Log.e(me.TAG, "Interstitial Displayed");
//                }
//
//                @Override
//                public void adHidden(AppLovinAd appLovinAd) {
//                    Log.e(me.TAG, "Interstitial Hidden");
//                    unityAdsCallback();
//                }
//            });
//
//            interstitialAdDialog.show();
//        }
//        else{
//            // No ad is available to display.  Perform failover logic...
//        }
//    }
    static public void askAllowAlert(){
        Message msg = new Message();
        msg.what = HANDLER_ASK_ALERT;
        handler.sendMessage(msg);
    }
    private void askAllowAlertHandler(){

    }
    private void showUnityAdsVideoHandler(){

        try {
//            Chartboost.showRewardedVideo("Level Complete");
//            Chartboost.showInterstitial("Default");
            Log.e(me.TAG, "Chartboost show");

            java.util.Random random = new java.util.Random();
            int index = random.nextInt(2);
            boolean isAvailable = CTServiceVideo.isRewardedVideoAvailable(loadedCtVideo);
            if (false && isAvailable ) {
                Log.i(TAG, "isAvailable: " + isAvailable);
//                CloudmobiSDK.showRewardedVideo(yemobiSlotId);

                CTServiceVideo.showRewardedVideo(loadedCtVideo, new VideoAdListener() {
                    @Override
                    public void videoPlayBegin() {
                        Log.e(TAG, "videoPlayBegin: ");
                    }

                    @Override
                    public void videoPlayFinished() {
                        Log.e(TAG, "videoPlayFinished: ");
                    }

                    @Override
                    public void videoPlayError(Exception e) {
                        Log.e(TAG, "videoPlayError: " + e);
                    }

                    @Override
                    public void videoClosed() {
                        Log.e(TAG, "videoClosed: ");
                    }

                    @Override
                    public void onRewardedVideoAdRewarded(String rewardName, String rewardAmount) {
                        Log.e(TAG, "onRewardedVideoAdRewarded: ");
                        unityAdsCallback();
                        loadYemobi();
                    }
                });
            } else if (UnityAds.isReady() || true) {
                Log.e(me.TAG, "showVideo");
                UnityAds.show(me);
//                loadYemobi();
            } else {
                /*insteadOfUnityAds = true;
                showImmediatly = true;
                if(mInterstitialAd.isLoaded()){
                    showInterstitial();
                }else{
                    requestNewInterstitial();
                }*/
                loadYemobi();
                Log.e(me.TAG, "cannot show");
            }
        }catch(Exception e){
            AlertDialog dialog = new AlertDialog.Builder(AppActivity.this).setTitle("Ads not available").setMessage("Ads maybe blocked")
                    .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            dialog.dismiss();
                        }
                    }).create();
            dialog.show();
        }
    }

    public void loadYemobi(){
        CTServiceVideo.setUserId("custom_id");
        CTServiceVideo.preloadRewardedVideo(getContext(), yemobiSlotId,
                new VideoAdLoadListener() {
                    @Override
                    public void onVideoAdLoaded(CTVideo ctVideo) {
                        loadedCtVideo = ctVideo;
                        Log.e(TAG, "onVideoAdLoaded: ");
                    }

                    @Override
                    public void onVideoAdLoadFailed(CTError error) {
                        Log.e(TAG, "onVideoAdLoadFailed: " + error.getMsg());
                    }
                });

    }
    static public void showUnityAdsVideo(){
        Message msg = new Message();
        msg.what = HANDLER_SHOW_UNITY_ADS_VIDEO;
        handler.sendMessage(msg);
    }
    // unityads start
    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError error, String message){

    }
    @Override
    public void onUnityAdsFinish(String placementId, UnityAds.FinishState result)
    {
        try{
            if(result == UnityAds.FinishState.SKIPPED) {
                Log.e(TAG, "video skipped");
            }else if(result == UnityAds.FinishState.COMPLETED){
                Log.e(TAG, "video complete");
                unityAdsCallback();;
            }
        }catch(Exception e) {

        }
    }
    @Override
    public void onUnityAdsReady(String placementId)
    {

    }
    @Override
    public void onUnityAdsStart(String placementId)
    {

    }
    /*
    @Override
    public void onVideoCompleted(String itemKey, boolean skipped){
        try{
            if(!skipped){
                Log.e(TAG, "video complete");
                unityAdsCallback();;
            }else{
                Log.e(TAG, "video skipped");
            }
        }catch(Exception e) {

        }
    }

    @Override
    public void onFetchCompleted() {

    }

    @Override
    public void onFetchFailed(){

    }
    @Override
    public void onVideoStarted(){

    }
    @Override
    public void onShow(){

    }

    @Override
    public void onHide(){

    }*/
    // unityads end
    private void showLeaderboardsHandler(){
        Log.e(me.TAG, "showLeaderboards on AppActivity.java" + strArg0);

//        if(me.mGoogleApiClient.isConnected()){
//            me.startActivityForResult(Games.Leaderboards.getLeaderboardIntent(me.mGoogleApiClient,
//                    strArg0), 2);//REQUEST_LEADERBOARD);
//        }else{
//            loginPlayService();
//        }
    }
    static public void showLeaderboards(String leaderboardID) {
        me.strArg0 = leaderboardID;
        Message msg = new Message();
        msg.what = HANDLER_SHOW_LEADERBOARDS;
        handler.sendMessage(msg);
    }
    private void updateTopScoreLeaderboardHandler(){
        Log.e(me.TAG, "post score on AppActivity.java");
//        if(me.mGoogleApiClient.isConnected()){
//            Games.Leaderboards.submitScore(me.mGoogleApiClient, strArg0, intArg0);
//        }
    }
    static public void updateTopScoreLeaderboard(int score, String leaderboardID) {
        me.intArg0 = score;
        me.strArg0 = leaderboardID;
        Message msg = new Message();
        msg.what = HANDLER_UPDATE_TOP_SCORE;
        handler.sendMessage(msg);
    }
    @Override
    protected void onPause() {
        super.onPause();
        AppEventsLogger.deactivateApp(this);
//        IgawCommon.endSession();
    }
    @Override
    protected void onResume() {
        super.onResume();
//        IgawCommon.startSession(AppActivity.this);
        //AppEventsLogger.activateApp(this);
        try{
            if(_isTapjoyReady) {
                Tapjoy.onActivityStart(this);
            }
        }catch(Exception e){

        }
    }
    @Override
    protected void onStart() {
        try{
            if(_isTapjoyReady) {
                Tapjoy.onActivityStop(this);
            }
        }catch(Exception e){

        }
        super.onStart();

//        me.mGoogleApiClient.connect();
        connectGoogle();
    }

    @Override
    protected void onStop() {
        super.onStop();
//        mGoogleApiClient.disconnect();
    }
    @Override
    public void onConnected(Bundle connectionHint) {
        // The player is signed in. Hide the sign-in button and allow the
        // player to proceed.
        Log.e(TAG, "tap onConnected:" + connectionHint);
        playServiceConnected(true);
    }
    private static int RC_SIGN_IN = 9001;

    private boolean mResolvingConnectionFailure = false;
    private boolean mAutoStartSignInflow = true;
    private boolean mSignInClicked = false;

    @Override
    public void onConnectionFailed(ConnectionResult connectionResult) {
        Log.e(TAG, "tap onConnectionFailed:" + connectionResult);
        playServiceConnected(false);
//        if (mResolvingConnectionFailure) {
//            // already resolving
//            Log.e(TAG, "tap onConnectionFailed return");
//            return;
//        }
        connectGoogle();
        if (connectionResult.hasResolution()) {
            try {
                Log.e(TAG, "tap onConnectionFailed try");
                connectionResult.startResolutionForResult(this, REQUEST_RESOLVE_ERROR);
                Log.e(TAG, "tap onConnectionFailed try safe");
            } catch (IntentSender.SendIntentException e) {
                connectGoogle();
                Log.e(TAG, "tap onConnectionFailed exception");
            }
        }


        // Put code here to display the sign-in button
    }
    private void connectGoogle(){
//        if(googlePlayRetry < 2){
//            mGoogleApiClient.connect();
//            googlePlayRetry++;
//        }
    }

    @Override
    public void onConnectionSuspended(int i) {
        // Attempt to reconnect
        connectGoogle();;
    }
}
