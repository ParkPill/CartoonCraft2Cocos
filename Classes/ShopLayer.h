//
//  ShopLayer.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#ifndef ShopLayer_hpp
#define ShopLayer_hpp

//
//  Title.hpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 13/03/2018.
//

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "PageBase.h"
#define DEFAULT_LEAGUE_INDEX 3
using namespace cocos2d;
using namespace cocos2d::ui;
class ShopLayer : public PageBase
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(ShopLayer);
    Node* ndShopLayer = nullptr;
    
    void update(float dt);
    
    void onGoldClickFromTopBar();
    void onTreeClickFromTopBar();
    void onGemClickFromTopBar();
    void onShopTabClick(Ref* ref);
    
    void onBuyStartKeys();
    void onBuyMartialArtPackage();
    void onBuyClassicWarPackage();
    void onBuyCrewManagerPackage();
    void onBuyMasterCrewPackage();
    void onBuyEventHeroPackage();
    void onBuyMidMonthEventHeroSet();
    void onBuyTree(Ref* ref);
    void onBuyGold(Ref* ref);
    void onBuyGem(Ref* ref);
    int getEventHeroBoughtCount();
    int getEventHeroSetBoughtCount();
    int getMineGoldPerHour(int level);
    int currentTab = 0;
    void hideTab(int tab);
    bool heroesAreChanged = false;
    void closeShop();
    int midMonthHeroType = 0;
};

#endif /* ShopLayer_hpp */
