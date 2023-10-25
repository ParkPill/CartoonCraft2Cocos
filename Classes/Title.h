//
//  Title.hpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 13/03/2018.
//

#ifndef Title_hpp
#define Title_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "TopBar.h"
#include "PageBase.h"
#include "NetworkWebSocket.h"

#define DEFAULT_LEAGUE_INDEX 3
using namespace cocos2d;
using namespace cocos2d::ui;
class Title : public PageBase
{
private:
    cocos2d::Size size;
    int _registerNameLimitStage = 3;
    bool isBeginnerLockOn = false;
    bool isServerCheckArrived = false;
    int selectedServer = 0;
public:
    Sprite* sptBackground;
    Sprite* sptTitle;
    Button* btnGoogle;
    
    virtual bool init();
    CREATE_FUNC(Title);
    void onServerSelect();
    void onHeroClick();
    void showHeroPage(bool showAlert = false);
    void onChapterClick(Ref* ref);
    void showChapterSelect();
    void onOptionClick();
    void onLoadClick();
    void onLoadSlotClick(Ref* ref);
    void onOkFromLoadData();
    void goToLoadedStage();
    void onArenaClick();
    void onMultiplayClick();
    void onMultiplayClickAndSearch();
    
    int selectedSaveSlot;
    
    bool isGameCenterLoginRequestedFromShowColosseum = false;
    
    void onCampaignChestShopClick();
    void onResetCampaignChestClick();
    void onForceOpenCampaignChestClick();
    void onOpenCampaignChestClick(Ref* ref);
    void onOpenCampaignChest();
    void openCampaignChest();
    void showStageSelect(int chapter);
    void onStageClick(Ref* ref);
    
    void showStageReady(int stage);
    void goToStage(Ref* ref);
    float defaultScale = 1.45f;

    PPLabel* addLabelToButton(Button* btn, std::string text, int fontSize = 60, Color3B color = Color3B(78, 78, 78), bool isBordered = false);
    void addLabelToButtonFromHud(std::string text, Button* btn, bool bordered, Color3B color, bool isSystemLabel);
    PPLabel* replaceTextToPPLabel(Text* lblText);
    void onEffectClick();
    void onMusicClick();
    Node* indicator = nullptr;
    void onClickFull();
    void buyFullPackage();
    Node* fullPackagePopup = nullptr;
    
    void titleUpdate(float dt);
    void showPostPopup();
    void onRewardNoticeClick();
    void onRewardNoticeOkClick();
    void onPostItemGemClick(Ref* ref);
    void onPostItemGemOkClick();
    void onPostItemUnitClick(Ref* ref);
    void onPostItemUnitOkClick();
    std::string selectedPostItem = "";
    void closeGemRewardPopup();
    Button* showGemRewardPopup(int gemCount, const char* msg);
    Button* showUnitRewardPopup(std::string units, const char* msg);
    void gemMoveToTopBarDone(Ref* ref);
    void goldMoveToTopBarDone(Ref* ref);
    void treeMoveToTopBarDone(Ref* ref);
    int gemRewardCount = 0;
    int goldRewardCount = 0;
    int treeRewardCount = 0;
    int stageRequested = -1;
    
    void doLabelFadeInLater(PPLabel* lbl, float delay, float fadeInDur);
    
    EventListenerKeyboard* listener;
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    void onExitClick();
    
    void showColosseum();
    void updateColosseumPopup(float dt);
    bool isColosseumRankInfoReceived = false;
    std::vector<std::string> colosseumRankInfoName;
    std::vector<int> colosseumRankInfoResult;
    std::vector<int> colosseumRankInfoTime;
    std::vector<int> colosseumRankInfoKill;
    std::vector<int> colosseumRankInfoWeapon;
    std::string GetCurrentLeagueText();
    std::string GetLeagueText(int league);
    void onPlayColosseum(Ref* ref);
    bool isWaitingForRankInfo = false;
    bool isRankInfoReceviedSuccessfully = false;
    void onGetTimeFailed();
    bool isTimeGetFailed = false;
    bool isGetRankFailed = false;
    bool isLeagueNotFound = false;
    std::string strMessageBox = "";
    void onEventClick();
    void onChatClick();
    
    void showEventPopup();
    
    void showTicketOffer();
    void onTicketOfferButtonClick(Ref* ref);
    bool isColosseumInfoRefreshRequired = false;
    float colosseumUITimer = 0;
    
    void showLeagueResult();
    void onCloseColosseumResult();
    
    void rollOpenScroll(ImageView* imgBack);
    void showGlowBack(Node* node);
    std::string getRandomWord();
    std::string getRandomName();
    std::string getRandomID();
    std::string getPlayerName();
    std::string getPlayerID();
    
    bool isUserInfoCreatedOrNotKnow = true;
    bool isWaitingForUserInfoCreated = false;
    bool isUserInfoReceived = false;
    bool isWaitingForUserInfo = false;
    void onMessageBoxReceived();
    void onUserInfoFailed();
    void onUserInfoNotFound();
    void onCreateUserFailed();
    void onCreateUserCreated();
    
    bool isTitleEnd = false;
    Node* titleLayer = nullptr;
    
    void closeColosseumPopup();
    void clearAssets();
    
    void onVideoForTicketClick();
    void onVideoDone();
    bool isVideoDone = false;
    bool isFreeAppResultArrived = false;
    bool isFreeApp = false;
    void showFreeAppAlert();
    void showExitPopup();
    bool showInterstitialRequested = false;
    bool isHeroInfoRequested = false;
    bool isHeroInfoArrived = false;
    bool isAllUserDataRequested = false;
    bool isAllUserDataArrived = false;
    void getHeroInfoFromServer();
    void onBattleClick(Ref* ref);
    int nameHandleState = 0;
    int renameHandleState = 0;
    bool isNameRequested = false;
    int networkStateGetData = 0;
    std::string nameToRegister;
    
    bool isRestartingReserved = false;
    void restartTheGame();
    bool isSendingPassword = false;
    Label* lblID;
    Label* lblName;
    Label* lblLoading;
    
    void setTitle(Node* titleBack, std::string menuKey);
    void showRegisterName();
    void onOkNameClick(Ref* ref);
    void onRandomNameClick(Ref* ref);
    void onSignInWithAppleID(Ref* ref);
    void requestRegisterName(std::string strName);
    void showUserSelect();
    void onNewUserSelect(Ref* ref);
    void onOldUserSelect(Ref* ref);
    void showInputUserIDAndName();
    void onCheckExistingUser(Ref* ref);
    int selectedChapter = -1;
    int colosseumTotal=1;
    int colosseumMe=-1;
    std::string strRankInfo;
    std::string arenaRecord = "";
    bool isArenaRecordRecieved = false;
    void onSettingClick();
    void onMusicInSettingClick();
    void onSoundClick();
    void onRestoreClick();
    void updateSettingPopup();
    void onNetworkResetClick();
    void onNetworkResetConfirmClick();
    bool isRestoreRequested = false;
    void onPurchaseChapterClick(Ref* ref);
    
    void onCommunityClick();
    void onFacebookClick();
    void showServerSelect(bool isNewUser);
    void onServerSelected(Ref* ref);
    void onMoveServerClick();
    void checkPlayerIDExist();
    bool isUserIDExistCheckRequested = false;
    bool isUserIDExistCheckArrived = false;
    void settingLog(std::string str);
    void onLanguageClick();
    void onLanguageOkClick();
    void onLanguageButtonClick(Ref* ref);
    
    void onDifficultyClick();
    void onEnterBackground();
    int iv = -1;
    int av = -1;
    int rewardIndex = -1;
    bool isGameInfoRecieved = false;
    const char* rewardInfo = "";
    bool isRewardInfoReceived = false;
//    bool isHardMode = false;
    int difficultyMode = DIFFICULTY_MODE_NORMAL;
    void checkMonthlyEventAttend();
    bool isMonthlyEventAttendChecked = false;
    bool firstIndicatorDisposed = false;
    void onPrivacyPolicySmartPassClick();
    void showSmartPassError(std::string msg);
    void onStartPassErrorOk();
    void clearCacheForSmartPass();
    void onUpdateClick();
    void onRenameClick();
    void onGoogleClick();
    bool isRename = false;
    
    void onYoutubeChannelClick();
    void onYoutubeClick(Ref* ref);
    void onYoutubeConfirm(Ref* ref);
    void showDiscordPopup();
    void onDiscordInvitationClick();
    void onDiscordOpenClick();
    
    void onCollectionClick();
    void goToMultiplayStage(int stageNumber);
    std::string crossSpriteName;
    void onCrossClick();
    void goToCross();
    void onMapClick();
    void onUploadedClick();
    
    void onServerChecked();
};
#endif /* Title_hpp */
