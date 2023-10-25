//
//  FindMatch.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 09/04/2019.
//

#ifndef FindMatch_hpp
#define FindMatch_hpp


#include <stdio.h>
#include "PPLabel.h"
#include "Movable.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"
#include "ui/UIScrollView.h"
#include "ui/UILoadingBar.h"
using namespace cocos2d;
using namespace cocos2d::ui;
class FindMatch : public Layer
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(FindMatch);
    void onCancelClick();
    void update(float dt);
    float minTime = 1.0f;
    
    EventListenerKeyboard* listener;
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    void showErrorMessage();
    void removeListener();
};


#endif /* FindMatch_hpp */
