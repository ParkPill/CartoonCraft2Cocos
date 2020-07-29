//
//  MonthlyEventPopup.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 25/10/2019.
//

#ifndef MonthlyEventPopup_hpp
#define MonthlyEventPopup_hpp


#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "TopBar.h"
#include "PageBase.h"
#define DEFAULT_LEAGUE_INDEX 3
using namespace cocos2d;
using namespace cocos2d::ui;
class MonthlyEventPopup : public PageBase
{
private:
    cocos2d::Size size;
public:
    
    virtual bool init();
    CREATE_FUNC(MonthlyEventPopup);
    
    void update(float dt);
    Node* layer;
    int month = 0;
    int day = 0;
    void addMissions();
    
    void onReceiveHeroClick(Ref* ref);
    void onReceiveRewardForItem(Ref* ref);
    void onTestClick(Ref* ref);
    bool isAllMissionClearAndReceived();
    
    void updateLayer();
};

#endif /* MonthlyEventPopup_hpp */
