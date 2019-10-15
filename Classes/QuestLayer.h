//
//  HeroPage.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 05/09/2019.
//

#ifndef QuestLayer_hpp
#define QuestLayer_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "PageBase.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class QuestLayer : public PageBase
{
private:
    Size size;
public:
    virtual bool init();
    CREATE_FUNC(QuestLayer);
    
};
#endif /* HeroPage_hpp */
