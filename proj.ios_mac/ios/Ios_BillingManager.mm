//
//  Ios_Billing.m
//  AsteroidInvaders2
//
//  Created by EAA van Poeijer on 23-12-12.
//
//

#import "Ios_BillingManager.h"
#import "GameSharing.h"



@implementation Ios_BillingManager


static Ios_BillingManager* mSelf = nil;

@synthesize availableItems;
@synthesize skuItems;
@synthesize myProducts;
@synthesize billingAvailableCallback;
@synthesize loadSKUItemsCallback;
@synthesize itemPriceCallback;
@synthesize purchaseCallback;
@synthesize checkPurchasesCallback;
@synthesize consumePurchaseCallback;
@synthesize viewController;
//@synthesize bannerView_;
//@synthesize rectView_;

+ (Ios_BillingManager*)sharedManager
{
    if (mSelf == nil)
    {
        mSelf = [[Ios_BillingManager alloc] init];
    }
    
    return mSelf;
}

- (id)init
{
    if (self = [super init])
    {
        skuItems = [NSMutableArray array];
        priceLocaleDic = [[NSMutableDictionary alloc] init];
        priceAmountDic = [[NSMutableDictionary alloc] init];
        priceCurrencyDic = [[NSMutableDictionary alloc] init];
        
        // Load the items
//        NSString* path = [[NSBundle mainBundle] pathForResource:@"skuitems"
//                                                         ofType:@"txt"];
//        NSArray* lines = [[NSString stringWithContentsOfFile:path
//                                                    encoding:NSUTF8StringEncoding
//                                                       error:nil]
//                          componentsSeparatedByString:@"\n"];
//        NSMutableArray* splitLines = [NSMutableArray array];
//        for (int count = 0; count < [lines count]; count++)
//        {
//            NSString* tmpString = (NSString*) [lines objectAtIndex:count];
//            NSArray *myWords = [tmpString componentsSeparatedByCharactersInSet:
//                                [NSCharacterSet characterSetWithCharactersInString:@";"]
//                                ];
//            [splitLines addObject:[myWords objectAtIndex:0]];
//        }
//
//        // Init the myProducts
//        availableItems = [[NSSet alloc] initWithArray:splitLines];
//
//        [availableItems retain];
//        for (int x = 0; x < [availableItems count];x++)
//        {
//            NSLog((NSString*)[splitLines objectAtIndex:x ]);
//        }
        isBottom = false;
        return self;
    }
    return nil;
}

- (void) initialize
{
    if ([SKPaymentQueue canMakePayments]) {
//        if (self.billingAvailableCallback)
        {
            // Init the rest
            [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
//            (*self.billingAvailableCallback)(NativeInterface::INIT_BILLING_YES);
        }
    }
    else
    {
//        (*self.billingAvailableCallback)(NativeInterface::INIT_BILLING_NO);
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
            default:
                break;
        }
    }
}
//Purchase complete
- (void) completeTransaction: (SKPaymentTransaction *)transaction
{
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    if (!receipt) {
        /* No local receipt -- handle the error. */
//        (*self.purchaseCallback)(NativeInterface::PURCHASE_FAIL);
        GameSharing::onIAPFailed();
        return;
    } else {
        //        NSLog(@"SKPaymentTransactionStatePurchased receipt : %@", receipt);
        //        NSString *encReceipt = [receipt base64EncodedStringWithOptions:0];
        //        NSLog(@"SKPaymentTransactionStatePurchased encReceipt : %@", encReceipt);
        
        //[self callDelegateSuccess:receipt];
        //[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    }
    
    
    // Your application should implement these two methods.
    // [self recordTransaction:transaction];
    //[self provideContent:transaction.payment.productIdentifier];
    
    NSLog(@"complete transaction");
    // Remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    NSString *myString = [[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding];
    NSLog(@"myString : %@", myString);
    NSString* productID = transaction.payment.productIdentifier;
    NSLog(@"productIdentifiter : %@", productID);
    
    NSString *encReceipt = [transaction.transactionReceipt base64EncodedStringWithOptions:0];
    
    NSLog(@"SKPaymentTransactionStatePurchased encReceipt : %@", encReceipt);
    
//    GameManager::getInstance()->verifyReceipt([encReceipt UTF8String], "");
//
//    return;
    
    NSString* amount = priceAmountDic[productID];
    NSString* currency = priceCurrencyDic[productID];
    self.strSignedData = [NSString stringWithFormat:@"{\"purchaseToken\":\"%@\",\"productId\":\"%@\",\"packageName\":\"com.magmon.cartooncraft\",\"developerPayload\":\"studionappayload_%@_%@\"}", encReceipt, productID, amount, currency];
    
//    if(self.purchaseCallback){
//        (*self.purchaseCallback)(NativeInterface::PURCHASE_SUCCESS);
//    }
    GameSharing::onIAPSuccess();
}


 //  Purchase succesfully restored
- (void) restoreTransaction: (SKPaymentTransaction *)transaction
{
    NSLog(@"restore transaction");
    //[self recordTransaction: transaction];
    //[self provideContent: transaction.originalTransaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
//    GameSharing::onIAPSuccess();
//    if(self.purchaseCallback){
//        (*self.purchaseCallback)(NativeInterface::PURCHASE_SUCCESS);
//    }
}

//Purchase failed
- (void) failedTransaction: (SKPaymentTransaction *)transaction
{
    NSLog(@"failed transaction");
    if (transaction.error.code != SKErrorPaymentCancelled) {
        // Optionally, display an error here.
    }
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
//    if(self.purchaseCallback){
//        (*self.purchaseCallback)(NativeInterface::PURCHASE_FAIL);
//    }
    GameSharing::onIAPFailed();
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    
    NSLog(@"Failed to load list of products.");
//    _productsRequest = nil;
    
//    _completionHandler(NO, nil);
//    _completionHandler = nil;
    
}

//Get the product from the local inventory
- (SKProduct*) getProduct:(const char *)item
{
    NSString* itemString = [NSString stringWithUTF8String:item];
    for (id object in self.myProducts)
    {
        SKProduct* tmpProduct = object;
        if ([itemString isEqualToString: tmpProduct.productIdentifier])
        {
            return tmpProduct;
        }
    }
    return NULL;
}
//Purchase item
- (void) purchase:(const char *)item code: (int) code
{
    if (code == 1)
    {
        [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    }
    else
    {
        SKProduct* reqProduct = [self getProduct:item];
        
        if (reqProduct != NULL)
        {
            SKPayment *payment = [SKPayment paymentWithProduct:reqProduct];
            [[SKPaymentQueue defaultQueue] addPayment:payment];
        }
        else
        {
//            (*self.purchaseCallback)(NativeInterface::PURCHASE_FAIL);
            GameSharing::onIAPFailed();
        }
    }
}

-(void)showAds{
    NSLog(@"show ads");
    /*
    [viewController.view addSubview:bannerView_];
    isBottom = !isBottom;
    if (true){//isBottom) {
        bannerView_.frame =CGRectMake(0,//viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                      viewController.view.frame.size.width,
                                      bannerView_.frame.size.width,
                                      bannerView_.frame.size.height);
        [UIView animateWithDuration:0.6 animations:^ {
            // Final frame of ad should be docked to bottom of screen
            bannerView_.frame = CGRectMake(0,//viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                           viewController.view.frame.size.width -
                                           bannerView_.frame.size.height,
                                           bannerView_.frame.size.width,
                                           bannerView_.frame.size.height);
            
        }];
    }else{
        bannerView_.frame =CGRectMake(viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                      -bannerView_.frame.size.height,
                                      bannerView_.frame.size.width,
                                      bannerView_.frame.size.height);
        [UIView animateWithDuration:0.6 animations:^ {
            bannerView_.frame = CGRectMake(viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                           0.0,
                                           bannerView_.frame.size.width,
                                           bannerView_.frame.size.height);
            
        }];
    }*/
}
-(void)showTopAds{
    /*NSLog(@"show ads");
    
    [viewController.view addSubview:bannerView_];
    isBottom = !isBottom;
    bannerView_.frame =CGRectMake(viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                  -bannerView_.frame.size.height,
                                  bannerView_.frame.size.width,
                                  bannerView_.frame.size.height);
    [UIView animateWithDuration:0.6 animations:^ {
        bannerView_.frame = CGRectMake(viewController.view.frame.size.height/2 - bannerView_.frame.size.width/2,
                                       0.0,
                                       bannerView_.frame.size.width,
                                       bannerView_.frame.size.height);
     
    }];*/
}
-(void)hideAds{
    /*NSLog(@"destroy ads");
    if (rand()%4>1) {
        [bannerView_ loadRequest:[GADRequest request]];
    }
    
    [bannerView_ removeFromSuperview];*/
}

-(void)showRectAds{
    NSLog(@"show rect ads");
    /*
    [viewController.view addSubview:rectView_];
    
    rectView_.frame =CGRectMake(viewController.view.frame.size.height + rectView_.frame.size.width,
                                  viewController.view.frame.size.width/2 - rectView_.frame.size.height/2,
                                  rectView_.frame.size.width,
                                  rectView_.frame.size.height);
    [UIView animateWithDuration:0.3 animations:^ {
        rectView_.frame = CGRectMake(viewController.view.frame.size.height*2/3 - rectView_.frame.size.width/2,
                                       viewController.view.frame.size.width/2 - rectView_.frame.size.height/2,
                                       rectView_.frame.size.width,
                                       rectView_.frame.size.height);
        
    }];*/
    
}
-(void)hideRectAds{
    NSLog(@"destroy ads");
    /*if (rand()%4>1) {
        [rectView_ loadRequest:[GADRequest request]];
    }
    
    [rectView_ removeFromSuperview];*/
}

-(void) restorePurchases;
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

//Request the productdata/items from apple
- (void) requestProductData
{
    SKProductsRequest *request= [[SKProductsRequest alloc] initWithProductIdentifiers:
                                 [[NSSet alloc] initWithArray:skuItems]];
                                  //[NSSet setWithObjects:@"goldenticket1", @"premium_retry", nil]];
    request.delegate = mSelf;
    [request start];
}

 //Items requested
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    myProducts = response.products;
    [myProducts retain];
    //SKProduct* myProd =  [myProducts objectAtIndex:0];
    
    if ([myProducts count] > 0)
    {
        for (SKProduct* product in response.products) {
            // Handle any invalid product identifiers.
            NSLog(@"here it comes! %@", product.price);
            
            NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
            [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
            [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
            [numberFormatter setLocale:product.priceLocale];
            NSString *formattedString = [numberFormatter stringFromNumber:product.price];
            
            NSLog(@"here it comes! %@, identifiter: %@, %@, %@", formattedString, product.productIdentifier, product.price, product.priceLocale.currencyCode);
            [priceLocaleDic setValue:formattedString forKey:product.productIdentifier];
            [priceAmountDic setValue:product.price forKey:product.productIdentifier];
            [priceCurrencyDic setValue:product.priceLocale.currencyCode forKey:product.productIdentifier];
            
            //self.itemPriceCallback([product.productIdentifier UTF8String], [[product.price stringValue] UTF8String]);
        }
//        (*self.loadSKUItemsCallback)(NativeInterface::LOADSKUITEMS_ITEMS_YES);
    }else{
        NSLog(@"NO IAP ITEM IS FOUND");
    }
//    else if (self.loadSKUItemsCallback && [myProducts count] == 0)
//    {
//        (*self.loadSKUItemsCallback)(NativeInterface::LOADSKUITEMS_ITEMS_NO);
//    }
}
- (void) addSkuItem:(const char*) item
{
    [skuItems addObject:[NSString stringWithUTF8String:item]];
}
-(NSString*) getPriceLocale:(NSString*) item{
    return [priceLocaleDic valueForKey:item];
}
-(NSString*) getPriceAmount:(NSString*) item{
    return [priceAmountDic valueForKey:item];
}
-(NSString*) getPriceCurrency:(NSString*) item{
    return [priceCurrencyDic valueForKey:item];
}
-(const char*) getSkuDescription:(const char*) item
{
    SKProduct* prod = [self getProduct:item];
    NSString* result = prod.description;
    return result ? [result UTF8String] : "";
}

-(const char*) getSkuPrice:(const char*) item
{    SKProduct* prod = [self getProduct:item];
    if (prod != NULL)
    {
        NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:prod.priceLocale];
        NSString *formattedString = [numberFormatter stringFromNumber:prod.price];
        return [formattedString UTF8String];
    }
    return "";
}
-(const char*) getSkuDetails:(const char*) item
{
    SKProduct* prod = [self getProduct:item];
    NSString* result = prod.productIdentifier;
    return result ? [result UTF8String] : "";
}
-(const char*) getSkuTitle:(const char*) item
{
    SKProduct* prod = [self getProduct:item];
    NSString* result = prod.localizedTitle;
    return result ? [result UTF8String] : "";
}

-(const char*) getSkuType:(const char*) item
{
    SKProduct* prod = [self getProduct:item];
    NSString* result = prod.productIdentifier;
    return result ? [result UTF8String] : "";
}

// Is called when AdColony has finished trying to show an ad, either successfully or unsuccessfully
// If shown == YES, an ad was displayed and apps should implement app-specific code such as unpausing a game and restarting app music
- ( void ) onAdColonyAdAttemptFinished:(BOOL)shown inZone:( NSString * )zoneID {
	if (shown) {
        //		[audio play];
	} else {
		cocos2d::MessageBox("Today's ads are all played. Or it is just an error. Try later.", "Failed to play");
	}
}


// 알림 등록 메소드
-(void)registerLocalNotification:(int)sec msg:(const char*)msg title:(const char*)title{
    // UILocalNotification 객체 생성
    UILocalNotification *noti = [[UILocalNotification alloc]init];
    
    // 알람 발생 시각 설정. 5초후로 설정. NSDate 타입.
    noti.fireDate = [NSDate dateWithTimeIntervalSinceNow:sec];
    
    // timeZone 설정.
    noti.timeZone = [NSTimeZone systemTimeZone];
    
    // 알림 메시지 설정
    noti.alertBody = [NSString stringWithUTF8String: msg];
    
    // 알림 액션 설정
    noti.alertAction = [NSString stringWithUTF8String: title];
    
    // 아이콘 뱃지 넘버 설정. 임의로 1 입력
    //    noti.applicationIconBadgeNumber = 1;
    
    // 알림 사운드 설정. 자체 제작 사운드도 가능. (if nil = no sound)
    noti.soundName = UILocalNotificationDefaultSoundName;
    
    // 임의의 사용자 정보 설정. 알림 화면엔 나타나지 않음
    noti.userInfo = [NSDictionary dictionaryWithObject:@"superretrobros" forKey:@"name"];
    
    // UIApplication을 이용하여 알림을 등록.
    [[UIApplication sharedApplication] scheduleLocalNotification:noti];
}
-(void)cancelAllLocalNotification{
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
}
@end
