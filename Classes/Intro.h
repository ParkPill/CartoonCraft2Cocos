//
//  Intro.hpp
//  SantaMountain-mobile
//
//  Created by PACKSUNG PILL on 22/12/2017.
//

#ifndef Intro_hpp
#define Intro_hpp

#include "SneakyJoystickSkinnedBase.h"
#include "SneakyJoystick.h"

#include "ui/UIWidget.h"
#include "ui/UISlider.h"
#include "ui/UIScrollView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"
#include "ui/UILoadingBar.h"
#include "cocos2d.h"
#include "LegendDaryButton.h"
#include "Movable.h"
#include "base/CCEventListenerController.h"
#include "base/CCEventController.h"
#include "base/CCController.h"
#include "ui/UIHelper.h"
#include "ui/UIButton.h"
#include "ui/UIListView.h"
#include "ui/UIText.h"
#include "ui/UITextField.h"
#include "ui/UITextBMFont.h"
#include "ui/UIImageView.h"
#include "ui/UILoadingBar.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "AwesomeDialogBox.h"

using namespace cocos2d;

using namespace cocos2d::ui;
using namespace cocostudio;
class Intro : public LayerColor
{
private:
    
public:
    cocos2d::Vec2 center;
    cocos2d::Size size;
    virtual bool init();
    CREATE_FUNC(Intro);
    Movable* player = nullptr;
    void setIntro(int index);
    void updateGame(float dt);
    void setViewPointCenter(Vec2 position);
    void goToNextScene();
    void startAnimation();
};


#endif /* Intro_hpp */
