//
//  PageBase.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 20/08/2019.
//

#ifndef PageBase_hpp
#define PageBase_hpp

#include <stdio.h>
#include "PPLabel.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class PageBase : public Layer
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(PageBase);
    
    Vector<Node*> popupArray;
    bool setAsPopup(Node* node);
    void closePopup();
    void closeThis();
    Node* getPopup();
    void addPopup(Node* node);
    
    void showInstanceMessage(std::string msg, int offset=0);
    void showIndicator();
    void hideIndicator();
    
};

#endif /* PageBase_hpp */
