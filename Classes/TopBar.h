//
//  TopBar.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#ifndef TopBar_hpp
#define TopBar_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"

#define DEFAULT_LEAGUE_INDEX 3
using namespace cocos2d;
using namespace cocos2d::ui;
class TopBar : public Layer
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(TopBar);
    
    void update(float dt);
    Node* ndTopBar;
    Text* lblGemInHud = nullptr;
    int lastGemUpdated;
    Text* lblGoldInHud = nullptr;
    int lastGoldUpdated;
    Text* lblTreeInHud = nullptr;
    int lastTreeUpdated;

    void onGoldClick();
    void onTreeClick();
    void onGemClick();
    Node* getShopLayer();
    
    void addGem(Ref* ref);
    void addLumber(Ref* ref);
    void addGold(Ref* ref);
};


#endif /* TopBar_hpp */
