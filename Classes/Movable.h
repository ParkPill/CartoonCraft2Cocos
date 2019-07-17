//
//  Movable.h
//  AlmostLegendary
//
//  Created by SUNG PILL PACK on 13. 6. 26..
//
//

#ifndef AlmostLegendary_Movable_h
#define AlmostLegendary_Movable_h

#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
#include "cocos2d.h"
#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "EventInfo.h"
#define CLIFF_NONE 0
#define CLIFF_LEFT 1
#define CLIFF_RIGHT 2
#define TILE_SIZE 100
#define ANIMATION_TYPE_IDLE 0
#define ANIMATION_TYPE_MOVE 1
#define ANIMATION_TYPE_ATTACK 2
#define SECOND_TAG_NULL -99372

#define UNIT_ACT_NONE 0
#define UNIT_ACT_MOVE 1
#define UNIT_ACT_ATTACK_DDANG 2
#define UNIT_ACT_ATTACK 3
#define UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH 4
#define UNIT_ACT_GATHER_GOLD 5
#define UNIT_ACT_GATHER_TREE 6

#define UNIT_ACT_DETAIL_IDLE 0
#define UNIT_ACT_DETAIL_WALK_STRAIGHT_TO_TARGET 1
#define UNIT_ACT_DETAIL_WALK_ROUTE 2
#define UNIT_ACT_DETAIL_ATTACK 3
//#define UNIT_ACT_DETAIL_ 4


#define AURA_LIGHTNING 0

#define MISSILE_SLASH 0
#define MISSILE_ARROW 1
#define MISSILE_FIREBALL 2
#define UNIT_STATE_IDLE 0
#define UNIT_STATE_UPGRADE 1
#define UNIT_STATE_BUILDING 2
#define UNIT_STATE_REMOVING 3

#define UNIT_WORKER 0
#define UNIT_SWORDMAN 1
#define UNIT_ARCHER 2
#define UNIT_CATAPULT 3
#define UNIT_HELICOPTER 4
#define UNIT_AIRPORT 5
#define UNIT_BARRACKS 6
#define UNIT_CASTLE 7
#define UNIT_FACTORY 8
#define UNIT_FARM 9
#define UNIT_LUMBERMILL 10
#define UNIT_MINE 11
#define UNIT_WATCHERTOWER 12
#define UNIT_METEOR 13
#define UNIT_MISSILE_STRAIGHT 15
#define UNIT_MISSILE_CHASING 16
#define UNIT_DESTRUCTABLE 17
#define UNIT_MISSILE_CUSTOM 18
#define UNIT_MISSILE_Movable 19
#define UNIT_ITEM 20
#define UNIT_NPC 21
#define UNIT_TREE 22
#define UNIT_ORC_AXE 23
#define UNIT_ORC_SPEAR 24
#define UNIT_GOBLIN 25
#define UNIT_GOBLIN_BOMB 26
#define UNIT_TROLL 27
#define UNIT_ORC_BUNKER 28
#define UNIT_ORC_HQ 29
#define UNIT_ZOMBIE_ORC_AXE 30
#define UNIT_ZOMBIE_SWORDSMAN 31
#define UNIT_ZOMBIE_CASTLE 32
#define UNIT_ZOMBIE_HQ 33
#define UNIT_LAMINGTON 34
#define UNIT_UNREACHABLE_TREE 35
#define UNIT_START_POINT 36
#define UNIT_EVENT_POINT 37
#define UNIT_UNDERGROUND_BUNKER 38
#define UNIT_TRIGGER 39
#define UNIT_WIZARD 40
#define UNIT_TEMPLE 41
#define UNIT_ORC_BARRACKS 42
#define UNIT_ORC_TROLL_HOUSE 43
#define UNIT_TREE_FOR_BATTLE 44
#define UNIT_GOBLIN_WORKER 45
#define UNIT_BARBECUE 46
#define UNIT_HERO_ORC 47

#define WEAPON_DAGGER 0
#define WEAPON_SWORD 1
#define WEAPON_LIGHTSWORD 2
#define WEAPON_RAPIER 3
#define WEAPON_UBASHIHRI 4
#define WEAPON_GIANTSWORD 5
#define WEAPON_ICESWORD 6
#define WEAPON_FIRESWORD 7
#define WEAPON_MOONSPEAR 8
#define WEAPON_HOLYMACE 9
#define WEAPON_NEEDLEPUNCH 10
#define WEAPON_SIGNOFDEATH 11
#define WEAPON_SHARKHUNTER 12
#define WEAPON_WHALEHUNTER 13
#define WEAPON_BLUEDRAGONSAVIOR 14
#define WEAPON_SCREWBAR 15
#define WEAPON_MAPLESOWRD 16
#define WEAPON_CRECENT 17
#define WEAPON_CRAZYSOLAR 18
#define WEAPON_SOULEATER 19
#define WEAPON_DRAGONSLAYER 20
#define WEAPON_DARKSWORD 21
#define WEAPON_XCALIBUR 22
#define WEAPON_DARKSPEAR 23
#define WEAPON_LIGHTSAVIOR 24
#define WEAPON_BOW 25
#define WEAPON_WAND 26
#define WEAPON_HAMMER 27
#define WEAPON_NEEDLE 28
#define WEAPON_NONE 30

#define ATTACK_TYPE_NEAR 0
#define ATTACK_TYPE_RANGE 1
#define ATTACK_TYPE_NONE 2

#define ATTACK_TARGET_TYPE_SINGLE 0
#define ATTACK_TARGET_TYPE_SPLASH 1

#define GROUND_EFFECT_LIGHT 0
#define GROUND_EFFECT_DARK 1


struct UnitInfo
{
    int unitType;
    int level=0;
    float x=-1;
    float y=-1;
    double endTime = 0;
    int unitState = UNIT_STATE_IDLE;
};

using namespace cocos2d;

class Movable : public cocos2d::Sprite
{
private:
    
public:
    
    float waterAccelX ;
    float waterMaxX ;
    float waterAccelYN;
    float waterAccelYP;
    float waterMinY;
    bool doubleJump;
    Movable();
    ~Movable(){}
    
    int unitAct = UNIT_ACT_NONE;
    float restingTime = 0;
    int unitActDetail = UNIT_ACT_DETAIL_IDLE;
    Vec2 moveToPos = Vec2::ZERO;
    Vec2 moveFlagPos = Vec2::ZERO;

    cocos2d::Point centerPosition;
    cocos2d::Point velocity;
    cocos2d::Point desiredPosition;
    cocos2d::Rect demageRect;
    Movable* target = nullptr;
    Movable* dummyTarget = nullptr;
    virtual void setFlippedX(bool flip);
    void resetRoute();
    void addRoute(cocos2d::Point dest);
    void move(float dt);
    void moveNew(float dt);
    void stopNew();
    void stop(bool search = true);
    bool canFindTarget = false;
    bool canGather = false;
    void gatherGold(Movable* mine);
    bool isGatheringGold = false;
    float gatheringTimer = 0;
    Movable* currentMine = nullptr;
    bool isCarryingGold = false;
    void gatherTree(Movable* tree);
    void attackTree();
    void playTreeSound();
    bool isGatheringTree = false;
    Movable* currentTree = nullptr;
    bool isCarryingTree = false;
    
    bool isGoingToBuild = false;
    cocos2d::Point builderCoordinate;
    cocos2d::Size builderSize;
    std::string builderSpriteName;
    int builderBuildingIndex;
    Movable* builderBuilding = nullptr;
    double buildingCompleteTime;
    double buildingCompleteTimeLeft=0;
    bool isBuildingComplete = false;
    void onBuildComplete();
    void startProductSchedule();
    void stopProductSchedule();
    int jobStartedTime;
    std::string jobDetail;
    void updateProductButtons();
    void queueUnit(int index);
    void cancelProduct(Ref* ref);
    bool forceAttackTarget = false;
    
    Vector<cocos2d::ui::Button*> btns;
    void productUpdate(float dt);
    float productCompleteTimer = 0;
    
    Movable* returningPlace = nullptr;
    bool isFlying = false;
    bool isTemporaryFlying = false;
    void releaseGathering();
    int delayFrameTimeBeforeMove = 0;
    PointArray* routePositionArray = nullptr;

    bool isArrived = true;
    std::string lastRunAniName = "";
    int movePointCounter = 0;
    float oneWayCancelY;
    int energy;
    int maxEnergy;
    int ap = 5;
    int dp = 1;
    int gp = 1;
    int sp = 2;
    int ySpeed;
    int xSpeed;
//    int state;
    
    
    int currentAnimationType = -1;
    bool freezed;
    float freezeTime = 1.4f;
    bool fixAngleToTargetWhenReleaseFreezing=false;
    void freezeForAWhile(float dur);
    void releaseFreezing();
    int extraSpeed = 10;
    int extraJump;
    int effectType;
    int movingAngle;
    float weight;
    int unitType = 0;
    float targetPositionCheckTime = 0;
    int attackType = ATTACK_TYPE_NEAR;
    int attackTargetType = ATTACK_TARGET_TYPE_SINGLE;
    int attackRange = 300;
    int goldPossiblity;
    float fireGuage;
    float blinkingTime;
    bool isMissile;
    bool isTowardLeft;
    bool exitingVehicle;
    bool inWater;
    bool untouchable;
    bool immortal;
    bool wantsToJump;
    bool wantsToGoLeft = false;
    bool wantsToGoRight = false;
    bool wantsToGoUp = false;
    bool wantsToGoDown = false;
//    bool wantsToStopVertical;
//    bool wantsToStopHorizontal;
    void move(int direction);
//    void goLeft();
//    void goRight();
//    void goUp();
//    void goDown();
//    void stopVertical();
//    void stopHorizontal();
    int monitoringDistance = 500;
    
    bool isFregile;
    bool ignoreGravity;
    
    bool onGround = true;
//    bool encounteredWall;
//    bool encounteredCliff;
    bool jumped;
    bool tumbled;
    virtual void tumbleAnimation();
    bool iWasInWater;
    bool isDropping = false;
//    void initialize();
    void turnIntoEnemy();
    bool isEnemy = false;
    int alliSide = 0;
    bool isAlli = false;
    bool isBuilding = false;
    bool isDetected = false;
    bool isTargetLockedOn();
    virtual void setInWater(bool inOrOut);
    virtual void updatePosition(float dt);
    void updatePositionForStraight(float dt);
    void updatePositionForBouncing(float dt);
    void updatePositionToFlyToFollowTarget(float dt);
    virtual float getCurrentY();
    void setTarget(Movable* t);
    bool isTargetFound;
    float findTarget(Movable* tar, float dist);
    const char* animationName;
    bool isAnimationRepeat;
    void runAnimationLater(float dt);
    void runAnimation(const char* name, bool repeat);
    void runAnimation(const char* name, bool repeat, float delay);
    void runAnimation(int aniType);
    void fireAnimationDone(Node* nd);
    virtual void onLanded();
    virtual void onJumped();
    virtual void onDropping();
    bool isHurting = true;
    void dontHurt();
    void jumpAnimation();
    bool jumpAllowed;
    void banJump(float dt);
    void setOnGround(bool ground);
    
    int secondTag=SECOND_TAG_NULL;
    std::string talk;
    void showTalk(std::string text, float time, float y, float delay);
    void updateTalkBalloon(float dt);
    float talkBalloonY;
    cocos2d::ui::ImageView* imgTalkBalloon;
    Label* lblTalk;
    float talkVisibleTime=2;
    float talkVisibleTimeElapsed=0;
    virtual void onDead();
    virtual void onHit();
    virtual void encounteredWall();
    virtual void encounteredCliff(int direction);
    virtual cocos2d::Rect damageBoundingBox();
    virtual cocos2d::Rect collisionBoundingBox();
    virtual cocos2d::Rect collectBoundingBox();
    Vector<Sprite*> childrenSprite;
    cocos2d::Rect RectOffset(cocos2d::Rect rect, float x, float y);
    cocos2d::Rect RectInset(cocos2d::Rect rect, float x, float y);
    
    void init(int unit, int eng, float extraSpd, const char* sptName);
    static Movable* createMovable(int unit, int eng, float extraSpd, const char* sptName);
    static Movable* createMovable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName);
    static Movable* createMovable(int unit, int eng, float extraSpd, const char* sptName, const char* animationName, bool repeat);

    virtual cocos2d::Point getGravityPosition();
    void blinkForAWhile();
    void blinkForSec(int sec);
    void blinking(float dt);
    virtual void runSchedule();
    int aimingAngle = 270;
    Sprite* sptWeapon = nullptr;
    Sprite* sptWeaponAura = nullptr;
    
    
    int weaponType = WEAPON_NONE;
    void updateAnimation();
    
    bool isAttacking = false;
    bool isDamaging = true;
    void attackUpdate(float dt);
    void attack();
    bool getHitAndIsDead(int ap, Movable* attacker);
    Movable* missile;
    Movable* shooter = nullptr;
    void onMissileMoveDone();
    
    std::string getCharacterName(int unitType);
    
    
    void showEffect(int effect, float delay);
    
    float attackCoolTime = 0;
    float attackHappenTime = 0;
    float attackCoolTimeMax = 1.5f;
    
    float moveInterval = 0;
    cocos2d::Point targetMoveTilePos = cocos2d::Point::ZERO;
    
    void moveToTarget();
    void moveToTarget(Movable* unit);
    void moveToTarget(cocos2d::Point pos);
    
    
    
    cocos2d::Point attackFlagPos = cocos2d::Point::ZERO;
    cocos2d::Point attackFlagTilePos = cocos2d::Point::ZERO;
    cocos2d::Point failedAttackFlagPos = cocos2d::Point::ZERO;
    bool wantToEli = false;
    cocos2d::Point targetCoordinate;
    
    int groundEffectType = GROUND_EFFECT_LIGHT;
    Node* groundEffect = nullptr;
    void setGroundEffect(int effect);
    void updateGroundEffect(float dt);
    
    bool canMove = true;
    cocos2d::Point approachingPoints[20];
    cocos2d::Point getApproachingPoint(cocos2d::Point from);
    cocos2d::Point lastApprochingPointCheckPosition = cocos2d::Point::ZERO;
    
    cocos2d::Point lastMissilePosition = cocos2d::Point::ZERO;
    void updateMissileAngle(float dt);
    cocos2d::Point buildingStartCoordinate;
    cocos2d::Size buildingOccupySize;
    void refreshApproachPoints();
    int approachPointCount= 0;
    float energyBarY = -1;
    Sprite* energyBar = nullptr;
    Sprite* energyBarContent = nullptr;
    Sprite* energyBarBack = nullptr;
    void updateEnergy();
    
    void makingSmoke(float dt);
    cocos2d::Point smokePoint;
    cocos2d::Point lastValidPoint = cocos2d::Point::ZERO;
    Sprite* sptSelectedCircle = nullptr;
    Node* ndLevelCircle = nullptr;
    Vec2 getEffectStartPosition();
    void showSelectedCircle(bool show);
    void blinkSelectedCircle();
    float timeCollapse = 0;
    int scheduledAttackTime = -1;
    int scheduledProductTime = -1;
    int scheduledProductUnit = -1;
    int scheduledProductUnitCount = -1;
    bool isZombie = false;
    float oneSecTimeChecker = 0;
    float oneSec = 1;
    cocos2d::Point failedFindPathStart = cocos2d::Point::ZERO;
    cocos2d::Point failedFindPathEnd = cocos2d::Point::ZERO;
    std::vector<EventInfo*> eventInfoList;
    std::string sptName;
    
    cocos2d::Point lastTreePos;
    bool wasIDoingLumber = false;
    
    void pauseProcess();
    void resumeProcess();
    ProgressTimer* processTimer = nullptr;
    void resetProcessTimer();
    int level = 0;
    int unitState = 0;
    ui::Text* lblTimeLeft = nullptr;
    Sprite* sptUpgradeArrow = nullptr;
    std::string unitName;
    spine::SkeletonAnimation* spine = nullptr;
    
    bool isSkillOn = false; // test 
    void startSkillAura(int aura);
    void updateSkillAura(float dt);
    void showSkillEffect();
    void stopSkillAura();
    int selectedSkillAura = -1;
    int skillEffectIndex = -1;
    int skillCounter = 0;
    Movable* skillTarget = nullptr;
    
    void checkAttackTarget(float dt);
//    virtual Rect getBoundingBox() const override;
    Rect getBoundingBoxForIntersect();
    int attackTag = 78;
    void cancelAttackSchedule();
};

#endif
