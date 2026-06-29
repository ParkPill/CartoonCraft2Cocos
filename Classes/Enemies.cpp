//
//  Enemies.cpp
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 6. 13..
//
//

#include "Enemies.h"
#include "GameManager.h"
#include "GameScene.h"
#include "LanguageManager.h"




///////////////////////////////////////
// EnemyBase
///////////////////////////////////////
EnemyBase::EnemyBase(){
//    effectType = EFFECT_NONE;
    energyBar = NULL;
    energyBarContent = NULL;
}

EnemyBase::~EnemyBase(){
    stopSchedule();
}
void EnemyBase::stopSchedule(){
    this->unschedule(schedule_selector(EnemyBase::movingSchedule));
}
void EnemyBase::resumeWalking(float dt){
    wantsToGoLeft = isTowardLeft;
    wantsToGoRight = !isTowardLeft;
}
void EnemyBase::setCurrentMoveType(int move){
    currentMoveType = move;
    if (currentMoveType == ENEMY_MOVE_STAND) {
//        stopHorizontal();
//        stopVertical();
    }else if (move == ENEMY_MOVE_WANDER_CROSS_PLATFORM ||
              move == ENEMY_MOVE_WANDER_ON_A_PLATFORM ||
              move == ENEMY_MOVE_WANDER_CROSS_PLATFORM_CRASH_BY_WALL){
        
    }
}
void EnemyBase::runSchedule(){

}
void EnemyBase::movingSchedule(float dt){
    
    
}
void EnemyBase::hideEnergy(){
    if(energyBar != NULL){
        energyBar->setVisible(false);
        energyBarContent->setVisible(false);
    }
}
void EnemyBase::initEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance){
    this->init(unit, eng, extraSpd, sptName);
    this->moveTypeBeforeFindHero = howToMoveBeforeFindHero;
    this->moveTypeAfterFindHero = howToMoveAfterFindHero;
    this->setCurrentMoveType(howToMoveBeforeFindHero);
    this->rewards = rewardsCoinCount;
    this->goldPossiblity = goldCoinPossiblity;
    this->monitoringDistance = monitorDistance;
    this->actionTypeBeforeFindHero = actionBeforeFindHero;
    this->actionTypeAfterFindHero = actionAfterFindHero;
    this->isTowardLeft = false;
//    this->wantsToGoLeft = this->isTowardLeft;
//    this->wantsToGoRight = !this->isTowardLeft;

    this->runSchedule();
}

EnemyBase* EnemyBase::createEnemy(int unit, int eng, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, 0, sptName, rewardsCoinCount, goldCoinPossiblity, ENEMY_MOVE_HANG, ENEMY_MOVE_HANG, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, 100);
    return enemy;
}
EnemyBase* EnemyBase::createEnemy(int unit, int eng, int spd, const char* sptName)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, spd, sptName, 0, 0, ENEMY_MOVE_HANG, ENEMY_MOVE_HANG, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, 100);
    return enemy;
}
EnemyBase* EnemyBase::createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    return enemy;
}

EnemyBase* EnemyBase::createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance)
{
    EnemyBase* enemy = new EnemyBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, actionBeforeFindHero, actionAfterFindHero, monitorDistance);
    return enemy;
}

void EnemyBase::setShield(std::string shield){
    if(sptShield != nullptr){
        sptShield->removeFromParent();
        sptShield = nullptr;
    }
    if(shield.size() == 0) return;
    sptShield = Sprite::createWithSpriteFrameName(GM->getWorld()->shieldStatTable[shield].asValueMap().at("sprite").asString());
    this->getParent()->addChild(sptShield, 11);
    sptShield->setVisible(false);
}

///////////////////////////////////////
// FireableBase
///////////////////////////////////////
FireableBase::FireableBase(){
    fireCoolTimeMax = 1.5f;
    fireCoolTime = fireCoolTimeMax;
}
FireableBase::~FireableBase(){}

FireableBase* FireableBase::create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance)
{
    FireableBase* enemy = new FireableBase();
    enemy->initEnemy(unit, eng, extraSpd, sptName, rewardsCoinCount, goldCoinPossiblity, howToMoveBeforeFindHero, howToMoveAfterFindHero, ENEMY_ACTION_NONE, ENEMY_ACTION_NONE, monitorDistance);
    enemy->sptGun = Sprite::createWithSpriteFrameName("handGun.png");
    enemy->addChild(enemy->sptGun);
    return enemy;
}

// hidden spike
HiddenSpike::HiddenSpike(){
    
}
HiddenSpike::~HiddenSpike(){
    
}
HiddenSpike* HiddenSpike::create(){
    HiddenSpike* spike = new HiddenSpike();
    
    spike->schedule(schedule_selector(HiddenSpike::update), 2);
    spike->immortal = true;
    spike->canMove = false;
    return spike;
}

void HiddenSpike::update(float dt){
    isDamaging = !isDamaging;
    if (isDamaging) {
        GM->runAnimation(this, "hiddenSpikeOn", false);
    }else{
        GM->runAnimation(this, "hiddenSpikeOff", false);
    }
}


// spike wall
SpikeWall::SpikeWall(){
    
}
SpikeWall::~SpikeWall(){
    
}
SpikeWall* SpikeWall::create(){
    SpikeWall* spike = new SpikeWall();
    
    spike->schedule(schedule_selector(SpikeWall::update), 1);
    spike->immortal = true;
    spike->canMove = false;
    spike->wantsToGoLeft = true;
    spike->wantsToGoRight = false;
    return spike;
}

void SpikeWall::update(float dt){
    if (!isReadyToMove) {
        return;
    }
    wantsToGoLeft = !wantsToGoLeft;
    wantsToGoRight = !wantsToGoRight;
    extraSpeed = wantsToGoRight?100:10;
    isReadyToMove = false;
}

void SpikeWall::encounteredWall(){
    if(wantsToGoRight && !isReadyToMove){
        WORLD->shakeScreen(2);
    }
    isReadyToMove = true;
}


// swing axe
SwingAxe::SwingAxe(){
    
}
SwingAxe::~SwingAxe(){
    
}
SwingAxe* SwingAxe::create(){
    SwingAxe* spike = new SwingAxe();
//    spike->init(UNIT_SWING_AXE, 0, 10, "doubleAxe.png");
    spike->immortal = true;
    spike->canMove = false;
    spike->setAnchorPoint(Vec2(0.5, -2));
    spike->runAction(RepeatForever::create(RotateBy::create(1, 200)));
    return spike;
}

// flying knife
FlyingKnife::FlyingKnife(){
    
}
FlyingKnife::~FlyingKnife(){
    
}
FlyingKnife* FlyingKnife::create(){
    FlyingKnife* spike = new FlyingKnife();
//    spike->init(UNIT_FLYING_KNIFE, 0, 30, "spikeWheel.png");
    spike->immortal = true;
    spike->monitoringDistance = 1000;
    spike->runAction(RepeatForever::create(RotateBy::create(0.5f, 360)));
    return spike;
}

