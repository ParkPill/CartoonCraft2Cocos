//
//  Intro.cpp
//  SantaMountain-mobile
//
//  Created by PACKSUNG PILL on 22/12/2017.
//

#include "Intro.h"
#include "Title.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "BuggyServerManager.h"
bool Intro::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !PageBase::init())
    {
        return false;
    }

    LayerColor* colorLayer = LayerColor::create(Color4B(51, 51, 51, 255));
    this->addChild(colorLayer, -1);

    size = Director::getInstance()->getWinSize();
    center = cocos2d::Vec2(size.width/2, size.height/2);
    
    int server = UDGetInt(KEY_SERVER, -1);
    log("server selected intro %d", server);
    if (server < 0) {
        std::string strName = UDGetStr(KEY_NAME, "");
        if (strName.length() > 0) {
            UDSetInt(KEY_SERVER, 0);
            this->runAction(Sequence::create(DelayTime::create(0), CallFunc::create(CC_CALLBACK_0(Intro::startAnimation, this)), NULL)); // test
        }else{
            showUserSelect();
        }
    }else{
        this->runAction(Sequence::create(DelayTime::create(0), CallFunc::create(CC_CALLBACK_0(Intro::startAnimation, this)), NULL)); // test
    }
//    UDSetStr(KEY_NAME, ""); // test
//    UDSetStr(KEY_SAVED_ID, "-1");
//    showUserSelect();
    return true;
}
void Intro::showUserSelect(){
    Node* layer = CSLoader::createNode("UserSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
//    setAsPopup(layer);
    layer->setName("userSelect");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn;Text* lbl;
    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)background->getChildByName("lblTitle");

    setTitle(layer->getChildByName("titleBack"), "new user");

    lbl = (Text*)background->getChildByName("lblDesc");
    LM->setLocalizedString(lbl, "is new user");

    btn = (Button*)background->getChildByName("btnNew");
    btn->addClickEventListener(CC_CALLBACK_1(Intro::onNewUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "new user");
    btn = (Button*)background->getChildByName("btnExist");
    btn->addClickEventListener(CC_CALLBACK_1(Intro::onOldUserSelect, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "existing user");
}
void Intro::onNewUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
//    showServerSelect(true);
    UDSetInt(KEY_SERVER, 1);
    UD->flush();
    goToNextScene();
}
void Intro::onOldUserSelect(Ref* ref){
    BTN_FROM_REF_AND_DISABLE_FOR_A_SEC
    showServerSelect(false);
}
void Intro::showServerSelect(bool isNewUser){
    Node* layer = CSLoader::createNode("ServerSelect.csb");
    this->addChild(layer, POPUP_ZORDER);
//    setAsPopup(layer);
    layer->setName("serverSelect");
    layer->setPositionX(size.width/2 - layer->getContentSize().width/2);
    Button* btn;Text* lbl;
//    Node* background = layer->getChildByName("imgBackground");
    lbl = (Text*)layer->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, "server select");
    
    lbl = (Text*)layer->getChildByName("lblDescription");
    LM->setLocalizedString(lbl, "server tip");
    
    btn = (Button*)layer->getChildByName("btn0");
    btn->addClickEventListener(CC_CALLBACK_1(Intro::onServerSelected, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "alfred");
    btn->setEnabled(!isNewUser);
    
    btn = (Button*)layer->getChildByName("btn1");
    btn->addClickEventListener(CC_CALLBACK_1(Intro::onServerSelected, this));
    lbl = (Text*)btn->getChildByName("lbl");
    LM->setLocalizedString(lbl, "bungee");
    
    Node* imgCheck = layer->getChildByName("imgCheck");
    imgCheck->setPosition(btn->getPosition());
    imgCheck->setVisible(false);
    
    btn = (Button*)layer->getChildByName("btnMoveServer");
    btn->setVisible(false);
    
    btn = (Button*)layer->getChildByName("btnOk");
    btn->setVisible(false);
    
    int server = UDGetInt(KEY_SERVER, -1);
    btn->setEnabled(server != 0);
    if(server == 0){
        imgCheck->setPosition(layer->getChildByName("btn0")->getPosition());
    }else if(server == 1){
        imgCheck->setPosition(layer->getChildByName("btn1")->getPosition());
    }else{
        imgCheck->setPosition(layer->getChildByName("btn0")->getPosition());
    }
}
void Intro::onServerSelected(Ref* ref){
    BTN_FROM_REF
    Node* layer = this->getChildByName("serverSelect");
    
    int clickedIndex = Value(btn->getName().substr(3)).asInt();
    UDSetInt(KEY_SERVER, clickedIndex);
    BSM->setServerUrl();
    UD->flush();
    goToNextScene();
}
void Intro::startAnimation(){
    Sprite* sptLogo = Sprite::create("studioNapMouthEmpty.png");
    this->addChild(sptLogo);
    sptLogo->setPosition(size/2);
    sptLogo->setScale(0.8f);
    
    Sprite* spt = Sprite::create("studioNapEyes.png");
    sptLogo->addChild(spt);
    spt->setAnchorPoint(Vec2(0, 0));
    spt->setPosition(Vec2(364, 430 - 239));
    float eyesMoveY = 10;
    spt->runAction(Sequence::create(DelayTime::create(0), EaseInOut::create(MoveBy::create(1, Vec2(0, eyesMoveY)), 1.5f),DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(1, Vec2(0, -eyesMoveY)), 1.5f), NULL));
    
    spt = Sprite::create("studioNapMouth.png");
    sptLogo->addChild(spt);
    spt->setAnchorPoint(Vec2(0.5, 0));
    spt->setPosition(Vec2(374 + 33, 430 - 289));
    spt->runAction(Sequence::create(DelayTime::create(0), EaseInOut::create(ScaleTo::create(1, 1.2f), 1.5f),DelayTime::create(0.15f), EaseInOut::create(ScaleTo::create(1, 1), 1.5f), NULL));
    
    // 767, 430 entire
    //    364, 239 eye start
    // 373, 289 mouth start
    // mouth 66, 43
    
    spt = Sprite::create("uiBox.png");
    this->addChild(spt);
    spt->setPosition(size/2);
    spt->setScale(100);
    spt->runAction(Sequence::create(FadeOut::create(0.5f), DelayTime::create(1.5), FadeIn::create(0.5), CallFunc::create(CC_CALLBACK_0(Intro::goToNextScene, this)), NULL));
}

void Intro::setIntro(int index){
   
}

void Intro::updateGame(float dt){
    
}
void Intro::setViewPointCenter(Vec2 position)
{
   
}
void Intro::goToNextScene(){
    Scene* scene = Scene::create();
    scene->addChild(Title::create());
    Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B::BLACK));
}
void Intro::setTitle(Node* titleBack, std::string menuKey){
    Text* lbl = (Text*)titleBack->getChildByName("lblTitle");
    LM->setLocalizedString(lbl, menuKey);
    ImageView* img = (ImageView*)titleBack;
    img->setContentSize(cocos2d::Size(lbl->getContentSize().width*lbl->getScaleX() + 100, 104.39f));
    img->setScale9Enabled(true);
    lbl->setPositionX(titleBack->getContentSize().width/2);
}
