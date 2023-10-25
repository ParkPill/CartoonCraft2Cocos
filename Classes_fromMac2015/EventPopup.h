//
//  EventPopup.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 25/10/2019.
//

#ifndef EventPopup_hpp
#define EventPopup_hpp

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
class EventPopup : public PageBase
{
private:
    cocos2d::Size size;
public:
    Sprite* sptBackground;
    Sprite* sptTitle;
    
    virtual bool init();
    CREATE_FUNC(EventPopup);
  
    void update(float dt);
    Node* layer;
    int month = 0;
    int day = 0;
    void addMonthlyEvent();
    void onMonthlyEventClick();
    void onMidMonthEventClick();
};

#endif /* EventPopup_hpp */
