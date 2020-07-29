//
//  Laser.cpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 8/4/16.
//
//

#include "Laser.h"
#include "GameManager.h"
Laser* Laser::create(std::string laserName, std::string gatheringName, std::string hitAniName){
    Laser* laser = new Laser();
    laser->init();
    Sprite* beam = Sprite::create(laserName);
    laser->sptBeam = beam;
    laser->addChild(laser->sptBeam);
    laser->sptHit = Sprite::create(gatheringName);
    Animation* animation = AnimationCache::getInstance()->getAnimation(hitAniName);
    if (animation != nullptr) {
        Animate* animate = Animate::create(animation);
        laser->sptHit->runAction(RepeatForever::create(animate));
    }else{
        laser->sptHit->runAction(RepeatForever::create(RotateBy::create(1, 460)));
    }
    laser->addChild(laser->sptHit);
    
    laser->setAnchorPoint(Vec2(0, 0.5));
    laser->sptBeam->setAnchorPoint(Vec2(0, 0.5));
    laser->schedule(schedule_selector(Laser::updateLaser), 0.07f);
    laser->gatheringImageName = gatheringName;
    return laser;
}
void Laser::changeLook(std::string laserName, std::string gatheringName, std::string hitAniName){
    sptHit->stopAllActions();
    Animation* animation = AnimationCache::getInstance()->getAnimation(hitAniName);
    Animate* animate = Animate::create(animation);
    sptHit->runAction(RepeatForever::create(animate));
}
Laser::Laser(){
    maxLength = 200;
    gatheringIntervalMax = 0.2;
    gatheringInterval = gatheringIntervalMax;
}
void Laser::setLaser(cocos2d::Vec2 startPos, float angle){
    this->setPosition(startPos);
    laserAngle = angle;
}
void Laser::setLaserOn(bool isOn){
    this->setVisible(true);
}
void Laser::updateLaser(float dt){
    if (!isVisible()) {
        return;
    }
    Vec2 finalPos;
    float length = 10;
    bool checked = false;
    while (true) {
        float x= cos(-laserAngle*3.14/180)*length;
        float y = sin(-laserAngle*3.14/180)*length;
        
        for (int i = 0; i < rectArray.size(); i++) {// tile
            if (rectArray.at(i).containsPoint(getPosition() + Vec2(x, y))) {
                checked = true;
                break;
            }
        }
        for (int i = 0; i < enemyArray.size(); i++) {// enemy
            if (enemyArray.at(i)->getBoundingBox().containsPoint(getPosition() + Vec2(x, y))) {
                checked = true;
                hitEnemy = enemyArray.at(i);
                break;
            }
        }
        
        if (checked) {
            break;
        }else{
            hitEnemy = nullptr;
        }
        length += 1;
        if (length > maxLength) {
            break;
        }
    }
    endPos = finalPos;
    this->setRotation(laserAngle);
    timer+=dt;
    if (timer > 0.05f) {
        timer = 0;
        gointUp = !gointUp;
        if (!gointUp) {
            scaleY = 1;
        }
    }
    if (gointUp) {
        scaleY += 0.1f;
    }else{
        scaleY -= 0.1f;
    }
    Sprite* sptGathering = Sprite::create(gatheringImageName);
    this->addChild(sptGathering);
    int gatheringRadius = 8;
    sptGathering->runAction(RotateBy::create(0.5, 180));
    sptGathering->setPosition(Vec2(cos((rand()*90 - 90)*3.14/180)*gatheringRadius, -sin((rand()*90 - 90)*3.14/180)*gatheringRadius));
    sptGathering->runAction(Sequence::create(EaseIn::create(MoveTo::create(0.3, Vec2(0, this->getContentSize().height/2)), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptGathering)), NULL));
    sptBeam->setScale(length/sptBeam->getContentSize().width, scaleY);
    sptHit->setPosition(Vec2(length, 0));
//    sptHit->setRotation(-laserAngle);
}
void LightningLine::createLine(std::string line, std::string tip, Vec2 startPos, Vec2 endPos, Node* parent){
    LightningLine* sptStartTip = new LightningLine();
    sptStartTip->initWithFile(tip);
    sptStartTip->getTexture()->setAntiAliasTexParameters();
    sptStartTip->setAnchorPoint(Vec2(1, 0.5));
    sptStartTip->setPosition(startPos);
    float angle = -atan2((endPos.y - startPos.y), (endPos.x - startPos.x))*180/3.14f;
    sptStartTip->setRotation(angle);
    //sptStartTip->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptStartTip);
    float delay = 0.01f;
    float fade = 0.6f;
    sptStartTip->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
    float length = sqrtf((endPos.y - startPos.y)*(endPos.y - startPos.y) + (endPos.x - startPos.x)*(endPos.x - startPos.x));
    
    Sprite* sptLine = Sprite::create(line);
    sptLine->setAnchorPoint(Vec2(0, 0.5));
    sptLine->getTexture()->setAntiAliasTexParameters();
    sptLine->setPosition(startPos);
    sptLine->setRotation(angle);
    sptLine->setScaleX(length/sptLine->getContentSize().width);
    //sptLine->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptLine);
    sptLine->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
    
    Sprite* sptEndTip = Sprite::create(tip);
    sptEndTip->getTexture()->setAntiAliasTexParameters();
    sptEndTip->setAnchorPoint(Vec2(1, 0.5));
    sptEndTip->setRotation(180 + angle);
    sptEndTip->setPosition(endPos);
    //sptEndTip->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
    parent->addChild(sptEndTip);
    sptEndTip->runAction(Sequence::create(DelayTime::create(delay), EaseOut::create(FadeOut::create(fade), 2), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptStartTip)), NULL));
}
Lightning* Lightning::create(std::string lineName, std::string tipName, cocos2d::Vec2 startPos, float angle, Vector<EnemyBase*> enemyArray, std::vector<cocos2d::Rect> rectArray){
    Lightning* lightning = new Lightning();
    lightning->init();
//    lightning->setSpriteFrame(lineName);
    
    Vec2 finalPos;
    float length = 10;
    bool checked = false;
    while (true) {
        float x= cos(-angle*3.14/180)*length;
        float y = sin(-angle*3.14/180)*length;
        
        for (int i = 0; i < rectArray.size(); i++) {// tile
            finalPos = startPos + Vec2(x, y);
            if (rectArray.at(i).containsPoint(finalPos)) {
                checked = true;
                break;
            }
        }
        
        if (checked) {
            break;
        }
        
        length += 1;
        if (length > lightning->maxLength) {
            break;
        }
    }
//    angle += 90;
    float finalLength = length;
    int nextLength = 0;
    int lengthUnit;
    Vec2 nextPos;
    Vec2 previousPos = startPos;
    int extraLength = 5;
    int minLengthUnit = 5;
    int verticalGap = 5;
    int verticalGapMade;
    lightning->hitPointArray = PointArray::create(200);
    lightning->hitPointArray->addControlPoint(startPos);
    angle = -angle;
    Vec2 zagPos;
    int counter = 0;
    Node* light;
    while(true){
        lengthUnit = minLengthUnit + rand()%extraLength;
        nextLength += lengthUnit;
        verticalGapMade = rand()%(verticalGap*2) - verticalGap;
        if (finalLength < nextLength) {
            nextLength = finalLength;
            zagPos = Vec2::ZERO;
        }else{
            zagPos = Vec2(cos((angle+90)*3.14/180)*verticalGapMade, sin((angle+90)*3.14/180)*verticalGapMade);
        }
        
        nextPos = Vec2(cos(angle*3.14/180)*nextLength, sin(angle*3.14/180)*nextLength) + startPos + zagPos;
        for (int i = 0; i < enemyArray.size(); i++) {// tile
             if (enemyArray.at(i)->getBoundingBox().containsPoint(nextPos)) {
                 if (!lightning->hitEnemyArray.contains(enemyArray.at(i))) {
                     lightning->hitEnemyArray.pushBack(enemyArray.at(i));
                 }
             }
        }
        LightningLine::createLine(lineName, tipName, previousPos, nextPos, lightning);
        if (counter%4 == 0) {
            light = GameManager::getInstance()->getWorld()->getLight();
            light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
            light->setColor(Color3B(50, 86, 250));
            lightning->addChild(light);
            light->setPosition(nextPos);
        }
        previousPos = nextPos;
        counter++;
        if (finalLength == nextLength) {
            break;
        }
    }
    float dur = 0.5;
    for (int i = 0; i < 8; i++) {
        Sprite* sptSpark = Sprite::create("smallLight.png");
        sptSpark->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        lightning->addChild(sptSpark);
        sptSpark->setPosition(nextPos);
        sptSpark->setScale(2);
        sptSpark->runAction(FadeOut::create(dur));
        sptSpark->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Vec2(rand()%30 - 15, rand()%10 - 5)), 3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptSpark)), NULL));
    }
    light = GameManager::getInstance()->getWorld()->getLight();
    light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(50, 86, 250));
    lightning->addChild(light);
    light->setPosition(startPos);
    
    light = GameManager::getInstance()->getWorld()->getLight();
    light->runAction(Sequence::create(DelayTime::create(0.05), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, light)), NULL));
    light->setColor(Color3B(50, 86, 250));
    lightning->addChild(light);
    light->setPosition(finalPos);
    return lightning;
}
