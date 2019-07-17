//
//  Ios_Billing.h
//  AsteroidInvaders2
//
//  Created by EAA van Poeijer on 23-12-12.
//
//

#import <Foundation/Foundation.h>
#import <Storekit/StoreKit.h>
//#import "GADBannerView.h"
#import "RootViewController.h"
//#import <AdSupport/ASIdentifierManager.h>
//#import <AdColony/AdColony.h>
#import "NativeInterface.h"
@interface Ios_BillingManager : NSObject<SKPaymentTransactionObserver,SKProductsRequestDelegate,SKRequestDelegate,SKStoreProductViewControllerDelegate> //< SKPaymentTransactionObserver,SKProductsRequestDelegate,SKRequestDelegate,SKStoreProductViewControllerDelegate>//, GADBannerViewDelegate, AdColonyAdDelegate>
{
    NSSet* availableItems;
    NSArray *myProducts;
    RootViewController* viewController;
    NSMutableDictionary *priceLocaleDic;
    NSMutableDictionary *priceAmountDic;
    NSMutableDictionary *priceCurrencyDic;
//    GADBannerView* bannerView_;
    
    NativeInterface::actionFinished billingAvailableCallback;
    NativeInterface::actionFinished loadSKUItemsCallback;
    NativeInterface::actionFinishedUTF8 itemPriceCallback;
    NativeInterface::actionFinished purchaseCallback;
    NativeInterface::actionFinished checkPurchasesCallback;
    NativeInterface::actionFinished consumePurchaseCallback;
    
    bool isBottom;
}

@property(retain) NSString *strSignedData;
@property(retain) NSSet* availableItems;
@property(retain) NSMutableArray* skuItems;
@property(retain) NSArray* myProducts;
//@property(retain) GADBannerView* bannerView_;
//@property(retain) GADBannerView* rectView_;
@property(retain) RootViewController* viewController;
@property(assign) NativeInterface::actionFinished billingAvailableCallback;
@property(assign) NativeInterface::actionFinished loadSKUItemsCallback;
@property(assign) NativeInterface::actionFinishedUTF8 itemPriceCallback;
@property(assign) NativeInterface::actionFinished purchaseCallback;
@property(assign) NativeInterface::actionFinished checkPurchasesCallback;
@property(assign) NativeInterface::actionFinished consumePurchaseCallback;

+ (Ios_BillingManager*)sharedManager;
- (id)init;

- (void) initialize;
- (void) requestProductData;
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response;
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;
- (void) completeTransaction: (SKPaymentTransaction *)transaction;
- (void) restoreTransaction: (SKPaymentTransaction *)transaction;
- (void) failedTransaction: (SKPaymentTransaction *)transaction;
- (void) purchase: (const char*) item code: (int) code;
- (SKProduct*) getProduct:(const char*)item;
- (void) restorePurchases;
- (void) showAds;
- (void) showTopAds;
- (void) hideAds;
- (void) showRectAds;
- (void) hideRectAds;
//- (void)adViewDidReceiveAd:(GADBannerView *)adView;
-(void)registerLocalNotification:(int)sec msg:(const char*)msg title:(const char*)title;
-(void)cancelAllLocalNotification;
-(const char*) getSkuDescription:(const char*) item;
-(const char*) getSkuPrice:(const char*) item;
-(const char*) getSkuDetails:(const char*) item;
-(const char*) getSkuTitle:(const char*) item;
-(const char*) getSkuType:(const char*) item;
-(NSString*) getPriceLocale:(NSString*) item;
-(NSString*) getPriceAmount:(NSString*) item;
-(NSString*) getPriceCurrency:(NSString*) item;
-(void) addSkuItem:(const char*) item;


@end
