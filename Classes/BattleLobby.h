//
//  BattleLobby.hpp
//  CartoonCraftNV-mobile
//
//  Created by PACKSUNG PILL on 27/12/2018.
//

#ifndef BattleLobby_hpp
#define BattleLobby_hpp


#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIButton.h"
#include "GameManager.h"
#include "GameSharing.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class BattleLobby : public Layer
{
private:
    cocos2d::Size size;
public:
    Sprite* sptBackground;
    Sprite* sptTitle;
    
    virtual bool init();
    CREATE_FUNC(BattleLobby);
    
    Vector<Node*> popupArray;
    bool setAsPopup(Node* node);
    void closePopup();
};


#endif /* BattleLobby_hpp */
