//
//  Enemies.h
//  LegendDaryTheTower
//
//  Created by SUNG PILL PACK on 14. 6. 13..
//
//

#ifndef __LegendDaryTheTower__Enemies__
#define __LegendDaryTheTower__Enemies__


#include "cocos2d.h"
#include "Movable.h"
#include "ui/UIHelper.h"
#include "editor-support/cocostudio/CCArmature.h"
#include "editor-support/cocostudio/CCArmatureDataManager.h"
#include "editor-support/cocostudio/CCArmatureDefine.h"
#include "editor-support/cocostudio/CCDataReaderHelper.h"
#include "editor-support/cocostudio/CCDatas.h"
#include "editor-support/cocostudio/CCSkin.h"
#include "editor-support/cocostudio/CocoStudio.h"


using namespace cocos2d;
#define ENEMY_GUN_TYPE_NONE 0
#define ENEMY_GUN_TYPE_HANDGUN 1
#define ENEMY_GUN_TYPE_RIFLE 2
#define ENEMY_GUN_TYPE_WAND 3

#define SELLING_ITEM_WEAPON 0
#define SELLING_ITEM_HEART -1
#define SELLING_ITEM_AMMO -2
#define SELLING_ITEM_KEY -3
#define SELLING_ITEM_DRAGON_BABY -4
#define SELLING_ITEM_DRAGON_ADOLESCENT -5
#define SELLING_ITEM_DRAGON_ADULT -6
#define SELLING_ITEM_DRAGON_SENIOR -7
#define SELLING_ITEM_ -9
///////////////////////////
// EnemyBase
///////////////////////////
class EnemyBase : public Movable
{
private:
    
public:
    virtual void resumeWalking(float dt);
    virtual void movingSchedule(float dt);
    EnemyBase();
    ~EnemyBase();
    void stopSchedule();
    int orderAction;
    int currentAction;
    bool isShieldUp = false;
    
    int currentMoveType;
    void setCurrentMoveType(int move);
    int moveTypeBeforeFindHero;
    int moveTypeAfterFindHero;
    int actionTypeBeforeFindHero;
    int actionTypeAfterFindHero;
    int rewardExp = 0;
    int rewardCoin = 0;
    Sprite* sptShield = nullptr;
    void setShield(std::string shield);
    std::string fireAniName="";
    cocos2d::Point wanderStart;
    cocos2d::Point wanderEnd;
    
    void hideEnergy();
    
    int rewards = 1;
//    int effectType;
    void initEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance);
    virtual void runSchedule();

    
    static EnemyBase* createEnemy(int unit, int eng, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity);
    static EnemyBase* createEnemy(int unit, int eng, int sped, const char* sptName);
    static EnemyBase* createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    static EnemyBase* createEnemy(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int actionBeforeFindHero, int actionAfterFindHero, int monitorDistance);
};

///////////////////////////
// FireableBase
///////////////////////////
class FireableBase : public EnemyBase
{
private:
    
public:
    Sprite* sptGun=nullptr;
    FireableBase();
    ~FireableBase();
    static FireableBase* create(int unit, int eng, float extraSpd, const char* sptName, int rewardsCoinCount, int goldCoinPossiblity, int howToMoveBeforeFindHero, int howToMoveAfterFindHero, int monitorDistance);
    
    float fireCoolTime;
    float fireCoolTimeMax;
    Vector<Movable*> missileArray;
    float missileAngle;
};

///////////////////////////
// hidden spike
///////////////////////////
class HiddenSpike : public EnemyBase
{
private:
    
public:
    HiddenSpike();
    ~HiddenSpike();
    static HiddenSpike* create();
    
    void update(float dt);
};

///////////////////////////
// spike wall
///////////////////////////
class SpikeWall : public EnemyBase
{
private:
    bool isReadyToMove = false;
public:
    SpikeWall();
    ~SpikeWall();
    static SpikeWall* create();
    void update(float dt);
    virtual void encounteredWall();
};


///////////////////////////
// swing axe
///////////////////////////
class SwingAxe : public EnemyBase
{
private:
    
public:
    SwingAxe();
    ~SwingAxe();
    static SwingAxe* create();
    
};
///////////////////////////
// flying knife
///////////////////////////
class FlyingKnife : public EnemyBase
{
private:
    
public:
    FlyingKnife();
    ~FlyingKnife();
    static FlyingKnife* create();
    
};

#endif
