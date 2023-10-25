//
//  Intro.cpp
//  SantaMountain-mobile
//
//  Created by PACKSUNG PILL on 22/12/2017.
//

#include "Intro.h"
#include "Title.h"
#include "GameManager.h"
bool Intro::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B::BLACK))
    {
        return false;
    }
    size = Director::getInstance()->getWinSize();
    center = cocos2d::Vec2(size.width/2, size.height/2);
    
    this->runAction(Sequence::create(DelayTime::create(0), CallFunc::create(CC_CALLBACK_0(Intro::startAnimation, this)), NULL)); // test 
    return true;
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
