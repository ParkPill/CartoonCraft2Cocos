//
//  CrossPromotion.cpp
//  CartoonCraftNV
//
//  Created by PACKSUNG PILL on 01/12/2018.
//

#include "CrossPromotion.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "BuggyServerManager.h"

bool CrossPromotion::init()
{
    //    UDSetInt(KEY_GOLDEN_TICKET, 0); // test
    //    UDSetInt(KEY_LAST_TICKET_USED_TIME, SM->getCurrentTime() - 30); // test
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init()){
        return false;
    }
    
    this->setScale(3);
    size = Director::getInstance()->getWinSize();
    
    serverUrl = "http://222.120.115.95:8102";
    imgServerUrl = "http://222.120.115.95:80";

    GM->crossPromotionLayer = this;
    
    getCrossPromotion("cartooncraft");
    this->schedule(schedule_selector(CrossPromotion::update), 0.07f);
    return true;
}

void CrossPromotion::getCrossPromotion(std::string thisGameForException){
    std::string platform = "aos";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    platform = "ios";
#endif
    std::string requestData =
    "game=" + thisGameForException + "&" +
    "platform=" + platform;
    
    log("sm request: %s", requestData.c_str());
    
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    request->setUrl(strmake("%s/crosspromotion", serverUrl.c_str()));
    std::vector<std::string> headers;
    headers.push_back("Content-Type:text/plain;charset=utf-8");
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    request->setResponseCallback(this, httpresponse_selector(CrossPromotion::onCrossPromotionCompleted));
    // write the post data
    const char* postData = requestData.c_str();
    request->setRequestData(requestData.c_str(), strlen(postData));
    cocos2d::network::HttpClient::getInstance()->send(request);
    request->release();
}
void CrossPromotion::onCrossPromotionCompleted(cocos2d::Node *sender, void *data){
    rapidjson::Document document = BSM->getDocument(sender, data);
    if(document.IsNull()){
        
        return;
    }
    if (document.HasMember("error")) { // error
        log("sm cross promotion error");
        //        WORLD->sendColosseumScore();
        
    }else { //
//        WORLD->onSendComplete(document["result"].GetInt());
        strAppID = document["appID"].GetString();
        strImgUrl = document["img"].GetString();
        strName = document["game"].GetString();
        isInfoArrived = true;
    }
}
void CrossPromotion::update(float dt){
    if(isInfoArrived){
        isInfoArrived = false;
        
        btnBlock = Button::create("crossPromotion/whiteRect.png");
        btnBlock->setPosition(size/2);
        btnBlock->setScale(size.width/btnBlock->getContentSize().width, size.height/btnBlock->getContentSize().height);
        btnBlock->setOpacity(0);
        btnBlock->setColor(Color3B::BLACK);
        btnBlock->runAction(FadeTo::create(1, 200));
        btnBlock->addClickEventListener(CC_CALLBACK_0(CrossPromotion::closeThis, this));
        this->addChild(btnBlock);
        std::string url = strmake("%s/%s", imgServerUrl.c_str(), strImgUrl.c_str()).c_str();
        sptFromUrl = CCUrlSprite::create(url.c_str());
        this->addChild(sptFromUrl, 1001);
        sptFromUrl->setPosition(Point(size.width/2, size.height + 500));
        
        isWaitingForImage = true;
        
        log("isINfoArrive");
    }
    if(isWaitingForImage && sptFromUrl->isReady){
        isWaitingForImage = false;
        log("url image ready");
        float dur = 0.8f;
        sptFromUrl->runAction(EaseBackOut::create(MoveTo::create(dur, size/2)));
        sptFromUrl->setRotation(60);
        sptFromUrl->runAction(EaseBackOut::create(RotateTo::create(dur, 0)));
        
        
        
        btnFrame = Button::create("crossPromotion/frame.png");
        btnFrame->setCapInsets(Rect(30, 30, 30, 30));
        float scale = 1;
        btnFrame->setScale(scale);
        btnFrame->setScale9Enabled(true);
        float margin = 52;
        btnFrame->setContentSize(Size((sptFromUrl->imgSize.width + margin)/scale, (sptFromUrl->imgSize.height + margin)/scale));
        btnFrame->setPosition(sptFromUrl->getPosition());
        this->addChild(btnFrame);
        btnFrame->runAction(EaseBackOut::create(MoveTo::create(dur, size/2)));
        btnFrame->setRotation(60);
        btnFrame->runAction(EaseBackOut::create(RotateTo::create(dur, 0)));
        btnFrame->addClickEventListener(CC_CALLBACK_0(CrossPromotion::onImageClick, this));
        
        
        btnClose = Button::create("crossPromotion/btnClose.png");
        this->addChild(btnClose);
        btnClose->addClickEventListener(CC_CALLBACK_0(CrossPromotion::closeThis, this));
        int offset = 24;
        btnClose->setPosition(Vec2(size.width/2 + sptFromUrl->imgSize.width/2 + offset,
                                   size.height + sptFromUrl->imgSize.height + offset));
        Vec2 targetPos = Vec2(size.width/2 + btnFrame->getContentSize().width/2 + offset,
                              size.height/2 + btnFrame->getContentSize().height/2 - btnClose->getContentSize().height/2);
        
        btnClose->runAction(Sequence::create(DelayTime::create(0.5f), EaseBackOut::create(MoveTo::create(0.5f, targetPos)), NULL));
        btnClose->setRotation(60);
        btnClose->runAction(Sequence::create(DelayTime::create(0.5f), EaseBackOut::create(RotateTo::create(0.5f, 0)), CallFunc::create(CC_CALLBACK_0(CrossPromotion::setCloseAvailable, this)),NULL));
    }
}
void CrossPromotion::onImageClick(){
    std::string strLog = strmake("Cross %s", strName.c_str()).c_str();
    log("%s", strLog.c_str());
    GameSharing::logFB(strLog.c_str());
    std::string link;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    link = strmake("https://itunes.apple.com/app/id%s", strAppID.c_str());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    link = strmake("https://play.google.com/store/apps/details?id=%s", strAppID.c_str());
#endif
    Application::getInstance()->openURL(link);
}
void CrossPromotion::setCloseAvailable(){
    isCloseAvailable = true;
}
void CrossPromotion::closeThis(){
    if(!isCloseAvailable){
        return;
    }
    GM->crossPromotionLayer = nullptr;
    btnClose->runAction(Sequence::create(DelayTime::create(0.0f), EaseBackIn::create(MoveBy::create(0.5f, Vec2(0, size.height/2))), NULL));
    sptFromUrl->runAction(Sequence::create(DelayTime::create(0.3f), EaseBackIn::create(MoveBy::create(0.5f, Vec2(0, size.height/2))), NULL));
    btnFrame->runAction(Sequence::create(DelayTime::create(0.3f), EaseBackIn::create(MoveBy::create(0.5f, Vec2(0, size.height/2))), NULL));
    btnBlock->runAction(Sequence::create(DelayTime::create(0.5f), FadeOut::create(0.4f), CallFunc::create(CC_CALLBACK_0 (Node::removeFromParent, this)), NULL));
}
