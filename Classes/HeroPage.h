//
//  HeroPage.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 05/09/2019.
//

#ifndef HeroPage_hpp
#define HeroPage_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "PageBase.h"

#define CHEST_WOOD 0
#define CHEST_GOLD 1

using namespace cocos2d;
using namespace cocos2d::ui;
class HeroPage : public PageBase
{
private:
    cocos2d::Size size;
public:
    std::vector<UnitInfo*> unitInfoListHeroInventory;
    std::vector<UnitInfo*> unitInfoListHeroDeck;
    std::vector<UnitInfo*> unitInfoListHeroInventoryArrangedForFusion;
    std::vector<UnitInfo*> unitInfoListHeroSelectedForFusion;
    virtual bool init();
    CREATE_FUNC(HeroPage);
    
    void update(float dt);
    bool isTimeRequestedForGacha = false;
    int requestedChestType = -1;
    void requestTimeForGacha();
    void onTimeRecievedForGacha();
    bool isTimeReceivedForGacha = false;
    void handleTimeReceivedForGacha();
    int heroPageTutorialIndex = 0;
    Node* heroPageTutorialNode = nullptr;
    bool shouldSaveHeroDeckInfo = false;
    bool shouldSaveHeroInventoryInfo = false;
    
    void onLimitedPacakgeForHeroClick();
    void onNeverShowHeroAlertClick();
    void onHeroPvpClick();
    void showHeroPageTutorial(std::string text);
    void onHeroPageTutorialBoxClick();
    void addGreenArrowToButton(Node* btn);
    void onClosePickHero();
    void onHeroDrawRateInfoClick();
    void onMoveToDeckUnitClick();
    void onRemoveFromDeckUnitClick();
    void onFireHeroClick();
    void onFireOk();
    void onFireCancel();
    void onSortByRarity();
    void onSortBySlot();
    void onFusionClick();
    void onEquipedHeroClick(Ref* ref);
    void onFusionButtonClick(Ref* ref);
    void updateUnitListForFusion();
    void onSelectedHeroClickForFusion(Ref* ref);
    void onOkFusionResult();
    void onHeroInFusionClick(Ref* ref);
    void onHeroWoodChestGachaAgainClick();
    void onHeroGoldChestGachaAgainClick();
    Color3B getRankColor(int rank);
    void onHeroInInventoryClick(Ref* ref);
    void showHeroDetail(UnitInfo* info, bool showLightning = false);
    void onUpgradeUnitClick();
    UnitInfo* selectedUnitInfo;
    void updateHeroInventorySaveData();
    void updateHeroDeckSaveData();
    void onChestClick(Ref* ref);
    void pickHero(int minRank, int maxRank);
    //    void saveUserData(std::vector<int>& datas);
    
    void onHeroShopClick();
    //    void onHeroMoreGemClick();
    //    void onHeroMoreResourceClick();
    void onCloseHeroPage();
    
    void checkChangesForHeroes();
    void updateHeroes();
    void onPvpModeClick(Ref* ref);
    void showPvpInfo(int number);
    
    int pvpNetworkState = NETWORK_HANDLE_STATE_COMPLETE;
    std::vector<std::string> pvp6RankInfoName;
    std::vector<int> pvp6RankInfoTrohpy;
    std::vector<std::string> pvp6RankInfoData;
    int pvp6MyRank = -1;
    int pvp6TotalUserCount = -1;
    std::string pvp6Reward = "";
    
    std::vector<std::string> pvp12RankInfoName;
    std::vector<int> pvp12RankInfoTrohpy;
    std::vector<std::string> pvp12RankInfoData;
    int pvp12MyRank = -1;
    int pvp12TotalUserCount = -1;
    std::string pvp12Reward = "";
    void onPlayPvpClick(Ref* ref);
    void onRankerClick(Ref* ref);
    bool isPvpInfoRequested = false;
    int pvpNumber = 6;
    
    bool isPvpFindMatchRequested = false;
    void showPvpReward(std::string strReward);
    int getPickRate(int unit);
    std::vector<int> getPickPossibleHeroes();
    
    void onVideoForTicketClick(Ref* ref);
    void onPvpTicketVideoDone();
    bool isVideoDoneForPvpTicket = false;
    
    void showChestOpenWithGemAskPopup(int price);
    void onConfirmChestOpenWithGem();
    int chestOpenPrice = 0;
    void clearVectors();
};
#endif /* HeroPage_hpp */
