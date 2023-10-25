//
//  EndingCredit.hpp
//  Pigwing
//
//  Created by PACKSUNG PILL on 12/7/15.
//
//

#ifndef EndingCredit_hpp
#define EndingCredit_hpp

#include "cocos2d.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UILoadingBar.h"
#include "editor-support/cocostudio/CocoStudio.h"
//#include "editor-support/cocostudio/CocoStudio.h"
#include "base/CCEventListenerController.h"
#include "base/CCEventController.h"
#include "base/CCController.h"


using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;
class EndingCredit : public Layer
{
private:
    
public:
    float speedY;
    float speedX;
    float tileWidth;
    Layer* backgroundLayer;
    bool tileSpeedy;
    float backTileStartX;
    
    cocos2d::Size size;
    virtual bool init();
    CREATE_FUNC(EndingCredit);
    void goToMain();
    void spriteMoveDone(Node* node);
    void backgroundSchedule(float dt);
};


#endif /* EndingCredit_hpp */
