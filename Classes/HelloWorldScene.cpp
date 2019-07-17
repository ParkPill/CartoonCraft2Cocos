#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LegendDaryButton.h"
#include "HudLayer.h"
#include "GameManager.h"
#include "GameSharing.h"
#include "BuggyServerManager.h"
#include "Fog.h"
//#include "NativeInterface.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"

using namespace cocos2d;
using namespace CocosDenshion;

Scene* HelloWorld::scene(int stage, bool boss)
{
//    stage = 1; // test
    
//    GameManager::getInstance()->stopSoundEffect(SOUND_THUNDER);
    GameManager::getInstance()->leftPressed = false;
    GameManager::getInstance()->rightPressed = false;
    GameManager::getInstance()->firePressed = false;
    GameManager::getInstance()->jumpPressed = false;
    GameManager::getInstance()->cPressed = false;
    GameManager::getInstance()->downPressed = false;
    GameManager::getInstance()->upPressed = false;
    Size size = Director::getInstance()->getWinSize();
    Scene *scene = Scene::create();
    int theme = GameManager::getInstance()->theme;
    GameManager::getInstance()->currentStageIndex = stage;
    
//    GameManager::getInstance()->showInterstitialAds();
    
//    LayerColor* grayLayer = LayerColor::create(Color4B(246,246,246,255));
//    LayerColor* grayLayer = LayerColor::create(Color4B(156, 156, 156, 255));
//    LayerColor* grayLayer = LayerColor::create(Color4B(150, 163, 83, 255)); // green
    LayerColor* grayLayer;
    if(GM->nextScene == STAGE_LOBBY || GM->nextScene == STAGE_RAID || GM->nextScene == STAGE_SINGLEPLAY){
        grayLayer = LayerColor::create(Color4B(206, 178, 138, 255)); // sand
    }else{
        grayLayer = LayerColor::create(Color4B(0, 0, 0, 255)); // sand
    }
    
    grayLayer->setName("grayLayer");
//    LayerColor* grayLayer = LayerColor::create(Color4B(206,206,206,255));
    scene->addChild(grayLayer);
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    layer->stageIndex = stage;
    
    if(stage == STAGE_LOBBY){
        BattleHud* bhud = BattleHud::create();
        scene->addChild(bhud, 10);
    }else{
        HudLayer* hudLayer = HudLayer::create();
        scene->addChild(hudLayer, 10);
        GameManager::getInstance()->setHudLayer(hudLayer);
        int tag = rand()%50;
        layer->setTag(tag);
        CCLOG("** New Tag: %d, theme: %d, stage: %d", tag, GameManager::getInstance()->theme, stage);
        hudLayer->setTag(tag);
        if(stage == 0 || stage == 12){
            if(GM->nextScene == STAGE_INTRO){
                hudLayer->addListener();
            }
        }
    }
    
    layer->isBossMap = boss;
    if(GM->nextScene == STAGE_FIELD || GM->nextScene == STAGE_LOBBY ||GM->nextScene == STAGE_SINGLEPLAY || GM->nextScene == STAGE_RAID){
        layer->addListener();
    }
    
    if(GM->nextScene == STAGE_CREDIT){ // test
        layer->setEntireMap(stage);
        HUD->showCredit();
        return scene;
    }else{
    }
//    if (boss || true) {//test
    if(boss){
        layer->setBossMap(stage);
    }else{
        layer->setEntireMap(stage);
    }
    if(GM->nextScene == STAGE_FIELD){
        layer->setClearCondition(stage);
    }
    if(stage == STAGE_LOBBY){
        layer->setLobby();
    }
    if(GM->nextScene == STAGE_RAID){
        HUD->setRaid();
    }
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    size = Director::getInstance()->getWinSize();
    //////////////////////////////
    // 1. super init first
    theBoss = NULL;
    
    // test
//    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), true);
//    GameManager::getInstance()->isGameClear = true;
    
    GameManager::getInstance()->isInMiddleOfGame = true;
    GameManager::getInstance()->setCurrentStageLayer(this);
    if (!Layer::init())
    {
        return false;
    }
    loadUnitSheet();
    isReloading = false;
    center = cocos2d::Point(size.width/2, size.height/2);
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
    this->setKeypadEnabled(true);
//    this->setTouchEnabled(true);
    playerIgnoreGravity = false;
    bulletWasted = false;
    
//    spriteBatchBuilding = SpriteBatchNode::create("cartoonCraftBuilding.png");
//    this->addChild(spriteBatchBuilding, 10);
//    spriteBatch = SpriteBatchNode::create("cartoonCraft.png");
//    this->addChild(spriteBatch, 10);
//    spriteBatchEffect = SpriteBatchNode::create("cartoonCraftEffect.png");
//    this->addChild(spriteBatchEffect, 10);
    Node* node = Node::create();
    this->addChild(node, 10);
    spriteBatch = node;
    spriteBatchBuilding = node;
    spriteBatchEffect = node;
    
    std::string stage = UD->getStringForKey(KEY_LAST_SAVE_POINT, "stage0");
    
    enemyPointArray = PointArray::create(100);
    enemyPointArray->retain();
    
    supportPointArray = PointArray::create(100);
    supportPointArray->retain();
    
    labelPoolCount = 10;
    labelPoolIndex = 0;
    for (int i = 0; i < labelPoolCount; i++) {
        Label* lbl = Label::createWithSystemFont("YES!", "ari", 30);
        lbl->setScale(0.4f);
        lbl->retain();
        lbl->setTextColor(Color4B(78, 78, 78, 255));
        lbl->enableOutline(Color4B::WHITE, 4);
        labelPool.pushBack(lbl);
    }
    
    battleReadyCountDown = 2;
    
    draw = DrawNode::create();
//    draw->drawSolidRect(Point(8*TILE_SIZE, 8*TILE_SIZE), Point(9*TILE_SIZE, 9*TILE_SIZE), Color4F::GREEN);
    this->addChild(draw, 125);
    
    targetHand = EnemyBase::createEnemy(UNIT_WORKER, 0, 0, "workerAxeStand0.png");
    spriteBatch->addChild(targetHand, 110);
    targetHand->setOpacity(0);
    targetHand->isEnemy = false;
    targetHand->untouchable = true;
    
    maxLayerScale = 2;
    minLayerScale = 1;
    if (stageIndex == 0) {
        
    }
    GM->playSoundEffect(SOUND_BGM_MAYDAY);
    
    
    // init done
    return true;
}
void HelloWorld::loadUnitSheet(){
    
}
int HelloWorld::getItemType(std::string name){
    if(weaponStatTable.find(name) != weaponStatTable.end()){
        return ITEM_TYPE_WEAPON;
    }else if(shieldStatTable.find(name) != shieldStatTable.end()){
        return ITEM_TYPE_SHIELD;
    }else if(helmetStatTable.find(name) != helmetStatTable.end()){
        return ITEM_TYPE_HELMET;
    }else if(shoesStatTable.find(name) != shoesStatTable.end()){
        return ITEM_TYPE_SHOES;
    }else if(itemStatTable.find(name) != shoesStatTable.end()){
        return ITEM_TYPE_ETC;
    }else{
        return -1;
    }
}


void HelloWorld::move(float dt, Movable* obj, Movable* target, bool horiFirst){
    if(target == nullptr){
//        if (obj->routePositionArray != nullptr && obj->routePositionArray->count() > 0) {
        if(!obj->isArrived){
            obj->move(dt);
        }
        return;
    }
    if(obj->routePositionArray != nullptr){
        obj->move(dt);
    }
    
    obj->moveInterval+= dt;
    if(obj->moveInterval < 0.5f){
        return;
    }
    obj->moveInterval = 0;
    obj->moveToTarget();
}
void HelloWorld::updateUnitMoveNew(float dt){
    for(auto hero: heroArray){
        if(hero->unitAct == UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH){
            hero->restingTime -= dt;
            if(hero->restingTime < 0){
                hero->unitAct = UNIT_ACT_NONE;
                hero->unitActDetail = UNIT_ACT_DETAIL_IDLE;
            }
        }else if (hero->unitAct == UNIT_ACT_NONE || hero->unitAct == UNIT_ACT_ATTACK_DDANG) {
            if (hero->unitActDetail == UNIT_ACT_DETAIL_IDLE || hero->unitAct == UNIT_ACT_ATTACK_DDANG) {
//                if (hero->canFindTarget) {
                    hero->target = findTargetEnemy(hero);
                    if (hero->target == nullptr) {
                        if(hero->unitAct == UNIT_ACT_NONE){
                            hero->unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
                            hero->restingTime = 1;
                        }
                    }else{
                        hero->unitAct = UNIT_ACT_ATTACK;
                        hero->moveToPos = Vec2::ZERO;
                        hero->moveFlagPos = hero->target->getPosition();
                    }
//                }
            }
        }
        hero->moveNew(dt);
    }
    
    for(auto unit: enemyArray){
        if(unit->unitAct == UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH){
            unit->restingTime -= dt;
            if(unit->restingTime < 0){
                unit->unitAct = UNIT_ACT_NONE;
                unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
            }
        }else if (unit->unitAct == UNIT_ACT_NONE) {
            if (unit->unitActDetail == UNIT_ACT_DETAIL_IDLE || unit->unitAct == UNIT_ACT_ATTACK_DDANG) {
//                if (unit->canFindTarget) {
                    unit->target = findTargetHero(unit);
                    if (unit->target == nullptr) {
                        unit->unitAct = UNIT_ACT_RESTING_FOR_NEXT_TARGET_SEARCH;
                        unit->restingTime = 1;
                    }else{
                        unit->unitAct = UNIT_ACT_ATTACK;
                        unit->moveToPos = Vec2::ZERO;
                        unit->moveFlagPos = unit->target->getPosition();
                    }
//                }
            }
        }
        unit->moveNew(dt);
    }
}
void HelloWorld::updateUnitMove(float dt){
    // heros
    int moveCounter = 1;
    for(auto hero: heroArray){
        if(!hero->canMove && !hero->canFindTarget && hero->energy <= 0) continue;
        // finding target
        if(hero->canFindTarget && (hero->target == nullptr || hero->target == hero->dummyTarget)){
//        if(hero->canFindTarget && ((hero->unitAct == UNIT_ACT_NONE && hero->unitActDetail != UNIT_ACT_DETAIL_ATTACK) || (hero->unitAct == UNIT_ACT_ATTACK_DDANG && hero->unitActDetail != UNIT_ACT_DETAIL_ATTACK))){
            float minDistance = 1000000;
            EnemyBase* nearest = nullptr;
            for(auto drop: enemyArray){
                if (drop->untouchable || drop->energy <= 0) {
                    continue;
                }
                if (!canAttack(hero, drop)) {
                    continue;
                }
                float distance = hero->getPosition().distanceSquared(drop->getPosition());
                if (distance < hero->monitoringDistance) {
                    if (minDistance > distance) {
                        minDistance = distance;
                        nearest = drop;
                    }
                }
            }
            EnemyBase* nearestTree = nullptr;
            if(hero->unitType == UNIT_WIZARD){
                for(auto tree: mutualArray){
                    if(tree->unitType == UNIT_TREE_FOR_BATTLE){
                        float distance = hero->getPosition().distanceSquared(tree->getPosition());
                        if (distance < hero->monitoringDistance) {
                            if (minDistance > distance) {
                                minDistance = distance;
                                nearestTree = tree;
                            }
                        }
                    }
                }
            }
            
            if(nearest != nullptr || nearestTree != nullptr){
                if(hero->canMove){
                    if(nearestTree != nullptr){
                        if(nearest == nullptr){
                            hero->moveToTarget(nearestTree);
                        }else{
                            float treeDistance = hero->getPosition().distanceSquared(nearestTree->getPosition());
                            float unitDistance = hero->getPosition().distanceSquared(nearest->getPosition());
                            hero->moveToTarget(treeDistance > unitDistance? nearest:nearestTree);
                        }
                        
                        hero->canFindTarget = false;
                        hero->forceAttackTarget = true;
                    }else{
                        hero->moveToTarget(nearest);
                        hero->canFindTarget = false;
                    }
                }else{
                    hero->target = nearest;
                }
            }
        }
        
        // move
        hero->move(dt);
    }
    // enemy
    for(auto enemy:enemyArray){
        if (enemy->energy <= 0) continue;
        if (!enemy->canFindTarget && enemy->target != nullptr){
            float distance = enemy->getPosition().distanceSquared(enemy->target->getPosition());
            if (distance >= enemy->monitoringDistance) {
                enemy->canFindTarget = true;
                enemy->target = nullptr;
                // test now if enemy is out of sight enemy stops following
            }
        }
        
        if(enemy->canFindTarget && enemy->target == nullptr){
            float minDistance = 1000000;
            EnemyBase* nearest = nullptr;
            for(auto drop: heroArray){
                if (drop->untouchable || drop->energy <= 0) {
                    continue;
                }
                if (!canAttack(enemy, drop)) {
                    continue;
                }
                
                float distance = enemy->getPosition().distanceSquared(drop->getPosition());
                
                if (distance < enemy->monitoringDistance) {
                    if (minDistance > distance) {
                        minDistance = distance;
                        nearest = drop;
                    }
                }
            }
            if(nearest != nullptr){
                if(enemy->canMove){
                    enemy->canFindTarget = false;
                    enemy->moveToTarget(nearest);
                }else{
                    enemy->target = nearest;
                }
            }
        }
        
        // move
        enemy->move(dt);
    }
}
Movable* HelloWorld::createMissile(int missileType, int dmg, bool visible, float time, int angle, int speed, Point pos, bool isFromEnemy, std::string weaponName){
    Movable* sptMissile;
    
    if(missileType == MISSILE_SLASH){
        sptMissile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "slash.png");
    }else if(missileType == MISSILE_ARROW){
        sptMissile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "arrow.png");
    }else if(missileType == MISSILE_FIREBALL){
        sptMissile = Movable::createMovable(UNIT_MISSILE_STRAIGHT, dmg, 0, "fire0.png");
        sptMissile->runAnimation("fire", true);
    }
    if(weaponName.size() > 0){
        sptMissile->setSpriteFrame(strmake("%sMs.png", weaponName.c_str()));
    }
    spriteBatch->addChild(sptMissile, 11);
    sptMissile->setPosition(pos);
    float delta = speed*time;
    sptMissile->runAction(Sequence::create(MoveBy::create(time, Point(delta*cos(angle*3.14f/180), delta*sin(angle*3.14f/180))), CallFuncN::create(CC_CALLBACK_1(HelloWorld::missileMoveDone, this)), nullptr));
    sptMissile->setRotation(-angle + 90);
    if(isFromEnemy){
        enemyMissileArray.pushBack(sptMissile);
    }else{
        heroMissileArray.pushBack(sptMissile);
    }
}
//void HelloWorld::gettingReadyForBattle(float dt){
//    battleReadyCountDown--;
//    if (battleReadyCountDown == 2) {
//        Label* lbl = Label::createWithTTF("READY", GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//        GameManager::getInstance()->getHudLayer()->addChild(lbl);
//        lbl->setPosition(size/2);
//        lbl->setScale(0.2f);
//        lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)), nullptr));
//    }else if (battleReadyCountDown == 1) {
//        Label* lbl = Label::createWithTTF("BATTLE!", GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//        GameManager::getInstance()->getHudLayer()->addChild(lbl);
//        lbl->setPosition(size/2);
//        lbl->setScale(0.2f);
//        lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)), nullptr));
//    }else if (battleReadyCountDown == 0) {
//        this->unschedule(schedule_selector(HelloWorld::gettingReadyForBattle));
//        isBattleStarted = true;
//    }
//}
HelloWorld::HelloWorld(){
    
}
void HelloWorld::getDown(){
   
}
void HelloWorld::dialogFinished(Ref* obj){
    
}
void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//        NativeInterface::NativeInterface::destroyAds();
#endif
        
//        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void HelloWorld::showWarpAnimation(){
    Sprite* sptBeam = Sprite::createWithSpriteFrameName("blueBeamVertical0.png");
    sptBeam->setAnchorPoint(Point(0.5, 0));
    sptBeam->setScaleY(10);
    sptBeam->setScaleX(2);
    spriteBatch->addChild(sptBeam, 11);
    const char* beamName = "blueBeam";
    if (stageIndex == indexToWarp || (isBossMap && !isSetStageDone)) {
        beamName = "redBeam";
    }
    Animation* animation = AnimationCache::getInstance()->getAnimation(beamName);
    Animate* animate = Animate::create(animation);
    sptBeam->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptBeam)), CallFunc::create(CC_CALLBACK_0( HelloWorld::checkEnding, this)), NULL));
    Point pos;
    
    sptBeam->setPosition(pos + Point(0, 100));
    sptBeam->runAction(MoveTo::create(0.05, pos));
    
    
    this->runAction(Sequence::create(DelayTime::create(18*0.04), CallFunc::create(CC_CALLBACK_0(HelloWorld::stopWoongSound, this)), nullptr));
    shakeScreen(6);
    
    
//    this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(HelloWorld::testFunction, this)), NULL));
}
void HelloWorld::checkEnding(){
    
}
void HelloWorld::testFunction(){
    GameManager::getInstance()->getHudLayer()->dialogBox->removeFromParent();
    
}
void HelloWorld::takeAction(){
    log("take action");
    GameManager::getInstance()->getHudLayer()->lblTouch->stopAllActions();
    GameManager::getInstance()->getHudLayer()->lblTouch->setOpacity(0);
    
}
void HelloWorld::warpOut(){
    
}
void HelloWorld::warp(){
    GameManager::getInstance()->getHudLayer()->showStageTitle();
    
    this->runAction(Sequence::create(DelayTime::create(4), CallFunc::create(CC_CALLBACK_0(HelloWorld::goToNextScene, this)), NULL));
}
void HelloWorld::goToNextScene(){
    GameManager::getInstance()->getHudLayer()->removeListener();
    Scene* scene = Scene::create();
    bool isBoss = false;
    if (stageIndex >= 0 && !isBossMap) {
        isBoss = true;
    }
    scene->addChild(HelloWorld::scene(indexToWarp, isBoss));
    Director::getInstance()->replaceScene(scene);
}
void HelloWorld::enemyUpdate(float dt)
{
    if (isGameOver) {
        return;
    }
//    dnDamageBoxes->clear();
//    dnDamageBoxes->drawRect(player->damageBoundingBox().origin, player->damageBoundingBox().origin + player->damageBoundingBox().size, Color4F::RED);
//    dnCollisionBoxes->clear();
//    dnCollisionBoxes->drawRect(player->collisionBoundingBox().origin, player->collisionBoundingBox().origin + player->collisionBoundingBox().size, Color4F::BLUE);
    Rect projectileRect;
    bool isEffectOn = false;
    missileEffectCollapsedTime += dt;
    if(missileEffectCollapsedTime < 0.023){
        
    }else{
        isEffectOn = true;
        missileEffectCollapsedTime = 0;
    }
    
    for (auto drop: enemyArray){
        
        
        projectileRect = drop->damageBoundingBox();

        if(isEffectOn){
            
            if(drop->effectType == MISSILE_EFFECT_SMOKE)
                runEffect(EFFECT_SMOKE, drop->getPosition());
            if(drop->effectType == MISSILE_EFFECT_RED_BALL)
                runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_GREEN_SMOKE)
                runEffect(EFFECT_GREEN_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_RED_SMOKE)
                runEffect(EFFECT_RED_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_SHARK_GUN){
                
            }
        }
        
    }
    
    for (auto missile: enemyMissileArray) {
        projectileRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                              missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                              missile->getContentSize().width*missile->getScaleX(),
                              missile->getContentSize().height*missile->getScaleY());
        projectileRect = RectInset(projectileRect, 3, 3);
        
        bool toBreak = false;
        for(auto hero: heroArray){
            if (projectileRect.intersectsRect(hero->getBoundingBox()) && missile->isHurting) {
                if (!hero->immortal) {
                    attackHero(hero, missile->energy);
                }
                enemyMissileArray.eraseObject(missile);
                missile->removeFromParent();
            }
        }
        if(toBreak){
            break;
        }
    }
}
void HelloWorld::updateFireStick(float dt){
    
}
void HelloWorld::destructableUpdate()
{
    if (isGameOver) {
        return;
    }
    
    Rect projectileRect, mRect;
    Vector<Movable*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        for (int i = 0; i<destructableArray.size(); i++) {
            drop = destructableArray.at(i);
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
//            mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
//                         missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
//                         missile->getContentSize().width*missile->getScaleX(),
//                         missile->getContentSize().height*missile->getScaleY());
            mRect = missile->getBoundingBox();
            projectileRect = drop->damageBoundingBox();
            if (projectileRect.intersectsRect(mRect)) {
                if (!drop->immortal) {
                    float demage = missile->energy;
                    
                    if (demage >= 10) {
                        demage = demage*0.8f + (rand()%((int)(demage*0.2f)));
                    }
                    
                    drop->energy -= (int)demage;
                    if (drop->energy <= 0) {
                        destroyDestructable(drop);
                        currentScore += drop->secondTag*10;
                        
                        
                        destructableArray.eraseObject(drop);
                        drop->removeFromParentAndCleanup(true);
                        
                        GameManager::getInstance()->totalDestroy++;
                        GameManager::getInstance()->totalScore += 10;
                    }
                    
                }
                
                missileToRemove.pushBack(missile);
                this->runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition(), missile->getRotation() + 180);
                
                break;
            }
        }
    }
    
    for (auto spt: missileToRemove){
        spt->stopAllActions();
        heroMissileArray.eraseObject(spt);
        enemyMissileArray.eraseObject(spt);
        MovableMissileArray.eraseObject(spt);
        enemyArray.eraseObject((EnemyBase*)spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_Movable){
            MovableMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
//    enemiesToRemove.clear();
}
void HelloWorld::destroyDestructable(Movable* drop){
    if (drop->secondTag < 0) {
        return;
    }
    const char* partName;
    if (drop->secondTag < 3) {
        partName = __String::createWithFormat("destructable%d_0.png", drop->secondTag)->getCString();
        
    }else{
        partName = "destructable3_0.png";
        
        
    }
    for (int i = 0; i < 3; i++) {
        Movable* part = Movable::createMovable(UNIT_DESTRUCTABLE, 0, 0, partName);
        spriteBatch->addChild(part, 1);
        part->setPosition(drop->getPosition());
        part->weight = 1;
        auto rotate = RotateBy::create(1, rand()%2 == 0?300:-300);
        rotate->setTag(ACTION_TAG_ROTATION);
        part->runAction(rotate);
        part->velocity = Point(rand()%500 - 250, 250);
        part->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::coinWaitDone, this)), NULL));
        MovableCoinArray.pushBack(part);
    }
    runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
}
void HelloWorld::showDamage(int damage, Point pos){
    if (damage <= 0) {
        return;
    }
    
    Node* sptNumber = createNumberSprite(damage);
    spriteBatch->addChild(sptNumber, 100);
    sptNumber->setScale(1.5f);
    sptNumber->setPosition(pos);
    sptNumber->runAction(EaseIn::create(MoveBy::create(1, cocos2d::Point(0, 25)), 0.3));
    sptNumber->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptNumber)), NULL));
}
void HelloWorld::attackEnemy(EnemyBase* drop, int damage){
//    damage = 100; // test 
    
    damage -= drop->dp;
    if(damage <= 0){
        damage = 1;
    }
    drop->energy -= (int)damage;
    drop->onHit();
    showDamage(damage, drop->getPosition());
    
    if (drop->energy <= 0) {
        enemiesToRemove.pushBack(drop);
    }
    else
    {
        drop->blinkForAWhile();
    }
    
    shakeScreen(1);
    isPaused = true;
    this->runAction(Sequence::create(DelayTime::create(0.07f), CallFunc::create(CC_CALLBACK_0(HelloWorld::resumeLayer, this)), NULL));
}

void HelloWorld::attackHero(EnemyBase* drop, int damage){
    damage -= drop->dp;
    
    
    if(damage <= 0){
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
        spriteBatch->removeChild(drop, true);
        spriteBatchBuilding->removeChild(drop, true);
        removeHero(drop);
    }
    else
    {
        drop->blinkForAWhile();
    }
    
}
void HelloWorld::immortalForSec(int sec){
    indestructableTime = sec;
    this->schedule(schedule_selector(HelloWorld::immortalForAWhile), 0.1);
}
void HelloWorld::immortalForAWhile(float dt){
    indestructableTime -= dt;
    if (indestructableTime < 0) {
        this->unschedule(schedule_selector(HelloWorld::immortalForAWhile));
    }
}
bool HelloWorld::hitPlayer(int damage){
//    return false; // test
    
    return false;
}
void HelloWorld::stepBackForHit(float dt)
{
}
void HelloWorld::unfreezePlayer(float dt)
{
}

Rect HelloWorld::RectInset(Rect rect, float x, float y)
{
    return Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}


void HelloWorld::endGame(bool win){
    this->unschedule(schedule_selector(HelloWorld::gravityUpdate));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForCoins));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForStraight));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForCustomMoving));
    this->unschedule(schedule_selector(HelloWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    
    for(auto unit: enemyArray) {
        unit->stopAllActions();
        unit->unscheduleAllCallbacks();
    }
    for(auto unit: heroArray) {
        unit->stopAllActions();
        unit->unscheduleAllCallbacks();
    }
    for(auto unit: mutualArray) {
        unit->stopAllActions();
        unit->unscheduleAllCallbacks();
    }
    for(auto unit: readyHeroArray) {
        unit->stopAllActions();
        unit->unscheduleAllCallbacks();
    }
    
    for(auto enemy : enemyArray){
        enemy->pause();
    }
    isGameOver = !win;
    if (GM->nextScene != STAGE_RAID && GM->nextScene != STAGE_SINGLEPLAY){
        HUD->showWinPopup(win);
    }
    
    if(win){
        if(GM->isColosseum){
            GM->isColosseum = false;
            sendColosseumScore();
            UDSetBool(KEY_DAILY_MISSION_ARENA, true);
        }else if(GM->nextScene == STAGE_SINGLEPLAY){
            UDSetBool(KEY_DAILY_MISSION_LOCAL_BATTLE, true);
        }else if(GM->nextScene == STAGE_RAID){
            UDSetBool(KEY_DAILY_MISSION_NETWORK_BATTLE, true);
        }else{
            int stageIndex = UDGetInt(KEY_DAY_COUNT, 0)%12;
            if(GM->currentStageIndex == stageIndex){
                UDSetBool(KEY_DAILY_MISSION_CAMPAIGN, true);
            }
        }
    }
    
}
void HelloWorld::sendTestScore(){
    gameTimer = 60*9;//rand()%1000;
    totalKillUnit = 6;
    totalProducedUnit = rand()%1000;
    
    isGameOver = true;
    
    sendColosseumScore();
}
void HelloWorld::sendColosseumScore(){
    HUD->showIndicator();
//    std::string playerId = GameSharing::getPlayerID();
//    std::string playerName = GameSharing::getPlayerName();
//#ifdef SDKBOX_ENABLED
//    playerId = sdkbox::PluginSdkboxPlay::getPlayerId();
//    playerName = sdkbox::PluginSdkboxPlay::getPlayerAccountField("name");
//#endif
//    UDSetInt(KEY_COLOSSEUM_SCORE, 33); // test
    

    
    int lastScore = UDGetInt(KEY_COLOSSEUM_SCORE, 0);
    int clearCount = UDGetInt(KEY_ARENA_CLEAR_COUNT, 0);
    clearCount++;
    UDSetInt(KEY_ARENA_CLEAR_COUNT, clearCount);
    if(lastScore == 0 || lastScore > gameTimer){
        BSM->sendArenaScore(gameTimer);
        UDSetInt(KEY_COLOSSEUM_SCORE, gameTimer);
    }else{
        HUD->hideIndicator();
    }
    
//    if(playerId.size() > 0 && gameTimer > 0){
//        SM->sendColosseumScore(gameTimer, totalKillUnit, totalProducedUnit, playerId, playerName, UDGetInt(KEY_CURRENT_COLOSSEUM_LEAGUE, 0), isGameOver?0:1);
//    }
}
void HelloWorld::onSendComplete(int leagueIndex){
    HUD->hideIndicator();
    log("league index: %d", leagueIndex);
    UDSetInt(KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX, leagueIndex);
}

void HelloWorld::updateResult(float dt){
    if (resultDone) {
        this->unschedule(schedule_selector(HelloWorld::updateResult));
        return;
    }
    
    Text* lbllblTotalScoreValue = (Text*)resultLayer->getChildByName("lblTotalScoreValue");
    Text* lblKill = (Text*)resultLayer->getChildByName("lblKill");
    Text* lblKillValue = (Text*)resultLayer->getChildByName("lblKillValue");
    lblKill->setVisible(true);
    lblKillValue->setVisible(true);
    if (kill < GameManager::getInstance()->totalKill) {
        kill++;
        lblKillValue->setString(Value(kill).asString());
        
        totalScore += 100;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblDestroy = (Text*)resultLayer->getChildByName("lblDestroy");
    Text* lblDestroyValue = (Text*)resultLayer->getChildByName("lblDestroyValue");
    lblDestroy->setVisible(true);
    lblDestroyValue->setVisible(true);
    if (destroy < GameManager::getInstance()->totalDestroy) {
        destroy++;
        lblDestroyValue->setString(Value(destroy).asString());
        
        totalScore += 10;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblCoin = (Text*)resultLayer->getChildByName("lblCoin");
    Text* lblCoinValue = (Text*)resultLayer->getChildByName("lblCoinValue");
    lblCoin->setVisible(true);
    lblCoinValue->setVisible(true);
    if (coin < GameManager::getInstance()->totalCoin) {
        int coinAmountToAdd = 1;
        if (coin + 10 < GameManager::getInstance()->totalCoin) {
            coinAmountToAdd = 10;
        }else if (coin + 100 < GameManager::getInstance()->totalCoin) {
            coinAmountToAdd = 100;
        }
        coin += coinAmountToAdd;
        lblCoinValue->setString(Value(coin).asString());
        
        totalScore += 10*coinAmountToAdd;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblRoom = (Text*)resultLayer->getChildByName("lblRoom");
    Text* lblRoomValue = (Text*)resultLayer->getChildByName("lblRoomValue");
    lblRoom->setVisible(true);
    lblRoomValue->setVisible(true);
    if (room < GameManager::getInstance()->totalRoom) {
        room++;
        lblRoomValue->setString(Value(room).asString());
        
        totalScore += 500;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblBoss = (Text*)resultLayer->getChildByName("lblBoss");
    Text* lblBossValue = (Text*)resultLayer->getChildByName("lblBossValue");
    lblBoss->setVisible(true);
    lblBossValue->setVisible(true);
    if (boss < GameManager::getInstance()->totalBoss) {
        boss++;
        lblBossValue->setString(Value(boss).asString());
        
        totalScore += 10000;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblTime = (Text*)resultLayer->getChildByName("lblTime");
    Text* lblTimeValue = (Text*)resultLayer->getChildByName("lblTimeValue");
    lblTime->setVisible(true);
    lblTimeValue->setVisible(true);
    
    if (time < GameManager::getInstance()->totalTime) {
        time+=60;
        if (time > GameManager::getInstance()->totalTime) {
            time = GameManager::getInstance()->totalTime;
        }
        lblTimeValue->setString(String::createWithFormat("%2d:%2d", (60*60 - time)/60, (60*60 - time)%60)->getCString());
        
        totalScore += 600;
        lbllblTotalScoreValue->setString(Value(totalScore).asString());
        return;
    }
    Text* lblOk = (Text*)resultLayer->getChildByName("lblOk");
    resultDone = true;
    lblOk->setVisible(true);
    
    Sprite* cursor = Sprite::create("targetWhite.png");
    resultLayer->addChild(cursor, 99990);
    cursor->runAction(RepeatForever::create(RotateBy::create(1, 100)));
    cursor->setPosition(lblOk->getPosition() + Point(-20, 0));
    
    Sprite* innerCursor = Sprite::create("targetWhite.png");
    cursor->addChild(innerCursor, 99990);
    innerCursor->runAction(RepeatForever::create(RotateBy::create(1, -160)));
    innerCursor->setScale(0.7);
    innerCursor->setPosition(Point(cursor->getContentSize().width/2, cursor->getContentSize().height/2));
}
void HelloWorld::gameOver()
{
    pauseLayer();
    isGameOver = true;
    
    for (auto drop: enemyMissileArray) {
        drop->removeFromParentAndCleanup(true);
    }
    enemyMissileArray.clear();
    
    
}

void HelloWorld::missileUpdate(float dt)
{
    if (isGameOver) {
        return;
    }

    Rect mRect;
//    enemiesToRemove.clear();

    Vector<Movable*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        mRect = Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                            missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                            missile->getContentSize().width*missile->getScaleX(),
                            missile->getContentSize().height*missile->getScaleY());
        for (int i = 0; i<enemyArray.size(); i++) {
            drop = enemyArray.at(i);
            
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            if (drop->damageBoundingBox().intersectsRect(mRect)) {
                Point pos = drop->getPosition();
                if(!drop->untouchable){
                    if (!drop->immortal) {
                        float damage = missile->energy + missile->energy*extraPower/100.0f;
                        
                        if (damage >= 10) {
                            damage = damage*0.8f + (rand()%((int)(damage*0.4f)));
                        }
                        
                        attackEnemy(drop, damage);
                        
                        if (false && rand()%100 > 100 - DEFAULT_CRITICAL - extraCritical - criticalLevel) {//critical
                            damage*=2;
                            Sprite* critical = Sprite::createWithSpriteFrameName("critical.png");
                            spriteBatch->addChild(critical, 100);
                            critical->getTexture()->setAliasTexParameters();
                            //                            critical->setColor(aColor3B(0,0,0));
                            critical->setScale(0.5f);
                            critical->setPosition(drop->getPosition() + Point(0, 15));
                            critical->runAction(EaseIn::create(MoveBy::create(1, Point(0, 25)), 0.3));
                            critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
                            
                            shakeScreen(1);
                        }
                        
//                        totalPower += damage;
                        if (drop == theBoss) {
//                            GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                        }else{
                            if(MovableArray.find(drop) != MovableArray.end()){
                                drop->desiredPosition = drop->desiredPosition + Point(0, 2);
                                drop->setPosition(drop->getPosition() + Point(0, 2));
                            }
                        }
                        
                        if (drop->moveTypeAfterFindHero != drop->currentMoveType) {
                            //                    drop->doActionAfterFindTarget();
                        }
                        
                        if (drop->energy <= 0) {
                            
                        }
                        else
                        {
                            drop->blinkForAWhile();
                        }
                        
                        
                    }
                    if(missile->secondTag != WEAPON_ENERGY_GUN){
                        missileToRemove.pushBack(missile);
                    }else{
                        
                    }
                    this->runEffect(EFFECT_EXPLODE_SMALL, pos);
                    
                    break;
                }
            }
        }
    }

    int killCount = 0;
    for (auto spt: enemiesToRemove){
        removeEnemy(spt);
    }

    for (auto spt: missileToRemove){
        spt->stopAllActions();
        heroMissileArray.eraseObject(spt);
        if(spt->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_Movable){
            MovableMissileArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_CUSTOM){
            customMovingArray.eraseObject(spt);
        }else if(spt->getTag() == UNIT_MISSILE_STRAIGHT){
            straightMovingArray.eraseObject(spt);
        }
        spt->removeFromParentAndCleanup(true);
    }
    enemiesToRemove.clear();
}
void HelloWorld::makeGate(float dt){
    
}
void HelloWorld::bossExplode(float dt){
    if (rand()%2 == 0) {
        runEffect(EFFECT_EXPLODE_HUGE, theBoss->getPosition() + Point(rand()%20 - 10, rand()%20 - 30));
    }else{
        runEffect(EFFECT_BOMB_WALL, theBoss->getPosition() + Point(rand()%20 - 10, rand()%30 - 30));
    }
}
void HelloWorld::shakeScreenOnce(){
    shakeScreen(2);
}
void HelloWorld::shakeScreen(int count){
    Vector<FiniteTimeAction*> array;
    for (int i = 0; i < count; i++) {
//        array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenFirst, this)));
//        array.pushBack(DelayTime::create(0.03));
//        array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenSecond, this)));
//        array.pushBack(DelayTime::create(0.05));
        array.pushBack(MoveBy::create(0.07, Point(4, 0)));
        array.pushBack(MoveBy::create(0.07, Point(-4, 0)));
    }
//    array.pushBack(CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenEnd, this)));
//    array.pushBack(DelayTime::create(0.03));
    this->runAction(Sequence::create(array));
}
void HelloWorld::shakeScreenFirst(){
//    extraCameraPos = Point((rand()%10)*0.1, (rand()%10)*0.1);
    extraCameraPos = Point(1, 0);
}
void HelloWorld::shakeScreenSecond(){
//    extraCameraPos = Point(-(rand()%10)*0.1, -(rand()%10)*0.1);
    extraCameraPos = Point(-1, 0);
}
void HelloWorld::shakeScreenEnd(){
    extraCameraPos = Point::ZERO;
}
void HelloWorld::removeHero(EnemyBase* spt){
    Movable* drop;
    for (int i = 0; i < chasingMissileArray.size(); i++){
        drop = (Movable*)chasingMissileArray.at(i);
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
    if(fireableArray.find((FireableBase*)spt) == fireableArray.end()){
        fireableArray.eraseObject((FireableBase*)spt);
    }
    
    spt->sptWeapon->removeFromParent();
    spt->removeFromParentAndCleanup(true);
}
void HelloWorld::creatingStarToGreatBall(float dt){
    for(int i = 0; i < 3; i++){
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spt->setScale(0.1f);
        spt->setPosition(GM->getRandomPosInCicle(greatBall->getPosition(), 6));
        spriteBatch->addChild(spt, 100);
        spt->runAction(RepeatForever::create(Sequence::create(FadeTo::create(0.05f, 50 + rand()%50), DelayTime::create(0.01f*(rand()%40)), FadeTo::create(0.05f, 240), NULL)));
        float duration = 1 + (rand()%10)*0.2f;
        spt->runAction(Sequence::create(DelayTime::create(duration), SPT_REMOVE_FUNC, NULL));
        spt->runAction(MoveBy::create(duration, Point(0, 16 + rand()%10)));
    }
}
void HelloWorld::removeDeadUnit(EnemyBase* unit){
    if(!unit) return;
    if (unit->isEnemy) {
        totalKillUnit++;
    }else{
        if (clearConditionIndex == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE) {
            bool allyElli = true;
            for (auto unit: heroArray) {
                if(unit->isAlli && unit->unitType == UNIT_ORC_HQ && unit->energy > 0){
                    allyElli = false;
                }
            }
            if (allyElli) {
                endGame(false);
            }
        }
    }
    if(unit->isBuilding){
        setOccupy(getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y), unit->buildingOccupySize.width, unit->buildingOccupySize.height, false);
        updateMiniMapForNonMoving();
        resetPathState();
    }
    if(unit->energyBar != nullptr){
        unit->energyBar->removeFromParent();
        unit->energyBar = nullptr;
        unit->energyBarContent->removeFromParent();
        unit->energyBarBack->removeFromParent();
    }
    deselect(unit);
    heroArray.eraseObject(unit);
    enemyArray.eraseObject(unit);
    mutualArray.eraseObject(unit);
    MovableArray.eraseObject(unit);
    bool elli = true;
    for (auto unit: heroArray) {
        if(unit->isBuilding && unit->energy > 0){
            elli = false;
        }else if(unit->attackType != ATTACK_TYPE_NONE){
            elli = false;
        }
    }
    if (elli) {
        endGame(false);
    }
    
    if(unit->unitType == UNIT_ZOMBIE_ORC_AXE || unit->unitType == UNIT_ZOMBIE_SWORDSMAN){
        unit->removeFromParent();
    }else if(unit->isBuilding || unit->unitType == UNIT_CATAPULT|| unit->unitType == UNIT_GOBLIN_BOMB){
        runEffect(EFFECT_EXPLODE_BIG, unit->getPosition());
        unit->removeFromParent();
    }else if(unit->unitType == UNIT_HELICOPTER){
        unit->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.5f, Point(200, 300)), 2), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
        unit->setAnchorPoint(Point(0, 0));
        unit->runAction(EaseInOut::create(RotateBy::create(0.5f, 720), 2));
    }else{
        std::string name = unit->getName();
        float disappearTime = 3;
        float fadeTime = 3;
        if(unit->isZombie){
            disappearTime = 0.01f;
            fadeTime = 0.1f;
        }else{
            if(name.compare("tree") != 0){
                if(unit->spine){
                    unit->spine->setAnimation(0, "dead", false);
                    unit->spine->runAction(Sequence::create(DelayTime::create(disappearTime), FadeOut::create(fadeTime), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
                }else{
                    SpriteFrameCache *cache = SpriteFrameCache::getInstance();
                    std::string imgFileName = strmake("%sDead.png", name.c_str());
                    if(unit->unitType == UNIT_ORC_SPEAR){
                        imgFileName = "orcSpearDead.png";
                    }else if(unit->unitType == UNIT_ORC_AXE){
                        imgFileName = "orcAxeDead.png";
                    }else if(unit->unitType == UNIT_GOBLIN_BOMB){
                        imgFileName = "goblinBombDead.png";
                    }else if(unit->unitType == UNIT_WORKER){
                        imgFileName = "workerDead.png";
                    }else if(unit->unitType == UNIT_GOBLIN_WORKER){
                        imgFileName = "goblinDead.png";
                    }
                    SpriteFrame *spriteFrame = cache->getSpriteFrameByName(imgFileName);
                    if(spriteFrame != nullptr && spriteFrame && unit){
                        unit->setSpriteFrame(spriteFrame);
                    }
                }
            }
        }
        unit->stopAllActions();
        unit->runAction(Sequence::create(DelayTime::create(disappearTime), FadeOut::create(fadeTime), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
        unit->setLocalZOrder(-100000);
    }
    if(!unit->isEnemy){
        updateFoodInUse();
    }
    checkClearGame();
}
void HelloWorld::removeEnemy(EnemyBase* spt){
    
    Movable* drop;
    for (int i = 0; i < chasingMissileArray.size(); i++){
        drop = (Movable*)chasingMissileArray.at(i);
        if (drop->target == spt) {
            drop->target = NULL;
        }
    }
    int thisEnemyKillCount = UDGetInt(strmake(KEY_ENEMY_KILL_COUNT_FORMAT, spt->unitType).c_str(), 0);
    thisEnemyKillCount++;
    UDSetInt(strmake(KEY_ENEMY_KILL_COUNT_FORMAT, spt->unitType).c_str(), thisEnemyKillCount);
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
    if(fireableArray.find((FireableBase*)spt) == fireableArray.end()){
        fireableArray.eraseObject((FireableBase*)spt);
    }
    this->runEffect(EFFECT_EXPLODE_SMALL, spt->getPosition());
    
    addHeroExp(0, spt->rewardExp);
    
    spiderArray.eraseObject(spt);
    if(spt->sptWeapon != nullptr)
        spt->sptWeapon->removeFromParent();
    spt->removeFromParentAndCleanup(true);
    
}

void HelloWorld::fixStageLayerFourTiles(experimental::TMXTiledMap* map){
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 2; x < map->getMapSize().width - 2; x++) {
        for (int rawY = 2; rawY < map->getMapSize().height - 2; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (!isWay(stageLayer->getTileGIDAt(Point(x, y)))) {
                if (compareFourTiles(0, 1, 1, 1, x, y, stageLayer)) {// top
                    stageLayer->setTileGID(6, Point(x, y));
                }else if (compareFourTiles(1, 0, 1, 1, x, y, stageLayer)) { // left
                    stageLayer->setTileGID(37, Point(x, y));
                }else if (compareFourTiles(1, 1, 0, 1, x, y, stageLayer)) { // right
                    stageLayer->setTileGID(39, Point(x, y));
                }else if (compareFourTiles(1, 1, 1, 0, x, y, stageLayer)) { // bottom
                    stageLayer->setTileGID(70, Point(x, y));
                }else if (compareFourTiles(0, 0, 1, 1, x, y, stageLayer)) { // left top
                    stageLayer->setTileGID(5, Point(x, y));
                }else if (compareFourTiles(0, 1, 0, 1, x, y, stageLayer)) { // right top
                    stageLayer->setTileGID(7, Point(x, y));
                }else if (compareFourTiles(1, 0, 1, 0, x, y, stageLayer)) { // left bottom
                    stageLayer->setTileGID(69, Point(x, y));
                }else if (compareFourTiles(1, 1, 0, 0, x, y, stageLayer)) { // right bottom
                    stageLayer->setTileGID(71, Point(x, y));
                }else if (compareFourTiles(1, 1, 1, 1, x, y, stageLayer)) {
                    stageLayer->setTileGID(38, Point(x, y));
                }
            }
        }
    }
}

bool HelloWorld::compareFourTiles(int t,int l, int r, int b, int x, int y, experimental::TMXLayer* stageLayer){
    bool result = true;
    if (!isWay(stageLayer->getTileGIDAt(Point(x, y - 1))) != t) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x - 1, y))) != l) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x + 1, y))) != r) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x, y + 1))) != b) {
        return false;
    }
    return result;
}

void HelloWorld::fixStageLayerTiles(experimental::TMXTiledMap* map){
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 1; x < map->getMapSize().width - 1; x++) {
        for (int rawY = 1; rawY < map->getMapSize().height - 1; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (!isWay(stageLayer->getTileGIDAt(Point(x, y)))) {
                if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // center
                    stageLayer->setTileGID(38, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right top corner
                    stageLayer->setTileGID(7, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right corner
                    stageLayer->setTileGID(39, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 0, 0, 0, x, y, stageLayer)) { // right bottom corner
                    stageLayer->setTileGID(71, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 0, x, y, stageLayer)) { // bottom corner
                    stageLayer->setTileGID(70, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 0, 0, x, y, stageLayer)) { // left bottom corner
                    stageLayer->setTileGID(69, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left corner
                    stageLayer->setTileGID(37, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left top corner
                    stageLayer->setTileGID(5, Point(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // top corner
                    stageLayer->setTileGID(6, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 1, 1, x, y, stageLayer)) { // right top wall
                    stageLayer->setTileGID(15, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // right bottom wall
                    stageLayer->setTileGID(79, Point(x, y));
                }else if (compareNineTiles(1, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // left bottom wall
                    stageLayer->setTileGID(77, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 0, x, y, stageLayer)) { // left top wall
                    stageLayer->setTileGID(13, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 1, x, y, stageLayer)) { // top wall
                    stageLayer->setTileGID(14, Point(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall top oepn
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // right wall bottom open
                    stageLayer->setTileGID(47, Point(x, y));
                }else if (compareNineTiles(0, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall left open
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall right open
                    stageLayer->setTileGID(78, Point(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall top open
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 0, x, y, stageLayer)) { // left wall bottom open
                    stageLayer->setTileGID(45, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 1, x, y, stageLayer)) { // top wall left open
                    stageLayer->setTileGID(14, Point(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 0, x, y, stageLayer)) { // top wall right open
                    stageLayer->setTileGID(14, Point(x, y));
                }
            }
        }
    }
}
bool HelloWorld::compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, experimental::TMXLayer* stageLayer){
    bool result = true;
    if (!isWay(stageLayer->getTileGIDAt(Point(x - 1, y - 1)) != lt)) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x, y - 1))) != t) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x + 1, y - 1))) != rt) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x - 1, y))) != l) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x + 1, y))) != r) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x - 1, y + 1))) != lb) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x, y + 1))) != b) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Point(x + 1, y + 1))) != rb) {
        return false;
    }
    return result;
}
void HelloWorld::backToLabelPool(Ref* ref){
    Label* lbl = (Label*)ref;
    lbl->removeFromParentAndCleanup(false);
}
Label* HelloWorld::showLabelFromPool(Node* parent, cocos2d::Point pos, std::string text, int moveHeight, float delay){
    Label* lbl = labelPool.at(labelPoolIndex);
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
    lbl->runAction(Sequence::create(DelayTime::create(delay), MoveBy::create(visibleTime+fadingTime, Point(0, moveHeight)), nullptr));
    lbl->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(visibleTime), FadeOut::create(fadingTime), CallFuncN::create(CC_CALLBACK_1(HelloWorld::backToLabelPool,   this)), NULL));
    labelPoolIndex++;
    if (labelPoolIndex >= labelPoolCount) {
        labelPoolIndex = 0;
    }
    return lbl;
}
Sprite* HelloWorld::getLightSpin(float persistTime){
    Sprite* shining = Sprite::create("lightSpin.png");
    shining->runAction(RotateBy::create(persistTime, persistTime*90));
    shining->runAction(Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    BlendFunc f = {GL_DST_COLOR, GL_DST_ALPHA};
    shining->setBlendFunc(f);
    
    Sprite* shining2 = Sprite::create("lightSpin.png");
    shining2->runAction(RotateBy::create(persistTime, -persistTime*180));
    shining2->runAction(Sequence::create(DelayTime::create(persistTime), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    f = {GL_DST_COLOR, GL_DST_ALPHA};
    shining2->setBlendFunc(f);
    shining->addChild(shining2);
    shining2->setPosition(Point(shining->getContentSize().width/2, shining->getContentSize().height/2));
    
    return shining;
}
int HelloWorld::getSlotForWeapon(int weapon){
    int index = 0;
    while(true){
        if (getWeaponAtSlot(index) == weapon) {
            return index;
        }else if(getWeaponAtSlot(index) == -1){
            return -1;
        }
        index++;
    }
}

Sprite* HelloWorld::createNumberSprite(int number)
{
    //	int place = 1;
    int digit = 0;
    Vector<Sprite*> sprites;
    if (number < 0) return NULL;
    char buf[6];
    while (true){
        digit = number % 10;
        sprintf(buf, "%d.png", digit);
        Sprite* sptNumber = Sprite::createWithSpriteFrameName(buf);
        sprites.insert(0, sptNumber);
        number /= 10;
        if (0 >= number){
            break;
        }
    }
    int startWidth = 0;
    int index = 0;
    Sprite* first;
    for (auto sprite : sprites){
        if (index == 0) {
            index++;
            startWidth = sprite->getContentSize().width;
            first = sprite;
            continue;
        }
        sprite->setPosition(Point(startWidth + sprite->getContentSize().width/2, sprite->getContentSize().height/2));
        startWidth += (sprite->getContentSize().width - 1);
        first->addChild(sprite);
    }
    sprites.clear();
    return first;
}

void HelloWorld::onjewelryBoxOpen(Ref* sender){
    GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("jewel collected").c_str(), GameManager::getInstance()->getHudLayer());
    Movable* jewelryBox = (Movable*)sender;
    Sprite* lightSpin = getLightSpin(5);
    
    
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(__String::createWithFormat("jewel%d.png", jewelryBox->secondTag)->getCString());
    sptWeapon->setOpacity(0);
    lightSpin->setOpacity(0);
    sptWeapon->setPosition(jewelryBox->getPosition());
    lightSpin->setPosition(jewelryBox->getPosition());
    sptWeapon->runAction(FadeIn::create(0.3));
    lightSpin->runAction(FadeIn::create(0.3));
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Point(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Point(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, jewelryBox->secondTag)->getCString(), true);
    //player->freezed = false;
    
    GameManager::getInstance()->collectedJewelIndex = jewelryBox->secondTag;
}
void HelloWorld::openWeapon(Ref* sender){
    Movable* suitcase = (Movable*)sender;
    Sprite* lightSpin = getLightSpin(5);
    GameManager::getInstance()->playSoundEffect(SOUND_DDALKACK);
    int var = rand()%(WEAPON_NO_MORE/4);
    if (suitcase->secondTag == 3) {
        var++;
    }
    int weapon = 1 + (WEAPON_NO_MORE/4)*suitcase->secondTag + var;
    if (weapon >= WEAPON_NO_MORE) {
        weapon = WEAPON_NO_MORE - 1;
    }else if (weapon < 1) {
        weapon = 1;
    }
    
    //int weapon = WEAPON_LIGHTNING_GUN; test
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(GameManager::getInstance()->getWeaponImageFileName(weapon));
    sptWeapon->setOpacity(0);
    lightSpin->setOpacity(0);
    sptWeapon->setPosition(suitcase->getPosition());
    lightSpin->setPosition(suitcase->getPosition());
    sptWeapon->runAction(FadeIn::create(0.3));
    lightSpin->runAction(FadeIn::create(0.3));
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Point(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Point(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    
    if (getSlotForWeapon(weapon) < 0) {
        addWeapon(weapon);
    }else{
        recharge(getSlotForWeapon(weapon));
    }
}
void HelloWorld::recharge(int slot){
    int weapon = getWeaponAtSlot(slot);
    if (currentSlot == slot) {
        totalBulletCount = getMaxTotalBulletCount(weapon);
        loadedBulletCount = getMaxLoadedBulletCount(weapon);
    }else{
        setTotalBulletCountAtSlot(slot, getMaxTotalBulletCount(weapon));
        setLoadedBulletCountAtSlot(slot, getMaxLoadedBulletCount(weapon));
    }
    
    changeWeapon(slot);
}
void HelloWorld::addWeapon(int weapon){
    int index = 0;
    while(getWeaponAtSlot(index) >= 0){
        index++;
    }
    setWeaponAtSlot(index, weapon);
    setWeaponAtSlot(index+1, -1);
    setLoadedBulletCountAtSlot(index, getMaxLoadedBulletCount(weapon));
    setTotalBulletCountAtSlot(index, getMaxTotalBulletCount(weapon));
    changeWeapon(index);
}
int HelloWorld::getTotalBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void HelloWorld::setTotalBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}
int HelloWorld::getLoadedBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void HelloWorld::setLoadedBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}

void HelloWorld::changeWeapon(int slot){
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
    GameManager::getInstance()->getHudLayer()->setBulletCount(loadedBulletCount,loadedBulletCountMax);
    GameManager::getInstance()->getHudLayer()->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
}
void HelloWorld::removeLaser(){
    if (laser!=nullptr) {
        laser->removeFromParent();
        laser = nullptr;
        
    }
}
int HelloWorld::getWeaponAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), -1);
}
void HelloWorld::setWeaponAtSlot(int slot, int weapon){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), weapon);
}
Sprite* HelloWorld::getShining(float delayTime){
    Sprite* shining = Sprite::createWithSpriteFrameName("shining0.png");
    Animation* animation = AnimationCache::getInstance()->getAnimation("shining");
    Animate* animate = Animate::create(animation);
    shining->runAction(Sequence::create(DelayTime::create(delayTime), MoveBy::create(0.7, Point(6, 6)), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), RotateBy::create(0.7, 250), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    
    return shining;
}
void HelloWorld::npcUpdate(float dt){
    
}
void HelloWorld::coinUpdate(float dt){//updateCoin
   
}
void HelloWorld::teleportLaterForThemeThree(float dt){
    
}
void HelloWorld::teleportLater(float dt){
    
}

void HelloWorld::jumpBySpring(float dt)
{
}
void HelloWorld::runEffect(int effect, Point point, float angle){
    float duration = 0.2;
    float baseDuration = 0.08;
    duration = baseDuration*2 + baseDuration*(rand()%3 + 1);
    if(effect == EFFECT_EXPLODE_SMALL){
        duration = 0.15;
        int radius = 20;
        for (int i = 0; i < 10; i++) {
            Sprite* explode = Sprite::createWithSpriteFrameName(strmake("snowFlake%d.png", rand()%4));
            spriteBatch->addChild(explode, 5);
            explode->setScale(1);
            explode->runAction(Sequence::create(MoveBy::create(duration, Point(rand()%(radius*2) - radius, rand()%(radius*2) - radius)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)), NULL));
            explode->runAction(Sequence::create(DelayTime::create(duration/2), FadeOut::create(duration/2), nullptr));
            explode->runAction(RotateBy::create(duration, rand()%260 - 130));
            explode->setPosition(point);
        }
        
        duration = 0.09f;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spriteBatch->addChild(spt, 5);
        spt->setColor(Color3B::BLACK);
        spt->runAction(Sequence::create(TintTo::create(0.07f, 255, 255, 255), DelayTime::create(duration), SPT_REMOVE_FUNC, NULL));
        spt->setPosition(point);
        
        radius = 5;
        duration = 1.0f;
        for (int i = 0; i < 15; i++) {
            Sprite* explode = Sprite::createWithSpriteFrameName(strmake("snowFlake%d.png", rand()%4));
            spriteBatch->addChild(explode, 5);
            explode->setColor(Color3B(90, 90, 90));
            explode->setOpacity(0);
            explode->runAction(Sequence::create(MoveBy::create(duration, Point(0, 10)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)), NULL));
            explode->runAction(Sequence::create(DelayTime::create(duration/4), FadeTo::create(duration/4, 100), DelayTime::create(duration/4), FadeOut::create(duration/4), nullptr));
//            explode->setScale(0.8);
            explode->runAction(RotateBy::create(duration, rand()%260 + 250));
            explode->setPosition(point + Point(rand()%(radius*2) - radius, rand()%(radius*2) - radius));
        }
    }else if (effect == EFFECT_EXPLODE_MIDDLE) {
        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
        spriteBatchEffect->addChild(spt, 5);
        spt->setPosition(point);
        GM->runAnimation(spt, "cartoonyExplosion", false);
        spt->runAction(Sequence::create(DelayTime::create(0.04*14), SPT_REMOVE_FUNC, nullptr));
    }else if (effect == EFFECT_FIREBALL_EXPLOSION) {
        for (int i = 0; i < 5; i++) {
            Sprite* spt = Sprite::createWithSpriteFrameName("groundExplosion0.png");
            spriteBatchEffect->addChild(spt, 5);
            spt->setPosition(point + Vec2(rand()%100 - 50, rand()%100 - 50));
            GM->runAnimation(spt, "fireExplosion", false);
            spt->runAction(Sequence::create(DelayTime::create(0.07*8), SPT_REMOVE_FUNC, nullptr));
        }
        
    }else if (effect == EFFECT_EXPLODE_BIG) {
        GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyExplosion0.png");
        spriteBatchEffect->addChild(spt, 5);
        spt->setPosition(point);
        GM->runAnimation(spt, "cartoonyExplosionBig", false);
        spt->runAction(Sequence::create(DelayTime::create(0.04*14), SPT_REMOVE_FUNC, nullptr));
    }else if (effect == EFFECT_HIT) {
        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyHitEffect0.png");
        spriteBatchEffect->addChild(spt, 5);
        spt->setPosition(point);
        spt->setRotation(angle);
        GM->runAnimation(spt, "cartoonyHitEffect", false);
        spt->runAction(Sequence::create(DelayTime::create(0.06*4), SPT_REMOVE_FUNC, nullptr));
    }else if(effect == EFFECT_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spriteBatch->addChild(spt, 100);
        spt->setPosition(point + Point(4-rand()%8, 4));
        spt->setScale((rand()%5 + 5)*0.1f);
        spt->setOpacity(100);
//        spt->setRotation(rand()%360);
        int num = 150 + rand()%100;
        spt->setColor(Color3B(num,num,num));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseOut::create(MoveBy::create(duration, Point(8 - rand()%16, 0)), 4));
        spt->runAction(FadeOut::create(duration));
        spt->runAction(TintTo::create(duration, 0, 0, 0));
        spt->runAction(ScaleBy::create(duration, 1.5f));
        
        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_TWINKLE){
        Sprite* spt = Sprite::createWithSpriteFrameName("twinkle0.png");
        spriteBatch->addChild(spt, 11);
        Animation* animation = AnimationCache::getInstance()->getAnimation("twinkle");
        Animate* animate = Animate::create(animation);
        spt->runAction(Sequence::create(animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        spt->setPosition(point);
    }else if(effect == EFFECT_LIGHTNING){
        Sprite* spt = Sprite::createWithSpriteFrameName("lightning0.png");
        spt->setAnchorPoint(Vec2(0.5, 0));
        spriteBatch->addChild(spt, 11);
        Animation* animation = AnimationCache::getInstance()->getAnimation("effectLightning");
        Animate* animate = Animate::create(animation);
        spt->runAction(Sequence::create(animate, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        spt->setPosition(point + Vec2(0, -49));
        
        spt = Sprite::create("whiteBigCircle.png");
        addChild(spt);
        spt->setPosition(point + Vec2(0, 100));
        spt->setOpacity(0);
        spt->setColor(Color3B::BLACK);
        spt->setScale(4);
        spt->runAction(Sequence::create(DelayTime::create(0.06f*5), FadeTo::create(0, 60), DelayTime::create(0.1f), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        
        spt = Sprite::create("whiteBigCircle.png");
        addChild(spt);
        spt->setPosition(point);// + Vec2(0, 49));
        spt->setOpacity(0);
        spt->setBlendFunc(BlendFunc::ADDITIVE);
        spt->setColor(Color3B(80, 250, 255));
        spt->runAction(Sequence::create(DelayTime::create(0.06f*4), FadeTo::create(0, 120), ScaleTo::create(0.1f, 2), FadeOut::create(0.2f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        
        for (int i = 0; i < 5; i++) {
            spt = Sprite::create("whiteBigCircle.png");
            addChild(spt);
            spt->setPosition(point);// + Vec2(0, 49));
            spt->setOpacity(0);
            spt->setScale(0.2f);
            spt->setBlendFunc(BlendFunc::ADDITIVE);
            spt->setColor(Color3B(0, 240, 250));
            int distance = 180;
            float angle = 3.14f*(rand()%360)/180;
            spt->runAction(Sequence::create(DelayTime::create(0.06f*4 + 0.1f), FadeOut::create(0.2f), NULL));
            spt->runAction(Sequence::create(DelayTime::create(0.06f*4), FadeTo::create(0, 120), MoveBy::create(0.2f, Vec2(distance*cos(angle), distance*sin(angle))), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
        }
        
        spt = Sprite::createWithSpriteFrameName("lightning5.png");
        spt->setOpacity(0);
        addChild(spt, WORLD->spriteBatch->getLocalZOrder() + 1);
        spt->setAnchorPoint(Vec2(0.5, 0));
        spt->setPosition(point + Vec2(0, -49));
        spt->runAction(Sequence::create(DelayTime::create(0.06f*5), FadeTo::create(0, 30), DelayTime::create(0.1f), FadeOut::create(0.3f), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }
    
    return;
    if(effect == EFFECT_BULLET_WALL_SMALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletSmallEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        explode->setRotation(angle);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletSmallEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        explode->setRotation(angle);
        if (angle < -90 || angle > 90) {
            explode->setFlippedY(true);
        }
        
    }else if(effect == EFFECT_BULLET_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        
    }else if(effect == EFFECT_BOMB_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_EXPLODE_SMALL){
//        float scale = 0.5f;
        duration = 0.15;
        Sprite* explode = Sprite::createWithSpriteFrameName("bombSmallEffect0.png");
        spriteBatch->addChild(explode);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombSmallEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, explode)), NULL));
        explode->setPosition(point);
    }else if(effect == EFFECT_EXPLODE_MIDDLE){
        int particleCount = 3;
        int range = 30;
        float easeRate = 2;
        float scale = 1.2f;
        
        Sprite* explode = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(explode);
        explode->setScale(scale + (rand()%4 + 1)*0.1f - 0.2f);
        explode->setColor(Color3B(237, 70, 37));
        explode->runAction(Sequence::create(DelayTime::create(duration*0.8f), TintTo::create(duration*0.4f, 255, 205, 52), TintTo::create(duration/2, 127, 110, 105), NULL));
        explode->runAction(Sequence::create(DelayTime::create(duration*1.3f), ScaleTo::create(duration, 0.1), NULL));
        explode->runAction(Sequence::create(DelayTime::create(duration*2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        explode->setPosition(point);
        
        for (int i = 0; i < particleCount; i++) {
            Sprite* particle = Sprite::createWithSpriteFrameName("whiteExplode.png");
            spriteBatch->addChild(particle);
            int angle = rand()%360;
            float x = cos(angle*3.14/180)*range*1.1f;
            float y = sin(angle*3.14/180)*range*1.1f;
            particle->setScale(0.3);
            particle->setColor(Color3B(237, 70, 37));
            particle->runAction(FadeOut::create(baseDuration));
            particle->setPosition(point);
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(baseDuration, Point(x, y)), easeRate), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        }
    }else if(effect == EFFECT_EXPLODE_HUGE){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffect0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_GREEN_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setRotation(rand()%360);
        spt->setScale(0.3);
        spt->setColor(Color3B(130,250,130));
        spt->runAction(Sequence::create(EaseIn::create(ScaleTo::create(duration, 0.05), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseIn::create(FadeOut::create(duration), 2));
        //        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_RED_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setRotation(rand()%360);
        spt->setScale(0.5);
        spt->setColor(Color3B(237, 70, 37));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Point(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseIn::create(FadeOut::create(duration), 2));
        spt->runAction(RotateBy::create(duration, 160));
    }else if(effect == EFFECT_TWINKLE){
        int initRange = 10;
        int range = initRange;
        baseDuration = 0.3f;
        float duration = 0;
        for (int i = 0; i < 15; i++) {
            Sprite* particle = Sprite::createWithSpriteFrameName("particle.png");
            spriteBatch->addChild(particle, 1);
            int angle = rand()%360;
            range = initRange + initRange*(rand()%50)*0.02;
            float x = cos(angle*3.14/180)*range*1.1f;
            float y = sin(angle*3.14/180)*range*1.1f;
            particle->setScale(0.5 + (rand()%10)*0.05);
            particle->setColor(Color3B::YELLOW);
            duration = baseDuration + baseDuration*(rand()%10)*0.1;
            particle->runAction(FadeOut::create(duration));
            particle->setPosition(point);
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(duration, Point(x, y)), 1.6f), CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMoveDone, this)), NULL));
        }
    }
}
void HelloWorld::runEffect(int effect, Point point)
{
    runEffect(effect, point, 0);
}
/*
void HelloWorld::addGlowEffect(Sprite* sprite,const Color3B& colour, const Size& size)
{
    Point pos = Point(sprite->getContentSize().width / 2,
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

    glowSprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(scale1, scale2, NULL)));
}*/
void HelloWorld::powerTestSchedule(float dt){
    powerTestTime += dt;
    if(powerTestTime > 10){
        powerTestTime = 0;
        totalPower = 0;
    }
    powerPerSec = totalPower/powerTestTime;
    CCLOG("power/sec: %f", powerPerSec);
}
Sprite* HelloWorld::getLight(){
    Sprite* sptLight = Sprite::create("whiteBigCircle.png");
    BlendFunc f = {GL_DST_COLOR, GL_ONE};
    sptLight->setBlendFunc(f);
    sptLight->setOpacity(255);
    sptLight->setColor(Color3B(255, 180, 0));
    sptLight->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.0, 0.9), DelayTime::create(0.3), ScaleTo::create(0.0, 1), DelayTime::create(0.3), nullptr)));
    return sptLight;
}
void HelloWorld::setStageCoverOpacity(int opacity){
    for (int i = 0; i < theMap->getMapSize().width; i++) {
        for (int j = 0; j < theMap->getMapSize().height; j++) {
            Sprite* spt = stageCover->getTileAt(Point(i, j));
            if(spt){
                spt->setOpacity(opacity);
            }
        }
    }
    if(opacity < 255){
        stageCover->setLocalZOrder(100);
    }else{
        stageCover->setLocalZOrder(1);
    }
}
void HelloWorld::setLayerTag(experimental::TMXTiledMap* map){
    if(map == nullptr){
        return;
    }
    map->getLayer("unit")->setTag(TAG_UNIT_LAYER);
    map->getLayer("stage")->setTag(TAG_STAGE_LAYER);
    if(map->getLayer("stage_high")){
        map->getLayer("stage_high")->setTag(TAG_HIGH_STAGE_LAYER);
    }
//    map->getLayer("back")->setTag(TAG_BACK_LAYER);
//    map->getLayer("fore")->setTag(TAG_FORE_LAYER);
    if(map->getLayer("deco")){
        map->getLayer("deco")->setTag(TAG_DECO_LAYER);
    }
    
    if(map->getLayer("stage_cover")){
        stageCover = map->getLayer("stage_cover");
        
        stageCover->removeFromParentAndCleanup(false);
        this->addChild(stageCover, 1);
        stageCover->setPosition(stageCover->getPosition() + theMap->getPosition());
    }
}
void HelloWorld::setBossMap(int stage){
    int tilesetIndex = rand()%3;
    tilesetIndex = 2;
    
    char buf[50];
  
//    GameManager::getInstance()->getHudLayer()->showStageTitle();
    stageIndex = stage;
    
    Rect rect;
    
    experimental::TMXTiledMap* map = cocos2d::experimental::TMXTiledMap::create(buf);
    
    mapArray.pushBack(map);
    setLayerTag(map);
    this->addChild(map);
    mapRectArray.push_back(map->getBoundingBox());
    rect = map->getBoundingBox();
    map->setTag(1);
    setStage(map);
    
    if (stageIndex == 0) {
//        this->initBoss(BOSS_BAZOOKA);
    }else if (stageIndex == 1) {
//        this->initBoss(BOSS_MACHINE_GUN);
    }else if (stageIndex == 2) {
//        this->initBoss(BOSS_GIANT);
    }else if (stageIndex == 3) {
//        this->initBoss(BOSS_GIANT_HOOD);
    }
}
void HelloWorld::setEntireMap(int stage){
//    Sprite* sptRect = Sprite::create("res/258_gray_rect.png");
//    this->addChild(sptRect);
//    sptRect->setPosition(Point(128, 128));
    bool shouldBeNodeGrid = false;
    if(GM->nextScene == STAGE_CREDIT){
        theMap = cocos2d::experimental::TMXTiledMap::create("springMountain.tmx");
    }else if(GM->nextScene == STAGE_TITLE){
        theMap = cocos2d::experimental::TMXTiledMap::create("title.tmx");
    }else if(GM->nextScene == STAGE_SINGLEPLAY){
        theMap = cocos2d::experimental::TMXTiledMap::create(strmake("battleSingle%d.tmx", GM->singlePlayStageIndex));
    }else if(GM->currentStageIndex == STAGE_LOBBY || GM->nextScene == STAGE_RAID){
        theMap = cocos2d::experimental::TMXTiledMap::create("battleLobby.tmx");
    }else if(GM->nextScene == STAGE_FIELD){
        if(GM->isColosseum){
            theMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("colosseum%d.tmx", stage));
            log("sm map %d ", stage);
        }else{
            theMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("stage%d.tmx", stage));
        }
    }else if(GM->nextScene == STAGE_INTRO){
        theMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("stage%dIntro.tmx", stage));
        isGameOver = true;
        if(stage == 0){
            showNPCEvent(0);
        }else if(stage == 12){
            showNPCEvent(1);
        }
    }
    
    mapSize = theMap->getMapSize();
    mapWidth = mapSize.width*TILE_SIZE;
    mapHeight = mapSize.height*TILE_SIZE;
    GM->astar->setMap(mapSize.width, mapSize.height);
    // fog setting
    fogMapSize = Size(mapWidth/FOG_SIZE, mapHeight/FOG_SIZE);
    
    for (int j = 0; j < mapHeight; j+=FOG_SIZE) {
        for (int i = 0; i < mapWidth; i+= FOG_SIZE) {
            Fog* fog = Fog::create();
            fogArray.pushBack(fog);
            spriteBatchEffect->addChild(fog, 100);
            fog->coordinate = Point(i, j);
            fog->setPosition(Point(i + FOG_SIZE/2, j + FOG_SIZE/2));
            
            if(GM->nextScene != STAGE_FIELD){
                fog->setPosition(Vec2(-500, -500));
            }
        }
    }
    
    setLayerTag(theMap);
    setStage(theMap);
    int index = 0;
    for(auto fog: fogArray){
        int x = index%(int)fogMapSize.width;
        int y = index/(int)fogMapSize.width;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }
                int adjacentX = x + i;
                int adjacentY = y + j;
                if (adjacentX < 0 || adjacentX >= fogMapSize.width || adjacentY < 0 || adjacentY >= fogMapSize.height) {
                    continue;
                }
                fog->adjacentFogs.pushBack(fogArray.at(adjacentX + adjacentY*(int)fogMapSize.width));
            }
        }
        for (int i = -1; i < 2; i++) {
            int farX = x + i;
            if (farX < 0 || farX >= fogMapSize.width) {
                continue;
            }
            int farY = y + 2;
            if (farY >= 0 && farY < fogMapSize.height) {
                fog->farFogs.pushBack(fogArray.at(farX + farY*(int)fogMapSize.width));
            }
            farY = y - 2;
            if (farY >= 0 && farY < fogMapSize.height) {
                fog->farFogs.pushBack(fogArray.at(farX + farY*(int)fogMapSize.width));
            }
        }
        for (int i = -1; i < 2; i++) {
            int farY = y + i;
            if (farY < 0 || farY >= fogMapSize.height) {
                continue;
            }
            int farX = x + 2;
            if (farX >= 0 && farX < fogMapSize.width) {
                fog->farFogs.pushBack(fogArray.at(farX + farY*(int)fogMapSize.width));
            }
            farX = x - 2;
            if (farX >= 0 && farX < fogMapSize.width) {
                fog->farFogs.pushBack(fogArray.at(farX + farY*(int)fogMapSize.width));
            }
        }
        index++;
    }
    if(GM->currentStageIndex != STAGE_LOBBY) {
        // minimap setting
        drawMiniMapFrame = DrawNode::create();
        HUD->addChild(drawMiniMapFrame, 100);
        drawMiniMapForNonMoving = DrawNode::create();
        HUD->addChild(drawMiniMapForNonMoving, 101);
        drawMiniMapForMoving = DrawNode::create();
        HUD->addChild(drawMiniMapForMoving, 102);
        log("size: %f", size.width);
        int offsetX = 0;
        if(size.height/size.width < 700.0f/1334.0f){
            offsetX = 100;
        }
        miniMapFrameWidth = 400;
        miniMapFrameHeight = 400;
        int offset = 20;
        miniMapStartPos = cocos2d::Point(offsetX + 20, 20);
        drawMiniMapFrame->setPosition(miniMapStartPos);
        drawMiniMapForNonMoving->setPosition(miniMapStartPos);
        drawMiniMapForMoving->setPosition(miniMapStartPos);
        
        ImageView* img = ImageView::create("uiBox.png");
        HUD->addChild(img, 99);
        img->setScale9Enabled(true);
        img->setContentSize(Size(miniMapFrameWidth + offset*2 + 5, miniMapFrameHeight + offset*2 + 5));
        img->setAnchorPoint(Point(0, 0));
        img->setPosition(miniMapStartPos - Point(offset, offset));
        if (GM->nextScene == STAGE_INTRO) {
            img->setVisible(false);
            drawMiniMapFrame->setVisible(false);
            drawMiniMapForNonMoving->setVisible(false);
            drawMiniMapForMoving->setVisible(false);
        }
        if (mapSize.width/mapSize.height < miniMapFrameWidth/miniMapFrameHeight) { // fix height
            miniMapScale = miniMapFrameHeight/(mapSize.height*TILE_SIZE);
            miniMapDrawStartPos = Point(miniMapFrameWidth/2 - mapSize.width*TILE_SIZE*miniMapScale/2, 0);
            miniMapHeight = miniMapFrameHeight;
            miniMapWidth = miniMapHeight*mapSize.width/mapSize.height;
        }else{ // fix width
            miniMapScale = miniMapFrameWidth/(mapSize.width*TILE_SIZE);
            miniMapDrawStartPos = Point(0, miniMapFrameHeight/2 - mapSize.height*TILE_SIZE*miniMapScale/2);
            miniMapWidth = miniMapFrameWidth;
            miniMapHeight = miniMapWidth*mapSize.height/mapSize.width;
        }
        miniMapBit = miniMapScale*TILE_SIZE;
        drawMiniMapFrame->drawSolidRect(Point::ZERO, Point(miniMapFrameWidth, miniMapFrameHeight), Color4F::GRAY);
        drawMiniMapFrame->drawSolidRect(miniMapDrawStartPos, miniMapDrawStartPos + Point(miniMapWidth, miniMapHeight), Color4F(0.4f, 0.4f, 0.4f, 1));
        miniMapViewRect = Rect(miniMapDrawStartPos.x + (-getPosition().x)*miniMapScale, miniMapDrawStartPos.y + (-getPosition().y)*miniMapScale, size.width*miniMapScale, size.height*miniMapScale);
        updateMiniMapForNonMoving();
        if(GM->nextScene== STAGE_FIELD){
            updateFog();
        }
    }
    this->addChild(theMap);
    moveScreen(getPosition());
    
    
    if (GM->nextScene == STAGE_SINGLEPLAY || GM->nextScene == STAGE_LOBBY || GM->nextScene == STAGE_RAID) {
        for (int i = 0; i < 50; i++) {
            addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot0.png"))->setLocalZOrder(-9999);
            addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot1.png"))->setLocalZOrder(-9999);
            addDecoToOutOfField(Sprite::createWithSpriteFrameName("sandDot2.png"))->setLocalZOrder(-9999);
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

Sprite* HelloWorld::addDecoToOutOfField(Sprite* spt){
    int where = rand()%4;
    Vec2 pos;
    if(where == 0){ // north to closewise
        pos = Vec2(rand()%(int)(TILE_SIZE*mapSize.width + 800) - 400, TILE_SIZE*mapSize.height + rand()%400);
    }else if(where == 1){
        pos = Vec2(TILE_SIZE*mapSize.width + rand()%400, rand()%(int)(TILE_SIZE*mapSize.height));
    }else if(where == 2){
        pos = Vec2(rand()%(int)(TILE_SIZE*mapSize.width + 800) - 400, -rand()%400);
    }else if(where == 3){
        pos = Vec2(-rand()%400, rand()%(int)(TILE_SIZE*mapSize.height));
    }
    spt->setPosition(pos);
    this->addChild(spt, -spt->getBoundingBox().origin.y);
    return spt;
}
void HelloWorld::purgatoryUpdate(float dt){
    int startX = 8*TILE_SIZE;
    Point pos = Point(startX, (16-(-6)-1)*TILE_SIZE) + Point(-TILE_SIZE/2, TILE_SIZE/2);
    
    for (int i = 0; i < 4; i++) {
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spt->setScale(0.1f, 0.2f);
//        spt->runAction(RepeatForever::create(Waves::create(1, Size(15, 15), 4, 4, 10, true)));
        pos.x = 0;
        int color = 50 + rand()%150;
        spt->setColor(Color3B(color, color, color));
        int x = (11 - rand()%22)*TILE_SIZE;
        int midX = x*2.5f;
        pos.x = startX;
        x += startX;
        midX += startX;
        spt->setPosition(pos);
        Point targetPos = Point(x, (16-(18)-1)*TILE_SIZE) + Point(-TILE_SIZE/2, TILE_SIZE/2);
//        spt->runAction(Sequence::create(MoveTo::create(1, targetPos), SPT_REMOVE_FUNC, NULL));
        if(i == 0){
            spriteBatch->addChild(spt);
            spt->setOpacity(150);
        }else{
            
        }
        
        ccBezierConfig config;
        config.controlPoint_1 = pos;//ccp(0 , 0); //開始位置
        config.controlPoint_2 = Point(midX, (16-(6)-1)*TILE_SIZE);//ccp(0 , -100);  //曲線のベクトル(というのかな？)
        config.endPosition = targetPos;//ccp(100,100);   //終了位置
        spt->runAction(Sequence::create(BezierTo::create(1, config), SPT_REMOVE_FUNC, NULL));
        spt->runAction(ScaleTo::create(1, 1, 2));
    }
}
void HelloWorld::tournamentSchedule(float dt){
    if (enemyArray.size() > 0) {
        return;
    }
    
    if (tournamentWaveIndex == 0) {
        
    }
}
void HelloWorld::addEnemiesToMap(experimental::TMXTiledMap* map, int levelScore, bool blueKey){
    Point pos;
    int enemyCountLeft = map->getMapSize().height*map->getMapSize().width/120;
    float dur = 2;
    if (enemyCountLeft <= 0) {
        enemyCountLeft = 1;
    }
    int tryCount = 0;
    int tryCountMax = 10;
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    while (enemyCountLeft > 0) {
        while(true){
            //pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 3);
            pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), 4 + rand()%(int)(map->getMapSize().height - 6));
            if (!!isWay(map->getLayer("stage")->getTileGIDAt(pos))) {
                break;
            }
        }
        
        if (blueKey && (rand()%100 < 20 || enemyCountLeft == 1)) {
            
        }
        tryCount = 0;
        tryCountMax = 10;
        while(tryCount < tryCountMax){
            tryCount++;
            if (!isWay(stageLayer->getTileGIDAt(pos))) {
                pos = Point(pos.x, pos.y + 1);
                if (pos.y >= map->getMapSize().height) {
                    break;
                }else{
                    continue;
                }
            }
            addEnemyToLoadStack(map, levelScore, pos, -1, -1, true);
            break;
        }
        
        enemyCountLeft--;
    }
    
//    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(HelloWorld::loadEnemies, this)), NULL));
    map->getLayer("deco")->setVisible(true);
    
    // add Destructable
    tryCount = 0;
    tryCountMax = map->getMapSize().width*map->getMapSize().height/15;
    bool found = false;
    while(tryCount < tryCountMax){
        found = false;
        while(tryCount < tryCountMax){
            tryCount++;
            pos = Point(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 2);
            int tileIndex = map->getLayer("stage")->getTileGIDAt(Point(pos.x, pos.y + 1));
            if ((!isWay(tileIndex) || isOneWay(tileIndex)) && map->getLayer("stage")->getTileGIDAt(pos) == 0 && tileIndex != 322) {
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        
        Point thePos = map->getPosition() + Point(pos.x*TILE_SIZE + rand()%TILE_SIZE, (map->getMapSize().height - pos.y - 1)*TILE_SIZE + TILE_SIZE/2 - 2);
        int destructableIndex = rand()%8;
        int coinCount = 0;
        int goldPossibility=10;
        int eng = 5;
        if (destructableIndex == 6 || (destructableIndex == 7 && goldChestCount > 1)) {
            destructableIndex = 6;
            coinCount = rand()%5;
            eng = 50;
        }else if (destructableIndex == 7) {
            goldChestCount++;
            coinCount = rand()%5 + 15;
            goldPossibility = 30;
            eng = 150;
        }
        EnemyBase* sptDestructable = EnemyBase::createEnemy(UNIT_DESTRUCTABLE, eng, __String::createWithFormat("destructable%d.png", destructableIndex)->getCString(), coinCount, goldPossibility);
        sptDestructable->secondTag = destructableIndex;
        sptDestructable->setPosition(thePos);
        destructableArray.pushBack(sptDestructable);
        spriteBatch->addChild(sptDestructable);
    }
}
void HelloWorld::setEmptyMap(experimental::TMXTiledMap* map){
    int mapSizeX = map->getMapSize().width;
    int mapSizeY = map->getMapSize().height;
    
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
//    experimental::TMXLayer* decoLayer = map->getLayer("deco");
    int currentX = 3;
    int currentY = 4;
    int squareWidth = 10;
    Point pos;
    
    while(currentY < mapSizeY - 3){
        while(currentX < mapSizeX - 3){
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            /*
            if (rand()%100 < 20) {
                pos = Point(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
                if (pos.y >= mapSizeY) {
                    pos.y = mapSizeY - 1;
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

FireableBase* HelloWorld::addEnemyToLoadStack(experimental::TMXTiledMap* map, int levelScore, Point pos, int missile, int enemyModel, bool addGround){
    
    return nullptr;
}
void HelloWorld::loadEnemies(){
    for(auto enemy: enemiesToLoad){
        enemyArray.pushBack(enemy);
        MovableArray.pushBack(enemy);
        fireableArray.pushBack((FireableBase*)enemy);
        enemy->setPosition(enemyPointArray->getControlPointAtIndex(0));
        enemyPointArray->removeControlPointAtIndex(0);
        addAppearEffect(enemy);
    }
    enemyPointArray->clone();
    enemiesToLoad.clear();
}
void HelloWorld::addAppearEffect(Sprite* parent){
    Sprite* sptEffect = Sprite::createWithSpriteFrameName("bombEffect0.png");
    sptEffect->setPosition(Point(parent->getContentSize().width/2, parent->getContentSize().height/2));
    sptEffect->runAction(RotateBy::create(1, 400));
    sptEffect->setScale(0.5);
    sptEffect->runAction(Sequence::create(ScaleTo::create(0.3, 1.5), ScaleTo::create(0.2, 0.1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptEffect)), NULL));
    parent->addChild(sptEffect);
}
void HelloWorld::updateMiniMapForMoving(){
    if(GM->currentStageIndex == STAGE_LOBBY) return;
    drawMiniMapForMoving->clear();
    
    Point startPos;
    for (auto unit: heroArray) {
        if(!unit->isBuilding){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Point(miniMapBit/2, miniMapBit/2);
            if (unit->isAlli) {
                drawMiniMapForMoving->drawSolidRect(startPos, startPos + Point(miniMapBit, miniMapBit), Color4F::YELLOW);
            }else{
                drawMiniMapForMoving->drawSolidRect(startPos, startPos + Point(miniMapBit, miniMapBit), Color4F::GREEN);
            }
        }
    }
    for (auto unit: enemyArray) {
        if(!unit->isBuilding && unit->isVisible()){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Point(miniMapBit/2, miniMapBit/2);
            drawMiniMapForMoving->drawSolidRect(startPos, startPos + Point(miniMapBit, miniMapBit), Color4F::RED);
        }
    }
    drawMiniMapForMoving->drawRect(miniMapViewRect.origin, Point(miniMapViewRect.getMaxX(), miniMapViewRect.getMaxY()), Color4F::WHITE);
}

void HelloWorld::updateMiniMapForNonMoving(){
    if(GM->currentStageIndex == STAGE_LOBBY) return;
    drawMiniMapForNonMoving->clear();
    
    Point startPos;
    Point fogCoordinate;
    Fog* fogAboveUnit;
    for (int i = 0; i < mapSize.width; i ++) {
        for (int j = 0; j < mapSize.height; j++) {
            fogCoordinate = Point(i*TILE_SIZE/FOG_SIZE, (mapSize.height - j - 1)*TILE_SIZE/FOG_SIZE);
            fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
            if (fogAboveUnit->appliedState > FOG_SEEN_NOT && decoLayer->getTileGIDAt(Point(i, j)) == 49) {
                startPos = miniMapDrawStartPos + Point(i, mapSize.height - j - 1)*TILE_SIZE*miniMapScale;
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit, miniMapBit), Color4F(0.2f, 0.2f, 0.2f, 1));
            }
        }
    }
    
    for(auto fog: fogArray){
        if (fog->appliedState == FOG_SEEN_NOT || fog->appliedState == FOG_SEEN_NOT_NOW) {
            startPos = miniMapDrawStartPos + Point(fog->getBoundingBox().origin.x*miniMapScale + 2, fog->getBoundingBox().origin.y*miniMapScale + 3);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit*FOG_SIZE/TILE_SIZE, miniMapBit*FOG_SIZE/TILE_SIZE), fog->appliedState == FOG_SEEN_NOT?Color4F::BLACK:Color4F(0, 0, 0, 0.5f));
        }
    }
    
    for (auto unit: mutualArray) {
        if(!unit->isBuilding) continue;
        fogCoordinate = Point(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Point(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::YELLOW);
        }
    }
    for (auto unit: heroArray) {
        if(!unit->isBuilding) continue;
        fogCoordinate = Point(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Point(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            
            if (unit->isAlli) {
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::YELLOW);
            }else{
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::GREEN);
            }
        }
    }
    for (auto unit: enemyArray) {
        if(!unit->isBuilding) continue;
        fogCoordinate = Point(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW && unit->isDetected){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Point(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Point(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::RED);
        }
    }
}
void HelloWorld::addMapToMiniMap(experimental::TMXTiledMap* map){
}
void HelloWorld::setNamingRoom(experimental::TMXTiledMap* map){
    
}
void HelloWorld::setLobby(){
//    BHUD->loadData();
}
bool HelloWorld::isRoomEmpty(Rect rect){
    Rect targetRect;
    for (int i = 0; i < mapArray.size(); i++) {
        targetRect = mapArray.at(i)->getBoundingBox();
        
        if (intersectsRect(rect, targetRect)) {
            return false;
        }
        
    }
    return true;
}
experimental::TMXTiledMap* HelloWorld::findDirectionAndPlaceTheMap(experimental::TMXTiledMap* srcMap, experimental::TMXTiledMap* dstMap){
    bool found = false;
    int directionToFind = rand()%4;
    int directionCounter = 0;
    Rect nextRect;
    Rect srcRect;
    Size dstSize = dstMap->getContentSize();
    int randomData;
    while(true){   // find room for the selected map
        srcRect = srcMap->getBoundingBox();
        if (directionToFind == 0) { // check right
            randomData = (rand()%(int)dstMap->getMapSize().height);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().height - 2){
                randomData = dstMap->getMapSize().height - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect(srcRect.getMaxX(), srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 1) { // check bottom
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            
            nextRect = Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMinY() - dstSize.height, dstSize.width, dstSize.height);
        }else if (directionToFind == 2) { // check left
            randomData = (rand()%(int)dstMap->getMapSize().height);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().height - 2){
                randomData = dstMap->getMapSize().height - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect(srcRect.getMinX() - dstSize.width, srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 3) { // check top
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMaxY(), dstSize.width, dstSize.height);
        }
        
        if (isRoomEmpty(nextRect)) {
            found = true;
            this->addChild(dstMap);
            dstMap->setPosition(nextRect.origin);
            
            if (directionToFind == 0) { // connect right
                int x = srcMap->getMapSize().width - 1;
                float minY = getMax(srcRect.getMinY(), nextRect.getMinY());
                float maxY = getMin(srcRect.getMaxY(), nextRect.getMaxY());
                int midY = (minY + maxY)/2;
                
                int y = srcMap->getMapSize().height - (midY - srcRect.getMinY())/TILE_SIZE;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Point(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(69, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x--;
                        y++;
                    }
                    
                    counter++;
                }
                x = 0;
                y = dstMap->getMapSize().height - (midY - nextRect.getMinY())/TILE_SIZE;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(7, Point(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x++;
                        y++;
                    }
                    
                    counter++;
                }
            }else if (directionToFind == 1) { // connect bottom
                float minX = getMax(srcRect.getMinX(), nextRect.getMinX());
                float maxX = getMin(srcRect.getMaxX(), nextRect.getMaxX());
                int midX = (minX + maxX)/2;
                int x = (midX - srcRect.getMinX())/TILE_SIZE;
                int y = srcMap->getMapSize().height - 1;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Point(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(7, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y--;
                        x++;
                    }
                    
                    counter++;
                }
                x = (midX - nextRect.getMinX())/TILE_SIZE;
                y = 0;//dstMap->getMapSize().height - 1;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(69, Point(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    if (counter%2==0) {
                        x--;
                    }else{
                        y++;
                        x++;
                    }
                    
                    counter++;
                }
                // ladder for top
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) == 0 && dstMap->getMapSize().height > y &&
                        dstMap->getLayer("stage")->getTileGIDAt(Point(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                            counter = 0;
                        }
                    }else{
                        break;
                    }
                    
                    y++;
                    counter++;
                }
            }else if (directionToFind == 2) { // connect left
                int x = 0;
                float minY = getMax(srcRect.getMinY(), nextRect.getMinY());
                float maxY = getMin(srcRect.getMaxY(), nextRect.getMaxY());
                int midY = (minY + maxY)/2;
                
                int y = srcMap->getMapSize().height - (midY - srcRect.getMinY())/TILE_SIZE;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(7, Point(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x++;
                        y++;
                    }
                    
                    counter++;
                }
                x = dstMap->getMapSize().width - 1;
                y = dstMap->getMapSize().height - (midY - nextRect.getMinY())/TILE_SIZE;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Point(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Point(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Point(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(69, Point(x, y - 1));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter%2==0) {
                        y--;
                    }else{
                        x--;
                        y++;
                    }
                    
                    counter++;
                }
            }else if (directionToFind == 3) { // connect top
                float minX = getMax(srcRect.getMinX(), nextRect.getMinX());
                float maxX = getMin(srcRect.getMaxX(), nextRect.getMaxX());
                int midX = (minX + maxX)/2;
                int x = (midX - srcRect.getMinX())/TILE_SIZE;
                int y = 0;//srcMap->getMapSize().height - 1;
                int counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(69, Point(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Point(x - 1, y));
                        }
                    }else{
                        
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y++;
                        x++;
                    }
                    
                    counter++;
                }
                // ladder for top
                counter = 0;
                while(true){
                    if (srcMap->getLayer("stage")->getTileGIDAt(Point(x, y)) == 0 && y < srcMap->getMapSize().height &&
                        srcMap->getLayer("stage")->getTileGIDAt(Point(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            srcMap->getLayer("stage")->setTileGID(130, Point(x, y));
                            counter = 0;
                        }
                    }else{
                        break;
                    }
                    
                    y++;
                    counter++;
                }
                x = (midX - nextRect.getMinX())/TILE_SIZE;
                y = dstMap->getMapSize().height - 1;
                counter = 0;
                while(true){
                    if (dstMap->getLayer("stage")->getTileGIDAt(Point(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Point(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Point(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Point(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Point(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(7, Point(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Point(x, y));
                    }
                    
                    if (counter%2==0) {
                        x--;
                    }else{
                        y--;
                        x++;
                    }
                    
                    counter++;
                }
            }
        }
        
        if (found) {
            break;
        }
        directionToFind++;
        if (directionToFind >= 4) {
            directionToFind = 0;
        }
        
        directionCounter++;
        if (directionCounter >= 4) {
            break;
        }
    }
    if (!found) {
        return nullptr;
    }
    
    return dstMap;
}
float HelloWorld::getMin(float src, float dst){
    return src < dst?src:dst;
}
float HelloWorld::getMax(float src, float dst){
    return src > dst?src:dst;
}
void HelloWorld::setStage(experimental::TMXTiledMap* tileMap)
{
    if(GM->currentStageIndex != STAGE_LOBBY){
        addGold(0);
        addLumber(0);
        if(UDGetBool(KEY_PREMIUM_START, false)){
            addGold(1500);
            addLumber(1000);
        }
//        addGold(10000); // test
//        addLumber(10000); // test
    }
    
    GM->mapSize = tileMap->getMapSize();
    
    if (tileMap->getTag() == MAP_SHOP) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("found a shop").c_str(), GameManager::getInstance()->getHudLayer());
    }else if (tileMap->getTag() == MAP_WEAPON) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("suitcase room").c_str(), GameManager::getInstance()->getHudLayer());
    }
    float laserTrapDelay=0;
    float missileTrapDelay=0;
    tileMap->getChildByTag(TAG_STAGE_LAYER)->setOpacity(255);
//    tileMap->getChildByTag(TAG_BACK_LAYER)->setOpacity(255);
//    tileMap->getChildByTag(TAG_DECO_LAYER)->setOpacity(255);
    
    ValueVector rows = GameManager::getInstance()->split(strStage, "_");
    int mapIndex = 0;
    int x = -1;
    int y = -1;
    Point checkPoint = Point(14, 8);
    if(rows.size() > 2){
        mapIndex = rows.at(0).asInt();
        x = rows.at(1).asInt();
        y = rows.at(2).asInt();
    }
    
    // change coin tile into coin animation
    int totalWidth = tileMap->getMapSize().width;
    int totalHeight = tileMap->getMapSize().height;
    Point point;
    int talkCount = 0;
    
    experimental::TMXLayer* stageLayer = tileMap->getLayer("stage");
    experimental::TMXLayer* soilLayer = tileMap->getLayer("soil");
    experimental::TMXLayer* unitLayer = tileMap->getLayer("unit");
    decoLayer = tileMap->getLayer("deco");
    decoLayer->setVisible(false);// test
//    experimental::TMXLayer* foreLayer = tileMap->getLayer("fore");
    experimental::TMXLayer* backLayer = tileMap->getLayer("back");
    PointArray* pointArrayToRemoveInUnitLayer = PointArray::create(3000);
    
    for (int i = 0; i < tileMap->getMapSize().width; i++) {
        for (int j = 0; j < tileMap->getMapSize().height; j++) {
            if (!isWay(stageLayer->getTileGIDAt(Point(i, j)))) {
                stageTileRectArray.push_back(Rect(i*TILE_SIZE + tileMap->getPositionX(), (tileMap->getMapSize().height - j - 1)*TILE_SIZE + tileMap->getPositionY(), TILE_SIZE, TILE_SIZE));
            }
        }
    }
    
    Point cameraPos = Point::ZERO;
    for (int i = 0; i < totalWidth; i++) {
        for (int j = 0; j < totalHeight; j++) {
            point = Point(i,j);
            int gid = unitLayer->getTileGIDAt(point);
            Point pos = getPositionFromTileCoordinate(i, j);
            if (gid && !GM->isLoadingGame) {
//                if(!keyExist(map, "MovingPlatformLine") &&
//                   !keyExist(map, "MovingPlatformEnd")){
//                    unitLayer->removeTileAt(point);
//                }
                EnemyBase* enemy = NULL;
                FireableBase* fEnemy = NULL;
//                log("gid: %d", gid);
                
                if(gid == 61){
                    int blockerIndex = blockerArray.size();
                    bool isBlocked = UDGetBool(strmake("blocker_on_%d_%d_%d_%d", mapIndex, x, y, blockerIndex).c_str(), blockerIndex == 2);
                    Sprite* sptBlock = Sprite::createWithSpriteFrameName(isBlocked?"blocker0.png":"blocker5.png");
                    decoLayer->setTileGID(isBlocked?138:0, point);
                    spriteBatch->addChild(sptBlock, 1);
                    sptBlock->setPosition(pos);
                    sptBlock->setTag(blockerIndex);
                    sptBlock->setLocalZOrder(isBlocked?-sptBlock->getBoundingBox().origin.y-15:-1000);
                    blockerArray.pushBack(sptBlock);
                    sptBlock->setName(strmake("%d_%d", i, j));
                }
//                else if(gid == 62){
//                    int triggerIndex = triggerArray.size();
//                    bool isOn = UDGetBool(strmake("blocker_on_%d_%d_%d_%d", mapIndex, x, y, triggerIndex).c_str(), triggerIndex == 2);
//                    Sprite* sptTrigger = Sprite::createWithSpriteFrameName("trigger.png");
//                    sptTrigger->setFlippedX(isOn);
//                    spriteBatch->addChild(sptTrigger, 1);
//                    sptTrigger->setPosition(pos);
//                    sptTrigger->setTag(triggerIndex);
//                    triggerArray.pushBack(sptTrigger);
//                }
                else if(gid == 6){ // catapult
                    createUnit(UNIT_CATAPULT, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "catapult");
                }else if(gid == 71){ // catapult enemy
                    createUnit(UNIT_CATAPULT, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "catapult");
                }else if(gid == 7){ // helicopter
                    createUnit(UNIT_HELICOPTER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "helicopter");
                }else if(gid == 72){ // helicopter ENEMY
                    createUnit(UNIT_HELICOPTER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "helicopter");
                }else if(gid == 8){ // worker
                    createUnit(UNIT_WORKER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "worker");
                }else if(gid == 65){ // worker ready
                    createUnit(UNIT_WORKER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "worker");
                }else if(gid == 73){ // worker enemy
                    createUnit(UNIT_WORKER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "worker");
                }else if(gid == 91){ // orc worker
                    createUnit(UNIT_GOBLIN_WORKER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "orc worker");
//                    createUnit(UNIT_WIZARD, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "wizard"); // test
                }else if(gid == 9){ // swordman
                    createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "swordman")->monitoringDistance = 300000;
                }else if(gid == 66){ // swordman ready
                    createUnit(UNIT_SWORDMAN, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "swordman");
                }else if(gid == 74){ // swordman enemy
                    createUnit(UNIT_SWORDMAN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "swordman");
                }else if(gid == 10){ // archer
                    createUnit(UNIT_ARCHER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "archer");
                }else if(gid == 67){ // archer ready
                    createUnit(UNIT_ARCHER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "archer");
                    log("archer ready");
                }else if(gid == 75){ // archer enemy
                    createUnit(UNIT_ARCHER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "archer");
                }else if(gid == 5){ // hero
                    EnemyBase* hero = createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "swordman");
                    hero->setName("hero");
                    //this->setPosition(-pos + Point(size.width/2 + TILE_SIZE, size.height*2/4));
                    npcArray.pushBack(hero);
                }else if(gid == 62){ // support pos
//                    this->setPosition(-pos + Point(size.width/2 + TILE_SIZE, size.height*2/4));
                    supportPointArray->addControlPoint(pos);
                }else if(gid == 63){ // camera init pos
                    this->setPosition(-pos + Point(size.width/2 + TILE_SIZE, size.height*2/4));
                    log("camera init pos");
                }else if(gid == 4){ // orc chief
                    EnemyBase* unit = createUnit(UNIT_ORC_AXE, gid == 88?WHICH_SIDE_HERO:WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_AXE));
                    npcArray.pushBack(unit);
                    unit->setName("orc");
                    if(GM->nextScene == STAGE_INTRO){
                        unit->setFlippedX(true);
                    }
                }else if(gid == 14){ // undead
                    EnemyBase* unit = createUnit(UNIT_LAMINGTON, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "wizard");
                    unit->setName("undead");
                    npcArray.pushBack(unit);
                }else if(gid == 16 || gid == 84){ // troll
                    EnemyBase* unit = createUnit(UNIT_TROLL, gid == 84?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "troll");
                    if (gid == 84) {
                        unit->isAlli = true;
                    }
                }else if(gid == 116){ // orc worker ready
                    createUnit(UNIT_GOBLIN_WORKER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_WORKER));
                }else if(gid == 106){ // troll ready
                    createUnit(UNIT_TROLL, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_TROLL));
                }else if(gid == 124){ // troll hero
                    createUnit(UNIT_TROLL, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "troll");
                }else if(gid == 36){ // troll schedule
                    createUnit(UNIT_TROLL, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "troll")->scheduledAttackTime = 60*5;
                }else if(gid == 117){ // wizard ready
                    createUnit(UNIT_WIZARD, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_WIZARD));
                }else if(gid == 17 || gid == 85){ // goblin bomb
                    EnemyBase* unit = createUnit(UNIT_GOBLIN_BOMB, gid == 85?WHICH_SIDE_HERO:(WHICH_SIDE_ENEMY), ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_BOMB));
                    if (gid == 85) {
                        unit->isAlli = true;
                    }
                }else if(gid == 125){ // goblin bomb hero
                    createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_BOMB));
                }else if(gid == 37){ // goblin bomb schedule
                    createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_BOMB))->scheduledAttackTime = 60*2.5f;
                }else if(gid == 107){ // goblin bomb ready
                    createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN_BOMB));
                }else if(gid == 18 || gid == 86){ // goblin
                    EnemyBase* unit = createUnit(UNIT_GOBLIN, gid == 86?WHICH_SIDE_HERO:(WHICH_SIDE_ENEMY), ITS_NOT_BUILDING, pos, "goblin");
                    if (gid == 86) {
                        unit->isAlli = true;
                    }
                }else if(gid == 126){ // goblin hero
                    createUnit(UNIT_GOBLIN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "goblin");
                }else if(gid == 38){ // goblin schedule
                    createUnit(UNIT_GOBLIN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblin")->scheduledAttackTime = 60*2;
                }else if(gid == 108){ // goblin ready
                    createUnit(UNIT_GOBLIN, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_GOBLIN));
                }else if(gid == 19 || gid == 28 || gid == 27 || gid == 39 || gid == 88){ // orc axe
                    int vary = 40;
                    EnemyBase* unit = createUnit(UNIT_ORC_AXE, gid == 88?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos + Point(rand()%vary - vary/2, rand()%vary - vary), GM->getUnitName(UNIT_ORC_AXE));
                    
                    if(GM->nextScene == STAGE_INTRO){
                        unit->setFlippedX(true);
                    }
                    if (gid == 88) {
                        unit->isAlli = true;
                    }
                    if(gid == 28){
                        unit->scheduledAttackTime = 60;
                    }else if(gid == 27){
                        unit->scheduledAttackTime = 60*3;
                    }else if(gid == 39){
                        unit->scheduledAttackTime = 60*5;
                    }
                }else if(gid == 127){ // orc axe hero
                    int vary = 40;
                    createUnit(UNIT_ORC_AXE, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos + Point(rand()%vary - vary/2, rand()%vary - vary), GM->getUnitName(UNIT_ORC_AXE));
                }else if(gid == 109){ // orc axe ready
                    createUnit(UNIT_ORC_AXE, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_AXE));
                }else if(gid == 20|| gid == 87){ // orc spear
                    EnemyBase* unit = createUnit(UNIT_ORC_SPEAR, gid == 87?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_SPEAR));
                    if(gid == 87){
                        unit->isAlli = true;
                    }
                    if(GM->nextScene == STAGE_INTRO){
                        unit->setFlippedX(true);
                    }
                }else if(gid == 110){ // worker ready
                    createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_SPEAR));
                }else if(gid == 128){ // orc spear hero
                    createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_SPEAR));
                }else if(gid == 26){ // orc spear schedule
                    createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ORC_SPEAR))->scheduledAttackTime = 60*3;
                }else if(gid == 69 || gid == 89){ // bunker
                    EnemyBase* unit = createUnit(UNIT_ORC_BUNKER, gid == 89?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Point(50, 0), "bunker", 1, "bunker.png");
                    if (gid == 89) {
                        unit->isAlli = true;
                    }
                    unit->canFindTarget = true;
                    setOccupy(pos, 2, 2, true, unit);
                }else if(gid == 70 || gid == 80 || gid == 79 || gid == 78 || gid == 90 || gid == 46){ // hq
                    EnemyBase* unit = createUnit(UNIT_ORC_HQ, gid == 90?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Point(150, -50), "hq", 1, "hq.png");
                    if (gid == 90) {
                        unit->isAlli = true;
                    }
                    unit->canFindTarget = true;
                    setOccupy(pos, 4, 3, true, unit);
                    Sprite* spt = Sprite::createWithSpriteFrameName("redFlag0.png");
                    GM->runAnimation(spt, "redFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Point(232, 335));
                    
                    if(gid == 80 || gid == 79 || gid == 78 ){
                        unit->scheduledProductUnit = UNIT_ORC_AXE;
                        if (gid == 80) {
                            unit->scheduledProductUnitCount = 2;
                            unit->scheduledProductTime = 60*1;
                        }else if(gid == 79) {
                            unit->scheduledProductUnit = UNIT_ORC_SPEAR;
                            unit->scheduledProductUnitCount = 3;
                            unit->scheduledProductTime = 60*1.5f;
                        }else if(gid == 78) {
                            unit->scheduledProductUnitCount = 5;
                            unit->scheduledProductTime = 60*2;
                        }
                    }
                    if (gid == 46) {
                        unit->scheduledProductUnit = UNIT_ZOMBIE_ORC_AXE;
                        unit->scheduledProductUnitCount = 1;
                        unit->scheduledProductTime = 30;
                    }
                }else if(gid == 93){ // hq hero
                    EnemyBase* unit = createUnit(UNIT_CASTLE, WHICH_SIDE_HERO, ITS_BUILDING, pos + Point(150, -50), "hq", 1, "hq.png");
                    unit->startProductSchedule();
                    unit->canFindTarget = true;
                    setOccupy(pos, 4, 3, true, unit);
                    Sprite* spt = Sprite::createWithSpriteFrameName("redFlag0.png");
                    GM->runAnimation(spt, "redFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Point(232, 335));
                }else if(gid == 68){ // factory
                    EnemyBase* unit = createUnit(UNIT_FACTORY, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Point(100, -100), "factory", 1, "factory.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                }else if(gid == 54 || gid == 94){ // farm
                    int side = WHICH_SIDE_HERO;
                    if(gid == 94){
                        side = WHICH_SIDE_ENEMY;
                    }
                    EnemyBase* unit = createUnit(UNIT_FARM, side, ITS_BUILDING, pos+ Point(100, -0), "farm", 1, "farm.png");
                    addFoodMax(getFoodGive(UNIT_FARM));
                    setOccupy(pos, 3, 2, true, unit);
                    unit->isBuildingComplete = true;
                    unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
                    unit->smokePoint = unit->getPosition() + Point(-18, 110);
                }else if(gid == 94){ // farm ENEMY
                    EnemyBase* unit = createUnit(UNIT_FARM, WHICH_SIDE_ENEMY, ITS_BUILDING, pos+ Point(100, -0), "farm", 1, "farm.png");
                    unit->isBuildingComplete = true;
                    unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
                    unit->smokePoint = unit->getPosition() + Point(-18, 110);
                }else if(gid == 55 || gid == 95){ // lumber mill
                    int side = WHICH_SIDE_HERO;
                    if(gid == 95){
                        side = WHICH_SIDE_ENEMY;
                    }
                    EnemyBase* unit = createUnit(UNIT_LUMBERMILL, side, ITS_BUILDING, pos+ Point(100, -100), "lumberMill", 1, "lumberMill.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                }else if(gid == 56 || gid == 96){ // airport
                    int side = WHICH_SIDE_HERO;
                    if(gid == 96){
                        side = WHICH_SIDE_ENEMY;
                    }
                    EnemyBase* unit = createUnit(UNIT_AIRPORT, side, ITS_BUILDING, pos+ Point(100, -100), "airport", 1, "airport.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                    Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
                    GM->runAnimation(spt, "propeller", true);
                    unit->addChild(spt);
                    spt->setPosition(Point(134, 206));
                }else if(gid == 57){ // mine
                    EnemyBase* unit = createUnit(UNIT_MINE, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos + Point(100, -100), "mine", 1, "mine.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->setTag(0);
                }else if(gid == 58 || gid == 98){ // barracks
                    int side = WHICH_SIDE_HERO;
                    if(gid == 98){
                        side = WHICH_SIDE_ENEMY;
                    }
                    EnemyBase* unit = createUnit(UNIT_BARRACKS, side, ITS_BUILDING, pos+ Point(100, -100), "barracks", 1, "barracks.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                }else if(gid == 98){ // barracks ENEMY
                    EnemyBase* unit = createUnit(UNIT_BARRACKS, WHICH_SIDE_ENEMY, ITS_BUILDING, pos+ Point(100, -100), "barracks", 1, "barracks.png");
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                }else if(gid == 59 || gid == 40 || gid == 64 || gid == 99){ // castle
                    int side = WHICH_SIDE_HERO;
                    if(gid == 40 || gid == 99){
                        side = WHICH_SIDE_ENEMY;
                    }else if(gid == 64){
                        side = WHICH_SIDE_READY_HERO;
                    }else{
                        addFoodMax(getFoodGive(UNIT_CASTLE));
                    }
                    EnemyBase* unit = createUnit(gid == 40?UNIT_ZOMBIE_CASTLE:UNIT_CASTLE, side, ITS_BUILDING, pos + Point(150, -100), "castle", 1, "castle.png");
                    if(gid == 59){
//                        this->setPosition(-pos + Point(size.width/2 - TILE_SIZE*1, size.height*3/4));
                    }
                    
                    if(getPositionY() > 0){
                        setPositionY(0);
                    }
                    setOccupy(pos, 4, 3, true, unit);
                    
                    unit->startProductSchedule();
                    unit->isBuildingComplete = true;
                    
                    Sprite* spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
                    GM->runAnimation(spt, "blueFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Point(192, 255));
                    if(gid == 40){
                        unit->scheduledProductUnit = UNIT_ZOMBIE_SWORDSMAN;
                        unit->scheduledProductUnitCount = 1;
                        unit->scheduledProductTime = 25;
                    }
                }else if(gid == 60 || gid == 100){ // watcher tower
                    EnemyBase* unit = createUnit(UNIT_WATCHERTOWER, gid == 60?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Point(50, 0), "watcherTower", 1, "watcherTower.png");
                    unit->canFindTarget = true;
                    setOccupy(pos, 2, 2, true, unit);
                    unit->isBuildingComplete = true;
                }else if(gid == 30 || gid == 77){ // zombie swordsman
                    EnemyBase* unit = createUnit(UNIT_ZOMBIE_SWORDSMAN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ZOMBIE_SWORDSMAN));
                    unit->isZombie = gid == 30;
                    unit->oneSec = 1 + (rand()%10)*0.1f;
                    unit->extraSpeed = unit->extraSpeed - rand()%60;
                }else if(gid == 29 || gid == 76){ // zombie orc
                    EnemyBase* unit = createUnit(UNIT_ZOMBIE_ORC_AXE, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_ZOMBIE_ORC_AXE));
                    unit->isZombie = gid == 29;
                    unit->oneSec = 1 + (rand()%10)*0.1f;
                    unit->extraSpeed = unit->extraSpeed - rand()%60;
                }else if(gid == 129){ // hero orc general
                    createUnit(UNIT_HERO_ORC, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, GM->getUnitName(UNIT_HERO_ORC));
                }
                
                if((enemy != NULL && enemy->energy > 0) || (fEnemy != NULL && fEnemy->energy > 0)){
                    enemyMaxCount++;
                    EnemyBase* theEnemy;
                    if(fEnemy != NULL){
                        theEnemy = fEnemy;
                    }else if(enemy != NULL){
                        theEnemy = enemy;
                    }
                    
                    if(theEnemy != NULL){
                        if( mastery >= 7){
                            theEnemy->energy *= 4;
                        }else{
                            theEnemy->energy = theEnemy->energy*(1 + 0.3*mastery);
                        }
                        theEnemy->maxEnergy = theEnemy->energy;
                        addAppearEffect(theEnemy);
                    }
                }
            }
            if(gid){
                unitLayer->removeTileAt(point);
            }
            if(decoLayer){
                gid = decoLayer->getTileGIDAt(point);
                if (gid && !GM->isLoadingGame) {
                    Point pos = tileMap->getPosition() + Point(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j-1)*TILE_SIZE) + Point(0, TILE_SIZE/2);
                    if(gid == 50){ // tree
                        EnemyBase* unit = createUnit(UNIT_TREE, WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING, pos, "tree", 1, "emptyRect.png");
                        unit->canMove = false;
//                        for(int i = 0; i < 3; i++){
                            Sprite* spt = Sprite::createWithSpriteFrameName(strmake("tree2_%d.png", rand()%5));
                            spriteBatch->addChild(spt);
                            if(i==0){
//                                spt->setPosition(pos + Point(-20 + rand()%40, -20 + rand()%40 + 20));
                            }else{
//                                spt->setPosition(pos + Point(-50 + rand()%100, -50 + rand()%100 + 20));
                            }
                            spt->setPosition(pos + Point(-20 + rand()%40, -20 + rand()%40 + 20));
                        spt->setLocalZOrder(-spt->getBoundingBox().getMinY());//.origin.y);
                            unit->childrenSprite.pushBack(spt);
//                        }
                        
                        decoLayer->setTileGID(49, point);
                    }
                }
            }
            if (rand()%2 == 0) {
                int soilGid = soilLayer->getTileGIDAt(point);
                if(soilGid > 0){
                    soilLayer->setTileGID(soilGid + 70, point);
                }
            }
            int grass = rand()%12;
            switch (grass) {
                case 0:
                    stageLayer->setTileGID(1, point);
                    break;
                case 1:
                    if(rand()%3 == 0)
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
            
//            int num = rand()%211 - 100;
//            Sprite* spt = nullptr;
//            if (soilLayer && soilLayer->getTileGIDAt(point) > 0) {
//                if (soilLayer->getTileGIDAt(point) == 42) {
//                    if (num > 100) {
//                        spt = Sprite::createWithSpriteFrameName("stone3.png");
//                    }else if (num > 90) {
//                        spt = Sprite::createWithSpriteFrameName("stone2.png");
//                    }else if (num > 80) {
//                        spt = Sprite::createWithSpriteFrameName("stone1.png");
//                    }else if (num > 70) {
//                        spt = Sprite::createWithSpriteFrameName("stone0.png");
//                    }else if (num > 60) {
//                        spt = Sprite::createWithSpriteFrameName("grassOnLand1.png");
//                    }else if (num > 50) {
//                        spt = Sprite::createWithSpriteFrameName("grassOnLand0.png");
//                    }
//                }
//            }else{
//                if (num > 100) {
//                    spt = Sprite::createWithSpriteFrameName("flowerOnGrass.png");
//                }else if (num > 30) {
//                    spt = Sprite::createWithSpriteFrameName("grassOnGrass2.png");
//                }else if (num > -20) {
//                    spt = Sprite::createWithSpriteFrameName("grassOnGrass1.png");
//                }else if (num > -70) {
//                    spt = Sprite::createWithSpriteFrameName("grassOnGrass0.png");
//                }
//            }
//            if (spt != nullptr) {
//                spriteBatch->addChild(spt);
//                spt->setPosition(pos + Point(-50 + rand()%100, -50 + rand()%100));
//                spt->setLocalZOrder(-100000);
//            }
        }
    }
    
    if(GM->isLoadingGame){
        int slot = GM->loadingSlot;
        std::string strData = UDGetStr(strmake("savedData%d", slot).c_str());
        ValueVector datas = GameManager::getInstance()->split(strData.c_str(), ",");
//        int loadedStageIndx = datas.at(0).asInt();
        lastTick = datas.at(1).asInt();
        gameTimer = lastTick;
        addGold(datas.at(2).asInt());
        addLumber(datas.at(3).asInt());
        this->setPosition(Point(datas.at(4).asInt(), datas.at(5).asInt()));
        int unitType = -1;
        std::string sptName;
        for (int i = 6; i < datas.size(); i += 21) {
            unitType = datas.at(i).asInt();
            sptName = datas.at(i+1).asString();
            if(unitType == UNIT_TREE){
                sptName = "emptyRect.png";
            }
            EnemyBase* unit = createUnit(unitType, datas.at(i+5).asInt(), datas.at(i+8).asBool(), Point(datas.at(i+6).asInt(), datas.at(i+7).asInt()), datas.at(i+4).asString(), 1, sptName);
            unit->maxEnergy = datas.at(i+2).asInt();
            unit->energy = datas.at(i+3).asInt();
            if(unit->maxEnergy > unit->energy){
                unit->updateEnergy();
            }
            unit->scheduledAttackTime = datas.at(i+9).asInt();
            unit->isAlli = datas.at(i+10).asBool();
            unit->canFindTarget = datas.at(i+11).asBool();
            unit->scheduledProductUnit = datas.at(i+12).asInt();
            unit->scheduledProductUnitCount = datas.at(i+13).asInt();
            unit->scheduledProductTime = datas.at(i+14).asInt();
            unit->isBuildingComplete = true;
            int queueIndex = datas.at(i+15).asInt();
            if(queueIndex >= 0) unit->queueUnit(queueIndex);
            queueIndex = datas.at(i+16).asInt();
            if(queueIndex >= 0) unit->queueUnit(queueIndex);
            queueIndex = datas.at(i+17).asInt();
            if(queueIndex >= 0) unit->queueUnit(queueIndex);
            queueIndex = datas.at(i+18).asInt();
            if(queueIndex >= 0) unit->queueUnit(queueIndex);
            queueIndex = datas.at(i+19).asInt();
            if(queueIndex >= 0) unit->queueUnit(queueIndex);
            unit->isZombie = datas.at(i+20).asBool();
            
            if(unit->unitType == UNIT_ORC_BUNKER){ // bunker
                unit->canFindTarget = true;
                setOccupy(unit->getPosition() + Point(-50, 0), 2, 2, true, unit);
            }else if(unit->unitType == UNIT_ORC_BUNKER){ // hq
                setOccupy(unit->getPosition() - Point(150, -50), 4, 3, true, unit);
                Sprite* spt = Sprite::createWithSpriteFrameName("redFlag0.png");
                GM->runAnimation(spt, "redFlag", true);
                unit->addChild(spt);
                spt->setPosition(Point(232, 335));
            }else if(unit->unitType == UNIT_FACTORY){ // factory
                setOccupy(unit->getPosition() - Point(100, -100), 3, 3, true, unit);
                unit->startProductSchedule();
            }else if(unit->unitType == UNIT_FARM){ // farm
                if(unit->alliSide == WHICH_SIDE_HERO){
                    addFoodMax(getFoodGive(UNIT_FARM));
                }
                setOccupy(unit->getPosition() - Point(100, -0), 3, 2, true, unit);
                unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
                unit->smokePoint = unit->getPosition() + Point(-18, 110);
            }else if(unit->unitType == UNIT_LUMBERMILL){ // lumber mill
                setOccupy(unit->getPosition() - Point(100, -100), 3, 3, true, unit);
            }else if(unit->unitType == UNIT_AIRPORT){ // airport
                setOccupy(unit->getPosition() - Point(100, -100), 3, 3, true, unit);
                unit->startProductSchedule();
                Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
                GM->runAnimation(spt, "propeller", true);
                unit->addChild(spt);
                spt->setPosition(Point(134, 206));
            }else if(unit->unitType == UNIT_MINE){ // mine
                setOccupy(unit->getPosition() - Point(100, -100), 3, 3, true, unit);
                unit->setTag(0);
            }else if(unit->unitType == UNIT_BARRACKS){ // barracks
                setOccupy(unit->getPosition() - Point(100, -100), 3, 3, true, unit);
                unit->startProductSchedule();
            }else if(unit->unitType == UNIT_CASTLE){ // castle
                setOccupy(unit->getPosition() - Point(150, -100), 4, 3, true, unit);
                if (unit->alliSide == WHICH_SIDE_HERO) {
                    addFoodMax(getFoodGive(UNIT_CASTLE));
                }
                unit->startProductSchedule();
                Sprite* spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
                GM->runAnimation(spt, "blueFlag", true);
                unit->addChild(spt);
                spt->setPosition(Point(192, 255));
                if(stageIndex > 11){
                    spt->removeFromParent();
                    spt = Sprite::createWithSpriteFrameName("redFlag0.png");
                    GM->runAnimation(spt, "redFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Point(232, 335));
                }
            }else if(unit->unitType == UNIT_WATCHERTOWER){ // watcher tower
                setOccupy(unit->getPosition() - Point(50, 0), 2, 2, true, unit);
                unit->canFindTarget = true;
            }else if(unit->unitType == UNIT_ZOMBIE_SWORDSMAN || unit->unitType == UNIT_ZOMBIE_ORC_AXE){ // zombie swordsman
                unit->oneSec = 1 + (rand()%10)*0.1f;
                unit->extraSpeed = unit->extraSpeed - rand()%60;
            }
            if(unit->unitType == UNIT_TREE){
                unit->canMove = false;
                decoLayer->setTileGID(49, getCoordinateFromPosition(unit->getPosition()));
                Sprite* spt = Sprite::createWithSpriteFrameName(strmake("tree2_%d.png", rand()%5));
                spriteBatch->addChild(spt);
                spt->setPosition(unit->getPosition() + Point(-20 + rand()%40, -20 + rand()%40 + 20));
                spt->setLocalZOrder(-spt->getBoundingBox().getMinY());//.origin.y);
                unit->childrenSprite.pushBack(spt);
            }
        }
//        GM->isLoadingGame = false; // moved to bottom
    }
    
    CCLOG("setStage map done");
    TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
        CCLOG("setStage object map done");
    if (pointArrayToRemoveInUnitLayer) {
        for (int i = 0; i < pointArrayToRemoveInUnitLayer->count(); i++) {
            Point pos = pointArrayToRemoveInUnitLayer->getControlPointAtIndex(i);
            unitLayer->removeTileAt(pos);
        }
    }
    
    this->schedule(schedule_selector(HelloWorld::gravityUpdate));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForCoins));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForStraight));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForCustomMoving));
    this->schedule(schedule_selector(HelloWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    if (talkCount > 0) {
        this->schedule(schedule_selector(HelloWorld::talkUpdate), 0.1f);
    }

    this->schedule(schedule_selector(HelloWorld::bubbleUpdate));
    this->schedule(schedule_selector(HelloWorld::makeSomeBubbles), 0.1f);

    
    extraCritical = 0;
    extraPower = 0;
    extraFruit = 0;
    
    GameManager::getInstance()->isVideoRewardEnergy = false;
    GameManager::getInstance()->isVideoRewardAttack = false;
    GameManager::getInstance()->isVideoRewardCoins = false;
    
    
    tileMap->setTag(MAP_FOUND);
    float dur = 2;
    if (enemiesToLoad.size() > 0) {
        this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(HelloWorld::loadEnemies, this)), NULL));
    }
    

    resetPathState();
    isSetStageDone = true;
    
    // test
    draw->clear();
    
    for(int i = 0; i < 6; i++){
        HUD->setMenu(i, BTN_TYPE_NONE);
    }
//    addGold(12000); // test
//    addLumber(12000); // test
    
    makeZombiesAttack();
    
    float rowCounter = 1;
    int columnCounter = 0;
    int startX = -getPositionX() + 100;
    int startY = -getPositionY() -400;
    int birdGapX = 90;
    int birdGapY = 90;
    int columnOffset = 100;
    dur = 2;
    for (int i = 0; i < 20; i++) {
        Sprite* spt = Sprite::createWithSpriteFrameName("bird0.png");
        GM->runAnimation(spt, strmake("bird%d", rand()%3).c_str(), true);
        spriteBatchEffect->addChild(spt);
        spt->setPosition(Point(startX - rowCounter*birdGapX/2 + birdGapX*columnCounter + rand()%400 + 200, startY - rowCounter*birdGapY - columnCounter*columnOffset + rand()%400 + 200));
        
        columnCounter++;
        spt->setRotation(20 + rand()%30);
        spt->setScale(0.7f + (rand()%60)*0.01f);
        spt->runAction(Sequence::create(DelayTime::create(0.3), MoveBy::create(dur/spt->getScale(), Point(2000 + rand()%200, 2200+ rand()%400)), SPT_REMOVE_FUNC, NULL));
        spt->runAction(Sequence::create(DelayTime::create(0.3), ScaleTo::create(dur, 0.7f + (rand()%60)*0.01f), nullptr));
        int columnMaxCount = rowCounter;
        if (rowCounter > 4) {
            columnMaxCount = 4;
        }
        if (columnCounter >= rowCounter) {
            rowCounter++;
            columnCounter = 0;
        }
    }
    this->runAction(Sequence::create(DelayTime::create(1.5), CallFunc::create(CC_CALLBACK_0(HelloWorld::checkBirdFly, this)), nullptr));
    
//    totalKillUnit = 99;
//    totalProducedUnit = 85;
//    totalUsedGold = 22345;
//    totalEarnedGold = 27877;
//    totalUsedLumber = 9244;
//    totalEarnedLumber = 11999;
//    gameTimer = 60*5 + 53; // test
//    endGame(); // test
    
    UD->flush();
    if(GM->currentStageIndex == STAGE_SINGLEPLAY || GM->currentStageIndex == STAGE_RAID){
        for(auto unit:unitsToCreateArray){
            if(unit->alliSide == WHICH_SIDE_HERO){
                unit->alliSide = WHICH_SIDE_ENEMY;
                unit->isEnemy = true;
            }
        }
    }
    CCLOG("setStage done");
}
cocos2d::Size HelloWorld::getBuildingOccupySize(int unit){
    if(unit == UNIT_ORC_BUNKER){
        return Size(2, 2);
    }else if(unit == UNIT_ORC_HQ){
        return Size(4, 3);
    }else if(unit == UNIT_FACTORY){
        return Size(3, 3);
    }else if(unit == UNIT_FARM){
        return Size(3, 2);
    }else if(unit == UNIT_TREE_FOR_BATTLE){
        return Size(1, 1);
    }else if(unit == UNIT_LUMBERMILL){
        return Size(3, 3);
    }else if(unit == UNIT_AIRPORT){
        return Size(3, 3);
    }else if(unit == UNIT_CASTLE){
        return Size(4, 3);
    }else if(unit == UNIT_MINE){
        return Size(3, 3);
    }else if(unit == UNIT_BARRACKS){
        return Size(3, 3);
    }else if(unit == UNIT_WATCHERTOWER){
        return Size(2, 2);
    }else if(unit == UNIT_ORC_BARRACKS){
        return Size(2, 2);
    }else if(unit == UNIT_TEMPLE){
        return Size(3, 3);
    }else if(unit == UNIT_ORC_TROLL_HOUSE){
        return Size(3, 3);
    }else if(unit == UNIT_BARBECUE){
        return Size(3, 2);
    }
    return Size(1, 1);
}
void HelloWorld::checkBirdFly(){
    if ((GM->nextScene == STAGE_FIELD && !GM->isLoadingGame && HUD && !HUD->isRaid && stageIndex > 1) || GM->isColosseum) {
        log("stageIndex: %d", stageIndex);
        if(stageIndex == 24 || stageIndex == 25){
            return;
        }
        HUD->showSupportOffer();
    }
    
    GM->isLoadingGame = false;
}
void HelloWorld::makeZombiesAttack(){
//    EnemyBase* castle = nullptr;
//    for(auto unit:heroArray){
//        if(unit->unitType == UNIT_CASTLE){
//            castle = unit;
//            break;
//        }
//    }
//    if (castle != nullptr) {
    for(auto unit:enemyArray){
        if (unit->isZombie) {
            attackNearHero(unit);
        }
    }
//    }
}
void HelloWorld::attackNearHero(EnemyBase* enemy){
    int nearestDistance = 50000;
    EnemyBase* nearest = nullptr;
    for(auto hero:heroArray){
        int distance = abs(enemy->getPositionX() - hero->getPositionX()) + abs(enemy->getPositionY() - hero->getPositionY());
        if(nearestDistance > distance && canAttack(enemy, hero)){
            nearest = hero;
            nearestDistance = distance;
        }
    }
    if(nearest != nullptr){
        if(nearest->isBuilding){
            moveAndAttackTo(enemy, nearest->approachingPoints[rand()%nearest->approachPointCount]);
        }else{
            moveAndAttackTo(enemy, nearest->getPosition());
        }
    }
}
bool HelloWorld::isSpotAvailable(bool isEnemy, cocos2d::Point pos){
    if (isEnemy) {
        Point coordinate = getCoordinateFromPosition(pos);
        for(auto unit: enemyArray){
            if (getCoordinateFromPosition(unit->getPosition()) == coordinate) {
                return false;
            }
        }
    }
    return true;
}
void HelloWorld::setOccupy(cocos2d::Point pos, int width, int height, bool occupy, EnemyBase* building){
    setOccupy(pos, width, height, occupy);
    building->buildingStartCoordinate = getCoordinateFromPosition(pos, theMap);
    building->buildingOccupySize = Size(width, height);
    building->refreshApproachPoints();
}
void HelloWorld::setOccupy(cocos2d::Point pos, int width, int height, bool occupy){
    Point point = getCoordinateFromPosition(pos, theMap);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            setTileGID(decoLayer, occupy?48:0, Point(point.x + i, point.y + j));
        }
    }
}
void HelloWorld::setTileGID(experimental::TMXLayer* layer, int gid, Vec2 pos){
    if(pos.x >= 0 && pos.y >= 0 && pos.x < layer->getLayerSize().width && pos.y < layer->getLayerSize().height){
        layer->setTileGID(gid, pos);
    }else{
        log("invalied position");
    }
}
bool HelloWorld::isOccupied(cocos2d::Point coordinate){
    bool valid = isCoordinateValid(coordinate);
    if(!valid) return true;
    int gid = decoLayer->getTileGIDAt(coordinate);
    return gid == 48 || gid == 49;
}
void HelloWorld::addSprite(std::string sptName, cocos2d::Point pos){
    Sprite* spt = Sprite::createWithSpriteFrameName(sptName);
    spt->setPosition(pos);
    spriteBatch->addChild(spt);
    spt->setLocalZOrder(-pos.y);
}
void HelloWorld::startCameraFollowNPCForEvent(Ref* ref){
    cameraTargetForEvent = (Node*)ref;
    this->schedule(schedule_selector(HelloWorld::cameraUpdateForEvent));
}
void HelloWorld::endCameraFollowNPCForEvent(){
    this->unschedule(schedule_selector(HelloWorld::cameraUpdateForEvent));
}
void HelloWorld::cameraUpdateForEvent(float dt){
    
}
void HelloWorld::eventUpdate(float dt){
    if(talkingNPC != nullptr && talkingNPC->getName().compare("undead") == 0){
        if(imgTalkBox != nullptr && imgTalkBox->getScale() == 1){
            imgTalkBox->setPosition(talkingNPC->getPosition() + Point(0, 10));
        }else{
            sptPointer->setScale(imgTalkBox->getScale());
        }
        sptPointer->setPosition(imgTalkBox->getPosition() + Point(0, 2));
    }
}
void HelloWorld::eventDone(){
    this->unschedule(schedule_selector(HelloWorld::eventUpdate));
}

void HelloWorld::turnOnEnemyDamaging(Ref* obj){
    EnemyBase* enemy = (EnemyBase*)obj;
    enemy->isDamaging = true;
    runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
}
void HelloWorld::addDecoBlock(Point coordinate, Point position, std::string spriteName){
    Sprite* spt = Sprite::createWithSpriteFrameName(spriteName);
    spriteBatch->addChild(spt);
    spt->setPosition(position);
    spt->setLocalZOrder(-spt->getBoundingBox().origin.y + 2);
    spt->setName(strmake("deco_%d_%d", (int)coordinate.x, (int)coordinate.y));
    decoLayer->setTileGID(138, coordinate);
}
Point HelloWorld::getPositionFromTileCoordinate(int x, int y){
    return Point(x*TILE_SIZE + TILE_SIZE/2, (theMap->getMapSize().height-y-1)*TILE_SIZE) + Point(0, TILE_SIZE/2);
}
void HelloWorld::resetPathState(){
    int gid;
    experimental::TMXLayer* stageLayer = theMap->getLayer("stage");
    Size mapSize = theMap->getMapSize();
    bool isBlocked;
    for (int j = 0; j < mapSize.height; j++) {
        for (int i = 0; i < mapSize.width; i++) {
        
            gid = stageLayer->getTileGIDAt(Point(i, j));
            if (gid) {
                isBlocked = (!isWay(gid) || (decoLayer && isDecoBlock(decoLayer->getTileGIDAt(Point(i, j)))));
//                ?TILE_STATE_OCCUPIED:TILE_STATE_EMPTY
                GM->setPathState(i, j, isBlocked);
//                GM->astar.m.map[i][j] = isBlocked;
//                ASTAR::Cordinate::blockState[i][j] = isBlocked;
                
            }else{
//                GM->setPathState(i, j, PATH_EMPTY);
                GM->setPathState(i, j, false);
            }
        }
    }
}
void HelloWorld::createTrap(EnemyBase* enemy, cocos2d::Point pos){
    enemy->setPosition(pos);
    enemy->ap = getHeroMaxHP(0)/4;
    spriteBatch->addChild(enemy);
    enemy->setLocalZOrder(-1000);
    enemyArray.pushBack(enemy);
}
void HelloWorld::addTalkBalloon(EnemyBase* npc, std::string imgName){
    Sprite* spt = Sprite::createWithSpriteFrameName(imgName);
    npc->addChild(spt, 10);
    spt->setPosition(Point(npc->getBoundingBox().size.width/2, npc->getContentSize().height + spt->getContentSize().height/2));
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Point(0, 2)), MoveBy::create(0.2f, Point(0, -2)), NULL)));
    spt->setName("questState");
}
void HelloWorld::addTalkBalloon(experimental::TMXTiledMap* map, std::string imgName, cocos2d::Point pos){
    Sprite* spt = Sprite::createWithSpriteFrameName(imgName);
    map->addChild(spt, 10);
//    spt->setPosition(Point(npc->getContentSize().width/2, npc->getContentSize().height + spt->getContentSize().height/2));
    spt->setPosition(pos);
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Point(0, 2)), MoveBy::create(0.2f, Point(0, -2)), NULL)));
    spt->setName("questState");
}
EnemyBase* HelloWorld::createUnit(int index, int whichSide, bool isBuilding, Point pos, std::string name, int scaleX, std::string charName){
    
    int eng = getUnitMaxHP(index);
    int spd = getUnitSP(index);
    EnemyBase* unit = EnemyBase::createEnemy(index, eng, spd, charName.c_str());
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
//    unit->runMoveAnimation(DIRECTION_STAY);
    unit->alliSide = whichSide;
    unit->desiredPosition = pos;
    unit->setLocalZOrder(-unit->getBoundingBox().getMinY());//-pos.y);
    
    unit->isEnemy = whichSide == WHICH_SIDE_ENEMY;
    
    if(whichSide == WHICH_SIDE_ENEMY){
        if (index >= 0 && index <= 12) {
            unit->setColor(Color3B(255, 200, 200));
        }else if(stageIndex >= 12 && index >= 23 && index <= 27){
            unit->setColor(Color3B(255, 200, 200));
        }
    }
    unitsToCreateArray.pushBack(unit);
    unit->isBuilding = isBuilding;
    
    if(isBuilding){
        unit->setAnchorPoint(Vec2(0.5, 0.5));
        unit->canMove = false;
        unit->isBuildingComplete = true;
        spriteBatchBuilding->addChild(unit);
    }else{
        spriteBatch->addChild(unit);
    }
    unit->setPosition(pos);
    unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
    unit->refreshApproachPoints();
    unit->stop();
    
    
//    MovableArray.pushBack(unit); // test
    
    if (unit->unitType == UNIT_HELICOPTER) {
        unit->isFlying = true;
        unit->setLocalZOrder(10);
    }
    return unit;
}
void HelloWorld::addUnit(Movable* unit, bool addToWorld){
    if(addToWorld){
        unitsToCreateArray.pushBack((EnemyBase*)unit);
        spriteBatch->addChild(unit);
        MovableArray.pushBack(unit);
    }
    unit->isBuilding = false;
    unit->stop();
    
    if (unit->unitType == UNIT_HELICOPTER) {
        unit->isFlying = true;
        unit->setLocalZOrder(10);
    }
}
int HelloWorld::getUnitAP(int unit){
//    return 0; // test
    if(unit == UNIT_WORKER){
        return 3;
    }else if(unit == UNIT_GOBLIN_WORKER){
        return 3;
    }else if(unit == UNIT_ARCHER){
        return 13;
    }else if(unit == UNIT_HELICOPTER){
        return 10;
    }else if(unit == UNIT_SWORDMAN){
        return 10;
    }else if(unit == UNIT_WATCHERTOWER){
        return 15;
    }else if(unit == UNIT_CATAPULT){
        return 40;
    }else if(unit == UNIT_GOBLIN){
        return 8;
    }else if(unit == UNIT_GOBLIN_BOMB){
        return 500;
    }else if(unit == UNIT_TROLL){
        return 25;
    }else if(unit == UNIT_ORC_AXE){
        return 13;
    }else if(unit == UNIT_ORC_SPEAR){
        return 18;
    }else if(unit == UNIT_ORC_BUNKER){
        return 18;
    }else if(unit == UNIT_ORC_HQ){
        return 18;
    }else if(unit == UNIT_ZOMBIE_SWORDSMAN){
        return 10;
    }else if(unit == UNIT_ZOMBIE_ORC_AXE){
        return 12;
    }else if(unit == UNIT_WIZARD){
        return 40;
    }else if(unit == UNIT_HERO_ORC){
        return 25;
    }
    return 5;
}
int HelloWorld::getUnitDP(int unit){
    if(unit == UNIT_WORKER){
        return 0;
    }else if(unit == UNIT_CATAPULT){
        return 0;
    }else if(unit == UNIT_TROLL){
        return 3;
    }else if(unit == UNIT_ORC_AXE){
        return 2;
    }else if(unit == UNIT_ORC_SPEAR){
        return 1;
    }else if(unit == UNIT_HERO_ORC){
        return 3;
    }
    return 0;
}
int HelloWorld::getUnitSP(int unit){
    if(unit == UNIT_WORKER){
        return 600;
    }else if(unit == UNIT_GOBLIN_WORKER){
        return 600;
    }else if(unit == UNIT_SWORDMAN){
        return 650;
    }else if(unit == UNIT_ARCHER){
        return 600;
    }else if(unit == UNIT_CATAPULT){
        return 300;
    }else if(unit == UNIT_HELICOPTER){
        return 650;
    }else if(unit == UNIT_WATCHERTOWER || unit == UNIT_ORC_BUNKER || unit == UNIT_ORC_HQ){
        return 0;
    }else if(unit == UNIT_TROLL){
        return 330;
    }else if(unit == UNIT_ORC_AXE){
        return 550;
    }else if(unit == UNIT_GOBLIN || unit == UNIT_GOBLIN_BOMB){
        return 900;
    }else if(unit == UNIT_ORC_SPEAR){
        return 550;
    }else if(unit == UNIT_ZOMBIE_ORC_AXE || unit == UNIT_ZOMBIE_SWORDSMAN){
        return 250;
    }else if(unit == UNIT_HERO_ORC){
        return 650;
    }
    return 450;
}
float HelloWorld::getUnitCreateTime(int index){
//    return 3; // test
    if (index == UNIT_CASTLE) {
        return 40;
    }else if (index == UNIT_FARM) {
        return 10;
    }else if (index == UNIT_BARRACKS) {
        return 20;
    }else if (index == UNIT_LUMBERMILL) {
        return 15;
    }else if (index == UNIT_WATCHERTOWER) {
        return 12;
    }else if (index == UNIT_AIRPORT) {
        return 25;
    }else if (index == UNIT_FACTORY) {
        return 30;
    }else if (index == UNIT_WORKER) {
        return 8;
    }else if (index == UNIT_GOBLIN_WORKER) {
        return 8;
    }else if (index == UNIT_SWORDMAN) {
        return 10;
    }else if (index == UNIT_ARCHER) {
        return 15;
    }else if (index == UNIT_HELICOPTER) {
        return 15;
    }else if (index == UNIT_CATAPULT) {
        return 30;
    }else if(index == UNIT_GOBLIN_BOMB){
        return 20;
    }else if(index == UNIT_GOBLIN){
        return 8;
    }else if(index == UNIT_TROLL){
        return 40;
    }else if(index == UNIT_ORC_AXE){
        return 15;
    }else if(index == UNIT_ORC_SPEAR){
        return 20;
    }else if (index == UNIT_WIZARD) {
        return 35;
    }else if (index == UNIT_ORC_HQ) {
        return 40;
    }else if (index == UNIT_BARBECUE) {
        return 10;
    }else if (index == UNIT_ORC_BARRACKS) {
        return 20;
    }else if (index == UNIT_ORC_BUNKER) {
        return 12;
    }else if (index == UNIT_TEMPLE) {
        return 35;
    }else if (index == UNIT_ORC_TROLL_HOUSE) {
        return 30;
    }
    return 10;
}
float HelloWorld::getUnitMonitoringDistance(int index){
    if(index == UNIT_ARCHER ||
       index == UNIT_HERO_ORC){
        return 600000;
    }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 950000;
    }else if(index == UNIT_HELICOPTER){
        return 1000000;
    }else if(index == UNIT_CATAPULT){
        return 1200000;
    }else if(index == UNIT_SWORDMAN || index == UNIT_WORKER|| index == UNIT_GOBLIN_WORKER || index == UNIT_ORC_AXE || index == UNIT_ORC_SPEAR || index == UNIT_GOBLIN || index == UNIT_TROLL|| index == UNIT_GOBLIN_BOMB){
        return 300000;
    }
    return 600000;
}
float HelloWorld::getUnitAttackRange(int index){
    if(index == UNIT_ARCHER){
        return 200000;
    }else if(index == UNIT_HELICOPTER){
        return 100000;
    }else if(index == UNIT_ORC_SPEAR){
        return 220000;
    }else if(index == UNIT_CATAPULT){
        return 500000;
    }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 400000;
    }else if(index == UNIT_WIZARD){
        return 150000;
    }
    return 200000;
}
int HelloWorld::getUnitAttackType(int index){
    if (index == UNIT_ARCHER ||index == UNIT_ORC_SPEAR || index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ || index == UNIT_HELICOPTER || index == UNIT_WIZARD) {
        return ATTACK_TYPE_RANGE;
    }else if (index == UNIT_SWORDMAN ||index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER || index == UNIT_GOBLIN_BOMB || index == UNIT_GOBLIN || index == UNIT_ORC_AXE || index == UNIT_TROLL || index == UNIT_ZOMBIE_ORC_AXE ||
              index == UNIT_ZOMBIE_SWORDSMAN ||
              index == UNIT_HERO_ORC) {
        return ATTACK_TYPE_NEAR;
    }
    return ATTACK_TYPE_NONE;
}
int HelloWorld::getUnitAttackTargetType(int index){
    if (index == UNIT_CATAPULT || index == UNIT_WIZARD) {
        return ATTACK_TARGET_TYPE_SPLASH;
    }
    return ATTACK_TARGET_TYPE_SINGLE;
}
float HelloWorld::getUnitAttackCoolTime(int index){
    if(index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER){
        return 1.0f;
    }else if(index == UNIT_ARCHER){
        return 1.5f;
    }else if(index == UNIT_HELICOPTER){
        return 1.1f;
    }else if(index == UNIT_SWORDMAN){
        return 1.2f;
    }else if(index == UNIT_CATAPULT){
        return 4.0f;
    }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 1.1f;
    }else if(index == UNIT_ORC_SPEAR){
        return 1.5f;
    }else if(index == UNIT_ORC_AXE){
        return 1.2f;
    }else if(index == UNIT_TROLL){
        return 1.0f;
    }else if(index == UNIT_GOBLIN){
        return 0.7f;
    }else if(index == UNIT_GOBLIN_BOMB){
        return 0.1f;
    }else if(index == UNIT_WIZARD){
        return 2.0f;
    }else if(index == UNIT_HERO_ORC){
        return 2.0f;
    }
    
    return 2.0f;
}
float HelloWorld::getUnitAttackHappenTime(int index){
    if(index == UNIT_ARCHER){
        return 0.3f;
    }else if(index == UNIT_SWORDMAN || index == UNIT_HELICOPTER || index == UNIT_ORC_SPEAR || index == UNIT_ORC_AXE || index == UNIT_GOBLIN || index == UNIT_TROLL){
        return 0.3f;
    }else if(index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 0.0f;
    }else if(index == UNIT_WORKER || index == UNIT_GOBLIN_WORKER){
        return 0.5f;
    }else if(index == UNIT_WIZARD){
        return 0.4f;
    }else if(index == UNIT_HERO_ORC){
        return 1.3f;
    }
    return 0.2f;
}
int HelloWorld::getUnitMaxHP(int unit){
    if(unit == UNIT_WORKER || unit == UNIT_GOBLIN_WORKER){
        return 60;
    }else if(unit == UNIT_ARCHER){
        return 90;
    }else if(unit == UNIT_SWORDMAN){
        return 150;
    }else if(unit == UNIT_CATAPULT){
        return 400;
    }else if(unit == UNIT_HELICOPTER){
        return 70;
    }else if(unit == UNIT_CASTLE || unit == UNIT_ZOMBIE_CASTLE){
        return 4000;
    }else if(unit == UNIT_FARM){
        return 700;
    }else if(unit == UNIT_BARRACKS){
        return 1400;
    }else if(unit == UNIT_LUMBERMILL){
        return 1000;
    }else if(unit == UNIT_FACTORY){
        return 1500;
    }else if(unit == UNIT_AIRPORT){
        return 2000;
    }else if(unit == UNIT_WATCHERTOWER){
        return 500;
    }else if(unit == UNIT_ORC_BUNKER){
        return 800;
    }else if(unit == UNIT_ORC_HQ || unit == UNIT_ZOMBIE_HQ){
        return 2000;
    }else if(unit == UNIT_TREE){
        return 20; // test
    }else if(unit == UNIT_MINE){
        return 50000; // test
    }else if(unit == UNIT_TROLL){
        return 580;
    }else if(unit == UNIT_GOBLIN || unit == UNIT_GOBLIN_BOMB){
        return 80;
    }else if(unit == UNIT_ORC_AXE){
        return 180;
    }else if(unit == UNIT_ORC_SPEAR){
        return 120;
    }else if(unit == UNIT_ZOMBIE_ORC_AXE){
        return 190;
    }else if(unit == UNIT_ZOMBIE_SWORDSMAN){
        return 160;
    }else if(unit == UNIT_WIZARD){
        return 40;
    }else if(unit == UNIT_HERO_ORC){
        return 550;
    }
    return 60;
}
void HelloWorld::stopWoongSound(){
    readyToMove = true;
    
}
void HelloWorld::addDirtToTile(experimental::TMXTiledMap* map, Point pos){
    Sprite* sptDirt;
    for (int i = 0; i < 4; i++) {
        sptDirt = Sprite::createWithSpriteFrameName(rand()%2==0?"dirt1.png":"dirt0.png");
        spriteBatch->addChild(sptDirt, 100);
        float dur = 1;
        sptDirt->runAction(MoveBy::create(0.1, Point(0, -2)));
        sptDirt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Point(i%2==0?-4:4, 4)), 6), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptDirt)), NULL));
        sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
        sptDirt->setPosition(Point(pos.x*TILE_SIZE + (i%2==0?0:TILE_SIZE), (map->getMapSize().height - pos.y - 1)*TILE_SIZE + (i>1?TILE_SIZE:0)) + map->getPosition());
        //log("dirt %f, %f", sptDirt->getPosition().x, sptDirt->getPosition().y);
    }
}


void HelloWorld::cloudSchedule(float dt){
    for (auto cloud : cloudArray) {
        if(cloud->getPosition().x < 0){
            cloud->setPosition(Point(cloud->getParent()->getContentSize().width + cloud->getContentSize().width/2, cloud->getPosition().y));
        }
    }
}
void HelloWorld::saveCoinIfStarCollected(){
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
void HelloWorld::sacrificeLater(float dt){
    
}
void HelloWorld::sacrificeDone(Ref* obj){
    
}

void HelloWorld::addRide(int unit, Point pos){
    const char* sptName = "rideTop.png";
    
    Movable* board = Movable::createMovable(unit, 1, 0, sptName);
    board->setPosition(pos);
    board->setTag(unit);
    
    /*Sprite* sprite = Sprite::createWithSpriteFrameName("silverCoin0.png");
    sprite->setPosition(Point(16, 20));
    sprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(MoveBy::create(0.3f, Point(0, 15)), MoveBy::create(0.3f, Point(0, -15)), NULL)));
    board->addChild(sprite);*/
    
    spriteBatch->addChild(board);
    helperArray.pushBack(board);
    float time = 0.6f;
    MoveTo* moveUpTop = MoveTo::create(time, pos + Point(0, 10));
    MoveTo* moveDownTop = MoveTo::create(time, pos);
    MoveTo* moveDownBottom = MoveTo::create(time, pos + Point(0, - 10));
    MoveTo* moveUpBottom = MoveTo::create(time, pos);
    float rate = 1;
    
    board->runAction(RepeatForever::create((ActionInterval*)Sequence::create(EaseIn::create(moveUpTop, rate), EaseOut::create(moveDownTop, rate), EaseIn::create(moveDownBottom, rate), EaseOut::create(moveUpBottom, rate), NULL)));
}
void HelloWorld::splashDamage(Point pos, int radius, int damage, bool isFromEnemy, Movable* attacker){
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
    for(auto unit: isFromEnemy? heroArray:enemyArray){
        if (unit->getPosition().distanceSquared(pos) < radius) {
            if(unit->getHitAndIsDead(damage, attacker)){
                removeDeadUnit(unit);
            }
            counter++;
        }
        if(counter > 3){
            break;
        }
    }
    if(attacker && attacker->unitType == UNIT_WIZARD){
        for(auto tree : mutualArray){
            if(tree->unitType == UNIT_TREE_FOR_BATTLE){
                if (tree->getPosition().distanceSquared(pos) < radius) {
                    if(tree->getHitAndIsDead(damage, attacker)){
                        removeDeadUnit(tree);
                        
                    }
                }
            }
        }
    }
    
//    if (target && target->energy > 0) {
//        if(target->getHitAndIsDead(ap)){
//            stop();
//        }
//    }
}
void HelloWorld::updateIndicators(){
    Movable* unit;
    float eX = 0;
    float eY = 0;
    float centerX = -this->getPosition().x + size.width/2;
    float centerY = -this->getPosition().y + size.height/2;
    float a;float y;float x;Rect rect;
    int outOfBoxCount = 0;
    Sprite* indicator;
    Sprite* spt;
    float scale = this->getScale();
    float offset = 15;
    for (int i = 0; i < indicatedArray.size(); i++) {
        spt = (Sprite*)indicatedArray.at(i);
       
        
        rect = Rect(-this->getPosition().x/scale, -this->getPosition().y/scale, size.width/scale, size.height/scale);
        if(rect.containsPoint(unit->getPosition())){
            continue;
        }else{
            
        }
        eX = unit->getPosition().x*scale;
        eY = unit->getPosition().y*scale;
        a = (eY - centerY)/(eX - centerX);
        y = centerY > eY? (-size.height/2 + offset):(size.height/2 - offset);
        x = y/a;
        if (x > size.width/2) {// right
            x = size.width/2 - offset;
            y = x*a;
        }else if(x < -size.width/2){//left
            x = -size.width/2 + offset;
            y = x*a;
        }else{
            
        }
        
        if (indicatorArray.size() > outOfBoxCount) {
//            indicator = indicatorArray.at(outOfBoxCount);
            for (auto indi: indicatorArray) {
                if(indi->getTag() == unit->getTag()){
                    indicator = indi;
                    break;
                }
            }
        }else{
            indicator = Sprite::create("blueArrow.png");\
            indicator->setTag(spt->getTag());
           
            
            spt->setTag(79);
            indicator->addChild(spt);

            spt->setPosition(Point(-50, indicator->getContentSize().height*indicator->getScale()/2));
            indicator->setScale(0.5);
            this->getParent()->addChild(indicator);
            GameManager::getInstance()->makeItSiluk(indicator);
            indicatorArray.pushBack(indicator);
        }

        indicator->setRotation(-atan2(y,x)*180.0f/3.14f);
        indicator->setPosition(Point(size.width/2 + x, size.height/2 + y));

        spt = (Sprite*)indicator->getChildByTag(79);
        spt->setRotation(-indicator->getRotation());
        
        outOfBoxCount++;
    }
    
    if (indicatorArray.size() > outOfBoxCount) {
//        for (int i = (int)indicatorArray.size() - 1; i >= outOfBoxCount; i--) {
//            indicator = (Sprite*)indicatorArray.at(i);
//            indicatorArray.eraseObject(indicator);
//            indicator->removeFromParentAndCleanup(true);
//        }
        for(auto indi: indicatorArray){
            indi->removeFromParentAndCleanup(true);
        }
        indicatorArray.clear();
        updateIndicators();
    }
}

void HelloWorld::removeUsedAssets(){
    CCLOG("HelloWorld removeUsedAssets");
    unscheduleAll();
    
    for (auto lbl: talkLabelArray){
        lbl->release();
    }
    _eventDispatcher->removeEventListener(listener);
    _eventDispatcher->removeEventListener(mouseListener);
    GM->setCurrentStageLayer(nullptr);
    
    for(auto drop: enemyArray){
        drop->stopAllActions();
    }
    for(auto drop: heroArray){
        drop->stopAllActions();
    }
    for(auto drop: mutualArray){
        drop->stopAllActions();
    }
    for(auto drop: spriteBatch->getChildren()){
        drop->stopAllActions();
    }
    for(auto drop: spriteBatchBuilding->getChildren()){
        drop->stopAllActions();
    }
    
    
//    this->removeAllChildrenWithCleanup(true);
//    this->unscheduleAllCallbacks();
//    this->stopAllActions();
//    GameManager::getInstance()->setHudLayer(NULL);
//    GameManager::getInstance()->setCurrentStageLayer(NULL);
}
void HelloWorld::testSchedule(float dt){
    
}

void HelloWorld::doTheBombToMissiles(float dt){
    CCLOG("doTheBombToMissiles");
    if (bombTargetMissileArray.size() <= 0) {
        this->unschedule(schedule_selector(HelloWorld::doTheBombToMissiles));
        bombTargetEnemyIndex = 0;
        this->schedule(schedule_selector(HelloWorld::doTheBombToEnemies), 0.1, bombTargetEnemyArray.size(), 0.1);
        return;
    }
    
    Movable* drop = bombTargetMissileArray.at(0);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
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
    spriteBatch->removeChild(drop, true);
    spriteBatchBuilding->removeChild(drop, true);
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%36 - 18, rand()%36 - 18)),
                                     MoveTo::create(0.05, screenPos),NULL));
}

void HelloWorld::doTheBombToEnemies(float dt){
    float demage = 1;
    
    if (bombTargetEnemyArray.size() <= bombTargetEnemyIndex ) {
        this->unschedule(schedule_selector(HelloWorld::doTheBombToEnemies));
        isPaused = false;
        
        for(auto spt: enemyArray)
        {
            spt->resume();
        }
        
        for(auto spt: heroMissileArray)
        {
            spt->resume();
        }
        
        for(auto spt: enemyMissileArray)
        {
            spt->resume();
        }
        
        for(auto enemy: bombTargetEnemyArray){
            if(enemy->untouchable){
                
            }else{
                enemy->energy -= (int)demage;
                
                if (enemy == theBoss) {
//                    GameManager::getInstance()->getHudLayer()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                }else{
                    enemy->desiredPosition = enemy->desiredPosition + Point(enemy->isTowardLeft?2:-2, 2);
                    enemy->setPosition(enemy->getPosition() + Point(enemy->isTowardLeft?2:-2, 2));
                }
                
                showDamage(demage, enemy->getPosition());
                
                if (enemy->energy <= 0) {
                    this->runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
                    
                    enemyArray.eraseObject(enemy);
                    MovableArray.eraseObject(enemy);
                    spriteBatch->removeChild(enemy, true);
                    spriteBatchBuilding->removeChild(enemy, true);
                }else{
                    enemy->blinkForAWhile();
                }
            }
        }
        
        return;
    }
    
    EnemyBase* enemy = bombTargetEnemyArray.at(bombTargetEnemyIndex);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    sprite->setPosition(enemy->getPosition());
    
    Point screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Point(rand()%18 - 9, rand()%18 - 9)),
                                     MoveTo::create(0.05, screenPos),NULL));
    
    bombTargetEnemyIndex++;
}

void HelloWorld::enemyFireLoop(float dt)
{
    for(auto enemy: fireableArray)
    {
        while(enemy->missileArray.size() > 0){
            Movable* missile = enemy->missileArray.at(0);
            enemy->missileArray.erase(0);
            Node* light = getLight();
            light->setPosition(missile->getPosition());
            this->addChild(light);
            light->runAction(Sequence::create(DelayTime::create(0.1), CallFuncN::create(CC_CALLBACK_1(Node::removeFromParentAndCleanup, light)), NULL));
            light->setColor(Color3B(250, 100, 100));
            if (missile->getScaleX() == missile->getScaleY()) {
                missile->runAction(ScaleTo::create(0.2f, 1));
            }
            
            if(missile->getTag() == UNIT_MISSILE_Movable){
                MovableMissileArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
               
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_STRAIGHT){
                straightMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
                //missile->secondTag = enemy->getTag();
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_CUSTOM){
                customMovingArray.pushBack(missile);
                enemyMissileArray.pushBack(missile);
                //missile->secondTag = enemy->getTag();
                missile = NULL;
            }else if(missile->getTag() == UNIT_MISSILE_CHASING){
                chasingMissileArray.pushBack(missile);
                
                missile = NULL;
            }
        }
    }
}
void HelloWorld::removeMeFromChasing(Ref* ref){
    Movable* drop = (Movable*)ref;
    drop->setTag(UNIT_MISSILE_STRAIGHT);
    chasingMissileArray.eraseObject(drop);
    straightMovingArray.pushBack(drop);
}

void HelloWorld::bubbleUpdate(float dt)
{
    char buf[80];
    sprintf( buf, "theme_%d_Background_tower_%d_shadow.png", GameManager::getInstance()->theme, rand()%2);
    
    for(auto bubble: bubbleArray)
    {
        bubble->setPosition(Point(bubble->getPosition().x, bubble->getPosition().y + 50*dt));
        
        experimental::TMXTiledMap* map = getTileMap(bubble->getPosition());
        //        Point plPos = this->getCoordinateFromPosition(bubble->getPosition() + Point(0, - bubble->getContentSize().height), map);
        int tgid = getTileAtPosition(bubble->getPosition(), TAG_FORE_LAYER, map);
        if (tgid == 0) {
            bubbleArray.eraseObject(bubble);
            bubble->stopAllActions();
            sprintf( buf, "bubble%dPop", bubble->getTag());
            Animation* animation = AnimationCache::getInstance()->getAnimation(buf);
            Animate* animate = Animate::create(animation);
            CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(HelloWorld::enemyBaseMoveDone, this));
            bubble->runAction(Sequence::create(animate, call, NULL));
            return;
        }
    }
}

void HelloWorld::makeSomeBubbles(float dt)
{
    int index = 0;
    char buf[50];
    for(auto sprite: inWaterArray)
    {
        if (rand()%7 > 0) {
            continue;
        }
        index = rand()%2;
        
        sprintf( buf, "bubble%d.png", index);
        
    }
}


void HelloWorld::talkUpdate(float dt)
{
    
}
Movable* HelloWorld::findTargetHero(Movable* finder){
    float minDistance = 1000000;
    EnemyBase* nearest = nullptr;
    for(auto drop: heroArray){
        if (drop->untouchable || drop->energy <= 0) {
            continue;
        }
        if (!canAttack(finder, drop)) {
            continue;
        }
        float distance = finder->getPosition().distanceSquared(drop->getPosition());
        if (distance < finder->monitoringDistance) {
            if (minDistance > distance) {
                minDistance = distance;
                nearest = drop;
            }
        }
    }
    
    return nearest;
}
Movable* HelloWorld::findTargetEnemy(Movable* finder){
    float minDistance = 1000000;
    EnemyBase* nearest = nullptr;
    for(auto drop: enemyArray){
        if (drop->untouchable || drop->energy <= 0) {
            continue;
        }
        if (!canAttack(finder, drop)) {
            continue;
        }
        float distance = finder->getPosition().distanceSquared(drop->getPosition());
        if (distance < finder->monitoringDistance) {
            if (minDistance > distance) {
                minDistance = distance;
                nearest = drop;
            }
        }
    }
    if (nearest == nullptr) {
        EnemyBase* nearestTree = nullptr;
        if(finder->unitType == UNIT_WIZARD){
            for(auto tree: mutualArray){
                if(tree->unitType == UNIT_TREE_FOR_BATTLE){
                    float distance = finder->getPosition().distanceSquared(tree->getPosition());
                    if (distance < finder->monitoringDistance) {
                        if (minDistance > distance) {
                            minDistance = distance;
                            nearestTree = tree;
                        }
                    }
                }
            }
            return nearestTree;
        }else{
            return nullptr;
        }
    }else{
        return nearest;
    }
}
void HelloWorld::autoTargeting(){
//    if (!isAutoTargetingOn) {
//        return;
//    }
//    Movable* enemy = findTarget(200);
//    float angle;
//
//    int radius = 170;
//    float dx = cosf(angle) * radius;
//    float dy = sinf(angle) * radius;
    
//    Point velocity = Point(dx/joystickRadius, dy/joystickRadius);
//    float degrees = angle * SJ_RAD2DEG;
//    
//    // Update the thumb's position
//    Point stickPosition = Point(dx, dy);
//    
//    Vec2 location = Vec2(size.width/2 + dx*6, size.height/2 + dy*6);
//    
//        
//
//    log("targeting: %f, dx: %f, dy: %f", angle*180/3.14f, dx, dy);
}
void HelloWorld::entranceSchedule(float dt){
   
}
void HelloWorld::releaseCustomMovingCamera(){
    isCameraInCustomMoving = false;
}
// gravityupdate
void HelloWorld::gravityUpdate(float dt)
{
//    if (stageIndex == STAGE_ENTRANCE) {
//        entranceSchedule(dt);
//    }
    if(finishedVideo >= 0){
        try
        {
            if(finishedVideo == VIDEO_SUPPORT_0){
                int unitType = UNIT_WORKER;
                if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                    unitType = UNIT_GOBLIN_WORKER;
                }
                for(int i = 0; i < 4; i++){
                    createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, supportPointArray->getControlPointAtIndex(i), GM->getUnitName(unitType));
                }
            }else if(finishedVideo == VIDEO_SUPPORT_1){
                for(int i = 0; i < 3; i++){
                    int unitType = UNIT_SWORDMAN;
                    if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                        unitType = UNIT_GOBLIN;
                    }
                    createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, supportPointArray->getControlPointAtIndex(i), GM->getUnitName(unitType))->monitoringDistance = 300000;
                    unitType = UNIT_ARCHER;
                    if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                        unitType = UNIT_GOBLIN_BOMB;
                    }
                    createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, supportPointArray->getControlPointAtIndex(i + 3), GM->getUnitName(unitType));
                }
            }else if(finishedVideo == VIDEO_SUPPORT_2){
                int unitType = UNIT_CATAPULT;
                if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                    unitType = UNIT_TROLL;
                }
                createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, supportPointArray->getControlPointAtIndex(0), GM->getUnitName(unitType));
                unitType = UNIT_HELICOPTER;
                if(GM->currentStageIndex >= 12 && GM->currentStageIndex <= 24){
                    unitType = UNIT_WIZARD;
                }
                createUnit(unitType, WHICH_SIDE_HERO, ITS_NOT_BUILDING, supportPointArray->getControlPointAtIndex(1), GM->getUnitName(unitType));
            }
        }
        catch( ... )
        {
            log("support ads crash!");
            // Handle exception here.
        }
        finishedVideo = -1;
    }
    for(auto drop: heroArray){
        if(!drop->isArrived){
            if(drop->isFlying){
                drop->setLocalZOrder(10);
            }else{
                drop->setLocalZOrder(-drop->getBoundingBox().getMinY());
            }
        }
    }
    for(auto drop: enemyArray){
        if(!drop->isArrived){
            drop->setLocalZOrder(-drop->getBoundingBox().getMinY());
        }
        if(drop->isFlying){
            drop->setLocalZOrder(10);
        }
    }
    if (isGameOver) {
        return;
    }
    if (isPaused || isInEvent) {
        return;
    }
    
    if(isGameClear){
        endGame();
        return;
    }
    if (doubleClickTimerForSelectTheSame > 0) {
        doubleClickTimerForSelectTheSame -= dt;
    }
    if(doubleClickTimerForAttackDdaing > 0){
        doubleClickTimerForAttackDdaing -= dt;
    }
    gameTimer += dt;
    if(GM->currentStageIndex != STAGE_LOBBY){
        
        if(lastTimeCheck + 60 < gameTimer){
            lastTimeCheck = gameTimer;
            checkClearGame();
        }
        if(lastTenTick + 10 < gameTimer){
            lastTenTick = gameTimer;
            checkGameSchedule();
        }
        if(lastTick + 1 < gameTimer){
            lastTick = gameTimer;
            HUD->lblTimer->setString(GM->getTimeLeftInString(lastTick));
        }
    }
    
    if(stageCover != nullptr){
//        Point plPos = this->getCoordinateFromPosition(player->getPosition() + Point(0, -player->collectBoundingBox().size.height/2), theMap);
        Point plPos;
        int tgid = stageCover->getTileGIDAt(plPos);
        if(tgid && !isStageCoverTransparent){
            setStageCoverOpacity(167);
            isStageCoverTransparent = true;
        }else if(tgid == 0 && isStageCoverTransparent){
            setStageCoverOpacity(255);
            isStageCoverTransparent = false;
        }
    }
    
    float mapMoveSpeed = 30;
    if (isMapMoveUp) {
        float next = getPositionY() - mapMoveSpeed;
        if(next < -TILE_SIZE*mapSize.height + size.height){
            next = -TILE_SIZE*mapSize.height + size.height;
        }
        this->setPositionY(next);
        moveScreen(getPosition());
    }
    if (isMapMoveDown) {
        float next = getPositionY() + mapMoveSpeed;
        if(next > 0){
            next = 0;
        }
        this->setPositionY(next);
        moveScreen(getPosition());
    }
    if (isMapMoveLeft) {
        float next = getPositionX() + mapMoveSpeed;
        if(next > 0){
            next = 0;
        }
        this->setPositionX(next);
        moveScreen(getPosition());
    }
    if (isMapMoveRight) {
        float next = getPositionX() - mapMoveSpeed;
        if(next < -TILE_SIZE*mapSize.width + size.width){
            next = -TILE_SIZE*mapSize.width + size.width;
        }
        this->setPositionX(next);
        moveScreen(getPosition());
    }
    if(isSnowing){
        for(int i = 0; i < 0; i++){
            Sprite* sptSnow = Sprite::createWithSpriteFrameName(StringUtils::format("snowFlake%d.png", rand()%4));
            spriteBatch->addChild(sptSnow);
            int color = rand()%30 + 255 - 30;
            sptSnow->setColor(Color3B(color, color, color));
            float dur = 0.4f + (rand()%10)*0.05f;
            sptSnow->runAction(RotateBy::create(dur, rand()%200 + 150));
            sptSnow->runAction(FadeTo::create(dur, 50));
            int xMove = 30;
            int yMove = 50;
            sptSnow->setPosition(rand()%256 + xMove, rand()%256 + yMove);
            sptSnow->runAction(Sequence::create(MoveBy::create(dur, Point(rand()%xMove + 5, -yMove)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptSnow)), NULL));
        }
    }
    
    draw->clear();
//    GM->drawPath(); // test
//    for(auto unit: selectedArray){
//        draw->drawCircle(unit->getPosition() + Point(0, -unit->getContentSize().height/2), unit->getContentSize().width/2, 360, 20, false, 1, 0.25f, Color4F::GREEN);
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
    for (auto unit: unitsToCreateArray) {
        if (unit->isEnemy) {
            enemyArray.pushBack(unit);
            if(unit->wantToEli){
               attackNearHero((EnemyBase*)unit);
            }
        }else if (unit->alliSide == WHICH_SIDE_HERO) {
            heroArray.pushBack(unit);
            updateFoodInUse();
            updateFoodMaxState();
            if(unit->isBuilding){
                unit->setLocalZOrder(-unit->getPositionY());
            }else{
                unit->setLocalZOrder(-unit->getBoundingBox().getMinY());
            }
            
        }else if(unit->alliSide == WHICH_SIDE_MUTUAL){
            mutualArray.pushBack(unit);
        }else if(unit->alliSide == WHICH_SIDE_READY_HERO){
            readyHeroArray.pushBack(unit);
        }
        if (unit->level > 0 && !unit->isBuilding) {
            Node* nd = Node::create();
            this->addChild(nd);
            Sprite* sptCircle = Sprite::create("whiteCircleLine.png");
            nd->addChild(sptCircle, -1);
            sptCircle->setScale((unit->getContentSize().width)*1.2f/sptCircle->getContentSize().width);
            sptCircle->setColor(BHUD->getCardColorForLevel(unit->level));
            unit->ndLevelCircle = nd;
            nd->setPosition(unit->getEffectStartPosition());
            nd->setScale(1, 0.5f);
            sptCircle->runAction(RepeatForever::create(RotateBy::create(1, 180)));
            
        }
    }
    bool shouldExit = false;
    for (auto unit: readyHeroArray) {
        if(unit->getPosition() == Vec2::ZERO){
            continue;
        }
        if(unit->getPositionX() == 0 && unit->getPositionY() == 0 ){
            continue;
        }
        
        for(auto hero: heroArray){
            if (hero->getPosition().distanceSquared(unit->getPosition()) < 500000) {
                unit->blinkSelectedCircle();
                readyHeroArray.eraseObject(unit);
                heroArray.pushBack(unit);
                unit->alliSide = WHICH_SIDE_HERO;
                shouldExit = true;
                break;
            }
        }
        if(shouldExit){
            break;
        }
    }
    unitsToCreateArray.clear();
    
    if (!isGameOver) {
        for(auto drop: MovableArray){
//            drop->setLocalZOrder(-drop->getBoundingBox().getMinY());
            
            if (drop->freezed) {
                continue;
            }
            drop->updatePosition(dt);
//            checkForAndResolveCollisions(drop);
            if(drop->sptWeapon != nullptr){
                drop->sptWeapon->setLocalZOrder(-drop->getBoundingBox().origin.y);
            }
        }
    }
    
    
    testingFlag = !testingFlag;
    
    
    
    if (GameManager::getInstance()->firePressed) {
        if (playerFireCoolTime > 0) {
//            playerFireCoolTime -= dt;
        }else{
            if (playerFireCoolTimeMax < 0.001) {
                playerFireCoolTimeMax = 0.3f;
            }
            playerFireCoolTime = playerFireCoolTimeMax;
            fire();
        }
    }else{
//        playerFireCoolTime = 0;
        isFired = false;
        removeLaser();
        //        if (player->weapon == WEAPON_GUN) {
        //            playerFireCoolTime = 0;
        //        }
    }
    
    isWaterBoy = false;
    for(auto drop: MovableMissileArray){
        drop->updatePosition(dt);
        if(checkMovableMissileCollision(drop)) break;
    }
    
    
    //    player->armatureBody->setPosition(player->getPosition());
    //    this->updateIndicators();
    
//    stickTimeLeft -= dt;
//    if (stickTimeLeft < 0 && stageIndex != STAGE_ENTRANCE) {

//        stickTimeLeft = 0.1f + (rand()%40)*0.1f;
//    }
    
    
    
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
}
void HelloWorld::checkGameSchedule(){
    if(GM->isColosseum){
        return;
    }
    if (stageIndex == 3) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*4) {
            for(int i = 12; i < 19;i++){
                decoLayer->setTileGID(0, Point(28, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*7) {
            for(int i = 13; i < 21;i++){
                decoLayer->setTileGID(0, Point(i, 31));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 1 && gameTimer > 60*10) {
            for(int i = 12; i < 19;i++){
                decoLayer->setTileGID(0, Point(41, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 2 && gameTimer > 60*13) {
            for(int i = 13; i < 21;i++){
                decoLayer->setTileGID(0, Point(i, 39));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 3 && gameTimer > 60*5) {
            for(int i = 57; i < 61;i++){
                decoLayer->setTileGID(0, Point(30, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
    if (stageIndex == 4) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*11) {
            for(int i = 8; i < 14;i++){
                decoLayer->setTileGID(0, Point(i, 17));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*8) {
            for(int i = 57; i < 61;i++){
                decoLayer->setTileGID(0, Point(i, 16));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
    
    if (stageIndex == 5) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*11) {
            for(int i = 11; i < 17;i++){
                decoLayer->setTileGID(0, Point(i, 9));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*8) {
            for(int i = 8; i < 12;i++){
                decoLayer->setTileGID(0, Point(40, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 1 && gameTimer > 60*8) {
            for(int i = 33; i < 39;i++){
                decoLayer->setTileGID(0, Point(i, 32));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
}
void HelloWorld::oneSecUpdate(float dt){
    oneSecTiemr -= dt;
    if(oneSecTiemr > 0)
        return;
    oneSecTiemr = 1;
}
void HelloWorld::halfSecUpdate(float dt){
    fogUpdateTimer -= dt;
    if(fogUpdateTimer < 0){
        fogUpdateTimer = 0.5f;
        updateFog();
    }
    halfSecTimer -= dt;
    if(halfSecTimer < 0){
        halfSecTimer = 0.5f;
        updateMiniMapForMoving();
    }
}
void HelloWorld::updateFog(){
//    return; // test
    if(GM->currentStageIndex == STAGE_SINGLEPLAY || GM->currentStageIndex == STAGE_RAID){
        blackSheepWell = true;
        isRevealingMap = true;
    }
    for(auto fog: fogArray){
        fog->newState = FOG_SEEN_NOT;
    }
    updateMiniMapForNonMoving();
    Point fogCoordinate;
    Fog* fogAboveUnit;
    for(auto unit: heroArray){
        if(unit->isBuilding){
            for (int i = 0; i < 4; i++) {
                if(i == 0){
//                    fogCoordinate = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y)/FOG_SIZE;
                    fogCoordinate = unit->approachingPoints[0]/FOG_SIZE;
                }else if(i == 1){
//                    fogCoordinate = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x + unit->buildingOccupySize.width-1, unit->buildingStartCoordinate.y)/FOG_SIZE;
                    fogCoordinate = unit->approachingPoints[1]/FOG_SIZE;
                }else if(i == 2){
//                    fogCoordinate = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y+unit->buildingOccupySize.height-1)/FOG_SIZE;
                    fogCoordinate = unit->approachingPoints[2]/FOG_SIZE;
                }else if(i == 3){
//                    fogCoordinate = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x + unit->buildingOccupySize.width-1, unit->buildingStartCoordinate.y+unit->buildingOccupySize.height-1)/FOG_SIZE;
                    fogCoordinate = unit->approachingPoints[3]/FOG_SIZE;
                }
                
                fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
                fogAboveUnit->newState = FOG_SEEN_NOW;
                for(auto fog:fogAboveUnit->adjacentFogs){
                    fog->newState = FOG_SEEN_NOW;
                }
                for(auto fog:fogAboveUnit->farFogs){
                    if(fog->appliedState != FOG_SEEN_NOT_NOW && fog->newState != FOG_SEEN_NOW){
                        fog->newState = FOG_SEEN_LITTLE;
                    }
                }
            }
        }else{
            fogCoordinate = Point(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
            fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
            fogAboveUnit->newState = FOG_SEEN_NOW;
            for(auto fog:fogAboveUnit->adjacentFogs){
                fog->newState = FOG_SEEN_NOW;
            }
            for(auto fog:fogAboveUnit->farFogs){
//                if(fog->appliedState != FOG_SEEN_NOT_NOW && fog->newState != FOG_SEEN_NOW){
                if(fog->newState != FOG_SEEN_NOW){
                    fog->newState = FOG_SEEN_LITTLE;
                }
            }
        }
    }
    if(blackSheepWell){ // test 
        for(auto fog: fogArray){
            fog->newState = FOG_SEEN_NOW;
//            fog->appliedState = FOG_SEEN_NOW;
        }
    }
    
    if(isRevealingMap){
        for(auto fog: fogArray){
            fog->setVisible(false);
            fog->appliedState = FOG_SEEN_NOW;
        }
    }else{
        for(auto fog: fogArray){
            if (fog->newState == fog->appliedState) {
                continue;
            }
            if (fog->newState == FOG_SEEN_NOW) {
                fog->setVisible(false);
                fog->appliedState = FOG_SEEN_NOW;
            }else if(fog->newState == FOG_SEEN_LITTLE){
                fog->setVisible(true);
                fog->setOpacity(100);
                fog->appliedState = FOG_SEEN_LITTLE;
            }else if(fog->appliedState > FOG_SEEN_NOT_NOW && fog->newState == FOG_SEEN_NOT){
                fog->setVisible(true);
                fog->setOpacity(150);
                fog->appliedState = FOG_SEEN_NOT_NOW;
            }
        }
    }
    
    for(auto unit: enemyArray){
        fogCoordinate = Point(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
        unit->setVisible(fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW || unit->isDetected);
        if(unit->energyBar != nullptr){
            unit->energyBar->setVisible(unit->isVisible());
            unit->energyBarContent->setVisible(unit->isVisible());
            unit->energyBarBack->setVisible(unit->isVisible());
        }
        if(!unit->isDetected && unit->isBuilding && unit->isVisible()){
            unit->isDetected = true;
            updateMiniMapForNonMoving();
        }
    }
}
void HelloWorld::coinMagnet(){
    
}
Sprite* HelloWorld::getTeleport(int teleport){
    for(auto tele: teleportArray){
        if(tele->getTag() == teleport)
            return tele;
    }
    return NULL;
}
void HelloWorld::gravityUpdateForFlyingOrSwimingEnemies(float dt)
{
    if (isPaused) {
        return;
    }
    if (dt > 0.05) {
        dt = 0.05;
    }
    
    for(auto drop: flyingSwimingEnemyArray){
        if (drop->currentMoveType == ENEMY_MOVE_FLY_TO_HERO_FOLLOW) {
            bool flip = drop->getPosition().x > drop->target->getPosition().x;
            if (flip != drop->isFlippedX()) {
                drop->setFlippedX(flip);
            }
            drop->updatePositionToFlyToFollowTarget(dt);
            checkForAndResolveCollisions(drop);
        }else if (drop->currentMoveType == ENEMY_MOVE_SWIM_TO_HERO_FOLLOW) {
            drop->updatePositionToFlyToFollowTarget(dt);
            experimental::TMXTiledMap* map = getTileMap(drop->getPosition());
            int tileId = this->getTileAtPosition(drop->desiredPosition, TAG_FORE_LAYER, map);
            if (tileId >= 0) { // check if out of water
                drop->desiredPosition = drop->getPosition();
            }
            checkForAndResolveCollisions(drop);
            
            //            checkForAndResolveCollisionsForWater(drop);
        }
    }
}

void HelloWorld::gravityUpdateForCoins(float dt)
{
    
}
void HelloWorld::gravityUpdateForStraight(float dt)
{
    if (isPaused) {
        return;
    }
    for(auto drop: straightMovingArray)
    {
        drop->updatePositionForStraight(dt);
    }
    bool allClear = false;
    do
    {
        allClear = true;
        for(auto drop: straightMovingArray)
        {
            if(checkForAndResolveCollisionsForMissile(drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
    
}
void HelloWorld::gravityUpdateForCustomMoving(float dt)
{
    if (isPaused) {
        return;
    }
    for(auto drop: customMovingArray)
    {
        drop->desiredPosition = drop->getPosition();
    }
    bool allClear = false;
    do
    {
        allClear = true;
        for(auto drop: customMovingArray)
        {
            if(checkForAndResolveCollisionsForMissile(drop)){
                allClear = false;
                break;
            }
        }
    }while(!allClear);
}
void HelloWorld::chasingMissileUpdate(float dt)
{
    for (auto drop: chasingMissileArray)
    {
        if (drop->getTag() == UNIT_MISSILE_CHASING && drop->target != NULL) {
            float xGap = drop->getPosition().x - drop->target->getPosition().x;
            float yGap = drop->getPosition().y - drop->target->getPosition().y;
            
            double th = atan2(yGap, xGap);
            double angle = th*180/3.14 + 180;
            float gap = angle - drop->movingAngle;
            drop->movingAngle += gap > 0?(gap > 180?-2:2):(gap < -180?2:-2);
            //            drop->setRotation(-angle);
            drop->setRotation(-drop->movingAngle);
            if(drop->movingAngle < 0)
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
void HelloWorld::setViewPointCenter(Point position, bool forceMove)
{
    float scale = getScale();
    int x = position.x*scale;
    int y = position.y*scale;
    
    Point actualPosition = Point(x, y);
    
    Point centerOfView = Point(size.width/2, size.height*3/4 - 30);
    Point viewPoint = centerOfView - actualPosition;
    float distance = 0;
    float maxDistance = 300;
    if (distance > maxDistance) {
        distance = maxDistance;
    }
    float angle = GameManager::getInstance()->getAngle(center, center);
    Point aimingPos = Point(cos(angle*3.14/180)*distance, -sin(-angle*3.14/180)*distance);
    if(forceMove){
        this->setPosition(viewPoint - aimingPos*0.7f);
    }else{
        this->setPosition((viewPoint - aimingPos*0.7f).lerp(this->getPosition(), 0.9f));
    }
}

void HelloWorld::showBigMiniMap(bool isBig){
//    ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
//    if (isBig) {
//        float height = size.height*3/4;
//        float width = miniMapSize.width*height/miniMapSize.height;
//        currentMapSize = Size(width, height);
//        scrollView->setContentSize(Size(width, height));
//        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
//        scrollView->setPosition(Point(size.width/2 + width/2, size.height/2 + height/2));
//        scrollView->setBackGroundColorOpacity(200);
//    }else{
//        currentMapSize = miniMapSize;
//        scrollView->setContentSize(miniMapSize);
//        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
//        scrollView->setPosition(Point(size.width - 10, size.height - 10));
//        scrollView->setBackGroundColorOpacity(100);
//    }
//    Node* dnHero = scrollView->getInnerContainer()->getChildByName("Hero");
//    if (dnHero != nullptr) {
//        dnHero->setPosition(scrollView->getContentSize()/2);
//        dnHero->setPosition(dnHero->getPosition() + Point(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
//    }
//
//    scrollView->setInnerContainerSize(scrollView->getContentSize());
}
void HelloWorld::setCurrentTileMap(Point pos){
    for(auto map: mapArray){
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
experimental::TMXTiledMap* HelloWorld::getTileMap(Point pos){
    return theMap;
    for(auto map: mapArray){
        if (map->getBoundingBox().containsPoint(pos)) {
            return map;
        }
    }
    return nullptr;
}
cocos2d::Point HelloWorld::getCoordinateFromPosition(cocos2d::Point position){
    return getCoordinateFromPosition(position, theMap);
}
Point HelloWorld::getCoordinateFromPosition(Point position, experimental::TMXTiledMap* map)
{
//    int mapRowCount = map->getMapSize().height;
//    int mapColumnCount = map->getMapSize().width;
    float x = floor(position.x / TILE_SIZE);
//    if (x < 0) x =  0;
//    if (x >= mapColumnCount - 1) x = mapColumnCount - 1;
    float y = floor(((map->getMapSize().height * TILE_SIZE) - position.y) / TILE_SIZE);
//    if (y < 0) y =  0;
//    if (y >= mapRowCount - 1) y = mapRowCount - 1;
    return Point(x, y);
}

Rect HelloWorld::tileRectFromTileCoords(Point tileCoords, experimental::TMXTiledMap* map)
{
    return Rect(tileCoords.x*TILE_SIZE + map->getPositionX(), (map->getMapSize().height - tileCoords.y - 1)*TILE_SIZE + map->getPositionY(), TILE_SIZE, TILE_SIZE);
    //    float levelHeightInPixels = tileMap->getMapSize().height * tileMap->getTileSize().height;
    //    Point origin = Point(tileCoords.x * tileMap->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * tileMap->getTileSize().height));
    //    return Rect(origin.x, origin.y, tileMap->getTileSize().width, tileMap->getTileSize().height);
}

int HelloWorld::getTileAtPosition(Point position, int tag, experimental::TMXTiledMap* map)
{
    float mapX = map->getPositionX();
    float mapY = map->getPositionY();
    if (position.x < mapX || position.y < mapY || mapX + map->getMapSize().width*TILE_SIZE < position.x || mapY + map->getMapSize().height*TILE_SIZE < position.y ) {
        return 5;
    }
    Point plPos = this->getCoordinateFromPosition(position - map->getPosition(), map); //1
    //experimental::TMXLayer* layer = map->getLayer("stage");
    //Node* parent = layer->getParent();
    return ((experimental::TMXLayer*)map->getChildByTag(tag))->getTileGIDAt(plPos); //4
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
}

void HelloWorld::registerScriptAccelerateHandler(int handler)
{
    //    Director::getInstance()->getEventDispatcher()->addTargetedDelegate(this, 0, true);
}


void HelloWorld::setPlayerPosition(Point position) {
    
}
void HelloWorld::Dispose(){
    
}

void HelloWorld::addListener(){
    listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesBegan. ID: %d", touches.at(0)->getID());
        isBuildingTemplateMoved = false;
        isNormalClick = true;
        if (buildingTemplate != nullptr) {
            return;
        }
        touchBeganPos = touches.at(0)->getLocation();
        this->stopAllActions();
        lastTouchPoint = touchBeganPos;
        
        touchCount += touches.size();
        if (touchCount > 2) {
            touchCount = 2;
        }
        twoTouchEnabled = false;
        if(touchCount > 1 && false){
            twoTouchEnabled = true;
            Touch* touch = touches.at(touches.size() - 1);
            secondTouchID = touch->getID();
            lastTouchPoint = touch->getLocation();
            touchBeganPos = touch->getLocation();
            if (touches.size() == 1) {
                secondTouchBeganDistance = touches.at(0)->getLocation().distanceSquared(touches.at(1)->getLocation());
            }
        }
        isMapMovingByMiniMap = false;
        
        Touch* touch = touches.at(0);
        isTouchBeganFromMiniMap = false;
        if (GM->currentStageIndex != STAGE_LOBBY && Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
//            if(selectedArray.size() == 0){
                Point newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                Point newPos = -newTargetPos + size/2;
                moveScreen(newPos);
//            }
            isTouchBeganFromMiniMap = true;
        }
        isSelectedBuildingTouchStarted = false;
        if(GM->nextScene == STAGE_LOBBY){
            Point pos = touch->getLocation() - this->getPosition();
            for(auto unit: heroArray){ // select hero
                if(unit->getBoundingBox().containsPoint(pos) && unit == BHUD->selectedUnit && unit->unitType != UNIT_TREE_FOR_BATTLE && unit->isBuilding){
                    isSelectedBuildingTouchStarted = true;
                    isBuildingMovingFirstTry = true;
                    int unitType = BHUD->selectedUnit->unitType;
                    Size buildingSize = getBuildingOccupySize(unitType);
//                    Movable* unit = BHUD->selectedUnit;
                    setOccupy(getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y), unit->buildingOccupySize.width, unit->buildingOccupySize.height, false);
                    
                    this->createBuildingTemplate(unitType, buildingSize.width, buildingSize.height, getSpriteNameForUnit(unitType));
                    buildingTemplate->setPosition(BHUD->selectedUnit->getPosition() - Vec2(buildingTemplateSize.width*TILE_SIZE/2, buildingTemplateSize.height*TILE_SIZE/2));
                    
                    
                    log("selected buidling is see");
                    break;
                }
            }
        }
    };
    listener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
//        log("onTouchesMoved. count: %d", (int)touches.size());
//        log("touch x: %f, y: %f", touches.at(0)->getLocation().x, touches.at(0)->getLocation().y);
        Touch* touch = touches.at(0);
        if (buildingTemplate != nullptr && touchBeganPos.distanceSquared(touch->getLocation()) > 1000) { // building positioning
            float extraY = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
//            extraY = TILE_SIZE;
#endif
            Point coordinate = getCoordinateFromPosition(touch->getLocation() + Point(-getPositionX() - buildingTemplateSize.width, - getPositionY() - buildingTemplateSize.height +extraY), theMap);
            
            if(true || buildingTemplateCoordinate != coordinate){
//                log("diff x: %d, y: %d", (int)coordinate.x, (int)coordinate.y);
                isBuildingTemplateMoved = true;
//                Point pos = this->getPositionFromTileCoordinate(coordinate.x - buildingTemplateSize.width/2 + 1, coordinate.y + buildingTemplateSize.height/2) - Point(getPositionX() - (int)getPositionX()%100, getPositionY() - (int)getPositionY()%100);
//                Point pos = this->getPositionFromTileCoordinate(coordinate.x - buildingTemplateSize.width/2 + 1, coordinate.y + buildingTemplateSize.height/2) - Point(getPositionX(), getPositionY());
                Point pos = this->getPositionFromTileCoordinate(coordinate.x, coordinate.y) - Point(TILE_SIZE/2, TILE_SIZE/2);
                if(pos.x < TILE_SIZE){
                    pos.x = TILE_SIZE;
                }
                if(pos.y < TILE_SIZE){
                    pos.y = TILE_SIZE;
                }
                if(pos.x >= (mapSize.width - buildingTemplateSize.width)*TILE_SIZE){
                    pos.x = (mapSize.width - buildingTemplateSize.width)*TILE_SIZE;
                }
                if(pos.y >= (mapSize.height - buildingTemplateSize.height)*TILE_SIZE){
                    pos.y = (mapSize.height - buildingTemplateSize.height)*TILE_SIZE;
                }
//                pos = viewRect.origin + size/2;
                buildingTemplate->setPosition(Point(pos.x , pos.y));
                log("moving x: %f, y: %f", pos.x, pos.y);
//                buildingTemplate->setPosition(pos);
                checkBuildingTemplate();
            }else{
//                log("x: %d, y: %d", (int)coordinate.x, (int)coordinate.y);
            }
            return;
        }
        Touch* secondTouch = nullptr;
        for(auto touch: touches){
            if(touch->getID() == secondTouchID){
                secondTouch = touch;
                break;
            }
        }
        float touchBeganToEndDiff = touchBeganPos.distanceSquared(touch->getLocation());
        if (isTouchBeganFromMiniMap && touchBeganToEndDiff > 150 && buildingTemplate == nullptr){ // click
            if (Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
                isMapMovingByMiniMap = true;
                Point newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                Point newPos = -newTargetPos + size/2;
                moveScreen(newPos);
            }
        }
        
        if(GM->currentStageIndex != STAGE_LOBBY){
            HUD->draw->clear();
        }
//        if(secondTouch != nullptr && touches.size() > 1){
//        if(touches.size() == 1 && buildingTemplate == nullptr){
        if((!HUD || !HUD->imgDragSelected->isVisible()) && touches.size() == 1 && buildingTemplate == nullptr){
            Point dtPos = touches.at(0)->getLocation() - lastTouchPoint;
            moveScreen(getPosition() + dtPos);
            
            lastTouchPoint = touches.at(0)->getLocation();
            if(touches.size() > 1 && false){
                int distanceSquared = touches.at(0)->getLocation().distanceSquared(touches.at(1)->getLocation());
                float scalingRate = 0.0000001f;
                float distanceDiff = distanceSquared - secondTouchBeganDistance;
                distanceDiff*= scalingRate;
                float scale = this->getScale() + distanceDiff;
                log("secondTouchBeganDistance: %d, distanceSquared: %d, distanceDiff: %f", secondTouchBeganDistance, distanceSquared, distanceDiff);
                if(scale > maxLayerScale){
                    scale = maxLayerScale;
                }else if(scale < minLayerScale){
                    scale = minLayerScale;
                }
                this->setScale(scale);
            }
            twoTouchEnabled = true;
        }else if((HUD && HUD->imgDragSelected->isVisible())  && !isMapMovingByMiniMap){
            HUD->draw->drawRect(touchBeganPos, touches.at(0)->getLocation(), Color4F::GREEN);
        }
        
//        log("touchCount: %d", touchCount);
    };
    listener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
        if(!isNormalClick){
            return;
        }
        isNormalClick = false;
//        log("onTouchesEnded. count: %d", (int)touches.size());
        if (buildingTemplate != nullptr && GM->currentStageIndex != STAGE_LOBBY) {
            if(!isBuildingTemplateMoved){
                selectCommand(COMMAND_NOTHING);
            }else if(isBuildingReadyToBuild && selectedArray.size() == 1 && (selectedArray.at(0)->unitType == UNIT_WORKER || selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER)){
                selectedArray.at(0)->builderSize = buildingTemplateSize;
                selectedArray.at(0)->builderBuildingIndex = buildingTemplate->getTag();
                selectedArray.at(0)->builderCoordinate = buildingTemplateCoordinate;
                selectedArray.at(0)->builderSpriteName = buildingTemplate->getName();
                selectedArray.at(0)->isGoingToBuild = true;
                moveTo(selectedArray, this->getPositionFromTileCoordinate(buildingTemplateCoordinate.x, buildingTemplateCoordinate.y) + Point(buildingTemplateSize.width*TILE_SIZE/2, buildingTemplateSize.height*TILE_SIZE/2));
                
                this->onBuildingBetterClick(); // cancel current menues
                
                selectCommand(COMMAND_NOTHING);
                HUD->getChildByName("btnCancelBuilding")->removeFromParent();
                if(buildingTemplate != nullptr){
                    buildingTemplate->removeFromParent();
                    buildingTemplate = nullptr;
                }
            }
            
            return;
        }
        if(buildingTemplate != nullptr && GM->nextScene == STAGE_LOBBY && isSelectedBuildingTouchStarted){
            if((!isBuildingTemplateMoved && isBuildingMovingFirstTry) || isBuildingReadyToBuild){
                Movable* unit = BHUD->selectedUnit;
//                setOccupy(getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y), unit->buildingOccupySize.width, unit->buildingOccupySize.height, false);
                updateMiniMapForNonMoving();
                resetPathState();
                Vec2 newPos = buildingTemplate->getPosition() + Vec2(buildingTemplateSize.width*TILE_SIZE/2, buildingTemplateSize.height*TILE_SIZE/2);
                if(unit->unitType == UNIT_MINE){
                    std::string name = strmake("mineButton%d_%d", (int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE);
                    double previousTime = UDGetDouble(name.c_str(), 0);
                    Node* ndMineButton = WORLD->getChildByName(name);
                    ndMineButton->setPosition(newPos + Vec2(0, 150));
                    std::string newKey = strmake("mineButton%d_%d", (int)newPos.x/TILE_SIZE, (int)newPos.y/TILE_SIZE);
                    ndMineButton->setName(newKey);
                    UDSetDouble(newKey.c_str(), previousTime);
                    log("on mine moved");
                }else if(unit->unitType == UNIT_LUMBERMILL){
                    std::string name = strmake("treeButton%d_%d", (int)unit->getPositionX()/TILE_SIZE, (int)unit->getPositionY()/TILE_SIZE);
                    double previousTime = UDGetDouble(name.c_str(), 0);
                    Node* ndMineButton = WORLD->getChildByName(name);
                    ndMineButton->setPosition(newPos + Vec2(0, 150));
                    std::string newKey = strmake("treeButton%d_%d", (int)newPos.x/TILE_SIZE, (int)newPos.y/TILE_SIZE);
                    ndMineButton->setName(newKey);
                    UDSetDouble(newKey.c_str(), previousTime);
                    log("on lumbermill moved");
                }
                unit->setPosition(newPos);
                if (unit->lblTimeLeft) {
                    unit->lblTimeLeft->setPosition(newPos + Vec2(0, 60));
                }
                if (unit->sptUpgradeArrow) {
                    unit->sptUpgradeArrow->removeFromParent();
                    BHUD->addUpgradeEffect(unit, true);
                }
                
                setOccupy(buildingTemplate->getPosition() + Vec2(0, buildingTemplateSize.height*TILE_SIZE), unit->buildingOccupySize.width, unit->buildingOccupySize.height, true, (EnemyBase*)unit);
                
                deselectAll();
                selectUnit((EnemyBase*)unit);
                
                std::vector<int> datas;
                
                if (unit->isBuilding) {
//                    BHUD->updateBuildingsSaveData();
//                    datas.push_back(DATA_TYPE_BUILDING);
                    BHUD->shouldSaveBuilding = true;
                }else{
                    for(auto drop: heroArray){
                        if(!drop->isFlying && !drop->isArrived){
                            drop->setLocalZOrder(-drop->getBoundingBox().getMinY());
                        }
                    }
                    BHUD->shouldSaveDeck = true;
//                    BHUD->updateDeckSaveData();
//                    datas.push_back(DATA_TYPE_DECK);
                }
//                BHUD->saveUserData(datas);
                
                for(auto movable: heroArray){
                    if (!movable->isBuilding && unit->getBoundingBox().containsPoint(movable->getPosition())) {
                        movable->stop();
                    }
                }
                
                isSelectedBuildingTouchStarted = false;
                buildingTemplate->removeFromParent();
                buildingTemplate = nullptr;
            }else{
                isBuildingMovingFirstTry = false;
            }
            return;
        }
        Touch* touch = touches.at(0);
        if(twoTouchEnabled){
//            Size mapSize = theMap->getMapSize()*TILE_SIZE;
//            Point currentPos = getPosition();
//            this->stopAllActions();
//            if(this->getPosition().x < -mapSize.width + size.width){
//                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(-mapSize.width + size.width-getPositionX(), 0)), 3));
//            }
//            if(this->getPosition().y < -mapSize.height + size.height){
//                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(0, -mapSize.height + size.height-getPositionY())), 3));
//            }
//            if(this->getPosition().x > 0){
//                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(-getPositionX(), 0)), 3));
//            }
//            if(this->getPosition().y > 0){
//                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(0, -getPositionY())), 3));
//            }
        }
        
        bool somethingPicked = false;
        Point pos = lastTouchPoint - getPosition();
        for(auto unit: heroArray){
            if(unit->getBoundingBox().containsPoint(pos)){
                somethingPicked = true;
                
                break;
            }
        }
        
        float touchBeganToEndDiff = touchBeganPos.distanceSquared(touch->getLocation());
        log("one click done: distance: %f", touchBeganToEndDiff);
//        if(touchCount == 1 && !twoTouchEnabled){
//        if(touchBeganToEndDiff < 2000){
        if(true){
            if(touchBeganToEndDiff < 2000){ // click
                if (isTouchBeganFromMiniMap && Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
                    if(false){
                        if(!isMapMovingByMiniMap){
                            Point newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                            if(selectedArray.size() > 0){
                                doClick(newTargetPos);
                            }else{
                                
                            }
                        }
                    }else{
                        Point newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                        Point newPos = -newTargetPos + size/2;
                        moveScreen(newPos);
                    }
                }else{
                    doClick(touch->getLocation() - this->getPosition());
                }
                
//            }else{ // drag
            }else if((HUD && HUD->imgDragSelected->isVisible())){ // drag
                if(!isMapMovingByMiniMap){
                    Rect rect = Rect(touchBeganPos-this->getPosition(), Size( touch->getLocation().x - touchBeganPos.x, touch->getLocation().y - touchBeganPos.y));
                    selectByDrag(rect);
                    GM->playSoundEffect(SOUND_PENCIL_SHORT);
                }
            }
//            log("selected: %d", (int)selectedArray.size());
        }
        if(HUD){
            HUD->imgDragSelected->setVisible(false);
        }
        
        touchCount -= touches.size();
        if(touchCount <= 0){
            touchCount = 0;
            twoTouchEnabled = false;
        }
        if(GM->currentStageIndex != STAGE_LOBBY) HUD->draw->clear();
    };
    listener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event* evt){
        log("onTouchesCancelled");
        
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);
    
    mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [&](EventMouse* evt){
        touchBeganPos = evt->getLocationInView();
        lastTouchPoint = touchBeganPos;
        this->stopAllActions();
    };
    mouseListener->onMouseMove = [&](EventMouse* evt){
        Point pos = evt->getLocationInView();
//        log("x: %f, y: %f", pos.x, pos.y);

        isMapMoveLeft = pos.x < 10;
        isMapMoveRight = pos.x > size.width - 10;
        isMapMoveDown = pos.y < 10;
        isMapMoveUp = pos.y > size.height - 10;
//        HUD->draw->clear();
        //        if(twoTouchEnabled || touches.size() > 1){
        if((int)(evt->getMouseButton()) == 1){
            Point dtPos = pos - lastTouchPoint;
            this->setPosition(getPosition() + dtPos);
            lastTouchPoint = pos;
            
            if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 2) {
                HUD->tutorialIndex++;
                HUD->talkIndex = 0;
                HUD->talkText = LM->getText("tutorial 3");
                Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
                spt->setPosition(Point(size.width/2 - 750, 330));
                spt->stopAllActions();
                spt->setSpriteFrame("mine.png");
            }
        }else if((int)(evt->getMouseButton()) == 0){
//            HUD->draw->drawRect(touchBeganPos, evt->getLocationInView(), Color4F::GREEN);
        }
    };
    mouseListener->onMouseUp = [&](EventMouse* evt){
        
        if((int)(evt->getMouseButton()) == 1){
            Size mapSize = theMap->getMapSize()*TILE_SIZE;
            Point currentPos = getPosition();
            if(this->getPosition().x < -mapSize.width + size.width){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(-mapSize.width + size.width-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y < -mapSize.height + size.height){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(0, -mapSize.height + size.height-getPositionY())), 3));
            }
            if(this->getPosition().x > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Point(0, -getPositionY())), 3));
            }
        }
//        HUD->draw->clear();
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(mouseListener, 1);
    
    /*listener = cocos2d::EventListenerTouchOneByOne::create();
     listener->setSwallowTouches(false);
     
     listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
     listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
     listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
     //    listener->onTouchCancelled = CC_CALLBACK_2(MenuLayerMainMenu::onTouchCancelled, this);
     
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
     
     cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 30);
     */
}

void HelloWorld::moveScreen(cocos2d::Point pos){
    float xMin = -400;
    float xMax = 400;
    float yMin = -400;
    float yMax = 400;
    if(pos.x > xMax){
        pos.x = xMax;
    }
    if(pos.y > yMax){
        pos.y = yMax;
    }
    if(pos.x < -TILE_SIZE*mapSize.width + size.width + xMin){
        pos.x = -TILE_SIZE*mapSize.width + size.width + xMin;
    }
    if(pos.y < -TILE_SIZE*mapSize.height + size.height + yMin){
        pos.y = -TILE_SIZE*mapSize.height + size.height + yMin;
    }
    this->setPosition(pos);
    if(GM->currentStageIndex != STAGE_LOBBY){
        miniMapViewRect = Rect(miniMapDrawStartPos.x + (-pos.x)*miniMapScale, miniMapDrawStartPos.y + (-pos.y)*miniMapScale, size.width*miniMapScale, size.height*miniMapScale);
        viewRect = Rect(-pos, size);
        
        updateMiniMapForMoving();
    }
}
void HelloWorld::doClick(cocos2d::Point pos){
    try
    {
        if(HUD){
            log("stage %d", GM->currentStageIndex);
            if(GM->currentStageIndex == STAGE_RAID || GM->currentStageIndex == STAGE_SINGLEPLAY){
                HUD->rightBottomPanel->getChildByName("ndMission")->setVisible(false);
                HUD->rightBottomPanel->getChildByName("ndInfo")->setVisible(false);
            }else{
                HUD->rightBottomPanel->getChildByName("ndMission")->setVisible(true);
                HUD->rightBottomPanel->getChildByName("ndInfo")->setVisible(false);
            }
            
            for(int i = 0; i < 6; i++){
                HUD->setMenu(i, BTN_TYPE_NONE);
            }
            if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 11) {
                HUD->tutorialEnded = true;
            }
        }
        
        selectedCommand = COMMAND_NOTHING;
        
        EnemyBase* selectedUnit = nullptr;
        if (selectedUnit == nullptr){
            Vector<EnemyBase*> selectedArray;
            for(auto unit: heroArray){ // select hero
                if(unit->spine){
                    if(unit->spine->getBoundingBox().containsPoint(pos - unit->getPosition())){
                        selectedArray.pushBack(unit);
                    }
                }else if(unit->Sprite::getBoundingBox().containsPoint(pos)){
                    selectedArray.pushBack(unit);
                }
            }
            int zOrder = -9999999;
            for(auto unit: selectedArray){
                if(unit->getLocalZOrder() > zOrder){
                    selectedUnit = unit;
                    zOrder = unit->getLocalZOrder();
                }
            }
            selectedArray.clear();
        }
        for (auto unit: enemyArray){ // select enemy
            if(unit->Sprite::getBoundingBox().containsPoint(pos)){
                selectedUnit = unit;
                doubleClickTimerForAttackDdaing = 0;
                doubleClickTimerForSelectTheSame = 0;
                break;
            }
        }
        
        EnemyBase* mutual = nullptr;
        if(selectedUnit == nullptr){
            for(auto unit: mutualArray){
    //            Rect rect = Rect(pos.x - 10, pos.y - 10, unit->getContentSize().width + 20, unit->getContentSize().height + 20);
                if(unit->getBoundingBox().containsPoint(pos)){
    //            if(unit->getBoundingBox().intersectsRect(rect)){
                    mutual = unit;
                    doubleClickTimerForAttackDdaing = 0;
                    doubleClickTimerForSelectTheSame = 0;
                    break;
                }
            }
        }
        if(mutual != nullptr){
            for(auto unit: selectedArray){
                if(unit->alliSide == WHICH_SIDE_ENEMY) continue;
                if(unit->unitType == UNIT_WORKER || unit->unitType == UNIT_GOBLIN_WORKER){
                    if(mutual->unitType == UNIT_MINE){
                        if(unit->isCarryingGold){
                            unit->isGatheringGold = true;
                            unit->failedFindPathStart = Point::ZERO;
                            unit->failedFindPathEnd = Point::ZERO;
                            unit->isTemporaryFlying = true;
                            if (isNewCommandSystem) {
                                unit->unitAct = UNIT_ACT_GATHER_GOLD;
                                unit->moveFlagPos = unit->returningPlace->getPosition();
                                unit->moveToPos = Vec2::ZERO;
                            }else{
                                unit->moveToTarget(unit->returningPlace);
                            }
                        }else{
                            unit->gatherGold(mutual);
                            if (WORLD->stageIndex == 0 && unit->unitType == UNIT_WORKER && HUD->tutorialIndex == 3) {
                                HUD->tutorialIndex++;
                                HUD->talkIndex = 0;
                                HUD->talkText = LM->getText("tutorial 4");
                                Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
                                spt->stopAllActions();
                                spt->setSpriteFrame("tree2_0.png");
                            }
                        }
                        if(mutual != nullptr){
                            GM->playSoundEffect(SOUND_PENCIL_SHORT);
                            Sprite* spt = Sprite::createWithSpriteFrameName("pickax.png");
                            spt->setPosition(mutual->getPosition() + Point(0, 100));
                            this->addChild(spt, 100);
                            spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Point(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Point(0, 50)), 2), SPT_REMOVE_FUNC, NULL));
                        }
                    }else if(mutual->unitType == UNIT_TREE || mutual->unitType == UNIT_UNREACHABLE_TREE){
                        if(mutual->unitType == UNIT_UNREACHABLE_TREE){
                            mutual->unitType = UNIT_TREE;
                        }
                        if(unit->isCarryingTree){
                            unit->isGatheringTree = true;
                            unit->failedFindPathStart = Point::ZERO;
                            unit->failedFindPathEnd = Point::ZERO;
                            unit->isTemporaryFlying = true;
                            if (isNewCommandSystem) {
                                unit->unitAct = UNIT_ACT_GATHER_GOLD;
                                unit->moveFlagPos = unit->returningPlace->getPosition();
                                unit->moveToPos = Vec2::ZERO;
                            }else{
                                unit->moveToTarget(unit->returningPlace);
                            }
                        }else{
                            Point coordinate = WORLD->getCoordinateFromPosition(unit->getPosition());
                            Point destCoordinate = WORLD->getCoordinateFromPosition(mutual->getApproachingPoint(unit->getPosition()));
                            PointArray* array = GM->getPath(coordinate, destCoordinate);
                            if(array->count() > 0){
                                
                            }else{
                                mutual = WORLD->getNearestTree(unit->getPosition());
                            }
                            if(mutual != nullptr){
                                unit->gatherTree(mutual);
                                if (WORLD->stageIndex == 0 && unit->unitType == UNIT_WORKER && HUD->tutorialIndex == 4) {
                                    HUD->tutorialIndex++;
                                    HUD->talkIndex = 0;
                                    HUD->talkText = LM->getText("tutorial 5");
                                    Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
                                    spt->stopAllActions();
                                    Sprite* sptTemp = Sprite::create("btnBuild.png");
                                    spt->setSpriteFrame(sptTemp->getSpriteFrame());
                                }
                                GM->playSoundEffect(SOUND_PENCIL_SHORT);
                                Sprite* spt = Sprite::createWithSpriteFrameName("axe.png");
                                spt->setPosition(mutual->getPosition() + Point(0, 100));
                                this->addChild(spt, 100);
                                spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Point(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Point(0, 50)), 2), SPT_REMOVE_FUNC, NULL));
                            }
                        }
                    }
                }else{
                    unit->moveToTarget(mutual);
                }
            }
            if(mutual->unitType == UNIT_MINE && GM->currentStageIndex != STAGE_LOBBY){
                HUD->showInstanceMessage(strmake(LM->getText("gold left").c_str(), mutual->energy));
            }
        }else if (selectedUnit == nullptr) { // ground selected
            if (selectedArray.size() > 0 && !selectedArray.at(0)->isEnemy && !selectedArray.at(0)->isAlli && !selectedArray.at(0)->isBuilding && pos.x > 0 && pos.y > 0 && pos.x < mapSize.width*TILE_SIZE && pos.y < mapSize.height*TILE_SIZE) { // hero is selected so move!
                if(selectedCommand == COMMAND_ATTACK){
                    moveAndAttackTo(selectedArray, pos);
                }else{
                    if (doubleClickTimerForAttackDdaing > 0) {
                        onAttackClick();
                        moveAndAttackTo(selectedArray, pos);
                        selectCommand(COMMAND_MOVE);
                    }else{
                        onMoveClick();
                        Movable* building = nullptr;
                        for(auto unit: heroArray){
                            if(unit->isBuilding){
                                Point buildingPos = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y);
                                float height = unit->buildingOccupySize.height*TILE_SIZE;
                                Rect theRect = Rect(buildingPos.x, buildingPos.y - height, unit->buildingOccupySize.width*TILE_SIZE, height);
                                if(theRect.containsPoint(pos)){
                                    building = unit;
                                    log("building clickek %d", unit->unitType);
                                    break;
                                }
                            }
                        }
                        for(auto unit: selectedArray){
                            unit->isGoingToBuild = false;
                        }
                        if(building == nullptr){
                            moveTo(selectedArray, pos);
                            doubleClickTimerForAttackDdaing = 0.3f;
                            if(GM->nextScene == STAGE_LOBBY){
                                moveArrows->setVisible(false);
                            }
                        }else{
                            moveTo(selectedArray, building->getApproachingPoint(pos));
                        }
                    }
    //                doubleClickTimer = 0.3f;
                }
            }else{
                doubleClickTimerForAttackDdaing = 0.3f;
                deselectAll();
            }
        }else if(selectedUnit->isEnemy){ // enemy selected
            if (selectedArray.size() > 0 && !selectedArray.at(0)->isEnemy && !selectedArray.at(0)->isBuilding) { // hero is selected so attack!
                forceAttack(selectedArray, selectedUnit);
            }else{
                deselectAll();
                selectUnit(selectedUnit);
            }
        }else if(!selectedUnit->isEnemy){ // hero selected
            bool isWorkingWorkerSelected = false;
            for(auto unit: selectedArray){
                if(unit->unitType == UNIT_WORKER || unit->unitType == UNIT_GOBLIN_WORKER){
                    if (unit->isCarryingTree && (selectedUnit->unitType == UNIT_CASTLE || selectedUnit->unitType == UNIT_LUMBERMILL))  {
                        unit->returningPlace = selectedUnit;
                        unit->moveToTarget(selectedUnit);
                        unit->unitAct = UNIT_ACT_GATHER_TREE;
                        isWorkingWorkerSelected = true;
                        unit->isTemporaryFlying = true;
//                        unit->runAnimation(ANIMATION_TYPE_MOVE);
                        unit->isGatheringTree = true;
                    }else if(unit->isCarryingGold && (selectedUnit->unitType == UNIT_CASTLE)){
                        unit->returningPlace = selectedUnit;
                        unit->moveToTarget(selectedUnit);
                        isWorkingWorkerSelected = true;
                        unit->isTemporaryFlying = true;
//                        unit->runMoveAnimation(DIRECTION_E);
                        unit->isGatheringGold = true;
                    }
                }
            }
            if (!isWorkingWorkerSelected) {
                if (selectedCommand == COMMAND_ATTACK) {
                    forceAttack(selectedArray, selectedUnit);
                }else{
                    deselectAll();
                    
                    if (doubleClickTimerForSelectTheSame > 0 && !selectedUnit->isBuilding) {
                        for(auto unit:heroArray){
                            if(unit->unitType == selectedUnit->unitType && viewRect.containsPoint(unit->getPosition())){
                                selectUnit(unit);
                            }
                        }
                    }else{
                        selectUnit(selectedUnit);
                        
                        if (GM->currentStageIndex != STAGE_LOBBY && WORLD->stageIndex == 0 && HUD->tutorialIndex == 0 && !GM->isColosseum) {
                            HUD->tutorialIndex++;
                            HUD->talkIndex = 0;
                            HUD->talkText = LM->getText("tutorial 1");
                            Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
                            Sprite* sptTemp = Sprite::create("handIcon.png");
                            spt->setSpriteFrame(sptTemp->getSpriteFrame());
                            Size size = Director::getInstance()->getWinSize();
                            spt->stopAllActions();
                            spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Point(size.width/2 - 750, 330 + 50)), MoveBy::create(0.1f, Point(0, -100)), MoveBy::create(0.15f, Point(0, 100)), DelayTime::create(1), NULL)));
                        }
                    }
                    
                    doubleClickTimerForSelectTheSame = 0.3f;
                    
                    
                    GM->playSoundEffect(SOUND_PENCIL_SHORT);
                }
            }
        }
        
        if(HUD && HUD->isRaid && !HUD->isRaidStarted && selectedUnit == nullptr && !GM->isVisiting){
            if(isOccupied(WORLD->getCoordinateFromPosition(pos))){
                log("the spot clicked is occupied");
                return;
            }
            HUD->rightBottomPanel->getChildByName("ndBattle")->getChildByName("btnFindMatch")->removeFromParent();
//            HUD->rightBottomPanel->getChildByName("ndBattle")->getChildByName("btnSurrender")->removeFromParent();
            HUD->bottomUnitBar->runAction(EaseBackIn::create(MoveBy::create(0.5f, Vec2(0, -600))));
            Vec2 originalCoordinate = getCoordinateFromPosition(pos);
            HUD->isRaidStarted = true;
            int startX = 0;
            int max=0;
            int x=startX;
            int y=max;
            int direction = -1;//0:E 1:S 2:W 3:N
            bool changeDirection = false;
            Vector<Movable*> unitList;
//            for(int i = 0; i < 100; i++){ // test
//                UnitInfo* info = new UnitInfo();
//                info->unitType = UNIT_ARCHER;
//                HUD->deckUnitList.push_back(info);
//            }
            
            for(auto info : HUD->deckUnitList){
                Vec2 posFromCoordinate = getPositionFromTileCoordinate(originalCoordinate.x + x, originalCoordinate.y + y);
                info->x = posFromCoordinate.x;
                info->y = posFromCoordinate.y;
                Movable* unit = GM->getUnitFromData(info);
//                unit->setPosition(posFromCoordinate);
//                addUnit(unit);
                if(unit->getPositionX() < TILE_SIZE){
                    unit->setPositionX(TILE_SIZE);
                }else if(unit->getPositionY() < TILE_SIZE){
                    unit->setPositionY(TILE_SIZE);
                }else if(unit->getPositionX() >= mapSize.width*TILE_SIZE - TILE_SIZE){
                    unit->setPositionX(mapSize.width*TILE_SIZE - TILE_SIZE);
                }else if(unit->getPositionY() >= mapSize.height*TILE_SIZE - TILE_SIZE){
                    unit->setPositionY(mapSize.height*TILE_SIZE - TILE_SIZE);
                }
                
                do{
                    if (direction < 0 || (x == 0 && y == max)) {
                        y++;
                        x++;
                        max++;
                        if(direction < 0){
                            direction = 0;
                        }
                    }else if(direction == 0){
                        x++;
                        if(x > max){
                            changeDirection = true;
                        }
                    }else if(direction == 1){
                        y--;
                        if(y < -max){
                            changeDirection = true;
                        }
                    }else if(direction == 2){
                        x--;
                        if(x < -max){
                            changeDirection = true;
                        }
                    }else if(direction == 3){
                        y++;
                        if(y > max){
                            changeDirection = true;
                        }
                    }
                    if(changeDirection){
                        changeDirection = false;
                        if(direction == 0){
                            x--;
                            y--;
                        }else if(direction == 1){
                            x--;
                            y++;
                        }else if(direction == 2){
                            x++;
                            y++;
                        }else if(direction == 3){
                            x++;
                            y--;
                        }
                        direction++;
                        if(direction>3){
                            direction = 0;
                        }
                    }
                }while(isOccupied(originalCoordinate + Vec2(x, y)));
            }
        }
    }
    catch(...){
        
    }
}
bool HelloWorld::isCoordinateValid(Vec2 coordinate){
    if(coordinate.x < 0 || coordinate.y < 0 || coordinate.x >= mapSize.width || coordinate.y >= mapSize.height){
        return false;
    }
    return true;
}
void HelloWorld::checkBuildingTemplate(){
    isBuildingReadyToBuild = true;
    buildingTemplateCoordinate = getCoordinateFromPosition(buildingTemplate->getPosition(), theMap);
    
    for (int j = 0; j < buildingTemplateSize.height; j++) {
        for (int i = 0; i < buildingTemplateSize.width; i++) {
            Sprite* spt = (Sprite*)buildingTemplate->getChildByTag(i + j*buildingTemplateSize.width);
            bool isBlock = isDecoBlock(decoLayer->getTileGIDAt(Point(buildingTemplateCoordinate.x + i, buildingTemplateCoordinate.y - j - 1)));
            spt->setColor(isBlock?Color3B::RED:Color3B::GREEN);
            if(isBlock){
                isBuildingReadyToBuild = false;
            }
        }
    }
}
EnemyBase* HelloWorld::buildTheBuilding(int index, int x, int y,  int width, int height, std::string spriteName){
    bool isPlaceReady = true;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            bool isBlock = isDecoBlock(decoLayer->getTileGIDAt(Point(x + i, y - j - 1)));
            if(isBlock){
                isPlaceReady = false;
            }
        }
    }
    int price = this->getGoldPriceForUnit(index);
    if(price > gold || !isPlaceReady){
        return nullptr;
    }
    int lumberPrice = getLumberPriceForUnit(index);
    if(lumberPrice > lumber){
        return nullptr;
    }
    
    Point pos = getPositionFromTileCoordinate(x, y - height);
    EnemyBase* unit = createUnit(index, WHICH_SIDE_HERO, ITS_BUILDING, pos + Point(width*TILE_SIZE/2 - TILE_SIZE/2, -(height - 1)*TILE_SIZE + height*TILE_SIZE/2 - TILE_SIZE/2), spriteName.substr(0, spriteName.size() - 4), 1, spriteName);
    unit->isBuildingComplete = false;
    setOccupy(pos, width, height, true, unit);
    unit->setRotation(180);
    unit->retain();
    
    addGold(-price);
    addLumber(-lumberPrice);
    return unit;
}
void HelloWorld::addDecoToBuilding(Movable* unit){
    if(unit->unitType == UNIT_FARM){
        unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
        unit->smokePoint = unit->getPosition() + Point(-18, 110);
    }else if(unit->unitType == UNIT_AIRPORT){
        Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
        GM->runAnimation(spt, "propeller", true);
        unit->addChild(spt);
        spt->setPosition(Point(134, 206));
    }else if(unit->unitType == UNIT_CASTLE){
        Sprite* spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
        GM->runAnimation(spt, "blueFlag", true);
        unit->addChild(spt);
        spt->setPosition(Point(192, 255));
    }
}
std::string HelloWorld::getSpriteNameForUnit(int index){
    if(index == UNIT_WORKER){
        return "workerAxeStand0.png";
    }else if(index == UNIT_GOBLIN_WORKER){
        return "goblinAxeStand0.png";
    }else if(index == UNIT_SWORDMAN){
        return "swordmanStand0.png";
    }else if(index == UNIT_MINE){
        return "mine.png";
    }else if(index == UNIT_TROLL){
        return "trollStand0.png";
    }else if(index == UNIT_GOBLIN){
        return "goblinStand0.png";
    }else if(index == UNIT_GOBLIN_BOMB){
        return "goblinBombStand0.png";
    }else if(index == UNIT_ORC_SPEAR){
        return "orcSpearStand0.png";
    }else if(index == UNIT_ORC_AXE){
        return "orcAxeStand0.png";
    }else if(index == UNIT_ARCHER){
        return "archerStand0.png";
    }else if(index == UNIT_HELICOPTER){
        return "helicopter0.png";
    }else if(index == UNIT_CATAPULT){
        return "catapult0.png";
    }else if(index == UNIT_ZOMBIE_ORC_AXE){
        return "zombieOrc0.png";
    }else if(index == UNIT_ZOMBIE_SWORDSMAN){
        return "zombieHuman0.png";
    }else if(index == UNIT_CASTLE){
        return "castle.png";
    }else if(index == UNIT_FARM){
        return "farm.png";
    }else if(index == UNIT_BARRACKS){
        return "barracks.png";
    }else if(index == UNIT_LUMBERMILL){
        return "lumberMill.png";
    }else if(index == UNIT_WATCHERTOWER){
        return "watcherTower.png";
    }else if(index == UNIT_ORC_BUNKER){
        return "bunker.png";
    }else if(index == UNIT_ORC_HQ){
        return "hq.png";
    }else if(index == UNIT_ORC_BARRACKS){
        return "axeport.png";
    }else if(index == UNIT_ORC_TROLL_HOUSE){
        return "statueHouse.png";
    }else if(index == UNIT_TEMPLE){
        return "alter.png";
    }else if(index == UNIT_BARBECUE){
        return "barbecue.png";
    }else if(index == UNIT_AIRPORT){
        return "airport.png";
    }else if(index == UNIT_FACTORY){
        return "factory.png";
    }else if(index == UNIT_UNDERGROUND_BUNKER){
        return "undergroundBunker.png";
    }else if(index == UNIT_TRIGGER){
        return "trigger.png";
    }else if(index == UNIT_TREE_FOR_BATTLE){
        return "tree3_0.png";
    }else if(index == UNIT_WIZARD){
        return "wizardStand0.png";
    }
    return "";
}

Sprite* HelloWorld::getSpriteForIcon(int index){
    Sprite* sptBuilding;
    if(index ==  UNIT_WORKER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
    }else if(index ==  UNIT_GOBLIN_WORKER){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinAxeStand0.png");
    }
    else if(index ==  UNIT_SWORDMAN ){
        sptBuilding = Sprite::createWithSpriteFrameName("swordmanStand0.png");
    }
    else if(index ==  UNIT_ARCHER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("archerStand0.png");
    }
    else if(index ==  UNIT_HELICOPTER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("helicopter0.png");
    }
    else if(index ==  UNIT_CATAPULT  ){
        sptBuilding = Sprite::createWithSpriteFrameName("catapult0.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x + 20, sptBuilding->getTextureRect().origin.y, 160, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_CASTLE  ){
        sptBuilding = Sprite::createWithSpriteFrameName("castle.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }
    else if(index ==  UNIT_FARM  ){
        sptBuilding = Sprite::createWithSpriteFrameName("farm.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_BARRACKS  ){
        sptBuilding = Sprite::createWithSpriteFrameName("barracks.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_LUMBERMILL  ){
        sptBuilding = Sprite::createWithSpriteFrameName("lumberMill.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x + 0, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }else if(index ==  UNIT_WATCHERTOWER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("watcherTower.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, sptBuilding->getContentSize().width, 210));
    }else if(index ==  UNIT_FACTORY  ){
        sptBuilding = Sprite::createWithSpriteFrameName("factory.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_AIRPORT  ){
        sptBuilding = Sprite::createWithSpriteFrameName("airport.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, sptBuilding->getContentSize().width, 200));
        Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
        sptBuilding->addChild(spt);
        spt->setPosition(Point(134, 176));
    }else if(index ==  UNIT_ORC_HQ){
        sptBuilding = Sprite::createWithSpriteFrameName("hq.png");
        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }else if(index ==  UNIT_ORC_BUNKER){
        sptBuilding = Sprite::createWithSpriteFrameName("bunker.png");
//        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }else if(index ==  UNIT_ORC_BARRACKS){
        sptBuilding = Sprite::createWithSpriteFrameName("axeport.png");
//        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }else if(index ==  UNIT_ORC_TROLL_HOUSE){
        sptBuilding = Sprite::createWithSpriteFrameName("statueHouse.png");
//        sptBuilding->setTextureRect(Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }else if(index ==  UNIT_TEMPLE){
        sptBuilding = Sprite::createWithSpriteFrameName("alter.png");
        sptBuilding->setScale(0.9f);
    }else if(index ==  UNIT_BARBECUE){
        sptBuilding = Sprite::createWithSpriteFrameName("barbecue.png");
    }else if(index ==  UNIT_GOBLIN){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinStand0.png");
    }else if(index ==  UNIT_GOBLIN_BOMB){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinBombStand0.png");
    }else if(index ==  UNIT_ORC_AXE){
        sptBuilding = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
    }else if(index ==  UNIT_ORC_SPEAR){
        sptBuilding = Sprite::createWithSpriteFrameName("orcSpearStand0.png");
    }else if(index ==  UNIT_TROLL){
        sptBuilding = Sprite::createWithSpriteFrameName("trollStand0.png");
        sptBuilding->setScale(0.8f);
    }else if(index ==  UNIT_WIZARD){
        sptBuilding = Sprite::createWithSpriteFrameName("wizardStand0.png");
    }
    
    return sptBuilding;
}
bool HelloWorld::isThisSpotAvailable(Movable* me){
    for(auto unit: heroArray){
        if (unit != me && unit->getPosition() == me->getPosition()) {
            return false;
        }
    }
    for(auto unit: enemyArray){
        if (unit != me && unit->getPosition() == me->getPosition()) {
            return false;
        }
    }
    
    Vec2 tileCoordinate = getCoordinateFromPosition(me->getPosition());
    if(tileCoordinate.x >= theMap->getMapSize().width){
        tileCoordinate.x = theMap->getMapSize().width - 1;
    }
    if(tileCoordinate.y >= theMap->getMapSize().height){
        tileCoordinate.y = theMap->getMapSize().height - 1;
    }
    
    if(!isCoordinateValid(tileCoordinate) || isDecoBlock(decoLayer->getTileGIDAt(tileCoordinate))){
        return false;
    }
    return true;
}
void HelloWorld::tryCreateUnit(int index){
    int goldPrice = getGoldPriceForUnit(index);
    int lumberPrice = getLumberPriceForUnit(index);
    if(gold < goldPrice){
//        HUD->showInstanceMessage(LM->getText("not enough gold"));
        HUD->showPriceInfo(LM->getText("not enough gold"), goldPrice, lumberPrice,  GM->getFoodUseForUnit(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else if(lumber < lumberPrice){
//        HUD->showInstanceMessage(LM->getText("not enough lumber"));
        HUD->showPriceInfo(LM->getText("not enough lumber"), goldPrice, lumberPrice,  GM->getFoodUseForUnit(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else if(foodInUse + GM->getFoodUseForUnit(index) > foodMax){
//        HUD->showInstanceMessage(LM->getText("not enough food"));
        HUD->showPriceInfo(LM->getText("not enough food"), goldPrice, lumberPrice,  GM->getFoodUseForUnit(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else{
        if (selectedArray.at(0)->btns.size() < 5) {
            addGold(-goldPrice);
            addLumber(-lumberPrice);
            selectedArray.at(0)->queueUnit(index);
        }
    }
}

void HelloWorld::setPriceInfo(int btnType){
    int unitIndex = getUnitIndexFromBtnMenuType(btnType);
    int goldPrice = getGoldPriceForUnit(unitIndex);
    int lumberPrice = getLumberPriceForUnit(unitIndex);
    Node* ndInfo = HUD->rightBottomPanel->getChildByName("ndInfo");
    Text* lblGold = (Text*)ndInfo->getChildByName("lblGold");
    Text* lblTree = (Text*)ndInfo->getChildByName("lblTree");
    lblGold->setString(Value(goldPrice).asString());
    lblTree->setString(Value(lumberPrice).asString());
    
    Node* imgBack = ndInfo->getChildByName("imgIconBack");
    imgBack->removeChildByName("sptIcon");
    
    Sprite* spt = getSpriteForIcon(unitIndex);
    spt->setName("sptIcon");
    imgBack->addChild(spt);
    spt->setPosition(imgBack->getContentSize()/2);
    
    Text* lbl = (Text*)ndInfo->getChildByName("lblName");
    LM->setLocalizedString(lbl, GM->getUnitName(unitIndex).c_str());
    ScrollView* sv = (ScrollView*)ndInfo->getChildByName("sv");
    Label* lblDescription = (Label*)sv->getChildByName("lblDescription");
    LM->setLocalizedString(lblDescription, GM->getUnitName(unitIndex) + " desc");
    if(sv->getContentSize().height > lblDescription->getContentSize().height){
        sv->setInnerContainerSize(sv->getContentSize());
    }else{
        sv->setInnerContainerSize(Size(sv->getContentSize().width, lblDescription->getContentSize().height));
    }
    lblDescription->setPositionY(sv->getInnerContainerSize().height);
}
int HelloWorld::getUnitIndexFromBtnMenuType(int btnType){
    if(btnType == BTN_TYPE_CASTLE){
        return UNIT_CASTLE;
    }else if(btnType == BTN_TYPE_FARM){
        return UNIT_FARM;
    }else if(btnType == BTN_TYPE_BARRACKS){
        return UNIT_BARRACKS;
    }else if(btnType == BTN_TYPE_WATCHER_TOWER){
        return UNIT_WATCHERTOWER;
    }else if(btnType == BTN_TYPE_LUMBER){
        return UNIT_LUMBERMILL;
    }else if(btnType == BTN_TYPE_FACTORY){
        return UNIT_FACTORY;
    }else if(btnType == BTN_TYPE_AIRPORT){
        return UNIT_AIRPORT;
    }else if(btnType == BTN_TYPE_WORKER){
        return UNIT_WORKER;
    }else if(btnType == BTN_TYPE_GOBLIN_WORKER){
        return UNIT_GOBLIN_WORKER;
    }else if(btnType == BTN_TYPE_SWORDMAN){
        return UNIT_SWORDMAN;
    }else if(btnType == BTN_TYPE_ARCHER){
        return UNIT_ARCHER;
    }else if(btnType == BTN_TYPE_HELICOPTER){
        return UNIT_HELICOPTER;
    }else if(btnType == BTN_TYPE_CATAPULT){
        return UNIT_CATAPULT;
    }else if(btnType == BTN_TYPE_HQ){
        return UNIT_ORC_HQ;
    }else if(btnType == BTN_TYPE_BUNKER){
        return UNIT_ORC_BUNKER;
    }else if(btnType == BTN_TYPE_ORC_BARRACKS){
        return UNIT_ORC_BARRACKS;
    }else if(btnType == BTN_TYPE_TROLL_HOUSE){
        return UNIT_ORC_TROLL_HOUSE;
    }else if(btnType == BTN_TYPE_TEMPLE){
        return UNIT_TEMPLE;
    }else if(btnType == BTN_TYPE_GOBLIN){
        return UNIT_GOBLIN;
    }else if(btnType == BTN_TYPE_GOBLIN_BOMB){
        return UNIT_GOBLIN_BOMB;
    }else if(btnType == BTN_TYPE_ORC_AXE){
        return UNIT_ORC_AXE;
    }else if(btnType == BTN_TYPE_ORC_SPEAR){
        return UNIT_ORC_SPEAR;
    }else if(btnType == BTN_TYPE_TROLL){
        return UNIT_TROLL;
    }else if(btnType == BTN_TYPE_MAGE){
        return UNIT_WIZARD;
    }else if(btnType == BTN_TYPE_BARBECUE){
        return UNIT_BARBECUE;
    }
}
bool HelloWorld::tryBuilding(int index){
    int goldPrice = getGoldPriceForUnit(index);
    int lumberPrice = getLumberPriceForUnit(index);
    if(gold < goldPrice){
//        HUD->showInstanceMessage(LM->getText("not enough gold"));
        HUD->showPriceInfo(LM->getText("not enough gold"), goldPrice, lumberPrice, 0);
        GM->playSoundEffect(SOUND_NAGATIVE);
        return false;
    }else if(lumber < lumberPrice){
//        HUD->showInstanceMessage(LM->getText("not enough lumber"));
        HUD->showPriceInfo(LM->getText("not enough lumber"), goldPrice, lumberPrice, 0);
        GM->playSoundEffect(SOUND_NAGATIVE);
        return false;
    }else{
        if(index == UNIT_FACTORY || index == UNIT_AIRPORT){
            bool exist = false;
            for(auto unit: heroArray){
                if(unit->unitType == UNIT_LUMBERMILL){
                    exist = true;
                    break;
                }
            }
            if(!exist){
                PPLabel* lbl = HUD->showInstanceMessage(LM->getText("build lumbermill first"));
                lbl->setWordColor("lumbermill", Color3B(245, 213, 71));
                GM->playSoundEffect(SOUND_NAGATIVE);
                return false;
            }
        }
        if(index == UNIT_TEMPLE || index == UNIT_ORC_TROLL_HOUSE){
            bool exist = false;
            for(auto unit: heroArray){
                if(unit->unitType == UNIT_ORC_BARRACKS){
                    exist = true;
                    break;
                }
            }
            if(!exist){
                PPLabel* lbl = HUD->showInstanceMessage(LM->getText("build orc barracks first"));
                lbl->setWordColor("weapon house", Color3B(245, 213, 71));
                GM->playSoundEffect(SOUND_NAGATIVE);
                return false;
            }
        }
        
        if(index == UNIT_CASTLE){
            createBuildingTemplate(index, 4, 3, "castle.png");
        }else if(index == UNIT_FARM){
            createBuildingTemplate(UNIT_FARM, 3, 2, "farm.png");
        }else if(index == UNIT_WATCHERTOWER){
            createBuildingTemplate(UNIT_WATCHERTOWER, 2, 2, "watcherTower.png");
        }else if(index == UNIT_LUMBERMILL){
            createBuildingTemplate(UNIT_LUMBERMILL, 3, 3, "lumberMill.png");
        }else if(index == UNIT_BARRACKS){
            createBuildingTemplate(UNIT_BARRACKS, 3, 3, "barracks.png");
        }else if(index == UNIT_FACTORY){
            createBuildingTemplate(UNIT_FACTORY, 3, 3, "factory.png");
        }else if(index == UNIT_AIRPORT){
            createBuildingTemplate(UNIT_AIRPORT, 3, 3, "airport.png");
        }else if(index == UNIT_ORC_HQ){
            createBuildingTemplate(UNIT_ORC_HQ, 4, 3, "hq.png");
        }else if(index == UNIT_ORC_BUNKER){
            createBuildingTemplate(UNIT_ORC_BUNKER, 2, 2, "bunker.png");
        }else if(index == UNIT_ORC_BARRACKS){
            createBuildingTemplate(UNIT_ORC_BARRACKS, 2, 2, "axeport.png");
        }else if(index == UNIT_ORC_TROLL_HOUSE){
            createBuildingTemplate(UNIT_ORC_TROLL_HOUSE, 3, 3, "statueHouse.png");
        }else if(index == UNIT_TEMPLE){
            createBuildingTemplate(UNIT_TEMPLE, 3, 3, "alter.png");
        }else if(index == UNIT_BARBECUE){
            createBuildingTemplate(UNIT_BARBECUE, 3, 2, "barbecue.png");
        }
    }
    return true;
}

void HelloWorld::createBuildingTemplate(int index, int width, int height, std::string spriteName){
    
    if(GM->currentStageIndex == STAGE_LOBBY){
        if(!isSelectedBuildingTouchStarted){
            BHUD->showCancelButton();
        }
    }else{
        HUD->showCancelBuildingButton();
        HUD->rightBottomPanel->getChildByName("ndMission")->setVisible(true);
        HUD->rightBottomPanel->getChildByName("ndInfo")->setVisible(false);
    }
    
    buildingTemplate = Node::create();
    this->addChild(buildingTemplate, 10);
    buildingTemplate->setTag(index);
    Point pos = Vec2(-this->getPosition().x + size.width/2, -this->getPosition().y + size.height/2);//viewRect.origin + size/2;
    buildingTemplate->setPosition(Point(pos.x - (int)(pos.x)%100, pos.y - (int)(pos.y)%100 - 1));
    log("x: %f, y: %f", pos.x, pos.y);
    buildingTemplate->setName(spriteName);
    Sprite* spt = Sprite::createWithSpriteFrameName(spriteName);
    buildingTemplate->addChild(spt, 1);
    spt->setPosition(Point(width*TILE_SIZE/2, height*TILE_SIZE/2));
    spt->setOpacity(150);
    buildingTemplateSize = Size(width, height);
    buildingTemplate->setContentSize(buildingTemplateSize);
    for (int i = 0; i < width*height; i++) {
        spt = Sprite::createWithSpriteFrameName("whiteRect.png");
        buildingTemplate->addChild(spt);
        spt->setAnchorPoint(Point::ZERO);
        spt->setPosition(Point((i%width)*TILE_SIZE, (i/width)*TILE_SIZE));
        spt->setTag(i);
        spt->setOpacity(150);
    }
    checkBuildingTemplate();
}
void HelloWorld::onMoveClick(){
    if(selectedCommand == COMMAND_BUILD){
        
    }else if(selectedCommand == COMMAND_BUILD_BETTER){
        int goldPrice = getGoldPriceForUnit(UNIT_AIRPORT);
        int lumberPrice = getLumberPriceForUnit(UNIT_AIRPORT);
        if(gold < goldPrice){
            HUD->showInstanceMessage(LM->getText("not enough gold"));
            GM->playSoundEffect(SOUND_NAGATIVE);
        }if(lumber < lumberPrice){
            HUD->showInstanceMessage(LM->getText("not enough lumber"));
            GM->playSoundEffect(SOUND_NAGATIVE);
        }else{
            createBuildingTemplate(UNIT_AIRPORT, 3, 3, "airport.png");
        }
    }else{
        selectCommand(COMMAND_MOVE);
    }
}
void HelloWorld::onStopClick(){
    if(selectedCommand == COMMAND_BUILD){
        
    }else{
        selectCommand(COMMAND_STOP);
        stop(selectedArray);
        selectCommand(COMMAND_NOTHING);
    }
}
void HelloWorld::onAttackClick(){
    if(selectedCommand == COMMAND_BUILD){
        tryBuilding(UNIT_BARRACKS);
    }else{
        selectCommand(COMMAND_ATTACK);
    }
}
void HelloWorld::onGatherClick(){
    if(selectedCommand == COMMAND_BUILD){
        
    }else{
        
        selectCommand(COMMAND_GATHER);
        
        long minTreeDistance = 100000000;
        long minMineDistance = 2000000;
        long distance = 0;
        EnemyBase* nearestTree = nullptr;
        EnemyBase* nearestMine = nullptr;
        for(auto unit:mutualArray){
            distance = unit->getPosition().distanceSquared(selectedArray.at(0)->getPosition());
            if(unit->unitType == UNIT_TREE){
                if(distance < minTreeDistance){
                    minTreeDistance = distance;
                    nearestTree= unit;
                }
            }
            if(unit->unitType == UNIT_MINE){
                if(distance < minMineDistance){
                    minMineDistance = distance;
                    nearestMine = unit;
                }
            }
        }
        if (nearestMine != nullptr) {
            for(auto unit:selectedArray){
                unit->gatherGold(nearestMine);
            }
        }else if (nearestTree != nullptr) {
            for(auto unit:selectedArray){
                unit->gatherTree(getNearestTree(unit->getPosition()));
            }
        }else{
            HUD->showInstanceMessage(LM->getText("No resources are found."));
        }
    }
    
}
void HelloWorld::onBuildingClick(){
    if(selectedArray.size() == 0 || selectedArray.at(0) == 0 || (selectedArray.at(0)->unitType != UNIT_WORKER && selectedArray.at(0)->unitType != UNIT_GOBLIN_WORKER)) return;
    if(selectedCommand == COMMAND_BUILD){
        
    }else{
        selectCommand(COMMAND_BUILD);
        if(selectedArray.at(0)->unitType == UNIT_WORKER){
            HUD->setMenu(0, BTN_TYPE_CASTLE);
            HUD->setMenu(1, BTN_TYPE_FARM);
            HUD->setMenu(2, BTN_TYPE_BARRACKS);
            HUD->setMenu(3, BTN_TYPE_LUMBER);
            HUD->setMenu(4, BTN_TYPE_WATCHER_TOWER);
        }else if(selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER){
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
void HelloWorld::onBuildingBetterClick(){
    if(selectedArray.size() == 0 || (selectedArray.at(0)->unitType != UNIT_WORKER && selectedArray.at(0)->unitType != UNIT_GOBLIN_WORKER)) return;
    if(selectedCommand == COMMAND_BUILD || selectedCommand == COMMAND_BUILD_BETTER){
        
    }else{
        selectCommand(COMMAND_BUILD_BETTER);
        if(selectedArray.at(0)->unitType == UNIT_WORKER){
            HUD->setMenu(0, BTN_TYPE_AIRPORT);
            HUD->setMenu(1, BTN_TYPE_FACTORY);
        }else if(selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER){
            HUD->setMenu(0, BTN_TYPE_TROLL_HOUSE);
            HUD->setMenu(1, BTN_TYPE_TEMPLE);
        }
        
        HUD->setMenu(2, BTN_TYPE_NONE);
        HUD->setMenu(3, BTN_TYPE_NONE);
        HUD->setMenu(4, BTN_TYPE_NONE);
        HUD->setMenu(5, BTN_TYPE_CANCEL);
        HUD->arrangeMenu(selectedArray.at(0)->getPosition());
    }
}

void HelloWorld::onSelectOnScreenClick(){
    
}
void HelloWorld::onSelectOnMapClick(){
    
}
void HelloWorld::selectCommand(int command){
    selectedCommand = command;
    if(command == COMMAND_NOTHING){
        if(HUD){
            if(GM->currentStageIndex == STAGE_RAID || GM->currentStageIndex == STAGE_SINGLEPLAY){
                HUD->rightBottomPanel->getChildByName("ndMission")->setVisible(false);
                HUD->rightBottomPanel->getChildByName("ndInfo")->setVisible(false);
            }else{
                HUD->rightBottomPanel->getChildByName("ndMission")->setVisible(true);
                HUD->rightBottomPanel->getChildByName("ndInfo")->setVisible(false);
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
bool HelloWorld::isBlockExistBetween(Vec2 start, Vec2 end){
    float width = end.x - start.x;
    float xGap = end.x - start.x;
    float yGap = end.y - start.y;
    float theta = atan2(yGap, xGap);
    float gapX = cos(theta)*50;
    float gapY = sin(theta)*50;
    float accumulatedX = 0;
    float accumulatedY = 0;
    Vec2 pos;
    while(true){
        accumulatedX += gapX;
        accumulatedY += gapY;
        if(width < 0){
            if(accumulatedX < width){
                break;
            }
        }else{
            if(accumulatedX > width){
                break;
            }
        }
        
        pos = getCoordinateFromPosition(Vec2(start.x + accumulatedX, start.y + accumulatedY), theMap);
        if (GM->tileState[(int)pos.x][(int)pos.y] > 0) {
            return true;
        }
    }
    
    return false;
}
void HelloWorld::moveTo(EnemyBase* unit, Point pos){
    if(isNewCommandSystem){
        unit->moveFlagPos = pos;
        unit->moveToPos = Vec2::ZERO;
    }else{
        targetHand->setPosition(pos);
        unit->moveToTarget(targetHand);
        unit->canFindTarget = false;
        unit->attackFlagPos = Point::ZERO;
    }
}
void HelloWorld::moveTo(Vector<EnemyBase*> troop, Point pos){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(pos, false);
    bool isHelicopter = false;
    // I was not happy when I was lonely
    // I am not lonely but I am not happy
    // Maybe I was not happy not because I was lonely
    // I don't remember when I was happy
    // There were joyful moments
    // But was there a happy period?
    if (isNewCommandSystem) {
        for (auto unit: troop){
            unit->moveToPos = Vec2::ZERO;
            unit->moveFlagPos = pos;
            unit->target = nullptr;
            unit->unitAct = UNIT_ACT_MOVE;
            unit->unitActDetail = UNIT_ACT_DETAIL_IDLE;
            unit->cancelAttackSchedule();
            if(unit->unitType == UNIT_HELICOPTER){
                isHelicopter = true;
            }
        }
    }else{
        for (auto unit: troop){
            unit->isTemporaryFlying = false;
            unit->moveToTarget(targetHand);
            unit->canFindTarget = false;
            unit->attackFlagPos = Point::ZERO;
            unit->isGatheringTree = false;
            unit->isGatheringGold = false;
            if(unit->unitType == UNIT_HELICOPTER){
                isHelicopter = true;
            }
        }
    }
    
    if (isHelicopter) {
        GM->playSoundEffect(SOUND_HELICOPTER);
    }

    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 1) {
        HUD->tutorialIndex++;
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 3");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        spt->setPosition(Point(size.width/2 - 750, 330));
        spt->stopAllActions();
        spt->setSpriteFrame("mine.png");
    }
    
//    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 1) {
//        HUD->tutorialIndex++;
//        HUD->talkIndex = 0;
//        HUD->talkText = LM->getText("tutorial 2");
//        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
//        Sprite* sptTemp = Sprite::create("handIcon2.png");
//        spt->setSpriteFrame(sptTemp->getSpriteFrame());
//        Size size = Director::getInstance()->getWinSize();
//        spt->stopAllActions();
//        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Point(size.width/2 - 750 - 100, 330 + 50)), MoveBy::create(0.5f, Point(150, -200)), DelayTime::create(1), NULL)));
//    }
}
void HelloWorld::moveTo(Vector<EnemyBase*> troop, EnemyBase* target){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(target->getPosition(), false);
    for (auto unit: troop){
        unit->moveToTarget(target);
        unit->canFindTarget = false;
        unit->attackFlagPos = Point::ZERO;
    }
}
void HelloWorld::gatherTo(Vector<EnemyBase*> troop, EnemyBase* target){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(target->getPosition(), false);
    for (auto unit: troop){
        unit->moveToTarget(target);
        unit->canFindTarget = false;
    }
}
void HelloWorld::showTargetHand(cocos2d::Point pos, bool isAttack){
    targetHand->setPosition(pos);
    if (this->getChildByName("pointer")) {
        this->removeChildByName("pointer");
        this->removeChildByName("targetCircle");
    }
    Sprite* spt = Sprite::create(isAttack?"swordIcon.png":"handIcon.png");
    this->addChild(spt, 100);
    spt->setPosition(pos + Point(50, 50));
    spt->setOpacity(0);
    spt->setName("pointer");
    spt->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(0.5f), FadeOut::create(0.1f), SPT_REMOVE_FUNC, NULL));
    spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Point(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Point(0, 50)), 2), NULL));
    
    Node* node = Node::create();
    node->setName("targetCircle");
    this->addChild(node, 10);
    node->setPosition(pos);
    spt = Sprite::create("targetCircle.png");
    node->addChild(spt);
    spt->setOpacity(0);
    node->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, node)), NULL));
    float dur = 0.2f;
    spt->runAction(Sequence::create(FadeIn::create(dur), DelayTime::create(0.6f - dur*2), FadeOut::create(dur), NULL));
    spt->runAction(Sequence::create(MoveBy::create(dur, Point(0, 10)), DelayTime::create(0.6f - dur*2), MoveBy::create(dur, Point(0, -10)), NULL));
}
void HelloWorld::stop(Vector<EnemyBase*> troop){
    for(auto unit: selectedArray){
        unit->stop();
    }
}
void HelloWorld::moveAndAttackTo(EnemyBase* unit, cocos2d::Point pos){
    unit->targetCoordinate = Point::ZERO;
    unit->target = nullptr;
    unit->isTemporaryFlying = false;
    unit->moveToTarget(pos);
    unit->canFindTarget = unit->attackType != ATTACK_TYPE_NONE;
    unit->attackFlagPos = pos;
}
void HelloWorld::moveAndAttackTo(Vector<EnemyBase*> troop, Point pos){
    moveTo(troop, pos);
    showTargetHand(pos, true);
    for (auto unit: troop){
//        moveAndAttackTo(unit, pos);
        unit->attackFlagPos = unit->moveFlagPos;
        unit->unitAct = UNIT_ACT_ATTACK_DDANG;
        unit->target = nullptr;
    }
    selectCommand(COMMAND_NOTHING);
    
    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 10) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 11");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        spt->setVisible(false);
        //                    spt->setSpriteFrame("handIcon.png");
        //                    Size size = Director::getInstance()->getWinSize();
        //                    spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Point(size.width/2 - 750 - 50, 330 - 50)), MoveBy::create(0.5f, Point(100, -100)), DelayTime::create(1), NULL)));
    }
}
void HelloWorld::forceAttack(Vector<EnemyBase*> troop, EnemyBase* target){
    if(!HUD->isRaid){
        if(target->unitType == UNIT_TREE || target->unitType == UNIT_MINE){
            return;
        }
    }
    moveTo(troop, target->getApproachingPoint(troop.at(0)->getPosition()));
    showTargetHand(target->getPosition(), true);
    for (auto unit: troop){
        if(unit->attackType == ATTACK_TYPE_NONE || !canAttack(unit, target)){
            unit->moveToTarget(target);
            continue;
        }else{
            unit->unitAct = UNIT_ACT_ATTACK;
            unit->target = target;
        }
//        unit->moveToTarget(target);
        
        unit->forceAttackTarget = true;
        unit->canFindTarget = false;
        unit->attackFlagPos = Point::ZERO;
    }
    selectCommand(COMMAND_NOTHING);
}

void HelloWorld::selectByDrag(cocos2d::Rect rect){
    deselectAll();
//    draw->drawRect(rect.origin, Point(rect.getMaxX(), rect.getMaxY()), Color4F::RED);
    if(rect.size.width < 0){
        rect.origin.x = rect.origin.x + rect.size.width;
        rect.size.width *= -1;
    }
    if(rect.size.height < 0){
        rect.origin.y = rect.origin.y + rect.size.height;
        rect.size.height *= -1;
    }
    for(auto unit: heroArray){  // select heroes
        if(rect.containsPoint(unit->getPosition()) && !unit->isBuilding && !unit->isAlli){
            selectUnit(unit);
        }
    }
    if(selectedArray.size() == 0){ // select hero's building
        for(auto unit: heroArray){
            if(rect.containsPoint(unit->getPosition()) && unit->isBuilding){
                selectUnit(unit);
                break;
            }if(rect.containsPoint(unit->getPosition()) && unit->isAlli){
                selectUnit(unit);
                break;
            }
        }
    }
    if(selectedArray.size() == 0){  // select enemy
        for(auto unit: enemyArray){
            if(rect.containsPoint(unit->getPosition())){
                selectUnit(unit);
                break;
            }
        }
    }
    
    if (WORLD->stageIndex == 0 && HUD->tutorialIndex == 9) {
        HUD->tutorialIndex++;
        HUD->talkIndex = 0;
        HUD->talkText = LM->getText("tutorial 10");
        Sprite* spt = (Sprite*)HUD->tutorialNode->getChildByName("sptIcon");
        Sprite* sptTemp = Sprite::create("handIcon.png");
        spt->setSpriteFrame(sptTemp->getSpriteFrame());
        Size size = Director::getInstance()->getWinSize();
        spt->stopAllActions();
        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Point(size.width/2 - 750, 330 + 50)), MoveBy::create(0.1f, Point(0, -100)), MoveBy::create(0.1f, Point(0, 100)), MoveBy::create(0.1f, Point(0, -100)), MoveBy::create(0.15f, Point(0, 100)), DelayTime::create(1), NULL)));
    }
}
void HelloWorld::deselectAll(){
    for(auto spt: selectedArray){
        spt->showSelectedCircle(false);
    }
    if(moveArrows){
        moveArrows->setVisible(false);
    }
    if (GM->nextScene == STAGE_LOBBY) {
        BHUD->onUnitSelected(nullptr);
    }
    selectedArray.clear();
    updateMenu();
}
void HelloWorld::deselect(Movable* unit){
    if(selectedArray.find((EnemyBase*)unit) != selectedArray.end()){
        selectedArray.eraseObject((EnemyBase*)unit);
        unit->showSelectedCircle(false);
//        updateMenu();
    }
}
void HelloWorld::selectUnit(EnemyBase* unit){
    if(selectedArray.size() == 1 && selectedArray.at(0) == unit){
//        doubleClickTimer = 0.3f;
    }else{
//        doubleClickTimer = -1;
    }
    doubleClickTimerForAttackDdaing = -1;
    if(unit->isVisible()){
        selectedArray.pushBack(unit);
        unit->showSelectedCircle(true);
        updateMenu();
    }
    
    if(GM->nextScene == STAGE_LOBBY){
        BHUD->onUnitSelected(unit);
        if(moveArrows == nullptr){
            moveArrows = Node::create();
            this->addChild(moveArrows);
            for(int i = 0; i < 4; i++){
                Sprite* spt = Sprite::create("greenArrow.png");
                spt->setScale(0.3f);
                moveArrows->addChild(spt);
                spt->setTag(i);
                spt->setRotation(i*90);
            }
            moveArrows->setVisible(false);
        }
        if (unit->unitType != UNIT_TREE_FOR_BATTLE && unit->isBuilding) {
            moveArrows->setVisible(true);
        }
        
        Node* circle = unit->sptSelectedCircle;
        moveArrows->setPosition(circle->getPosition());
        for(int i = 0; i < 4; i++){
            Node* spt = moveArrows->getChildByTag(i);
            if(i == 0){
                spt->setPosition(Vec2(circle->getContentSize().width*circle->getScaleX()*1.3f/2, 0));
            }else if(i == 1){
                spt->setPosition(Vec2(0, -circle->getContentSize().height*circle->getScaleY()*1.3f/2));
            }else if(i == 2){
                spt->setPosition(Vec2(-circle->getContentSize().width*circle->getScaleX()*1.3f/2, 0));
            }else if(i == 3){
                spt->setPosition(Vec2(0, circle->getContentSize().height*circle->getScaleY()*1.3f/2));
            }
        }
    }
}
void HelloWorld::updateFoodMaxState(){
    if(GM->currentStageIndex == STAGE_LOBBY) return;
    int max = 0;
    for(auto unit: heroArray){
        if(unit->isBuildingComplete && !unit->isAlli){
            max += getFoodGive(unit->unitType);
        }
    }
    foodMax = max;
    HUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
    checkClearGame();
}
void HelloWorld::updateMenu(){
//    if(buildingTemplate != nullptr && GM->currentStageIndex != STAGE_LOBBY){
//        buildingTemplate->removeFromParent();
//        buildingTemplate = nullptr;
//    }
    selectCommand(COMMAND_NOTHING);
    if(selectedArray.size() == 0 || selectedArray.size() > 1){
        
        for(int i = 0; i < 6; i++){
            HUD->setMenu(i, BTN_TYPE_NONE);
        }
    }else if(selectedArray.at(0)->alliSide == WHICH_SIDE_HERO){
        if (selectedArray.at(0)->isBuilding && selectedArray.at(0)->isBuildingComplete) {
            int noneFrom = 3;
            if(selectedArray.at(0)->unitType == UNIT_CASTLE){
                if(selectedArray.at(0)->unitName.compare("hq") == 0){
                    HUD->setMenu(0, BTN_TYPE_GOBLIN_WORKER);
                }else{
                    HUD->setMenu(0, BTN_TYPE_WORKER);
                }
                noneFrom = 1;
            }else if(selectedArray.at(0)->unitType == UNIT_FARM){
                noneFrom = 0;
            }else if(selectedArray.at(0)->unitType == UNIT_BARRACKS){
                HUD->setMenu(0, BTN_TYPE_SWORDMAN);
                HUD->setMenu(1, BTN_TYPE_ARCHER);
                noneFrom = 2;
            }else if(selectedArray.at(0)->unitType == UNIT_ORC_BARRACKS){
                HUD->setMenu(0, BTN_TYPE_GOBLIN);
                HUD->setMenu(1, BTN_TYPE_GOBLIN_BOMB);
                HUD->setMenu(2, BTN_TYPE_ORC_AXE);
                HUD->setMenu(3, BTN_TYPE_ORC_SPEAR);
                noneFrom = 4;
            }else if(selectedArray.at(0)->unitType == UNIT_LUMBERMILL){
                noneFrom = 0;
            }else if(selectedArray.at(0)->unitType == UNIT_WATCHERTOWER){
                noneFrom = 0;
            }else if(selectedArray.at(0)->unitType == UNIT_AIRPORT){
                HUD->setMenu(0, BTN_TYPE_HELICOPTER);
                noneFrom = 1;
            }else if(selectedArray.at(0)->unitType == UNIT_ORC_TROLL_HOUSE){
                HUD->setMenu(0, BTN_TYPE_TROLL);
                noneFrom = 1;
            }else if(selectedArray.at(0)->unitType == UNIT_FACTORY){
                HUD->setMenu(0, BTN_TYPE_CATAPULT);
                noneFrom = 1;
            }else if(selectedArray.at(0)->unitType == UNIT_TEMPLE){
                HUD->setMenu(0, BTN_TYPE_MAGE);
                noneFrom = 1;
            }
            
            for(int i = noneFrom; i < 6; i++){
                HUD->setMenu(i, BTN_TYPE_NONE);
            }
            HUD->arrangeMenu(selectedArray.at(0)->getPosition());
        }else{
//            HUD->setMenu(0, BTN_TYPE_MOVE);
//            HUD->setMenu(1, BTN_TYPE_STOP);
            bool canAttack = false;
            bool canGather = true;
            for(auto unit:selectedArray){
                if(unit->unitType != UNIT_WORKER && unit->unitType != UNIT_GOBLIN_WORKER){
                    canGather = false;
                }
//                if (unit->unitType != UNIT_HELICOPTER) {
                    canAttack = true;
//                }
            }
            int noneFrom = 2;
            if (canGather && selectedArray.at(0)->alliSide == WHICH_SIDE_HERO) {
                selectedArray.at(0)->isGoingToBuild = false;
                HUD->setMenu(0, BTN_TYPE_BUILD);
                HUD->setMenu(1, BTN_TYPE_BUILD_BETTER);
            }else{
                noneFrom = 0;
            }
            for(int i = noneFrom; i < 6; i++){
                HUD->setMenu(i, BTN_TYPE_NONE);
            }
            
            HUD->arrangeMenu(selectedArray.at(0)->getPosition());
            if(selectedArray.size() == 1 && (selectedArray.at(0)->unitType == UNIT_WORKER || selectedArray.at(0)->unitType == UNIT_GOBLIN_WORKER)){
                selectedArray.at(0)->stop();
            }
        }
    }
}
void HelloWorld::resetTouchStart(float dt){
//    touchStart = Point(0, size.height);
}
void HelloWorld::setPlayer(Point pos){
    
    
}

void HelloWorld::getOffVehicle(){
   
}


//void HelloWorld::TouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
//{
//    Touch *touch = (Touch*)(touches.at(0));
//    Point location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);

//}

void HelloWorld::solveCollision(Movable* p, Point pos, Rect rect){
    
}
EnemyBase* HelloWorld::getEncounteredNPC(cocos2d::Point pos){
    for(auto npc: npcArray){
        if(npc->getBoundingBox().containsPoint(pos)){
            return npc;
        }
    }
    return nullptr;
}
void HelloWorld::showNPCEvent(int index){
    isInEvent = true;
    HUD->showEvent(index, false);
}
void HelloWorld::showNPCDisposableTalk(std::string talk){
    HUD->showSingleNPCMessage(talk);
//    HUD->showBlackTopAndBottom();
//    isInEvent = true;
//    showTalkText(talk, WHOSE_TALK_NPC);
//    float delay = 3;
//    imgTalkBox->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)), NULL));
//    sptPointer->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)), NULL));
}
float HelloWorld::checkBottom(Movable* p){
    Rect rect;
    Point pos;
    Point plPos;
    Point newPlPos;
    bool ground = false;
    bool escape = false;
    float temp = -1;
    // check bottom
    rect = p->collisionBoundingBox();
    int offset = 1;
    pos = rect.origin + Point(rect.size.width - offset, 0);
    do{
        if(pos.x <= rect.origin.x + offset){
            pos.x = rect.origin.x + offset;
            escape = true;
        }
        experimental::TMXTiledMap* map = getTileMap(pos);
//        if(map){
            plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid;
        tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
//            if(tgid > 0){
                Rect tileRect = tileRectFromTileCoords(plPos, map);
                //            tileRect.origin.y = floorf(tileRect.origin.y);
                //            rect.origin.y = floorf(rect.origin.y);
                Rect inter = intersection(tileRect, rect);
                //            inter.size.height = floorf(inter.size.height);
                if(p->velocity.y < 0){
                    if(inter.size.width > 0 && inter.size.height > 0 && p->getCurrentY() >= inter.origin.y + inter.size.height){
                        bool way = !isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos));
                        if (way || (decoLayer && isDecoBlock(((experimental::TMXLayer*)map->getChildByTag(TAG_DECO_LAYER))->getTileGIDAt(plPos)))){
                            temp = inter.size.height;
                            ground = true;
                            
                            if (p->onGround) {
                                int tgid;
                                if (p->isTowardLeft) {
                                    //                                plPos = getCoordinateFromPosition(rect.origin + Point(0, -5), map);
                                    tgid = getTileAtPosition(rect.origin + Point(0, -5), TAG_STAGE_LAYER, map);
                                }else{
                                    //                                plPos = getCoordinateFromPosition(rect.origin + Point(rect.size.width, -5), map);
                                    tgid = getTileAtPosition(rect.origin + Point(rect.size.width, -5), TAG_STAGE_LAYER, map);
                                }
                                
//                                if(!!isWay(tgid) && !isOneWay(tgid)){
//                                    p->encounteredCliff(p->isTowardLeft?CLIFF_LEFT:CLIFF_RIGHT);
//                                }
                            }
                            
                            break;
                        }
                    }
                }
//            }
//        }
        
        pos = pos - Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    //    return ground;
    return temp;
}
void HelloWorld::checkForAndResolveCollisions(Movable* p){
    Rect rect;
    Point pos;
    Point plPos;
    Point newPlPos;
    bool ground = false;
    bool escape = false;
    float tempTotal = 0;
    float temp = -1;
    justEncounteredNPC = nullptr;
    //    ground = checkBottom(p);
    temp = checkBottom(p);
    if(temp > 0){
        ground = true;
        tempTotal += temp;
    }
    // check top
    escape = false;
    int xOffset = 1;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(xOffset, rect.size.height);
    do{
        if(pos.x >= rect.origin.x + rect.size.width - xOffset){
            pos.x = rect.origin.x + rect.size.width - xOffset;
            escape = true;
        }
        
        experimental::TMXTiledMap* map = getTileMap(pos);
        if (map && p->velocity.y > 0) {
            plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
            int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
//            if(tgid > 0){
//                if(!isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos))){
            bool way = !isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos));
            if (way || (decoLayer && isDecoBlock(((experimental::TMXLayer*)map->getChildByTag(TAG_DECO_LAYER))->getTileGIDAt(plPos)))){
                    Rect tileRect = tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    
                    if(inter.size.width > 3 && inter.size.height > 0 && inter.size.width > inter.size.height){
                        
//                        if(p->velocity.y >= 0){
//                            //                            plPos = this->getCoordinateFromPosition(pos - Point(0, TILE_SIZE));
//                            //                            tgid = stageLayer->getTileGIDAt(plPos);
//                            //                            property = tileMap->getPropertiesForGID(tgid);
//                            if (!isWay(tgid)) {
//                                p->desiredPosition.y -= inter.size.height;
//                                p->velocity.y  = 0;
//                                break;
//                            }
//                        }
                        
                        p->desiredPosition.y -= inter.size.height;
                        p->velocity.y  = 0;
                        
                        break;
                    }
                }
//            }
            
        }
        
        pos = pos + Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Point(-xOffset, 1);
    do{
        if(pos.y >= rect.origin.y + rect.size.height -1){
            pos.y = rect.origin.y + rect.size.height -1;
            escape = true;
        }
//        if(escape) break;
        experimental::TMXTiledMap* map = getTileMap(pos);
        if (map && p->velocity.x < 0) {
            plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
            int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
//            if(!isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos))){
            bool way = !isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos));
            if (way || (decoLayer && isDecoBlock(((experimental::TMXLayer*)map->getChildByTag(TAG_DECO_LAYER))->getTileGIDAt(plPos)))){
                Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                Rect inter = intersection(tileRect, rect);
                if(inter.size.width > 0 && inter.size.height > 0) {
                    p->desiredPosition.x += inter.size.width;
                    
                    //                        ground = checkBottom(p);
                    temp = checkBottom(p);
                    if(tempTotal > 0 && temp < 0){
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
        pos = pos + Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check right
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Point(rect.size.width + xOffset, 1);
    do{
        if(pos.y >= rect.origin.y + rect.size.height){
            pos.y = rect.origin.y + rect.size.height;
            escape = true;
        }
//        if(escape) break;
        experimental::TMXTiledMap* map = getTileMap(pos);
        if (map && p->velocity.x > 0) {
            plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
            int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
//            if(tgid){
//                if (!isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos))) {
            bool way = !isWay(tgid) && !isHighWay(((experimental::TMXLayer*)map->getChildByTag(TAG_HIGH_STAGE_LAYER))->getTileGIDAt(plPos));
            if (way || (decoLayer && isDecoBlock(((experimental::TMXLayer*)map->getChildByTag(TAG_DECO_LAYER))->getTileGIDAt(plPos)))){
                    Rect tileRect = tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0){
                        p->desiredPosition.x -= inter.size.width;
                        
                        //                    ground = checkBottom(p);
                        temp = checkBottom(p);
                        if(tempTotal > 0 && temp < 0){
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
        
        pos = pos + Point(0, TILE_SIZE/2);
    }while(!escape);
    experimental::TMXTiledMap* map = getTileMap(pos);
    if (map) {
        if (((experimental::TMXLayer*)map->getChildByTag(TAG_FORE_LAYER))) {
            bool inWater = false;
            int tileId = getTileAtPosition(p->getPosition(), TAG_FORE_LAYER, map);
            if (tileId) {
                if(tileId == 0 || /*info->type.asString().compare("WaterSurface")*/tileId == 0){
                    inWater = true;
                }
            }
            
            if (inWater && !p->inWater) {
                spriteBatch->setLocalZOrder(-10);
                log("spriteBatch z order set %d", spriteBatch->getLocalZOrder());
                p->setInWater(true);
                //            p->setOnGround(false);
                inWaterArray.pushBack(p);
            }else if(!inWater && p->inWater){
                
                p->setInWater(false);
                
                inWaterArray.eraseObject(p);
            }
        }
    }
    if(justEncounteredNPC == nullptr && !isInEvent){ // to meet the same npc again
        encounteredNPC = nullptr;
    }
    
    if(ground){
        if(!p->onGround){
            p->onLanded();
        }
        tempTotal = floorf(tempTotal * 100) / 100;
        p->velocity.y = -0.001;
        p->desiredPosition.y += tempTotal;
    }
    
//    p->setOnGround(ground);
    p->setPosition( p->desiredPosition);
}
void HelloWorld::changeSprite(Ref* ref){
    Sprite* spt = (Sprite*)ref;
    spt->setSpriteFrame(spt->getName());
}
bool HelloWorld::isFregile(int index){
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
bool HelloWorld::isHighWay(int index){
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
bool HelloWorld::isWay(int index){
    return true;
    if(isOnFirstFloor){
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
    }else{
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
bool HelloWorld::isDecoBlock(int index){
    switch (index) {
        case 48:
        case 49:
            return true;
        default:
            return false;
    }
}
bool HelloWorld::isOneWay(int index){
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
bool HelloWorld::checkMovableMissileCollision(Movable* p){
    //    Vector<TileInfo*> tiles = getSurroundingTilesAtPosition(p->getGravityPosition(), stageLayer, p); //1
    /*if (tiles.size() < 1) {
     MovableMissileArray.eraseObject(p);
     heroMissileArray.eraseObject(p);
     enemyMissileArray.eraseObject(p);
     p->removeFromParentAndCleanup(true);
     CCLOG("tileSize is wrong");
     return true;
     }*/
    //    bool ground = false;
    //    Point current = p->getPosition();
    Rect pRect = p->collisionBoundingBox(); //2
    p->setPosition( p->desiredPosition);
    int gid;
    experimental::TMXTiledMap* map = getTileMap(p->getPosition());
    Point plPos = this->getCoordinateFromPosition(p->getPosition(), map); //1
    Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
    if(p->isFregile){
        gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
        if (gid) {
            if (!isWay(gid)) {
                MovableMissileArray.eraseObject(p);
                heroMissileArray.eraseObject(p);
                enemyMissileArray.eraseObject(p);
                enemyArray.eraseObject((EnemyBase*)p);
                p->removeFromParent();
                return true;
            }
            
        }
    }else{
        if(p->velocity.y < 0){
            if(!isWay(gid)){
                p->velocity.y = 0;
                Rect intersectionR = intersection(pRect, tileRect);
                if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                    p->setPosition(p->getPosition() + Point(0, intersectionR.size.height));
                }
            }
        }
        if(p->velocity.y > 0){
            p->velocity.y = -0.0021;
            Rect intersectionR = intersection(pRect, tileRect);
            if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                p->setPosition(p->getPosition() + Point(0, -intersectionR.size.height));
            }
        }
        
        
        if (p->velocity.x > 0) {
            gid = getTileAtPosition(p->getPosition() + Point(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
        }else if(p->velocity.x < 0){
            gid = getTileAtPosition(p->getPosition() - Point(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
        }
        if (!isWay(gid)) {
            MovableMissileArray.eraseObject(p);
            heroMissileArray.eraseObject(p);
            enemyMissileArray.eraseObject(p);
            enemyArray.eraseObject((EnemyBase*)p);
            p->removeFromParent();
            return true;
        }
    }
    return false;
}

bool HelloWorld::intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect)
{
    return !(srcRect.getMaxX() <= dstRect.getMinX() ||
             dstRect.getMaxX() <= srcRect.getMinX() ||
             srcRect.getMaxY() <= dstRect.getMinY() ||
             dstRect.getMaxY() <= srcRect.getMinY());
}
Rect HelloWorld::intersection(Rect source, Rect rect)
{
    Rect intersection;
    intersection = Rect(source.getMinX()>rect.getMinX()?source.getMinX():rect.getMinX(),
                        source.getMinY()>rect.getMinY()?source.getMinY():rect.getMinY(),
                        0,
                        0);
    intersection.size.width = (source.getMaxX()<rect.getMaxX()?source.getMaxX():rect.getMaxX())-intersection.getMinX();
    intersection.size.height = (source.getMaxY()<rect.getMaxY()?source.getMaxY():rect.getMaxY())-intersection.getMinY();
    return intersection;
}
void HelloWorld::checkForAndResolveCollisionsForWater(Movable* p){
    
    Point current = p->getGravityPosition();
    experimental::TMXTiledMap* map = getTileMap(current);
    Rect pRect = p->collisionBoundingBox();
    int gid;
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            current = current + Point(0, -TILE_SIZE);
        }else if(i == 1) {
            current = current + Point(0, TILE_SIZE);
        }else if(i == 2) {
            current = current + Point(-TILE_SIZE, 0);
        }else if(i == 3) {
            current = current + Point(TILE_SIZE, 0);
        }else if(i == 4) {
            current = current + Point(-TILE_SIZE, -TILE_SIZE);
        }else if(i == 5) {
            current = current + Point(TILE_SIZE, -TILE_SIZE);
        }else if(i == 6) {
            current = current + Point(-TILE_SIZE, TILE_SIZE);
        }else if(i == 7) {
            current = current + Point(TILE_SIZE, TILE_SIZE);
        }
        
        gid = getTileAtPosition(current, TAG_STAGE_LAYER, map);
        if (gid) {
            Point plPos = this->getCoordinateFromPosition(current - map->getPosition(), map); //1
            Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
            
            Rect intersectionR = intersection(pRect, tileRect);
            if (intersectionR.size.width > 0 && intersectionR.size.height > 0){
                
                if (!isWay(gid)) {
                    //                    int tileIndx = tiles->indexOfObject(dic);
                    int tileIndx = i;
                    
                    if (tileIndx == 0 && p->velocity.y <= 0) {
                        //tile is directly below Hero
                        p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                        p->velocity = Point(p->velocity.x, 0);
                        //                        p->setOnGround(true);
                        
                    } else if (tileIndx == 1) {
                        //tile is directly above Hero
                        p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                        p->velocity = Point(p->velocity.x, 0.0);
                    } else if (tileIndx == 2) {
                        //tile is left of Hero
                        p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                        
                    } else if (tileIndx == 3) {
                        //tile is right of Hero
                        p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                    } else {
                        if (p->velocity.y > 0) {    // rasing
                            if (tileIndx > 5) {
                                p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                                p->velocity = Point(p->velocity.x, 0.0);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                            }
                        }else{  // falling
                            if (tileIndx > 5) {
                                p->desiredPosition = Point(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                                p->velocity = Point(p->velocity.x, 0);
                                //                                p->setOnGround(true);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Point(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Point(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (getTileAtPosition(p->desiredPosition, TAG_FORE_LAYER, map) > 0) { // check if out of water
        p->desiredPosition = p->getPosition();
    }
    
    p->setPosition(p->desiredPosition);
}

void HelloWorld::checkForAndResolveCollisionsForBouncing(Movable* p){
    Rect rect;
    Point pos;
    Point plPos;
    bool ground = false;
    
    bool escape = false;
    
    
    //    p->setOnGround(false);
    // check top
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(0, rect.size.height);
    experimental::TMXTiledMap* map;
    do{
        if(pos.x >= rect.origin.x + rect.size.width){
            pos.x = rect.origin.x + rect.size.width; escape = true;
        }
        
        map = getTileMap(pos);
        plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0){
            Value property = map->getPropertiesForGID(tgid);
            if (property.getType() != Value::Type::NONE) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 3 && inter.size.height > 0 && inter.size.width > inter.size.height){
                        if(p->velocity.y > 0){
                            p->desiredPosition.y -= inter.size.height;
                            p->velocity.y  = 0;
                            break;
                        }
                    }
                }
            }
        }
        pos = pos + Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Point(0, rect.size.height);
    do{
        if(pos.y <= rect.origin.y){
            break;//pos.y = rect.origin.y + rect.size.height; escape = true;
        }
        map = getTileMap(pos);
        plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0 && p->velocity.x <= 0){
            Value property = map->getPropertiesForGID(tgid);
            if(property.getType() != Value::Type::NONE && property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                if (property.getType() != Value::Type::NONE) {
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0) {
                        p->desiredPosition.x += inter.size.width;
                        
                        if(p->isTowardLeft ){
                            p->encounteredWall();
                        }
                        
                        break;
                    }
                }
            }
        }
        pos = pos - Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check right
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + rect.size;
    do{
        if(pos.y <= rect.origin.y){
            break;//pos.y = rect.origin.y; escape = true;
        }
        map = getTileMap(pos);
        plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0 && p->velocity.x >= 0){
            Value property = map->getPropertiesForGID(tgid);
            if (property.getType() != Value::Type::NONE) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    Rect inter = intersection(tileRect, rect);
                    if(inter.size.width > 0 && inter.size.height > 0){
                        p->desiredPosition.x -= inter.size.width;
                        
                        if(!p->isTowardLeft ){
                            p->encounteredWall();
                        }
                        break;
                    }
                }
            }
        }
        
        pos = pos - Point(0, TILE_SIZE/2);
    }while(!escape);
    
    // check bottom
    rect = p->collisionBoundingBox();
    int offset = 2;
    pos = rect.origin + Point(rect.size.width - offset, 0);
    do{
        if(pos.x <= rect.origin.x + offset){
            pos.x = rect.origin.x;
            escape = true;
        }
        map = getTileMap(pos);
        plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0){
            Rect tileRect = this->tileRectFromTileCoords(plPos, map);
            Rect inter = intersection(tileRect, rect);
            
            if(p->velocity.y < 0){
                if(inter.size.width > 0 && inter.size.height > 0 && p->getCurrentY() >= inter.origin.y + inter.size.height){
                    if (tgid > 0) {
                        if(isOneWay(tgid)){
                            if((int)p->oneWayCancelY == inter.origin.y + inter.size.height){
                                p->setOnGround(false);
                                
                                break;
                            }
                            lastClearY = rect.origin.y;
                        }
                        
                        //                            p->desiredPosition.y += inter.size.height;
                        //                        temp = inter.size.height;
                        p->desiredPosition.y += inter.size.height;
                        //                        p->setOnGround(true);
                        //                        p->velocity.y = 0;
                        p->velocity = Point(p->velocity.x*0.8, -p->velocity.y*0.6);
                        
                        
                        break;
                    }
                }
            }
            
        }
        pos = pos - Point(TILE_SIZE/2, 0);
    }while(!escape);
    
    p->setPosition( p->desiredPosition);
}

bool HelloWorld::checkForAndResolveCollisionsForMissile(Movable* p){
    p->setPosition( p->desiredPosition); //7
    
    experimental::TMXTiledMap* map = getTileMap(p->getPosition());
    int gid = 0;
    if (map != nullptr) {
        gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
    }
    
    if(map == nullptr || !isWay(gid)){
        /*if (p->velocity.x > 0){
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL,
         p->getPosition());//Point(intersectionR.origin.x, intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
         }else{
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL_LEFT,
         p->getPosition());//Point(intersectionR.origin.x + intersectionR.size.width,intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
         }*/
        runEffect(EFFECT_BULLET_WALL_SMALL, p->getPosition(), p->getRotation());
        
        bulletWasted = true;
        
        if(heroMissileArray.find(p) != heroMissileArray.end()){
            heroMissileArray.eraseObject(p);
            
        }
        if (enemyMissileArray.find(p) != enemyMissileArray.end()) {
            enemyMissileArray.eraseObject(p);
        }
        
        if(p->getTag() == UNIT_MISSILE_STRAIGHT) straightMovingArray.eraseObject(p);
        if(p->getTag() == UNIT_MISSILE_CHASING){
            chasingMissileArray.eraseObject(p);
        }
        if(p->getTag() == UNIT_MISSILE_CUSTOM){
            //                            customMovingArray.eraseObject(p);
            missileMoveDone(p);
        }
        if(p->getTag() == UNIT_MISSILE_Movable) MovableMissileArray.eraseObject(p);
        
        enemyArray.eraseObject((EnemyBase*)p);
        if(spriteBatch->getChildren().find(p) != spriteBatch->getChildren().end()){
            spriteBatch->removeChild(p, true);
        }
        if(spriteBatchBuilding->getChildren().find(p) != spriteBatchBuilding->getChildren().end()){
            spriteBatchBuilding->removeChild(p, true);
        }
        return true;
    }
    
    return false;
}
void HelloWorld::unscheduleAll(){
    this->unscheduleAllCallbacks();
}

void HelloWorld::showSuccessLayer(float dt)
{
    if (collectedCoinCount < MAX_COIN_COUNT) {
        GameManager::getInstance()->addCoin(collectedCoinCount);
    }
    //    GameManager::getInstance()->showInterstitialAds();
    
    isGameOver = true;
    CCLOG("star count: %d", starCount);
    
    char buf[50];
    const char* totalStarCountKey = "Tt_SC";
    int totalStarCount = UserDefault::getInstance()->getIntegerForKey(totalStarCountKey, 0);
    
    sprintf( buf, "T%dS%dC", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
    int previousStar = UserDefault::getInstance()->getIntegerForKey(buf, 0);
    if (starCount > previousStar) {
        int starToAdd = starCount - previousStar;
        
        sprintf( buf, "T%dS%dC", GameManager::getInstance()->theme, GameManager::getInstance()->currentStageIndex);
        UserDefault::getInstance()->setIntegerForKey(buf, starCount);
        
        totalStarCount += starToAdd;
        UserDefault::getInstance()->setIntegerForKey(totalStarCountKey, totalStarCount);
    }
    
    totalStarCount = GameManager::getInstance()->getTotalStarCount();
    
    if(starCount >= 3){
        GameManager::getInstance()->addMastery(GameManager::getInstance()->theme*24 + GameManager::getInstance()->currentStageIndex);
    }
    
    GameManager::getInstance()->getHudLayer()->setVisible(true);
    activityIndicator = Sprite::create("sandglass.png");
    GameManager::getInstance()->getHudLayer()->addChild(activityIndicator);
    activityIndicator->setPosition(Point(size.width/2, size.height/2));
    activityIndicator->runAction(RepeatForever::create(RotateBy::create(1, 360)));
    GameManager::getInstance()->saveCoin();
    if (starCount >= 3) {
        showStageClearLayer(true);
    }else{
        showStageClearLayer(false);
    }
}

void HelloWorld::showStageClearLayer(bool threeStars){
    activityIndicator->setVisible(false);
    activityIndicator->stopAllActions();
    activityIndicator->removeFromParentAndCleanup(true);
    
    if (GameManager::getInstance()->life >= 0 || UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
        if (!UserDefault::getInstance()->getBoolForKey(KEY_INFINITY_LIFE_GET, false)) {
            GameManager::getInstance()->life++;
            UserDefault::getInstance()->setIntegerForKey(KEY_LIFE, GameManager::getInstance()->life);
        }
    }
    
    if (enemyKilledCount > enemyMaxCount) {
        enemyKilledCount = enemyMaxCount;
    }
}
void HelloWorld::rateResult(Node* node){
    
}
void HelloWorld::onUpdateResultErrorComplete(char const *status, char const *error) {
    CCLOG("Update result failed. Status: %s, Error: %s", status, error);
    activityIndicator->setVisible(false);
    activityIndicator->stopAllActions();
    activityIndicator->removeFromParentAndCleanup(true);
    
}

void HelloWorld::gameClearAnimationDone(Ref* obj)
{
    //    CCLOG("Show stage clear popup");
}


int HelloWorld::getMaxLoadedBulletCount(int weapon){
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
int HelloWorld::getMaxTotalBulletCount(int weapon){
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
void HelloWorld::playReloadingSoundLater(float dt){
    
}
//int testWeaponIndex = WEAPON_LASER_GUN - 1;
void HelloWorld::reload(){
    
}
void HelloWorld::fire()
{
    
}
void HelloWorld::missileEffectUpdate(float dt){
    missileEffectCollapsedTime += dt;
    if(missileEffectCollapsedTime < 0.053){
        return;
    }
    missileEffectCollapsedTime = 0;
    Vector<Movable*> doneArray;
    bool checkChange = false;
    do
    {
        checkChange = false;
        for(auto drop: heroMissileArray){
            if(drop->effectType == MISSILE_EFFECT_SMOKE)
                runEffect(EFFECT_SMOKE, drop->getPosition());
            if(drop->effectType == MISSILE_EFFECT_RED_BALL)
                runEffect(EFFECT_EXPLODE_SMALL, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_GREEN_SMOKE)
                runEffect(EFFECT_GREEN_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_RED_SMOKE)
                runEffect(EFFECT_RED_SMOKE, drop->getPosition());
            else if(drop->effectType == MISSILE_EFFECT_SHARK_GUN){
                
            }
        }
    }while(checkChange);
    doneArray.clear();
}
void HelloWorld::missileMoveDone(Ref* obj)
{
    EnemyBase* missile = (EnemyBase*)obj;
    heroMissileArray.eraseObject(missile);
    enemyMissileArray.eraseObject(missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void HelloWorld::fireBulletMoveDone(Ref* obj)
{
    Movable* missile = (Movable*)obj;
    heroMissileArray.eraseObject(missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void HelloWorld::enemyBaseMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    EnemyBase* sprite = (EnemyBase*)obj;
    sprite->stopAllActions();
    enemyArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}
void HelloWorld::coinWaitDone(Ref* obj){
    Movable* drop = (Movable*)obj;
    coinArray.eraseObject(drop);
    MovableCoinArray.eraseObject(drop);
    drop->runAction(Sequence::create(FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(HelloWorld::removeFromParentAndCleanup, drop)), NULL));
}
void HelloWorld::spriteMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Sprite* sprite = (Sprite*)obj;
    sprite->stopAllActions();
    sprite->removeFromParentAndCleanup(true);
}
void HelloWorld::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Label* lbl = (Label*)obj;
    
    lbl->stopAllActions();
    lbl->removeFromParentAndCleanup(true);
}


void HelloWorld::blinkingPlayer(float dt)
{
    blinkingTime -= dt;
    
}

void HelloWorld::pauseLayer()
{
    for(auto spt: enemyArray)
    {
        spt->pause();
    }
    
    for(auto spt: heroMissileArray)
    {
        spt->pause();
    }
    
    for(auto spt: enemyMissileArray)
    {
        spt->pause();
    }
    
    for(auto spt: dropItemArray)
    {
        spt->pause();
    }
    for(auto spt: MovableArray)
    {
        spt->pause();
        spt->pauseProcess();
    }
    for(auto spt: heroArray)
    {
        spt->pause();
        spt->pauseProcess();
    }
    
    
    
    isPaused = true;
//    this->pause();
}

void HelloWorld::resumeLayer()
{
    for(auto spt: enemyArray)
    {
        spt->resume();
    }
    
    for(auto spt: heroMissileArray)
    {
        spt->resume();
    }
    
    for(auto spt: enemyMissileArray)
    {
        spt->resume();
    }
    
    for(auto spt: dropItemArray)
    {
        spt->resume();
    }
    for(auto spt: MovableArray)
    {
        spt->resume();
        spt->resumeProcess();
    }
    for(auto spt: heroArray)
    {
        spt->resume();
        spt->resumeProcess();
    }
    
    isPaused = false;
//    this->resume();
}

void HelloWorld::refreshPlayerInfo(int costum, int weapon)
{
    
}


void HelloWorld::receivingData(float dt){
    
}
void HelloWorld::removeCharacter(EnemyBase* unit){
    if(unit->sptWeapon != nullptr){
        unit->sptWeapon->removeFromParent();
    }
    if (unit->ndLevelCircle) {
        unit->ndLevelCircle->removeFromParent();
    }
    MovableArray.eraseObject(unit);
    heroArray.eraseObject(unit);
    enemyArray.eraseObject(unit);
    unit->removeFromParent();
}
void HelloWorld::showTalkText(std::string text, int whoseTalk, std::string npcName){
    talkText = text;
    talkIndex = 0;
    if(imgTalkBox != nullptr){
        imgTalkBox->removeFromParent();
        sptPointer->removeFromParent();
    }
    imgTalkBox = ImageView::create("talkBoxFrame.png", ImageView::TextureResType::PLIST);
    imgTalkBox->setAnchorPoint(Point(0.5, 0));
    float splitPadding = 10;
    imgTalkBox->setCapInsets(Rect(splitPadding, splitPadding, imgTalkBox->getContentSize().width - splitPadding*2, imgTalkBox->getContentSize().height - splitPadding*2));
    imgTalkBox->setScale9Enabled(true);
    imgTalkBox->setContentSize(Size(100, 10));
    this->addChild(imgTalkBox, 100);
    
    sptPointer = Sprite::createWithSpriteFrameName("talkBoxPointer.png");
    this->addChild(sptPointer, 100);
    lblTalk = LanguageManager::getInstance()->getLocalizedLabel();
    lblTalk->setTextColor(Color4B::WHITE);//(78, 78, 78, 255));
    EnemyBase* finder;
    talkingNpcName = npcName;
//    talkingNPC = nullptr;
    if(whoseTalk == WHOSE_TALK_HERO){
//        imgTalkBox->setPosition(hero->getPosition() + Point(0, 100));
        talkingNPC = finder;
    }else if(whoseTalk == WHOSE_TALK_NPC){
        Point npcPos;
        if(npcName.size() > 0){
            for(auto npc: npcArray){
                if(npc->getName().compare(npcName) == 0){
                    npcPos = Point(npc->getPositionX(), npc->getBoundingBox().getMaxY());
                    talkingNPC = npc;
                    break;
                }
            }
        }else{
            npcPos = Point(encounteredNPC->getPositionX(), encounteredNPC->getBoundingBox().getMaxY());
        }
        imgTalkBox->setPosition(npcPos + Point(0, 5));
//        imgTalkBox->setColor(Color3B(188, 188, 188));
//        sptPointer->setColor(Color3B(188, 188, 188));
        lblTalk->setTextColor(Color4B::BLACK);
    }

    
    sptPointer->setAnchorPoint(Point(0.5, 1));
    sptPointer->setPosition(imgTalkBox->getPosition() + Point(0, 18));
    

    lblTalkShadow = LanguageManager::getInstance()->getLocalizedLabel();
    lblTalkShadow->setTextColor(Color4B(178, 78, 78, 255));
    
    bool isYell = false;
    if(talkText.find("(yell)") != std::string::npos){
        isYell = true;
        talkText = talkText.substr(0, talkText.find("(yell)"));
    }
    if (isYell) {
        GM->shakeIt(imgTalkBox, 12, 10);
        imgTalkBox->setScale(1.4f);
        isTalkBoxInCustomMoving = true;
    }else{
        isTalkBoxInCustomMoving = false;
    }
    // resize talkbox
    lblTalkShadow->setString(talkText);
    Size talkSize = Size(lblTalkShadow->getContentSize().width*lblTalk->getScale() + 10, lblTalkShadow->getContentSize().height*lblTalk->getScale());
    float width = 600;
    if(talkSize.width > width){
        lblTalkShadow->setWidth(width/lblTalkShadow->getScale());
    }else{
        lblTalkShadow->setWidth(talkSize.width/lblTalkShadow->getScale());
    }
    lblTalk->setWidth(lblTalkShadow->getWidth());
    talkSize = Size(lblTalkShadow->getContentSize().width*lblTalk->getScale(), lblTalkShadow->getContentSize().height*lblTalk->getScale());
    float padding = 30;
    int talkBoxWidth = talkSize.width + padding;
    int minTalkBoxWidth = 300;
    if(talkBoxWidth < minTalkBoxWidth){
        talkBoxWidth = minTalkBoxWidth;
    }
    imgTalkBox->setContentSize(Size(talkBoxWidth, talkSize.height + padding));
    lblTalk->setPosition(Point(imgTalkBox->getContentSize().width/2, padding/2 + talkSize.height/2));
    lblTalkShadow->setPosition(Point(imgTalkBox->getContentSize().width/2, padding/2 + talkSize.height/2));
    // resize talkbox end
    
    imgTalkBox->addChild(lblTalk);
    
    this->schedule(schedule_selector(HelloWorld::talkBoxUpdate));
    
    // event script
    if(HUD->eventIndex == 0){
        if(HUD->talkIndex == 15){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    float moveDistance = 3.0f;
                    unit->runAction(Sequence::create(MoveBy::create(5, Point(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)), CallFunc::create(CC_CALLBACK_0(HelloWorld::onUndeadMoveDone, this)), nullptr));
                    imgTalkBox->runAction(MoveBy::create(5, Point(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)));
                    sptPointer->runAction(MoveBy::create(5, Point(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)));
                    unit->runAnimation("wizardRun", true);
                }
            }
        }else if( HUD->talkIndex >= 16 && HUD->talkIndex <= 21){ // create portal for john event
            int counter = 0;
            log("talkIndex: %d", HUD->talkIndex);
            for(auto unit:unitsToCreateArray){
                if (counter%6 == HUD->talkIndex - 16 && npcArray.find(unit) == npcArray.end()) {
                    unit->stopAllActions();
                    unit->runAnimation(unit->isEnemy?"zombie orc axeStand":"zombie swordmanStand", true);
                }
                counter++;
            }
        }else if(HUD->talkIndex == 28){
            for(auto unit:npcArray){
                if(unit->getName().compare("hero") == 0){
                    unit->setFlippedX(true);
                }
            }
        }else if(HUD->talkIndex == 29){
            for(auto unit:npcArray){
                if(unit->getName().compare("orc") == 0){
                    unit->setFlippedX(false);
                }
            }
        }else if(HUD->talkIndex == 30){
            for(auto unit:npcArray){
                if(unit->getName().compare("hero") == 0){
                    unit->setFlippedX(false);
                }
                if(unit->getName().compare("orc") == 0){
                    unit->setFlippedX(true);
                }
            }
        }
    }else if(HUD->eventIndex == 1){
        if(HUD->talkIndex == 0){
            int moveTileCount = 12;
            int dur = 8;
            for(auto unit:unitsToCreateArray){
                if(unit->alliSide == WHICH_SIDE_HERO){
                    npcArray.pushBack(unit);
                    std::string name = unit->getName();
                    if(name.compare("hero") == 0){
                        name = "swordman";
                    }
                    unit->runAnimation(strmake("%sRun", name.c_str()).c_str(), true);
                    unit->runAction(Sequence::create(MoveBy::create(dur, Point(-TILE_SIZE*moveTileCount, -TILE_SIZE*moveTileCount)), CallFunc::create(CC_CALLBACK_0(HelloWorld::onNPCMoveDone, this)), nullptr));
                }
            }
            for(auto unit:npcArray){
                if(unit->getName().compare("orc") != 0){
                    if(unit->getName().compare("undead") == 0){
                        unit->runAnimation("wizardRun", true);
                        unit->runAction(Sequence::create(MoveBy::create(dur, Point(-TILE_SIZE*(moveTileCount-2), -TILE_SIZE*(moveTileCount-2))), CallFunc::create(CC_CALLBACK_0(HelloWorld::onUndeadMoveDone, this)), nullptr));
                    }
                    else if(unit->alliSide == WHICH_SIDE_HERO){
//                        unit->runAnimation(strmake("%sRun", unit->getName().c_str()).c_str(), true);
//                        unit->runAction(Sequence::create(MoveBy::create(5, Point(-TILE_SIZE*4, -TILE_SIZE*4)), CallFunc::create(CC_CALLBACK_0(HelloWorld::onNPCMoveDone, this)), nullptr));
                    }
                }
            }
            isCameraInCustomMoving = true;
            this->runAction(MoveBy::create(dur, Point(TILE_SIZE*moveTileCount, TILE_SIZE*moveTileCount)));
        }else if(HUD->talkIndex == 7){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    unit->setFlippedX(true);
                }
            }
        }else if(HUD->talkIndex == 14){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    unit->setFlippedX(false);
                }
            }
        }else if(HUD->talkIndex == 17){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    unit->setFlippedX(true);
                }
            }
        }else if( HUD->talkIndex >= 16 && HUD->talkIndex <= 21){ 
            int counter = 0;
            log("talkIndex: %d", HUD->talkIndex);
            for(auto unit:unitsToCreateArray){
                if (counter%6 == HUD->talkIndex - 16 && npcArray.find(unit) == npcArray.end()) {
                    unit->stopAllActions();
                    unit->runAnimation(unit->isEnemy?"zombie orc axeStand":"zombie swordmanStand", true);
                }
                counter++;
            }
        }else if(HUD->talkIndex == 26){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
                    runEffect(EFFECT_EXPLODE_MIDDLE, unit->getPosition());
                    unit->setVisible(false);
                }
            }
        }
    }
}
void HelloWorld::onNPCMoveDone(){
    for(auto unit:npcArray){
        if(unit->getName().compare("hero") == 0){
            unit->runAnimation("swordmanStand", true);
        }else if(unit->getName().compare("archer") == 0){
            unit->runAnimation("archerStand", true);
        }
    }
}
void HelloWorld::onUndeadMoveDone(){
    for(auto unit:npcArray){
        if(unit->getName().compare("undead") == 0){
            unit->runAnimation("wizardStand", true);
        }
    }
}
void HelloWorld::changeToSuperAdin(){
    this->unschedule(schedule_selector(HelloWorld::creatingStarToGreatBall));
    greatBall->removeFromParent();
    addSuperEffectToAdin();
}
void HelloWorld::addSuperEffectToAdin(){
    
}

void HelloWorld::talkBoxUpdate(float dt){
    for(auto npc: npcArray){
        if(npc->getName().compare(talkingNpcName) == 0 && HUD->talkIndex < 22 && !isTalkBoxInCustomMoving){
            Point npcPos = Point(npc->getPositionX(), npc->getBoundingBox().getMaxY());
            imgTalkBox->setPosition(npcPos + Point(0, 5));
            sptPointer->setPosition(imgTalkBox->getPosition() + Point(0, 18));
            break;
        }
    }
    if(talkIndex >= talkText.size()){
        if(imgTalkBox->getChildByName("TOUCH") == nullptr){
//            Sprite* sptAPress = Sprite::createWithSpriteFrameName("aPressed.png");
//            imgTalkBox->addChild(sptAPress);
//            sptAPress->setName("aPress");
//            GameManager::getInstance()->runAnimation(sptAPress, "aPressAni", true);
//            sptAPress->setPosition(Point(imgTalkBox->getContentSize().width - 5, -5));
            
            Label* lbl = LM->getLocalizedLabel(LM->getText("next").c_str(), Color4B::WHITE);
            lbl->enableShadow();
            imgTalkBox->addChild(lbl);
            lbl->setName("TOUCH");
            lbl->setVisible(false);
            lbl->setColor(Color3B::WHITE);
            lbl->setPosition(Point(imgTalkBox->getContentSize().width - lbl->getContentSize().width/2, -lbl->getContentSize().height/2));
            lbl->runAction(RepeatForever::create(Blink::create(1, 1)));
            lblTalk->setString(talkText);
            return;
        }
    }
    std::string strPre = lblTalk->getString();
    lblTalk->setString(talkText.substr(0, talkIndex));
    if(lblTalk->getContentSize().width == 0){
        lblTalk->setString(strPre);
    }
    talkIndex++;
}
void HelloWorld::endEvent(){
    isCameraInCustomMoving = false;
    extraCameraPos = Point(0, 0);
    this->unschedule(schedule_selector(HelloWorld::talkBoxUpdate));
    if(imgTalkBox != nullptr){
        imgTalkBox->removeFromParent();
        imgTalkBox = nullptr;
        sptPointer->removeFromParent();
    }
    
    isInEvent = false;
//    updateNPCQuestState(encounteredNPC);
    this->unschedule(schedule_selector(HelloWorld::eventUpdate));
    
}
int HelloWorld::getHeroLevel(int slot){
    return UD->getIntegerForKey(strmake(KEY_HERO_LEVEL_FORMAT, slot).c_str(), 0);
}
void HelloWorld::setHeroLevel(int slot, int level){
    UD->setIntegerForKey(strmake(KEY_HERO_LEVEL_FORMAT, slot).c_str(), level);
}
int HelloWorld::getHeroExp(int slot){
    return UD->getIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), 0);
}
void HelloWorld::setHeroExp(int slot, int exp){
    UD->setIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), exp);
}
void HelloWorld::addHeroExp(int slot, int exp){
    int currentExp = getHeroExp(slot);
    currentExp += exp;
    int previousLevel = getHeroLevel(slot);
    int level = getHeroLevel(slot);
    int maxExp =  getMaxExp(level);
    std::string tier = UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0");
    std::string maxLevel = getEvolutionStat(tier, "max level");
    if(level >= Value(maxLevel).asInt()){
        if(currentExp >= maxExp){
            currentExp = maxExp - 1;
        }
        setHeroExp(slot, currentExp);
        return;
    }
    while(true){
        level = getHeroLevel(slot);
        maxExp =  getMaxExp(level);
        if(currentExp >= maxExp){
            currentExp -= maxExp;
            setHeroLevel(slot, level + 1);
        }else{
            break;
        }
    }
    int currentLevel = getHeroLevel(slot);
    EnemyBase* hero;
    if (currentLevel > previousLevel) {
        for (int i = 0; i < currentLevel - previousLevel; i++) {
            hero->showEffect(EFFECT_BOTTOM_RECT_GLOW, 0 + i*1);
            hero->showEffect(EFFECT_WING, 0.1f + i*1);
            hero->showEffect(EFFECT_RISING_PARTICLE, 0.2 + i*1);
            hero->showEffect(EFFECT_PARTICLE_TORNADO, 0.1 + i*1);
            showLabelFromPool(WORLD, this->getPosition() + Point(0, 10), "LEVEL UP", 15, 0.2f + i*1);
        }
        setHeroExp(0, 0);
        hero->maxEnergy = getHeroMaxHP(0);
        hero->energy = hero->maxEnergy;
        HUD->setEnergy(hero->energy, hero->maxEnergy);
    }
    
    HUD->setExp(getHeroLevel(slot), getHeroExp(slot), getMaxExp(getHeroLevel(slot)));
    UD->setIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), currentExp);
}
int HelloWorld::getMaxExp(int level){
    if(level < 10){
        return 30*(level + 1);
    }else if(level < 30){
        return 30*(level + 1) + 1247*(level - 10);
    }else if(level < 40){
        return 30*(level + 1) + 1247*20 + 1247*(level - 30)*5;
    }else{
        return 30*(level + 1) + 1247*20 + 1247*10*5 + 10247*3*(level - 40 + 10);
    }
}
int HelloWorld::getHeroMaxLevel(int slot){
    std::string tier = UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0");
    std::string maxLevel = getEvolutionStat(tier, "max level");
    return Value(maxLevel).asInt();
}
std::string HelloWorld::getHeroName(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_NAME_FORMAT, slot).c_str(), "");
}
int HelloWorld::getHeroMaxHP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
//    return getUnitMaxHP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getHelmetStat(getHelmet(slot), "hp");
    return 60;
}
int HelloWorld::getHeroAP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
//    return getUnitAP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getWeaponStat(getWeapon(slot), "ap") + getHelmetStat(getHelmet(slot), "ap");
    return 10;
}
int HelloWorld::getHeroSP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
//    return getUnitSP(getHeroName(slot))*(1 + 0.1f*tier) + getShoesStat(getShoes(slot), "sp");
    return 25;
}
int HelloWorld::getHeroDP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
//    return getUnitDP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getHelmetStat(getHelmet(slot), "dp") + getShieldStat(getShield(slot), "dp");
    return 0;
}
int HelloWorld::getHeroGP(int slot){
    return getShieldStat(getShield(0), "gp");
}
void HelloWorld::updatePlayerStat(){
    
}
std::string HelloWorld::getHeroWeapon(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, slot).c_str(), "sword");
}
void HelloWorld::setHeroWeapon(int slot, std::string weapon){
    UD->setStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, slot).c_str(), weapon);
}
std::string HelloWorld::getHeroHelmet(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, slot).c_str(), "");
}
void HelloWorld::setHeroHelmet(int slot, std::string helmet){
    UD->setStringForKey(strmake(KEY_HERO_HELMET_FORMAT, slot).c_str(), helmet);
}
std::string HelloWorld::getHeroShield(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, slot).c_str(), "");
}
void HelloWorld::setHeroShield(int slot, std::string shield){
    UD->setStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, slot).c_str(), shield);
}
std::string HelloWorld::getHeroShoes(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, slot).c_str(), "");
}
void HelloWorld::setHeroShoes(int slot, std::string shoes){
    UD->setStringForKey(strmake(KEY_HERO_SHOES_FORMAT, slot).c_str(), shoes);
}
int HelloWorld::getUnitStat(std::string unitName, std::string stat){
    if(unitStatTable.find(unitName) == unitStatTable.end()){
        return -1;
    }
    ValueMap row = unitStatTable[unitName].asValueMap();
    return Value(row.at(stat)).asInt();
}
std::string HelloWorld::getUnitDropItem(std::string unitName, int dropIndex){
    if(unitStatTable.find(unitName) == unitStatTable.end()){
        return "";
    }
    ValueMap row = unitStatTable[unitName].asValueMap();
    return Value(row.at(strmake("drop %d", dropIndex))).asString();
}
int HelloWorld::getWeaponStat(std::string name, std::string stat){
    if(name.size() == 0 || weaponStatTable[name].asValueMap().find(stat) == weaponStatTable[name].asValueMap().end()) return 0;
    return Value(weaponStatTable[name].asValueMap().at(stat)).asInt();
}
std::string HelloWorld::getItemStat(int itemType, std::string name, std::string stat){
    ValueMap valueMap;
    if(itemType == ITEM_TYPE_ETC){
        valueMap = itemStatTable[name].asValueMap();
    }else if(itemType == ITEM_TYPE_SHIELD){
        valueMap = shieldStatTable[name].asValueMap();
    }else if(itemType == ITEM_TYPE_WEAPON){
        valueMap = weaponStatTable[name].asValueMap();
    }else if(itemType == ITEM_TYPE_HELMET){
        valueMap = helmetStatTable[name].asValueMap();
    }else if(itemType == ITEM_TYPE_SHOES){
        valueMap = shoesStatTable[name].asValueMap();
    }
    if(valueMap.find(stat) == valueMap.end()){
        return "";
    }else{
        return Value(valueMap.at(stat)).asString();
    }
}
std::string HelloWorld::getEvolutionStat(std::string name, std::string stat){
    ValueMap valueMap = evolutionTable[name].asValueMap();
    if(valueMap.find(stat) == valueMap.end()){
        return "";
    }else{
        return Value(valueMap.at(stat)).asString();
    }
}
std::string HelloWorld::getItemStat(std::string name, std::string stat){
    int itemType = getItemType(name);
    return getItemStat(itemType, name, stat);
//    if(name.size() == 0) return 0;
//    return Value(itemStatTable[name].asValueMap().at(stat)).asInt();
}
int HelloWorld::getItemPrice(int itemType, std::string name){
    if(itemType == ITEM_TYPE_ETC){
        return Value(itemStatTable[name].asValueMap().at("price")).asInt();
    }else if(itemType == ITEM_TYPE_SHIELD){
        return Value(shieldStatTable[name].asValueMap().at("price")).asInt();
    }else if(itemType == ITEM_TYPE_WEAPON){
        return Value(weaponStatTable[name].asValueMap().at("price")).asInt();
    }else if(itemType == ITEM_TYPE_HELMET){
        return Value(helmetStatTable[name].asValueMap().at("price")).asInt();
    }else if(itemType == ITEM_TYPE_SHOES){
        return Value(shoesStatTable[name].asValueMap().at("price")).asInt();
    }
    return -1;
}
std::string HelloWorld::getItemSpriteName(int itemType, std::string name){
    if(itemType == ITEM_TYPE_ETC){
        return Value(itemStatTable[name].asValueMap().at("sprite")).asString();
    }else if(itemType == ITEM_TYPE_SHIELD){
        return Value(shieldStatTable[name].asValueMap().at("sprite")).asString();
    }else if(itemType == ITEM_TYPE_WEAPON){
        return Value(weaponStatTable[name].asValueMap().at("sprite")).asString();
    }else if(itemType == ITEM_TYPE_HELMET){
        return Value(helmetStatTable[name].asValueMap().at("sprite")).asString();
    }else if(itemType == ITEM_TYPE_SHOES){
        return Value(shoesStatTable[name].asValueMap().at("sprite")).asString();
    }
    return "";
}
int HelloWorld::getShieldStat(std::string name, std::string stat){
    if(name.size() == 0 || shieldStatTable[name].asValueMap().find(stat) == shieldStatTable[name].asValueMap().end()) return 0;
    return Value(shieldStatTable[name].asValueMap().at(stat)).asInt();
}
int HelloWorld::getShoesStat(std::string name, std::string stat){
    if(name.size() == 0 || shoesStatTable[name].asValueMap().find(stat) == shoesStatTable[name].asValueMap().end()) return 0;
    return Value(shoesStatTable[name].asValueMap().at(stat)).asInt();
}
int HelloWorld::getHelmetStat(std::string name, std::string stat){
    if(name.size() == 0 || helmetStatTable[name].asValueMap().find(stat) == helmetStatTable[name].asValueMap().end()) return 0;
    return Value(helmetStatTable[name].asValueMap().at(stat)).asInt();
}
std::string HelloWorld::getHelmet(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, heroSlot).c_str());
}
std::string HelloWorld::getWeapon(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, heroSlot).c_str());
}
std::string HelloWorld::getShield(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, heroSlot).c_str());
}
std::string HelloWorld::getShoes(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, heroSlot).c_str());
}
void HelloWorld::movePlayer(int direction){
    
}
void HelloWorld::addGold(int amount){
    if (amount > 0) {
        totalEarnedGold += amount;
    }else{
        totalUsedGold -= amount;
    }
    gold += amount;
    HUD->lblGold->setString(strmake("%d", gold));
}
void HelloWorld::addLumber(int amount){
    if (amount > 0) {
        totalEarnedLumber += amount;
    }else{
        totalUsedLumber -= amount;
    }
    lumber += amount;
    HUD->lblLumber->setString(strmake("%d", lumber));
}
void HelloWorld::updateFoodInUse(){
    if(GM->currentStageIndex == STAGE_LOBBY)return;
    int foodUse = 0;
    for(auto unit: heroArray){
        if(!unit->isAlli){
            foodUse += GM->getFoodUseForUnit(unit->unitType);
        }
    }
    foodInUse = foodUse;
    HUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
}
void HelloWorld::addFoodMax(int amount){
    if(GM->currentStageIndex == STAGE_LOBBY){
        return;
    }
    foodMax += amount;
    PPLabel* lbl = HUD->lblFood;
    std::string str = strmake("%d/%d", foodInUse, foodMax);
    lbl->setString(str);
}
int HelloWorld::getGoldPriceForUnit(int index){
    if (index == UNIT_CASTLE) {
        return 1200;
    }else if (index == UNIT_FARM) {
        return 400;
    }else if (index == UNIT_WATCHERTOWER) {
        return 600;
    }else if (index == UNIT_LUMBERMILL) {
        return 900;
    }else if (index == UNIT_FACTORY) {
        return 1000;
    }else if (index == UNIT_AIRPORT) {
        return 600;
    }else if (index == UNIT_BARRACKS) {
        return 800;
    }else if (index == UNIT_WORKER) {
        return 200;
    }else if (index == UNIT_GOBLIN_WORKER) {
        return 200;
    }else if (index == UNIT_SWORDMAN) {
        return 300;
    }else if (index == UNIT_GOBLIN) {
        return 230;
    }else if (index == UNIT_GOBLIN_BOMB) {
        return 300;
    }else if (index == UNIT_ORC_AXE) {
        return 250;
    }else if (index == UNIT_ORC_SPEAR) {
        return 150;
    }else if (index == UNIT_TROLL) {
        return 550;
    }else if (index == UNIT_WIZARD) {
        return 250;
    }else if (index == UNIT_ARCHER) {
        return 200;
    }else if (index == UNIT_CATAPULT) {
        return 500;
    }else if (index == UNIT_HELICOPTER) {
        return 100;
    }else if (index == UNIT_ORC_HQ) {
        return 1100;
    }else if (index == UNIT_ORC_BUNKER) {
        return 700;
    }else if (index == UNIT_ORC_BARRACKS) {
        return 850;
    }else if (index == UNIT_BARBECUE) {
        return 150;
    }else if (index == UNIT_TEMPLE) {
        return 1100;
    }else if (index == UNIT_ORC_TROLL_HOUSE) {
        return 700;
    }
    
    return 0;
}
int HelloWorld::getLumberPriceForUnit(int index){
    if (index == UNIT_WATCHERTOWER) {
        return 200;
    }else if (index == UNIT_CASTLE) {
        return 800;
    }else if (index == UNIT_FARM) {
        return 200;
    }else if (index == UNIT_BARRACKS) {
        return 400;
    }else if (index == UNIT_LUMBERMILL) {
        return 500;
    }else if (index == UNIT_FACTORY) {
        return 700;
    }else if (index == UNIT_AIRPORT) {
        return 600;
    }else if (index == UNIT_ARCHER) {
        return 150;
    }else if (index == UNIT_GOBLIN_BOMB) {
        return 100;
    }else if (index == UNIT_ORC_AXE) {
        return 80;
    }else if (index == UNIT_ORC_SPEAR) {
        return 180;
    }else if (index == UNIT_TROLL) {
        return 360;
    }else if (index == UNIT_WIZARD) {
        return 550;
    }else if (index == UNIT_CATAPULT) {
        return 400;
    }else if (index == UNIT_HELICOPTER) {
        return 200;
    }else if (index == UNIT_ORC_HQ) {
        return 800;
    }else if (index == UNIT_ORC_BUNKER) {
        return 220;
    }else if (index == UNIT_ORC_BARRACKS) {
        return 350;
    }else if (index == UNIT_BARBECUE) {
        return 150;
    }else if (index == UNIT_TEMPLE) {
        return 800;
    }else if (index == UNIT_ORC_TROLL_HOUSE) {
        return 500;
    }
    return 0;
}

int HelloWorld::getFoodGive(int index){
    if (index == UNIT_CASTLE) {
        return 6;
    }else if (index == UNIT_FARM) {
        return 7;
    }else if (index == UNIT_BARBECUE) {
        return 6;
    }
    return 0;
}
EnemyBase* HelloWorld::getNearestCastle(cocos2d::Point pos){
    long minDistance = 20000000;
    long distance = 0;
    EnemyBase* nearest = nullptr;
    for(auto unit:heroArray){
        distance = unit->getPosition().distanceSquared(pos);
        if(unit->unitType == UNIT_CASTLE){
            if(distance < minDistance){
                minDistance = distance;
                nearest = unit;
            }
        }
    }
    return nearest;
}
EnemyBase* HelloWorld::getNearestLumberTank(cocos2d::Point pos){
    long minDistance = 20000000;
    long distance = 0;
    EnemyBase* nearest = nullptr;
    for(auto unit:heroArray){
        distance = unit->getPosition().distanceSquared(pos);
        if(unit->unitType == UNIT_CASTLE || unit->unitType == UNIT_LUMBERMILL){
            if(distance < minDistance){
                minDistance = distance;
                nearest = unit;
            }
        }
    }
    return nearest;
}
EnemyBase* HelloWorld::getNearestTree(cocos2d::Point pos){
    long minDistance = 20000000;
    long distance = 0;
    EnemyBase* nearest = nullptr;
    bool unreachableTreeExist = false;
    bool treeExist = false;
    for(auto unit:mutualArray){
        distance = unit->getPosition().distanceSquared(pos);
        if(unit->unitType == UNIT_TREE){
            if(distance < minDistance){
                minDistance = distance;
                nearest = unit;
            }
            treeExist = true;
        }
        if(unit->unitType == UNIT_UNREACHABLE_TREE){
            unreachableTreeExist = true;
        }
    }
    if(!treeExist && unreachableTreeExist){
        for(auto unit:mutualArray){
            if(unit->unitType == UNIT_UNREACHABLE_TREE){
                unit->unitType = UNIT_TREE;
            }
        }
    }
    return nearest;
}
void HelloWorld::setClearCondition(int stage){
    int offsetX = 0;
    if(size.height/size.width < 700.0f/1334.0f){
        offsetX = 100;
    }
//    Sprite* sptMission = Sprite::create("mission.png");
//    HUD->addChild(sptMission);
//    sptMission->setPosition(Point(offsetX + 50 + sptMission->getContentSize().width/2, size.height - 66));
    int condition = GM->getStageObjective(stage);
    Node* ndMission = HUD->rightBottomPanel->getChildByName("ndMission");
    int hideFrom = 3;
    if(condition == CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND){
        clearConditionIndex = GM->getStageObjective(stage);
        for (int i = 0; i < 3; i++) {
//            Label* lbl = LM->getLocalizedLabel();
//            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, true, false, TextHAlignment::LEFT, true);
//            HUD->addChild(lbl);
//            lbl->setAnchorPoint(Point(0, 0.5));
//            lbl->setPosition(Point(offsetX + 50, size.height - 166 - 80*i));
//            lblConditionArray.pushBack((Text*)ndMission->getChildByName(strmake("lblCondition%d", i)));
            
//            LM->setLocalizedString(lblConditionArray.at(i), "");
//            lbl->enableShadow();
        }
    }else if(condition == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE){
        clearConditionIndex = GM->getStageObjective(stage);
        for (int i = 0; i < 2; i++) {
//            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, true, false, TextHAlignment::LEFT, true);
//            HUD->addChild(lbl);
//            lbl->setPosition(Point(offsetX + 50, size.height - 166 - 80*i));
//            lblConditionArray.pushBack((Text*)ndMission->getChildByName(strmake("lblCondition%d", i)));
//            LM->setLocalizedString(lblConditionArray.at(i), "");
//            lblConditionArray.pushBack(lbl);
        }
        hideFrom = 2;
    }else{
        clearConditionIndex = GM->getStageObjective(stage);
//        PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, true, false, TextHAlignment::LEFT, true);
//        HUD->addChild(lbl);
//        lbl->setPosition(Point(offsetX + 50, size.height - 166 - 80*0));
//        lblConditionArray.pushBack((Text*)ndMission->getChildByName("lblCondition0"));
//        LM->setLocalizedString(lblConditionArray.at(0), "");
        hideFrom = 1;
    }
    for(int i = hideFrom; i < 3; i++){
        ndMission->getChildByName(strmake("lblCondition%d", i))->setVisible(false);
    }
    checkClearGame();
}
bool HelloWorld::checkClearGame(){
    if(GM->currentStageIndex == STAGE_SINGLEPLAY || GM->currentStageIndex == STAGE_RAID){
        if(isGameOver || isGameClear) return false;
        bool isCastleExist = false;
        bool isSoldierExist = false;
        for(auto unit: enemyArray){
            if(unit->unitType == UNIT_CASTLE){
                isCastleExist = true;
            }
            if (GM->nextScene == STAGE_SINGLEPLAY && unit->unitType == UNIT_ORC_BUNKER) {
                isCastleExist = true;
            }
            if(!unit->isBuilding){
                isSoldierExist = true;
            }
        }
        int starCountTemp = 0;
        if (enemyArray.size() == 0) {
            starCountTemp++;
        }
        if(!isCastleExist){
            starCountTemp++;
        }
        if(!isSoldierExist){
            starCountTemp++;
        }
        while (starCountTemp > starCountForRaid) {
            starCountForRaid++;
            HUD->showStarForRaid(starCountForRaid-1);
        }
        if(starCountForRaid >= 3){
            isGameClear = true;
            this->runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(HudLayer::showRaidResultWithSaveRecord, HUD)), NULL));
        }else{
            if (heroArray.size() == 0 && HUD->isRaidStarted) {
                HUD->showRaidResult(true);
            }
        }
        
        return false;
    }
    bool isAllClear = false;
    Text* lbl;
    Node* ndMission = HUD->rightBottomPanel->getChildByName("ndMission");
    if(clearConditionIndex == CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND){
        isAllClear = true;
        int barracksCount = 0;
        int farmCount = 0;
        int swordmanCount = 0;
        for(auto unit: heroArray){
            if(unit->unitType == UNIT_SWORDMAN){
                swordmanCount++;
            }else if(unit->unitType == UNIT_FARM){
                farmCount++;
            }else if(unit->unitType == UNIT_BARRACKS){
                barracksCount++;
            }
        }
        
        bool complete;
        
        for(int i = 0; i < 3; i++){
            complete = false;
            if(i==0){
                lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", i));
                LM->setLocalizedString(lbl, strmake("%s(%d/2)", LM->getText("objective build farm").c_str(), farmCount));
                if(farmCount < 2){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }else if(i == 1){
                lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", i));
                LM->setLocalizedString(lbl, strmake("%s(%d/1)", LM->getText("objective build barracks").c_str(), barracksCount));
                if(barracksCount < 1){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }else if(i == 2){
                lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", i));
                LM->setLocalizedString(lbl, strmake("%s(%d/4)", LM->getText("objective train swordsman").c_str(), swordmanCount));
                if(swordmanCount < 4){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }
            if(complete){
                lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", i));
                if(lbl->getTextColor() != Color4B(245, 213, 71, 255)){
                    lbl->setTextColor(Color4B(245, 213, 71, 255));
                    Sprite* spt = Sprite::create("check.png");
                    ndMission->getParent()->addChild(spt);
                    spt->setPosition(Point(lbl->getPositionX() + lbl->getContentSize().width + 50, lbl->getPositionY()));
                }
            }
        }
    }else if(clearConditionIndex == CLEAR_CONDITION_TERMINATE){
        isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
//        isAllClear = unitsToCreateArray.size() == 0;
//        for (int i = 0; i < enemyArray.size(); i++) {
//            if(enemyArray.at(i)->isVisible()){
//                isAllClear = false;
//                break;
//            }
//        }
//
        lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", 0));
        LM->setLocalizedString(lbl, LM->getText("objective destroy them all"));
        if(enemyArray.size() < 5 && enemyArray.size() > 0 && unitsToCreateArray.size() == 0){
            isRevealingMap = true;
        }
        if (gameTimer > 60*40) {
            isRevealingMap = true;
        }
    }else if(clearConditionIndex == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE){
        isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
        lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", 0));
        LM->setLocalizedString(lbl, LM->getText("objective protect all"));
        lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", 1));
        LM->setLocalizedString(lbl, LM->getText("objective destroy them all"));
    }else if(clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_10_MIN || clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN){
        
        int min = 10;
        if(clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN){
            min = 20;
        }
        isAllClear = gameTimer > min*60;
        lbl = (Text*)ndMission->getChildByName(strmake("lblCondition%d", 0));
        LM->setLocalizedString(lbl, LM->getText(strmake(LM->getText("objective survive").c_str(), min).c_str()));
    }
    
    isGameClear = isAllClear;
    return isAllClear;
}
int HelloWorld::getAttackPriority(int index){
    if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ ){
        return 1;
    }else if(index == UNIT_CASTLE || index == UNIT_FARM || index == UNIT_BARRACKS || index == UNIT_LUMBERMILL || index == UNIT_WATCHERTOWER || index == UNIT_FACTORY || index == UNIT_AIRPORT){
        return 0;
    }
    return 2;
}
bool HelloWorld::canAttack(Movable* attacker, Movable* target){
    bool can = true;
    if(attacker->attackType == ATTACK_TYPE_NEAR && target->isFlying){
        can = false;
    }
    return can;
}
bool HelloWorld::isInScreen(cocos2d::Point pos){
    return Rect(-getPositionX(), -getPositionY(), size.width, size.height).containsPoint(pos);
}
void HelloWorld::revengeAttack(Movable* attackee, Movable* attacker){
    if (attackee->unitAct == UNIT_ACT_MOVE || attackee->unitAct == UNIT_ACT_ATTACK) {
        return;
    }
    if (attackee->unitActDetail == UNIT_ACT_DETAIL_ATTACK) {
        return;
    }
    if (attacker->isEnemy) {
        for(auto unit: heroArray){
            if (!unit->isBuilding && unit->attackType != ATTACK_TYPE_NONE && unit->getPosition().distanceSquared(attackee->getPosition()) < 300000 && !unit->isGatheringGold && !unit->isGatheringTree && !unit->isGoingToBuild && unit->isArrived) {
                moveAndAttackTo(unit, attacker->getPosition());
            }
        }
    }else{
        for(auto unit: enemyArray){
            if (!unit->isBuilding && unit->attackType != ATTACK_TYPE_NONE && unit->getPosition().distanceSquared(attackee->getPosition()) < 300000 && !unit->isGatheringGold && !unit->isGatheringTree && !unit->isGoingToBuild) {
                moveAndAttackTo(unit, attacker->getPosition());
            }
        }
    }
}
void HelloWorld::selectAllForces(){
    deselectAll();
    
    for(auto unit: heroArray){  // select heroes
        if(unit->isAlli){
            continue;
        }
        if(unit->unitType == UNIT_SWORDMAN ||unit->unitType == UNIT_ARCHER ||unit->unitType == UNIT_CATAPULT||
           unit->unitType == UNIT_HELICOPTER ||
           unit->unitType == UNIT_GOBLIN ||
           unit->unitType == UNIT_GOBLIN_BOMB ||
           unit->unitType == UNIT_ORC_AXE ||
           unit->unitType == UNIT_ORC_SPEAR ||
           unit->unitType == UNIT_TROLL ||
           unit->unitType == UNIT_ZOMBIE_ORC_AXE ||
           unit->unitType == UNIT_ZOMBIE_SWORDSMAN ||
           unit->unitType == UNIT_WIZARD ||
           unit->spine
           ){
            selectUnit(unit);
        }
    }
}
void HelloWorld::onSelectForcesInScreen(){
    deselectAll();
    
    for(auto unit: heroArray){  // select heroes
        if(unit->isAlli){
            continue;
        }
        if(this->isInScreen(unit->getPosition())){
            if(unit->unitType == UNIT_SWORDMAN ||unit->unitType == UNIT_ARCHER ||unit->unitType == UNIT_CATAPULT||
               unit->unitType == UNIT_HELICOPTER ||
               unit->unitType == UNIT_GOBLIN ||
               unit->unitType == UNIT_GOBLIN_BOMB ||
               unit->unitType == UNIT_ORC_AXE ||
               unit->unitType == UNIT_ORC_SPEAR ||
               unit->unitType == UNIT_TROLL ||
               unit->unitType == UNIT_ZOMBIE_ORC_AXE ||
               unit->unitType == UNIT_ZOMBIE_SWORDSMAN||
               unit->unitType == UNIT_WIZARD ||
               unit->spine){
                selectUnit(unit);
            }
        }
    }
}
void HelloWorld::getSupportFromVideo(int video){
    finishedVideo = video;
}
