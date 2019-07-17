/****************************************************************************
 Copyright (c) 2013      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#import "RootViewController.h"
#import "cocos2d.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "GameManager.h"
#import "Ios_BillingManager.h"
#import <FBSDKCoreKit/FBSDKCoreKit.h>


@implementation RootViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
    // Initialize the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [UIScreen mainScreen].bounds
                                         pixelFormat: (__bridge NSString *)cocos2d::GLViewImpl::_pixelFormat
                                         depthFormat: cocos2d::GLViewImpl::_depthFormat
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: cocos2d::GLViewImpl::_multisamplingCount > 0 ? YES : NO
                                     numberOfSamples: cocos2d::GLViewImpl::_multisamplingCount ];
    
    // Enable or disable multiple touches
    [eaglView setMultipleTouchEnabled:NO];
    
    // Set EAGLView as view of RootViewController
    self.view = eaglView;
    
    
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
//    [UnityAds initialize:@"2751873" delegate:self];
    [Ios_BillingManager sharedManager].viewController = self;
    
    // admob
    [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
    
    self.interstitial = [self createAndLoadInterstitial];
    // Initialize Google Mobile Ads SDK
    [[GADMobileAds sharedInstance]
     startWithCompletionHandler:^(GADInitializationStatus *_Nonnull status) {
//         GADAdapterInitializationState adapterState =
//         status.adapterStatusesByClassName[@"SampleAdapter"].state;
//         
//         if (adapterState == GADAdapterInitializationStateReady) {
//             // Sample adapter was successfully initialized.
//         } else {
//             // Sample adapter is not ready.
//         }
     }];
    self.rewardedAd = [self createAndLoadRewardedAd];
//    [FBSDKAppEvents logEvent:@"iOS launch"];
}

- (GADRewardedAd *)createAndLoadRewardedAd {
    GADRewardedAd *rewardedAd = [[GADRewardedAd alloc]
                                 initWithAdUnitID:@"ca-app-pub-7893694248975700/5182384895"];
    GADRequest *request = [GADRequest request];
    [rewardedAd loadRequest:request completionHandler:^(GADRequestError * _Nullable error) {
        if (error) {
            // Handle ad failed to load case.
            NSLog(@"admob load error: !%@", error);
        } else {
            // Ad successfully loaded.
            NSLog(@"admob loaded!");
            
        }
    }];
    return rewardedAd;
}

- (void)rewardedAdDidDismiss:(GADRewardedAd *)rewardedAd {
    self.rewardedAd = [self createAndLoadRewardedAd];
}
/// Tells the delegate that the user earned a reward.
- (void)rewardedAd:(GADRewardedAd *)rewardedAd userDidEarnReward:(GADAdReward *)reward {
    // TODO: Reward the user.
    NSLog(@"rewardedAd:userDidEarnReward:");
    GM->showVideoDone();
}

/// Tells the delegate that the rewarded ad was presented.
- (void)rewardedAdDidPresent:(GADRewardedAd *)rewardedAd {
    NSLog(@"rewardedAdDidPresent:");
}

/// Tells the delegate that the rewarded ad failed to present.
- (void)rewardedAd:(GADRewardedAd *)rewardedAd didFailToPresentWithError:(NSError *)error {
    NSLog(@"rewardedAd:didFailToPresentWithError");
}

- (GADInterstitial *)createAndLoadInterstitial {
    GADInterstitial *interstitial =
    [[GADInterstitial alloc] initWithAdUnitID:@"ca-app-pub-7893694248975700/8072751041"];
    interstitial.delegate = self;
    [interstitial loadRequest:[GADRequest request]];
    return interstitial;
}
/// Tells the delegate an ad request succeeded.
- (void)interstitialDidReceiveAd:(GADInterstitial *)ad {
    NSLog(@"interstitialDidReceiveAd1");
}

/// Tells the delegate an ad request failed.
- (void)interstitial:(GADInterstitial *)ad
didFailToReceiveAdWithError:(GADRequestError *)error {
    NSLog(@"interstitial:didFailToReceiveAdWithError: %@", [error localizedDescription]);
}

/// Tells the delegate that an interstitial will be presented.
- (void)interstitialWillPresentScreen:(GADInterstitial *)ad {
    NSLog(@"interstitialWillPresentScreen");
}

/// Tells the delegate the interstitial is to be animated off the screen.
- (void)interstitialWillDismissScreen:(GADInterstitial *)ad {
    NSLog(@"interstitialWillDismissScreen");
}

/// Tells the delegate the interstitial had been animated off the screen.
- (void)interstitialDidDismissScreen:(GADInterstitial *)interstitial {
    self.interstitial = [self createAndLoadInterstitial];
    NSLog(@"admob interstitial dismissed create and load it again!");
}

/// Tells the delegate that a user click will open another app
/// (such as the App Store), backgrounding the current app.
- (void)interstitialWillLeaveApplication:(GADInterstitial *)ad {
    NSLog(@"interstitialWillLeaveApplication");
}
- (void)showInterstitial{
    if (self.interstitial.isReady) {
        [self.interstitial presentFromRootViewController:self];
    }else{
        NSLog(@"not ready!");
    }
}
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}


// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
#ifdef __IPHONE_6_0
- (NSUInteger) supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskAllButUpsideDown;
}
#endif

- (BOOL) shouldAutorotate {
    return YES;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();

    if (glview)
    {
        CCEAGLView *eaglview = (__bridge CCEAGLView *)glview->getEAGLView();

        if (eaglview)
        {
            CGSize s = CGSizeMake([eaglview getWidth], [eaglview getHeight]);
            cocos2d::Application::getInstance()->applicationScreenSizeChanged((int) s.width, (int) s.height);
        }
    }
}

//fix not hide status on ios7
- (BOOL)prefersStatusBarHidden {
    return YES;
}

// Controls the application's preferred home indicator auto-hiding when this view controller is shown.
- (BOOL)prefersHomeIndicatorAutoHidden {
    return YES;
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];

    // Release any cached data, images, etc that aren't in use.
}

-(void)showUnityAds{
    NSLog(@"showUnityAds");
//    if ([UnityAds isReady:@"rewardedVideo"]) {
//        [UnityAds show:self placementId:@"rewardedVideo"];
//    }
//    [UnityAds show:self placementId:@"rewardedVideo"];
}
-(void)showVideoAds{
    NSLog(@"admob showVideoAds");
    if (self.rewardedAd.isReady) {
        [self.rewardedAd presentFromRootViewController:self delegate:self];
    } else {
        NSLog(@"Ad wasn't ready");
    }
}
//- (void)unityAdsReady:(NSString *)placementId{
//    NSLog(@"*unityAdsReady");
//}
//
//- (void)unityAdsDidError:(UnityAdsError)error withMessage:(NSString *)message{
//    NSLog(@"*unityAdsDidError");
//}
//
//- (void)unityAdsDidStart:(NSString *)placementId{
//}
//
//- (void)unityAdsDidFinish:(NSString *)placementId withFinishState:(UnityAdsFinishState)state{
//    if(state == kUnityAdsFinishStateCompleted){
//        GM->showVideoDone();
//    }
//}

@end
