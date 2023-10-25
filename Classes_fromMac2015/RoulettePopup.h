//
//  RoulettePopup.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 03/12/2019.
//

#ifndef RoulettePopup_hpp
#define RoulettePopup_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "ui/UIImageView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "TopBar.h"
#include "PageBase.h"
#define RESOURCE_TYPE_GOLD 0
#define RESOURCE_TYPE_LUMBER 1
#define RESOURCE_TYPE_GEM 2
using namespace cocos2d;
using namespace cocos2d::ui;
class RoulettePopup : public PageBase
{
private:
    cocos2d::Size size;
public:
    int month = 0;
    int day = 0;
    virtual bool init();
    CREATE_FUNC(RoulettePopup);
    Node* layer;
    void update(float dt);
    void onRollClick(Ref* ref);
    void onVideoClick(Ref* ref);
    void rollTheRoulette();
    ImageView* imgRoulette = nullptr;
    void showReward();
    
    void onTestLeft(Ref* sender, Widget::TouchEventType type);
    void onTestRight(Ref* sender, Widget::TouchEventType type);
    
    bool isTestLeftPressed = false;
    bool isTestRightPressed = false;

    Button* showResourceRewardPopup(int resourceType, int resourceCount, const char* msg);
    void closeResourceRewardPopup();
    
    std::string strUnitReward;
    Button* showUnitRewardPopup(std::string units, const char* msg);
    void onRewardUnitOkClick();
};


#endif /* RoulettePopup_hpp */
