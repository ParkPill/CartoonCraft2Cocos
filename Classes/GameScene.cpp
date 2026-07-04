#include "GameScene.h"
// #include "SimpleAudioEngine.h"
#include "BuggyServerManager.h"
#include "Fog.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "HudLayer.h"
#include "LegendDaryButton.h"

// #include "NativeInterface.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"
#include "MultiplayManager.h"
#include "json/document.h"
#include <cstdio>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "Win32MouseCursor.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
using namespace cocos2d;
// using namespace CocosDenshion;

// Client* client = new Client("ws://colyseus-examples.herokuapp.com");
// Client* client = new Client("ws://localhost:2567");
// Room<State>* theRoom;
// Map<std::string, Sprite*> players;

Scene *GameScene::scene(int stage, int mode, bool multiplay) {
  log("scene created");
  //    stage = 1; // test

  //    GameManager::getInstance()->stopSoundEffect(SOUND_THUNDER);
  //    GameManager::getInstance()->leftPressed = false;
  //    GameManager::getInstance()->rightPressed = false;
  //    GameManager::getInstance()->firePressed = false;
  //    GameManager::getInstance()->jumpPressed = false;
  //    GameManager::getInstance()->cPressed = false;
  //    GameManager::getInstance()->downPressed = false;
  //    GameManager::getInstance()->upPressed = false;
  cocos2d::Size size = Director::getInstance()->getWinSize();
  Scene *scene = Scene::create();
  int theme = GameManager::getInstance()->theme;
  GameManager::getInstance()->currentStageIndex = stage;

  //    GameManager::getInstance()->showInterstitialAds();

  //    LayerColor* grayLayer = LayerColor::create(Color4B(246,246,246,255));
  //    LayerColor* grayLayer = LayerColor::create(Color4B(156, 156, 156, 255));
  //    LayerColor* grayLayer = LayerColor::create(Color4B(150, 163, 83, 255));
  //    // green
  LayerColor *grayLayer;
  if (GM->nextScene == STAGE_LOBBY || GM->nextScene == STAGE_RAID ||
      GM->nextScene == STAGE_SINGLEPLAY) {
    grayLayer = LayerColor::create(Color4B(206, 178, 138, 255)); // sand
  } else {
    grayLayer = LayerColor::create(Color4B(0, 0, 0, 255)); // sand
  }

  grayLayer->setName("grayLayer");
  //    LayerColor* grayLayer = LayerColor::create(Color4B(206,206,206,255));
  scene->addChild(grayLayer);
  GameScene *layer = GameScene::create();
  layer->isMultiplay = multiplay;
  if (multiplay) {
    MultiplayManager::getInstance()->msgList.clear();
    MultiplayManager::getInstance()->rmsgList.clear();
  }
  if (mode == GAME_MODE_PVP6 || mode == GAME_MODE_PVP12 || layer->isMultitest) {
    layer->blackSheepWell = true;
  }
  // test now
  layer->blackSheepWell = true;
  layer->gameMode = mode;
  scene->addChild(layer);
  layer->stageIndex = stage;
  layer->difficultyMode = mode; // == GAME_MODE_HARD;
  if (mode > GAME_MODE_HARD) {
    layer->difficultyMode = DIFFICULTY_MODE_NORMAL;
  }
  {
    HudLayer *hudLayer = HudLayer::create();
    scene->addChild(hudLayer, 10);
    GameManager::getInstance()->setHudLayer(hudLayer);
    int tag = rand() % 50;
    layer->setTag(tag);
    //        CCLOG("** New Tag: %d, theme: %d, stage: %d", tag,
    //        GameManager::getInstance()->theme, stage);
    hudLayer->setTag(tag);
    if (stage == 0 || stage == 12) {
      if (GM->nextScene == STAGE_INTRO) {
        hudLayer->addListener();
      }
    }
    Label *lbl = LM->getLocalizedLabel();
    if (layer->difficultyMode == DIFFICULTY_MODE_EASY) {
      lbl->setString("EASY");
    } else if (layer->difficultyMode == DIFFICULTY_MODE_NORMAL) {
      lbl->setString("NORMAL");
    } else if (layer->difficultyMode == DIFFICULTY_MODE_HARD) {
      lbl->setString("HARD");
    } else if (layer->difficultyMode == DIFFICULTY_MODE_HELL) {
      lbl->setString("HELL");
    }
    hudLayer->addChild(lbl);
    lbl->setAnchorPoint(Vec2(0, 1));
    lbl->setPosition(0, size.height);
  }

  //    layer->isBossMap = boss;
  if (GM->nextScene == STAGE_FIELD || GM->nextScene == STAGE_LOBBY ||
      GM->nextScene == STAGE_SINGLEPLAY || GM->nextScene == STAGE_RAID) {
    layer->addListener();
  }

  if (GM->nextScene == STAGE_CREDIT) { // test
    layer->setEntireMap(stage);
    HUD->showCredit();
    return scene;
  } else {
  }
  //    if (boss || true) {//test
  //    if(boss){
  //        layer->setBossMap(stage);
  //    }else{
  layer->setEntireMap(stage);
  //    }
  if (GM->nextScene == STAGE_FIELD) {
    layer->setClearCondition(stage);
  }

  if (GM->nextScene == STAGE_RAID) {
    HUD->setRaid();
  }
  return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {
  size = Director::getInstance()->getWinSize();
  //////////////////////////////
  // 1. super init first
  theBoss = NULL;

  // test
  //    UserDefault::getInstance()->setBoolForKey(strmake(KEY_JEWEL_COLLECTED_FORMAT,
  //    3), true); GameManager::getInstance()->isGameClear = true;

  GameManager::getInstance()->isInMiddleOfGame = true;
  GameManager::getInstance()->setCurrentStageLayer(this);
  if (!Layer::init()) {
    return false;
  }
  loadUnitSheet();
  isReloading = false;
  center = cocos2d::Vec2(size.width / 2, size.height / 2);
  killCountForRecord = 0;
  isSetStageDone = false;
  everySecond = false;
  isTouchStarted = false;
  lastClearY = 0;
  isPaused = false;
  isTeleporting = false;
  coinDelay = 0;
  otherDelay = 0;
  missileEffectCollapsedTime = 0;
  playerFireCoolTime = 0;
  //    this->setKeypadEnabled(true);
  //    this->setTouchEnabled(true);
  playerIgnoreGravity = false;
  bulletWasted = false;

  spriteBatchBuilding = SpriteBatchNode::create("CartoonCraftNV.png");
  this->addChild(spriteBatchBuilding, 10);
  //    spriteBatch = SpriteBatchNode::create("cartoonCraft.png");
  //    this->addChild(spriteBatch, 10);
  //    batch = SpriteBatchNode::create("cartoonCraftEffect.png");
  //    this->addChild(batch, 10);
  //    Node* node = Node::create();
  //    this->addChild(node, 10);
  //    spriteBatch = node;
  //    spriteBatchBuilding = node;
  //    batch = node;
  //    batch = SpriteBatchNode::create("CartoonCraftNV.png"); // test
  batch = Node::create();
  this->addChild(batch, 10);
  //    batch->setVisible(false); // test

  std::string stage = UD->getStringForKey(KEY_LAST_SAVE_POINT, "stage0");

  enemyPointArray = PointArray::create(100);
  enemyPointArray->retain();

  supportPointArray = PointArray::create(100);
  supportPointArray->retain();

  heroPointArray = PointArray::create(100);
  heroPointArray->retain();

  labelPoolCount = 10;
  labelPoolIndex = 0;
  for (int i = 0; i < labelPoolCount; i++) {
    Label *lbl = Label::createWithSystemFont("YES!", "ari", 30);
    lbl->setScale(0.4f);
    lbl->retain();
    lbl->setTextColor(Color4B(78, 78, 78, 255));
    lbl->enableOutline(Color4B::WHITE, 4);
    labelPool.pushBack(lbl);
  }

  battleReadyCountDown = 2;

  draw = DrawNode::create();
  //    draw->drawSolidRect(Vec2(8*TILE_SIZE, 8*TILE_SIZE), Vec2(9*TILE_SIZE,
  //    9*TILE_SIZE), Color4F::GREEN);
  this->addChild(draw, 125);

  targetHand = EnemyBase::createEnemy(UNIT_WORKER, 0, 0, "workerAxeStand0.png");
  batch->addChild(targetHand, 110);
  targetHand->setOpacity(0);
  targetHand->isEnemy = false;
  targetHand->untouchable = true;

  layerScale = defaultLayerScale;
  if (stageIndex == 0) {
  }
  GM->playSoundEffect(SOUND_BGM_MAYDAY);
  this->setScale(layerScale);
  this->setAnchorPoint(Vec2::ZERO);

  gold = 21000;   // test now
  lumber = 21000; // test now
  //    if(true){// test
  //        onConnectToServer();
  //    }
  // init done
  return true;
}
// static void problemLoading(const char* filename)
//{
//     printf("Error while loading: %s\n", filename);
//     printf("Depending on how you compiled you might have to add 'Resources/'
//     in front of filenames in HelloWorldScene.cpp\n");
// }
// void GameScene::onConnectToServer()
//{
//     log("Colyseus: CONNECTED TO SERVER!");
//     client->joinOrCreate<State>("state_handler", {}, [=](std::string err,
//     Room<State>* _room) {
//         if (err != "") {
//             std::cout << "JOIN ERROR! " << err << std::endl;
//             return;
//         }
//
//         theRoom = _room;
//
//         theRoom->onMessage = CC_CALLBACK_1(GameScene::onRoomMessage, this);
//         theRoom->onStateChange = CC_CALLBACK_1(GameScene::onRoomStateChange,
//         this);
//
//         theRoom->onError = [this](std::string message) -> void {
//             std::cout << "ROOM ERROR => " << message.c_str() << std::endl;
//         };
//
//         theRoom->onLeave = [this]() -> void {
//             std::cout << "LEFT ROOM" << std::endl;
//         };
//
//         theRoom->getState()->players->onAdd = [this](Player* player, string
//         sessionId) -> void {
//             // add player sprite
//             auto sprite = Sprite::create("shadow.png");
//             sprite->setPosition(player->x, player->y);
//             players.insert(sessionId, sprite);
//             this->addChild(sprite, 0);
//
//             player->onChange = [this, sprite,
//             player](std::vector<colyseus::schema::DataChange> changes) ->
//             void {
//                 for(int i=0; i < changes.size(); i++)   {
//                     if (changes[i].field == "x") {
//                         sprite->setPositionX(player->x);
//
//                     } else if (changes[i].field == "y") {
//                         sprite->setPositionY(player->y);
//                     }
//                 }
//             };
//         };
//
//         theRoom->getState()->players->onRemove = [this](Player* player,
//         string sessionId) -> void {
//             std::cout << "onRemove called!" << std::endl;
//             auto sprite = players.at(sessionId);
//             this->removeChild(sprite);
//             players.erase(sessionId);
//             std::cout << "onRemove complete!" << std::endl;
//         };
//
//         std::cout << "Done!" << std::endl;
//
//     });
// }
//
//
// void GameScene::onRoomMessage(msgpack::object message){
//     std::cout << "--------------------------------------" << std::endl;
//     std::cout << "GameScene::onRoomMessage" << std::endl;
//     std::cout << message << std::endl;
// }
//
// void GameScene::onRoomStateChange(State* state)
//{
//     std::cout << "--------------------------------------" << std::endl;
//     std::cout << "GameScene::onRoomStateChange" << std::endl;
//
//     // send command to move x
//     auto data = std::map<std::string, float>();
//     data.insert(std::make_pair("x", 0.1));
//     theRoom->send(data);
// }

void GameScene::loadUnitSheet() {}
int GameScene::getItemType(std::string name) {
  if (weaponStatTable.find(name) != weaponStatTable.end()) {
    return ITEM_TYPE_WEAPON;
  } else if (shieldStatTable.find(name) != shieldStatTable.end()) {
    return ITEM_TYPE_SHIELD;
  } else if (helmetStatTable.find(name) != helmetStatTable.end()) {
    return ITEM_TYPE_HELMET;
  } else if (shoesStatTable.find(name) != shoesStatTable.end()) {
    return ITEM_TYPE_SHOES;
  } else if (itemStatTable.find(name) != shoesStatTable.end()) {
    return ITEM_TYPE_ETC;
  } else {
    return -1;
  }
}

void GameScene::move(float dt, Movable *obj, Movable *target, bool horiFirst) {
  if (target == nullptr) {
    //        if (obj->routePositionArray != nullptr &&
    //        obj->routePositionArray->count() > 0) {
    if (!obj->isArrived) {
      obj->move(dt);
    }
    return;
  }
  if (obj->routePositionArray != nullptr) {
    obj->move(dt);
  }

  obj->moveInterval += dt;
  if (obj->moveInterval < 0.5f) {
    return;
  }
  obj->moveInterval = 0;
  obj->moveToTarget();
}
// A rallying enemy unit is considered "parked" once within this distance of the
// rally point (used by updateUnitMoveNew; see enemyAIManageWaves for the rest of
// the rally/wave constants).
static const float kEAIRallyHoldDist = TILE_SIZE * 2.0f;

void GameScene::updateUnitMoveNew(float dt) {
  for (auto unit : heroArray) {
    if (!unit->isInShuttle && unit->pendingShuttle) {
      EnemyBase *shuttle = (EnemyBase *)unit->pendingShuttle;
      if (!shuttle || shuttle->isDead || shuttle->energy <= 0) {
        unit->pendingShuttle = nullptr;
      } else {
        float dist = unit->getPosition().distance(shuttle->getPosition());
        if (dist <= TILE_SIZE * 3 && isShuttleAdjacentToLand(shuttle)) {
          unit->pendingShuttle = nullptr;
          if (loadUnitToShuttle(shuttle, unit)) {
            refreshShuttleCargoPanel();
          }
        }
      }
    }
    if (unit->isInShuttle) {
      // Keep cargo position in sync with the shuttle
      if (unit->myShuttle)
        unit->setPosition(unit->myShuttle->getPosition());
      continue;
    }
    if (unit->unitAct == UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH) {
      unit->restingTime -= dt;
      if (unit->restingTime < 0) {
        //                unit->unitAct = UNIT_ACT_NONE;
        //                unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
        unit->stopNew();
      } else if (unit->attackDdangPos != Vec2::ZERO &&
                 unit->unitType != UNIT_WORKER && unit->unitType != UNIT_GOBLIN_WORKER &&
                 unit->unitType != UNIT_HUMAN_SHUTTLE && unit->unitType != UNIT_ORC_SHUTTLE &&
                 unit->unitType != UNIT_HUMAN_OIL_SHIP && unit->unitType != UNIT_ORC_OIL_SHIP) {
        unit->target = findTargetEnemy(unit);
        if (unit->target != nullptr) {
          Vector<EnemyBase *> list;
          list.pushBack(unit);
          Vec2 savedDdangPos = unit->attackDdangPos;
          forceAttack(list, (EnemyBase *)unit->target);
          unit->attackDdangPos = savedDdangPos;
          list.clear();
        }
      }
    } else if (unit->unitAct == UNIT_ACT_NONE ||
               unit->unitAct == UNIT_ACT_ATTACK_DDANG) {
      if (unit->unitType != UNIT_WORKER &&
          unit->unitType != UNIT_GOBLIN_WORKER &&
          unit->unitType != UNIT_HUMAN_SHUTTLE &&
          unit->unitType != UNIT_ORC_SHUTTLE &&
          unit->unitType != UNIT_HUMAN_OIL_SHIP &&
          unit->unitType != UNIT_ORC_OIL_SHIP) {
        unit->target = findTargetEnemy(unit);
        if (unit->target == nullptr) {
          if (unit->unitAct == UNIT_ACT_NONE) {
            unit->unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
            unit->restingTime = 3 + 0.1f * (rand() % 70);
          }
        } else {
          Vector<EnemyBase *> list;
          list.pushBack(unit);
          Vec2 attackDdangPos = unit->attackDdangPos;
          forceAttack(list, (EnemyBase *)unit->target);
          unit->attackDdangPos = attackDdangPos;
          list.clear();
        }
      }
    }
    unit->moveNew(dt);
  }

  for (auto unit : enemyArray) {
    if (!unit->isInShuttle && unit->pendingShuttle) {
      EnemyBase *shuttle = (EnemyBase *)unit->pendingShuttle;
      if (!shuttle || shuttle->isDead || shuttle->energy <= 0) {
        unit->pendingShuttle = nullptr;
      } else {
        float dist = unit->getPosition().distance(shuttle->getPosition());
        if (dist <= TILE_SIZE * 3 && isShuttleAdjacentToLand(shuttle)) {
          unit->pendingShuttle = nullptr;
          if (loadUnitToShuttle(shuttle, unit)) {
            refreshShuttleCargoPanel();
          }
        }
      }
    }
    if (unit->isInShuttle) {
      if (unit->myShuttle)
        unit->setPosition(unit->myShuttle->getPosition());
      continue;
    }
    if (unit->unitAct == UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH) {
      unit->restingTime -= dt;
      if (unit->restingTime < 0) {
        unit->unitAct = UNIT_ACT_NONE;
        unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
      }
    } else if (unit->unitAct == UNIT_ACT_NONE ||
               unit->unitAct == UNIT_ACT_ATTACK_DDANG) {
      //            if (unit->unitActDetail == UNIT_ACT_DETAIL_IDLE) {
      //                if (unit->canFindTarget) {
      //            if (!isMultiplay) {
      if (unit->unitType == UNIT_HUMAN_SHUTTLE || unit->unitType == UNIT_ORC_SHUTTLE) {
        unit->target = nullptr;
      } else {
        unit->target = findTargetHero(unit);
      }
      //            }

      if (unit->unreachableTarget == unit->target) {
        unit->target = nullptr;
        unit->unreachableTarget = nullptr;
      }

      if (unit->target == nullptr) {
        if (!isMultiplay && unit->isRallying) {
          // Rally hold: no threat in sight. Walk back to the rally point if
          // we've drifted from it, otherwise rest and hold. Keep revenge on so
          // the unit still fights back locally when attacked. Crucially the
          // unit does NOT hunt the player from here - that only happens once
          // its wave is released (enemyAIManageWaves clears isRallying).
          unit->canRevengeAttack = true;
          if (enemyAIRallyPointValid &&
              unit->getPosition().distance(enemyAIRallyPoint) > kEAIRallyHoldDist) {
            Vector<EnemyBase *> list;
            list.pushBack(unit);
            moveTo(list, enemyAIRallyPoint);
            list.clear();
          } else if (unit->unitAct == UNIT_ACT_NONE) {
            unit->unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
            unit->restingTime = 3;
          }
        } else if (unit->unitAct == UNIT_ACT_NONE) {
          unit->unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
          unit->restingTime = 3; // 0.1f*(rand()%100);
        }
      } else {
        //                        unit->unitAct = UNIT_ACT_ATTACK;
        //                        unit->moveToPos = Vec2::ZERO;
        //                        unit->moveFlagPos =
        //                        unit->target->getPosition();
        Vector<EnemyBase *> list;
        list.pushBack(unit);
        Vec2 attackDdangPos = unit->attackDdangPos;
        forceAttack(list, (EnemyBase *)unit->target);
        unit->attackDdangPos = attackDdangPos;
        list.clear();
        unit->canRevengeAttack = true;
        //                        if (isMultiplay) {
        //                            MM->enemyMoveAndAttackTo(Value(unit->unitID).asString(),
        //                            (int)unit->moveFlagPos.x,
        //                            (int)unit->moveFlagPos.y);
        //                        }
      }
      //                }
      //            }
    }
    if (!isMultiplay) {
      if (unit->wantToEli) {
        if (unit->unitActDetail == UNIT_ACT_DETAIL_IDLE &&
            unit->unitAct == UNIT_ACT_NONE) {
          attackNearHero((EnemyBase *)unit);
        }
      }
    }

    unit->moveNew(dt);
  }
}
void GameScene::updateUnitMove(float dt) {
  // heros
  //    int moveCounter = 1;
  //    for(auto hero: heroArray){
  //        if(!hero->canMove && !hero->canFindTarget && hero->energy <= 0)
  //        continue;
  //        // finding target
  //        if(hero->canFindTarget && (hero->target == nullptr || hero->target
  //        == hero->dummyTarget)){
  ////        if(hero->canFindTarget && ((hero->unitAct == UNIT_ACT_NONE &&
  /// hero->unitActDetail != UNIT_ACT_DETAIL_ATTACK) || (hero->unitAct ==
  /// UNIT_ACT_ATTACK_DDANG && hero->unitActDetail != UNIT_ACT_DETAIL_ATTACK))){
  //            float minDistance = 1000000;
  //            EnemyBase* nearest = nullptr;
  //            for(auto drop: enemyArray){
  //                if (drop->untouchable || drop->energy <= 0) {
  //                    continue;
  //                }
  //                if (!canAttack(hero, drop)) {
  //                    continue;
  //                }
  //                float distance =
  //                hero->getPosition().distanceSquared(drop->getPosition()); if
  //                (distance < hero->monitoringDistance) {
  //                    if (minDistance > distance) {
  //                        minDistance = distance;
  //                        nearest = drop;
  //                    }
  //                }
  //            }
  //            EnemyBase* nearestTree = nullptr;
  //            if(hero->unitType == UNIT_WIZARD){
  //                for(auto tree: mutualArray){
  //                    if(tree->unitType == UNIT_TREE_FOR_BATTLE){
  //                        float distance =
  //                        hero->getPosition().distanceSquared(tree->getPosition());
  //                        if (distance < hero->monitoringDistance) {
  //                            if (minDistance > distance) {
  //                                minDistance = distance;
  //                                nearestTree = tree;
  //                            }
  //                        }
  //                    }
  //                }
  //            }
  //
  //            if(nearest != nullptr || nearestTree != nullptr){
  //                if(hero->canMove){
  //                    if(nearestTree != nullptr){
  //                        if(nearest == nullptr){
  //                            hero->moveToTarget(nearestTree);
  //                        }else{
  //                            float treeDistance =
  //                            hero->getPosition().distanceSquared(nearestTree->getPosition());
  //                            float unitDistance =
  //                            hero->getPosition().distanceSquared(nearest->getPosition());
  //                            hero->moveToTarget(treeDistance > unitDistance?
  //                            nearest:nearestTree);
  //                        }
  //
  //                        hero->canFindTarget = false;
  //                        hero->forceAttackTarget = true;
  //                    }else{
  //                        hero->moveToTarget(nearest);
  //                        hero->canFindTarget = false;
  //                    }
  //                }else{
  //                    hero->target = nearest;
  //                }
  //            }
  //        }
  //
  //        // move
  //        hero->move(dt);
  //    }
  //    // enemy
  //    for(auto enemy:enemyArray){
  //        if (enemy->energy <= 0) continue;
  //        if (!enemy->canFindTarget && enemy->target != nullptr){
  //            float distance =
  //            enemy->getPosition().distanceSquared(enemy->target->getPosition());
  //            if (distance >= enemy->monitoringDistance) {
  //                enemy->canFindTarget = true;
  //                enemy->target = nullptr;
  //                // if enemy is out of sight enemy stops following test
  //            }
  //        }
  //
  //        if(enemy->canFindTarget && enemy->target == nullptr){
  //            float minDistance = 1000000;
  //            EnemyBase* nearest = nullptr;
  //            for(auto drop: heroArray){
  //                if (drop->untouchable || drop->energy <= 0) {
  //                    continue;
  //                }
  //                if (!canAttack(enemy, drop)) {
  //                    continue;
  //                }
  //
  //                float distance =
  //                enemy->getPosition().distanceSquared(drop->getPosition());
  //
  //                if (distance < enemy->monitoringDistance) {
  //                    if (minDistance > distance) {
  //                        minDistance = distance;
  //                        nearest = drop;
  //                    }
  //                }
  //            }
  //            if(nearest != nullptr){
  //                if(enemy->canMove){
  //                    enemy->canFindTarget = false;
  //                    enemy->moveToTarget(nearest);
  //                }else{
  //                    enemy->target = nearest;
  //                }
  //            }
  //        }
  //
  //        // move
  //        enemy->move(dt);
  //    }
}
void GameScene::createMissile(std::string strMsName,
                               std::string strArrivedMsName, Vec2 startPos,
                               Vec2 endPos, float moveTime, int damage,
                               bool isEnemy, float angle, Movable *attacker,
                               float delay) {
  Movable *ms = Movable::createMovable(UNIT_MISSILE_NOTHING, damage, 0,
                                       strMsName.c_str());
  ms->isEnemy = isEnemy;
  ms->ap = damage;
  batch->addChild(ms);
  ms->setPosition(startPos);
  ms->setOpacity(0);
  ms->setRotation(angle + rand() % 30 - 15);
  ms->runAction(Sequence::create(
      DelayTime::create(delay), FadeIn::create(0),
      MoveTo::create(moveTime, endPos),
      CallFuncN::create(CC_CALLBACK_1(GameScene::checkMissileHit, this)),
      NULL));
  ms->setName(strArrivedMsName);
  ms->shooter = attacker;
}
void GameScene::checkMissileHit(Ref *ref) {
  Movable *ms = (Movable *)ref;
  if (!ms->isEnemy) {
    for (auto unit : enemyArray) {
      if (unit->getBoundingBox().containsPoint(ms->getPosition())) {
        if (unit->getHitAndIsDead(ms->ap, ms->shooter)) {
          removeDeadUnit((EnemyBase *)unit);
        }
        break;
      }
    }
  } else {
    for (auto unit : heroArray) {
      if (unit->getBoundingBox().containsPoint(ms->getPosition())) {
        if (unit->getHitAndIsDead(ms->ap, ms->shooter)) {
          removeDeadUnit((EnemyBase *)unit);
        }
        break;
      }
    }
  }
  if (ms->getName().size() > 0) {
    ms->setSpriteFrame(ms->getName());
    ms->runAction(Sequence::create(
        DelayTime::create(1), FadeOut::create(1),
        CallFunc::create(CC_CALLBACK_0(Movable::removeFromParent, ms)), NULL));
  } else {
    ms->removeFromParent();
  }
}
Movable *GameScene::createMissile(int missileType, int dmg, bool visible,
                                   float time, int angle, int speed, Vec2 pos,
                                   bool isFromEnemy, std::string weaponName) {
  Movable *sptMissile = nullptr;

  if (missileType == MISSILE_SLASH) {
    sptMissile =
        Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "slash.png");
  } else if (missileType == MISSILE_ARROW) {
    sptMissile =
        Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "arrow.png");
  } else if (missileType == MISSILE_FIREBALL) {
    sptMissile =
        Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "fire0.png");
    sptMissile->runAnimation("fire", true);
  }
  if (sptMissile == nullptr) {
    return nullptr;
  }
  if (weaponName.size() > 0) {
    sptMissile->setSpriteFrame(strmake("%sMs.png", weaponName.c_str()));
  }
  batch->addChild(sptMissile, 11);
  sptMissile->setPosition(pos);
  float delta = speed * time;
  sptMissile->runAction(Sequence::create(
      MoveBy::create(time, Vec2(delta * cos(angle * 3.14f / 180),
                                delta * sin(angle * 3.14f / 180))),
      CallFuncN::create(CC_CALLBACK_1(GameScene::missileMoveDone, this)),
      nullptr));
  sptMissile->setRotation(-angle + 90);
  if (isFromEnemy) {
    enemyMissileArray.pushBack(sptMissile);
  } else {
    heroMissileArray.pushBack(sptMissile);
  }
  return sptMissile;
}
// void GameScene::gettingReadyForBattle(float dt){
//     battleReadyCountDown--;
//     if (battleReadyCountDown == 2) {
//         Label* lbl = Label::createWithTTF("READY",
//         GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//         GameManager::getInstance()->getHudLayer()->addChild(lbl);
//         lbl->setPosition(size/2);
//         lbl->setScale(0.2f);
//         lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f,
//         1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f),
//         CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)),
//         nullptr));
//     }else if (battleReadyCountDown == 1) {
//         Label* lbl = Label::createWithTTF("BATTLE!",
//         GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//         GameManager::getInstance()->getHudLayer()->addChild(lbl);
//         lbl->setPosition(size/2);
//         lbl->setScale(0.2f);
//         lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f,
//         1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f),
//         CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)),
//         nullptr));
//     }else if (battleReadyCountDown == 0) {
//         this->unschedule(schedule_selector(GameScene::gettingReadyForBattle));
//         isBattleStarted = true;
//     }
// }
GameScene::GameScene() {}
void GameScene::getDown() {}
void GameScene::dialogFinished(Ref *obj) {}
void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode,
                               Event *unused_event) {
  if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS ||                                  \
     CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//        NativeInterface::NativeInterface::destroyAds();
#endif

    //        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
  }
}
void GameScene::showWarpAnimation() {
  Sprite *sptBeam = Sprite::createWithSpriteFrameName("blueBeamVertical0.png");
  sptBeam->setAnchorPoint(Vec2(0.5, 0));
  sptBeam->setScaleY(10);
  sptBeam->setScaleX(2);
  batch->addChild(sptBeam, 11);
  const char *beamName = "blueBeam";
  if (stageIndex == indexToWarp || (isBossMap && !isSetStageDone)) {
    beamName = "redBeam";
  }
  Animation *animation = AnimationCache::getInstance()->getAnimation(beamName);
  Animate *animate = Animate::create(animation);
  sptBeam->runAction(Sequence::create(
      animate,
      CallFuncN::create(
          CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptBeam)),
      CallFunc::create(CC_CALLBACK_0(GameScene::checkEnding, this)), NULL));
  Vec2 pos;

  sptBeam->setPosition(pos + Vec2(0, 100));
  sptBeam->runAction(MoveTo::create(0.05, pos));

  this->runAction(Sequence::create(
      DelayTime::create(18 * 0.04),
      CallFunc::create(CC_CALLBACK_0(GameScene::stopWoongSound, this)),
      nullptr));
  shakeScreen(6);

  //    this->runAction(Sequence::create(DelayTime::create(1),
  //    CallFunc::create(CC_CALLBACK_0(GameScene::testFunction, this)), NULL));
}
void GameScene::checkEnding() {}
void GameScene::testFunction() {
  GameManager::getInstance()->getHudLayer()->dialogBox->removeFromParent();
}
void GameScene::takeAction() {
  log("take action");
  GameManager::getInstance()->getHudLayer()->lblTouch->stopAllActions();
  GameManager::getInstance()->getHudLayer()->lblTouch->setOpacity(0);
}
void GameScene::warpOut() {}
void GameScene::warp() {
  GameManager::getInstance()->getHudLayer()->showStageTitle();

  this->runAction(Sequence::create(
      DelayTime::create(4),
      CallFunc::create(CC_CALLBACK_0(GameScene::goToNextScene, this)), NULL));
}
void GameScene::goToNextScene() {
  GameManager::getInstance()->getHudLayer()->removeListener();
  Scene *scene = Scene::create();
  bool isBoss = false;
  if (stageIndex >= 0 && !isBossMap) {
    isBoss = true;
  }
  scene->addChild(GameScene::scene(indexToWarp, isBoss));
  Director::getInstance()->replaceScene(scene);
}
void GameScene::enemyUpdate(float dt) {
  if (isGameOver) {
    return;
  }
  //    dnDamageBoxes->clear();
  //    dnDamageBoxes->drawRect(player->damageBoundingBox().origin,
  //    player->damageBoundingBox().origin + player->damageBoundingBox().size,
  //    Color4F::RED); dnCollisionBoxes->clear();
  //    dnCollisionBoxes->drawRect(player->collisionBoundingBox().origin,
  //    player->collisionBoundingBox().origin +
  //    player->collisionBoundingBox().size, Color4F::BLUE);
  cocos2d::Rect projectileRect;
  bool isEffectOn = false;
  missileEffectCollapsedTime += dt;
  if (missileEffectCollapsedTime < 0.023) {

  } else {
    isEffectOn = true;
    missileEffectCollapsedTime = 0;
  }

  for (auto drop : enemyArray) {

    projectileRect = drop->damageBoundingBox();

    if (isEffectOn) {

      if (drop->effectType == MISSILE_EFFECT_SMOKE)
        runEffect(EFFECT_SMOKE, drop->getPosition());
      if (drop->effectType == MISSILE_EFFECT_RED_BALL)
        runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_GREEN_SMOKE)
        runEffect(EFFECT_GREEN_SMOKE, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_RED_SMOKE)
        runEffect(EFFECT_RED_SMOKE, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_SHARK_GUN) {
      }
    }
  }

  for (auto missile : enemyMissileArray) {
    projectileRect = cocos2d::Rect(
        missile->getPosition().x -
            (missile->getContentSize().width * missile->getScaleX() / 2),
        missile->getPosition().y -
            (missile->getContentSize().height * missile->getScaleY() / 2),
        missile->getContentSize().width * missile->getScaleX(),
        missile->getContentSize().height * missile->getScaleY());
    projectileRect = RectInset(projectileRect, 3, 3);

    bool toBreak = false;
    for (auto hero : heroArray) {
      if (projectileRect.intersectsRect(hero->getBoundingBox()) &&
          missile->isHurting) {
        if (!hero->immortal) {
          attackHero(hero, missile->energy);
        }
        enemyMissileArray.eraseObject(missile);
        missile->removeFromParent();
      }
    }
    if (toBreak) {
      break;
    }
  }
}
void GameScene::updateFireStick(float dt) {}
void GameScene::destructableUpdate() {
  if (isGameOver) {
    return;
  }

  cocos2d::Rect projectileRect, mRect;
  Vector<Movable *> missileToRemove;
  EnemyBase *drop;
  for (auto missile : heroMissileArray) {
    for (int i = 0; i < destructableArray.size(); i++) {
      drop = destructableArray.at(i);

      if (drop->untouchable ||
          drop->getPosition().y - missile->getPosition().y > 50 ||
          drop->getPosition().y - missile->getPosition().y < -50) {
        continue;
      }

      //            mRect = cocos2d::Rect(missile->getPosition().x -
      //            (missile->getContentSize().width*missile->getScaleX()/2),
      //                         missile->getPosition().y -
      //                         (missile->getContentSize().height*missile->getScaleY()/2),
      //                         missile->getContentSize().width*missile->getScaleX(),
      //                         missile->getContentSize().height*missile->getScaleY());
      mRect = missile->getBoundingBox();
      projectileRect = drop->damageBoundingBox();
      if (projectileRect.intersectsRect(mRect)) {
        if (!drop->immortal) {
          float demage = missile->energy;

          if (demage >= 10) {
            demage = demage * 0.8f + (rand() % ((int)(demage * 0.2f)));
          }

          drop->energy -= (int)demage;
          if (drop->energy <= 0) {
            destroyDestructable(drop);
            currentScore += drop->secondTag * 10;

            destructableArray.eraseObject(drop);
            drop->removeFromParentAndCleanup(true);

            GameManager::getInstance()->totalDestroy++;
            GameManager::getInstance()->totalScore += 10;
          }
        }

        missileToRemove.pushBack(missile);
        this->runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition(),
                        missile->getRotation() + 180);

        break;
      }
    }
  }

  for (auto spt : missileToRemove) {
    spt->stopAllActions();
    heroMissileArray.eraseObject(spt);
    enemyMissileArray.eraseObject(spt);
    MovableMissileArray.eraseObject(spt);
    enemyArray.eraseObject((EnemyBase *)spt);
    if (spt->getTag() == UNIT_MISSILE_CHASING) {
      chasingMissileArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_Movable) {
      MovableMissileArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_CUSTOM) {
      customMovingArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_STRAIGHT) {
      straightMovingArray.eraseObject(spt);
    }
    spt->removeFromParentAndCleanup(true);
  }
  //    enemiesToRemove.clear();
}
void GameScene::destroyDestructable(Movable *drop) {
  if (drop->secondTag < 0) {
    return;
  }
  const char *partName;
  if (drop->secondTag < 3) {
    partName = strmake("destructable%d_0.png", drop->secondTag).c_str();

  } else {
    partName = "destructable3_0.png";
  }
  for (int i = 0; i < 3; i++) {
    Movable *part = Movable::createMovable(UNIT_DESTRUCTABLE, 0, 0, partName);
    batch->addChild(part, 1);
    part->setPosition(drop->getPosition());
    part->weight = 1;
    auto rotate = RotateBy::create(1, rand() % 2 == 0 ? 300 : -300);
    rotate->setTag(ACTION_TAG_ROTATION);
    part->runAction(rotate);
    part->velocity = Vec2(rand() % 500 - 250, 250);
    part->runAction(Sequence::create(
        DelayTime::create(3),
        CallFuncN::create(CC_CALLBACK_1(GameScene::coinWaitDone, this)),
        NULL));
    MovableCoinArray.pushBack(part);
  }
  runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
}
void GameScene::showDamage(int damage, Vec2 pos) {
  if (damage <= 0) {
    return;
  }

  Node *sptNumber = createNumberSprite(damage);
  batch->addChild(sptNumber, 100);
  sptNumber->setScale(1.5f);
  sptNumber->setPosition(pos);
  sptNumber->runAction(
      EaseIn::create(MoveBy::create(1, cocos2d::Vec2(0, 25)), 0.3));
  sptNumber->runAction(
      Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, sptNumber)),
                       NULL));
}
void GameScene::attackEnemy(EnemyBase *drop, int damage) {
  //    damage = 100; // test

  damage -= drop->dp;
  if (damage <= 0) {
    damage = 1;
  }
  drop->energy -= (int)damage;
  drop->onHit();
  showDamage(damage, drop->getPosition());

  if (drop->energy <= 0) {
    enemiesToRemove.pushBack(drop);
  } else {
    drop->blinkForAWhile();
  }

  shakeScreen(1);
  isPaused = true;
  this->runAction(Sequence::create(
      DelayTime::create(0.07f),
      CallFunc::create(CC_CALLBACK_0(GameScene::resumeLayer, this)), NULL));
}

void GameScene::attackHero(EnemyBase *drop, int damage) {
  damage -= drop->dp;

  if (damage <= 0) {
    damage = 1;
  }
  drop->energy -= (int)damage;

  showDamage(damage, drop->getPosition());

  if (drop->energy <= 0) {
    this->runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
    //        this->createRewards(drop);

    //        killCountForRecord++;
    //        GameManager::getInstance()->totalKill++;
    //        GameManager::getInstance()->totalScore += 100;
    //        GameManager::getInstance()->getHudLayer()->setScore(GameManager::getInstance()->totalScore);

    destructableArray.eraseObject(drop);
    MovableArray.eraseObject(drop);
    batch->removeChild(drop, true);
    batch->removeChild(drop, true);
    removeHero(drop);
  } else {
    drop->blinkForAWhile();
  }
}
void GameScene::immortalForSec(int sec) {
  indestructableTime = sec;
  this->schedule(schedule_selector(GameScene::immortalForAWhile), 0.1);
}
void GameScene::immortalForAWhile(float dt) {
  indestructableTime -= dt;
  if (indestructableTime < 0) {
    this->unschedule(schedule_selector(GameScene::immortalForAWhile));
  }
}
bool GameScene::hitPlayer(int damage) {
  //    return false; // test

  return false;
}
void GameScene::stepBackForHit(float dt) {}
void GameScene::unfreezePlayer(float dt) {}

cocos2d::Rect GameScene::RectInset(cocos2d::Rect rect, float x, float y) {
  return cocos2d::Rect(rect.origin.x + x, rect.origin.y + y,
                       rect.size.width - x * 2, rect.size.height - y * 2);
}
void GameScene::endGame(bool win) {
  if (isGameEnded) {
    return;
  }
  isGameEnded = true;
  this->unschedule(schedule_selector(GameScene::gravityUpdate));
  //    this->unschedule(schedule_selector(GameScene::gravityUpdateForStraight));
  //    this->unschedule(schedule_selector(GameScene::gravityUpdateForCustomMoving));
  //    this->unschedule(schedule_selector(GameScene::gravityUpdateForFlyingOrSwimingEnemies));

  //    for(auto unit: heroList) {
  //        unit->stopAllActions();
  //        unit->unscheduleAllCallbacks();
  //    }
  //    for(auto unit: enemyHeroList) {
  //        unit->stopAllActions();
  //        unit->unscheduleAllCallbacks();
  //    }
  for (auto unit : enemyArray) {
    unit->stopAllActions();
    unit->unscheduleAllCallbacks();
  }
  for (auto unit : heroArray) {
    unit->stopAllActions();
    unit->unscheduleAllCallbacks();
  }
  for (auto unit : mutualArray) {
    unit->stopAllActions();
    unit->unscheduleAllCallbacks();
  }
  for (auto unit : readyHeroArray) {
    unit->stopAllActions();
    unit->unscheduleAllCallbacks();
  }

  for (auto enemy : enemyArray) {
    enemy->pause();
  }
  isGameOver = !win;
  if (GM->nextScene != STAGE_RAID && GM->nextScene != STAGE_SINGLEPLAY) {
    HUD->showWinPopup(win);
  }

  if (win) {
    if (GM->isColosseum) {
      GM->isColosseum = false;
      sendColosseumScore();
      UDSetBool(KEY_DAILY_MISSION_ARENA, true);
    } else if (GM->nextScene == STAGE_SINGLEPLAY) {
      UDSetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, true);
    } else if (GM->nextScene == STAGE_RAID) {
      UDSetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, true);
    } else {
      int stage = GM->getDailyMissionCampaignStageIndex();
      if (GM->isThisCampaignFromDailyMission || stageIndex == stage) {
        UDSetBool(KEY_DAILY_MISSION_CAMPAIGN, true);
        GM->isThisCampaignFromDailyMission = false;
      }
    }
    if (isMultiplay && gameTimer > 60) {
      // send multiplay win record
    }
  }
}
void GameScene::sendTestScore() {
  gameTimer = 60 * 9; // rand()%1000;
  totalKillUnit = 6;
  totalProducedUnit = rand() % 1000;

  isGameOver = true;

  sendColosseumScore();
}
void GameScene::sendColosseumScore() {
  HUD->showIndicator();

  int lastScore = UDGetInt(KEY_COLOSSEUM_SCORE, 0);
  int clearCount = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
  clearCount++;
  UDSetInt(KEY_ARENA_CLEAR_COUNT, clearCount);
  if ((lastScore == 0 || lastScore > gameTimer) && gameTimer > 60) {
    GM->pendingArenaScore = gameTimer;
    GM->requestOnlineAccount(HUD, PENDING_ONLINE_ACTION_ARENA_SCORE);
  } else {
    HUD->hideIndicator();
  }
  GM->addMonthlyEventProgress(EVENT_MISSION_ARENA_CLEAR, 1);

  //    if(playerId.size() > 0 && gameTimer > 0){
  //        SM->sendColosseumScore(gameTimer, totalKillUnit, totalProducedUnit,
  //        playerId, playerName, UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0),
  //        isGameOver?0:1);
  //    }
}
void GameScene::onSendComplete(int leagueIndex) {
  HUD->hideIndicator();
  log("league index: %d", leagueIndex);
  UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, leagueIndex);
}

void GameScene::updateResult(float dt) {
  if (resultDone) {
    this->unschedule(schedule_selector(GameScene::updateResult));
    return;
  }

  Text *lbllblTotalScoreValue =
      (Text *)resultLayer->getChildByName("lblTotalScoreValue");
  Text *lblKill = (Text *)resultLayer->getChildByName("lblKill");
  Text *lblKillValue = (Text *)resultLayer->getChildByName("lblKillValue");
  lblKill->setVisible(true);
  lblKillValue->setVisible(true);
  if (kill < GameManager::getInstance()->totalKill) {
    kill++;
    lblKillValue->setString(Value(kill).asString());

    totalScore += 100;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblDestroy = (Text *)resultLayer->getChildByName("lblDestroy");
  Text *lblDestroyValue =
      (Text *)resultLayer->getChildByName("lblDestroyValue");
  lblDestroy->setVisible(true);
  lblDestroyValue->setVisible(true);
  if (destroy < GameManager::getInstance()->totalDestroy) {
    destroy++;
    lblDestroyValue->setString(Value(destroy).asString());

    totalScore += 10;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblCoin = (Text *)resultLayer->getChildByName("lblCoin");
  Text *lblCoinValue = (Text *)resultLayer->getChildByName("lblCoinValue");
  lblCoin->setVisible(true);
  lblCoinValue->setVisible(true);
  if (coin < GameManager::getInstance()->totalCoin) {
    int coinAmountToAdd = 1;
    if (coin + 10 < GameManager::getInstance()->totalCoin) {
      coinAmountToAdd = 10;
    } else if (coin + 100 < GameManager::getInstance()->totalCoin) {
      coinAmountToAdd = 100;
    }
    coin += coinAmountToAdd;
    lblCoinValue->setString(Value(coin).asString());

    totalScore += 10 * coinAmountToAdd;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblRoom = (Text *)resultLayer->getChildByName("lblRoom");
  Text *lblRoomValue = (Text *)resultLayer->getChildByName("lblRoomValue");
  lblRoom->setVisible(true);
  lblRoomValue->setVisible(true);
  if (room < GameManager::getInstance()->totalRoom) {
    room++;
    lblRoomValue->setString(Value(room).asString());

    totalScore += 500;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblBoss = (Text *)resultLayer->getChildByName("lblBoss");
  Text *lblBossValue = (Text *)resultLayer->getChildByName("lblBossValue");
  lblBoss->setVisible(true);
  lblBossValue->setVisible(true);
  if (boss < GameManager::getInstance()->totalBoss) {
    boss++;
    lblBossValue->setString(Value(boss).asString());

    totalScore += 10000;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblTime = (Text *)resultLayer->getChildByName("lblTime");
  Text *lblTimeValue = (Text *)resultLayer->getChildByName("lblTimeValue");
  lblTime->setVisible(true);
  lblTimeValue->setVisible(true);

  if (time < GameManager::getInstance()->totalTime) {
    time += 60;
    if (time > GameManager::getInstance()->totalTime) {
      time = GameManager::getInstance()->totalTime;
    }
    lblTimeValue->setString(
        strmake("%2d:%2d", (60 * 60 - time) / 60, (60 * 60 - time) % 60));

    totalScore += 600;
    lbllblTotalScoreValue->setString(Value(totalScore).asString());
    return;
  }
  Text *lblOk = (Text *)resultLayer->getChildByName("lblOk");
  resultDone = true;
  lblOk->setVisible(true);

  Sprite *cursor = Sprite::create("targetWhite.png");
  resultLayer->addChild(cursor, 99990);
  cursor->runAction(RepeatForever::create(RotateBy::create(1, 100)));
  cursor->setPosition(lblOk->getPosition() + Vec2(-20, 0));

  Sprite *innerCursor = Sprite::create("targetWhite.png");
  cursor->addChild(innerCursor, 99990);
  innerCursor->runAction(RepeatForever::create(RotateBy::create(1, -160)));
  innerCursor->setScale(0.7);
  innerCursor->setPosition(Vec2(cursor->getContentSize().width / 2,
                                cursor->getContentSize().height / 2));
}
void GameScene::gameOver() {
  pauseLayer();
  isGameOver = true;

  for (auto drop : enemyMissileArray) {
    drop->removeFromParentAndCleanup(true);
  }
  enemyMissileArray.clear();
}

void GameScene::missileUpdate(float dt) {
  if (isGameOver) {
    return;
  }

  cocos2d::Rect mRect;
  //    enemiesToRemove.clear();

  Vector<Movable *> missileToRemove;
  EnemyBase *drop;
  for (auto missile : heroMissileArray) {
    mRect = cocos2d::Rect(
        missile->getPosition().x -
            (missile->getContentSize().width * missile->getScaleX() / 2),
        missile->getPosition().y -
            (missile->getContentSize().height * missile->getScaleY() / 2),
        missile->getContentSize().width * missile->getScaleX(),
        missile->getContentSize().height * missile->getScaleY());
    for (int i = 0; i < enemyArray.size(); i++) {
      drop = enemyArray.at(i);

      if (drop->untouchable ||
          drop->getPosition().y - missile->getPosition().y > 50 ||
          drop->getPosition().y - missile->getPosition().y < -50) {
        continue;
      }

      if (drop->damageBoundingBox().intersectsRect(mRect)) {
        Vec2 pos = drop->getPosition();
        if (!drop->untouchable) {
          if (!drop->immortal) {
            float damage =
                missile->energy + missile->energy * extraPower / 100.0f;

            if (damage >= 10) {
              damage = damage * 0.8f + (rand() % ((int)(damage * 0.4f)));
            }

            attackEnemy(drop, damage);

            if (false && rand() % 100 > 100 - DEFAULT_CRITICAL - extraCritical -
                                            criticalLevel) { // critical
              damage *= 2;
              Sprite *critical =
                  Sprite::createWithSpriteFrameName("critical.png");
              batch->addChild(critical, 100);
              critical->getTexture()->setAliasTexParameters();
              //                            critical->setColor(aColor3B(0,0,0));
              critical->setScale(0.5f);
              critical->setPosition(drop->getPosition() + Vec2(0, 15));
              critical->runAction(
                  EaseIn::create(MoveBy::create(1, Vec2(0, 25)), 0.3));
              critical->runAction(
                  Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3),
                                   CallFuncN::create(CC_CALLBACK_1(
                                       GameScene::spriteMoveDone, this)),
                                   NULL));

              shakeScreen(1);
            }

            //                        totalPower += damage;
            if (drop == theBoss) {
              //                            GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
            } else {
              if (MovableArray.find(drop) != MovableArray.end()) {
                drop->desiredPosition = drop->desiredPosition + Vec2(0, 2);
                drop->setPosition(drop->getPosition() + Vec2(0, 2));
              }
            }

            if (drop->moveTypeAfterFindHero != drop->currentMoveType) {
              //                    drop->doActionAfterFindTarget();
            }

            if (drop->energy <= 0) {

            } else {
              drop->blinkForAWhile();
            }
          }
          if (missile->secondTag != WEAPON_ENERGY_GUN) {
            missileToRemove.pushBack(missile);
          } else {
          }
          this->runEffect(EFFECT_EXPLODE_SMALL, pos);

          break;
        }
      }
    }
  }

  int killCount = 0;
  for (auto spt : enemiesToRemove) {
    removeEnemy(spt);
  }

  for (auto spt : missileToRemove) {
    spt->stopAllActions();
    heroMissileArray.eraseObject(spt);
    if (spt->getTag() == UNIT_MISSILE_CHASING) {
      chasingMissileArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_Movable) {
      MovableMissileArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_CUSTOM) {
      customMovingArray.eraseObject(spt);
    } else if (spt->getTag() == UNIT_MISSILE_STRAIGHT) {
      straightMovingArray.eraseObject(spt);
    }
    spt->removeFromParentAndCleanup(true);
  }
  enemiesToRemove.clear();
}
void GameScene::makeGate(float dt) {}
void GameScene::bossExplode(float dt) {
  if (rand() % 2 == 0) {
    runEffect(EFFECT_EXPLODE_HUGE,
              theBoss->getPosition() +
                  Vec2(rand() % 20 - 10, rand() % 20 - 30));
  } else {
    runEffect(EFFECT_BOMB_WALL, theBoss->getPosition() +
                                    Vec2(rand() % 20 - 10, rand() % 30 - 30));
  }
}
void GameScene::shakeScreenOnce() { shakeScreen(2); }
void GameScene::shakeScreen(int count) {
  Vector<FiniteTimeAction *> array;
  for (int i = 0; i < count; i++) {
    //        array.pushBack(CallFunc::create(CC_CALLBACK_0(GameScene::shakeScreenFirst,
    //        this))); array.pushBack(DelayTime::create(0.03));
    //        array.pushBack(CallFunc::create(CC_CALLBACK_0(GameScene::shakeScreenSecond,
    //        this))); array.pushBack(DelayTime::create(0.05));
    array.pushBack(MoveBy::create(0.07, Vec2(4, 0)));
    array.pushBack(MoveBy::create(0.07, Vec2(-4, 0)));
  }
  //    array.pushBack(CallFunc::create(CC_CALLBACK_0(GameScene::shakeScreenEnd,
  //    this))); array.pushBack(DelayTime::create(0.03));
  this->runAction(Sequence::create(array));
}
void GameScene::shakeScreenFirst() {
  //    extraCameraPos = Vec2((rand()%10)*0.1, (rand()%10)*0.1);
  extraCameraPos = Vec2(1, 0);
}
void GameScene::shakeScreenSecond() {
  //    extraCameraPos = Vec2(-(rand()%10)*0.1, -(rand()%10)*0.1);
  extraCameraPos = Vec2(-1, 0);
}
void GameScene::shakeScreenEnd() { extraCameraPos = Vec2::ZERO; }
void GameScene::removeHero(EnemyBase *spt) {
  Movable *drop;
  for (int i = 0; i < chasingMissileArray.size(); i++) {
    drop = (Movable *)chasingMissileArray.at(i);
    if (drop->target == spt) {
      drop->target = NULL;
    }
  }

  spt->stopAllActions();
  chasingMissileArray.eraseObject(spt);
  straightMovingArray.eraseObject(spt);
  heroArray.eraseObject(spt);
  MovableArray.eraseObject(spt);
  flyingSwimingEnemyArray.eraseObject(spt);
  chasingMissileArray.eraseObject(spt);
  inWaterArray.eraseObject(spt);
  if (fireableArray.find((FireableBase *)spt) == fireableArray.end()) {
    fireableArray.eraseObject((FireableBase *)spt);
  }

  spt->sptWeapon->removeFromParent();
  spt->removeFromParentAndCleanup(true);
}
void GameScene::creatingStarToGreatBall(float dt) {
  for (int i = 0; i < 3; i++) {
    Sprite *spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
    spt->setScale(0.1f);
    spt->setPosition(GM->getRandomPosInCicle(greatBall->getPosition(), 6));
    batch->addChild(spt, 100);
    spt->runAction(RepeatForever::create(
        Sequence::create(FadeTo::create(0.05f, 50 + rand() % 50),
                         DelayTime::create(0.01f * (rand() % 40)),
                         FadeTo::create(0.05f, 240), NULL)));
    float duration = 1 + (rand() % 10) * 0.2f;
    spt->runAction(
        Sequence::create(DelayTime::create(duration), SPT_REMOVE_FUNC, NULL));
    spt->runAction(MoveBy::create(duration, Vec2(0, 16 + rand() % 10)));
  }
}
void GameScene::removeDeadUnit(EnemyBase *unit) {
  if (!unit)
    return;
  if (unit->unitType == UNIT_HUMAN_SHUTTLE || unit->unitType == UNIT_ORC_SHUTTLE ||
      unit->unitType == UNIT_HUMAN_SHIP || unit->unitType == UNIT_ORC_SHIP ||
      unit->unitType == UNIT_HUMAN_BATTLE_SHIP || unit->unitType == UNIT_HUMAN_OIL_SHIP ||
      unit->unitType == UNIT_ORC_OIL_SHIP) {
    // Movable::moveNew re-registers a ship's tile every frame while it's
    // alive, but never unregisters it on death - without this, a ship killed
    // outside that path (combat, or the trigger interpreter's RemoveUnit)
    // would leave its last tile permanently phantom-blocked for every other
    // ship.
    Vec2 tile = getCoordinateFromPosition(unit->getPosition());
    shipOccupiedTiles.erase(std::make_pair((int)tile.x, (int)tile.y));
  }
  // If this shuttle dies, kill all its cargo
  if ((unit->unitType == UNIT_HUMAN_SHUTTLE || unit->unitType == UNIT_ORC_SHUTTLE) &&
      !unit->shuttleCargo.empty()) {
    auto cargoCopy = unit->shuttleCargo;
    memset(unit->cargoGrid, 0, sizeof(unit->cargoGrid));
    unit->shuttleCargo.clear();
    for (auto &slot : cargoCopy) {
      EnemyBase *cargo = (EnemyBase *)slot.unit;
      if (cargo && !cargo->isDead && cargo->energy > 0) {
        cargo->isInShuttle = false;
        cargo->myShuttle = nullptr;
        cargo->untouchable = false;
        removeDeadUnit(cargo);
      }
    }
    if (shuttlePanelTarget == unit)
      hideShuttleCargoPanel();
    for (auto u : heroArray) {
      if (u && u->pendingShuttle == unit)
        u->pendingShuttle = nullptr;
    }
  }
  // If this unit was aboard a shuttle, remove it from cargo
  if (unit->isInShuttle && unit->myShuttle) {
    EnemyBase *shuttle = (EnemyBase *)unit->myShuttle;
    for (int i = 0; i < (int)shuttle->shuttleCargo.size(); i++) {
      if (shuttle->shuttleCargo[i].unit == unit) {
        auto &slot = shuttle->shuttleCargo[i];
        for (int dc = 0; dc < slot.cols; dc++)
          for (int dr = 0; dr < slot.rows; dr++)
            shuttle->cargoGrid[slot.col + dc][slot.row + dr] = false;
        shuttle->shuttleCargo.erase(shuttle->shuttleCargo.begin() + i);
        break;
      }
    }
    unit->isInShuttle = false;
    unit->myShuttle = nullptr;
  }
  unit->isDead = true;
  if (unit->isEnemy) {
    totalKillUnit++;
    if (unit->unitType == UNIT_GOBLIN) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_GOBLIN, 1);
    } else if (unit->unitType == UNIT_SWORDMAN) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_SWORDSMAN, 1);
    } else if (unit->unitType == UNIT_TROLL) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_TROLL, 1);
    } else if (unit->unitType == UNIT_ORC_AXE) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_ORC_AXE, 1);
    } else if (unit->unitType == UNIT_ORC_SPEAR) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_ORC_SPEAR, 1);
    } else if (unit->unitType == UNIT_ZOMBIE_ORC_AXE ||
               unit->unitType == UNIT_ZOMBIE_SWORDSMAN) {
      GM->addMonthlyEventProgress(EVENT_MISSION_KILL_ZOMBIE, 1);
    }
  } else {
    if (clearConditionIndex == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
      bool allyElli = true;
      for (auto unit : heroArray) {
        if (unit->isAlli && unit->unitType == UNIT_ORC_HQ && unit->energy > 0) {
          allyElli = false;
        }
      }
      if (allyElli) {
        endGame(false);
      }
    }
  }
  if (unit->ndLevelCircle) {
    unit->ndLevelCircle->removeFromParent();
    unit->ndLevelCircle = nullptr;
  }
  if (unit->isBuilding) {
    // A destroyed building may be some unit's resource drop-off
    // (returningPlace: HQ / lumber mill / oil depot) or an oil ship's
    // extractor (currentOilExtractor). Unlike a fading unit corpse, a building
    // is removeFromParent()'d immediately below, so those raw pointers dangle
    // at once - the next moveNew()/move() intersectsRect test would deref freed
    // memory and crash, or at best strand the gatherer forever holding
    // resources that never get credited. Null every reference to this building
    // and idle the referencing unit so the next AI tick reassigns it. Safe for
    // both sides (a pointer to a freed building is never valid); this only
    // nulls a dangling reference and un-strands the unit - it does not change
    // player-side gather policy.
    for (int side = 0; side < 2; side++) {
      auto &army = (side == 0) ? heroArray : enemyArray;
      for (auto u : army) {
        if (u == unit)
          continue;
        bool idled = false;
        if (u->returningPlace == unit) {
          // If the worker is hidden inside a mine right now, keep the mine's
          // miner count consistent: mirror the exit path in Movable::moveNew()
          // (decrement the tag, restore the idle frame at zero), otherwise the
          // mine stays on its "occupied" sprite forever.
          if (u->isInMine && u->currentMine != nullptr) {
            int miner = u->currentMine->getTag();
            miner--;
            u->currentMine->setTag(miner);
            if (miner == 0) {
              u->currentMine->setSpriteFrame("mine.png");
            }
            u->currentMine = nullptr;
          }
          u->returningPlace = nullptr;
          u->isGatheringGold = false;
          u->isGatheringTree = false;
          u->isCarryingGold = false;
          u->isCarryingTree = false;
          u->isInMine = false;
          // An oil ship's drop-off died: clear the whole oil state machine too
          // (enemyAIManageShips() treats a ship as idle only when neither
          // isGatheringOil nor isCarryingOil is set). Losing the extractor
          // assignment is fine - the next tick reassigns idle ships.
          u->currentOilExtractor = nullptr;
          u->isGatheringOil = false;
          u->isCarryingOil = false;
          u->isInExtractor = false;
          idled = true;
        }
        if (u->currentOilExtractor == unit) {
          u->currentOilExtractor = nullptr;
          u->isGatheringOil = false;
          u->isCarryingOil = false;
          u->isInExtractor = false;
          // Drop the (possibly still live) depot reference as well so the
          // reset matches a fresh dispatch: re-dispatch goes through
          // setOilShipToExtractor()/gatherOil(), which never reads
          // returningPlace - it is re-derived at pickup via
          // getNearestOilDepot() in Movable::move().
          u->returningPlace = nullptr;
          idled = true;
        }
        if (idled) {
          if (u->target == unit)
            u->target = nullptr;
          u->setVisible(true);
          u->untouchable = false;
          u->stopNew();
        }
      }
    }
    setOccupy(getPositionFromTileCoordinate(unit->buildingStartCoordinate.x,
                                            unit->buildingStartCoordinate.y),
              unit->buildingOccupySize.width, unit->buildingOccupySize.height,
              false);
    updateMiniMapForNonMoving();
    resetPathState();
  }
  if (unit->unitType == UNIT_TREE || unit->unitType == UNIT_TREE_FOR_BATTLE) {
    updateMiniMapForNonMoving();
  }
  if (unit->energyBar != nullptr) {
    unit->energyBar->removeFromParent();
    unit->energyBar = nullptr;
    unit->energyBarContent->removeFromParent();
    unit->energyBarBack->removeFromParent();
  }
  for (auto child : unit->childrenSprite) {
    child->removeFromParent();
  }
  deselect(unit);
  heroArray.eraseObject(unit);
  enemyArray.eraseObject(unit);
  heroList.eraseObject(unit);
  enemyHeroList.eraseObject(unit);
  mutualArray.eraseObject(unit);
  MovableArray.eraseObject(unit);
  bool elli = true;
  for (auto unit : heroArray) {
    if (unit->isBuilding && unit->energy > 0) {
      elli = false;
    }
    if (isBuildingExistWhenStartTheGame) {

    } else {
      if (unit->attackType != ATTACK_TYPE_NONE) {
        elli = false;
      }
    }
  }
  if (elli) {
    endGame(false);
  }

  if (unit->unitType == UNIT_ZOMBIE_ORC_AXE ||
      unit->unitType == UNIT_ZOMBIE_SWORDSMAN) {
    unit->removeFromParent();
  } else if (unit->isBuilding || unit->unitType == UNIT_CATAPULT ||
             unit->unitType == UNIT_GOBLIN_BOMB) {
    runEffect(EFFECT_EXPLODE_BIG, unit->getPosition());
    unit->removeFromParent();
  } else if (unit->unitType == UNIT_HELICOPTER) {
    unit->runAction(Sequence::create(
        EaseInOut::create(MoveBy::create(0.5f, Vec2(200, 300)), 2),
        CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
    unit->setAnchorPoint(Vec2(0, 0));
    unit->runAction(EaseInOut::create(RotateBy::create(0.5f, 720), 2));
  } else {
    std::string name = unit->getName();
    float disappearTime = 3;
    float fadeTime = 3;
    if (unit->isZombie) {
      disappearTime = 0.01f;
      fadeTime = 0.1f;
    } else {
      if (name.compare("tree") != 0) {
        if (unit->spine) {
          unit->spine->setAnimation(0, "dead", false);
          unit->spine->runAction(Sequence::create(
              DelayTime::create(disappearTime), FadeOut::create(fadeTime),
              CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)),
              NULL));
        } else {
          SpriteFrameCache *cache = SpriteFrameCache::getInstance();
          std::string imgFileName = strmake("%sDead.png", name.c_str());
          if (unit->unitType == UNIT_ORC_SPEAR) {
            imgFileName = "orcSpearDead.png";
          } else if (unit->unitType == UNIT_ORC_AXE) {
            imgFileName = "orcAxeDead.png";
          } else if (unit->unitType == UNIT_GOBLIN_BOMB) {
            imgFileName = "goblinBombDead.png";
          } else if (unit->unitType == UNIT_WORKER) {
            imgFileName = "workerDead.png";
          } else if (unit->unitType == UNIT_GOBLIN_WORKER) {
            imgFileName = "goblinDead.png";
          }
          SpriteFrame *spriteFrame = cache->getSpriteFrameByName(imgFileName);
          if (spriteFrame != nullptr && spriteFrame && unit) {
            unit->setSpriteFrame(spriteFrame);
          }
        }
      }
    }
    unit->stopAllActions();
    unit->runAction(Sequence::create(
        DelayTime::create(disappearTime), FadeOut::create(fadeTime),
        CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
    unit->setLocalZOrder(-100000);
  }
  if (!unit->isEnemy) {
    updateFoodInUse();
  }
  checkClearGame();
}

void GameScene::removeIncompleteBuilding(EnemyBase *building) {
  if (building == nullptr) {
    return;
  }
  setOccupy(getPositionFromTileCoordinate(building->buildingStartCoordinate.x,
                                          building->buildingStartCoordinate.y),
            building->buildingOccupySize.width,
            building->buildingOccupySize.height, false);
  deselect(building);
  heroArray.eraseObject(building);
  MovableArray.eraseObject(building);
  building->removeFromParent();
  building->release();
  resetPathState();
  updateMiniMapForNonMoving();
}

void GameScene::removeEnemy(EnemyBase *spt) {
  Movable *drop;
  for (int i = 0; i < chasingMissileArray.size(); i++) {
    drop = (Movable *)chasingMissileArray.at(i);
    if (drop->target == spt) {
      drop->target = NULL;
    }
  }
  int thisEnemyKillCount =
      UDGetInt(strmake(KEY_ENEMY_KILL_COUNT_FORMAT, spt->unitType).c_str(), 0);
  thisEnemyKillCount++;
  UDSetInt(strmake(KEY_ENEMY_KILL_COUNT_FORMAT, spt->unitType).c_str(),
           thisEnemyKillCount);
  int enemyKillCountForQuest = UDGetInt(spt->getName().c_str(), 0);
  enemyKillCountForQuest++;
  UDSetInt(spt->getName().c_str(), enemyKillCountForQuest);

  killCountForRecord++;
  GameManager::getInstance()->totalKill++;
  GameManager::getInstance()->totalScore += 100;
  spt->onDead();
  spt->stopAllActions();
  chasingMissileArray.eraseObject(spt);
  straightMovingArray.eraseObject(spt);
  enemyArray.eraseObject(spt);
  MovableArray.eraseObject(spt);
  flyingSwimingEnemyArray.eraseObject(spt);
  chasingMissileArray.eraseObject(spt);
  destructableArray.eraseObject(spt);
  inWaterArray.eraseObject(spt);
  if (fireableArray.find((FireableBase *)spt) == fireableArray.end()) {
    fireableArray.eraseObject((FireableBase *)spt);
  }
  this->runEffect(EFFECT_EXPLODE_SMALL, spt->getPosition());

  addHeroExp(0, spt->rewardExp);

  spiderArray.eraseObject(spt);
  if (spt->sptWeapon != nullptr)
    spt->sptWeapon->removeFromParent();
  spt->removeFromParentAndCleanup(true);
}

void GameScene::fixStageLayerFourTiles(TMXTiledMap *map) {
  TMXLayer *stageLayer = map->getLayer("stage");
  float y;
  for (int x = 2; x < map->getMapSize().width - 2; x++) {
    for (int rawY = 2; rawY < map->getMapSize().height - 2; rawY++) {
      y = map->getMapSize().height - rawY - 1;
      if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y)))) {
        if (compareFourTiles(0, 1, 1, 1, x, y, stageLayer)) { // top
          stageLayer->setTileGID(6, Vec2(x, y));
        } else if (compareFourTiles(1, 0, 1, 1, x, y, stageLayer)) { // left
          stageLayer->setTileGID(37, Vec2(x, y));
        } else if (compareFourTiles(1, 1, 0, 1, x, y, stageLayer)) { // right
          stageLayer->setTileGID(39, Vec2(x, y));
        } else if (compareFourTiles(1, 1, 1, 0, x, y, stageLayer)) { // bottom
          stageLayer->setTileGID(70, Vec2(x, y));
        } else if (compareFourTiles(0, 0, 1, 1, x, y, stageLayer)) { // left top
          stageLayer->setTileGID(5, Vec2(x, y));
        } else if (compareFourTiles(0, 1, 0, 1, x, y,
                                    stageLayer)) { // right top
          stageLayer->setTileGID(7, Vec2(x, y));
        } else if (compareFourTiles(1, 0, 1, 0, x, y,
                                    stageLayer)) { // left bottom
          stageLayer->setTileGID(69, Vec2(x, y));
        } else if (compareFourTiles(1, 1, 0, 0, x, y,
                                    stageLayer)) { // right bottom
          stageLayer->setTileGID(71, Vec2(x, y));
        } else if (compareFourTiles(1, 1, 1, 1, x, y, stageLayer)) {
          stageLayer->setTileGID(38, Vec2(x, y));
        }
      }
    }
  }
}

bool GameScene::compareFourTiles(int t, int l, int r, int b, int x, int y,
                                  TMXLayer *stageLayer) {
  bool result = true;
  if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y - 1))) != t) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x - 1, y))) != l) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x + 1, y))) != r) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y + 1))) != b) {
    return false;
  }
  return result;
}
int GameScene::getTileGIDAt(TMXLayer *layer, Vec2 pos) {
  if (!layer)
    return 0;
  if (pos.x < 0 || pos.y < 0) {
    return -1;
  }
  return layer->getTileGIDAt(pos);
}
void GameScene::fixStageLayerTiles(TMXTiledMap *map) {
  TMXLayer *stageLayer = map->getLayer("stage");
  float y;
  for (int x = 1; x < map->getMapSize().width - 1; x++) {
    for (int rawY = 1; rawY < map->getMapSize().height - 1; rawY++) {
      y = map->getMapSize().height - rawY - 1;
      if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y)))) {
        if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 1, x, y,
                             stageLayer)) { // center
          stageLayer->setTileGID(38, Vec2(x, y));
        } else if (compareNineTiles(0, 0, 0, 1, 0, 1, 1, 0, x, y,
                                    stageLayer)) { // right top corner
          stageLayer->setTileGID(7, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 0, x, y,
                                    stageLayer)) { // right corner
          stageLayer->setTileGID(39, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 0, 1, 0, 0, 0, 0, x, y,
                                    stageLayer)) { // right bottom corner
          stageLayer->setTileGID(71, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 0, x, y,
                                    stageLayer)) { // bottom corner
          stageLayer->setTileGID(70, Vec2(x, y));
        } else if (compareNineTiles(0, 1, 1, 0, 1, 0, 0, 0, x, y,
                                    stageLayer)) { // left bottom corner
          stageLayer->setTileGID(69, Vec2(x, y));
        } else if (compareNineTiles(0, 1, 1, 0, 1, 0, 1, 1, x, y,
                                    stageLayer)) { // left corner
          stageLayer->setTileGID(37, Vec2(x, y));
        } else if (compareNineTiles(0, 0, 0, 0, 1, 0, 1, 1, x, y,
                                    stageLayer)) { // left top corner
          stageLayer->setTileGID(5, Vec2(x, y));
        } else if (compareNineTiles(0, 0, 0, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // top corner
          stageLayer->setTileGID(6, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 0, 1, 1, x, y,
                                    stageLayer)) { // right top wall
          stageLayer->setTileGID(15, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 0, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // right wall
          stageLayer->setTileGID(47, Vec2(x, y));
        } else if (compareNineTiles(0, 1, 1, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // right bottom wall
          stageLayer->setTileGID(79, Vec2(x, y));
        } else if (compareNineTiles(1, 0, 1, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // bottom wall
          stageLayer->setTileGID(78, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 0, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // left bottom wall
          stageLayer->setTileGID(77, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 1, x, y,
                                    stageLayer)) { // left wall
          stageLayer->setTileGID(45, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 0, x, y,
                                    stageLayer)) { // left top wall
          stageLayer->setTileGID(13, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 1, x, y,
                                    stageLayer)) { // top wall
          stageLayer->setTileGID(14, Vec2(x, y));
        } else if (compareNineTiles(0, 1, 1, 0, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // right wall top oepn
          stageLayer->setTileGID(47, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 0, 1, 0, 1, 1, x, y,
                                    stageLayer)) { // right wall bottom open
          stageLayer->setTileGID(47, Vec2(x, y));
        } else if (compareNineTiles(0, 0, 1, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // bottom wall left open
          stageLayer->setTileGID(78, Vec2(x, y));
        } else if (compareNineTiles(1, 0, 0, 1, 1, 1, 1, 1, x, y,
                                    stageLayer)) { // bottom wall right open
          stageLayer->setTileGID(78, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 1, x, y,
                                    stageLayer)) { // left wall top open
          stageLayer->setTileGID(45, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 0, x, y,
                                    stageLayer)) { // left wall bottom open
          stageLayer->setTileGID(45, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 1, x, y,
                                    stageLayer)) { // top wall left open
          stageLayer->setTileGID(14, Vec2(x, y));
        } else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 0, x, y,
                                    stageLayer)) { // top wall right open
          stageLayer->setTileGID(14, Vec2(x, y));
        }
      }
    }
  }
}
bool GameScene::compareNineTiles(int lt, int t, int rt, int l, int r, int lb,
                                  int b, int rb, int x, int y,
                                  TMXLayer *stageLayer) {
  bool result = true;
  if (!isWay(getTileGIDAt(stageLayer, Vec2(x - 1, y - 1)) != lt)) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y - 1))) != t) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x + 1, y - 1))) != rt) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x - 1, y))) != l) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x + 1, y))) != r) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x - 1, y + 1))) != lb) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x, y + 1))) != b) {
    return false;
  } else if (!isWay(getTileGIDAt(stageLayer, Vec2(x + 1, y + 1))) != rb) {
    return false;
  }
  return result;
}
void GameScene::backToLabelPool(Ref *ref) {
  Label *lbl = (Label *)ref;
  lbl->removeFromParentAndCleanup(false);
}
Label *GameScene::showLabelFromPool(Node *parent, cocos2d::Vec2 pos,
                                     std::string text, int moveHeight,
                                     float delay) {
  Label *lbl = labelPool.at(labelPoolIndex);
  if (lbl->getParent()) {
    lbl->removeFromParentAndCleanup(false);
  }
  parent->addChild(lbl, 10);
  float visibleTime = 0.5f;
  float fadingTime = 1;
  lbl->setString(text);
  lbl->stopAllActions();
  lbl->setPosition(pos);
  lbl->setOpacity(0);
  lbl->runAction(Sequence::create(
      DelayTime::create(delay),
      MoveBy::create(visibleTime + fadingTime, Vec2(0, moveHeight)), nullptr));
  lbl->runAction(Sequence::create(
      DelayTime::create(delay), FadeIn::create(0.1f),
      DelayTime::create(visibleTime), FadeOut::create(fadingTime),
      CallFuncN::create(CC_CALLBACK_1(GameScene::backToLabelPool, this)),
      NULL));
  labelPoolIndex++;
  if (labelPoolIndex >= labelPoolCount) {
    labelPoolIndex = 0;
  }
  return lbl;
}
Sprite *GameScene::getLightSpin(float persistTime) {
  Sprite *shining = Sprite::create("lightSpin.png");
  shining->runAction(RotateBy::create(persistTime, persistTime * 90));
  shining->runAction(
      Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, shining)),
                       NULL));
  //    BlendFunc f = {GL_DST_COLOR, GL_DST_ALPHA};
  shining->setBlendFunc(BlendFunc::ADDITIVE);

  Sprite *shining2 = Sprite::create("lightSpin.png");
  shining2->runAction(RotateBy::create(persistTime, -persistTime * 180));
  shining2->runAction(
      Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, shining)),
                       NULL));
  //    f = {GL_DST_COLOR, GL_DST_ALPHA};
  shining2->setBlendFunc(BlendFunc::ADDITIVE);
  shining->addChild(shining2);
  shining2->setPosition(Vec2(shining->getContentSize().width / 2,
                             shining->getContentSize().height / 2));

  return shining;
}
int GameScene::getSlotForWeapon(int weapon) {
  int index = 0;
  while (true) {
    if (getWeaponAtSlot(index) == weapon) {
      return index;
    } else if (getWeaponAtSlot(index) == -1) {
      return -1;
    }
    index++;
  }
}

Sprite *GameScene::createNumberSprite(int number) {
  //	int place = 1;
  int digit = 0;
  Vector<Sprite *> sprites;
  if (number < 0)
    return NULL;
  char buf[6];
  while (true) {
    digit = number % 10;
    sprintf(buf, "%d.png", digit);
    Sprite *sptNumber = Sprite::createWithSpriteFrameName(buf);
    sprites.insert(0, sptNumber);
    number /= 10;
    if (0 >= number) {
      break;
    }
  }
  int startWidth = 0;
  int index = 0;
  Sprite *first;
  for (auto sprite : sprites) {
    if (index == 0) {
      index++;
      startWidth = sprite->getContentSize().width;
      first = sprite;
      continue;
    }
    sprite->setPosition(Vec2(startWidth + sprite->getContentSize().width / 2,
                             sprite->getContentSize().height / 2));
    startWidth += (sprite->getContentSize().width - 1);
    first->addChild(sprite);
  }
  sprites.clear();
  return first;
}

void GameScene::onjewelryBoxOpen(Ref *sender) {
  GameManager::getInstance()->showDisposableMessage(
      LanguageManager::getInstance()->getText("jewel collected").c_str(),
      GameManager::getInstance()->getHudLayer());
  Movable *jewelryBox = (Movable *)sender;
  Sprite *lightSpin = getLightSpin(5);

  Sprite *sptWeapon = Sprite::createWithSpriteFrameName(
      strmake("jewel%d.png", jewelryBox->secondTag));
  sptWeapon->setOpacity(0);
  lightSpin->setOpacity(0);
  sptWeapon->setPosition(jewelryBox->getPosition());
  lightSpin->setPosition(jewelryBox->getPosition());
  sptWeapon->runAction(FadeIn::create(0.3));
  lightSpin->runAction(FadeIn::create(0.3));
  sptWeapon->runAction(
      Sequence::create(MoveBy::create(1, Vec2(0, 18)), DelayTime::create(4),
                       FadeOut::create(0.5),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, sptWeapon)),
                       nullptr));
  lightSpin->runAction(MoveBy::create(1, Vec2(0, 18)));
  batch->addChild(sptWeapon, 1);
  this->addChild(lightSpin);
  UserDefault::getInstance()->setBoolForKey(
      strmake(KEY_JEWEL_COLLECTED_FORMAT, jewelryBox->secondTag).c_str(), true);
  // player->freezed = false;

  GameManager::getInstance()->collectedJewelIndex = jewelryBox->secondTag;
}
void GameScene::openWeapon(Ref *sender) {
  Movable *suitcase = (Movable *)sender;
  Sprite *lightSpin = getLightSpin(5);
  GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
  int var = rand() % (WEAPON_NO_MORE / 4);
  if (suitcase->secondTag == 3) {
    var++;
  }
  int weapon = 1 + (WEAPON_NO_MORE / 4) * suitcase->secondTag + var;
  if (weapon >= WEAPON_NO_MORE) {
    weapon = WEAPON_NO_MORE - 1;
  } else if (weapon < 1) {
    weapon = 1;
  }

  // int weapon = WEAPON_LIGHTNING_GUN; test
  Sprite *sptWeapon = Sprite::createWithSpriteFrameName(
      GameManager::getInstance()->getWeaponImageFileName(weapon));
  sptWeapon->setOpacity(0);
  lightSpin->setOpacity(0);
  sptWeapon->setPosition(suitcase->getPosition());
  lightSpin->setPosition(suitcase->getPosition());
  sptWeapon->runAction(FadeIn::create(0.3));
  lightSpin->runAction(FadeIn::create(0.3));
  sptWeapon->runAction(
      Sequence::create(MoveBy::create(1, Vec2(0, 18)), DelayTime::create(4),
                       FadeOut::create(0.5),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, sptWeapon)),
                       nullptr));
  lightSpin->runAction(MoveBy::create(1, Vec2(0, 18)));
  batch->addChild(sptWeapon, 1);
  this->addChild(lightSpin);

  if (getSlotForWeapon(weapon) < 0) {
    addWeapon(weapon);
  } else {
    recharge(getSlotForWeapon(weapon));
  }
}
void GameScene::recharge(int slot) {
  int weapon = getWeaponAtSlot(slot);
  if (currentSlot == slot) {
    totalBulletCount = getMaxTotalBulletCount(weapon);
    loadedBulletCount = getMaxLoadedBulletCount(weapon);
  } else {
    setTotalBulletCountAtSlot(slot, getMaxTotalBulletCount(weapon));
    setLoadedBulletCountAtSlot(slot, getMaxLoadedBulletCount(weapon));
  }

  changeWeapon(slot);
}
void GameScene::addWeapon(int weapon) {
  int index = 0;
  while (getWeaponAtSlot(index) >= 0) {
    index++;
  }
  setWeaponAtSlot(index, weapon);
  setWeaponAtSlot(index + 1, -1);
  setLoadedBulletCountAtSlot(index, getMaxLoadedBulletCount(weapon));
  setTotalBulletCountAtSlot(index, getMaxTotalBulletCount(weapon));
  changeWeapon(index);
}
int GameScene::getTotalBulletCountAtSlot(int slot) {
  return UserDefault::getInstance()->getIntegerForKey(
      strmake(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot).c_str(), 0);
}
void GameScene::setTotalBulletCountAtSlot(int slot, int count) {
  UserDefault::getInstance()->setIntegerForKey(
      strmake(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot).c_str(), count);
}
int GameScene::getLoadedBulletCountAtSlot(int slot) {
  return UserDefault::getInstance()->getIntegerForKey(
      strmake(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot).c_str(), 0);
}
void GameScene::setLoadedBulletCountAtSlot(int slot, int count) {
  UserDefault::getInstance()->setIntegerForKey(
      strmake(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot).c_str(), count);
}

void GameScene::changeWeapon(int slot) {
  if (isReloading) {
    return;
  }
  isFired = true;
  if (getWeaponAtSlot(slot) < 0) {
    return;
  }
  if (totalBulletCount >= 0) {
    setTotalBulletCountAtSlot(currentSlot, totalBulletCount);
  }
  if (loadedBulletCount >= 0) {
    setLoadedBulletCountAtSlot(currentSlot, loadedBulletCount);
  }

  currentSlot = slot;
  int weapon = getWeaponAtSlot(slot);

  totalBulletCountMax = getMaxTotalBulletCount(weapon);
  totalBulletCount = getTotalBulletCountAtSlot(slot);
  loadedBulletCount = getLoadedBulletCountAtSlot(slot);
  loadedBulletCountMax = getMaxLoadedBulletCount(weapon);
  GameManager::getInstance()->getHudLayer()->setWeapon(weapon);
  GameManager::getInstance()->getHudLayer()->setBulletCount(
      loadedBulletCount, loadedBulletCountMax);
  GameManager::getInstance()->getHudLayer()->setTotalBulletCount(
      totalBulletCount, totalBulletCountMax);
}
void GameScene::removeLaser() {
  if (laser != nullptr) {
    laser->removeFromParent();
    laser = nullptr;
  }
}
int GameScene::getWeaponAtSlot(int slot) {
  return UserDefault::getInstance()->getIntegerForKey(
      strmake("WeaponInSlot%d", slot).c_str(), -1);
}
void GameScene::setWeaponAtSlot(int slot, int weapon) {
  UserDefault::getInstance()->setIntegerForKey(
      strmake("WeaponInSlot%d", slot).c_str(), weapon);
}
Sprite *GameScene::getShining(float delayTime) {
  Sprite *shining = Sprite::createWithSpriteFrameName("shining0.png");
  Animation *animation = AnimationCache::getInstance()->getAnimation("shining");
  Animate *animate = Animate::create(animation);
  shining->runAction(Sequence::create(
      DelayTime::create(delayTime), MoveBy::create(0.7, Vec2(6, 6)), nullptr));
  shining->runAction(Sequence::create(DelayTime::create(delayTime),
                                      RotateBy::create(0.7, 250), nullptr));
  shining->runAction(
      Sequence::create(DelayTime::create(delayTime), animate,
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, shining)),
                       NULL));

  return shining;
}
void GameScene::npcUpdate(float dt) {}
void GameScene::coinUpdate(float dt) { // updateCoin
}
void GameScene::teleportLaterForThemeThree(float dt) {}
void GameScene::teleportLater(float dt) {}

void GameScene::jumpBySpring(float dt) {}
void GameScene::showLaser(cocos2d::Vec2 srcPoint, cocos2d::Vec2 tarPoint) {
  Sprite *spt = Sprite::createWithSpriteFrameName("laser0.png");
  batch->addChild(spt, 5);
  //    spt->setScale(0.7f);
  spt->setAnchorPoint(Vec2(0, 0.5f));
  spt->setPosition(srcPoint);
  spt->setRotation(-GM->getAngle(srcPoint, tarPoint) + 180);
  float distance = GM->getDistance(srcPoint, tarPoint);
  spt->setScaleX(distance / spt->getContentSize().width);
  GM->runAnimation(spt, "laser", false);
  spt->runAction(
      Sequence::create(DelayTime::create(0.07 * 7), SPT_REMOVE_FUNC, nullptr));
}
void GameScene::runEffect(int effect, Vec2 point, float angle) {
  float duration = 0.2;
  float baseDuration = 0.08;
  duration = baseDuration * 2 + baseDuration * (rand() % 3 + 1);
  if (effect == EFFECT_EXPLODE_SMALL) {
    duration = 0.15;
    int radius = 20;
    for (int i = 0; i < 10; i++) {
      Sprite *explode = Sprite::createWithSpriteFrameName(
          strmake("snowFlake%d.png", rand() % 4));
      batch->addChild(explode, 5);
      explode->setScale(1);
      explode->runAction(Sequence::create(
          MoveBy::create(duration, Vec2(rand() % (radius * 2) - radius,
                                        rand() % (radius * 2) - radius)),
          CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)),
          NULL));
      explode->runAction(Sequence::create(DelayTime::create(duration / 2),
                                          FadeOut::create(duration / 2),
                                          nullptr));
      explode->runAction(RotateBy::create(duration, rand() % 260 - 130));
      explode->setPosition(point);
    }

    duration = 0.09f;
    Sprite *spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
    batch->addChild(spt, 5);
    spt->setColor(Color3B::BLACK);
    spt->runAction(Sequence::create(TintTo::create(0.07f, 255, 255, 255),
                                    DelayTime::create(duration),
                                    SPT_REMOVE_FUNC, NULL));
    spt->setPosition(point);

    radius = 5;
    duration = 1.0f;
    for (int i = 0; i < 15; i++) {
      Sprite *explode = Sprite::createWithSpriteFrameName(
          strmake("snowFlake%d.png", rand() % 4));
      batch->addChild(explode, 5);
      explode->setColor(Color3B(90, 90, 90));
      explode->setOpacity(0);
      explode->runAction(Sequence::create(
          MoveBy::create(duration, Vec2(0, 10)),
          CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)),
          NULL));
      explode->runAction(Sequence::create(
          DelayTime::create(duration / 4), FadeTo::create(duration / 4, 100),
          DelayTime::create(duration / 4), FadeOut::create(duration / 4),
          nullptr));
      //            explode->setScale(0.8);
      explode->runAction(RotateBy::create(duration, rand() % 260 + 250));
      explode->setPosition(point + Vec2(rand() % (radius * 2) - radius,
                                        rand() % (radius * 2) - radius));
    }
  } else if (effect == EFFECT_EXPLODE_MIDDLE) {
    Sprite *spt =
        Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
    batch->addChild(spt, 5);
    spt->setScale(0.7f);
    spt->setPosition(point);
    GM->runAnimation(spt, "cartoonyExplosion", false);
    spt->runAction(Sequence::create(DelayTime::create(0.04 * 14),
                                    SPT_REMOVE_FUNC, nullptr));
  } else if (effect == EFFECT_HIT_WITH_CIRCLE ||
             effect == EFFECT_HIT_WITH_CIRCLE_ON_GROUND) {
    Sprite *spt = Sprite::createWithSpriteFrameName("hitWithCircle0.png");
    batch->addChild(spt, 5);
    spt->setPosition(point);
    GM->runAnimation(spt, "hitWithCircle", false);
    spt->runAction(Sequence::create(DelayTime::create(0.06 * 7),
                                    SPT_REMOVE_FUNC, nullptr));
    if (effect == EFFECT_HIT_WITH_CIRCLE_ON_GROUND) {
      spt->setRotation(-90);
    }
  } else if (effect == EFFECT_PURPLE_SLASH) {
    for (int i = 0; i < 10; i++) {
      Sprite *spt = Sprite::createWithSpriteFrameName("purpleBall.png");
      batch->addChild(spt, 5);
      spt->setPosition(point + Vec2(rand() % 40 - 20, 60 - i * 15));
      spt->setColor(Color3B(153, 105, 241));
      spt->setOpacity(0);
      spt->setScale((rand() % 5) * 0.1f + 0.5f);
      float delay = i * 0.03f;
      spt->runAction(Sequence::create(DelayTime::create(delay),
                                      DelayTime::create(0.7f),
                                      ScaleBy::create(0.8f, 1.5f), nullptr));
      spt->runAction(Sequence::create(
          DelayTime::create(delay), FadeIn::create(0), DelayTime::create(1.0f),
          TintTo::create(0.5f, Color3B(85, 36, 191)), nullptr));
      spt->runAction(Sequence::create(
          DelayTime::create(delay),
          EaseOut::create(MoveBy::create(0.5f, Vec2(0, -20)), 2),
          EaseIn::create(MoveBy::create(1, Vec2(0, 10)), 2), nullptr));
      spt->runAction(
          Sequence::create(DelayTime::create(delay), DelayTime::create(1),
                           FadeOut::create(0.5f), SPT_REMOVE_FUNC, nullptr));
    }
  } else if (effect == EFFECT_WATER_SPLASH) {
    Sprite *spt = Sprite::createWithSpriteFrameName("waterSplash0.png");
    batch->addChild(spt, 5);
    spt->setPosition(point);
    spt->setScale(0.8f);
    spt->runAction(ScaleTo::create(0.1f, 1));
    spt->runAction(
        Sequence::create(RotateBy::create(0.1f, 30), SPT_REMOVE_FUNC, NULL));
    spt = Sprite::createWithSpriteFrameName("waterSplash1.png");
    batch->addChild(spt, 5);
    spt->setOpacity(0);
    spt->setPosition(point);
    spt->setScale(0.8f);
    spt->runAction(Sequence::create(
        DelayTime::create(0.09f), FadeIn::create(0.01f),
        DelayTime::create(0.1f), FadeOut::create(0.05f), NULL));
    spt->runAction(Sequence::create(DelayTime::create(0.1f),
                                    ScaleTo::create(0.15f, 1), SPT_REMOVE_FUNC,
                                    NULL));

    float delay = 0.0f;
    float radius = 100;
    int sptCount = 10;
    for (int i = 0; i < sptCount; i++) {
      spt = Sprite::createWithSpriteFrameName("waterSplash0.png");
      batch->addChild(spt, 5);
      spt->setPosition(point);
      spt->setOpacity(0);
      spt->setScale(0.2f);
      float radian = (i * 360 / sptCount) * 3.14f / 180;
      spt->runAction(Sequence::create(
          DelayTime::create(delay), FadeIn::create(0.03f),
          DelayTime::create(0.1f), FadeOut::create(0.07f), nullptr));
      spt->runAction(
          Sequence::create(DelayTime::create(delay),
                           MoveBy::create(0.2f, Vec2(cos(radian) * radius,
                                                     sin(radian) * radius)),
                           SPT_REMOVE_FUNC, nullptr));
    }

  } else if (effect == EFFECT_FIREBALL_EXPLOSION) {
    for (int i = 0; i < 7; i++) {
      Sprite *spt = Sprite::createWithSpriteFrameName("groundExplosion0.png");
      batch->addChild(spt, 5);
      spt->setPosition(point + Vec2(rand() % 200 - 100, rand() % 200 - 100) *
                                   imageScale);
      GM->runAnimation(spt, "fireExplosion", false);
      spt->runAction(Sequence::create(DelayTime::create(0.07 * 8),
                                      SPT_REMOVE_FUNC, nullptr));
    }
  } else if (effect == EFFECT_EXPLODE_BIG) {
    GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
    Sprite *spt = Sprite::createWithSpriteFrameName("cartoonyExplosion0.png");
    batch->addChild(spt, 5);
    spt->setPosition(point);
    GM->runAnimation(spt, "cartoonyExplosionBig", false);
    spt->runAction(Sequence::create(DelayTime::create(0.04 * 14),
                                    SPT_REMOVE_FUNC, nullptr));
  } else if (effect == EFFECT_HIT) {
    Sprite *spt = Sprite::createWithSpriteFrameName("cartoonyHitEffect0.png");
    batch->addChild(spt, 5);
    spt->setPosition(point);
    spt->setRotation(angle);
    GM->runAnimation(spt, "cartoonyHitEffect", false);
    spt->runAction(Sequence::create(DelayTime::create(0.06 * 4),
                                    SPT_REMOVE_FUNC, nullptr));
  } else if (effect == EFFECT_SMOKE) {
    float duration = 0.5;
    Sprite *spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
    batch->addChild(spt, 100);
    spt->setPosition(point + Vec2(4 - rand() % 8, 4));
    spt->setScale((rand() % 5 + 5) * 0.1f);
    spt->setOpacity(100);
    //        spt->setRotation(rand()%360);
    int num = 150 + rand() % 100;
    spt->setColor(Color3B(num, num, num));
    spt->runAction(Sequence::create(
        EaseIn::create(MoveBy::create(duration, Vec2(0, 7)), 2),
        CallFuncN::create(CC_CALLBACK_1(GameScene::enemyBaseMoveDone, this)),
        NULL));
    spt->runAction(
        EaseOut::create(MoveBy::create(duration, Vec2(8 - rand() % 16, 0)), 4));
    spt->runAction(FadeOut::create(duration));
    spt->runAction(TintTo::create(duration, 0, 0, 0));
    spt->runAction(ScaleBy::create(duration, 1.5f));

    spt->runAction(RotateBy::create(duration, 160));
  } else if (effect == EFFECT_TWINKLE) {
    Sprite *spt = Sprite::createWithSpriteFrameName("twinkle0.png");
    batch->addChild(spt, 11);
    Animation *animation =
        AnimationCache::getInstance()->getAnimation("twinkle");
    Animate *animate = Animate::create(animation);
    spt->runAction(Sequence::create(
        animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)),
        NULL));
    spt->setPosition(point);
  } else if (effect == EFFECT_LIGHTNING) {
    Sprite *spt = Sprite::createWithSpriteFrameName("lightning0.png");
    spt->setAnchorPoint(Vec2(0.5, 0));
    batch->addChild(spt, 11);
    Animation *animation =
        AnimationCache::getInstance()->getAnimation("effectLightning");
    Animate *animate = Animate::create(animation);
    spt->runAction(Sequence::create(
        animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)),
        NULL));
    spt->setPosition(point + Vec2(0, -49));

    spt = Sprite::create("whiteBigCircle.png");
    addChild(spt);
    spt->setPosition(point + Vec2(0, 100));
    spt->setOpacity(0);
    spt->setColor(Color3B::BLACK);
    spt->setScale(4);
    spt->runAction(Sequence::create(
        DelayTime::create(0.06f * 5), FadeTo::create(0, 60),
        DelayTime::create(0.1f), FadeOut::create(0.3f),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));

    spt = Sprite::create("whiteBigCircle.png");
    addChild(spt);
    spt->setPosition(point); // + Vec2(0, 49));
    spt->setOpacity(0);
    spt->setBlendFunc(BlendFunc::ADDITIVE);
    spt->setColor(Color3B(80, 250, 255));
    spt->runAction(Sequence::create(
        DelayTime::create(0.06f * 4), FadeTo::create(0, 120),
        ScaleTo::create(0.1f, 2), FadeOut::create(0.2f),
        CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));

    for (int i = 0; i < 5; i++) {
      spt = Sprite::create("whiteBigCircle.png");
      addChild(spt);
      spt->setPosition(point); // + Vec2(0, 49));
      spt->setOpacity(0);
      spt->setScale(0.2f);
      spt->setBlendFunc(BlendFunc::ADDITIVE);
      spt->setColor(Color3B(0, 240, 250));
      int distance = 180;
      float angle = 3.14f * (rand() % 360) / 180;
      spt->runAction(Sequence::create(DelayTime::create(0.06f * 4 + 0.1f),
                                      FadeOut::create(0.2f), NULL));
      spt->runAction(Sequence::create(
          DelayTime::create(0.06f * 4), FadeTo::create(0, 120),
          MoveBy::create(0.2f,
                         Vec2(distance * cos(angle), distance * sin(angle))),
          CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)),
          NULL));
    }

    spt = Sprite::createWithSpriteFrameName("lightning5.png");
    spt->setOpacity(0);
    addChild(spt, WORLD->batch->getLocalZOrder() + 1);
    spt->setAnchorPoint(Vec2(0.5, 0));
    spt->setPosition(point + Vec2(0, -49));
    spt->runAction(Sequence::create(
        DelayTime::create(0.06f * 5), FadeTo::create(0, 30),
        DelayTime::create(0.1f), FadeOut::create(0.3f), SPT_REMOVE_FUNC, NULL));
  } else if (effect == EFFECT_FIREBOMB) {
    Sprite *spt = Sprite::createWithSpriteFrameName("explosionDirection0.png");
    batch->addChild(spt, 11);
    Animation *animation =
        AnimationCache::getInstance()->getAnimation("firebomb");
    Animate *animate = Animate::create(animation);
    spt->runAction(Sequence::create(animate, SPT_REMOVE_FUNC, NULL));
    spt->setPosition(point);
    spt->setRotation(angle);
  } else if (effect == EFFECT_BLUE_TEETH || effect == EFFECT_RED_TEETH) {
    for (int i = 0; i < 2; i++) {
      Sprite *spt = Sprite::createWithSpriteFrameName("whiteSharpTeeth.png");
      batch->addChild(spt, 5);
      spt->setPosition(point);
      if (effect == EFFECT_BLUE_TEETH) {
        spt->setColor(Color3B(0, 240, 255));
      } else if (effect == EFFECT_RED_TEETH) {
        spt->setColor(Color3B(235, 20, 25));
      }
      spt->setScale(2);
      spt->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1)));
      spt->setOpacity(100);
      spt->runAction(FadeIn::create(0.1f));
      spt->runAction(Sequence::create(DelayTime::create(0.3),
                                      FadeOut::create(0.6f), SPT_REMOVE_FUNC,
                                      nullptr));
      spt->setAnchorPoint(Vec2(0.5, 0));
      if (i == 0) {
        spt->setRotation(180);
      } else {
      }
    }
  } else if (effect == EFFECT_BLUE_SLASH) {
    Sprite *spt = Sprite::createWithSpriteFrameName("blueSlash0.png");
    batch->addChild(spt, 5);
    GM->runAnimation(spt, "blueslash", false, true);
    spt->setPosition(point);
    spt->setRotation(angle);
  } else if (effect == EFFECT_FIRE_SLASH) {
    Sprite *spt = Sprite::createWithSpriteFrameName("fireSlash0.png");
    batch->addChild(spt, 5);
    GM->runAnimation(spt, "fireSlash", false, true);
    spt->setPosition(point);
    spt->setRotation(angle);
  } else if (effect == EFFECT_GREEN_HIT) {
    Sprite *spt = Sprite::createWithSpriteFrameName("greenHit0.png");
    batch->addChild(spt, 5);
    GM->runAnimation(spt, "greenHit", false, true);
    spt->setPosition(point);
    spt->setRotation(angle);
    spt->runAction(ScaleTo::create(0.07f * 6, 3));
    //        spt->runAction(Sequence::create(DelayTime::create(0.18f),
    //        FadeOut::create(0.07f*2), nullptr));
    GM->showSpriteExplosion(batch, "leaf.png", point, 7, 100, 0.7f);
  } else if (effect == EFFECT_HEAL) {
    Sprite *spt = Sprite::createWithSpriteFrameName("yellowBall.png");
    batch->addChild(spt, 5);
    spt->runAction(
        Sequence::create(FadeIn::create(0.18f), DelayTime::create(0.3f),
                         FadeOut::create(0.14f), SPT_REMOVE_FUNC, NULL));
    float widthScale = 5;
    spt->setScale(widthScale, widthScale * 0.3f);
    spt->setPosition(point);
    for (int i = 0; i < 8; i++) {
      float delayTime = (rand() % 3) * 0.01f;
      spt = Sprite::createWithSpriteFrameName("yellowBall.png");
      spt->setOpacity(0);
      spt->runAction(Sequence::create(DelayTime::create(0.18f + delayTime),
                                      FadeOut::create(0.14f), SPT_REMOVE_FUNC,
                                      NULL));
      spt->runAction(
          Sequence::create(DelayTime::create(delayTime), FadeIn::create(0.05f),
                           MoveBy::create(0.3, Vec2(0, 70)), nullptr));
      float scaleX = 0.3f + (rand() % 6) * 0.1f;
      spt->setScale(scaleX, scaleX * 2);
      spt->runAction(
          Sequence::create(DelayTime::create(delayTime),
                           ScaleTo::create(0.1f, spt->getScaleX() * 1.4f,
                                           spt->getScaleY() * 3.4f),
                           NULL));
      batch->addChild(spt, 5);
      int diff = (int)(50 / getScaleX());
      spt->setPosition(point + Vec2(diff - rand() % (diff * 2),
                                    rand() % (int)(30 / getScaleY())));
    }
    GM->showSpriteExplosion(batch, "yellowBall.png", point, 7, 100, 0.3f);
  }
}
void GameScene::runEffect(int effect, Vec2 point) {
  runEffect(effect, point, 0);
}
void GameScene::healHeroNearPoint(Movable *healer, Vec2 point, int hp) {
  if (healer->alliSide == WHICH_SIDE_HERO) {
    for (auto unit : heroArray) {
      if (unit->energy < unit->maxEnergy &&
          unit->getPosition().getDistanceSq(point) < 100000) {
        unit->energy += hp;
        if (unit->energy > unit->maxEnergy) {
          unit->energy = unit->maxEnergy;
        }
        runEffect(EFFECT_HEAL, unit->getPosition());
        unit->updateEnergy();
        break;
      }
    }
  } else {
    for (auto unit : enemyArray) {
      if (unit->energy < unit->maxEnergy &&
          unit->getPosition().getDistanceSq(point) < 100000) {
        unit->energy += hp;
        if (unit->energy > unit->maxEnergy) {
          unit->energy = unit->maxEnergy;
        }
        runEffect(EFFECT_HEAL, unit->getPosition());
        unit->updateEnergy();
        break;
      }
    }
  }
}

void GameScene::healHeroesNearPoint(Movable *healer, Vec2 point, int hp,
                                     int heroCount) {
  int index = 0;
  if (healer->alliSide == WHICH_SIDE_HERO) {
    for (auto unit : heroArray) {
      if (unit->energy < unit->maxEnergy &&
          unit->getPosition().getDistanceSq(point) < 100000) {
        unit->energy += hp;
        if (unit->energy > unit->maxEnergy) {
          unit->energy = unit->maxEnergy;
        }
        runEffect(EFFECT_HEAL, unit->getPosition());
        unit->updateEnergy();
        index++;
        if (index >= heroCount) {
          break;
        }
      }
    }
  } else {
    for (auto unit : enemyArray) {
      if (unit->energy < unit->maxEnergy &&
          unit->getPosition().getDistanceSq(point) < 100000) {
        unit->energy += hp;
        if (unit->energy > unit->maxEnergy) {
          unit->energy = unit->maxEnergy;
        }
        runEffect(EFFECT_HEAL, unit->getPosition());
        unit->updateEnergy();
        index++;
        if (index >= heroCount) {
          break;
        }
      }
    }
  }
}
/*
void GameScene::addGlowEffect(Sprite* sprite,const Color3B& colour, const
cocos2d::Size& size)
{
    Vec2 pos = Vec2(sprite->getContentSize().width / 2,
                      sprite->getContentSize().height / 2);

    Sprite* glowSprite = Sprite::createWithSpriteFrameName("lizardStand.png");

    glowSprite->setColor(colour);
    glowSprite->setPosition(pos);
    glowSprite->setRotation(sprite->getRotation());

    ccBlendFunc f = {GL_ONE, GL_ONE};
    glowSprite->setBlendFunc(f);
    sprite->addChild(glowSprite, -1);

    // Run some animation which scales a bit the glow
    ScaleTo* scale1= ScaleTo::create(0.9f, 4);
    ScaleTo* scale2 = ScaleTo::create(0.9f, 3.75f);

    glowSprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(scale1,
scale2, NULL)));
}*/
void GameScene::powerTestSchedule(float dt) {
  powerTestTime += dt;
  if (powerTestTime > 10) {
    powerTestTime = 0;
    totalPower = 0;
  }
  powerPerSec = totalPower / powerTestTime;
  CCLOG("power/sec: %f", powerPerSec);
}
Sprite *GameScene::getLight() {
  Sprite *sptLight = Sprite::create("whiteBigCircle.png");
  //    BlendFunc f = {GL_DST_COLOR, GL_ONE};
  sptLight->setBlendFunc(BlendFunc::ADDITIVE);
  sptLight->setOpacity(255);
  sptLight->setColor(Color3B(255, 180, 0));
  sptLight->runAction(RepeatForever::create(Sequence::create(
      ScaleTo::create(0.0, 0.9), DelayTime::create(0.3),
      ScaleTo::create(0.0, 1), DelayTime::create(0.3), nullptr)));
  return sptLight;
}
void GameScene::setStageCoverOpacity(int opacity) {
  for (int i = 0; i < theMap->getMapSize().width; i++) {
    for (int j = 0; j < theMap->getMapSize().height; j++) {
      Sprite *spt = stageCover->getTileAt(Vec2(i, j));
      if (spt) {
        spt->setOpacity(opacity);
      }
    }
  }
  if (opacity < 255) {
    stageCover->setLocalZOrder(100);
  } else {
    stageCover->setLocalZOrder(1);
  }
}
void GameScene::setLayerTag(TMXTiledMap *map) {
  if (map == nullptr) {
    return;
  }
  if (auto *l = map->getLayer("unit"))
    l->setTag(TAG_UNIT_LAYER);
  if (auto *l = map->getLayer("stage"))
    l->setTag(TAG_STAGE_LAYER);
  if (map->getLayer("stage_high")) {
    map->getLayer("stage_high")->setTag(TAG_HIGH_STAGE_LAYER);
  }
  //    map->getLayer("back")->setTag(TAG_BACK_LAYER);
  //    map->getLayer("fore")->setTag(TAG_FORE_LAYER);
  if (map->getLayer("deco")) {
    map->getLayer("deco")->setTag(TAG_DECO_LAYER);
  }

  if (map->getLayer("stage_cover")) {
    stageCover = map->getLayer("stage_cover");

    stageCover->removeFromParentAndCleanup(false);
    this->addChild(stageCover, 1);
    stageCover->setPosition(stageCover->getPosition() + theMap->getPosition());
  }
}
void GameScene::setEntireMap(int stage) {
  //    Sprite* sptRect = Sprite::create("res/258_gray_rect.png");
  //    this->addChild(sptRect);
  //    sptRect->setPosition(Vec2(128, 128));
  //    bool shouldBeNodeGrid = false;
  // Every mode now loads from a MapEditor-authored JSON map instead of a
  // .tmx file (CartoonCraft1 leftovers, no longer used) - createTMXFromEditorJsonFile
  // falls back to a blank grass map if the scene's json hasn't been authored
  // yet, so this is safe even before every mode has a real map.
  std::string editorJsonPath;
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
    editorJsonPath = "heroesArena_0.json";
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->nextScene == STAGE_CREDIT) {
    editorJsonPath = "springMountain_0.json";
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->nextScene == STAGE_TITLE) {
    editorJsonPath = "title_0.json";
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->nextScene == STAGE_SINGLEPLAY) {
    editorJsonPath = strmake("battleSingle%d_0.json", GM->singlePlayStageIndex);
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->currentStageIndex == STAGE_LOBBY ||
             GM->nextScene == STAGE_RAID) {
    editorJsonPath = "battleLobby_0.json";
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->nextScene == STAGE_FIELD) {
    if (stage == STAGE_CUSTOM) {
      editorJsonPath = FileUtils::getInstance()->getWritablePath() + "stage_70_templete_0.json";
    } else if (GM->isColosseum) {
      editorJsonPath = StringUtils::format("colosseum%d_0.json", stage);
      log("sm map %d ", stage);
    } else {
      editorJsonPath = StringUtils::format("stage%d_0.json", stage);
    }
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
  } else if (GM->nextScene == STAGE_INTRO) {
    editorJsonPath = StringUtils::format("stage%dIntro_0.json", stage);
    theMap = createTMXFromEditorJsonFile(editorJsonPath);
    isGameOver = true;
    if (stage == 0) {
      showNPCEvent(0);
    } else if (stage == 12) {
      showNPCEvent(1);
    }
  }

  //    theMap->setVisible(false); // test
  mapSize = theMap->getMapSize();
  mapWidth = mapSize.width * TILE_SIZE;
  mapHeight = mapSize.height * TILE_SIZE;
  GM->astar->setMap(mapSize.width, mapSize.height);
  GM->waterAstar->setMap(mapSize.width, mapSize.height);
  // fog setting
  fogMapSize = cocos2d::Size(mapWidth / FOG_SIZE, mapHeight / FOG_SIZE);
  for (int j = 0; j < mapHeight; j += FOG_SIZE) {
    for (int i = 0; i < mapWidth; i += FOG_SIZE) {
      Fog *fog = Fog::create();
      fogArray.pushBack(fog);
      batch->addChild(fog, 100);
      fog->coordinate = Vec2(i, j);
      fog->setPosition(Vec2(i + FOG_SIZE / 2, j + FOG_SIZE / 2));

      if (GM->nextScene != STAGE_FIELD) {
        fog->setPosition(Vec2(-500, -500));
      }
    }
  }

  setLayerTag(theMap);
  setStage(theMap);
  if (!editorJsonPath.empty()) {
    spawnObjectsFromEditorJsonFile(editorJsonPath);
    loadTriggersFromEditorJsonFile(editorJsonPath);
  }
  int index = 0;
  for (auto fog : fogArray) {
    int x = index % (int)fogMapSize.width;
    int y = index / (int)fogMapSize.width;
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if (i == 0 && j == 0) {
          continue;
        }
        int adjacentX = x + i;
        int adjacentY = y + j;
        if (adjacentX < 0 || adjacentX >= fogMapSize.width || adjacentY < 0 ||
            adjacentY >= fogMapSize.height) {
          continue;
        }
        fog->adjacentFogs.pushBack(
            fogArray.at(adjacentX + adjacentY * (int)fogMapSize.width));
      }
    }
    for (int i = -1; i < 2; i++) {
      int farX = x + i;
      if (farX < 0 || farX >= fogMapSize.width) {
        continue;
      }
      int farY = y + 2;
      if (farY >= 0 && farY < fogMapSize.height) {
        fog->farFogs.pushBack(fogArray.at(farX + farY * (int)fogMapSize.width));
      }
      farY = y - 2;
      if (farY >= 0 && farY < fogMapSize.height) {
        fog->farFogs.pushBack(fogArray.at(farX + farY * (int)fogMapSize.width));
      }
    }
    for (int i = -1; i < 2; i++) {
      int farY = y + i;
      if (farY < 0 || farY >= fogMapSize.height) {
        continue;
      }
      int farX = x + 2;
      if (farX >= 0 && farX < fogMapSize.width) {
        fog->farFogs.pushBack(fogArray.at(farX + farY * (int)fogMapSize.width));
      }
      farX = x - 2;
      if (farX >= 0 && farX < fogMapSize.width) {
        fog->farFogs.pushBack(fogArray.at(farX + farY * (int)fogMapSize.width));
      }
    }
    index++;
  }
  if (GM->currentStageIndex != STAGE_LOBBY) {
    // minimap setting
    drawMiniMapFrame = DrawNode::create();
    HUD->addChild(drawMiniMapFrame, 100);
    drawMiniMapForNonMoving = DrawNode::create();
    HUD->addChild(drawMiniMapForNonMoving, 101);
    drawMiniMapForMoving = DrawNode::create();
    HUD->addChild(drawMiniMapForMoving, 102);
    log("size: %f", size.width);
    int offsetX = 120;
    if (size.height / size.width < 700.0f / 1334.0f) {
      //            offsetX = 100;
    }
    miniMapFrameWidth = 400;
    miniMapFrameHeight = 400;
    int offset = 20;
    miniMapStartPos = cocos2d::Vec2(offsetX + 20, 20);
    drawMiniMapFrame->setPosition(miniMapStartPos);
    drawMiniMapForNonMoving->setPosition(miniMapStartPos);
    drawMiniMapForMoving->setPosition(miniMapStartPos);

    ImageView *img = ImageView::create("uiBox.png");
    HUD->addChild(img, 99);
    if (gameMode != GAME_MODE_PVP6 && gameMode != GAME_MODE_PVP12) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
      HUD->initShortcuts(miniMapStartPos + Vec2(miniMapFrameWidth, 0));
#endif
    }

    img->setName("miniMapBack");
    img->setScale9Enabled(true);
    img->setContentSize(cocos2d::Size(miniMapFrameWidth + offset * 2 + 5,
                                      miniMapFrameHeight + offset * 2 + 5));
    img->setAnchorPoint(Vec2(0, 0));
    img->setPosition(miniMapStartPos - Vec2(offset, offset));
    if (GM->nextScene == STAGE_INTRO) {
      img->setVisible(false);
      drawMiniMapFrame->setVisible(false);
      drawMiniMapForNonMoving->setVisible(false);
      drawMiniMapForMoving->setVisible(false);
    }
    if (mapSize.width / mapSize.height <
        miniMapFrameWidth / miniMapFrameHeight) { // fix height
      miniMapScale = miniMapFrameHeight / (mapSize.height * TILE_SIZE);
      miniMapDrawStartPos = Vec2(
          miniMapFrameWidth / 2 - mapSize.width * TILE_SIZE * miniMapScale / 2,
          0);
      miniMapHeight = miniMapFrameHeight;
      miniMapWidth = miniMapHeight * mapSize.width / mapSize.height;
    } else { // fix width
      miniMapScale = miniMapFrameWidth / (mapSize.width * TILE_SIZE);
      miniMapDrawStartPos =
          Vec2(0, miniMapFrameHeight / 2 -
                      mapSize.height * TILE_SIZE * miniMapScale / 2);
      miniMapWidth = miniMapFrameWidth;
      miniMapHeight = miniMapWidth * mapSize.height / mapSize.width;
    }
    miniMapBit = miniMapScale * TILE_SIZE;

    // Terrain layer: drawn once at map load, never cleared (static VBO).
    // Uses the same position/draw formula as drawMiniMapForNonMoving so
    // alignment is guaranteed correct.
    drawMiniMapTerrain = DrawNode::create();
    drawMiniMapTerrain->setPosition(miniMapStartPos);
    HUD->addChild(drawMiniMapTerrain, 100); // ZOrder 100, added after drawMiniMapFrame → renders on top
    for (int ti = 0; ti < (int)mapSize.width; ti++) {
      for (int tj = 0; tj < (int)mapSize.height; tj++) {
        int soilGid = soilLayer ? getTileGIDAt(soilLayer, Vec2(ti, tj)) : 0;
        Color4F color = isSoilBlock(soilGid)
                            ? Color4F(0.22f, 0.48f, 0.75f, 1.0f)  // water: blue
                            : Color4F(0.52f, 0.60f, 0.28f, 1.0f); // land: muted olive-green
        Vec2 p = miniMapDrawStartPos +
                 Vec2(ti, mapSize.height - tj - 1) * TILE_SIZE * miniMapScale;
        drawMiniMapTerrain->drawSolidRect(p, p + Vec2(miniMapBit, miniMapBit),
                                          color);
      }
    }
    if (GM->nextScene == STAGE_INTRO) {
      drawMiniMapTerrain->setVisible(false);
    }

    drawMiniMapFrame->drawSolidRect(
        Vec2::ZERO, Vec2(miniMapFrameWidth, miniMapFrameHeight), Color4F::GRAY);
    miniMapViewRect =
        cocos2d::Rect(miniMapDrawStartPos.x + (-getPosition().x) * miniMapScale,
                      miniMapDrawStartPos.y + (-getPosition().y) * miniMapScale,
                      size.width * miniMapScale, size.height * miniMapScale);
    updateMiniMapForNonMoving();
    if (GM->nextScene == STAGE_FIELD) {
      updateFog();
    }
  }
  this->addChild(theMap);
  moveScreen(getPosition());

  if (GM->nextScene == STAGE_SINGLEPLAY || GM->nextScene == STAGE_LOBBY ||
      GM->nextScene == STAGE_RAID) {
    for (int i = 0; i < 50; i++) {
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot0.png"))
          ->setLocalZOrder(-9999);
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot1.png"))
          ->setLocalZOrder(-9999);
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot2.png"))
          ->setLocalZOrder(-9999);
    }
    for (int i = 0; i < 20; i++) {
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("decoGrass0.png"));
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("decoGrass1.png"));
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("decoRock0.png"));
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("decoRock1.png"));
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("decoRock2.png"));
      addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandFlower0.png"));
    }
  }
}

// --- MapEditor JSON stage support -----------------------------------------
// Mirrors MapEditor::terrainAt/cornerTerrain/pickTileIdForCell (MapEditor.cpp)
// exactly, working off a passed-in grid instead of MapEditor's own member
// state, so a JSON-authored stage's terrain autotiles identically to how it
// looked in the editor. 0=grass,1=soil,2=water.
int GameScene::editorTerrainAt(const std::vector<unsigned char> &grid,
                                int width, int height, int x, int y) {
  if (x < 0 || y < 0 || x >= width || y >= height) {
    return 0;
  }
  return grid[y * width + x];
}
int GameScene::editorCornerTerrain(const std::vector<unsigned char> &grid,
                                    int width, int height, int cx, int cy) {
  int neighborX[4] = {cx - 1, cx, cx - 1, cx};
  int neighborY[4] = {cy - 1, cy - 1, cy, cy};
  int best = 0;
  for (int i = 0; i < 4; i++) {
    int t = editorTerrainAt(grid, width, height, neighborX[i], neighborY[i]);
    if (t == 2) {
      return 2;
    }
    if (t == 1) {
      best = 1;
    }
  }
  return best;
}
int GameScene::editorPickTileIdForCell(const std::vector<unsigned char> &grid,
                                        int width, int height, int x, int y) {
  int tl = editorCornerTerrain(grid, width, height, x, y);
  int tr = editorCornerTerrain(grid, width, height, x + 1, y);
  int bl = editorCornerTerrain(grid, width, height, x, y + 1);
  int br = editorCornerTerrain(grid, width, height, x + 1, y + 1);

  int renderTerrain = editorTerrainAt(grid, width, height, x, y);
  if (renderTerrain == 0) {
    if (tl == 2 || tr == 2 || bl == 2 || br == 2) {
      renderTerrain = 2;
    } else if (tl == 1 || tr == 1 || bl == 1 || br == 1) {
      renderTerrain = 1;
    } else {
      return 0; // BASE_GRASS_TILE_ID
    }
  }

  bool btl = (tl == renderTerrain);
  bool btr = (tr == renderTerrain);
  bool bbl = (bl == renderTerrain);
  bool bbr = (br == renderTerrain);

  struct CornerTileEntry {
    int tileId;
    bool tl, tr, bl, br;
  };
  static const CornerTileEntry kCornerTiles[] = {
      {30, false, false, false, true},
      {31, false, false, true, true},
      {32, false, false, true, false},
      {33, true, true, true, false},
      {34, true, true, false, true},
      {40, false, true, false, true},
      {41, true, true, true, true},
      {42, true, false, true, false},
      {43, true, false, true, true},
      {44, false, true, true, true},
      {50, false, true, false, false},
      {51, true, true, false, false},
      {52, true, false, false, false},
  };
  int soilTileId = 41; // FULL_SOIL_TILE_ID
  for (const CornerTileEntry &entry : kCornerTiles) {
    if (entry.tl == btl && entry.tr == btr && entry.bl == bbl &&
        entry.br == bbr) {
      soilTileId = entry.tileId;
      break;
    }
  }
  return renderTerrain == 2 ? soilTileId + 110 : soilTileId; // WATER_TILE_ID_OFFSET
}

namespace {
// Builds one TMX <layer> CSV data block. Tiled tolerates the trailing comma
// after every value except the very last - matches the real stageN.tmx files.
std::string editorMapBuildCsvLayer(const std::vector<int> &gids, int width,
                                   int height) {
  std::string out;
  out.reserve(gids.size() * 4);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      out += std::to_string(gids[y * width + x]);
      if (!(y == height - 1 && x == width - 1)) {
        out += ",";
      }
    }
    out += "\n";
  }
  return out;
}

// index into MapEditor's kObjectTypes (MapEditor.cpp) -> Movable.h UNIT_*
// constant. -1 = not a real placeable unit (Flag, index 33) - skip it.
// Indices 34-78 are the Spine-animated heroes, in the exact order
// MapEditor.cpp declares them (re-derived directly from the live array, not
// from memory). Single shared source of truth - both spawnObjectsFromEditorJsonFile
// and the trigger interpreter (updateTriggers/executeTriggerAction) use this
// exact array, nothing else should declare a second copy.
const int kEditorTypeToUnit[] = {
    UNIT_WORKER,        UNIT_SWORDMAN,        UNIT_ARCHER,
    UNIT_CATAPULT,       UNIT_HELICOPTER,      UNIT_CASTLE,
    UNIT_FARM,           UNIT_BARRACKS,        UNIT_LUMBERMILL,
    UNIT_WATCHERTOWER,   UNIT_FACTORY,         UNIT_AIRPORT,
    UNIT_HUMAN_SHUTTLE,     UNIT_ORC_AXE,         UNIT_ORC_SPEAR,
    UNIT_GOBLIN,         UNIT_GOBLIN_BOMB,     UNIT_GOBLIN_WORKER,
    UNIT_TROLL,          UNIT_WIZARD,          UNIT_ORC_HQ,
    UNIT_ORC_BUNKER,     UNIT_ORC_BARRACKS,    UNIT_ORC_TROLL_HOUSE,
    UNIT_TEMPLE,         UNIT_BARBECUE,        UNIT_ORC_SHUTTLE,
    UNIT_ZOMBIE_ORC_AXE, UNIT_ZOMBIE_SWORDSMAN,UNIT_ZOMBIE_CASTLE,
    UNIT_ZOMBIE_HQ,      UNIT_TREE,            UNIT_MINE,
    -1, // 33 = Flag, not a real unit
    // 34-78: heroes
    UNIT_HERO_ORC,             UNIT_HERO_GOBLIN,          UNIT_HERO_SPEARMAN,
    UNIT_HERO_LIZARDMAN,       UNIT_HERO_ARCHER,          UNIT_HERO_WEREWOLF,
    UNIT_HERO_MONK,            UNIT_HERO_FIGHTER,         UNIT_HERO_BEAR,
    UNIT_HERO_HEALER,          UNIT_HERO_KNIGHT,          UNIT_HERO_ELF_SWORDMAN,
    UNIT_HERO_ASSASSIN,        UNIT_HERO_LION,            UNIT_HERO_WIZARD,
    UNIT_HERO_TANKER,          UNIT_HERO_SKELETON,        UNIT_HERO_REAPER,
    UNIT_HERO_ENT,             UNIT_HERO_SALAMANDER,      UNIT_HERO_UNDINE,
    UNIT_HERO_CRAZY_WEREWOLF,  UNIT_HERO_CRAZY_BEAR,      UNIT_HERO_CRAZY_LION,
    UNIT_HERO_LADY_WEREWOLF,   UNIT_HERO_LADY_LION,       UNIT_HERO_LADY_BEAR,
    UNIT_HERO_SANTA,           UNIT_HERO_RUDOLPH,         UNIT_HERO_SANTADOG,
    UNIT_HERO_PENGUIN,         UNIT_HERO_CATINBOOTS,      UNIT_HERO_MOLE,
    UNIT_HERO_TOYMOUSE,        UNIT_HERO_SAVAGEARCHER,    UNIT_HERO_BATMONSTER,
    UNIT_HERO_MEMEAT,          UNIT_HERO_PARASITE,        UNIT_HERO_WATERMELON,
    UNIT_HERO_BABYMINO,        UNIT_HERO_MINO,            UNIT_HERO_KERBEROS,
    UNIT_HERO_LAMIA,           UNIT_HERO_CHUNJA,          UNIT_HERO_GOLEM,
    // indices 79-81: ships (appended after heroes to avoid shifting existing indices)
    UNIT_HUMAN_SHIP,    UNIT_ORC_SHIP,    UNIT_HUMAN_BATTLE_SHIP,
    // index 82: camera start marker (non-unit, handled in spawnObjectsFromEditorJsonFile)
    UNIT_START_POINT,
    // indices 83-85: oil system
    UNIT_OIL_SPOT, UNIT_HUMAN_OIL_SHIP, UNIT_ORC_OIL_SHIP,
};
const int kEditorTypeCount = sizeof(kEditorTypeToUnit) / sizeof(kEditorTypeToUnit[0]);
}

TMXTiledMap *GameScene::createTMXFromEditorJsonFile(const std::string &path) {
  std::string jsonStr = FileUtils::getInstance()->getStringFromFile(path);
  rapidjson::Document doc;
  doc.Parse(jsonStr.c_str());
  bool ok = !doc.HasParseError() && doc.IsObject() && doc.HasMember("width") &&
            doc.HasMember("height") && doc.HasMember("tileData");
  int width = 40;
  int height = 40;
  std::vector<unsigned char> grid;
  if (ok) {
    width = doc["width"].GetInt();
    height = doc["height"].GetInt();
    std::string base64Data = doc["tileData"].GetString();
    unsigned char *decoded = nullptr;
    int decodedLen = base64Decode(
        reinterpret_cast<const unsigned char *>(base64Data.c_str()),
        static_cast<unsigned int>(base64Data.size()), &decoded);
    if (decodedLen > 0) {
      std::string compressed(reinterpret_cast<char *>(decoded), decodedLen);
      free(decoded);
      std::string rawBytes = GameManager::getInstance()->decompress_string(compressed);
      if (static_cast<int>(rawBytes.size()) == width * height) {
        grid.assign(rawBytes.begin(), rawBytes.end());
      } else {
        ok = false;
      }
    } else {
      free(decoded);
      ok = false;
    }
  }
  if (!ok || grid.empty()) {
    log("createTMXFromEditorJsonFile: failed to parse %s, falling back to a blank grass map",
        path.c_str());
    width = 40;
    height = 40;
    grid.assign(width * height, 0);
  }

  std::vector<int> stageGids(width * height, 1); // GID 1 = tileId 0 = base grass
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      stageGids[y * width + x] = editorPickTileIdForCell(grid, width, height, x, y) + 1;
    }
  }
  // Seed with GID=1 so the TMX parser always creates the layer object.
  // All-zero layers are silently dropped by tilesetForLayer(), which causes
  // decoLayer/soilLayer/unitLayer to be nullptr and crash on first access.
  // GID=1 (base grass) is not a soil block, not a deco block, and not a
  // unit type, so the single seed tile at (0,0) is harmless.
  std::vector<int> emptyGids(width * height, 0);
  emptyGids[0] = 1;
  // GameScene::resetPathState() blocks movement off the "soil" layer's GID
  // (isSoilBlock), not the "stage" layer - real hand-authored stage*.tmx files
  // duplicate the water tile-shapes onto "soil" for exactly this reason. Mirror
  // that here so JSON-loaded water is actually unwalkable.
  std::vector<int> soilGids(width * height, 0);
  for (int i = 0; i < width * height; i++) {
    if (stageGids[i] - 1 >= 140) { // tile-id >= 140 = water range (WATER_TILE_ID_OFFSET+30)
      soilGids[i] = stageGids[i];
    }
  }
  if (soilGids[0] == 0)
    soilGids[0] = 1; // seed to prevent TMX parser from dropping the layer

  std::string xml;
  xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xml += StringUtils::format(
      "<map version=\"1.4\" tiledversion=\"1.4.1\" orientation=\"orthogonal\" "
      "renderorder=\"right-down\" width=\"%d\" height=\"%d\" tilewidth=\"50\" "
      "tileheight=\"50\" infinite=\"0\" nextlayerid=\"5\" nextobjectid=\"1\">\n",
      width, height);
  xml += " <tileset firstgid=\"1\" name=\"grassTilemap\" tilewidth=\"50\" tileheight=\"50\" tilecount=\"200\" columns=\"10\">\n"
         "  <image source=\"50TileSet.png\" width=\"512\" height=\"1024\"/>\n"
         "  <terraintypes>\n"
         "   <terrain name=\"soil\" tile=\"-1\"/>\n"
         "   <terrain name=\"water\" tile=\"-1\"/>\n"
         "  </terraintypes>\n"
         "  <tile id=\"30\" terrain=\",,,0\"/>\n"
         "  <tile id=\"31\" terrain=\",,0,0\"/>\n"
         "  <tile id=\"32\" terrain=\",,0,\"/>\n"
         "  <tile id=\"33\" terrain=\"0,0,0,\"/>\n"
         "  <tile id=\"34\" terrain=\"0,0,,0\"/>\n"
         "  <tile id=\"40\" terrain=\",0,,0\"/>\n"
         "  <tile id=\"41\" terrain=\"0,0,0,0\"/>\n"
         "  <tile id=\"42\" terrain=\"0,,0,\"/>\n"
         "  <tile id=\"43\" terrain=\"0,,0,0\"/>\n"
         "  <tile id=\"44\" terrain=\",0,0,0\"/>\n"
         "  <tile id=\"50\" terrain=\",0,,\"/>\n"
         "  <tile id=\"51\" terrain=\"0,0,,\"/>\n"
         "  <tile id=\"52\" terrain=\"0,,,\"/>\n"
         "  <tile id=\"140\" terrain=\",,,1\"/>\n"
         "  <tile id=\"141\" terrain=\",,1,1\"/>\n"
         "  <tile id=\"142\" terrain=\",,1,\"/>\n"
         "  <tile id=\"143\" terrain=\"1,1,1,\"/>\n"
         "  <tile id=\"144\" terrain=\"1,1,,1\"/>\n"
         "  <tile id=\"150\" terrain=\",1,,1\"/>\n"
         "  <tile id=\"151\" terrain=\"1,1,1,1\"/>\n"
         "  <tile id=\"152\" terrain=\"1,,1,\"/>\n"
         "  <tile id=\"153\" terrain=\"1,,1,1\"/>\n"
         "  <tile id=\"154\" terrain=\",1,1,1\"/>\n"
         "  <tile id=\"160\" terrain=\",1,,\"/>\n"
         "  <tile id=\"161\" terrain=\"1,1,,\"/>\n"
         "  <tile id=\"162\" terrain=\"1,,,\"/>\n"
         " </tileset>\n";
  xml += StringUtils::format(" <layer id=\"1\" name=\"stage\" width=\"%d\" height=\"%d\">\n", width, height);
  xml += "  <data encoding=\"csv\">\n" + editorMapBuildCsvLayer(stageGids, width, height) + " </data>\n </layer>\n";
  xml += StringUtils::format(" <layer id=\"2\" name=\"soil\" width=\"%d\" height=\"%d\">\n", width, height);
  xml += "  <data encoding=\"csv\">\n" + editorMapBuildCsvLayer(soilGids, width, height) + " </data>\n </layer>\n";
  xml += StringUtils::format(" <layer id=\"3\" name=\"deco\" width=\"%d\" height=\"%d\">\n", width, height);
  xml += "  <data encoding=\"csv\">\n" + editorMapBuildCsvLayer(emptyGids, width, height) + " </data>\n </layer>\n";
  xml += StringUtils::format(" <layer id=\"4\" name=\"unit\" width=\"%d\" height=\"%d\">\n", width, height);
  xml += "  <data encoding=\"csv\">\n" + editorMapBuildCsvLayer(emptyGids, width, height) + " </data>\n </layer>\n";
  xml += "</map>\n";

  return TMXTiledMap::createWithXML(xml, "tilemap");
}

void GameScene::spawnObjectsFromEditorJsonFile(const std::string &path) {
  std::string jsonStr = FileUtils::getInstance()->getStringFromFile(path);
  rapidjson::Document doc;
  doc.Parse(jsonStr.c_str());
  if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("objects") ||
      !doc["objects"].IsArray()) {
    return;
  }

  editorObjectPositionsById.clear();
  for (auto &item : doc["objects"].GetArray()) {
    if (!item.HasMember("type") || !item.HasMember("x") || !item.HasMember("y")) {
      continue;
    }
    int typeIndex = item["type"].GetInt();
    if (typeIndex < 0 || typeIndex >= kEditorTypeCount) {
      continue;
    }
    int unitType = kEditorTypeToUnit[typeIndex];
    int x = item["x"].GetInt();
    int y = item["y"].GetInt();
    int side = item.HasMember("side") ? item["side"].GetInt() : 0;
    int hp = item.HasMember("hp") ? item["hp"].GetInt() : -1;
    int level = item.HasMember("level") ? item["level"].GetInt() : 1;
    int id = item.HasMember("id") ? item["id"].GetInt() : -1;

    Vec2 pos = getPositionFromTileCoordinate(x, y);
    if (id >= 0) {
      // Snapshot every placed object's position, including Flag (typeIndex
      // 33, unitType -1) and any other non-unit marker - trigger actions can
      // target those by id too (e.g. "center camera on this Flag").
      editorObjectPositionsById[id] = pos;
    }
    if (unitType < 0) {
      continue;
    }
    if (unitType == UNIT_START_POINT) {
      if (!isMultiplay) {
        this->setPosition(-pos * layerScale +
                          Vec2(size.width / 2, size.height / 2));
      }
      continue;
    }

    int whichSide = WHICH_SIDE_HERO;
    if (side == 1) {
      whichSide = WHICH_SIDE_ENEMY;
    } else if (side == 2 || side == 3) {
      whichSide = WHICH_SIDE_MUTUAL;
    }
    // Trees, mines, and oil spots are always neutral resources regardless of the editor-set side.
    if (unitType == UNIT_TREE || unitType == UNIT_TREE_FOR_BATTLE ||
        unitType == UNIT_MINE || unitType == UNIT_OIL_SPOT) {
      whichSide = WHICH_SIDE_MUTUAL;
    }

    bool building = GM->isThisBuilding(unitType);
    EnemyBase *unit = createUnit(unitType, whichSide, building, pos,
                                 GM->getUnitName(unitType), 1,
                                 getSpriteNameForUnit(unitType));
    if (unit != nullptr && unit->spine != nullptr) {
      // createUnit already set unit->spine (Movable::init, based on
      // GameManager::getSpineFileName) for any hero type - this is the same
      // "is this actually a hero" check the rest of the codebase uses.
      setHeroLevelInfo(unit, std::max(1, level));
    }
    if (unit != nullptr &&
        (unitType == UNIT_TREE || unitType == UNIT_TREE_FOR_BATTLE)) {
      unit->canMove = false;
      unit->unitID = mutualID++;
      // Mark the tree's tile as blocked so pathfinding routes around it.
      if (decoLayer)
        setTileGID(decoLayer, 48, getCoordinateFromPosition(pos, theMap));
    }
    if (unit != nullptr && building) {
      unit->isBuildingComplete = true;
      if (unitType == UNIT_ORC_BUNKER) {
        unit->canFindTarget = true;
        setOccupy(pos + Vec2(-50, 0) * imageScale, 2, 2, true, unit);
      } else if (unitType == UNIT_ORC_HQ || unitType == UNIT_ZOMBIE_HQ) {
        setOccupy(pos - Vec2(150, -100) * imageScale, 4, 3, true, unit);
      } else if (unitType == UNIT_FACTORY) {
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
        unit->startProductSchedule();
      } else if (unitType == UNIT_FARM) {
        if (whichSide == WHICH_SIDE_HERO)
          addFoodMax(getFoodGive(UNIT_FARM));
        setOccupy(pos - Vec2(100, 0) * imageScale, 3, 2, true, unit);
      } else if (unitType == UNIT_LUMBERMILL) {
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
      } else if (unitType == UNIT_AIRPORT) {
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
        unit->startProductSchedule();
        Sprite *spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
        GM->runAnimation(spt, "propeller", true);
        unit->addChild(spt);
        spt->setPosition(Vec2(134, 206) * imageScale);
      } else if (unitType == UNIT_MINE) {
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
        unit->setTag(0);
      } else if (unitType == UNIT_BARRACKS) {
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
        unit->startProductSchedule();
      } else if (unitType == UNIT_CASTLE || unitType == UNIT_ZOMBIE_CASTLE) {
        setOccupy(pos - Vec2(150, -100) * imageScale, 4, 3, true, unit);
        if (whichSide == WHICH_SIDE_HERO)
          addFoodMax(getFoodGive(UNIT_CASTLE));
        unit->startProductSchedule();
        if (unitType == UNIT_CASTLE) {
          Sprite *spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
          GM->runAnimation(spt, "blueFlag", true);
          unit->addChild(spt);
          spt->setPosition(Vec2(192, 255) * imageScale);
        }
      } else if (unitType == UNIT_WATCHERTOWER) {
        setOccupy(pos - Vec2(50, 0), 2, 2, true, unit);
        unit->canFindTarget = true;
      } else if (unitType == UNIT_ORC_BARRACKS) {
        unit->startProductSchedule();
        setOccupy(pos - Vec2(50, -50) * imageScale, 2, 2, true, unit);
      } else if (unitType == UNIT_TEMPLE) {
        unit->startProductSchedule();
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
      } else if (unitType == UNIT_ORC_TROLL_HOUSE) {
        unit->startProductSchedule();
        setOccupy(pos - Vec2(100, -100) * imageScale, 3, 3, true, unit);
      } else if (unitType == UNIT_BARBECUE) {
        setOccupy(pos - Vec2(100, -50) * imageScale, 3, 2, true, unit);
      }
    }
    if (hp > 0 && unit != nullptr) {
      unit->maxEnergy = hp;
      unit->energy = hp;
    }
  }
  resetPathState();
}

bool GameScene::editorStageHasVictoryTrigger(const std::string &path) {
  std::string jsonStr = FileUtils::getInstance()->getStringFromFile(path);
  rapidjson::Document doc;
  doc.Parse(jsonStr.c_str());
  if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("triggers") ||
      !doc["triggers"].IsArray()) {
    return false;
  }
  for (auto &t : doc["triggers"].GetArray()) {
    if (!t.HasMember("actions") || !t["actions"].IsArray()) {
      continue;
    }
    for (auto &a : t["actions"].GetArray()) {
      if (a.HasMember("type") && a["type"].GetInt() == 4) { // TACT_VICTORY
        return true;
      }
    }
  }
  return false;
}

void GameScene::loadTriggersFromEditorJsonFile(const std::string &path) {
  activeTriggers.clear();
  std::string jsonStr = FileUtils::getInstance()->getStringFromFile(path);
  rapidjson::Document doc;
  doc.Parse(jsonStr.c_str());
  if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("triggers") ||
      !doc["triggers"].IsArray()) {
    return;
  }
  for (auto &titem : doc["triggers"].GetArray()) {
    RuntimeTrigger t;
    if (titem.HasMember("name")) {
      t.name = titem["name"].GetString();
    }
    if (titem.HasMember("preserve")) {
      t.preserve = titem["preserve"].GetBool();
    }
    if (titem.HasMember("sides") && titem["sides"].IsArray()) {
      int s = 0;
      for (auto &sv : titem["sides"].GetArray()) {
        if (s >= 4) {
          break;
        }
        t.sides[s] = sv.GetBool();
        s++;
      }
    }
    if (titem.HasMember("conditions") && titem["conditions"].IsArray()) {
      for (auto &citem : titem["conditions"].GetArray()) {
        RuntimeTriggerCondition c;
        if (citem.HasMember("type")) c.type = citem["type"].GetInt();
        if (citem.HasMember("elapsedSeconds")) c.elapsedSeconds = citem["elapsedSeconds"].GetInt();
        if (citem.HasMember("switchIndex")) c.switchIndex = citem["switchIndex"].GetInt();
        if (citem.HasMember("switchState")) c.switchState = citem["switchState"].GetInt();
        if (citem.HasMember("unitSide")) c.unitSide = citem["unitSide"].GetInt();
        if (citem.HasMember("unitTypeIndex")) c.unitTypeIndex = citem["unitTypeIndex"].GetInt();
        if (citem.HasMember("comparison")) c.comparison = citem["comparison"].GetInt();
        if (citem.HasMember("amount")) c.amount = citem["amount"].GetInt();
        if (citem.HasMember("resourceKind")) c.resourceKind = citem["resourceKind"].GetInt();
        if (citem.HasMember("isRepeat")) c.isRepeat = citem["isRepeat"].GetBool();
        t.conditions.push_back(c);
      }
    }
    if (titem.HasMember("actions") && titem["actions"].IsArray()) {
      for (auto &aitem : titem["actions"].GetArray()) {
        RuntimeTriggerAction a;
        if (aitem.HasMember("type")) a.type = aitem["type"].GetInt();
        if (aitem.HasMember("message")) a.message = aitem["message"].GetString();
        if (aitem.HasMember("unitSide")) a.unitSide = aitem["unitSide"].GetInt();
        if (aitem.HasMember("unitTypeIndex")) a.unitTypeIndex = aitem["unitTypeIndex"].GetInt();
        if (aitem.HasMember("tileX")) a.tileX = aitem["tileX"].GetInt();
        if (aitem.HasMember("tileY")) a.tileY = aitem["tileY"].GetInt();
        if (aitem.HasMember("targetObjectId")) a.targetObjectId = aitem["targetObjectId"].GetInt();
        if (aitem.HasMember("count")) a.count = aitem["count"].GetInt();
        if (aitem.HasMember("switchIndex")) a.switchIndex = aitem["switchIndex"].GetInt();
        if (aitem.HasMember("switchAction")) a.switchAction = aitem["switchAction"].GetInt();
        if (aitem.HasMember("waitSeconds")) a.waitSeconds = static_cast<float>(aitem["waitSeconds"].GetDouble());
        if (aitem.HasMember("visionEnabled")) a.visionEnabled = aitem["visionEnabled"].GetBool();
        t.actions.push_back(a);
      }
    }
    activeTriggers.push_back(t);
  }
}

cocos2d::Vec2 GameScene::resolveTriggerTargetPosition(int targetObjectId, int tileX, int tileY) {
  if (targetObjectId >= 0) {
    auto it = editorObjectPositionsById.find(targetObjectId);
    if (it != editorObjectPositionsById.end()) {
      return it->second;
    }
    // Id not found (e.g. removed, or a stale reference) - fall back to the
    // tile coordinate rather than returning a bogus (0,0).
  }
  return getPositionFromTileCoordinate(tileX, tileY);
}

void GameScene::clearTriggerTalkBubble() {
  if (activeTalkBubble != nullptr) {
    activeTalkBubble->removeFromParent();
    activeTalkBubble = nullptr;
  }
}

void GameScene::showTriggerTalkBubble(const std::string &message, const cocos2d::Vec2 &worldPos) {
  clearTriggerTalkBubble();

  Label *lbl = Label::createWithSystemFont(message, "Arial", 28);
  lbl->setAnchorPoint(Vec2(0.5f, 0.5f));
  Size textSize = lbl->getContentSize();

  const float paddingX = 24;
  const float paddingY = 16;
  Size bubbleSize(textSize.width + paddingX * 2, textSize.height + paddingY * 2);

  Node *bubble = Node::create();
  // Float above the target's head, same general idea as the dead/unused
  // addTalkBalloon's "questState" marker (npc bounding box + an offset) -
  // but this one carries real text, not a static icon.
  bubble->setPosition(worldPos + Vec2(0, 90));

  DrawNode *bg = DrawNode::create();
  Vec2 half(bubbleSize.width / 2, bubbleSize.height / 2);
  bg->drawSolidRect(-half, half, Color4F(0, 0, 0, 0.75f));
  bubble->addChild(bg);

  lbl->setPosition(Vec2::ZERO);
  bubble->addChild(lbl, 1);

  batch->addChild(bubble, 100000); // draw above units, same spirit as Fog's z-order
  activeTalkBubble = bubble;
}

bool GameScene::evaluateTriggerCondition(const RuntimeTriggerCondition &c) {
  switch (c.type) {
    case 0: // Always
      return true;
    case 1: // ElapsedTime
      // isRepeat=true: check from last fire time (reset by updateTriggers when preserve=false)
      // isRepeat=false: check from game start (one-shot)
      if (c.isRepeat) {
        return (gameTimer - c.elapsedTimeLastFired) >= static_cast<float>(c.elapsedSeconds);
      }
      return gameTimer >= static_cast<float>(c.elapsedSeconds);
    case 2: { // Switch
      if (c.switchIndex < 0 || c.switchIndex >= 16) {
        return false;
      }
      bool wantSet = (c.switchState == 0);
      return triggerSwitches[c.switchIndex] == wantSet;
    }
    case 3: { // UnitCount
      const Vector<EnemyBase *> *array = &heroArray;
      if (c.unitSide == 1) {
        array = &enemyArray;
      } else if (c.unitSide == 2 || c.unitSide == 3) {
        array = &mutualArray;
      }
      int wantUnitType = -1;
      if (c.unitTypeIndex >= 0 && c.unitTypeIndex < kEditorTypeCount) {
        wantUnitType = kEditorTypeToUnit[c.unitTypeIndex];
      }
      int count = 0;
      for (EnemyBase *unit : *array) {
        if (wantUnitType < 0 || unit->unitType == wantUnitType) {
          count++;
        }
      }
      if (c.comparison == 0) return count >= c.amount;       // AtLeast
      if (c.comparison == 1) return count <= c.amount;       // AtMost
      return count == c.amount;                              // Exactly
    }
    case 4: { // Resource
      int value = (c.resourceKind == 0) ? GM->getCoin() : GM->getTree();
      if (c.comparison == 0) return value >= c.amount;
      if (c.comparison == 1) return value <= c.amount;
      return value == c.amount;
    }
    default:
      return false;
  }
}

void GameScene::executeTriggerAction(const RuntimeTriggerAction &a, bool flipOutcome) {
  // Whatever step is starting now ends the previous Talk bubble's lifetime,
  // if one is still showing - its visible duration is however long the
  // *previous* step's own pacing (typically a Wait) lasted, not a fixed timer.
  clearTriggerTalkBubble();

  switch (a.type) {
    case 0: { // DisplayMessage
      if (HUD != nullptr) {
        HUD->showInstanceMessage(a.message);
      }
      break;
    }
    case 1: { // CreateUnit
      if (a.unitTypeIndex < 0 || a.unitTypeIndex >= kEditorTypeCount) {
        break;
      }
      int unitType = kEditorTypeToUnit[a.unitTypeIndex];
      if (unitType < 0) {
        break;
      }
      int whichSide = WHICH_SIDE_HERO;
      if (a.unitSide == 1) {
        whichSide = WHICH_SIDE_ENEMY;
      } else if (a.unitSide == 2 || a.unitSide == 3) {
        whichSide = WHICH_SIDE_MUTUAL;
      }
      Vec2 basePos = resolveTriggerTargetPosition(a.targetObjectId, a.tileX, a.tileY);
      bool building = GM->isThisBuilding(unitType);
      int spawnCount = std::max(1, a.count);
      for (int i = 0; i < spawnCount; i++) {
        // Small fixed offset per extra unit so a count>1 spawn doesn't stack
        // every unit on the exact same point - doesn't need to be clever.
        Vec2 pos = basePos + Vec2(static_cast<float>(i % 5) * 40.0f - 80.0f,
                                   static_cast<float>(i / 5) * 40.0f);
        EnemyBase *unit = createUnit(unitType, whichSide, building, pos,
                                     GM->getUnitName(unitType), 1,
                                     getSpriteNameForUnit(unitType));
        if (unit != nullptr && unit->spine != nullptr) {
          setHeroLevelInfo(unit, 1);
        }
      }
      break;
    }
    case 2: { // RemoveUnit
      Vector<EnemyBase *> *array = &heroArray;
      if (a.unitSide == 1) {
        array = &enemyArray;
      } else if (a.unitSide == 2 || a.unitSide == 3) {
        array = &mutualArray;
      }
      int wantUnitType = -1;
      if (a.unitTypeIndex >= 0 && a.unitTypeIndex < kEditorTypeCount) {
        wantUnitType = kEditorTypeToUnit[a.unitTypeIndex];
      }
      // Snapshot first - removeDeadUnit mutates heroArray/enemyArray/mutualArray
      // (all of them, regardless of which one "array" points at), so iterating
      // *array directly while removing from it is unsafe.
      std::vector<EnemyBase *> matches;
      for (EnemyBase *unit : *array) {
        if (wantUnitType < 0 || unit->unitType == wantUnitType) {
          matches.push_back(unit);
        }
      }
      for (EnemyBase *unit : matches) {
        removeDeadUnit(unit);
      }
      break;
    }
    case 3: { // SetSwitch
      if (a.switchIndex < 0 || a.switchIndex >= 16) {
        break;
      }
      if (a.switchAction == 0) {
        triggerSwitches[a.switchIndex] = true;
      } else if (a.switchAction == 1) {
        triggerSwitches[a.switchIndex] = false;
      } else {
        triggerSwitches[a.switchIndex] = !triggerSwitches[a.switchIndex];
      }
      break;
    }
    case 4: // Victory - flipped: this is the Enemy's victory, i.e. the player lost
      endGame(!flipOutcome);
      break;
    case 5: // Defeat - flipped: this is the Enemy's defeat, i.e. the player won
      endGame(flipOutcome);
      break;
    case 6: // Wait - the pause itself is the DelayTime runTriggerActions
            // inserts for this step; nothing to do here.
      break;
    case 7: { // CenterCamera
      Vec2 targetPos = resolveTriggerTargetPosition(a.targetObjectId, a.tileX, a.tileY);
      Size viewSize = Director::getInstance()->getVisibleSize();
      moveScreen(-targetPos * layerScale + viewSize / 2);
      break;
    }
    case 8: { // Talk
      Vec2 targetPos = resolveTriggerTargetPosition(a.targetObjectId, a.tileX, a.tileY);
      showTriggerTalkBubble(a.message, targetPos);
      break;
    }
    case 9: { // RevealFog
      Vec2 centerPos = resolveTriggerTargetPosition(a.targetObjectId, a.tileX, a.tileY);
      int radius = std::max(1, a.count);
      if (a.visionEnabled) {
        // Add a new permanently-revealed region (allow duplicates so two
        // separate ON triggers for the same area each have their own entry
        // to be removed by a matching OFF trigger).
        TriggerRevealedFogRegion region;
        region.centerWorldPos = centerPos;
        region.radius = radius;
        triggerRevealedFogRegions.push_back(region);
      } else {
        // Remove the first region whose centre matches within one fog tile so
        // each OFF trigger cancels exactly one prior ON trigger.
        float threshold = FOG_SIZE * 1.0f;
        auto it = std::find_if(triggerRevealedFogRegions.begin(), triggerRevealedFogRegions.end(),
          [&](const TriggerRevealedFogRegion& r) {
            return r.centerWorldPos.distance(centerPos) < threshold;
          });
        if (it != triggerRevealedFogRegions.end()) {
          triggerRevealedFogRegions.erase(it);
        }
      }
      break;
    }
    case 10: { // OrderAttack — attack-move toward nearest enemy building
      Vector<EnemyBase *> *srcArray = &heroArray;
      Vector<EnemyBase *> *tgtArray = &enemyArray;
      if (a.unitSide == 1) { srcArray = &enemyArray; tgtArray = &heroArray; }
      else if (a.unitSide == 2 || a.unitSide == 3) { srcArray = &mutualArray; tgtArray = &enemyArray; }
      int wantUnitType = -1;
      if (a.unitTypeIndex >= 0 && a.unitTypeIndex < kEditorTypeCount) {
        wantUnitType = kEditorTypeToUnit[a.unitTypeIndex];
      }
      // Snapshot target buildings (and non-buildings as fallback) once,
      // then for each ordered unit pick the nearest building to attack-move toward.
      std::vector<EnemyBase *> tgtBuildings;
      std::vector<EnemyBase *> tgtUnits;
      for (EnemyBase *tgt : *tgtArray) {
        if (tgt->energy <= 0 || tgt->untouchable) continue;
        if (tgt->isBuilding) tgtBuildings.push_back(tgt);
        else tgtUnits.push_back(tgt);
      }
      std::vector<EnemyBase *> srcs;
      for (EnemyBase *u : *srcArray) {
        if (u->energy <= 0) continue;
        if (wantUnitType >= 0 && u->unitType != wantUnitType) continue;
        srcs.push_back(u);
      }
      for (EnemyBase *unit : srcs) {
        // Prefer nearest building; fall back to nearest unit if no buildings remain.
        const std::vector<EnemyBase *> &pool = tgtBuildings.empty() ? tgtUnits : tgtBuildings;
        EnemyBase *destination = nullptr;
        float minDist = FLT_MAX;
        for (EnemyBase *tgt : pool) {
          float d = unit->getPosition().distanceSquared(tgt->getPosition());
          if (d < minDist) { minDist = d; destination = tgt; }
        }
        if (destination != nullptr) {
          Vector<EnemyBase *> single;
          single.pushBack(unit);
          moveAndAttackTo(single, destination->getPosition());
        } else {
          unit->canFindTarget = true;
        }
      }
      break;
    }
    default:
      break;
  }
}

void GameScene::runTriggerActions(const RuntimeTrigger &t) {
  // sides[0]=Ally, sides[1]=Enemy. An Enemy-only trigger (Enemy checked,
  // Ally not) represents the enemy's perspective, so its Victory/Defeat
  // actions flip relative to the human player. Ally checked (alone or with
  // Enemy too) and the two Neutral flags all resolve literally - there's no
  // clear single-player meaning for "the neutral side's victory" yet.
  bool flipOutcome = t.sides[1] && !t.sides[0];
  Vector<FiniteTimeAction *> steps;
  for (const RuntimeTriggerAction &a : t.actions) {
    RuntimeTriggerAction actionCopy = a;
    steps.pushBack(CallFunc::create([this, actionCopy, flipOutcome]() {
      executeTriggerAction(actionCopy, flipOutcome);
    }));
    if (a.type == 6) { // Wait
      steps.pushBack(DelayTime::create(std::max(0.0f, a.waitSeconds)));
    }
  }
  // End-of-sequence cleanup in case the last action was a Talk - otherwise
  // its bubble would linger forever with nothing to dismiss it.
  steps.pushBack(CallFunc::create([this]() {
    clearTriggerTalkBubble();
  }));
  if (!steps.empty()) {
    this->runAction(Sequence::create(steps));
  }
}

void GameScene::updateTriggers() {
  for (RuntimeTrigger &t : activeTriggers) {
    if (t.hasFired) continue; // permanently deactivated
    // Conditions evaluate the same regardless of which sides[] are marked -
    // a condition that cares about a particular side already says so itself
    // (e.g. UnitCount's own unitSide field). sides[] only matters once the
    // trigger fires, for resolving Victory/Defeat - see runTriggerActions.
    bool allTrue = true;
    for (const RuntimeTriggerCondition &c : t.conditions) {
      if (!evaluateTriggerCondition(c)) {
        allTrue = false;
        break;
      }
    }
    if (!allTrue) continue;
    runTriggerActions(t);
    // isRepeat=false on any condition (or no conditions) → deactivate after one fire
    bool anyRepeat = false;
    for (const auto &c : t.conditions) {
      if (c.isRepeat) { anyRepeat = true; break; }
    }
    if (!anyRepeat) {
      t.hasFired = true; // deactivate regardless of preserve
    } else if (!t.preserve) {
      // isRepeat=true + preserve=false: reset elapsed timer so it fires again after N more seconds
      for (auto &c : t.conditions) {
        if (c.type == 1 && c.isRepeat) {
          c.elapsedTimeLastFired = gameTimer;
        }
      }
    }
    // isRepeat=true + preserve=true: no reset, fires every frame while condition met
  }
}

void GameScene::loadMapData() {
  ValueVector entireList = GM->split(GM->loadMapData, "_");
  ValueVector smallList =
      GM->split(entireList.at(0).asString(), "/"); // map size
  mapSizeWidth = smallList.at(0).asInt();
  mapSizeHeight = smallList.at(1).asInt();

  smallList = GM->split(entireList.at(1).asString(), "|"); // tree

  ValueVector unitInfoList;
  vector<int> decoLayerList;
  for (int i = 0; i < smallList.size(); i++) {
    unitInfoList = GM->split(smallList.at(i).asString(), "/");
    if (unitInfoList.size() > 0) {
      for (int j = 0; j < unitInfoList.at(1).asInt(); j++) {
        int tile = unitInfoList.at(0).asInt();
        decoLayerList.push_back(tile);
      }
    }
  }

  int x = 0;
  int y = 0;
  for (int i = 0; i < decoLayerList.size(); i++) {
    if (decoLayerList.at(i) == 49) {
      //            brushTile(BRUSH_TREE, Vec2(y, x));
      EnemyBase *unit =
          createUnit(UNIT_TREE, WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING,
                     getPositionFromTileCoordinate(y, x), "tree", 1,
                     strmake("tree2_%d.png", rand() % 5).c_str());
      unit->canMove = false;
      unit->unitID = mutualID++;

      decoLayer->setTileGID(49, Vec2(y, x));
    }
    x++;
    if (x >= mapSizeWidth) {
      x = 0;
      y++;
    }
  }
  smallList = GM->split(entireList.at(2).asString(), "|"); // hero
  for (int i = 0; i < smallList.size(); i++) {
    unitInfoList = GM->split(smallList.at(i).asString(), "/");
    if (unitInfoList.size() > 1) {
      Vec2 coordinate =
          Vec2(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
      Vec2 pos = getPositionFromTileCoordinate(coordinate.x, coordinate.y);
      bool isBuilding = GM->isThisBuilding(unitInfoList.at(0).asInt());
      int unitType = unitInfoList.at(0).asInt();
      //            Sprite* spt = getSpriteForUnit(unitType);
      EnemyBase *unit = createUnit(unitType, WHICH_SIDE_HERO, isBuilding, pos,
                                   GM->getUnitName(unitInfoList.at(0).asInt()),
                                   1, getSpriteNameForUnit(unitType));
      unit->setTag(unitInfoList.at(1).asInt() +
                   unitInfoList.at(2).asInt() * mapSizeWidth);

      if (isBuilding) {
        cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
        setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y),
                  occupySize.width, occupySize.height, true, unit);

        setAfterBuildingProcess(unit);

        if (unitType == UNIT_CASTLE) {
          this->setPosition(
              -pos * layerScale +
              Vec2(size.width / 2 + TILE_SIZE * layerScale, size.height / 2));
        }
        //                unit->setPosition(pos +
        //                Vec2(TILE_SIZE*occupySize.width/2,
        //                -TILE_SIZE*occupySize.height/2));

        unit->setPosition(pos + Vec2(TILE_SIZE * (occupySize.width - 1) / 2,
                                     -TILE_SIZE * (occupySize.height - 1) / 2));
        unit->refreshApproachPoints();
        //                spt->setPosition(pos +
        //                Vec2(TILE_SIZE*(occupySize.width-1)/2,
        //                -TILE_SIZE*(occupySize.height-1)/2));
        //                this->addChild(spt, 1000);
        //                spt->setScale(0.1f);
        //                log("anchorPoint: %f, %f", unit->getAnchorPoint().x,
        //                unit->getAnchorPoint().y);
      }
    }
  }

  smallList = GM->split(entireList.at(3).asString(), "|"); // ready hero
  for (int i = 0; i < smallList.size(); i++) {
    unitInfoList = GM->split(smallList.at(i).asString(), "/");
    if (unitInfoList.size() > 1) {
      Vec2 coordinate =
          Vec2(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
      Vec2 pos = getPositionFromTileCoordinate(coordinate.x, coordinate.y);
      bool isBuilding = GM->isThisBuilding(unitInfoList.at(0).asInt());
      int unitType = unitInfoList.at(0).asInt();
      //            Sprite* spt = getSpriteForUnit(unitType);
      EnemyBase *unit =
          createUnit(unitType, WHICH_SIDE_READY_HERO, isBuilding, pos,
                     GM->getUnitName(unitInfoList.at(0).asInt()), 1,
                     getSpriteNameForUnit(unitInfoList.at(0).asInt()));
      unit->setTag(unitInfoList.at(1).asInt() +
                   unitInfoList.at(2).asInt() * mapSizeWidth);
      if (isBuilding) {
        cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
        setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y),
                  occupySize.width, occupySize.height, true, unit);

        setAfterBuildingProcess(unit);
        unit->setPosition(pos + Vec2(TILE_SIZE * (occupySize.width - 1) / 2,
                                     -TILE_SIZE * (occupySize.height - 1) / 2));
        unit->refreshApproachPoints();
      }
    }
  }

  smallList = GM->split(entireList.at(4).asString(), "|"); // enemy
  for (int i = 0; i < smallList.size(); i++) {
    unitInfoList = GM->split(smallList.at(i).asString(), "/");
    if (unitInfoList.size() > 1) {
      Vec2 coordinate =
          Vec2(unitInfoList.at(3).asInt(), unitInfoList.at(4).asInt());
      Vec2 pos = getPositionFromTileCoordinate(coordinate.x, coordinate.y);
      bool isBuilding = GM->isThisBuilding(unitInfoList.at(0).asInt());
      int unitType = unitInfoList.at(0).asInt();
      //            Sprite* spt = getSpriteForUnit(unitType);
      EnemyBase *unit =
          createUnit(unitInfoList.at(0).asInt(), WHICH_SIDE_ENEMY,
                     GM->isThisBuilding(unitInfoList.at(0).asInt()), pos,
                     GM->getUnitName(unitInfoList.at(0).asInt()), 1,
                     getSpriteNameForUnit(unitInfoList.at(0).asInt()));
      if (!isBuilding) {
        unit->unitAct = unitInfoList.at(1).asInt();
        unit->scheduledAttackTime = unitInfoList.at(2).asInt() * 60;
        if (unit->scheduledAttackTime < 0) {
          unit->scheduledAttackTime *= -1;
        }
      }
      unit->setTag(unitInfoList.at(3).asInt() +
                   unitInfoList.at(4).asInt() * mapSizeWidth);

      if (isBuilding) {
        cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
        setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y),
                  occupySize.width, occupySize.height, true, unit);
        if (unitType == UNIT_ORC_HQ) {
          unit->scheduledProductUnit = UNIT_ORC_AXE;
          unit->scheduledProductUnitCount = 2;
          unit->scheduledProductTime = unitInfoList.at(2).asInt() * 60;
          if (unit->scheduledProductTime < 0) {
            unit->scheduledProductTime *= -1;
          }
        }
        int previousType = unit->unitType;
        setAfterBuildingProcess(unit);
        unit->unitType = previousType;

        unit->setPosition(pos + Vec2(TILE_SIZE * (occupySize.width - 1) / 2,
                                     -TILE_SIZE * (occupySize.height - 1) / 2));
        unit->refreshApproachPoints();
      }
    }
  }

  smallList = GM->split(entireList.at(5).asString(), "|"); // mutual
  for (int i = 0; i < smallList.size(); i++) {
    unitInfoList = GM->split(smallList.at(i).asString(), "/");
    if (unitInfoList.size() > 1) {
      Vec2 coordinate =
          Vec2(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
      Vec2 pos = getPositionFromTileCoordinate(coordinate.x, coordinate.y);
      bool isBuilding = GM->isThisBuilding(unitInfoList.at(0).asInt());
      int unitType = unitInfoList.at(0).asInt();
      //            Sprite* spt = getSpriteForUnit(unitType);
      //            spt->setScale(0.2f);
      //            this->addChild(spt, 10);
      //            spt->setPosition(pos);
      EnemyBase *unit =
          createUnit(unitType, WHICH_SIDE_MUTUAL, isBuilding, pos,
                     GM->getUnitName(unitInfoList.at(0).asInt()), 1,
                     getSpriteNameForUnit(unitInfoList.at(0).asInt()));
      unit->setTag(unitInfoList.at(1).asInt() +
                   unitInfoList.at(2).asInt() * mapSizeWidth);
      if (isBuilding) {
        cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
        setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y),
                  occupySize.width, occupySize.height, true, unit);
        setAfterBuildingProcess(unit);
        if (unitType == UNIT_MINE) {
          unit->setPosition(pos +
                            Vec2(TILE_SIZE * (occupySize.width - 1) / 2,
                                 -TILE_SIZE * (occupySize.height - 1) / 2));

          unit->setTag(0);
          unit->unitID = mutualID++;
        }
        unit->refreshApproachPoints();
      }
    }
  }

  GM->loadMapData = "";
  resetPathState();
  //    updateMiniMapForNonMoving();
}

Sprite *GameScene::addDecoToOutOfField(Sprite *spt) {
  int where = rand() % 4;
  Vec2 pos;
  if (where == 0) { // north to closewise
    pos = Vec2(rand() % (int)(TILE_SIZE * mapSize.width + 800) - 400,
               TILE_SIZE * mapSize.height + rand() % 400);
  } else if (where == 1) {
    pos = Vec2(TILE_SIZE * mapSize.width + rand() % 400,
               rand() % (int)(TILE_SIZE * mapSize.height));
  } else if (where == 2) {
    pos = Vec2(rand() % (int)(TILE_SIZE * mapSize.width + 800) - 400,
               -rand() % 400);
  } else if (where == 3) {
    pos = Vec2(-rand() % 400, rand() % (int)(TILE_SIZE * mapSize.height));
  }
  spt->setPosition(pos);
  this->addChild(spt, -spt->getBoundingBox().origin.y);
  return spt;
}
void GameScene::purgatoryUpdate(float dt) {
  int startX = 8 * TILE_SIZE;
  Vec2 pos = Vec2(startX, (16 - (-6) - 1) * TILE_SIZE) +
             Vec2(-TILE_SIZE / 2, TILE_SIZE / 2);

  for (int i = 0; i < 4; i++) {
    Sprite *spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
    spt->setScale(0.1f, 0.2f);
    //        spt->runAction(RepeatForever::create(Waves::create(1,
    //        cocos2d::Size(15, 15), 4, 4, 10, true)));
    pos.x = 0;
    int color = 50 + rand() % 150;
    spt->setColor(Color3B(color, color, color));
    int x = (11 - rand() % 22) * TILE_SIZE;
    int midX = x * 2.5f;
    pos.x = startX;
    x += startX;
    midX += startX;
    spt->setPosition(pos);
    Vec2 targetPos = Vec2(x, (16 - (18) - 1) * TILE_SIZE) +
                     Vec2(-TILE_SIZE / 2, TILE_SIZE / 2);
    //        spt->runAction(Sequence::create(MoveTo::create(1, targetPos),
    //        SPT_REMOVE_FUNC, NULL));
    if (i == 0) {
      batch->addChild(spt);
      spt->setOpacity(150);
    } else {
    }

    ccBezierConfig config;
    config.controlPoint_1 = pos; // ccp(0 , 0); //開始位置
    config.controlPoint_2 = Vec2(
        midX, (16 - (6) - 1) *
                  TILE_SIZE); // ccp(0 , -100); //曲線のベクトル(というのかな？)
    config.endPosition = targetPos; // ccp(100,100);   //終了位置
    spt->runAction(
        Sequence::create(BezierTo::create(1, config), SPT_REMOVE_FUNC, NULL));
    spt->runAction(ScaleTo::create(1, 1, 2));
  }
}
void GameScene::tournamentSchedule(float dt) {
  if (enemyArray.size() > 0) {
    return;
  }

  if (tournamentWaveIndex == 0) {
  }
}
void GameScene::addEnemiesToMap(TMXTiledMap *map, int levelScore,
                                 bool blueKey) {
  Vec2 pos;
  int enemyCountLeft = map->getMapSize().height * map->getMapSize().width / 120;
  float dur = 2;
  if (enemyCountLeft <= 0) {
    enemyCountLeft = 1;
  }
  int tryCount = 0;
  int tryCountMax = 10;
  TMXLayer *stageLayer = map->getLayer("stage");
  while (enemyCountLeft > 0) {
    while (true) {
      // pos = Vec2(3 + rand()%(int)(map->getMapSize().width - 6),
      // map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) -
      // 3);
      pos = Vec2(3 + rand() % (int)(map->getMapSize().width - 6),
                 4 + rand() % (int)(map->getMapSize().height - 6));
      if (!!isWay(getTileGIDAt(map->getLayer("stage"), pos))) {
        break;
      }
    }

    if (blueKey && (rand() % 100 < 20 || enemyCountLeft == 1)) {
    }
    tryCount = 0;
    tryCountMax = 10;
    while (tryCount < tryCountMax) {
      tryCount++;
      if (!isWay(getTileGIDAt(stageLayer, pos))) {
        pos = Vec2(pos.x, pos.y + 1);
        if (pos.y >= map->getMapSize().height) {
          break;
        } else {
          continue;
        }
      }
      addEnemyToLoadStack(map, levelScore, pos, -1, -1, true);
      break;
    }

    enemyCountLeft--;
  }

  //    this->runAction(Sequence::create(DelayTime::create(dur),
  //    CallFunc::create(CC_CALLBACK_0(GameScene::loadEnemies, this)), NULL));
  map->getLayer("deco")->setVisible(true);

  // add Destructable
  tryCount = 0;
  tryCountMax = map->getMapSize().width * map->getMapSize().height / 15;
  bool found = false;
  while (tryCount < tryCountMax) {
    found = false;
    while (tryCount < tryCountMax) {
      tryCount++;
      pos = Vec2(3 + rand() % (int)(map->getMapSize().width - 6),
                 map->getMapSize().height / 2 +
                     rand() % (int)(map->getMapSize().height / 2) - 2);
      int tileIndex =
          getTileGIDAt(map->getLayer("stage"), Vec2(pos.x, pos.y + 1));
      if ((!isWay(tileIndex) || isOneWay(tileIndex)) &&
          getTileGIDAt(map->getLayer("stage"), pos) == 0 && tileIndex != 322) {
        found = true;
        break;
      }
    }
    if (!found) {
      continue;
    }

    Vec2 thePos = map->getPosition() +
                  Vec2(pos.x * TILE_SIZE + rand() % TILE_SIZE,
                       (map->getMapSize().height - pos.y - 1) * TILE_SIZE +
                           TILE_SIZE / 2 - 2);
    int destructableIndex = rand() % 8;
    int coinCount = 0;
    int goldPossibility = 10;
    int eng = 5;
    if (destructableIndex == 6 ||
        (destructableIndex == 7 && goldChestCount > 1)) {
      destructableIndex = 6;
      coinCount = rand() % 5;
      eng = 50;
    } else if (destructableIndex == 7) {
      goldChestCount++;
      coinCount = rand() % 5 + 15;
      goldPossibility = 30;
      eng = 150;
    }
    EnemyBase *sptDestructable = EnemyBase::createEnemy(
        UNIT_DESTRUCTABLE, eng,
        strmake("destructable%d.png", destructableIndex).c_str(), coinCount,
        goldPossibility);
    sptDestructable->secondTag = destructableIndex;
    sptDestructable->setPosition(thePos);
    destructableArray.pushBack(sptDestructable);
    batch->addChild(sptDestructable);
  }
}
void GameScene::setEmptyMap(TMXTiledMap *map) {
  int mapSizeX = map->getMapSize().width;
  int mapSizeY = map->getMapSize().height;

  TMXLayer *stageLayer = map->getLayer("stage");
  //    TMXLayer* decoLayer = map->getLayer("deco");
  int currentX = 3;
  int currentY = 4;
  int squareWidth = 10;
  Vec2 pos;

  while (currentY < mapSizeY - 3) {
    while (currentX < mapSizeX - 3) {
      pos = Vec2(currentX + rand() % squareWidth,
                 currentY + rand() % squareWidth);
      if (pos.x < map->getMapSize().width - 3 &&
          pos.y < map->getMapSize().height - 3) {
        stageLayer->setTileGID(34, pos);
      }
      pos = Vec2(currentX + rand() % squareWidth,
                 currentY + rand() % squareWidth);
      if (pos.x < map->getMapSize().width - 3 &&
          pos.y < map->getMapSize().height - 3) {
        stageLayer->setTileGID(34, pos);
      }

      pos = Vec2(currentX + rand() % squareWidth,
                 currentY + rand() % squareWidth);
      if (pos.x < map->getMapSize().width - 3 &&
          pos.y < map->getMapSize().height - 3) {
        stageLayer->setTileGID(130, pos);
      }

      pos = Vec2(currentX + rand() % squareWidth,
                 currentY + rand() % squareWidth);
      if (pos.x < map->getMapSize().width - 3 &&
          pos.y < map->getMapSize().height - 3) {
        stageLayer->setTileGID(130, pos);
      }

      /*
      if (rand()%100 < 20) {
          pos = Vec2(currentX + rand()%squareWidth, currentY +
      rand()%squareWidth); if (pos.y >= mapSizeY) { pos.y = mapSizeY - 1;
          }
          if (pos.x >= mapSizeX) {
              pos.x = mapSizeX - 1;
          }
          decoLayer->setTileGID(rand()%2==0?50:51, pos);
      }*/

      currentX += squareWidth;
    }
    currentX = 3;
    currentY += squareWidth;
  }
}

FireableBase *GameScene::addEnemyToLoadStack(TMXTiledMap *map, int levelScore,
                                              Vec2 pos, int missile,
                                              int enemyModel, bool addGround) {

  return nullptr;
}
void GameScene::loadEnemies() {
  for (auto enemy : enemiesToLoad) {
    enemyArray.pushBack(enemy);
    MovableArray.pushBack(enemy);
    fireableArray.pushBack((FireableBase *)enemy);
    enemy->setPosition(enemyPointArray->getControlPointAtIndex(0));
    enemyPointArray->removeControlPointAtIndex(0);
    addAppearEffect(enemy);
  }
  enemyPointArray->clone();
  enemiesToLoad.clear();
}
void GameScene::addAppearEffect(Sprite *parent) {
  Sprite *sptEffect = Sprite::createWithSpriteFrameName("bombEffect0.png");
  sptEffect->setPosition(Vec2(parent->getContentSize().width / 2,
                              parent->getContentSize().height / 2));
  sptEffect->runAction(RotateBy::create(1, 400));
  sptEffect->setScale(0.5);
  sptEffect->runAction(
      Sequence::create(ScaleTo::create(0.3, 1.5), ScaleTo::create(0.2, 0.1),
                       CallFuncN::create(CC_CALLBACK_1(
                           Sprite::removeFromParentAndCleanup, sptEffect)),
                       NULL));
  parent->addChild(sptEffect);
}
void GameScene::updateMiniMapForMoving() {
  //    return; // test
  if (GM->currentStageIndex == STAGE_LOBBY)
    return;
  drawMiniMapForMoving->clear();

  Vec2 startPos;
  for (auto unit : heroArray) {
    if (!unit->isBuilding) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit / 2, miniMapBit / 2);
      if (unit->isAlli) {
        drawMiniMapForMoving->drawSolidRect(
            startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::YELLOW);
      } else {
        drawMiniMapForMoving->drawSolidRect(
            startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::GREEN);
      }
    }
  }
  for (auto unit : readyHeroArray) {
    if (!unit->isBuilding) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit / 2, miniMapBit / 2);
      drawMiniMapForMoving->drawSolidRect(
          startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::YELLOW);
    }
  }

  for (auto unit : enemyArray) {
    if (!unit->isBuilding && !unit->isUnderFog) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit / 2, miniMapBit / 2);
      drawMiniMapForMoving->drawSolidRect(
          startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::RED);
    }
  }
  drawMiniMapForMoving->drawRect(
      miniMapViewRect.origin,
      Vec2(miniMapViewRect.getMaxX(), miniMapViewRect.getMaxY()),
      Color4F::WHITE);
}

void GameScene::updateMiniMapForNonMoving() {
  //    return; // test
  if (GM->currentStageIndex == STAGE_LOBBY)
    return;
  drawMiniMapForNonMoving->clear();

  Vec2 startPos;
  Vec2 fogCoordinate;
  Fog *fogAboveUnit;
  for (int i = 0; i < mapSize.width; i++) {
    for (int j = 0; j < mapSize.height; j++) {
      fogCoordinate = Vec2(i * TILE_SIZE / FOG_SIZE,
                           (mapSize.height - j - 1) * TILE_SIZE / FOG_SIZE);
      fogAboveUnit = fogArray.at((int)fogCoordinate.x +
                                 (int)fogCoordinate.y * (int)fogMapSize.width);
      if (fogAboveUnit->appliedState > FOG_SEEN_NOT &&
          getTileGIDAt(decoLayer, Vec2(i, j)) == 49) {
        // Building/deco blocks only — water is shown by terrain layer.
        startPos = miniMapDrawStartPos +
                   Vec2(i, mapSize.height - j - 1) * TILE_SIZE * miniMapScale;
        drawMiniMapForNonMoving->drawSolidRect(
            startPos, startPos + Vec2(miniMapBit, miniMapBit),
            Color4F(0.2f, 0.2f, 0.2f, 1));
      }
    }
  }

  for (auto fog : fogArray) {
    if (fog->appliedState == FOG_SEEN_NOT ||
        fog->appliedState == FOG_SEEN_NOT_NOW) {
      startPos = miniMapDrawStartPos +
                 Vec2(fog->getBoundingBox().origin.x * miniMapScale + 2,
                      fog->getBoundingBox().origin.y * miniMapScale + 3);
      drawMiniMapForNonMoving->drawSolidRect(
          startPos,
          startPos + Vec2(miniMapBit * FOG_SIZE / TILE_SIZE,
                          miniMapBit * FOG_SIZE / TILE_SIZE),
          fog->appliedState == FOG_SEEN_NOT ? Color4F::BLACK
                                            : Color4F(0, 0, 0, 0.5f));
    }
  }

  for (auto unit : mutualArray) {
    if (!unit->isBuilding)
      continue;
    float x = unit->getPositionX() / FOG_SIZE;
    float y = unit->getPositionY() / FOG_SIZE;
    if (x < 0) {
      x = 0;
    }
    if (y < 0) {
      y = 0;
    }
    fogCoordinate = Vec2(x, y);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size()) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);
    if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit * unit->buildingOccupySize.width / 2,
                      miniMapBit * unit->buildingOccupySize.width / 2);
      drawMiniMapForNonMoving->drawSolidRect(
          startPos,
          startPos + Vec2(miniMapBit * unit->buildingOccupySize.width,
                          miniMapBit * unit->buildingOccupySize.width),
          Color4F::YELLOW);
    }
  }
  for (auto unit : heroArray) {
    if (!unit->isBuilding)
      continue;
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size()) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);
    if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit * unit->buildingOccupySize.width / 2,
                      miniMapBit * unit->buildingOccupySize.width / 2);

      if (unit->isAlli) {
        drawMiniMapForNonMoving->drawSolidRect(
            startPos,
            startPos + Vec2(miniMapBit * unit->buildingOccupySize.width,
                            miniMapBit * unit->buildingOccupySize.width),
            Color4F::YELLOW);
      } else {
        drawMiniMapForNonMoving->drawSolidRect(
            startPos,
            startPos + Vec2(miniMapBit * unit->buildingOccupySize.width,
                            miniMapBit * unit->buildingOccupySize.width),
            Color4F::GREEN);
      }
    }
  }
  for (auto unit : enemyArray) {
    if (!unit->isBuilding)
      continue;
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size()) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);
    if (!unit->isUnderFog) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit * unit->buildingOccupySize.width / 2,
                      miniMapBit * unit->buildingOccupySize.width / 2);
      drawMiniMapForNonMoving->drawSolidRect(
          startPos,
          startPos + Vec2(miniMapBit * unit->buildingOccupySize.width,
                          miniMapBit * unit->buildingOccupySize.width),
          Color4F::RED);
    }
  }

  // Draw trees as dark green dots. Trees are in mutualArray with isBuilding=false
  // so they're skipped by the mutualArray building loop above. Drawing them here
  // means the dots disappear automatically when a tree is cut (unit removed).
  for (auto unit : mutualArray) {
    if (unit->unitType != UNIT_TREE && unit->unitType != UNIT_TREE_FOR_BATTLE)
      continue;
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= (int)fogArray.size())
      continue;
    fogAboveUnit = fogArray.at(fogIndex);
    if (fogAboveUnit->appliedState > FOG_SEEN_NOT) {
      startPos = miniMapDrawStartPos + unit->getPosition() * miniMapScale -
                 Vec2(miniMapBit / 2, miniMapBit / 2);
      drawMiniMapForNonMoving->drawSolidRect(
          startPos, startPos + Vec2(miniMapBit, miniMapBit),
          Color4F(0.10f, 0.32f, 0.10f, 1.0f)); // dark green
    }
  }
}

float GameScene::getMin(float src, float dst) { return src < dst ? src : dst; }
float GameScene::getMax(float src, float dst) { return src > dst ? src : dst; }
void GameScene::setStage(TMXTiledMap *tileMap) {
  //    addGold(1500);
  //    addLumber(1000); // test
  if (isMultiplay && isMultitest) { // test
    addGold(1500);
    addLumber(1000);
  }
  if (GM->currentStageIndex != STAGE_LOBBY) {
    addGold(0);
    addLumber(0);
    bool isPremium = UDGetBool(KEY_PREMIUM_START, false);
    if (GM->market == MARKET_SMARTPASS) {
      isPremium = true;
    }
    //        isPremium = true;// test
    std::string iapList = UDGetStr(KEY_IAP_LIST, "");
    if (iapList.find(IAP_DETAIL_PREMIUM_RETRY) != std::string::npos) {
      isPremium = true;
      UDSetBool(KEY_PREMIUM_START, true);
    }
    bool isLoading = GM->isLoadingGame;
    bool isYoutuber = UDGetStr(KEY_SAVED_ID, "-1").compare("279685") == 0;
    if (isPremium && !isLoading && !isYoutuber && !isMultiplay) { // test
      addGold(1500);
      addLumber(1000);
    }
    //        addGold(10000); // test
    //        addLumber(10000); // test
  }

  GM->mapSize = tileMap->getMapSize();

  if (tileMap->getTag() == MAP_SHOP) {
    GameManager::getInstance()->showDisposableMessage(
        LanguageManager::getInstance()->getText("found a shop").c_str(),
        GameManager::getInstance()->getHudLayer());
  } else if (tileMap->getTag() == MAP_WEAPON) {
    GameManager::getInstance()->showDisposableMessage(
        LanguageManager::getInstance()->getText("suitcase room").c_str(),
        GameManager::getInstance()->getHudLayer());
  }
  float laserTrapDelay = 0;
  float missileTrapDelay = 0;
  tileMap->getChildByTag(TAG_STAGE_LAYER)->setOpacity(255);
  //    tileMap->getChildByTag(TAG_BACK_LAYER)->setOpacity(255);
  //    tileMap->getChildByTag(TAG_DECO_LAYER)->setOpacity(255);

  ValueVector rows = GameManager::getInstance()->split(strStage, "_");
  int mapIndex = 0;
  int x = -1;
  int y = -1;
  Vec2 checkPoint = Vec2(14, 8);
  if (rows.size() > 2) {
    mapIndex = rows.at(0).asInt();
    x = rows.at(1).asInt();
    y = rows.at(2).asInt();
  }

  // change coin tile into coin animation
  int totalWidth = tileMap->getMapSize().width;
  int totalHeight = tileMap->getMapSize().height;
  Vec2 point;
  int talkCount = 0;

  TMXLayer *stageLayer = tileMap->getLayer("stage");
  soilLayer = tileMap->getLayer("soil");
  TMXLayer *unitLayer = tileMap->getLayer("unit");
  decoLayer = tileMap->getLayer("deco");
  if (decoLayer)
    decoLayer->setVisible(false); // test
  //    TMXLayer* foreLayer = tileMap->getLayer("fore");
  TMXLayer *backLayer = tileMap->getLayer("back");
  PointArray *pointArrayToRemoveInUnitLayer = PointArray::create(3000);

  //    for (int i = 0; i < tileMap->getMapSize().width; i++) {
  //        for (int j = 0; j < tileMap->getMapSize().height; j++) {
  //            if (!isWay(stageLayer->getTileGIDAt(Vec2(i, j)))) {
  //                stageTileRectArray.push_back(cocos2d::Rect(i*TILE_SIZE +
  //                tileMap->getPositionX(), (tileMap->getMapSize().height - j -
  //                1)*TILE_SIZE + tileMap->getPositionY(), TILE_SIZE,
  //                TILE_SIZE));
  //            }
  //        }
  //    }

  Vec2 cameraPos = Vec2::ZERO;

  for (int i = 0; i < totalWidth; i++) {
    for (int j = 0; j < totalHeight; j++) {
      point = Vec2(i, j);
      int gid = getTileGIDAt(unitLayer, point);
      Vec2 pos = getPositionFromTileCoordinate(i, j);
      if (gid && !GM->isLoadingGame) {
        //                if(!keyExist(map, "MovingPlatformLine") &&
        //                   !keyExist(map, "MovingPlatformEnd")){
        //                    unitLayer->removeTileAt(point);
        //                }
        EnemyBase *enemy = NULL;
        FireableBase *fEnemy = NULL;
        //                log("gid: %d", gid);

        if (gid == 61) {
          int blockerIndex = blockerArray.size();
          bool isBlocked = UDGetBool(
              strmake("blocker_on_%d_%d_%d_%d", mapIndex, x, y, blockerIndex)
                  .c_str(),
              blockerIndex == 2);
          Sprite *sptBlock = Sprite::createWithSpriteFrameName(
              isBlocked ? "blocker0.png" : "blocker5.png");
          decoLayer->setTileGID(isBlocked ? 138 : 0, point);
          batch->addChild(sptBlock, 1);
          sptBlock->setPosition(pos);
          sptBlock->setTag(blockerIndex);
          sptBlock->setLocalZOrder(
              isBlocked ? -sptBlock->getBoundingBox().origin.y - 15 : -1000);
          blockerArray.pushBack(sptBlock);
          sptBlock->setName(strmake("%d_%d", i, j));
        }
        //                else if(gid == 62){
        //                    int triggerIndex = triggerArray.size();
        //                    bool isOn =
        //                    UDGetBool(strmake("blocker_on_%d_%d_%d_%d",
        //                    mapIndex, x, y, triggerIndex).c_str(),
        //                    triggerIndex == 2); Sprite* sptTrigger =
        //                    Sprite::createWithSpriteFrameName("trigger.png");
        //                    sptTrigger->setFlippedX(isOn);
        //                    batch->addChild(sptTrigger, 1);
        //                    sptTrigger->setPosition(pos);
        //                    sptTrigger->setTag(triggerIndex);
        //                    triggerArray.pushBack(sptTrigger);
        //                }
        else if (gid == 6) { // catapult
          createUnit(UNIT_CATAPULT, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "catapult");
        } else if (gid == 71) { // catapult enemy
          createUnit(UNIT_CATAPULT, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "catapult");
        } else if (gid == 7) { // helicopter
          createUnit(UNIT_HELICOPTER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "helicopter");
        } else if (gid == 72) { // helicopter ENEMY
          createUnit(UNIT_HELICOPTER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "helicopter");
        } else if (gid == 8) { // worker
          int side = WHICH_SIDE_HERO;
          int unitIndex = UNIT_WORKER;
          std::string strName = "worker";
          if (isMultiplay) {
            if (MM->isHost) {
              side = WHICH_SIDE_HERO;
              if (MM->heroRace == 1) {
                unitIndex = UNIT_GOBLIN_WORKER;
              }
            } else {
              unitLayer->removeTileAt(point);
              continue;
              //                            side = WHICH_SIDE_ENEMY;
            }
          }

          EnemyBase *justCreated = createUnit(unitIndex, side, ITS_NOT_BUILDING,
                                              pos, GM->getUnitName(unitIndex));
          if (isMultiplay) {
            MM->createdUnit(justCreated->unitID, justCreated->unitType,
                            (int)pos.x, (int)pos.y, 0);

            if (isMultitest) {
              for (int i = 0; i < 5; i++) { // test
                justCreated = createUnit(UNIT_SWORDMAN, side, ITS_NOT_BUILDING,
                                         pos, GM->getUnitName(UNIT_SWORDMAN));
                MM->createdUnit(justCreated->unitID, justCreated->unitType,
                                (int)pos.x, (int)pos.y, 0);
              }
            }
          }
        } else if (gid == 65) { // worker ready
          int side = WHICH_SIDE_READY_HERO;
          int unitIndex = UNIT_WORKER;
          if (isMultiplay) {
            if (MM->isHost) {
              unitLayer->removeTileAt(point);
              continue;
              //                            side = WHICH_SIDE_ENEMY;
            } else {
              side = WHICH_SIDE_HERO;
              if (MM->heroRace == 1) {
                unitIndex = UNIT_GOBLIN_WORKER;
              }
            }
          }
          EnemyBase *justCreated = createUnit(unitIndex, side, ITS_NOT_BUILDING,
                                              pos, GM->getUnitName(unitIndex));
          if (isMultiplay) {
            MM->createdUnit(justCreated->unitID, justCreated->unitType,
                            (int)pos.x, (int)pos.y, 0);

            if (isMultitest) {
              for (int i = 0; i < 5; i++) { // test
                justCreated = createUnit(UNIT_SWORDMAN, side, ITS_NOT_BUILDING,
                                         pos, GM->getUnitName(UNIT_SWORDMAN));
                MM->createdUnit(justCreated->unitID, justCreated->unitType,
                                (int)pos.x, (int)pos.y, 0);
              }
            }
          }
        } else if (gid == 73) { // worker enemy
          int side = WHICH_SIDE_ENEMY;
          createUnit(UNIT_WORKER, side, ITS_NOT_BUILDING, pos, "worker");
        } else if (gid == 91) { // orc worker
          createUnit(UNIT_GOBLIN_WORKER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "orc worker");
          //                    createUnit(UNIT_WIZARD, WHICH_SIDE_HERO,
          //                    ITS_NOT_BUILDING, pos, "wizard"); // test
        } else if (gid == 9) { // swordman
          createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "swordman");
        } else if (gid == 66) { // swordman ready
          createUnit(UNIT_SWORDMAN, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING,
                     pos, "swordman");
        } else if (gid == 74) { // swordman enemy
          createUnit(UNIT_SWORDMAN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "swordman");
        } else if (gid == 10) { // archer
          createUnit(UNIT_ARCHER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "archer");
        } else if (gid == 67) { // archer ready
          createUnit(UNIT_ARCHER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos,
                     "archer");
          log("archer ready");
        } else if (gid == 75) { // archer enemy
          createUnit(UNIT_ARCHER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "archer");
        } else if (gid == 5) { // hero
          EnemyBase *hero = createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO,
                                       ITS_NOT_BUILDING, pos, "swordman");
          hero->setName("hero");
          // this->setPosition(-pos + Vec2(size.width/2 + TILE_SIZE,
          // size.height*2/4));
          npcArray.pushBack(hero);
        } else if (gid == 62) { // support pos
          //                    this->setPosition(-pos + Vec2(size.width/2 +
          //                    TILE_SIZE, size.height*2/4));
          supportPointArray->addControlPoint(pos);
        } else if (gid == 63) { // camera init pos
          if (!isMultiplay) {
            this->setPosition(
                -pos * layerScale +
                Vec2(size.width / 2 + TILE_SIZE * layerScale, size.height / 2));
            log("camera init pos");
          }
        } else if (gid == 4) { // orc chief
          EnemyBase *unit = createUnit(
              UNIT_ORC_AXE, gid == 88 ? WHICH_SIDE_HERO : WHICH_SIDE_MUTUAL,
              ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_AXE));
          npcArray.pushBack(unit);
          unit->setName("orc");
          if (GM->nextScene == STAGE_INTRO) {
            unit->setFlippedX(true);
          }
        } else if (gid == 14) { // undead
          EnemyBase *unit = createUnit(UNIT_LAMINGTON, WHICH_SIDE_ENEMY,
                                       ITS_NOT_BUILDING, pos, "wizard");
          unit->setName("undead");
          npcArray.pushBack(unit);
        } else if (gid == 16 || gid == 84) { // troll
          EnemyBase *unit = createUnit(
              UNIT_TROLL, gid == 84 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_NOT_BUILDING, pos, "troll");
          if (gid == 84) {
            unit->isAlli = true;
          }
        } else if (gid == 116) { // orc worker ready
          createUnit(UNIT_GOBLIN_WORKER, WHICH_SIDE_READY_HERO,
                     ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_GOBLIN_WORKER));
        } else if (gid == 106) { // troll ready
          createUnit(UNIT_TROLL, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_TROLL));
        } else if (gid == 124) { // troll hero
          createUnit(UNIT_TROLL, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "troll");
        } else if (gid == 36) { // troll schedule
          createUnit(UNIT_TROLL, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "troll")
              ->scheduledAttackTime = 60 * 5;
        } else if (gid == 92) { // wizard hero
          createUnit(UNIT_WIZARD, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_WIZARD));
        } else if (gid == 117) { // wizard ready
          createUnit(UNIT_WIZARD, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_WIZARD));
        } else if (gid == 118) { // wizard enemy
          createUnit(UNIT_WIZARD, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_WIZARD));
        } else if (gid == 17 || gid == 85) { // goblin bomb
          EnemyBase *unit = createUnit(
              UNIT_GOBLIN_BOMB,
              gid == 85 ? WHICH_SIDE_HERO : (WHICH_SIDE_ENEMY),
              ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_BOMB));
          if (gid == 85) {
            unit->isAlli = true;
          }
        } else if (gid == 125) { // goblin bomb hero
          createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_GOBLIN_BOMB));
        } else if (gid == 37) { // goblin bomb schedule
          createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_GOBLIN_BOMB))
              ->scheduledAttackTime = 60 * 2.5f;
        } else if (gid == 107) { // goblin bomb ready
          createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING,
                     pos, GM->getUnitName(UNIT_GOBLIN_BOMB));
        } else if (gid == 18 || gid == 86) { // goblin
          EnemyBase *unit = createUnit(
              UNIT_GOBLIN, gid == 86 ? WHICH_SIDE_HERO : (WHICH_SIDE_ENEMY),
              ITS_NOT_BUILDING, pos, "goblin");
          if (gid == 86) {
            unit->isAlli = true;
          }
        } else if (gid == 126) { // goblin hero
          createUnit(UNIT_GOBLIN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     "goblin");
        } else if (gid == 38) { // goblin schedule
          createUnit(UNIT_GOBLIN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     "goblin")
              ->scheduledAttackTime = 60 * 2;
        } else if (gid == 108) { // goblin ready
          createUnit(UNIT_GOBLIN, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_GOBLIN));
        } else if (gid == 19 || gid == 28 || gid == 27 || gid == 39 ||
                   gid == 88) { // orc axe
          int vary = 40;
          EnemyBase *unit = createUnit(
              UNIT_ORC_AXE, gid == 88 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_NOT_BUILDING,
              pos + Vec2(rand() % vary - vary / 2, rand() % vary - vary),
              GM->getUnitName(UNIT_ORC_AXE));

          if (GM->nextScene == STAGE_INTRO) {
            unit->setFlippedX(true);
          }
          if (gid == 88) {
            unit->isAlli = true;
          }
          if (gid == 28) {
            unit->scheduledAttackTime = 60;
          } else if (gid == 27) {
            unit->scheduledAttackTime = 60 * 3;
          } else if (gid == 39) {
            unit->scheduledAttackTime = 60 * 5;
          }
        } else if (gid == 127) { // orc axe hero
          int vary = 40;
          createUnit(UNIT_ORC_AXE, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                     pos + Vec2(rand() % vary - vary / 2, rand() % vary - vary),
                     GM->getUnitName(UNIT_ORC_AXE));
        } else if (gid == 109) { // orc axe ready
          createUnit(UNIT_ORC_AXE, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_ORC_AXE));
        } else if (gid == 20 || gid == 87) { // orc spear
          EnemyBase *unit = createUnit(
              UNIT_ORC_SPEAR, gid == 87 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_SPEAR));
          if (gid == 87) {
            unit->isAlli = true;
          }
          if (GM->nextScene == STAGE_INTRO) {
            unit->setFlippedX(true);
          }
        } else if (gid == 110) { // worker ready
          createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING,
                     pos, GM->getUnitName(UNIT_ORC_SPEAR));
        } else if (gid == 128) { // orc spear hero
          createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_ORC_SPEAR));
        } else if (gid == 26) { // orc spear schedule
          createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_ORC_SPEAR))
              ->scheduledAttackTime = 60 * 3;
        } else if (gid == 69 || gid == 89) { // bunker
          EnemyBase *unit = createUnit(
              UNIT_ORC_BUNKER, gid == 89 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_BUILDING, pos + Vec2(50, 0), "bunker", 1, "bunker.png");
          if (gid == 89) {
            unit->isAlli = true;
          }
          unit->canFindTarget = true;
          setOccupy(pos, 2, 2, true, unit);
        } else if (gid == 81) { // temple
          EnemyBase *unit = createUnit(
              UNIT_TEMPLE, WHICH_SIDE_MUTUAL, ITS_BUILDING,
              pos + Vec2(50, 0) * imageScale, "temple", 1, "bunker.png");
          setOccupy(pos, 3, 3, true, unit);
        } else if (gid == 70 || gid == 80 || gid == 79 || gid == 78 ||
                   gid == 90 || gid == 46) { // hq
          EnemyBase *unit = createUnit(
              UNIT_ORC_HQ, gid == 90 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_BUILDING, pos + Vec2(150, -50), "hq", 1, "hq.png");
          if (gid == 90) {
            unit->isAlli = true;
          }
          unit->canFindTarget = true;
          setOccupy(pos, 4, 3, true, unit);
          Sprite *spt = Sprite::createWithSpriteFrameName("redFlag0.png");
          GM->runAnimation(spt, "redFlag", true);
          unit->addChild(spt);
          spt->setPosition(Vec2(232, 335) * imageScale);

          if (gid == 80 || gid == 79 || gid == 78) {
            unit->scheduledProductUnit = UNIT_ORC_AXE;
            if (gid == 80) {
              unit->scheduledProductUnitCount = 2;
              unit->scheduledProductTime = 60 * 1;
            } else if (gid == 79) {
              unit->scheduledProductUnit = UNIT_ORC_SPEAR;
              unit->scheduledProductUnitCount = 3;
              unit->scheduledProductTime = 60 * 1.5f;
            } else if (gid == 78) {
              unit->scheduledProductUnitCount = 5;
              unit->scheduledProductTime = 60 * 2;
            }
          }
          if (gid == 46) {
            unit->scheduledProductUnit = UNIT_ZOMBIE_ORC_AXE;
            unit->scheduledProductUnitCount = 1;
            unit->scheduledProductTime = 30;
          }
        } else if (gid == 93) { // hq hero
          EnemyBase *unit =
              createUnit(UNIT_CASTLE, WHICH_SIDE_HERO, ITS_BUILDING,
                         pos + Vec2(150, -50) * imageScale, "hq", 1, "hq.png");
          unit->startProductSchedule();
          unit->canFindTarget = true;
          setOccupy(pos, 4, 3, true, unit);
          Sprite *spt = Sprite::createWithSpriteFrameName("redFlag0.png");
          GM->runAnimation(spt, "redFlag", true);
          unit->addChild(spt);
          spt->setPosition(Vec2(232, 335) * imageScale);
        } else if (gid == 68) { // factory
          EnemyBase *unit = createUnit(
              UNIT_FACTORY, WHICH_SIDE_HERO, ITS_BUILDING,
              pos + Vec2(100, -100) * imageScale, "factory", 1, "factory.png");
          setOccupy(pos, 3, 3, true, unit);
          unit->isBuildingComplete = true;
          unit->startProductSchedule();
        } else if (gid == 54 || gid == 94) { // farm
          int side = WHICH_SIDE_HERO;
          if (gid == 94) {
            side = WHICH_SIDE_ENEMY;
          }
          EnemyBase *unit =
              createUnit(UNIT_FARM, side, ITS_BUILDING, pos + Vec2(100, -0),
                         "farm", 1, "farm.png");
          addFoodMax(getFoodGive(UNIT_FARM));
          setOccupy(pos, 3, 2, true, unit);
          unit->isBuildingComplete = true;
          unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
          unit->smokePoint = unit->getPosition() + Vec2(-18, 110);
        } else if (gid == 94) { // farm ENEMY
          EnemyBase *unit = createUnit(
              UNIT_FARM, WHICH_SIDE_ENEMY, ITS_BUILDING,
              pos + Vec2(100, -0) * imageScale, "farm", 1, "farm.png");
          unit->isBuildingComplete = true;
          unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
          unit->smokePoint = unit->getPosition() + Vec2(-18, 110) * imageScale;
        } else if (gid == 55 || gid == 95) { // lumber mill
          int side = WHICH_SIDE_HERO;
          if (gid == 95) {
            side = WHICH_SIDE_ENEMY;
          }
          EnemyBase *unit = createUnit(UNIT_LUMBERMILL, side, ITS_BUILDING,
                                       pos + Vec2(100, -100) * imageScale,
                                       "lumberMill", 1, "lumberMill.png");
          setOccupy(pos, 3, 3, true, unit);
          unit->isBuildingComplete = true;
        } else if (gid == 56 || gid == 96) { // airport
          int side = WHICH_SIDE_HERO;
          if (gid == 96) {
            side = WHICH_SIDE_ENEMY;
          }
          EnemyBase *unit = createUnit(UNIT_AIRPORT, side, ITS_BUILDING,
                                       pos + Vec2(100, -100) * imageScale,
                                       "airport", 1, "airport.png");
          setOccupy(pos, 3, 3, true, unit);
          unit->isBuildingComplete = true;
          unit->startProductSchedule();
          Sprite *spt =
              Sprite::createWithSpriteFrameName("airportPropeller0.png");
          GM->runAnimation(spt, "propeller", true);
          unit->addChild(spt);
          spt->setPosition(Vec2(134, 206) * imageScale);
        } else if (gid == 57) { // mine
          EnemyBase *unit = createUnit(
              UNIT_MINE, WHICH_SIDE_MUTUAL, ITS_BUILDING,
              pos + Vec2(100, -100) * imageScale, "mine", 1, "mine.png");
          setOccupy(pos, 3, 3, true, unit);
          unit->setTag(0);
          unit->unitID = mutualID++;
        } else if (gid == 58 || gid == 98) { // barracks
          int side = WHICH_SIDE_HERO;
          if (gid == 98) {
            side = WHICH_SIDE_ENEMY;
          }
          EnemyBase *unit = createUnit(UNIT_BARRACKS, side, ITS_BUILDING,
                                       pos + Vec2(100, -100) * imageScale,
                                       "barracks", 1, "barracks.png");
          setOccupy(pos, 3, 3, true, unit);
          unit->isBuildingComplete = true;
          unit->startProductSchedule();
        } else if (gid == 98) { // barracks ENEMY
          EnemyBase *unit =
              createUnit(UNIT_BARRACKS, WHICH_SIDE_ENEMY, ITS_BUILDING,
                         pos + Vec2(100, -100) * imageScale, "barracks", 1,
                         "barracks.png");
          unit->isBuildingComplete = true;
          unit->startProductSchedule();
        } else if (gid == 59 || gid == 40 || gid == 64 || gid == 99) { // castle
          int side = WHICH_SIDE_HERO;
          int unitIndex = UNIT_CASTLE;
          int unitIndexForSprite = UNIT_CASTLE;
          if (gid == 40 || gid == 99) {
            side = WHICH_SIDE_ENEMY;
          } else if (gid == 59 && isMultiplay) {
            if (MM->isHost) {
              this->setPosition(-pos * layerScale +
                                Vec2(size.width / 2 + TILE_SIZE * layerScale,
                                     size.height / 2));
              side = WHICH_SIDE_HERO;
              if (MM->heroRace == 1) {
                //                                unitIndex = UNIT_CASTLE;
                unitIndexForSprite = UNIT_ORC_HQ;
              }
              addFoodMax(getFoodGive(unitIndex));
            } else {
              unitLayer->removeTileAt(point);
              continue;
              //                            side = WHICH_SIDE_ENEMY;
              //                            if (MM->enemyRace == 1) {
              //                                unitIndex = UNIT_ORC_HQ;
              //                            }
            }
          } else if (gid == 64 && isMultiplay) {
            if (MM->isHost) {
              unitLayer->removeTileAt(point);
              continue;
              //                            side = WHICH_SIDE_ENEMY;
              //                            if (MM->enemyRace == 1) {
              //                                unitIndex = UNIT_ORC_HQ;
              //                            }
            } else {
              this->setPosition(-pos * layerScale +
                                Vec2(size.width / 2 + TILE_SIZE * layerScale,
                                     size.height / 2));
              side = WHICH_SIDE_HERO;
              if (MM->heroRace == 1) {
                //                                unitIndex = UNIT_ORC_HQ;
                unitIndexForSprite = UNIT_ORC_HQ;
              }
              addFoodMax(getFoodGive(unitIndex));
            }
          } else if (gid == 64) {
            side = WHICH_SIDE_READY_HERO;
          } else {
            addFoodMax(getFoodGive(UNIT_CASTLE));
          }
          if (gid == 40) {
            unitIndex = UNIT_ZOMBIE_CASTLE;
          }
          Vec2 newPos = pos + Vec2(150, -100) * imageScale;
          newPos.x = (int)newPos.x;
          newPos.y = (int)newPos.y;
          EnemyBase *unit = createUnit(
              unitIndex, side, ITS_BUILDING, newPos, GM->getUnitName(unitIndex),
              1, getSpriteNameForUnit(unitIndexForSprite));

          if (isMultiplay) {
            if (MM->heroRace == 1) {
              unit->unitName = "hq";
            }
            MM->createdUnit(unit->unitID, unitIndexForSprite, (int)newPos.x,
                            (int)newPos.y, 0);
          }
          if (gid == 59) {
            //                        this->setPosition(-pos + Vec2(size.width/2
            //                        - TILE_SIZE*1, size.height*3/4));
          }

          if (getPositionY() > 0) {
            setPositionY(0);
          }
          setOccupy(pos, 4, 3, true, unit);

          unit->startProductSchedule();
          unit->isBuildingComplete = true;

          if (unitIndexForSprite == UNIT_CASTLE) {
            Sprite *spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
            GM->runAnimation(spt, "blueFlag", true);
            unit->addChild(spt);
            spt->setPosition(Vec2(192, 255) * imageScale);
          } else if (unitIndexForSprite == UNIT_ORC_HQ) {
            Sprite *spt = Sprite::createWithSpriteFrameName("redFlag0.png");
            GM->runAnimation(spt, "redFlag", true);
            unit->addChild(spt);
            spt->setPosition(Vec2(232, 335) * imageScale);
          }

          if (gid == 40) {
            unit->scheduledProductUnit = UNIT_ZOMBIE_SWORDSMAN;
            unit->scheduledProductUnitCount = 1;
            unit->scheduledProductTime = 25;
          }
        } else if (gid == 60 || gid == 100) { // watcher tower
          EnemyBase *unit = createUnit(
              UNIT_WATCHERTOWER, gid == 60 ? WHICH_SIDE_HERO : WHICH_SIDE_ENEMY,
              ITS_BUILDING, pos + Vec2(50, 0) * imageScale, "watcherTower", 1,
              "watcherTower.png");
          unit->canFindTarget = true;
          setOccupy(pos, 2, 2, true, unit);
          unit->isBuildingComplete = true;
        } else if (gid == 30 || gid == 77) { // zombie swordsman
          EnemyBase *unit = createUnit(UNIT_ZOMBIE_SWORDSMAN, WHICH_SIDE_ENEMY,
                                       ITS_NOT_BUILDING, pos,
                                       GM->getUnitName(UNIT_ZOMBIE_SWORDSMAN));
          unit->isZombie = gid == 30;
          unit->oneSec = 1 + (rand() % 10) * 0.1f;
          unit->extraSpeed = unit->extraSpeed - rand() % 60;
        } else if (gid == 29 || gid == 76) { // zombie orc
          EnemyBase *unit = createUnit(UNIT_ZOMBIE_ORC_AXE, WHICH_SIDE_ENEMY,
                                       ITS_NOT_BUILDING, pos,
                                       GM->getUnitName(UNIT_ZOMBIE_ORC_AXE));
          unit->isZombie = gid == 29;
          unit->oneSec = 1 + (rand() % 10) * 0.1f;
          unit->extraSpeed = unit->extraSpeed - rand() % 60;
        } else if (gid == 129) { // hero orc general
          createUnit(UNIT_HERO_ORC, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos,
                     GM->getUnitName(UNIT_HERO_ORC));
        } else if (gid == 130) { // heros pos
          heroPointArray->addControlPoint(pos);
        }

        if ((enemy != NULL && enemy->energy > 0) ||
            (fEnemy != NULL && fEnemy->energy > 0)) {
          enemyMaxCount++;
          EnemyBase *theEnemy;
          if (fEnemy != NULL) {
            theEnemy = fEnemy;
          } else if (enemy != NULL) {
            theEnemy = enemy;
          }

          if (theEnemy != NULL) {
            if (mastery >= 7) {
              theEnemy->energy *= 4;
            } else {
              theEnemy->energy = theEnemy->energy * (1 + 0.3 * mastery);
            }
            theEnemy->maxEnergy = theEnemy->energy;
            addAppearEffect(theEnemy);
          }
        }
      }
      if (gid) {
        unitLayer->removeTileAt(point);
      }

      if (decoLayer) {
        gid = getTileGIDAt(decoLayer, point);
        if (gid && !GM->isLoadingGame) {
          Vec2 pos = tileMap->getPosition() +
                     Vec2(i * TILE_SIZE + TILE_SIZE / 2,
                          (totalHeight - j - 1) * TILE_SIZE) +
                     Vec2(0, TILE_SIZE / 2);
          if (gid == 50) { // tree
            EnemyBase *unit = createUnit(
                UNIT_TREE, WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING, pos, "tree", 1,
                strmake("tree2_%d.png", rand() % 5).c_str());
            unit->canMove = false;
            unit->unitID = mutualID++;
            // test
            //                        for(int i = 0; i < 3; i++){
            //                            Sprite* spt =
            //                            Sprite::createWithSpriteFrameName(strmake("tree2_%d.png",
            //                            rand()%5)); batch->addChild(spt);
            //                            if(i==0){
            ////                                spt->setPosition(pos + Vec2(-20
            ///+ rand()%40, -20 + rand()%40 + 20));
            //                            }else{
            ////                                spt->setPosition(pos + Vec2(-50
            ///+ rand()%100, -50 + rand()%100 + 20));
            //                            }
            //                            spt->setPosition(pos + Vec2(-20 +
            //                            rand()%40, -20 + rand()%40 +
            //                            20)*imageScale);
            //                        spt->setLocalZOrder(-spt->getBoundingBox().getMinY());//.origin.y);
            //                            unit->childrenSprite.pushBack(spt);
            //                        }

            decoLayer->setTileGID(49, point);
          }
        }
      }
      if (rand() % 2 == 0) {
        int soilGid = getTileGIDAt(soilLayer, point);
        if (soilGid > 0 &&
            soilGid < 130) { // change decorationed soil to plain soil
          soilLayer->setTileGID(soilGid + 70, point);
        }
      }
      int stageGid = getTileGIDAt(stageLayer, point);
      if (stageGid >= 1 && stageGid <= 24) {
        int grass = rand() % 12;
        switch (grass) {
        case 0:
          stageLayer->setTileGID(1, point);
          break;
        case 1:
          if (rand() % 3 == 0)
            stageLayer->setTileGID(2, point);
          else
            stageLayer->setTileGID(21, point);
          break;
        case 2:
          stageLayer->setTileGID(3, point);
          break;
        case 3:
          stageLayer->setTileGID(11, point);
          break;
        case 4:
          stageLayer->setTileGID(12, point);
          break;
        case 5:
          stageLayer->setTileGID(13, point);
          break;
        case 6:
          stageLayer->setTileGID(21, point);
          break;
        case 7:
          stageLayer->setTileGID(22, point);
          break;
        case 8:
          stageLayer->setTileGID(23, point);
          break;
        case 9:
          stageLayer->setTileGID(24, point);
          break;
        default:
          stageLayer->setTileGID(24, point);
          break;
        }
      }

      //            int num = rand()%211 - 100;
      //            Sprite* spt = nullptr;
      //            if (soilLayer && soilLayer->getTileGIDAt(point) > 0) {
      //                if (soilLayer->getTileGIDAt(point) == 42) {
      //                    if (num > 100) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("stone3.png");
      //                    }else if (num > 90) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("stone2.png");
      //                    }else if (num > 80) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("stone1.png");
      //                    }else if (num > 70) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("stone0.png");
      //                    }else if (num > 60) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("grassOnLand1.png");
      //                    }else if (num > 50) {
      //                        spt =
      //                        Sprite::createWithSpriteFrameName("grassOnLand0.png");
      //                    }
      //                }
      //            }else{
      //                if (num > 100) {
      //                    spt =
      //                    Sprite::createWithSpriteFrameName("flowerOnGrass.png");
      //                }else if (num > 30) {
      //                    spt =
      //                    Sprite::createWithSpriteFrameName("grassOnGrass2.png");
      //                }else if (num > -20) {
      //                    spt =
      //                    Sprite::createWithSpriteFrameName("grassOnGrass1.png");
      //                }else if (num > -70) {
      //                    spt =
      //                    Sprite::createWithSpriteFrameName("grassOnGrass0.png");
      //                }
      //            }
      //            if (spt != nullptr) {
      //                batch->addChild(spt);
      //                spt->setPosition(pos + Vec2(-50 + rand()%100, -50 +
      //                rand()%100)); spt->setLocalZOrder(-100000);
      //            }
    }
  }

  if (GM->isLoadingGame) {
    loadGame();
    //        GM->isLoadingGame = false; // moved to bottom
  }

  CCLOG("setStage map done");
  TMXObjectGroup *objectGroup = tileMap->getObjectGroup("Object");
  CCLOG("setStage object map done");
  if (pointArrayToRemoveInUnitLayer) {
    for (int i = 0; i < pointArrayToRemoveInUnitLayer->count(); i++) {
      Vec2 pos = pointArrayToRemoveInUnitLayer->getControlPointAtIndex(i);
      unitLayer->removeTileAt(pos);
    }
  }

  this->schedule(schedule_selector(GameScene::gravityUpdate));
  //    this->schedule(schedule_selector(GameScene::gravityUpdateForStraight));
  //    this->schedule(schedule_selector(GameScene::gravityUpdateForCustomMoving));
  //    this->schedule(schedule_selector(GameScene::gravityUpdateForFlyingOrSwimingEnemies));

  //    this->schedule(schedule_selector(GameScene::bubbleUpdate));

  extraCritical = 0;
  extraPower = 0;
  extraFruit = 0;

  GameManager::getInstance()->isVideoRewardEnergy = false;
  GameManager::getInstance()->isVideoRewardAttack = false;
  GameManager::getInstance()->isVideoRewardCoins = false;

  tileMap->setTag(MAP_FOUND);
  float dur = 2;
  if (enemiesToLoad.size() > 0) {
    this->runAction(Sequence::create(
        DelayTime::create(dur),
        CallFunc::create(CC_CALLBACK_0(GameScene::loadEnemies, this)), NULL));
  }

  resetPathState();
  isSetStageDone = true;

  // test
  draw->clear();

  for (int i = 0; i < 6; i++) {
    HUD->setMenu(i, BTN_TYPE_NONE);
  }
  //    addGold(12000); // test
  //    addLumber(12000); // test

  makeZombiesAttack();

  float rowCounter = 1;
  int columnCounter = 0;
  int startX = -getPositionX() + 100;
  int startY = -getPositionY() - 400;
  int birdGapX = 90;
  int birdGapY = 90;
  int columnOffset = 100;
  dur = 2;
  for (int i = 0; i < 20; i++) {
    Sprite *spt = Sprite::createWithSpriteFrameName("bird0.png");
    GM->runAnimation(spt, strmake("bird%d", rand() % 3).c_str(), true);
    batch->addChild(spt);
    spt->setPosition(Vec2(startX - rowCounter * birdGapX / 2 +
                              birdGapX * columnCounter + rand() % 400 + 200,
                          startY - rowCounter * birdGapY -
                              columnCounter * columnOffset + rand() % 400 +
                              200));

    columnCounter++;
    spt->setRotation(20 + rand() % 30);
    spt->setScale(0.7f + (rand() % 60) * 0.01f);
    spt->runAction(Sequence::create(
        DelayTime::create(0.3),
        MoveBy::create(dur / spt->getScale(),
                       Vec2(2000 + rand() % 200, 2200 + rand() % 400)),
        SPT_REMOVE_FUNC, NULL));
    spt->runAction(Sequence::create(
        DelayTime::create(0.3),
        ScaleTo::create(dur, 0.7f + (rand() % 60) * 0.01f), nullptr));
    int columnMaxCount = rowCounter;
    if (rowCounter > 4) {
      columnMaxCount = 4;
    }
    if (columnCounter >= rowCounter) {
      rowCounter++;
      columnCounter = 0;
    }
  }
  this->runAction(Sequence::create(
      DelayTime::create(1.5),
      CallFunc::create(CC_CALLBACK_0(GameScene::checkBirdFly, this)),
      nullptr));

  //    totalKillUnit = 99;
  //    totalProducedUnit = 85;
  //    totalUsedGold = 22345;
  //    totalEarnedGold = 27877;
  //    totalUsedLumber = 9244;
  //    totalEarnedLumber = 11999;
  //    gameTimer = 60*5 + 53; // test
  //    endGame(); // test

  UD->flush();
  if (GM->currentStageIndex == STAGE_SINGLEPLAY ||
      GM->currentStageIndex == STAGE_RAID) {
    for (auto unit : unitsToCreateArray) {
      if (unit->alliSide == WHICH_SIDE_HERO) {
        unit->alliSide = WHICH_SIDE_ENEMY;
        unit->isEnemy = true;
      }
    }
  }

  bool isBuildingExist = false;
  for (auto unit : heroArray) {
    if (unit->isBuilding) {
      isBuildingExist = true;
    }
  }

  for (auto unit : unitsToCreateArray) {
    if (unit->alliSide == WHICH_SIDE_HERO && unit->isBuilding) {
      isBuildingExist = true;
    }
  }

  isBuildingExistWhenStartTheGame = isBuildingExist;
  if (!isBuildingExist) {
    while (heroPointArray->count() > 0) {
      heroPointArray->removeControlPointAtIndex(0);
    }
  }
  if (heroPointArray->count() > 0 && GM->nextScene != STAGE_LOBBY && HUD &&
      !HUD->isRaid && difficultyMode != DIFFICULTY_MODE_HARD) {
    //        std::string strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
    //        strEquipped = "_64/200_"; // test
    //        ValueVector units = GM->split(strEquipped, "_");
    std::vector<UnitInfo *> list = GM->getHeroDeck();
    UnitInfo *info;
    //        list.at(0)->unitType = UNIT_HERO_GOLEM; // test
    int index = 0;
    for (int i = 0; i < list.size(); i++) {
      //            if(i > 0) break; // test
      //            std::string str = units.at(i).asString();
      //            if(str.length() <= 0){
      //                continue;
      //            }
      //            if(i > 0) continue; // test
      //            info = GM->getUnitInfoFromString(units.at(i).asString());
      info = list.at(i);
      Vec2 heroPos = heroPointArray->getControlPointAtIndex(index);
      //        UnitInfo* info = getHeroAt(i);
      //            if(info != nullptr && info && units.size() >= i){
      if (info != nullptr) {
        int unitType = info->unitType;
        int unitLevel = info->level;
        if (unitType >= 0) {
          EnemyBase *createdHero =
              createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, heroPos,
                         GM->getUnitName(unitType));
          setHeroLevelInfo(createdHero, unitLevel);
          if (index > 5) {
            heroListToCreate.pushBack(createdHero);
            createdHero->setVisible(false);
            createdHero->spine->setVisible(false);
            unitsToCreateArray.eraseObject(createdHero);
          } else {
            heroList.pushBack(createdHero);
            createdHero->setLocalZOrder(
                -createdHero->getBoundingBox().getMinY());
            if (createdHero->spine) {
              createdHero->spine->setLocalZOrder(createdHero->getLocalZOrder());
            }
          }
        }
      }
      index++;
    }
  }

  if (isMultiplay) {
    MM->ping();
  }

  //    if(isMultiplay){ // test
  //        addGold(2000);
  //        addLumber(2000);
  //    }

  CCLOG("setStage done");
}
void GameScene::setHeroLevelInfo(EnemyBase *hero, int level) {
  hero->level = level;
  hero->ap = GM->getUnitATT(hero->unitType, level) *
             getUnitAttackCoolTime(hero->unitType) * (1 + hero->rank * 0.2f);
  hero->maxEnergy =
      GM->getUnitHP(hero->unitType, level) * (1 + hero->rank * 0.2f);
  hero->energy = hero->maxEnergy;
  hero->skillRate = GM->getHeroSkillRate(hero->unitType, hero->rank);

  if (hero->unitType == GM->getMonthlyHeroType()) {
    hero->ap *= 1.5f;
    hero->skillRate *= 1.5f;
  }
  if (hero->unitType == GM->getMidMonthHeroType()) {
    hero->ap *= 1.5f;
    hero->skillRate *= 1.5f;
  }

  //    hero->skillRate = 100; // test
}
void GameScene::loadGame() {
  int slot = GM->loadingSlot;
  std::string strData = UDGetStr(strmake("savedData%d", slot).c_str());
  ValueVector datas = GameManager::getInstance()->split(strData.c_str(), ",");
  //        int loadedStageIndx = datas.at(0).asInt();
  lastTick = datas.at(1).asInt();
  gameTimer = lastTick;
  addGold(datas.at(2).asInt());
  addLumber(datas.at(3).asInt());
  this->setPosition(Vec2(datas.at(4).asInt(), datas.at(5).asInt()));
  int unitType = -1;
  std::string sptName;
  for (int i = 6; i < datas.size(); i += 21) {
    unitType = datas.at(i).asInt();
    sptName = datas.at(i + 1).asString();
    if (unitType == UNIT_TREE) {
      sptName = strmake("tree2_%d.png", rand() % 5);
    }
    EnemyBase *unit =
        createUnit(unitType, datas.at(i + 5).asInt(), datas.at(i + 8).asBool(),
                   Vec2(datas.at(i + 6).asInt(), datas.at(i + 7).asInt()),
                   datas.at(i + 4).asString(), 1, sptName);
    unit->maxEnergy = datas.at(i + 2).asInt();
    unit->energy = datas.at(i + 3).asInt();
    if (unit->maxEnergy > unit->energy) {
      unit->updateEnergy();
    }
    unit->scheduledAttackTime = datas.at(i + 9).asInt();
    unit->isAlli = datas.at(i + 10).asBool();
    unit->canFindTarget = datas.at(i + 11).asBool();
    unit->scheduledProductUnit = datas.at(i + 12).asInt();
    unit->scheduledProductUnitCount = datas.at(i + 13).asInt();
    unit->scheduledProductTime = datas.at(i + 14).asInt();
    unit->isBuildingComplete = true;
    int queueIndex = datas.at(i + 15).asInt();
    if (queueIndex >= 0)
      unit->queueUnit(queueIndex);
    queueIndex = datas.at(i + 16).asInt();
    if (queueIndex >= 0)
      unit->queueUnit(queueIndex);
    queueIndex = datas.at(i + 17).asInt();
    if (queueIndex >= 0)
      unit->queueUnit(queueIndex);
    queueIndex = datas.at(i + 18).asInt();
    if (queueIndex >= 0)
      unit->queueUnit(queueIndex);
    queueIndex = datas.at(i + 19).asInt();
    if (queueIndex >= 0)
      unit->queueUnit(queueIndex);
    unit->isZombie = datas.at(i + 20).asBool();

    if (unit->unitType == UNIT_ORC_BUNKER) { // bunker
      unit->canFindTarget = true;
      setOccupy(unit->getPosition() + Vec2(-50, 0) * imageScale, 2, 2, true,
                unit);
    } else if (unit->unitType == UNIT_ORC_HQ) { // hq
      setOccupy(unit->getPosition() - Vec2(150, -50) * imageScale, 4, 3, true,
                unit);
      Sprite *spt = Sprite::createWithSpriteFrameName("redFlag0.png");
      GM->runAnimation(spt, "redFlag", true);
      unit->addChild(spt);
      spt->setPosition(Vec2(232, 335) * imageScale);
    } else if (unit->unitType == UNIT_FACTORY) { // factory
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
      unit->startProductSchedule();
    } else if (unit->unitType == UNIT_FARM) { // farm
      if (unit->alliSide == WHICH_SIDE_HERO) {
        addFoodMax(getFoodGive(UNIT_FARM));
      }
      setOccupy(unit->getPosition() - Vec2(100, -0) * imageScale, 3, 2, true,
                unit);
      unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
      unit->smokePoint = unit->getPosition() + Vec2(-18, 110) * imageScale;
    } else if (unit->unitType == UNIT_LUMBERMILL) { // lumber mill
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
    } else if (unit->unitType == UNIT_AIRPORT) { // airport
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
      unit->startProductSchedule();
      Sprite *spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
      GM->runAnimation(spt, "propeller", true);
      unit->addChild(spt);
      spt->setPosition(Vec2(134, 206) * imageScale);
    } else if (unit->unitType == UNIT_MINE) { // mine
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
      unit->setTag(0);
    } else if (unit->unitType == UNIT_BARRACKS) { // barracks
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
      unit->startProductSchedule();
    } else if (unit->unitType == UNIT_CASTLE ||
               unit->unitType == UNIT_ZOMBIE_CASTLE) { // castle
      setOccupy(unit->getPosition() - Vec2(150, -100) * imageScale, 4, 3, true,
                unit);
      if (unit->alliSide == WHICH_SIDE_HERO) {
        addFoodMax(getFoodGive(UNIT_CASTLE));
      }
      unit->startProductSchedule();
      Sprite *spt;
      if (unit->unitType == UNIT_CASTLE) {
        spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
        GM->runAnimation(spt, "blueFlag", true);
        unit->addChild(spt);
        spt->setPosition(Vec2(192, 255) * imageScale);

        if (stageIndex > 11) {
          spt->removeFromParent();
          spt = Sprite::createWithSpriteFrameName("redFlag0.png");
          GM->runAnimation(spt, "redFlag", true);
          unit->addChild(spt);
          spt->setPosition(Vec2(232, 335) * imageScale);
        }
      }

    } else if (unit->unitType == UNIT_WATCHERTOWER) { // watcher tower
      setOccupy(unit->getPosition() - Vec2(50, 0), 2, 2, true, unit);
      unit->canFindTarget = true;
    } else if (unit->unitType == UNIT_ZOMBIE_SWORDSMAN ||
               unit->unitType == UNIT_ZOMBIE_ORC_AXE) { // zombie swordsman
      unit->oneSec = 1 + (rand() % 10) * 0.1f;
      unit->extraSpeed = unit->extraSpeed - rand() % 60;
    } else if (unit->unitType == UNIT_ORC_BARRACKS) {
      unit->startProductSchedule();
      setOccupy(unit->getPosition() - Vec2(50, -50) * imageScale, 2, 2, true,
                unit);
    } else if (unit->unitType == UNIT_TEMPLE) {
      unit->startProductSchedule();
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
    } else if (unit->unitType == UNIT_ORC_TROLL_HOUSE) {
      unit->startProductSchedule();
      setOccupy(unit->getPosition() - Vec2(100, -100) * imageScale, 3, 3, true,
                unit);
    } else if (unit->unitType == UNIT_BARBECUE) {
      setOccupy(unit->getPosition() - Vec2(100, -50) * imageScale, 3, 2, true,
                unit);
    }

    if (unit->unitType == UNIT_TREE) { // it is loading the saved game
      unit->canMove = false;
      decoLayer->setTileGID(49, getCoordinateFromPosition(unit->getPosition()));
      //            Sprite* spt =
      //            Sprite::createWithSpriteFrameName(strmake("tree2_%d.png",
      //            rand()%5)); batch->addChild(spt);
      //            spt->setPosition(unit->getPosition() + Vec2(-20 + rand()%40,
      //            -20 + rand()%40 + 20)*imageScale);
      //            spt->setLocalZOrder(-spt->getBoundingBox().getMinY());//.origin.y);
      //            unit->childrenSprite.pushBack(spt);
    }
  }

  strData = UDGetStr(strmake("savedDataExtra%d", slot).c_str());
  datas = GameManager::getInstance()->split(strData.c_str(), ",");
  if (datas.size() > 0) {
    difficultyMode = Value(datas.at(0)).asInt();
    gameMode = difficultyMode;
  }

  if (datas.size() > 1) {
    std::string strFog = Value(datas.at(1)).asString();
    if (strFog.length() >= fogArray.size()) {
      int counter = 0;
      for (auto fog : fogArray) {
        fog->appliedState = Value(strFog.substr(counter, 1)).asInt();
        if (fog->appliedState == FOG_SEEN_NOT_NOW) {
          fog->newState = FOG_SEEN_NOW;
        }
        counter++;
      }
      processNewFogState();
    }
  }
}
UnitInfo *GameScene::getHeroAt(int index) { return nullptr; }
void GameScene::checkBirdFly() {
  if ((GM->nextScene == STAGE_FIELD && !GM->isLoadingGame && HUD &&
       !HUD->isRaid && stageIndex > 0) ||
      GM->isColosseum) {
    log("stageIndex: %d", stageIndex);
    if (stageIndex == 24 || stageIndex == 25) {
      return;
    }
    if (!isMultiplay) {
      if (gameMode == GAME_MODE_HARD || gameMode == GAME_MODE_EASY ||
          gameMode == GAME_MODE_NORMAL) {
        HUD->showSupportOffer();
      }
    }
  }

  GM->isLoadingGame = false;
}
void GameScene::makeZombiesAttack() {
  //    EnemyBase* castle = nullptr;
  //    for(auto unit:heroArray){
  //        if(unit->unitType == UNIT_CASTLE){
  //            castle = unit;
  //            break;
  //        }
  //    }
  //    if (castle != nullptr) {
  for (auto unit : enemyArray) {
    if (unit->isZombie) {
      attackNearHero(unit);
    }
  }
  //    }
}
void GameScene::attackNearHero(EnemyBase *enemy) {
  int nearestDistance = 50000;
  EnemyBase *nearest = nullptr;
  for (auto hero : heroArray) {
    int distance = abs(enemy->getPositionX() - hero->getPositionX()) +
                   abs(enemy->getPositionY() - hero->getPositionY());
    if (nearestDistance > distance && canAttack(enemy, hero)) {
      nearest = hero;
      nearestDistance = distance;
    }
  }
  if (nearest != nullptr) {
    if (nearest->isBuilding) {
      moveAndAttackTo(
          enemy,
          nearest->approachingPoints[rand() % nearest->approachPointCount]);
    } else {
      moveAndAttackTo(enemy, nearest->getPosition());
    }
  }
}
void GameScene::setOccupy(cocos2d::Vec2 pos, int width, int height,
                           bool occupy, EnemyBase *building) {
  setOccupy(pos, width, height, occupy);
  building->buildingStartCoordinate = getCoordinateFromPosition(pos, theMap);
  building->buildingOccupySize = cocos2d::Size(width, height);
  building->refreshApproachPoints();
}
void GameScene::setOccupy(cocos2d::Vec2 pos, int width, int height,
                           bool occupy) {
  Vec2 point = getCoordinateFromPosition(pos, theMap);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      setTileGID(decoLayer, occupy ? 48 : 0, Vec2(point.x + i, point.y + j));

      //            Sprite* spt = Sprite::create("btnStop.png");
      //            this->addChild(spt, 1111);
      //            spt->setScale(0.1f);
      //            spt->setPosition(getPositionFromTileCoordinate(point.x + i,
      //            point.y + j)); // test
    }
  }
}
void GameScene::setTileGID(TMXLayer *layer, int gid, Vec2 pos) {
  if (pos.x >= 0 && pos.y >= 0 && pos.x < layer->getLayerSize().width &&
      pos.y < layer->getLayerSize().height) {
    layer->setTileGID(gid, pos);
  } else {
    log("invalied position");
  }
}
bool GameScene::isOccupied(cocos2d::Vec2 coordinate) {
  return GM->tileState[(int)coordinate.x][(int)coordinate.y] > 0;
  //    bool valid = isCoordinateValid(coordinate);
  //    if(!valid) return true;
  //    return isDecoBlock(decoLayer->getTileGIDAt(coordinate)) ||
  //    isSoilBlock(soilLayer->getTileGIDAt(coordinate)); int dgid =
  //    decoLayer->getTileGIDAt(coordinate); int sgid =
  //    soilLayer->getTileGIDAt(coordinate); return dgid == 48 || dgid == 49;
}
void GameScene::addSprite(std::string sptName, cocos2d::Vec2 pos) {
  Sprite *spt = Sprite::createWithSpriteFrameName(sptName);
  spt->setPosition(pos);
  batch->addChild(spt);
  spt->setLocalZOrder(-pos.y);
}
void GameScene::startCameraFollowNPCForEvent(Ref *ref) {
  cameraTargetForEvent = (Node *)ref;
  this->schedule(schedule_selector(GameScene::cameraUpdateForEvent));
}
void GameScene::endCameraFollowNPCForEvent() {
  this->unschedule(schedule_selector(GameScene::cameraUpdateForEvent));
}
void GameScene::cameraUpdateForEvent(float dt) {}
void GameScene::eventUpdate(float dt) {
  if (talkingNPC != nullptr && talkingNPC->getName().compare("undead") == 0) {
    if (imgTalkBox != nullptr && imgTalkBox->getScale() == 1) {
      imgTalkBox->setPosition(talkingNPC->getPosition() + Vec2(0, 10));
    } else {
      sptPointer->setScale(imgTalkBox->getScale());
    }
    sptPointer->setPosition(imgTalkBox->getPosition() + Vec2(0, 2));
  }
}
void GameScene::eventDone() {
  this->unschedule(schedule_selector(GameScene::eventUpdate));
}

void GameScene::turnOnEnemyDamaging(Ref *obj) {
  EnemyBase *enemy = (EnemyBase *)obj;
  enemy->isDamaging = true;
  runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
}
void GameScene::addDecoBlock(Vec2 coordinate, Vec2 position,
                              std::string spriteName) {
  Sprite *spt = Sprite::createWithSpriteFrameName(spriteName);
  batch->addChild(spt);
  spt->setPosition(position);
  spt->setLocalZOrder(-spt->getBoundingBox().origin.y + 2);
  spt->setName(strmake("deco_%d_%d", (int)coordinate.x, (int)coordinate.y));
  decoLayer->setTileGID(138, coordinate);
}
Vec2 GameScene::getPositionFromTileCoordinate(int x, int y) {
  return Vec2(x * TILE_SIZE + TILE_SIZE / 2,
              (theMap->getMapSize().height - y - 1) * TILE_SIZE) +
         Vec2(0, TILE_SIZE / 2);
}
void GameScene::resetPathState() {
  int gid;
  TMXLayer *stageLayer = theMap->getLayer("deco");
  cocos2d::Size mapSize = theMap->getMapSize();
  bool isBlocked;
  for (int j = 0; j < mapSize.height; j++) {
    for (int i = 0; i < mapSize.width; i++) {

      //            gid = stageLayer->getTileGIDAt(Vec2(i, j));
      //            if (gid) {
      if (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, Vec2(i, j)))) {
        isBlocked = true;
      } else if (isSoilBlock(getTileGIDAt(soilLayer, Vec2(i, j)))) {
        isBlocked = true;
      } else {
        isBlocked = false;
      }
      GM->setPathState(i, j, isBlocked);
    }
  }
  resetWaterPathState();
}

void GameScene::resetWaterPathState() {
  if (theMap == nullptr) return;
  cocos2d::Size mapSize = theMap->getMapSize();

  // Water tiles passable for ships; everything else blocked.
  for (int j = 0; j < (int)mapSize.height; j++) {
    for (int i = 0; i < (int)mapSize.width; i++) {
      GM->setWaterPathState(i, j, isWaterTileAt(i, j) ? 0 : 1);
    }
  }

  // Completed water buildings block ship A* paths.
  auto isWaterBuildingType = [](int type) -> bool {
    return type == UNIT_HUMAN_SHIPYARD || type == UNIT_ORC_SHIPYARD ||
           type == UNIT_HUMAN_OIL_EXTRACTOR || type == UNIT_ORC_OIL_EXTRACTOR ||
           type == UNIT_HUMAN_OIL_REFINERY || type == UNIT_ORC_OIL_REFINERY;
  };
  auto markBuilding = [&](EnemyBase *u) {
    if (!u->isBuilding || !u->isBuildingComplete || !isWaterBuildingType(u->unitType))
      return;
    Vec2 st = u->buildingStartCoordinate;
    Size sz = u->buildingOccupySize;
    for (int j2 = (int)st.y; j2 < (int)(st.y + sz.height); j2++)
      for (int i2 = (int)st.x; i2 < (int)(st.x + sz.width); i2++)
        GM->setWaterPathState(i2, j2, 1);
  };
  for (auto u : heroArray)  markBuilding(u);
  for (auto u : enemyArray) markBuilding(u);
}

// Generic 4-connected flood fill: assigns an incrementing region ID to every
// tile where `passable(x,y)` holds. outIds is sized w x h, -1 = ungrouped.
// Returns the number of regions found.
template <typename Pred>
static int floodFillRegions(int w, int h, Pred passable,
                             std::vector<std::vector<int>> &outIds) {
  outIds.assign(w, std::vector<int>(h, -1));
  int nextId = 0;
  std::vector<std::pair<int, int>> stack;
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      if (outIds[i][j] != -1 || !passable(i, j)) continue;
      int id = nextId++;
      stack.clear();
      stack.push_back(std::make_pair(i, j));
      outIds[i][j] = id;
      while (!stack.empty()) {
        int cx = stack.back().first, cy = stack.back().second;
        stack.pop_back();
        for (int k = 0; k < 4; k++) {
          int nx = cx + dx[k], ny = cy + dy[k];
          if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
          if (outIds[nx][ny] != -1 || !passable(nx, ny)) continue;
          outIds[nx][ny] = id;
          stack.push_back(std::make_pair(nx, ny));
        }
      }
    }
  }
  return nextId;
}

// Labels every land tile with an island ID and every water tile with a water-
// region ID (both via floodFillRegions above), then records which water
// regions border which islands. Terrain-only (soilLayer/stageLayer) so the
// result is stable regardless of buildings/units placed later.
void GameScene::computeIslandRegions() {
  islandRegionsComputed = true;
  landIslandId.clear();
  waterRegionId.clear();
  islandBorderWaterRegions.clear();
  if (theMap == nullptr) return;

  int w = (int)mapSize.width, h = (int)mapSize.height;
  if (w <= 0 || h <= 0) return;

  floodFillRegions(w, h, [&](int x, int y) {
    return !isSoilBlock(getTileGIDAt(soilLayer, Vec2(x, y)));
  }, landIslandId);

  floodFillRegions(w, h, [&](int x, int y) {
    return isWaterTileAt(x, y);
  }, waterRegionId);

  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int island = landIslandId[i][j];
      if (island < 0) continue;
      for (int k = 0; k < 4; k++) {
        int nx = i + dx[k], ny = j + dy[k];
        if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
        int wr = waterRegionId[nx][ny];
        if (wr >= 0) islandBorderWaterRegions[island].insert(wr);
      }
    }
  }
}

int GameScene::getIslandIdAt(cocos2d::Vec2 worldPos) {
  if (!islandRegionsComputed) computeIslandRegions();
  if (landIslandId.empty()) return -1;
  Vec2 t = getCoordinateFromPosition(worldPos);
  int x = (int)t.x, y = (int)t.y;
  if (x < 0 || y < 0 || x >= (int)landIslandId.size() || y >= (int)landIslandId[0].size())
    return -1;
  return landIslandId[x][y];
}

bool GameScene::isSameIsland(cocos2d::Vec2 worldPosA, cocos2d::Vec2 worldPosB) {
  int a = getIslandIdAt(worldPosA);
  int b = getIslandIdAt(worldPosB);
  return a >= 0 && a == b;
}

bool GameScene::isSingleShuttleHopAway(cocos2d::Vec2 worldPosA, cocos2d::Vec2 worldPosB) {
  int a = getIslandIdAt(worldPosA);
  int b = getIslandIdAt(worldPosB);
  if (a < 0 || b < 0 || a == b) return false;
  auto itA = islandBorderWaterRegions.find(a);
  auto itB = islandBorderWaterRegions.find(b);
  if (itA == islandBorderWaterRegions.end() || itB == islandBorderWaterRegions.end())
    return false;
  for (int wr : itA->second)
    if (itB->second.count(wr)) return true;
  return false;
}

cocos2d::Vec2 GameScene::findCoastalDropPoint(cocos2d::Vec2 worldTarget) {
  if (!islandRegionsComputed) computeIslandRegions();
  int targetIsland = getIslandIdAt(worldTarget);
  if (targetIsland < 0 || landIslandId.empty()) return Vec2::ZERO;

  int w = (int)landIslandId.size(), h = (int)landIslandId[0].size();
  Vec2 targetTile = getCoordinateFromPosition(worldTarget);
  static const int dx[] = {1, -1, 0, 0};
  static const int dy[] = {0, 0, 1, -1};

  Vec2 best = Vec2::ZERO;
  float bestD = FLT_MAX;
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      if (landIslandId[i][j] != targetIsland) continue;
      for (int k = 0; k < 4; k++) {
        int nx = i + dx[k], ny = j + dy[k];
        if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
        if (waterRegionId[nx][ny] < 0) continue;
        float d = ((float)nx - targetTile.x) * ((float)nx - targetTile.x) +
                  ((float)ny - targetTile.y) * ((float)ny - targetTile.y);
        if (d < bestD) {
          bestD = d;
          best = getPositionFromTileCoordinate(nx, ny);
        }
      }
    }
  }
  return best;
}

// Finds a same-side Shuttle with cargo room and sends `unit` to board it
// (reusing the existing pendingShuttle "walk over, auto-board" pump). Marks
// the Shuttle as auto-ferrying toward finalWorldTarget's island so
// updateShuttleFerries() picks up sailing/unloading once boarding completes.
bool GameScene::beginShuttleFerry(Movable *unit, cocos2d::Vec2 finalWorldTarget) {
  if (unit == nullptr || unit->isInShuttle || unit->pendingShuttle != nullptr)
    return false;

  int cols, rows;
  getCargoSize(unit->unitType, cols, rows);

  auto &army = unit->isEnemy ? enemyArray : heroArray;
  EnemyBase *bestShuttle = nullptr;
  float bestD = FLT_MAX;
  for (auto *u : army) {
    if (!u || u->energy <= 0) continue;
    if (u->unitType != UNIT_HUMAN_SHUTTLE && u->unitType != UNIT_ORC_SHUTTLE) continue;
    // Don't hijack a Shuttle that's already ferrying passengers somewhere else.
    if (u->isAutoFerrying && u->autoFerryDropTarget != finalWorldTarget) continue;
    int outCol, outRow;
    if (!findCargoSlot(u, cols, rows, outCol, outRow)) continue;
    float d = unit->getPosition().distanceSquared(u->getPosition());
    if (d < bestD) { bestD = d; bestShuttle = u; }
  }
  if (!bestShuttle) return false;

  unit->pendingShuttle = bestShuttle;
  unit->needsFerryToTarget = true;
  unit->ferryFinalTarget = finalWorldTarget;
  unit->moveToPos = Vec2::ZERO;
  unit->moveFlagPos = bestShuttle->getPosition();
  unit->target = nullptr;
  unit->unitAct = UNIT_ACT_MOVE;
  unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;

  bestShuttle->isAutoFerrying = true;
  bestShuttle->autoFerryDropTarget = finalWorldTarget;
  return true;
}

void GameScene::updateShuttleFerries() {
  // Passengers whose ferry stalled (e.g. lost a boarding race to another
  // unit) still have needsFerryToTarget set but no Shuttle assigned - retry.
  auto retryPending = [&](Vector<EnemyBase *> &army) {
    for (auto *u : army) {
      if (!u || u->energy <= 0) continue;
      if (!u->needsFerryToTarget || u->isInShuttle || u->pendingShuttle) continue;
      beginShuttleFerry(u, u->ferryFinalTarget);
    }
  };
  retryPending(heroArray);
  retryPending(enemyArray);

  auto driveShuttles = [&](Vector<EnemyBase *> &army) {
    // Snapshot: a completed ferry calls unloadShuttle(), which can mutate
    // shuttleCargo/positions of other units the outer loop hasn't reached yet.
    std::vector<EnemyBase *> snap(army.begin(), army.end());
    for (auto *shuttle : snap) {
      if (!shuttle || shuttle->energy <= 0) continue;
      if (shuttle->unitType != UNIT_HUMAN_SHUTTLE && shuttle->unitType != UNIT_ORC_SHUTTLE) continue;
      if (!shuttle->isAutoFerrying) continue;

      bool hasFerryPassenger = false;
      for (auto &slot : shuttle->shuttleCargo) {
        EnemyBase *pax = (EnemyBase *)slot.unit;
        if (pax && pax->needsFerryToTarget) { hasFerryPassenger = true; break; }
      }
      if (!hasFerryPassenger) continue; // still waiting for boarding

      if (shuttle->unitAct != UNIT_ACT_MOVE) {
        // Not yet sailing for this ferry - issue the order.
        Vec2 drop = findCoastalDropPoint(shuttle->autoFerryDropTarget);
        if (drop == Vec2::ZERO) {
          // No reachable coast found (shouldn't happen once
          // isSingleShuttleHopAway already passed) - abandon rather than
          // strand the passengers forever.
          for (auto &slot : shuttle->shuttleCargo) {
            EnemyBase *pax = (EnemyBase *)slot.unit;
            if (pax) pax->needsFerryToTarget = false;
          }
          shuttle->isAutoFerrying = false;
          continue;
        }
        shuttle->moveToPos = Vec2::ZERO;
        shuttle->moveFlagPos = drop;
        shuttle->target = nullptr;
        shuttle->unitAct = UNIT_ACT_MOVE;
        shuttle->unitActDetail = UNIT_ACT_DETAIL_IDLE;
        continue;
      }

      // Already sailing - check arrival.
      float dist = shuttle->getPosition().distance(shuttle->moveFlagPos);
      if (dist <= TILE_SIZE * 1.5f && isShuttleAdjacentToLand(shuttle)) {
        std::vector<std::pair<EnemyBase *, Vec2>> toResume;
        for (auto &slot : shuttle->shuttleCargo) {
          EnemyBase *pax = (EnemyBase *)slot.unit;
          if (pax && pax->needsFerryToTarget)
            toResume.push_back(std::make_pair(pax, pax->ferryFinalTarget));
        }
        unloadShuttle(shuttle);
        shuttle->isAutoFerrying = false;
        shuttle->autoFerryDropTarget = Vec2::ZERO;
        shuttle->unitAct = UNIT_ACT_NONE;
        shuttle->unitActDetail = UNIT_ACT_DETAIL_IDLE;
        shuttle->moveToPos = Vec2::ZERO;
        shuttle->moveFlagPos = Vec2::ZERO;
        for (auto &pr : toResume) {
          EnemyBase *pax = pr.first;
          if (!pax || pax->energy <= 0) continue;
          pax->needsFerryToTarget = false;
          moveAndAttackTo(pax, pr.second);
        }
      }
    }
  };
  driveShuttles(heroArray);
  driveShuttles(enemyArray);
}

void GameScene::createTrap(EnemyBase *enemy, cocos2d::Vec2 pos) {
  enemy->setPosition(pos);
  enemy->ap = getHeroMaxHP(0) / 4;
  batch->addChild(enemy);
  enemy->setLocalZOrder(-1000);
  enemyArray.pushBack(enemy);
}
void GameScene::addTalkBalloon(EnemyBase *npc, std::string imgName) {
  Sprite *spt = Sprite::createWithSpriteFrameName(imgName);
  npc->addChild(spt, 10);
  spt->setPosition(
      Vec2(npc->getBoundingBox().size.width / 2,
           npc->getContentSize().height + spt->getContentSize().height / 2));
  spt->runAction(RepeatForever::create(
      Sequence::create(MoveBy::create(0.2f, Vec2(0, 2)),
                       MoveBy::create(0.2f, Vec2(0, -2)), NULL)));
  spt->setName("questState");
}
void GameScene::addTalkBalloon(TMXTiledMap *map, std::string imgName,
                                cocos2d::Vec2 pos) {
  Sprite *spt = Sprite::createWithSpriteFrameName(imgName);
  map->addChild(spt, 10);
  //    spt->setPosition(Vec2(npc->getContentSize().width/2,
  //    npc->getContentSize().height + spt->getContentSize().height/2));
  spt->setPosition(pos);
  spt->runAction(RepeatForever::create(
      Sequence::create(MoveBy::create(0.2f, Vec2(0, 2)),
                       MoveBy::create(0.2f, Vec2(0, -2)), NULL)));
  spt->setName("questState");
}
EnemyBase *GameScene::createUnit(int index, int whichSide, bool isBuilding,
                                  Vec2 pos, std::string name, int scaleX,
                                  std::string charName) {
  //    log("create unit spt: %s", charName.c_str());
  int id = 0;
  if (whichSide == WHICH_SIDE_HERO) {
    nextHeroID++;
    id = nextHeroID;
  } else if (whichSide == WHICH_SIDE_ENEMY) {
    nextEnemyID++;
    id = nextEnemyID;
  }
  int eng = getUnitMaxHP(index);
  int spd = getUnitSP(index);
  EnemyBase *unit = EnemyBase::createEnemy(index, eng, spd, charName.c_str());
  unit->unitID = id;
  unit->maxEnergy = eng;
  unit->energy = eng;
  unit->setName(name);
  unit->unitName = name;
  unit->ap = getUnitAP(index);
  unit->dp = getUnitDP(index);
  unit->attackRange = getUnitAttackRange(index);
  unit->attackType = getUnitAttackType(index);
  unit->attackTargetType = getUnitAttackTargetType(index);
  unit->monitoringDistance = getUnitMonitoringDistance(index);
  unit->attackCoolTimeMax = getUnitAttackCoolTime(index);
  unit->attackHappenTime = getUnitAttackHappenTime(index);

  unit->setScaleX(scaleX);
  unit->rewardExp = 10;
  unit->rewardCoin = 10;
  unit->runAnimation(ANIMATION_TYPE_IDLE);
  unit->alliSide = whichSide;
  unit->desiredPosition = pos;
  unit->setLocalZOrder(-unit->getBoundingBox().getMinY()); //-pos.y);

  unit->isEnemy = whichSide == WHICH_SIDE_ENEMY;

  if (whichSide == WHICH_SIDE_HERO) {
    unit->isDetected = true;
  } else if (whichSide == WHICH_SIDE_ENEMY) {
    if (index >= 0 && index <= 12) {
      unit->setColor(Color3B(255, 200, 200));
    } else if (stageIndex >= 12 && index >= 23 && index <= 27) {
      unit->setColor(Color3B(255, 200, 200));
    }
  }
  unitsToCreateArray.pushBack(unit);
  unit->isBuilding = isBuilding;
  //    int count = unit->getChildren().size();
  if (isBuilding) {
    unit->setAnchorPoint(Vec2(0.5, 0.5));
    unit->canMove = false;
    unit->isBuildingComplete = true;
    spriteBatchBuilding->addChild(unit);
    unit->refreshApproachPoints();
  } else {
    if (unit->unitType == UNIT_TREE || unit->unitType == UNIT_TREE_FOR_BATTLE) {
      spriteBatchBuilding->addChild(unit);
      unit->buildingOccupySize = Size(1, 1);
      unit->buildingStartCoordinate = getCoordinateFromPosition(pos);
      //            pos.y -= 25;
      unit->refreshApproachPoints();
    } else {
      batch->addChild(unit);
    }
  }
  unit->setPosition(pos);
  unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
  if (unit->unitType == UNIT_OIL_SPOT) {
    unit->setLocalZOrder(-100000);
  }
  unit->stop();

  //    if (isMultiplay && whichSide == WHICH_SIDE_HERO && !blockSending) {
  //        log("** send create unit");

  //    }

  //    MovableArray.pushBack(unit); // test

  if (unit->unitType == UNIT_HELICOPTER ||
      unit->unitType == UNIT_HERO_BATMONSTER) {
    unit->isFlying = true;
    unit->setLocalZOrder(10);
  }
  return unit;
}
void GameScene::addUnit(Movable *unit, bool addToWorld) {
  if (addToWorld) {
    unitsToCreateArray.pushBack((EnemyBase *)unit);
    batch->addChild(unit);
    MovableArray.pushBack(unit);
  }
  unit->isBuilding = false;
  unit->stop();

  if (unit->unitType == UNIT_HELICOPTER ||
      unit->unitType == UNIT_HERO_BATMONSTER) {
    unit->isFlying = true;
    unit->setLocalZOrder(10);
  }
}
int GameScene::getUnitAP(int unit) {
  //    return 0; // test
  return GM->getUnitAP(unit);
}
int GameScene::getUnitDP(int unit) {
  if (unit == UNIT_WORKER) {
    return 0;
  } else if (unit == UNIT_CATAPULT) {
    return 0;
  } else if (unit == UNIT_TROLL) {
    return 3;
  } else if (unit == UNIT_ORC_AXE) {
    return 2;
  } else if (unit == UNIT_ORC_SPEAR) {
    return 1;
  } else if (unit == UNIT_HERO_ORC) {
    return 1;
  }
  return 0;
}
int GameScene::getUnitSP(int unit) {
  if (unit == UNIT_WORKER) {
    return 600;
  } else if (unit == UNIT_GOBLIN_WORKER || unit == UNIT_GOBLIN_BOMB) {
    return 600;
  } else if (unit == UNIT_SWORDMAN) {
    return 650;
  } else if (unit == UNIT_ARCHER) {
    return 600;
  } else if (unit == UNIT_CATAPULT) {
    return 300;
  } else if (unit == UNIT_HELICOPTER) {
    return 650;
  } else if (unit == UNIT_WATCHERTOWER || unit == UNIT_ORC_BUNKER ||
             unit == UNIT_ORC_HQ) {
    return 0;
  } else if (unit == UNIT_TROLL) {
    return 330;
  } else if (unit == UNIT_ORC_AXE) {
    return 550;
  } else if (unit == UNIT_GOBLIN) {
    return 900;
  } else if (unit == UNIT_ORC_SPEAR) {
    return 550;
  } else if (unit == UNIT_ZOMBIE_ORC_AXE || unit == UNIT_ZOMBIE_SWORDSMAN) {
    return 250;
  } else if (unit == UNIT_HERO_ORC || unit == UNIT_HERO_KNIGHT ||
             unit == UNIT_HERO_LION || unit == UNIT_HERO_SKELETON ||
             unit == UNIT_HERO_REAPER || unit == UNIT_HERO_MOLE ||
             unit == UNIT_HERO_TOYMOUSE || unit == UNIT_HERO_CATINBOOTS ||
             unit == UNIT_HERO_MINO || unit == UNIT_HERO_CHUNJA) {
    return 650;
  } else if (unit == UNIT_HERO_ARCHER || unit == UNIT_HERO_HEALER ||
             unit == UNIT_HERO_WIZARD || unit == UNIT_HERO_ENT ||
             unit == UNIT_HERO_SANTA || unit == UNIT_HERO_SALAMANDER ||
             unit == UNIT_HERO_UNDINE || unit == UNIT_HERO_TANKER ||
             unit == UNIT_HERO_MEMEAT || unit == UNIT_HERO_PENGUIN ||
             unit == UNIT_HERO_PARASITE) {
    return 550;
  } else if (unit == UNIT_HERO_WEREWOLF || unit == UNIT_HERO_GOBLIN ||
             unit == UNIT_HERO_CRAZY_LION || unit == UNIT_HERO_CRAZY_BEAR ||
             unit == UNIT_HERO_CRAZY_WEREWOLF || unit == UNIT_HERO_RUDOLPH ||
             unit == UNIT_HERO_LADY_WEREWOLF ||
             unit == UNIT_HERO_SAVAGEARCHER || unit == UNIT_HERO_BATMONSTER) {
    return 800;
  } else if (unit == UNIT_HERO_MONK || unit == UNIT_HERO_SANTADOG ||
             unit == UNIT_HERO_FIGHTER || unit == UNIT_HERO_BEAR ||
             unit == UNIT_HERO_ELF_SWORDMAN || unit == UNIT_HERO_ASSASSIN ||
             unit == UNIT_HERO_LADY_LION || unit == UNIT_HERO_LAMIA ||
             unit == UNIT_HERO_LADY_BEAR || unit == UNIT_HERO_WATERMELON ||
             unit == UNIT_HERO_BABYMINO || unit == UNIT_HERO_KERBEROS) {
    return 700;
  } else if (unit == UNIT_HERO_GOLEM) {
    return 430;
  } else if (unit == UNIT_HUMAN_SHIP || unit == UNIT_ORC_SHIP) {
    return 350;
  } else if (unit == UNIT_HUMAN_BATTLE_SHIP) {
    return 245;
  }
  return 450;
}
float GameScene::getUnitCreateTime(int index) {
  //    return 3; // test
  if (index == UNIT_CASTLE) {
    return 40;
  } else if (index == UNIT_FARM) {
    return 10;
  } else if (index == UNIT_BARRACKS) {
    return 20;
  } else if (index == UNIT_LUMBERMILL) {
    return 15;
  } else if (index == UNIT_WATCHERTOWER) {
    return 12;
  } else if (index == UNIT_AIRPORT) {
    return 25;
  } else if (index == UNIT_FACTORY) {
    return 30;
  } else if (index == UNIT_WORKER) {
    return 8;
  } else if (index == UNIT_GOBLIN_WORKER) {
    return 8;
  } else if (index == UNIT_SWORDMAN) {
    return 10;
  } else if (index == UNIT_ARCHER) {
    return 15;
  } else if (index == UNIT_HELICOPTER) {
    return 15;
  } else if (index == UNIT_CATAPULT) {
    return 30;
  } else if (index == UNIT_GOBLIN_BOMB) {
    return 20;
  } else if (index == UNIT_GOBLIN) {
    return 8;
  } else if (index == UNIT_TROLL) {
    return 40;
  } else if (index == UNIT_ORC_AXE) {
    return 15;
  } else if (index == UNIT_ORC_SPEAR) {
    return 20;
  } else if (index == UNIT_WIZARD) {
    return 35;
  } else if (index == UNIT_ORC_HQ) {
    return 40;
  } else if (index == UNIT_BARBECUE) {
    return 10;
  } else if (index == UNIT_ORC_BARRACKS) {
    return 20;
  } else if (index == UNIT_ORC_BUNKER) {
    return 12;
  } else if (index == UNIT_TEMPLE) {
    return 35;
  } else if (index == UNIT_ORC_TROLL_HOUSE) {
    return 30;
  } else if (index == UNIT_HUMAN_SHIPYARD) {
    return 35;
  } else if (index == UNIT_HUMAN_SHUTTLE) {
    return 15;
  } else if (index == UNIT_HUMAN_SHIP) {
    return 25;
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return 40;
  } else if (index == UNIT_ORC_SHIPYARD) {
    return 35;
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    return 40;
  } else if (index == UNIT_HUMAN_OIL_SHIP) {
    return 20;
  } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
    return 30;
  }
  return 10;
}
float GameScene::getUnitMonitoringDistance(int index) {
  if (index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP || index == UNIT_HUMAN_BATTLE_SHIP) {
    return 200000;
  } else if (index == UNIT_ARCHER) {
    return 150000;
  } else if (index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER ||
             index == UNIT_ORC_HQ) {
    return 87500;
  } else if (index == UNIT_HELICOPTER) {
    return 300000;
  } else if (index == UNIT_CATAPULT) {
    return 250000;
  } else if (index == UNIT_SWORDMAN || index == UNIT_WORKER ||
             index == UNIT_GOBLIN_WORKER || index == UNIT_ORC_AXE ||
             index == UNIT_ORC_SPEAR || index == UNIT_GOBLIN ||
             index == UNIT_TROLL || index == UNIT_GOBLIN_BOMB) {
    return 75000;
  }
  return 150000;
}

// if(index == UNIT_ARCHER){
//     return 600000;
// }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index ==
// UNIT_ORC_HQ){
//     return 350000;
// }else if(index == UNIT_HELICOPTER){
//     return 1000000;
// }else if(index == UNIT_CATAPULT){
//     return 1200000;
// }else if(index == UNIT_SWORDMAN || index == UNIT_WORKER|| index ==
// UNIT_GOBLIN_WORKER || index == UNIT_ORC_AXE || index == UNIT_ORC_SPEAR ||
// index == UNIT_GOBLIN || index == UNIT_TROLL|| index == UNIT_GOBLIN_BOMB){
//     return 300000;
// }
// return 600000;

float GameScene::getUnitAttackRange(int index) {
  if (index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP || index == UNIT_HUMAN_BATTLE_SHIP) {
    return 85000;
  } else if (index == UNIT_ARCHER) {
    return 50176;
  } else if (index == UNIT_HELICOPTER) {
    return 24964;
  } else if (index == UNIT_ORC_SPEAR) {
    return 54990;
  } else if (index == UNIT_CATAPULT) {
    return 124609;
  } else if (index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER ||
             index == UNIT_ORC_HQ) {
    return 99868;
  } else if (index == UNIT_WIZARD) {
    return 37442;
  }
  return 50176;
}
int GameScene::getUnitAttackType(int index) {
  if (index == UNIT_ARCHER || index == UNIT_ORC_SPEAR ||
      index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER ||
      index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ ||
      index == UNIT_HELICOPTER || index == UNIT_WIZARD ||
      index == UNIT_HERO_ARCHER || index == UNIT_HERO_WIZARD ||
      index == UNIT_HERO_UNDINE || index == UNIT_HERO_HEALER ||
      index == UNIT_HERO_REAPER || index == UNIT_HERO_SANTA ||
      index == UNIT_HERO_LADY_WEREWOLF || index == UNIT_HERO_LADY_BEAR ||
      index == UNIT_HERO_PARASITE || index == UNIT_HERO_BABYMINO ||
      index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP ||
      index == UNIT_HUMAN_BATTLE_SHIP) {
    return ATTACK_TYPE_RANGE;
  } else if (index == UNIT_FARM || index == UNIT_BARRACKS ||
             index == UNIT_LUMBERMILL || index == UNIT_FACTORY ||
             index == UNIT_AIRPORT || index == UNIT_CASTLE ||
             index == UNIT_ORC_HQ || index == UNIT_BARBECUE ||
             index == UNIT_ORC_BUNKER || index == UNIT_ORC_BARRACKS ||
             index == UNIT_ORC_TROLL_HOUSE || index == UNIT_TEMPLE ||
             index == UNIT_TREE || index == UNIT_TREE_FOR_BATTLE ||
             index == UNIT_MINE || index == UNIT_HUMAN_SHUTTLE ||
             index == UNIT_ORC_SHUTTLE) {
    return ATTACK_TYPE_NONE;
  } else if (index == UNIT_HERO_RUDOLPH || index == UNIT_HERO_SANTADOG ||
             index == UNIT_HERO_SALAMANDER || index == UNIT_HERO_PENGUIN ||
             index == UNIT_HERO_CATINBOOTS || index == UNIT_HERO_MOLE ||
             index == UNIT_HERO_TOYMOUSE || index == UNIT_HERO_LADY_LION ||
             index == UNIT_HERO_SAVAGEARCHER || index == UNIT_HERO_BATMONSTER ||
             index == UNIT_HERO_MEMEAT || index == UNIT_HERO_WATERMELON ||
             index == UNIT_HERO_MINO || index == UNIT_HERO_KERBEROS ||
             index == UNIT_HERO_LAMIA || index == UNIT_HERO_CHUNJA ||
             index == UNIT_HERO_GOLEM) {
    return ATTACK_TYPE_NEAR;
  }
  //    else if (index == UNIT_SWORDMAN ||index == UNIT_WORKER || index ==
  //    UNIT_GOBLIN_WORKER || index == UNIT_GOBLIN_BOMB || index == UNIT_GOBLIN
  //    || index == UNIT_ORC_AXE || index == UNIT_TROLL || index ==
  //    UNIT_ZOMBIE_ORC_AXE ||
  //              index == UNIT_ZOMBIE_SWORDSMAN ||
  //              index == UNIT_HERO_ORC || index == UNIT_HERO_GOBLIN || index
  //              == UNIT_HERO_LIZARDMAN || index == UNIT_HERO_SPEARMAN || index
  //              == UNIT_HERO_WEREWOLF) {
  //        return ATTACK_TYPE_NEAR;
  //    }

  //    return ATTACK_TYPE_NONE;

  return ATTACK_TYPE_NEAR;
}
int GameScene::getUnitAttackTargetType(int index) {
  if (index == UNIT_CATAPULT || index == UNIT_WIZARD ||
      index == UNIT_HERO_UNDINE || index == UNIT_HERO_WIZARD ||
      index == UNIT_HERO_SANTA || index == UNIT_HUMAN_BATTLE_SHIP) {
    return ATTACK_TARGET_TYPE_SPLASH;
  }
  return ATTACK_TARGET_TYPE_SINGLE;
}
float GameScene::getUnitAttackCoolTime(int index) {
  if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return 6.0f;
  } else if (index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP) {
    return 3.0f;
  } else if (index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER ||
      index == UNIT_HERO_PENGUIN) {
    return 1.0f;
  } else if (index == UNIT_ARCHER) {
    return 1.5f;
  } else if (index == UNIT_HELICOPTER) {
    return 1.1f;
  } else if (index == UNIT_SWORDMAN) {
    return 1.2f;
  } else if (index == UNIT_CATAPULT) {
    return 4.0f;
  } else if (index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER ||
             index == UNIT_ORC_HQ) {
    return 1.1f;
  } else if (index == UNIT_ORC_SPEAR) {
    return 1.5f;
  } else if (index == UNIT_ORC_AXE) {
    return 1.2f;
  } else if (index == UNIT_TROLL) {
    return 1.0f;
  } else if (index == UNIT_GOBLIN) {
    return 0.7f;
  } else if (index == UNIT_GOBLIN_BOMB) {
    return 0.1f;
  } else if (index == UNIT_WIZARD) {
    return 2.0f;
  } else if (index == UNIT_HERO_ORC) {
    return 2.0f;
  } else if (index == UNIT_HERO_SPEARMAN) {
    return 1.56f;
  } else if (index == UNIT_HERO_WEREWOLF) {
    return 2;
  } else if (index == UNIT_HERO_GOBLIN) {
    return 1.6f;
  } else if (index == UNIT_HERO_MONK) {
    return 1.1f;
  } else if (index == UNIT_HERO_FIGHTER) {
    return 35.0f / 30;
  } else if (index == UNIT_HERO_ELF_SWORDMAN) {
    return 50.0f / 30;
  } else if (index == UNIT_HERO_ASSASSIN) {
    return 50.0f / 30;
  } else if (index == UNIT_HERO_LION || index == UNIT_HERO_LADY_BEAR) {
    return 60.0f / 30;
  } else if (index == UNIT_HERO_WIZARD || index == UNIT_HERO_LADY_WEREWOLF ||
             index == UNIT_HERO_BABYMINO) {
    return 45.0f / 30;
  } else if (index == UNIT_HERO_TANKER) {
    return 60.0f / 30;
  } else if (index == UNIT_HERO_SKELETON) {
    return 70.0f / 30;
  } else if (index == UNIT_HERO_REAPER) {
    return 60.0f / 30;
  } else if (index == UNIT_HERO_ENT || index == UNIT_HERO_UNDINE ||
             index == UNIT_HERO_LADY_LION || index == UNIT_HERO_BATMONSTER ||
             index == UNIT_HERO_MEMEAT || index == UNIT_HERO_PARASITE ||
             index == UNIT_HERO_CHUNJA) {
    return 60.0f / 30;
  } else if (index == UNIT_HERO_SANTA || index == UNIT_HERO_RUDOLPH ||
             index == UNIT_HERO_SANTADOG || index == UNIT_HERO_SALAMANDER ||
             index == UNIT_HERO_CATINBOOTS || index == UNIT_HERO_MOLE ||
             index == UNIT_HERO_SAVAGEARCHER || index == UNIT_HERO_WATERMELON ||
             index == UNIT_HERO_MINO || index == UNIT_HERO_KERBEROS) {
    return 50.0f / 30;
  } else if (index == UNIT_HERO_TOYMOUSE) {
    return 75.0f / 30;
  } else if (index == UNIT_HERO_LAMIA) {
    return 55.0f / 30;
  } else if (index == UNIT_HERO_GOLEM) {
    return 65.0f / 30;
  }

  return 2.0f;
}
float GameScene::getUnitAttackHappenTime(int index) {
  if (index == UNIT_HUMAN_SHIP || index == UNIT_ORC_SHIP || index == UNIT_HUMAN_BATTLE_SHIP) {
    return 0.5f;
  } else if (index == UNIT_ARCHER) {
    return 0.3f;
  } else if (index == UNIT_SWORDMAN || index == UNIT_HELICOPTER ||
             index == UNIT_ORC_SPEAR || index == UNIT_ORC_AXE ||
             index == UNIT_GOBLIN || index == UNIT_TROLL) {
    return 0.3f;
  } else if (index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER ||
             index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ) {
    return 0.0f;
  } else if (index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER) {
    return 0.5f;
  } else if (index == UNIT_WIZARD) {
    return 0.4f;
  } else if (index == UNIT_HERO_ORC) {
    return 1.3f;
  } else if (index == UNIT_HERO_ARCHER) {
    return 1.2f;
  } else if (index == UNIT_HERO_GOBLIN) {
    return 1.13f;
  } else if (index == UNIT_HERO_LIZARDMAN) {
    return 1.33f;
  } else if (index == UNIT_HERO_SPEARMAN) {
    return 0.73f;
  } else if (index == UNIT_HERO_WEREWOLF) {
    return 0.76f;
  } else if (index == UNIT_HERO_MONK) {
    return 0.9f;
  } else if (index == UNIT_HERO_FIGHTER) {
    return 19.0f / 30;
  } else if (index == UNIT_HERO_BEAR) {
    return 19.0f / 30;
  } else if (index == UNIT_HERO_HEALER) {
    return 32.0f / 30;
  } else if (index == UNIT_HERO_KNIGHT) {
    return 20.0f / 30;
  } else if (index == UNIT_HERO_ELF_SWORDMAN) {
    return 20.0f / 30;
  } else if (index == UNIT_HERO_ASSASSIN) {
    return 29.0f / 30;
  } else if (index == UNIT_HERO_LION || index == UNIT_HERO_PENGUIN) {
    return 20.0f / 30;
  } else if (index == UNIT_HERO_WIZARD) {
    return 13.0f / 30;
  } else if (index == UNIT_HERO_TANKER || index == UNIT_HERO_MEMEAT) {
    return 40.0f / 30;
  } else if (index == UNIT_HERO_SKELETON) {
    return 22.0f / 30;
  } else if (index == UNIT_HERO_REAPER) {
    return 37.0f / 30;
  } else if (index == UNIT_HERO_ENT) {
    return 18.0f / 30;
  } else if (index == UNIT_HERO_SANTA) {
    return 38.0f / 30;
  } else if (index == UNIT_HERO_SALAMANDER) {
    return 26.0f / 30;
  } else if (index == UNIT_HERO_RUDOLPH) {
    return 26.0f / 30;
  } else if (index == UNIT_HERO_SANTADOG) {
    return 28.0f / 30;
  } else if (index == UNIT_HERO_UNDINE) {
    return 24.0f / 30;
  } else if (index == UNIT_HERO_LADY_WEREWOLF) {
    return 25.0f / 30;
  } else if (index == UNIT_HERO_CATINBOOTS) {
    return 29.0f / 30;
  } else if (index == UNIT_HERO_MOLE) {
    return 19.0f / 30;
  } else if (index == UNIT_HERO_LADY_BEAR) {
    return 23.0f / 30;
  } else if (index == UNIT_HERO_LADY_LION) {
    return 20.0f / 30;
  } else if (index == UNIT_HERO_TOYMOUSE) {
    return 57.0f / 30;
  } else if (index == UNIT_HERO_SAVAGEARCHER) {
    return 30.0f / 30;
  } else if (index == UNIT_HERO_BATMONSTER) {
    return 22.0f / 30;
  } else if (index == UNIT_HERO_PARASITE) {
    return 29.0f / 30;
  } else if (index == UNIT_HERO_WATERMELON) {
    return 25.0f / 30;
  } else if (index == UNIT_HERO_BABYMINO) {
    return 23.0f / 30;
  } else if (index == UNIT_HERO_MINO) {
    return 19.0f / 30;
  } else if (index == UNIT_HERO_KERBEROS) {
    return 16.0f / 30;
  } else if (index == UNIT_HERO_LAMIA) {
    return 25.0f / 30;
  } else if (index == UNIT_HERO_CHUNJA) {
    return 13.0f / 30;
  } else if (index == UNIT_HERO_GOLEM) {
    return 47.0f / 30;
  }
  return 0.2f;
}
int GameScene::getUnitMaxHP(int unit) { return GM->getUnitMaxHP(unit); }
void GameScene::stopWoongSound() { readyToMove = true; }
void GameScene::addDirtToTile(TMXTiledMap *map, Vec2 pos) {
  Sprite *sptDirt;
  for (int i = 0; i < 4; i++) {
    sptDirt = Sprite::createWithSpriteFrameName(rand() % 2 == 0 ? "dirt1.png"
                                                                : "dirt0.png");
    batch->addChild(sptDirt, 100);
    float dur = 1;
    sptDirt->runAction(MoveBy::create(0.1, Vec2(0, -2)));
    sptDirt->runAction(Sequence::create(
        EaseOut::create(MoveBy::create(dur, Vec2(i % 2 == 0 ? -4 : 4, 4)), 6),
        CallFuncN::create(
            CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptDirt)),
        NULL));
    sptDirt->runAction(Sequence::create(DelayTime::create(dur / 2),
                                        FadeOut::create(dur / 2), NULL));
    sptDirt->setPosition(
        Vec2(pos.x * TILE_SIZE + (i % 2 == 0 ? 0 : TILE_SIZE),
             (map->getMapSize().height - pos.y - 1) * TILE_SIZE +
                 (i > 1 ? TILE_SIZE : 0)) +
        map->getPosition());
    // log("dirt %f, %f", sptDirt->getPosition().x, sptDirt->getPosition().y);
  }
}

void GameScene::cloudSchedule(float dt) {
  for (auto cloud : cloudArray) {
    if (cloud->getPosition().x < 0) {
      cloud->setPosition(Vec2(cloud->getParent()->getContentSize().width +
                                  cloud->getContentSize().width / 2,
                              cloud->getPosition().y));
    }
  }
}
void GameScene::saveCoinIfStarCollected() {
  //    if (starCount > 0) {
  if (collectedCoinCount < MAX_COIN_COUNT) {
    GameManager::getInstance()->addCoin(collectedCoinCount);
  }
  GameManager::getInstance()->saveCoin();
  int totalKillCount = UserDefault::getInstance()->getIntegerForKey("KC", 0);
  totalKillCount += killCountForRecord;
  UserDefault::getInstance()->setIntegerForKey("KC", totalKillCount);
  //    }
}
void GameScene::sacrificeLater(float dt) {}
void GameScene::sacrificeDone(Ref *obj) {}

void GameScene::addRide(int unit, Vec2 pos) {
  const char *sptName = "rideTop.png";

  Movable *board = Movable::createMovable(unit, 1, 0, sptName);
  board->setPosition(pos);
  board->setTag(unit);

  /*Sprite* sprite = Sprite::createWithSpriteFrameName("silverCoin0.png");
  sprite->setPosition(Vec2(16, 20));
  sprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(MoveBy::create(0.3f,
  Vec2(0, 15)), MoveBy::create(0.3f, Vec2(0, -15)), NULL)));
  board->addChild(sprite);*/

  batch->addChild(board);
  helperArray.pushBack(board);
  float time = 0.6f;
  MoveTo *moveUpTop = MoveTo::create(time, pos + Vec2(0, 10));
  MoveTo *moveDownTop = MoveTo::create(time, pos);
  MoveTo *moveDownBottom = MoveTo::create(time, pos + Vec2(0, -10));
  MoveTo *moveUpBottom = MoveTo::create(time, pos);
  float rate = 1;

  board->runAction(RepeatForever::create((ActionInterval *)Sequence::create(
      EaseIn::create(moveUpTop, rate), EaseOut::create(moveDownTop, rate),
      EaseIn::create(moveDownBottom, rate), EaseOut::create(moveUpBottom, rate),
      NULL)));
}
void GameScene::splashDamage(Vec2 pos, int radius, int damage,
                              bool isFromEnemy, Movable *attacker) {
  GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
  //    draw->drawSolidCircle(pos, radius, 360, 20, 1, 1, Color4F::RED);
  //    for(auto unit:heroArray){
  //        if (unit->getPosition().distanceSquared(pos) < sqrt(radius)) {
  //            if(unit->getHitAndIsDead(damage)){
  //                removeDeadUnit(unit);
  //            }
  //        }
  //    }
  int counter = 0;
  Vector<EnemyBase *> deadUnits;
  auto &splashTargets = isFromEnemy ? heroArray : enemyArray;
  for (auto unit : splashTargets) {
    if (!unit) {
      continue;
    }
    if (unit->getPosition().distanceSquared(pos) < radius) {
      if (canAttack(attacker, unit) &&
          unit->getHitAndIsDead(damage, attacker)) {
        deadUnits.pushBack(unit);
      }
      counter++;
    }
    if (counter > 10) {
      break;
    }
  }
  for (auto unit : deadUnits) {
    removeDeadUnit(unit);
  }
  if (attacker && attacker->unitType == UNIT_WIZARD) {
    Vector<EnemyBase *> deadTrees;
    for (auto tree : mutualArray) {
      if (!tree || tree->unitType != UNIT_TREE_FOR_BATTLE) {
        continue;
      }
      if (tree->getPosition().distanceSquared(pos) < radius) {
        if (tree->getHitAndIsDead(damage, attacker)) {
          deadTrees.pushBack(tree);
        }
      }
    }
    for (auto tree : deadTrees) {
      removeDeadUnit(tree);
    }
  }

  //    if (target && target->energy > 0) {
  //        if(target->getHitAndIsDead(ap)){
  //            stop();
  //        }
  //    }
}
void GameScene::updateIndicators() {
  Movable *unit;
  float eX = 0;
  float eY = 0;
  float centerX = -this->getPosition().x + size.width / 2;
  float centerY = -this->getPosition().y + size.height / 2;
  float a;
  float y;
  float x;
  cocos2d::Rect rect;
  int outOfBoxCount = 0;
  Sprite *indicator;
  Sprite *spt;
  float scale = this->getScale();
  float offset = 15;
  for (int i = 0; i < indicatedArray.size(); i++) {
    spt = (Sprite *)indicatedArray.at(i);

    rect = cocos2d::Rect(-this->getPosition().x / scale,
                         -this->getPosition().y / scale, size.width / scale,
                         size.height / scale);
    if (rect.containsPoint(unit->getPosition())) {
      continue;
    } else {
    }
    eX = unit->getPosition().x * scale;
    eY = unit->getPosition().y * scale;
    a = (eY - centerY) / (eX - centerX);
    y = centerY > eY ? (-size.height / 2 + offset) : (size.height / 2 - offset);
    x = y / a;
    if (x > size.width / 2) { // right
      x = size.width / 2 - offset;
      y = x * a;
    } else if (x < -size.width / 2) { // left
      x = -size.width / 2 + offset;
      y = x * a;
    } else {
    }

    if (indicatorArray.size() > outOfBoxCount) {
      //            indicator = indicatorArray.at(outOfBoxCount);
      for (auto indi : indicatorArray) {
        if (indi->getTag() == unit->getTag()) {
          indicator = indi;
          break;
        }
      }
    } else {
      indicator = Sprite::create("blueArrow.png");
      indicator->setTag(spt->getTag());

      spt->setTag(79);
      indicator->addChild(spt);

      spt->setPosition(Vec2(-50, indicator->getContentSize().height *
                                     indicator->getScale() / 2));
      indicator->setScale(0.5);
      this->getParent()->addChild(indicator);
      GameManager::getInstance()->makeItSiluk(indicator);
      indicatorArray.pushBack(indicator);
    }

    indicator->setRotation(-atan2(y, x) * 180.0f / 3.14f);
    indicator->setPosition(Vec2(size.width / 2 + x, size.height / 2 + y));

    spt = (Sprite *)indicator->getChildByTag(79);
    spt->setRotation(-indicator->getRotation());

    outOfBoxCount++;
  }

  if (indicatorArray.size() > outOfBoxCount) {
    //        for (int i = (int)indicatorArray.size() - 1; i >= outOfBoxCount;
    //        i--) {
    //            indicator = (Sprite*)indicatorArray.at(i);
    //            indicatorArray.eraseObject(indicator);
    //            indicator->removeFromParentAndCleanup(true);
    //        }
    for (auto indi : indicatorArray) {
      indi->removeFromParentAndCleanup(true);
    }
    indicatorArray.clear();
    updateIndicators();
  }
}

void GameScene::removeUsedAssets() {
  CCLOG("HelloWorld removeUsedAssets");

  for (auto lbl : talkLabelArray) {
    lbl->release();
  }
  _eventDispatcher->removeEventListener(listener);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  if (mouseListener != nullptr) {
    _eventDispatcher->removeEventListener(mouseListener);
    mouseListener = nullptr;
  }
  if (usesPcRtsControls()) {
    Win32MouseCursor::restoreSystemCursor();
  }
#endif
  GM->setCurrentStageLayer(nullptr);

  for (auto drop : enemyHeroList) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : heroList) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : enemyArray) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : heroArray) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : mutualArray) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : batch->getChildren()) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }
  for (auto drop : batch->getChildren()) {
    drop->stopAllActions();
    drop->unscheduleAllCallbacks();
  }

  //    this->removeAllChildren();
  this->unscheduleAllCallbacks();
  this->stopAllActions();
  //    GameManager::getInstance()->setHudLayer(NULL);
  //    GameManager::getInstance()->setCurrentStageLayer(NULL);
}
void GameScene::testSchedule(float dt) {}

void GameScene::doTheBombToMissiles(float dt) {
  CCLOG("doTheBombToMissiles");
  if (bombTargetMissileArray.size() <= 0) {
    this->unschedule(schedule_selector(GameScene::doTheBombToMissiles));
    bombTargetEnemyIndex = 0;
    this->schedule(schedule_selector(GameScene::doTheBombToEnemies), 0.1,
                   bombTargetEnemyArray.size(), 0.1);
    return;
  }

  Movable *drop = bombTargetMissileArray.at(0);
  Sprite *sprite = Sprite::createWithSpriteFrameName("daryFace.png");
  sprite->runAction(Sequence::create(
      ScaleTo::create(0.5, 5),
      CallFuncN::create(CC_CALLBACK_1(GameScene::enemyBaseMoveDone, this)),
      NULL));
  sprite->runAction(FadeOut::create(0.5));
  batch->addChild(sprite);
  switch (drop->getTag()) {
  case UNIT_MISSILE_CHASING:
    chasingMissileArray.eraseObject(drop);
    break;
  case UNIT_MISSILE_Movable:
    MovableMissileArray.eraseObject(drop);
    break;
  case UNIT_MISSILE_CUSTOM:
    customMovingArray.eraseObject(drop);
    break;
  case UNIT_MISSILE_STRAIGHT:
    straightMovingArray.eraseObject(drop);
    break;
  default:
    break;
  }
  sprite->setPosition(drop->getPosition());
  bombTargetMissileArray.erase(0);
  enemyMissileArray.eraseObject(drop);
  batch->removeChild(drop, true);
  batch->removeChild(drop, true);

  Vec2 screenPos = this->getPosition();
  this->runAction(Sequence::create(
      MoveBy::create(0.05, Vec2(rand() % 36 - 18, rand() % 36 - 18)),
      MoveTo::create(0.05, screenPos), NULL));
}

void GameScene::doTheBombToEnemies(float dt) {
  float demage = 1;

  if (bombTargetEnemyArray.size() <= bombTargetEnemyIndex) {
    this->unschedule(schedule_selector(GameScene::doTheBombToEnemies));
    isPaused = false;

    for (auto spt : enemyArray) {
      spt->resume();
    }

    for (auto spt : heroMissileArray) {
      spt->resume();
    }

    for (auto spt : enemyMissileArray) {
      spt->resume();
    }

    for (auto enemy : bombTargetEnemyArray) {
      if (enemy->untouchable) {

      } else {
        enemy->energy -= (int)demage;

        if (enemy == theBoss) {
          //                    GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
        } else {
          enemy->desiredPosition =
              enemy->desiredPosition + Vec2(enemy->isTowardLeft ? 2 : -2, 2);
          enemy->setPosition(enemy->getPosition() +
                             Vec2(enemy->isTowardLeft ? 2 : -2, 2));
        }

        showDamage(demage, enemy->getPosition());

        if (enemy->energy <= 0) {
          this->runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());

          enemyArray.eraseObject(enemy);
          MovableArray.eraseObject(enemy);
          batch->removeChild(enemy, true);
          batch->removeChild(enemy, true);
        } else {
          enemy->blinkForAWhile();
        }
      }
    }

    return;
  }

  EnemyBase *enemy = bombTargetEnemyArray.at(bombTargetEnemyIndex);
  Sprite *sprite = Sprite::createWithSpriteFrameName("daryFace.png");
  sprite->runAction(Sequence::create(
      ScaleTo::create(0.5, 5),
      CallFuncN::create(CC_CALLBACK_1(GameScene::enemyBaseMoveDone, this)),
      NULL));
  sprite->runAction(FadeOut::create(0.5));
  batch->addChild(sprite);
  sprite->setPosition(enemy->getPosition());

  Vec2 screenPos = this->getPosition();
  this->runAction(Sequence::create(
      MoveBy::create(0.05, Vec2(rand() % 18 - 9, rand() % 18 - 9)),
      MoveTo::create(0.05, screenPos), NULL));

  bombTargetEnemyIndex++;
}

void GameScene::enemyFireLoop(float dt) {
  for (auto enemy : fireableArray) {
    while (enemy->missileArray.size() > 0) {
      Movable *missile = enemy->missileArray.at(0);
      enemy->missileArray.erase(0);
      Node *light = getLight();
      light->setPosition(missile->getPosition());
      this->addChild(light);
      light->runAction(
          Sequence::create(DelayTime::create(0.1),
                           CallFuncN::create(CC_CALLBACK_1(
                               Node::removeFromParentAndCleanup, light)),
                           NULL));
      light->setColor(Color3B(250, 100, 100));
      if (missile->getScaleX() == missile->getScaleY()) {
        missile->runAction(ScaleTo::create(0.2f, 1));
      }

      if (missile->getTag() == UNIT_MISSILE_Movable) {
        MovableMissileArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);

        missile = NULL;
      } else if (missile->getTag() == UNIT_MISSILE_STRAIGHT) {
        straightMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        // missile->secondTag = enemy->getTag();
        missile = NULL;
      } else if (missile->getTag() == UNIT_MISSILE_CUSTOM) {
        customMovingArray.pushBack(missile);
        enemyMissileArray.pushBack(missile);
        // missile->secondTag = enemy->getTag();
        missile = NULL;
      } else if (missile->getTag() == UNIT_MISSILE_CHASING) {
        chasingMissileArray.pushBack(missile);

        missile = NULL;
      }
    }
  }
}
void GameScene::removeMeFromChasing(Ref *ref) {
  Movable *drop = (Movable *)ref;
  drop->setTag(UNIT_MISSILE_STRAIGHT);
  chasingMissileArray.eraseObject(drop);
  straightMovingArray.pushBack(drop);
}

void GameScene::bubbleUpdate(float dt) {
  char buf[80];
  sprintf(buf, "theme_%d_Background_tower_%d_shadow.png",
          GameManager::getInstance()->theme, rand() % 2);

  for (auto bubble : bubbleArray) {
    bubble->setPosition(
        Vec2(bubble->getPosition().x, bubble->getPosition().y + 50 * dt));

    TMXTiledMap *map = getTileMap(bubble->getPosition());
    //        Vec2 plPos = this->getCoordinateFromPosition(bubble->getPosition()
    //        + Vec2(0, - bubble->getContentSize().height), map);
    int tgid = getTileAtPosition(bubble->getPosition(), TAG_FORE_LAYER, map);
    if (tgid == 0) {
      bubbleArray.eraseObject(bubble);
      bubble->stopAllActions();
      sprintf(buf, "bubble%dPop", bubble->getTag());
      Animation *animation = AnimationCache::getInstance()->getAnimation(buf);
      Animate *animate = Animate::create(animation);
      CallFuncN *call =
          CallFuncN::create(CC_CALLBACK_1(GameScene::enemyBaseMoveDone, this));
      bubble->runAction(Sequence::create(animate, call, NULL));
      return;
    }
  }
}
Movable *GameScene::findTargetHero(Movable *finder) {
  float minDistance = 1000000;
  EnemyBase *nearest = nullptr;

  Vec2 finderPos = finder->getPosition();
  Vec2 pos;
  int distanceToSkip = 500;
  for (auto drop : heroArray) {
    pos = drop->getPosition();
    if (pos.x < finderPos.x - distanceToSkip ||
        pos.x > finderPos.x + distanceToSkip ||
        pos.y < finderPos.y - distanceToSkip ||
        pos.y > finderPos.y + distanceToSkip) {
      continue;
    }
    float distance = finder->getPosition().distanceSquared(drop->getPosition());
    if (distance < finder->monitoringDistance) {
      if ((finder->wantToEli || finder->isZombie) && nearest == nullptr) {
        nearest = drop;
        continue;
      }
      if (drop->untouchable || drop->energy <= 0) {
        continue;
      }
      if (!canAttack(finder, drop)) {
        continue;
      }
      if (minDistance > distance) {
        minDistance = distance;
        nearest = drop;
      }
    }
  }

  return nearest;
}
Movable *GameScene::findTargetEnemy(Movable *finder) {
  float minDistance = 1000000;
  EnemyBase *nearest = nullptr;
  Vec2 finderPos = finder->getPosition();
  Vec2 pos;
  int distanceToSkip = 500;
  for (auto unit : enemyArray) {
    pos = unit->getPosition();
    if (pos.x < finderPos.x - distanceToSkip ||
        pos.x > finderPos.x + distanceToSkip ||
        pos.y < finderPos.y - distanceToSkip ||
        pos.y > finderPos.y + distanceToSkip) {
      continue;
    }
    float distance = finderPos.distanceSquared(pos);
    if (distance < finder->monitoringDistance) {

      if (unit->untouchable || unit->energy <= 0) {
        continue;
      }
      if (!canAttack(finder, unit)) {
        continue;
      }
      if (minDistance > distance) {
        if (finder->UnreachableUnitList.find(unit) ==
            finder->UnreachableUnitList.end()) {
          minDistance = distance;
          nearest = unit;
        }
      }
    }
  }
  if (nearest == nullptr) {
    if (finder->unitType == UNIT_WIZARD) {
      EnemyBase *nearestTree = nullptr;
      for (auto tree : mutualArray) {
        if (tree->unitType == UNIT_TREE_FOR_BATTLE) {
          float distance =
              finder->getPosition().distanceSquared(tree->getPosition());
          if (distance < finder->monitoringDistance) {
            if (minDistance > distance) {
              minDistance = distance;
              nearestTree = tree;
            }
          }
        }
      }
      return nearestTree;
    } else {
      return nullptr;
    }
  } else {

    return nearest;
  }
}
void GameScene::autoTargeting() {
  //    if (!isAutoTargetingOn) {
  //        return;
  //    }
  //    Movable* enemy = findTarget(200);
  //    float angle;
  //
  //    int radius = 170;
  //    float dx = cosf(angle) * radius;
  //    float dy = sinf(angle) * radius;

  //    Vec2 velocity = Vec2(dx/joystickRadius, dy/joystickRadius);
  //    float degrees = angle * SJ_RAD2DEG;
  //
  //    // Update the thumb's position
  //    Vec2 stickPosition = Vec2(dx, dy);
  //
  //    Vec2 location = Vec2(size.width/2 + dx*6, size.height/2 + dy*6);
  //
  //
  //
  //    log("targeting: %f, dx: %f, dy: %f", angle*180/3.14f, dx, dy);
}
void GameScene::entranceSchedule(float dt) {}
void GameScene::releaseCustomMovingCamera() { isCameraInCustomMoving = false; }
// gravityupdate
void GameScene::gravityUpdate(float dt) {
  if (isMultiplay) {
    MM->update(dt);

    // for hero
    for (int i = (int)ownMsgList.size() - 1; i >= 0; i--) {
      HMsg msg = ownMsgList.at(i);

      if (msg.startTime > gameFrameTimer) {
        continue;
      }

      if (msg.msgType.compare(MSG_CODE_UNIT_MOVE) == 0) {
        Vector<EnemyBase *> el;
        ValueVector list = GM->split(msg.sourceIDs, "|");
        for (auto hero : heroArray) {
          for (int i = 0; i < list.size(); i++) {
            if (list.at(i).asInt() == hero->unitID) {
              el.pushBack(hero);
              hero->setVisible(true);
              hero->isGatheringGold = false;
            }
          }
        }
        if (el.size() > 0) {
          Vec2 pos = Vec2(msg.value0, msg.value1);
          moveTo(el, pos);
          //                log("lets move %d", )
        }
        el.clear();
      } else if (msg.msgType.compare(MSG_CODE_ATTACK_DDANG) == 0) {
        Vector<EnemyBase *> el;
        ValueVector list = GM->split(msg.sourceIDs, "|");
        for (auto hero : heroArray) {
          for (int i = 0; i < list.size(); i++) {
            if (list.at(i).asInt() == hero->unitID) {
              el.pushBack(hero);
            }
          }
        }

        if (el.size() > 0) {
          Vec2 pos = Vec2(msg.value0, msg.value1);
          //                moveTo(el, pos);
          moveAndAttackTo(selectedArray, pos);

          //                log("lets move %d", )
        }
        el.clear();
      } else if (msg.msgType.compare(MSG_CODE_ATTACK_TARGET) == 0) {
        Vector<EnemyBase *> el;
        ValueVector list = GM->split(msg.sourceIDs, "|");
        for (auto hero : heroArray) {
          for (int i = 0; i < list.size(); i++) {
            if (list.at(i).asInt() == hero->unitID) {
              el.pushBack(hero);
            }
          }
        }
        if (el.size() > 0) {
          EnemyBase *selectedUnit;
          bool isPicked = false;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == msg.targetID) {
              selectedUnit = enemy;
              isPicked = true;
              break;
            }
          }
          if (isPicked) {
            forceAttack(el, selectedUnit);
          }
        }
        el.clear();
      } else if (msg.msgType.compare(MSG_CODE_WORKER_GO_TO_MINE) == 0) {
        int unitID = Value(msg.sourceIDs).asInt();
        for (auto hero : heroArray) {
          if (unitID == hero->unitID) {
            for (auto mutual : mutualArray) {
              if (mutual->unitID == msg.targetID) {
                setWorkerToMine(hero, mutual);
                break;
              }
            }
            break;
          }
        }
      } else if (msg.msgType.compare(MSG_CODE_WORKER_GO_TO_TREE) == 0) {
        int unitID = Value(msg.sourceIDs).asInt();
        for (auto hero : heroArray) {
          if (unitID == hero->unitID) {
            for (auto mutual : mutualArray) {
              if (mutual->unitID == msg.targetID) {
                setWorkerToTree(hero, mutual);
                hero->setVisible(true);
                break;
              }
            }
            break;
          }
        }
      } else if (msg.msgType.compare(MSG_CODE_CONSTRUCT) == 0) {
        int unitID = Value(msg.sourceIDs).asInt();
        for (auto hero : heroArray) {
          if (unitID == hero->unitID) {
            setWorkerToBuild(hero, msg.value0, msg.value1, msg.value2);

            break;
          }
        }
      }
      ownMsgList.erase(ownMsgList.begin() + i);
    }
    if (!isMultiplayStarted) { // test
      time_t now = BSM->getCurrentTimeT();
      time_t gameStartedTime = MultiplayManager::getInstance()->gameStartedTime;
      bool isReady = difftime(now, gameStartedTime) >= 3;
      if (isReady) {
        isMultiplayStarted = true;
        gameTimer = 0;
      }
    } else {
      MultiplayManager::getInstance()->rmsgLock.lock();
      for (int i = (int)MultiplayManager::getInstance()->rmsgList.size() - 1;
           i >= 0; i--) {
        Msg msg = MultiplayManager::getInstance()->rmsgList.at(i);

        if (frameToCover == 0 && msg.startTime - LATANCY > gameFrameTimer) {
          frameToCover = (msg.startTime - LATANCY) - gameFrameTimer;
          log("frame to cover: %d, %d", msg.startTime, frameToCover);
        }

        if (msg.startTime > gameFrameTimer) {
          continue;
        }

        log("msg.msgType hello: %s", msg.msgType.c_str());
        if (msg.msgType.compare(MSG_CODE_UNIT_MOVE) == 0) {
          bool shouldDoIt = false;
          int executedCount = 0;
          Vector<EnemyBase *> el;
          ValueVector list = GM->split(msg.sourceIDs, "|");
          for (auto enemy : enemyArray) {
            shouldDoIt = false;
            for (int i = 0; i < list.size(); i++) {
              if (list.at(i).asInt() == enemy->unitID) {
                el.pushBack(enemy);
                shouldDoIt = true;
                //                                break;
              }
            }

            if (shouldDoIt) {
              if (enemy == nullptr) {

              } else {
                //                                enemy->runAction(MoveTo::create(0.3f,
                //                                Vec2(msg.value0,
                //                                msg.value1)));

                //                                moveTo(enemy, Vec2(msg.value0,
                //                                msg.value1));
              }
              executedCount++;
              if (executedCount >=
                  list.size()) { // exit if all done before looping the rest
                break;
              }
            }
          }
          moveMultiplayEnemyTo(el, Vec2(msg.value0, msg.value1));
          el.clear();
          //                    if (sourceUnit == nullptr) {
          //                        bool itsDead = false;
          //                        for(int i =0;i < deadEnemyArray.size();i++){
          //                            if(deadEnemyArray.at(i) ==
          //                            msg.sourceID){
          //                                itsDead = true;
          //                                break;
          //                            }
          //                        }
          //
          //                    }else{
          //                        sourceUnit->runAction(MoveTo::create(0.3f,
          //                        Vec2(msg.value0, msg.value1)));
          //                        moveTo(sourceUnit, Vec2(msg.value2,
          //                        msg.value3));
          //                    }
        } else if (msg.msgType.compare(MSG_CODE_ATTACK_DDANG) == 0) {
          Vector<EnemyBase *> el;
          ValueVector list = GM->split(msg.sourceIDs, "|");
          for (auto enemy : enemyArray) {
            for (int i = 0; i < list.size(); i++) {
              if (list.at(i).asInt() == enemy->unitID) {
                el.pushBack(enemy);
                //                                break;
              }
            }
          }

          //                    int sourceID = Value(msg.sourceIDs).asInt();
          int x = msg.value0;
          int y = msg.value1;

          //                    for(auto enemy : enemyArray){
          //                        if (enemy->unitID == sourceID) {
          //                            enemy->attackDdangTo(Vec2(x, y));
          //                            log("multiplay enemy move");
          //                            break;
          //                        }
          //                    }
          Vec2 pos = Vec2(x, y);
          //                    showTargetHand(pos, true);
          //                    onAttackClick();
          moveAndAttackTo(el, pos);

          //                    selectCommand(COMMAND_MOVE);

        } else if (msg.msgType.compare(MSG_CODE_ATTACK_TARGET) == 0) {
          Vector<EnemyBase *> el;
          ValueVector list = GM->split(msg.sourceIDs, "|");
          for (auto enemy : enemyArray) {
            for (int i = 0; i < list.size(); i++) {
              if (list.at(i).asInt() == enemy->unitID) {
                el.pushBack(enemy);
                //                                break;
              }
            }
          }

          EnemyBase *selectedTarget = nullptr;
          bool isPicekd = false;
          for (auto hero : heroArray) {
            if (hero->unitID == msg.targetID) {
              selectedTarget = hero;
              isPicekd = true;
              break;
            }
          }
          if (selectedTarget != nullptr && isPicekd) {
            forceAttack(el, selectedTarget);
          }

          //                    int sourceID = Value(msg.sourceIDs).asInt();
          //                    int targetID = msg.targetID;
          //                    for(auto enemy : enemyArray){
          //                        if (enemy->unitID == sourceID) {
          //                            enemy->unitAct = UNIT_ACT_ATTACK;
          //                            enemy->unitActDetail =
          //                            UNIT_ACT_DETAIL_IDLE; enemy->moveToPos =
          //                            Vec2::ZERO; for(auto hero : heroArray){
          //                                if (hero->unitID == targetID) {
          //                                    enemy->moveFlagPos =
          //                                    hero->getPosition();
          //                                    enemy->target = hero;
          //                                    break;
          //                                }
          //                            }
          //                            enemy->canRevengeAttack = true;
          //                            break;
          //                        }
          //                    }
        } else if (msg.msgType.compare(MSG_CODE_UNIT_CREATED) == 0) {
          log("**enemy created unit: %s, %d, %d, %d", msg.sourceIDs.c_str(),
              msg.value0, msg.value1, msg.value2);
          EnemyBase *unit = createUnit(
              msg.value0, WHICH_SIDE_ENEMY, isBuilding(msg.value0),
              Vec2(msg.value1, msg.value2), GM->getUnitName(msg.value0), 1,
              getSpriteNameForUnit(msg.value0));
          unit->unitID = Value(msg.sourceIDs).asInt();
          if (unit->isBuilding && (unit->unitType == UNIT_CASTLE ||
                                   unit->unitType == UNIT_ORC_HQ)) {
            setOccupy(unit->getPosition() + Vec2(-150, 100) * imageScale, 4, 3,
                      true, unit);
            resetPathState();
          }
        } else if (msg.msgType.compare(MSG_CODE_WORKER_GO_TO_MINE) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              for (auto mutual : mutualArray) {
                if (mutual->unitType == UNIT_MINE &&
                    mutual->unitID == msg.targetID) {
                  enemy->attackType = ATTACK_TYPE_NEAR;
                  enemy->isCarryingGold = false;
                  enemy->canFindTarget = true;
                  enemy->setVisible(true);
                  enemy->isTemporaryFlying = true;
                  enemy->unitName = enemy->getName();
                  enemy->currentAnimationType = ANIMATION_TYPE_IDLE;
                  enemy->runAnimation(ANIMATION_TYPE_MOVE);

                  enemy->gatherGold(mutual);
                  break;
                }
              }
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_WORKER_GO_TO_TREE) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              for (auto mutual : mutualArray) {
                if (mutual->unitType == UNIT_TREE &&
                    mutual->unitID == msg.targetID) {
                  //                                    enemy->isCarryingTree =
                  //                                    false; enemy->stop();
                  //                                    enemy->unitName =
                  //                                    enemy->getName();
                  //                                    enemy->runAnimation(ANIMATION_TYPE_IDLE);
                  //
                  //                                    enemy->gatherTree(mutual);
                  setWorkerToTree(enemy, mutual);
                  break;
                }
              }
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_CONSTRUCT) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          int buildingIndex = msg.value0;
          int tx = msg.value1;
          int ty = msg.value2;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              //                            enemy->builderSize =
              //                            GM->getBuildingOccupySize(buildingIndex);
              //                            enemy->builderBuildingIndex =
              //                            buildingIndex;
              //                            enemy->builderCoordinate = Vec2(tx,
              //                            ty); enemy->builderSpriteName =
              //                            getSpriteNameForUnit(buildingIndex);
              //                            enemy->isGoingToBuild = true;
              //                            Vector<EnemyBase*> el;
              //                            moveMultiplayEnemyTo(el,
              //                            this->getPositionFromTileCoordinate(tx,
              //                            ty) +
              //                            Vec2(enemy->builderSize.width*TILE_SIZE/2,
              //                            enemy->builderSize.height*TILE_SIZE/2));
              //                            el.clear();
              setWorkerToBuild(enemy, buildingIndex, tx, ty);

              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_UNIT_ATTACKED) == 0) {
          //                    EnemyBase* source = nullptr;
          //                    ValueVector list;
          //                    int sourceID = list.at(0).asInt();
          //                    for(auto enemy : enemyArray){
          //                        if(enemy->unitID == sourceID){
          //                            source = enemy;
          //                            break;
          //                        }
          //                    }
          //                    for(auto hero : heroArray){
          //                        if (hero->unitID == msg.targetID && source
          //                        != nullptr) {
          //                            hero->getHitAndIsDead(msg.value0,
          //                            source); break;
          //                        }
          //                    }
        }

        // bug - lag if many units selected order multiple times.
        else if (msg.msgType.compare(MSG_CODE_UNIT_DEAD) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto hero : heroArray) {
            if (hero->unitID == sourceID) {
              removeDeadUnit(hero);
              break;
            }
          }
          int attackerID = msg.value0;
          int ax = msg.value1;
          int ay = msg.value2;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == attackerID) {
              //                            float dur= 0.3f;
              enemy->setPosition(Vec2(ax, ay));
              //                            enemy->runAction(MoveTo::create(0.3f,
              //                            Vec2(ax, ay))); if
              //                            (GM->getDistance(enemy->getPosition(),
              //                            Vec2(ax, ay)) > 100) {
              //                                enemy->runAction(Sequence::create(FadeOut::create(dur/2),
              //                                FadeIn::create(dur/2), NULL));
              //                            }

              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_UNIT_KILLED) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto hero : enemyArray) {
            if (hero->unitID == sourceID) {
              removeDeadUnit(hero);
              break;
            }
          }
          int attackerID = msg.value0;
          int ax = msg.value1;
          int ay = msg.value2;
          for (auto enemy : heroArray) {
            if (enemy->unitID == attackerID) {
              //                            float dur= 0.3f;
              enemy->setPosition(Vec2(ax, ay));
              //                            enemy->runAction(MoveTo::create(0.3f,
              //                            Vec2(ax, ay))); if
              //                            (GM->getDistance(enemy->getPosition(),
              //                            Vec2(ax, ay)) > 100) {
              //                                enemy->runAction(Sequence::create(FadeOut::create(dur/2),
              //                                FadeIn::create(dur/2), NULL));
              //                            }

              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_CUT_TREE) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          EnemyBase *tree = nullptr;
          for (auto mutual : mutualArray) {
            if (mutual->unitType == UNIT_TREE && mutual->unitID == sourceID) {
              tree = mutual;
              Vec2 treePos =
                  getCoordinateFromPosition(mutual->getPosition(), theMap);
              decoLayer->setTileGID(0, treePos);
              updateMiniMapForNonMoving();
              resetPathState();

              mutual->setSpriteFrame(strmake("treeTrunk%d.png", rand() % 3));
              mutual->setLocalZOrder(-10000);
              mutual->runAction(
                  Sequence::create(FadeOut::create(5),
                                   CallFunc::create(CC_CALLBACK_0(
                                       Node::removeFromParent, mutual)),
                                   NULL));
              mutual->setLocalZOrder(-this->getBoundingBox().origin.y - 100);
              mutual->childrenSprite.clear();

              break;
            }
          }
          int workerID = msg.value0;
          int ax = msg.value1;
          int ay = msg.value2;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == workerID) {
              enemy->setPosition(Vec2(ax, ay));
              enemy->isCarryingTree = true;
              enemy->unitName = strmake("%sWood", getName().c_str());
              //                            EnemyBase* tank =
              //                            getNearestLumberTank(getPosition(),
              //                            enemy->isEnemy);
              EnemyBase *tank = nullptr;
              if (msg.value3 >= 0) {
                for (auto building : enemyArray) {
                  if (building->unitID == msg.value3) {
                    tank = building;
                    break;
                  }
                }
              }
              if (tank != nullptr) {
                enemy->returningPlace = tank;
                enemy->moveToTarget(tank);
                enemy->unitAct = UNIT_ACT_GATHER_TREE;
              } else {
                enemy->runAnimation(ANIMATION_TYPE_IDLE);
              }
              if (tree != nullptr) {
                mutualArray.eraseObject(tree);
              }
              enemy->currentTree = nullptr;

              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_WORKER_BRING_GOLD_TO) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          EnemyBase *sourceEnemy;
          EnemyBase *returningPlace;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              sourceEnemy = enemy;
            } else if (enemy->unitID == msg.value0) {
              returningPlace = enemy;
            }
          }
          if (returningPlace && sourceEnemy) {
            sourceEnemy->returningPlace = returningPlace;
            sourceEnemy->moveToTarget(returningPlace);
            sourceEnemy->unitAct = UNIT_ACT_GATHER_GOLD;
            sourceEnemy->isGatheringTree = true;
          }
        } else if (msg.msgType.compare(MSG_CODE_WORKER_BRING_LUMBER_TO) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          EnemyBase *sourceEnemy;
          EnemyBase *returningPlace;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              sourceEnemy = enemy;
            } else if (enemy->unitID == msg.value0) {
              returningPlace = enemy;
            }
          }
          if (returningPlace && sourceEnemy) {
            sourceEnemy->returningPlace = returningPlace;
            sourceEnemy->moveToTarget(returningPlace);
            sourceEnemy->unitAct = UNIT_ACT_GATHER_TREE;
            sourceEnemy->isGatheringTree = true;
          }
        } else if (msg.msgType.compare(MSG_CODE_UNIT_STOP) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              enemy->stopNew();
              enemy->runAction(
                  MoveTo::create(0.2f, Vec2(msg.value0, msg.value1)));
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_MOVE_ATTACK) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              moveAndAttackTo(enemy, Vec2(msg.value0, msg.value1));
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_ENEMY_MOVE_ATTACK) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          for (auto hero : heroArray) {
            if (hero->unitID == sourceID) {
              moveAndAttackTo(hero, Vec2(msg.value0, msg.value1));
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_MOVEFLAG_CHANGED) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          int x = msg.value0;
          int y = msg.value1;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              enemy->unitAct = UNIT_ACT_MOVE;
              enemy->unitActDetail = UNIT_ACT_DETAIL_IDLE;
              enemy->moveToPos = Vec2::ZERO;
              enemy->moveFlagPos = Vec2(x, y);
              enemy->target = nullptr;
              enemy->canRevengeAttack = true;
              log("multiplay enemy move");
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_TARGET_CHANGED) == 0) {
          int sourceID = Value(msg.sourceIDs).asInt();
          int targetID = msg.targetID;
          for (auto enemy : enemyArray) {
            if (enemy->unitID == sourceID) {
              enemy->unitAct = UNIT_ACT_ATTACK;
              enemy->unitActDetail = UNIT_ACT_DETAIL_IDLE;
              enemy->moveToPos = Vec2::ZERO;
              for (auto hero : heroArray) {
                if (hero->unitID == targetID) {
                  enemy->moveFlagPos = hero->getPosition();
                  enemy->target = hero;
                  log("multiplay enemy set target");
                  break;
                }
              }
              enemy->canRevengeAttack = false;
              break;
            }
          }
        } else if (msg.msgType.compare(MSG_CODE_PAUSED) == 0) {
          enemyPaused();
        } else if (msg.msgType.compare(MSG_CODE_RESUMED) == 0) {
          enemyResumed();
        } else if (msg.msgType.compare(MSG_CODE_QUIT) == 0) {
          endGame(true);
        }
        MultiplayManager::getInstance()->rmsgList.erase(
            MultiplayManager::getInstance()->rmsgList.begin() + i);
      }
      //            MultiplayManager::getInstance()->rmsgList.clear();
      MultiplayManager::getInstance()->rmsgLock.unlock();

      gravityUpdateHandler(dt);
      gameFrameTimer++;
      if (frameToCover > 0) {
        gravityUpdateHandler(dt);
        //                gameTimer -= dt;
        gameFrameTimer++;
        frameToCover--;
      }
      //            log("gameFrameTimer %d", gameFrameTimer);
      //            log("gameTimer %f", gameTimer);
    }
  } else {

    if (dt > 0.033f && gameSpeed > 1) {
      dt = 0.033f;
    }
    for (int i = 0; i < gameSpeed; i++) {
      gravityUpdateHandler(dt);
    }
  }
}
void GameScene::enemyPaused() {}
void GameScene::enemyResumed() {}
bool GameScene::isBuilding(int unitType) {
  return unitType == UNIT_CASTLE || unitType == UNIT_FARM ||
         unitType == UNIT_BARRACKS || unitType == UNIT_LUMBERMILL ||
         unitType == UNIT_FACTORY || unitType == UNIT_AIRPORT ||
         unitType == UNIT_WATCHERTOWER || unitType == UNIT_ORC_HQ ||
         unitType == UNIT_ORC_BUNKER || unitType == UNIT_BARBECUE ||
         unitType == UNIT_ORC_BARRACKS || unitType == UNIT_TEMPLE ||
         unitType == UNIT_ORC_TROLL_HOUSE;
}
void GameScene::gravityUpdateHandler(float dt) {
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
    updatePvp(dt);
    if (HUD->pvpTimeLeftToStart > 1) {
      return;
    }
  } else {
    if (heroList.size() < 6 && heroListToCreate.size() > 0) {
      EnemyBase *hero = heroListToCreate.at(0);
      heroListToCreate.eraseObject(hero);
      unitsToCreateArray.pushBack(hero);
      heroList.pushBack(hero);
      hero->setVisible(true);
      hero->spine->setVisible(true);
      hero->setLocalZOrder(-hero->getBoundingBox().getMinY());
      hero->setLocalZOrder(-hero->spine->getBoundingBox().getMinY());
      runEffect(EFFECT_EXPLODE_MIDDLE, hero->getPosition());
    }
  }
  if (finishedVideo >= 0) {
    try {
      if (finishedVideo == VIDEO_SUPPORT_0) {
        int unitType = UNIT_WORKER;
        if (GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24) {
          unitType = UNIT_GOBLIN_WORKER;
        }
        for (int i = 0; i < 4; i++) {
          createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                     supportPointArray->getControlPointAtIndex(i),
                     GM->getUnitName(unitType));
        }
      } else if (finishedVideo == VIDEO_SUPPORT_1) {
        for (int i = 0; i < 3; i++) {
          int unitType = UNIT_SWORDMAN;
          if (GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24) {
            unitType = UNIT_GOBLIN;
          }
          createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                     supportPointArray->getControlPointAtIndex(i),
                     GM->getUnitName(unitType));
          unitType = UNIT_ARCHER;
          if (GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24) {
            unitType = UNIT_GOBLIN_BOMB;
          }
          createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                     supportPointArray->getControlPointAtIndex(i + 3),
                     GM->getUnitName(unitType));
        }
      } else if (finishedVideo == VIDEO_SUPPORT_2) {
        int unitType = UNIT_CATAPULT;
        if (GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24) {
          unitType = UNIT_TROLL;
        }
        createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                   supportPointArray->getControlPointAtIndex(0),
                   GM->getUnitName(unitType));
        unitType = UNIT_HELICOPTER;
        if (GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24) {
          unitType = UNIT_WIZARD;
        }
        createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING,
                   supportPointArray->getControlPointAtIndex(1),
                   GM->getUnitName(unitType));
      }
    } catch (...) {
      log("support ads crash!");
      // Handle exception here.
    }
    finishedVideo = -1;
  }
  for (auto drop : heroArray) {
    if (!drop->isArrived) {
      if (drop->isFlying) {
        if (drop->getLocalZOrder() != 10) drop->setLocalZOrder(10);
      } else {
        int z = -(int)drop->getPositionY();
        if (drop->getLocalZOrder() != z) {
          drop->setLocalZOrder(z);
          if (drop->spine) drop->spine->setLocalZOrder(z);
        }
      }
    }
  }
  for (auto drop : enemyArray) {
    if (!drop->isArrived) {
      if (drop->isFlying) {
        if (drop->getLocalZOrder() != 10) drop->setLocalZOrder(10);
      } else {
        int z = -(int)drop->getPositionY();
        if (drop->getLocalZOrder() != z) {
          drop->setLocalZOrder(z);
          if (drop->spine) drop->spine->setLocalZOrder(z);
        }
      }
    }
  }
  if (isGameOver) {
    return;
  }
  if (isPaused || isInEvent) {
    return;
  }

  if (isGameClear) {
    endGame();
    return;
  }
  if (doubleClickTimerForSelectTheSame > 0) {
    doubleClickTimerForSelectTheSame -= dt;
  }
  if (doubleClickTimerForAttackDdaing > 0) {
    doubleClickTimerForAttackDdaing -= dt;
  }
  gameTimer += dt;
  //        gameTimer += 60*60*10; // test
  if (GM->currentStageIndex != STAGE_LOBBY) {

    if (lastTimeCheck + 60 < gameTimer) {
      lastTimeCheck = gameTimer;
      checkClearGame();
    }
    if (lastTenTick + 10 < gameTimer) {
      lastTenTick = gameTimer;
      checkGameSchedule();
    }
    if (lastTick + 1 < gameTimer) {
      lastTick = gameTimer;
      HUD->lblTimer->setString(GM->getTimeLeftInString(lastTick));
      updateTriggers();
    }
  }

  if (stageCover != nullptr) {
    //        Vec2 plPos = this->getCoordinateFromPosition(player->getPosition()
    //        + Vec2(0, -player->collectBoundingBox().size.height/2), theMap);
    Vec2 plPos;
    int tgid = getTileGIDAt(stageCover, plPos);
    if (tgid && !isStageCoverTransparent) {
      setStageCoverOpacity(167);
      isStageCoverTransparent = true;
    } else if (tgid == 0 && isStageCoverTransparent) {
      setStageCoverOpacity(255);
      isStageCoverTransparent = false;
    }
  }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  if (usesPcRtsControls()) {
    updateWin32EdgeScroll(win32LastMouseViewPos);
  }
#endif

  float mapMoveSpeed = 30;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  bool mapMoveUp = isMapMoveUp || isMapMoveUpByEdge;
  bool mapMoveDown = isMapMoveDown || isMapMoveDownByEdge;
  bool mapMoveLeft = isMapMoveLeft || isMapMoveLeftByEdge;
  bool mapMoveRight = isMapMoveRight || isMapMoveRightByEdge;
#else
  bool mapMoveUp = isMapMoveUp;
  bool mapMoveDown = isMapMoveDown;
  bool mapMoveLeft = isMapMoveLeft;
  bool mapMoveRight = isMapMoveRight;
#endif
  if (mapMoveUp) {
    float next = getPositionY() - mapMoveSpeed;
    if (next < -TILE_SIZE * mapSize.height * layerScale + size.height) {
      next = -TILE_SIZE * mapSize.height * layerScale + size.height;
    }
    this->setPositionY(next);
    moveScreen(getPosition());
  }
  if (mapMoveDown) {
    float next = getPositionY() + mapMoveSpeed;
    if (next > 0) {
      next = 0;
    }
    this->setPositionY(next);
    moveScreen(getPosition());
  }
  if (mapMoveLeft) {
    float next = getPositionX() + mapMoveSpeed;
    if (next > 0) {
      next = 0;
    }
    this->setPositionX(next);
    moveScreen(getPosition());
  }
  if (mapMoveRight) {
    float next = getPositionX() - mapMoveSpeed;
    if (next < -TILE_SIZE * mapSize.width * layerScale + size.width) {
      next = -TILE_SIZE * mapSize.width * layerScale + size.width;
    }
    this->setPositionX(next);
    moveScreen(getPosition());
  }
  if (isSnowing) {
    for (int i = 0; i < 0; i++) {
      Sprite *sptSnow = Sprite::createWithSpriteFrameName(
          StringUtils::format("snowFlake%d.png", rand() % 4));
      batch->addChild(sptSnow);
      int color = rand() % 30 + 255 - 30;
      sptSnow->setColor(Color3B(color, color, color));
      float dur = 0.4f + (rand() % 10) * 0.05f;
      sptSnow->runAction(RotateBy::create(dur, rand() % 200 + 150));
      sptSnow->runAction(FadeTo::create(dur, 50));
      int xMove = 30;
      int yMove = 50;
      sptSnow->setPosition(rand() % 256 + xMove, rand() % 256 + yMove);
      sptSnow->runAction(Sequence::create(
          MoveBy::create(dur, Vec2(rand() % xMove + 5, -yMove)),
          CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptSnow)),
          NULL));
    }
  }

  draw->clear();
  //    GM->drawPath(); // test
  //    for(auto unit: selectedArray){
  //        draw->drawCircle(unit->getPosition() + Vec2(0,
  //        -unit->getContentSize().height/2), unit->getContentSize().width/2,
  //        360, 20, false, 1, 0.25f, Color4F::GREEN);
  //    }

  if (dt > 0.03) {
    dt = 0.03;
  }

  if (isGameOver) {
    return;
  }
  if (GameManager::getInstance()->totalTime > 0) {
    GameManager::getInstance()->totalTime -= dt;
  }

  //    updateUnitMove(dt);
  updateUnitMoveNew(dt);
  for (auto unit : unitsToCreateArray) {
    if (unit->isEnemy) {
      enemyArray.pushBack(unit);
      if (unit->wantToEli) {
        attackNearHero((EnemyBase *)unit);
      }
    } else if (unit->alliSide == WHICH_SIDE_HERO) {
      heroArray.pushBack(unit);
      updateFoodInUse();
      updateFoodMaxState();
      if (unit->isBuilding) {
        unit->setLocalZOrder(-unit->getPositionY());
      } else {
        unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
      }

    } else if (unit->alliSide == WHICH_SIDE_MUTUAL) {
      mutualArray.pushBack(unit);
    } else if (unit->alliSide == WHICH_SIDE_READY_HERO) {
      readyHeroArray.pushBack(unit);
    }
    if (unit->level > 0 && !unit->isBuilding) {
      Node *nd = Node::create();
      this->addChild(nd);
      Sprite *sptCircle = Sprite::create("whiteCircleLine.png");
      nd->addChild(sptCircle, -1);
      sptCircle->setScale((unit->getContentSize().width) * 1.2f /
                          sptCircle->getContentSize().width);
      sptCircle->setColor(GM->getCardColorForLevel(unit->level));
      unit->ndLevelCircle = nd;
      nd->setPosition(unit->getEffectStartPosition());
      nd->setScale(1, 0.5f);
      sptCircle->runAction(RepeatForever::create(RotateBy::create(1, 180)));
    }
    if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
      moveAndAttackTo(
          unit, theMap->getPosition() +
                    Vec2((20) * TILE_SIZE + TILE_SIZE / 2,
                         (theMap->getMapSize().height - 18 - 1) * TILE_SIZE));
    }
    unit->extraSpeed /= 2;
  }
  bool shouldExit = false;
  for (auto unit : readyHeroArray) {
    if (unit->getPosition() == Vec2::ZERO) {
      continue;
    }
    if (unit->getPositionX() == 0 && unit->getPositionY() == 0) {
      continue;
    }

    for (auto hero : heroArray) {
      if (hero->getPosition().distanceSquared(unit->getPosition()) < 125000) {
        unit->blinkSelectedCircle();
        readyHeroArray.eraseObject(unit);
        heroArray.pushBack(unit);
        unit->alliSide = WHICH_SIDE_HERO;
        shouldExit = true;
        break;
      }
    }
    if (shouldExit) {
      break;
    }
  }
  unitsToCreateArray.clear();

  if (!isGameOver) {
    for (auto drop : MovableArray) {
      //            drop->setLocalZOrder(-drop->getBoundingBox().getMinY());

      if (drop->freezed) {
        continue;
      }
      drop->updatePosition(dt);
      //            checkForAndResolveCollisions(drop);
      if (drop->sptWeapon != nullptr) {
        drop->sptWeapon->setLocalZOrder(-drop->getBoundingBox().origin.y);
      }
    }
  }

  testingFlag = !testingFlag;

  if (GameManager::getInstance()->firePressed) {
    if (playerFireCoolTime > 0) {
      //            playerFireCoolTime -= dt;
    } else {
      if (playerFireCoolTimeMax < 0.001) {
        playerFireCoolTimeMax = 0.3f;
      }
      playerFireCoolTime = playerFireCoolTimeMax;
      fire();
    }
  } else {
    //        playerFireCoolTime = 0;
    isFired = false;
    removeLaser();
    //        if (player->weapon == WEAPON_GUN) {
    //            playerFireCoolTime = 0;
    //        }
  }

  for (auto drop : MovableMissileArray) {
    drop->updatePosition(dt);
    if (checkMovableMissileCollision(drop))
      break;
  }

  //    player->armatureBody->setPosition(player->getPosition());
  //    this->updateIndicators();

  //    stickTimeLeft -= dt;
  //    if (stickTimeLeft < 0 && stageIndex != STAGE_ENTRANCE) {

  //        stickTimeLeft = 0.1f + (rand()%40)*0.1f;
  //    }

  if (isGameEnded) {
    return;
  }

  updateFireStick(dt);
  oneSecUpdate(dt);
  halfSecUpdate(dt);
  this->chasingMissileUpdate(dt);
  this->enemyUpdate(dt);
  this->missileUpdate(dt);
  this->coinUpdate(dt);
  this->npcUpdate(dt);
  this->missileEffectUpdate(dt);
  this->enemyFireLoop(dt);
  this->destructableUpdate();
  gravityUpdateForStraight(dt);
  gravityUpdateForCustomMoving(dt);
  gravityUpdateForFlyingOrSwimingEnemies(dt);
  if (difficultyMode == DIFFICULTY_MODE_HELL) {
    hardModeUpdate(dt);
  }
}
void GameScene::updatePvp(float dt) {
  if (heroList.size() < gameMode && heroListToCreate.size() > 0) {
    EnemyBase *hero = heroListToCreate.at(0);
    heroListToCreate.eraseObject(hero);
    unitsToCreateArray.pushBack(hero);
    heroList.pushBack(hero);
    hero->setVisible(true);
    hero->spine->setVisible(true);
    runEffect(EFFECT_EXPLODE_MIDDLE, hero->getPosition());
  }
  if (enemyHeroList.size() < gameMode && enemyHeroListToCreate.size() > 0) {
    EnemyBase *hero = enemyHeroListToCreate.at(0);
    enemyHeroListToCreate.eraseObject(hero);
    unitsToCreateArray.pushBack(hero);
    enemyHeroList.pushBack(hero);
    hero->setVisible(true);
    hero->spine->setVisible(true);
    runEffect(EFFECT_EXPLODE_MIDDLE, hero->getPosition());
  }

  int heroHp = 0;
  for (auto unit : heroList) {
    heroHp += unit->energy;
  }
  for (auto unit : heroListToCreate) {
    heroHp += unit->energy;
  }
  int enemyHp = 0;
  for (auto unit : enemyHeroList) {
    enemyHp += unit->energy;
  }
  for (auto unit : enemyHeroListToCreate) {
    enemyHp += unit->energy;
  }

  HUD->updatePvpUI(heroHp, enemyHp);

  if (HUD->pvpTimeLeftToStart > 0) {
    HUD->pvpTimeLeftToStart -= dt;
    if (HUD->pvpTimeLeftToStart < 1) {
      if (HUD->lblPvpCount->getString().compare("1") == 0) {
        HUD->lblPvpCount->setString("Battle!");
        HUD->lblPvpCount->setTextColor(Color4B(248, 59, 7, 255));
        GM->shakeIt(HUD->lblPvpCount);
        //                GM->shakeIt(HUD->lblPvpCount, 10, 5);
      }
    } else {
      std::string strNumber =
          Value((int)floor(HUD->pvpTimeLeftToStart)).asString();
      if (HUD->lblPvpCount->getString().compare(strNumber) != 0) {
        HUD->lblPvpCount->setString(strNumber);
      }
    }
  } else if (HUD->lblPvpCount->isVisible()) {
    HUD->lblPvpCount->setVisible(false);
  }
}
void GameScene::setPvpMode(int mode) {
  gameMode = mode;
  // hero start pos left top 17, 19
  //    std::string strEquipped = UDGetStr(KEY_UNITS_HERO_DECK, "");
  //    ValueVector units = GM->split(strEquipped, "_");
  UnitInfo *info;
  std::vector<UnitInfo *> list = GM->getHeroDeck();
  int index = 0;
  int playerHpMax = 0;
  for (int i = 0; i < list.size(); i++) {
    //        std::string str = units.at(i).asString();
    //        if(str.length() <= 0){
    //            continue;
    //        }
    info = list.at(i); // GM->getUnitInfoFromString(units.at(i).asString());
    Vec2 heroPos =
        theMap->getPosition() +
        Vec2(17 * TILE_SIZE + TILE_SIZE / 2 - (index / 6) * 75,
             (theMap->getMapSize().height - 19 - 1 - index % 6) * TILE_SIZE) +
        Vec2(0, TILE_SIZE / 2);
    //        if (gameMode == GAME_MODE_PVP6 && index > 5) {
    //            heroPos = theMap->getPosition() + Vec2((8 + index/6)*TILE_SIZE
    //            + TILE_SIZE/2,
    //            (theMap->getMapSize().height-30-1-index%6)*TILE_SIZE) +
    //            Vec2(0, TILE_SIZE/2);
    //        }
    if (info != nullptr) { // && info){
      int unitType = info->unitType;
      int unitLevel = info->level;
      if (unitType >= 0) {
        //                if(index == 0) unitType = UNIT_HERO_WATERMELON; //
        //                test
        EnemyBase *createdHero =
            createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, heroPos,
                       GM->getUnitName(unitType));
        setHeroLevelInfo(createdHero, unitLevel);
        playerHpMax += createdHero->maxEnergy;
        createdHero->setFlippedX(true);
        createdHero->setTag(i);
        if (heroList.size() < gameMode) {
          heroList.pushBack(createdHero);
        } else {
          heroListToCreate.pushBack(createdHero);
          createdHero->setVisible(false);
          createdHero->spine->setVisible(false);
          unitsToCreateArray.eraseObject(createdHero);
        }
      }
    }
    index++;
  }

  // enemy start pos right bottom 22, 24

  std::string strEquipped = BSM->pvpTargetData;
  ValueVector units = GM->split(strEquipped, "_");
  index = 0;
  int enemyHpMax = 0;
  for (int i = 0; i < units.size(); i++) {
    std::string str = units.at(i).asString();
    if (str.length() <= 0) {
      continue;
    }
    info = GM->getUnitInfoFromString(units.at(i).asString());
    Vec2 heroPos =
        theMap->getPosition() +
        Vec2(22 * TILE_SIZE + TILE_SIZE / 2 + (index / 6) * 75,
             (theMap->getMapSize().height - 19 - 1 - index % 6) * TILE_SIZE) +
        Vec2(0, TILE_SIZE / 2);
    //        if (gameMode == GAME_MODE_PVP6 && index > 5) {
    //            heroPos = theMap->getPosition() + Vec2((32 +
    //            index/6)*TILE_SIZE + TILE_SIZE/2,
    //            (theMap->getMapSize().height-30-1-index%6)*TILE_SIZE) +
    //            Vec2(0, TILE_SIZE/2);
    //        }
    if (info != nullptr && info) {
      int unitType = info->unitType;
      int unitLevel = info->level;
      if (unitType >= 0) {
        //                if(index == 0) unitType = UNIT_HERO_HEALER; // test
        EnemyBase *createdHero =
            createUnit(unitType, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, heroPos,
                       GM->getUnitName(unitType));
        setHeroLevelInfo(createdHero, unitLevel);
        enemyHpMax += createdHero->maxEnergy;
        createdHero->setTag(i);
        if (enemyHeroList.size() < gameMode) {
          enemyHeroList.pushBack(createdHero);
        } else {
          enemyHeroListToCreate.pushBack(createdHero);
          createdHero->setVisible(false);
          createdHero->spine->setVisible(false);
          unitsToCreateArray.eraseObject(createdHero);
        }
      }
    }
    index++;
  }
  drawMiniMapFrame->setVisible(false);
  drawMiniMapForMoving->setVisible(false);
  drawMiniMapForNonMoving->setVisible(false);
  if (drawMiniMapTerrain) drawMiniMapTerrain->setVisible(false);
  HUD->getChildByName("miniMapBack")->setVisible(false);
  HUD->getChildByName("btnMenu")->setVisible(false);
  HUD->getChildByName("imgTimeBack")->setVisible(false);
  HUD->lblTimer->setVisible(false);
  HUD->rightBottomPanelForCampaign->setVisible(false);
  HUD->getChildByName("imgResourceBack")->setVisible(false);
  HUD->getChildByName("iconGold")->setVisible(false);
  HUD->getChildByName("iconLumber")->setVisible(false);
  HUD->getChildByName("iconFood")->setVisible(false);
  HUD->lblGold->setVisible(false);
  HUD->lblLumber->setVisible(false);
  HUD->lblFood->setVisible(false);

  HUD->setPvpUI(playerHpMax, enemyHpMax);
  this->runAction(MoveBy::create(2, Vec2(275, 350) * getScaleX()));
}
void GameScene::hardModeUpdate(float dt) {
  if (isHardModeEnded) {
    return;
  }
  hardModeTimer += dt;
  //    hardModeTimer += dt*10; // test
  bool isReady = false;
  if (cloneCounter < 2) {
    isReady = hardModeTimer > 30;
  } else if (cloneCounter < 5) {
    isReady = hardModeTimer > 60;
  } else if (cloneCounter < 10) {
    isReady = hardModeTimer > 90;
  } else if (cloneCounter < 20) {
    isReady = hardModeTimer > 180;
  } else {
    isReady = hardModeTimer > 300;
  }
  if (isReady) {
    hardModeTimer = 0;
    cloneCounter++;
    if (cloneCounter > 25) {
      cloneCounter = 1;
    }
    EnemyBase *theCastle;
    bool isCastleExist = false;
    for (auto unit : heroArray) {
      if (unit->unitType == UNIT_CASTLE) {
        isCastleExist = true;
        theCastle = unit;
      }
    }

    if (isCastleExist) {
      int count = 0;
      for (auto unit : enemyArray) {
        if (!unit->isBuilding) {
          if (count < cloneCounter) {
            count++;
            EnemyBase *clone = createUnit(unit->unitType, unit->alliSide,
                                          unit->isBuilding, unit->getPosition(),
                                          GM->getUnitName(unit->unitType));
            clone->wantToEli = true;
          }
        }
      }
    } else {
      for (auto unit : enemyArray) {
        if (unit->getScale() == 1) {
          unit->setScale(1.5f);
          unit->maxEnergy *= 1.5f;
          unit->energy = unit->maxEnergy;
        }
      }
      isHardModeEnded = true;
    }
  }
}
void GameScene::checkGameSchedule() {
  if (GM->isColosseum) {
    return;
  }
  if (stageIndex == 3) {
    if (lastGameScheduleIndex == -1 && gameTimer > 60 * 4) {
      //        if (lastGameScheduleIndex == -1 && gameTimer >= 30) { // test
      for (int i = 12; i < 19; i++) {
        decoLayer->setTileGID(0, Vec2(28, i));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 0 && gameTimer > 60 * 7) {
      for (int i = 13; i < 21; i++) {
        decoLayer->setTileGID(0, Vec2(i, 31));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 1 && gameTimer > 60 * 10) {
      for (int i = 12; i < 19; i++) {
        decoLayer->setTileGID(0, Vec2(41, i));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 2 && gameTimer > 60 * 13) {
      for (int i = 13; i < 21; i++) {
        decoLayer->setTileGID(0, Vec2(i, 39));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 3 && gameTimer > 60 * 16) {
      for (int i = 57; i < 61; i++) {
        decoLayer->setTileGID(0, Vec2(30, i));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    }
  }
  if (stageIndex == 4) {
    if (lastGameScheduleIndex == -1 && gameTimer > 60 * 12) {
      for (int i = 8; i < 14; i++) {
        decoLayer->setTileGID(0, Vec2(i, 17));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 0 && gameTimer > 60 * 13) {
      for (int i = 57; i < 61; i++) {
        decoLayer->setTileGID(0, Vec2(i, 16));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    }
  }

  if (stageIndex == 5) {
    if (lastGameScheduleIndex == -1 && gameTimer > 60 * 11) {
      for (int i = 11; i < 17; i++) {
        decoLayer->setTileGID(0, Vec2(i, 9));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 0 && gameTimer > 60 * 8) {
      for (int i = 8; i < 12; i++) {
        decoLayer->setTileGID(0, Vec2(40, i));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    } else if (lastGameScheduleIndex == 1 && gameTimer > 60 * 8) {
      for (int i = 33; i < 39; i++) {
        decoLayer->setTileGID(0, Vec2(i, 32));
      }
      resetPathState();
      makeZombiesAttack();
      lastGameScheduleIndex++;
    }
  }
}
void GameScene::oneSecUpdate(float dt) {
  oneSecTiemr -= dt;
  if (oneSecTiemr > 0)
    return;
  oneSecTiemr = 1;
  checkVisibilityForInScreen();

  if (GM->loadMapData.length() > 0) {
    loadMapData();
  }

  updateEnemyAI();
  if (!isMultiplay) updateShuttleFerries();
}
void GameScene::halfSecUpdate(float dt) {
  fogUpdateTimer -= dt;
  if (fogUpdateTimer < 0) {
    fogUpdateTimer = 0.5f;
    updateFog();
  }
  halfSecTimer -= dt;
  if (halfSecTimer < 0) {
    halfSecTimer = 0.5f;
    updateMiniMapForMoving();
  }
}
void GameScene::updateFog() {
  //    return; // test
  if (GM->currentStageIndex == STAGE_SINGLEPLAY ||
      GM->currentStageIndex == STAGE_RAID) {
    blackSheepWell = true;
    isRevealingMap = true;
  }
  for (auto fog : fogArray) {
    fog->newState = FOG_SEEN_NOT;
  }
  updateMiniMapForNonMoving();
  Vec2 fogCoordinate;
  Fog *fogAboveUnit;
  for (auto unit : heroArray) {
    if (unit->isBuilding) {
      for (int i = 0; i < 4; i++) {
        if (i == 0) {
          //                    fogCoordinate =
          //                    getPositionFromTileCoordinate(unit->buildingStartCoordinate.x,
          //                    unit->buildingStartCoordinate.y)/FOG_SIZE;
          fogCoordinate = unit->approachingPoints[0] / FOG_SIZE;
        } else if (i == 1) {
          //                    fogCoordinate =
          //                    getPositionFromTileCoordinate(unit->buildingStartCoordinate.x
          //                    + unit->buildingOccupySize.width-1,
          //                    unit->buildingStartCoordinate.y)/FOG_SIZE;
          fogCoordinate = unit->approachingPoints[1] / FOG_SIZE;
        } else if (i == 2) {
          //                    fogCoordinate =
          //                    getPositionFromTileCoordinate(unit->buildingStartCoordinate.x,
          //                    unit->buildingStartCoordinate.y+unit->buildingOccupySize.height-1)/FOG_SIZE;
          fogCoordinate = unit->approachingPoints[2] / FOG_SIZE;
        } else if (i == 3) {
          //                    fogCoordinate =
          //                    getPositionFromTileCoordinate(unit->buildingStartCoordinate.x
          //                    + unit->buildingOccupySize.width-1,
          //                    unit->buildingStartCoordinate.y+unit->buildingOccupySize.height-1)/FOG_SIZE;
          fogCoordinate = unit->approachingPoints[3] / FOG_SIZE;
        }

        fogAboveUnit =
            fogArray.at((int)fogCoordinate.x +
                        (int)fogCoordinate.y * (int)fogMapSize.width);
        fogAboveUnit->newState = FOG_SEEN_NOW;
        for (auto fog : fogAboveUnit->adjacentFogs) {
          fog->newState = FOG_SEEN_NOW;
        }
        for (auto fog : fogAboveUnit->farFogs) {
          if (fog->appliedState != FOG_SEEN_NOT_NOW &&
              fog->newState != FOG_SEEN_NOW) {
            fog->newState = FOG_SEEN_LITTLE;
          }
        }
      }
    } else {
      fogCoordinate = Vec2(unit->getPositionX() / FOG_SIZE,
                           unit->getPositionY() / FOG_SIZE);
      int fogIndex =
          (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
      if (fogIndex >= fogArray.size() || fogIndex < 0) {
        continue;
      }
      fogAboveUnit = fogArray.at(fogIndex);
      fogAboveUnit->newState = FOG_SEEN_NOW;
      for (auto fog : fogAboveUnit->adjacentFogs) {
        fog->newState = FOG_SEEN_NOW;
      }
      bool hasWideVision = (unit->unitType == UNIT_HELICOPTER ||
                            unit->unitType == UNIT_HUMAN_SHUTTLE ||
                            unit->unitType == UNIT_ORC_SHUTTLE ||
                            unit->unitType == UNIT_HUMAN_SHIP ||
                            unit->unitType == UNIT_ORC_SHIP ||
                            unit->unitType == UNIT_HUMAN_BATTLE_SHIP ||
                            unit->unitType == UNIT_HUMAN_OIL_SHIP ||
                            unit->unitType == UNIT_ORC_OIL_SHIP);
      if (hasWideVision) {
        for (auto fog : fogAboveUnit->farFogs) {
          fog->newState = FOG_SEEN_NOW;
        }
        for (auto adj : fogAboveUnit->adjacentFogs) {
          for (auto fog : adj->farFogs) {
            if (fog->newState != FOG_SEEN_NOW) {
              fog->newState = FOG_SEEN_LITTLE;
            }
          }
        }
      } else {
        for (auto fog : fogAboveUnit->farFogs) {
          if (fog->newState != FOG_SEEN_NOW) {
            fog->newState = FOG_SEEN_LITTLE;
          }
        }
      }
    }
  }
  for (auto unit : readyHeroArray) {
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size() || fogIndex < 0) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);
    fogAboveUnit->newState = FOG_SEEN_NOW;
    for (auto fog : fogAboveUnit->adjacentFogs) {
      fog->newState = FOG_SEEN_NOW;
    }
    for (auto fog : fogAboveUnit->farFogs) {
      //                if(fog->appliedState != FOG_SEEN_NOT_NOW &&
      //                fog->newState != FOG_SEEN_NOW){
      if (fog->newState != FOG_SEEN_NOW) {
        fog->newState = FOG_SEEN_LITTLE;
      }
    }
  }
  if (blackSheepWell) { // test
    for (auto fog : fogArray) {
      fog->newState = FOG_SEEN_NOW;
      //            fog->appliedState = FOG_SEEN_NOW;
    }
  }

  // Apply trigger-revealed fog regions: permanently clear regardless of units.
  for (const auto& region : triggerRevealedFogRegions) {
    int cx = static_cast<int>(region.centerWorldPos.x / FOG_SIZE);
    int cy = static_cast<int>(region.centerWorldPos.y / FOG_SIZE);
    int r = region.radius;
    for (int dy = -r; dy <= r; dy++) {
      for (int dx = -r; dx <= r; dx++) {
        int fx = cx + dx;
        int fy = cy + dy;
        if (fx < 0 || fy < 0 || fx >= static_cast<int>(fogMapSize.width) || fy >= static_cast<int>(fogMapSize.height))
          continue;
        int idx = fx + fy * static_cast<int>(fogMapSize.width);
        if (idx < 0 || idx >= static_cast<int>(fogArray.size()))
          continue;
        fogArray.at(idx)->newState = FOG_SEEN_NOW;
      }
    }
  }

  if (isRevealingMap) {
    for (auto fog : fogArray) {
      fog->setVisible(false);
      fog->appliedState = FOG_SEEN_NOW;
    }
  } else {
    processNewFogState();
  }

  for (auto unit : enemyArray) {
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size() || fogIndex < 0) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);
    if (unit->isBuilding) {
      unit->isUnderFog = fogAboveUnit->appliedState == FOG_SEEN_NOT;
    } else {
      unit->isUnderFog = fogAboveUnit->appliedState <= FOG_SEEN_NOT_NOW;
    }

    unit->checkVisible();
    if (unit->energyBar != nullptr) {
      unit->energyBar->setVisible(unit->isVisible());
      unit->energyBarContent->setVisible(unit->isVisible());
      unit->energyBarBack->setVisible(unit->isVisible());
    }
    if (!unit->isDetected && unit->isBuilding && unit->isVisible()) {
      unit->isDetected = true;
      updateMiniMapForNonMoving();
    }
  }

  for (auto unit : mutualArray) {
    fogCoordinate =
        Vec2(unit->getPositionX() / FOG_SIZE, unit->getPositionY() / FOG_SIZE);
    int fogIndex =
        (int)fogCoordinate.x + (int)fogCoordinate.y * (int)fogMapSize.width;
    if (fogIndex >= fogArray.size()) {
      continue;
    }
    fogAboveUnit = fogArray.at(fogIndex);

    unit->isUnderFog = fogAboveUnit->appliedState == FOG_SEEN_NOT;
    if (unit->energyBar != nullptr) {
      unit->energyBar->setVisible(unit->isVisible());
      unit->energyBarContent->setVisible(unit->isVisible());
      unit->energyBarBack->setVisible(unit->isVisible());
    }
    if (!unit->isDetected && unit->isVisible()) {
      unit->isDetected = true;
      updateMiniMapForNonMoving();
    }
    unit->checkVisible();
  }
}
void GameScene::processNewFogState() {
  for (auto fog : fogArray) {
    if (fog->newState == fog->appliedState) {
      continue;
    }
    if (fog->newState == FOG_SEEN_NOW) {
      fog->setVisible(false);
      fog->appliedState = FOG_SEEN_NOW;
    } else if (fog->newState == FOG_SEEN_LITTLE) {
      fog->setVisible(true);
      fog->setOpacity(100);
      fog->appliedState = FOG_SEEN_LITTLE;
    } else if (fog->appliedState > FOG_SEEN_NOT_NOW &&
               fog->newState == FOG_SEEN_NOT) {
      fog->setVisible(true);
      fog->setOpacity(150);
      fog->appliedState = FOG_SEEN_NOT_NOW;
    }
  }
}
void GameScene::coinMagnet() {}
Sprite *GameScene::getTeleport(int teleport) {
  for (auto tele : teleportArray) {
    if (tele->getTag() == teleport)
      return tele;
  }
  return NULL;
}
void GameScene::gravityUpdateForFlyingOrSwimingEnemies(float dt) {
  if (isPaused) {
    return;
  }
  if (dt > 0.05) {
    dt = 0.05;
  }

  for (auto drop : flyingSwimingEnemyArray) {
    if (drop->currentMoveType == ENEMY_MOVE_FLY_TO_HERO_FOLLOW) {
      bool flip = drop->getPosition().x > drop->target->getPosition().x;
      if (flip != drop->isFlippedX()) {
        drop->setFlippedX(flip);
      }
      drop->updatePositionToFlyToFollowTarget(dt);
      checkForAndResolveCollisions(drop);
    } else if (drop->currentMoveType == ENEMY_MOVE_SWIM_TO_HERO_FOLLOW) {
      drop->updatePositionToFlyToFollowTarget(dt);
      TMXTiledMap *map = getTileMap(drop->getPosition());
      int tileId =
          this->getTileAtPosition(drop->desiredPosition, TAG_FORE_LAYER, map);
      if (tileId >= 0) { // check if out of water
        drop->desiredPosition = drop->getPosition();
      }
      checkForAndResolveCollisions(drop);

      //            checkForAndResolveCollisionsForWater(drop);
    }
  }
}

void GameScene::gravityUpdateForStraight(float dt) {
  if (isPaused) {
    return;
  }
  for (auto drop : straightMovingArray) {
    drop->updatePositionForStraight(dt);
  }
  bool allClear = false;
  do {
    allClear = true;
    for (auto drop : straightMovingArray) {
      if (checkForAndResolveCollisionsForMissile(drop)) {
        allClear = false;
        break;
      }
    }
  } while (!allClear);
}
void GameScene::gravityUpdateForCustomMoving(float dt) {
  if (isPaused) {
    return;
  }
  for (auto drop : customMovingArray) {
    drop->desiredPosition = drop->getPosition();
  }
  bool allClear = false;
  do {
    allClear = true;
    for (auto drop : customMovingArray) {
      if (checkForAndResolveCollisionsForMissile(drop)) {
        allClear = false;
        break;
      }
    }
  } while (!allClear);
}
void GameScene::chasingMissileUpdate(float dt) {
  for (auto drop : chasingMissileArray) {
    if (drop->getTag() == UNIT_MISSILE_CHASING && drop->target != NULL) {
      float xGap = drop->getPosition().x - drop->target->getPosition().x;
      float yGap = drop->getPosition().y - drop->target->getPosition().y;

      double th = atan2(yGap, xGap);
      double angle = th * 180 / 3.14 + 180;
      float gap = angle - drop->movingAngle;
      drop->movingAngle +=
          gap > 0 ? (gap > 180 ? -2 : 2) : (gap < -180 ? 2 : -2);
      //            drop->setRotation(-angle);
      drop->setRotation(-drop->movingAngle);
      if (drop->movingAngle < 0)
        drop->movingAngle += 360;
      if (drop->movingAngle > 360) {
        drop->movingAngle -= 360;
      }
      drop->setFlippedY(drop->movingAngle > 90 && drop->movingAngle < 270);
    }
    drop->updatePositionForStraight(dt);

    checkForAndResolveCollisionsForMissile(drop);
  }
}
void GameScene::setViewPointCenter(Vec2 position, bool forceMove) {
  float scale = getScale();
  int x = position.x / scale;
  int y = position.y / scale;

  Vec2 actualPosition = Vec2(x, y);

  Vec2 centerOfView = Vec2(size.width / 2, size.height * 3 / 4 - 30);
  Vec2 viewPoint = centerOfView - actualPosition;
  float distance = 0;
  float maxDistance = 300;
  if (distance > maxDistance) {
    distance = maxDistance;
  }
  float angle = GameManager::getInstance()->getAngle(center, center);
  Vec2 aimingPos = Vec2(cos(angle * 3.14 / 180) * distance,
                        -sin(-angle * 3.14 / 180) * distance);
  if (forceMove) {
    this->setPosition(viewPoint - aimingPos * 0.7f);
  } else {
    this->setPosition(
        (viewPoint - aimingPos * 0.7f).lerp(this->getPosition(), 0.9f));
  }
}

void GameScene::showBigMiniMap(bool isBig) {
  //    ScrollView* scrollView =
  //    (ScrollView*)dnMiniMap->getParent()->getParent(); if (isBig) {
  //        float height = size.height*3/4;
  //        float width = miniMapSize.width*height/miniMapSize.height;
  //        currentMapSize = cocos2d::Size(width, height);
  //        scrollView->setContentSize(cocos2d::Size(width, height));
  //        log("mini map pos: %f, %f", scrollView->getPositionX(),
  //        scrollView->getPositionY());
  //        scrollView->setPosition(Vec2(size.width/2 + width/2, size.height/2 +
  //        height/2)); scrollView->setBackGroundColorOpacity(200);
  //    }else{
  //        currentMapSize = miniMapSize;
  //        scrollView->setContentSize(miniMapSize);
  //        log("mini map pos: %f, %f", scrollView->getPositionX(),
  //        scrollView->getPositionY()); scrollView->setPosition(Vec2(size.width
  //        - 10, size.height - 10));
  //        scrollView->setBackGroundColorOpacity(100);
  //    }
  //    Node* dnHero = scrollView->getInnerContainer()->getChildByName("Hero");
  //    if (dnHero != nullptr) {
  //        dnHero->setPosition(scrollView->getContentSize()/2);
  //        dnHero->setPosition(dnHero->getPosition() +
  //        Vec2(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
  //    }
  //
  //    scrollView->setInnerContainerSize(scrollView->getContentSize());
}
void GameScene::setCurrentTileMap(Vec2 pos) {
  for (auto map : mapArray) {
    if (map->getBoundingBox().containsPoint(pos)) {
      //            if (currentTileMap != map) {
      //                currentTileMap = map;
      //                stageLayer = currentTileMap->getLayer("stage");
      //                foreLayer = currentTileMap->getLayer("fore");
      //            }
      break;
    }
  }
}
TMXTiledMap *GameScene::getTileMap(Vec2 pos) {
  return theMap;
  for (auto map : mapArray) {
    if (map->getBoundingBox().containsPoint(pos)) {
      return map;
    }
  }
  return nullptr;
}
cocos2d::Vec2 GameScene::getCoordinateFromPosition(cocos2d::Vec2 position) {
  return getCoordinateFromPosition(position, theMap);
}
Vec2 GameScene::getCoordinateFromPosition(Vec2 position, TMXTiledMap *map) {
  int mapRowCount = map->getMapSize().height;
  int mapColumnCount = map->getMapSize().width;
  float x = floor(position.x / TILE_SIZE);
  if (x < 0)
    x = 0;
  if (x >= mapColumnCount) {
    x = mapColumnCount - 1;
  }
  float y =
      floor(((map->getMapSize().height * TILE_SIZE) - position.y) / TILE_SIZE);
  if (y < 0)
    y = 0;
  if (y >= mapRowCount)
    y = mapRowCount - 1;
  return Vec2(x, y);
}

cocos2d::Rect GameScene::tileRectFromTileCoords(Vec2 tileCoords,
                                                 TMXTiledMap *map) {
  return cocos2d::Rect(tileCoords.x * TILE_SIZE + map->getPositionX(),
                       (map->getMapSize().height - tileCoords.y - 1) *
                               TILE_SIZE +
                           map->getPositionY(),
                       TILE_SIZE, TILE_SIZE);
  //    float levelHeightInPixels = tileMap->getMapSize().height *
  //    tileMap->getTileSize().height; Vec2 origin = Vec2(tileCoords.x *
  //    tileMap->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1)
  //    * tileMap->getTileSize().height)); return cocos2d::Rect(origin.x,
  //    origin.y, tileMap->getTileSize().width, tileMap->getTileSize().height);
}

int GameScene::getTileAtPosition(Vec2 position, int tag, TMXTiledMap *map) {
  float mapX = map->getPositionX();
  float mapY = map->getPositionY();
  if (position.x < mapX || position.y < mapY ||
      mapX + map->getMapSize().width * TILE_SIZE < position.x ||
      mapY + map->getMapSize().height * TILE_SIZE < position.y) {
    return 5;
  }
  Vec2 plPos =
      this->getCoordinateFromPosition(position - map->getPosition(), map); // 1
  // TMXLayer* layer = map->getLayer("stage");
  // Node* parent = layer->getParent();
  return getTileGIDAt((TMXLayer *)map->getChildByTag(tag), plPos); // 4
}

void GameScene::menuCloseCallback(Ref *pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS ||                                  \
     CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  exit(0);
#endif
}

void GameScene::registerScriptAccelerateHandler(int handler) {
  //    Director::getInstance()->getEventDispatcher()->addTargetedDelegate(this,
  //    0, true);
}

void GameScene::setPlayerPosition(Vec2 position) {}
void GameScene::Dispose() {}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
bool GameScene::usesPcRtsControls() { return true; }

bool GameScene::isShiftHeldForSelection() {
  if (!usesPcRtsControls()) {
    return false;
  }
  return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}

bool GameScene::isCtrlHeldForSelection() {
  if (!usesPcRtsControls()) {
    return false;
  }
  return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
}

void GameScene::updateWin32EdgeScroll(const Vec2 &viewPos) {
  const float edge = 20.0f;
  isMapMoveLeftByEdge = viewPos.x < edge;
  isMapMoveRightByEdge = viewPos.x > size.width - edge;
  isMapMoveDownByEdge = viewPos.y < edge;
  isMapMoveUpByEdge = viewPos.y > size.height - edge;
}

Vec2 GameScene::getWin32MouseWorldPosFromView(const Vec2 &viewPos) {
  Size visibleSize = Director::getInstance()->getVisibleSize();
  return Director::getInstance()->convertToGL(
      Vec2(viewPos.x, visibleSize.height - viewPos.y));
}

Vec2 GameScene::getWin32MouseWorldPos(EventMouse *evt) {
  return getWin32MouseWorldPosFromView(evt->getLocationInView());
}

bool GameScene::isClickOnWorkerMenu(const Vec2 &glPos) {
  return getWorkerMenuButtonAtGlPos(glPos) != nullptr;
}

Button *GameScene::getWorkerMenuButtonAtGlPos(const Vec2 &glPos) {
  if (GM->currentStageIndex == STAGE_LOBBY) {
    return nullptr;
  }
  const float padding = 24.0f * getWorkerMenuScale();
  for (int i = 0; i < 6; i++) {
    Node *btnMenu = getChildByName(strmake("btnMenu%d", i));
    if (btnMenu == nullptr || !btnMenu->isVisible()) {
      continue;
    }
    Button *btn = (Button *)btnMenu->getChildByName("btn");
    if (btn == nullptr || btn->getTag() == BTN_TYPE_NONE) {
      continue;
    }
    Vec2 localInMenu = btnMenu->convertToNodeSpace(glPos);
    Rect menuBox = btnMenu->getBoundingBox();
    menuBox.origin.x -= padding;
    menuBox.origin.y -= padding;
    menuBox.size.width += padding * 2;
    menuBox.size.height += padding * 2;

    Rect btnBox = btn->getBoundingBox();
    btnBox.origin.x -= padding;
    btnBox.origin.y -= padding;
    btnBox.size.width += padding * 2;
    btnBox.size.height += padding * 2;

    if (menuBox.containsPoint(localInMenu) ||
        btnBox.containsPoint(localInMenu)) {
      return btn;
    }
  }
  return nullptr;
}

void GameScene::handleWorkerMenuClick(const Vec2 &glPos) {
  if (HUD == nullptr) {
    return;
  }
  Button *btn = getWorkerMenuButtonAtGlPos(glPos);
  if (btn != nullptr) {
    HUD->onCommandClick(btn);
  }
}

void GameScene::setWin32AttackMoveMode(bool enabled) {
  win32AttackMoveMode = enabled;
  printf("cursor setWin32AttackMoveMode: %d\n", enabled);
  if (usesPcRtsControls()) {
    printf("cursor setWin32AttackMoveMode 2 %d", enabled);
    if (enabled) {
      Win32MouseCursor::setAttackMoveCursor();
    } else {
      Win32MouseCursor::setDefaultCursor();
    }
  }
  doubleClickTimerForAttackDdaing = 0;
}

void GameScene::cancelWin32AttackMoveMode() { setWin32AttackMoveMode(false); }

void GameScene::handleWin32RightClick(Vec2 screenPos) {
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12 ||
      buildingTemplate != nullptr) {
    return;
  }
  if (win32AttackMoveMode) {
    cancelWin32AttackMoveMode();
    return;
  }
  if (selectedArray.size() == 0) {
    return;
  }

  if (drawMiniMapForMoving != nullptr) {
    cocos2d::Rect miniMapRect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos,
                              cocos2d::Size(miniMapWidth, miniMapHeight));
    if (miniMapRect.containsPoint(screenPos)) {
      Vec2 mapPos = (screenPos - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos)) / miniMapScale;
      moveTo(selectedArray, mapPos);
      showTargetHand(mapPos, false);
      return;
    }
  }

  Vec2 mapPos = (screenPos - getPosition()) / layerScale;

  if (tryDeliverCarriedResourcesAt(mapPos)) {
    return;
  }

  EnemyBase *targetEnemy = nullptr;
  for (auto unit : enemyArray) {
    if (unit == nullptr) {
      continue;
    }
    if (unit->getBoundingBox().containsPoint(mapPos)) {
      targetEnemy = unit;
      break;
    }
  }

  bool hasCombatUnits = false;
  for (auto unit : selectedArray) {
    if (unit == nullptr || unit->isBuilding || unit->isAlli) {
      continue;
    }
    if (!unit->isEnemy) {
      hasCombatUnits = true;
      break;
    }
  }

  if (targetEnemy != nullptr && targetEnemy->isEnemy && hasCombatUnits) {
    if (isMultiplay) {
      std::string strIDs = "";
      for (auto unit : selectedArray) {
        strIDs += Value(unit->unitID).asString() + "|";
      }
      MM->attackTarget(strIDs, targetEnemy->unitID);
      HMsg msg;
      msg.msgType = MSG_CODE_ATTACK_TARGET;
      msg.sourceIDs = strIDs;
      msg.targetID = targetEnemy->unitID;
      msg.startTime = gameFrameTimer + LATANCY;
      ownMsgList.push_back(msg);
    } else {
      forceAttack(selectedArray, targetEnemy);
    }
    showTargetHand(targetEnemy->getPosition(), true);
    return;
  }

  EnemyBase *resourceTarget = nullptr;
  for (auto unit : mutualArray) {
    if (unit == nullptr) {
      continue;
    }
    if (unit->getBoundingBox().containsPoint(mapPos)) {
      resourceTarget = unit;
      break;
    }
  }
  if (resourceTarget != nullptr &&
      (resourceTarget->unitType == UNIT_TREE ||
       resourceTarget->unitType == UNIT_MINE) &&
      gatherResourceWithSelectedWorkers(resourceTarget)) {
    return;
  }

  // OilShip right-clicks an OilExtractor in heroArray
  EnemyBase *oilExtractorTarget = nullptr;
  for (auto unit : heroArray) {
    if (unit == nullptr || (unit->unitType != UNIT_HUMAN_OIL_EXTRACTOR &&
                            unit->unitType != UNIT_ORC_OIL_EXTRACTOR))
      continue;
    if (unit->getBoundingBox().containsPoint(mapPos)) {
      oilExtractorTarget = unit;
      break;
    }
  }
  if (oilExtractorTarget != nullptr && gatherOilWithSelectedOilShip(oilExtractorTarget)) {
    showTargetHand(oilExtractorTarget->getPosition(), false);
    return;
  }

  // Ground units selected + right-click on a shuttle → board or move to shore
  {
    EnemyBase *clickedShuttle = nullptr;
    for (auto unit : heroArray) {
      if (!unit || !unit->isVisible()) continue;
      if ((unit->unitType == UNIT_HUMAN_SHUTTLE || unit->unitType == UNIT_ORC_SHUTTLE) &&
          unit->getBoundingBox().containsPoint(mapPos)) {
        clickedShuttle = unit;
        break;
      }
    }
    if (clickedShuttle) {
      Vector<EnemyBase *> boarders;
      for (auto unit : selectedArray) {
        if (unit && canUnitBoard(unit) && !unit->isInShuttle &&
            unit->alliSide == clickedShuttle->alliSide)
          boarders.pushBack(unit);
      }
      if (!boarders.empty()) {
        int tileMapH = (int)theMap->getMapSize().height;
        Vec2 shuttleTile = getCoordinateFromPosition(clickedShuttle->getPosition());
        int stx = (int)shuttleTile.x, sty = (int)shuttleTile.y;
        int adj4x[] = {1, -1, 0, 0};
        int adj4y[] = {0, 0, 1, -1};
        if (isShuttleAdjacentToLand(clickedShuttle)) {
          // Find the adjacent land tile to use as approach position
          Vec2 boardPos = clickedShuttle->getPosition();
          for (int i = 0; i < 4; i++) {
            int nx = stx + adj4x[i], ny = sty + adj4y[i];
            if (!isWaterTileAt(nx, ny)) {
              boardPos = Vec2((nx + 0.5f) * TILE_SIZE, (tileMapH - ny - 0.5f) * TILE_SIZE);
              break;
            }
          }
          for (auto unit : boarders) {
            float dist = unit->getPosition().distance(clickedShuttle->getPosition());
            if (dist <= TILE_SIZE * 3) {
              if (loadUnitToShuttle(clickedShuttle, unit))
                refreshShuttleCargoPanel();
            } else {
              unit->pendingShuttle = clickedShuttle;
              Vector<EnemyBase *> list;
              list.pushBack(unit);
              moveTo(list, boardPos);
            }
          }
        } else {
          // Shuttle not adjacent to land: find nearest shoreline tile to the shuttle
          int mapW = (int)mapSize.width;
          int mapHt = (int)mapSize.height;
          Vec2 nearestLandPos = Vec2::ZERO;
          float nearestDistSq = FLT_MAX;
          Vec2 shuttleWorldPos = clickedShuttle->getPosition();
          for (int r = 1; r <= 40 && nearestLandPos == Vec2::ZERO; r++) {
            for (int ox = -r; ox <= r; ox++) {
              for (int oy = -r; oy <= r; oy++) {
                if (ox > -r && ox < r && oy > -r && oy < r) continue; // skip interior
                int nx = stx + ox, ny = sty + oy;
                if (nx < 0 || ny < 0 || nx >= mapW || ny >= mapHt) continue;
                if (!isWaterTileAt(nx, ny)) {
                  Vec2 p = Vec2((nx + 0.5f) * TILE_SIZE, (tileMapH - ny - 0.5f) * TILE_SIZE);
                  float dsq = p.distanceSquared(shuttleWorldPos);
                  if (dsq < nearestDistSq) { nearestDistSq = dsq; nearestLandPos = p; }
                }
              }
            }
            if (nearestLandPos != Vec2::ZERO) break;
          }
          if (nearestLandPos != Vec2::ZERO) {
            for (auto unit : boarders) {
              unit->pendingShuttle = clickedShuttle;
              Vector<EnemyBase *> list;
              list.pushBack(unit);
              moveTo(list, nearestLandPos);
            }
          }
        }
        showTargetHand(clickedShuttle->getPosition(), false);
        return;
      }
    }
  }

  // Shuttle boarding via right-click: exactly one shuttle selected, right-click on a friendly unit
  if (selectedArray.size() == 1) {
    EnemyBase *shuttle = selectedArray.at(0);
    if (shuttle->unitType == UNIT_HUMAN_SHUTTLE || shuttle->unitType == UNIT_ORC_SHUTTLE) {
      for (auto unit : heroArray) {
        if (!unit || !unit->isVisible() || unit->isBuilding || unit == shuttle)
          continue;
        if (unit->getBoundingBox().containsPoint(mapPos) &&
            canUnitBoard(unit) && !unit->isInShuttle &&
            unit->alliSide == shuttle->alliSide) {
          float dist = unit->getPosition().distance(shuttle->getPosition());
          if (dist <= TILE_SIZE * 3 && isShuttleAdjacentToLand(shuttle)) {
            if (loadUnitToShuttle(shuttle, unit)) {
              refreshShuttleCargoPanel();
              showTargetHand(shuttle->getPosition(), false);
            }
          } else {
            // Move unit to the land tile adjacent to the shuttle
            Vec2 boardPos = shuttle->getPosition();
            Vec2 shuttleTile = getCoordinateFromPosition(shuttle->getPosition());
            int stx = (int)shuttleTile.x, sty = (int)shuttleTile.y;
            int smapH = (int)theMap->getMapSize().height;
            int sdx[] = {1, -1, 0, 0};
            int sdy[] = {0, 0, 1, -1};
            for (int i = 0; i < 4; i++) {
              int nx = stx + sdx[i], ny = sty + sdy[i];
              if (!isWaterTileAt(nx, ny)) {
                boardPos = Vec2((nx + 0.5f) * TILE_SIZE, (smapH - ny - 0.5f) * TILE_SIZE);
                break;
              }
            }
            unit->pendingShuttle = shuttle;
            Vector<EnemyBase *> list;
            list.pushBack(unit);
            moveTo(list, boardPos);
            showTargetHand(shuttle->getPosition(), false);
          }
          return;
        }
      }
    }
  }

  bool hasMovableUnits = false;
  for (auto unit : selectedArray) {
    if (unit == nullptr || unit->isBuilding || unit->isEnemy || unit->isAlli) {
      continue;
    }
    hasMovableUnits = true;
    break;
  }
  if (!hasMovableUnits) {
    return;
  }
  if (mapPos.x <= 0 || mapPos.y <= 0 || mapPos.x >= mapSize.width * TILE_SIZE ||
      mapPos.y >= mapSize.height * TILE_SIZE) {
    return;
  }

  onMoveClick();
  Vec2 coordinate = getCoordinateFromPosition(mapPos, theMap);
  bool tileOpen = (GM->tileState[(int)coordinate.x][(int)coordinate.y] == 0);
  if (!tileOpen) {
    // Ships can move to water tiles blocked for land units.
    if (isWaterTileAt((int)coordinate.x, (int)coordinate.y)) {
      for (auto unit : selectedArray) {
        if (unit && (unit->unitType == UNIT_HUMAN_SHUTTLE ||
                     unit->unitType == UNIT_ORC_SHUTTLE ||
                     unit->unitType == UNIT_HUMAN_SHIP ||
                     unit->unitType == UNIT_ORC_SHIP ||
                     unit->unitType == UNIT_HUMAN_BATTLE_SHIP ||
                     unit->unitType == UNIT_HUMAN_OIL_SHIP ||
                     unit->unitType == UNIT_ORC_OIL_SHIP)) {
          tileOpen = true;
          break;
        }
      }
    }
    // Air units can fly over any ground obstacle (trees, buildings, water).
    if (!tileOpen) {
      for (auto unit : selectedArray) {
        if (unit && unit->unitType == UNIT_HELICOPTER) {
          tileOpen = true;
          break;
        }
      }
    }
  }
  if (tileOpen) {
    if (isMultiplay) {
      std::string strIDs = "";
      for (auto unit : selectedArray) {
        strIDs += Value(unit->unitID).asString() + "|";
      }
      HMsg msg;
      msg.msgType = MSG_CODE_UNIT_MOVE;
      msg.sourceIDs = strIDs;
      msg.value0 = mapPos.x;
      msg.value1 = mapPos.y;
      msg.startTime = gameFrameTimer + LATANCY;
      ownMsgList.push_back(msg);
    } else {
      moveTo(selectedArray, mapPos);
    }
    showTargetHand(mapPos, false);
  }
}

void GameScene::handleWin32AttackMoveClick(Vec2 screenPos) {
  if (selectedArray.size() == 0) {
    cancelWin32AttackMoveMode();
    return;
  }
  Vec2 mapPos = (screenPos - getPosition()) / layerScale;

  EnemyBase *targetEnemy = nullptr;
  for (auto unit : enemyArray) {
    if (unit == nullptr) {
      continue;
    }
    if (unit->getBoundingBox().containsPoint(mapPos)) {
      targetEnemy = unit;
      break;
    }
  }
  if (targetEnemy != nullptr && targetEnemy->isEnemy) {
    if (isMultiplay) {
      std::string strIDs = "";
      for (auto unit : selectedArray) {
        strIDs += Value(unit->unitID).asString() + "|";
      }
      MM->attackTarget(strIDs, targetEnemy->unitID);
    } else {
      forceAttack(selectedArray, targetEnemy);
    }
    showTargetHand(targetEnemy->getPosition(), true);
    cancelWin32AttackMoveMode();
    return;
  }

  if (mapPos.x <= 0 || mapPos.y <= 0 || mapPos.x >= mapSize.width * TILE_SIZE ||
      mapPos.y >= mapSize.height * TILE_SIZE) {
    cancelWin32AttackMoveMode();
    return;
  }

  if (isMultiplay) {
    std::string strIDs = "";
    for (auto unit : selectedArray) {
      strIDs += Value(unit->unitID).asString() + "|";
    }
    MM->moveAndAttackTo(strIDs, mapPos.x, mapPos.y);
    HMsg msg;
    msg.msgType = MSG_CODE_ATTACK_DDANG;
    msg.sourceIDs = strIDs;
    msg.value0 = mapPos.x;
    msg.value1 = mapPos.y;
    msg.startTime = gameFrameTimer + LATANCY;
    ownMsgList.push_back(msg);
  } else {
    onAttackClick();
    moveAndAttackTo(selectedArray, mapPos);
    selectCommand(COMMAND_MOVE);
  }
  showTargetHand(mapPos, true);
  cancelWin32AttackMoveMode();
}
#else
bool GameScene::usesPcRtsControls() { return false; }
bool GameScene::isShiftHeldForSelection() { return false; }
bool GameScene::isCtrlHeldForSelection() { return false; }
#endif

void GameScene::addListener() {
  listener = EventListenerTouchAllAtOnce::create();
  listener->onTouchesBegan = [&](const std::vector<Touch *> &touches,
                                 Event *evt) {
//        log("onTouchesBegan. ID: %d", touches.at(0)->getID());
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    if (usesPcRtsControls()) {
      return;
    }
#endif
    if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
      return;
    }
    isBuildingTemplateMoved = false;
    isNormalClick = true;
    if (buildingTemplate != nullptr) {
      return;
    }
    touchBeganPos = touches.at(0)->getLocation();
    //        this->stopAllActions();
    lastTouchPoint = touchBeganPos;

    touchCount += touches.size();
    if (touchCount > 2) {
      touchCount = 2;
    }
    twoTouchEnabled = false;
    if (touchCount > 1 && false) {
      twoTouchEnabled = true;
      Touch *touch = touches.at(touches.size() - 1);
      secondTouchID = touch->getID();
      lastTouchPoint = touch->getLocation();
      touchBeganPos = touch->getLocation();
      if (touches.size() == 1) {
        secondTouchBeganDistance = touches.at(0)->getLocation().distanceSquared(
            touches.at(1)->getLocation());
      }
    }
    isMapMovingByMiniMap = false;

    Touch *touch = touches.at(0);
    isTouchBeganFromMiniMap = false;
    if (GM->currentStageIndex != STAGE_LOBBY &&
        cocos2d::Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos,
                      cocos2d::Size(miniMapWidth, miniMapHeight))
            .containsPoint(touch->getLocation())) { // click on minimap
      moveScreenToMiniMapGlPos(touch->getLocation());
      isTouchBeganFromMiniMap = true;
    }
    isSelectedBuildingTouchStarted = false;
  };
  listener->onTouchesMoved = [&](const std::vector<Touch *> &touches,
                                 Event *evt) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    if (usesPcRtsControls()) {
      return;
    }
#endif
    //        log("onTouchesMoved. count: %d", (int)touches.size());
    //        log("touch x: %f, y: %f", touches.at(0)->getLocation().x,
    //        touches.at(0)->getLocation().y);
    Touch *touch = touches.at(0);
    if (buildingTemplate != nullptr &&
        touchBeganPos.distanceSquared(touch->getLocation()) >
            1000) { // building positioning
      Vec2 touchPos = touch->getLocation();
      isMapMoveLeft = touchPos.x < 100;
      isMapMoveRight = touchPos.x > size.width - 100;
      isMapMoveDown = touchPos.y < 100;
      isMapMoveUp = touchPos.y > size.height - 100;
      updateBuildingTemplateAtScreenPos(touch->getLocation());
      return;
    }
    Touch *secondTouch = nullptr;
    for (auto touch : touches) {
      if (touch->getID() == secondTouchID) {
        secondTouch = touch;
        break;
      }
    }
    if (isTouchBeganFromMiniMap && buildingTemplate == nullptr) {
      moveScreenToMiniMapGlPos(touch->getLocation());
      isMapMovingByMiniMap = true;
    }

    if (GM->currentStageIndex != STAGE_LOBBY) {
      HUD->draw->clear();
    }
//        if(secondTouch != nullptr && touches.size() > 1){
//        if(touches.size() == 1 && buildingTemplate == nullptr){
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    if ((!HUD || !HUD->imgDragSelected->isVisible()) && touches.size() >= 1 &&
        buildingTemplate == nullptr) {
      Vec2 dtPos = touches.at(0)->getLocation() - lastTouchPoint;
      moveScreen(getPosition() + dtPos);

      lastTouchPoint = touches.at(0)->getLocation();
      if (touches.size() > 1) {
        int distanceSquared = touches.at(0)->getLocation().distanceSquared(
            touches.at(1)->getLocation());
        float scalingRate = 0.0000001f;
        float distanceDiff = distanceSquared - secondTouchBeganDistance;
        distanceDiff *= scalingRate;
        Vec2 pinchCenter = (touches.at(0)->getLocation() +
                            touches.at(1)->getLocation()) /
                           2.0f;
        setLayerScale(layerScale + distanceDiff, pinchCenter);
      }
      twoTouchEnabled = true;
    } else if ((HUD && HUD->imgDragSelected->isVisible()) &&
               !isMapMovingByMiniMap) {
      HUD->draw->drawRect(touchBeganPos, touches.at(0)->getLocation(),
                          Color4F::GREEN);
    }
#endif

    //        log("touchCount: %d", touchCount);
  };
  listener->onTouchesEnded = [&](const std::vector<Touch *> &touches,
                                 Event *evt) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    if (usesPcRtsControls()) {
      return;
    }
#endif
    isMapMoveUp = false;
    isMapMoveDown = false;
    isMapMoveLeft = false;
    isMapMoveRight = false;
    if (!isNormalClick) {
      return;
    }
    isNormalClick = false;
    //        log("onTouchesEnded. count: %d", (int)touches.size());
    if (buildingTemplate != nullptr && GM->currentStageIndex != STAGE_LOBBY) {
      confirmBuildingPlacement();
      return;
    }
    Touch *touch = touches.at(0);
    if (twoTouchEnabled) {
      //            cocos2d::Size mapSize = theMap->getMapSize()*TILE_SIZE;
      //            Vec2 currentPos = getPosition();
      //            this->stopAllActions();
      //            if(this->getPosition().x < -mapSize.width + size.width){
      //                this->runAction(EaseOut::create(MoveBy::create(0.2f,
      //                Vec2(-mapSize.width + size.width-getPositionX(), 0)),
      //                3));
      //            }
      //            if(this->getPosition().y < -mapSize.height + size.height){
      //                this->runAction(EaseOut::create(MoveBy::create(0.2f,
      //                Vec2(0, -mapSize.height + size.height-getPositionY())),
      //                3));
      //            }
      //            if(this->getPosition().x > 0){
      //                this->runAction(EaseOut::create(MoveBy::create(0.2f,
      //                Vec2(-getPositionX(), 0)), 3));
      //            }
      //            if(this->getPosition().y > 0){
      //                this->runAction(EaseOut::create(MoveBy::create(0.2f,
      //                Vec2(0, -getPositionY())), 3));
      //            }
    }

    bool somethingPicked = false;
    Vec2 pos = (lastTouchPoint - getPosition()) / layerScale;
    for (auto unit : heroArray) {
      if (unit->getBoundingBox().containsPoint(pos)) {
        somethingPicked = true;

        break;
      }
    }

    float touchBeganToEndDiff =
        touchBeganPos.distanceSquared(touch->getLocation());
    log("one click done: distance: %f", touchBeganToEndDiff);
    //        if(touchCount == 1 && !twoTouchEnabled){
    //        if(touchBeganToEndDiff < 2000){
    if (true) {
      if (touchBeganToEndDiff < 2000) { // click
        if (isTouchBeganFromMiniMap &&
            cocos2d::Rect(drawMiniMapForMoving->getPosition() +
                              miniMapDrawStartPos,
                          cocos2d::Size(miniMapWidth, miniMapHeight))
                .containsPoint(touch->getLocation())) { // click on minimap
          moveScreenToMiniMapGlPos(touch->getLocation());
        } else {
          doClick((touch->getLocation() - this->getPosition()) / layerScale);
        }

        //            }else{ // drag
      } else if ((HUD && HUD->imgDragSelected->isVisible())
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
                 && false
#endif
      ) { // drag
        if (!isMapMovingByMiniMap) {
          cocos2d::Rect rect = cocos2d::Rect(
              (touchBeganPos - this->getPosition()) / layerScale,
              cocos2d::Size(touch->getLocation().x - touchBeganPos.x,
                            touch->getLocation().y - touchBeganPos.y) /
                  layerScale);
          selectByDrag(rect);
          GM->playSoundEffect(SOUND_PENCIL_SHORT);
        }
      }
      //            log("selected: %d", (int)selectedArray.size());
    }
    if (HUD) {
      HUD->imgDragSelected->setVisible(false);
    }

    touchCount -= touches.size();
    if (touchCount <= 0) {
      touchCount = 0;
      twoTouchEnabled = false;
    }
    if (GM->currentStageIndex != STAGE_LOBBY)
      HUD->draw->clear();
  };
  listener->onTouchesCancelled = [&](const std::vector<Touch *> &touches,
                                     Event *evt) { log("onTouchesCancelled"); };
  getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  win32LastMouseViewPos = size / 2;
  if (usesPcRtsControls()) {
    Win32MouseCursor::setDefaultCursor();
  }
  mouseListener = EventListenerMouse::create();
  mouseListener->onMouseDown = [this](EventMouse *evt) {
    if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
      return;
    }
    win32LastMouseViewPos = evt->getLocationInView();
    Vec2 pos = getWin32MouseWorldPos(evt);
    if ((int)evt->getMouseButton() == 0) {
      win32LeftMouseDown = true;
      if (buildingTemplate != nullptr) {
        touchBeganPos = pos;
        lastTouchPoint = pos;
        return;
      }
      win32MouseOnWorkerMenu = isClickOnWorkerMenu(pos);
      touchBeganPos = pos;
      lastTouchPoint = pos;
      isNormalClick = true;
      isTouchBeganFromMiniMap = false;
      if (GM->currentStageIndex != STAGE_LOBBY &&
          drawMiniMapForMoving != nullptr &&
          cocos2d::Rect(drawMiniMapForMoving->getPosition() +
                            miniMapDrawStartPos,
                        cocos2d::Size(miniMapWidth, miniMapHeight))
              .containsPoint(pos)) {
        isTouchBeganFromMiniMap = true;
        if (!win32AttackMoveMode) {
          moveScreenToMiniMapGlPos(pos);
        }
      }
    }
  };
  mouseListener->onMouseMove = [this](EventMouse *evt) {
    win32LastMouseViewPos = evt->getLocationInView();
    Vec2 pos = getWin32MouseWorldPos(evt);
    updateWin32EdgeScroll(win32LastMouseViewPos);

    if (win32LeftMouseDown && isTouchBeganFromMiniMap && !win32AttackMoveMode) {
      moveScreenToMiniMapGlPos(pos);
      isMapMovingByMiniMap = true;
      return;
    }

    if (buildingTemplate != nullptr) {
      isMapMoveLeft = win32LastMouseViewPos.x < 100;
      isMapMoveRight = win32LastMouseViewPos.x > size.width - 100;
      isMapMoveDown = win32LastMouseViewPos.y < 100;
      isMapMoveUp = win32LastMouseViewPos.y > size.height - 100;
      updateBuildingTemplateAtScreenPos(pos);
      return;
    }

    if (win32LeftMouseDown && HUD != nullptr &&
        GM->currentStageIndex != STAGE_LOBBY) {
      HUD->draw->clear();
      if (touchBeganPos.distanceSquared(pos) > 150) {
        HUD->draw->drawRect(touchBeganPos, pos, Color4F::GREEN);
      }
    }

    // test now
    if (HUD != nullptr && GM->currentStageIndex != STAGE_LOBBY) {
      EnemyBase *hoveredEnemyBuilding = getEnemyBuildingAtMapPos(pos);
      if (hoveredEnemyBuilding != nullptr) {
        HUD->showEnemyResourceTooltip(pos, enemyGold, enemyLumber);
      } else {
        HUD->hideEnemyResourceTooltip();
      }
    }
  };
  mouseListener->onMouseUp = [this](EventMouse *evt) {
    win32LastMouseViewPos = evt->getLocationInView();
    Vec2 pos = getWin32MouseWorldPos(evt);

    if ((int)evt->getMouseButton() == 0) {
      if (!win32LeftMouseDown) {
        return;
      }
      win32LeftMouseDown = false;

      if (win32HudButtonClicked) {
        win32HudButtonClicked = false;
        win32MouseOnWorkerMenu = false;
        return;
      }

      float touchBeganToEndDiff = touchBeganPos.distanceSquared(pos);
      if (touchBeganToEndDiff < 2000 && HUD != nullptr && HUD->tryHandleHudClick(pos)) {
        win32MouseOnWorkerMenu = false;
        return;
      }
      if (touchBeganToEndDiff < 2000) {
        if (isTouchBeganFromMiniMap) {
          if (win32AttackMoveMode && drawMiniMapForMoving != nullptr) {
            cocos2d::Rect miniMapRect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos,
                                      cocos2d::Size(miniMapWidth, miniMapHeight));
            if (miniMapRect.containsPoint(pos)) {
              Vec2 mapPos = (pos - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos)) / miniMapScale;
              moveAndAttackTo(selectedArray, mapPos);
              showTargetHand(mapPos, true);
              cancelWin32AttackMoveMode();
            } else {
              moveScreenToMiniMapGlPos(pos);
            }
          } else {
            moveScreenToMiniMapGlPos(pos);
          }
        } else if (win32AttackMoveMode) {
          handleWin32AttackMoveClick(pos);
        } else if (win32MouseOnWorkerMenu || isClickOnWorkerMenu(pos)) {
          handleWorkerMenuClick(pos);
          if (HUD != nullptr && GM->currentStageIndex != STAGE_LOBBY) {
            HUD->draw->clear();
          }
          win32MouseOnWorkerMenu = false;
          return;
        } else if (buildingTemplate != nullptr) {
          if (HUD != nullptr && GM->currentStageIndex != STAGE_LOBBY) {
            HUD->draw->clear();
          }
          confirmBuildingPlacement();
          win32MouseOnWorkerMenu = false;
          return;
        } else if (tryClickBuildProgressIcon(pos)) {
          win32MouseOnWorkerMenu = false;
          return;
        } else {
          doClick((pos - getPosition()) / layerScale);
        }
      } else if (!isMapMovingByMiniMap) {
        Vec2 mapBegan = (touchBeganPos - getPosition()) / layerScale;
        Vec2 mapEnd = (pos - getPosition()) / layerScale;
        Vec2 mapDelta = mapEnd - mapBegan;
        cocos2d::Rect rect(mapBegan, Size(mapDelta.x, mapDelta.y));
        selectByDrag(rect);
        GM->playSoundEffect(SOUND_PENCIL_SHORT);
      }
      isTouchBeganFromMiniMap = false;
      isMapMovingByMiniMap = false;
      if (HUD != nullptr) {
        if (HUD->imgDragSelected != nullptr) {
          HUD->imgDragSelected->setVisible(false);
        }
        if (GM->currentStageIndex != STAGE_LOBBY) {
          HUD->draw->clear();
        }
      }
      win32MouseOnWorkerMenu = false;
    } else if ((int)evt->getMouseButton() == 1) {
      handleWin32RightClick(pos);
    }
  };
  mouseListener->onMouseScroll = [this](EventMouse *evt) {
    if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
      return;
    }
    if (!usesPcRtsControls()) {
      return;
    }
    float scrollY = evt->getScrollY();
    if (fabsf(scrollY) < 0.01f) {
      return;
    }
    setLayerScale(layerScale - scrollY * layerScaleWheelStep,
                  getWin32MouseWorldPos(evt));
  };
  getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener,
                                                               this);
#endif

  /*listener = cocos2d::EventListenerTouchOneByOne::create();
   listener->setSwallowTouches(false);

   listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
   listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
   listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
   //    listener->onTouchCancelled =
   CC_CALLBACK_2(MenuLayerMainMenu::onTouchCancelled, this);

   _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);*/

  /*
   listener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
   {
   this->onTouchBegan(touch, event);
   return false; // we did not consume this event, pass thru.
   };
   listener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
   {
   this->onTouchMoved(touch, event);
   };
   listener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
   {
   this->onTouchEnded(touch, event);
   };

   cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener,
   30);
   */
}
void GameScene::setWorkerToBuild(EnemyBase *worker, int buildingIndex, int x,
                                  int y) {
  buildingTemplateSize = GM->getBuildingOccupySize(buildingIndex);
  worker->builderSize = buildingTemplateSize;
  worker->builderBuildingIndex = buildingIndex;
  worker->builderCoordinate = Vec2(x, y);
  worker->builderSpriteName = getSpriteNameForUnit(buildingIndex);

  Vec2 pos;
  if (buildingIndex == UNIT_HUMAN_OIL_EXTRACTOR || buildingIndex == UNIT_ORC_OIL_EXTRACTOR) {
    // OilShip is a water unit: move directly to the build location
    pos = getPositionFromTileCoordinate(worker->builderCoordinate.x,
                                        worker->builderCoordinate.y) +
          Vec2(buildingTemplateSize.width * TILE_SIZE / 2,
               buildingTemplateSize.height * TILE_SIZE / 2);
  } else if (buildingIndex == UNIT_HUMAN_SHIPYARD || buildingIndex == UNIT_ORC_SHIPYARD ||
             buildingIndex == UNIT_HUMAN_OIL_REFINERY || buildingIndex == UNIT_ORC_OIL_REFINERY ||
             buildingIndex == UNIT_HUMAN_FOUNDRY || buildingIndex == UNIT_ORC_FOUNDRY) {
    // Water building: move worker to the nearest adjacent land tile instead of
    // the water center (workers cannot path through water tiles).
    int cx = x;
    int cy = y;
    int w  = (int)buildingTemplateSize.width;
    int h  = (int)buildingTemplateSize.height;
    Vec2 workerTile = getCoordinateFromPosition(worker->getPosition());
    Vec2 bestTile   = Vec2(-1, -1);
    float minDist   = FLT_MAX;
    auto tryTile = [&](int tx, int ty) {
      if (tx < 0 || ty < 0 || tx >= (int)mapSize.width || ty >= (int)mapSize.height) return;
      Vec2 t(tx, ty);
      if (!isSoilBlock(getTileGIDAt(soilLayer, t)) &&
          GM->tileState[tx][ty] == 0) {
        float d = workerTile.distanceSquared(t);
        if (d < minDist) { minDist = d; bestTile = t; }
      }
    };
    for (int i = cx - 1; i <= cx + w; i++) {
      tryTile(i, cy);
      tryTile(i, cy - h - 1);
    }
    for (int j = cy - 1; j >= cy - h; j--) {
      tryTile(cx - 1, j);
      tryTile(cx + w, j);
    }
    if (bestTile.x >= 0) {
      pos = getPositionFromTileCoordinate(bestTile.x, bestTile.y);
    } else {
      // Fallback: building center (construction triggers immediately)
      pos = getPositionFromTileCoordinate(cx, cy) +
            Vec2(w * TILE_SIZE / 2, h * TILE_SIZE / 2);
    }
  } else {
    pos = this->getPositionFromTileCoordinate(worker->builderCoordinate.x,
                                               worker->builderCoordinate.y) +
          Vec2(buildingTemplateSize.width * TILE_SIZE / 2,
               buildingTemplateSize.height * TILE_SIZE / 2);
  }

  Vector<EnemyBase *> list;
  list.pushBack(worker);
  if (isMultiplay) {
    if (worker->isEnemy) {
      moveTo(list, pos);
    } else {
      moveTo(list, pos);
    }
  } else {
    moveTo(list, pos);
  }
  worker->isGoingToBuild = true;

  list.clear();
}
float GameScene::getCommandMarkerScale() const {
  return commandMarkerBaseScale;
}

float GameScene::getWorkerMenuScale() const {
  return workerMenuBaseScale * commandMarkerBaseScale;
}

float GameScene::getWorkerMenuRadius() const {
  return workerMenuBaseRadius * commandMarkerBaseScale;
}

float GameScene::getWorkerMenuCenterOffsetY() const {
  return workerMenuCenterOffsetY * commandMarkerBaseScale;
}

void GameScene::setLayerScale(float newScale, const Vec2 &anchorGlPos) {
  if (newScale < minLayerScale) {
    newScale = minLayerScale;
  }
  if (newScale > maxLayerScale) {
    newScale = maxLayerScale;
  }
  if (fabsf(newScale - layerScale) < 0.0001f) {
    return;
  }
  Vec2 mapPos = (anchorGlPos - getPosition()) / layerScale;
  layerScale = newScale;
  setScale(layerScale);
  moveScreen(anchorGlPos - mapPos * layerScale);
}

void GameScene::moveScreenToMiniMapGlPos(const Vec2 &glPos) {
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
    return;
  }
  if (GM->currentStageIndex == STAGE_LOBBY || drawMiniMapForMoving == nullptr) {
    return;
  }
  cocos2d::Rect miniMapRect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos,
                            cocos2d::Size(miniMapWidth, miniMapHeight));
  if (!miniMapRect.containsPoint(glPos)) {
    return;
  }
  Vec2 newTargetPos =
      (glPos - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos)) *
      layerScale / miniMapScale;
  Vec2 newPos = -newTargetPos + size / 2;
  moveScreen(newPos);
}

void GameScene::moveScreen(cocos2d::Vec2 pos) {
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
    return;
  }
  float xMin = -500;
  float xMax = 500;
  float yMin = -500;
  float yMax = 500;
  if (pos.x > xMax) {
    pos.x = xMax;
  }
  if (pos.y > yMax) {
    pos.y = yMax;
  }
  if (pos.x < -TILE_SIZE * mapSize.width * layerScale + size.width + xMin) {
    pos.x = -TILE_SIZE * mapSize.width * layerScale + size.width + xMin;
  }
  if (pos.y < -TILE_SIZE * mapSize.height * layerScale + size.height + yMin) {
    pos.y = -TILE_SIZE * mapSize.height * layerScale + size.height + yMin;
  }
  this->setPosition(pos); // test
  if (GM->currentStageIndex != STAGE_LOBBY) {
    miniMapViewRect = cocos2d::Rect(
        (miniMapDrawStartPos.x + (-pos.x) * miniMapScale) / layerScale,
        (miniMapDrawStartPos.y + (-pos.y) * miniMapScale) / layerScale,
        size.width * miniMapScale / layerScale,
        size.height * miniMapScale / layerScale);
    viewRect = cocos2d::Rect(-pos / layerScale, size / layerScale);
  }
}
void GameScene::checkVisibilityForInScreen() {
  if (gameMode == GAME_MODE_NORMAL || gameMode == GAME_MODE_HARD) {
    for (auto unit : mutualArray) {
      unit->isInScreen = this->isInScreen(unit->getPosition());
      unit->checkVisible();
    }
    for (auto unit : heroArray) {
      unit->isInScreen = this->isInScreen(unit->getPosition());
      unit->checkVisible();
    }
    for (auto unit : enemyArray) {
      unit->isInScreen = this->isInScreen(unit->getPosition());
      unit->checkVisible();
    }
  }
}
void GameScene::doClick(cocos2d::Vec2 pos) {
  if (gameMode == GAME_MODE_PVP6 || gameMode == GAME_MODE_PVP12) {
    return;
  }
  try {
    if (HUD) {
      //            log("stage %d", GM->currentStageIndex);
      if (GM->currentStageIndex == STAGE_RAID ||
          GM->currentStageIndex == STAGE_SINGLEPLAY) {
        HUD->rightBottomPanelForCampaign->getChildByName("ndMission")
            ->setVisible(false);
        HUD->rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(
            true);
      } else {
        HUD->rightBottomPanelForCampaign->getChildByName("ndMission")
            ->setVisible(true);
        HUD->rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(
            false);
      }

      for (int i = 0; i < 6; i++) {
        HUD->setMenu(i, BTN_TYPE_NONE);
      }
      if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 11) {
        HUD->tutorialEnded = true;
      }
    }

    if (MM->enemyStateChanged) {
      MM->enemyStateChanged = false;
      if (MM->enemyRace < 0) {
        endGame(true);
      }
    }

    selectedCommand = COMMAND_NOTHING;

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    if (tryDeliverCarriedResourcesAt(pos)) {
      return;
    }
#endif

    EnemyBase *selectedUnit = nullptr;
    if (selectedUnit == nullptr) {
      Vector<EnemyBase *> selectedArray;
      for (auto unit : heroArray) { // select hero
        if (!unit || unit == nullptr || !unit->isVisible())
          continue;
        if (unit->spine) {
          //                    if(unit->spine->getBoundingBox().containsPoint(pos
          //                    - unit->getPosition())){
          if (unit->getBoundingBoxForIntersect().containsPoint(pos)) {
            selectedArray.pushBack(unit);
          }
        } else if (unit->Sprite::getBoundingBox().containsPoint(pos)) {
          selectedArray.pushBack(unit);
        }
      }
      int zOrder = -9999999;
      for (auto unit : selectedArray) {
        if (unit->getLocalZOrder() > zOrder) {
          selectedUnit = unit;
          zOrder = unit->getLocalZOrder();
        }
      }
      selectedArray.clear();
    }
    if (selectedUnit == nullptr) {
      for (auto unit : enemyArray) { // select enemy
        if (!unit || unit == nullptr)
          continue;
        if (unit->Sprite::getBoundingBox().containsPoint(pos)) {
          selectedUnit = unit;
          doubleClickTimerForAttackDdaing = 0;
          doubleClickTimerForSelectTheSame = 0;
          break;
        }
      }
    }

    EnemyBase *mutual = nullptr;
    if (selectedUnit == nullptr || selectedUnit->isCarryingGold) {
      for (auto unit : mutualArray) {
        if (!unit || unit == nullptr)
          continue;
        //            cocos2d::Rect rect = cocos2d::Rect(pos.x - 10, pos.y - 10,
        //            unit->getContentSize().width + 20,
        //            unit->getContentSize().height + 20);
        if (unit->getBoundingBox().containsPoint(pos)) {
          //            if(unit->getBoundingBox().intersectsRect(rect)){
          mutual = unit;
          doubleClickTimerForAttackDdaing = 0;
          doubleClickTimerForSelectTheSame = 0;
          break;
        }
      }
      if (mutual != nullptr && mutual->unitType == UNIT_MINE &&
          selectedUnit != nullptr && selectedUnit->unitType == UNIT_WORKER) {
        selectedUnit = nullptr;
      }
    }
    // Mobile tap: OilShip taps OilExtractor
    if (mutual == nullptr && selectedUnit == nullptr) {
      EnemyBase *tapExtractor = nullptr;
      for (auto unit : heroArray) {
        if (unit == nullptr || (unit->unitType != UNIT_HUMAN_OIL_EXTRACTOR &&
                                unit->unitType != UNIT_ORC_OIL_EXTRACTOR))
          continue;
        if (unit->getBoundingBox().containsPoint(pos)) {
          tapExtractor = unit;
          break;
        }
      }
      if (tapExtractor != nullptr) {
        bool deferOilGatherToRightClick = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        deferOilGatherToRightClick = usesPcRtsControls();
#endif
        if (!deferOilGatherToRightClick) {
          gatherOilWithSelectedOilShip(tapExtractor);
        }
      }
    }
    if (mutual != nullptr) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
      if (usesPcRtsControls() && mutual->unitType == UNIT_TREE) {
        for (auto unit : selectedArray) {
          if (!unit || unit == nullptr)
            continue;
          if (unit->alliSide == WHICH_SIDE_ENEMY)
            continue;
          if (unit->unitType == UNIT_WORKER ||
              unit->unitType == UNIT_GOBLIN_WORKER) {
            deselectAll();
            return;
          }
        }
      }
#endif
      bool deferGatherToRightClick = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
      deferGatherToRightClick = usesPcRtsControls();
#endif
      if (!deferGatherToRightClick) {
        gatherResourceWithSelectedWorkers(mutual);
      }
      for (auto unit : selectedArray) {
        if (!unit || unit == nullptr)
          continue;
        if (unit->alliSide == WHICH_SIDE_ENEMY)
          continue;
        if (unit->unitType == UNIT_WORKER ||
            unit->unitType == UNIT_GOBLIN_WORKER) {
          continue;
        } else {
          if (mutual->unitType == UNIT_TREE_FOR_BATTLE &&
              unit->unitType == UNIT_WIZARD) {
            unit->unitAct = UNIT_ACT_ATTACK;
            unit->moveToPos = Vec2::ZERO;
            unit->target = mutual;
            unit->moveFlagPos = mutual->getPosition();
            showTargetHand(mutual->getPosition(), true);
          } else {
            unit->moveToTarget(mutual);
          }
        }
      }
      if (mutual->unitType == UNIT_MINE &&
          GM->currentStageIndex != STAGE_LOBBY) {
        HUD->showInstanceMessage(
            strmake(LM->getText("gold left").c_str(), mutual->energy));
      }
    } else if (selectedUnit == nullptr) { // ground selected
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
      if (usesPcRtsControls() && selectedArray.size() > 0 &&
          !isShiftHeldForSelection()) {
        deselectAll();
      } else
#endif
          if (selectedArray.size() > 0 && !selectedArray.at(0)->isEnemy &&
              !selectedArray.at(0)->isAlli &&
              !selectedArray.at(0)->isBuilding && pos.x > 0 && pos.y > 0 &&
              pos.x < mapSize.width * TILE_SIZE &&
              pos.y < mapSize.height * TILE_SIZE) { // hero is selected so move!
        if (selectedCommand == COMMAND_ATTACK) {
          moveAndAttackTo(selectedArray, pos);
          log("do click whatwaht?");
        } else {
          if (doubleClickTimerForAttackDdaing > 0) {
            if (isMultiplay) { // test
              std::string strIDs = "";
              for (auto unit : selectedArray) {
                strIDs += Value(unit->unitID).asString() + "|";
              }
              // for network
              MM->moveAndAttackTo(strIDs, pos.x, pos.y);
              attackDdangSent = true;
              // for hero
              HMsg msg;
              msg.msgType = MSG_CODE_ATTACK_DDANG;
              msg.sourceIDs = strIDs;
              msg.value0 = pos.x;
              msg.value1 = pos.y;
              msg.startTime = gameFrameTimer + LATANCY;
              ownMsgList.push_back(msg);
            } else {
              onAttackClick();
              moveAndAttackTo(selectedArray, pos);
              selectCommand(COMMAND_MOVE);
            }
            showTargetHand(pos, true);
          } else {
            onMoveClick();
            Movable *building = nullptr;
            for (auto unit : heroArray) {
              if (unit->isBuilding) {
                Vec2 buildingPos = getPositionFromTileCoordinate(
                    unit->buildingStartCoordinate.x,
                    unit->buildingStartCoordinate.y);
                float height = unit->buildingOccupySize.height * TILE_SIZE;
                cocos2d::Rect theRect = cocos2d::Rect(
                    buildingPos.x, buildingPos.y - height,
                    unit->buildingOccupySize.width * TILE_SIZE, height);
                if (theRect.containsPoint(pos)) {
                  building = unit;
                  log("building clickek %d", unit->unitType);
                  break;
                }
              }
            }
            for (auto unit : selectedArray) {
              unit->isGoingToBuild = false;
            }
            //                        if(building == nullptr){
            Vec2 coordinate = getCoordinateFromPosition(pos, theMap);
            int tileState = GM->tileState[(int)coordinate.x][(int)coordinate.y];
            if (tileState == 0) {
              if (isMultiplay) { // test
                std::string strIDs = "";
                for (auto unit : selectedArray) {
                  strIDs += Value(unit->unitID).asString() + "|";
                }
                // for network
                moveIDForMultiplay = strIDs;
                movePosForMultiplay = pos;
                this->scheduleOnce(
                    schedule_selector(GameScene::checkAttackDdangSent),
                    doubleClickTimerForAttackDdaing);

                // for hero
                HMsg msg;
                msg.msgType = MSG_CODE_UNIT_MOVE;
                msg.sourceIDs = strIDs;
                msg.value0 = pos.x;
                msg.value1 = pos.y;
                msg.startTime = gameFrameTimer + LATANCY;
                ownMsgList.push_back(msg);
                showTargetHand(pos, false);
              } else {
                showTargetHand(pos, false);
                moveTo(selectedArray, pos);
              }

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
              doubleClickTimerForAttackDdaing = 0.3f * gameSpeed;
#endif
            }
            //                        }else if(building != nullptr){
            //                            moveTo(selectedArray,
            //                            building->getApproachingPoint(pos));
            //                        }
          }
          //                doubleClickTimer = 0.3f;
        }
      } else {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
        doubleClickTimerForAttackDdaing = 0.3f * gameSpeed;
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        if (!isShiftHeldForSelection())
#endif
          deselectAll();
      }
    } else if (selectedUnit->isEnemy) { // enemy selected
      if (selectedArray.size() > 0 && !selectedArray.at(0)->isAlli &&
          !selectedArray.at(0)->isEnemy &&
          !selectedArray.at(0)->isBuilding) { // hero is selected so attack!
        if (isMultiplay) {
          std::string strIDs = "";
          for (auto unit : selectedArray) {
            strIDs += Value(unit->unitID).asString() + "|";
          }
          // for network
          MM->attackTarget(strIDs, selectedUnit->unitID);

          // for hero
          HMsg msg;
          msg.msgType = MSG_CODE_ATTACK_TARGET;
          msg.sourceIDs = strIDs;
          msg.targetID = selectedUnit->unitID;
          msg.startTime = gameFrameTimer + LATANCY;
          ownMsgList.push_back(msg);
          showTargetHand(pos, true);
        } else {
          forceAttack(selectedArray, selectedUnit);
        }
        showTargetHand(selectedUnit->getPosition(), true);
      } else {
        deselectAll();
        selectUnit(selectedUnit);
      }
    } else if (!selectedUnit->isEnemy) { // hero selected
      // Shuttle boarding via click (mobile only — PC uses right-click)
      if (!usesPcRtsControls() && selectedArray.size() == 1) {
        EnemyBase *shuttle = selectedArray.at(0);
        if ((shuttle->unitType == UNIT_HUMAN_SHUTTLE ||
             shuttle->unitType == UNIT_ORC_SHUTTLE) &&
            selectedUnit != shuttle && !selectedUnit->isBuilding &&
            isShuttleAdjacentToLand(shuttle)) {
          if (loadUnitToShuttle(shuttle, selectedUnit)) {
            refreshShuttleCargoPanel();
            return;
          }
        }
      }
      if (isCtrlHeldForSelection()) {
        if (selectedArray.find(selectedUnit) != selectedArray.end()) {
          deselect((Movable *)selectedUnit);
        } else {
          selectUnit(selectedUnit);
          if (selectedUnit->isBuilding &&
              GM->currentStageIndex != STAGE_LOBBY) {
            HUD->showDisassembleButton();
          }
        }
        GM->playSoundEffect(SOUND_PENCIL_SHORT);
      } else {
      bool isWorkingWorkerSelected = false;
      for (auto unit : selectedArray) {
        if (!unit || unit == nullptr || !unit->isVisible())
          continue;
        if (unit->unitType == UNIT_WORKER ||
            unit->unitType == UNIT_GOBLIN_WORKER) {
          if (unit->isCarryingTree &&
              (selectedUnit->unitType == UNIT_CASTLE ||
               selectedUnit->unitType == UNIT_ORC_HQ ||
               selectedUnit->unitType == UNIT_LUMBERMILL)) {
            unit->returningPlace = selectedUnit;
            unit->moveToTarget(selectedUnit);
            unit->unitAct = UNIT_ACT_GATHER_TREE;
            isWorkingWorkerSelected = true;
            //                        unit->isTemporaryFlying = true;
            //                        unit->runAnimation(ANIMATION_TYPE_MOVE);
            unit->isGatheringTree = true;
            if (isMultiplay) {
              MM->returnLumber(Value(unit->unitID).asString(),
                               selectedUnit->unitID);
            }
          } else if (unit->isCarryingGold &&
                     (selectedUnit->unitType == UNIT_CASTLE ||
                      selectedUnit->unitType == UNIT_ORC_HQ)) {
            unit->returningPlace = selectedUnit;
            unit->unitAct = UNIT_ACT_GATHER_GOLD;
            unit->moveToTarget(selectedUnit);
            isWorkingWorkerSelected = true;
            //                        unit->isTemporaryFlying = true;
            //                        unit->runMoveAnimation(DIRECTION_E);
            unit->isGatheringGold = true;
            if (isMultiplay) {
              MM->returnGold(Value(unit->unitID).asString(),
                             selectedUnit->unitID);
            }
          }
        }
      }
      if (!isWorkingWorkerSelected) {
        if (selectedCommand == COMMAND_ATTACK) {
          forceAttack(selectedArray, selectedUnit);
        } else {
          const bool shiftAddSelection = isShiftHeldForSelection();
          if (!shiftAddSelection) {
            deselectAll();
          }

          if (doubleClickTimerForSelectTheSame > 0 &&
              !selectedUnit->isBuilding) {
            for (auto unit : heroArray) {
              if (!unit || unit == nullptr || !unit->isVisible())
                continue;
              if (unit->unitType == selectedUnit->unitType &&
                  viewRect.containsPoint(unit->getPosition())) {
                selectUnit(unit);
              }
            }
          } else if (!shiftAddSelection ||
                     selectedArray.find(selectedUnit) ==
                         selectedArray.end()) {
            selectUnit(selectedUnit);
            if (selectedUnit->isBuilding &&
                GM->currentStageIndex != STAGE_LOBBY) {
              HUD->showDisassembleButton();
            }
            if (GM->currentStageIndex != STAGE_LOBBY &&
                WORLD->stageIndex == 0 && HUD->tutorialIndex == 0 &&
                !GM->isColosseum && difficultyMode != DIFFICULTY_MODE_HELL) {
              HUD->tutorialIndex++;
              HUD->talkIndex = 0;
              HUD->talkText = LM->getText("tutorial 1");
              Sprite *spt =
                  (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
              spt->setScale(1);
              Sprite *sptTemp = Sprite::create("handIcon.png");
              spt->setSpriteFrame(sptTemp->getSpriteFrame());
              cocos2d::Size size = Director::getInstance()->getWinSize();
              spt->stopAllActions();
              spt->runAction(RepeatForever::create(Sequence::create(
                  MoveTo::create(0, Vec2(size.width / 2 - 750, 330 + 50)),
                  MoveBy::create(0.1f, Vec2(0, -100)),
                  MoveBy::create(0.15f, Vec2(0, 100)), DelayTime::create(1),
                  NULL)));
            }
          }

          doubleClickTimerForSelectTheSame = 0.3f * gameSpeed;

          GM->playSoundEffect(SOUND_PENCIL_SHORT);
        }
      }
      }
    }

    if (HUD && HUD->isRaid && !HUD->isRaidStarted && selectedUnit == nullptr) {
      placeDeckUnitForRaid(pos);
    }
    //        if(selectedUnit != nullptr){
    //            log("zorder %d", selectedUnit->getLocalZOrder());
    //        }
  } catch (...) {
  }
}
void GameScene::checkAttackDdangSent(float dt) {
  if (!attackDdangSent) {
    MM->moveUnit(moveIDForMultiplay, movePosForMultiplay.x,
                 movePosForMultiplay.y);
  }
  attackDdangSent = false;
}
bool GameScene::gatherResourceWithSelectedWorkers(EnemyBase *mutual) {
  if (mutual == nullptr) {
    return false;
  }
  bool didGather = false;
  for (auto unit : selectedArray) {
    if (!unit || unit == nullptr)
      continue;
    if (unit->alliSide == WHICH_SIDE_ENEMY)
      continue;
    if (unit->unitType != UNIT_WORKER && unit->unitType != UNIT_GOBLIN_WORKER) {
      continue;
    }
    if (mutual->unitType == UNIT_MINE) {
      if (isMultiplay) {
        MM->gatherGold(Value(unit->unitID).asString(), mutual->unitID);
        HMsg msg;
        msg.msgType = MSG_CODE_WORKER_GO_TO_MINE;
        msg.sourceIDs = Value(unit->unitID).asString();
        msg.targetID = mutual->unitID;
        msg.startTime = gameFrameTimer + LATANCY;
        ownMsgList.push_back(msg);
      } else {
        setWorkerToMine(unit, mutual);
      }

      if (WORLD->stageIndex == 0 && unit->unitType == UNIT_WORKER &&
          HUD->tutorialIndex == 3) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 4");
        Sprite *spt = (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
        spt->stopAllActions();
        spt->setSpriteFrame("tree2_0.png");
        spt->setScale(1 / WORLD->imageScale);
      }
      GM->playSoundEffect(SOUND_PENCIL_SHORT);
      Sprite *spt = Sprite::createWithSpriteFrameName("pickax.png");
      spt->setPosition(mutual->getPosition() + Vec2(0, 100));
      this->addChild(spt, 100);
      spt->runAction(Sequence::create(
          EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2),
          DelayTime::create(0.1f),
          EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2),
          SPT_REMOVE_FUNC, NULL));
      didGather = true;
    } else if (mutual->unitType == UNIT_TREE) {
      if (isMultiplay) {
        MM->gatherTree(Value(unit->unitID).asString(), mutual->unitID);
        HMsg msg;
        msg.msgType = MSG_CODE_WORKER_GO_TO_TREE;
        msg.sourceIDs = Value(unit->unitID).asString();
        msg.targetID = mutual->unitID;
        msg.startTime = gameFrameTimer + LATANCY;
        ownMsgList.push_back(msg);
      } else {
        setWorkerToTree(unit, mutual);
      }

      if (WORLD->stageIndex == 0 && unit->unitType == UNIT_WORKER &&
          HUD->tutorialIndex == 4) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 5");
        Sprite *spt = (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
        spt->stopAllActions();
        spt->setScale(1);
        Sprite *sptTemp = Sprite::create("btnBuild.png");
        spt->setSpriteFrame(sptTemp->getSpriteFrame());
      }

      GM->playSoundEffect(SOUND_PENCIL_SHORT);
      Sprite *spt = Sprite::createWithSpriteFrameName("axe.png");
      spt->setPosition(mutual->getPosition() + Vec2(0, 100));
      this->addChild(spt, 100);
      spt->runAction(Sequence::create(
          EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2),
          DelayTime::create(0.1f),
          EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2),
          SPT_REMOVE_FUNC, NULL));
      didGather = true;
    }
  }
  if (didGather && HUD != nullptr) {
    HUD->dismissMenu();
  }
  return didGather;
}

void GameScene::setWorkerToTree(EnemyBase *unit, EnemyBase *mutual) {
  //    if(mutual->unitType == UNIT_UNREACHABLE_TREE){
  //        mutual->unitType = UNIT_TREE;
  //    }
  if (unit->isCarryingTree) {
    unit->isGatheringTree = true;
    unit->failedFindPathStart = Vec2::ZERO;
    unit->failedFindPathEnd = Vec2::ZERO;
    unit->isTemporaryFlying = true;
    if (isNewCommandSystem) {
      unit->unitAct = UNIT_ACT_GATHER_TREE;
      if (unit->returningPlace) {
        //                unit->moveFlagPos =
        //                unit->returningPlace->getPosition();
        unit->moveFlagPos =
            unit->returningPlace->getApproachingPoint(unit->getPosition());
      }
      unit->moveToPos = Vec2::ZERO;
      //            if (isMultiplay) {
      //                                    MM->moveUnit(Value(unit->unitID).asString(),
      //                                    (int)unit->moveFlagPos.x,
      //                                    (int)unit->moveFlagPos.y);
      //            }
    } else {
      unit->moveToTarget(unit->returningPlace);
    }
  } else {
    //        Vec2 coordinate =
    //        WORLD->getCoordinateFromPosition(unit->getPosition()); Vec2
    //        destCoordinate =
    //        WORLD->getCoordinateFromPosition(mutual->getApproachingPoint(unit->getPosition()));
    ////                            PointArray* array = GM->getPath(coordinate,
    /// destCoordinate);
    //
    //        if(mutual != nullptr){
    unit->gatherTree(mutual);
    //        }
  }
}
void GameScene::setWorkerToMine(EnemyBase *worker, EnemyBase *mine) {
  if (worker->isCarryingGold) {
    worker->isGatheringGold = true;
    worker->failedFindPathStart = Vec2::ZERO;
    worker->failedFindPathEnd = Vec2::ZERO;
    //        worker->isTemporaryFlying = true;
    if (isNewCommandSystem) {
      worker->unitAct = UNIT_ACT_GATHER_GOLD;
      if (worker->returningPlace) {

        worker->moveFlagPos =
            worker->returningPlace->getApproachingPoint(worker->getPosition());
      }
      worker->moveToPos = Vec2::ZERO;
      //            if (isMultiplay) {
      //                MM->moveUnit(Value(worker->unitID).asString(),
      //                (int)worker->moveFlagPos.x, (int)worker->moveFlagPos.y);
      //            }
    } else {
      worker->moveToTarget(worker->returningPlace);
    }
  } else {
    worker->gatherGold(mine);
    //        if (isMultiplay) {
    //            MM->gatherGold(Value(worker->unitID).asString(),
    //            mine->unitID);
    //        }
  }
}
void GameScene::placeDeckUnitForRaid(Vec2 pos) {
  if (isOccupied(WORLD->getCoordinateFromPosition(pos))) {
    log("the spot clicked is occupied");
    return;
  }
  HUD->rightBottomPanel->getChildByName("ndBattle")
      ->getChildByName("btnFindMatch")
      ->removeFromParent();
  HUD->rightBottomPanel->getChildByName("btnSelectDrag")->setVisible(true);
  HUD->rightBottomPanel->getChildByName("btnSelectScreen")->setVisible(true);
  HUD->rightBottomPanel->getChildByName("btnSelectAll")->setVisible(true);
  //            HUD->rightBottomPanel->getChildByName("ndBattle")->getChildByName("btnSurrender")->removeFromParent();
  HUD->bottomUnitBar->runAction(
      EaseBackIn::create(MoveBy::create(0.5f, Vec2(0, -600))));
  Vec2 originalCoordinate = getCoordinateFromPosition(pos);
  HUD->isRaidStarted = true;
  int startX = 0;
  int max = 0;
  int x = startX;
  int y = max;
  int direction = -1; // 0:E 1:S 2:W 3:N
  bool changeDirection = false;
  Vector<Movable *> unitList;
  //            for(int i = 0; i < 100; i++){ // test
  //                UnitInfo* info = new UnitInfo();
  //                info->unitType = UNIT_ARCHER;
  //                HUD->deckUnitList.push_back(info);
  //            }

  for (auto info : HUD->deckUnitList) {
    Vec2 posFromCoordinate = getPositionFromTileCoordinate(
        originalCoordinate.x + x, originalCoordinate.y + y);
    info->x = posFromCoordinate.x;
    info->y = posFromCoordinate.y;
    Movable *unit = GM->getUnitFromData(info);
    //                unit->setPosition(posFromCoordinate);
    //                addUnit(unit);
    if (unit->spine) {
      setHeroLevelInfo((EnemyBase *)unit, info->level % 100);
    }
    if (unit->getPositionX() < TILE_SIZE) {
      unit->setPositionX(TILE_SIZE);
    } else if (unit->getPositionY() < TILE_SIZE) {
      unit->setPositionY(TILE_SIZE);
    } else if (unit->getPositionX() >= mapSize.width * TILE_SIZE - TILE_SIZE) {
      unit->setPositionX(mapSize.width * TILE_SIZE - TILE_SIZE);
    } else if (unit->getPositionY() >= mapSize.height * TILE_SIZE - TILE_SIZE) {
      unit->setPositionY(mapSize.height * TILE_SIZE - TILE_SIZE);
    }

    do {
      if (direction < 0 || (x == 0 && y == max)) {
        y++;
        x++;
        max++;
        if (direction < 0) {
          direction = 0;
        }
      } else if (direction == 0) {
        x++;
        if (x > max) {
          changeDirection = true;
        }
      } else if (direction == 1) {
        y--;
        if (y < -max) {
          changeDirection = true;
        }
      } else if (direction == 2) {
        x--;
        if (x < -max) {
          changeDirection = true;
        }
      } else if (direction == 3) {
        y++;
        if (y > max) {
          changeDirection = true;
        }
      }
      if (changeDirection) {
        changeDirection = false;
        if (direction == 0) {
          x--;
          y--;
        } else if (direction == 1) {
          x--;
          y++;
        } else if (direction == 2) {
          x++;
          y++;
        } else if (direction == 3) {
          x++;
          y--;
        }
        direction++;
        if (direction > 3) {
          direction = 0;
        }
      }
    } while (isOccupied(originalCoordinate + Vec2(x, y)));
  }

  for (auto info : HUD->heroDeckUnitList) {
    Vec2 posFromCoordinate = getPositionFromTileCoordinate(
        originalCoordinate.x + x, originalCoordinate.y + y);
    info->x = posFromCoordinate.x;
    info->y = posFromCoordinate.y;
    Movable *unit = GM->getUnitFromData(info);
    setHeroLevelInfo((EnemyBase *)unit, info->level % 100);
    if (unit->getPositionX() < TILE_SIZE) {
      unit->setPositionX(TILE_SIZE);
    } else if (unit->getPositionY() < TILE_SIZE) {
      unit->setPositionY(TILE_SIZE);
    } else if (unit->getPositionX() >= mapSize.width * TILE_SIZE - TILE_SIZE) {
      unit->setPositionX(mapSize.width * TILE_SIZE - TILE_SIZE);
    } else if (unit->getPositionY() >= mapSize.height * TILE_SIZE - TILE_SIZE) {
      unit->setPositionY(mapSize.height * TILE_SIZE - TILE_SIZE);
    }

    do {
      if (direction < 0 || (x == 0 && y == max)) {
        y++;
        x++;
        max++;
        if (direction < 0) {
          direction = 0;
        }
      } else if (direction == 0) {
        x++;
        if (x > max) {
          changeDirection = true;
        }
      } else if (direction == 1) {
        y--;
        if (y < -max) {
          changeDirection = true;
        }
      } else if (direction == 2) {
        x--;
        if (x < -max) {
          changeDirection = true;
        }
      } else if (direction == 3) {
        y++;
        if (y > max) {
          changeDirection = true;
        }
      }
      if (changeDirection) {
        changeDirection = false;
        if (direction == 0) {
          x--;
          y--;
        } else if (direction == 1) {
          x--;
          y++;
        } else if (direction == 2) {
          x++;
          y++;
        } else if (direction == 3) {
          x++;
          y--;
        }
        direction++;
        if (direction > 3) {
          direction = 0;
        }
      }
    } while (isOccupied(originalCoordinate + Vec2(x, y)));
  }
}
bool GameScene::isCoordinateValid(Vec2 coordinate) {
  if (coordinate.x < 0 || coordinate.y < 0 || coordinate.x >= mapSize.width ||
      coordinate.y >= mapSize.height) {
    return false;
  }
  return true;
}
EnemyBase *GameScene::getHeroBuildingAtMapPos(Vec2 mapPos) {
  EnemyBase *found = nullptr;
  int zOrder = -9999999;
  for (auto unit : heroArray) {
    if (!unit || unit == nullptr || !unit->isBuilding ||
        !unit->isBuildingComplete) {
      continue;
    }
    Vec2 buildingPos = getPositionFromTileCoordinate(
        unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y);
    float height = unit->buildingOccupySize.height * TILE_SIZE;
    cocos2d::Rect theRect =
        cocos2d::Rect(buildingPos.x, buildingPos.y - height,
                      unit->buildingOccupySize.width * TILE_SIZE, height);
    if (theRect.containsPoint(mapPos) && unit->getLocalZOrder() > zOrder) {
      found = unit;
      zOrder = unit->getLocalZOrder();
    }
  }
  return found;
}

// test now
EnemyBase *GameScene::getEnemyBuildingAtMapPos(Vec2 mapPos) {
  EnemyBase *found = nullptr;
  int zOrder = -9999999;
  for (auto unit : enemyArray) {
    if (!unit || unit == nullptr || !unit->isBuilding ||
        !unit->isBuildingComplete) {
      continue;
    }
    if (unit->unitType != UNIT_CASTLE && unit->unitType != UNIT_ORC_HQ) {
      continue;
    }
    Vec2 buildingPos = getPositionFromTileCoordinate(
        unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y);
    float height = unit->buildingOccupySize.height * TILE_SIZE;
    cocos2d::Rect theRect =
        cocos2d::Rect(buildingPos.x, buildingPos.y - height,
                      unit->buildingOccupySize.width * TILE_SIZE, height);
    if (theRect.containsPoint(mapPos) && unit->getLocalZOrder() > zOrder) {
      found = unit;
      zOrder = unit->getLocalZOrder();
    }
  }
  return found;
}

bool GameScene::tryDeliverCarriedResourcesAt(Vec2 mapPos) {
  EnemyBase *building = getHeroBuildingAtMapPos(mapPos);
  if (building == nullptr || selectedArray.size() == 0) {
    return false;
  }

  bool handled = false;
  for (auto unit : selectedArray) {
    if (!unit || unit == nullptr || !unit->isVisible()) {
      continue;
    }
    if (unit->alliSide == WHICH_SIDE_ENEMY) {
      continue;
    }
    if (unit->unitType != UNIT_WORKER && unit->unitType != UNIT_GOBLIN_WORKER) {
      continue;
    }
    if (unit->isCarryingTree && (building->unitType == UNIT_CASTLE ||
                                 building->unitType == UNIT_ORC_HQ ||
                                 building->unitType == UNIT_LUMBERMILL)) {
      unit->returningPlace = building;
      unit->moveToTarget(building);
      unit->unitAct = UNIT_ACT_GATHER_TREE;
      unit->isGatheringTree = true;
      if (isMultiplay) {
        MM->returnLumber(Value(unit->unitID).asString(), building->unitID);
      }
      handled = true;
    } else if (unit->isCarryingGold && (building->unitType == UNIT_CASTLE ||
                                        building->unitType == UNIT_ORC_HQ)) {
      unit->returningPlace = building;
      unit->unitAct = UNIT_ACT_GATHER_GOLD;
      unit->moveToTarget(building);
      unit->isGatheringGold = true;
      if (isMultiplay) {
        MM->returnGold(Value(unit->unitID).asString(), building->unitID);
      }
      handled = true;
    }
  }
  if (handled && HUD != nullptr) {
    HUD->dismissMenu();
  }
  return handled;
}

namespace {

bool hitTestWorldUiButton(cocos2d::ui::Button *btn, const Vec2 &glPos) {
  if (btn == nullptr || !btn->isVisible() || btn->getParent() == nullptr) {
    return false;
  }
  Node *parent = btn->getParent();
  Vec2 local = parent->convertToNodeSpace(glPos);
  Size size = btn->getContentSize();
  float sx = fabsf(btn->getScaleX());
  float sy = fabsf(btn->getScaleY());
  Vec2 ap = btn->getAnchorPoint();
  Rect box(btn->getPositionX() - size.width * ap.x * sx,
           btn->getPositionY() - size.height * ap.y * sy, size.width * sx,
           size.height * sy);
  return box.containsPoint(local);
}

} // namespace

bool GameScene::tryClickBuildProgressIcon(const Vec2 &glPos) {
  if (GM->currentStageIndex == STAGE_LOBBY) {
    return false;
  }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  if (getWorkerMenuButtonAtGlPos(glPos) != nullptr) {
    return false;
  }
#endif

  ui::Button *hitBtn = nullptr;
  int hitZOrder = -9999999;
  Movable *hitBuilder = nullptr;
  EnemyBase *hitBuilding = nullptr;
  bool hitIsBuildProgress = false;

  for (auto unit : heroArray) {
    Movable *movable = (Movable *)unit;
    if (movable->buildProgressBtn != nullptr &&
        movable->buildProgressBtn->getParent() == this &&
        hitTestWorldUiButton(movable->buildProgressBtn, glPos)) {
      int z = movable->buildProgressBtn->getLocalZOrder();
      if (z >= hitZOrder) {
        hitZOrder = z;
        hitBtn = movable->buildProgressBtn;
        hitBuilder = movable;
        hitBuilding = nullptr;
        hitIsBuildProgress = true;
      }
    }
    if (unit->isBuilding && unit->isBuildingComplete) {
      for (auto btn : unit->btns) {
        if (btn == nullptr || btn->getParent() != this) {
          continue;
        }
        if (!hitTestWorldUiButton(btn, glPos)) {
          continue;
        }
        int z = btn->getLocalZOrder();
        if (z >= hitZOrder) {
          hitZOrder = z;
          hitBtn = btn;
          hitBuilder = nullptr;
          hitBuilding = unit;
          hitIsBuildProgress = false;
        }
      }
    }
  }

  if (hitBtn == nullptr) {
    return false;
  }
  if (hitIsBuildProgress && hitBuilder != nullptr) {
    hitBuilder->cancelBuildingConstruction(hitBtn);
  } else if (hitBuilding != nullptr) {
    hitBuilding->cancelProduct(hitBtn);
  }
  return true;
}

void GameScene::updateBuildingTemplateAtScreenPos(Vec2 glPos,
                                                   bool markAsUserMoved) {
  if (buildingTemplate == nullptr) {
    return;
  }
  float extraY = 0;
  Vec2 coordinate = getCoordinateFromPosition(
      (glPos + Vec2(-getPositionX() - buildingTemplateSize.width,
                    -getPositionY() - buildingTemplateSize.height + extraY)) /
          layerScale,
      theMap);

  if (markAsUserMoved) {
    isBuildingTemplateMoved = true;
  }
  Vec2 pos = this->getPositionFromTileCoordinate(coordinate.x, coordinate.y) -
             Vec2(TILE_SIZE / 2, TILE_SIZE / 2);
  if (pos.x < TILE_SIZE) {
    pos.x = TILE_SIZE;
  }
  if (pos.y < TILE_SIZE) {
    pos.y = TILE_SIZE;
  }
  if (pos.x >= (mapSize.width - buildingTemplateSize.width) * TILE_SIZE) {
    pos.x = (mapSize.width - buildingTemplateSize.width) * TILE_SIZE;
  }
  if (pos.y >= (mapSize.height - buildingTemplateSize.height) * TILE_SIZE) {
    pos.y = (mapSize.height - buildingTemplateSize.height) * TILE_SIZE;
  }
  buildingTemplate->setPosition(pos);
  checkBuildingTemplate();
}

void GameScene::confirmBuildingPlacement() {
  if (buildingTemplate == nullptr || GM->currentStageIndex == STAGE_LOBBY) {
    return;
  }
  if (!isBuildingTemplateMoved) {
    selectCommand(COMMAND_NOTHING);
    return;
  }
  if (isBuildingReadyToBuild && selectedArray.size() == 1 &&
      (selectedArray.at(0)->unitType == UNIT_WORKER ||
       selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER ||
       selectedArray.at(0)->unitType == UNIT_HUMAN_OIL_SHIP ||
       selectedArray.at(0)->unitType == UNIT_ORC_OIL_SHIP)) {
    if (isMultiplay) {
      HMsg msg;
      msg.msgType = MSG_CODE_CONSTRUCT;
      msg.sourceIDs = Value(selectedArray.at(0)->unitID).asString();
      msg.value0 = buildingTemplate->getTag();
      msg.value1 = buildingTemplateCoordinate.x;
      msg.value2 = buildingTemplateCoordinate.y;
      msg.startTime = gameFrameTimer + LATANCY;
      ownMsgList.push_back(msg);

      Movable *unit = selectedArray.at(0);
      MM->workerBuilding(unit->unitID, buildingTemplate->getTag(),
                         buildingTemplateCoordinate.x,
                         buildingTemplateCoordinate.y);
    } else {
      setWorkerToBuild(selectedArray.at(0), buildingTemplate->getTag(),
                       buildingTemplateCoordinate.x,
                       buildingTemplateCoordinate.y);
    }

    selectCommand(COMMAND_NOTHING);
    if (HUD != nullptr) {
      HUD->getChildByName("btnCancelBuilding")->removeFromParent();
    }
    if (buildingTemplate != nullptr) {
      buildingTemplate->removeFromParent();
      buildingTemplate = nullptr;
    }
  }
}

void GameScene::checkBuildingTemplate() {
  isBuildingReadyToBuild = true;
  buildingTemplateCoordinate =
      getCoordinateFromPosition(buildingTemplate->getPosition(), theMap);

  int btag = buildingTemplate->getTag();
  bool isShipyard = (btag == UNIT_HUMAN_SHIPYARD || btag == UNIT_ORC_SHIPYARD);
  bool isWaterBuilding = isShipyard ||
      btag == UNIT_HUMAN_OIL_EXTRACTOR || btag == UNIT_ORC_OIL_EXTRACTOR ||
      btag == UNIT_HUMAN_OIL_REFINERY  || btag == UNIT_ORC_OIL_REFINERY  ||
      btag == UNIT_HUMAN_FOUNDRY       || btag == UNIT_ORC_FOUNDRY;
  Vec2 pos;
  for (int j = 0; j < buildingTemplateSize.height; j++) {
    for (int i = 0; i < buildingTemplateSize.width; i++) {
      Sprite *spt = (Sprite *)buildingTemplate->getChildByTag(
          i + j * buildingTemplateSize.width);
      if (spt != nullptr) {
        pos = Vec2(buildingTemplateCoordinate.x + i,
                   buildingTemplateCoordinate.y - j - 1);

        bool isBlock;
        if (isWaterBuilding) {
          bool notWater = !isSoilBlock(getTileGIDAt(soilLayer, pos));
          bool decoBlocked = isDecoBlock(getTileGIDAt(decoLayer, pos));
          // OilExtractor: a deco block from a placed OilSpot is acceptable
          if ((btag == UNIT_HUMAN_OIL_EXTRACTOR || btag == UNIT_ORC_OIL_EXTRACTOR) && decoBlocked) {
            for (auto mutual : mutualArray) {
              if (mutual == nullptr || mutual->unitType != UNIT_OIL_SPOT) continue;
              Vec2 st = getCoordinateFromPosition(mutual->getPosition());
              if ((int)st.x == (int)pos.x && (int)st.y == (int)pos.y) {
                decoBlocked = false;
                break;
              }
            }
          }
          isBlock = notWater || decoBlocked;
        } else {
          isBlock = isDecoBlock(getTileGIDAt(decoLayer, pos)) ||
                    isSoilBlock(getTileGIDAt(soilLayer, pos));
        }
        spt->setColor(isBlock ? Color3B::RED : Color3B::GREEN);
        if (isBlock) {
          isBuildingReadyToBuild = false;
        }
      }
    }
  }

  // Shipyard, OilRefinery, and Foundry must border at least one land tile.
  // OilExtractor is exempt — it floats over an OilSpot in open water.
  bool needsAdjacentLand = isShipyard ||
      btag == UNIT_HUMAN_OIL_REFINERY || btag == UNIT_ORC_OIL_REFINERY ||
      btag == UNIT_HUMAN_FOUNDRY      || btag == UNIT_ORC_FOUNDRY;
  if (needsAdjacentLand && isBuildingReadyToBuild) {
    bool hasAdjacentLand = false;
    int cx = (int)buildingTemplateCoordinate.x;
    int cy = (int)buildingTemplateCoordinate.y;
    int w  = (int)buildingTemplateSize.width;
    int h  = (int)buildingTemplateSize.height;
    for (int i = cx - 1; i <= cx + w && !hasAdjacentLand; i++) {
      if (!isSoilBlock(getTileGIDAt(soilLayer, Vec2(i, cy))))     hasAdjacentLand = true;
      if (!isSoilBlock(getTileGIDAt(soilLayer, Vec2(i, cy-h-1)))) hasAdjacentLand = true;
    }
    for (int j = cy - 1; j >= cy - h && !hasAdjacentLand; j--) {
      if (!isSoilBlock(getTileGIDAt(soilLayer, Vec2(cx-1, j)))) hasAdjacentLand = true;
      if (!isSoilBlock(getTileGIDAt(soilLayer, Vec2(cx+w,  j)))) hasAdjacentLand = true;
    }
    if (!hasAdjacentLand) isBuildingReadyToBuild = false;
  }

  // OilExtractor must be placed on top of an OilSpot
  if (btag == UNIT_HUMAN_OIL_EXTRACTOR || btag == UNIT_ORC_OIL_EXTRACTOR) {
    int cx = (int)buildingTemplateCoordinate.x;
    int cy = (int)buildingTemplateCoordinate.y;
    int ew = (int)buildingTemplateSize.width;
    int eh = (int)buildingTemplateSize.height;
    bool hasOilSpot = false;
    for (auto mutual : mutualArray) {
      if (mutual == nullptr || mutual->unitType != UNIT_OIL_SPOT) continue;
      Vec2 st = getCoordinateFromPosition(mutual->getPosition());
      if ((int)st.x >= cx && (int)st.x < cx + ew &&
          (int)st.y >= cy - eh && (int)st.y < cy) {
        hasOilSpot = true;
        break;
      }
    }
    if (!hasOilSpot) {
      isBuildingReadyToBuild = false;
      for (int jj = 0; jj < eh; jj++) {
        for (int ii = 0; ii < ew; ii++) {
          Sprite *s = (Sprite *)buildingTemplate->getChildByTag(ii + jj * ew);
          if (s) s->setColor(Color3B::RED);
        }
      }
    }
  }
}
EnemyBase *GameScene::buildTheBuilding(int index, int x, int y, int width,
                                        int height, std::string spriteName,
                                        int whichSide, bool isFree) {
  bool isPlaceReady = true;
  bool isShipyard = (index == UNIT_HUMAN_SHIPYARD || index == UNIT_ORC_SHIPYARD);
  bool isWaterBuilding = isShipyard ||
      index == UNIT_HUMAN_OIL_EXTRACTOR || index == UNIT_ORC_OIL_EXTRACTOR ||
      index == UNIT_HUMAN_OIL_REFINERY  || index == UNIT_ORC_OIL_REFINERY  ||
      index == UNIT_HUMAN_FOUNDRY       || index == UNIT_ORC_FOUNDRY;
  Vec2 pos;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      pos = Vec2(x + i, y - j - 1);
      bool isBlock;
      if (isWaterBuilding) {
        isBlock = !isSoilBlock(getTileGIDAt(soilLayer, pos)) ||
                  isDecoBlock(getTileGIDAt(decoLayer, pos));
      } else {
        isBlock = isDecoBlock(getTileGIDAt(decoLayer, pos)) ||
                  isSoilBlock(getTileGIDAt(soilLayer, pos));
      }
      if (isBlock) {
        isPlaceReady = false;
      }
    }
  }
  int price = this->getGoldPriceForUnit(index);
  int lumberPrice = getLumberPriceForUnit(index);
  if (isFree) {
    price = 0;
    lumberPrice = 0;
  }
  if (price > gold || !isPlaceReady) {
    return nullptr;
  }
  if (lumberPrice > lumber) {
    return nullptr;
  }

  // OilExtractor must be placed on an OilSpot
  if (index == UNIT_HUMAN_OIL_EXTRACTOR || index == UNIT_ORC_OIL_EXTRACTOR) {
    bool hasOilSpot = false;
    for (auto mutual : mutualArray) {
      if (mutual == nullptr || mutual->unitType != UNIT_OIL_SPOT) continue;
      Vec2 st = getCoordinateFromPosition(mutual->getPosition());
      if ((int)st.x >= x && (int)st.x < x + width &&
          (int)st.y >= y - height && (int)st.y < y) {
        hasOilSpot = true;
        break;
      }
    }
    if (!hasOilSpot) return nullptr;
  }

  pos = getPositionFromTileCoordinate(x, y - height);
  blockSending = true;
  EnemyBase *unit =
      createUnit(index, whichSide, ITS_BUILDING,
                 pos + Vec2(width * TILE_SIZE / 2 - TILE_SIZE / 2,
                            -(height - 1) * TILE_SIZE + height * TILE_SIZE / 2 -
                                TILE_SIZE / 2),
                 spriteName.substr(0, spriteName.size() - 4), 1, spriteName);
  blockSending = false;
  unit->isBuildingComplete = false;
  setOccupy(pos, width, height, true, unit);
  // Worker-driven construction (Movable::moveNew) resets rotation to 0 right
  // after this call returns - the enemy AI's instant-build callers don't go
  // through that path, so leaving this at 180 left every AI-built building
  // upside-down.
  unit->setRotation(0);
  unit->retain();

  addGold(-price);
  addLumber(-lumberPrice);
  return unit;
}
void GameScene::addDecoToBuilding(Movable *unit) {
  if (unit->unitType == UNIT_FARM) {
    unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
    unit->smokePoint = unit->getPosition() + Vec2(-18, 110) * imageScale;
  } else if (unit->unitType == UNIT_AIRPORT) {
    Sprite *spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
    GM->runAnimation(spt, "propeller", true);
    unit->addChild(spt);
    spt->setPosition(Vec2(134, 206) * imageScale);
  } else if (unit->unitType == UNIT_CASTLE) {
    Sprite *spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
    GM->runAnimation(spt, "blueFlag", true);
    unit->addChild(spt);
    spt->setPosition(Vec2(192, 255) * imageScale);
  } else if (unit->unitType == UNIT_ORC_HQ) {
    Sprite *spt = Sprite::createWithSpriteFrameName("redFlag0.png");
    GM->runAnimation(spt, "redFlag", true);
    unit->addChild(spt);
    spt->setPosition(Vec2(232, 335) * imageScale);
  }
}
void GameScene::setAfterBuildingProcess(Movable *unit) {
  unit->stopAllActions();
  unit->setSpriteFrame(getSpriteNameForUnit(unit->unitType));
  unit->isBuildingComplete = true;

  addDecoToBuilding(unit);
  if (unit->unitType == UNIT_WATCHERTOWER ||
      unit->unitType == UNIT_ORC_BUNKER || unit->unitType == UNIT_ORC_HQ) {
    unit->canFindTarget = true;
  }
  if (unit->unitType == UNIT_ORC_HQ) {
    unit->unitType = UNIT_CASTLE;
  }
  // Note: a UNIT_ORC_HQ was just renamed to UNIT_CASTLE a few lines above, so it
  // is already covered by the UNIT_CASTLE case here; an explicit UNIT_ORC_HQ
  // check would be dead code. (Both AI and player orc HQs go through that
  // rename, so this list is behaviourally unchanged for either side.)
  if (unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_BARRACKS ||
      unit->unitType == UNIT_AIRPORT || unit->unitType == UNIT_FACTORY ||
      unit->unitType == UNIT_ORC_BARRACKS ||
      unit->unitType == UNIT_ORC_TROLL_HOUSE || unit->unitType == UNIT_TEMPLE ||
      unit->unitType == UNIT_HUMAN_SHIPYARD ||
      unit->unitType == UNIT_ORC_SHIPYARD ||
      unit->unitType == UNIT_HUMAN_FOUNDRY ||
      unit->unitType == UNIT_ORC_FOUNDRY) {
    unit->startProductSchedule();
  }
  updateFoodMaxState();
  resetPathState();
  updateMiniMapForNonMoving();
}
Sprite *GameScene::getSpriteForUnit(int index) {
  Sprite *spt = Sprite::createWithSpriteFrameName(getSpriteNameForUnit(index));
  spt->setScale(1 / imageScale);
  return spt;
}
bool GameScene::isHero(int index) {
  if (index >= UNIT_HERO_ORC) {
    return true;
  }
  return false;
}
std::string GameScene::getSpriteNameForUnit(int index) {
  if (index == UNIT_WORKER) {
    return "workerAxeStand0.png";
  } else if (index == UNIT_GOBLIN_WORKER) {
    return "goblinAxeStand0.png";
  } else if (index == UNIT_SWORDMAN) {
    return "swordmanStand0.png";
  } else if (index == UNIT_MINE) {
    return "mine.png";
  } else if (index == UNIT_TROLL) {
    return "trollStand0.png";
  } else if (index == UNIT_GOBLIN) {
    return "goblinStand0.png";
  } else if (index == UNIT_GOBLIN_BOMB) {
    return "goblinBombStand0.png";
  } else if (index == UNIT_ORC_SPEAR) {
    return "orcSpearStand0.png";
  } else if (index == UNIT_ORC_AXE) {
    return "orcAxeStand0.png";
  } else if (index == UNIT_ARCHER) {
    return "archerStand0.png";
  } else if (index == UNIT_HELICOPTER) {
    return "helicopter0.png";
  } else if (index == UNIT_CATAPULT) {
    return "catapult0.png";
  } else if (index == UNIT_ZOMBIE_ORC_AXE) {
    return "zombieOrc0.png";
  } else if (index == UNIT_ZOMBIE_SWORDSMAN) {
    return "zombieHuman0.png";
  } else if (index == UNIT_CASTLE || index == UNIT_ZOMBIE_CASTLE) {
    return "castle.png";
  } else if (index == UNIT_FARM) {
    return "farm.png";
  } else if (index == UNIT_BARRACKS) {
    return "barracks.png";
  } else if (index == UNIT_LUMBERMILL) {
    return "lumberMill.png";
  } else if (index == UNIT_WATCHERTOWER) {
    return "watcherTower.png";
  } else if (index == UNIT_ORC_BUNKER) {
    return "bunker.png";
  } else if (index == UNIT_ORC_HQ || index == UNIT_ZOMBIE_HQ) {
    return "hq.png";
  } else if (index == UNIT_ORC_BARRACKS) {
    return "axeport.png";
  } else if (index == UNIT_ORC_TROLL_HOUSE) {
    return "statueHouse.png";
  } else if (index == UNIT_TEMPLE) {
    return "alter.png";
  } else if (index == UNIT_BARBECUE) {
    return "barbecue.png";
  } else if (index == UNIT_AIRPORT) {
    return "airport.png";
  } else if (index == UNIT_FACTORY) {
    return "factory.png";
  } else if (index == UNIT_UNDERGROUND_BUNKER) {
    return "undergroundBunker.png";
  } else if (index == UNIT_TRIGGER) {
    return "trigger.png";
  } else if (index == UNIT_TREE_FOR_BATTLE) {
    return "tree3_0.png";
  } else if (index == UNIT_WIZARD) {
    return "wizardStand0.png";
  } else if (index == UNIT_HUMAN_SHUTTLE) {
    return "humanShuttle.png";
  } else if (index == UNIT_ORC_SHUTTLE) {
    return "orcShuttle.png";
  } else if (index == UNIT_HUMAN_SHIP) {
    return "humanShip.png";
  } else if (index == UNIT_ORC_SHIP) {
    return "orcShip.png";
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return "humanBattleShip.png";
  } else if (index == UNIT_HUMAN_SHIPYARD) {
    return "humanShipyard.png";
  } else if (index == UNIT_ORC_SHIPYARD) {
    return "orcShipyard.png";
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    return "orcBattleShip.png";
  } else if (index == UNIT_OIL_SPOT) {
    return "oilSpot.png";
  } else if (index == UNIT_HUMAN_OIL_SHIP) {
    return "humanOilShip.png";
  } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
    return "humanOilExtractor.png";
  } else if (index == UNIT_HUMAN_FOUNDRY) {
    return "humanFoundary.png";
  } else if (index == UNIT_HUMAN_OIL_REFINERY) {
    return "humanOilRefinery.png";
  } else if (index == UNIT_ORC_OIL_SHIP) {
    return "orcOilShip.png";
  } else if (index == UNIT_ORC_OIL_EXTRACTOR) {
    return "orcOilExtractor.png";
  } else if (index == UNIT_ORC_OIL_REFINERY) {
    return "orcOilRefinery.png";
  } else if (index == UNIT_ORC_FOUNDRY) {
    return "orcFoundary.png";
  } else if (index == UNIT_TREE) {
    return "tree2_0.png";
  }
  return "workerAxeStand0.png";
}

Sprite *GameScene::getSpriteForIcon(int index) {
  Sprite *sptBuilding;

  if (index == UNIT_WORKER) {
    sptBuilding = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
  } else if (index == UNIT_GOBLIN_WORKER) {
    sptBuilding = Sprite::createWithSpriteFrameName("goblinAxeStand0.png");
  } else if (index == UNIT_SWORDMAN) {
    sptBuilding = Sprite::createWithSpriteFrameName("swordmanStand0.png");
  } else if (index == UNIT_ARCHER) {
    sptBuilding = Sprite::createWithSpriteFrameName("archerStand0.png");
  } else if (index == UNIT_HELICOPTER) {
    sptBuilding = Sprite::createWithSpriteFrameName("helicopter0.png");
  } else if (index == UNIT_CATAPULT) {
    sptBuilding = Sprite::createWithSpriteFrameName("catapult0.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x + 20 * imageScale,
                      sptBuilding->getTextureRect().origin.y, 160 * imageScale,
                      sptBuilding->getContentSize().height));
  } else if (index == UNIT_CASTLE) {
    sptBuilding = Sprite::createWithSpriteFrameName("castle.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y + 50 * imageScale,
                      230 * imageScale, 210 * imageScale));
  } else if (index == UNIT_FARM) {
    sptBuilding = Sprite::createWithSpriteFrameName("farm.png");
    sptBuilding->setTextureRect(cocos2d::Rect(
        sptBuilding->getTextureRect().origin.x,
        sptBuilding->getTextureRect().origin.y + 50 * imageScale,
        230 * imageScale, sptBuilding->getContentSize().height * imageScale));
  } else if (index == UNIT_BARRACKS) {
    sptBuilding = Sprite::createWithSpriteFrameName("barracks.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y, 230 * imageScale,
                      sptBuilding->getContentSize().height));
  } else if (index == UNIT_LUMBERMILL) {
    sptBuilding = Sprite::createWithSpriteFrameName("lumberMill.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x + 0,
                      sptBuilding->getTextureRect().origin.y, 230 * imageScale,
                      sptBuilding->getContentSize().height));
  } else if (index == UNIT_WATCHERTOWER) {
    sptBuilding = Sprite::createWithSpriteFrameName("watcherTower.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y,
                      sptBuilding->getContentSize().width, 210 * imageScale));
  } else if (index == UNIT_FACTORY) {
    sptBuilding = Sprite::createWithSpriteFrameName("factory.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y, 230 * imageScale,
                      sptBuilding->getContentSize().height));
  } else if (index == UNIT_AIRPORT) {
    sptBuilding = Sprite::createWithSpriteFrameName("airport.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y,
                      sptBuilding->getContentSize().width, 200 * imageScale));
    Sprite *spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
    sptBuilding->addChild(spt);
    spt->setPosition(Vec2(134, 176) * imageScale);
  } else if (index == UNIT_ORC_HQ) {
    sptBuilding = Sprite::createWithSpriteFrameName("hq.png");
    sptBuilding->setTextureRect(
        cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
                      sptBuilding->getTextureRect().origin.y + 50 * imageScale,
                      230 * imageScale, 210 * imageScale));
  } else if (index == UNIT_ORC_BUNKER) {
    sptBuilding = Sprite::createWithSpriteFrameName("bunker.png");
    //        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
    //        sptBuilding->getTextureRect().origin.y + 50, 230, 210));
  } else if (index == UNIT_ORC_BARRACKS) {
    sptBuilding = Sprite::createWithSpriteFrameName("axeport.png");
    //        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
    //        sptBuilding->getTextureRect().origin.y + 50, 230, 210));
  } else if (index == UNIT_ORC_TROLL_HOUSE) {
    sptBuilding = Sprite::createWithSpriteFrameName("statueHouse.png");
    //        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x,
    //        sptBuilding->getTextureRect().origin.y + 50, 230, 210));
  } else if (index == UNIT_TEMPLE) {
    sptBuilding = Sprite::createWithSpriteFrameName("alter.png");
    sptBuilding->setScale(0.9f);
  } else if (index == UNIT_BARBECUE) {
    sptBuilding = Sprite::createWithSpriteFrameName("barbecue.png");
  } else if (index == UNIT_GOBLIN) {
    sptBuilding = Sprite::createWithSpriteFrameName("goblinStand0.png");
  } else if (index == UNIT_GOBLIN_BOMB) {
    sptBuilding = Sprite::createWithSpriteFrameName("goblinBombStand0.png");
  } else if (index == UNIT_ORC_AXE) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
  } else if (index == UNIT_ORC_SPEAR) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcSpearStand0.png");
  } else if (index == UNIT_TROLL) {
    sptBuilding = Sprite::createWithSpriteFrameName("trollStand0.png");
    sptBuilding->setScale(0.8f);
  } else if (index == UNIT_WIZARD) {
    sptBuilding = Sprite::createWithSpriteFrameName("wizardStand0.png");
  } else if (index == UNIT_HUMAN_SHUTTLE) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanShuttle.png");
  } else if (index == UNIT_ORC_SHUTTLE) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcShuttle.png");
  } else if (index == UNIT_HUMAN_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanShip.png");
  } else if (index == UNIT_ORC_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcShip.png");
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanBattleShip.png");
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcBattleShip.png");
  } else if (index == UNIT_HUMAN_SHIPYARD) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanShipyard.png");
  } else if (index == UNIT_ORC_SHIPYARD) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcShipyard.png");
  } else if (index == UNIT_OIL_SPOT) {
    sptBuilding = Sprite::createWithSpriteFrameName("oilSpot.png");
  } else if (index == UNIT_HUMAN_OIL_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanOilShip.png");
  } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanOilExtractor.png");
  } else if (index == UNIT_HUMAN_FOUNDRY) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanFoundary.png");
  } else if (index == UNIT_HUMAN_OIL_REFINERY) {
    sptBuilding = Sprite::createWithSpriteFrameName("humanOilRefinery.png");
  } else if (index == UNIT_ORC_OIL_SHIP) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcOilShip.png");
  } else if (index == UNIT_ORC_OIL_EXTRACTOR) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcOilExtractor.png");
  } else if (index == UNIT_ORC_OIL_REFINERY) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcOilRefinery.png");
  } else if (index == UNIT_ORC_FOUNDRY) {
    sptBuilding = Sprite::createWithSpriteFrameName("orcFoundary.png");
  } else {
    sptBuilding = Sprite::create("hh.png");
  }
  sptBuilding->setScale(2);
  return sptBuilding;
}
bool GameScene::isThisSpotAvailable(Movable *me) {
  for (auto unit : heroArray) {
    if (unit != me && unit->getPosition() == me->getPosition()) {
      return false;
    }
  }
  for (auto unit : enemyArray) {
    if (unit != me && unit->getPosition() == me->getPosition()) {
      return false;
    }
  }
  Vec2 tileCoordinate = getCoordinateFromPosition(me->getPosition());

  return isThisSpotAvailable(tileCoordinate);
}
bool GameScene::isThisSpotAvailable(cocos2d::Vec2 tileCoordinate) {
  if (!isCoordinateValid(tileCoordinate) ||
      isDecoBlock(getTileGIDAt(decoLayer, tileCoordinate)) ||
      isSoilBlock(getTileGIDAt(soilLayer, tileCoordinate))) {
    return false;
  }
  return true;
}
void GameScene::tryCreateUnit(int index) {
  int goldPrice = getGoldPriceForUnit(index);
  int lumberPrice = getLumberPriceForUnit(index);
  int oilPrice = getOilPriceForUnit(index);
  if (gold < goldPrice) {
    HUD->showPriceInfo(LM->getText("not enough gold"), goldPrice, lumberPrice,
                       GM->getFoodUseForUnit(index));
    GM->playSoundEffect(SOUND_NAGATIVE);
  } else if (lumber < lumberPrice) {
    HUD->showPriceInfo(LM->getText("not enough lumber"), goldPrice, lumberPrice,
                       GM->getFoodUseForUnit(index));
    GM->playSoundEffect(SOUND_NAGATIVE);
  } else if (oilPrice > 0 && oil < oilPrice) {
    HUD->showPriceInfo(LM->getText("not enough oil"), goldPrice, lumberPrice,
                       GM->getFoodUseForUnit(index));
    GM->playSoundEffect(SOUND_NAGATIVE);
  } else if (foodInUse + GM->getFoodUseForUnit(index) > foodMax) {
    HUD->showPriceInfo(LM->getText("not enough food"), goldPrice, lumberPrice,
                       GM->getFoodUseForUnit(index));
    GM->playSoundEffect(SOUND_NAGATIVE);
  } else {
    if (selectedArray.at(0)->btns.size() < 5) {
      addGold(-goldPrice);
      addLumber(-lumberPrice);
      addOil(-oilPrice);
      selectedArray.at(0)->queueUnit(index);
    }
  }
}

void GameScene::setPriceInfo(int btnType) {
  int unitIndex = getUnitIndexFromBtnMenuType(btnType);
  int goldPrice = getGoldPriceForUnit(unitIndex);
  int lumberPrice = getLumberPriceForUnit(unitIndex);

  if (btnType == BTN_TYPE_UPGRADE_ATTACK || btnType == BTN_TYPE_UPGRADE_DEFENSE) {
    bool isOrc = !selectedArray.empty() && selectedArray.at(0)->unitType == UNIT_ORC_FOUNDRY;
    int currentLevel = 0;
    if (btnType == BTN_TYPE_UPGRADE_ATTACK)
      currentLevel = isOrc ? orcAttackLevel : humanAttackLevel;
    else
      currentLevel = isOrc ? orcDefenseLevel : humanDefenseLevel;
    goldPrice = (currentLevel == 0) ? 500 : 1000;
    lumberPrice = (currentLevel == 0) ? 250 : 500;
  }
  Node *ndInfo = HUD->rightBottomPanelForCampaign->getChildByName("ndInfo");
  Text *lblGold = (Text *)ndInfo->getChildByName("lblGold");
  Text *lblTree = (Text *)ndInfo->getChildByName("lblTree");
  lblGold->setString(Value(goldPrice).asString());
  lblTree->setString(Value(lumberPrice).asString());

  Node *imgBack = ndInfo->getChildByName("imgIconBack");
  imgBack->removeChildByName("sptIcon");

  Sprite *spt = getSpriteForIcon(unitIndex);
  spt->setName("sptIcon");
  imgBack->addChild(spt);
  spt->setPosition(imgBack->getContentSize() / 2);

  Text *lbl = (Text *)ndInfo->getChildByName("lblName");
  LM->setLocalizedString(lbl, GM->getUnitName(unitIndex).c_str());
  ScrollView *sv = (ScrollView *)ndInfo->getChildByName("sv");
  Label *lblDescription = (Label *)sv->getChildByName("lblDescription");
  LM->setLocalizedString(lblDescription, GM->getUnitName(unitIndex) + " desc");
  if (sv->getContentSize().height > lblDescription->getContentSize().height) {
    sv->setInnerContainerSize(sv->getContentSize());
  } else {
    sv->setInnerContainerSize(cocos2d::Size(
        sv->getContentSize().width, lblDescription->getContentSize().height));
  }
  lblDescription->setPositionY(sv->getInnerContainerSize().height);
}
int GameScene::getUnitIndexFromBtnMenuType(int btnType) {
  if (btnType == BTN_TYPE_CASTLE) {
    return UNIT_CASTLE;
  } else if (btnType == BTN_TYPE_FARM) {
    return UNIT_FARM;
  } else if (btnType == BTN_TYPE_BARRACKS) {
    return UNIT_BARRACKS;
  } else if (btnType == BTN_TYPE_WATCHER_TOWER) {
    return UNIT_WATCHERTOWER;
  } else if (btnType == BTN_TYPE_LUMBER) {
    return UNIT_LUMBERMILL;
  } else if (btnType == BTN_TYPE_FACTORY) {
    return UNIT_FACTORY;
  } else if (btnType == BTN_TYPE_AIRPORT) {
    return UNIT_AIRPORT;
  } else if (btnType == BTN_TYPE_WORKER) {
    return UNIT_WORKER;
  } else if (btnType == BTN_TYPE_GOBLIN_WORKER) {
    return UNIT_GOBLIN_WORKER;
  } else if (btnType == BTN_TYPE_SWORDMAN) {
    return UNIT_SWORDMAN;
  } else if (btnType == BTN_TYPE_ARCHER) {
    return UNIT_ARCHER;
  } else if (btnType == BTN_TYPE_HELICOPTER) {
    return UNIT_HELICOPTER;
  } else if (btnType == BTN_TYPE_CATAPULT) {
    return UNIT_CATAPULT;
  } else if (btnType == BTN_TYPE_HQ) {
    return UNIT_ORC_HQ;
  } else if (btnType == BTN_TYPE_BUNKER) {
    return UNIT_ORC_BUNKER;
  } else if (btnType == BTN_TYPE_ORC_BARRACKS) {
    return UNIT_ORC_BARRACKS;
  } else if (btnType == BTN_TYPE_TROLL_HOUSE) {
    return UNIT_ORC_TROLL_HOUSE;
  } else if (btnType == BTN_TYPE_TEMPLE) {
    return UNIT_TEMPLE;
  } else if (btnType == BTN_TYPE_GOBLIN) {
    return UNIT_GOBLIN;
  } else if (btnType == BTN_TYPE_GOBLIN_BOMB) {
    return UNIT_GOBLIN_BOMB;
  } else if (btnType == BTN_TYPE_ORC_AXE) {
    return UNIT_ORC_AXE;
  } else if (btnType == BTN_TYPE_ORC_SPEAR) {
    return UNIT_ORC_SPEAR;
  } else if (btnType == BTN_TYPE_TROLL) {
    return UNIT_TROLL;
  } else if (btnType == BTN_TYPE_MAGE) {
    return UNIT_WIZARD;
  } else if (btnType == BTN_TYPE_BARBECUE) {
    return UNIT_BARBECUE;
  } else if (btnType == BTN_TYPE_SHIPYARD) {
    return UNIT_HUMAN_SHIPYARD;
  } else if (btnType == BTN_TYPE_HUMAN_SHUTTLE) {
    return UNIT_HUMAN_SHUTTLE;
  } else if (btnType == BTN_TYPE_HUMAN_SHIP) {
    return UNIT_HUMAN_SHIP;
  } else if (btnType == BTN_TYPE_HUMAN_BATTLE_SHIP) {
    return UNIT_HUMAN_BATTLE_SHIP;
  } else if (btnType == BTN_TYPE_ORC_SHIPYARD) {
    return UNIT_ORC_SHIPYARD;
  } else if (btnType == BTN_TYPE_ORC_SHUTTLE) {
    return UNIT_ORC_SHUTTLE;
  } else if (btnType == BTN_TYPE_ORC_SHIP) {
    return UNIT_ORC_SHIP;
  } else if (btnType == BTN_TYPE_ORC_BATTLE_SHIP) {
    return UNIT_ORC_BATTLE_SHIP;
  } else if (btnType == BTN_TYPE_OIL_SHIP) {
    return UNIT_HUMAN_OIL_SHIP;
  } else if (btnType == BTN_TYPE_OIL_EXTRACTOR) {
    return UNIT_HUMAN_OIL_EXTRACTOR;
  } else if (btnType == BTN_TYPE_ORC_OIL_SHIP) {
    return UNIT_ORC_OIL_SHIP;
  } else if (btnType == BTN_TYPE_ORC_OIL_EXTRACTOR) {
    return UNIT_ORC_OIL_EXTRACTOR;
  } else if (btnType == BTN_TYPE_ORC_OIL_REFINERY) {
    return UNIT_ORC_OIL_REFINERY;
  } else if (btnType == BTN_TYPE_ORC_FOUNDRY) {
    return UNIT_ORC_FOUNDRY;
  } else if (btnType == BTN_TYPE_UPGRADE_ATTACK || btnType == BTN_TYPE_UPGRADE_DEFENSE) {
    return UNIT_HUMAN_FOUNDRY;
  }
  return -1;
}
bool GameScene::tryBuilding(int index) {
  int goldPrice = getGoldPriceForUnit(index);
  int lumberPrice = getLumberPriceForUnit(index);
  if (gold < goldPrice) {
    //        HUD->showInstanceMessage(LM->getText("not enough gold"));
    HUD->showPriceInfo(LM->getText("not enough gold"), goldPrice, lumberPrice,
                       0);
    GM->playSoundEffect(SOUND_NAGATIVE);
    return false;
  } else if (lumber < lumberPrice) {
    //        HUD->showInstanceMessage(LM->getText("not enough lumber"));
    HUD->showPriceInfo(LM->getText("not enough lumber"), goldPrice, lumberPrice,
                       0);
    GM->playSoundEffect(SOUND_NAGATIVE);
    return false;
  } else {
    if (index == UNIT_FACTORY || index == UNIT_AIRPORT) {
      bool exist = false;
      for (auto unit : heroArray) {
        if (unit->unitType == UNIT_LUMBERMILL && unit->isBuildingComplete) {
          exist = true;
          break;
        }
      }
      if (!exist) {
        PPLabel *lbl =
            HUD->showInstanceMessage(LM->getText("build lumbermill first"));
        lbl->setWordColor("lumbermill", Color3B(245, 213, 71));
        GM->playSoundEffect(SOUND_NAGATIVE);
        return false;
      }
    }
    if (index == UNIT_TEMPLE || index == UNIT_ORC_TROLL_HOUSE) {
      bool exist = false;
      for (auto unit : heroArray) {
        if (unit->unitType == UNIT_ORC_BARRACKS) {
          exist = true;
          break;
        }
      }
      if (!exist) {
        PPLabel *lbl =
            HUD->showInstanceMessage(LM->getText("build orc barracks first"));
        lbl->setWordColor("weapon house", Color3B(245, 213, 71));
        GM->playSoundEffect(SOUND_NAGATIVE);
        return false;
      }
    }

    if (index == UNIT_CASTLE) {
      createBuildingTemplate(index, "castle.png");
    } else if (index == UNIT_FARM) {
      createBuildingTemplate(UNIT_FARM, "farm.png");
    } else if (index == UNIT_WATCHERTOWER) {
      createBuildingTemplate(UNIT_WATCHERTOWER, "watcherTower.png");
    } else if (index == UNIT_LUMBERMILL) {
      createBuildingTemplate(UNIT_LUMBERMILL, "lumberMill.png");
    } else if (index == UNIT_BARRACKS) {
      createBuildingTemplate(UNIT_BARRACKS, "barracks.png");
    } else if (index == UNIT_FACTORY) {
      createBuildingTemplate(UNIT_FACTORY, "factory.png");
    } else if (index == UNIT_AIRPORT) {
      createBuildingTemplate(UNIT_AIRPORT, "airport.png");
    } else if (index == UNIT_ORC_HQ) {
      createBuildingTemplate(UNIT_ORC_HQ, "hq.png");
    } else if (index == UNIT_ORC_BUNKER) {
      createBuildingTemplate(UNIT_ORC_BUNKER, "bunker.png");
    } else if (index == UNIT_ORC_BARRACKS) {
      createBuildingTemplate(UNIT_ORC_BARRACKS, "axeport.png");
    } else if (index == UNIT_ORC_TROLL_HOUSE) {
      createBuildingTemplate(UNIT_ORC_TROLL_HOUSE, "statueHouse.png");
    } else if (index == UNIT_TEMPLE) {
      createBuildingTemplate(UNIT_TEMPLE, "alter.png");
    } else if (index == UNIT_BARBECUE) {
      createBuildingTemplate(UNIT_BARBECUE, "barbecue.png");
    } else if (index == UNIT_HUMAN_SHIPYARD) {
      createBuildingTemplate(UNIT_HUMAN_SHIPYARD, "humanShipyard.png");
    } else if (index == UNIT_ORC_SHIPYARD) {
      createBuildingTemplate(UNIT_ORC_SHIPYARD, "orcShipyard.png");
    } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
      createBuildingTemplate(UNIT_HUMAN_OIL_EXTRACTOR, "humanOilExtractor.png");
    } else if (index == UNIT_HUMAN_FOUNDRY) {
      createBuildingTemplate(UNIT_HUMAN_FOUNDRY, "humanFoundary.png");
    } else if (index == UNIT_HUMAN_OIL_REFINERY) {
      createBuildingTemplate(UNIT_HUMAN_OIL_REFINERY, "humanOilRefinery.png");
    } else if (index == UNIT_ORC_OIL_EXTRACTOR) {
      createBuildingTemplate(UNIT_ORC_OIL_EXTRACTOR, "orcOilExtractor.png");
    } else if (index == UNIT_ORC_OIL_REFINERY) {
      createBuildingTemplate(UNIT_ORC_OIL_REFINERY, "orcOilRefinery.png");
    } else if (index == UNIT_ORC_FOUNDRY) {
      createBuildingTemplate(UNIT_ORC_FOUNDRY, "orcFoundary.png");
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    if (usesPcRtsControls()) {
      isBuildingTemplateMoved = false;
      updateBuildingTemplateAtScreenPos(
          getWin32MouseWorldPosFromView(win32LastMouseViewPos), false);
    }
#endif
  }
  return true;
}

void GameScene::createBuildingTemplate(int index, std::string spriteName) {
  isBuildingTemplateMoved = false;

  HUD->showCancelBuildingButton();
  HUD->rightBottomPanelForCampaign->getChildByName("ndMission")
      ->setVisible(true);
  HUD->rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(
      false);

  buildingTemplate = Node::create();
  this->addChild(buildingTemplate, 10);
  buildingTemplate->setTag(index);
  Vec2 pos = Vec2(-this->getPosition().x + size.width / 2,
                  -this->getPosition().y + size.height / 2) *
             imageScale; // viewRect.origin + size/2;
  buildingTemplate->setPosition(
      Vec2(pos.x - (int)(pos.x) % 100, pos.y - (int)(pos.y) % 100 - 1));
  log("x: %f, y: %f", pos.x, pos.y);
  buildingTemplate->setName(spriteName);
  Sprite *spt = Sprite::createWithSpriteFrameName(spriteName);
  buildingTemplate->addChild(spt, 1);
  Vec2 buildingSize = GM->getBuildingOccupySize(index);
  int width = buildingSize.x;
  int height = buildingSize.y;
  spt->setPosition(Vec2(width * TILE_SIZE / 2, height * TILE_SIZE / 2));
  spt->setOpacity(150);
  buildingTemplateSize = cocos2d::Size(width, height);
  buildingTemplate->setContentSize(buildingTemplateSize);
  for (int i = 0; i < width * height; i++) {
    spt = Sprite::createWithSpriteFrameName("whiteRect.png");
    buildingTemplate->addChild(spt);
    spt->setAnchorPoint(Vec2::ZERO);
    spt->setPosition(Vec2((i % width) * TILE_SIZE, (i / width) * TILE_SIZE));
    spt->setTag(i);
    spt->setOpacity(150);
  }
  checkBuildingTemplate();
}
void GameScene::onMoveClick() {
  if (selectedCommand == COMMAND_BUILD) {

  } else if (selectedCommand == COMMAND_BUILD_BETTER) {
    int goldPrice = getGoldPriceForUnit(UNIT_AIRPORT);
    int lumberPrice = getLumberPriceForUnit(UNIT_AIRPORT);
    if (gold < goldPrice) {
      HUD->showInstanceMessage(LM->getText("not enough gold"));
      GM->playSoundEffect(SOUND_NAGATIVE);
    }
    if (lumber < lumberPrice) {
      HUD->showInstanceMessage(LM->getText("not enough lumber"));
      GM->playSoundEffect(SOUND_NAGATIVE);
    } else {
      createBuildingTemplate(UNIT_AIRPORT, "airport.png");
    }
  } else {
    selectCommand(COMMAND_MOVE);
  }
}
void GameScene::onStopClick() {
  if (selectedCommand == COMMAND_BUILD) {

  } else {
    selectCommand(COMMAND_STOP);
    stop(selectedArray);
    selectCommand(COMMAND_NOTHING);
  }
}
void GameScene::onAttackClick() {
  if (selectedCommand == COMMAND_BUILD) {
    tryBuilding(UNIT_BARRACKS);
  } else {
    selectCommand(COMMAND_ATTACK);
  }
}
void GameScene::onGatherClick() {
  if (selectedCommand == COMMAND_BUILD) {

  } else {

    selectCommand(COMMAND_GATHER);

    long minTreeDistance = 100000000;
    long minMineDistance = 2000000;
    long distance = 0;
    EnemyBase *nearestTree = nullptr;
    EnemyBase *nearestMine = nullptr;
    for (auto unit : mutualArray) {
      distance = unit->getPosition().distanceSquared(
          selectedArray.at(0)->getPosition());
      if (unit->unitType == UNIT_TREE) {
        if (distance < minTreeDistance) {
          minTreeDistance = distance;
          nearestTree = unit;
        }
      }
      if (unit->unitType == UNIT_MINE) {
        if (distance < minMineDistance) {
          minMineDistance = distance;
          nearestMine = unit;
        }
      }
    }
    if (nearestMine != nullptr) {
      for (auto unit : selectedArray) {
        unit->gatherGold(nearestMine);
      }
      if (HUD != nullptr) {
        HUD->dismissMenu();
      }
    } else if (nearestTree != nullptr) {
      //            for(auto unit:selectedArray){
      //                unit->gatherTree(getNearestTree(unit->getPosition()));
      //            }
    } else {
      HUD->showInstanceMessage(LM->getText("No resources are found."));
    }
  }
}
void GameScene::onBuildingClick() {
  if (selectedArray.size() == 0 || selectedArray.at(0) == 0 ||
      (selectedArray.at(0)->unitType != UNIT_WORKER &&
       selectedArray.at(0)->unitType != UNIT_GOBLIN_WORKER))
    return;
  if (selectedCommand == COMMAND_BUILD) {

  } else {
    selectCommand(COMMAND_BUILD);
    if (selectedArray.at(0)->unitType == UNIT_WORKER) {
      HUD->setMenu(0, BTN_TYPE_CASTLE);
      HUD->setMenu(1, BTN_TYPE_FARM);
      HUD->setMenu(2, BTN_TYPE_BARRACKS);
      HUD->setMenu(3, BTN_TYPE_LUMBER);
      HUD->setMenu(4, BTN_TYPE_WATCHER_TOWER);
    } else if (selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER) {
      HUD->setMenu(0, BTN_TYPE_HQ);
      HUD->setMenu(1, BTN_TYPE_BARBECUE);
      HUD->setMenu(2, BTN_TYPE_BUNKER);
      HUD->setMenu(3, BTN_TYPE_ORC_BARRACKS);
      HUD->setMenu(4, BTN_TYPE_NONE);
    }
    HUD->setMenu(5, BTN_TYPE_CANCEL);
    HUD->arrangeMenu(selectedArray.at(0)->getPosition());
  }
}
void GameScene::onBuildingBetterClick() {
  if (selectedArray.size() == 0 ||
      (selectedArray.at(0)->unitType != UNIT_WORKER &&
       selectedArray.at(0)->unitType != UNIT_GOBLIN_WORKER))
    return;
  if (selectedCommand == COMMAND_BUILD ||
      selectedCommand == COMMAND_BUILD_BETTER) {

  } else {
    selectCommand(COMMAND_BUILD_BETTER);
    if (selectedArray.at(0)->unitType == UNIT_WORKER) {
      HUD->setMenu(0, BTN_TYPE_AIRPORT);
      HUD->setMenu(1, BTN_TYPE_FACTORY);
      HUD->setMenu(2, BTN_TYPE_SHIPYARD);
      HUD->setMenu(3, BTN_TYPE_FOUNDRY);
      HUD->setMenu(4, BTN_TYPE_OIL_REFINERY);
    } else if (selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER) {
      HUD->setMenu(0, BTN_TYPE_TROLL_HOUSE);
      HUD->setMenu(1, BTN_TYPE_TEMPLE);
      HUD->setMenu(2, BTN_TYPE_ORC_SHIPYARD);
      HUD->setMenu(3, BTN_TYPE_ORC_OIL_REFINERY);
      HUD->setMenu(4, BTN_TYPE_ORC_FOUNDRY);
    }

    HUD->setMenu(5, BTN_TYPE_CANCEL);
    HUD->arrangeMenu(selectedArray.at(0)->getPosition());
  }
}

void GameScene::onSelectOnScreenClick() {}
void GameScene::onSelectOnMapClick() {}
void GameScene::selectCommand(int command) {
  selectedCommand = command;
  if (command == COMMAND_NOTHING) {
    if (HUD) {
      if (GM->currentStageIndex == STAGE_RAID ||
          GM->currentStageIndex == STAGE_SINGLEPLAY) {
        HUD->rightBottomPanelForCampaign->getChildByName("ndMission")
            ->setVisible(false);
        HUD->rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(
            true);
      } else {
        HUD->rightBottomPanelForCampaign->getChildByName("ndMission")
            ->setVisible(true);
        HUD->rightBottomPanelForCampaign->getChildByName("ndInfo")->setVisible(
            false);
      }
    }
  }
  //    if(buildingTemplate != nullptr){
  //        buildingTemplate->removeFromParent();
  //        buildingTemplate = nullptr;
  //        HUD->removeChildByName("btnCancelBuilding");
  //    }

  //    if(command == COMMAND_NOTHING){
  //        HUD->lblCommand->setString("CMD: nothing");
  //    }else if(command == COMMAND_MOVE){
  //        HUD->lblCommand->setString("CMD: move");
  //    }else if(command == COMMAND_STOP){
  //        HUD->lblCommand->setString("CMD: stop");
  //    }else if(command == COMMAND_ATTACK){
  //        HUD->lblCommand->setString("CMD: attack");
  //    }else if(command == COMMAND_GATHER){
  //        HUD->lblCommand->setString("CMD: gather");
  //    }else if(command == COMMAND_BUILD){
  //        HUD->lblCommand->setString("CMD: build");
  //    }else if(command == COMMAND_BUILD){
  //        HUD->lblCommand->setString("CMD: build better");
  //    }
}
bool GameScene::isBlockExistBetween(Vec2 start, Vec2 end) {
  float width = end.x - start.x;
  float height = end.y - start.y;
  if (width == 0 && height == 0) {
    return false;
  }
  float xGap = end.x - start.x;
  float yGap = end.y - start.y;
  float theta = atan2(yGap, xGap);
  float gapX = cos(theta) * 50;
  float gapY = sin(theta) * 50;
  float accumulatedX = 0;
  float accumulatedY = 0;
  Vec2 pos;
  while (true) {
    accumulatedX += gapX;
    accumulatedY += gapY;
    if (width < 0) {
      if (accumulatedX < width) {
        break;
      }
    } else {
      if (accumulatedX > width) {
        break;
      }
    }
    if (height < 0) {
      if (accumulatedY < height) {
        break;
      }
    } else {
      if (accumulatedY > height) {
        break;
      }
    }

    pos = getCoordinateFromPosition(
        Vec2(start.x + accumulatedX, start.y + accumulatedY), theMap);
    if (GM->tileState[(int)pos.x][(int)pos.y] > 0) {
      return true;
    }
  }

  return false;
}
void GameScene::moveTo(EnemyBase *unit, Vec2 pos) {
  if (isNewCommandSystem) {
    unit->moveFlagPos = pos;
    unit->moveToPos = Vec2::ZERO;
    unit->isGatheringTree = false;
    unit->isGatheringGold = false;
    unit->isGoingToBuild = false;
  } else {
    targetHand->setPosition(pos);
    unit->moveToTarget(targetHand);
    unit->canFindTarget = false;
    unit->attackFlagPos = Vec2::ZERO;
  }
}

void GameScene::moveMultiplayEnemyTo(Vector<EnemyBase *> troop, Vec2 pos) {
  //    bool isHelicopter = false;
  //    std::string sourceIDs = "";
  log("move multiplay enemy to %d", (int)troop.size());
  for (auto unit : troop) {
    unit->moveToPos = Vec2::ZERO;
    unit->moveFlagPos = pos;
    unit->target = nullptr;
    unit->unitAct = UNIT_ACT_MOVE;
    unit->canRevengeAttack = false;
    unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
    unit->cancelAttackSchedule();
    log("unit id: %d", unit->unitID);
    if (unit->isInMine) {
      unit->isGatheringGold = false;

      unit->setVisible(true);
      unit->isInMine = false;
      unit->untouchable = false;
      unit->attackType = ATTACK_TYPE_NEAR;
      unit->unitAct = UNIT_ACT_NONE;

      unit->canFindTarget = true;
      //                    WORLD->deselect(unit);
      int miner = unit->currentMine->getTag();
      miner--;
      if (miner == 0) {
        unit->currentMine->setSpriteFrame("mine0.png");
      }
      unit->currentMine->setTag(miner);
    }

    //                if(unit->unitType == UNIT_HELICOPTER){
    //                    isHelicopter = true;
    //                }
    //                sourceIDs = strmake("%d.", unit->unitID);
  }
  //            sourceIDs = sourceIDs.substr(0, sourceIDs.length()-1);
}
void GameScene::moveTo(Vector<EnemyBase *> troop, Vec2 pos) {
  //    GM->playSoundEffect(SOUND_PENCIL_SHORT);

  if (HUD != nullptr) {
    for (auto unit : troop) {
      if (unit != nullptr && (unit->unitType == UNIT_WORKER ||
                              unit->unitType == UNIT_GOBLIN_WORKER)) {
        HUD->dismissMenu();
        break;
      }
    }
  }

  bool isHelicopter = false;
  // I was not happy when I was lonely
  // I am not lonely but I am not happy
  // Maybe I was not happy not because I was lonely
  // I don't remember when I was happy
  // There were joyful moments
  // But was there a happy period?
  if (isNewCommandSystem) {
    std::string sourceIDs = "";
    for (auto unit : troop) {
      unit->unreachableTarget = nullptr;
      unit->moveToPos = Vec2::ZERO;
      unit->attackDdangPos = Vec2::ZERO;
      unit->moveFlagPos = pos;
      //            unit->targetMoveTilePos = getCoordinateFromPosition(pos);
      unit->target = nullptr;
      unit->unitAct = UNIT_ACT_MOVE;
      unit->canRevengeAttack = false;
      unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
      unit->cancelAttackSchedule();
      unit->resetRoute();
      unit->isGatheringTree = false;
      unit->isGatheringGold = false;
      unit->isGoingToBuild = false;
      if (unit->unitType == UNIT_HELICOPTER) {
        isHelicopter = true;
      }
      sourceIDs = strmake("%d.", unit->unitID);
    }
    sourceIDs = sourceIDs.substr(0, sourceIDs.length() - 1);
    //        if (isMultiplay && !troop.at(0)->isEnemy) {
    //            MM->moveUnit(sourceIDs, (int)pos.x, (int)pos.y);
    //        }
  } else {
    //        for (auto unit: troop){
    //            unit->isTemporaryFlying = false;
    //            unit->moveToTarget(targetHand);
    //            unit->canFindTarget = false;
    //            unit->attackFlagPos = Vec2::ZERO;
    //            unit->isGatheringTree = false;
    //            unit->isGatheringGold = false;
    //            if(unit->unitType == UNIT_HELICOPTER){
    //                isHelicopter = true;
    //            }
    //        }
  }

  if (isHelicopter) {
    GM->playSoundEffect(SOUND_HELICOPTER);
  }

  if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 1) {
    HUD->tutorialIndex++;
    HUD->tutorialIndex++;
    HUD->talkIndex = 0;
    HUD->talkText = LM->getText("tutorial 3");
    Sprite *spt = (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
    spt->setPosition(Vec2(size.width / 2 - 750, 330));
    spt->stopAllActions();
    spt->setScale(1 / WORLD->imageScale);
    spt->setSpriteFrame("mine.png");
  }

  //    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 1) {
  //        HUD->tutorialIndex++;
  //        HUD->talkIndex = 0;
  //        HUD->talkText = LM->getText("tutorial 2");
  //        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
  //        Sprite* sptTemp = Sprite::create("handIcon2.png");
  //        spt->setSpriteFrame(sptTemp->getSpriteFrame());
  //        cocos2d::Size size = Director::getInstance()->getWinSize();
  //        spt->stopAllActions();
  //        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0,
  //        Vec2(size.width/2 - 750 - 100, 330 + 50)), MoveBy::create(0.5f,
  //        Vec2(150, -200)), DelayTime::create(1), NULL)));
  //    }
}
void GameScene::moveTo(Vector<EnemyBase *> troop, EnemyBase *target) {
  GM->playSoundEffect(SOUND_PENCIL_SHORT);
  showTargetHand(target->getPosition(), false);
  for (auto unit : troop) {
    unit->moveToTarget(target);
    unit->canFindTarget = false;
    unit->attackFlagPos = Vec2::ZERO;
  }
}
void GameScene::gatherTo(Vector<EnemyBase *> troop, EnemyBase *target) {
  GM->playSoundEffect(SOUND_PENCIL_SHORT);
  showTargetHand(target->getPosition(), false);
  for (auto unit : troop) {
    unit->moveToTarget(target);
    unit->canFindTarget = false;
  }
}
void GameScene::showTargetHand(cocos2d::Vec2 pos, bool isAttack) {
  const float markerScale = getCommandMarkerScale();
  targetHand->setPosition(pos);
  if (this->getChildByName("pointer")) {
    this->removeChildByName("pointer");
  }
  if (this->getChildByName("targetCircle")) {
    this->removeChildByName("targetCircle");
  }
  Sprite *spt = Sprite::create(isAttack ? "swordIcon.png" : "handIcon.png");
  this->addChild(spt, 100);
  spt->setPosition(pos + Vec2(50, 50));
  spt->setOpacity(0);
  spt->setScale(markerScale);
  spt->setName("pointer");
  spt->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(0.5f),
                                  FadeOut::create(0.1f), SPT_REMOVE_FUNC,
                                  NULL));
  spt->runAction(Sequence::create(
      EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2),
      DelayTime::create(0.1f),
      EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2), NULL));

  Node *node = Node::create();
  node->setName("targetCircle");
  this->addChild(node, 10);
  node->setPosition(pos);
  spt = Sprite::create("targetCircle.png");
  node->addChild(spt);
  spt->setOpacity(0);
  spt->setScale(markerScale);
  node->runAction(Sequence::create(
      DelayTime::create(1),
      CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, node)), NULL));
  float dur = 0.2f;
  spt->runAction(Sequence::create(FadeIn::create(dur),
                                  DelayTime::create(0.6f - dur * 2),
                                  FadeOut::create(dur), NULL));
  spt->runAction(Sequence::create(MoveBy::create(dur, Vec2(0, 10)),
                                  DelayTime::create(0.6f - dur * 2),
                                  MoveBy::create(dur, Vec2(0, -10)), NULL));
}
void GameScene::stop(Vector<EnemyBase *> troop) {
  for (auto unit : selectedArray) {
    unit->stop();
  }
}
void GameScene::moveAndAttackTo(EnemyBase *unit, cocos2d::Vec2 pos) {
  if (isNewCommandSystem) {
    unit->target = nullptr;
    unit->attackDdangTo(pos);
    //        MM->moveUnit(Value(unit->unitID).asString(), (int)pos.x,
    //        (int)pos.y);
  } else {
    unit->targetCoordinate = Vec2::ZERO;
    unit->target = nullptr;
    unit->isTemporaryFlying = false;
    unit->moveToTarget(pos);
    unit->canFindTarget = unit->attackType != ATTACK_TYPE_NONE;
    unit->attackFlagPos = pos;
  }
}
void GameScene::moveAndAttackTo(Vector<EnemyBase *> troop, Vec2 pos) {
  moveTo(troop, pos);

  for (auto unit : troop) {
    //        moveAndAttackTo(unit, pos);
    unit->attackFlagPos = unit->moveFlagPos;
    unit->attackDdangPos = unit->moveFlagPos;
    unit->unitAct = UNIT_ACT_ATTACK_DDANG;
    unit->target = nullptr;
  }
  selectCommand(COMMAND_NOTHING);

  if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 10) {
    HUD->tutorialIndex++;
    HUD->talkIndex = 0;
    HUD->talkText = LM->getText("tutorial 11");
    Sprite *spt = (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
    spt->setVisible(false);
    //                    spt->setSpriteFrame("handIcon.png");
    //                    cocos2d::Size size =
    //                    Director::getInstance()->getWinSize();
    //                    spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0,
    //                    Vec2(size.width/2 - 750 - 50, 330 - 50)),
    //                    MoveBy::create(0.5f, Vec2(100, -100)),
    //                    DelayTime::create(1), NULL)));
  }
}
void GameScene::forceAttack(Vector<EnemyBase *> troop, EnemyBase *target) {
  if (target == nullptr || !target || troop.size() == 0) {
    return;
  }
  if (!HUD->isRaid) {
    if (target->unitType == UNIT_TREE || target->unitType == UNIT_MINE) {
      return;
    }
  }
  if (troop.size() == 0) {
    return;
  } else {
    moveTo(troop, target->getApproachingPoint(troop.at(0)->getPosition()));
  }

  for (auto unit : troop) {
    if (unit->attackType == ATTACK_TYPE_NONE || !canAttack(unit, target)) {
      unit->moveToTarget(target);
      continue;
    } else {
      unit->unitAct = UNIT_ACT_ATTACK;
      unit->moveToPos = Vec2::ZERO;
      unit->target = target;
    }
    unit->canRevengeAttack = false;
    //        unit->moveToTarget(target);

    unit->forceAttackTarget = true;
    unit->canFindTarget = false;
    unit->attackFlagPos = Vec2::ZERO;
  }
  selectCommand(COMMAND_NOTHING);
}

void GameScene::selectByDrag(cocos2d::Rect rect) {
  if (!isShiftHeldForSelection()) {
    deselectAll();
  }
  //    draw->drawRect(rect.origin, Vec2(rect.getMaxX(), rect.getMaxY()),
  //    Color4F::RED);
  if (rect.size.width < 0) {
    rect.origin.x = rect.origin.x + rect.size.width;
    rect.size.width *= -1;
  }
  if (rect.size.height < 0) {
    rect.origin.y = rect.origin.y + rect.size.height;
    rect.size.height *= -1;
  }
  for (auto unit : heroArray) { // select heroes
    if (rect.containsPoint(unit->getPosition()) && !unit->isBuilding &&
        !unit->isAlli && unit->isVisible()) {
      selectUnit(unit);
    }
  }
  if (selectedArray.size() == 0) { // select hero's building
    for (auto unit : heroArray) {
      if (rect.containsPoint(unit->getPosition()) && unit->isBuilding) {
        selectUnit(unit);
        break;
      }
      if (rect.containsPoint(unit->getPosition()) && unit->isAlli) {
        selectUnit(unit);
        break;
      }
    }
  }
  if (selectedArray.size() == 0) { // select enemy
    for (auto unit : enemyArray) {
      if (rect.containsPoint(unit->getPosition())) {
        selectUnit(unit);
        break;
      }
    }
  }

  if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 9) {
    HUD->tutorialIndex++;
    HUD->talkIndex = 0;
    HUD->talkText = LM->getText("tutorial 10");
    Sprite *spt = (Sprite *)HUD->tutorialNode->getChildByName("sptIcon");
    Sprite *sptTemp = Sprite::create("handIcon.png");
    spt->setScale(1);
    spt->setSpriteFrame(sptTemp->getSpriteFrame());
    cocos2d::Size size = Director::getInstance()->getWinSize();
    spt->stopAllActions();
    spt->runAction(RepeatForever::create(Sequence::create(
        MoveTo::create(0, Vec2(size.width / 2 - 750, 330 + 50)),
        MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.1f, Vec2(0, 100)),
        MoveBy::create(0.1f, Vec2(0, -100)),
        MoveBy::create(0.15f, Vec2(0, 100)), DelayTime::create(1), NULL)));
  }
}
void GameScene::deselectAll() {
  Button *btn;
  if (GM->nextScene != STAGE_LOBBY) {
    Button *btn = (Button *)HUD->getChildByName("btnDisassemble");
    if (btn != nullptr) {
      HUD->hideDisassembleButton();
      HUD->hideDisassembleConfirmButtons();
    }
  }

  for (auto spt : selectedArray) {
    spt->showSelectedCircle(false);
  }
  if (moveArrows) {
    moveArrows->setVisible(false);
  }
  selectedArray.clear();
  updateMenu();
}
void GameScene::deselect(Movable *unit) {
  if (selectedArray.find((EnemyBase *)unit) != selectedArray.end()) {
    selectedArray.eraseObject((EnemyBase *)unit);
    unit->showSelectedCircle(false);
    updateMenu();
  }
}
void GameScene::selectUnit(EnemyBase *unit) {
  if (selectedArray.find(unit) != selectedArray.end()) {
    return;
  }
  if (selectedArray.size() == 1 && selectedArray.at(0) == unit) {
    //        doubleClickTimer = 0.3f;
  } else {
    //        doubleClickTimer = -1;
  }
  doubleClickTimerForAttackDdaing = -1;
  //    if(unit->isVisible()){
  selectedArray.pushBack(unit);

  unit->showSelectedCircle(true);
  updateMenu();
  //    }
}
void GameScene::updateFoodMaxState() {
  if (GM->currentStageIndex == STAGE_LOBBY)
    return;
  int max = 0;
  for (auto unit : heroArray) {
    if (unit->isBuildingComplete && !unit->isAlli) {
      max += getFoodGive(unit->unitType);
    }
  }
  foodMax = max;
  if (foodMax > 100) {
    foodMax = 100;
  }
  HUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
  checkClearGame();
}
void GameScene::updateMenu() {
  //    if(buildingTemplate != nullptr && GM->currentStageIndex != STAGE_LOBBY){
  //        buildingTemplate->removeFromParent();
  //        buildingTemplate = nullptr;
  //    }
  hideShuttleCargoPanel();
  selectCommand(COMMAND_NOTHING);
  if (selectedArray.size() == 0 || selectedArray.size() > 1) {

    for (int i = 0; i < 6; i++) {
      HUD->setMenu(i, BTN_TYPE_NONE);
    }
  } else if (selectedArray.at(0)->alliSide == WHICH_SIDE_HERO) {
    if (selectedArray.at(0)->isBuilding &&
        selectedArray.at(0)->isBuildingComplete) {
      int noneFrom = 3;
      if (selectedArray.at(0)->unitType == UNIT_CASTLE) {
        if (selectedArray.at(0)->unitName.compare("hq") == 0) {
          HUD->setMenu(0, BTN_TYPE_GOBLIN_WORKER);
        } else {
          HUD->setMenu(0, BTN_TYPE_WORKER);
        }
        noneFrom = 1;
      } else if (selectedArray.at(0)->unitType == UNIT_FARM) {
        noneFrom = 0;
      } else if (selectedArray.at(0)->unitType == UNIT_BARRACKS) {
        HUD->setMenu(0, BTN_TYPE_SWORDMAN);
        HUD->setMenu(1, BTN_TYPE_ARCHER);
        noneFrom = 2;
      } else if (selectedArray.at(0)->unitType == UNIT_ORC_BARRACKS) {
        HUD->setMenu(0, BTN_TYPE_GOBLIN);
        HUD->setMenu(1, BTN_TYPE_GOBLIN_BOMB);
        HUD->setMenu(2, BTN_TYPE_ORC_AXE);
        HUD->setMenu(3, BTN_TYPE_ORC_SPEAR);
        noneFrom = 4;
      } else if (selectedArray.at(0)->unitType == UNIT_LUMBERMILL) {
        noneFrom = 0;
      } else if (selectedArray.at(0)->unitType == UNIT_WATCHERTOWER) {
        noneFrom = 0;
      } else if (selectedArray.at(0)->unitType == UNIT_AIRPORT) {
        HUD->setMenu(0, BTN_TYPE_HELICOPTER);
        noneFrom = 1;
      } else if (selectedArray.at(0)->unitType == UNIT_ORC_TROLL_HOUSE) {
        HUD->setMenu(0, BTN_TYPE_TROLL);
        noneFrom = 1;
      } else if (selectedArray.at(0)->unitType == UNIT_FACTORY) {
        HUD->setMenu(0, BTN_TYPE_CATAPULT);
        noneFrom = 1;
      } else if (selectedArray.at(0)->unitType == UNIT_TEMPLE) {
        HUD->setMenu(0, BTN_TYPE_MAGE);
        noneFrom = 1;
      } else if (selectedArray.at(0)->unitType == UNIT_HUMAN_SHIPYARD) {
        HUD->setMenu(0, BTN_TYPE_HUMAN_SHUTTLE);
        HUD->setMenu(1, BTN_TYPE_HUMAN_SHIP);
        HUD->setMenu(2, BTN_TYPE_HUMAN_BATTLE_SHIP);
        HUD->setMenu(3, BTN_TYPE_OIL_SHIP);
        noneFrom = 4;
      } else if (selectedArray.at(0)->unitType == UNIT_ORC_SHIPYARD) {
        HUD->setMenu(0, BTN_TYPE_ORC_SHUTTLE);
        HUD->setMenu(1, BTN_TYPE_ORC_SHIP);
        HUD->setMenu(2, BTN_TYPE_ORC_BATTLE_SHIP);
        HUD->setMenu(3, BTN_TYPE_ORC_OIL_SHIP);
        noneFrom = 4;
      } else if (selectedArray.at(0)->unitType == UNIT_HUMAN_FOUNDRY ||
                 selectedArray.at(0)->unitType == UNIT_ORC_FOUNDRY) {
        EnemyBase *foundry = selectedArray.at(0);
        bool isOrc = (foundry->unitType == UNIT_ORC_FOUNDRY);
        int atkLv = isOrc ? orcAttackLevel  : humanAttackLevel;
        int defLv = isOrc ? orcDefenseLevel : humanDefenseLevel;
        int atkResType = isOrc ? RESEARCH_ORC_ATTACK  : RESEARCH_HUMAN_ATTACK;
        int defResType = isOrc ? RESEARCH_ORC_DEFENSE : RESEARCH_HUMAN_DEFENSE;
        bool busyAtk = foundry->isResearching && foundry->researchType == atkResType;
        bool busyDef = foundry->isResearching && foundry->researchType == defResType;
        int slot = 0;
        if (atkLv < 2 && !busyAtk) HUD->setMenu(slot++, BTN_TYPE_UPGRADE_ATTACK);
        if (defLv < 2 && !busyDef) HUD->setMenu(slot++, BTN_TYPE_UPGRADE_DEFENSE);
        noneFrom = slot;
      }

      for (int i = noneFrom; i < 6; i++) {
        HUD->setMenu(i, BTN_TYPE_NONE);
      }
      HUD->arrangeMenu(selectedArray.at(0)->getPosition());
    } else {
      // Shuttle: show cargo panel instead of normal command buttons
      if (selectedArray.size() == 1) {
        int ut = selectedArray.at(0)->unitType;
        if (ut == UNIT_HUMAN_SHUTTLE || ut == UNIT_ORC_SHUTTLE) {
          showShuttleCargoPanel(selectedArray.at(0));
          for (int i = 0; i < 6; i++)
            HUD->setMenu(i, BTN_TYPE_NONE);
          HUD->arrangeMenu(selectedArray.at(0)->getPosition());
          return;
        }
        if (ut == UNIT_HUMAN_OIL_SHIP) {
          HUD->setMenu(0, BTN_TYPE_OIL_EXTRACTOR);
          for (int i = 1; i < 6; i++)
            HUD->setMenu(i, BTN_TYPE_NONE);
          HUD->arrangeMenu(selectedArray.at(0)->getPosition());
          return;
        }
        if (ut == UNIT_ORC_OIL_SHIP) {
          HUD->setMenu(0, BTN_TYPE_ORC_OIL_EXTRACTOR);
          for (int i = 1; i < 6; i++)
            HUD->setMenu(i, BTN_TYPE_NONE);
          HUD->arrangeMenu(selectedArray.at(0)->getPosition());
          return;
        }
      }
      //            HUD->setMenu(0, BTN_TYPE_MOVE);
      //            HUD->setMenu(1, BTN_TYPE_STOP);
      bool canAttack = false;
      bool canGather = true;
      for (auto unit : selectedArray) {
        if (unit->unitType != UNIT_WORKER &&
            unit->unitType != UNIT_GOBLIN_WORKER) {
          canGather = false;
        }
        //                if (unit->unitType != UNIT_HELICOPTER) {
        canAttack = true;
        //                }
      }
      bool workerIsBusy = false;
      if (selectedArray.size() == 1) {
        EnemyBase *worker = selectedArray.at(0);
        if (worker->unitType == UNIT_WORKER || worker->unitType == UNIT_GOBLIN_WORKER) {
          workerIsBusy = worker->isGatheringGold || worker->isGatheringTree ||
                         worker->isBuildingABuilding;
        }
      }

      int noneFrom = 2;
      if (canGather && selectedArray.at(0)->alliSide == WHICH_SIDE_HERO &&
          !workerIsBusy) {
        selectedArray.at(0)->isGoingToBuild = false;
        HUD->setMenu(0, BTN_TYPE_BUILD);
        HUD->setMenu(1, BTN_TYPE_BUILD_BETTER);
      } else {
        noneFrom = 0;
      }
      for (int i = noneFrom; i < 6; i++) {
        HUD->setMenu(i, BTN_TYPE_NONE);
      }

      HUD->arrangeMenu(selectedArray.at(0)->getPosition());
      if (selectedArray.size() == 1 &&
          (selectedArray.at(0)->unitType == UNIT_WORKER ||
           selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER)) {
        EnemyBase *worker = selectedArray.at(0);
        if (!workerIsBusy) {
          if (isMultiplay) {
            std::string strIDs = Value(worker->unitID).asString();
            Vec2 pos = worker->getPosition();
            // for network
            MM->moveUnit(strIDs, pos.x, pos.y - LATANCY);

            // for hero
            HMsg msg;
            msg.msgType = MSG_CODE_UNIT_MOVE;
            msg.sourceIDs = strIDs;
            msg.value0 = pos.x;
            msg.value1 = pos.y;
            msg.startTime = gameFrameTimer;
            ownMsgList.push_back(msg);
          } else {
            worker->stop();
          }
        }

        //                if(isMultiplay){
        //                    MM->stopUnit(Value(selectedArray.at(0)->unitID).asString(),
        //                    (int)selectedArray.at(0)->getPositionX(),
        //                    (int)selectedArray.at(0)->getPositionY());
        //                }
      }
    }
  }
}
void GameScene::resetTouchStart(float dt) {
  //    touchStart = Vec2(0, size.height);
}
void GameScene::setPlayer(Vec2 pos) {}

void GameScene::getOffVehicle() {}

// void GameScene::TouchesCancelled(const std::vector<Touch*>& touches, Event
// *unused_event)
//{
//     Touch *touch = (Touch*)(touches.at(0));
//     Vec2 location = touch->getLocationInView();
//     location = Director::getInstance()->convertToGL(location);

//}

void GameScene::solveCollision(Movable *p, Vec2 pos, cocos2d::Rect rect) {}
EnemyBase *GameScene::getEncounteredNPC(cocos2d::Vec2 pos) {
  for (auto npc : npcArray) {
    if (npc->getBoundingBox().containsPoint(pos)) {
      return npc;
    }
  }
  return nullptr;
}
void GameScene::showNPCEvent(int index) {
  isInEvent = true;
  HUD->showEvent(index, false);
}
void GameScene::showNPCDisposableTalk(std::string talk) {
  HUD->showSingleNPCMessage(talk);
  //    HUD->showBlackTopAndBottom();
  //    isInEvent = true;
  //    showTalkText(talk, WHOSE_TALK_NPC);
  //    float delay = 3;
  //    imgTalkBox->runAction(Sequence::create(DelayTime::create(delay),
  //    CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)),
  //    NULL)); sptPointer->runAction(Sequence::create(DelayTime::create(delay),
  //    CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)),
  //    NULL));
}
float GameScene::checkBottom(Movable *p) {
  cocos2d::Rect rect;
  Vec2 pos;
  Vec2 plPos;
  Vec2 newPlPos;
  bool ground = false;
  bool escape = false;
  float temp = -1;
  // check bottom
  rect = p->collisionBoundingBox();
  int offset = 1;
  pos = rect.origin + Vec2(rect.size.width - offset, 0);
  do {
    if (pos.x <= rect.origin.x + offset) {
      pos.x = rect.origin.x + offset;
      escape = true;
    }
    TMXTiledMap *map = getTileMap(pos);
    //        if(map){
    plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
    int tgid;
    tgid = getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
    //            if(tgid > 0){
    cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
    //            tileRect.origin.y = floorf(tileRect.origin.y);
    //            rect.origin.y = floorf(rect.origin.y);
    cocos2d::Rect inter = intersection(tileRect, rect);
    //            inter.size.height = floorf(inter.size.height);
    if (p->velocity.y < 0) {
      if (inter.size.width > 0 && inter.size.height > 0 &&
          p->getCurrentY() >= inter.origin.y + inter.size.height) {
        bool way =
            !isWay(tgid) &&
            !isHighWay(getTileGIDAt(
                (TMXLayer *)map->getChildByTag(TAG_HIGH_STAGE_LAYER), plPos));
        if (way || (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, plPos))) ||
            (soilLayer && isSoilBlock(getTileGIDAt(soilLayer, plPos)))) {
          temp = inter.size.height;
          ground = true;

          if (p->onGround) {
            int tgid;
            if (p->isTowardLeft) {
              //                                plPos =
              //                                getCoordinateFromPosition(rect.origin
              //                                + Vec2(0, -5), map);
              tgid = getTileAtPosition(rect.origin + Vec2(0, -5),
                                       TAG_STAGE_LAYER, map);
            } else {
              //                                plPos =
              //                                getCoordinateFromPosition(rect.origin
              //                                + Vec2(rect.size.width, -5),
              //                                map);
              tgid = getTileAtPosition(rect.origin + Vec2(rect.size.width, -5),
                                       TAG_STAGE_LAYER, map);
            }

            //                                if(!!isWay(tgid) &&
            //                                !isOneWay(tgid)){
            //                                    p->encounteredCliff(p->isTowardLeft?CLIFF_LEFT:CLIFF_RIGHT);
            //                                }
          }

          break;
        }
      }
    }
    //            }
    //        }

    pos = pos - Vec2(TILE_SIZE / 2, 0);
  } while (!escape);

  //    return ground;
  return temp;
}
void GameScene::checkForAndResolveCollisions(Movable *p) {
  cocos2d::Rect rect;
  Vec2 pos;
  Vec2 plPos;
  Vec2 newPlPos;
  bool ground = false;
  bool escape = false;
  float tempTotal = 0;
  float temp = -1;
  justEncounteredNPC = nullptr;
  //    ground = checkBottom(p);
  temp = checkBottom(p);
  if (temp > 0) {
    ground = true;
    tempTotal += temp;
  }
  // check top
  escape = false;
  int xOffset = 1;
  rect = p->collisionBoundingBox();
  pos = rect.origin + Vec2(xOffset, rect.size.height);
  do {
    if (pos.x >= rect.origin.x + rect.size.width - xOffset) {
      pos.x = rect.origin.x + rect.size.width - xOffset;
      escape = true;
    }

    TMXTiledMap *map = getTileMap(pos);
    if (map && p->velocity.y > 0) {
      plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
      int tgid =
          getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
      //            if(tgid > 0){
      //                if(!isWay(tgid) &&
      //                !isHighWay(((TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos))){
      bool way =
          !isWay(tgid) &&
          !isHighWay(getTileGIDAt(
              (TMXLayer *)map->getChildByTag(TAG_HIGH_STAGE_LAYER), plPos));
      if (way || (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, plPos))) ||
          (soilLayer && isSoilBlock(getTileGIDAt(soilLayer, plPos)))) {
        cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
        cocos2d::Rect inter = intersection(tileRect, rect);

        if (inter.size.width > 3 && inter.size.height > 0 &&
            inter.size.width > inter.size.height) {

          //                        if(p->velocity.y >= 0){
          //                            //                            plPos =
          //                            this->getCoordinateFromPosition(pos -
          //                            Vec2(0, TILE_SIZE));
          //                            //                            tgid =
          //                            stageLayer->getTileGIDAt(plPos);
          //                            //                            property =
          //                            tileMap->getPropertiesForGID(tgid); if
          //                            (!isWay(tgid)) {
          //                                p->desiredPosition.y -=
          //                                inter.size.height; p->velocity.y  =
          //                                0; break;
          //                            }
          //                        }

          p->desiredPosition.y -= inter.size.height;
          p->velocity.y = 0;

          break;
        }
      }
      //            }
    }

    pos = pos + Vec2(TILE_SIZE / 2, 0);
  } while (!escape);

  // check left
  escape = false;
  rect = p->collisionBoundingBox();
  if (tempTotal > 0) {
    rect.origin.y += tempTotal;
  }
  pos = rect.origin + Vec2(-xOffset, 1);
  do {
    if (pos.y >= rect.origin.y + rect.size.height - 1) {
      pos.y = rect.origin.y + rect.size.height - 1;
      escape = true;
    }
    //        if(escape) break;
    TMXTiledMap *map = getTileMap(pos);
    if (map && p->velocity.x < 0) {
      plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
      int tgid =
          getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
      //            if(!isWay(tgid) &&
      //            !isHighWay(((TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos))){
      bool way =
          !isWay(tgid) &&
          !isHighWay(getTileGIDAt(
              (TMXLayer *)map->getChildByTag(TAG_HIGH_STAGE_LAYER), plPos));
      if (way || (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, plPos))) ||
          (soilLayer && isSoilBlock(getTileGIDAt(soilLayer, plPos)))) {
        cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
        cocos2d::Rect inter = intersection(tileRect, rect);
        if (inter.size.width > 0 && inter.size.height > 0) {
          p->desiredPosition.x += inter.size.width;

          //                        ground = checkBottom(p);
          temp = checkBottom(p);
          if (tempTotal > 0 && temp < 0) {
            tempTotal = 0;
            ground = false;
          }

          //                    if(p->isTowardLeft ){
          p->encounteredWall();
          //                    }

          break;
        }
      }
    }
    pos = pos + Vec2(0, TILE_SIZE / 2);
  } while (!escape);

  // check right
  escape = false;
  rect = p->collisionBoundingBox();
  if (tempTotal > 0) {
    rect.origin.y += tempTotal;
  }
  pos = rect.origin + Vec2(rect.size.width + xOffset, 1);
  do {
    if (pos.y >= rect.origin.y + rect.size.height) {
      pos.y = rect.origin.y + rect.size.height;
      escape = true;
    }
    //        if(escape) break;
    TMXTiledMap *map = getTileMap(pos);
    if (map && p->velocity.x > 0) {
      plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
      int tgid =
          getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
      //            if(tgid){
      //                if (!isWay(tgid) &&
      //                !isHighWay(((TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos)))
      //                {
      bool way =
          !isWay(tgid) &&
          !isHighWay(getTileGIDAt(
              (TMXLayer *)map->getChildByTag(TAG_HIGH_STAGE_LAYER), plPos));
      if (way || (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, plPos))) ||
          (soilLayer && isSoilBlock(getTileGIDAt(soilLayer, plPos)))) {
        cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
        cocos2d::Rect inter = intersection(tileRect, rect);
        if (inter.size.width > 0 && inter.size.height > 0) {
          p->desiredPosition.x -= inter.size.width;

          //                    ground = checkBottom(p);
          temp = checkBottom(p);
          if (tempTotal > 0 && temp < 0) {
            tempTotal = 0;
            ground = false;
          }

          //                        if(!p->isTowardLeft ){
          p->encounteredWall();
          //                        }
          break;
        }
      }
      //            }
    }

    pos = pos + Vec2(0, TILE_SIZE / 2);
  } while (!escape);
  TMXTiledMap *map = getTileMap(pos);
  if (map) {
    if (((TMXLayer *)map->getChildByTag(TAG_FORE_LAYER))) {
      bool inWater = false;
      int tileId = getTileAtPosition(p->getPosition(), TAG_FORE_LAYER, map);
      if (tileId) {
        if (tileId == 0 ||
            /*info->type.asString().compare("WaterSurface")*/ tileId == 0) {
          inWater = true;
        }
      }

      if (inWater && !p->inWater) {
        batch->setLocalZOrder(-10);
        log("spriteBatch z order set %d", batch->getLocalZOrder());
        p->setInWater(true);
        //            p->setOnGround(false);
        inWaterArray.pushBack(p);
      } else if (!inWater && p->inWater) {

        p->setInWater(false);

        inWaterArray.eraseObject(p);
      }
    }
  }
  if (justEncounteredNPC == nullptr &&
      !isInEvent) { // to meet the same npc again
    encounteredNPC = nullptr;
  }

  if (ground) {
    if (!p->onGround) {
      p->onLanded();
    }
    tempTotal = floorf(tempTotal * 100) / 100;
    p->velocity.y = -0.001;
    p->desiredPosition.y += tempTotal;
  }

  //    p->setOnGround(ground);
  p->setPosition(p->desiredPosition);
}
void GameScene::changeSprite(Ref *ref) {
  Sprite *spt = (Sprite *)ref;
  spt->setSpriteFrame(spt->getName());
}
bool GameScene::isFregile(int index) {
  switch (index) {
  case 4:
  case 5:
  case 6:
  case 33:
  case 70:
    return true;

  default:
    return false;
  }
}
bool GameScene::isHighWay(int index) {
  switch (index) {
  case 21:
  case 37:
  case 53:
  case 135:
  case 136:
  case 137:
  case 151:
  case 152:
  case 153:
  case 168:
    return true;
  default:
    return false;
  }
}
bool GameScene::isWay(int index) {
  return true;
  if (isOnFirstFloor) {
    switch (index) {
    case 82:
    case 98:
    case 84:
    case 85:
    case 86:
    case 100:
    case 101:
    case 102:
    case 116:
    case 117:
    case 118:

      return true;
    default:
      return false;
    }
  } else {
    switch (index) {
    case 39:
    case 40:
    case 41:
    case 43:
    case 44:
    case 55:
    case 56:
    case 57:
    case 59:
    case 60:
    case 135:
    case 136:
    case 137:
    case 151:
    case 152:
    case 153:
    case 158:
      return true;
    default:
      return false;
    }
  }
}
bool GameScene::isSoilBlock(int index) {
  switch (index) {
  case 141:
  case 142:
  case 143:
  case 144:
  case 145:
  case 151:
  case 152:
  case 153:
  case 154:
  case 155:
  case 161:
  case 162:
  case 163:
    return true;
  default:
    return false;
  }
}
bool GameScene::isDecoBlock(int index) {
  switch (index) {
  case 48:
  case 49:
    return true;
  default:
    return false;
  }
}
cocos2d::Vec2 GameScene::findEmptyWaterSpawnTile(int shipyardTileX, int shipyardTileY, int shipyardW, int shipyardH) {
  // BFS outward from the shipyard border to find the nearest empty water tile
  std::vector<std::pair<int,int>> frontier;
  std::set<std::pair<int,int>> visited;

  // Seed with all border tiles around the shipyard
  int x0 = shipyardTileX - 1, x1 = shipyardTileX + shipyardW;
  int y0 = shipyardTileY - shipyardH - 1, y1 = shipyardTileY;
  for (int x = shipyardTileX; x < shipyardTileX + shipyardW; x++) {
    frontier.push_back({x, y0});
    frontier.push_back({x, y1});
  }
  for (int y = shipyardTileY - shipyardH; y < shipyardTileY; y++) {
    frontier.push_back({x0, y});
    frontier.push_back({x1, y});
  }
  // corners
  frontier.push_back({x0, y0});
  frontier.push_back({x1, y0});
  frontier.push_back({x0, y1});
  frontier.push_back({x1, y1});

  int dx[] = {1, -1, 0, 0};
  int dy[] = {0, 0, 1, -1};

  int limit = 0;
  while (!frontier.empty() && limit++ < 200) {
    std::vector<std::pair<int,int>> next;
    for (auto& p : frontier) {
      if (visited.count(p)) continue;
      visited.insert(p);
      int tx = p.first, ty = p.second;
      if (tx < 0 || ty < 0 || tx >= (int)mapSize.width || ty >= (int)mapSize.height) continue;
      if (isWaterTileAt(tx, ty) && !isShipTileBlocked(tx, ty, nullptr)) {
        return getPositionFromTileCoordinate(tx, ty);
      }
      for (int i = 0; i < 4; i++) {
        std::pair<int,int> nb = {tx + dx[i], ty + dy[i]};
        if (!visited.count(nb)) next.push_back(nb);
      }
    }
    frontier = next;
  }
  return Vec2(-1, -1);
}

bool GameScene::isWaterTileAt(int tileX, int tileY) {
  if (theMap == nullptr) {
    return false;
  }
  TMXLayer *stageLayer = theMap->getLayer("stage");
  if (stageLayer == nullptr) {
    return false;
  }
  // Water GIDs are 141-163 (tile-ids 140-162 + the TMX firstgid=1 offset) -
  // see createTMXFromEditorJsonFile's autotiler and isSoilBlock, which checks
  // this exact same range on the separate "soil" layer for the opposite
  // purpose (blocking land units from water).
  int gid = getTileGIDAt(stageLayer, Vec2(tileX, tileY));
  return gid >= 141 && gid <= 163;
}
bool GameScene::isShipTileBlocked(int tileX, int tileY, Movable *self) {
  for (const auto &occupied : shipOccupiedTiles) {
    if (occupied.first == tileX && occupied.second == tileY) {
      // A ship never blocks its own current tile - without this it could
      // never be re-evaluated as "staying put" while still standing on it.
      if (self != nullptr) {
        Vec2 selfTile = getCoordinateFromPosition(self->getPosition());
        if ((int)selfTile.x == tileX && (int)selfTile.y == tileY) {
          continue;
        }
      }
      return true;
    }
  }
  // Water buildings block ship movement just like other ships do.
  // Exception: if the ship is already trapped inside a building (e.g. an oil
  // ship whose extractor just completed under it), let it pass through that
  // building's tiles so it can escape — but still block all other buildings.
  Vec2 selfTile = (self != nullptr)
                      ? getCoordinateFromPosition(self->getPosition())
                      : Vec2(-1, -1);
  auto isWaterBuildingType = [](int type) -> bool {
    return type == UNIT_HUMAN_SHIPYARD || type == UNIT_ORC_SHIPYARD ||
           type == UNIT_HUMAN_OIL_EXTRACTOR || type == UNIT_ORC_OIL_EXTRACTOR ||
           type == UNIT_HUMAN_OIL_REFINERY || type == UNIT_ORC_OIL_REFINERY;
  };
  auto tileInUnit = [&](EnemyBase *bldg, int tx, int ty) -> bool {
    if (!bldg->isBuilding || !bldg->isBuildingComplete ||
        !isWaterBuildingType(bldg->unitType))
      return false;
    Vec2 st = bldg->buildingStartCoordinate;
    Size sz = bldg->buildingOccupySize;
    return tx >= (int)st.x && tx < (int)(st.x + sz.width) &&
           ty >= (int)st.y && ty < (int)(st.y + sz.height);
  };
  for (auto u : heroArray) {
    if (!tileInUnit(u, tileX, tileY)) continue;
    // Let an already-trapped ship pass through this building's tiles to escape.
    if (self != nullptr && tileInUnit(u, (int)selfTile.x, (int)selfTile.y))
      continue;
    return true;
  }
  for (auto u : enemyArray) {
    if (!tileInUnit(u, tileX, tileY)) continue;
    if (self != nullptr && tileInUnit(u, (int)selfTile.x, (int)selfTile.y))
      continue;
    return true;
  }
  return false;
}

void GameScene::fleeShipFromAttacker(Movable *unit, Movable *attacker) {
  if (unit == nullptr || attacker == nullptr) return;
  if (unit->unitType != UNIT_HUMAN_OIL_SHIP && unit->unitType != UNIT_ORC_OIL_SHIP &&
      unit->unitType != UNIT_HUMAN_SHUTTLE && unit->unitType != UNIT_ORC_SHUTTLE) {
    return;
  }
  Vec2 tile = getCoordinateFromPosition(unit->getPosition());
  int tx = (int)tile.x, ty = (int)tile.y;

  Vec2 away = unit->getPosition() - attacker->getPosition();
  if (away.lengthSquared() < 0.001f) away = Vec2(1, 0);
  away.normalize();

  // Tile-grid neighbors, ordered so ties prefer the purely-opposite direction
  // first. Tile y grows downward (see getPositionFromTileCoordinate), so a
  // neighbor's world-space direction is (dx, -dy).
  static const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  static const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

  int bestIdx = -1;
  float bestDot = -2.0f;
  for (int i = 0; i < 8; i++) {
    int nx = tx + dx[i], ny = ty + dy[i];
    if (nx < 0 || ny < 0 || nx >= (int)mapSize.width || ny >= (int)mapSize.height)
      continue;
    if (!isWaterTileAt(nx, ny) || isShipTileBlocked(nx, ny, unit))
      continue;
    Vec2 worldDir = Vec2((float)dx[i], -(float)dy[i]).getNormalized();
    float dot = worldDir.dot(away);
    if (dot > bestDot) {
      bestDot = dot;
      bestIdx = i;
    }
  }
  if (bestIdx < 0) return;

  Vec2 newPos = getPositionFromTileCoordinate(tx + dx[bestIdx], ty + dy[bestIdx]);
  unit->setPosition(newPos);
  if (unit->spine) unit->spine->setPosition(newPos);
}

bool GameScene::isShuttleAdjacentToLand(EnemyBase *shuttle) {
  Vec2 tile = getCoordinateFromPosition(shuttle->getPosition());
  int tx = (int)tile.x, ty = (int)tile.y;
  int dx[] = {1, -1, 0, 0};
  int dy[] = {0, 0, 1, -1};
  for (int i = 0; i < 4; i++) {
    if (!isWaterTileAt(tx + dx[i], ty + dy[i]))
      return true;
  }
  return false;
}

bool GameScene::canUnitBoard(EnemyBase *unit) {
  return !unit->isFlying && unit->unitType != UNIT_HUMAN_SHUTTLE &&
         unit->unitType != UNIT_ORC_SHUTTLE &&
         unit->unitType != UNIT_HUMAN_SHIP && unit->unitType != UNIT_ORC_SHIP &&
         unit->unitType != UNIT_HUMAN_BATTLE_SHIP;
}

void GameScene::getCargoSize(int unitType, int &cols, int &rows) {
  if (unitType == UNIT_TROLL || unitType == UNIT_CATAPULT) {
    cols = 2; rows = 2;
  } else if (unitType == UNIT_ORC_AXE || unitType == UNIT_ORC_SPEAR) {
    cols = 1; rows = 2;
  } else {
    cols = 1; rows = 1;
  }
}

bool GameScene::findCargoSlot(EnemyBase *shuttle, int cols, int rows,
                               int &outCol, int &outRow) {
  for (int r = 0; r <= 2 - rows; r++) {
    for (int c = 0; c <= 4 - cols; c++) {
      bool fits = true;
      for (int dc = 0; dc < cols && fits; dc++)
        for (int dr = 0; dr < rows && fits; dr++)
          if (shuttle->cargoGrid[c + dc][r + dr])
            fits = false;
      if (fits) {
        outCol = c;
        outRow = r;
        return true;
      }
    }
  }
  return false;
}

bool GameScene::loadUnitToShuttle(EnemyBase *shuttle, EnemyBase *unit) {
  if (!canUnitBoard(unit) || unit->isInShuttle)
    return false;
  if (unit->alliSide != shuttle->alliSide || unit->energy <= 0)
    return false;
  int cols, rows;
  getCargoSize(unit->unitType, cols, rows);
  int slotCol, slotRow;
  if (!findCargoSlot(shuttle, cols, rows, slotCol, slotRow))
    return false;
  for (int dc = 0; dc < cols; dc++)
    for (int dr = 0; dr < rows; dr++)
      shuttle->cargoGrid[slotCol + dc][slotRow + dr] = true;
  Movable::CargoSlot slot;
  slot.unit = unit;
  slot.col = slotCol;
  slot.row = slotRow;
  slot.cols = cols;
  slot.rows = rows;
  shuttle->shuttleCargo.push_back(slot);
  unit->isInShuttle = true;
  unit->myShuttle = shuttle;
  unit->setPosition(shuttle->getPosition());
  unit->setVisible(false);
  if (unit->spine) unit->spine->setVisible(false);
  unit->untouchable = true;
  unit->stop();
  deselect(unit);
  return true;
}

void GameScene::unloadShuttle(EnemyBase *shuttle) {
  if (!isShuttleAdjacentToLand(shuttle))
    return;
  Vec2 tile = getCoordinateFromPosition(shuttle->getPosition());
  int tx = (int)tile.x, ty = (int)tile.y;
  int mapH = (int)theMap->getMapSize().height;
  std::vector<Vec2> landPos;
  int dx[] = {1, -1, 0, 0};
  int dy[] = {0, 0, 1, -1};
  for (int i = 0; i < 4; i++) {
    int nx = tx + dx[i], ny = ty + dy[i];
    if (!isWaterTileAt(nx, ny))
      landPos.push_back(Vec2((nx + 0.5f) * TILE_SIZE, (mapH - ny - 0.5f) * TILE_SIZE));
  }
  if (landPos.empty())
    return;
  int idx = 0;
  for (auto &slot : shuttle->shuttleCargo) {
    EnemyBase *unit = (EnemyBase *)slot.unit;
    if (!unit || unit->energy <= 0)
      continue;
    unit->isInShuttle = false;
    unit->myShuttle = nullptr;
    unit->untouchable = false;
    unit->setVisible(true);
    if (unit->spine) unit->spine->setVisible(true);
    unit->setPosition(landPos[idx % landPos.size()]);
    // Clear all stale movement state set during boarding (stop() runs while
    // unit is at the shuttle's water tile, which causes stopNew() to queue a
    // move to a nearby land tile — we don't want that after unloading).
    unit->unitAct = UNIT_ACT_NONE;
    unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
    unit->moveToPos = Vec2::ZERO;
    unit->moveFlagPos = Vec2::ZERO;
    unit->attackFlagPos = Vec2::ZERO;
    unit->attackDdangPos = Vec2::ZERO;
    unit->target = nullptr;
    unit->resetRoute();
    idx++;
  }
  memset(shuttle->cargoGrid, 0, sizeof(shuttle->cargoGrid));
  shuttle->shuttleCargo.clear();
  refreshShuttleCargoPanel();
}

void GameScene::unloadSingleUnitFromShuttle(EnemyBase *shuttle, EnemyBase *unit) {
  if (!shuttle || !unit) return;
  if (!isShuttleAdjacentToLand(shuttle)) {
    HUD->showPriceInfo(LM->getText("unload land only"), 0, 0, 0);
    return;
  }
  Vec2 tile = getCoordinateFromPosition(shuttle->getPosition());
  int tx = (int)tile.x, ty = (int)tile.y;
  int mapH = (int)theMap->getMapSize().height;
  int dx[] = {1, -1, 0, 0};
  int dy[] = {0, 0, 1, -1};
  Vec2 landPos = Vec2::ZERO;
  for (int i = 0; i < 4; i++) {
    int nx = tx + dx[i], ny = ty + dy[i];
    if (!isWaterTileAt(nx, ny)) {
      landPos = Vec2((nx + 0.5f) * TILE_SIZE, (mapH - ny - 0.5f) * TILE_SIZE);
      break;
    }
  }
  if (landPos == Vec2::ZERO) return;

  // Remove from cargo grid
  for (int i = 0; i < (int)shuttle->shuttleCargo.size(); i++) {
    if (shuttle->shuttleCargo[i].unit == unit) {
      auto &slot = shuttle->shuttleCargo[i];
      for (int dc = 0; dc < slot.cols; dc++)
        for (int dr = 0; dr < slot.rows; dr++)
          shuttle->cargoGrid[slot.col + dc][slot.row + dr] = false;
      shuttle->shuttleCargo.erase(shuttle->shuttleCargo.begin() + i);
      break;
    }
  }
  unit->isInShuttle = false;
  unit->myShuttle = nullptr;
  unit->untouchable = false;
  unit->setVisible(true);
  if (unit->spine) unit->spine->setVisible(true);
  unit->setPosition(landPos);
  unit->unitAct = UNIT_ACT_NONE;
  unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
  unit->moveToPos = Vec2::ZERO;
  unit->moveFlagPos = Vec2::ZERO;
  unit->attackFlagPos = Vec2::ZERO;
  unit->attackDdangPos = Vec2::ZERO;
  unit->target = nullptr;
  unit->resetRoute();
  refreshShuttleCargoPanel();
}

void GameScene::showShuttleCargoPanel(EnemyBase *shuttle) {
  hideShuttleCargoPanel();
  shuttlePanelTarget = shuttle;
  if (!shuttle)
    return;

  const float CELL = 65.0f;
  const float PAD = 6.0f;
  float panelW = 4.0f * CELL + 5.0f * PAD;
  float panelH = 2.0f * CELL + 3.0f * PAD;

  Node *panel = Node::create();
  panel->setName("shuttleCargoPanel");
  panel->setContentSize(Size(panelW, panelH));

  Scale9Sprite *bg = Scale9Sprite::create("btnBlack.png");
  bg->setCapInsets(Rect(50, 50, 167, 136));
  bg->setContentSize(Size(panelW, panelH));
  bg->setAnchorPoint(Vec2::ZERO);
  bg->setPosition(Vec2::ZERO);
  bg->setOpacity(230);
  panel->addChild(bg, 0);

  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 2; r++) {
      Scale9Sprite *cell = Scale9Sprite::create("btnBlack.png");
      cell->setCapInsets(Rect(50, 50, 167, 136));
      cell->setContentSize(Size(CELL - 2.0f, CELL - 2.0f));
      cell->setAnchorPoint(Vec2::ZERO);
      cell->setPosition(Vec2(PAD + c * (CELL + PAD) + 1.0f, PAD + r * (CELL + PAD) + 1.0f));
      cell->setOpacity(160);
      panel->addChild(cell, 1);
    }
  }

  for (auto &slot : shuttle->shuttleCargo) {
    EnemyBase *unit = (EnemyBase *)slot.unit;
    if (!unit)
      continue;
    float iconW = slot.cols * CELL + (slot.cols - 1) * PAD;
    float iconH = slot.rows * CELL + (slot.rows - 1) * PAD;
    float cx = PAD + slot.col * (CELL + PAD) + iconW * 0.5f;
    float cy = PAD + slot.row * (CELL + PAD) + iconH * 0.5f;
    Sprite *icon = getSpriteForIcon(unit->unitType);
    if (icon) {
      float natW = icon->getContentSize().width * icon->getScale();
      float natH = icon->getContentSize().height * icon->getScale();
      float sc = std::min((iconW * 0.75f) / natW, (iconH * 0.75f) / natH);
      icon->setScale(icon->getScale() * sc);
      icon->setAnchorPoint(Vec2(0.5f, 0.5f));
      icon->setPosition(Vec2(cx, cy));
      panel->addChild(icon, 2);
    }
    // Transparent clickable button over this slot for individual unload
    EnemyBase *capturedUnit = unit;
    EnemyBase *capturedShuttle = shuttle;
    ui::Button *slotBtn = ui::Button::create("btnBlack.png");
    slotBtn->setOpacity(0);
    slotBtn->setContentSize(Size(iconW, iconH));
    slotBtn->setAnchorPoint(Vec2(0.5f, 0.5f));
    slotBtn->setPosition(Vec2(cx, cy));
    slotBtn->addClickEventListener([this, capturedShuttle, capturedUnit](Ref *) {
      unloadSingleUnitFromShuttle(capturedShuttle, capturedUnit);
    });
    panel->addChild(slotBtn, 3);
  }

  Size winSize = Director::getInstance()->getVisibleSize();
  panel->setAnchorPoint(Vec2::ZERO);
  panel->setPosition(Vec2(winSize.width * 0.5f - panelW * 0.5f, 80.0f));
  HUD->addChild(panel, 500);

  ui::Button *btnUnload = ui::Button::create("btnUnload.png");
  btnUnload->setName("btnUnloadShuttle");
  btnUnload->setAnchorPoint(Vec2(0.0f, 0.5f));
  float btnH = panelH;
  float btnScale = btnH / btnUnload->getContentSize().height;
  btnUnload->setScale(btnScale);
  btnUnload->setPosition(Vec2(winSize.width * 0.5f + panelW * 0.5f + 12.0f, 80.0f + panelH * 0.5f));
  btnUnload->addClickEventListener([this](Ref *) {
    if (!shuttlePanelTarget) return;
    if (isShuttleAdjacentToLand(shuttlePanelTarget)) {
      unloadShuttle(shuttlePanelTarget);
    } else {
      HUD->showPriceInfo(LM->getText("unload land only"), 0, 0, 0);
    }
  });
  HUD->addChild(btnUnload, 500);
}

void GameScene::hideShuttleCargoPanel() {
  if (HUD) {
    HUD->removeChildByName("shuttleCargoPanel");
    HUD->removeChildByName("btnUnloadShuttle");
  }
  shuttlePanelTarget = nullptr;
}

void GameScene::refreshShuttleCargoPanel() {
  if (!shuttlePanelTarget)
    return;
  showShuttleCargoPanel(shuttlePanelTarget);
}

bool GameScene::isOneWay(int index) {
  return false;
  switch (index) {
  case 130:
  case 133:
  case 134:
  case 135:
  case 324:
  case 325:
  case 356:
  case 357:
  case 258:
  case 261:
  case 262:
  case 263:
    return true;

  default:
    return false;
  }
}
bool GameScene::checkMovableMissileCollision(Movable *p) {
  //    Vector<TileInfo*> tiles =
  //    getSurroundingTilesAtPosition(p->getGravityPosition(), stageLayer, p);
  //    //1
  /*if (tiles.size() < 1) {
   MovableMissileArray.eraseObject(p);
   heroMissileArray.eraseObject(p);
   enemyMissileArray.eraseObject(p);
   p->removeFromParentAndCleanup(true);
   CCLOG("tileSize is wrong");
   return true;
   }*/
  //    bool ground = false;
  //    Vec2 current = p->getPosition();
  cocos2d::Rect pRect = p->collisionBoundingBox(); // 2
  p->setPosition(p->desiredPosition);
  int gid;
  TMXTiledMap *map = getTileMap(p->getPosition());
  Vec2 plPos = this->getCoordinateFromPosition(p->getPosition(), map); // 1
  cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map); // info->rect;
  if (p->isFregile) {
    gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
    if (gid) {
      if (!isWay(gid)) {
        MovableMissileArray.eraseObject(p);
        heroMissileArray.eraseObject(p);
        enemyMissileArray.eraseObject(p);
        enemyArray.eraseObject((EnemyBase *)p);
        p->removeFromParent();
        return true;
      }
    }
  } else {
    if (p->velocity.y < 0) {
      if (!isWay(gid)) {
        p->velocity.y = 0;
        cocos2d::Rect intersectionR = intersection(pRect, tileRect);
        if (intersectionR.size.width > 0 && intersectionR.size.height > 0) {
          p->setPosition(p->getPosition() + Vec2(0, intersectionR.size.height));
        }
      }
    }
    if (p->velocity.y > 0) {
      p->velocity.y = -0.0021;
      cocos2d::Rect intersectionR = intersection(pRect, tileRect);
      if (intersectionR.size.width > 0 && intersectionR.size.height > 0) {
        p->setPosition(p->getPosition() + Vec2(0, -intersectionR.size.height));
      }
    }

    if (p->velocity.x > 0) {
      gid = getTileAtPosition(p->getPosition() +
                                  Vec2(p->getContentSize().width / 2, 0),
                              TAG_STAGE_LAYER, map);
    } else if (p->velocity.x < 0) {
      gid = getTileAtPosition(p->getPosition() -
                                  Vec2(p->getContentSize().width / 2, 0),
                              TAG_STAGE_LAYER, map);
    }
    if (!isWay(gid)) {
      MovableMissileArray.eraseObject(p);
      heroMissileArray.eraseObject(p);
      enemyMissileArray.eraseObject(p);
      enemyArray.eraseObject((EnemyBase *)p);
      p->removeFromParent();
      return true;
    }
  }
  return false;
}

bool GameScene::intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect) {
  return !(srcRect.getMaxX() <= dstRect.getMinX() ||
           dstRect.getMaxX() <= srcRect.getMinX() ||
           srcRect.getMaxY() <= dstRect.getMinY() ||
           dstRect.getMaxY() <= srcRect.getMinY());
}
cocos2d::Rect GameScene::intersection(cocos2d::Rect source,
                                       cocos2d::Rect rect) {
  cocos2d::Rect intersection;
  intersection = cocos2d::Rect(
      source.getMinX() > rect.getMinX() ? source.getMinX() : rect.getMinX(),
      source.getMinY() > rect.getMinY() ? source.getMinY() : rect.getMinY(), 0,
      0);
  intersection.size.width =
      (source.getMaxX() < rect.getMaxX() ? source.getMaxX() : rect.getMaxX()) -
      intersection.getMinX();
  intersection.size.height =
      (source.getMaxY() < rect.getMaxY() ? source.getMaxY() : rect.getMaxY()) -
      intersection.getMinY();
  return intersection;
}
void GameScene::checkForAndResolveCollisionsForWater(Movable *p) {

  Vec2 current = p->getGravityPosition();
  TMXTiledMap *map = getTileMap(current);
  cocos2d::Rect pRect = p->collisionBoundingBox();
  int gid;
  for (int i = 0; i < 8; i++) {
    if (i == 0) {
      current = current + Vec2(0, -TILE_SIZE);
    } else if (i == 1) {
      current = current + Vec2(0, TILE_SIZE);
    } else if (i == 2) {
      current = current + Vec2(-TILE_SIZE, 0);
    } else if (i == 3) {
      current = current + Vec2(TILE_SIZE, 0);
    } else if (i == 4) {
      current = current + Vec2(-TILE_SIZE, -TILE_SIZE);
    } else if (i == 5) {
      current = current + Vec2(TILE_SIZE, -TILE_SIZE);
    } else if (i == 6) {
      current = current + Vec2(-TILE_SIZE, TILE_SIZE);
    } else if (i == 7) {
      current = current + Vec2(TILE_SIZE, TILE_SIZE);
    }

    gid = getTileAtPosition(current, TAG_STAGE_LAYER, map);
    if (gid) {
      Vec2 plPos = this->getCoordinateFromPosition(current - map->getPosition(),
                                                   map); // 1
      cocos2d::Rect tileRect =
          tileRectFromTileCoords(plPos, map); // info->rect;

      cocos2d::Rect intersectionR = intersection(pRect, tileRect);
      if (intersectionR.size.width > 0 && intersectionR.size.height > 0) {

        if (!isWay(gid)) {
          //                    int tileIndx = tiles->indexOfObject(dic);
          int tileIndx = i;

          if (tileIndx == 0 && p->velocity.y <= 0) {
            // tile is directly below Hero
            p->desiredPosition =
                Vec2(p->desiredPosition.x,
                     p->desiredPosition.y + intersectionR.size.height);
            p->velocity = Vec2(p->velocity.x, 0);
            //                        p->setOnGround(true);

          } else if (tileIndx == 1) {
            // tile is directly above Hero
            p->desiredPosition =
                Vec2(p->desiredPosition.x,
                     p->desiredPosition.y - intersectionR.size.height);
            p->velocity = Vec2(p->velocity.x, 0.0);
          } else if (tileIndx == 2) {
            // tile is left of Hero
            p->desiredPosition =
                Vec2(p->desiredPosition.x + intersectionR.size.width,
                     p->desiredPosition.y);

          } else if (tileIndx == 3) {
            // tile is right of Hero
            p->desiredPosition =
                Vec2(p->desiredPosition.x - intersectionR.size.width,
                     p->desiredPosition.y);
          } else {
            if (p->velocity.y > 0) { // rasing
              if (tileIndx > 5) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x,
                         p->desiredPosition.y - intersectionR.size.height);
                p->velocity = Vec2(p->velocity.x, 0.0);
              } else if (tileIndx == 4) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x + intersectionR.size.width,
                         p->desiredPosition.y);
              } else if (tileIndx == 5) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x - intersectionR.size.width,
                         p->desiredPosition.y);
              }
            } else { // falling
              if (tileIndx > 5) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x,
                         p->desiredPosition.y + intersectionR.size.height);
                p->velocity = Vec2(p->velocity.x, 0);
                //                                p->setOnGround(true);
              } else if (tileIndx == 4) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x + intersectionR.size.width,
                         p->desiredPosition.y);
              } else if (tileIndx == 5) {
                p->desiredPosition =
                    Vec2(p->desiredPosition.x - intersectionR.size.width,
                         p->desiredPosition.y);
              }
            }
          }
        }
      }
    }
  }

  if (getTileAtPosition(p->desiredPosition, TAG_FORE_LAYER, map) >
      0) { // check if out of water
    p->desiredPosition = p->getPosition();
  }

  p->setPosition(p->desiredPosition);
}

void GameScene::checkForAndResolveCollisionsForBouncing(Movable *p) {
  cocos2d::Rect rect;
  Vec2 pos;
  Vec2 plPos;
  bool ground = false;

  bool escape = false;

  //    p->setOnGround(false);
  // check top
  escape = false;
  rect = p->collisionBoundingBox();
  pos = rect.origin + Vec2(0, rect.size.height);
  TMXTiledMap *map;
  do {
    if (pos.x >= rect.origin.x + rect.size.width) {
      pos.x = rect.origin.x + rect.size.width;
      escape = true;
    }

    map = getTileMap(pos);
    plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
    int tgid =
        getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
    if (tgid > 0) {
      Value property = map->getPropertiesForGID(tgid);
      if (property.getType() != Value::Type::NONE) {
        if (property.asValueMap().at("Type").asString().compare("OneWay") !=
            0) {
          cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
          cocos2d::Rect inter = intersection(tileRect, rect);
          if (inter.size.width > 3 && inter.size.height > 0 &&
              inter.size.width > inter.size.height) {
            if (p->velocity.y > 0) {
              p->desiredPosition.y -= inter.size.height;
              p->velocity.y = 0;
              break;
            }
          }
        }
      }
    }
    pos = pos + Vec2(TILE_SIZE / 2, 0);
  } while (!escape);

  // check left
  escape = false;
  rect = p->collisionBoundingBox();
  pos = rect.origin + Vec2(0, rect.size.height);
  do {
    if (pos.y <= rect.origin.y) {
      break; // pos.y = rect.origin.y + rect.size.height; escape = true;
    }
    map = getTileMap(pos);
    plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
    int tgid =
        getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
    if (tgid > 0 && p->velocity.x <= 0) {
      Value property = map->getPropertiesForGID(tgid);
      if (property.getType() != Value::Type::NONE &&
          property.asValueMap().at("Type").asString().compare("OneWay") != 0) {
        if (property.getType() != Value::Type::NONE) {
          cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
          cocos2d::Rect inter = intersection(tileRect, rect);
          if (inter.size.width > 0 && inter.size.height > 0) {
            p->desiredPosition.x += inter.size.width;

            if (p->isTowardLeft) {
              p->encounteredWall();
            }

            break;
          }
        }
      }
    }
    pos = pos - Vec2(0, TILE_SIZE / 2);
  } while (!escape);

  // check right
  escape = false;
  rect = p->collisionBoundingBox();
  pos = rect.origin + rect.size;
  do {
    if (pos.y <= rect.origin.y) {
      break; // pos.y = rect.origin.y; escape = true;
    }
    map = getTileMap(pos);
    plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
    int tgid =
        getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
    if (tgid > 0 && p->velocity.x >= 0) {
      Value property = map->getPropertiesForGID(tgid);
      if (property.getType() != Value::Type::NONE) {
        if (property.asValueMap().at("Type").asString().compare("OneWay") !=
            0) {
          cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
          cocos2d::Rect inter = intersection(tileRect, rect);
          if (inter.size.width > 0 && inter.size.height > 0) {
            p->desiredPosition.x -= inter.size.width;

            if (!p->isTowardLeft) {
              p->encounteredWall();
            }
            break;
          }
        }
      }
    }

    pos = pos - Vec2(0, TILE_SIZE / 2);
  } while (!escape);

  // check bottom
  rect = p->collisionBoundingBox();
  int offset = 2;
  pos = rect.origin + Vec2(rect.size.width - offset, 0);
  do {
    if (pos.x <= rect.origin.x + offset) {
      pos.x = rect.origin.x;
      escape = true;
    }
    map = getTileMap(pos);
    plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
    int tgid =
        getTileGIDAt((TMXLayer *)map->getChildByTag(TAG_STAGE_LAYER), plPos);
    if (tgid > 0) {
      cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
      cocos2d::Rect inter = intersection(tileRect, rect);

      if (p->velocity.y < 0) {
        if (inter.size.width > 0 && inter.size.height > 0 &&
            p->getCurrentY() >= inter.origin.y + inter.size.height) {
          if (tgid > 0) {
            if (isOneWay(tgid)) {
              if ((int)p->oneWayCancelY == inter.origin.y + inter.size.height) {
                p->setOnGround(false);

                break;
              }
              lastClearY = rect.origin.y;
            }

            //                            p->desiredPosition.y +=
            //                            inter.size.height;
            //                        temp = inter.size.height;
            p->desiredPosition.y += inter.size.height;
            //                        p->setOnGround(true);
            //                        p->velocity.y = 0;
            p->velocity = Vec2(p->velocity.x * 0.8, -p->velocity.y * 0.6);

            break;
          }
        }
      }
    }
    pos = pos - Vec2(TILE_SIZE / 2, 0);
  } while (!escape);

  p->setPosition(p->desiredPosition);
}

bool GameScene::checkForAndResolveCollisionsForMissile(Movable *p) {
  p->setPosition(p->desiredPosition); // 7

  TMXTiledMap *map = getTileMap(p->getPosition());
  int gid = 0;
  if (map != nullptr) {
    gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
  }

  if (map == nullptr || !isWay(gid)) {
    /*if (p->velocity.x > 0){
     runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL,
     p->getPosition());//Vec2(intersectionR.origin.x, intersectionR.origin.y +
     intersectionR.size.height/2));//p->getPosition()); }else{
     runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL_LEFT,
     p->getPosition());//Vec2(intersectionR.origin.x +
     intersectionR.size.width,intersectionR.origin.y +
     intersectionR.size.height/2));//p->getPosition());
     }*/
    runEffect(EFFECT_BULLET_WALL_SMALL, p->getPosition(), p->getRotation());

    bulletWasted = true;

    if (heroMissileArray.find(p) != heroMissileArray.end()) {
      heroMissileArray.eraseObject(p);
    }
    if (enemyMissileArray.find(p) != enemyMissileArray.end()) {
      enemyMissileArray.eraseObject(p);
    }

    if (p->getTag() == UNIT_MISSILE_STRAIGHT)
      straightMovingArray.eraseObject(p);
    if (p->getTag() == UNIT_MISSILE_CHASING) {
      chasingMissileArray.eraseObject(p);
    }
    if (p->getTag() == UNIT_MISSILE_CUSTOM) {
      //                            customMovingArray.eraseObject(p);
      missileMoveDone(p);
    }
    if (p->getTag() == UNIT_MISSILE_Movable)
      MovableMissileArray.eraseObject(p);

    enemyArray.eraseObject((EnemyBase *)p);
    if (batch->getChildren().find(p) != batch->getChildren().end()) {
      batch->removeChild(p, true);
    }
    if (batch->getChildren().find(p) != batch->getChildren().end()) {
      batch->removeChild(p, true);
    }
    return true;
  }

  return false;
}

void GameScene::showSuccessLayer(float dt) {
  if (collectedCoinCount < MAX_COIN_COUNT) {
    GameManager::getInstance()->addCoin(collectedCoinCount);
  }
  //    GameManager::getInstance()->showInterstitialAds();

  isGameOver = true;
  CCLOG("star count: %d", starCount);

  char buf[50];
  const char *totalStarCountKey = "Tt_SC";
  int totalStarCount =
      UserDefault::getInstance()->getIntegerForKey(totalStarCountKey, 0);

  sprintf(buf, "T%dS%dC", GameManager::getInstance()->theme,
          GameManager::getInstance()->currentStageIndex);
  int previousStar = UserDefault::getInstance()->getIntegerForKey(buf, 0);
  if (starCount > previousStar) {
    int starToAdd = starCount - previousStar;

    sprintf(buf, "T%dS%dC", GameManager::getInstance()->theme,
            GameManager::getInstance()->currentStageIndex);
    UserDefault::getInstance()->setIntegerForKey(buf, starCount);

    totalStarCount += starToAdd;
    UserDefault::getInstance()->setIntegerForKey(totalStarCountKey,
                                                 totalStarCount);
  }

  totalStarCount = GameManager::getInstance()->getTotalStarCount();

  if (starCount >= 3) {
    GameManager::getInstance()->addMastery(
        GameManager::getInstance()->theme * 24 +
        GameManager::getInstance()->currentStageIndex);
  }

  GameManager::getInstance()->getHudLayer()->setVisible(true);
  activityIndicator = Sprite::create("sandglass.png");
  GameManager::getInstance()->getHudLayer()->addChild(activityIndicator);
  activityIndicator->setPosition(Vec2(size.width / 2, size.height / 2));
  activityIndicator->runAction(RepeatForever::create(RotateBy::create(1, 360)));
  GameManager::getInstance()->saveCoin();
  if (starCount >= 3) {
    showStageClearLayer(true);
  } else {
    showStageClearLayer(false);
  }
}

void GameScene::showStageClearLayer(bool threeStars) {
  activityIndicator->setVisible(false);
  activityIndicator->stopAllActions();
  activityIndicator->removeFromParentAndCleanup(true);

  if (GameManager::getInstance()->life >= 0 ||
      UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
    if (!UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET,
                                                   false)) {
      GameManager::getInstance()->life++;
      UserDefault::getInstance()->setIntegerForKey(
          KEY_LIFE, GameManager::getInstance()->life);
    }
  }

  if (enemyKilledCount > enemyMaxCount) {
    enemyKilledCount = enemyMaxCount;
  }
}
void GameScene::rateResult(Node *node) {}
void GameScene::onUpdateResultErrorComplete(char const *status,
                                             char const *error) {
  CCLOG("Update result failed. Status: %s, Error: %s", status, error);
  activityIndicator->setVisible(false);
  activityIndicator->stopAllActions();
  activityIndicator->removeFromParentAndCleanup(true);
}

void GameScene::gameClearAnimationDone(Ref *obj) {
  //    CCLOG("Show stage clear popup");
}

int GameScene::getMaxLoadedBulletCount(int weapon) {
  switch (weapon) {
  case WEAPON_PISTOL: // 20 p/s
    return 10;
  case WEAPON_MAGNUM: // 25
    return 8;
  case WEAPON_UZI: // 34
    return 20;
  case WEAPON_RIFLE: // 25
    return 5;
  case WEAPON_FIRE_BOTTLER: // 42
    return 5;
  case WEAPON_ANTI_BAT: // 60
    return 10;
  case WEAPON_SNIPER_RIFLE: // 36
    return 2;
  case WEAPON_RIFLE_GRENADE: // 35
    return 5;
  case WEAPON_CROSS_BOW: // 49
    return 8;
  case WEAPON_MACHINE_GUN: // 110
    return 30;
  case WEAPON_STRIKER: // 61
    return 4;
  case WEAPON_NIFE_THROWER: // 88
    return 10;
  case WEAPON_BALL_SHOOTER: // 111
    return 15;
  case WEAPON_FLAME_THROWER: // 207
    return 100;
  case WEAPON_ROCKET_LAUNCHER: // 132
    return 4;
  case WEAPON_BAZOOKA: // 44
    return 2;
  case WEAPON_MATT_A4: // 164
    return 7;
  case WEAPON_LASER_GUN: // 161
    return 150;
  case WEAPON_ENERGY_GUN: // 85
    return 4;
  case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
    return 3;
  case WEAPON_SLUGGER: // 171
    return 10;
  case WEAPON_DRAGON_RIFLE: // 282
    return 9;
  case WEAPON_SLUGGER_NG: // 171
    return 9;
  case WEAPON_LIGHTNING_GUN:
    return 5;
  case WEAPON_SHARK_GUN:
    return 4;
  case WEAPON_LASER_COIN:
    return 150;
  default:
    return 8;
  }
}
int GameScene::getMaxTotalBulletCount(int weapon) {
  switch (weapon) {
  case WEAPON_PISTOL: // 20 p/s
    return -1;
  case WEAPON_MAGNUM: // 25
    return 24;
  case WEAPON_UZI: // 34
    return 100;
  case WEAPON_RIFLE: // 25
    return 25;
  case WEAPON_FIRE_BOTTLER: // 42
    return 30;
  case WEAPON_ANTI_BAT: // 60
    return 80;
  case WEAPON_SNIPER_RIFLE: // 36
    return 10;
  case WEAPON_RIFLE_GRENADE: // 35
    return 25;
  case WEAPON_CROSS_BOW: // 49
    return 40;
  case WEAPON_MACHINE_GUN: // 110
    return 100;
  case WEAPON_STRIKER: // 61
    return 20;
  case WEAPON_NIFE_THROWER: // 88
    return 50;
  case WEAPON_BALL_SHOOTER: // 111
    return 75;
  case WEAPON_FLAME_THROWER: // 207
    return 400;
  case WEAPON_ROCKET_LAUNCHER: // 132
    return 12;
  case WEAPON_BAZOOKA: // 44
    return 8;
  case WEAPON_MATT_A4: // 164
    return 35;
  case WEAPON_LASER_GUN: // 161
    return 250;
  case WEAPON_ENERGY_GUN: // 85
    return 20;
  case WEAPON_GUIDED_ROCKET_LAUNCHER: // 107
    return 15;
  case WEAPON_SLUGGER: // 171
    return 50;
  case WEAPON_DRAGON_RIFLE: // 282
    return 45;
  case WEAPON_SLUGGER_NG: // 171
    return 45;
  case WEAPON_LIGHTNING_GUN:
    return 25;
  case WEAPON_SHARK_GUN:
    return 12;
  case WEAPON_LASER_COIN:
    return 200;
  default:
    return 40;
  }
}
void GameScene::playReloadingSoundLater(float dt) {}
// int testWeaponIndex = WEAPON_LASER_GUN - 1;
void GameScene::reload() {}
void GameScene::fire() {}
void GameScene::missileEffectUpdate(float dt) {
  missileEffectCollapsedTime += dt;
  if (missileEffectCollapsedTime < 0.053) {
    return;
  }
  missileEffectCollapsedTime = 0;
  Vector<Movable *> doneArray;
  bool checkChange = false;
  do {
    checkChange = false;
    for (auto drop : heroMissileArray) {
      if (drop->effectType == MISSILE_EFFECT_SMOKE)
        runEffect(EFFECT_SMOKE, drop->getPosition());
      if (drop->effectType == MISSILE_EFFECT_RED_BALL)
        runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_GREEN_SMOKE)
        runEffect(EFFECT_GREEN_SMOKE, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_RED_SMOKE)
        runEffect(EFFECT_RED_SMOKE, drop->getPosition());
      else if (drop->effectType == MISSILE_EFFECT_SHARK_GUN) {
      }
    }
  } while (checkChange);
  doneArray.clear();
}
void GameScene::missileMoveDone(Ref *obj) {
  EnemyBase *missile = (EnemyBase *)obj;
  heroMissileArray.eraseObject(missile);
  enemyMissileArray.eraseObject(missile);
  customMovingArray.eraseObject(missile);
  missile->stopAllActions();
  missile->removeFromParentAndCleanup(true);
}
void GameScene::fireBulletMoveDone(Ref *obj) {
  Movable *missile = (Movable *)obj;
  heroMissileArray.eraseObject(missile);
  customMovingArray.eraseObject(missile);
  missile->stopAllActions();
  missile->removeFromParentAndCleanup(true);
}
void GameScene::enemyBaseMoveDone(Ref *obj) {
  if (obj == NULL) {
    return;
  }
  EnemyBase *sprite = (EnemyBase *)obj;
  sprite->stopAllActions();
  enemyArray.eraseObject(sprite);
  sprite->removeFromParentAndCleanup(true);
}
void GameScene::coinWaitDone(Ref *obj) {
  Movable *drop = (Movable *)obj;
  coinArray.eraseObject(drop);
  MovableCoinArray.eraseObject(drop);
  drop->runAction(
      Sequence::create(FadeOut::create(1),
                       CallFuncN::create(CC_CALLBACK_1(
                           GameScene::removeFromParentAndCleanup, drop)),
                       NULL));
}
void GameScene::spriteMoveDone(Ref *obj) {
  if (obj == NULL) {
    return;
  }
  Sprite *sprite = (Sprite *)obj;
  sprite->stopAllActions();
  sprite->removeFromParentAndCleanup(true);
}
void GameScene::nodeMoveDone(Ref *obj) {
  if (obj == NULL) {
    return;
  }
  Label *lbl = (Label *)obj;

  lbl->stopAllActions();
  lbl->removeFromParentAndCleanup(true);
}

void GameScene::blinkingPlayer(float dt) { blinkingTime -= dt; }

void GameScene::pauseLayer() {
  if (isMultiplay) {
    //        for(auto hero : enemyArray){
    //            if(hero->unitType == UNIT_WORKER){
    //                for(auto mutual: mutualArray){
    //                    if(mutual->unitID == 520){
    //                        setWorkerToTree(hero, mutual);
    //                        break;
    //                    }
    //                }
    //                break;
    //            }
    //        }
    return;
  }
  for (auto spt : enemyArray) {
    if (spt)
      spt->pause();
  }

  for (auto spt : heroMissileArray) {
    if (spt)
      spt->pause();
  }

  for (auto spt : enemyMissileArray) {
    if (spt)
      spt->pause();
  }

  for (auto spt : dropItemArray) {
    if (spt)
      spt->pause();
  }
  for (auto spt : MovableArray) {
    if (spt) {
      spt->pause();
      spt->pauseProcess();
    }
  }
  for (auto spt : heroArray) {
    if (spt) {
      spt->pause();
      spt->pauseProcess();
    }
  }

  isPaused = true;
  //    this->pause();
}

void GameScene::resumeLayer() {
  for (auto spt : enemyArray) {
    if (spt)
      spt->resume();
  }

  for (auto spt : heroMissileArray) {
    if (spt)
      spt->resume();
  }

  for (auto spt : enemyMissileArray) {
    if (spt)
      spt->resume();
  }

  for (auto spt : dropItemArray) {
    if (spt)
      spt->resume();
  }
  for (auto spt : MovableArray) {
    if (spt) {
      spt->resume();
      spt->resumeProcess();
    }
  }
  for (auto spt : heroArray) {
    if (spt) {
      spt->resume();
      spt->resumeProcess();
    }
  }

  isPaused = false;
  //    this->resume();
}

void GameScene::refreshPlayerInfo(int costum, int weapon) {}

void GameScene::receivingData(float dt) {}
void GameScene::removeCharacter(EnemyBase *unit) {
  if (unit->sptWeapon != nullptr) {
    unit->sptWeapon->removeFromParent();
  }
  if (unit->ndLevelCircle) {
    unit->ndLevelCircle->removeFromParent();
  }
  if (selectedArray.find(unit) != selectedArray.end()) {
    unit->showSelectedCircle(false);
  }
  MovableArray.eraseObject(unit);
  heroArray.eraseObject(unit);
  enemyArray.eraseObject(unit);
  unit->removeFromParent();
}
void GameScene::showTalkText(std::string text, int whoseTalk,
                              std::string npcName) {
  std::string str = "" + text;
  talkText = str;
  talkIndex = 0;
  if (imgTalkBox != nullptr) {
    imgTalkBox->removeFromParent();
    sptPointer->removeFromParent();
  }
  log("text: %s", text.c_str());
  imgTalkBox =
      ImageView::create("talkBoxFrame.png", ImageView::TextureResType::PLIST);
  imgTalkBox->setAnchorPoint(Vec2(0.5, 0));
  float splitPadding = 10;
  imgTalkBox->setCapInsets(
      cocos2d::Rect(splitPadding, splitPadding,
                    imgTalkBox->getContentSize().width - splitPadding * 2,
                    imgTalkBox->getContentSize().height - splitPadding * 2));
  imgTalkBox->setScale9Enabled(true);
  imgTalkBox->setContentSize(cocos2d::Size(100, 10));
  this->addChild(imgTalkBox, 100);

  sptPointer = Sprite::createWithSpriteFrameName("talkBoxPointer.png");
  this->addChild(sptPointer, 100);
  lblTalk = LanguageManager::getInstance()->getLocalizedLabel();
  lblTalk->setScale(imageScale);
  lblTalk->setTextColor(Color4B::WHITE); //(78, 78, 78, 255));
  EnemyBase *finder;
  talkingNpcName = npcName;
  //    talkingNPC = nullptr;
  if (whoseTalk == WHOSE_TALK_HERO) {
    //        imgTalkBox->setPosition(hero->getPosition() + Vec2(0, 100));
    talkingNPC = finder;
  } else if (whoseTalk == WHOSE_TALK_NPC) {
    Vec2 npcPos;
    if (npcName.size() > 0) {
      for (auto npc : npcArray) {
        if (npc->getName().compare(npcName) == 0) {
          npcPos = Vec2(npc->getPositionX(), npc->getBoundingBox().getMaxY());
          talkingNPC = npc;
          break;
        }
      }
    } else {
      npcPos = Vec2(encounteredNPC->getPositionX(),
                    encounteredNPC->getBoundingBox().getMaxY());
    }
    imgTalkBox->setPosition(npcPos + Vec2(0, 5));
    //        imgTalkBox->setColor(Color3B(188, 188, 188));
    //        sptPointer->setColor(Color3B(188, 188, 188));
    lblTalk->setTextColor(Color4B::BLACK);
  }

  sptPointer->setAnchorPoint(Vec2(0.5, 1));
  sptPointer->setPosition(imgTalkBox->getPosition() + Vec2(0, 18) * imageScale);

  lblTalkShadow = LanguageManager::getInstance()->getLocalizedLabel();
  lblTalkShadow->setScale(imageScale);
  lblTalkShadow->setTextColor(Color4B(178, 78, 78, 255));

  bool isYell = false;
  if (talkText.find("(yell)") != std::string::npos) {
    isYell = true;
    talkText = talkText.substr(0, talkText.find("(yell)"));
  }
  if (isYell) {
    GM->shakeIt(imgTalkBox, 12, 10);
    imgTalkBox->setScale(1.4f);
    isTalkBoxInCustomMoving = true;
  } else {
    //        imgTalkBox->setScale(imageScale);
    isTalkBoxInCustomMoving = false;
  }
  // resize talkbox
  lblTalkShadow->setString(talkText);
  cocos2d::Size talkSize = cocos2d::Size(
      lblTalkShadow->getContentSize().width * lblTalk->getScale() + 10,
      lblTalkShadow->getContentSize().height * lblTalk->getScale());
  float width = 620;
  if (talkSize.width > width) {
    lblTalkShadow->setWidth(width / lblTalkShadow->getScale());
  } else {
    lblTalkShadow->setWidth(talkSize.width / lblTalkShadow->getScale());
  }
  lblTalk->setWidth(lblTalkShadow->getWidth());
  talkSize = cocos2d::Size(
      lblTalkShadow->getContentSize().width * lblTalk->getScale(),
      lblTalkShadow->getContentSize().height * lblTalk->getScale());
  float padding = 30;
  int talkBoxWidth = talkSize.width + padding;
  int minTalkBoxWidth = 300;
  if (talkBoxWidth < minTalkBoxWidth) {
    talkBoxWidth = minTalkBoxWidth;
  }
  imgTalkBox->setContentSize(
      cocos2d::Size(talkBoxWidth, talkSize.height + padding));
  lblTalk->setPosition(Vec2(imgTalkBox->getContentSize().width / 2,
                            padding / 2 + talkSize.height / 2));
  lblTalkShadow->setPosition(Vec2(imgTalkBox->getContentSize().width / 2,
                                  padding / 2 + talkSize.height / 2));
  // resize talkbox end

  imgTalkBox->addChild(lblTalk);

  this->schedule(schedule_selector(GameScene::talkBoxUpdate));

  // event script
  if (HUD->eventIndex == 0) {
    if (HUD->talkIndex == 15) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("undead") == 0) {
          float moveDistance = 3.0f;
          unit->runAction(Sequence::create(
              MoveBy::create(
                  5, Vec2(-TILE_SIZE * moveDistance, TILE_SIZE * moveDistance)),
              CallFunc::create(
                  CC_CALLBACK_0(GameScene::onUndeadMoveDone, this)),
              nullptr));
          imgTalkBox->runAction(MoveBy::create(
              5, Vec2(-TILE_SIZE * moveDistance, TILE_SIZE * moveDistance)));
          sptPointer->runAction(MoveBy::create(
              5, Vec2(-TILE_SIZE * moveDistance, TILE_SIZE * moveDistance)));
          unit->runAnimation("wizardRun", true);
        }
      }
    } else if (HUD->talkIndex >= 16 &&
               HUD->talkIndex <= 21) { // create portal for john event
      int counter = 0;
      for (auto unit : unitsToCreateArray) {
        if (counter % 6 == HUD->talkIndex - 16 &&
            npcArray.find(unit) == npcArray.end() &&
            unit->unitType != UNIT_TREE) {
          unit->stopAllActions();
          unit->runAnimation(unit->isEnemy ? "zombie orc axeStand"
                                           : "zombie swordmanStand",
                             true);
        }
        counter++;
      }
    } else if (HUD->talkIndex == 28) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("hero") == 0) {
          unit->setFlippedX(true);
        }
      }
    } else if (HUD->talkIndex == 29) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("orc") == 0) {
          unit->setFlippedX(false);
        }
      }
    } else if (HUD->talkIndex == 30) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("hero") == 0) {
          unit->setFlippedX(false);
        }
        if (unit->getName().compare("orc") == 0) {
          unit->setFlippedX(true);
        }
      }
    }
  } else if (HUD->eventIndex == 1) {
    if (HUD->talkIndex == 0) {
      int moveTileCount = 12;
      int dur = 8;
      for (auto unit : unitsToCreateArray) {
        if (unit->alliSide == WHICH_SIDE_HERO) {
          npcArray.pushBack(unit);
          std::string name = unit->getName();
          if (name.compare("hero") == 0) {
            name = "swordman";
          }
          unit->runAnimation(strmake("%sRun", name.c_str()).c_str(), true);
          unit->runAction(Sequence::create(
              MoveBy::create(dur, Vec2(-TILE_SIZE * moveTileCount,
                                       -TILE_SIZE * moveTileCount)),
              CallFunc::create(CC_CALLBACK_0(GameScene::onNPCMoveDone, this)),
              nullptr));
        }
      }
      for (auto unit : npcArray) {
        if (unit->getName().compare("orc") != 0) {
          if (unit->getName().compare("undead") == 0) {
            unit->runAnimation("wizardRun", true);
            unit->runAction(Sequence::create(
                MoveBy::create(dur, Vec2(-TILE_SIZE * (moveTileCount - 2),
                                         -TILE_SIZE * (moveTileCount - 2))),
                CallFunc::create(
                    CC_CALLBACK_0(GameScene::onUndeadMoveDone, this)),
                nullptr));
          } else if (unit->alliSide == WHICH_SIDE_HERO) {
            //                        unit->runAnimation(strmake("%sRun",
            //                        unit->getName().c_str()).c_str(), true);
            //                        unit->runAction(Sequence::create(MoveBy::create(5,
            //                        Vec2(-TILE_SIZE*4, -TILE_SIZE*4)),
            //                        CallFunc::create(CC_CALLBACK_0(GameScene::onNPCMoveDone,
            //                        this)), nullptr));
          }
        }
      }
      isCameraInCustomMoving = true;
      this->runAction(MoveBy::create(
          dur, Vec2(TILE_SIZE * moveTileCount, TILE_SIZE * moveTileCount)));
    } else if (HUD->talkIndex == 7) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("undead") == 0) {
          unit->setFlippedX(true);
        }
      }
    } else if (HUD->talkIndex == 14) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("undead") == 0) {
          unit->setFlippedX(false);
        }
      }
    } else if (HUD->talkIndex == 17) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("undead") == 0) {
          unit->setFlippedX(true);
        }
      }
    } else if (HUD->talkIndex >= 16 && HUD->talkIndex <= 21) {
      int counter = 0;
      log("talkIndex: %d", HUD->talkIndex);
      for (auto unit : unitsToCreateArray) {
        if (counter % 6 == HUD->talkIndex - 16 &&
            npcArray.find(unit) == npcArray.end()) {
          unit->stopAllActions();
          unit->runAnimation(unit->isEnemy ? "zombie orc axeStand"
                                           : "zombie swordmanStand",
                             true);
        }
        counter++;
      }
    } else if (HUD->talkIndex == 26) {
      for (auto unit : npcArray) {
        if (unit->getName().compare("undead") == 0) {
          GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
          runEffect(EFFECT_EXPLODE_MIDDLE, unit->getPosition());
          unit->setVisible(false);
        }
      }
    }
  }
}
void GameScene::onNPCMoveDone() {
  for (auto unit : npcArray) {
    if (unit->getName().compare("hero") == 0) {
      unit->runAnimation("swordmanStand", true);
    } else if (unit->getName().compare("archer") == 0) {
      unit->runAnimation("archerStand", true);
    }
  }
}
void GameScene::onUndeadMoveDone() {
  for (auto unit : npcArray) {
    if (unit->getName().compare("undead") == 0) {
      unit->runAnimation("wizardStand", true);
    }
  }
}
void GameScene::changeToSuperAdin() {
  this->unschedule(schedule_selector(GameScene::creatingStarToGreatBall));
  greatBall->removeFromParent();
  addSuperEffectToAdin();
}
void GameScene::addSuperEffectToAdin() {}

void GameScene::talkBoxUpdate(float dt) {
  for (auto npc : npcArray) {
    if (npc->getName().compare(talkingNpcName) == 0 && HUD->talkIndex < 22 &&
        !isTalkBoxInCustomMoving) {
      Vec2 npcPos = Vec2(npc->getPositionX(), npc->getBoundingBox().getMaxY());
      imgTalkBox->setPosition(npcPos + Vec2(0, 5));
      sptPointer->setPosition(imgTalkBox->getPosition() +
                              Vec2(0, 18) * imageScale);
      break;
    }
  }
  if (talkIndex >= talkText.size()) {
    if (imgTalkBox->getChildByName("TOUCH") == nullptr) {
      //            Sprite* sptAPress =
      //            Sprite::createWithSpriteFrameName("aPressed.png");
      //            imgTalkBox->addChild(sptAPress);
      //            sptAPress->setName("aPress");
      //            GameManager::getInstance()->runAnimation(sptAPress,
      //            "aPressAni", true);
      //            sptAPress->setPosition(Vec2(imgTalkBox->getContentSize().width
      //            - 5, -5));

      Label *lbl =
          LM->getLocalizedLabel(LM->getText("next").c_str(), Color4B::WHITE);
      lbl->enableShadow();
      imgTalkBox->addChild(lbl);
      lbl->setName("TOUCH");
      lbl->setScale(imageScale);
      lbl->setVisible(false);
      lbl->setColor(Color3B::WHITE);
      lbl->setPosition(Vec2(imgTalkBox->getContentSize().width -
                                lbl->getContentSize().width / 2,
                            -lbl->getContentSize().height / 2));
      lbl->runAction(RepeatForever::create(Blink::create(1, 1)));
      lblTalk->setString(talkText);
      return;
    }
  }
  std::string strPre = lblTalk->getString();
  lblTalk->setString(talkText.substr(0, talkIndex));
  if (lblTalk->getContentSize().width == 0) {
    lblTalk->setString(strPre);
  }
  talkIndex++;
}
void GameScene::endEvent() {
  isCameraInCustomMoving = false;
  extraCameraPos = Vec2(0, 0);
  this->unschedule(schedule_selector(GameScene::talkBoxUpdate));
  if (imgTalkBox != nullptr) {
    imgTalkBox->removeFromParent();
    imgTalkBox = nullptr;
    sptPointer->removeFromParent();
  }

  isInEvent = false;
  //    updateNPCQuestState(encounteredNPC);
  this->unschedule(schedule_selector(GameScene::eventUpdate));
}
int GameScene::getHeroLevel(int slot) {
  return UD->getIntegerForKey(strmake(KEY_HERO_LEVEL_FORMAT, slot).c_str(), 0);
}
int GameScene::getHeroExp(int slot) {
  return UD->getIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), 0);
}
void GameScene::addHeroExp(int slot, int exp) {}
int GameScene::getMaxExp(int level) {
  if (level < 10) {
    return 30 * (level + 1);
  } else if (level < 30) {
    return 30 * (level + 1) + 1247 * (level - 10);
  } else if (level < 40) {
    return 30 * (level + 1) + 1247 * 20 + 1247 * (level - 30) * 5;
  } else {
    return 30 * (level + 1) + 1247 * 20 + 1247 * 10 * 5 +
           10247 * 3 * (level - 40 + 10);
  }
}
int GameScene::getHeroMaxLevel(int slot) {
  std::string tier = UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0");
  std::string maxLevel = getEvolutionStat(tier, "max level");
  return Value(maxLevel).asInt();
}
std::string GameScene::getHeroName(int slot) {
  return UD->getStringForKey(strmake(KEY_HERO_NAME_FORMAT, slot).c_str(), "");
}
int GameScene::getHeroMaxHP(int slot) {
  int tier =
      Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
  //    return getUnitMaxHP(getHeroName(slot), getHeroLevel(slot))*(1 +
  //    0.1f*tier) + getHelmetStat(getHelmet(slot), "hp");
  return 60;
}
int GameScene::getHeroAP(int slot) {
  int tier =
      Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
  //    return getUnitAP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier)
  //    + getWeaponStat(getWeapon(slot), "ap") + getHelmetStat(getHelmet(slot),
  //    "ap");
  return 10;
}
int GameScene::getHeroSP(int slot) {
  int tier =
      Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
  //    return getUnitSP(getHeroName(slot))*(1 + 0.1f*tier) +
  //    getShoesStat(getShoes(slot), "sp");
  return 25;
}
int GameScene::getHeroDP(int slot) {
  int tier =
      Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
  //    return getUnitDP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier)
  //    + getHelmetStat(getHelmet(slot), "dp") + getShieldStat(getShield(slot),
  //    "dp");
  return 0;
}
int GameScene::getHeroGP(int slot) {
  return getShieldStat(getShield(0), "gp");
}
void GameScene::updatePlayerStat() {}
std::string GameScene::getHeroWeapon(int slot) {
  return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, slot).c_str(),
                             "sword");
}
std::string GameScene::getHeroHelmet(int slot) {
  return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, slot).c_str(), "");
}
std::string GameScene::getHeroShield(int slot) {
  return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, slot).c_str(), "");
}
std::string GameScene::getHeroShoes(int slot) {
  return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, slot).c_str(), "");
}
int GameScene::getUnitStat(std::string unitName, std::string stat) {
  if (unitStatTable.find(unitName) == unitStatTable.end()) {
    return -1;
  }
  ValueMap row = unitStatTable[unitName].asValueMap();
  return Value(row.at(stat)).asInt();
}
std::string GameScene::getUnitDropItem(std::string unitName, int dropIndex) {
  if (unitStatTable.find(unitName) == unitStatTable.end()) {
    return "";
  }
  ValueMap row = unitStatTable[unitName].asValueMap();
  return Value(row.at(strmake("drop %d", dropIndex))).asString();
}
int GameScene::getWeaponStat(std::string name, std::string stat) {
  if (name.size() == 0 || weaponStatTable[name].asValueMap().find(stat) ==
                              weaponStatTable[name].asValueMap().end())
    return 0;
  return Value(weaponStatTable[name].asValueMap().at(stat)).asInt();
}
std::string GameScene::getItemStat(int itemType, std::string name,
                                    std::string stat) {
  ValueMap valueMap;
  if (itemType == ITEM_TYPE_ETC) {
    valueMap = itemStatTable[name].asValueMap();
  } else if (itemType == ITEM_TYPE_SHIELD) {
    valueMap = shieldStatTable[name].asValueMap();
  } else if (itemType == ITEM_TYPE_WEAPON) {
    valueMap = weaponStatTable[name].asValueMap();
  } else if (itemType == ITEM_TYPE_HELMET) {
    valueMap = helmetStatTable[name].asValueMap();
  } else if (itemType == ITEM_TYPE_SHOES) {
    valueMap = shoesStatTable[name].asValueMap();
  }
  if (valueMap.find(stat) == valueMap.end()) {
    return "";
  } else {
    return Value(valueMap.at(stat)).asString();
  }
}
std::string GameScene::getEvolutionStat(std::string name, std::string stat) {
  ValueMap valueMap = evolutionTable[name].asValueMap();
  if (valueMap.find(stat) == valueMap.end()) {
    return "";
  } else {
    return Value(valueMap.at(stat)).asString();
  }
}
std::string GameScene::getItemStat(std::string name, std::string stat) {
  int itemType = getItemType(name);
  return getItemStat(itemType, name, stat);
  //    if(name.size() == 0) return 0;
  //    return Value(itemStatTable[name].asValueMap().at(stat)).asInt();
}
int GameScene::getItemPrice(int itemType, std::string name) {
  if (itemType == ITEM_TYPE_ETC) {
    return Value(itemStatTable[name].asValueMap().at("price")).asInt();
  } else if (itemType == ITEM_TYPE_SHIELD) {
    return Value(shieldStatTable[name].asValueMap().at("price")).asInt();
  } else if (itemType == ITEM_TYPE_WEAPON) {
    return Value(weaponStatTable[name].asValueMap().at("price")).asInt();
  } else if (itemType == ITEM_TYPE_HELMET) {
    return Value(helmetStatTable[name].asValueMap().at("price")).asInt();
  } else if (itemType == ITEM_TYPE_SHOES) {
    return Value(shoesStatTable[name].asValueMap().at("price")).asInt();
  }
  return -1;
}
std::string GameScene::getItemSpriteName(int itemType, std::string name) {
  if (itemType == ITEM_TYPE_ETC) {
    return Value(itemStatTable[name].asValueMap().at("sprite")).asString();
  } else if (itemType == ITEM_TYPE_SHIELD) {
    return Value(shieldStatTable[name].asValueMap().at("sprite")).asString();
  } else if (itemType == ITEM_TYPE_WEAPON) {
    return Value(weaponStatTable[name].asValueMap().at("sprite")).asString();
  } else if (itemType == ITEM_TYPE_HELMET) {
    return Value(helmetStatTable[name].asValueMap().at("sprite")).asString();
  } else if (itemType == ITEM_TYPE_SHOES) {
    return Value(shoesStatTable[name].asValueMap().at("sprite")).asString();
  }
  return "";
}
int GameScene::getShieldStat(std::string name, std::string stat) {
  if (name.size() == 0 || shieldStatTable[name].asValueMap().find(stat) ==
                              shieldStatTable[name].asValueMap().end())
    return 0;
  return Value(shieldStatTable[name].asValueMap().at(stat)).asInt();
}
int GameScene::getShoesStat(std::string name, std::string stat) {
  if (name.size() == 0 || shoesStatTable[name].asValueMap().find(stat) ==
                              shoesStatTable[name].asValueMap().end())
    return 0;
  return Value(shoesStatTable[name].asValueMap().at(stat)).asInt();
}
int GameScene::getHelmetStat(std::string name, std::string stat) {
  if (name.size() == 0 || helmetStatTable[name].asValueMap().find(stat) ==
                              helmetStatTable[name].asValueMap().end())
    return 0;
  return Value(helmetStatTable[name].asValueMap().at(stat)).asInt();
}
std::string GameScene::getHelmet(int heroSlot) {
  return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, heroSlot).c_str());
}
std::string GameScene::getWeapon(int heroSlot) {
  return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, heroSlot).c_str());
}
std::string GameScene::getShield(int heroSlot) {
  return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, heroSlot).c_str());
}
std::string GameScene::getShoes(int heroSlot) {
  return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, heroSlot).c_str());
}
void GameScene::movePlayer(int direction) {}
void GameScene::addGold(int amount) {
  if (amount > 0) {
    totalEarnedGold += amount;
  } else {
    totalUsedGold -= amount;
  }
  gold += amount;
  HUD->lblGold->setString(strmake("%d", gold));
}
void GameScene::addLumber(int amount) {
  if (amount > 0) {
    totalEarnedLumber += amount;
  } else {
    totalUsedLumber -= amount;
  }
  lumber += amount;
  HUD->lblLumber->setString(strmake("%d", lumber));
}
void GameScene::addOil(int amount) {
  oil += amount;
  HUD->lblOil->setString(strmake("%d", oil));
}
void GameScene::updateFoodInUse() {
  if (GM->currentStageIndex == STAGE_LOBBY)
    return;
  int foodUse = 0;
  for (auto unit : heroArray) {
    if (!unit->isAlli) {
      foodUse += GM->getFoodUseForUnit(unit->unitType);
    }
  }
  foodInUse = foodUse;
  HUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
}
void GameScene::addFoodMax(int amount) {
  if (GM->currentStageIndex == STAGE_LOBBY) {
    return;
  }
  foodMax += amount;
  if (foodMax > 100) {
    foodMax = 100;
  }
  PPLabel *lbl = HUD->lblFood;
  std::string str = strmake("%d/%d", foodInUse, foodMax);
  lbl->setString(str);
}
int GameScene::getGoldPriceForUnit(int index) {
  if (index == UNIT_CASTLE) {
    return 1200;
  } else if (index == UNIT_FARM) {
    return 400;
  } else if (index == UNIT_WATCHERTOWER) {
    return 600;
  } else if (index == UNIT_LUMBERMILL) {
    return 900;
  } else if (index == UNIT_FACTORY) {
    return 1000;
  } else if (index == UNIT_AIRPORT) {
    return 600;
  } else if (index == UNIT_BARRACKS) {
    return 800;
  } else if (index == UNIT_WORKER) {
    return 200;
  } else if (index == UNIT_GOBLIN_WORKER) {
    return 200;
  } else if (index == UNIT_SWORDMAN) {
    return 300;
  } else if (index == UNIT_GOBLIN) {
    return 230;
  } else if (index == UNIT_GOBLIN_BOMB) {
    return 300;
  } else if (index == UNIT_ORC_AXE) {
    return 250;
  } else if (index == UNIT_ORC_SPEAR) {
    return 150;
  } else if (index == UNIT_TROLL) {
    return 550;
  } else if (index == UNIT_WIZARD) {
    return 250;
  } else if (index == UNIT_ARCHER) {
    return 200;
  } else if (index == UNIT_CATAPULT) {
    return 500;
  } else if (index == UNIT_HELICOPTER) {
    return 150;
  } else if (index == UNIT_ORC_HQ) {
    return 1100;
  } else if (index == UNIT_ORC_BUNKER) {
    return 700;
  } else if (index == UNIT_ORC_BARRACKS) {
    return 900;
  } else if (index == UNIT_BARBECUE) {
    return 500;
  } else if (index == UNIT_TEMPLE) {
    return 1500;
  } else if (index == UNIT_ORC_TROLL_HOUSE) {
    return 1000;
  } else if (index == UNIT_HUMAN_SHIPYARD) {
    return 1000;
  } else if (index == UNIT_HUMAN_SHUTTLE) {
    return 400;
  } else if (index == UNIT_HUMAN_SHIP) {
    return 700;
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return 1100;
  } else if (index == UNIT_ORC_SHIPYARD) {
    return 1000;
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    return 1100;
  } else if (index == UNIT_HUMAN_OIL_SHIP) {
    return 500;
  } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
    return 800;
  } else if (index == UNIT_HUMAN_FOUNDRY) {
    return 900;
  } else if (index == UNIT_HUMAN_OIL_REFINERY) {
    return 1000;
  } else if (index == UNIT_ORC_OIL_SHIP) {
    return 500;
  } else if (index == UNIT_ORC_OIL_EXTRACTOR) {
    return 800;
  } else if (index == UNIT_ORC_OIL_REFINERY) {
    return 1000;
  } else if (index == UNIT_ORC_FOUNDRY) {
    return 900;
  }

  return 0;
}
int GameScene::getLumberPriceForUnit(int index) {
  if (index == UNIT_WATCHERTOWER) {
    return 200;
  } else if (index == UNIT_CASTLE) {
    return 800;
  } else if (index == UNIT_FARM) {
    return 200;
  } else if (index == UNIT_BARRACKS) {
    return 400;
  } else if (index == UNIT_LUMBERMILL) {
    return 500;
  } else if (index == UNIT_FACTORY) {
    return 700;
  } else if (index == UNIT_AIRPORT) {
    return 600;
  } else if (index == UNIT_ARCHER) {
    return 150;
  } else if (index == UNIT_GOBLIN_BOMB) {
    return 100;
  } else if (index == UNIT_ORC_AXE) {
    return 80;
  } else if (index == UNIT_ORC_SPEAR) {
    return 180;
  } else if (index == UNIT_TROLL) {
    return 360;
  } else if (index == UNIT_WIZARD) {
    return 550;
  } else if (index == UNIT_CATAPULT) {
    return 400;
  } else if (index == UNIT_HELICOPTER) {
    return 250;
  } else if (index == UNIT_ORC_HQ) {
    return 800;
  } else if (index == UNIT_ORC_BUNKER) {
    return 220;
  } else if (index == UNIT_ORC_BARRACKS) {
    return 350;
  } else if (index == UNIT_BARBECUE) {
    return 150;
  } else if (index == UNIT_TEMPLE) {
    return 800;
  } else if (index == UNIT_ORC_TROLL_HOUSE) {
    return 500;
  } else if (index == UNIT_HUMAN_SHIPYARD) {
    return 600;
  } else if (index == UNIT_HUMAN_SHUTTLE) {
    return 150;
  } else if (index == UNIT_HUMAN_SHIP) {
    return 300;
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return 500;
  } else if (index == UNIT_ORC_SHIPYARD) {
    return 600;
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    return 500;
  } else if (index == UNIT_HUMAN_OIL_SHIP) {
    return 200;
  } else if (index == UNIT_HUMAN_OIL_EXTRACTOR) {
    return 400;
  } else if (index == UNIT_HUMAN_FOUNDRY) {
    return 500;
  } else if (index == UNIT_HUMAN_OIL_REFINERY) {
    return 600;
  } else if (index == UNIT_ORC_OIL_SHIP) {
    return 200;
  } else if (index == UNIT_ORC_OIL_EXTRACTOR) {
    return 400;
  } else if (index == UNIT_ORC_OIL_REFINERY) {
    return 600;
  } else if (index == UNIT_ORC_FOUNDRY) {
    return 500;
  }
  return 0;
}

int GameScene::getFoodGive(int index) {
  if (index == UNIT_CASTLE) {
    return 6;
  } else if (index == UNIT_FARM) {
    return 7;
  } else if (index == UNIT_BARBECUE) {
    return 6;
  }
  return 0;
}
int GameScene::getOilPriceForUnit(int index) {
  if (index == UNIT_HUMAN_SHIP) {
    return 200;
  } else if (index == UNIT_HUMAN_BATTLE_SHIP) {
    return 400;
  } else if (index == UNIT_ORC_SHIP) {
    return 200;
  } else if (index == UNIT_ORC_BATTLE_SHIP) {
    return 400;
  }
  return 0;
}
EnemyBase *GameScene::getNearestOilDepot(cocos2d::Vec2 pos, bool isEnemy) {
  long minDistance = 20000000;
  EnemyBase *nearest = nullptr;
  auto &army = isEnemy ? enemyArray : heroArray;
  for (auto unit : army) {
    if (unit->unitType != UNIT_HUMAN_SHIPYARD &&
        unit->unitType != UNIT_HUMAN_OIL_REFINERY &&
        unit->unitType != UNIT_ORC_SHIPYARD &&
        unit->unitType != UNIT_ORC_OIL_REFINERY)
      continue;
    if (!unit->isBuildingComplete)
      continue;
    long distance = unit->getPosition().distanceSquared(pos);
    if (distance < minDistance) {
      minDistance = distance;
      nearest = unit;
    }
  }
  return nearest;
}
void GameScene::setOilShipToExtractor(EnemyBase *ship, EnemyBase *extractor) {
  ship->isGatheringOil = true;
  ship->isCarryingOil = false;
  ship->currentOilExtractor = extractor;
  ship->oilCarryAmount = 0;
  ship->gatherOil(extractor);
}
bool GameScene::gatherOilWithSelectedOilShip(EnemyBase *extractor) {
  bool found = false;
  for (auto unit : selectedArray) {
    if (unit->unitType != UNIT_HUMAN_OIL_SHIP &&
        unit->unitType != UNIT_ORC_OIL_SHIP)
      continue;
    setOilShipToExtractor(unit, extractor);
    found = true;
  }
  return found;
}
EnemyBase *GameScene::getNearestCastle(cocos2d::Vec2 pos, bool isEnemy) {
  long minDistance = 20000000;
  long distance = 0;
  EnemyBase *nearest = nullptr;
  if (isEnemy) {
    for (auto unit : enemyArray) {
      distance = unit->getPosition().distanceSquared(pos);
      if (unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_ORC_HQ) {
        if (distance < minDistance) {
          minDistance = distance;
          nearest = unit;
        }
      }
    }
  } else {
    for (auto unit : heroArray) {
      distance = unit->getPosition().distanceSquared(pos);
      if (unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_ORC_HQ) {
        if (distance < minDistance) {
          minDistance = distance;
          nearest = unit;
        }
      }
    }
  }
  return nearest;
}
EnemyBase *GameScene::getNearestLumberTank(cocos2d::Vec2 pos, bool isEnemy) {
  long minDistance = 20000000;
  long distance = 0;
  EnemyBase *nearest = nullptr;
  if (isEnemy) {
    for (auto unit : enemyArray) {
      distance = unit->getPosition().distanceSquared(pos);
      if (unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_ORC_HQ ||
          unit->unitType == UNIT_LUMBERMILL) {
        if (distance < minDistance) {
          minDistance = distance;
          nearest = unit;
        }
      }
    }
  } else {
    for (auto unit : heroArray) {
      distance = unit->getPosition().distanceSquared(pos);
      if (unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_ORC_HQ ||
          unit->unitType == UNIT_LUMBERMILL) {
        if (distance < minDistance) {
          minDistance = distance;
          nearest = unit;
        }
      }
    }
  }
  return nearest;
}
EnemyBase *GameScene::getTreeIfTreeIsHere(cocos2d::Vec2 pos) {
  for (auto unit : mutualArray) {
    if (unit->unitType == UNIT_TREE &&
        unit->getBoundingBox().containsPoint((pos))) {
      return unit;
    }
  }
  return nullptr;
}
EnemyBase *GameScene::getNearestTree(cocos2d::Vec2 pos,
                                      Vector<Movable *> excludeList) {
  long minDistance = 20000000;
  long distance = 0;
  EnemyBase *nearest = nullptr;
  bool unreachableTreeExist = false;
  //    bool treeExist = false;
  Vec2 coordinate = getCoordinateFromPosition(pos);
  //    for (int v = 1; v < 100; v++) {
  //        for (int y = -v; y <= v; y++) {
  //            if (y + coordinate.y < 0 || y + coordinate.y >= mapSize.height)
  //            {
  //                continue;
  //            }
  //            if (y == -v || y == v) {
  //                for (int x = -v; x <= v; x++) {
  //                    if (coordinate.x + x >= 0 && coordinate.x + x <
  //                    mapSize.width && isOccupied(coordinate + Vec2(x, y))) {
  //                        EnemyBase* tree =
  //                        getTreeIfTreeIsHere(getPositionFromTileCoordinate(coordinate.x
  //                        + x, coordinate.y + y)); if (tree != nullptr) {
  //                            Vec2 co =
  //                            WORLD->getCoordinateFromPosition(tree->getApproachingPoint(pos));
  //                            Vec2 destCoordinate =
  //                            WORLD->getCoordinateFromPosition(pos);
  //                            PointArray* array = GM->getPath(co,
  //                            destCoordinate); if (array->count() > 0) {
  //                                return tree;
  //                            }
  //                        }
  //                    }
  //                }
  //            }else{
  //                if (coordinate.x - v >= 0 && isOccupied(coordinate +
  //                Vec2(-v, y))) {
  //                    EnemyBase* tree =
  //                    getTreeIfTreeIsHere(getPositionFromTileCoordinate(coordinate.x
  //                    - v, coordinate.y + y)); if (tree != nullptr) {
  //                        Vec2 co =
  //                        WORLD->getCoordinateFromPosition(tree->getApproachingPoint(pos));
  //                        Vec2 destCoordinate =
  //                        WORLD->getCoordinateFromPosition(pos); PointArray*
  //                        array = GM->getPath(co, destCoordinate); if
  //                        (array->count() > 0) {
  //                            return tree;
  //                        }
  //                    }
  //                }
  //                if (coordinate.x + v < mapSize.width &&
  //                isOccupied(coordinate + Vec2(v, y))) {
  //                    EnemyBase* tree =
  //                    getTreeIfTreeIsHere(getPositionFromTileCoordinate(coordinate.x
  //                    + v, coordinate.y + y)); if (tree != nullptr) {
  //                        Vec2 co =
  //                        WORLD->getCoordinateFromPosition(tree->getApproachingPoint(pos));
  //                        Vec2 destCoordinate =
  //                        WORLD->getCoordinateFromPosition(pos); PointArray*
  //                        array = GM->getPath(co, destCoordinate); if
  //                        (array->count() > 0) {
  //                            return tree;
  //                        }
  //                    }
  //                }
  //            }
  //        }
  //    }
  //    return nullptr;
  for (auto unit : mutualArray) {
    distance = unit->getPosition().distanceSquared(pos);
    if (unit->unitType == UNIT_TREE) {
      //            if(distance < minDistance && !isBlockExistBetween(pos,
      //            unit->getPosition())){
      if (distance < minDistance &&
          excludeList.find(unit) == excludeList.end()) {
        //            if(distance < minDistance){
        if (isOccupied(
                getCoordinateFromPosition(unit->getApproachingPoint(pos)))) {
          continue;
        } else {
          //                    Vec2 coordinate =
          //                    WORLD->getCoordinateFromPosition(unit->getApproachingPoint(pos));
          //                    Vec2 destCoordinate =
          //                    WORLD->getCoordinateFromPosition(pos);
          //                    PointArray* array = GM->getPath(coordinate,
          //                    destCoordinate); if (array->count() > 0) {
          minDistance = distance;
          nearest = unit;
          //                    }
        }
      }
      //            treeExist = true;
    }
    //        else if(unit->unitType == UNIT_UNREACHABLE_TREE){
    //            unreachableTreeExist = true;
    //        }
  }
  //    if(!treeExist && unreachableTreeExist){
  //        for(auto unit:mutualArray){
  //            if(unit->unitType == UNIT_UNREACHABLE_TREE){
  //                unit->unitType = UNIT_TREE;
  //            }
  //        }
  //    }
  return nearest;
}
void GameScene::setClearCondition(int stage) {
  int offsetX = 0;
  if (size.height / size.width < 700.0f / 1334.0f) {
    //        offsetX = 100;
  }
  //    Sprite* sptMission = Sprite::create("mission.png");
  //    HUD->addChild(sptMission);
  //    sptMission->setPosition(Vec2(offsetX + 50 +
  //    sptMission->getContentSize().width/2, size.height - 66));
  // CartoonCraft1's per-stage-index objective table (GM->getStageObjective)
  // doesn't apply to MapEditor-authored content - a stage's win condition now
  // comes from its own map: a Victory trigger action if it defines one, else
  // the default of destroying every enemy. Triggers aren't evaluated at
  // runtime yet, so for now this only distinguishes "has one" (defer to the
  // future trigger interpreter, no automatic clear) from "doesn't" (default).
  int condition =
      editorStageHasVictoryTrigger(StringUtils::format("stage%d_0.json", stage))
          ? -1
          : CLEAR_CONDITION_TERMINATE;
  Node *ndMission =
      HUD->rightBottomPanelForCampaign->getChildByName("ndMission");
  int hideFrom = 3;
  if (condition == CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND) {
    clearConditionIndex = condition;
    for (int i = 0; i < 3; i++) {
      //            Label* lbl = LM->getLocalizedLabel();
      //            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE,
      //            true, false, TextHAlignment::LEFT, true);
      //            HUD->addChild(lbl);
      //            lbl->setAnchorPoint(Vec2(0, 0.5));
      //            lbl->setPosition(Vec2(offsetX + 50, size.height - 166 -
      //            80*i));
      //            lblConditionArray.pushBack((Text*)ndMission->getChildByName(strmake("lblCondition%d",
      //            i)));

      //            LM->setLocalizedString(lblConditionArray.at(i), "");
      //            lbl->enableShadow();
    }
  } else if (condition == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
    clearConditionIndex = condition;
    for (int i = 0; i < 2; i++) {
      //            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE,
      //            true, false, TextHAlignment::LEFT, true);
      //            HUD->addChild(lbl);
      //            lbl->setPosition(Vec2(offsetX + 50, size.height - 166 -
      //            80*i));
      //            lblConditionArray.pushBack((Text*)ndMission->getChildByName(strmake("lblCondition%d",
      //            i))); LM->setLocalizedString(lblConditionArray.at(i), "");
      //            lblConditionArray.pushBack(lbl);
    }
    hideFrom = 2;
  } else {
    clearConditionIndex = condition;
    //        PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, true,
    //        false, TextHAlignment::LEFT, true); HUD->addChild(lbl);
    //        lbl->setPosition(Vec2(offsetX + 50, size.height - 166 - 80*0));
    //        lblConditionArray.pushBack((Text*)ndMission->getChildByName("lblCondition0"));
    //        LM->setLocalizedString(lblConditionArray.at(0), "");
    hideFrom = 1;
  }
  for (int i = hideFrom; i < 3; i++) {
    ndMission->getChildByName(strmake("lblCondition%d", i))->setVisible(false);
  }
  if (GM->loadMapData.length() <= 0) {
    checkClearGame(); // test
  }
}
bool GameScene::checkClearGame() {
  if (GM->currentStageIndex == STAGE_SINGLEPLAY ||
      GM->currentStageIndex == STAGE_RAID) {
    if (isGameClear)
      return false;
    bool isCastleExist = false;
    bool isSoldierExist = false;
    for (auto unit : enemyArray) {
      if (unit->unitType == UNIT_CASTLE) {
        isCastleExist = true;
      }
      if (GM->nextScene == STAGE_SINGLEPLAY &&
          unit->unitType == UNIT_ORC_BUNKER) {
        isCastleExist = true;
      }
      if (!unit->isBuilding) {
        isSoldierExist = true;
      }
    }
    int starCountTemp = 0;
    if (enemyArray.size() == 0) {
      starCountTemp++;
    }
    if (!isCastleExist) {
      starCountTemp++;
    }
    if (!isSoldierExist) {
      starCountTemp++;
    }
    while (starCountTemp > starCountForRaid) {
      starCountForRaid++;
      HUD->showStarForRaid(starCountForRaid - 1);
    }
    if (starCountForRaid >= 3) {
      isGameClear = true;
      this->runAction(
          Sequence::create(DelayTime::create(0.1f),
                           CallFunc::create(CC_CALLBACK_0(
                               HudLayer::showRaidResultWithSaveRecord, HUD)),
                           NULL));
    } else {
      if (heroArray.size() == 0 && HUD->isRaidStarted) {
        HUD->showRaidResult(true);
      }
    }

    return false;
  }
  bool isAllClear = false;
  Text *lbl;
  Node *ndMission =
      HUD->rightBottomPanelForCampaign->getChildByName("ndMission");
  if (clearConditionIndex ==
      CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND) {
    isAllClear = true;
    int barracksCount = 0;
    int farmCount = 0;
    int swordmanCount = 0;
    for (auto unit : heroArray) {
      if (unit->unitType == UNIT_SWORDMAN) {
        swordmanCount++;
      } else if (unit->unitType == UNIT_FARM) {
        farmCount++;
      } else if (unit->unitType == UNIT_BARRACKS) {
        barracksCount++;
      }
    }

    bool complete;

    for (int i = 0; i < 3; i++) {
      complete = false;
      if (i == 0) {
        lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", i));
        LM->setLocalizedString(
            lbl,
            strmake("%s(%d/2)", LM->getText("objective build farm").c_str(),
                    farmCount));
        if (farmCount < 2) {
          isAllClear = false;
        } else {
          complete = true;
        }
      } else if (i == 1) {
        lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", i));
        LM->setLocalizedString(
            lbl,
            strmake("%s(%d/1)", LM->getText("objective build barracks").c_str(),
                    barracksCount));
        if (barracksCount < 1) {
          isAllClear = false;
        } else {
          complete = true;
        }
      } else if (i == 2) {
        lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", i));
        LM->setLocalizedString(
            lbl, strmake("%s(%d/4)",
                         LM->getText("objective train swordsman").c_str(),
                         swordmanCount));
        if (swordmanCount < 4) {
          isAllClear = false;
        } else {
          complete = true;
        }
      }
      if (complete) {
        lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", i));
        if (lbl->getTextColor() != Color4B(245, 213, 71, 255)) {
          lbl->setTextColor(Color4B(245, 213, 71, 255));
          Sprite *spt = Sprite::create("check.png");
          lbl->addChild(spt);
          spt->setPosition(Vec2(lbl->getBoundingBox().size.width - 40,
                                lbl->getBoundingBox().size.height / 2));
        }
      }
    }
  } else if (clearConditionIndex == CLEAR_CONDITION_TERMINATE) {
    isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
    //        isAllClear = unitsToCreateArray.size() == 0;
    //        for (int i = 0; i < enemyArray.size(); i++) {
    //            if(enemyArray.at(i)->isVisible()){
    //                isAllClear = false;
    //                break;
    //            }
    //        }
    //
    lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", 0));
    LM->setLocalizedString(lbl, LM->getText("objective destroy them all"));
    if (enemyArray.size() < 5 && enemyArray.size() > 0 &&
        unitsToCreateArray.size() == 0) {
      isRevealingMap = true;
    }
    if (gameTimer > 60 * 40) {
      isRevealingMap = true;
    }
  } else if (clearConditionIndex ==
             CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
    isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
    lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", 0));
    LM->setLocalizedString(lbl, LM->getText("objective protect all"));
    lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", 1));
    LM->setLocalizedString(lbl, LM->getText("objective destroy them all"));
  } else if (clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_10_MIN ||
             clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN) {

    int min = 10;
    if (clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN) {
      min = 20;
    }
    isAllClear = gameTimer > min * 60;
    lbl = (Text *)ndMission->getChildByName(strmake("lblCondition%d", 0));
    LM->setLocalizedString(
        lbl,
        LM->getText(
            strmake(LM->getText("objective survive").c_str(), min).c_str()));
  }

  isGameClear = isAllClear;
  return isAllClear;
}
int GameScene::getAttackPriority(Movable *unit) {
  if (unit == nullptr) {
    return 0;
  }
  if (unit->isBuilding) {
    if (unit->attackType != ATTACK_TYPE_NONE) {
      return 1;
    } else {
      return 0;
    }
  }
  return 2;
}
bool GameScene::canAttack(Movable *attacker, Movable *target) {
  if (!attacker || !target) {
    return true;
  }
  bool can = true;
  if (attacker->unitType == UNIT_HUMAN_OIL_SHIP ||
      attacker->unitType == UNIT_ORC_OIL_SHIP) {
    can = false;
  } else if (attacker->attackType == ATTACK_TYPE_NEAR && target->isFlying) {
    can = false;
  } else if (target->unitType == UNIT_TREE_FOR_BATTLE &&
             attacker->unitAct != UNIT_WIZARD) {
    can = false;
  }
  return can;
}
bool GameScene::isInScreen(cocos2d::Vec2 pos) {
  return cocos2d::Rect(-(getPositionX() + 150) / layerScale,
                       -(getPositionY() + 150) / layerScale,
                       (size.width + 200) / layerScale,
                       (size.height + 200) / layerScale)
      .containsPoint(pos);
}
void GameScene::revengeAttack(Movable *attackee, Movable *attacker) {
  //    if (attackee->unitAct == UNIT_ACT_MOVE || attackee->unitAct ==
  //    UNIT_ACT_ATTACK) {
  if (!attackee->canRevengeAttack) {
    return;
  }
  if (attackee && attackee != nullptr &&
      attackee->unitActDetail == UNIT_ACT_DETAIL_ATTACK && attacker &&
      attacker != nullptr && attacker->target && attacker->target != nullptr &&
      getAttackPriority(attacker) <= getAttackPriority(attackee->target)) {
    return;
  }
  Vector<EnemyBase *> list;
  if (attacker->isEnemy) {
    for (auto unit : heroArray) {
      if (unit && unit != nullptr && !unit->isBuilding &&
          unit->attackType != ATTACK_TYPE_NONE &&
          unit->getPosition().distanceSquared(attackee->getPosition()) <
              300000 &&
          !unit->isGatheringGold && !unit->isGatheringTree &&
          !unit->isGoingToBuild) {
        if (unit->unitAct == UNIT_ACT_ATTACK_DDANG ||
            unit->unitAct == UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH ||
            unit->unitAct == UNIT_ACT_NONE) {
          if (unit->isVisible()) {
            list.pushBack(unit);
            //                        if (isMultiplay) {
            //                            MM->enemyMoveAndAttackTo(Value(unit->unitID).asString(),
            //                            (int)unit->getPositionX(),
            //                            (int)unit->getPositionY());
            //                        }
          }
        }
      }
    }
  } else {
    for (auto unit : enemyArray) {
      if (unit && unit != nullptr && !unit->isBuilding &&
          unit->attackType != ATTACK_TYPE_NONE &&
          unit->getPosition().distanceSquared(attackee->getPosition()) <
              300000 &&
          !unit->isGatheringGold && !unit->isGatheringTree &&
          !unit->isGoingToBuild) {
        list.pushBack(unit);
        //                moveAndAttackTo(unit, attacker->getPosition());
        //                if (isMultiplay) {
        //                    MM->moveAndAttackTo(Value(unit->unitID).asString(),
        //                    (int)unit->getPositionX(),
        //                    (int)unit->getPositionY());
        //                }
      }
    }
  }
  if (list.size() > 0) {

    if (attacker->isBuilding) {
      forceAttack(list, (EnemyBase *)attacker);
      //        moveAndAttackTo(list,
      //        attacker->getApproachingPoint(attackee->getPosition()));
    } else {
      moveAndAttackTo(list, attacker->getPosition());
    }
  }
}
void GameScene::selectAllForces() {
  deselectAll();

  for (auto unit : heroArray) {
    if (unit->isAlli)
      continue;
    if (unit->unitType == UNIT_WORKER || unit->unitType == UNIT_GOBLIN_WORKER)
      continue;
    if (GM->isThisBuilding(unit->unitType))
      continue;
    selectUnit(unit);
  }
}
void GameScene::onSelectForcesInScreen() {
  deselectAll();

  for (auto unit : heroArray) { // select heroes
    if (unit->isAlli) {
      continue;
    }
    if (this->isInScreen(unit->getPosition())) {
      if (unit->unitType == UNIT_SWORDMAN || unit->unitType == UNIT_ARCHER ||
          unit->unitType == UNIT_CATAPULT ||
          unit->unitType == UNIT_HELICOPTER || unit->unitType == UNIT_GOBLIN ||
          unit->unitType == UNIT_GOBLIN_BOMB ||
          unit->unitType == UNIT_ORC_AXE || unit->unitType == UNIT_ORC_SPEAR ||
          unit->unitType == UNIT_TROLL ||
          unit->unitType == UNIT_ZOMBIE_ORC_AXE ||
          unit->unitType == UNIT_ZOMBIE_SWORDSMAN ||
          unit->unitType == UNIT_WIZARD || unit->spine) {
        selectUnit(unit);
      }
    }
  }
}
void GameScene::getSupportFromVideo(int video) { finishedVideo = video; }
Movable *GameScene::getGroundOwner(Vec2 pos) {
  Vec2 coordinate = getCoordinateFromPosition(pos);
  for (auto unit : heroArray) {
    if (unit->isBuilding) {
      cocos2d::Rect rect = cocos2d::Rect(unit->buildingStartCoordinate,
                                         unit->buildingOccupySize);
      if (rect.containsPoint(coordinate)) {
        return unit;
      }
    }
  }
  for (auto unit : enemyArray) {
    if (unit->isBuilding) {
      cocos2d::Rect rect = cocos2d::Rect(unit->buildingStartCoordinate,
                                         unit->buildingOccupySize);
      if (rect.containsPoint(coordinate)) {
        return unit;
      }
    }
  }
  for (auto unit : mutualArray) {
    if (unit->isBuilding) {
      cocos2d::Rect rect = cocos2d::Rect(unit->buildingStartCoordinate,
                                         unit->buildingOccupySize);
      if (rect.containsPoint(coordinate)) {
        return unit;
      }
    } else if (unit->unitType == UNIT_TREE ||
               unit->unitType == UNIT_TREE_FOR_BATTLE) {
      if (getCoordinateFromPosition(unit->getPosition()) == coordinate) {
        return unit;
      }
    }
  }
  return nullptr;
}

bool GameScene::isOrcTypeUnit(int unitType) {
  return unitType == UNIT_GOBLIN_WORKER || unitType == UNIT_GOBLIN ||
         unitType == UNIT_GOBLIN_BOMB || unitType == UNIT_ORC_AXE ||
         unitType == UNIT_ORC_SPEAR || unitType == UNIT_TROLL ||
         unitType == UNIT_ORC_HQ || unitType == UNIT_ORC_BUNKER ||
         unitType == UNIT_ORC_BARRACKS || unitType == UNIT_BARBECUE ||
         unitType == UNIT_TEMPLE || unitType == UNIT_ORC_TROLL_HOUSE ||
         unitType == UNIT_ORC_SHIPYARD || unitType == UNIT_ORC_SHUTTLE ||
         unitType == UNIT_ORC_SHIP || unitType == UNIT_ORC_BATTLE_SHIP ||
         unitType == UNIT_ORC_OIL_SHIP || unitType == UNIT_ORC_OIL_EXTRACTOR ||
         unitType == UNIT_ORC_OIL_REFINERY || unitType == UNIT_ORC_FOUNDRY;
}

void GameScene::startResearch(EnemyBase *foundry, int type) {
  if (foundry->isResearching) return;

  int currentLevel = 0;
  if (type == RESEARCH_HUMAN_ATTACK) currentLevel = humanAttackLevel;
  else if (type == RESEARCH_HUMAN_DEFENSE) currentLevel = humanDefenseLevel;
  else if (type == RESEARCH_ORC_ATTACK) currentLevel = orcAttackLevel;
  else if (type == RESEARCH_ORC_DEFENSE) currentLevel = orcDefenseLevel;

  if (currentLevel >= 2) return;

  int goldCost = (currentLevel == 0) ? 500 : 1000;
  int lumberCost = (currentLevel == 0) ? 250 : 500;

  if (gold < goldCost || lumber < lumberCost) return;

  addGold(-goldCost);
  addLumber(-lumberCost);

  foundry->isResearching = true;
  foundry->researchTimer = 30.0f;
  foundry->researchType = type;

  // Add a progress button so the player can see research in progress.
  std::string dummy = (foundry->unitType == UNIT_ORC_FOUNDRY)
                          ? "orcFoundary.png" : "humanFoundary.png";
  Button *btn = Button::create("uiBox.png");
  foundry->getParent()->getParent()->addChild(btn, 10);
  btn->setTag(type);
  foundry->btns.pushBack(btn);
  btn->setScale(0.4f * imageScale);

  Sprite *sptBg = Sprite::createWithSpriteFrameName(dummy);
  btn->addChild(sptBg);
  sptBg->setPosition(btn->getContentSize() / 2);
  sptBg->setOpacity(50);

  Sprite *sptFg = Sprite::createWithSpriteFrameName(dummy);
  auto progressTimer = ProgressTimer::create(sptFg);
  progressTimer->setType(ProgressTimer::Type::RADIAL);
  progressTimer->setName("timer");
  progressTimer->setScale(2);
  progressTimer->setPosition(btn->getContentSize() / 2);
  progressTimer->setPercentage(0);
  progressTimer->runAction(ProgressTo::create(foundry->researchTimer, 100));
  btn->addChild(progressTimer);

  foundry->updateProductButtons();
  updateMenu();
}

void GameScene::applyResearch(int type) {
  if (type == RESEARCH_HUMAN_ATTACK) humanAttackLevel++;
  else if (type == RESEARCH_HUMAN_DEFENSE) humanDefenseLevel++;
  else if (type == RESEARCH_ORC_ATTACK) orcAttackLevel++;
  else if (type == RESEARCH_ORC_DEFENSE) orcDefenseLevel++;
}

// ─────────────────────────────────────────────────────────────────────────────
// Enemy AI
// ─────────────────────────────────────────────────────────────────────────────

static const int kEAIMaxWorkersPerHQ = 10;
static const int kEAIMaxUnitsPerType = 10;
// Worker/army food prioritization (see enemyAITrainUnits).
// Target food set aside for the combat army (excluding workers). The food
// building count below is derived from this so the AI can actually field an
// army instead of drowning in workers.
static const int kEAIArmyFoodBudget = 50;
// Desired steady-state worker headcount per HQ (each worker eats 1 food).
static const int kEAITargetWorkers = 10;
// Below this many workers per HQ the economy is still bootstrapping and the HQ
// may spend freely on workers. At or above it, worker training must leave
// headroom for combat units (below).
static const int kEAIBootstrapWorkersPerHQ = 6;
// Once bootstrapped, the HQ only trains another worker while free food headroom
// exceeds this and gold exceeds the reserve, so combat buildings win contested
// resources. Combat training ignores these.
static const int kEAIWorkerFoodReserve = 8;
static const int kEAIWorkerGoldReserve = 300;
// Two OilShips per OilSpot - one can be shuttling to/from the depot while
// the other loads at the extractor, keeping oil actually flowing.
static const int kEAIShipsPerExtractor = 2;
// Shipyard fleet composition caps.
static const int kEAIMaxShuttles = 3;      // cheap transport - a few are enough
static const int kEAIMaxShips = 10;        // main naval workhorse
static const int kEAIMaxBattleShips = 5;   // priciest unit - keep the fleet affordable
// Extra tiles of empty space required around a building footprint (and the
// HQ's own footprint) before the AI will place something there, so bases
// spread out instead of packing buildings edge-to-edge.
static const int kEAIBuildGap = 2;
// Every other entry in kHumanBuildList/kOrcBuildList is capped at one per HQ
// (see the "already have one nearby" skip below), but the food building
// (Farm/Barbecue) needs to scale with population - one only ever grants a
// fixed +6/+7 food, so the AI would hit its food cap permanently once that
// single building existed and never train another unit again.
//
// Derived from kEAIArmyFoodBudget so the AI can support workers + a real army.
// Goal per HQ: HQ food (6) + foodBuildings*perBuildingFood
//              >= kEAITargetWorkers*1 (worker food) + kEAIArmyFoodBudget = 60.
//   Human: Farm = +7  -> 6 + 8*7 = 62 >= 60  (needs 8)
//   Orc:   Barbecue = +6 -> 6 + 9*6 = 60 >= 60 (needs 9)
// A single shared cap must satisfy the worst case (Orc), so use 9. That still
// stays under the enemyFoodMax hard cap of 100 (Human 9 farms = 69, Orc 9
// barbecues = 60).
static const int kEAIMaxFoodBuildingsPerHQ = 9;

// ── Rally / wave system (single-player only) ──────────────────────────────────
// AI-trained combat units gather at a rally point near their HQ and hold there
// (still defending locally) until their combined food value crosses a threshold,
// then they release as one attack wave. The threshold ramps so early waves are
// small pokes and later waves are full-army pushes.
static const int   kEAIFirstWaveFood = 8;    // food value that triggers the first wave
static const int   kEAIWaveFoodStep  = 4;    // added to the threshold per wave released
static const int   kEAIMaxWaveFood   = 24;   // threshold ceiling
static const float kEAIWaveTimeout   = 180.0f; // s; release regardless of size (anti-stall)
// How far in front of the HQ (toward the target) the rally point sits.
static const float kEAIRallyOffset   = TILE_SIZE * 6.0f;

void GameScene::addEnemyGold(int amount) {
  enemyGold += amount;
  if (enemyGold < 0) enemyGold = 0;
}

void GameScene::addEnemyLumber(int amount) {
  enemyLumber += amount;
  if (enemyLumber < 0) enemyLumber = 0;
}

void GameScene::addEnemyOil(int amount) {
  enemyOil += amount;
  if (enemyOil < 0) enemyOil = 0;
}

// Recount enemy food from buildings/units (called each AI tick).
void GameScene::enemyAIUpdateFood() {
  int foodMax = 0;
  int foodUse = 0;
  for (auto *u : enemyArray) {
    if (!u || u->energy <= 0) continue;
    if (u->isBuilding && u->isBuildingComplete) {
      if (u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ) foodMax += 6;
      else if (u->unitType == UNIT_FARM)      foodMax += 7;
      else if (u->unitType == UNIT_BARBECUE)  foodMax += 6;
    } else if (!u->isBuilding) {
      foodUse += GM->getFoodUseForUnit(u->unitType);
    }
  }
  enemyFoodMax  = std::min(foodMax, 100);
  enemyFoodInUse = foodUse;
}

// Check whether the building footprint (same coord convention as buildTheBuilding)
// is fully clear of deco/soil blocks. Also requires a kEAIBuildGap-tile margin
// around the footprint to be clear of *other buildings* (deco blocks) - not of
// terrain - so the AI spreads buildings out instead of butting them together.
bool GameScene::enemyAIIsTileFree(int bx, int by, int w, int h) {
  for (int j = -kEAIBuildGap; j < h + kEAIBuildGap; j++) {
    for (int i = -kEAIBuildGap; i < w + kEAIBuildGap; i++) {
      int tx = bx + i;
      int ty = by - j - 1;
      if (tx < 1 || ty < 1 ||
          tx >= (int)mapSize.width  - 1 ||
          ty >= (int)mapSize.height - 1) return false;
      Vec2 coord(tx, ty);
      if (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, coord))) return false;
      bool inFootprint = i >= 0 && i < w && j >= 0 && j < h;
      if (inFootprint && soilLayer && isSoilBlock(getTileGIDAt(soilLayer, coord)))
        return false;
    }
  }
  return true;
}

// Spiral search around nearPos for a free placement tile.
// avoidMines=true keeps the spot at least 6 tiles away from any mine.
bool GameScene::enemyAIFindBuildTile(Vec2 nearPos, int w, int h,
                                     int &outBx, int &outBy, bool avoidMines) {
  Vec2 coord = getCoordinateFromPosition(nearPos);
  int cx = (int)coord.x;
  int cy = (int)coord.y;

  for (int radius = 3; radius <= 22; radius++) {
    for (int dy = -radius; dy <= radius; dy++) {
      for (int dx = -radius; dx <= radius; dx++) {
        if (std::abs(dx) != radius && std::abs(dy) != radius) continue;
        // Footprint bottom row starts at cy+dy; buildTheBuilding uses by-h as bottom row
        int bx = cx + dx;
        int by = cy + dy + h;
        if (!enemyAIIsTileFree(bx, by, w, h)) continue;
        if (avoidMines) {
          Vec2 worldPos = getPositionFromTileCoordinate(bx + w / 2, by - h / 2);
          bool tooClose = false;
          for (auto *m : mutualArray) {
            if (m->unitType != UNIT_MINE) continue;
            if (worldPos.distance(m->getPosition()) < 6.0f * TILE_SIZE) {
              tooClose = true;
              break;
            }
          }
          if (tooClose) continue;
        }
        outBx = bx;
        outBy = by;
        return true;
      }
    }
  }
  return false;
}

// Water-building variant of enemyAIIsTileFree: the footprint must be open
// water (soilLayer marks water tiles - see isWaterTileAt) and not deco-blocked.
bool GameScene::enemyAIIsWaterTileFree(int bx, int by, int w, int h) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int tx = bx + i;
      int ty = by - j - 1;
      if (tx < 1 || ty < 1 ||
          tx >= (int)mapSize.width  - 1 ||
          ty >= (int)mapSize.height - 1) return false;
      Vec2 coord(tx, ty);
      if (!isSoilBlock(getTileGIDAt(soilLayer, coord))) return false;
      if (decoLayer && isDecoBlock(getTileGIDAt(decoLayer, coord))) return false;
    }
  }
  return true;
}

// Spiral search around nearPos for a free water placement tile (Shipyard).
bool GameScene::enemyAIFindWaterBuildTile(Vec2 nearPos, int w, int h,
                                          int &outBx, int &outBy) {
  Vec2 coord = getCoordinateFromPosition(nearPos);
  int cx = (int)coord.x;
  int cy = (int)coord.y;

  for (int radius = 1; radius <= 30; radius++) {
    for (int dy = -radius; dy <= radius; dy++) {
      for (int dx = -radius; dx <= radius; dx++) {
        if (std::abs(dx) != radius && std::abs(dy) != radius) continue;
        int bx = cx + dx;
        int by = cy + dy + h;
        if (!enemyAIIsWaterTileFree(bx, by, w, h)) continue;
        outBx = bx;
        outBy = by;
        return true;
      }
    }
  }
  return false;
}

// ── Worker management ─────────────────────────────────────────────────────────

void GameScene::enemyAIManageWorkers() {
  const float HQ_RADIUS_SQ = (28.0f * TILE_SIZE) * (28.0f * TILE_SIZE);

  // Snapshot to avoid iterator invalidation if units spawn/die during iteration.
  std::vector<EnemyBase *> snap(enemyArray.begin(), enemyArray.end());

  // Collect enemy HQs.
  std::vector<EnemyBase *> hqs;
  for (auto *u : snap) {
    if ((u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ) &&
        u->isBuilding && u->isBuildingComplete && u->energy > 0)
      hqs.push_back(u);
  }
  if (hqs.empty()) return;

  for (auto *hq : hqs) {
    Vec2 hqPos = hq->getPosition();

    std::vector<EnemyBase *> nearWorkers;
    for (auto *u : snap) {
      if (u->isBuilding || u->energy <= 0) continue;
      if (u->unitType != UNIT_WORKER && u->unitType != UNIT_GOBLIN_WORKER) continue;
      if (!u->isEnemy) continue;
      if (hqPos.distanceSquared(u->getPosition()) > HQ_RADIUS_SQ) continue;
      nearWorkers.push_back(u);
    }

    int goldWorkers = 0, woodWorkers = 0;
    std::vector<EnemyBase *> idleWorkers;
    for (auto *w : nearWorkers) {
      if (w->isGatheringGold || w->unitAct == UNIT_ACT_GATHER_GOLD) goldWorkers++;
      else if (w->isGatheringTree || w->unitAct == UNIT_ACT_GATHER_TREE) woodWorkers++;
      else if (!w->isGoingToBuild && w->unitAct == UNIT_ACT_NONE) idleWorkers.push_back(w);
    }

    // Stop handing out jobs once this HQ has kEAIMaxWorkersPerHQ workers
    // committed. assignedCount seeds from the workers already near the HQ and
    // grows as we assign idle ones this tick. (The old guard compared the
    // static nearWorkers.size() once - it never changed inside the loop and was
    // off-by-one, allowing an 11th worker.)
    int assignedCount = (int)nearWorkers.size();
    for (auto *worker : idleWorkers) {
      if (assignedCount >= kEAIMaxWorkersPerHQ) break;

      // Assign returning place so deposits work.
      EnemyBase *tank = getNearestLumberTank(worker->getPosition(), true);
      if (tank) worker->returningPlace = tank;

      bool sendToGold = (goldWorkers <= woodWorkers);
      if (sendToGold) {
        EnemyBase *nearest = nullptr;
        float minD = FLT_MAX;
        for (auto *m : mutualArray) {
          if (m->unitType != UNIT_MINE || m->energy <= 0) continue;
          float d = worker->getPosition().distanceSquared(m->getPosition());
          if (d < minD) { minD = d; nearest = m; }
        }
        if (nearest) { setWorkerToMine(worker, nearest); goldWorkers++; assignedCount++; }
      } else {
        EnemyBase *nearest = nullptr;
        float minD = FLT_MAX;
        for (auto *m : mutualArray) {
          if ((m->unitType != UNIT_TREE && m->unitType != UNIT_TREE_FOR_BATTLE) ||
              m->energy <= 0) continue;
          float d = worker->getPosition().distanceSquared(m->getPosition());
          if (d < minD) { minD = d; nearest = m; }
        }
        if (nearest) { setWorkerToTree(worker, nearest); woodWorkers++; assignedCount++; }
      }
    }
  }
}

// ── Ship management (OilShip) ─────────────────────────────────────────────────

// Assign idle enemy OilShips to an OilExtractor: reuse a completed extractor
// that has no ship working it yet, or claim the nearest unclaimed OilSpot and
// build a new extractor there (mirrors enemyAICheckBuildings' instant-build
// pattern - no travel/construction time, same as every other AI building).
void GameScene::enemyAIManageShips() {
  if (!mapHasWater) return;

  std::vector<EnemyBase *> snap(enemyArray.begin(), enemyArray.end());

  std::vector<EnemyBase *> idleOilShips;
  for (auto *u : snap) {
    if (u->isBuilding || u->energy <= 0 || !u->isEnemy) continue;
    if (u->unitType != UNIT_HUMAN_OIL_SHIP && u->unitType != UNIT_ORC_OIL_SHIP) continue;
    if (u->isGatheringOil || u->isCarryingOil) continue;
    idleOilShips.push_back(u);
  }
  if (idleOilShips.empty()) return;

  // Race is detected once and cached in updateEnemyAI() (see enemyAIIsOrc).
  bool isOrc = enemyAIIsOrc;

  int extractorType = isOrc ? UNIT_ORC_OIL_EXTRACTOR : UNIT_HUMAN_OIL_EXTRACTOR;
  const char *extractorSprite = isOrc ? "orcOilExtractor.png" : "humanOilExtractor.png";
  Vec2 extractorSize = GM->getBuildingOccupySize(extractorType);
  int ew = (int)extractorSize.x, eh = (int)extractorSize.y;
  int gCost = getGoldPriceForUnit(extractorType);
  int lCost = getLumberPriceForUnit(extractorType);
  int oCost = getOilPriceForUnit(extractorType);

  for (auto *ship : idleOilShips) {
    // Prefer a completed enemy extractor that isn't fully staffed yet.
    EnemyBase *bestExtractor = nullptr;
    float bestD = FLT_MAX;
    for (auto *u : snap) {
      if (u->unitType != extractorType || !u->isBuildingComplete || u->energy <= 0) continue;
      int assigned = 0;
      for (auto *s2 : snap) {
        if (s2 == ship) continue;
        if (s2->unitType != UNIT_HUMAN_OIL_SHIP && s2->unitType != UNIT_ORC_OIL_SHIP) continue;
        if ((s2->isGatheringOil || s2->isCarryingOil) && s2->currentOilExtractor == u) {
          assigned++;
        }
      }
      if (assigned >= kEAIShipsPerExtractor) continue;
      float d = ship->getPosition().distanceSquared(u->getPosition());
      if (d < bestD) { bestD = d; bestExtractor = u; }
    }
    if (bestExtractor) {
      setOilShipToExtractor(ship, bestExtractor);
      continue;
    }

    // No free extractor - claim the nearest OilSpot and build one there.
    if (enemyGold < gCost || enemyLumber < lCost || enemyOil < oCost) continue;

    // Try oil spots nearest-first; skip ones already tried this pass so a
    // build failure (e.g. spot already occupied) falls through to the next.
    std::vector<EnemyBase *> tried;
    while (true) {
      EnemyBase *nearestSpot = nullptr;
      float bestSpotD = FLT_MAX;
      for (auto *m : mutualArray) {
        if (m->unitType != UNIT_OIL_SPOT || m->energy <= 0) continue;
        bool alreadyTried = false;
        for (auto *t : tried) if (t == m) { alreadyTried = true; break; }
        if (alreadyTried) continue;
        float d = ship->getPosition().distanceSquared(m->getPosition());
        if (d < bestSpotD) { bestSpotD = d; nearestSpot = m; }
      }
      if (!nearestSpot) break;
      tried.push_back(nearestSpot);

      Vec2 spotCoord = getCoordinateFromPosition(nearestSpot->getPosition());
      int sx = (int)spotCoord.x, sy = (int)spotCoord.y;
      EnemyBase *extractor = nullptr;
      for (int oy = 0; oy < eh && !extractor; oy++) {
        for (int ox = 0; ox < ew && !extractor; ox++) {
          int bx = sx - ox;
          int by = sy + eh - oy;
          extractor = buildTheBuilding(extractorType, bx, by, ew, eh,
                                       extractorSprite, WHICH_SIDE_ENEMY, true);
        }
      }
      if (extractor) {
        addEnemyGold(-gCost);
        addEnemyLumber(-lCost);
        addEnemyOil(-oCost);
        setAfterBuildingProcess(extractor);
        // Stamp ownership (nearest HQ, or 0/ownerless if none exists yet).
        // Extractors are not counted by enemyAICheckBuildings, but keeping the
        // owner consistent lets the adopt-orphans pass treat them uniformly.
        extractor->aiOwnerHQId = enemyAINearestHQOwnerId(extractor->getPosition());
        setOilShipToExtractor(ship, extractor);
        break;
      }
    }
  }
}

// ── Building management ───────────────────────────────────────────────────────

struct EAIBuildDef {
  int unitType;
  const char *sprite;
  int prereqType; // -1 = none
};

static const EAIBuildDef kHumanBuildList[] = {
  {UNIT_FARM,           "farm.png",         -1},
  {UNIT_BARRACKS,       "barracks.png",     -1},
  {UNIT_LUMBERMILL,     "lumberMill.png",   -1},
  {UNIT_FACTORY,        "factory.png",      UNIT_LUMBERMILL},
  {UNIT_AIRPORT,        "airport.png",      UNIT_LUMBERMILL},
  {UNIT_HUMAN_SHIPYARD, "humanShipyard.png",-1},
  // OilRefinery is the oil drop-off that grants the +30% oil-per-trip bonus
  // (see Movable::moveNew). Needs a Shipyard first and only makes sense on
  // water maps; placed on water like the Shipyard (see eaiIsWaterBuildType).
  {UNIT_HUMAN_OIL_REFINERY, "humanOilRefinery.png", UNIT_HUMAN_SHIPYARD},
};
static const EAIBuildDef kOrcBuildList[] = {
  {UNIT_BARBECUE,        "barbecue.png",    -1},
  {UNIT_ORC_BUNKER,      "bunker.png",      -1},
  {UNIT_ORC_BARRACKS,    "axeport.png",     -1},
  {UNIT_ORC_TROLL_HOUSE, "statueHouse.png", UNIT_ORC_BARRACKS},
  {UNIT_TEMPLE,          "alter.png",       UNIT_ORC_BARRACKS},
  {UNIT_ORC_SHIPYARD,    "orcShipyard.png", -1},
  {UNIT_ORC_OIL_REFINERY, "orcOilRefinery.png", UNIT_ORC_SHIPYARD},
};

// Buildings the AI must place on water tiles (via enemyAIFindWaterBuildTile)
// and only on maps that have water.
static bool eaiIsWaterBuildType(int t) {
  return t == UNIT_HUMAN_SHIPYARD     || t == UNIT_ORC_SHIPYARD ||
         t == UNIT_HUMAN_OIL_REFINERY || t == UNIT_ORC_OIL_REFINERY;
}

// Returns the AI owner-id of the nearest live enemy HQ to pos, assigning that
// HQ a fresh id if it lacks one. Returns 0 when there is no live HQ (caller
// treats that as "ownerless").
int GameScene::enemyAINearestHQOwnerId(Vec2 pos) {
  EnemyBase *nearest = nullptr;
  float bestD = FLT_MAX;
  for (auto *u : enemyArray) {
    if (u->unitType != UNIT_CASTLE && u->unitType != UNIT_ORC_HQ) continue;
    if (!u->isBuilding || !u->isBuildingComplete || u->energy <= 0) continue;
    float d = pos.distanceSquared(u->getPosition());
    if (d < bestD) { bestD = d; nearest = u; }
  }
  if (!nearest) return 0;
  if (nearest->aiOwnerHQId == 0) nearest->aiOwnerHQId = enemyAINextHQId++;
  return nearest->aiOwnerHQId;
}

void GameScene::enemyAICheckBuildings() {
  std::vector<EnemyBase *> snap(enemyArray.begin(), enemyArray.end());

  // Race is detected once and cached in updateEnemyAI() (see enemyAIIsOrc).
  bool isOrc = enemyAIIsOrc;

  const EAIBuildDef *buildList     = isOrc ? kOrcBuildList : kHumanBuildList;
  const int          buildListSize = isOrc
      ? (int)(sizeof(kOrcBuildList)   / sizeof(kOrcBuildList[0]))
      : (int)(sizeof(kHumanBuildList) / sizeof(kHumanBuildList[0]));

  // Collect completed HQs.
  std::vector<EnemyBase *> hqs;
  for (auto *u : snap) {
    if ((u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ) &&
        u->isBuilding && u->isBuildingComplete && u->energy > 0)
      hqs.push_back(u);
  }

  // If workers exist but no HQ, build one first.
  if (hqs.empty()) {
    bool hasWorker = false;
    Vec2 workerPos;
    for (auto *u : snap) {
      if (!u->isBuilding && u->energy > 0 && u->isEnemy &&
          (u->unitType == UNIT_WORKER || u->unitType == UNIT_GOBLIN_WORKER)) {
        hasWorker = true;
        workerPos = u->getPosition();
        break;
      }
    }
    if (!hasWorker) {
      log("enemyAI[build]: no HQ and no worker found - AI is stuck");
      return;
    }

    int hqType = isOrc ? UNIT_ORC_HQ : UNIT_CASTLE;
    int gCost  = getGoldPriceForUnit(hqType);
    int lCost  = getLumberPriceForUnit(hqType);
    int oCost  = getOilPriceForUnit(hqType);
    if (enemyGold < gCost || enemyLumber < lCost || enemyOil < oCost) {
      log("enemyAI[build]: no HQ - waiting on resources for HQ (have %d/%d/%d, need %d/%d/%d)",
          enemyGold, enemyLumber, enemyOil, gCost, lCost, oCost);
      return;
    }

    Vec2 sz = GM->getBuildingOccupySize(hqType);
    int w = (int)sz.x, h = (int)sz.y;
    int bx, by;
    if (!enemyAIFindBuildTile(workerPos, w, h, bx, by, true)) {
      log("enemyAI[build]: no HQ - could not find a free tile near worker");
      return;
    }

    const char *spr = isOrc ? "hq.png" : "castle.png";
    EnemyBase *bld = buildTheBuilding(hqType, bx, by, w, h, spr, WHICH_SIDE_ENEMY, true);
    if (bld) {
      addEnemyGold(-gCost);
      addEnemyLumber(-lCost);
      addEnemyOil(-oCost);
      setAfterBuildingProcess(bld);
      // No other HQ exists (that is why we are here), so this HQ is its own
      // owner: give it a fresh identity now instead of waiting for the next pass.
      bld->aiOwnerHQId = enemyAINextHQId++;
      log("enemyAI[build]: built HQ at tile (%d,%d)", bx, by);
    } else {
      log("enemyAI[build]: buildTheBuilding rejected HQ placement at tile (%d,%d)", bx, by);
    }
    return;
  }

  bool foodShort = (enemyFoodMax > 0 && enemyFoodInUse >= enemyFoodMax - 4);

  // ── Ownership bookkeeping (fixes the radius-mismatch duplicate-build bug) ──
  // Each AI building remembers the unique id of the HQ that built it
  // (aiOwnerHQId); an HQ's own aiOwnerHQId is its identity. countNear (below),
  // the prereq check and the food-building cap all count by ownership instead
  // of by Euclidean distance, so a building placed in the far corner of the
  // square tile-search area still counts and never gets rebuilt every cycle.
  //
  // First give every live HQ a stable identity.
  for (auto *hq : hqs)
    if (hq->aiOwnerHQId == 0) hq->aiOwnerHQId = enemyAINextHQId++;

  auto isLiveHQId = [&](int id) {
    if (id == 0) return false;
    for (auto *hq : hqs) if (hq->aiOwnerHQId == id) return true;
    return false;
  };

  // Destroyed-HQ policy: when an HQ dies its buildings are orphaned. We adopt
  // any orphaned (owner no longer alive) or never-owned (e.g. map-placed, or
  // an oil extractor built before any HQ existed) enemy building into the
  // nearest surviving HQ, so it resumes counting toward that HQ's caps. This
  // keeps a captured/inherited base deduplicated rather than letting it drift
  // ownerless and get rebuilt on top of itself.
  for (auto *u : snap) {
    if (!u->isBuilding || u->energy <= 0 || !u->isEnemy) continue;
    if (u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ) continue;
    if (isLiveHQId(u->aiOwnerHQId)) continue;
    EnemyBase *nearest = nullptr;
    float bestD = FLT_MAX;
    for (auto *hq : hqs) {
      float d = u->getPosition().distanceSquared(hq->getPosition());
      if (d < bestD) { bestD = d; nearest = hq; }
    }
    if (nearest) u->aiOwnerHQId = nearest->aiOwnerHQId;
  }

  auto countNear = [&](int type, EnemyBase *hq) {
    int n = 0;
    int ownerId = hq->aiOwnerHQId;
    for (auto *u : snap)
      if (u->unitType == type && u->isBuilding && u->energy > 0 &&
          u->aiOwnerHQId == ownerId) n++;
    return n;
  };

  for (auto *hq : hqs) {
    Vec2 hqPos = hq->getPosition();

    const EAIBuildDef *pick = nullptr;

    if (foodShort) {
      int foodType = isOrc ? UNIT_BARBECUE : UNIT_FARM;
      if (countNear(foodType, hq) < kEAIMaxFoodBuildingsPerHQ)
        for (int i = 0; i < buildListSize; i++)
          if (buildList[i].unitType == foodType) { pick = &buildList[i]; break; }
    }

    if (!pick) {
      for (int i = 0; i < buildListSize; i++) {
        const EAIBuildDef &def = buildList[i];
        if (eaiIsWaterBuildType(def.unitType) && !mapHasWater) continue;
        if (countNear(def.unitType, hq) > 0) continue;
        if (def.prereqType != -1 && countNear(def.prereqType, hq) == 0) continue;
        pick = &def;
        break;
      }
    }

    if (!pick) {
      log("enemyAI[build]: HQ at (%.0f,%.0f) has nothing left to build in its list",
          hqPos.x, hqPos.y);
      continue;
    }

    int gCost = getGoldPriceForUnit(pick->unitType);
    int lCost = getLumberPriceForUnit(pick->unitType);
    int oCost = getOilPriceForUnit(pick->unitType);
    if (enemyGold < gCost || enemyLumber < lCost || enemyOil < oCost) {
      log("enemyAI[build]: waiting on resources for unitType %d (have %d/%d/%d, need %d/%d/%d)",
          pick->unitType, enemyGold, enemyLumber, enemyOil, gCost, lCost, oCost);
      continue;
    }

    bool isWaterPick = eaiIsWaterBuildType(pick->unitType);
    Vec2 sz = GM->getBuildingOccupySize(pick->unitType);
    int w = (int)sz.x, h = (int)sz.y;
    int bx, by;
    bool foundTile = isWaterPick
        ? enemyAIFindWaterBuildTile(hqPos, w, h, bx, by)
        : enemyAIFindBuildTile(hqPos, w, h, bx, by, false);
    if (!foundTile) {
      log("enemyAI[build]: could not find a free tile for unitType %d near HQ (%.0f,%.0f)",
          pick->unitType, hqPos.x, hqPos.y);
      continue;
    }

    EnemyBase *bld = buildTheBuilding(pick->unitType, bx, by, w, h,
                                      pick->sprite, WHICH_SIDE_ENEMY, true);
    if (!bld) {
      log("enemyAI[build]: buildTheBuilding rejected unitType %d at tile (%d,%d)",
          pick->unitType, bx, by);
      continue;
    }
    addEnemyGold(-gCost);
    addEnemyLumber(-lCost);
    addEnemyOil(-oCost);
    setAfterBuildingProcess(bld);
    bld->aiOwnerHQId = hq->aiOwnerHQId;  // stamp ownership so it counts next pass
    log("enemyAI[build]: built unitType %d at tile (%d,%d)", pick->unitType, bx, by);
  }
}

// ── Unit training ─────────────────────────────────────────────────────────────

struct EAIProdEntry {
  int buildType;
  int units[4]; // -1 terminates
};

static const EAIProdEntry kHumanProd[] = {
  {UNIT_BARRACKS,       {UNIT_SWORDMAN, UNIT_ARCHER, -1, -1}},
  {UNIT_FACTORY,        {UNIT_CATAPULT,  -1, -1, -1}},
  {UNIT_AIRPORT,        {UNIT_HELICOPTER,-1, -1, -1}},
  // Priority order: OilShip (economy) > Shuttle (cheap utility) >
  // Ship (main workhorse) > BattleShip (priciest, capped low).
  {UNIT_HUMAN_SHIPYARD, {UNIT_HUMAN_OIL_SHIP, UNIT_HUMAN_SHUTTLE,
                         UNIT_HUMAN_SHIP, UNIT_HUMAN_BATTLE_SHIP}},
  {-1, {-1,-1,-1,-1}},
};
static const EAIProdEntry kOrcProd[] = {
  {UNIT_ORC_BARRACKS,    {UNIT_ORC_AXE,  UNIT_ORC_SPEAR, -1, -1}},
  {UNIT_ORC_TROLL_HOUSE, {UNIT_TROLL,    -1, -1, -1}},
  {UNIT_TEMPLE,          {UNIT_GOBLIN,   UNIT_GOBLIN_BOMB, -1, -1}},
  {UNIT_ORC_SHIPYARD,    {UNIT_ORC_OIL_SHIP, UNIT_ORC_SHUTTLE,
                         UNIT_ORC_SHIP, UNIT_ORC_BATTLE_SHIP}},
  {-1, {-1,-1,-1,-1}},
};

// Per-unit-type population cap for AI production, overriding the generic
// kEAIMaxUnitsPerType for units that need their own ceiling.
static int enemyAIUnitCap(int uType, int oilSpotCount) {
  if (uType == UNIT_HUMAN_OIL_SHIP || uType == UNIT_ORC_OIL_SHIP)
    return oilSpotCount * kEAIShipsPerExtractor;
  if (uType == UNIT_HUMAN_SHUTTLE || uType == UNIT_ORC_SHUTTLE)
    return kEAIMaxShuttles;
  if (uType == UNIT_HUMAN_SHIP || uType == UNIT_ORC_SHIP)
    return kEAIMaxShips;
  if (uType == UNIT_HUMAN_BATTLE_SHIP || uType == UNIT_ORC_BATTLE_SHIP)
    return kEAIMaxBattleShips;
  return kEAIMaxUnitsPerType;
}

void GameScene::enemyAITrainUnits() {
  enemyAITrainTimer += 1.0f;
  if (enemyAITrainTimer < 30.0f) return;
  enemyAITrainTimer = 0.0f;

  std::vector<EnemyBase *> snap(enemyArray.begin(), enemyArray.end());

  // Race is detected once and cached in updateEnemyAI() (see enemyAIIsOrc).
  bool isOrc = enemyAIIsOrc;

  const EAIProdEntry *prodTable = isOrc ? kOrcProd : kHumanProd;
  int workerType = isOrc ? UNIT_GOBLIN_WORKER : UNIT_WORKER;

  // Count existing non-building enemy units by type.
  std::map<int, int> counts;
  for (auto *u : snap)
    if (!u->isBuilding && u->energy > 0) counts[u->unitType]++;

  // Count HQs to set worker target.
  int hqCount = 0;
  for (auto *u : snap)
    if ((u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ) &&
        u->isBuilding && u->isBuildingComplete && u->energy > 0)
      hqCount++;

  // Cap live OilShips at kEAIShipsPerExtractor per OilSpot still on the map,
  // so the AI doesn't keep pouring gold into ships it has nowhere to put to work.
  int oilSpotCount = 0;
  for (auto *m : mutualArray)
    if (m->unitType == UNIT_OIL_SPOT && m->energy > 0) oilSpotCount++;

  for (auto *bld : snap) {
    if (!bld->isBuilding || !bld->isBuildingComplete || bld->energy <= 0) continue;

    Vec2 spawnPos = bld->getApproachingPoint(bld->getPosition());
    bool isShipyardBld = (bld->unitType == UNIT_HUMAN_SHIPYARD ||
                          bld->unitType == UNIT_ORC_SHIPYARD);
    if (isShipyardBld) {
      Vec2 emptyTile = findEmptyWaterSpawnTile(
          (int)bld->buildingStartCoordinate.x, (int)bld->buildingStartCoordinate.y,
          (int)bld->buildingOccupySize.width, (int)bld->buildingOccupySize.height);
      if (emptyTile.x >= 0) spawnPos = emptyTile;
    }

    // Castle / HQ → workers
    if (bld->unitType == UNIT_CASTLE || bld->unitType == UNIT_ORC_HQ) {
      int targetWorkers = hqCount * kEAITargetWorkers;
      if (counts[workerType] >= targetWorkers) continue;
      // Prioritization: bootstrap the economy freely up to the bootstrap
      // threshold, then reserve food/gold headroom for combat units so workers
      // don't win every contested resource (the HQ usually iterates first).
      int bootstrapWorkers = hqCount * kEAIBootstrapWorkersPerHQ;
      if (counts[workerType] >= bootstrapWorkers) {
        if ((enemyFoodMax - enemyFoodInUse) <= kEAIWorkerFoodReserve) continue;
        if (enemyGold <= kEAIWorkerGoldReserve) continue;
      }
      int gCost = getGoldPriceForUnit(workerType);
      int lCost = getLumberPriceForUnit(workerType);
      int oCost = getOilPriceForUnit(workerType);
      int fCost = GM->getFoodUseForUnit(workerType);
      if (enemyGold < gCost || enemyLumber < lCost || enemyOil < oCost) continue;
      if (enemyFoodInUse + fCost > enemyFoodMax) continue;
      EnemyBase *u = createUnit(workerType, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING,
                                spawnPos, GM->getUnitName(workerType), 1,
                                getSpriteNameForUnit(workerType).c_str());
      if (u) {
        addEnemyGold(-gCost);
        addEnemyLumber(-lCost);
        addEnemyOil(-oCost);
        enemyFoodInUse += fCost;
        counts[workerType]++;
      }
      continue;
    }

    // Combat buildings — round-robin over the building's unit list so it
    // alternates types (e.g. Swordman/Archer) instead of massing the first
    // affordable one. bld->aiNextTrainIndex remembers where the last tick left
    // off; each tick we walk the whole list once starting there, skipping
    // entries that are at cap / unaffordable / food-blocked and trying the
    // next, then stop after the first successful training.
    for (int pi = 0; prodTable[pi].buildType != -1; pi++) {
      if (prodTable[pi].buildType != bld->unitType) continue;

      int listLen = 0;
      while (listLen < 4 && prodTable[pi].units[listLen] != -1) listLen++;
      if (listLen == 0) break;

      int start = bld->aiNextTrainIndex % listLen;
      for (int step = 0; step < listLen; step++) {
        int ui = (start + step) % listLen;
        int uType = prodTable[pi].units[ui];
        int cap = enemyAIUnitCap(uType, oilSpotCount);
        if (counts[uType] >= cap) continue;
        int gCost = getGoldPriceForUnit(uType);
        int lCost = getLumberPriceForUnit(uType);
        int oCost = getOilPriceForUnit(uType);
        int fCost = GM->getFoodUseForUnit(uType);
        if (enemyGold < gCost || enemyLumber < lCost || enemyOil < oCost) {
          if (enemyOil < oCost)
            log("enemyAI[train]: unitType %d waiting on oil (%d/%d)",
                uType, enemyOil, oCost);
          continue;
        }
        if (enemyFoodInUse + fCost > enemyFoodMax) {
          log("enemyAI[train]: unitType %d blocked by food cap (%d/%d, needs %d more)",
              uType, enemyFoodInUse, enemyFoodMax, fCost);
          continue;
        }
        EnemyBase *u = createUnit(uType, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING,
                                  spawnPos, GM->getUnitName(uType), 1,
                                  getSpriteNameForUnit(uType).c_str());
        if (u) {
          addEnemyGold(-gCost);
          addEnemyLumber(-lCost);
          addEnemyOil(-oCost);
          enemyFoodInUse += fCost;
          counts[uType]++;
          // AI-trained combat units gather at the rally point and hold there
          // until their wave fires (enemyAIManageWaves). Workers/ships/shuttles
          // and any map/trigger-spawned units are never flagged, so they keep
          // their current behavior.
          if (enemyAIIsRallyCombatType(uType)) u->isRallying = true;
          // Advance the round-robin cursor past the type we just trained so the
          // next tick starts with the following entry (and wraps around).
          bld->aiNextTrainIndex = ui + 1;
          log("enemyAI[train]: trained unitType %d", uType);
        }
        break; // one successful training per building per tick
      }
      break;
    }
  }
}

// ── Rally / wave helpers (single-player only) ─────────────────────────────────

bool GameScene::enemyAIIsRallyCombatType(int unitType) {
  // Workers, oil ships and shuttles never rally - they have their own jobs.
  if (unitType == UNIT_WORKER || unitType == UNIT_GOBLIN_WORKER) return false;
  if (unitType == UNIT_HUMAN_OIL_SHIP || unitType == UNIT_ORC_OIL_SHIP) return false;
  if (unitType == UNIT_HUMAN_SHUTTLE || unitType == UNIT_ORC_SHUTTLE) return false;
  return true;
}

void GameScene::enemyAINotifyBuildingHit(cocos2d::Vec2 attackerPos) {
  // Cheap flag-set from the damage path; the 1s AI tick does the real work.
  if (isMultiplay) return;
  enemyAIBaseUnderAttack = true;
  enemyAIBaseAttackerPos = attackerPos;
}

// Release every living AI-rallying unit as one attack wave. Returns the count.
int GameScene::enemyAIReleaseWave(cocos2d::Vec2 towardPos, bool towardValid) {
  Vector<EnemyBase *> list;
  for (auto *u : enemyArray) {
    if (!u || u->energy <= 0 || u->isBuilding || !u->isRallying) continue;
    u->isRallying = false;
    u->wantToEli = true;   // released wave units become persistent attackers
    list.pushBack(u);
  }
  // March them out together via the existing attack flow.
  for (auto *u : list) {
    if (towardValid) {
      moveAndAttackTo(u, towardPos);   // defense: converge on the attacker
    } else {
      attackNearHero(u);               // offense: each heads for the nearest hero
    }
  }
  enemyAIRallyActive = false;
  enemyAIRallyTimer = 0.0f;
  return (int)list.size();
}

void GameScene::enemyAIManageWaves() {
  // Recompute the rally point: nearest live HQ pushed toward the nearest known
  // player building (or the map centre if none is known yet). Stored as a bare
  // Vec2 so it can never dangle when buildings die.
  EnemyBase *hq = nullptr;
  for (auto *u : enemyArray) {
    if (u && u->energy > 0 && u->isBuilding && u->isBuildingComplete &&
        (u->unitType == UNIT_CASTLE || u->unitType == UNIT_ORC_HQ)) {
      hq = u; break;
    }
  }
  if (hq) {
    Vec2 hqPos = hq->getPosition();
    Vec2 targetPos = getPositionFromTileCoordinate((int)(mapSize.width / 2),
                                                   (int)(mapSize.height / 2));
    float best = 1e18f;
    for (auto *h : heroArray) {
      if (!h || h->energy <= 0 || !h->isBuilding) continue;
      float d = hqPos.distanceSquared(h->getPosition());
      if (d < best) { best = d; targetPos = h->getPosition(); }
    }
    Vec2 dir = targetPos - hqPos;
    if (dir.lengthSquared() > 1.0f) dir.normalize(); else dir = Vec2(0, -1);
    enemyAIRallyPoint = hqPos + dir * kEAIRallyOffset;
    enemyAIRallyPointValid = true;
  }
  // else: no live HQ - keep the previous point; rallying units just hold in place.

  // Sum the food of all living rallying units this tick. Iterating live units
  // (rather than an incremental counter) means a unit that dies while rallying
  // simply stops counting - no bookkeeping can leak or dangle.
  int rallyFood = 0;
  int rallyCount = 0;
  for (auto *u : enemyArray) {
    if (!u || u->energy <= 0 || u->isBuilding || !u->isRallying) continue;
    rallyFood += GM->getFoodUseForUnit(u->unitType);
    rallyCount++;
  }

  // Track how long the current wave has been gathering (for the safety timeout).
  if (rallyCount > 0) {
    if (!enemyAIRallyActive) {
      enemyAIRallyActive = true;
      enemyAIRallyTimer = 0.0f;
    } else {
      enemyAIRallyTimer += 1.0f;
    }
  } else {
    enemyAIRallyActive = false;
    enemyAIRallyTimer = 0.0f;
  }

  // 1) Base-defense override beats wave timing: an enemy building was just hit.
  if (enemyAIBaseUnderAttack) {
    enemyAIBaseUnderAttack = false;
    if (rallyCount > 0) {
      int n = enemyAIReleaseWave(enemyAIBaseAttackerPos, true);
      enemyAIWaveCount++;
      log("enemyAI[wave]: BASE DEFENSE - released %d units toward (%.0f,%.0f)",
          n, enemyAIBaseAttackerPos.x, enemyAIBaseAttackerPos.y);
    }
    return;
  }

  if (rallyCount == 0) return;

  // 2) Food threshold reached: launch a scaling wave.
  int threshold = kEAIFirstWaveFood + enemyAIWaveCount * kEAIWaveFoodStep;
  if (threshold > kEAIMaxWaveFood) threshold = kEAIMaxWaveFood;
  if (rallyFood >= threshold) {
    int n = enemyAIReleaseWave(Vec2::ZERO, false);
    enemyAIWaveCount++;
    log("enemyAI[wave]: released %d units (food %d >= threshold %d), wave #%d",
        n, rallyFood, threshold, enemyAIWaveCount);
    return;
  }

  // 3) Safety timeout: never let units sit in the rally forever.
  if (enemyAIRallyTimer >= kEAIWaveTimeout) {
    int n = enemyAIReleaseWave(Vec2::ZERO, false);
    enemyAIWaveCount++;
    log("enemyAI[wave]: SAFETY TIMEOUT - released %d units (food %d) after %.0fs",
        n, rallyFood, enemyAIRallyTimer);
  }
}

// ── Main AI entry point (called every second) ─────────────────────────────────

void GameScene::updateEnemyAI() {
  if (isMultiplay) return;

  // One-time init: detect whether the map has any water tiles.
  if (!enemyAIInitialized) {
    enemyAIInitialized = true;
    mapHasWater = false;
    for (int j = 0; j < (int)mapSize.height && !mapHasWater; j++)
      for (int i = 0; i < (int)mapSize.width && !mapHasWater; i++)
        if (isWaterTileAt(i, j)) mapHasWater = true;
    enemyGold   = 1000;
    enemyLumber = 1000;
  }

  // Run when the enemy side has at least one living building OR at least one
  // living enemy worker. A surviving worker lets enemyAICheckBuildings() rebuild
  // a lost HQ, so the AI must not early-out just because every building is gone.
  bool hasEnemyBuilding = false;
  bool hasEnemyWorker   = false;
  for (auto *u : enemyArray) {
    if (!u || u->energy <= 0) continue;
    if (u->isBuilding) { hasEnemyBuilding = true; break; }
    if (u->isEnemy &&
        (u->unitType == UNIT_WORKER || u->unitType == UNIT_GOBLIN_WORKER))
      hasEnemyWorker = true;
  }
  if (!hasEnemyBuilding && !hasEnemyWorker) {
    static bool loggedNoBuilding = false;
    if (!loggedNoBuilding) {
      loggedNoBuilding = true;
      log("enemyAI: no enemy building and no enemy worker - AI will not run until one exists");
    }
    return;
  }

  // Detect the enemy race once and cache it. We reach here only after the
  // guard above confirmed a living enemy building or worker, so enemyArray is
  // non-empty. Detecting now (rather than re-scanning every tick) fixes a bug:
  // a completed UNIT_ORC_HQ is renamed to UNIT_CASTLE in setAfterBuildingProcess,
  // so an Orc base whose only living unit is its finished HQ would scan as
  // Human. isOrcTypeUnit() already covers UNIT_ORC_HQ and UNIT_GOBLIN_WORKER.
  if (!enemyAIRaceDetected) {
    for (auto *u : enemyArray) {
      if (!u) continue;
      if (isOrcTypeUnit(u->unitType)) { enemyAIIsOrc = true; break; }
    }
    enemyAIRaceDetected = true;
  }

  enemyAIUpdateFood();
  enemyAIManageWorkers();
  enemyAIManageShips();
  enemyAITrainUnits();
  enemyAIManageWaves();

  enemyAIBuildTimer += 1.0f;
  if (enemyAIBuildTimer >= 60.0f) {
    enemyAIBuildTimer = 0.0f;
    // Once-per-minute food status (enemyAIUpdateFood ran earlier this tick).
    int workerCnt = 0;
    for (auto *u : enemyArray)
      if (u && u->energy > 0 && !u->isBuilding &&
          (u->unitType == UNIT_WORKER || u->unitType == UNIT_GOBLIN_WORKER))
        workerCnt++;
    log("enemyAI[food]: %d/%d (workers %d)",
        enemyFoodInUse, enemyFoodMax, workerCnt);
    enemyAICheckBuildings();
  }
}

