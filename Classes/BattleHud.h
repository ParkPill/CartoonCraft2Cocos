//
//  BattleHud.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 22/03/2019.
//

#ifndef BattleHud_h
#define BattleHud_h

#include <stdio.h>
#include "PPLabel.h"
#include "Movable.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "ui/UIScrollView.h"
#include "ui/UILoadingBar.h"
#include "PageBase.h"

#define JOB_NONE 0
#define JOB_BUILDING 1
#define JOB_MOVE_BUILDING_TEMPATE 2
#define UNIT_TAG_UNIT 0
#define UNIT_TAG_BUILDING 1
#define UNIT_TAG_TREE 2
#define BUILDING_CASTLE -1
#define BUILDING_MINE 0
#define BUILDING_TREE 1
#define BUILDING_FARM 2
#define BUILDING_BARRACKS 3
#define BUILDING_WATCHER_TOWER 4
#define BUILDING_LUMBURMILL 5
#define BUILDING_FACTORY 6
#define BUILDING_AIRPORT 7
#define BUILDING_ORC_HQ 8
#define BUILDING_ORC_BARRACKS 9
#define BUILDING_ORC_BUNKER 10
#define BUILDING_ORC_TROLL_HOUSE 11
#define BUILDING_TOTAL 6

#define BELONG_TO_DECK 0
#define BELONG_TO_INVENTORY 1
#define BELONG_TO_HIRE 2

#define DATA_TYPE_GEM 0
#define DATA_TYPE_GOLD 1
#define DATA_TYPE_TREE 2
#define DATA_TYPE_BUILDING 3
#define DATA_TYPE_INVENTORY 4
#define DATA_TYPE_DECK 5
#define DATA_TYPE_IAP 6
#define DATA_TYPE_SEARCH_STATE 7
#define DATA_TYPE_LAST_LAUNCH_DATE 8
#define DATA_TYPE_SEARCH_ITEMS 9
#define DATA_TYPE_HERO_INVENTORY 10
#define DATA_TYPE_HERO_DECK 11
#define DATA_TYPE_KEYS 12
#define DATA_TYPE_KEY_GET_STATE 13
#define DATA_TYPE_STAGE_CLEAR 14
#define DATA_TYPE_STAGE_HARD_CLEAR 15
using namespace cocos2d;
using namespace cocos2d::ui;

class BattleHud : public PageBase
{
private:
    cocos2d::Size size;
public:
//    Vector<Node*> popupArray;
    virtual bool init();
    CREATE_FUNC(BattleHud);
    Node* hudLayer;
//    void setAsPopup(Node* node);
    void closePopup();
    void onBattleClick();
    void onLocalBattleClick(Ref* ref);
    void onNetworkBattleClick(Ref* ref);
    void onCompleteRestingClick(Ref* ref);
    
    void onShopClick();
    void onBuyWorkersClick(Ref* ref);
    
//    void onBuyBuilding(Ref* ref);
//    void onResourceInShopClick();
//    void onBuyResource(Ref* ref);
//    void onGemInShopClick();
//    void onBuyGem(Ref* ref);
//    void onBuyTree(Ref* ref);
//    void onBuyGold(Ref* ref);
    Node* onBackClick();
    void onMissionClick();
    void onDailyMissionClick(Ref* ref);
    void onDailyRewardClick();
    void onDailyRewardReceiveClick(Ref* ref);
    void onCollectGoldClick(int amount);
    void onPopulationClick();
    void onUpgradeClick();
    void onUpgradeConfirmClick();
    void onInfoClick();
    void onUnitSelected(Movable* unit);
    void onCancelClick();
    void onOkClick();
    void onCompleteClick();
    void onTrainClick();
    void onRefreshHireClick();
    void refreshSearchState();
    int getFoodMax();
    int getFoodInUse();
    void onTrainUnitClick(int unitIndex);
    void onCancelTrainUnitClick(Ref* ref);
    void onExitGame();
    void onEnterGame();
    void handleReaccess();
    void updateTrainPopup();
    void updateUI(float dt);
    PPLabel* replaceTextToPPLabel(Text* lblText);
    float currentJobRemainTime = 0;
    void onSpeedUpClick();
    Text* lblCurrentRemainTime = nullptr;
    LoadingBar* pbCurrentRemainTime = nullptr;
    Node* buildingTemplate = nullptr;
    void updateStatus(Node* layer);
//    void onTabClick(Ref* ref);
    int getCurrentPopulation();
    int getPopulationMax();
    std::string getBuildingName(int index);
    std::string getBuildingSpriteName(int buildingIndex);
    int getCurrentBuildingCount(int index);
    int getMaxBuildingCount(int index);
    
    Button* btnComplete;
    Movable* selectedUnit = nullptr;
    PPLabel* showInstanceMessage(std::string msg);
    
    int getBuildingPriceGold(int buildingIndex);
    int getBuildingPriceTree(int buildingIndex);
    int getUnitUpgradePriceGold(int index, int level);
    int getUnitUpgradePriceTree(int index, int level);
    int getFoodProvide(int unitType);
    int getMaxGold(int level);
    int getMaxGold();
    int getMaxTree();
    int getMaxTree(int level);
    int getMineGoldPerHour(int level);
    int getMineGoldStorage(int level);
    int getLumbermillTreePerHour(int level);
    int getLumbermillTreeStorage(int level);
    void onMoreGemInLobbyClick();
    void onMoreGoldClick();
    void onMoreTreeClick();
    void onWorkerClick();
    int workerCount = 1;
    int availableWorkerCount = 1;
    void updateAvailableWorkerCount();
    bool isWorkerAvailable();
    void showCancelButton();
    void hideCancelButton();
    int currentJob = JOB_NONE;
    int currentJobDetailIndex = -1;
    void update(float dt);
    bool isAnybodyMoving = false;
    int timePassedFromLastAccess = 0;
    int getUnitCreatingTime(int unit, int level = 0);
//    void saveBuildings();
//    void saveInventoryAndDeck(bool includeIap = false);
    void updateInventorySaveData();
    void updateHeroDeckSaveData();
    void updateDeckSaveData();
    void updateBuildingsSaveData();
    void saveUserData(std::vector<int>& datas);
    bool isBuilding(int unit);
    int getBuildingIndex(int unit);
    int getUnitIndex(int buildingUnidex);
    void loadData();
//    Vector<Movable*> buildingsCreating;
    Vector<Movable*> buildings;
//    std::vector<std::string> unitsInventory; // x -1 to save
    Vector<Movable*> unitsDeck;
    Vector<Movable*> unitsHeroDeck;
    std::vector<UnitInfo*> unitInfoListHeroDeck;
    std::vector<UnitInfo*> unitInfoListDeck;
    std::vector<UnitInfo*> unitInfoListInventory;
    std::vector<UnitInfo*> unitInfoListHeroInventory;
    UnitInfo* selectedUnitInfo;
    void updateUnitInfoInList();
    void onConfirmFireClick();
    void onUnitInDeckClick(Ref* ref);
    void onUnitInInventoryClick(Ref* ref);
//    Vector<Text*> timeLeftLabels;
    Movable* getUnitFromData(std::string str);
    int getUnitUpgradeTime(int unit, int level);
    Movable* addBuildingCreating(int index, float timeLeft);
    Node* ndTopBar;
    Text* lblGemInHud;
    int lastGemUpdated;
    Text* lblGoldInHud;
    int lastGoldUpdated;
    Text* lblTreeInHud;
    int lastTreeUpdated;
    void showInfoPopup(bool isUpgrade);
    int networkStateGetData = 0;
    float oneSecChecker = 0;
    float oneSecCheckerForUI = 0;
    bool isLoadAllUserDataRequested = false;
    bool isTopRankRequested = false;
    bool isMyRankRequested = false;
    void onVisitTopRankerClick(Ref* ref);
    void addUpgradeEffect(Movable* node, bool isWorld);
    void updateMineName(Movable* mine);
    void setupBuilding(Movable* unit);
    void setupNonBuilding(Movable* unit);
    void onMineButtonClick(Ref* ref);
    void onTreeButtonClick(Ref* ref);
    void onCompleteUnitTrainClick(Ref* ref);
    bool reopenUnitTrainInfoRequested = false;
    void reopenUnitTrainInfo();
//    void showIndicator();
//    void hideIndicator();
    void updateSearchState();
    void onHireClick(Ref* ref);
    int hireIndex;
    void showUnitTrainInfo(int popupType, int unit, int level);
    void onUpgradeUnitClick();
    void onHireUnitClick(Ref* ref);
    void onMoveToDeckUnitClick();
    void onRemoveFromDeckUnitClick();
    void onFireUnitClick();
    void moveTopBarTo(Node* parent, bool moveToLeft = true);
    void onCutTreeClick();
    void addTreeToMap(int x, int y);
    cocos2d::Size mapSize;
    void getLastAccessTime();
    bool isLastAccessTimeRequested = false;
    void showNotEnoughWorkerUseGem();
    void onUseGemToFinishWorkerClick(Ref* ref);
    void showNotEnoughBarracksUseGem();
    void onUseGemToFinishBarracksClick(Ref* ref);
    
    void onDefenceRecordClick();
    void showDefenceRecord();
    void onRevengeClick(Ref* ref);
    
    std::vector<std::string> topRankIDs;
    std::vector<std::string> topRankNames;
    std::vector<int> topRankTrophys;
    int myRank=999;
    bool isVisitRequested = false;
    
    float lastTimeLeftToMidnight = 90000;
    time_t shieldEndTimeT = 0;
    bool addShieldRequested = false;
    void onBuyShieldWithGem(Ref* ref);
    void onPlayForShieldClick(Ref* ref);
    
    void onSpecialOfferClick();
    void onBuySpecialOffer(Ref* ref);
    
    bool isSpecialOfferCheckOff = false;
    void showSpecialOfferLastChance();
    void onYesToSeeTheSpecialOfferForTheLastTime();
    void onNoToSeeTheSpecialOfferForTheLastTime();
    bool isFromLastChance = false;
    int iapFlag = -1;
    void setLocalizedStringForChildLbl(Node* parent, std::string key);
    void addUnitToDeck(UnitInfo* info);
    std::string strStarterPrice0 = "BUY";
    std::string strStarterPrice1 = "BUY";
    std::string strStarterPrice2 = "BUY";
    
    EventListenerKeyboard* keyListener;
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    void removeUsedAssets();
    void onBanMessageBoxClosed();
    bool isLoadComplete = false;
    
    int tutorialIndex = 0;
    bool tutorialEnded = false;
    int talkIndex = 0;
    Node* tutorialNode;
    void talkBoxUpdate(float dt);
    std::string talkText;
    void onTutorialBoxClick();
    void showGemRewardMessageBox(std::string text, int gemCount, int gold = 0, int tree = 0);
    int gemRewardCount = 0;
    int goldRewardCount = 0;
    int treeRewardCount = 0;
    void onCloseGemRewardMessageBox();
    void gemMoveToTopBarDone(Ref* ref);
    void goldMoveToTopBarDone(Ref* ref);
    void treeMoveToTopBarDone(Ref* ref);
    int getTileIndexForPos(Vec2 pos);
    Vec2 getPositionForTileIndex(int index);
    int getBattleCooltime();
    Color3B getCardColorForLevel(int level);
    float svUpgardeX = 0;
    bool isTimeHackChecked = false;
    bool shouldSaveBuilding = false;
    bool shouldSaveDeckAndInventoryWhenExitTrainPopup = false;
    bool shouldSaveDeck = false;
    void onCloseTrainPopup();
    void checkUnsaved();
    
    void onVideoStoreClick();
    void onVideoTabClick(Ref* ref);
    void onVideoRandomClick();
    void onVideoUnitClick();
    void onVideoGemClick();
    void showTreeReward(int amount);
    void showGoldReward(int amount);
    int getRandomUnit();
    int getUnitIndexForVideoStore(int day);
    int videoStoreWaitForNextVideoTime = 300;
    int videoGachaWaitForNextVideoTime = 21600;
    int inventoryCountMax = 50;
    void onTrainBRClick();
    void onTrainTabClick(Ref* ref);
    void onTrainTab0Click();
    void onTrainGachaClick(Ref* ref);
    void onTrainGachaAgainClick(Ref* ref);
    void showItemGetAndAddToBag(std::vector<int> array);
    void onUpgradeClickInUpgradeTab();
    void onSlotClickInUpgradeTab(Ref* ref);
    void onUpgradeTabClick(Ref* ref);
    void updateUnitListForUpgrade();
    void onUnitClickInUpgradeTab(Ref* ref);
    void onCompleteUpgradeClick(Ref* ref);
    void onFinishUpgradeClick();
    void showMessageBox(std::string strKey);
    void onTrainGachaInfoClick();
    
    void showTutorialWithBigWorker(std::string text);
    void onHeroClick();
    void updateHero();
    Vec2 castlePos;
    void onInitComplete();
    void checkBattlePopupUI(time_t now);
    void checkSpecialOfferUI(time_t now);
    void checkVideoStoreUI(time_t now, bool isNewDay, int timeLeftToMidnight);
    void checkDailyMissionUI(bool isNewDay, int timeLeftToMidnight);
    void checkTrainLayerUI(time_t now, int timeLeftToMidnight);
    void checkDefenceRecordPopupUI(time_t now);
};
#endif /* BattleHud_h */
