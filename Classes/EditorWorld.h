#ifndef __editor_scene
#define __editor_scene

#include "cocos2d.h"
#include "Movable.h"
#include "PPLabel.h"
#include "Enemies.h"
#include "AwesomeDialogBox.h"
#include "Laser.h"
#include "../cocos2d/cocos/editor-support/cocostudio/CocoStudio.h"
#include "../cocos2d/cocos/ui/UILayout.h"
#include "../cocos2d/cocos/ui/UIImageView.h"
#include "../cocos2d/cocos/ui/UIScrollView.h"
#include "../cocos2d/cocos/ui/UIHelper.h"
#include "../cocos2d/cocos/2d/CCFastTMXTiledMap.h"
#include "../cocos2d/cocos/2d/CCFastTMXLayer.h"

#define TAG_TELEPORT_0_ENTRY 0
#define TAG_TELEPORT_0_EXIT 1
#define TAG_TELEPORT_1_ENTRY 2
#define TAG_TELEPORT_1_EXIT 3
#define TAG_TELEPORT_2_ENTRY 4
#define TAG_TELEPORT_2_EXIT 5

#define MAX_DROP_SPEED -10
#define MAX_COIN_COUNT 691


#define MAP_FOUND 0
#define MAP_NORMAL -1
#define MAP_BLUE_KEY -2
#define MAP_WEAPON -3
#define MAP_BOSS_GATE -4
#define MAP_SHOP -5
#define MAP_FINE -6

#define WHOSE_TALK_HERO 0
#define WHOSE_TALK_NPC 1
#define WHOSE_TALK_UNDEAD 2

#define ITS_BUILDING true
#define ITS_NOT_BUILDING false

#define CLEAR_CONDITION_TERMINATE 0
#define CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND 1
#define CLEAR_CONDITION_KILL_ZOMBIES 2
#define CLEAR_CONDITION_SURVIVE_FOR_10_MIN 3
#define CLEAR_CONDITION_SURVIVE_FOR_20_MIN 4
#define CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE 5


#define COMMAND_NOTHING -1
#define COMMAND_MOVE 0
#define COMMAND_STOP 1
#define COMMAND_ATTACK 2
#define COMMAND_GATHER 3
#define COMMAND_BUILD 4
#define COMMAND_BUILD_BETTER 5
#define COMMAND_ATTACK_DDANG 6
//#include "../cocos2d/cocos/ui/UILayout.h"
//#include "../cocos2d/cocos/ui/UIButton.h"
//#include "../cocos2d/cocos/ui/UIText.h"
//#include "../cocos2d/cocos/ui/UIImageView.h"
//#include "../cocos2d/cocos/ui/UITextField.h"
//#include "../cocos2d/cocos/ui/UIHelper.h"

//#include "../cocos2d/cocos/2d/CCFastTMXTiledMap.h"
//#include "../cocos2d/cocos/2d/CCFastTMXLayer.h"

#define WHICH_SIDE_HERO 0
#define WHICH_SIDE_ENEMY 1
#define WHICH_SIDE_MUTUAL 2
#define WHICH_SIDE_READY_HERO 3
using namespace cocos2d;
//using namespace cocostudio;
using namespace cocos2d::ui;
class EditorWorld : public cocos2d::Layer
{
protected:
    bool blackSheepWell = true; // test
    float stickTimeLeft=0;
    void updateIndicators();
    Vector<Sprite*> indicatorArray;
    Vector<Movable*> indicatedArray;
    std::vector<int> shopWeaponItem;
    float missileEffectCollapsedTime;
    Vector<Movable*> enemyMissileArray;
    Vector<EnemyBase*> flyingSwimingEnemyArray;
    Vector<Movable*> MovableArray;
    Vector<Movable*> straightMovingArray;
    Vector<Movable*> customMovingArray;
    Vector<Movable*> chasingMissileArray;
    Vector<Movable*> MovableMissileArray;
    Vector<Movable*> MovableCoinArray;
    Vector<EnemyBase*> destructableArray;
    Vector<EnemyBase*> trapArray;
    void createTrap(EnemyBase* enemy, cocos2d::Vec2 pos);
    Vector<Sprite*> teleportArray;
    Vector<Movable*> dropItemArray;
    Vector<EnemyBase*> enemiesToRemove;
    Vector<EnemyBase*> heroesToRemove;
    Movable* encounteredDropItem = nullptr;
    
    int extraCritical;
    int extraPower;
    int extraFruit;
    void sacrificeLater(float dt);
    void sacrificeDone(Ref* obj);
    
    void testSchedule(float dt);
    Sprite* testDragon;
    Sprite* testPet;
    Sprite* testPet2;
    
    int totalCoinCount;
    //    int getEpisodeIndex();
    //    void episodeActionNeeded();
    //    void episodeActionFinished();
    //    int episodeActionIndex;
    //    void showEpisode();
    //    EpisodeLayer* episodeLayer;
    //    void episodeDone();
    //    Movable* episodeNpc;
    bool testingFlag = false;
    Laser* laser=nullptr;
    std::vector<cocos2d::Rect> stageTileRectArray;
    Sprite* blackSquare;
    cocos2d::Size size;
    Vector<Movable*> coinArray;
    Vector<Movable*> starArray;
    
    Vector<EnemyBase*> npcArray;
    Vector<Sprite*> exitArray;
    
    void updateNPCQuestState(EnemyBase* npc);
    void addTalkBalloon(EnemyBase* npc, std::string imgName);
    void addTalkBalloon(experimental::TMXTiledMap* map, std::string imgName, cocos2d::Vec2 pos);
    EnemyBase* addNPC(cocos2d::Vec2 pos, std::string name);
    Vector<Movable*> heroMissileArray;
    bool isBattleStarted = false;
    void updateUnitMove(float dt);
    void move(float dt, Movable* obj, Movable* target, bool horiFirst);
    //    void gettingReadyForBattle(float dt);
    int battleReadyCountDown = 3;
    Vector<Movable*> helperArray;
    Vector<Movable*> inWaterArray;
    Vector<Movable*> bubbleArray;
    Vector<FireableBase*> fireableArray;
    Vector<Movable*> spiderArray;
    
    Vector<Movable*> coinToRemove;
    Sprite* nearBack;
    Sprite* farBack;
    Sprite* furtherBack;
    AwesomeDialogBox* _dialogBox;
    Movable* theDoor;
    Movable* theKey;
    Vector<Movable*>talkArray;
    Vector<Label*> talkLabelArray;
    float indestructableTime;
    int playerWeaponType;
    //    int playerMissileDemage;
    void destroyDestructable(Movable* drop);
    int roomNumber=0;
    int currentTalkIndex;
    int starCount;
    float coinDelay;
    float otherDelay;
    bool bulletWasted;
    cocos2d::Vec2 deadPoint;
    bool guidedMissile;
    Movable* findTarget(int distance);
    Sprite* activityIndicator;
    int criticalLevel;
    
    int furtherWidth;
    int furtherHeight;
    int farWidth;
    int farHeight;
    int nearWidth;
    int nearHeight;
    void solveCollision(Movable* p, cocos2d::Vec2 pos, cocos2d::Rect rect);
    void removeLaser();
    Vector<experimental::TMXTiledMap*> mapArray;
    Vector<experimental::TMXTiledMap*> revealedMapArray;
    std::vector<cocos2d::Rect> mapRectArray;
    //    DrawNode* dnMiniMap;
    //    cocos2d::Size miniMapSize = cocos2d::Size(200,160);
    //    cocos2d::Size currentMapSize = miniMapSize;
    //    int miniMapAlpha=255;
    //    bool isMiniMapFadeIn=false;
    //    void fadeMiniMap(float dt);
    bool isRoomEmpty(cocos2d::Rect rect);
    experimental::TMXTiledMap* findDirectionAndPlaceTheMap(experimental::TMXTiledMap* srcMap, experimental::TMXTiledMap* dstMap);
    
    void setEntireMap(int stage);
    void setBossMap(int stage);
    bool intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect);
    void addMapToMiniMap(experimental::TMXTiledMap* map);
    
    int suitcaseCount = 0;
    void receivingData(float dt);
public:
    Vector<EnemyBase*> unitsToCreateArray;
    Vector<EnemyBase*> heroArray;
    Vector<EnemyBase*> enemyArray;
    Vector<EnemyBase*> mutualArray;
    Vector<EnemyBase*> readyHeroArray;
    EnemyBase* createUnit(int index, int whichSide, bool isBuilding, cocos2d::Vec2 pos, std::string name, int scaleX=1, std::string charName = "workerAxeStand0.png");
    Vector<EnemyBase*> selectedArray;
    void updateMiniMapForMoving();
    void updateMiniMapForNonMoving();
    
    experimental::TMXTiledMap* theMap;
    Movable* createMissile(int missileType, int dmg, bool visible, float time, int angle, int speed, cocos2d::Vec2 pos, bool isFromEnemy, std::string weaponName = "");
    EnemyBase* getEncounteredNPC(cocos2d::Vec2 pos);
    EnemyBase* talkingNPC = nullptr;
    void movePlayer(int direction);
    float dashTimer = 0;
    int lastMoveDirection = -1;
    void onjewelryBoxOpen(Ref* sender);
    void showBigMiniMap(bool isBig);
    float getMin(float src, float dst);
    float getMax(float src, float dst);
    int secondTouchID=-1;
    int secondTouchBeganDistance = 0;
    float maxLayerScale = 1;
    float minLayerScale = 1;
    bool isTouchStarted;
    float playerFireCoolTime;
    float playerFireCoolTimeMax;
    float reloadingCoolTime;
    float reloadingCoolTimeSpan;
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object cocos2d::Vec2er)
    virtual bool init();
    Sprite* heroLight;
    cocos2d::Vec2 lastTouchPoint;
    cocos2d::Vec2 touchBeganPos;
    int touchCount = 0;
    int twoTouchEnabled = false;
    void showDamage(int damage, cocos2d::Vec2 pos);
    //    bool leftPressed;
    //    bool rightPressed;
    //    bool firePressed;
    //    bool jumpPressed;
    int collectedCoinCount;
    bool isGameOver = false;
    bool isInEvent = false;
    bool keyCollected;
    bool isWay(int index);
    bool isDecoBlock(int index);
    bool isHighWay(int index);
    bool isOneWay(int index);
    bool isFregile(int index);
    int mastery=0;
    int alphabetCount=0;
    void setNamingRoom(experimental::TMXTiledMap* map);
    void setLobby(experimental::TMXTiledMap* map);
    void setEmptyMap(experimental::TMXTiledMap* map);
    int itemCount=0;
    PointArray* enemyPointArray;
    Vector<EnemyBase*> enemiesToLoad;
    void loadEnemies();
    void addAppearEffect(Sprite* parent);
    FireableBase* addEnemyToLoadStack(experimental::TMXTiledMap* map, int levelScore, cocos2d::Vec2 pos, int missile, int enemyModel, bool addGround);
    void addEnemiesToMap(experimental::TMXTiledMap* map, int levelScore, bool blueKey);
    int currentLevelScore;
    long currentScore=0;
    void tournamentSchedule(float dt);
    int tournamentWaveIndex=0;
    EnemyBase* theBoss;
    bool isSnowing = true;
    void removeMeFromChasing(Ref* ref);
    void bossExplode(float dt);
    void makeGate(float dt);
    //    cocos2d::Vec2 playerSpawnPoint;
    void angelReviveLater(float dt);
    void bossScheduleBazooka(float dt);
    void bossScheduleGiantPenguin(float dt);
    void bossScheduleGiantWood(float dt);
    void bossScheduleGiantFish(float dt);
    void cloudSchedule(float dt);
    Vector<Sprite*> cloudArray;
    float bossScheduleTime;
    int bossStep;
    int bossInnerStep;
    void bossClear();
    float lastClearY;
    int enemyKilledCount;
    int enemyMaxCount;
    void rateResult(Node* node);
    void revive();
    float checkBottom(Movable* p);
    bool isSetStageDone;
    int stageIndex;
    void addWeapon(int weapon);
    int getWeaponAtSlot(int slot);
    void setWeaponAtSlot(int slot, int weapon);
    void changeWeapon(int slot);
    int getTotalBulletCountAtSlot(int slot);
    void setTotalBulletCountAtSlot(int slot, int count);
    int getLoadedBulletCountAtSlot(int slot);
    void setLoadedBulletCountAtSlot(int slot, int count);
    int currentSlot;
    cocos2d::Vec2 positionToTeleport;
    void teleportLater(float dt);
    void teleportLaterForThemeThree(float dt);
    bool isTeleporting;
    bool playerIgnoreGravity;
    Sprite* getTeleport(int teleport);
    bool isMapMoveLeft = false;
    bool isMapMoveRight = false;
    bool isMapMoveUp = false;
    bool isMapMoveDown = false;
    cocos2d::Size mapSize;
    // there's no 'id' in cpp, so we recommend to return the class instance cocos2d::Vec2er
    static cocos2d::Scene* scene(int stage, bool boss);
    Node* spriteBatch;
    Node* spriteBatchEffect;
    void setOccupy(cocos2d::Vec2 pos, int width, int height, bool occupy);
    void setOccupy(cocos2d::Vec2 pos, int width, int height, bool occupy, EnemyBase* building);
    NodeGrid* nodeGrid;
    //    SpriteBatchNode* spriteBatchForHero;
    //    SpriteBatchNode* coinSpriteBatch;
    SpriteBatchNode* backgroundSpriteBatch;
    // a selector callback
    void menuCloseCallback(Ref* pSender);
    
    void removeUsedAssets();
    EventListenerTouchAllAtOnce* listener;
    EventListenerMouse* mouseListener;
    
    
    EditorWorld();
    //    ~EditorWorld();
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(EditorWorld);
    void unscheduleAll();
    int killCountForRecord;
    int collectedCoinPart;
    Label* lblCoinGot = nullptr;
    int goldChestCount = 0;
    bool readyToMove = false;
    
    void attackEnemy(EnemyBase* drop, int damage);
    void attackHero(EnemyBase* drop, int demage);
    void setViewPointCenter(cocos2d::Vec2 position, bool forceMove = false);
    void registerScriptAccelerateHandler(int handler);
    double getAngle(cocos2d::Vec2 pos1, cocos2d::Vec2 pos2);
    void setPlayerPosition(cocos2d::Vec2 position);
    void bubbleUpdate(float dt);
    void gravityUpdate(float dt);
    void gravityUpdateForCoins(float dt);
    void gravityUpdateForStraight(float dt);
    void gravityUpdateForCustomMoving(float dt);
    bool hitPlayer(int damage);
    void useBomb();
    void usePotion();
    int demagingUnit;
    void doTheBombToMissiles(float dt);
    void doTheBombToEnemies(float dt);
    Vector<Movable*> bombTargetMissileArray;
    int bombTargetEnemyIndex;
    Vector<EnemyBase*> bombTargetEnemyArray;
    //    void gravityUpdateForOthers(float dt);
    //    void gravityUpdateForOnlyCheckCollision(float dt);
    void gravityUpdateForFlyingOrSwimingEnemies(float dt);
    void gravityUpdateForWaterEnemies(float dt);
    void chasingMissileUpdate(float dt);
    void missileEffectUpdate(float dt);
    void coinUpdate(float dt);
    void npcUpdate(float dt);
    void enemyUpdate(float dt);
    void destructableUpdate();
    void missileUpdate(float dt);
    void talkUpdate(float dt);
    void updateFireStick(float dt);
    void helperAndTrapUpdate(float dt);
    void createRewards(EnemyBase* enemy);
    bool keyExist(ValueMap map, const char* key);
    void takeAction();
    float powerTestTime;
    void powerTestSchedule(float dt);
    float powerPerSec;
    float totalPower;
    void coinMagnet();
    bool isPaused;
    bool isVideoRewardAttack;
    bool everySecond;
    void Dispose();
    void playReloadingSoundLater(float dt);
    //    int mapHight;
    //    int mapRowCount;
    //    int mapColumnCount;
    //    TMXLayer* stageLayer;
    //    TMXLayer* foreLayer;
    void setCurrentTileMap(cocos2d::Vec2 pos);
    experimental::TMXTiledMap* getTileMap(cocos2d::Vec2 pos);
    void setLayerTag(experimental::TMXTiledMap* map);
    
    //    cocos2d::TMXTiledMap *currentTileMap;
    //    Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer, Movable* p);
    //    Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
    int getTileAtPosition(cocos2d::Vec2 position, int tag, experimental::TMXTiledMap* map);
    /*
     
     cocos2d::TMXTiledMap *tileMap;
     cocos2d::TMXLayer *stageLayer;
     cocos2d::TMXLayer *unitLayer;
     cocos2d::TMXLayer *foreLayer;
     cocos2d::TMXLayer *backLayer;
     Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer, Movable* p);
     Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     TileInfo* getTileAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     
     #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
     cocos2d::TMXTiledMap *tileMap;
     cocos2d::TMXLayer *stageLayer;
     cocos2d::TMXLayer *unitLayer;
     cocos2d::TMXLayer *foreLayer;
     cocos2d::TMXLayer *backLayer;
     Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer, Movable* p);
     Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     TileInfo* getTileAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     #else
     cocos2d::TMXTiledMap *tileMap;
     cocos2d::TMXLayer *stageLayer;
     cocos2d::TMXLayer *unitLayer;
     cocos2d::TMXLayer *foreLayer;
     cocos2d::TMXLayer *backLayer;
     Vector<TileInfo*>getSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer, Movable* p);
     Vector<TileInfo*>getAllSurroundingTilesAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     TileInfo* getTileAtPosition(cocos2d::Vec2 position, cocos2d::TMXLayer *layer);
     #endif*/
    //bool isPlayerSet=false;
    cocos2d::Vec2 getCoordinateFromPosition(cocos2d::Vec2 position, experimental::TMXTiledMap* map);
    cocos2d::Vec2 getCoordinateFromPosition(cocos2d::Vec2 position);
    cocos2d::Rect tileRectFromTileCoords(cocos2d::Vec2 tileCoords, experimental::TMXTiledMap* map);
    void getDown();
    void checkForDoor();
    void addDirtToTile(experimental::TMXTiledMap* map, cocos2d::Vec2 pos);
    virtual void TouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
    virtual void TouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
    virtual void TouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
    
    void checkForAndResolveCollisions(Movable* p);
    void checkForAndResolveCollisionsForOthers(Movable* p);
    void checkForAndResolveCollisionsForWater(Movable* p);
    void checkForAndResolveCollisionsForBouncing(Movable* p);
    bool checkForAndResolveCollisionsForMissile(Movable* p);
    bool checkMovableMissileCollision(Movable* p);
    void changePlayerState(int state);
    void saveCoinIfStarCollected();
    void setStage(experimental::TMXTiledMap* tileMap);
    void fixStageLayerTiles(experimental::TMXTiledMap* map);
    bool compareFourTiles(int t,int l, int r, int b, int x, int y, experimental::TMXLayer* stageLayer);
    void fixStageLayerFourTiles(experimental::TMXTiledMap* map);
    void stopWoongSound();
    bool compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, experimental::TMXLayer* stageLayer);
    void getOffVehicle();
    bool isCameraInCustomMoving = false;
    void addRide(int unit, cocos2d::Vec2 pos);
    void makeSomeBubbles(float dt);
    void fire();
    void enemyFireLoop(float dt);
    //int collectedKeyCount=0;
    void openWeapon(Ref* sender);
    Sprite* createNumberSprite(int number);
    Sprite* getLightSpin(float persistTime);
    Sprite* getShining(float delayTime);
    //    void fireLoop(float dt);
    void missileMoveDone(Ref* obj);
    void spriteMoveDone(Ref* obj);
    void coinWaitDone(Ref* obj);
    void enemyBaseMoveDone(Ref* obj);
    void fireBulletMoveDone(Ref* drop);
    void nodeMoveDone(Ref* obj);
    void runEffect(int effect, cocos2d::Vec2 point);
    void runEffect(int effect, cocos2d::Vec2 point, float angle);
    void addGlowEffect(Sprite* sprite,const Color3B& colour, const cocos2d::Size& size);
    void gameClearAnimationDone(Ref* obj);
    void gameOver();
    void shakeScreen(int count);
    void shakeScreenOnce();
    cocos2d::Vec2 extraCameraPos=cocos2d::Vec2::ZERO;
    void shakeScreenFirst();
    void shakeScreenSecond();
    void shakeScreenEnd();
    
    
    void jumpBySpring(float dt);
    float blinkingTime;
    void blinkingPlayer(float dt);
    void immortalForAWhile(float dt);
    cocos2d::Rect RectInset(cocos2d::Rect rect, float x, float y);
    void removeEnemy(EnemyBase* spt);
    void removeHero(EnemyBase* spt);
    void showSuccessLayer(float dt);
    void unfreezePlayer(float dt);
    void stepBackForHit(float dt);
    void pauseLayer();
    void resumeLayer();
    void refreshPlayerInfo(int costum, int weapon);
    void smokeForVehicle();
    cocos2d::Rect intersection(cocos2d::Rect source, cocos2d::Rect rect);
    //    virtual void draw();
    DrawNode* draw;
    DrawNode* drawMiniMapFrame;
    DrawNode* drawMiniMapForMoving;
    DrawNode* drawMiniMapForNonMoving;
    EnemyBase* hero;
    void oneSecUpdate(float dt);
    float oneSecTiemr = 0;
    void halfSecUpdate(float dt);
    float halfSecTimer = 0;
    float miniMapFrameWidth = 300;
    float miniMapFrameHeight = 200;
    float miniMapWidth;
    float miniMapHeight;
    float miniMapScale = 1;
    float miniMapBit;
    cocos2d::Vec2 miniMapStartPos = cocos2d::Vec2(10, 10);
    cocos2d::Vec2 miniMapDrawStartPos;
    void doClick(cocos2d::Vec2 pos);
    void selectByDrag(cocos2d::Rect rect);
    int selectedCommand = -1;
    void selectCommand(int command);
    void onMoveClick();
    void onStopClick();
    void onAttackClick();
    void onGatherClick();
    void onBuildingClick();
    void onBuildingBetterClick();
    void onSelectOnScreenClick();
    void onSelectOnMapClick();
    Movable* targetHand;
    float doubleClickTimer = 0;
    void showTargetHand(cocos2d::Vec2 pos, bool isAttack);
    void moveTo(EnemyBase* unit, cocos2d::Vec2 pos);
    void moveTo(Vector<EnemyBase*> troop, cocos2d::Vec2 pos);
    void moveTo(Vector<EnemyBase*> troop, EnemyBase* target);
    void gatherTo(Vector<EnemyBase*> troop, EnemyBase* target);
    void stop(Vector<EnemyBase*> troop);
    void moveAndAttackTo(Vector<EnemyBase*> troop, cocos2d::Vec2 pos);
    void moveAndAttackTo(EnemyBase* unit, cocos2d::Vec2 pos);
    void forceAttack(Vector<EnemyBase*> troop, EnemyBase* target);
    void dialogFinished(Ref* obj);
    virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event);
    cocos2d::Vec2 waterBoyPos;
    bool isReloading;
    void releaseCustomMovingCamera();
    void entranceSchedule(float dt);
    void reload();
    int loadedBulletCount=-1;
    int loadedBulletCountMax;
    int totalBulletCount=-1;
    int totalBulletCountMax;
    int getSlotForWeapon(int weapon);
    void recharge(int slot);
    bool isFired;
    int getMaxLoadedBulletCount(int weapon);
    int getMaxTotalBulletCount(int weapon);
    cocos2d::Vec2 center;
    Sprite* cursor;
    Sprite* getLight();
    bool isWaterBoy;
    bool isWaterBoyFlip;
    float waterBoyAngle;
    
    void showStageClearLayer(bool threeStars);
    void onUpdateResultComplete();
    void onUpdateResultErrorComplete(char const *status, char const *error);
    void onUpdateUserComplete();
    void onUpdateUserErrorComplete(char const *status, char const *error);
    bool containsTouchLocation(Touch* touch);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    void resetTouchStart(float dt);
    void addListener();
    void warp();
    void goToNextScene();
    int indexToWarp = 0;
    bool isBossMap;
    void showWarpAnimation();
    void endGame(bool win = true);
    Node* resultLayer = nullptr;
    void autoTargeting();
    void updateResult(float dt);
    int kill=0;
    int destroy=0;
    int coin=0;
    int room=0;
    int boss=0;
    int time=0;
    bool resultDone = false;
    bool blockKeys = false;
    int totalScore=0;
    int currentQuestID = -1;
    EnemyBase* encounteredNPC = nullptr;
    EnemyBase* justEncounteredNPC = nullptr;
    void showNPCEvent(int index);
    std::string talkingNpcName;
    
    cocos2d::Vec2 _cameraPoint;
    void warpOut();
    //DrawNode* dnDamageBoxes;
    //DrawNode* dnCollisionBoxes;
    
    Node* lastTalkedNpc = nullptr;
    bool isAutoTargetingOn = true;
    void testFunction();
    void checkEnding();
    void changeSprite(Ref* ref);
    
    bool isOnFirstFloor = false;
    
    void removeCharacter(EnemyBase* unit);
    void showTalkText(std::string text, int whoseTalk, std::string npcName = "");
    ImageView* imgTalkBox = nullptr;
    Sprite* sptPointer = nullptr;
    Label* lblTalk = nullptr;
    Label* lblTalkShadow = nullptr;
    std::string talkText;
    void talkBoxUpdate(float dt);
    int talkIndex = 0;
    void endEvent();
    int clearConditionIndex = 0;
    void setClearCondition(int stage);
    bool checkClearGame();
    Vector<PPLabel*> lblConditionArray;
    float gameTimer = 0;
    float lastTimeCheck = 0;
    int lastTenTick = 0;
    int lastTick = 0;
    bool isGameClear = false;
    int getHeroLevel(int slot);
    void setHeroLevel(int slot, int level);
    int getHeroExp(int slot);
    int getMaxExp(int level);
    void addHeroExp(int slot, int exp);
    void setHeroExp(int slot, int exp);
    int getHeroMaxLevel(int slot);
    std::string getHeroName(int slot);
    int getHeroMaxHP(int slot);
    int getHeroAP(int slot);
    int getHeroDP(int slot);
    int getHeroGP(int slot);
    int getHeroSP(int slot);
    void updatePlayerStat();
    int getUnitMaxHP(int index);
    int getUnitAP(int index);
    int getUnitDP(int index);
    int getUnitSP(int index);
    float getUnitCreateTime(int index);
    float getUnitMonitoringDistance(int index);
    float getUnitAttackRange(int index);
    int getUnitAttackType(int index);
    int getUnitAttackTargetType(int index);
    float getUnitAttackCoolTime(int index);
    float getUnitAttackHappenTime(int index);
    bool isThisSpotAvailable(Movable* me);
    std::string getSpriteNameForUnit(int index);
    std::string getUnitName(int index);
    Sprite* getSpriteForIcon(int index);
    Node* buildingTemplete = nullptr;
    bool isBuildingTempleteMoved = false;
    void checkBuildingTemplete();
    void checkGameSchedule();
    int lastGameScheduleIndex = -1;
    void makeZombiesAttack();
    bool tryBuilding(int index);
    void tryCreateUnit(int index);
    void createBuildingTemplete(int index, int width, int height, std::string spriteName);
    EnemyBase* buildTheBuilding(int index, int x, int y, int width, int height, std::string spriteName);
    cocos2d::Size buildingTempleteSize;
    cocos2d::Vec2 buildingTempleteCoordinate;
    bool isBuildingReadyToBuild = false;
    std::string getHeroWeapon(int slot);
    void setHeroWeapon(int slot, std::string weapon);
    std::string getHeroHelmet(int slot);
    void setHeroHelmet(int slot, std::string helmet);
    std::string getHeroShield(int slot);
    void setHeroShield(int slot, std::string shield);
    std::string getHeroShoes(int slot);
    void setHeroShoes(int slot, std::string shoes);
    void loadUnitSheet();
    ValueMap unitStatTable;
    ValueMap weaponStatTable;
    ValueMap itemStatTable;
    ValueMap shieldStatTable;
    ValueMap shoesStatTable;
    ValueMap helmetStatTable;
    ValueMap npcTable;
    ValueMap shopTable;
    ValueMap evolutionTable;
    Sprite* sptDungeonEntrance = nullptr;
    Vector<Sprite*> blockerArray;
    Vector<Sprite*> triggerArray;
    Vector<Sprite*> stoneArray;
    Sprite* encounteredTrigger = nullptr;
    
    int getUnitStat(std::string unitName, std::string stat);
    std::string getUnitDropItem(std::string unitName, int dropIndex);
    int getWeaponStat(std::string name, std::string stat);
    std::string getItemStat(std::string name, std::string stat);
    std::string getItemStat(int itemType, std::string name, std::string stat);
    std::string getEvolutionStat(std::string name, std::string stat);
    std::string getItemSpriteName(int itemType, std::string name);
    int getItemPrice(int itemType, std::string name);
    int getItemType(std::string name);
    int getShieldStat(std::string name, std::string stat);
    int getShoesStat(std::string name, std::string stat);
    int getHelmetStat(std::string name, std::string stat);
    std::string getHelmet(int heroSlot);
    std::string getWeapon(int heroSlot);
    std::string getShield(int heroSlot);
    std::string getShoes(int heroSlot);
    experimental::TMXLayer* stageCover = nullptr;
    void setPlayer(cocos2d::Vec2 pos);
    void exitMap(Node* exit);
    void enterDungeon();
    void exitDungeon();
    void setStageCoverOpacity(int opacity);
    bool isStageCoverTransparent = false;
    Node* lastExit = nullptr;
    std::string strStage = "";
    void moveInTheMap();
    void moveInTheMapDone();
    int directionToMoveInTheMap = 0;
    EnemyBase* unitInTheMap;
    void walkInTheMapDone();
    void shieldUp(bool up);
    bool pickUpItem();
    void showNPCDisposableTalk(std::string talk);
    void onQuestComplete();
    bool isQuestConditionMet();
    void claimQuestReward();
    void removeItemFromField(Ref* ref);
    void checkAndShowQuestOnMap(cocos2d::experimental::TMXTiledMap* map);
    Vector<Label*> labelPool;
    int labelPoolCount = 10;
    int labelPoolIndex = 0;
    void backToLabelPool(Ref* ref);
    Label* showLabelFromPool(Node* parent, cocos2d::Vec2 pos, std::string text, int moveHeight, float delay = 0);
    //    Vector<Sprite*> spritePool;
    //    int spritePoolCount = 10;
    //    int spritePoolIndex = 0;
    //    void backToSpritePool(Ref* ref);
    //    Sprite* showSpriteFromPool(Node* parent, cocos2d::Vec2 pos);
    //    Vector<Sprite*> selectedCircleArray;
    void setQuest();
    void checkQuestDone();
    void resetPathState();
    cocos2d::Vec2 getPositionFromTileCoordinate(int x, int y);
    experimental::TMXLayer* decoLayer = nullptr;
    void addDecoBlock(cocos2d::Vec2 coordinate, cocos2d::Vec2 position, std::string spriteName);
    void stepBackDone();
    void createPortal(cocos2d::Vec2 pos);
    
    Node* suckingObject = nullptr;
    void sucking(float dt);
    void goThroughPortal();
    void purgatoryClear();
    EnemyBase* getNPC(std::string name);
    void headingUp(Ref* ref);
    void headingDown(Ref* ref);
    void headingLeft(Ref* ref);
    void headingRight(Ref* ref);
    void stay(Ref* ref);
    void startNPCEvent();
    void eventUpdate(float dt);
    void eventDone();
    void dragonUpdate(float dt);
    int dragonAttackCounter = 0; // test
    EnemyBase* dragonTail;
    EnemyBase* dragonBody;
    EnemyBase* dragonWing;
    EnemyBase* dragonHead;
    void turnOnEnemyDamaging(Ref* obj);
    void purgatoryUpdate(float dt);
    Node* thePortal = nullptr;
    Node* greatBall = nullptr;
    void creatingStarToGreatBall(float dt);
    void changeToSuperAdin();
    void addSuperEffectToAdin();
    void startCameraFollowNPCForEvent(Ref* ref);
    void endCameraFollowNPCForEvent();
    void cameraUpdateForEvent(float dt);
    Node* cameraTargetForEvent;
    void addSprite(std::string sptName, cocos2d::Vec2 pos);
    void immortalForSec(int sec);
    void deselectAll();
    void deselect(Movable* unit);
    void updateMenu();
    void updateFoodMaxState();
    void selectUnit(EnemyBase* unit);
    void splashDamage(cocos2d::Vec2 pos, int radius, int damage, bool isFromEnemy, Movable* attacker);
    void removeDeadUnit(EnemyBase* unit);
    EnemyBase* getNearestCastle(cocos2d::Vec2 pos);
    EnemyBase* getNearestLumberTank(cocos2d::Vec2 pos);
    EnemyBase* getNearestTree(cocos2d::Vec2 pos);
    int mapWidth = 0;
    int mapHeight = 0;
    int gold = 0;
    int lumber = 0;
    int foodInUse = 0;
    int foodMax = 0;
    void addGold(int amount);
    void addLumber(int amount);
    void updateFoodInUse();
    void addFoodMax(int amount);
    int getGoldPriceForUnit(int index);
    int getLumberPriceForUnit(int index);
    int getFoodUse(int index);
    int getFoodGive(int index);
    bool isTouchBeganFromMiniMap = false;
    bool isMapMovingByMiniMap = false;
    void moveScreen(cocos2d::Vec2 pos);
    cocos2d::Rect miniMapViewRect;
    cocos2d::Rect viewRect;
    void revengeAttack(Movable* attackee, Movable* attacker);
    
    int getAttackPriority(int index);
    bool canAttack(Movable* attacker, Movable* target);
    bool isSpotAvailable(bool isEnemy, cocos2d::Vec2 pos);
    
    int totalEarnedGold = 0;
    int totalEarnedLumber = 0;
    int totalUsedGold = 0;
    int totalUsedLumber = 0;
    int totalProducedUnit = 0;
    int totalKillUnit = 0;
    void onUndeadMoveDone();
    
    bool inInScreen(cocos2d::Vec2 pos);
    void addDecoToBuilding(Movable* unit);
    void attackNearHero(EnemyBase* enemy);
    
    int getPlacedUnit(cocos2d::Vec2 pos);
    int mapSizeWidth = 70;
    int mapSizeHeight = 70;
    int placedArray[70][70];
    EnemyBase* brushTile(int brush, cocos2d::Vec2 coordinate);
    void eraseTile(cocos2d::Vec2 coordinate);
    void createTree(cocos2d::Vec2 pos);
    std::string getMapData();
    void loadMapData();
    void setMapData(std::string data);
    bool isBrushBuilding(int brush);
    int getUnitIndexForBrush(int brush);
    bool isSelectedUnitSelected = false;
    int selectedUnitsBrush = 0;
    Vec2 selectedArrayPlacedPosition;
    Vec2 lastBrushedCoordinate;
};

#endif // __editor_scene

