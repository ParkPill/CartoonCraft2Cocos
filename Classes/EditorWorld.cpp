

#include "EditorWorld.h"
#include "SimpleAudioEngine.h"
#include "LegendDaryButton.h"
#include "EditorHud.h"
#include "GameManager.h"

#include "Fog.h"
//#include "NativeInterface.h"
#include "AwesomeDialogBox.h"
#include "LanguageManager.h"
using namespace cocos2d;
using namespace CocosDenshion;


Scene* EditorWorld::scene(int stage, bool boss)
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
    cocos2d::Size size = Director::getInstance()->getWinSize();
    Scene *scene = Scene::create();
    int theme = GameManager::getInstance()->theme;
    GameManager::getInstance()->currentStageIndex = stage;
    
    //    GameManager::getInstance()->showInterstitialAds();
    
    //    LayerColor* grayLayer = LayerColor::create(Color4B(246,246,246,255));
    LayerColor* grayLayer = LayerColor::create(Color4B(156, 156, 156, 255));
    grayLayer->setName("grayLayer");
    //    LayerColor* grayLayer = LayerColor::create(Color4B(206,206,206,255));
    scene->addChild(grayLayer);
    EditorWorld *layer = EditorWorld::create();
    scene->addChild(layer);
    layer->stageIndex = stage;
    
    EditorHud* hudLayer = EditorHud::create();
    scene->addChild(hudLayer, 10);
    GameManager::getInstance()->setEditorHud(hudLayer);
    
    
    
    
    layer->isBossMap = boss;
    if(GM->nextScene == STAGE_FIELD){
        layer->addListener();
    }
    int tag = rand()%50;
    layer->setTag(tag);
    hudLayer->setTag(tag);
    hudLayer->world = layer;
    CCLOG("** New Tag: %d, theme: %d, stage: %d", tag, GameManager::getInstance()->theme, stage);
    
    
    
    if(GM->nextScene == STAGE_CREDIT){ // test
        layer->setEntireMap(stage);
        EHUD->showCredit();
        return scene;
    }else{
        hudLayer->addListener();
    }
    //    if (boss || true) {//test
    if(boss){
        layer->setBossMap(stage);
    }else{
        layer->setEntireMap(stage);
    }
    if(GM->nextScene == STAGE_FIELD){
        
    }
    

//    Vec2 pos = Vec2(110, 110);
//    EHUD->selectedMode = MODE_PENCIL;
//    EHUD->selectedBrush = BRUSH_SWORDSMAN;
//    EHUD->doBrush(pos);
//    EHUD->showEditDetail(pos); // test 
    
    return scene;
}

// on "init" you need to initialize your instance
bool EditorWorld::init()
{
    size = Director::getInstance()->getWinSize();
    //////////////////////////////
    // 1. super init first
    theBoss = NULL;
    
    // test
    //    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, 3)->getCString(), true);
    //    GameManager::getInstance()->isGameClear = true;
    
    GameManager::getInstance()->isInMiddleOfGame = true;
    
    if (!Layer::init())
    {
        return false;
    }
    loadUnitSheet();
    isReloading = false;
    center = cocos2d::Vec2(size.width/2, size.height/2);
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
    
    spriteBatch = Node::create();//"cartoonCraft.png");
    this->addChild(spriteBatch, 10);
    spriteBatchEffect = Node::create();//"cartoonCraftEffect.png");
    this->addChild(spriteBatchEffect, 10);
    std::string stage = UD->getStringForKey(KEY_LAST_SAVE_POINT, "stage0");
    
    enemyPointArray = PointArray::create(100);
    enemyPointArray->retain();
    
    
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
    //    draw->drawSolidRect(Vec2(8*TILE_SIZE, 8*TILE_SIZE), Vec2(9*TILE_SIZE, 9*TILE_SIZE), Color4F::GREEN);
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
    for(int i = 0; i < mapSizeWidth; i++){
        for (int j = 0; j < mapSizeHeight; j++) {
            placedArray[j][i] = 0;
        }
    }
    // init done
    return true;
}
void EditorWorld::loadUnitSheet(){
    
}
void EditorWorld::checkAndShowQuestOnMap(cocos2d::experimental::TMXTiledMap* map){
//    int level = getHeroLevel(0);
//    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//
//    //    int currentQuestIndex = npcTable[Value(questID).asString()].asValueMap().at("event index").asInt();
//
//    if(questID >= 0){
//        //        std::string currentQuestNpcName = npcTable[Value(questID).asString()].asValueMap().at("npc name").asString();
//        std::string strStage = npcTable[Value(questID).asString()].asValueMap().at("stage").asString();
//        ValueVector rows = GameManager::getInstance()->split(strStage, "_");
//        int mapIndex = rows.at(0).asInt();
//        int x = rows.at(1).asInt();
//        int y = rows.at(2).asInt();
//        Vec2 pos = Vec2((x*3+1+2)*TILE_SIZE + TILE_SIZE/2, (16-1+(y*2)-12)*TILE_SIZE) + Vec2(-TILE_SIZE, TILE_SIZE*1.0f);
//        if (isQuestConditionMet()) {
//            addTalkBalloon(map, "exclaim.png", pos);
//        }else{
//            addTalkBalloon(map, "waitingTalkBox.png", pos);
//        }
//    }else{
//        int eventIndex = -1;
//        bool isQuest = false;
//        int questID = -1;
//        int eIndex = -1;
//        for(int i = 0;i < npcTable.size();i++){
//            //            if(npcTable[Value(i).asString()].asValueMap().at("npc name").asString().compare(npcName) == 0){
//            std::string str = npcTable[Value(i).asString()].asValueMap().at("level limit").asString();
//            if(str.size() <= 0){
//                continue;
//            }
//            int levelLimit = Value(str).asInt();
//            eIndex = npcTable[Value(i).asString()].asValueMap().at("event index").asInt();;
//            if(level >= levelLimit && !UDGetBool(strmake(KEY_QUEST_DONE_FORMAT, eIndex).c_str(), false)){
//                eventIndex = eIndex;
//                questID = i;
//                isQuest = npcTable[Value(i).asString()].asValueMap().at("event type").asString().compare("quest") == 0;
//
//                std::string npcStage = npcTable[Value(i).asString()].asValueMap().at("stage").asString();
//                if(npcStage.compare("purgatory") == 0){
//                    continue;
//                }
//                ValueVector rows = GameManager::getInstance()->split(strStage, "_");
//                int mapIndex = rows.at(0).asInt();
//
//                rows = GameManager::getInstance()->split(npcStage, "_");
//                int npcMapIndex = rows.at(0).asInt();
//                int npcX = rows.at(1).asInt();
//                int npcY = rows.at(2).asInt();
//
//                if(mapIndex == npcMapIndex){
//                    Vec2 pos = Vec2((npcX*3+1+2)*TILE_SIZE + TILE_SIZE/2, (16-1+(npcY*2)-12)*TILE_SIZE) + Vec2(-TILE_SIZE, TILE_SIZE*1.0f);
//                    addTalkBalloon(map, "question.png", pos);
//                }
//                //                    break;
//            }
//            //            }
//        }
//        //        if(eventIndex < 0){
//        //
//        //        }else{
//        //            addTalkBalloon(npc, "question.png");
//        //        }
//    }
}
void EditorWorld::moveInTheMap(){
    
}
int EditorWorld::getItemType(std::string name){
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

void EditorWorld::walkInTheMapDone(){
//    unitInTheMap->runMoveAnimation(directionToMoveInTheMap);
}
void EditorWorld::moveInTheMapDone(){
//    GM->mapMoveRequested = false;
//    this->runAction(Sequence::create(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(EditorHud::showBIAndReplaceScene, EHUD)), NULL));
//    unitInTheMap->movingDirection = DIRECTION_S;
//    unitInTheMap->runMoveAnimation(DIRECTION_STAY);
}
bool EditorWorld::pickUpItem(){
    if(encounteredDropItem == nullptr){
        return false;
    }
    if (EHUD->inventory->isSlotAvailableForThisItem(encounteredDropItem->getName(), EHUD->getInventoryTypeForItemType(getItemType(encounteredDropItem->getName())), 1)) {
        
    }else{
        EHUD->showInstanceMessage("not enough inventory slot");
        return true;
    }
    EHUD->inventory->addItem(encounteredDropItem->getName(), EHUD->getInventoryTypeForItemType(getItemType(encounteredDropItem->getName())));
    runEffect(EFFECT_TWINKLE, encounteredDropItem->getPosition());
    
    dropItemArray.eraseObject(encounteredDropItem);
    encounteredDropItem->runAction(Sequence::create(EaseOut::create(MoveBy::create(0.1f, Vec2(60, -80)), 2), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, encounteredDropItem)), NULL));
    
    encounteredDropItem = nullptr;
    
    return true;
}
void EditorWorld::shieldUp(bool up){
    
}
void EditorWorld::move(float dt, Movable* obj, Movable* target, bool horiFirst){
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
void EditorWorld::updateUnitMove(float dt){
    // heros
    int moveCounter = 1;
    for(auto hero: heroArray){
        if(!hero->canMove && !hero->canFindTarget && hero->energy <= 0) continue;
        // finding target
        if(hero->canFindTarget && (hero->target == nullptr || hero->target == hero->dummyTarget)){
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
            if(nearest != nullptr){
                if(hero->canMove){
                    hero->moveToTarget(nearest);
                    hero->canFindTarget = false;
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
        if(enemy->energy <= 0) continue;
        if(!enemy->canFindTarget && enemy->target != nullptr){
            float distance = enemy->getPosition().distanceSquared(enemy->target->getPosition());
            if (distance >= enemy->monitoringDistance) {
                enemy->canFindTarget = true;
                enemy->target = nullptr;
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
Movable* EditorWorld::createMissile(int missileType, int dmg, bool visible, float time, int angle, int speed, Vec2 pos, bool isFromEnemy, std::string weaponName){
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
    sptMissile->runAction(Sequence::create(MoveBy::create(time, Vec2(delta*cos(angle*3.14f/180), delta*sin(angle*3.14f/180))), CallFuncN::create(CC_CALLBACK_1(EditorWorld::missileMoveDone, this)), nullptr));
    sptMissile->setRotation(-angle + 90);
    if(isFromEnemy){
        enemyMissileArray.pushBack(sptMissile);
    }else{
        heroMissileArray.pushBack(sptMissile);
    }
}
//void EditorWorld::gettingReadyForBattle(float dt){
//    battleReadyCountDown--;
//    if (battleReadyCountDown == 2) {
//        Label* lbl = Label::createWithTTF("READY", GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//        GameManager::getInstance()->getEditorHud()->addChild(lbl);
//        lbl->setPosition(size/2);
//        lbl->setScale(0.2f);
//        lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)), nullptr));
//    }else if (battleReadyCountDown == 1) {
//        Label* lbl = Label::createWithTTF("BATTLE!", GameManager::getInstance()->getFont(FONT_VISITOR), 40);
//        GameManager::getInstance()->getEditorHud()->addChild(lbl);
//        lbl->setPosition(size/2);
//        lbl->setScale(0.2f);
//        lbl->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.2f)), DelayTime::create(0.7f), FadeOut::create(0.1f), CallFunc::create(CC_CALLBACK_0(Label::removeFromParent, lbl)), nullptr));
//    }else if (battleReadyCountDown == 0) {
//        this->unschedule(schedule_selector(EditorWorld::gettingReadyForBattle));
//        isBattleStarted = true;
//    }
//}
EditorWorld::EditorWorld(){
    
}
void EditorWorld::getDown(){
    
}
void EditorWorld::dialogFinished(Ref* obj){
    
}
void EditorWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        //        NativeInterface::NativeInterface::destroyAds();
#endif
        
        //        Director::getInstance()->replaceScene(StageSelectScene::scene(GameManager::getInstance()->theme));
    }
}
void EditorWorld::showWarpAnimation(){
    Sprite* sptBeam = Sprite::createWithSpriteFrameName("blueBeamVertical0.png");
    sptBeam->setAnchorPoint(Vec2(0.5, 0));
    sptBeam->setScaleY(10);
    sptBeam->setScaleX(2);
    spriteBatch->addChild(sptBeam, 11);
    const char* beamName = "blueBeam";
    if (stageIndex == indexToWarp || (isBossMap && !isSetStageDone)) {
        beamName = "redBeam";
    }
    Animation* animation = AnimationCache::getInstance()->getAnimation(beamName);
    Animate* animate = Animate::create(animation);
    sptBeam->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptBeam)), CallFunc::create(CC_CALLBACK_0( EditorWorld::checkEnding, this)), NULL));
    Vec2 pos;
    
    sptBeam->setPosition(pos + Vec2(0, 100));
    sptBeam->runAction(MoveTo::create(0.05, pos));
    
    
    this->runAction(Sequence::create(DelayTime::create(18*0.04), CallFunc::create(CC_CALLBACK_0(EditorWorld::stopWoongSound, this)), nullptr));
    shakeScreen(6);
    
    
    //    this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(EditorWorld::testFunction, this)), NULL));
}
void EditorWorld::checkEnding(){
    
}
void EditorWorld::testFunction(){
//    GameManager::getInstance()->getEditorHud()->dialogBox->removeFromParent();
    
}
void EditorWorld::takeAction(){
    log("take action");
//    GameManager::getInstance()->getEditorHud()->lblTouch->stopAllActions();
//    GameManager::getInstance()->getEditorHud()->lblTouch->setOpacity(0);
    
}
void EditorWorld::warpOut(){
    
}
void EditorWorld::warp(){
//    GameManager::getInstance()->getEditorHud()->showStageTitle();
    
    this->runAction(Sequence::create(DelayTime::create(4), CallFunc::create(CC_CALLBACK_0(EditorWorld::goToNextScene, this)), NULL));
}
void EditorWorld::goToNextScene(){
    GameManager::getInstance()->getEditorHud()->removeListener();
    Scene* scene = Scene::create();
    bool isBoss = false;
    if (stageIndex >= 0 && !isBossMap) {
        isBoss = true;
    }
    scene->addChild(EditorWorld::scene(indexToWarp, isBoss));
    Director::getInstance()->replaceScene(scene);
}
void EditorWorld::enemyUpdate(float dt)
{
    if (isGameOver) {
        return;
    }
    //    dnDamageBoxes->clear();
    //    dnDamageBoxes->drawRect(player->damageBoundingBox().origin, player->damageBoundingBox().origin + player->damageBoundingBox().size, Color4F::RED);
    //    dnCollisionBoxes->clear();
    //    dnCollisionBoxes->drawRect(player->collisionBoundingBox().origin, player->collisionBoundingBox().origin + player->collisionBoundingBox().size, Color4F::BLUE);
    cocos2d::Rect projectileRect;
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
        projectileRect = cocos2d::Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
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
void EditorWorld::updateFireStick(float dt){
    
}
void EditorWorld::destructableUpdate()
{
    if (isGameOver) {
        return;
    }
    
    cocos2d::Rect projectileRect, mRect;
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
            
            //            mRect = cocos2d::Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
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
                        
                        createRewards(drop);
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
    
    for (auto missile: enemyMissileArray) {
        for (int i = 0; i<destructableArray.size(); i++) {
            drop = destructableArray.at(i);
            
            if (drop->untouchable ||
                drop->getPosition().y-missile->getPosition().y > 50 ||
                drop->getPosition().y - missile->getPosition().y < -50) {
                continue;
            }
            
            mRect = cocos2d::Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
                         missile->getPosition().y - (missile->getContentSize().height*missile->getScaleY()/2),
                         missile->getContentSize().width*missile->getScaleX(),
                         missile->getContentSize().height*missile->getScaleY());
            projectileRect = drop->damageBoundingBox();
            if (projectileRect.intersectsRect(mRect)) {
                if (!drop->immortal) {
                    float demage = missile->energy;
                    
                    drop->energy -= (int)10;
                }
                if (drop->energy <= 0) {
                    destroyDestructable(drop);
                    createRewards(drop);
                    destructableArray.eraseObject(drop);
                    drop->removeFromParentAndCleanup(true);
                }
                missileToRemove.pushBack(missile);
                this->runEffect(EFFECT_EXPLODE_MIDDLE, drop->getPosition());
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
void EditorWorld::destroyDestructable(Movable* drop){
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
        part->velocity = Vec2(rand()%500 - 250, 250);
        part->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create(CC_CALLBACK_1(EditorWorld::coinWaitDone, this)), NULL));
        MovableCoinArray.pushBack(part);
    }
    runEffect(EFFECT_EXPLODE_BIG, drop->getPosition());
}
void EditorWorld::showDamage(int damage, Vec2 pos){
    if (damage <= 0) {
        return;
    }
    
    Node* sptNumber = createNumberSprite(damage);
    spriteBatch->addChild(sptNumber, 100);
    sptNumber->setScale(1.5f);
    sptNumber->setPosition(pos);
    sptNumber->runAction(EaseIn::create(MoveBy::create(1, cocos2d::Vec2(0, 25)), 0.3));
    sptNumber->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptNumber)), NULL));
}
void EditorWorld::attackEnemy(EnemyBase* drop, int damage){
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
    this->runAction(Sequence::create(DelayTime::create(0.07f), CallFunc::create(CC_CALLBACK_0(EditorWorld::resumeLayer, this)), NULL));
}

void EditorWorld::attackHero(EnemyBase* drop, int damage){
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
        //        GameManager::getInstance()->getEditorHud()->setScore(GameManager::getInstance()->totalScore);
        
        destructableArray.eraseObject(drop);
        MovableArray.eraseObject(drop);
        spriteBatch->removeChild(drop, true);
        removeHero(drop);
    }
    else
    {
        drop->blinkForAWhile();
    }
    
}
void EditorWorld::immortalForSec(int sec){
    indestructableTime = sec;
    this->schedule(schedule_selector(EditorWorld::immortalForAWhile), 0.1);
}
void EditorWorld::immortalForAWhile(float dt){
    indestructableTime -= dt;
    if (indestructableTime < 0) {
        this->unschedule(schedule_selector(EditorWorld::immortalForAWhile));
    }
}
bool EditorWorld::hitPlayer(int damage){
    //    return false; // test
    
    return false;
}
void EditorWorld::stepBackForHit(float dt)
{
}
void EditorWorld::unfreezePlayer(float dt)
{
}

cocos2d::Rect EditorWorld::RectInset(cocos2d::Rect rect, float x, float y)
{
    return cocos2d::Rect(rect.origin.x + x, rect.origin.y + y, rect.size.width - x*2, rect.size.height - y*2);
}

void EditorWorld::angelReviveLater(float dt){
    revive();
}
void EditorWorld::endGame(bool win){
    this->unschedule(schedule_selector(EditorWorld::gravityUpdate));
    this->unschedule(schedule_selector(EditorWorld::gravityUpdateForCoins));
    this->unschedule(schedule_selector(EditorWorld::gravityUpdateForStraight));
    this->unschedule(schedule_selector(EditorWorld::gravityUpdateForCustomMoving));
    this->unschedule(schedule_selector(EditorWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    for(auto enemy : enemyArray){
        enemy->pause();
    }
    isGameOver = !win;
    EHUD->showWinPopup(win);
}
void EditorWorld::updateResult(float dt){
    if (resultDone) {
        this->unschedule(schedule_selector(EditorWorld::updateResult));
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
    cursor->setPosition(lblOk->getPosition() + Vec2(-20, 0));
    
    Sprite* innerCursor = Sprite::create("targetWhite.png");
    cursor->addChild(innerCursor, 99990);
    innerCursor->runAction(RepeatForever::create(RotateBy::create(1, -160)));
    innerCursor->setScale(0.7);
    innerCursor->setPosition(Vec2(cursor->getContentSize().width/2, cursor->getContentSize().height/2));
}
void EditorWorld::gameOver()
{
    pauseLayer();
    isGameOver = true;
    
    for (auto drop: enemyMissileArray) {
        drop->removeFromParentAndCleanup(true);
    }
    enemyMissileArray.clear();
    
    
}

void EditorWorld::missileUpdate(float dt)
{
    if (isGameOver) {
        return;
    }
    
    cocos2d::Rect mRect;
    //    enemiesToRemove.clear();
    
    Vector<Movable*> missileToRemove;
    EnemyBase* drop;
    for (auto missile: heroMissileArray) {
        mRect = cocos2d::Rect(missile->getPosition().x - (missile->getContentSize().width*missile->getScaleX()/2),
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
                Vec2 pos = drop->getPosition();
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
                            critical->setPosition(drop->getPosition() + Vec2(0, 15));
                            critical->runAction(EaseIn::create(MoveBy::create(1, Vec2(0, 25)), 0.3));
                            critical->runAction(Sequence::create(DelayTime::create(0.7), FadeOut::create(0.3), CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
                            
                            shakeScreen(1);
                        }
                        
                        //                        totalPower += damage;
                        if (drop == theBoss) {
                            //                            GameManager::getInstance()->getEditorHud()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                        }else{
                            if(MovableArray.find(drop) != MovableArray.end()){
                                drop->desiredPosition = drop->desiredPosition + Vec2(0, 2);
                                drop->setPosition(drop->getPosition() + Vec2(0, 2));
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
void EditorWorld::makeGate(float dt){
    
}
void EditorWorld::bossExplode(float dt){
    if (rand()%2 == 0) {
        runEffect(EFFECT_EXPLODE_HUGE, theBoss->getPosition() + Vec2(rand()%20 - 10, rand()%20 - 30));
    }else{
        runEffect(EFFECT_BOMB_WALL, theBoss->getPosition() + Vec2(rand()%20 - 10, rand()%30 - 30));
    }
}
void EditorWorld::shakeScreenOnce(){
    shakeScreen(2);
}
void EditorWorld::shakeScreen(int count){
    Vector<FiniteTimeAction*> array;
    for (int i = 0; i < count; i++) {
        //        array.pushBack(CallFunc::create(CC_CALLBACK_0(EditorWorld::shakeScreenFirst, this)));
        //        array.pushBack(DelayTime::create(0.03));
        //        array.pushBack(CallFunc::create(CC_CALLBACK_0(EditorWorld::shakeScreenSecond, this)));
        //        array.pushBack(DelayTime::create(0.05));
        array.pushBack(MoveBy::create(0.07, Vec2(4, 0)));
        array.pushBack(MoveBy::create(0.07, Vec2(-4, 0)));
    }
    //    array.pushBack(CallFunc::create(CC_CALLBACK_0(EditorWorld::shakeScreenEnd, this)));
    //    array.pushBack(DelayTime::create(0.03));
    this->runAction(Sequence::create(array));
}
void EditorWorld::shakeScreenFirst(){
    //    extraCameraPos = Vec2((rand()%10)*0.1, (rand()%10)*0.1);
    extraCameraPos = Vec2(1, 0);
}
void EditorWorld::shakeScreenSecond(){
    //    extraCameraPos = Vec2(-(rand()%10)*0.1, -(rand()%10)*0.1);
    extraCameraPos = Vec2(-1, 0);
}
void EditorWorld::shakeScreenEnd(){
    extraCameraPos = Vec2::ZERO;
}
void EditorWorld::removeHero(EnemyBase* spt){
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
void EditorWorld::checkQuestDone(){
//    bool isDone = UDGetBool(KEY_CURRENT_QUEST_DONE, false);
//    if(!isDone && isQuestConditionMet()){
//        UDSetBool(KEY_CURRENT_QUEST_DONE, true);
//        if (currentQuestID == 9) {
//        }else if (currentQuestID == 10) {
//        }
//        this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(EditorHud::showQuestDoneAlert, EHUD)), NULL));
//    }
}
void EditorWorld::purgatoryClear(){
    
}
void EditorWorld::creatingStarToGreatBall(float dt){
    for(int i = 0; i < 3; i++){
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spt->setScale(0.1f);
        spt->setPosition(GM->getRandomPosInCicle(greatBall->getPosition(), 6));
        spriteBatch->addChild(spt, 100);
        spt->runAction(RepeatForever::create(Sequence::create(FadeTo::create(0.05f, 50 + rand()%50), DelayTime::create(0.01f*(rand()%40)), FadeTo::create(0.05f, 240), NULL)));
        float duration = 1 + (rand()%10)*0.2f;
        spt->runAction(Sequence::create(DelayTime::create(duration), SPT_REMOVE_FUNC, NULL));
        spt->runAction(MoveBy::create(duration, Vec2(0, 16 + rand()%10)));
    }
}
void EditorWorld::removeDeadUnit(EnemyBase* unit){
    if (unit->isEnemy) {
        totalKillUnit++;
    }else{
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
    if(unit->unitType == UNIT_ZOMBIE_ORC_AXE || unit->unitType == UNIT_ZOMBIE_SWORDSMAN){
        unit->removeFromParent();
    }else if(unit->isBuilding || unit->unitType == UNIT_CATAPULT){
        runEffect(EFFECT_EXPLODE_BIG, unit->getPosition());
        unit->removeFromParent();
    }else if(unit->unitType == UNIT_HELICOPTER){
        unit->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.5f, Vec2(200, 300)), 2), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, unit)), NULL));
        unit->setAnchorPoint(Vec2(0, 0));
        unit->runAction(EaseInOut::create(RotateBy::create(0.5f, 720), 2));
    }else{
        std::string name = unit->getName();
        if(unit->unitType == UNIT_WORKER){
            name = "worker";
        }
        float disappearTime = 3;
        float fadeTime = 3;
        if(unit->isZombie){
            disappearTime = 0.01f;
            fadeTime = 0.1f;
        }else{
            if(name.compare("tree") != 0){
                unit->setSpriteFrame(strmake("%sDead.png", name.c_str()));
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
void EditorWorld::removeEnemy(EnemyBase* spt){
    
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
    checkQuestDone();
    
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
    this->createRewards(spt);
    addHeroExp(0, spt->rewardExp);
    
    spiderArray.eraseObject(spt);
    if(spt->sptWeapon != nullptr)
        spt->sptWeapon->removeFromParent();
    spt->removeFromParentAndCleanup(true);
    
}

void EditorWorld::fixStageLayerFourTiles(experimental::TMXTiledMap* map){
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 2; x < map->getMapSize().width - 2; x++) {
        for (int rawY = 2; rawY < map->getMapSize().height - 2; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y)))) {
                if (compareFourTiles(0, 1, 1, 1, x, y, stageLayer)) {// top
                    stageLayer->setTileGID(6, Vec2(x, y));
                }else if (compareFourTiles(1, 0, 1, 1, x, y, stageLayer)) { // left
                    stageLayer->setTileGID(37, Vec2(x, y));
                }else if (compareFourTiles(1, 1, 0, 1, x, y, stageLayer)) { // right
                    stageLayer->setTileGID(39, Vec2(x, y));
                }else if (compareFourTiles(1, 1, 1, 0, x, y, stageLayer)) { // bottom
                    stageLayer->setTileGID(70, Vec2(x, y));
                }else if (compareFourTiles(0, 0, 1, 1, x, y, stageLayer)) { // left top
                    stageLayer->setTileGID(5, Vec2(x, y));
                }else if (compareFourTiles(0, 1, 0, 1, x, y, stageLayer)) { // right top
                    stageLayer->setTileGID(7, Vec2(x, y));
                }else if (compareFourTiles(1, 0, 1, 0, x, y, stageLayer)) { // left bottom
                    stageLayer->setTileGID(69, Vec2(x, y));
                }else if (compareFourTiles(1, 1, 0, 0, x, y, stageLayer)) { // right bottom
                    stageLayer->setTileGID(71, Vec2(x, y));
                }else if (compareFourTiles(1, 1, 1, 1, x, y, stageLayer)) {
                    stageLayer->setTileGID(38, Vec2(x, y));
                }
            }
        }
    }
}

bool EditorWorld::compareFourTiles(int t,int l, int r, int b, int x, int y, experimental::TMXLayer* stageLayer){
    bool result = true;
    if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y - 1))) != t) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x - 1, y))) != l) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x + 1, y))) != r) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y + 1))) != b) {
        return false;
    }
    return result;
}

void EditorWorld::fixStageLayerTiles(experimental::TMXTiledMap* map){
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    float y;
    for (int x = 1; x < map->getMapSize().width - 1; x++) {
        for (int rawY = 1; rawY < map->getMapSize().height - 1; rawY++) {
            y = map->getMapSize().height - rawY - 1;
            if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y)))) {
                if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // center
                    stageLayer->setTileGID(38, Vec2(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right top corner
                    stageLayer->setTileGID(7, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 0, x, y, stageLayer)) { // right corner
                    stageLayer->setTileGID(39, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 0, 0, 0, x, y, stageLayer)) { // right bottom corner
                    stageLayer->setTileGID(71, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 0, x, y, stageLayer)) { // bottom corner
                    stageLayer->setTileGID(70, Vec2(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 0, 0, x, y, stageLayer)) { // left bottom corner
                    stageLayer->setTileGID(69, Vec2(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left corner
                    stageLayer->setTileGID(37, Vec2(x, y));
                }else if (compareNineTiles(0, 0, 0, 0, 1, 0, 1, 1, x, y, stageLayer)) { // left top corner
                    stageLayer->setTileGID(5, Vec2(x, y));
                }else if (compareNineTiles(0, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // top corner
                    stageLayer->setTileGID(6, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 1, 1, x, y, stageLayer)) { // right top wall
                    stageLayer->setTileGID(15, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall
                    stageLayer->setTileGID(47, Vec2(x, y));
                }else if (compareNineTiles(0, 1, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // right bottom wall
                    stageLayer->setTileGID(79, Vec2(x, y));
                }else if (compareNineTiles(1, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall
                    stageLayer->setTileGID(78, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // left bottom wall
                    stageLayer->setTileGID(77, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall
                    stageLayer->setTileGID(45, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 1, 0, x, y, stageLayer)) { // left top wall
                    stageLayer->setTileGID(13, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 1, x, y, stageLayer)) { // top wall
                    stageLayer->setTileGID(14, Vec2(x, y));
                }else if (compareNineTiles(0, 1, 1, 0, 1, 1, 1, 1, x, y, stageLayer)) { // right wall top oepn
                    stageLayer->setTileGID(47, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 0, 1, 0, 1, 1, x, y, stageLayer)) { // right wall bottom open
                    stageLayer->setTileGID(47, Vec2(x, y));
                }else if (compareNineTiles(0, 0, 1, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall left open
                    stageLayer->setTileGID(78, Vec2(x, y));
                }else if (compareNineTiles(1, 0, 0, 1, 1, 1, 1, 1, x, y, stageLayer)) { // bottom wall right open
                    stageLayer->setTileGID(78, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 0, 1, 0, 1, 1, 1, x, y, stageLayer)) { // left wall top open
                    stageLayer->setTileGID(45, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 0, 1, 1, 0, x, y, stageLayer)) { // left wall bottom open
                    stageLayer->setTileGID(45, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 0, 0, 1, x, y, stageLayer)) { // top wall left open
                    stageLayer->setTileGID(14, Vec2(x, y));
                }else if (compareNineTiles(1, 1, 1, 1, 1, 1, 0, 0, x, y, stageLayer)) { // top wall right open
                    stageLayer->setTileGID(14, Vec2(x, y));
                }
            }
        }
    }
}
bool EditorWorld::compareNineTiles(int lt, int t, int rt, int l, int r, int lb, int b, int rb, int x, int y, experimental::TMXLayer* stageLayer){
    bool result = true;
    if (!isWay(stageLayer->getTileGIDAt(Vec2(x - 1, y - 1)) != lt)) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y - 1))) != t) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x + 1, y - 1))) != rt) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x - 1, y))) != l) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x + 1, y))) != r) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x - 1, y + 1))) != lb) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x, y + 1))) != b) {
        return false;
    }else if (!isWay(stageLayer->getTileGIDAt(Vec2(x + 1, y + 1))) != rb) {
        return false;
    }
    return result;
}
void EditorWorld::createRewards(EnemyBase* enemy){
    //    if (enemy->rewards <= 0 || enemy->rewards > 100000) {
    //        return;
    //    }
    // reward drop
    
    std::string dropName;
    int dropIndex;
    for(int i = 0; i < 3; i++){
        dropName = "";
        dropIndex = -1;
        for(int i = 0; i < 3; i++){
            if (rand()%100 < 50 - i*10) {
                dropIndex++;
            }else{
                break;
            }
        }
        if(dropIndex < 0)
            continue;
        
        dropName = getUnitDropItem(enemy->getName(), dropIndex);
        if(dropName.size() == 0){
            return;
        }
        std::string spriteName = getItemSpriteName(getItemType(dropName), dropName);
        Movable* sptItem = Movable::createMovable(UNIT_ITEM, 0, 0, spriteName.c_str());
        sptItem->setPosition(enemy->getPosition());
        sptItem->setName(dropName);
        sptItem->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(rand()%40 - 20, rand()%40 - 20)), 2));
        sptItem->runAction(EaseOut::create(RotateBy::create(0.3f, rand()%360), 2));
        spriteBatch->addChild(sptItem, 5);
        dropItemArray.pushBack(sptItem);
        sptItem->runAction(Sequence::create(DelayTime::create(5), Blink::create(1, 3), CallFuncN::create(CC_CALLBACK_1(EditorWorld::removeItemFromField, this)), NULL));
    }
    
    addHeroExp(0, enemy->rewardExp);
    EHUD->setExp(getHeroLevel(0), getHeroExp(0), getMaxExp(getHeroLevel(0)));
    showLabelFromPool(theMap, enemy->getPosition()+Vec2(0, 5), strmake("EXP +%d", enemy->rewardExp), 15, 0.2f);
}
void EditorWorld::backToLabelPool(Ref* ref){
    Label* lbl = (Label*)ref;
    lbl->removeFromParentAndCleanup(false);
}
Label* EditorWorld::showLabelFromPool(Node* parent, cocos2d::Vec2 pos, std::string text, int moveHeight, float delay){
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
    lbl->runAction(Sequence::create(DelayTime::create(delay), MoveBy::create(visibleTime+fadingTime, Vec2(0, moveHeight)), nullptr));
    lbl->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.1f), DelayTime::create(visibleTime), FadeOut::create(fadingTime), CallFuncN::create(CC_CALLBACK_1(EditorWorld::backToLabelPool,   this)), NULL));
    labelPoolIndex++;
    if (labelPoolIndex >= labelPoolCount) {
        labelPoolIndex = 0;
    }
    return lbl;
}
void EditorWorld::removeItemFromField(Ref* ref){
    Movable* sptItem = (Movable*)ref;
    dropItemArray.eraseObject(sptItem);
    coinArray.eraseObject(sptItem);
    sptItem->removeFromParent();
}
Sprite* EditorWorld::getLightSpin(float persistTime){
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
    shining2->setPosition(Vec2(shining->getContentSize().width/2, shining->getContentSize().height/2));
    
    return shining;
}
int EditorWorld::getSlotForWeapon(int weapon){
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

Sprite* EditorWorld::createNumberSprite(int number)
{
    //    int place = 1;
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
        sprite->setPosition(Vec2(startWidth + sprite->getContentSize().width/2, sprite->getContentSize().height/2));
        startWidth += (sprite->getContentSize().width - 1);
        first->addChild(sprite);
    }
    sprites.clear();
    return first;
}

void EditorWorld::onjewelryBoxOpen(Ref* sender){
    GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("jewel collected").c_str(), GameManager::getInstance()->getEditorHud());
    Movable* jewelryBox = (Movable*)sender;
    Sprite* lightSpin = getLightSpin(5);
    
    
    Sprite* sptWeapon = Sprite::createWithSpriteFrameName(__String::createWithFormat("jewel%d.png", jewelryBox->secondTag)->getCString());
    sptWeapon->setOpacity(0);
    lightSpin->setOpacity(0);
    sptWeapon->setPosition(jewelryBox->getPosition());
    lightSpin->setPosition(jewelryBox->getPosition());
    sptWeapon->runAction(FadeIn::create(0.3));
    lightSpin->runAction(FadeIn::create(0.3));
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Vec2(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Vec2(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    UserDefault::getInstance()->setBoolForKey(__String::createWithFormat(KEY_JEWEL_COLLECTED_FORMAT, jewelryBox->secondTag)->getCString(), true);
    //player->freezed = false;
    
    GameManager::getInstance()->collectedJewelIndex = jewelryBox->secondTag;
}
void EditorWorld::openWeapon(Ref* sender){
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
    sptWeapon->runAction(Sequence::create(MoveBy::create(1, Vec2(0, 18)), DelayTime::create(4), FadeOut::create(0.5), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptWeapon)), nullptr));
    lightSpin->runAction(MoveBy::create(1, Vec2(0, 18)));
    spriteBatch->addChild(sptWeapon, 1);
    this->addChild(lightSpin);
    
    if (getSlotForWeapon(weapon) < 0) {
        addWeapon(weapon);
    }else{
        recharge(getSlotForWeapon(weapon));
    }
}
void EditorWorld::recharge(int slot){
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
void EditorWorld::addWeapon(int weapon){
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
int EditorWorld::getTotalBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void EditorWorld::setTotalBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}
int EditorWorld::getLoadedBulletCountAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), 0);
}
void EditorWorld::setLoadedBulletCountAtSlot(int slot, int count){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat(KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT, slot)->getCString(), count);
}

void EditorWorld::changeWeapon(int slot){
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
    GameManager::getInstance()->getEditorHud()->setWeapon(weapon);
    GameManager::getInstance()->getEditorHud()->setBulletCount(loadedBulletCount,loadedBulletCountMax);
    GameManager::getInstance()->getEditorHud()->setTotalBulletCount(totalBulletCount, totalBulletCountMax);
}
void EditorWorld::removeLaser(){
    if (laser!=nullptr) {
        laser->removeFromParent();
        laser = nullptr;
        
    }
}
int EditorWorld::getWeaponAtSlot(int slot){
    return UserDefault::getInstance()->getIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), -1);
}
void EditorWorld::setWeaponAtSlot(int slot, int weapon){
    UserDefault::getInstance()->setIntegerForKey(__String::createWithFormat("WeaponInSlot%d", slot)->getCString(), weapon);
}
Sprite* EditorWorld::getShining(float delayTime){
    Sprite* shining = Sprite::createWithSpriteFrameName("shining0.png");
    Animation* animation = AnimationCache::getInstance()->getAnimation("shining");
    Animate* animate = Animate::create(animation);
    shining->runAction(Sequence::create(DelayTime::create(delayTime), MoveBy::create(0.7, Vec2(6, 6)), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), RotateBy::create(0.7, 250), nullptr));
    shining->runAction(Sequence::create(DelayTime::create(delayTime), animate, CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, shining)), NULL));
    
    return shining;
}
void EditorWorld::npcUpdate(float dt){
    
}
void EditorWorld::coinUpdate(float dt){//updateCoin
    
}
void EditorWorld::exitDungeon(){
    ValueVector rows = GameManager::getInstance()->split(strStage, "_");
    int mapIndex = rows.at(0).asInt();
    int x = rows.at(1).asInt();
    int y = rows.at(2).asInt();
    
    if (mapIndex == 1 && x == 2 && y == 4) {
        mapIndex = 0; x = 2; y = 4;
    }
    
    UD->setStringForKey(KEY_LAST_SAVE_POINT, strmake("%d_%d_%d", mapIndex, x, y));
//    UD->setIntegerForKey(KEY_NEXT_SPAWN_DIRECTION, DIRECTION_STAY);
//    GM->mapMoveRequested = true;
    //GM->getEditorHud()->showBIAndReplaceScene();
}
void EditorWorld::enterDungeon(){
    ValueVector rows = GameManager::getInstance()->split(strStage, "_");
    int mapIndex = rows.at(0).asInt();
    int x = rows.at(1).asInt();
    int y = rows.at(2).asInt();
    
    if (mapIndex == 0 && x == 2 && y == 4) {
        mapIndex = 1; x = 2; y = 4;
    }else if (mapIndex == 0 && x == 2 && y == 0) {
        mapIndex = 3; x = 2; y = 4;
    }
    
    UD->setStringForKey(KEY_LAST_SAVE_POINT, strmake("%d_%d_%d", mapIndex, x, y));
//    UD->setIntegerForKey(KEY_NEXT_SPAWN_DIRECTION, DIRECTION_N);
//    GM->mapMoveRequested = true;
    //GM->getEditorHud()->showBIAndReplaceScene();
}
void EditorWorld::exitMap(Node* exit){
//    if(exit == lastExit) return;
//    lastExit = exit;
//    //    if(exit->getTag() == DIRECTION_E && strStage.compare("0_4_2") == 0){
//    //        GM->getEditorHud()->showInstanceMessage(LM->getText("map not ready"));
//    //        return;
//    //    }
//
//    ValueVector rows = GameManager::getInstance()->split(strStage, "_");
//    int mapIndex = rows.at(0).asInt();
//    int x = rows.at(1).asInt();
//    int y = rows.at(2).asInt();
//    if (mapIndex == 1 && x == 2 && y == 4 && exit->getTag() == DIRECTION_N) {
//        exitDungeon();
//        return;
//    }
//    int nextDirection;
//    if(exit->getTag() == DIRECTION_N){
//        y++;
//        nextDirection = DIRECTION_S;
//    }else if(exit->getTag() == DIRECTION_E){
//        x++;
//        nextDirection = DIRECTION_W;
//    }else if(exit->getTag() == DIRECTION_S){
//        y--;
//        nextDirection = DIRECTION_N;
//    }else if(exit->getTag() == DIRECTION_W){
//        x--;
//        nextDirection = DIRECTION_E;
//    }
//    UD->setStringForKey(KEY_LAST_SAVE_POINT, strmake("%d_%d_%d", mapIndex, x, y));
//    UD->setIntegerForKey(KEY_NEXT_SPAWN_DIRECTION, nextDirection);
//    GM->mapMoveRequested = true;
    //GM->getEditorHud()->showBIAndReplaceScene();
}
void EditorWorld::teleportLaterForThemeThree(float dt){
    
}
void EditorWorld::teleportLater(float dt){
    
}

void EditorWorld::jumpBySpring(float dt)
{
}
void EditorWorld::runEffect(int effect, Vec2 point, float angle){
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
            explode->runAction(Sequence::create(MoveBy::create(duration, Vec2(rand()%(radius*2) - radius, rand()%(radius*2) - radius)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)), NULL));
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
            explode->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, 10)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, explode)), NULL));
            explode->runAction(Sequence::create(DelayTime::create(duration/4), FadeTo::create(duration/4, 100), DelayTime::create(duration/4), FadeOut::create(duration/4), nullptr));
            //            explode->setScale(0.8);
            explode->runAction(RotateBy::create(duration, rand()%260 + 250));
            explode->setPosition(point + Vec2(rand()%(radius*2) - radius, rand()%(radius*2) - radius));
        }
    }else if (effect == EFFECT_EXPLODE_MIDDLE) {
        Sprite* spt = Sprite::createWithSpriteFrameName("cartoonyFastExplosion0.png");
        spriteBatchEffect->addChild(spt, 5);
        spt->setPosition(point);
        GM->runAnimation(spt, "cartoonyExplosion", false);
        spt->runAction(Sequence::create(DelayTime::create(0.04*14), SPT_REMOVE_FUNC, nullptr));
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
        spt->setPosition(point + Vec2(4-rand()%8, 4));
        spt->setScale((rand()%5 + 5)*0.1f);
        spt->setOpacity(100);
        //        spt->setRotation(rand()%360);
        int num = 150 + rand()%100;
        spt->setColor(Color3B(num,num,num));
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Vec2(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this)), NULL));
        spt->runAction(EaseOut::create(MoveBy::create(duration, Vec2(8 - rand()%16, 0)), 4));
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
    }
    
    return;
    if(effect == EFFECT_BULLET_WALL_SMALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bulletSmallEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        explode->setRotation(angle);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bulletSmallEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
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
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
        
    }else if(effect == EFFECT_BOMB_WALL){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffectWall0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffectWall");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
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
        explode->runAction(Sequence::create(DelayTime::create(duration*2), CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
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
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(baseDuration, Vec2(x, y)), easeRate), CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
        }
    }else if(effect == EFFECT_EXPLODE_HUGE){
        Sprite* explode = Sprite::createWithSpriteFrameName("bombEffect0.png");
        spriteBatch->addChild(explode);
        explode->setPosition(point);
        Animation* animation = AnimationCache::getInstance()->getAnimation("bombEffect");
        Animate* animate = Animate::create(animation);
        explode->runAction(Sequence::create(animate, CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
    }else if(effect == EFFECT_GREEN_SMOKE){
        float duration = 0.5;
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteExplode.png");
        spriteBatch->addChild(spt, -1);
        spt->setPosition(point);
        spt->setRotation(rand()%360);
        spt->setScale(0.3);
        spt->setColor(Color3B(130,250,130));
        spt->runAction(Sequence::create(EaseIn::create(ScaleTo::create(duration, 0.05), 2), CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this)), NULL));
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
        spt->runAction(Sequence::create(EaseIn::create(MoveBy::create(duration, Vec2(0, 7)), 2), CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this)), NULL));
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
            particle->runAction(Sequence::create(EaseOut::create(MoveBy::create(duration, Vec2(x, y)), 1.6f), CallFuncN::create(CC_CALLBACK_1(EditorWorld::spriteMoveDone, this)), NULL));
        }
    }
}
void EditorWorld::runEffect(int effect, Vec2 point)
{
    runEffect(effect, point, 0);
}
/*
 void EditorWorld::addGlowEffect(Sprite* sprite,const Color3B& colour, const cocos2d::Size& size)
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
 
 glowSprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(scale1, scale2, NULL)));
 }*/
void EditorWorld::powerTestSchedule(float dt){
    powerTestTime += dt;
    if(powerTestTime > 10){
        powerTestTime = 0;
        totalPower = 0;
    }
    powerPerSec = totalPower/powerTestTime;
    CCLOG("power/sec: %f", powerPerSec);
}
Sprite* EditorWorld::getLight(){
    Sprite* sptLight = Sprite::create("whiteBigCircle.png");
    BlendFunc f = {GL_DST_COLOR, GL_ONE};
    sptLight->setBlendFunc(f);
    sptLight->setOpacity(255);
    sptLight->setColor(Color3B(255, 180, 0));
    sptLight->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.0, 0.9), DelayTime::create(0.3), ScaleTo::create(0.0, 1), DelayTime::create(0.3), nullptr)));
    return sptLight;
}
void EditorWorld::setStageCoverOpacity(int opacity){
    for (int i = 0; i < theMap->getMapSize().width; i++) {
        for (int j = 0; j < theMap->getMapSize().height; j++) {
            Sprite* spt = stageCover->getTileAt(Vec2(i, j));
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
void EditorWorld::setLayerTag(experimental::TMXTiledMap* map){
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
        //        spriteBatch->removeFromParentAndCleanup(false);
        //        map->addChild(spriteBatch);
        
        stageCover->removeFromParentAndCleanup(false);
        this->addChild(stageCover, 1);
        stageCover->setPosition(stageCover->getPosition() + theMap->getPosition());
    }
}
void EditorWorld::setBossMap(int stage){
    int tilesetIndex = rand()%3;
    tilesetIndex = 2;
    
    char buf[50];
    
    //    GameManager::getInstance()->getEditorHud()->showStageTitle();
    stageIndex = stage;
    
    cocos2d::Rect rect;
    
    experimental::TMXTiledMap* map = cocos2d::experimental::TMXTiledMap::create(buf);
    
    mapArray.pushBack(map);
    setLayerTag(map);
    this->addChild(map);
    mapRectArray.push_back(map->getBoundingBox());
    rect = map->getBoundingBox();
    map->setTag(1);
    setStage(map);
    
    if(stage == STAGE_LOBBY){
        setLobby(map);
    }
    
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
void EditorWorld::setEntireMap(int stage){
    //    Sprite* sptRect = Sprite::create("res/258_gray_rect.png");
    //    this->addChild(sptRect);
    //    sptRect->setPosition(Vec2(128, 128));
    bool shouldBeNodeGrid = false;
    theMap = cocos2d::experimental::TMXTiledMap::create("stage_70_templete.tmx");
    mapSize = theMap->getMapSize();
    mapWidth = mapSize.width*TILE_SIZE;
    mapHeight = mapSize.height*TILE_SIZE;
    
    GM->astar->setMap(mapSize.width, mapSize.height);
    
    setLayerTag(theMap);
    setStage(theMap);
    // minimap setting
    drawMiniMapFrame = DrawNode::create();
    EHUD->addChild(drawMiniMapFrame, 100);
    drawMiniMapForNonMoving = DrawNode::create();
    EHUD->addChild(drawMiniMapForNonMoving, 101);
    drawMiniMapForMoving = DrawNode::create();
    EHUD->addChild(drawMiniMapForMoving, 102);
    log("size: %f", size.width);
    int offsetX = 0;
    if(size.height/size.width < 700.0f/1334.0f){
        offsetX = 100;
    }
    miniMapFrameWidth = 400;
    miniMapFrameHeight = 400;
    int offset = 20;
    miniMapStartPos = cocos2d::Vec2(offsetX + 20, 20);
    drawMiniMapFrame->setPosition(miniMapStartPos);
    drawMiniMapForNonMoving->setPosition(miniMapStartPos);
    drawMiniMapForMoving->setPosition(miniMapStartPos);
    
    ImageView* img = ImageView::create("uiBox.png");
    EHUD->addChild(img, 99);
    img->setScale9Enabled(true);
    img->setContentSize(cocos2d::Size(miniMapFrameWidth + offset*2 + 5, miniMapFrameHeight + offset*2 + 5));
    img->setAnchorPoint(Vec2(0, 0));
    img->setPosition(miniMapStartPos - Vec2(offset, offset));
    if (GM->nextScene == STAGE_INTRO) {
        img->setVisible(false);
        drawMiniMapFrame->setVisible(false);
        drawMiniMapForNonMoving->setVisible(false);
        drawMiniMapForMoving->setVisible(false);
    }
    if (mapSize.width/mapSize.height < miniMapFrameWidth/miniMapFrameHeight) { // fix height
        miniMapScale = miniMapFrameHeight/(mapSize.height*TILE_SIZE);
        miniMapDrawStartPos = Vec2(miniMapFrameWidth/2 - mapSize.width*TILE_SIZE*miniMapScale/2, 0);
        miniMapHeight = miniMapFrameHeight;
        miniMapWidth = miniMapHeight*mapSize.width/mapSize.height;
    }else{ // fix width
        miniMapScale = miniMapFrameWidth/(mapSize.width*TILE_SIZE);
        miniMapDrawStartPos = Vec2(0, miniMapFrameHeight/2 - mapSize.height*TILE_SIZE*miniMapScale/2);
        miniMapWidth = miniMapFrameWidth;
        miniMapHeight = miniMapWidth*mapSize.height/mapSize.width;
    }
    miniMapBit = miniMapScale*TILE_SIZE;
    drawMiniMapFrame->drawSolidRect(Vec2::ZERO, Vec2(miniMapFrameWidth, miniMapFrameHeight), Color4F::GRAY);
    drawMiniMapFrame->drawSolidRect(miniMapDrawStartPos, miniMapDrawStartPos + Vec2(miniMapWidth, miniMapHeight), Color4F(0.4f, 0.4f, 0.4f, 1));
    miniMapViewRect = cocos2d::Rect(miniMapDrawStartPos.x + (-getPosition().x)*miniMapScale, miniMapDrawStartPos.y + (-getPosition().y)*miniMapScale, size.width*miniMapScale, size.height*miniMapScale);
    updateMiniMapForNonMoving();
    
    //    if(shouldBeNodeGrid){
    //        nodeGrid->addChild(theMap);
    //    }else{
    //        this->addChild(theMap);
    //    }
    this->addChild(theMap);
    moveScreen(getPosition());
}
void EditorWorld::loadMapData(){
    ValueVector entireList = GM->split(GM->loadMapData, "_");
    ValueVector smallList = GM->split(entireList.at(0).asString(), "/");// map size
    mapSizeWidth = smallList.at(0).asInt();
    mapSizeHeight = smallList.at(1).asInt();
    
    smallList = GM->split(entireList.at(1).asString(), "|");// tree
    
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
            brushTile(BRUSH_TREE, Vec2(y, x));
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
            Vec2 pos = getPositionFromTileCoordinate(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
            int unitType =unitInfoList.at(0).asInt();
            EnemyBase* unit = createUnit(unitType, WHICH_SIDE_HERO, GM->isThisBuilding(unitInfoList.at(0).asInt()), pos, "worker", 1, getSpriteNameForUnit(unitType));
            unit->setTag(unitInfoList.at(1).asInt() + unitInfoList.at(2).asInt()*mapSizeWidth);
            
            if (GM->isThisBuilding(unitType)) {
                cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
                Vec2 coordinate = getCoordinateFromPosition(pos);
                setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y), occupySize.width, occupySize.height, true, unit);
                unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
                unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
//                setAfterBuildingProcess(unit);
                
//                if (unitType == UNIT_CASTLE) {
//                    this->setPosition(-pos*layerScale + Vec2(size.width/2 + TILE_SIZE*layerScale, size.height/2));
//                }
//                unit->setPosition(pos + Vec2(TILE_SIZE*occupySize.width/2, -TILE_SIZE*occupySize.height/2));
                
//                unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
//                spt->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
//                this->addChild(spt, 1000);
//                spt->setScale(0.1f);
//                log("anchorPoint: %f, %f", unit->getAnchorPoint().x, unit->getAnchorPoint().y);
            }
        }
    }
    
    smallList = GM->split(entireList.at(3).asString(), "|"); // mutual
    for (int i = 0; i < smallList.size(); i++) {
        unitInfoList = GM->split(smallList.at(i).asString(), "/");
        if (unitInfoList.size() > 1) {
            Vec2 pos = getPositionFromTileCoordinate(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
            int unitType =unitInfoList.at(0).asInt();
            EnemyBase* unit = createUnit(unitInfoList.at(0).asInt(), WHICH_SIDE_READY_HERO, GM->isThisBuilding(unitInfoList.at(0).asInt()), pos, "worker", 1, getSpriteNameForUnit(unitInfoList.at(0).asInt()));
            unit->setTag(unitInfoList.at(1).asInt() + unitInfoList.at(2).asInt()*mapSizeWidth);
            
            if (GM->isThisBuilding(unitType)) {
                cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
                Vec2 coordinate = getCoordinateFromPosition(pos);
                setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y), occupySize.width, occupySize.height, true, unit);
                unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
                unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }
    
    smallList = GM->split(entireList.at(4).asString(), "|"); // enemy
    for (int i = 0; i < smallList.size(); i++) {
        unitInfoList = GM->split(smallList.at(i).asString(), "/");
        if (unitInfoList.size() > 1) {
            Vec2 pos = getPositionFromTileCoordinate(unitInfoList.at(3).asInt(), unitInfoList.at(4).asInt());
            int unitType =unitInfoList.at(0).asInt();
            EnemyBase* unit = createUnit(unitInfoList.at(0).asInt(), WHICH_SIDE_ENEMY, GM->isThisBuilding(unitInfoList.at(0).asInt()), pos, "worker", 1, getSpriteNameForUnit(unitInfoList.at(0).asInt()));
            unit->unitAct = unitInfoList.at(1).asInt();
            unit->scheduledAttackTime = unitInfoList.at(2).asInt()*60;
            unit->setTag(unitInfoList.at(3).asInt() + unitInfoList.at(4).asInt()*mapSizeWidth);
            if (GM->isThisBuilding(unitType)) {
                cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
                Vec2 coordinate = getCoordinateFromPosition(pos);
                setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y), occupySize.width, occupySize.height, true, unit);
                unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
                unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }
    
    smallList = GM->split(entireList.at(5).asString(), "|"); // mutual
    for (int i = 0; i < smallList.size(); i++) {
        unitInfoList = GM->split(smallList.at(i).asString(), "/");
        if (unitInfoList.size() > 1) {
            Vec2 pos = getPositionFromTileCoordinate(unitInfoList.at(1).asInt(), unitInfoList.at(2).asInt());
            int unitType =unitInfoList.at(0).asInt();
            EnemyBase* unit = createUnit(unitInfoList.at(0).asInt(), WHICH_SIDE_MUTUAL, GM->isThisBuilding(unitInfoList.at(0).asInt()), pos, "worker", 1, getSpriteNameForUnit(unitInfoList.at(0).asInt()));
            unit->setTag(unitInfoList.at(1).asInt() + unitInfoList.at(2).asInt()*mapSizeWidth);
            if (GM->isThisBuilding(unitType)) {
                cocos2d::Size occupySize = GM->getBuildingOccupySize(unitType);
                Vec2 coordinate = getCoordinateFromPosition(pos);
                setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y), occupySize.width, occupySize.height, true, unit);
                unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
                unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            }
        }
    }
    
    GM->loadMapData = "";
    updateMiniMapForNonMoving();
}
void EditorWorld::purgatoryUpdate(float dt){
    int startX = 8*TILE_SIZE;
    Vec2 pos = Vec2(startX, (16-(-6)-1)*TILE_SIZE) + Vec2(-TILE_SIZE/2, TILE_SIZE/2);
    
    for (int i = 0; i < 4; i++) {
        Sprite* spt = Sprite::createWithSpriteFrameName("whiteCircle.png");
        spt->setScale(0.1f, 0.2f);
        //        spt->runAction(RepeatForever::create(Waves::create(1, cocos2d::Size(15, 15), 4, 4, 10, true)));
        pos.x = 0;
        int color = 50 + rand()%150;
        spt->setColor(Color3B(color, color, color));
        int x = (11 - rand()%22)*TILE_SIZE;
        int midX = x*2.5f;
        pos.x = startX;
        x += startX;
        midX += startX;
        spt->setPosition(pos);
        Vec2 targetPos = Vec2(x, (16-(18)-1)*TILE_SIZE) + Vec2(-TILE_SIZE/2, TILE_SIZE/2);
        //        spt->runAction(Sequence::create(MoveTo::create(1, targetPos), SPT_REMOVE_FUNC, NULL));
        if(i == 0){
            spriteBatch->addChild(spt);
            spt->setOpacity(150);
        }else{
            
        }
        
        ccBezierConfig config;
        config.controlPoint_1 = pos;//ccp(0 , 0); //開始位置
        config.controlPoint_2 = Vec2(midX, (16-(6)-1)*TILE_SIZE);//ccp(0 , -100);  //曲線のベクトル(というのかな？)
        config.endPosition = targetPos;//ccp(100,100);   //終了位置
        spt->runAction(Sequence::create(BezierTo::create(1, config), SPT_REMOVE_FUNC, NULL));
        spt->runAction(ScaleTo::create(1, 1, 2));
    }
}
void EditorWorld::tournamentSchedule(float dt){
    if (enemyArray.size() > 0) {
        return;
    }
    
    if (tournamentWaveIndex == 0) {
        
    }
}
void EditorWorld::addEnemiesToMap(experimental::TMXTiledMap* map, int levelScore, bool blueKey){
    Vec2 pos;
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
            //pos = Vec2(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 3);
            pos = Vec2(3 + rand()%(int)(map->getMapSize().width - 6), 4 + rand()%(int)(map->getMapSize().height - 6));
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
                pos = Vec2(pos.x, pos.y + 1);
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
    
    //    this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(EditorWorld::loadEnemies, this)), NULL));
    map->getLayer("deco")->setVisible(true);
    
    // add Destructable
    tryCount = 0;
    tryCountMax = map->getMapSize().width*map->getMapSize().height/15;
    bool found = false;
    while(tryCount < tryCountMax){
        found = false;
        while(tryCount < tryCountMax){
            tryCount++;
            pos = Vec2(3 + rand()%(int)(map->getMapSize().width - 6), map->getMapSize().height/2 + rand()%(int)(map->getMapSize().height/2) - 2);
            int tileIndex = map->getLayer("stage")->getTileGIDAt(Vec2(pos.x, pos.y + 1));
            if ((!isWay(tileIndex) || isOneWay(tileIndex)) && map->getLayer("stage")->getTileGIDAt(pos) == 0 && tileIndex != 322) {
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        
        Vec2 thePos = map->getPosition() + Vec2(pos.x*TILE_SIZE + rand()%TILE_SIZE, (map->getMapSize().height - pos.y - 1)*TILE_SIZE + TILE_SIZE/2 - 2);
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
void EditorWorld::setEmptyMap(experimental::TMXTiledMap* map){
    int mapSizeX = map->getMapSize().width;
    int mapSizeY = map->getMapSize().height;
    
    experimental::TMXLayer* stageLayer = map->getLayer("stage");
    //    experimental::TMXLayer* decoLayer = map->getLayer("deco");
    int currentX = 3;
    int currentY = 4;
    int squareWidth = 10;
    Vec2 pos;
    
    while(currentY < mapSizeY - 3){
        while(currentX < mapSizeX - 3){
            pos = Vec2(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            pos = Vec2(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(34, pos);
            }
            
            pos = Vec2(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            pos = Vec2(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
            if (pos.x < map->getMapSize().width - 3 && pos.y < map->getMapSize().height - 3) {
                stageLayer->setTileGID(130, pos);
            }
            
            /*
             if (rand()%100 < 20) {
             pos = Vec2(currentX + rand()%squareWidth, currentY + rand()%squareWidth);
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

FireableBase* EditorWorld::addEnemyToLoadStack(experimental::TMXTiledMap* map, int levelScore, Vec2 pos, int missile, int enemyModel, bool addGround){
    
    return nullptr;
}
void EditorWorld::loadEnemies(){
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
void EditorWorld::addAppearEffect(Sprite* parent){
    Sprite* sptEffect = Sprite::createWithSpriteFrameName("bombEffect0.png");
    sptEffect->setPosition(Vec2(parent->getContentSize().width/2, parent->getContentSize().height/2));
    sptEffect->runAction(RotateBy::create(1, 400));
    sptEffect->setScale(0.5);
    sptEffect->runAction(Sequence::create(ScaleTo::create(0.3, 1.5), ScaleTo::create(0.2, 0.1), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptEffect)), NULL));
    parent->addChild(sptEffect);
}
void EditorWorld::updateMiniMapForMoving(){
    drawMiniMapForMoving->clear();
    
    Vec2 startPos;
    for (auto unit: heroArray) {
        if(!unit->isBuilding){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit/2, miniMapBit/2);
            if (unit->isAlli) {
                drawMiniMapForMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::YELLOW);
            }else{
                drawMiniMapForMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::GREEN);
            }
        }
    }
    for (auto unit: enemyArray) {
        if(!unit->isBuilding && unit->isVisible()){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit/2, miniMapBit/2);
            drawMiniMapForMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::RED);
        }
    }
    for (auto unit: readyHeroArray) {
        if(!unit->isBuilding && unit->isVisible()){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit/2, miniMapBit/2);
            drawMiniMapForMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F::YELLOW);
        }
    }
    drawMiniMapForMoving->drawRect(miniMapViewRect.origin, Vec2(miniMapViewRect.getMaxX(), miniMapViewRect.getMaxY()), Color4F::WHITE);
}

void EditorWorld::updateMiniMapForNonMoving(){
    drawMiniMapForNonMoving->clear();
    
    Vec2 startPos;
    for (int i = 0; i < mapSize.width; i ++) {
        for (int j = 0; j < mapSize.height; j++) {
            if (decoLayer->getTileGIDAt(Vec2(i, j)) == 49) {
                startPos = miniMapDrawStartPos + Vec2(i, mapSize.height - j - 1)*TILE_SIZE*miniMapScale;
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit, miniMapBit), Color4F(0.2f, 0.2f, 0.2f, 1));
            }
        }
    }

    for (auto unit: readyHeroArray) {
        if(!unit->isBuilding) continue;
//        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::YELLOW);
//        }
    }
    for (auto unit: mutualArray) {
        if(!unit->isBuilding) continue;
//        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::YELLOW);
//        }
    }
    for (auto unit: heroArray) {
        if(!unit->isBuilding) continue;
//        fogCoordinate = Vec2(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
//        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
//        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            
            if (unit->isAlli) {
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::YELLOW);
            }else{
                drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::GREEN);
            }
//        }
    }
    for (auto unit: enemyArray) {
        if(!unit->isBuilding) continue;
//        fogCoordinate = Vec2(unit->getPositionX()/FOG_SIZE, unit->getPositionY()/FOG_SIZE);
//        fogAboveUnit = fogArray.at((int)fogCoordinate.x + (int)fogCoordinate.y*(int)fogMapSize.width);
//        if (fogAboveUnit->appliedState > FOG_SEEN_NOT_NOW && unit->isDetected){
            startPos = miniMapDrawStartPos + unit->getPosition()*miniMapScale - Vec2(miniMapBit*unit->buildingOccupySize.width/2, miniMapBit*unit->buildingOccupySize.width/2);
            drawMiniMapForNonMoving->drawSolidRect(startPos, startPos + Vec2(miniMapBit*unit->buildingOccupySize.width, miniMapBit*unit->buildingOccupySize.width), Color4F::RED);
//        }
    }
}
void EditorWorld::addMapToMiniMap(experimental::TMXTiledMap* map){
}
void EditorWorld::setNamingRoom(experimental::TMXTiledMap* map){
    
}
void EditorWorld::setLobby(experimental::TMXTiledMap* map){
    //    if (UserDefault::getInstance()->getBoolForKey(KEY_IS_TUTORIAL_CLEAR, false)) {  // naming
    //        map->getLayer("stage")->removeTileAt(Vec2(21, 17));
    //        map->getLayer("stage")->removeTileAt(Vec2(22, 17));
    //        map->getLayer("stage")->removeTileAt(Vec2(23, 17));
    //        map->getLayer("stage")->removeTileAt(Vec2(21, 18));
    //        map->getLayer("stage")->removeTileAt(Vec2(22, 18));
    //        map->getLayer("stage")->removeTileAt(Vec2(23, 18));
    //    }
    for (int i = 0; i < WEAPON_NO_MORE; i++) {
        setWeaponAtSlot(i, WEAPON_NOT_EXIST);
    }
    GameManager::getInstance()->isWeaponSet = false;
    changeWeapon(0);
    GameManager::getInstance()->totalKill = 0;
    GameManager::getInstance()->totalDestroy=0;
    GameManager::getInstance()->totalCoin=0;
    GameManager::getInstance()->totalRoom=0;
    GameManager::getInstance()->totalBoss=0;
    GameManager::getInstance()->totalTime=60*60;
    GameManager::getInstance()->totalScore = 0;
    GameManager::getInstance()->jewelryCollected = false;
    
}
bool EditorWorld::isRoomEmpty(cocos2d::Rect rect){
    cocos2d::Rect targetRect;
    for (int i = 0; i < mapArray.size(); i++) {
        targetRect = mapArray.at(i)->getBoundingBox();
        
        if (intersectsRect(rect, targetRect)) {
            return false;
        }
        
    }
    return true;
}
experimental::TMXTiledMap* EditorWorld::findDirectionAndPlaceTheMap(experimental::TMXTiledMap* srcMap, experimental::TMXTiledMap* dstMap){
    bool found = false;
    int directionToFind = rand()%4;
    int directionCounter = 0;
    cocos2d::Rect nextRect;
    cocos2d::Rect srcRect;
    cocos2d::Size dstSize = dstMap->getContentSize();
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
            nextRect = cocos2d::Rect(srcRect.getMaxX(), srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 1) { // check bottom
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            
            nextRect = cocos2d::Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMinY() - dstSize.height, dstSize.width, dstSize.height);
        }else if (directionToFind == 2) { // check left
            randomData = (rand()%(int)dstMap->getMapSize().height);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().height - 2){
                randomData = dstMap->getMapSize().height - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = cocos2d::Rect(srcRect.getMinX() - dstSize.width, srcRect.origin.y + ((int)srcMap->getMapSize().height/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, dstSize.width, dstSize.height);
        }else if (directionToFind == 3) { // check top
            randomData = (rand()%(int)dstMap->getMapSize().width);
            if (randomData <= 1) {
                randomData = 2;
            }else if(randomData >= dstMap->getMapSize().width - 2){
                randomData = dstMap->getMapSize().width - 3;
            }
            randomData *= TILE_SIZE;
            nextRect = cocos2d::Rect( srcRect.origin.x + ((int)srcMap->getMapSize().width/2)*TILE_SIZE - randomData - randomData%TILE_SIZE, srcRect.getMaxY(), dstSize.width, dstSize.height);
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
                    if (srcMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Vec2(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Vec2(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Vec2(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(69, Vec2(x, y - 1));
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
                    if (dstMap->getLayer("stage")->getTileGIDAt(Vec2(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Vec2(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Vec2(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(7, Vec2(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Vec2(x, y - 1));
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
                    if (srcMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Vec2(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Vec2(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(5, Vec2(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(7, Vec2(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
                    if (dstMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Vec2(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Vec2(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(69, Vec2(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(71, Vec2(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
                    if (dstMap->getLayer("stage")->getTileGIDAt(Vec2(x, y)) == 0 && dstMap->getMapSize().height > y &&
                        dstMap->getLayer("stage")->getTileGIDAt(Vec2(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            dstMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
                    if (srcMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(6, Vec2(x, y + 1));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(70, Vec2(x, y - 1));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(7, Vec2(x, y + 1));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Vec2(x, y - 1));
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
                    if (dstMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        dstMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(6, Vec2(x, y + 1));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(70, Vec2(x, y - 1));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Vec2(x, y + 1));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(69, Vec2(x, y - 1));
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
                    if (srcMap->getLayer("stage")->getTileGIDAt(Vec2(x, y))) {
                        srcMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            srcMap->getLayer("stage")->setTileGID(37, Vec2(x + 1, y));
                        }else if (counter == 1) {
                            srcMap->getLayer("stage")->setTileGID(39, Vec2(x - 1, y));
                        }else if (counter == 2) {
                            srcMap->getLayer("stage")->setTileGID(69, Vec2(x + 1, y));
                        }else if (counter == 3) {
                            srcMap->getLayer("stage")->setTileGID(71, Vec2(x - 1, y));
                        }
                    }else{
                        
                        break;
                    }
                    if (counter==1) {
                        srcMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
                    if (srcMap->getLayer("stage")->getTileGIDAt(Vec2(x, y)) == 0 && y < srcMap->getMapSize().height &&
                        srcMap->getLayer("stage")->getTileGIDAt(Vec2(x - 1, y)) == 0) {
                        if (counter >= 3) {
                            srcMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
                    if (dstMap->getLayer("stage")->getTileGIDAt(Vec2(x, y)) && y > 0) {
                        dstMap->getLayer("stage")->setTileGID(0, Vec2(x, y));
                        if (counter == 0) {
                            dstMap->getLayer("stage")->setTileGID(37, Vec2(x + 1, y));
                        }else if (counter == 1) {
                            dstMap->getLayer("stage")->setTileGID(39, Vec2(x - 1, y));
                        }else if (counter == 2) {
                            dstMap->getLayer("stage")->setTileGID(5, Vec2(x + 1, y));
                        }else if (counter == 3) {
                            dstMap->getLayer("stage")->setTileGID(7, Vec2(x - 1, y));
                        }
                    }else{
                        break;
                    }
                    
                    if (counter==1) {
                        dstMap->getLayer("stage")->setTileGID(130, Vec2(x, y));
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
float EditorWorld::getMin(float src, float dst){
    return src < dst?src:dst;
}
float EditorWorld::getMax(float src, float dst){
    return src > dst?src:dst;
}
void EditorWorld::setStage(experimental::TMXTiledMap* tileMap)
{
    GM->mapSize = tileMap->getMapSize();
    
    if (tileMap->getTag() == MAP_SHOP) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("found a shop").c_str(), GameManager::getInstance()->getEditorHud());
    }else if (tileMap->getTag() == MAP_WEAPON) {
        GameManager::getInstance()->showDisposableMessage(LanguageManager::getInstance()->getText("suitcase room").c_str(), GameManager::getInstance()->getEditorHud());
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
    Vec2 checkPoint = Vec2(14, 8);
    if(rows.size() > 2){
        mapIndex = rows.at(0).asInt();
        x = rows.at(1).asInt();
        y = rows.at(2).asInt();
    }
    
    // change coin tile into coin animation
    int totalWidth = tileMap->getMapSize().width;
    int totalHeight = tileMap->getMapSize().height;
    Vec2 point;
    int talkCount = 0;
    
    experimental::TMXLayer* stageLayer = tileMap->getLayer("stage");
    experimental::TMXLayer* soilLayer = tileMap->getLayer("soil");
    experimental::TMXLayer* unitLayer = tileMap->getLayer("unit");
    decoLayer = tileMap->getLayer("deco");
    decoLayer->setVisible(false);// test
    //    experimental::TMXLayer* foreLayer = tileMap->getLayer("fore");«
    experimental::TMXLayer* backLayer = tileMap->getLayer("back");
    PointArray* pointArrayToRemoveInUnitLayer = PointArray::create(3000);
    
    for (int i = 0; i < tileMap->getMapSize().width; i++) {
        for (int j = 0; j < tileMap->getMapSize().height; j++) {
            if (!isWay(stageLayer->getTileGIDAt(Vec2(i, j)))) {
                stageTileRectArray.push_back(cocos2d::Rect(i*TILE_SIZE + tileMap->getPositionX(), (tileMap->getMapSize().height - j - 1)*TILE_SIZE + tileMap->getPositionY(), TILE_SIZE, TILE_SIZE));
            }
        }
    }
    
    Vec2 cameraPos = Vec2::ZERO;
    for (int i = 0; i < totalWidth; i++) {
        for (int j = 0; j < totalHeight; j++) {
            point = Vec2(i,j);
            int gid = unitLayer->getTileGIDAt(point);
            Vec2 pos = getPositionFromTileCoordinate(i, j);
            if (gid) {
                //                if(!keyExist(map, "MovingPlatformLine") &&
                //                   !keyExist(map, "MovingPlatformEnd")){
                //                    unitLayer->removeTileAt(point);
                //                }
                EnemyBase* enemy = NULL;
                FireableBase* fEnemy = NULL;
                log("gid: %d", gid);
                
                if(gid == 106){
                    
                }else if(gid == 619){
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
                }else if(gid == 629){
                    int triggerIndex = triggerArray.size();
                    bool isOn = UDGetBool(strmake("blocker_on_%d_%d_%d_%d", mapIndex, x, y, triggerIndex).c_str(), triggerIndex == 2);
                    Sprite* sptTrigger = Sprite::createWithSpriteFrameName("trigger.png");
                    sptTrigger->setFlippedX(isOn);
                    spriteBatch->addChild(sptTrigger, 1);
                    sptTrigger->setPosition(pos);
                    sptTrigger->setTag(triggerIndex);
                    triggerArray.pushBack(sptTrigger);
                }else if(gid == 6){ // catapult
                    createUnit(UNIT_CATAPULT, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "catapult");
                }else if(gid == 7){ // helicopter
                    createUnit(UNIT_HELICOPTER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "helicopter");
                }else if(gid == 8){ // worker
                    createUnit(UNIT_WORKER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "workerAxe");
                }else if(gid == 65){ // worker ready
                    createUnit(UNIT_WORKER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "workerAxe");
                }else if(gid == 9){ // swordman
                    createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "swordman")->monitoringDistance = 300000;
                }else if(gid == 66){ // swordman ready
                    createUnit(UNIT_SWORDMAN, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "swordman");
                }else if(gid == 10){ // archer
                    createUnit(UNIT_ARCHER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "archer");
                }else if(gid == 67){ // archer ready
                    createUnit(UNIT_ARCHER, WHICH_SIDE_READY_HERO, ITS_NOT_BUILDING, pos, "archer");
                }else if(gid == 5){ // hero
                    hero = createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "swordman");
                    hero->setName("hero");
                    //this->setPosition(-pos + Vec2(size.width/2 + TILE_SIZE, size.height*2/4));
                    npcArray.pushBack(hero);
                }else if(gid == 63){ // camera init pos
                    this->setPosition(-pos + Vec2(size.width/2 + TILE_SIZE, size.height*2/4));
                    log("camera init pos");
                }else if(gid == 4){ // orc chief
                    EnemyBase* unit = createUnit(UNIT_ORC_AXE, gid == 88?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "orcAxe");
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
                }else if(gid == 36){ // troll schedule
                    createUnit(UNIT_TROLL, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "troll")->scheduledAttackTime = 60*5;
                }else if(gid == 17 || gid == 85){ // goblin bomb
                    EnemyBase* unit = createUnit(UNIT_GOBLIN_BOMB, gid == 85?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblinBomb");
                    if (gid == 85) {
                        unit->isAlli = true;
                    }
                }else if(gid == 37){ // goblin bomb schedule
                    createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblinBomb")->scheduledAttackTime = 60*2.5f;
                }else if(gid == 18 || gid == 86){ // goblin
                    EnemyBase* unit = createUnit(UNIT_GOBLIN, gid == 86?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblin");
                    if (gid == 86) {
                        unit->isAlli = true;
                    }
                }else if(gid == 38){ // goblin schedule
                    createUnit(UNIT_GOBLIN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblin")->scheduledAttackTime = 60*2;
                }else if(gid == 19 || gid == 28 || gid == 27 || gid == 39 || gid == 88){ // orc axe
                    int vary = 40;
                    EnemyBase* unit = createUnit(UNIT_ORC_AXE, gid == 88?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos + Vec2(rand()%vary - vary/2, rand()%vary - vary), "orcAxe");
                    
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
                }else if(gid == 20|| gid == 87){ // orc spear
                    EnemyBase* unit = createUnit(UNIT_ORC_SPEAR, gid == 87?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "orcSpear");
                    if(gid == 87){
                        unit->isAlli = true;
                    }
                    if(GM->nextScene == STAGE_INTRO){
                        unit->setFlippedX(true);
                    }
                }else if(gid == 26){ // orc spear
                    createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "orcSpear")->scheduledAttackTime = 60*3;
                }else if(gid == 69 || gid == 89){ // bunker
                    EnemyBase* unit = createUnit(UNIT_ORC_BUNKER, gid == 89?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Vec2(50, 0), "bunker", 1, "bunker.png");
                    if (gid == 89) {
                        unit->isAlli = true;
                    }
                    unit->canFindTarget = true;
                    setOccupy(pos, 2, 2, true, unit);
                }else if(gid == 70 || gid == 80 || gid == 79 || gid == 78 || gid == 90 || gid == 46){ // hq
                    EnemyBase* unit = createUnit(UNIT_ORC_HQ, gid == 90?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Vec2(150, -50), "hq", 1, "hq.png");
                    if (gid == 90) {
                        unit->isAlli = true;
                    }
                    unit->canFindTarget = true;
                    setOccupy(pos, 4, 3, true, unit);
                    Sprite* spt = Sprite::createWithSpriteFrameName("redFlag0.png");
                    GM->runAnimation(spt, "redFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Vec2(232, 335));
                    
                    if(gid == 80 || gid == 79 || gid == 78 ){
                        unit->scheduledProductUnit = UNIT_ORC_AXE;
                        if (gid == 80) {
                            unit->scheduledProductUnitCount = 2;
                            unit->scheduledProductTime = 60*1;
                        }else if(gid == 79) {
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
                }else if(gid == 68){ // factory
                    EnemyBase* unit = createUnit(UNIT_FACTORY, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "factory", 1, "factory.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                }else if(gid == 54){ // farm
                    EnemyBase* unit = createUnit(UNIT_FARM, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -0), "farm", 1, "farm.png");
                    addFoodMax(getFoodGive(UNIT_FARM));
                    setOccupy(pos, 3, 2, true, unit);
                    unit->isBuildingComplete = true;
                    unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
                    unit->smokePoint = unit->getPosition() + Vec2(-18, 110);
                }else if(gid == 55){ // lumber mill
                    EnemyBase* unit = createUnit(UNIT_LUMBERMILL, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "lumberMill", 1, "lumberMill.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                }else if(gid == 56){ // airport
                    EnemyBase* unit = createUnit(UNIT_AIRPORT, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "airport", 1, "airport.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                    Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
                    GM->runAnimation(spt, "propeller", true);
                    unit->addChild(spt);
                    spt->setPosition(Vec2(134, 206));
                }else if(gid == 57){ // mine
                    EnemyBase* unit = createUnit(UNIT_MINE, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos+ Vec2(100, -100), "mine", 1, "mine.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->setTag(0);
                }else if(gid == 58){ // barracks
                    EnemyBase* unit = createUnit(UNIT_BARRACKS, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "barracks", 1, "barracks.png");
                    setOccupy(pos, 3, 3, true, unit);
                    unit->isBuildingComplete = true;
                    unit->startProductSchedule();
                }else if(gid == 59 || gid == 40 || gid == 64){ // castle
                    int side = WHICH_SIDE_HERO;
                    if(gid == 40){
                        side = WHICH_SIDE_ENEMY;
                    }else if(gid == 64){
                        side = WHICH_SIDE_READY_HERO;
                    }
                    EnemyBase* unit = createUnit(gid == 40?UNIT_ZOMBIE_CASTLE:UNIT_CASTLE, side, ITS_BUILDING, pos + Vec2(150, -100), "castle", 1, "castle.png");
                    if(gid == 59){
                        //                        this->setPosition(-pos + Vec2(size.width/2 - TILE_SIZE*1, size.height*3/4));
                    }
                    
                    if(getPositionY() > 0){
                        setPositionY(0);
                    }
                    setOccupy(pos, 4, 3, true, unit);
                    addFoodMax(getFoodGive(UNIT_CASTLE));
                    unit->startProductSchedule();
                    unit->isBuildingComplete = true;
                    
                    Sprite* spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
                    GM->runAnimation(spt, "blueFlag", true);
                    unit->addChild(spt);
                    spt->setPosition(Vec2(192, 255));
                    if(gid == 40){
                        unit->scheduledProductUnit = UNIT_ZOMBIE_SWORDSMAN;
                        unit->scheduledProductUnitCount = 1;
                        unit->scheduledProductTime = 25;
                    }
                }else if(gid == 60 || gid == 100){ // watcher tower
                    EnemyBase* unit = createUnit(UNIT_WATCHERTOWER, gid == 60?WHICH_SIDE_HERO:WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Vec2(50, 0), "watcherTower", 1, "watcherTower.png");
                    unit->canFindTarget = true;
                    setOccupy(pos, 2, 2, true, unit);
                    unit->isBuildingComplete = true;
                }else if(gid == 30 || gid == 77){ // zombie swordsman
                    EnemyBase* unit = createUnit(UNIT_ZOMBIE_SWORDSMAN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "zombieSwordman");
                    unit->isZombie = gid == 30;
                    unit->oneSec = 1 + (rand()%10)*0.1f;
                    unit->extraSpeed = unit->extraSpeed - rand()%60;
                }else if(gid == 29 || gid == 76){ // zombie orc
                    EnemyBase* unit = createUnit(UNIT_ZOMBIE_ORC_AXE, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "zombieOrcAxe");
                    unit->isZombie = gid == 29;
                    unit->oneSec = 1 + (rand()%10)*0.1f;
                    unit->extraSpeed = unit->extraSpeed - rand()%60;
                }
                unitLayer->removeTileAt(point);
                
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
            if(decoLayer){
                gid = decoLayer->getTileGIDAt(point);
                if (gid) {
                    Vec2 pos = tileMap->getPosition() + Vec2(i*TILE_SIZE + TILE_SIZE/2, (totalHeight-j-1)*TILE_SIZE) + Vec2(0, TILE_SIZE/2);
                    if(gid == 50){ // tree
                        createTree(pos);
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
            //                spt->setPosition(pos + Vec2(-50 + rand()%100, -50 + rand()%100));
            //                spt->setLocalZOrder(-100000);
            //            }
        }
    }
    
    
    
    CCLOG("setStage map done");
    TMXObjectGroup* objectGroup = tileMap->getObjectGroup("Object");
    CCLOG("setStage object map done");
    if (pointArrayToRemoveInUnitLayer) {
        for (int i = 0; i < pointArrayToRemoveInUnitLayer->count(); i++) {
            Vec2 pos = pointArrayToRemoveInUnitLayer->getControlPointAtIndex(i);
            unitLayer->removeTileAt(pos);
        }
    }
    
    this->schedule(schedule_selector(EditorWorld::gravityUpdate));
    this->schedule(schedule_selector(EditorWorld::gravityUpdateForCoins));
    this->schedule(schedule_selector(EditorWorld::gravityUpdateForStraight));
    this->schedule(schedule_selector(EditorWorld::gravityUpdateForCustomMoving));
    this->schedule(schedule_selector(EditorWorld::gravityUpdateForFlyingOrSwimingEnemies));
    
    if (talkCount > 0) {
        this->schedule(schedule_selector(EditorWorld::talkUpdate), 0.1f);
    }
    
    this->schedule(schedule_selector(EditorWorld::bubbleUpdate));
    this->schedule(schedule_selector(EditorWorld::makeSomeBubbles), 0.1f);
    
    
    extraCritical = 0;
    extraPower = 0;
    extraFruit = 0;
    
    GameManager::getInstance()->isVideoRewardEnergy = false;
    GameManager::getInstance()->isVideoRewardAttack = false;
    GameManager::getInstance()->isVideoRewardCoins = false;
    
    
    tileMap->setTag(MAP_FOUND);
    float dur = 2;
    if (enemiesToLoad.size() > 0) {
        this->runAction(Sequence::create(DelayTime::create(dur), CallFunc::create(CC_CALLBACK_0(EditorWorld::loadEnemies, this)), NULL));
    }
    
    
    resetPathState();
    isSetStageDone = true;
    
    // test
    draw->clear();
    
    //    addGold(12000); // test
    //    addLumber(12000); // test
    
    float rowCounter = 1;
    int columnCounter = 0;
    int startX = -getPositionX() + 600;
    int startY = -getPositionY() + 100;
    int birdGapX = 90;
    int birdGapY = 90;
    int columnOffset = 100;
    dur = 2;
    for (int i = 0; i < 20; i++) {
        Sprite* spt = Sprite::createWithSpriteFrameName("bird0.png");
        GM->runAnimation(spt, strmake("bird%d", rand()%3).c_str(), true);
        spriteBatchEffect->addChild(spt);
        spt->setPosition(Vec2(startX - rowCounter*birdGapX/2 + birdGapX*columnCounter + rand()%400 + 200, startY - rowCounter*birdGapY - columnCounter*columnOffset + rand()%400 + 200));
        
        columnCounter++;
        spt->setRotation(20 + rand()%30);
        spt->setScale(0.7f + (rand()%60)*0.01f);
        spt->runAction(Sequence::create(MoveBy::create(dur/spt->getScale(), Vec2(2000 + rand()%200, 2200+ rand()%400)), SPT_REMOVE_FUNC, NULL));
        spt->runAction(ScaleTo::create(dur, 0.7f + (rand()%60)*0.01f));
        int columnMaxCount = rowCounter;
        if (rowCounter > 4) {
            columnMaxCount = 4;
        }
        if (columnCounter >= rowCounter) {
            rowCounter++;
            columnCounter = 0;
        }
    }
    
    //    totalKillUnit = 99;
    //    totalProducedUnit = 85;
    //    totalUsedGold = 22345;
    //    totalEarnedGold = 27877;
    //    totalUsedLumber = 9244;
    //    totalEarnedLumber = 11999;
    //    gameTimer = 60*5 + 53; // test
    //    endGame(); // test
    
    UD->flush();
    CCLOG("setStage done");
}
void EditorWorld::makeZombiesAttack(){
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
void EditorWorld::attackNearHero(EnemyBase* enemy){
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
bool EditorWorld::isSpotAvailable(bool isEnemy, cocos2d::Vec2 pos){
    if (isEnemy) {
        Vec2 coordinate = getCoordinateFromPosition(pos);
        for(auto unit: enemyArray){
            if (getCoordinateFromPosition(unit->getPosition()) == coordinate) {
                return false;
            }
        }
    }
    return true;
}
void EditorWorld::setOccupy(cocos2d::Vec2 pos, int width, int height, bool occupy, EnemyBase* building){
    setOccupy(pos, width, height, occupy);
    building->buildingStartCoordinate = getCoordinateFromPosition(pos, theMap);
    building->buildingOccupySize = cocos2d::Size(width, height);
//    building->refreshApproachPoints();
}
void EditorWorld::setOccupy(cocos2d::Vec2 pos, int width, int height, bool occupy){
    Vec2 point = getCoordinateFromPosition(pos, theMap);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            decoLayer->setTileGID(occupy?48:0, Vec2(point.x + i, point.y + j));

//            Sprite* spt = Sprite::create("btnStop.png"); // test
//            this->addChild(spt);
//            spt->setScale(0.1f);
//            spt->setPosition(getPositionFromTileCoordinate(point.x + i, point.y + j)); // test
        }
    }
}
void EditorWorld::addSprite(std::string sptName, cocos2d::Vec2 pos){
    Sprite* spt = Sprite::createWithSpriteFrameName(sptName);
    spt->setPosition(pos);
    spriteBatch->addChild(spt);
    spt->setLocalZOrder(-pos.y);
}
void EditorWorld::startCameraFollowNPCForEvent(Ref* ref){
    cameraTargetForEvent = (Node*)ref;
    this->schedule(schedule_selector(EditorWorld::cameraUpdateForEvent));
}
void EditorWorld::endCameraFollowNPCForEvent(){
    this->unschedule(schedule_selector(EditorWorld::cameraUpdateForEvent));
}
void EditorWorld::cameraUpdateForEvent(float dt){
    
}
void EditorWorld::eventUpdate(float dt){
    if(talkingNPC != nullptr && talkingNPC->getName().compare("undead") == 0){
        if(imgTalkBox != nullptr && imgTalkBox->getScale() == 1){
            imgTalkBox->setPosition(talkingNPC->getPosition() + Vec2(0, 10));
        }else{
            sptPointer->setScale(imgTalkBox->getScale());
        }
        sptPointer->setPosition(imgTalkBox->getPosition() + Vec2(0, 2));
    }
}
void EditorWorld::eventDone(){
    this->unschedule(schedule_selector(EditorWorld::eventUpdate));
}

void EditorWorld::headingUp(Ref* ref){
    EnemyBase* unit = (EnemyBase*)ref;
    unit->velocity = Vec2(0, 1);
//    unit->move(DIRECTION_N);
//    unit->updateWeaponPosition();
}
void EditorWorld::headingDown(Ref* ref){
    EnemyBase* unit = (EnemyBase*)ref;
    unit->velocity = Vec2(0, -1);
//    unit->move(DIRECTION_S);
//    unit->updateWeaponPosition();
}
void EditorWorld::headingLeft(Ref* ref){
    EnemyBase* unit = (EnemyBase*)ref;
    unit->velocity = Vec2(-1, 0);
//    unit->move(DIRECTION_W);
//    unit->updateWeaponPosition();
}
void EditorWorld::headingRight(Ref* ref){
    EnemyBase* unit = (EnemyBase*)ref;
    unit->velocity = Vec2(1, 0);
//    unit->move(DIRECTION_E);
//    unit->updateWeaponPosition();
}
void EditorWorld::stay(Ref* ref){
    EnemyBase* unit = (EnemyBase*)ref;
//    unit->runMoveAnimation(DIRECTION_STAY);
}
void EditorWorld::startNPCEvent(){
    
}

void EditorWorld::dragonUpdate(float dt){
    
}
void EditorWorld::turnOnEnemyDamaging(Ref* obj){
    EnemyBase* enemy = (EnemyBase*)obj;
    enemy->isDamaging = true;
    runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
}
void EditorWorld::addDecoBlock(Vec2 coordinate, Vec2 position, std::string spriteName){
    Sprite* spt = Sprite::createWithSpriteFrameName(spriteName);
    spriteBatch->addChild(spt);
    spt->setPosition(position);
    spt->setLocalZOrder(-spt->getBoundingBox().origin.y + 2);
    spt->setName(strmake("deco_%d_%d", (int)coordinate.x, (int)coordinate.y));
    decoLayer->setTileGID(138, coordinate);
}
Vec2 EditorWorld::getPositionFromTileCoordinate(int x, int y){
    return Vec2(x*TILE_SIZE + TILE_SIZE/2, (theMap->getMapSize().height-y-1)*TILE_SIZE) + Vec2(0, TILE_SIZE/2);
}
void EditorWorld::resetPathState(){
    int gid;
    experimental::TMXLayer* stageLayer = theMap->getLayer("stage");
    cocos2d::Size mapSize = theMap->getMapSize();
    for (int j = 0; j < mapSize.height; j++) {
        for (int i = 0; i < mapSize.width; i++) {
            
            gid = stageLayer->getTileGIDAt(Vec2(i, j));
            if (gid) {
                GM->setPathState(i, j, (!isWay(gid) || (decoLayer && isDecoBlock(decoLayer->getTileGIDAt(Vec2(i, j)))))?TILE_STATE_OCCUPIED:TILE_STATE_EMPTY);
                
            }
        }
    }
}
void EditorWorld::createTrap(EnemyBase* enemy, cocos2d::Vec2 pos){
    enemy->setPosition(pos);
    enemy->ap = getHeroMaxHP(0)/4;
    spriteBatch->addChild(enemy);
    enemy->setLocalZOrder(-1000);
    enemyArray.pushBack(enemy);
}
EnemyBase* EditorWorld::addNPC(cocos2d::Vec2 pos, std::string name){
    EnemyBase* unit = createUnit(UNIT_NPC, WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING, pos, name.c_str());
    heroArray.eraseObject(unit);
    MovableArray.eraseObject(unit);
    npcArray.pushBack(unit);
    unit->setLocalZOrder(-unit->getBoundingBox().origin.y);
    updateNPCQuestState(unit);
    return unit;
}
void EditorWorld::updateNPCQuestState(EnemyBase* npc){
//    std::string npcName = npc->getName();
//    int level = getHeroLevel(0);
//    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//    //    int currentQuestIndex = npcTable[Value(questID).asString()].asValueMap().at("event index").asInt();
//    if(npc->getChildByName("questState") != nullptr){
//        npc->removeChildByName("questState");
//    }
//    if(questID >= 0){
//        std::string currentQuestNpcName = npcTable[Value(questID).asString()].asValueMap().at("npc name").asString();
//        std::string talk;
//        if(currentQuestNpcName.compare(npcName) == 0){
//            if (isQuestConditionMet()) {
//                addTalkBalloon(npc, "exclaim.png");
//            }else{
//                addTalkBalloon(npc, "waitingTalkBox.png");
//            }
//        }
//    }else{
//        int eventIndex = -1;
//        bool isQuest = false;
//        int questID = -1;
//        int eIndex = -1;
//        for(int i = 0;i < npcTable.size();i++){
//            if(npcTable[Value(i).asString()].asValueMap().at("npc name").asString().compare(npcName) == 0){
//                int levelLimit = npcTable[Value(i).asString()].asValueMap().at("level limit").asInt();
//                eIndex = npcTable[Value(i).asString()].asValueMap().at("event index").asInt();;
//                if(level >= levelLimit && !UDGetBool(strmake(KEY_QUEST_DONE_FORMAT, eIndex).c_str(), false)){
//                    eventIndex = eIndex;
//                    questID = i;
//                    isQuest = npcTable[Value(i).asString()].asValueMap().at("event type").asString().compare("quest") == 0;
//                    break;
//                }
//            }
//        }
//        if(eventIndex < 0){
//
//        }else{
//            addTalkBalloon(npc, "question.png");
//        }
//    }
}
void EditorWorld::addTalkBalloon(EnemyBase* npc, std::string imgName){
    Sprite* spt = Sprite::createWithSpriteFrameName(imgName);
    npc->addChild(spt, 10);
    spt->setPosition(Vec2(npc->getBoundingBox().size.width/2, npc->getContentSize().height + spt->getContentSize().height/2));
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Vec2(0, 2)), MoveBy::create(0.2f, Vec2(0, -2)), NULL)));
    spt->setName("questState");
}
void EditorWorld::addTalkBalloon(experimental::TMXTiledMap* map, std::string imgName, cocos2d::Vec2 pos){
    Sprite* spt = Sprite::createWithSpriteFrameName(imgName);
    map->addChild(spt, 10);
    //    spt->setPosition(Vec2(npc->getContentSize().width/2, npc->getContentSize().height + spt->getContentSize().height/2));
    spt->setPosition(pos);
    spt->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.2f, Vec2(0, 2)), MoveBy::create(0.2f, Vec2(0, -2)), NULL)));
    spt->setName("questState");
}
EnemyBase* EditorWorld::createUnit(int index, int whichSide, bool isBuilding, Vec2 pos, std::string name, int scaleX, std::string charName){
    int eng = 50;
    int spd = 25;
    
    eng = getUnitMaxHP(index);
    spd = getUnitSP(index);
    EnemyBase* unit = EnemyBase::createEnemy(index, eng, spd, charName.c_str());
    unit->maxEnergy = eng;
    unit->energy = eng;
    unit->setName(name);
    unit->ap = getUnitAP(index);
    unit->dp = getUnitDP(index);
    unit->attackRange = getUnitAttackRange(index);
    unit->attackType = getUnitAttackType(index);
    unit->attackTargetType = getUnitAttackTargetType(index);
    unit->monitoringDistance = getUnitMonitoringDistance(index);
    unit->attackCoolTimeMax = getUnitAttackCoolTime(index);
    unit->attackHappenTime = getUnitAttackHappenTime(index);
//    unit->movingDirection = DIRECTION_S;
    unit->setScaleX(scaleX);
    unit->rewardExp = 10;
    unit->rewardCoin = 10;
    
//    unit->runMoveAnimation(DIRECTION_STAY);
    unit->alliSide = whichSide;
    unit->setPosition(pos);
    unit->desiredPosition = pos;
    unit->setLocalZOrder(-pos.y);
    
//    unit->setWeapon();
    unit->isEnemy = whichSide == WHICH_SIDE_ENEMY;
    unitsToCreateArray.pushBack(unit);
    unit->isBuilding = isBuilding;
    if(isBuilding){
        unit->canMove = false;
        unit->isBuildingComplete = true;
        
//        Sprite* spt = Sprite::createWithSpriteFrameName(getSpriteNameForUnit(index));
//        this->addChild(spt, 1900);
//        spt->setScale(0.2f);
//        spt->setPosition(pos);
//        spt->setPosition(getPositionFromTileCoordinate(point.x + i, point.y + j)); // test
    }
//    unit->refreshApproachPoints();
//    unit->stop();
    spriteBatch->addChild(unit);
    //    MovableArray.pushBack(unit); // test
    
    if (unit->unitType == UNIT_HELICOPTER) {
        unit->isFlying = true;
        unit->setLocalZOrder(10);
    }
    
    
    return unit;
}

int EditorWorld::getUnitAP(int unit){
    //    return 0; // test
    if(unit == UNIT_WORKER){
        return 3;
    }else if(unit == UNIT_ARCHER){
        return 13;
    }else if(unit == UNIT_SWORDMAN){
        return 10;
    }else if(unit == UNIT_WATCHERTOWER){
        return 15;
    }else if(unit == UNIT_CATAPULT){
        return 40;
    }else if(unit == UNIT_GOBLIN){
        return 15;
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
    }
    return 5;
}
int EditorWorld::getUnitDP(int unit){
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
    }
    return 0;
}
int EditorWorld::getUnitSP(int unit){
    if(unit == UNIT_WORKER){
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
    }
    return 450;
}
float EditorWorld::getUnitCreateTime(int index){
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
    }
    return 10;
}
float EditorWorld::getUnitMonitoringDistance(int index){
    if(index == UNIT_ARCHER){
        return 600000;
    }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 950000;
    }else if(index == UNIT_CATAPULT){
        return 1200000;
    }else if(index == UNIT_SWORDMAN || index == UNIT_WORKER || index == UNIT_ORC_AXE || index == UNIT_ORC_SPEAR || index == UNIT_GOBLIN || index == UNIT_TROLL|| index == UNIT_GOBLIN_BOMB){
        return 300000;
    }else if(index == UNIT_HELICOPTER){
        return 0;
    }
    return 600000;
}
float EditorWorld::getUnitAttackRange(int index){
    if(index == UNIT_ARCHER){
        return 200000;
    }else if(index == UNIT_ORC_SPEAR){
        return 220000;
    }else if(index == UNIT_CATAPULT){
        return 500000;
    }else if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 400000;
    }
    return 200000;
}
int EditorWorld::getUnitAttackType(int index){
    if (index == UNIT_ARCHER ||index == UNIT_ORC_SPEAR || index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ) {
        return ATTACK_TYPE_RANGE;
    }else if (index == UNIT_SWORDMAN ||index == UNIT_WORKER || index == UNIT_GOBLIN_BOMB || index == UNIT_GOBLIN || index == UNIT_ORC_AXE || index == UNIT_TROLL || index == UNIT_ZOMBIE_ORC_AXE || index == UNIT_ZOMBIE_SWORDSMAN) {
        return ATTACK_TYPE_NEAR;
    }
    return ATTACK_TYPE_NONE;
}
int EditorWorld::getUnitAttackTargetType(int index){
    if (index == UNIT_CATAPULT) {
        return ATTACK_TARGET_TYPE_SPLASH;
    }
    return ATTACK_TARGET_TYPE_SINGLE;
}
float EditorWorld::getUnitAttackCoolTime(int index){
    if(index == UNIT_WORKER){
        return 1.0f;
    }else if(index == UNIT_ARCHER){
        return 1.5f;
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
    }
    return 2.0f;
}
float EditorWorld::getUnitAttackHappenTime(int index){
    if(index == UNIT_ARCHER){
        return 0.3f;
    }else if(index == UNIT_SWORDMAN || index == UNIT_ORC_SPEAR || index == UNIT_ORC_AXE || index == UNIT_GOBLIN || index == UNIT_TROLL){
        return 0.3f;
    }else if(index == UNIT_CATAPULT || index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ){
        return 0.0f;
    }else if(index == UNIT_WORKER){
        return 0.5f;
    }
    return 0.2f;
}
int EditorWorld::getUnitMaxHP(int unit){
    if(unit == UNIT_WORKER){
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
        return 200;
    }else if(unit == UNIT_ZOMBIE_SWORDSMAN){
        return 180;
    }
    return 60;
}
void EditorWorld::stopWoongSound(){
    readyToMove = true;
    
}
void EditorWorld::addDirtToTile(experimental::TMXTiledMap* map, Vec2 pos){
    Sprite* sptDirt;
    for (int i = 0; i < 4; i++) {
        sptDirt = Sprite::createWithSpriteFrameName(rand()%2==0?"dirt1.png":"dirt0.png");
        spriteBatch->addChild(sptDirt, 100);
        float dur = 1;
        sptDirt->runAction(MoveBy::create(0.1, Vec2(0, -2)));
        sptDirt->runAction(Sequence::create(EaseOut::create(MoveBy::create(dur, Vec2(i%2==0?-4:4, 4)), 6), CallFuncN::create(CC_CALLBACK_1(Sprite::removeFromParentAndCleanup, sptDirt)), NULL));
        sptDirt->runAction(Sequence::create(DelayTime::create(dur/2), FadeOut::create(dur/2),NULL));
        sptDirt->setPosition(Vec2(pos.x*TILE_SIZE + (i%2==0?0:TILE_SIZE), (map->getMapSize().height - pos.y - 1)*TILE_SIZE + (i>1?TILE_SIZE:0)) + map->getPosition());
        //log("dirt %f, %f", sptDirt->getPosition().x, sptDirt->getPosition().y);
    }
}


void EditorWorld::cloudSchedule(float dt){
    for (auto cloud : cloudArray) {
        if(cloud->getPosition().x < 0){
            cloud->setPosition(Vec2(cloud->getParent()->getContentSize().width + cloud->getContentSize().width/2, cloud->getPosition().y));
        }
    }
}
void EditorWorld::saveCoinIfStarCollected(){
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
void EditorWorld::sacrificeLater(float dt){
    
}
void EditorWorld::sacrificeDone(Ref* obj){
    
}

void EditorWorld::addRide(int unit, Vec2 pos){
    const char* sptName = "rideTop.png";
    
    Movable* board = Movable::createMovable(unit, 1, 0, sptName);
    board->setPosition(pos);
    board->setTag(unit);
    
    /*Sprite* sprite = Sprite::createWithSpriteFrameName("silverCoin0.png");
     sprite->setPosition(Vec2(16, 20));
     sprite->runAction(RepeatForever::create((ActionInterval*)Sequence::create(MoveBy::create(0.3f, Vec2(0, 15)), MoveBy::create(0.3f, Vec2(0, -15)), NULL)));
     board->addChild(sprite);*/
    
    spriteBatch->addChild(board);
    helperArray.pushBack(board);
    float time = 0.6f;
    MoveTo* moveUpTop = MoveTo::create(time, pos + Vec2(0, 10));
    MoveTo* moveDownTop = MoveTo::create(time, pos);
    MoveTo* moveDownBottom = MoveTo::create(time, pos + Vec2(0, - 10));
    MoveTo* moveUpBottom = MoveTo::create(time, pos);
    float rate = 1;
    
    board->runAction(RepeatForever::create((ActionInterval*)Sequence::create(EaseIn::create(moveUpTop, rate), EaseOut::create(moveDownTop, rate), EaseIn::create(moveDownBottom, rate), EaseOut::create(moveUpBottom, rate), NULL)));
}
void EditorWorld::splashDamage(Vec2 pos, int radius, int damage, bool isFromEnemy, Movable* attacker){
    GM->playSoundEffect(SOUND_EXPLOSION_MIDDLE);
    //    draw->drawSolidCircle(pos, radius, 360, 20, 1, 1, Color4F::RED);
    //    for(auto unit:heroArray){
    //        if (unit->getPosition().distanceSquared(pos) < sqrt(radius)) {
    //            if(unit->getHitAndIsDead(damage)){
    //                removeDeadUnit(unit);
    //            }
    //        }
    //    }
    for(auto unit: isFromEnemy? heroArray:enemyArray){
        if (unit->getPosition().distanceSquared(pos) < radius) {
            if(unit->getHitAndIsDead(damage, attacker)){
                removeDeadUnit(unit);
            }
        }
    }
    
    //    if (target && target->energy > 0) {
    //        if(target->getHitAndIsDead(ap)){
    //            stop();
    //        }
    //    }
}
void EditorWorld::updateIndicators(){
    Movable* unit;
    float eX = 0;
    float eY = 0;
    float centerX = -this->getPosition().x + size.width/2;
    float centerY = -this->getPosition().y + size.height/2;
    float a;float y;float x;cocos2d::Rect rect;
    int outOfBoxCount = 0;
    Sprite* indicator;
    Sprite* spt;
    float scale = this->getScale();
    float offset = 15;
    for (int i = 0; i < indicatedArray.size(); i++) {
        spt = (Sprite*)indicatedArray.at(i);
        
        
        rect = cocos2d::Rect(-this->getPosition().x/scale, -this->getPosition().y/scale, size.width/scale, size.height/scale);
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
            
            spt->setPosition(Vec2(-50, indicator->getContentSize().height*indicator->getScale()/2));
            indicator->setScale(0.5);
            this->getParent()->addChild(indicator);
            GameManager::getInstance()->makeItSiluk(indicator);
            indicatorArray.pushBack(indicator);
        }
        
        indicator->setRotation(-atan2(y,x)*180.0f/3.14f);
        indicator->setPosition(Vec2(size.width/2 + x, size.height/2 + y));
        
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

void EditorWorld::removeUsedAssets(){
    CCLOG("EditorWorld removeUsedAssets");
    unscheduleAll();
    
    for (auto lbl: talkLabelArray){
        lbl->release();
    }
    _eventDispatcher->removeEventListener(listener);
    _eventDispatcher->removeEventListener(mouseListener);
    
    this->removeAllChildrenWithCleanup(true);
    this->unscheduleAllCallbacks();
    this->stopAllActions();
    GameManager::getInstance()->setEditorHud(NULL);
    GameManager::getInstance()->setCurrentStageLayer(NULL);
}
void EditorWorld::testSchedule(float dt){
    
}
void EditorWorld::useBomb(){
    if(isGameOver)
        return;
    
    CCLOG("Use Bomb!");
    isPaused = true;
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
    
    bombTargetMissileArray.clear();
    bombTargetEnemyArray.clear();
    
    this->schedule(schedule_selector(EditorWorld::doTheBombToMissiles), 0.1, bombTargetMissileArray.size(), 1);
}

void EditorWorld::doTheBombToMissiles(float dt){
    CCLOG("doTheBombToMissiles");
    if (bombTargetMissileArray.size() <= 0) {
        this->unschedule(schedule_selector(EditorWorld::doTheBombToMissiles));
        bombTargetEnemyIndex = 0;
        this->schedule(schedule_selector(EditorWorld::doTheBombToEnemies), 0.1, bombTargetEnemyArray.size(), 0.1);
        return;
    }
    
    Movable* drop = bombTargetMissileArray.at(0);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this)), NULL));
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
    
    Vec2 screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Vec2(rand()%36 - 18, rand()%36 - 18)),
                                     MoveTo::create(0.05, screenPos),NULL));
}

void EditorWorld::doTheBombToEnemies(float dt){
    float demage = 1;
    
    if (bombTargetEnemyArray.size() <= bombTargetEnemyIndex ) {
        this->unschedule(schedule_selector(EditorWorld::doTheBombToEnemies));
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
                    //                    GameManager::getInstance()->getEditorHud()->setBossEnergy(theBoss->energy*100.0f/theBoss->maxEnergy);
                }else{
                    enemy->desiredPosition = enemy->desiredPosition + Vec2(enemy->isTowardLeft?2:-2, 2);
                    enemy->setPosition(enemy->getPosition() + Vec2(enemy->isTowardLeft?2:-2, 2));
                }
                
                showDamage(demage, enemy->getPosition());
                
                if (enemy->energy <= 0) {
                    this->runEffect(EFFECT_EXPLODE_SMALL, enemy->getPosition());
                    this->createRewards(enemy);
                    
                    enemyArray.eraseObject(enemy);
                    MovableArray.eraseObject(enemy);
                    spriteBatch->removeChild(enemy, true);
                }else{
                    enemy->blinkForAWhile();
                }
            }
        }
        
        return;
    }
    
    EnemyBase* enemy = bombTargetEnemyArray.at(bombTargetEnemyIndex);
    Sprite* sprite = Sprite::createWithSpriteFrameName("daryFace.png");
    sprite->runAction(Sequence::create(ScaleTo::create(0.5, 5), CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this)), NULL));
    sprite->runAction(FadeOut::create(0.5));
    spriteBatch->addChild(sprite);
    sprite->setPosition(enemy->getPosition());
    
    Vec2 screenPos = this->getPosition();
    this->runAction(Sequence::create(MoveBy::create(0.05, Vec2(rand()%18 - 9, rand()%18 - 9)),
                                     MoveTo::create(0.05, screenPos),NULL));
    
    bombTargetEnemyIndex++;
}
void EditorWorld::usePotion(){
    if(isGameOver)
        return;
    
    // heal effect
    CCLOG("Use Potion!");
}

bool EditorWorld::keyExist(ValueMap map, const char* key){
    return map.find(key) != map.end();
}

void EditorWorld::revive(){
    resumeLayer();
    
    isGameOver = false;
}

void EditorWorld::enemyFireLoop(float dt)
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
void EditorWorld::removeMeFromChasing(Ref* ref){
    Movable* drop = (Movable*)ref;
    drop->setTag(UNIT_MISSILE_STRAIGHT);
    chasingMissileArray.eraseObject(drop);
    straightMovingArray.pushBack(drop);
}

void EditorWorld::bubbleUpdate(float dt)
{
    char buf[80];
    sprintf( buf, "theme_%d_Background_tower_%d_shadow.png", GameManager::getInstance()->theme, rand()%2);
    
    for(auto bubble: bubbleArray)
    {
        bubble->setPosition(Vec2(bubble->getPosition().x, bubble->getPosition().y + 50*dt));
        
        experimental::TMXTiledMap* map = getTileMap(bubble->getPosition());
        //        Vec2 plPos = this->getCoordinateFromPosition(bubble->getPosition() + Vec2(0, - bubble->getContentSize().height), map);
        int tgid = getTileAtPosition(bubble->getPosition(), TAG_FORE_LAYER, map);
        if (tgid == 0) {
            bubbleArray.eraseObject(bubble);
            bubble->stopAllActions();
            sprintf( buf, "bubble%dPop", bubble->getTag());
            Animation* animation = AnimationCache::getInstance()->getAnimation(buf);
            Animate* animate = Animate::create(animation);
            CallFuncN* call = CallFuncN::create(CC_CALLBACK_1(EditorWorld::enemyBaseMoveDone, this));
            bubble->runAction(Sequence::create(animate, call, NULL));
            return;
        }
    }
}

void EditorWorld::makeSomeBubbles(float dt)
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


void EditorWorld::talkUpdate(float dt)
{
    
}
void EditorWorld::autoTargeting(){
    if (!isAutoTargetingOn) {
        return;
    }
    Movable* enemy = findTarget(200);
    float angle;
    
    int radius = 170;
    float dx = cosf(angle) * radius;
    float dy = sinf(angle) * radius;
    
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
void EditorWorld::entranceSchedule(float dt){
    
}
void EditorWorld::releaseCustomMovingCamera(){
    isCameraInCustomMoving = false;
}
// gravityupdate
void EditorWorld::gravityUpdate(float dt)
{
    //    if (stageIndex == STAGE_ENTRANCE) {
    //        entranceSchedule(dt);
    //    }
    for(auto drop: heroArray){
        if(!drop->isFlying){
            drop->setLocalZOrder(-drop->getBoundingBox().origin.y);
        }
    }
    for(auto drop: enemyArray){
        if(!drop->isFlying){
            drop->setLocalZOrder(-drop->getBoundingBox().origin.y);
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
    if (doubleClickTimer > 0) {
        doubleClickTimer -= dt;
    }
    if(GM->loadMapData.length() > 0){
        loadMapData();
    }
    gameTimer += dt;
    if(lastTick + 1 < gameTimer){
        lastTick = gameTimer;
        EHUD->lblTimer->setString(GM->getTimeLeftInString(lastTick));
    }
    
    if(stageCover != nullptr){
        //        Vec2 plPos = this->getCoordinateFromPosition(player->getPosition() + Vec2(0, -player->collectBoundingBox().size.height/2), theMap);
        Vec2 plPos;
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
            sptSnow->runAction(Sequence::create(MoveBy::create(dur, Vec2(rand()%xMove + 5, -yMove)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sptSnow)), NULL));
        }
    }
    
    draw->clear();
    //    GM->drawPath(); // test
    //    for(auto unit: selectedArray){
    //        draw->drawCircle(unit->getPosition() + Vec2(0, -unit->getContentSize().height/2), unit->getContentSize().width/2, 360, 20, false, 1, 0.25f, Color4F::GREEN);
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
    
    bool isMiniMapForMovingChanged = false;
    bool isMiniMapForNonMovingChanged = false;
    for (auto unit: unitsToCreateArray) {
        if (unit->isEnemy) {
            enemyArray.pushBack(unit);
            if(unit->wantToEli){
                attackNearHero((EnemyBase*)unit);
            }
        }else if (unit->alliSide == WHICH_SIDE_HERO) {
            heroArray.pushBack(unit);
//            updateFoodInUse();
//            updateFoodMaxState();
        }else if(unit->alliSide == WHICH_SIDE_MUTUAL){
            mutualArray.pushBack(unit);
        }else if(unit->alliSide == WHICH_SIDE_READY_HERO){
            readyHeroArray.pushBack(unit);
        }
        if (unit->isBuilding) {
            isMiniMapForNonMovingChanged = true;
        }else{
            isMiniMapForMovingChanged = true;
        }
    }
    if (isMiniMapForMovingChanged) {
        updateMiniMapForMoving();
    }
    if (isMiniMapForNonMovingChanged) {
        updateMiniMapForNonMoving();
    }
    
    
    bool shouldExit = false;
//    for (auto unit: readyHeroArray) {
//        for(auto hero: heroArray){
//            if (hero->getPosition().distanceSquared(unit->getPosition()) < 1000000) {
//                unit->blinkSelectedCircle();
//                readyHeroArray.eraseObject(unit);
//                heroArray.pushBack(unit);
//                unit->alliSide = WHICH_SIDE_HERO;
//                shouldExit = true;
//                break;
//            }
//        }
//        if(shouldExit){
//            break;
//        }
//    }
    unitsToCreateArray.clear();
    
    if (!isGameOver) {
        for(auto drop: MovableArray){
            drop->setLocalZOrder(-drop->getBoundingBox().origin.y);
            if (drop->freezed) {
                continue;
            }
            drop->updatePosition(dt);
            checkForAndResolveCollisions(drop);
            if(drop->sptWeapon != nullptr){
//                drop->sptWeapon->setLocalZOrder(drop->headingDirection == DIRECTION_N?-drop->getBoundingBox().origin.y - 1:-drop->getBoundingBox().origin.y + 1);
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
void EditorWorld::checkGameSchedule(){
    if (stageIndex == 3) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*4) {
            for(int i = 12; i < 19;i++){
                decoLayer->setTileGID(0, Vec2(28, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*7) {
            for(int i = 13; i < 21;i++){
                decoLayer->setTileGID(0, Vec2(i, 31));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 1 && gameTimer > 60*10) {
            for(int i = 12; i < 19;i++){
                decoLayer->setTileGID(0, Vec2(41, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 2 && gameTimer > 60*13) {
            for(int i = 13; i < 21;i++){
                decoLayer->setTileGID(0, Vec2(i, 39));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 3 && gameTimer > 60*5) {
            for(int i = 57; i < 61;i++){
                decoLayer->setTileGID(0, Vec2(30, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
    if (stageIndex == 4) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*11) {
            for(int i = 8; i < 14;i++){
                decoLayer->setTileGID(0, Vec2(i, 17));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*8) {
            for(int i = 57; i < 61;i++){
                decoLayer->setTileGID(0, Vec2(i, 16));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
    
    if (stageIndex == 5) {
        if (lastGameScheduleIndex == -1 && gameTimer > 60*11) {
            for(int i = 11; i < 17;i++){
                decoLayer->setTileGID(0, Vec2(i, 9));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 0 && gameTimer > 60*8) {
            for(int i = 8; i < 12;i++){
                decoLayer->setTileGID(0, Vec2(40, i));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }else if (lastGameScheduleIndex == 1 && gameTimer > 60*8) {
            for(int i = 33; i < 39;i++){
                decoLayer->setTileGID(0, Vec2(i, 32));
            }
            resetPathState();
            makeZombiesAttack();
            lastGameScheduleIndex++;
        }
    }
}
void EditorWorld::oneSecUpdate(float dt){
    oneSecTiemr -= dt;
    if(oneSecTiemr > 0)
        return;
    oneSecTiemr = 1;
}
void EditorWorld::halfSecUpdate(float dt){
    halfSecTimer -= dt;
    if(halfSecTimer < 0){
        halfSecTimer = 0.5f;
        updateMiniMapForMoving();
    }
}
Sprite* EditorWorld::getTeleport(int teleport){
    for(auto tele: teleportArray){
        if(tele->getTag() == teleport)
            return tele;
    }
    return NULL;
}
void EditorWorld::gravityUpdateForFlyingOrSwimingEnemies(float dt)
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

void EditorWorld::gravityUpdateForCoins(float dt)
{
    
}
void EditorWorld::gravityUpdateForStraight(float dt)
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
void EditorWorld::gravityUpdateForCustomMoving(float dt)
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
void EditorWorld::chasingMissileUpdate(float dt)
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
void EditorWorld::setViewPointCenter(Vec2 position, bool forceMove)
{
    float scale = getScale();
    int x = position.x*scale;
    int y = position.y*scale;
    
    Vec2 actualPosition = Vec2(x, y);
    
    Vec2 centerOfView = Vec2(size.width/2, size.height*3/4 - 30);
    Vec2 viewPoint = centerOfView - actualPosition;
    float distance = 0;
    float maxDistance = 300;
    if (distance > maxDistance) {
        distance = maxDistance;
    }
    float angle = GameManager::getInstance()->getAngle(center, center);
    Vec2 aimingPos = Vec2(cos(angle*3.14/180)*distance, -sin(-angle*3.14/180)*distance);
    if(forceMove){
        this->setPosition(viewPoint - aimingPos*0.7f);
    }else{
        this->setPosition((viewPoint - aimingPos*0.7f).lerp(this->getPosition(), 0.9f));
    }
}

void EditorWorld::showBigMiniMap(bool isBig){
    //    ScrollView* scrollView = (ScrollView*)dnMiniMap->getParent()->getParent();
    //    if (isBig) {
    //        float height = size.height*3/4;
    //        float width = miniMapSize.width*height/miniMapSize.height;
    //        currentMapSize = cocos2d::Size(width, height);
    //        scrollView->setContentSize(cocos2d::Size(width, height));
    //        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
    //        scrollView->setPosition(Vec2(size.width/2 + width/2, size.height/2 + height/2));
    //        scrollView->setBackGroundColorOpacity(200);
    //    }else{
    //        currentMapSize = miniMapSize;
    //        scrollView->setContentSize(miniMapSize);
    //        log("mini map pos: %f, %f", scrollView->getPositionX(), scrollView->getPositionY());
    //        scrollView->setPosition(Vec2(size.width - 10, size.height - 10));
    //        scrollView->setBackGroundColorOpacity(100);
    //    }
    //    Node* dnHero = scrollView->getInnerContainer()->getChildByName("Hero");
    //    if (dnHero != nullptr) {
    //        dnHero->setPosition(scrollView->getContentSize()/2);
    //        dnHero->setPosition(dnHero->getPosition() + Vec2(-TILE_SIZE*0.3f*0.5f, -TILE_SIZE*0.3f*0.5f));
    //    }
    //
    //    scrollView->setInnerContainerSize(scrollView->getContentSize());
}
void EditorWorld::setCurrentTileMap(Vec2 pos){
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
experimental::TMXTiledMap* EditorWorld::getTileMap(Vec2 pos){
    return theMap;
    for(auto map: mapArray){
        if (map->getBoundingBox().containsPoint(pos)) {
            return map;
        }
    }
    return nullptr;
}
cocos2d::Vec2 EditorWorld::getCoordinateFromPosition(cocos2d::Vec2 position){
    return getCoordinateFromPosition(position, theMap);
}
Vec2 EditorWorld::getCoordinateFromPosition(Vec2 position, experimental::TMXTiledMap* map)
{
    //    int mapRowCount = map->getMapSize().height;
    //    int mapColumnCount = map->getMapSize().width;
    float x = floor(position.x / TILE_SIZE);
    //    if (x < 0) x =  0;
    //    if (x >= mapColumnCount - 1) x = mapColumnCount - 1;
    float y = floor(((map->getMapSize().height * TILE_SIZE) - position.y) / TILE_SIZE);
    //    if (y < 0) y =  0;
    //    if (y >= mapRowCount - 1) y = mapRowCount - 1;
    return Vec2(x, y);
}

cocos2d::Rect EditorWorld::tileRectFromTileCoords(Vec2 tileCoords, experimental::TMXTiledMap* map)
{
    return cocos2d::Rect(tileCoords.x*TILE_SIZE + map->getPositionX(), (map->getMapSize().height - tileCoords.y - 1)*TILE_SIZE + map->getPositionY(), TILE_SIZE, TILE_SIZE);
    //    float levelHeightInPixels = tileMap->getMapSize().height * tileMap->getTileSize().height;
    //    Vec2 origin = Vec2(tileCoords.x * tileMap->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1) * tileMap->getTileSize().height));
    //    return cocos2d::Rect(origin.x, origin.y, tileMap->getTileSize().width, tileMap->getTileSize().height);
}

int EditorWorld::getTileAtPosition(Vec2 position, int tag, experimental::TMXTiledMap* map)
{
    float mapX = map->getPositionX();
    float mapY = map->getPositionY();
    if (position.x < mapX || position.y < mapY || mapX + map->getMapSize().width*TILE_SIZE < position.x || mapY + map->getMapSize().height*TILE_SIZE < position.y ) {
        return 5;
    }
    Vec2 plPos = this->getCoordinateFromPosition(position - map->getPosition(), map); //1
    //experimental::TMXLayer* layer = map->getLayer("stage");
    //Node* parent = layer->getParent();
    return ((experimental::TMXLayer*)map->getChildByTag(tag))->getTileGIDAt(plPos); //4
}

void EditorWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    exit(0);
#endif
}

void EditorWorld::registerScriptAccelerateHandler(int handler)
{
    //    Director::getInstance()->getEventDispatcher()->addTargetedDelegate(this, 0, true);
}


void EditorWorld::setPlayerPosition(Vec2 position) {
    
}
void EditorWorld::Dispose(){
    
}

void EditorWorld::addListener(){
    listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* evt){
        //        log("onTouchesBegan. ID: %d", touches.at(0)->getID());
        isBuildingTempleteMoved = false;
        if (buildingTemplete != nullptr) {
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
        if(touchCount > 1){
            twoTouchEnabled = true;
            Touch* touch = touches.at(touches.size() - 1);
            secondTouchID = touch->getID();
            lastTouchPoint = touch->getLocation();
            touchBeganPos = touch->getLocation();
            if (touches.size() > 1) {
                secondTouchBeganDistance = touches.at(0)->getLocation().distanceSquared(touches.at(1)->getLocation());
            }
        }
        isMapMovingByMiniMap = false;
        
        Touch* touch = touches.at(0);
        isTouchBeganFromMiniMap = false;
        if (cocos2d::Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, cocos2d::Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
            //            if(selectedArray.size() == 0){
            Vec2 newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
            Vec2 newPos = -newTargetPos + size/2;
            moveScreen(newPos);
            //            }
            isTouchBeganFromMiniMap = true;
            isMapMovingByMiniMap = true;
        }
        Vec2 location = touch->getLocationInView();
                        
        location = Director::getInstance()->convertToGL(location);
        
                        
        if (EHUD->selectedMode == MODE_SELECT && EHUD->selectedUnitForEditDetail != nullptr && EHUD->selectedUnitForEditDetail->getBoundingBox().containsPoint(location - getPosition())) {
            isSelectedUnitSelected = true;
            selectedArrayPlacedPosition = getCoordinateFromPosition(EHUD->selectedUnitForEditDetail->getPosition());
            selectedUnitsBrush = placedArray[(int)selectedArrayPlacedPosition.y][(int)selectedArrayPlacedPosition.x];
        }else{
            isSelectedUnitSelected = false;
        }
    };
    listener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event* evt){
        //        log("onTouchesMoved. count: %d", (int)touches.size());
        //        log("x: %f, y: %f", touches.at(0)->getLocation().x, touches.at(0)->getLocation().y);
        Touch* touch = touches.at(0);
        if (buildingTemplete != nullptr && (!isBuildingReadyToBuild || touchBeganPos.distanceSquared(touch->getLocation()) > 10000)) { // building positioning
            float extraY = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
            extraY = TILE_SIZE;
#endif
            Vec2 coordinate = getCoordinateFromPosition(touch->getLocation() + Vec2(0, TILE_SIZE +extraY), theMap);
            if(buildingTempleteCoordinate != coordinate){
                //                log("diff x: %d, y: %d", (int)coordinate.x, (int)coordinate.y);
                isBuildingTempleteMoved = true;
                Vec2 pos = this->getPositionFromTileCoordinate(coordinate.x - buildingTempleteSize.width/2 + 1, coordinate.y + buildingTempleteSize.height/2) - Vec2(getPositionX() - (int)getPositionX()%100, getPositionY() - (int)getPositionY()%100);
                if(pos.x < TILE_SIZE){
                    pos.x = TILE_SIZE;
                }
                if(pos.y < TILE_SIZE){
                    pos.y = TILE_SIZE;
                }
                if(pos.x >= (mapSize.width - buildingTempleteSize.width)*TILE_SIZE){
                    pos.x = (mapSize.width - buildingTempleteSize.width)*TILE_SIZE;
                }
                if(pos.y >= (mapSize.height - buildingTempleteSize.height)*TILE_SIZE){
                    pos.y = (mapSize.height - buildingTempleteSize.height)*TILE_SIZE;
                }
                buildingTemplete->setPosition(Vec2(pos.x - ((int)pos.x)%100, pos.y - ((int)pos.y)%100));
                //                buildingTemplete->setPosition(pos);
                checkBuildingTemplete();
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
        if (isTouchBeganFromMiniMap && touchBeganToEndDiff > 150){ // click
            if (cocos2d::Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, cocos2d::Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
                isMapMovingByMiniMap = true;
                Vec2 newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                Vec2 newPos = -newTargetPos + size/2;
                moveScreen(newPos);
                
                
            }
        }
        
        EHUD->draw->clear();
        //        if(twoTouchEnabled || touches.size() > 1){
//        if(secondTouch != nullptr && touches.size() > 1){
        if(touches.size() >= 1 && EHUD->selectedMode == MODE_SELECT){
            Vec2 dtPos = touches.at(0)->getLocation() - lastTouchPoint;
            if (EHUD->selectedUnitForEditDetail != nullptr && isSelectedUnitSelected) {
                EHUD->selectedUnitForEditDetail->setPosition(EHUD->selectedUnitForEditDetail->getPosition() + dtPos);
            }else{

                moveScreen(getPosition() + dtPos);
                
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
            }
            lastTouchPoint = touches.at(0)->getLocation();
            
        }else if(touchCount < 2 && !isMapMovingByMiniMap){
//            EHUD->draw->drawRect(touchBeganPos, touches.at(0)->getLocation(), Color4F::GREEN);
            Touch *touch = touches.at(0);
            Vec2 location = touch->getLocationInView();
            if(location.x < 0 || location.y < 0 || location.x >= size.width || location.y >= size.height){
                return;
            }
            location = Director::getInstance()->convertToGL(location);
            Vec2 pos = location - getPosition();
            if(EHUD->selectedMode == MODE_PENCIL || EHUD->selectedMode == MODE_ERASER){
                Vec2 newCoorinate = getCoordinateFromPosition(pos);
                if (lastBrushedCoordinate != newCoorinate) {
                    EHUD->doBrush(pos);
                    lastBrushedCoordinate = newCoorinate;
                }
            }
            
        }
        //        log("touchCount: %d", touchCount);
    };
    listener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event* evt){
        //        log("onTouchesEnded. count: %d", (int)touches.size());
        if (buildingTemplete != nullptr) {
            if(!isBuildingTempleteMoved){
                buildingTemplete->removeFromParent();
                buildingTemplete = nullptr;
                selectCommand(COMMAND_NOTHING);
            }else if(isBuildingReadyToBuild && selectedArray.size() == 1 && selectedArray.at(0)->unitType == UNIT_WORKER){
                selectedArray.at(0)->builderSize = buildingTempleteSize;
                selectedArray.at(0)->builderBuildingIndex = buildingTemplete->getTag();
                selectedArray.at(0)->builderCoordinate = buildingTempleteCoordinate;
                selectedArray.at(0)->builderSpriteName = buildingTemplete->getName();
                selectedArray.at(0)->isGoingToBuild = true;
                moveTo(selectedArray, this->getPositionFromTileCoordinate(buildingTempleteCoordinate.x, buildingTempleteCoordinate.y) + Vec2(buildingTempleteSize.width*TILE_SIZE/2, buildingTempleteSize.height*TILE_SIZE/2));
                
                this->onBuildingBetterClick(); // cancel current menues
                
                buildingTemplete->removeFromParent();
                buildingTemplete = nullptr;
                selectCommand(COMMAND_NOTHING);
            }
            
            return;
        }
        Touch* touch = touches.at(0);
        if(twoTouchEnabled){
            cocos2d::Size mapSize = theMap->getMapSize()*TILE_SIZE;
            Vec2 currentPos = getPosition();
            this->stopAllActions();
            if(this->getPosition().x < -mapSize.width + size.width){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(-mapSize.width + size.width-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y < -mapSize.height + size.height){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(0, -mapSize.height + size.height-getPositionY())), 3));
            }
            if(this->getPosition().x > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(0, -getPositionY())), 3));
            }
        }
        
        bool somethingPicked = false;
        Vec2 pos = lastTouchPoint - getPosition();
        for(auto unit: heroArray){
            if(unit->getBoundingBox().containsPoint(pos)){
                somethingPicked = true;
                
                break;
            }
        }
        
        if(touchCount == 1 && !twoTouchEnabled){
            float touchBeganToEndDiff = touchBeganPos.distanceSquared(touch->getLocation());
            if(touchBeganToEndDiff < 2000 && !isMapMovingByMiniMap){ // click
                if (isTouchBeganFromMiniMap && cocos2d::Rect(drawMiniMapForMoving->getPosition() + miniMapDrawStartPos, cocos2d::Size(miniMapWidth, miniMapHeight)).containsPoint(touch->getLocation())) {   // click on minimap
                    if(false){
                        if(!isMapMovingByMiniMap){
                            Vec2 newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                            if(selectedArray.size() > 0){
                                doClick(newTargetPos);
                            }else{
                                
                            }
                        }
                    }else{
                        Vec2 newTargetPos = (touch->getLocation() - (drawMiniMapForMoving->getPosition() + miniMapDrawStartPos))/miniMapScale;
                        Vec2 newPos = -newTargetPos + size/2;
                        moveScreen(newPos);
                    }
                }else{
                    
                }
                
                Touch *touch = touches.at(0);
                Vec2 location = touch->getLocationInView();
                location = Director::getInstance()->convertToGL(location);
                if(EHUD->selectedMode == MODE_PENCIL || EHUD->selectedMode == MODE_ERASER){
                    Vec2 pos = location - getPosition();
                    Vec2 newCoorinate = getCoordinateFromPosition(pos);
                    if (lastBrushedCoordinate != newCoorinate) {
                        EHUD->doBrush(pos);
                        lastBrushedCoordinate = newCoorinate;
                    }
                    
                }else if(EHUD->selectedMode == MODE_SELECT){
//                    EHUD->showDetailPopup(location - getPosition());
                    EHUD->hideOptions();
                    log("doubleClickTimer: %f", doubleClickTimer);
                    if (doubleClickTimer > 0) {
                        EHUD->showOptions(location - getPosition(), true);
                    }else{
                        EHUD->showOptions(location - getPosition(), false);
                    }
                    doubleClickTimer = 0.3f;
                }
                
            }else{ // drag
                if(!isMapMovingByMiniMap){
//                    cocos2d::Rect rect = cocos2d::Rect(touchBeganPos-this->getPosition(), cocos2d::Size( touch->getLocation().x - touchBeganPos.x, touch->getLocation().y - touchBeganPos.y));
//                    selectByDrag(rect);
                    GM->playSoundEffect(SOUND_PENCIL_SHORT);
                }
            }
            if (EHUD->selectedUnitForEditDetail != nullptr && isSelectedUnitSelected) {
                
                
                Vec2 coordinate = getCoordinateFromPosition(EHUD->selectedUnitForEditDetail->getPosition());
                
                int owner = placedArray[(int)coordinate.y][(int)coordinate.x];
                
                placedArray[(int)selectedArrayPlacedPosition.y][(int)selectedArrayPlacedPosition.x] = owner;
                placedArray[(int)coordinate.y][(int)coordinate.x] = selectedUnitsBrush;
                Vec2 oldCoordinate = selectedArrayPlacedPosition;
                if (owner > 0) {
                    for(auto child : spriteBatch->getChildren()){
                        Movable* unit = (Movable*)child;
                        if (unit != EHUD->selectedUnitForEditDetail && unit->getBoundingBox().containsPoint(EHUD->selectedUnitForEditDetail->getPosition())) {
                            Vec2 oldPos = getPositionFromTileCoordinate(selectedArrayPlacedPosition.x, selectedArrayPlacedPosition.y);
                            unit->setPosition(oldPos);
                            log("%d, %f, %f, ",unit->unitType, unit->getPositionX(), unit->getPositionY());
                            unit->setTag(selectedArrayPlacedPosition.x + selectedArrayPlacedPosition.y*mapSizeWidth);
                            break;
                        }
                    }
                }
                
                EHUD->selectedUnitForEditDetail->setPosition(getPositionFromTileCoordinate(coordinate.x, coordinate.y));
                EHUD->selectedUnitForEditDetail->setTag(coordinate.x + coordinate.y*mapSizeWidth);
                EHUD->showSelectedCircle(true);
            }
            
        }
        touchCount -= touches.size();
        if(touchCount <= 0){
            touchCount = 0;
            twoTouchEnabled = false;
        }
        updateMiniMapForNonMoving();
        EHUD->draw->clear();
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
        Vec2 pos = evt->getLocationInView();
        //        log("x: %f, y: %f", pos.x, pos.y);
        
        isMapMoveLeft = pos.x < 10;
        isMapMoveRight = pos.x > size.width - 10;
        isMapMoveDown = pos.y < 10;
        isMapMoveUp = pos.y > size.height - 10;
        //        EHUD->draw->clear();
        //        if(twoTouchEnabled || touches.size() > 1){
        if((int)(evt->getMouseButton()) == 1){
            Vec2 dtPos = pos - lastTouchPoint;
            this->setPosition(getPosition() + dtPos);
            lastTouchPoint = pos;
        }else if((int)(evt->getMouseButton()) == 0){
            //            EHUD->draw->drawRect(touchBeganPos, evt->getLocationInView(), Color4F::GREEN);
        }
    };
    mouseListener->onMouseUp = [&](EventMouse* evt){
        
        if((int)(evt->getMouseButton()) == 1){
            cocos2d::Size mapSize = theMap->getMapSize()*TILE_SIZE;
            Vec2 currentPos = getPosition();
            if(this->getPosition().x < -mapSize.width + size.width){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(-mapSize.width + size.width-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y < -mapSize.height + size.height){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(0, -mapSize.height + size.height-getPositionY())), 3));
            }
            if(this->getPosition().x > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(-getPositionX(), 0)), 3));
            }
            if(this->getPosition().y > 0){
                this->runAction(EaseOut::create(MoveBy::create(0.2f, Vec2(0, -getPositionY())), 3));
            }
        }
        //        EHUD->draw->clear();
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(mouseListener, 1);
    
    /*listener = cocos2d::EventListenerTouchOneByOne::create();
     listener->setSwallowTouches(false);
     
     listener->onTouchBegan = CC_CALLBACK_2(EditorWorld::onTouchBegan, this);
     listener->onTouchMoved = CC_CALLBACK_2(EditorWorld::onTouchMoved, this);
     listener->onTouchEnded = CC_CALLBACK_2(EditorWorld::onTouchEnded, this);
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

void EditorWorld::moveScreen(cocos2d::Vec2 pos){
    if(pos.x > 0){
        pos.x = 0;
    }
    if(pos.y > 600){
        pos.y = 600;
    }
    if(pos.y < -TILE_SIZE*mapSize.height + size.height){
        pos.y = -TILE_SIZE*mapSize.height + size.height;
    }
    if(pos.x < -TILE_SIZE*mapSize.width + size.width){
        pos.x = -TILE_SIZE*mapSize.width + size.width;
    }
    this->setPosition(pos);
    miniMapViewRect = cocos2d::Rect(miniMapDrawStartPos.x + (-pos.x)*miniMapScale, miniMapDrawStartPos.y + (-pos.y)*miniMapScale, size.width*miniMapScale, size.height*miniMapScale);
    viewRect = cocos2d::Rect(-pos, size);
    
    updateMiniMapForMoving();
}
void EditorWorld::doClick(cocos2d::Vec2 pos){
    EHUD->setMenu(0, BTN_TYPE_NONE);
    EHUD->setMenu(1, BTN_TYPE_NONE);
    EHUD->setMenu(2, BTN_TYPE_NONE);
    EHUD->setMenu(3, BTN_TYPE_NONE);
    EHUD->setMenu(4, BTN_TYPE_NONE);
    EHUD->setMenu(5, BTN_TYPE_NONE);
    selectedCommand = COMMAND_NOTHING;
    if (this->stageIndex == 0 && EHUD->tutorialIndex == 11) {
        EHUD->tutorialEnded = true;
    }
    
    EnemyBase* selectedUnit = nullptr;
    for(auto unit: heroArray){ // select hero
        if(unit->getBoundingBox().containsPoint(pos)){
            selectedUnit = unit;
            break;
        }
    }
    if(selectedUnit == nullptr){    // select enemy
        for(auto unit: enemyArray){
            if(unit->getBoundingBox().containsPoint(pos)){
                selectedUnit = unit;
                doubleClickTimer = 0;
                break;
            }
        }
    }
    EnemyBase* mutual = nullptr;
    if(selectedUnit == nullptr){
        for(auto unit: mutualArray){
            //            cocos2d::Rect rect = cocos2d::Rect(pos.x - 10, pos.y - 10, unit->getContentSize().width + 20, unit->getContentSize().height + 20);
            if(unit->getBoundingBox().containsPoint(pos)){
                //            if(unit->getBoundingBox().intersectsRect(rect)){
                mutual = unit;
                doubleClickTimer = 0;
                break;
            }
        }
    }
    if(mutual != nullptr){
        for(auto unit: selectedArray){
            if(unit->unitType == UNIT_WORKER){
                if(mutual->unitType == UNIT_MINE){
                    if(unit->isCarryingGold){
                        unit->isGatheringGold = true;
                        unit->failedFindPathStart = Vec2::ZERO;
                        unit->failedFindPathEnd = Vec2::ZERO;
                        unit->isTemporaryFlying = true;
                        unit->moveToTarget(unit->returningPlace);
                    }else{
                        unit->gatherGold(mutual);
                        if (this->stageIndex == 0 && unit->unitType == UNIT_WORKER && EHUD->tutorialIndex == 3) {
                            EHUD->tutorialIndex++;
                            EHUD->talkIndex = 0;
                            EHUD->talkText = LM->getText("tutorial 4");
                            Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
                            spt->stopAllActions();
                            spt->setSpriteFrame("tree2_0.png");
                        }
                    }
                    if(mutual != nullptr){
                        GM->playSoundEffect(SOUND_PENCIL_SHORT);
                        Sprite* spt = Sprite::createWithSpriteFrameName("pickax.png");
                        spt->setPosition(mutual->getPosition() + Vec2(0, 100));
                        this->addChild(spt, 100);
                        spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2), SPT_REMOVE_FUNC, NULL));
                    }
                }else if(mutual->unitType == UNIT_TREE || mutual->unitType == UNIT_UNREACHABLE_TREE){
                    if(mutual->unitType == UNIT_UNREACHABLE_TREE){
                        mutual->unitType = UNIT_TREE;
                    }
                    if(unit->isCarryingTree){
                        unit->isGatheringTree = true;
                        unit->failedFindPathStart = Vec2::ZERO;
                        unit->failedFindPathEnd = Vec2::ZERO;
                        unit->isTemporaryFlying = true;
                        unit->moveToTarget(unit->returningPlace);
                    }else{
                        Vec2 coordinate = this->getCoordinateFromPosition(unit->getPosition());
                        Vec2 destCoordinate = this->getCoordinateFromPosition(mutual->getApproachingPoint(unit->getPosition()));
                        PointArray* array = GM->getPath(coordinate, destCoordinate);
                        if(array->count() > 0){
                            
                        }else{
                            mutual = this->getNearestTree(unit->getPosition());
                        }
                        if(mutual != nullptr){
                            unit->gatherTree(mutual);
                            if (this->stageIndex == 0 && unit->unitType == UNIT_WORKER && EHUD->tutorialIndex == 4) {
                                EHUD->tutorialIndex++;
                                EHUD->talkIndex = 0;
                                EHUD->talkText = LM->getText("tutorial 5");
                                Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
                                spt->stopAllActions();
                                Sprite* sptTemp = Sprite::create("btnBuild.png");
                                spt->setSpriteFrame(sptTemp->getSpriteFrame());
                            }
                            GM->playSoundEffect(SOUND_PENCIL_SHORT);
                            Sprite* spt = Sprite::createWithSpriteFrameName("axe.png");
                            spt->setPosition(mutual->getPosition() + Vec2(0, 100));
                            this->addChild(spt, 100);
                            spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2), SPT_REMOVE_FUNC, NULL));
                        }
                    }
                }
            }else{
                unit->moveToTarget(mutual);
            }
        }
        if(mutual->unitType == UNIT_MINE){
            //            EHUD->showInstanceMessage(strmake("GOLD LEFT: %d", mutual->energy));
        }
    }else if (selectedUnit == nullptr) { // ground selected
        if (selectedArray.size() > 0 && !selectedArray.at(0)->isEnemy && !selectedArray.at(0)->isAlli && !selectedArray.at(0)->isBuilding) { // hero is selected so move!
            if(selectedCommand == COMMAND_ATTACK){
                moveAndAttackTo(selectedArray, pos);
            }else{
                if (doubleClickTimer > 0) {
                    onAttackClick();
                    moveAndAttackTo(selectedArray, pos);
                    selectCommand(COMMAND_MOVE);
                }else{
                    onMoveClick();
                    Movable* building = nullptr;
                    for(auto unit: heroArray){
                        if(unit->isBuilding){
                            Vec2 buildingPos = getPositionFromTileCoordinate(unit->buildingStartCoordinate.x, unit->buildingStartCoordinate.y);
                            float height = unit->buildingOccupySize.height*TILE_SIZE;
                            cocos2d::Rect theRect = cocos2d::Rect(buildingPos.x, buildingPos.y - height, unit->buildingOccupySize.width*TILE_SIZE, height);
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
                        doubleClickTimer = 0.3f;
                    }else{
                        moveTo(selectedArray, building->getApproachingPoint(pos));
                    }
                }
                //                doubleClickTimer = 0.3f;
            }
        }else{
            doubleClickTimer = 0.3f;
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
            if(unit->unitType == UNIT_WORKER){
                if (unit->isCarryingTree && (selectedUnit->unitType == UNIT_CASTLE || selectedUnit->unitType == UNIT_LUMBERMILL))  {
                    unit->returningPlace = selectedUnit;
                    unit->moveToTarget(selectedUnit);
                    isWorkingWorkerSelected = true;
                    unit->isTemporaryFlying = true;
//                    unit->runMoveAnimation(DIRECTION_E);
                    unit->isGatheringTree = true;
                }else if(unit->isCarryingGold && (selectedUnit->unitType == UNIT_CASTLE)){
                    unit->returningPlace = selectedUnit;
                    unit->moveToTarget(selectedUnit);
                    isWorkingWorkerSelected = true;
                    unit->isTemporaryFlying = true;
//                    unit->runMoveAnimation(DIRECTION_E);
                    unit->isGatheringGold = true;
                }
            }
        }
        if (!isWorkingWorkerSelected) {
            if (selectedCommand == COMMAND_ATTACK) {
                forceAttack(selectedArray, selectedUnit);
            }else{
                deselectAll();
                if (doubleClickTimer > 0 && !selectedUnit->isBuilding) {
                    for(auto unit:heroArray){
                        if(unit->unitType == selectedUnit->unitType && viewRect.containsPoint(unit->getPosition())){
                            selectUnit(unit);
                        }
                    }
                }else{
                    selectUnit(selectedUnit);
                    
                    if (this->stageIndex == 0 && EHUD->tutorialIndex == 0) {
                        EHUD->tutorialIndex++;
                        EHUD->talkIndex = 0;
                        EHUD->talkText = LM->getText("tutorial 1");
                        Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
                        Sprite* sptTemp = Sprite::create("handIcon.png");
                        spt->setSpriteFrame(sptTemp->getSpriteFrame());
                        cocos2d::Size size = Director::getInstance()->getWinSize();
                        spt->stopAllActions();
                        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750, 330 + 50)), MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.15f, Vec2(0, 100)), DelayTime::create(1), NULL)));
                    }
                }
                doubleClickTimer = 0.3f;
                GM->playSoundEffect(SOUND_PENCIL_SHORT);
            }
        }
    }
}
void EditorWorld::checkBuildingTemplete(){
    isBuildingReadyToBuild = true;
    buildingTempleteCoordinate = getCoordinateFromPosition(buildingTemplete->getPosition(), theMap);
    
    for (int j = 0; j < buildingTempleteSize.height; j++) {
        for (int i = 0; i < buildingTempleteSize.width; i++) {
            Sprite* spt = (Sprite*)buildingTemplete->getChildByTag(i + j*buildingTempleteSize.width);
            bool isBlock = isDecoBlock(decoLayer->getTileGIDAt(Vec2(buildingTempleteCoordinate.x + i, buildingTempleteCoordinate.y - j - 1)));
            spt->setColor(isBlock?Color3B::RED:Color3B::GREEN);
            if(isBlock){
                isBuildingReadyToBuild = false;
            }
        }
    }
}
EnemyBase* EditorWorld::buildTheBuilding(int index, int x, int y,  int width, int height, std::string spriteName){
    bool isPlaceReady = true;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            bool isBlock = isDecoBlock(decoLayer->getTileGIDAt(Vec2(x + i, y - j - 1)));
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
    
    Vec2 pos = getPositionFromTileCoordinate(x, y - height);
    EnemyBase* unit = createUnit(index, WHICH_SIDE_HERO, ITS_BUILDING, pos + Vec2(width*TILE_SIZE/2 - TILE_SIZE/2, -(height - 1)*TILE_SIZE + height*TILE_SIZE/2 - TILE_SIZE/2), spriteName.substr(0, spriteName.size() - 4), 1, spriteName);
    unit->isBuildingComplete = false;
    setOccupy(pos, width, height, true, unit);
    unit->setRotation(180);
    unit->retain();
    
    addGold(-price);
    addLumber(-lumberPrice);
    return unit;
}
void EditorWorld::addDecoToBuilding(Movable* unit){
    if(unit->unitType == UNIT_FARM){
        unit->schedule(schedule_selector(Movable::makingSmoke), 1.7f);
        unit->smokePoint = unit->getPosition() + Vec2(-18, 110);
    }else if(unit->unitType == UNIT_AIRPORT){
        Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
        GM->runAnimation(spt, "propeller", true);
        unit->addChild(spt);
        spt->setPosition(Vec2(134, 206));
    }else if(unit->unitType == UNIT_CASTLE){
        Sprite* spt = Sprite::createWithSpriteFrameName("blueFlag0.png");
        GM->runAnimation(spt, "blueFlag", true);
        unit->addChild(spt);
        spt->setPosition(Vec2(192, 255));
    }
}

std::string EditorWorld::getSpriteNameForUnit(int index){
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
    }else if(index == UNIT_TREE_FOR_BATTLE || index == UNIT_TREE){
        return "tree3_0.png";
    }else if(index == UNIT_WIZARD){
        return "wizardStand0.png";
    }
    return "workerAxeStand0.png";
}

std::string EditorWorld::getUnitName(int index){
    if(index == UNIT_WORKER){
        return "workerAxe";
    }else if(index == UNIT_SWORDMAN){
        return "swordman";
    }else if(index == UNIT_ARCHER){
        return "archer";
    }else if(index == UNIT_GOBLIN){
        return "goblin";
    }else if(index == UNIT_ORC_SPEAR){
        return "orcSpear";
    }else if(index == UNIT_ORC_AXE){
        return "orcAxe";
    }else if(index == UNIT_HELICOPTER){
        return "helicopter";
    }else if(index == UNIT_CATAPULT){
        return "catapult";
    }else if(index == UNIT_CASTLE){
        return "castle";
    }else if(index == UNIT_FARM){
        return "farm";
    }else if(index == UNIT_BARRACKS){
        return "barracks";
    }else if(index == UNIT_LUMBERMILL){
        return "lumberMill";
    }else if(index == UNIT_WATCHERTOWER){
        return "watcherTower";
    }else if(index == UNIT_ORC_BUNKER){
        return "bunker";
    }else if(index == UNIT_ORC_HQ){
        return "hq";
    }else if(index == UNIT_AIRPORT){
        return "airport";
    }else if(index == UNIT_FACTORY){
        return "factory";
    }else if(index == UNIT_ZOMBIE_ORC_AXE){
        return "zombieOrcAxe";
    }else if(index == UNIT_ZOMBIE_SWORDSMAN){
        return "zombieSwordsman";
    }
    return "";
}
Sprite* EditorWorld::getSpriteForIcon(int index){
    Sprite* sptBuilding;
    if(index ==  UNIT_WORKER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("workerAxeStand0.png");
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
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x + 20, sptBuilding->getTextureRect().origin.y, 160, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_CASTLE  ){
        sptBuilding = Sprite::createWithSpriteFrameName("castle.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y + 50, 230, 210));
    }
    else if(index ==  UNIT_FARM  ){
        sptBuilding = Sprite::createWithSpriteFrameName("farm.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_BARRACKS  ){
        sptBuilding = Sprite::createWithSpriteFrameName("barracks.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_LUMBERMILL  ){
        sptBuilding = Sprite::createWithSpriteFrameName("lumberMill.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x + 0, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }else if(index ==  UNIT_WATCHERTOWER  ){
        sptBuilding = Sprite::createWithSpriteFrameName("watcherTower.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, sptBuilding->getContentSize().width, 210));
    }else if(index ==  UNIT_FACTORY  ){
        sptBuilding = Sprite::createWithSpriteFrameName("factory.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y , 230, sptBuilding->getContentSize().height));
    }
    else if(index ==  UNIT_AIRPORT  ){
        sptBuilding = Sprite::createWithSpriteFrameName("airport.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x, sptBuilding->getTextureRect().origin.y, sptBuilding->getContentSize().width, sptBuilding->getContentSize().height));
        Sprite* spt = Sprite::createWithSpriteFrameName("airportPropeller0.png");
        sptBuilding->addChild(spt);
        spt->setPosition(Vec2(134, 206));
    }else if(index ==  UNIT_TREE){
        sptBuilding = Sprite::createWithSpriteFrameName("tree2_0.png");
    }else if(index ==  UNIT_MINE){
        sptBuilding = Sprite::createWithSpriteFrameName("mine.png");
    }else if(index ==  UNIT_ORC_AXE){
        sptBuilding = Sprite::createWithSpriteFrameName("orcAxeStand0.png");
    }else if(index ==  UNIT_ORC_SPEAR){
        sptBuilding = Sprite::createWithSpriteFrameName("orcSpearStand0.png");
    }else if(index ==  UNIT_TROLL){
        sptBuilding = Sprite::createWithSpriteFrameName("trollStand0.png");
    }else if(index ==  UNIT_GOBLIN_WORKER){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinAxeStand0.png");
    }else if(index ==  UNIT_GOBLIN){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinStand0.png");
    }else if(index ==  UNIT_GOBLIN_BOMB){
        sptBuilding = Sprite::createWithSpriteFrameName("goblinBombStand0.png");
    }else if(index ==  UNIT_ORC_BUNKER){
        sptBuilding = Sprite::createWithSpriteFrameName("bunker.png");
    }else if(index ==  UNIT_ORC_HQ){
        sptBuilding = Sprite::createWithSpriteFrameName("hq.png");
//        sptBuilding->setTextureRect(cocos2d::Rect(sptBuilding->getTextureRect().origin.x + 50, sptBuilding->getTextureRect().origin.y + 50 , 220, 210));
    }else if(index ==  UNIT_ZOMBIE_ORC_AXE){
        sptBuilding = Sprite::createWithSpriteFrameName("zombieOrc0.png");
    }else if(index ==  UNIT_ZOMBIE_SWORDSMAN){
        sptBuilding = Sprite::createWithSpriteFrameName("zombieHuman0.png");
    }else if(index ==  UNIT_WIZARD){
        sptBuilding = Sprite::createWithSpriteFrameName("wizardStand0.png");
    }else if(index ==  UNIT_ORC_BARRACKS){
        sptBuilding = Sprite::createWithSpriteFrameName("axeport.png");
    }else if(index ==  UNIT_BARBECUE){
        sptBuilding = Sprite::createWithSpriteFrameName("barbecue.png");
    }else if(index ==  UNIT_ORC_TROLL_HOUSE){
        sptBuilding = Sprite::createWithSpriteFrameName("statueHouse.png");
    }else if(index ==  UNIT_TEMPLE){
        sptBuilding = Sprite::createWithSpriteFrameName("alter.png");
    }
    
    return sptBuilding;
}
bool EditorWorld::isThisSpotAvailable(Movable* me){
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
    
    if(isDecoBlock(decoLayer->getTileGIDAt(getCoordinateFromPosition(me->getPosition())))){
        return false;
    }
    return true;
}
void EditorWorld::tryCreateUnit(int index){
    int goldPrice = getGoldPriceForUnit(index);
    int lumberPrice = getLumberPriceForUnit(index);
    if(gold < goldPrice){
        //        EHUD->showInstanceMessage(LM->getText("not enough gold"));
        EHUD->showPriceInfo(LM->getText("not enough gold"), goldPrice, lumberPrice,  getFoodUse(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else if(lumber < lumberPrice){
        //        EHUD->showInstanceMessage(LM->getText("not enough lumber"));
        EHUD->showPriceInfo(LM->getText("not enough lumber"), goldPrice, lumberPrice,  getFoodUse(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else if(foodInUse + getFoodUse(index) > foodMax){
        //        EHUD->showInstanceMessage(LM->getText("not enough food"));
        EHUD->showPriceInfo(LM->getText("not enough food"), goldPrice, lumberPrice,  getFoodUse(index));
        GM->playSoundEffect(SOUND_NAGATIVE);
    }else{
        if (selectedArray.at(0)->btns.size() < 5) {
            addGold(-goldPrice);
            addLumber(-lumberPrice);
            selectedArray.at(0)->queueUnit(index);
        }
    }
}
bool EditorWorld::tryBuilding(int index){
    
    return true;
}
void EditorWorld::createBuildingTemplete(int index, int width, int height, std::string spriteName){
    buildingTemplete = Node::create();
    this->addChild(buildingTemplete, 10);
    buildingTemplete->setTag(index);
    moveScreen(getPosition());// to update viewRect
    Vec2 pos = viewRect.origin + size/2;
    buildingTemplete->setPosition(Vec2(pos.x - (int)(pos.x)%100, pos.y - (int)(pos.y)%100 - 1));
    buildingTemplete->setName(spriteName);
    Sprite* spt = Sprite::createWithSpriteFrameName(spriteName);
    buildingTemplete->addChild(spt, 1);
    spt->setPosition(Vec2(width*TILE_SIZE/2, height*TILE_SIZE/2));
    spt->setOpacity(150);
    buildingTempleteSize = cocos2d::Size(width, height);
    for (int i = 0; i < width*height; i++) {
        spt = Sprite::createWithSpriteFrameName("whiteRect.png");
        buildingTemplete->addChild(spt);
        spt->setAnchorPoint(Vec2::ZERO);
        spt->setPosition(Vec2((i%width)*TILE_SIZE, (i/width)*TILE_SIZE));
        spt->setTag(i);
        spt->setOpacity(150);
    }
    checkBuildingTemplete();
}
void EditorWorld::onMoveClick(){
    if(selectedCommand == COMMAND_BUILD){
        
    }else if(selectedCommand == COMMAND_BUILD_BETTER){
        int goldPrice = getGoldPriceForUnit(UNIT_AIRPORT);
        int lumberPrice = getLumberPriceForUnit(UNIT_AIRPORT);
        if(gold < goldPrice){
            EHUD->showInstanceMessage(LM->getText("not enough gold"));
            GM->playSoundEffect(SOUND_NAGATIVE);
        }if(lumber < lumberPrice){
            EHUD->showInstanceMessage(LM->getText("not enough lumber"));
            GM->playSoundEffect(SOUND_NAGATIVE);
        }else{
            createBuildingTemplete(UNIT_AIRPORT, 3, 3, "airport.png");
        }
    }else{
        selectCommand(COMMAND_MOVE);
    }
}
void EditorWorld::onStopClick(){
    if(selectedCommand == COMMAND_BUILD){
        
    }else{
        selectCommand(COMMAND_STOP);
        stop(selectedArray);
        selectCommand(COMMAND_NOTHING);
    }
}
void EditorWorld::onAttackClick(){
    if(selectedCommand == COMMAND_BUILD){
        tryBuilding(UNIT_BARRACKS);
    }else{
        selectCommand(COMMAND_ATTACK);
    }
}
void EditorWorld::onGatherClick(){
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
            EHUD->showInstanceMessage(LM->getText("No resources are found."));
        }
    }
    
}
void EditorWorld::onBuildingClick(){
    if(selectedArray.size() == 0 || selectedArray.at(0) == 0 || selectedArray.at(0)->unitType != UNIT_WORKER) return;
    if(selectedCommand == COMMAND_BUILD){
        
    }else{
        selectCommand(COMMAND_BUILD);
        
        EHUD->setMenu(0, BTN_TYPE_CASTLE);
        EHUD->setMenu(1, BTN_TYPE_FARM);
        EHUD->setMenu(2, BTN_TYPE_BARRACKS);
        EHUD->setMenu(3, BTN_TYPE_LUMBER);
        EHUD->setMenu(4, BTN_TYPE_WATCHER_TOWER);
        EHUD->setMenu(5, BTN_TYPE_CANCEL);
        EHUD->arrangeMenu(selectedArray.at(0)->getPosition());
    }
}
void EditorWorld::onBuildingBetterClick(){
    if(selectedArray.size() == 0 || selectedArray.at(0)->unitType != UNIT_WORKER) return;
    if(selectedCommand == COMMAND_BUILD || selectedCommand == COMMAND_BUILD_BETTER){
        
    }else{
        selectCommand(COMMAND_BUILD_BETTER);
        EHUD->setMenu(0, BTN_TYPE_AIRPORT);
        EHUD->setMenu(1, BTN_TYPE_FACTORY);
        EHUD->setMenu(2, BTN_TYPE_NONE);
        EHUD->setMenu(3, BTN_TYPE_NONE);
        EHUD->setMenu(4, BTN_TYPE_NONE);
        EHUD->setMenu(5, BTN_TYPE_CANCEL);
        EHUD->arrangeMenu(selectedArray.at(0)->getPosition());
    }
}

void EditorWorld::onSelectOnScreenClick(){
    
}
void EditorWorld::onSelectOnMapClick(){
    
}
void EditorWorld::selectCommand(int command){
    selectedCommand = command;
    //    if(command == COMMAND_NOTHING){
    //        EHUD->lblCommand->setString("CMD: nothing");
    //    }else if(command == COMMAND_MOVE){
    //        EHUD->lblCommand->setString("CMD: move");
    //    }else if(command == COMMAND_STOP){
    //        EHUD->lblCommand->setString("CMD: stop");
    //    }else if(command == COMMAND_ATTACK){
    //        EHUD->lblCommand->setString("CMD: attack");
    //    }else if(command == COMMAND_GATHER){
    //        EHUD->lblCommand->setString("CMD: gather");
    //    }else if(command == COMMAND_BUILD){
    //        EHUD->lblCommand->setString("CMD: build");
    //    }else if(command == COMMAND_BUILD){
    //        EHUD->lblCommand->setString("CMD: build better");
    //    }
}
void EditorWorld::moveTo(EnemyBase* unit, Vec2 pos){
    targetHand->setPosition(pos);
    unit->moveToTarget(targetHand);
    unit->canFindTarget = false;
    unit->attackFlagPos = Vec2::ZERO;
}
void EditorWorld::moveTo(Vector<EnemyBase*> troop, Vec2 pos){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(pos, false);
    bool isHelicopter = false;
    for (auto unit: troop){
        unit->isTemporaryFlying = false;
        unit->moveToTarget(targetHand);
        unit->canFindTarget = false;
        unit->attackFlagPos = Vec2::ZERO;
        unit->isGatheringTree = false;
        unit->isGatheringGold = false;
        if(unit->unitType == UNIT_HELICOPTER){
            isHelicopter = true;
        }
    }
    if (isHelicopter) {
        GM->playSoundEffect(SOUND_HELICOPTER);
    }
    
    if (this->stageIndex == 0 && EHUD->tutorialIndex == 1) {
        EHUD->tutorialIndex++;
        EHUD->talkIndex = 0;
        EHUD->talkText = LM->getText("tutorial 2");
        Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
        Sprite* sptTemp = Sprite::create("handIcon2.png");
        spt->setSpriteFrame(sptTemp->getSpriteFrame());
        cocos2d::Size size = Director::getInstance()->getWinSize();
        spt->stopAllActions();
        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750 - 100, 330 + 50)), MoveBy::create(0.5f, Vec2(150, -200)), DelayTime::create(1), NULL)));
    }
}
void EditorWorld::moveTo(Vector<EnemyBase*> troop, EnemyBase* target){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(target->getPosition(), false);
    for (auto unit: troop){
        unit->moveToTarget(target);
        unit->canFindTarget = false;
        unit->attackFlagPos = Vec2::ZERO;
    }
}
void EditorWorld::gatherTo(Vector<EnemyBase*> troop, EnemyBase* target){
    GM->playSoundEffect(SOUND_PENCIL_SHORT);
    showTargetHand(target->getPosition(), false);
    for (auto unit: troop){
        unit->moveToTarget(target);
        unit->canFindTarget = false;
    }
}
void EditorWorld::showTargetHand(cocos2d::Vec2 pos, bool isAttack){
    targetHand->setPosition(pos);
    Sprite* spt = Sprite::create(isAttack?"swordIcon.png":"handIcon.png");
    this->addChild(spt, 100);
    spt->setPosition(pos + Vec2(50, 50));
    spt->setOpacity(0);
    spt->runAction(Sequence::create(FadeIn::create(0.1f), DelayTime::create(0.5f), FadeOut::create(0.1f), SPT_REMOVE_FUNC, NULL));
    spt->runAction(Sequence::create(EaseInOut::create(MoveBy::create(0.3f, Vec2(0, -50)), 2), DelayTime::create(0.1f), EaseInOut::create(MoveBy::create(0.3f, Vec2(0, 50)), 2), NULL));
    
    Node* node = Node::create();
    this->addChild(node, 10);
    node->setPosition(pos);
    spt = Sprite::create("targetCircle.png");
    node->addChild(spt);
    spt->setOpacity(0);
    node->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, node)), NULL));
    float dur = 0.2f;
    spt->runAction(Sequence::create(FadeIn::create(dur), DelayTime::create(0.6f - dur*2), FadeOut::create(dur), NULL));
    spt->runAction(Sequence::create(MoveBy::create(dur, Vec2(0, 10)), DelayTime::create(0.6f - dur*2), MoveBy::create(dur, Vec2(0, -10)), NULL));
}
void EditorWorld::stop(Vector<EnemyBase*> troop){
    for(auto unit: selectedArray){
        unit->stop();
    }
}
void EditorWorld::moveAndAttackTo(EnemyBase* unit, cocos2d::Vec2 pos){
    unit->targetCoordinate = Vec2::ZERO;
    unit->target = nullptr;
    unit->isTemporaryFlying = false;
    unit->moveToTarget(pos);
    unit->canFindTarget = unit->attackType != ATTACK_TYPE_NONE;
    unit->attackFlagPos = pos;
}
void EditorWorld::moveAndAttackTo(Vector<EnemyBase*> troop, Vec2 pos){
    showTargetHand(pos, true);
    for (auto unit: troop){
        moveAndAttackTo(unit, pos);
    }
    selectCommand(COMMAND_NOTHING);
    
    if (this->stageIndex == 0 && EHUD->tutorialIndex == 10) {
        EHUD->tutorialIndex++;
        EHUD->talkIndex = 0;
        EHUD->talkText = LM->getText("tutorial 11");
        Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
        spt->setVisible(false);
        //                    spt->setSpriteFrame("handIcon.png");
        //                    cocos2d::Size size = Director::getInstance()->getWinSize();
        //                    spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750 - 50, 330 - 50)), MoveBy::create(0.5f, Vec2(100, -100)), DelayTime::create(1), NULL)));
    }
}
void EditorWorld::forceAttack(Vector<EnemyBase*> troop, EnemyBase* target){
    if(target->unitType == UNIT_TREE || target->unitType == UNIT_MINE){
        
        return;
    }
    showTargetHand(target->getPosition(), true);
    for (auto unit: troop){
        if(unit->attackType == ATTACK_TYPE_NONE || !canAttack(unit, target)){
            unit->moveToTarget(target);
            continue;
        }
        unit->moveToTarget(target);
        unit->forceAttackTarget = true;
        unit->canFindTarget = false;
        unit->attackFlagPos = Vec2::ZERO;
    }
    selectCommand(COMMAND_NOTHING);
}

void EditorWorld::selectByDrag(cocos2d::Rect rect){
    deselectAll();
    //    draw->drawRect(rect.origin, Vec2(rect.getMaxX(), rect.getMaxY()), Color4F::RED);
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
    
    if (this->stageIndex == 0 && EHUD->tutorialIndex == 9) {
        EHUD->tutorialIndex++;
        EHUD->talkIndex = 0;
        EHUD->talkText = LM->getText("tutorial 10");
        Sprite* spt = (Sprite*)EHUD->tutorialNode->getChildByName("sptIcon");
        Sprite* sptTemp = Sprite::create("handIcon.png");
        spt->setSpriteFrame(sptTemp->getSpriteFrame());
        cocos2d::Size size = Director::getInstance()->getWinSize();
        spt->stopAllActions();
        spt->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0, Vec2(size.width/2 - 750, 330 + 50)), MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.1f, Vec2(0, 100)), MoveBy::create(0.1f, Vec2(0, -100)), MoveBy::create(0.15f, Vec2(0, 100)), DelayTime::create(1), NULL)));
    }
}
void EditorWorld::deselectAll(){
    for(auto spt: selectedArray){
        spt->showSelectedCircle(false);
    }
    selectedArray.clear();
    updateMenu();
}
void EditorWorld::deselect(Movable* unit){
    if(selectedArray.find((EnemyBase*)unit) != selectedArray.end()){
        selectedArray.eraseObject((EnemyBase*)unit);
        unit->showSelectedCircle(false);
        //        updateMenu();
    }
}
void EditorWorld::selectUnit(EnemyBase* unit){
    if(selectedArray.size() == 1 && selectedArray.at(0) == unit){
        //        doubleClickTimer = 0.3f;
    }else{
        //        doubleClickTimer = -1;
    }
    doubleClickTimer = -1;
    if(unit->isVisible()){
        selectedArray.pushBack(unit);
        unit->showSelectedCircle(true);
        updateMenu();
    }
}
void EditorWorld::updateFoodMaxState(){
    int max = 0;
    for(auto unit: heroArray){
        if(unit->isBuildingComplete && !unit->isAlli){
            max += getFoodGive(unit->unitType);
        }
    }
    foodMax = max;
    EHUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
    checkClearGame();
}
void EditorWorld::updateMenu(){
    if(buildingTemplete != nullptr){
        buildingTemplete->removeFromParent();
        buildingTemplete = nullptr;
    }
    selectCommand(COMMAND_NOTHING);
    if(selectedArray.size() == 0 || selectedArray.size() > 1){
        EHUD->setMenu(0, BTN_TYPE_NONE);
        EHUD->setMenu(1, BTN_TYPE_NONE);
        EHUD->setMenu(2, BTN_TYPE_NONE);
        EHUD->setMenu(3, BTN_TYPE_NONE);
        EHUD->setMenu(4, BTN_TYPE_NONE);
        EHUD->setMenu(5, BTN_TYPE_NONE);
    }else{
        if (selectedArray.at(0)->isBuilding && selectedArray.at(0)->isBuildingComplete) {
            if(selectedArray.at(0)->unitType == UNIT_CASTLE){
                EHUD->setMenu(0, BTN_TYPE_WORKER);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_FARM){
                EHUD->setMenu(0, BTN_TYPE_NONE);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_BARRACKS){
                EHUD->setMenu(0, BTN_TYPE_SWORDMAN);
                EHUD->setMenu(1, BTN_TYPE_ARCHER);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_LUMBERMILL){
                EHUD->setMenu(0, BTN_TYPE_NONE);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_WATCHERTOWER){
                EHUD->setMenu(0, BTN_TYPE_NONE);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_AIRPORT){
                EHUD->setMenu(0, BTN_TYPE_HELICOPTER);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }else if(selectedArray.at(0)->unitType == UNIT_FACTORY){
                EHUD->setMenu(0, BTN_TYPE_CATAPULT);
                EHUD->setMenu(1, BTN_TYPE_NONE);
                EHUD->setMenu(2, BTN_TYPE_NONE);
            }
            EHUD->setMenu(3, BTN_TYPE_NONE);
            EHUD->setMenu(4, BTN_TYPE_NONE);
            EHUD->setMenu(5, BTN_TYPE_NONE);
            EHUD->arrangeMenu(selectedArray.at(0)->getPosition());
        }else{
            //            EHUD->setMenu(0, BTN_TYPE_MOVE);
            //            EHUD->setMenu(1, BTN_TYPE_STOP);
            bool canAttack = false;
            bool canGather = true;
            for(auto unit:selectedArray){
                if(unit->unitType != UNIT_WORKER){
                    canGather = false;
                }
                if (unit->unitType != UNIT_HELICOPTER) {
                    canAttack = true;
                }
            }
            //            EHUD->setMenu(2, canAttack? BTN_TYPE_ATTACK:BTN_TYPE_NONE);
            //            EHUD->setMenu(3, canGather? BTN_TYPE_GATHER:BTN_TYPE_NONE);
            //            EHUD->setMenu(4, canGather? BTN_TYPE_BUILD:BTN_TYPE_NONE);
            //            EHUD->setMenu(5, canGather? BTN_TYPE_BUILD_BETTER:BTN_TYPE_NONE);
            if (canGather) {
                selectedArray.at(0)->isGoingToBuild = false;
                EHUD->setMenu(0, BTN_TYPE_BUILD);
                EHUD->setMenu(1, BTN_TYPE_BUILD_BETTER);
            }else{
                EHUD->setMenu(0, BTN_TYPE_NONE);
                EHUD->setMenu(1, BTN_TYPE_NONE);
            }
            EHUD->setMenu(2, BTN_TYPE_NONE);
            EHUD->setMenu(3, BTN_TYPE_NONE);
            EHUD->setMenu(4, BTN_TYPE_NONE);
            EHUD->setMenu(5, BTN_TYPE_NONE);
            EHUD->arrangeMenu(selectedArray.at(0)->getPosition());
            if(selectedArray.size() == 1 && selectedArray.at(0)->unitType == UNIT_WORKER){
                selectedArray.at(0)->stop();
            }
        }
    }
}
bool EditorWorld::onTouchBegan(Touch *touch, Event *unused_event){
    Vec2 location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    /*if(location.y > size.height*3/4){
     if(true){//location.x > size.width/2){
     int nextWeapon = GameManager::getInstance()->getNextToThisWeapon(playerWeaponType);
     if (nextWeapon >= WEAPON_NO_MORE) {
     nextWeapon = WEAPON_PISTOL;
     }
     setWeaponInfo(nextWeapon);
     return false;
     }
     return false;
     }*/
    /*
     if(GameManager::getInstance()->getEditorHud()->btnLeftRight->getBoundingBox().containsPoint(location) ||
     GameManager::getInstance()->getEditorHud()->btnA->getBoundingBox().containsPoint(location) ||
     GameManager::getInstance()->getEditorHud()->btnB->getBoundingBox().containsPoint(location)){
     return true;
     }*/
    
    return true;
}

void EditorWorld::onTouchMoved(Touch *touch, Event *unused_event){
    
}
void EditorWorld::onTouchEnded(Touch *touch, Event *unused_event){
    
}
void EditorWorld::TouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = touches.at(0);
    Vec2 location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    //    touchStart = location;
//    this->unschedule(schedule_selector(EditorWorld::resetTouchStart));
//    this->scheduleOnce(schedule_selector(EditorWorld::resetTouchStart), 0.5);
    
}
void EditorWorld::resetTouchStart(float dt){
    //    touchStart = Vec2(0, size.height);
}
void EditorWorld::setPlayer(Vec2 pos){
    
}
void EditorWorld::TouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = touches.at(0);
    Vec2 location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
    int min = 350;
    
    //    int toDown = touchStart.y - location.y;
    //    int toLeft = touchStart.x - location.x;
    //    if (toLeft< 0) {
    //        toLeft = -toLeft;
    //    }
}

void EditorWorld::getOffVehicle(){
    
}

void EditorWorld::TouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
    Touch *touch = (Touch*)(touches.at(0));
    Vec2 location = touch->getLocationInView();
    location = Director::getInstance()->convertToGL(location);
    
}

//void EditorWorld::TouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
//{
//    Touch *touch = (Touch*)(touches.at(0));
//    Vec2 location = touch->getLocationInView();
//    location = Director::getInstance()->convertToGL(location);

//}

void EditorWorld::solveCollision(Movable* p, Vec2 pos, cocos2d::Rect rect){
    
}
EnemyBase* EditorWorld::getEncounteredNPC(cocos2d::Vec2 pos){
    for(auto npc: npcArray){
        if(npc->getBoundingBox().containsPoint(pos)){
            return npc;
        }
    }
    return nullptr;
}
void EditorWorld::claimQuestReward(){
//    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//    std::string currentQuestNpcName = npcTable[Value(questID).asString()].asValueMap().at("npc name").asString();
//    int eIndex = npcTable[Value(questID).asString()].asValueMap().at("event index").asInt();
//    if(npcTable[Value(questID).asString()].asValueMap().at("repeat").asString().compare("repeat") != 0){
//        UDSetBool(strmake(KEY_QUEST_DONE_FORMAT, eIndex).c_str(), true);
//    }
//
//    for(int i = 0; i < 2; i++){
//        std::string rewardName = npcTable[Value(questID).asString()].asValueMap().at(strmake("reward %d", i)).asString();
//        if(rewardName.compare("exp") == 0){
//            int rewardCount = npcTable[Value(questID).asString()].asValueMap().at(strmake("reward %d count", i)).asInt();
//            addHeroExp(0, rewardCount);
//        }else if(rewardName.compare("coin") == 0){
//            int rewardCount = npcTable[Value(questID).asString()].asValueMap().at(strmake("reward %d count", i)).asInt();
//            GM->addCoin(rewardCount);
//        }
//    }
}
void EditorWorld::goThroughPortal(){
//    if(strStage.compare("purgatory") == 0){
//        UD->setStringForKey(KEY_LAST_SAVE_POINT, "0_4_2");
//        //GM->getEditorHud()->showBIAndReplaceScene();
//        UD->setIntegerForKey(KEY_NEXT_SPAWN_DIRECTION, DIRECTION_E);
//        return;
//    }
//    ValueVector rows = GameManager::getInstance()->split(strStage, "_");
//    int mapIndex = rows.at(0).asInt();
//    int x = rows.at(1).asInt();
//    int y = rows.at(2).asInt();
//    if (mapIndex == 3 && x == 0 && y == 4) {
//        UD->setStringForKey(KEY_LAST_SAVE_POINT, "purgatory");
//    }
//
    //GM->getEditorHud()->showBIAndReplaceScene();
}
void EditorWorld::showNPCEvent(int index){
    isInEvent = true;
    
}
void EditorWorld::stepBackDone(){
    
    
}
void EditorWorld::setQuest(){
//    if(currentQuestID == 8){
//        decoLayer->setTileGID(-1, Vec2(6, 3));
//        decoLayer->setTileGID(-1, Vec2(7, 3));
//        UDSetBool(strmake(KEY_QUEST_DONE_FORMAT, 8).c_str(), true);
//        return;
//    }
//    bool isQuest = npcTable[Value(currentQuestID).asString()].asValueMap().at("event type").asString().compare("quest") == 0;
//    if(isQuest){
//        for(int i = 0; i < 3; i++){
//            std::string itemName = npcTable[Value(currentQuestID).asString()].asValueMap().at(strmake("item %d", i)).asString();
//            if(getItemType(itemName) == ITEM_TYPE_ETC){
//
//            }else if(unitStatTable.find(itemName) != unitStatTable.end()){
//                UDSetInt(itemName.c_str(), 0);
//            }
//        }
//        UDSetBool(KEY_CURRENT_QUEST_DONE, false);
//        UDSetInt(KEY_CURRENT_QUEST_ID, currentQuestID);
//        EHUD->updateQuestInfo();
//    }else{
//
//    }
}
bool EditorWorld::isQuestConditionMet(){
//    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//    if(questID < 0){
//        return false;
//    }
//    std::string currentQuestNpcName = npcTable[Value(questID).asString()].asValueMap().at("npc name").asString();
//
//    for(int i = 0; i < 3; i++){
//        std::string itemName = npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d", i)).asString();
//        int itemCount = npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d count", i)).asInt();
//
//        if(getItemType(itemName) == ITEM_TYPE_ETC){
//            int itemHaveCount = EHUD->inventory->getTotalItemCount(itemName);
//            if(itemCount > itemHaveCount){
//                return false;
//            }
//        }else if(unitStatTable.find(itemName) != unitStatTable.end()){
//            int enemyKillCountForQuest = UDGetInt(itemName.c_str(), 0);
//            if(itemCount > enemyKillCountForQuest){
//                return false;
//            }
//        }
//    }
//
    return true;
}
void EditorWorld::onQuestComplete(){
//    int questID = UDGetInt(KEY_CURRENT_QUEST_ID, -1);
//    std::string currentQuestNpcName = npcTable[Value(questID).asString()].asValueMap().at("npc name").asString();
//    
//    for(int i = 0; i < 3; i++){
//        std::string itemName = npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d", i)).asString();
//        int itemCount = npcTable[Value(questID).asString()].asValueMap().at(strmake("item %d count", i)).asInt();
//        if(itemCount > 0){
//            EHUD->inventory->removeItem(itemName, itemCount);
//        }
//    }
//    
//    UDSetInt(KEY_CURRENT_QUEST_ID, -1);
}
void EditorWorld::showNPCDisposableTalk(std::string talk){
    EHUD->showSingleNPCMessage(talk);
    //    EHUD->showBlackTopAndBottom();
    //    isInEvent = true;
    //    showTalkText(talk, WHOSE_TALK_NPC);
    //    float delay = 3;
    //    imgTalkBox->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)), NULL));
    //    sptPointer->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(Node::removeFromParent, imgTalkBox)), NULL));
}
float EditorWorld::checkBottom(Movable* p){
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
        cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
        //            tileRect.origin.y = floorf(tileRect.origin.y);
        //            rect.origin.y = floorf(rect.origin.y);
        cocos2d::Rect inter = intersection(tileRect, rect);
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
                            //                                plPos = getCoordinateFromPosition(rect.origin + Vec2(0, -5), map);
                            tgid = getTileAtPosition(rect.origin + Vec2(0, -5), TAG_STAGE_LAYER, map);
                        }else{
                            //                                plPos = getCoordinateFromPosition(rect.origin + Vec2(rect.size.width, -5), map);
                            tgid = getTileAtPosition(rect.origin + Vec2(rect.size.width, -5), TAG_STAGE_LAYER, map);
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
        
        pos = pos - Vec2(TILE_SIZE/2, 0);
    }while(!escape);
    
    //    return ground;
    return temp;
}
void EditorWorld::checkForAndResolveCollisions(Movable* p){
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
    if(temp > 0){
        ground = true;
        tempTotal += temp;
    }
    // check top
    escape = false;
    int xOffset = 1;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Vec2(xOffset, rect.size.height);
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
                cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
                cocos2d::Rect inter = intersection(tileRect, rect);
                
                if(inter.size.width > 3 && inter.size.height > 0 && inter.size.width > inter.size.height){
                    
                    //                        if(p->velocity.y >= 0){
                    //                            //                            plPos = this->getCoordinateFromPosition(pos - Vec2(0, TILE_SIZE));
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
        
        pos = pos + Vec2(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Vec2(-xOffset, 1);
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
                cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                cocos2d::Rect inter = intersection(tileRect, rect);
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
        pos = pos + Vec2(0, TILE_SIZE/2);
    }while(!escape);
    
    // check right
    escape = false;
    rect = p->collisionBoundingBox();
    if(tempTotal > 0){
        rect.origin.y += tempTotal;
    }
    pos = rect.origin + Vec2(rect.size.width + xOffset, 1);
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
                cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);
                cocos2d::Rect inter = intersection(tileRect, rect);
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
        
        pos = pos + Vec2(0, TILE_SIZE/2);
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
//    p->updateWeaponPosition();
}
void EditorWorld::changeSprite(Ref* ref){
    Sprite* spt = (Sprite*)ref;
    spt->setSpriteFrame(spt->getName());
}
bool EditorWorld::isFregile(int index){
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
bool EditorWorld::isHighWay(int index){
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
bool EditorWorld::isWay(int index){
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
bool EditorWorld::isDecoBlock(int index){
    switch (index) {
        case 48:
        case 49:
            return true;
        default:
            return false;
    }
}
bool EditorWorld::isOneWay(int index){
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
bool EditorWorld::checkMovableMissileCollision(Movable* p){
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
    //    Vec2 current = p->getPosition();
    cocos2d::Rect pRect = p->collisionBoundingBox(); //2
    p->setPosition( p->desiredPosition);
    int gid;
    experimental::TMXTiledMap* map = getTileMap(p->getPosition());
    Vec2 plPos = this->getCoordinateFromPosition(p->getPosition(), map); //1
    cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
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
                cocos2d::Rect intersectionR = intersection(pRect, tileRect);
                if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                    p->setPosition(p->getPosition() + Vec2(0, intersectionR.size.height));
                }
            }
        }
        if(p->velocity.y > 0){
            p->velocity.y = -0.0021;
            cocos2d::Rect intersectionR = intersection(pRect, tileRect);
            if(intersectionR.size.width > 0 && intersectionR.size.height > 0){
                p->setPosition(p->getPosition() + Vec2(0, -intersectionR.size.height));
            }
        }
        
        
        if (p->velocity.x > 0) {
            gid = getTileAtPosition(p->getPosition() + Vec2(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
        }else if(p->velocity.x < 0){
            gid = getTileAtPosition(p->getPosition() - Vec2(p->getContentSize().width/2, 0), TAG_STAGE_LAYER, map);
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
int EditorWorld::getPlacedUnit(cocos2d::Vec2 coordinate){
    return placedArray[(int)coordinate.y][(int)coordinate.x];
}
void EditorWorld::eraseTile(cocos2d::Vec2 coordinate){
    int index = coordinate.x + coordinate.y*mapSizeWidth;
    Vec2 pos;
    for(auto unit: spriteBatch->getChildren()){
        if(unit->getTag() == index){
//        if(unit->getBoundingBox().containsPoint(getPositionFromTileCoordinate(coordinate.x, coordinate.y))){
//            pos = unit->getPosition();
            EnemyBase* enemy = (EnemyBase*)unit;
            if ((Movable*)unit == EHUD->selectedUnitForEditDetail) {
                EHUD->hideOptions();
            }
            if(enemy->isBuilding){
                for(int i = 0; i < enemy->buildingOccupySize.width; i++){
                    for(int j = 0;j < enemy->buildingOccupySize.height; j++){
                        decoLayer->setTileGID(0, Vec2(enemy->buildingStartCoordinate.x + i, enemy->buildingStartCoordinate.y + j));
                    }
                }
//                building->buildingStartCoordinate = getCoordinateFromPosition(pos, theMap);
//                building->buildingOccupySize = cocos2d::Size(width, height);
                updateMiniMapForNonMoving();
            }else if(enemy->unitType == UNIT_TREE){
                decoLayer->setTileGID(0, getCoordinateFromPosition(enemy->getPosition()));
            }
            
            mutualArray.eraseObject(enemy);
            readyHeroArray.eraseObject(enemy);
            heroArray.eraseObject(enemy);
            enemyArray.eraseObject(enemy);
            unit->removeFromParent();
            break;
        }
    }
//    int brush = placedArray[(int)coordinate.x][(int)coordinate.y];
    placedArray[(int)coordinate.y][(int)coordinate.x] = 0;
}
void EditorWorld::setMapData(std::string data){
    ValueVector numbers = GM->split(data, ",");
    char newCharArray[numbers.size()];
    for(int i = 0; i < numbers.size(); i++){
        newCharArray[i] = numbers.at(i).asInt();
        log("int %d", (int)newCharArray[i]);
    }
    std::string newCharString(newCharArray, numbers.size());
//    log("what about this %d: %s", (int)newCharString.length(), newCharString.c_str());
    std::string decompressed = GM->decompress_string(newCharString);
//    log("decompressed: %s", decompressed.c_str());
    
    ValueVector brushes = GM->split(decompressed, ",");
    for(int i = 0; i < brushes.size(); i++){
        int brush = brushes.at(i).asInt();
        brushTile(brush, Vec2(i%mapSizeWidth, i/mapSizeWidth));
    }
}
std::string EditorWorld::getMapData(){
    std::string mapData = "";
    for(int i = 0; i < mapSizeWidth; i++){
        for (int j = 0; j < mapSizeHeight; j++) {
            int unit = placedArray[j][i];
            mapData.append(std::to_string(unit));
            mapData.append(",");
            
//            if(unit == BRUSH_TREE){
//                mapData.append("T");
//            }else if(unit == 0){
//                mapData.append("E");
//            }else{
//
//            }
        }
    }
    mapData = mapData.substr(0, mapData.length() - 1);
    std::string compressed = GM->compress_string(mapData);
//    std::string decompressed = GM->decompressString(compressed);
    log("compressed %d: %s", (int)compressed.length(), compressed.c_str());
//    log("decompressed: %s", decompressed.c_str());
    
    compressed = GM->compress_string(mapData);
    const char* rawCharArray = compressed.c_str();
    int counter = 0;
    int newNumberedArray[(int)compressed.size()];
    while(counter < compressed.size()){
        newNumberedArray[counter] = rawCharArray[counter];
        counter++;
    }
    
    std::string finalData = "";
    for(int i = 0; i < counter; i++){
        finalData.append(std::to_string(newNumberedArray[i]));
        if(i < counter-1){
            finalData.append(",");
        }
    }
    
    return finalData;
    
    
//
//    log("compressed: %s [end]", compressed.c_str());
//    int length = (int)compressed.size();
//    int numberedArray[length];
//    for(int i =0; i < compressed.size(); i++){
//        int number = (int)compressed[i];
//        log("%c %d %c %d %s",compressed[i], number, number, (int)((char)number), std::to_string((char)number).c_str());
//        numberedArray[i] = number;
//    }
//    std::string newCompressed = "";
//    char newConstChar[length];
//    for(int i = 0; i < length; i++){
//        newCompressed.append(std::to_string((char)numberedArray[i]));
//        int num = numberedArray[i];
//        newConstChar[i] = (char)num;
//        newConstChar[i] = num;
//    }
//    newConstChar[length] = '\0';
//    log("decompressed: %s", decompressed.c_str());
//
//
////    log("newDecompressed: %s", GM->decompress_string(newCompressed).c_str());
//
//    std::string strFromCharArray(newConstChar);
////    std::string strFromNumberArray(numberedArray);
//    log("strFromCharArray: %d", (int)strFromCharArray.size());
//    log("byte decompressed: %s", GM->decompress_string(strFromCharArray).c_str());

//    return compressed;
}
bool EditorWorld::isBrushBuilding(int brush){
    if (brush == BRUSH_MINE ||
        brush == BRUSH_TREE ||
        brush == BRUSH_FARM ||
        brush == BRUSH_BARRACKS ||
        brush == BRUSH_LUMBERMILL ||
        brush == BRUSH_CASTLE ||
        brush == BRUSH_WATCHER_TOWER ||
        brush == BRUSH_FACTORY ||
        brush == BRUSH_AIRPORT ||
        brush == BRUSH_HQ ||
        brush == BRUSH_BUNKER ||
        brush == BRUSH_ORC_BARRACKS ||
        brush == BRUSH_BARBEQUE ||
        brush == BRUSH_TROLL_HOUSE ||
        brush == BRUSH_TEMPLE) {
        return true;
    }
    return false;
}
int EditorWorld::getUnitIndexForBrush(int brush){
    int unitIndex = -1;
    if (brush == BRUSH_ORC_AXE) {
        return UNIT_ORC_AXE;
    }else if (brush == BRUSH_BUNKER) {
        return UNIT_ORC_BUNKER;
    }else if (brush == BRUSH_HQ) {
        return UNIT_ORC_HQ;
    }else if (brush == BRUSH_AIRPORT) {
        return UNIT_AIRPORT;
    }else if (brush == BRUSH_FACTORY) {
        return UNIT_FACTORY;
    }else if (brush == BRUSH_WATCHER_TOWER) {
        return UNIT_WATCHERTOWER;
    }else if (brush == BRUSH_CASTLE) {
        return UNIT_CASTLE;
    }else if (brush == BRUSH_LUMBERMILL) {
        return UNIT_LUMBERMILL;
    }else if (brush == BRUSH_BARRACKS) {
        return UNIT_BARRACKS;
    }else if (brush == BRUSH_FARM) {
        return UNIT_FARM;
    }else if (brush == BRUSH_TREE) {
        return UNIT_TREE;
    }else if (brush == BRUSH_MINE) {
        return UNIT_MINE;
    }else if (brush == BRUSH_TROLL) {
        return UNIT_TROLL;
    }else if (brush == BRUSH_ARCHER) {
        return UNIT_ARCHER;
    }else if (brush == BRUSH_GOLBIN) {
        return UNIT_GOBLIN;
    }else if (brush == BRUSH_GOLBIN_BOMB) {
        return UNIT_GOBLIN_BOMB;
    }else if (brush == BRUSH_TEMPLE) {
        return UNIT_TEMPLE;
    }else if (brush == BRUSH_WORKER) {
        return UNIT_WORKER;
    }else if (brush == BRUSH_BARBEQUE) {
        return UNIT_BARBECUE;
    }else if (brush == BRUSH_CATAPULT) {
        return UNIT_CATAPULT;
    }else if (brush == BRUSH_ORC_SPEAR) {
        return UNIT_ORC_SPEAR;
    }else if (brush == BRUSH_SWORDSMAN) {
        return UNIT_SWORDMAN;
    }else if (brush == BRUSH_WIZARD) {
        return UNIT_WIZARD;
    }else if (brush == BRUSH_HELICOPTER) {
        return UNIT_HELICOPTER;
    }else if (brush == BRUSH_TROLL_HOUSE) {
        return UNIT_ORC_TROLL_HOUSE;
    }else if (brush == BRUSH_ORC_BARRACKS) {
        return UNIT_ORC_BARRACKS;
    }else if (brush == BRUSH_ZOMBIE_ORC_AXE) {
        return UNIT_ZOMBIE_ORC_AXE;
    }else if (brush == BRUSH_ZOMBIE_SWORDSMAN) {
        return UNIT_ZOMBIE_SWORDSMAN;
    }else if (brush == BRUSH_GOBLIN_WORKER) {
        return UNIT_GOBLIN_WORKER;
    }
    return -1;
}
EnemyBase* EditorWorld::brushTile(int brush, cocos2d::Vec2 coordinate){
    Vec2 pos = getPositionFromTileCoordinate(coordinate.x, coordinate.y);
    eraseTile(coordinate);
    placedArray[(int)coordinate.y][(int)coordinate.x] = brush;
    bool isBuilding = isBrushBuilding(brush);
    if (isBuilding) {
        Vec2 occupySize = GM->getBuildingOccupySize(getUnitIndexForBrush(brush));
        if (occupySize.x + coordinate.x >= mapSizeWidth ||
            occupySize.y + coordinate.y >= mapSizeHeight) {
            return nullptr;
        }
    }
    
    EnemyBase* unit = nullptr;
    if(brush == BRUSH_TREE ){
        createTree(pos);
    }else if(brush ==  BRUSH_MINE ){
        unit = createUnit(UNIT_MINE, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos, "mine", 1, "mine.png");
    }else{
        int unitIndex = getUnitIndexForBrush(brush);
        unit = createUnit(unitIndex, WHICH_SIDE_HERO, GM->isThisBuilding(unitIndex), pos, getUnitName(unitIndex), 1, getSpriteNameForUnit(unitIndex));
    }
//    else if(brush ==  BRUSH_MINE ){
//        unit = createUnit(UNIT_MINE, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos + Vec2(100, -100), "mine", 1, "mine.png");
//        setOccupy(pos, 3, 3, true, unit);
//    }else if(brush ==  BRUSH_SWORDSMAN ){
//        unit = createUnit(UNIT_SWORDMAN, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "swordman");
//    }else if(brush ==  BRUSH_ARCHER ){
//        unit = createUnit(UNIT_ARCHER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "archer");
//    }else if(brush ==  BRUSH_WORKER ){
//        unit = createUnit(UNIT_WORKER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "workerAxe");
//    }else if(brush ==  BRUSH_CATAPULT ){
//        unit = createUnit(UNIT_CATAPULT, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "catapult");
//    }else if(brush ==  BRUSH_HELICOPTER ){
//        unit = createUnit(UNIT_HELICOPTER, WHICH_SIDE_HERO, ITS_NOT_BUILDING, pos, "helicopter");
//    }else if(brush ==  BRUSH_CASTLE ){
//        unit = createUnit(UNIT_CASTLE, WHICH_SIDE_HERO, ITS_BUILDING, pos, "castle", 1, "castle.png");
//        setOccupy(pos, 4, 3, true, unit);
//    }else if(brush ==  BRUSH_FARM ){
//        unit = createUnit(UNIT_FARM, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -0), "farm", 1, "farm.png");
//        setOccupy(pos, 3, 2, true, unit);
//    }else if(brush == BRUSH_BARRACKS ){
//        unit = createUnit(UNIT_BARRACKS, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "barracks", 1, "barracks.png");
//        setOccupy(pos, 3, 3, true, unit);
//    }else if(brush == BRUSH_LUMBERMILL ){
//        unit = createUnit(UNIT_LUMBERMILL, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "lumberMill", 1, "lumberMill.png");
//        setOccupy(pos, 3, 3, true, unit);
//    }else if(brush == BRUSH_WATCHER_TOWER ){
//        unit = createUnit(UNIT_WATCHERTOWER, WHICH_SIDE_HERO, ITS_BUILDING, pos + Vec2(50, 0), "watcherTower", 1, "watcherTower.png");
//        setOccupy(pos, 2, 2, true, unit);
//    }else if(brush == BRUSH_FACTORY ){
//        unit = createUnit(UNIT_FACTORY, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "factory", 1, "factory.png");
//        setOccupy(pos, 3, 3, true, unit);
//    }else if(brush == BRUSH_AIRPORT ){
//        unit = createUnit(UNIT_AIRPORT, WHICH_SIDE_HERO, ITS_BUILDING, pos+ Vec2(100, -100), "airport", 1, "airport.png");
//        setOccupy(pos, 3, 3, true, unit);
//    }else if(brush == BRUSH_ORC_AXE ){
//        unit = createUnit(UNIT_ORC_AXE, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "orcAxe");
//    }else if(brush == BRUSH_ORC_SPEAR ){
//        unit = createUnit(UNIT_ORC_SPEAR, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "orcSpear");
//    }else if(brush == BRUSH_TROLL ){
//        unit = createUnit(UNIT_TROLL, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "troll");
//    }else if(brush == BRUSH_GOLBIN ){
//        unit = createUnit(UNIT_GOBLIN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblin");
//    }else if(brush == BRUSH_GOLBIN_BOMB ){
//        unit = createUnit(UNIT_GOBLIN_BOMB, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblinBomb");
//    }else if(brush == BRUSH_GOBLIN_WORKER ){
//        unit = createUnit(UNIT_GOBLIN_WORKER, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "goblinWorker");
//    }else if(brush == BRUSH_BUNKER ){
//        unit = createUnit(UNIT_ORC_BUNKER, WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Vec2(50, 0), "bunker", 1, "bunker.png");
//    }else if(brush == BRUSH_HQ ){
//        unit = createUnit(UNIT_ORC_HQ, WHICH_SIDE_ENEMY, ITS_BUILDING, pos + Vec2(150, -50), "hq", 1, "hq.png");
//    }else if(brush == BRUSH_BARBEQUE){
//        unit = createUnit(UNIT_BARBECUE, WHICH_SIDE_ENEMY, ITS_BUILDING, pos, "barbeque");
//    }else if(brush == BRUSH_ORC_BARRACKS){
//        unit = createUnit(UNIT_ORC_BARRACKS, WHICH_SIDE_ENEMY, ITS_BUILDING, pos, "orcBarracks");
//    }else if(brush == BRUSH_TROLL_HOUSE){
//        unit = createUnit(UNIT_ORC_TROLL_HOUSE, WHICH_SIDE_ENEMY, ITS_BUILDING, pos, "trollHouse");
//    }else if(brush == BRUSH_TEMPLE){
//        unit = createUnit(UNIT_TEMPLE, WHICH_SIDE_ENEMY, ITS_BUILDING, pos, "temple");
//    }else if(brush == BRUSH_WIZARD){
//        unit = createUnit(UNIT_WIZARD, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "wizard");
//    }else if(brush == BRUSH_ZOMBIE_SWORDSMAN ){
//        unit = createUnit(UNIT_ZOMBIE_SWORDSMAN, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "zombieSwordsman");
//    }else if(brush == BRUSH_ZOMBIE_ORC_AXE ){
//        unit = createUnit(UNIT_ZOMBIE_ORC_AXE, WHICH_SIDE_ENEMY, ITS_NOT_BUILDING, pos, "zombieOrcAxe");
//    }else if(brush == BRUSH_START_POINT ){
//        unit = createUnit(UNIT_START_POINT, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos, "");
//    }else if(brush == BRUSH_EVENT_POINT ){
//        unit = createUnit(UNIT_EVENT_POINT, WHICH_SIDE_MUTUAL, ITS_BUILDING, pos, "");
//    }else{
//        log("because its non");// start point should be added
//    }
    if (unit != nullptr ) {
        unit->setTag(coordinate.x + coordinate.y*mapSizeWidth);
//        if(unit->unitType != UNIT_TREE && !unit->isBuilding){
            unit->setSpriteFrame(getSpriteNameForUnit(unit->unitType));
//        }

        if (unit->isBuilding) {
            cocos2d::Size occupySize = GM->getBuildingOccupySize(unit->unitType);
            Vec2 coordinate = getCoordinateFromPosition(pos);
            setOccupy(getPositionFromTileCoordinate(coordinate.x, coordinate.y), occupySize.width, occupySize.height, true, unit);
            unit->setPosition(pos + Vec2(TILE_SIZE*(occupySize.width-1)/2, -TILE_SIZE*(occupySize.height-1)/2));
            unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        }
    }
    
    
    return unit;
}
bool EditorWorld::intersectsRect(cocos2d::Rect srcRect, cocos2d::Rect dstRect)
{
    return !(srcRect.getMaxX() <= dstRect.getMinX() ||
             dstRect.getMaxX() <= srcRect.getMinX() ||
             srcRect.getMaxY() <= dstRect.getMinY() ||
             dstRect.getMaxY() <= srcRect.getMinY());
}
cocos2d::Rect EditorWorld::intersection(cocos2d::Rect source, cocos2d::Rect rect)
{
    cocos2d::Rect intersection;
    intersection = cocos2d::Rect(source.getMinX()>rect.getMinX()?source.getMinX():rect.getMinX(),
                        source.getMinY()>rect.getMinY()?source.getMinY():rect.getMinY(),
                        0,
                        0);
    intersection.size.width = (source.getMaxX()<rect.getMaxX()?source.getMaxX():rect.getMaxX())-intersection.getMinX();
    intersection.size.height = (source.getMaxY()<rect.getMaxY()?source.getMaxY():rect.getMaxY())-intersection.getMinY();
    return intersection;
}
void EditorWorld::checkForAndResolveCollisionsForWater(Movable* p){
    
    Vec2 current = p->getGravityPosition();
    experimental::TMXTiledMap* map = getTileMap(current);
    cocos2d::Rect pRect = p->collisionBoundingBox();
    int gid;
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            current = current + Vec2(0, -TILE_SIZE);
        }else if(i == 1) {
            current = current + Vec2(0, TILE_SIZE);
        }else if(i == 2) {
            current = current + Vec2(-TILE_SIZE, 0);
        }else if(i == 3) {
            current = current + Vec2(TILE_SIZE, 0);
        }else if(i == 4) {
            current = current + Vec2(-TILE_SIZE, -TILE_SIZE);
        }else if(i == 5) {
            current = current + Vec2(TILE_SIZE, -TILE_SIZE);
        }else if(i == 6) {
            current = current + Vec2(-TILE_SIZE, TILE_SIZE);
        }else if(i == 7) {
            current = current + Vec2(TILE_SIZE, TILE_SIZE);
        }
        
        gid = getTileAtPosition(current, TAG_STAGE_LAYER, map);
        if (gid) {
            Vec2 plPos = this->getCoordinateFromPosition(current - map->getPosition(), map); //1
            cocos2d::Rect tileRect = tileRectFromTileCoords(plPos, map);//info->rect;
            
            cocos2d::Rect intersectionR = intersection(pRect, tileRect);
            if (intersectionR.size.width > 0 && intersectionR.size.height > 0){
                
                if (!isWay(gid)) {
                    //                    int tileIndx = tiles->indexOfObject(dic);
                    int tileIndx = i;
                    
                    if (tileIndx == 0 && p->velocity.y <= 0) {
                        //tile is directly below Hero
                        p->desiredPosition = Vec2(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                        p->velocity = Vec2(p->velocity.x, 0);
                        //                        p->setOnGround(true);
                        
                    } else if (tileIndx == 1) {
                        //tile is directly above Hero
                        p->desiredPosition = Vec2(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                        p->velocity = Vec2(p->velocity.x, 0.0);
                    } else if (tileIndx == 2) {
                        //tile is left of Hero
                        p->desiredPosition = Vec2(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                        
                    } else if (tileIndx == 3) {
                        //tile is right of Hero
                        p->desiredPosition = Vec2(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                    } else {
                        if (p->velocity.y > 0) {    // rasing
                            if (tileIndx > 5) {
                                p->desiredPosition = Vec2(p->desiredPosition.x, p->desiredPosition.y - intersectionR.size.height);
                                p->velocity = Vec2(p->velocity.x, 0.0);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Vec2(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Vec2(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
                            }
                        }else{  // falling
                            if (tileIndx > 5) {
                                p->desiredPosition = Vec2(p->desiredPosition.x, p->desiredPosition.y + intersectionR.size.height);
                                p->velocity = Vec2(p->velocity.x, 0);
                                //                                p->setOnGround(true);
                            }else if (tileIndx == 4){
                                p->desiredPosition = Vec2(p->desiredPosition.x + intersectionR.size.width, p->desiredPosition.y);
                            }else if (tileIndx == 5){
                                p->desiredPosition = Vec2(p->desiredPosition.x - intersectionR.size.width, p->desiredPosition.y);
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

void EditorWorld::checkForAndResolveCollisionsForBouncing(Movable* p){
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
            if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    cocos2d::Rect inter = intersection(tileRect, rect);
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
        pos = pos + Vec2(TILE_SIZE/2, 0);
    }while(!escape);
    
    // check left
    escape = false;
    rect = p->collisionBoundingBox();
    pos = rect.origin + Vec2(0, rect.size.height);
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
                if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                    cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    cocos2d::Rect inter = intersection(tileRect, rect);
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
        pos = pos - Vec2(0, TILE_SIZE/2);
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
            if (property.getType() != Value::Type::NONE && keyExist(property.asValueMap(), "Type")) {
                if(property.asValueMap().at("Type").asString().compare("OneWay") != 0){
                    cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
                    cocos2d::Rect inter = intersection(tileRect, rect);
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
        
        pos = pos - Vec2(0, TILE_SIZE/2);
    }while(!escape);
    
    // check bottom
    rect = p->collisionBoundingBox();
    int offset = 2;
    pos = rect.origin + Vec2(rect.size.width - offset, 0);
    do{
        if(pos.x <= rect.origin.x + offset){
            pos.x = rect.origin.x;
            escape = true;
        }
        map = getTileMap(pos);
        plPos = this->getCoordinateFromPosition(pos - map->getPosition(), map);
        int tgid = ((experimental::TMXLayer*)map->getChildByTag(TAG_STAGE_LAYER))->getTileGIDAt(plPos);
        if(tgid > 0){
            cocos2d::Rect tileRect = this->tileRectFromTileCoords(plPos, map);
            cocos2d::Rect inter = intersection(tileRect, rect);
            
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
                        p->velocity = Vec2(p->velocity.x*0.8, -p->velocity.y*0.6);
                        
                        
                        break;
                    }
                }
            }
            
        }
        pos = pos - Vec2(TILE_SIZE/2, 0);
    }while(!escape);
    
    p->setPosition( p->desiredPosition);
}

bool EditorWorld::checkForAndResolveCollisionsForMissile(Movable* p){
    p->setPosition( p->desiredPosition); //7
    
    experimental::TMXTiledMap* map = getTileMap(p->getPosition());
    int gid = 0;
    if (map != nullptr) {
        gid = getTileAtPosition(p->getPosition(), TAG_STAGE_LAYER, map);
    }
    
    if(map == nullptr || !isWay(gid)){
        /*if (p->velocity.x > 0){
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL,
         p->getPosition());//Vec2(intersectionR.origin.x, intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
         }else{
         runEffect(EFFECT_EXPLODE_SMALL,//EFFECT_BULLET_AGAINST_WALL_LEFT,
         p->getPosition());//Vec2(intersectionR.origin.x + intersectionR.size.width,intersectionR.origin.y + intersectionR.size.height/2));//p->getPosition());
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
        return true;
    }
    
    return false;
}
void EditorWorld::checkForDoor()
{
    
}
void EditorWorld::unscheduleAll(){
    this->unscheduleAllCallbacks();
}

void EditorWorld::showSuccessLayer(float dt)
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
    
    //GameManager::getInstance()->getEditorHud()->setVisible(true);
    activityIndicator = Sprite::create("sandglass.png");
    //GameManager::getInstance()->getEditorHud()->addChild(activityIndicator);
    activityIndicator->setPosition(Vec2(size.width/2, size.height/2));
    activityIndicator->runAction(RepeatForever::create(RotateBy::create(1, 360)));
    GameManager::getInstance()->saveCoin();
    if (starCount >= 3) {
        showStageClearLayer(true);
    }else{
        showStageClearLayer(false);
    }
}

void EditorWorld::showStageClearLayer(bool threeStars){
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
void EditorWorld::rateResult(Node* node){
    
}
void EditorWorld::onUpdateResultErrorComplete(char const *status, char const *error) {
    CCLOG("Update result failed. Status: %s, Error: %s", status, error);
    activityIndicator->setVisible(false);
    activityIndicator->stopAllActions();
    activityIndicator->removeFromParentAndCleanup(true);
    
}

void EditorWorld::gameClearAnimationDone(Ref* obj)
{
    //    CCLOG("Show stage clear popup");
}

Movable* EditorWorld::findTarget(int distance){
    Movable* targetEnemy = NULL;
    
    float minDistance = distance;
    float x = 0;
    float y = 0;
    Node* finder;
    for(auto drop: enemyArray){
        if (drop->untouchable) {
            continue;
        }
        
        x = finder->getPositionX() - drop->getPositionX();
        y = finder->getPositionY() - drop->getPositionY();
        if (x < 0) x = -x;
        if (y < 0) y = -y;
        
        if (x + y < minDistance) {
            minDistance = x + y;
            targetEnemy = drop;
        }
    }
    return targetEnemy;
}
int EditorWorld::getMaxLoadedBulletCount(int weapon){
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
int EditorWorld::getMaxTotalBulletCount(int weapon){
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
void EditorWorld::playReloadingSoundLater(float dt){
    
}
//int testWeaponIndex = WEAPON_LASER_GUN - 1;
void EditorWorld::reload(){
    
}
void EditorWorld::fire()
{
    
}
void EditorWorld::missileEffectUpdate(float dt){
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
void EditorWorld::missileMoveDone(Ref* obj)
{
    EnemyBase* missile = (EnemyBase*)obj;
    heroMissileArray.eraseObject(missile);
    enemyMissileArray.eraseObject(missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void EditorWorld::fireBulletMoveDone(Ref* obj)
{
    Movable* missile = (Movable*)obj;
    heroMissileArray.eraseObject(missile);
    customMovingArray.eraseObject(missile);
    missile->stopAllActions();
    missile->removeFromParentAndCleanup(true);
}
void EditorWorld::enemyBaseMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    EnemyBase* sprite = (EnemyBase*)obj;
    sprite->stopAllActions();
    enemyArray.eraseObject(sprite);
    sprite->removeFromParentAndCleanup(true);
}
void EditorWorld::coinWaitDone(Ref* obj){
    Movable* drop = (Movable*)obj;
    coinArray.eraseObject(drop);
    MovableCoinArray.eraseObject(drop);
    drop->runAction(Sequence::create(FadeOut::create(1), CallFuncN::create(CC_CALLBACK_1(EditorWorld::removeFromParentAndCleanup, drop)), NULL));
}
void EditorWorld::spriteMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Sprite* sprite = (Sprite*)obj;
    sprite->stopAllActions();
    sprite->removeFromParentAndCleanup(true);
}
void EditorWorld::nodeMoveDone(Ref* obj)
{
    if (obj == NULL) {
        return;
    }
    Label* lbl = (Label*)obj;
    
    lbl->stopAllActions();
    lbl->removeFromParentAndCleanup(true);
}


void EditorWorld::blinkingPlayer(float dt)
{
    blinkingTime -= dt;
    
}

void EditorWorld::pauseLayer()
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
    
    isPaused = true;
    //    this->pause();
}

void EditorWorld::resumeLayer()
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
    
    isPaused = false;
    //    this->resume();
}

void EditorWorld::refreshPlayerInfo(int costum, int weapon)
{
    
}


void EditorWorld::receivingData(float dt){
    
}
void EditorWorld::removeCharacter(EnemyBase* unit){
    if(unit->sptWeapon != nullptr){
        unit->sptWeapon->removeFromParent();
    }
    MovableArray.eraseObject(unit);
    heroArray.eraseObject(unit);
    enemyArray.eraseObject(unit);
    unit->removeFromParent();
}
void EditorWorld::showTalkText(std::string text, int whoseTalk, std::string npcName){
    talkText = text;
    talkIndex = 0;
    if(imgTalkBox != nullptr){
        imgTalkBox->removeFromParent();
        sptPointer->removeFromParent();
    }
    imgTalkBox = ImageView::create("talkBoxFrame.png", ImageView::TextureResType::PLIST);
    imgTalkBox->setAnchorPoint(Vec2(0.5, 0));
    float splitPadding = 10;
    imgTalkBox->setCapInsets(cocos2d::Rect(splitPadding, splitPadding, imgTalkBox->getContentSize().width - splitPadding*2, imgTalkBox->getContentSize().height - splitPadding*2));
    imgTalkBox->setScale9Enabled(true);
    imgTalkBox->setContentSize(cocos2d::Size(100, 10));
    this->addChild(imgTalkBox, 100);
    
    sptPointer = Sprite::createWithSpriteFrameName("talkBoxPointer.png");
    this->addChild(sptPointer, 100);
    lblTalk = LanguageManager::getInstance()->getLocalizedLabel();
    lblTalk->setTextColor(Color4B::WHITE);//(78, 78, 78, 255));
    EnemyBase* finder;
    talkingNpcName = npcName;
    //    talkingNPC = nullptr;
    if(whoseTalk == WHOSE_TALK_HERO){
        imgTalkBox->setPosition(hero->getPosition() + Vec2(0, 100));
        talkingNPC = finder;
    }else if(whoseTalk == WHOSE_TALK_NPC){
        Vec2 npcPos;
        if(npcName.size() > 0){
            for(auto npc: npcArray){
                if(npc->getName().compare(npcName) == 0){
                    npcPos = Vec2(npc->getPositionX(), npc->getBoundingBox().getMaxY());
                    talkingNPC = npc;
                    break;
                }
            }
        }else{
            npcPos = Vec2(encounteredNPC->getPositionX(), encounteredNPC->getBoundingBox().getMaxY());
        }
        imgTalkBox->setPosition(npcPos + Vec2(0, 5));
        //        imgTalkBox->setColor(Color3B(188, 188, 188));
        //        sptPointer->setColor(Color3B(188, 188, 188));
        lblTalk->setTextColor(Color4B::BLACK);
    }
    
    
    sptPointer->setAnchorPoint(Vec2(0.5, 1));
    sptPointer->setPosition(imgTalkBox->getPosition() + Vec2(0, 18));
    
    
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
    }
    // resize talkbox
    lblTalkShadow->setString(talkText);
    cocos2d::Size talkSize = cocos2d::Size(lblTalkShadow->getContentSize().width*lblTalk->getScale(), lblTalkShadow->getContentSize().height*lblTalk->getScale());
    float width = 600;
    if(talkSize.width > width){
        lblTalkShadow->setWidth(width/lblTalkShadow->getScale());
    }else{
        lblTalkShadow->setWidth(talkSize.width/lblTalkShadow->getScale());
    }
    lblTalk->setWidth(lblTalkShadow->getWidth());
    talkSize = cocos2d::Size(lblTalkShadow->getContentSize().width*lblTalk->getScale(), lblTalkShadow->getContentSize().height*lblTalk->getScale());
    float padding = 30;
    int talkBoxWidth = talkSize.width + padding;
    int minTalkBoxWidth = 300;
    if(talkBoxWidth < minTalkBoxWidth){
        talkBoxWidth = minTalkBoxWidth;
    }
    imgTalkBox->setContentSize(cocos2d::Size(talkBoxWidth, talkSize.height + padding));
    lblTalk->setPosition(Vec2(imgTalkBox->getContentSize().width/2, padding/2 + talkSize.height/2));
    lblTalkShadow->setPosition(Vec2(imgTalkBox->getContentSize().width/2, padding/2 + talkSize.height/2));
    // resize talkbox end
    
    imgTalkBox->addChild(lblTalk);
    
    this->schedule(schedule_selector(EditorWorld::talkBoxUpdate));
    
    if(EHUD->eventIndex == 0){
        if(EHUD->talkIndex == 15){
            for(auto unit:npcArray){
                if(unit->getName().compare("undead") == 0){
                    float moveDistance = 3.0f;
                    unit->runAction(Sequence::create(MoveBy::create(5, Vec2(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)), CallFunc::create(CC_CALLBACK_0(EditorWorld::onUndeadMoveDone, this)), nullptr));
                    imgTalkBox->runAction(MoveBy::create(5, Vec2(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)));
                    sptPointer->runAction(MoveBy::create(5, Vec2(-TILE_SIZE*moveDistance, TILE_SIZE*moveDistance)));
                    unit->runAnimation("lamingtonRun", true);
                }
            }
        }else if( EHUD->talkIndex >= 16 && EHUD->talkIndex <= 21){ // create portal for john event
            int counter = 0;
            log("talkIndex: %d", EHUD->talkIndex);
            for(auto unit:unitsToCreateArray){
                if (counter%6 == EHUD->talkIndex - 16 && npcArray.find(unit) == npcArray.end()) {
                    unit->stopAllActions();
                    unit->runAnimation(unit->isEnemy?"zombieOrcAxeStand":"zombieSwordsmanStand", true);
                }
                counter++;
            }
        }else if(EHUD->talkIndex == 28){
            for(auto unit:npcArray){
                if(unit->getName().compare("hero") == 0){
                    unit->setFlippedX(true);
                }
            }
        }else if(EHUD->talkIndex == 29){
            for(auto unit:npcArray){
                if(unit->getName().compare("orc") == 0){
                    unit->setFlippedX(false);
                }
            }
        }else if(EHUD->talkIndex == 30){
            for(auto unit:npcArray){
                if(unit->getName().compare("hero") == 0){
                    unit->setFlippedX(false);
                }
                if(unit->getName().compare("orc") == 0){
                    unit->setFlippedX(true);
                }
            }
        }
    }
}
void EditorWorld::onUndeadMoveDone(){
    for(auto unit:npcArray){
        if(unit->getName().compare("undead") == 0){
            unit->runAnimation("lamingtonStand", true);
        }
    }
}
EnemyBase* EditorWorld::getNPC(std::string name){
    for(auto npc: npcArray){
        if(npc->getName().compare(name) == 0){
            return npc;
        }
    }
    return nullptr;
}
void EditorWorld::changeToSuperAdin(){
    this->unschedule(schedule_selector(EditorWorld::creatingStarToGreatBall));
    greatBall->removeFromParent();
    addSuperEffectToAdin();
}
void EditorWorld::addSuperEffectToAdin(){
    
}
void EditorWorld::createPortal(Vec2 pos){
    EnemyBase* portal = addNPC(pos, "portal");
    thePortal = portal;
    portal->setScale(0.1f);;
    portal->runAction(ScaleTo::create(1, 1));
    portal->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    portal->runAction(RepeatForever::create(RotateBy::create(1, 90)));
    portal->setLocalZOrder(-pos.y);
    //    portal->setOpacity(150);
    //    portal->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(ScaleTo::create(1, 1, 0.5f), 2), EaseInOut::create(ScaleTo::create(1, 0.5f, 1 ), 2), NULL)));
    
    Sprite* spt = Sprite::createWithSpriteFrameName("portal0.png");
    GM->runAnimation(spt, "portal", true);
    spt->setScale(0.1f);
    portal->runAction(ScaleTo::create(1, 0.8f));
    spt->setPosition(pos);
    spriteBatch->addChild(spt, 101);
    spt->runAction(RepeatForever::create(RotateBy::create(1, -180)));
    spt->setFlippedX(true);
    spt->setOpacity(100);
    suckingObject = spt;
    //    portal->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(ScaleTo::create(1, -1, -0.5f), 2), EaseInOut::create(ScaleTo::create(1, -0.5f, -1 ), 2), NULL)));
    this->schedule(schedule_selector(EditorWorld::sucking), 0.1f);
}
void EditorWorld::sucking(float dt){
    if(suckingObject == nullptr){
        return;
    }
    for(int i = 0; i < 3; i++){
        Sprite* spt = Sprite::createWithSpriteFrameName("ball.png");
        spt->setColor(rand()%2==0?Color3B::BLACK:Color3B::GRAY);
        spt->setOpacity(0);
        spt->runAction(Sequence::create(DelayTime::create(0.2f), FadeTo::create(0.2f, 50 + rand()%100), DelayTime::create(0.4f), FadeOut::create(0.4f), nullptr));
        spt->setScale(0.4f);
        spt->runAction(ScaleTo::create(1, 0.05f));
        spriteBatch->addChild(spt, 150);
        int distance = 20;
        spt->setPosition(suckingObject->getPosition() + Vec2(cos(i*(rand()%360)*3.14f/180)*distance, sin(i*(rand()%360)*3.14f/180)*distance));
        spt->runAction(Sequence::create(EaseIn::create(MoveTo::create(1, suckingObject->getPosition()), 3), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt)), NULL));
    }
}
void EditorWorld::talkBoxUpdate(float dt){
    for(auto npc: npcArray){
        if(npc->getName().compare(talkingNpcName) == 0 && EHUD->talkIndex < 22){
            Vec2 npcPos = Vec2(npc->getPositionX(), npc->getBoundingBox().getMaxY());
            imgTalkBox->setPosition(npcPos + Vec2(0, 5));
            sptPointer->setPosition(imgTalkBox->getPosition() + Vec2(0, 18));
            break;
        }
    }
    if(talkIndex >= talkText.size()){
        if(imgTalkBox->getChildByName("TOUCH") == nullptr){
            //            Sprite* sptAPress = Sprite::createWithSpriteFrameName("aPressed.png");
            //            imgTalkBox->addChild(sptAPress);
            //            sptAPress->setName("aPress");
            //            GameManager::getInstance()->runAnimation(sptAPress, "aPressAni", true);
            //            sptAPress->setPosition(Vec2(imgTalkBox->getContentSize().width - 5, -5));
            
            Label* lbl = LM->getLocalizedLabel("Next", Color4B::WHITE);
            lbl->enableShadow();
            imgTalkBox->addChild(lbl);
            lbl->setName("TOUCH");
            lbl->setVisible(false);
            lbl->setColor(Color3B::WHITE);
            lbl->setPosition(Vec2(imgTalkBox->getContentSize().width - lbl->getContentSize().width/2, -lbl->getContentSize().height/2));
            lbl->runAction(RepeatForever::create(Blink::create(1, 1)));
            lblTalk->setString(talkText);
            return;
        }
    }
    lblTalk->setString(talkText.substr(0, talkIndex));
    talkIndex++;
}
void EditorWorld::endEvent(){
    isCameraInCustomMoving = false;
    extraCameraPos = Vec2(0, 0);
    this->unschedule(schedule_selector(EditorWorld::talkBoxUpdate));
    if(imgTalkBox != nullptr){
        imgTalkBox->removeFromParent();
        imgTalkBox = nullptr;
        sptPointer->removeFromParent();
    }
    
    isInEvent = false;
    //    updateNPCQuestState(encounteredNPC);
    this->unschedule(schedule_selector(EditorWorld::eventUpdate));
    if(EHUD->isThisEventQuest){
        
    }
}
int EditorWorld::getHeroLevel(int slot){
    return UD->getIntegerForKey(strmake(KEY_HERO_LEVEL_FORMAT, slot).c_str(), 0);
}
void EditorWorld::setHeroLevel(int slot, int level){
    UD->setIntegerForKey(strmake(KEY_HERO_LEVEL_FORMAT, slot).c_str(), level);
}
int EditorWorld::getHeroExp(int slot){
    return UD->getIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), 0);
}
void EditorWorld::setHeroExp(int slot, int exp){
    UD->setIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), exp);
}
void EditorWorld::addHeroExp(int slot, int exp){
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
            showLabelFromPool(WORLD, this->getPosition() + Vec2(0, 10), "LEVEL UP", 15, 0.2f + i*1);
        }
        setHeroExp(0, 0);
        hero->maxEnergy = getHeroMaxHP(0);
        hero->energy = hero->maxEnergy;
        EHUD->setEnergy(hero->energy, hero->maxEnergy);
    }
    
    EHUD->setExp(getHeroLevel(slot), getHeroExp(slot), getMaxExp(getHeroLevel(slot)));
    UD->setIntegerForKey(strmake(KEY_HERO_EXP_FORMAT, slot).c_str(), currentExp);
}
int EditorWorld::getMaxExp(int level){
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
int EditorWorld::getHeroMaxLevel(int slot){
    std::string tier = UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0");
    std::string maxLevel = getEvolutionStat(tier, "max level");
    return Value(maxLevel).asInt();
}
std::string EditorWorld::getHeroName(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_NAME_FORMAT, slot).c_str(), "");
}
int EditorWorld::getHeroMaxHP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
    //    return getUnitMaxHP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getHelmetStat(getHelmet(slot), "hp");
    return 60;
}
int EditorWorld::getHeroAP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
    //    return getUnitAP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getWeaponStat(getWeapon(slot), "ap") + getHelmetStat(getHelmet(slot), "ap");
    return 10;
}
int EditorWorld::getHeroSP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
    //    return getUnitSP(getHeroName(slot))*(1 + 0.1f*tier) + getShoesStat(getShoes(slot), "sp");
    return 25;
}
int EditorWorld::getHeroDP(int slot){
    int tier = Value(UDGetStr(strmake("hero_tier_%d", slot).c_str(), "0")).asInt();
    //    return getUnitDP(getHeroName(slot), getHeroLevel(slot))*(1 + 0.1f*tier) + getHelmetStat(getHelmet(slot), "dp") + getShieldStat(getShield(slot), "dp");
    return 0;
}
int EditorWorld::getHeroGP(int slot){
    return getShieldStat(getShield(0), "gp");
}
void EditorWorld::updatePlayerStat(){
    
}
std::string EditorWorld::getHeroWeapon(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, slot).c_str(), "sword");
}
void EditorWorld::setHeroWeapon(int slot, std::string weapon){
    UD->setStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, slot).c_str(), weapon);
}
std::string EditorWorld::getHeroHelmet(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, slot).c_str(), "");
}
void EditorWorld::setHeroHelmet(int slot, std::string helmet){
    UD->setStringForKey(strmake(KEY_HERO_HELMET_FORMAT, slot).c_str(), helmet);
}
std::string EditorWorld::getHeroShield(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, slot).c_str(), "");
}
void EditorWorld::setHeroShield(int slot, std::string shield){
    UD->setStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, slot).c_str(), shield);
}
std::string EditorWorld::getHeroShoes(int slot){
    return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, slot).c_str(), "");
}
void EditorWorld::setHeroShoes(int slot, std::string shoes){
    UD->setStringForKey(strmake(KEY_HERO_SHOES_FORMAT, slot).c_str(), shoes);
}
int EditorWorld::getUnitStat(std::string unitName, std::string stat){
    if(unitStatTable.find(unitName) == unitStatTable.end()){
        return -1;
    }
    ValueMap row = unitStatTable[unitName].asValueMap();
    return Value(row.at(stat)).asInt();
}
std::string EditorWorld::getUnitDropItem(std::string unitName, int dropIndex){
    if(unitStatTable.find(unitName) == unitStatTable.end()){
        return "";
    }
    ValueMap row = unitStatTable[unitName].asValueMap();
    return Value(row.at(strmake("drop %d", dropIndex))).asString();
}
int EditorWorld::getWeaponStat(std::string name, std::string stat){
    if(name.size() == 0 || weaponStatTable[name].asValueMap().find(stat) == weaponStatTable[name].asValueMap().end()) return 0;
    return Value(weaponStatTable[name].asValueMap().at(stat)).asInt();
}
std::string EditorWorld::getItemStat(int itemType, std::string name, std::string stat){
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
std::string EditorWorld::getEvolutionStat(std::string name, std::string stat){
    ValueMap valueMap = evolutionTable[name].asValueMap();
    if(valueMap.find(stat) == valueMap.end()){
        return "";
    }else{
        return Value(valueMap.at(stat)).asString();
    }
}
std::string EditorWorld::getItemStat(std::string name, std::string stat){
    int itemType = getItemType(name);
    return getItemStat(itemType, name, stat);
    //    if(name.size() == 0) return 0;
    //    return Value(itemStatTable[name].asValueMap().at(stat)).asInt();
}
int EditorWorld::getItemPrice(int itemType, std::string name){
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
std::string EditorWorld::getItemSpriteName(int itemType, std::string name){
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
int EditorWorld::getShieldStat(std::string name, std::string stat){
    if(name.size() == 0 || shieldStatTable[name].asValueMap().find(stat) == shieldStatTable[name].asValueMap().end()) return 0;
    return Value(shieldStatTable[name].asValueMap().at(stat)).asInt();
}
int EditorWorld::getShoesStat(std::string name, std::string stat){
    if(name.size() == 0 || shoesStatTable[name].asValueMap().find(stat) == shoesStatTable[name].asValueMap().end()) return 0;
    return Value(shoesStatTable[name].asValueMap().at(stat)).asInt();
}
int EditorWorld::getHelmetStat(std::string name, std::string stat){
    if(name.size() == 0 || helmetStatTable[name].asValueMap().find(stat) == helmetStatTable[name].asValueMap().end()) return 0;
    return Value(helmetStatTable[name].asValueMap().at(stat)).asInt();
}
std::string EditorWorld::getHelmet(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_HELMET_FORMAT, heroSlot).c_str());
}
std::string EditorWorld::getWeapon(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_WEAPON_FORMAT, heroSlot).c_str());
}
std::string EditorWorld::getShield(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_SHIELD_FORMAT, heroSlot).c_str());
}
std::string EditorWorld::getShoes(int heroSlot){
    return UD->getStringForKey(strmake(KEY_HERO_SHOES_FORMAT, heroSlot).c_str());
}
void EditorWorld::movePlayer(int direction){
    
}
void EditorWorld::addGold(int amount){
    if (amount > 0) {
        totalEarnedGold += amount;
    }else{
        totalUsedGold -= amount;
    }
    gold += amount;
    EHUD->lblGold->setString(strmake("%d", gold));
}
void EditorWorld::addLumber(int amount){
    if (amount > 0) {
        totalEarnedLumber += amount;
    }else{
        totalUsedLumber -= amount;
    }
    lumber += amount;
    EHUD->lblLumber->setString(strmake("%d", lumber));
}
void EditorWorld::updateFoodInUse(){
    int foodUse = 0;
    for(auto unit: heroArray){
        if(!unit->isAlli){
            foodUse += getFoodUse(unit->unitType);
        }
    }
    foodInUse = foodUse;
    EHUD->lblFood->setString(strmake("%d/%d", foodInUse, foodMax));
}void EditorWorld::addFoodMax(int amount){
    foodMax += amount;
    PPLabel* lbl = EHUD->lblFood;
    std::string str = strmake("%d/%d", foodInUse, foodMax);
    lbl->setString(str);
}
int EditorWorld::getGoldPriceForUnit(int index){
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
    }else if (index == UNIT_SWORDMAN) {
        return 300;
    }else if (index == UNIT_ARCHER) {
        return 200;
    }else if (index == UNIT_CATAPULT) {
        return 500;
    }else if (index == UNIT_HELICOPTER) {
        return 100;
    }
    
    return 0;
}
int EditorWorld::getLumberPriceForUnit(int index){
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
    }else if (index == UNIT_CATAPULT) {
        return 400;
    }else if (index == UNIT_HELICOPTER) {
        return 200;
    }
    return 0;
}
int EditorWorld::getFoodUse(int index){
    if (index == UNIT_WORKER) {
        return 1;
    }else if (index == UNIT_HELICOPTER) {
        return 2;
    }else if (index == UNIT_SWORDMAN) {
        return 1;
    }else if (index == UNIT_ARCHER) {
        return 1;
    }else if (index == UNIT_CATAPULT) {
        return 4;
    }
    return 0;
}
int EditorWorld::getFoodGive(int index){
    if (index == UNIT_CASTLE) {
        return 6;
    }else if (index == UNIT_FARM) {
        return 7;
    }
    return 0;
}
EnemyBase* EditorWorld::getNearestCastle(cocos2d::Vec2 pos){
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
EnemyBase* EditorWorld::getNearestLumberTank(cocos2d::Vec2 pos){
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
EnemyBase* EditorWorld::getNearestTree(cocos2d::Vec2 pos){
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
void EditorWorld::setClearCondition(int stage){
    int offsetX = 0;
    if(size.height/size.width < 700.0f/1334.0f){
        offsetX = 100;
    }
    Sprite* sptMission = Sprite::create("mission.png");
    EHUD->addChild(sptMission);
    sptMission->setPosition(Vec2(offsetX + 50 + sptMission->getContentSize().width/2, size.height - 66));
    int condition = GM->getStageObjective(stage);
    if(condition == CLEAR_CONDITION_BARRACKS_TWO_FARMS_FOUR_SWORDMAND){
        clearConditionIndex = GM->getStageObjective(stage);
        for (int i = 0; i < 3; i++) {
            //            Label* lbl = LM->getLocalizedLabel();
            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, false, false, TextHAlignment::LEFT, true);
            EHUD->addChild(lbl);
            //            lbl->setAnchorPoint(Vec2(0, 0.5));
            lbl->setPosition(Vec2(offsetX + 50, size.height - 166 - 80*i));
            lblConditionArray.pushBack(lbl);
            //            lbl->enableShadow();
        }
    }else if(condition == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE){
        clearConditionIndex = GM->getStageObjective(stage);
        for (int i = 0; i < 2; i++) {
            PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE,false,  false, TextHAlignment::LEFT, true);
            EHUD->addChild(lbl);
            lbl->setPosition(Vec2(offsetX + 50, size.height - 166 - 80*i));
            lblConditionArray.pushBack(lbl);
        }
    }else{
        clearConditionIndex = GM->getStageObjective(stage);
        PPLabel* lbl = PPLabel::create("sdf", 40, Color3B::WHITE, false, false, TextHAlignment::LEFT, true);
        EHUD->addChild(lbl);
        lbl->setPosition(Vec2(offsetX + 50, size.height - 166 - 80*0));
        lblConditionArray.pushBack(lbl);
    }
    checkClearGame();
}
bool EditorWorld::checkClearGame(){
    bool isAllClear = false;
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
                lblConditionArray.at(i)->setString(strmake("%s(%d/2)", LM->getText("objective build farm").c_str(), farmCount));
                if(farmCount < 2){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }else if(i == 1){
                lblConditionArray.at(i)->setString(strmake("%s(%d/1)", LM->getText("objective build barracks").c_str(), barracksCount));
                if(barracksCount < 1){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }else if(i == 2){
                lblConditionArray.at(i)->setString(strmake("%s(%d/4)", LM->getText("objective train swordsman").c_str(), swordmanCount));
                if(swordmanCount < 4){
                    isAllClear = false;
                }else{
                    complete = true;
                }
            }
            if(complete){
                if(lblConditionArray.at(i)->getColor() != Color3B(245, 213, 71)){
                    lblConditionArray.at(i)->setFontColor(Color3B(245, 213, 71));
                    Sprite* spt = Sprite::create("check.png");
                    EHUD->addChild(spt);
                    spt->setPosition(Vec2(lblConditionArray.at(i)->getPositionX() + lblConditionArray.at(i)->getSize().width + 50, lblConditionArray.at(i)->getPositionY()));
                }
            }
        }
    }else if(clearConditionIndex == CLEAR_CONDITION_TERMINATE){
        isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
        lblConditionArray.at(0)->setString(LM->getText("objective destroy them all"));
    }else if(clearConditionIndex == CLEAR_CONDITION_PROTECT_ALLI_AND_TERMINATE){
        isAllClear = enemyArray.size() == 0 && unitsToCreateArray.size() == 0;
        lblConditionArray.at(0)->setString(LM->getText("objective protect all"));
        lblConditionArray.at(1)->setString(LM->getText("objective destroy them all"));
    }else if(clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_10_MIN || clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN){
        
        int min = 10;
        if(clearConditionIndex == CLEAR_CONDITION_SURVIVE_FOR_20_MIN){
            min = 20;
        }
        isAllClear = gameTimer > min*60;
        lblConditionArray.at(0)->setString(LM->getText(strmake(LM->getText("objective survive").c_str(), min).c_str()));
    }
    isGameClear = isAllClear;
    return isAllClear;
}
int EditorWorld::getAttackPriority(int index){
    if(index == UNIT_WATCHERTOWER || index == UNIT_ORC_BUNKER || index == UNIT_ORC_HQ ){
        return 1;
    }else if(index == UNIT_CASTLE || index == UNIT_FARM || index == UNIT_BARRACKS || index == UNIT_LUMBERMILL || index == UNIT_WATCHERTOWER || index == UNIT_FACTORY || index == UNIT_AIRPORT){
        return 0;
    }
    return 2;
}
bool EditorWorld::canAttack(Movable* attacker, Movable* target){
    bool can = true;
    if(attacker->attackType == ATTACK_TYPE_NEAR && target->isFlying){
        can = false;
    }
    return can;
}
bool EditorWorld::inInScreen(cocos2d::Vec2 pos){
    return cocos2d::Rect(-getPositionX(), -getPositionY(), size.width, size.height).containsPoint(pos);
}
void EditorWorld::revengeAttack(Movable* attackee, Movable* attacker){
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

void EditorWorld::createTree(cocos2d::Vec2 pos){
    EnemyBase* unit = createUnit(UNIT_TREE, WHICH_SIDE_MUTUAL, ITS_NOT_BUILDING, pos, "tree", 1, strmake("tree2_%d.png", rand()%5));
    unit->canMove = false;
    
//    Sprite* spt = Sprite::createWithSpriteFrameName(strmake("tree2_%d.png", rand()%5));
//    spriteBatch->addChild(spt);
    Vec2 coordinate = getCoordinateFromPosition(pos);
    unit->setTag(coordinate.x + coordinate.y*mapSizeWidth);
//    unit->setPosition(pos + Vec2(-20 + rand()%40, -20 + rand()%40 + 20));
    unit->setPosition(pos);
    unit->setLocalZOrder(-unit->getBoundingBox().origin.y);
//    unit->childrenSprite.pushBack(spt);
    
    decoLayer->setTileGID(49, coordinate);
}
