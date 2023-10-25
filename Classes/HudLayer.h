//
//  HudLayer.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 29..
//
//

#ifndef AlmostLegendary_HudLayer_h
#define AlmostLegendary_HudLayer_h
#include "PPLabel.h"
#include "SneakyJoystickSkinnedBase.h"
#include "SneakyJoystick.h"
#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UILoadingBar.h"
#include "cocos2d.h"
#include "LegendDaryButton.h"
#include "Movable.h"
#include "base/CCEventListenerController.h"
#include "base/CCEventController.h"
#include "base/CCController.h"
#include "ui/UIHelper.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/UITextField.h"
#include "ui/UITextBMFont.h"
#include "ui/UIImageView.h"
#include "ui/UILoadingBar.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "AwesomeDialogBox.h"
#include "Inventory.h"
#define INVENTORY_MAX_COUNT_FOR_SLOT 50
#define TALK_STATE_ASKING 0
#define TALK_STATE_QUESTIONING 1
#define TALK_STATE_CHOOSED 2

#define ACTION_TYPE_EQUIP 0
#define ACTION_TYPE_THROW_AWAY 1
#define ACTION_TYPE_CANCEL 2
#define ACTION_TYPE_USE 3
#define ACTION_TYPE_SELL 4
#define ACTION_TYPE_EXCHANGE 5

#define DIALOG_TYPE_AB_RESET 0
#define DIALOG_TYPE_ABC 1

#define BTN_TYPE_NONE -1
#define BTN_TYPE_MOVE 0
#define BTN_TYPE_STOP 1
#define BTN_TYPE_ATTACK 2
#define BTN_TYPE_GATHER 3
#define BTN_TYPE_BUILD 4
#define BTN_TYPE_BUILD_BETTER 5
#define BTN_TYPE_WORKER 6
#define BTN_TYPE_SWORDMAN 7
#define BTN_TYPE_ARCHER 8
#define BTN_TYPE_HELICOPTER 9
#define BTN_TYPE_CATAPULT 10
#define BTN_TYPE_CASTLE 11
#define BTN_TYPE_FARM 12
#define BTN_TYPE_BARRACKS 13
#define BTN_TYPE_LUMBER 14
#define BTN_TYPE_WATCHER_TOWER 15
#define BTN_TYPE_FACTORY 16
#define BTN_TYPE_AIRPORT 17
#define BTN_TYPE_GOBLIN_WORKER 18
#define BTN_TYPE_ORC_AXE 19
#define BTN_TYPE_0RC_SPEAR 20
#define BTN_TYPE_GOBLIN 21
#define BTN_TYPE_GOBLIN_BOMB 22
#define BTN_TYPE_TROLL 23
#define BTN_TYPE_MAGE 24
#define BTN_TYPE_HQ 25
#define BTN_TYPE_BUNKER 26
#define BTN_TYPE_ORC_BARRACKS 27
#define BTN_TYPE_TROLL_HOUSE 28
#define BTN_TYPE_TEMPLE 29
#define BTN_TYPE_BARBECUE 30
#define BTN_TYPE_CANCEL 31
#define SELECT_DIALOG_GO_TO_NPC 0

using namespace cocos2d;

using namespace cocos2d::ui;
using namespace cocostudio;
class HudLayer : public Layer
{
private:
    
    int weaponIndex;
    bool toChar;
    //Constructor
    Vector<LegendDaryButton*> buttonArray;
    Vector<Sprite*> lifeArray;
//    Sprite* coin;
//    Label* lblStage;
    int starCount;
    Label* talkLabel;
    Layer* talkLayer;
    MenuItemImage* pauseItem;
    MenuItemImage* recordItem;
    MenuItemImage* bombItem;
    MenuItemImage* potionItem;
    void closeWaitMessageBox(float dt);
    void closeWaitMessageBoxAndShowSuccess(float dt);
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
public:
    Label* lblCommand;
    Inventory* inventory;
    cocos2d::Vec2 btnJumpPos;
    cocos2d::Vec2 btnReloadPos;
    cocos2d::Vec2 btnWeaponPos;
    
    Vector<Button*> menus;
    DrawNode* draw;
    AwesomeDialogBox* dialogBox = nullptr;
    void onDialogDone();
    void showNextTalk();
    void showDialog(std::string str);
    EventListenerKeyboard* listener;
//    EventListenerTouchOneByOne* touchListener;
    EventListenerTouchAllAtOnce* touchListener;
    void tryResultOK();
    Sprite* bossEnergyBackground;
    Label* bossName;
    DrawNode* bossEnergyBar;
    DrawNode* bossEnergyBarBack;
    int bossEnergyBarWidth;
    bool isRecording;
    void addEnergyEffect();
    void addEnergyDone(float dt);

    TextBMFont* lblKeyCollected;
//    Label* coinLabel;
    cocos2d::Size size;
    Layout* pauseLayer;
    Layout* reviveLayer;
    Layout* gameOverLayer;
    Layout* gameClearLayer;
    void showStageTitle();
    void stageTitleLineUpdate(float dt);
    DrawNode* dnTitleLine;
    float lineLength;
    float currentLineLength;
    void whiteGunFired();
    Sprite* sptWhiteGun;
    
    void tryGameContinue(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void toMain(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void toStageSelect(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void nextStage(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void retryStage(Ref* pSender, ui::Widget::TouchEventType eEventType);
    void onReplayVideo(Ref* pSender, ui::Widget::TouchEventType eEventType);
    
    void buyGemClosed(Ref* obj);
    void reviveLater(float dt);
    void reviveFromVideo();
    virtual bool init();
    CREATE_FUNC(HudLayer);
    void showTalk(const char* talk);
    Text* lblVs = nullptr;

    void hideTalk();
    Layer* bombLayer;
    void useBomb();
    void closeWaitMessageBoxLater();
    
    void closeWaitMessageBoxLaterAndShowSucess();
        
    void suggestBombShop();
    Layer* potionLayer;
    void suggestPotionShop();
    void buyPotion();
    void buyGems();
    void notEnoughGemClosed(Ref* obj);
    void resumeGame(Ref* pSender, ui::Widget::TouchEventType eEventType);
//    void hideItems();
//    Sprite* blackScreen;
    
    void closePopupLayer();
    void updateCoin(int howMuch);
    void enableJoystick(bool enable);
    void initializeSreen(int lifeCount);
    void reduceLife();
    void setWeapon(int weapon);
    Vector<Sprite*> bulletArray;
    void setTotalBulletCount(int count, int maxCount);
    void reduceBulletCount();
    void setBulletCount(int count, int maxCount);
    void keyCollected(int count);
    void starCollectedFancy(int count, cocos2d::Vec2 collectedPos);
    void starCollected(int count);
    void starChange(float dt);
    void showPauseLayer();
    void showShopLayer(std::string shopName);
    void selectShopItem(Ref* ref);
    Node* selectedShopItem = nullptr;
    void showBuyLayer(Button* btn, std::string itemName);
    Node* buyLayer = nullptr;
    void onBuyItemClick();
    Sprite* changeCoinToAnimation(Node* imgCoin);
    Sprite* changeGemToAnimation(Node* imgCoin);
    Node* shopLayer = nullptr;
    void showCoinShopLayer();
    Text* lblCoinCount;
    Text* lblEnemyCount;
    Text* lblExpCount;
    LoadingBar* pbExp;
    ImageView* sptMaxTalkBalloon;
    Text* lblPerfect;
    Text* lblNext;
    ImageView* sptBackLight0;
    ImageView* sptBackLight1;
    ImageView* sptBackLight2;
    Button* btnNext;
    Button* btnRetry;
    Button* btnHome;
    int increasingCoinCount;
    float increasingEnemyCount;
    int increasingExpCount;
    int increasingCounter;
    int increasingCounterMax;
    float countingInterval;
    int expCount;
    int maxExpCount;
    EventListenerController* _listener;
    void registerControllerListener();
    //Controller is the obejects of the Controller，keyCode means the keycode of the controller you click down
    void onKeyDown(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    void onKeyUp(cocos2d::Controller *controller, int keyCode, cocos2d::Event *event);
    
    //The axis includes X-axis and Y-axis and its range is from -1 to 1. X-axis is start from left to right and Y-axis is bottom to top.
    void onAxisEvent(cocos2d::Controller* controller, int keyCode, cocos2d::Event* event);
    void onConnectController(Controller* controller, Event* event);
    void onDisconnectedController(Controller* controller, Event* event_);
                                            
    void coinLabelScheduler(float dt);
    void enemyLabelScheduler(float dt);
    void expLabelScheduler(float dt);
    void playCoinSoundLater();
    void removeUsedAssets();
    void onFreeCoin(Ref* pSender, ui::Widget::TouchEventType eEventType);
    Button* btnFreeCoin;
    void videoDone();
    void videoFailed();
    void closeVideoDone(float dt);
    void closeVideoFailed(float dt);
    bool videoContinued = false;
    int reviveCount = 0;
    Layout* messageBox;
    void setGameSpeed(float speed);
    
    
    int coinCount;
    int enemyKilledCount;
    void setExp(int level, int exp, int expMax);
    void setEnergy(int hp, int max);
    int enemyMaxCount;
    int getHPFromItem(std::string itemName);
    int getAPFromItem(std::string itemName);
    int getDPFromItem(std::string itemName);
    int getGPFromItem(std::string itemName);
    int getSPFromItem(std::string itemName);
    int selectedHeroSlot = 0;
    ui::Layout* rankingLayer;
    Label* changeTextWithLabel(Text* txt, float width = 0);
    Sprite* addSpriteToButton(std::string imgFileName, Button* btn, Button::TextureResType type);
    Sprite* addSpriteToButton(std::string itemName, Button* btn);
    void addLabelToButton(std::string text, Button* btn, bool bordered, Color3B color, bool isSystemLabel = true);
    Sprite* getIconSprite(std::string itemName);
    Label* lblTyping;
    std::string originalString="";
    void showTyping();
    Node* hudLayer;
    
    
    void typingCursorBlinking(float dt);
    void typing(std::string str);
    bool isInScene=false;
    void showInfo(std::string info);
    void hideInfo();
    Label* lblInfo;
    void spriteMoveDone(Node* sprite);
    void coinWaitDone(Node* sprite);
    void displayBossEnergy(const char* name);
    void setBossEnergy(int percent);
    void setButtonSize(float siz);
    void enableButtons(bool enb);
    void showAchievementComplete(const char* text);
    void achievementEffect(Node* node);
    void nodeMoveDone(Ref* obj);
    void addHeartAnimated(cocos2d::Vec2 pos);
    void addListener();
    void onStartClick();
    void onSelectClick();
    void removeListener();
    void addHeartDone(Ref* obj);
    void playBackgroundMusicLater(float dt);
    int whereToGo;
    void closeSchedule(float dt);
    void showDialog(const char* message, const char* btn1, const char* btn2);
    void messageBoxClosed(Node* sender);
    void recommandCoinShopClosed(Node* sender);
    Vector<Node*> popupArray;
    
    void ropeAround(Node* node);
    Node* sptCursor = nullptr;

    bool setAsPopup(Node* node);
    Node* getPopup();
    void closePopup();
    bool isPopupAvailable();
    std::string selectedMenuName = "-1";
    Node* rightBottomPanel;
    Node* rightBottomPanelForCampaign;
    Node* bottomUnitBar;
    
//    virtual void keyBackClicked();
//    virtual void draw();
    virtual void keyDown(int key);
    virtual void keyUp(int key);

    Label* lblTouch;
    void toggleAuto(bool showMsg = true);
    void onEndCredit();
    void hideBtns();
    void showBtns();
    
    void showEvent(int index, bool isQuest);
    void onEvent(float dt);
    void showSingleNPCMessage(std::string msg);
    void onDisposableMessageEvent(float dt);
    
    
    
    Sprite* blackBottom;
    Sprite* blackTop;
    void showBlackTopAndBottom();
    void hideBlackTopAndBottom();
    Sprite* sptTalkBox;
    void updateTalkBoxRope(float dt);
    void onTalkBoxResizeDone();
    int eventIndex=-1;
    bool isReadyToShowNextTalk = false;
    
    int talkState = TALK_STATE_ASKING;
    int answer = -1;
    void onLeftPress();
    void onRightPress();
    void onUpPress();
    void onDownPress();
    void onLeftRelease();
    void onRightRelease();
    void onUpRelease();
    void onDownRelease();
    void showOptions(std::string option0, std::string option1);
    void readyToShowNextTalk();
    void endEvent();

    
    void showEnding(int index);
    void updateEnding(float dt);
    int endingIndex = 0;
    int endingTalkIndex = 0;
    int lastEndingTalkY = 0;
    bool inputShouldWait = false;
    Vector<Sprite*> endingTalkArray;
    bool videoDoneRequested = false;
    Node* inventoryLayer = nullptr;
    Node* mapLayer = nullptr;
    void showMap();
    void initShortcuts(Vec2 startPos);
    void onShortcutClick(Ref* ref);
    void onShortcutDeleteClick(Ref* ref);
    int selectedShortcut = -1;
    int shortcutCount = 5;
    std::string shortcutList[5];
    
    Node* selectedInventorySlot = nullptr;
    Node* abcLayer = nullptr;
    Node* selectedAbcOption = nullptr;
    PPLabel* showInstanceMessage(std::string msg);
    void showInventory();
    void updateInventory();
    void showDisassembleButton();
    void hideDisassembleButton();
    void showDisassembleConfirmButtons();
    void hideDisassembleConfirmButtons();
    void onDisassembleClick();
    void onDisassembleConfirmClick();
    void onDisassembleCancelClick();
    void selectInventorySlot(Ref* ref);
    int getItemTypeInSlot(int slot);
    
    
    bool isCursorReadyToMove = true;
    void selectABCOption(Node* node);
    void closeABCLayerLater(float dt);
    void equipItem(int slot);
    
    void showCancelBuildingButton();
    void onCancelBuildingClick();
    Node* selectedMenu = nullptr;
    void selectMenu(Node* selectedItem);
    Node* menuLayer = nullptr;
    Sprite* addBToCloseToLayer(Node* layer);
    
    
    Node* selectDialog = nullptr;
    int currentSelectDialog = -1;
    void ddiyong(Node* node);
    void selectSelectDialog(float dt);
    Node* goToNPCLayer = nullptr;
    void showGoToNPC();
    void showGoToNPCProgress(float dt);
    void showIntro();
    float loadingTime = -2;
    void onLoading(float dt);
    void showAlchyShop();
    bool isReadyCombineInAlchyShop = false;
    int alchyNextWeaponLevel = -1;
    std::string alchyIngredientWeaponName;
    std::string alchyIngredientNextWeaponName;
    int getAlchyWeaponCombineMinutes(int weaponIndex);
    Node* alchyShopLayer = nullptr;
    void updateAlchyShop();
    
    void onAcceptCombineInAlchyShop();
    void showSAMShop();
    void selectSamShopItem(Ref* ref);
    void showPeterInsuranceShop();
    void onChoosedInsurancePlan(Ref* ref);
    void selectPeterInsurancePlan(int index);
    Node* peterShopLayer;
    Node* selectedPeterItem = nullptr;
    Node* samShopLayer = nullptr;
    Node* selectedSamShopItem = nullptr;
    void updateSamShop();
    std::string samShopIngredientName;
    std::string samShopResultStoneName;
    int samShopStoneLevel;
    int samShopExchangeRate = 7;
    int getInventoryTypeForItemType(int itemType);
    
    void showDoctorShop();
    Node* doctorShopLayer = nullptr;
    Node* selectedDoctorShopItem = nullptr;
    void updateDoctorShop();
    void evolving(float dt);
    int evolvingPercent = 0;
    void selectDoctorShopItem(Ref* ref);
    void onResetClick();
    void processReset();
    void showCredit();
    int getGemForMinute(int min);
    void onConfirmInsurancePlan(Ref* ref);
    
    void showPVP();
    void oneSecUpdate(float dt);
    void showPotSoul(cocos2d::Vec2 pos);
    void onPotSoulMoveDone();
    
    bool isSceneChanging = false;
    void showWinPopup(bool win);
    void onSkipClick();
    float resultTime = 0;
    Node* resultPopup = nullptr;
    int resultUsedGold = 0;
    int resultEarndGold = 0;
    int resultUsedLumber = 0;
    int resultEarndLumber = 0;
    int resultProduce = 0;
    int resultKill = 0;
    void updateResultPopup(float dt);
    Node* deadPopup = nullptr;
    float deadCounter = 5;
    
    void onReviveByGem();
    void onReviveByVideo();
    void onReviveByDead();
    
    PPLabel* lblGold;
    PPLabel* lblLumber;
    PPLabel* lblFood;
    
    void onCommandClick(Ref* ref);
    Label* getLabel(std::string txt, int fontSize);
    void setMenu(int index, int btnType);
    
    PPLabel* lblTimer;
    void onOkFromWinPopup(Ref* ref);
    void arrangeMenu(cocos2d::Vec2 pos);
    
    Node* priceInfo = nullptr;
    void showPriceInfo(std::string msg, int gold, int lumber, int food);
    void hidePriceInfo();
    
    bool tutorialEnded = false;
    Node* tutorialNode = nullptr;
    Node* tutorialHowTo = nullptr;
    void updateTutorialHowTo(float dt);
    int tutorialHowToIndex = 0;
    void onTutorialBoxClick();
    Sprite* tutorialHand = nullptr;
    int tutorialIndex=0;
    void talkBoxUpdate(float dt);
    int talkIndex = 0;
    std::string talkText;
    
    void onMenuClick();
    void onResumeClick();
    void onExitClick(Ref* ref);
    void onOptionClick();
    void onSaveClick();
    void onSaveSlotClick(Ref* ref);
    int selectedSaveSlot = 0;
    void onOkFromSaveOverwrite();
    void saveCurrentStageData(int slot);
    void onLoadClick();
    void onLoadSlotClick(Ref* ref);
    void onOkFromLoadData();
    void goToLoadedStage();
    void onEffectClick();
    void onMusicClick();
    void onSelectAllForces();
    void onSelectForcesInScreen();
    void onShowMoreMenuClick(Ref* ref);
    void on2xFastClick();
    void on3xFastClick();
    void onReivewPopupButtonClick(Ref* ref);
    void showSupportOffer();
    void onSupportSelected(Ref* ref);
    void onSupportSelectedByTicket(Ref* ref);
    void showIndicator();
    void hideIndicator();
    void showPremiumRetry();
    void onBuyPremiumRetryClick();
    
    void update(float dt);
//    int iapFlag = 0;
    bool isVideoDone = false;
    
    void onDragSelectClick(Ref* ref);
    Node* imgDragSelected;
    bool isRaid = false;
    std::vector<UnitInfo*> deckUnitList;
    std::vector<UnitInfo*> heroDeckUnitList;
    void onUnitInBottomDeckClick(Ref* ref);
    bool isRaidStarted = false;
    void showStarForRaid(int starIndex);
    void showRaidResult(bool saveRecord);
    void showRaidResultWithSaveRecord();
    void goToTitleScene();
    void goToBattleScene();
    void findMatchForRaid();
    void setRaid();
    void onNextMatchClick(Ref* ref);
    void onSurrenderClick(Ref* ref);
    
    void setPvpUI(int playerHPMax, int enemyHPMax);
    void updatePvpUI(int playerHP, int enemyHP);
    ImageView* imgPvpPlayerEnergy;
    ImageView* imgPvpEnemyEnergy;
    Text* lblPvpCount;
    float pvpTimeLeftToStart = 4;
    float playerHpMax = 0;
    float enemyHpMax = 0;
    void onPvpResultOk(Ref* ref);
    bool isWon = false;
    void onOpenCampaignChestClick(Ref* ref);
    void onOkCampaignChestClick();
    void onOpenCampaignChest();
    
    void onOkFromRaidWinPopup(Ref* ref);
    bool hideIndicatorRequested = false;
    int offsetRight;
    
    void hackerReportAlert(float dt);
    void onLikeClick();
    void onDislikeClick();
};
#endif
