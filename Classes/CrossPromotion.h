//
//  CrossPromotion.hpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 01/12/2018.
//

#ifndef CrossPromotion_hpp
#define CrossPromotion_hpp

//
//  Title.hpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 13/03/2018.
//

#include <stdio.h>
#include "json/document.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpClient.h"
#include "CCUrlSprite.h"
#include "ui/UIButton.h"

using namespace cocos2d;
using namespace cocos2d::ui;
class CrossPromotion : public Layer
{
private:
    cocos2d::Size size;
public:
    virtual bool init();
    CREATE_FUNC(CrossPromotion);
    
    void getCrossPromotion(std::string thisGameForException);
    void onCrossPromotionCompleted(cocos2d::Node *sender, void *data);
    void update(float dt);
    CCUrlSprite* sptFromUrl;
    std::string strImgUrl;
    std::string strAppID;
    std::string strName;
    bool isInfoArrived = false;
    bool isCloseAvailable = false;
    void setCloseAvailable();
    void closeThis();
    Button* btnClose;
    Button* btnBlock;
    Button* btnFrame;
    bool isWaitingForImage = false;
    std::string serverUrl;
    std::string imgServerUrl;
    void onImageClick();
};


#endif /* CrossPromotion_hpp */
