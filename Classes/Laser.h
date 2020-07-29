//
//  Laser.hpp
//  LegendDaryDungeon
//
//  Created by PACKSUNG PILL on 8/4/16.
//
//

#ifndef Laser_hpp
#define Laser_hpp

#include <stdio.h>
#include "Enemies.h"
#include "cocos2d.h"
using namespace cocos2d;

class Laser : public Sprite
{
private:
    float gatheringInterval;
    float gatheringIntervalMax;
    float laserAngle;
    float scaleY=1;
    float timer=0;
    bool gointUp=false;
    std::string gatheringImageName;
public:
    cocos2d::Sprite* sptHit;
    cocos2d::Sprite* sptBeam;
    Laser();
    std::vector<cocos2d::Rect> rectArray;
//    Vector<cocos2d::Rect> rectArray;
    Vector<EnemyBase*> enemyArray;
    float maxLength;
    EnemyBase* hitEnemy=nullptr;
    cocos2d::Vec2 endPos;
    bool isGatheringOn=false;
    bool isHitOn=false;
    static Laser* create(std::string laserName, std::string gatheringName, std::string hitAniName);
    void changeLook(std::string laserName, std::string gatheringAniName, std::string hitAniName);
    void setLaser(cocos2d::Vec2 startPos, float angle);
    void setLaserOn(bool isOn);
    void updateLaser(float dt);
};

class LightningLine : public Sprite
{
private:
    
public:
    LightningLine(){};
    static void createLine(std::string line, std::string tip, cocos2d::Vec2 startPos, cocos2d::Vec2 endPos, Node* parent);
};

class Lightning : public Sprite
{
private:
    Vector<LightningLine*> lineArray;
    PointArray* hitPointArray;
    float mainAngle;
    float scaleY=1;
public:
    Lightning(){};
    Vector<EnemyBase*> hitEnemyArray;
    float maxLength=200;
    cocos2d::Vec2 endPos;
    static Lightning* create(std::string line, std::string tip, cocos2d::Vec2 startPos, float angle, Vector<EnemyBase*> enemyArray, std::vector<cocos2d::Rect> rectArray);
};


#endif /* Laser_hpp */
