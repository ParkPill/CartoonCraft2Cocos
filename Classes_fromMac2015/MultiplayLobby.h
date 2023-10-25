//
//  MultiplayLobby.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 06/12/2019.
//

#ifndef MultiplayLobby_hpp
#define MultiplayLobby_hpp

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
class MultiplayLobby : public PageBase
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(MultiplayLobby);
    
    void update(float dt);
    void onFindMatchClick(Ref* ref);
};


#endif /* MultiplayLobby_hpp */
